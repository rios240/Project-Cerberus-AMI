#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "pldm_fwup_commands.h"
#include "status/rot_status.h"

#ifdef PLDM_FWUP_FD_ENABLE
#include "libpldm/firmware_update.h"
#include "libpldm/utils.h"
#elif defined(PLDM_FWUP_UA_ENABLE)
#include "firmware_update.h"
#include "utils.h"
#endif


#ifdef PLDM_FWUP_FD_ENABLE

int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_multipart_transfer_handler *multipart_transfer, 
    uint8_t *buffer, size_t buf_len)
{
    printf("FD generating GetPackageData command request.\n");
    struct pldm_multipart_transfer_req rq_data;
    rq_data.data_transfer_handle = multipart_transfer->transfer_handle;
    rq_data.transfer_operation_flag = multipart_transfer->transfer_op_flag;

    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *request = (struct pldm_msg *)(buffer + 1);
    int status = encode_get_package_data_req(instance_id, request, &rq_data);
    if (status != 0) {
        return status;
    }

    instance_id += 1;
    return sizeof (struct pldm_msg_hdr) + sizeof (struct pldm_multipart_transfer_req) + 1;
    
}

int pldm_fwup_process_get_package_data_response(struct pldm_fwup_multipart_transfer_handler *multipart_transfer,
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *response)
{
    printf("FD proccessing UA GetPackageData command response.\n");
    struct pldm_msg *rsp = (struct pldm_msg *)(&response->data[1]);

    struct pldm_multipart_transfer_resp rsp_data;
    struct variable_field portion_of_pkg_data;

    size_t response_payload_length = response->length - sizeof (struct pldm_msg_hdr) - 1;
    int status = decode_get_package_data_resp(rsp, &rsp_data, &portion_of_pkg_data, response_payload_length);
    if (status != 0 || rsp_data.completion_code != PLDM_SUCCESS) {
        return status;
    }

    if (rsp_data.transfer_flag == PLDM_START || rsp_data.transfer_flag == PLDM_START_AND_END) {
        status = flash_map->package_data->write(flash_map->package_data, flash_map->package_data_addr, 
        portion_of_pkg_data.ptr, portion_of_pkg_data.length);
        if (rsp_data.transfer_flag == PLDM_START) {
            multipart_transfer->transfer_op_flag = PLDM_GET_NEXTPART;
        }
    } else {
        status = flash_map->package_data->write(flash_map->package_data, flash_map->package_data_addr + multipart_transfer->transfer_handle, 
        portion_of_pkg_data.ptr, portion_of_pkg_data.length);
        if (rsp_data.transfer_flag == PLDM_END) {
            multipart_transfer->transfer_op_flag = PLDM_GET_FIRSTPART;
        }
    }
    printf("Writing %d of package data to flash.\n", portion_of_pkg_data.length);

    if (ROT_IS_ERROR(status)) {
        return status;
    } else {
        status = 0;
    }


    multipart_transfer->transfer_handle = rsp_data.next_data_transfer_handle;

    response->length = 0;
    return status;

}


#elif defined(PLDM_FWUP_UA_ENABLE)

int pldm_fwup_process_get_package_data_request(struct pldm_fwup_multipart_transfer_handler *multipart_transfer, 
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *request)
{
        printf("UA proccessing and responding to FD GetPackageData command request.\n");
        struct pldm_msg *rq = (struct pldm_msg *)(&request->data[1]);

        struct get_fd_data_req rq_data = { 0 };
        const size_t request_payload_length = request->length - sizeof (struct pldm_msg_hdr) - 1;

        int status = decode_get_pacakge_data_req(rq, request_payload_length, &rq_data.data_transfer_handle, &rq_data.transfer_operation_flag);
        if (status != 0) {
            return status;
        }

        struct get_fd_data_resp rsp_data;
        struct variable_field portion_of_pkg_data;

        portion_of_pkg_data.length = FWUP_BASELINE_TRANSFER_SIZE;
        uint8_t buffer[FWUP_BASELINE_TRANSFER_SIZE];

        memset(buffer, 0x00, FWUP_BASELINE_TRANSFER_SIZE);

        if (rq_data.transfer_operation_flag == PLDM_GET_FIRSTPART) {
            status = flash_map->firmware_update_package->read(flash_map->firmware_update_package, 
                flash_map->firmware_update_package_addr, buffer, sizeof (buffer));
        } else {
             status = flash_map->firmware_update_package->read(flash_map->firmware_update_package, 
                flash_map->firmware_update_package_addr + rq_data.data_transfer_handle, buffer, sizeof (buffer));
        }

        if (status != 0) {
            return status;
        }

        portion_of_pkg_data.ptr = (const uint8_t *)buffer;
        printf("Reads %d of package data from flash to send.\n", portion_of_pkg_data.length);

        if (rq_data.transfer_operation_flag == PLDM_GET_FIRSTPART) {
            if (flash_map->firmware_update_package_size <= FWUP_BASELINE_TRANSFER_SIZE) {
                rsp_data.transfer_flag = PLDM_START_AND_END;
            } else {
                rsp_data.transfer_flag = PLDM_START;
            }
        } else {
            if (rq_data.data_transfer_handle + FWUP_BASELINE_TRANSFER_SIZE >= flash_map->firmware_update_package_size) {
                rsp_data.transfer_flag = PLDM_END;
            } else {
                rsp_data.transfer_flag = PLDM_MIDDLE;
            }
        }

        rsp_data.next_data_transfer_handle = rq_data.data_transfer_handle + FWUP_BASELINE_TRANSFER_SIZE;
        multipart_transfer->transfer_flag = rsp_data.transfer_flag;

        rsp_data.completion_code = PLDM_SUCCESS;
        static uint8_t instance_id = 1;

        const size_t response_payload_length = sizeof (struct get_fd_data_resp) + portion_of_pkg_data.length;
        struct pldm_msg *rsp = (struct pldm_msg *)(&request->data[1]);

        status = encode_get_package_data_resp(instance_id, response_payload_length, rsp, &rsp_data, &portion_of_pkg_data);

        request->length = sizeof (struct pldm_msg_hdr) + response_payload_length + 1;
        instance_id += 1;

        return status;

}

#endif