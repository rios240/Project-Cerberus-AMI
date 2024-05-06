#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "cmd_interface_pldm.h"
#include "pldm_fwup_protocol_commands.h"
#include "fup/fup_interface.h"
#include "status/rot_status.h"
#include "common/unused.h"
#include "platform.h"

#include "libpldm/firmware_update.h"
#include "libpldm/utils.h"





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
/*
int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request)
{
    fwup_state->previous_cmd = PLDM_QUERY_DEVICE_IDENTIFIERS;

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    static uint8_t instance_id = 1;
    
    struct variable_field descriptors;
    descriptors.length = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS * sizeof (uint16_t);
    descriptors.ptr = (const uint8_t *)platform_calloc(DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS, sizeof (uint16_t));

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
        completion_code, device_identifiers_len, descriptor_count, &descriptors);

    fwup_state->previous_completion_code = completion_code;

    free((uint8_t *) descriptors.ptr);

    instance_id += 1;
    return status;
}
*/

/**
 * Process a GetFirmwareParameters request.
 * 
 * @param fwup_state - Variable context for a FWUP.
 * @param device_manager - Module which holds a table of all devices.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
/*
int pldm_fwup_prcocess_get_firmware_parameters_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request)
{
    fwup_state->previous_cmd = PLDM_GET_FIRMWARE_PARAMETERS;

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    static uint8_t instance_id = 1;

    struct variable_field active_comp_image_set_ver_str;
    active_comp_image_set_ver_str.length = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].active_comp_img_set_ver_str_len;
    active_comp_image_set_ver_str.ptr = (const uint8_t *)device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].active_comp_img_set_ver_str;

    struct variable_field pending_comp_image_set_ver_str;
    pending_comp_image_set_ver_str.length = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pending_comp_img_set_ver_str_len;
    pending_comp_image_set_ver_str.ptr = (const uint8_t *)device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pending_comp_img_set_ver_str;

    struct variable_field comp_parameter_table;
    comp_parameter_table.length = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].num_components * 
        sizeof (struct device_manager_component_entry);
    comp_parameter_table.ptr = (const uint8_t *)device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].comp_entries;

    struct pldm_get_firmware_parameters_resp rsp_data;
    rsp_data.capabilities_during_update.value = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].update_capabilities.value;
    rsp_data.comp_count = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].num_components;
    rsp_data.active_comp_image_set_ver_str_type = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].active_comp_img_set_ver_str_type;
    rsp_data.active_comp_image_set_ver_str_len = active_comp_image_set_ver_str.length;
    rsp_data.pending_comp_image_set_ver_str_type = device_manager->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pending_comp_img_set_ver_str_type;
    rsp_data.pending_comp_image_set_ver_str_len = pending_comp_image_set_ver_str.length;
    rsp_data.completion_code = PLDM_SUCCESS;

    size_t rsp_payload_length = sizeof (rsp_data) + active_comp_image_set_ver_str.length + 
        pending_comp_image_set_ver_str.length + 
        comp_parameter_table.length;

    int status = encode_get_firmware_parameters_resp(instance_id, rsp, rsp_payload_length,
        &rsp_data, &active_comp_image_set_ver_str, &pending_comp_image_set_ver_str, &comp_parameter_table);

    fwup_state->previous_completion_code = rsp_data.completion_code;

    instance_id += 1;
    return status;
}   
*/

