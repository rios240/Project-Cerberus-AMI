#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "cmd_interface_pldm.h"
#include "pldm_fwup_commands.h"
#include "status/rot_status.h"

#include "libpldm/firmware_update.h"
#include "libpldm/utils.h"



//#ifdef PLDM_FWUP_FD_ENABLE


/**
 * Process a QueryDeviceIdentifiers request.
 * 
 * @param fwup_state - Variable context for a FWUP.
 * @param device_manager - Module which holds a table of all devices.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request)
{
    struct pldm_msg *rsp = (struct pldm_msg *)&request[PLDM_MCTP_BINDING_MSG_OFFSET];

    static uint8_t instance_id;
    
    struct variable_field descriptors;
    descriptors.length = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS * sizeof (uint16_t);
    descriptors.ptr = (const uint8_t *)malloc(DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS * sizeof (uint16_t));

    uint32_t device_identifiers_len = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS * sizeof (uint16_t);
    uint8_t descriptor_count = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS;

    //copied in the order they appear in the device_manager_entry
    memcpy((uint8_t *)descriptors.ptr, 
    &device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_vid, sizeof (uint16_t));            
    memcpy((uint8_t *)descriptors.ptr + sizeof (uint16_t), 
    &device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_device_id, sizeof (uint16_t));
    memcpy((uint8_t *)descriptors.ptr + (2 * sizeof (uint16_t)), 
    &device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_vid, sizeof (uint16_t));
    memcpy((uint8_t *)descriptors.ptr + (3 * sizeof (uint16_t)), 
    &device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_id, sizeof (uint16_t));

    uint8_t completion_code = PLDM_SUCCESS;

    size_t rsp_payload_length = sizeof (struct pldm_query_device_identifiers_resp) + device_identifiers_len;
    int status = encode_query_device_identifiers_resp(instance_id, rsp_payload_length, rsp,
        completion_code, device_identifiers_len, descriptor_count, (const struct variable_field *)&descriptors);

    fwup_state->previous_cmd = PLDM_QUERY_DEVICE_IDENTIFIERS;
    fwup_state->previous_completion_code = completion_code;

    free((uint8_t *) descriptors.ptr);

    return status;
}

/**
* Generate a GetPackageData request.
*
* @param multipart_transfer Context for a multipart transfer.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
/*
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_multipart_transfer_context *multipart_transfer, 
    uint8_t *buffer, size_t buf_len)
{
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

*/

/**
* Process a GetPackageData response.
*
* @param multipart_transfer Context for a multipart transfer.
* @param flash_map The flash_map for a PLDM FWUP.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
/*
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_multipart_transfer_context *multipart_transfer,
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)(&response->data[1]);

    struct pldm_multipart_transfer_resp rsp_data;
    struct variable_field portion_of_pkg_data;

    size_t response_payload_length = response->length - sizeof (struct pldm_msg_hdr) - 1;
    int status = decode_get_package_data_resp(rsp, &rsp_data, &portion_of_pkg_data, response_payload_length);
    if (status != 0 || rsp_data.completion_code != PLDM_SUCCESS) {
        return status;
    }

    if (rsp_data.transfer_flag == PLDM_START || rsp_data.transfer_flag == PLDM_START_AND_END) {
        status = flash_map->pkg_data->write(flash_map->pkg_data, flash_map->pkg_data_addr, 
        portion_of_pkg_data.ptr, portion_of_pkg_data.length);
        if (rsp_data.transfer_flag == PLDM_START) {
            multipart_transfer->transfer_op_flag = PLDM_GET_NEXTPART;
        }
    } else {
        status = flash_map->pkg_data->write(flash_map->pkg_data, flash_map->pkg_data_addr + multipart_transfer->transfer_handle, 
        portion_of_pkg_data.ptr, portion_of_pkg_data.length);
        if (rsp_data.transfer_flag == PLDM_END) {
            multipart_transfer->transfer_op_flag = PLDM_GET_FIRSTPART;
        }
    }

    if (ROT_IS_ERROR(status)) {
        return status;
    } else {
        status = 0;
    }


    multipart_transfer->transfer_handle = rsp_data.next_data_transfer_handle;

    response->length = 0;
    return status;

}
*/


//#elif defined(PLDM_FWUP_UA_ENABLE)


/**
* Process a GetPackageData request and generate a response.
*
* @param multipart_transfer Context for a multipart transfer.
* @param flash_map The flash_map for a PLDM FWUP.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
/*
int pldm_fwup_process_get_package_data_request(struct pldm_fwup_multipart_transfer_context *multipart_transfer, 
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *request)
{
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
            status = flash_map->fup->read(flash_map->fup, 
                flash_map->fup_addr, buffer, sizeof (buffer));
        } else {
             status = flash_map->fup->read(flash_map->fup, 
                flash_map->fup_addr + rq_data.data_transfer_handle, buffer, sizeof (buffer));
        }

        if (status != 0) {
            return status;
        }

        portion_of_pkg_data.ptr = (const uint8_t *)buffer;

        if (rq_data.transfer_operation_flag == PLDM_GET_FIRSTPART) {
            if (flash_map->fup_size <= FWUP_BASELINE_TRANSFER_SIZE) {
                rsp_data.transfer_flag = PLDM_START_AND_END;
            } else {
                rsp_data.transfer_flag = PLDM_START;
            }
        } else {
            if (rq_data.data_transfer_handle + FWUP_BASELINE_TRANSFER_SIZE >= flash_map->fup_size) {
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
*/

/**
* Generate a QueryDeviceIdentifiers request.
*
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_query_device_identifiers_request(uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t request_payload_length = 0;

    int status = encode_query_device_identifiers_req(instance_id, request_payload_length, rq);
    if (status != 0) {
        return status;
    }

    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
 * Process a QueryDeviceIdentifiers response.
 * 
 * @param fwup_state - Variable context for a FWUP.
 * @param device_manager - Module which holds a table of all devices.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_query_device_identifiers_response(struct pldm_fwup_state *fwup_state, 
    struct device_manager *device_manager, struct cmd_interface_msg *response)
{
    int status;
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;
    fwup_state->previous_cmd = PLDM_QUERY_DEVICE_IDENTIFIERS;
    
    uint8_t completion_code = 0;
	uint32_t device_identifiers_len = 0;
	uint8_t descriptor_count = 0;
	uint8_t *descriptor_data = 0;

    struct pldm_msg *rsp = (struct pldm_msg *)&response->data[PLDM_MCTP_BINDING_MSG_OFFSET];
    status = decode_query_device_identifiers_resp(rsp, rsp_payload_length, 
        &completion_code, &device_identifiers_len, &descriptor_count, &descriptor_data);
    if (status != 0) {
        return status;
    }
    fwup_state->previous_completion_code = completion_code;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    for (int i = 0; i < device_manager->num_devices; i++) {
        int device_eid = device_manager_get_device_eid(device_manager, i);
        int device_addr = device_manager_get_device_addr(device_manager, i);
        if(device_eid == response->source_eid && device_addr == response->source_addr) {
            memcpy(&device_manager->entries[i].pci_vid, 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_manager->entries[i].pci_device_id + sizeof (uint16_t), 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_manager->entries[i].pci_vid + (2 * sizeof (uint16_t)), 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_manager->entries[i].pci_vid + (3 * sizeof (uint16_t)), 
            descriptor_data, sizeof (uint16_t));
            break;
        } else if (ROT_IS_ERROR(device_eid) || ROT_IS_ERROR(device_addr)) {
            return device_eid;
        }
    }

    return status;
}

//#endif