/**
 * Process a RequestUpdate request.
 * 
 * @param cmd_pldm - Command interface for PLDM
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
/*
int pldm_fwup_process_request_update_request(struct cmd_interface_pldm *cmd_pldm, struct cmd_interface_msg *request)
{
    cmd_pldm->fwup_state->previous_cmd = PLDM_REQUEST_UPDATE;

    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;
    
    struct variable_field comp_img_set_ver_str;
    comp_img_set_ver_str.ptr = (const uint8_t *)cmd_pldm->comp_img_set_ver_str;

    int status = decode_request_update_req(rq, rq_payload_length, &cmd_pldm->max_transfer_size,
        &cmd_pldm->num_components, &cmd_pldm->max_outstanding_transfer_requests, &cmd_pldm->package_data_len,
        &cmd_pldm->comp_img_set_ver_str_type, &cmd_pldm->comp_img_set_ver_str_len, &comp_img_set_ver_str);
    if (status != 0) {
        return status;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_request_update_resp);

    static uint8_t instance_id = 1;

    uint8_t completion_code = 0;
	uint16_t fd_meta_data_len = sizeof (cmd_pldm->fwup_state);
	uint8_t fd_will_send_pkg_data = 0;
    if (cmd_pldm->package_data_len > 0) {
        fd_will_send_pkg_data = 1;
    }

    if (cmd_pldm->fwup_state->update_mode) {
        completion_code = PLDM_FWUP_ALREADY_IN_UPDATE_MODE;
    } else if (cmd_pldm->fwup_flash == NULL || cmd_pldm->device_manager == NULL) {
        completion_code = PLDM_FWUP_UNABLE_TO_INITIATE_UPDATE;
    } else {
        completion_code = PLDM_SUCCESS;
        cmd_pldm->fwup_state->state = PLDM_FD_STATE_LEARN_COMPONENTS;
    }

    cmd_pldm->fwup_state->previous_completion_code = completion_code;
    cmd_pldm->fwup_state->update_mode = 1;

    status = encode_request_update_resp(instance_id, rsp_payload_length, rsp, completion_code, 
        fd_meta_data_len, fd_will_send_pkg_data);
   
    instance_id += 1;
    return status;
}
*/


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
/*
int pldm_fwup_generate_query_device_identifiers_request(uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_query_device_identifiers_req(instance_id, rq_payload_length, rq);
    if (status != 0) {
        return status;
    }

    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
*/

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
/*
int pldm_fwup_process_query_device_identifiers_response(struct pldm_fwup_state *fwup_state, 
    struct device_manager *device_manager, struct cmd_interface_msg *response)
{
    fwup_state->previous_cmd = PLDM_QUERY_DEVICE_IDENTIFIERS;

    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;
    
    uint8_t completion_code = 0;
	uint32_t device_identifiers_len = 0;
	uint8_t descriptor_count = 0;
	uint8_t *descriptor_data = 0;

    int status = decode_query_device_identifiers_resp(rsp, rsp_payload_length, 
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
        if(device_eid == response->source_eid) {
            memcpy(&device_manager->entries[i].pci_vid, 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_manager->entries[i].pci_device_id + sizeof (uint16_t), 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_manager->entries[i].pci_vid + (2 * sizeof (uint16_t)), 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_manager->entries[i].pci_vid + (3 * sizeof (uint16_t)), 
            descriptor_data, sizeof (uint16_t));
            break;
        } else if (ROT_IS_ERROR(device_eid)) {
            return device_eid;
        }
    }

    return status;
}
*/

/**
* Generate a GetFirmwareParameters request.
*
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
/*
int pldm_fwup_generate_get_firmware_parameters_request(uint8_t *buffer, size_t buf_len)
{   
    static uint8_t instance_id;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_get_firmware_parameters_req(instance_id, rq_payload_length, rq);
    if (status != 0) {
        return status;
    }

    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
*/

/**
 * Process a GetFirmwareParameters response.
 * 
 * @param fwup_state - Variable context for a FWUP.
 * @param device_manager - Module which holds a table of all devices.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
/*
int pldm_fwup_process_get_firmware_parameters_response(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *response)
{
    fwup_state->previous_cmd = PLDM_GET_FIRMWARE_PARAMETERS;

    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    struct pldm_get_firmware_parameters_resp rsp_data = {0};
	struct variable_field active_comp_image_set_ver_str = {0};
	struct variable_field pending_comp_image_set_ver_str = {0};
	struct variable_field comp_parameter_table = {0};

    int status = decode_get_firmware_parameters_resp(rsp, rsp_payload_length, &rsp_data, &active_comp_image_set_ver_str,
        &pending_comp_image_set_ver_str, &comp_parameter_table);
    if (status != 0) {
        return status;
    }

    fwup_state->previous_completion_code = rsp_data.completion_code;
    if (rsp_data.completion_code != PLDM_SUCCESS) {
        return 0;
    }

    for (int i = 0; i < device_manager->num_devices; i++) {
        int device_eid = device_manager_get_device_eid(device_manager, i);
        if(device_eid == response->source_eid) {
            device_manager->entries[i].comp_entries = (struct device_manager_component_entry *)platform_realloc(device_manager->entries[i].comp_entries, 
            comp_parameter_table.length);
            memcpy((uint8_t *)device_manager->entries[i].comp_entries, comp_parameter_table.ptr, comp_parameter_table.length);
            break;
        } else if (ROT_IS_ERROR(device_eid)) {
            return device_eid;
        }
    }

    return status;
}
*/

/**
* Generate a RequestUpdate request.
*
* @param fwup_flash The flash addresses and devices to use for different PLDM FWUP regions.
* @param device_manager Device manager instance.
* @param device_eid EID of the device to be updated.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
/*
int pldm_fwup_generate_request_update_request(struct pldm_fwup_flash_map *fwup_flash, struct device_manager *device_manager, 
    uint8_t device_eid, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    uint32_t max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
	uint8_t max_outstanding_transfer_req = PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ;
	struct variable_field comp_img_set_ver_str;

    struct pldm_firmware_device_id_record device_id_record;
    struct variable_field applicable_components;
    struct variable_field record_descriptors;
    struct variable_field fw_device_pkg_data;
    

    int status = fup_interface_get_device_id_record(fwup_flash->fw_update_package_flash, fwup_flash->fw_update_package_addr,
        &device_manager->entries[device_eid], &device_id_record, &applicable_components, 
        &comp_img_set_ver_str, &record_descriptors, &fw_device_pkg_data);
    if (status != 0) {
        return status;
    }

   	uint16_t pkg_data_len = device_id_record.fw_device_pkg_data_length;
    uint8_t comp_image_set_ver_str_type = device_id_record.comp_image_set_version_string_type;
    uint8_t comp_image_set_ver_str_len = device_id_record.comp_image_set_version_string_length;

    uint16_t num_of_comp = 0;
    for (int i = 0; i < (int)applicable_components.length; i++) {
        for (int j = 0; j < 8; j++) {
            uint8_t mask = 1 << j;
            if (buffer[i] & mask) {
                num_of_comp++;
            }
        }
    }

    size_t rq_payload_length = sizeof (struct pldm_request_update_req) + comp_img_set_ver_str.length;

    status = encode_request_update_req(instance_id, max_transfer_size, num_of_comp, max_outstanding_transfer_req,
        pkg_data_len, comp_image_set_ver_str_type, comp_image_set_ver_str_len, &comp_img_set_ver_str, rq, rq_payload_length);
    if (status != 0) {
        return status;
    }

    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
*/



/**
 * Process a RequestUpdate response.
 * 
 * @param cmd_pldm - Command interface for PLDM.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
/*
int pldm_fwup_process_request_update_response(struct cmd_interface_pldm *cmd_pldm, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    cmd_pldm->fwup_state->previous_cmd = PLDM_REQUEST_UPDATE;

    uint8_t fd_will_send_pkg_data_cmd = 0;
    int status = decode_request_update_resp(rsp, rsp_payload_length, &cmd_pldm->fwup_state->previous_completion_code,
    &cmd_pldm->fw_device_meta_data_len, &fd_will_send_pkg_data_cmd);
    if (status != 0) {
        return status;
    }
    if (cmd_pldm->fwup_state->previous_completion_code!= PLDM_SUCCESS) {
        return 0;
    }
    
    cmd_pldm->fd_will_send_pkg_data_cmd = fd_will_send_pkg_data_cmd;

    return status;
}
*/

#ifdef PLDM_FWUP_FD_ENABLE


int pldm_fwup_process_query_device_identifiers_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_QUERY_DEVICE_IDENTIFIERS;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_IDLE;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_prcocess_get_firmware_parameters_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_GET_FIRMWARE_PARAMETERS;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_IDLE;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_process_request_update_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request) 
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_REQUEST_UPDATE;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_LEARN_COMPONENTS;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    interface->package_data_len = 1;
    interface->num_components = 3;

    return 0;
}


int pldm_fwup_generate_get_package_data_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_GET_PACKAGE_DATA;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->state = PLDM_FD_STATE_LEARN_COMPONENTS;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

int pldm_fwup_process_get_package_data_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response) {
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_GET_FIRMWARE_PARAMETERS;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->state = PLDM_FD_STATE_LEARN_COMPONENTS;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    interface->fwup_state->fwup_multipart_transfer.transfer_op_flag = PLDM_GET_FIRSTPART;

    return 0;
}


int pldm_fwup_process_get_device_meta_data_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_GET_DEVICE_METADATA;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_LEARN_COMPONENTS;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    interface->fwup_state->fwup_multipart_transfer.transfer_flag = PLDM_START_AND_END;

    return 0;
}
int pldm_fwup_process_pass_component_table_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_PASS_COMPONENT_TABLE;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_READY_XFER;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}
int pldm_fwup_process_update_component_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request) {
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_UPDATE_COMPONENT;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_DOWNLOAD;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_generate_request_firmware_data_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_REQUEST_FIRMWARE_DATA;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->state = PLDM_FD_STATE_DOWNLOAD;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_request_firmware_data_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response) {
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_REQUEST_FIRMWARE_DATA;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->state = PLDM_FD_STATE_DOWNLOAD;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    interface->fwup_state->comp_offset = 1;
    interface->fwup_state->comp_size = 1;

    return 0;
}

int pldm_fwup_generate_transfer_complete_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_TRANSFER_COMPLETE;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->state = PLDM_FD_STATE_VERIFY;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_transfer_complete_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_TRANSFER_COMPLETE;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->state = PLDM_FD_STATE_VERIFY;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_generate_verify_complete_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_VERIFY_COMPLETE;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->state = PLDM_FD_STATE_APPLY;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_verify_complete_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response) 
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_VERIFY_COMPLETE;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->state = PLDM_FD_STATE_APPLY;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_generate_apply_complete_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_APPLY_COMPLETE;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->state = PLDM_FD_STATE_READY_XFER;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_apply_complete_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_APPLY_COMPLETE;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->state = PLDM_FD_STATE_READY_XFER;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_process_activate_firmware_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_ACTIVATE_FIRMWARE;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->state = PLDM_FD_STATE_IDLE;
    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

#elif defined(PLDM_FWUP_UA_ENABLE)

int pldm_fwup_generate_query_device_identifiers_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_QUERY_DEVICE_IDENTIFIERS;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_query_device_identifiers_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_QUERY_DEVICE_IDENTIFIERS;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_generate_get_firmware_parameters_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_GET_FIRMWARE_PARAMETERS;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_get_firmware_parameters_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_GET_FIRMWARE_PARAMETERS;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_generate_request_update_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_REQUEST_UPDATE;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_request_update_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_REQUEST_UPDATE;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;
    interface->fd_will_send_pkg_data_cmd = 1;

    return 0; 
}

int pldm_fwup_process_get_package_data_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_GET_PACKAGE_DATA;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;
    interface->fwup_state->fwup_multipart_transfer.transfer_flag = PLDM_START_AND_END;

    return 0;
}

int pldm_fwup_generate_get_device_meta_data_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_GET_DEVICE_METADATA;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;

}
int pldm_fwup_process_get_device_meta_data_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_GET_DEVICE_METADATA;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;
    interface->fwup_state->fwup_multipart_transfer.transfer_op_flag = PLDM_GET_FIRSTPART;

    return 0; 
}

int pldm_fwup_generate_pass_component_table_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_PASS_COMPONENT_TABLE;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_pass_component_table_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_PASS_COMPONENT_TABLE;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0; 
}

int pldm_fwup_generate_update_component_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_UPDATE_COMPONENT;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}
int pldm_fwup_process_update_component_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_UPDATE_COMPONENT;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0; 
}


int pldm_fwup_process_request_firmware_data_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_REQUEST_FIRMWARE_DATA;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;
    interface->fwup_state->comp_offset = 1;
    interface->fwup_state->comp_size = 1;

    return 0;
}

int pldm_fwup_process_transfer_complete_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_TRANSFER_COMPLETE;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_process_verify_complete_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_VERIFY_COMPLETE;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_process_apply_complete_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)request->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_APPLY_COMPLETE;

    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    request->length = PLDM_MCTP_BINDING_MSG_OVERHEAD;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0;
}

int pldm_fwup_generate_activate_firmware_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len)
{
    struct pldm_msg *rq = (struct pldm_msg *)buffer + PLDM_MCTP_BINDING_MSG_OFFSET;
    rq->hdr.command = PLDM_ACTIVATE_FIRMWARE;

    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

int pldm_fwup_process_activate_firmware_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response)
{
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    rsp->hdr.command = PLDM_ACTIVATE_FIRMWARE;

    response->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    response->length = 0;

    interface->fwup_state->previous_completion_code = PLDM_SUCCESS;

    return 0; 
}

#endif