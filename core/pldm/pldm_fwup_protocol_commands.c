#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "pldm_fwup_protocol_commands.h"
#include "cmd_interface_pldm.h"
#include "status/rot_status.h"
#include "common/unused.h"
#include "common/buffer_util.h"
#include "platform.h"

#include "libpldm/firmware_update.h"
#include "libpldm/utils.h"


/*******************
 * FD Helper functions
 *******************/
void switch_state(struct pldm_fwup_fd_state *state, enum pldm_firmware_device_states new_state) 
{
    state->previous_state = state->current_state;
    state->current_state = new_state;
}

/*******************
 * FD Inventory commands
 *******************/

/**
 * Process a QueryDeviceIdentifiers request.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param device_mgr - The device manager linked to command interface.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_fd_state *state,
    struct device_manager *device_mgr, struct cmd_interface_msg *request)
{

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    static uint8_t instance_id = 1;
    
    struct variable_field descriptors;
    descriptors.length = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS * sizeof (uint16_t);
    descriptors.ptr = (const uint8_t *)platform_calloc(DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS, sizeof (uint16_t));

    uint32_t device_identifiers_len = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS * sizeof (uint16_t);
    uint8_t descriptor_count = DEVICE_MANAGER_PLDM_NUM_DESCRIPTORS;

    //copied in the order they appear in the device entry
    memcpy((uint8_t *)descriptors.ptr, 
    &device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_vid, sizeof (uint16_t));            
    memcpy((uint8_t *)descriptors.ptr + sizeof (uint16_t), 
    &device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_device_id, sizeof (uint16_t));
    memcpy((uint8_t *)descriptors.ptr + (2 * sizeof (uint16_t)), 
    &device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_vid, sizeof (uint16_t));
    memcpy((uint8_t *)descriptors.ptr + (3 * sizeof (uint16_t)), 
    &device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_id, sizeof (uint16_t));

    uint8_t completion_code = PLDM_SUCCESS;

    size_t rsp_payload_length = sizeof (struct pldm_query_device_identifiers_resp) + device_identifiers_len;
    int status = encode_query_device_identifiers_resp(instance_id, rsp_payload_length, rsp,
        completion_code, device_identifiers_len, descriptor_count, &descriptors);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    free((uint8_t *) descriptors.ptr);

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_QUERY_DEVICE_IDENTIFIERS;
    switch_state(state, PLDM_FD_STATE_IDLE);
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
 * Process a GetFirmwareParameters request.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param fw_parameters - FD firmware parameters.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_prcocess_get_firmware_parameters_request(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_protocol_firmware_parameters *fw_parameters, struct cmd_interface_msg *request)
{

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    static uint8_t instance_id = 1;

    struct variable_field active_comp_img_set_ver;
    active_comp_img_set_ver.length = fw_parameters->active_comp_img_set_ver.version_str_length;
    active_comp_img_set_ver.ptr = (const uint8_t *)fw_parameters->active_comp_img_set_ver.version_str;

    struct variable_field pending_comp_img_set_ver;
    pending_comp_img_set_ver.length = fw_parameters->pending_comp_img_set_ver.version_str_length;
    pending_comp_img_set_ver.ptr = (const uint8_t *)fw_parameters->pending_comp_img_set_ver.version_str;

    uint8_t comp_parameter_table_buf[fw_parameters->count * sizeof (struct pldm_fwup_protocol_component_parameter_entry)];

    size_t offset = 0;
    for (int i = 0; i < fw_parameters->count; i++) {
        size_t active_comp_ver_str_len = fw_parameters->entries[i].active_comp_ver.version_str_length;
        size_t pending_comp_ver_str_len = fw_parameters->entries[i].pending_comp_ver.version_str_length;
        size_t comp_parameter_table_fixed_len = sizeof (struct pldm_fwup_protocol_component_parameter_entry) - 
            (2 * sizeof (struct pldm_fwup_protocol_version_string));
        size_t comp_ver_fixed_len = sizeof (struct pldm_fwup_protocol_version_string) - 255;

        memcpy(comp_parameter_table_buf + offset, &fw_parameters->entries[i], comp_parameter_table_fixed_len);
        memcpy(comp_parameter_table_buf + offset + comp_parameter_table_fixed_len, 
            &fw_parameters->entries[i].active_comp_ver, comp_ver_fixed_len);
        memcpy(comp_parameter_table_buf + offset + comp_parameter_table_fixed_len + comp_ver_fixed_len,
            fw_parameters->entries[i].active_comp_ver.version_str, active_comp_ver_str_len);

        memcpy(comp_parameter_table_buf + offset + comp_parameter_table_fixed_len + comp_ver_fixed_len + active_comp_ver_str_len, 
            &fw_parameters->entries[i].pending_comp_ver, comp_ver_fixed_len);
        memcpy(comp_parameter_table_buf + offset + comp_parameter_table_fixed_len + comp_ver_fixed_len + active_comp_ver_str_len + comp_ver_fixed_len,
            fw_parameters->entries[i].pending_comp_ver.version_str, pending_comp_ver_str_len);

        offset += comp_parameter_table_fixed_len + (2 * comp_ver_fixed_len) + active_comp_ver_str_len + pending_comp_ver_str_len;
    }

    struct variable_field comp_parameter_table;
    comp_parameter_table.length = offset;
    comp_parameter_table.ptr = (const uint8_t *)comp_parameter_table_buf;

    struct pldm_get_firmware_parameters_resp rsp_data;
    rsp_data.comp_count = fw_parameters->count;
    rsp_data.capabilities_during_update = fw_parameters->capabilities_during_update;
    rsp_data.active_comp_image_set_ver_str_len = fw_parameters->active_comp_img_set_ver.version_str_length;
    rsp_data.active_comp_image_set_ver_str_type = fw_parameters->active_comp_img_set_ver.version_str_type;
    rsp_data.pending_comp_image_set_ver_str_len = fw_parameters->pending_comp_img_set_ver.version_str_length;
    rsp_data.pending_comp_image_set_ver_str_type = fw_parameters->pending_comp_img_set_ver.version_str_type;
    rsp_data.completion_code = PLDM_SUCCESS;

    size_t rsp_payload_length = sizeof (rsp_data) + active_comp_img_set_ver.length + 
        pending_comp_img_set_ver.length + comp_parameter_table.length;

    int status = encode_get_firmware_parameters_resp(instance_id, rsp, rsp_payload_length,
        &rsp_data, &active_comp_img_set_ver, &pending_comp_img_set_ver, &comp_parameter_table);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_GET_FIRMWARE_PARAMETERS;
    switch_state(state, PLDM_FD_STATE_IDLE);
    state->previous_completion_code = rsp_data.completion_code;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}   


/*******************
 * FD Update commands
 *******************/

/**
 * Process a RequestUpdate request.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param flash_mgr - The flash manager for a PLDM FWUP.
 * @param update_info - Update information retained by FD.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
 * @note For AMI, the function does not handle UNABLE_TO_INITIATE_UPDATE and RETRY_REQUEST_UPDATE errors and corresponding state changes.
 *       This should be implemented later on according to some other Cerberus criteria.
*/
int pldm_fwup_process_request_update_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_fd_update_info *update_info,
    struct cmd_interface_msg *request)
{   

    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint32_t max_transfer_size = 0;
	uint16_t num_of_comp = 0;
	uint8_t max_outstanding_transfer_req = 0;
	uint16_t package_data_len = 0;
	uint8_t comp_img_set_ver_str_type = 0;
	uint8_t comp_img_set_ver_str_len = 0;
    struct variable_field comp_img_set_ver;

    int status = decode_request_update_req(rq, rq_payload_length, &max_transfer_size,
    &num_of_comp, &max_outstanding_transfer_req, &package_data_len, &comp_img_set_ver_str_type,
    &comp_img_set_ver_str_len, &comp_img_set_ver);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->num_components = num_of_comp;
    update_info->max_transfer_size = max_transfer_size;
    update_info->max_outstanding_transfer_req = max_outstanding_transfer_req;
    update_info->package_data_len = package_data_len;
    memcpy(update_info->comp_img_set_ver.version_str, comp_img_set_ver.ptr, comp_img_set_ver.length);
    update_info->comp_img_set_ver.version_str_type = comp_img_set_ver_str_type;
    update_info->comp_img_set_ver.version_str_length = comp_img_set_ver_str_len;

    update_info->comp_entries = platform_calloc(num_of_comp, sizeof (struct pldm_fwup_protocol_component_entry));
    flash_mgr->comp_regions = platform_calloc(num_of_comp, sizeof (struct flash_region));
    flash_mgr->package_data_size = package_data_len;

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_request_update_resp);

    static uint8_t instance_id = 1;

    uint8_t completion_code = 0;
	uint16_t fd_meta_data_len = flash_mgr->device_meta_data_size;
	uint8_t fd_will_send_pkg_data = 0;

    if (package_data_len > 0) {
        fd_will_send_pkg_data = 1;
    } else {
        fd_will_send_pkg_data = 0;
    }

    if (state->update_mode) {
        completion_code = PLDM_FWUP_ALREADY_IN_UPDATE_MODE;
        switch_state(state, PLDM_FD_STATE_IDLE);
    } else {
        completion_code = PLDM_SUCCESS;
        switch_state(state, PLDM_FD_STATE_LEARN_COMPONENTS);
        state->update_mode = 1;
    }

    status = encode_request_update_resp(instance_id, rsp_payload_length, rsp, completion_code, 
        fd_meta_data_len, fd_will_send_pkg_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }
   
    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_REQUEST_UPDATE;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
* Generate a GetPackageData request.
*
* @param state - Variable context for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    uint8_t *buffer, size_t buf_len)
{
    if (state->current_state != PLDM_FD_STATE_LEARN_COMPONENTS) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint32_t data_transfer_handle = get_cmd_state->data_transfer_handle;
    uint8_t transfer_operation_flag = get_cmd_state->transfer_op_flag;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_multipart_transfer_req);

    int status = encode_get_package_data_req(instance_id, rq_payload_length, rq, data_transfer_handle, transfer_operation_flag);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    switch_state(state, PLDM_FD_STATE_LEARN_COMPONENTS);
    state->previous_cmd = PLDM_GET_PACKAGE_DATA;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD + rq_payload_length;
    
}


/**
* Process a GetPackageData response.
*
* @param state - Variable context for a PLDM FWUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_GET_PACKAGE_DATA) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code;
    uint32_t next_data_transfer_handle;
    uint8_t transfer_flag;
	struct variable_field portion_of_package_data;

    int status = decode_get_package_data_resp(rsp, &completion_code, &next_data_transfer_handle, 
        &transfer_flag, &portion_of_package_data, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    switch_state(state, PLDM_FD_STATE_LEARN_COMPONENTS);
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    status = flash_mgr->flash->write(flash_mgr->flash, 
        flash_mgr->package_data_region.start_addr + get_cmd_state->data_transfer_handle, 
        (uint8_t *)portion_of_package_data.ptr, portion_of_package_data.length);
    if (ROT_IS_ERROR(status)) {
        return status;
    } else {
        status = 0;
    }

    if (transfer_flag == PLDM_START || transfer_flag == PLDM_MIDDLE) {
        get_cmd_state->transfer_op_flag = PLDM_GET_NEXTPART;
        get_cmd_state->data_transfer_handle = next_data_transfer_handle;
    } 
    else {
        get_cmd_state->transfer_op_flag = PLDM_GET_FIRSTPART;
    }

    return status;
}

/**
* Process a GetDeviceMetaData request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param update_info - Update information retained by FD.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_get_device_meta_data_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint32_t data_transfer_handle;
    uint8_t transfer_operation_flag;

    int status = decode_get_device_meta_data_req(rq, rq_payload_length, &data_transfer_handle, &transfer_operation_flag);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    uint8_t transfer_flag = 0;
    struct variable_field portion_of_device_meta_data;
    uint32_t next_data_transfer_handle = 0;
    portion_of_device_meta_data.ptr = (const uint8_t *)&completion_code;
    portion_of_device_meta_data.length = sizeof (completion_code);
    uint8_t device_meta_data_buf[(size_t)update_info->max_transfer_size];

    if (state->previous_cmd != PLDM_GET_PACKAGE_DATA && state->previous_cmd != PLDM_GET_DEVICE_METADATA && state->current_state != PLDM_FD_STATE_LEARN_COMPONENTS) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
        goto exit;
    }
    else if (flash_mgr->device_meta_data_size <= 0) {
        completion_code = PLDM_FWUP_NO_DEVICE_METADATA;
        goto exit;
    }
    else if (get_cmd_state->transfer_flag == PLDM_MIDDLE && transfer_operation_flag == PLDM_GET_FIRSTPART) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_OPERATION_FLAG;
        goto exit;
    } 
    else if (data_transfer_handle != get_cmd_state->next_data_transfer_handle) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_HANDLE;
        goto exit;
    }

    if (transfer_operation_flag == PLDM_GET_FIRSTPART) {
        if (flash_mgr->device_meta_data_size < update_info->max_transfer_size) {
            transfer_flag = PLDM_START_AND_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_START;
            next_data_transfer_handle = data_transfer_handle + update_info->max_transfer_size;
        }
    }
    else {
        if (data_transfer_handle + update_info->max_transfer_size >= flash_mgr->device_meta_data_size) {
            transfer_flag = PLDM_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_MIDDLE;
            next_data_transfer_handle = data_transfer_handle + update_info->max_transfer_size;
        }
    }

    status = flash_mgr->flash->read(flash_mgr->flash, flash_mgr->device_meta_data_region.start_addr + data_transfer_handle,
        device_meta_data_buf, update_info->max_transfer_size);
    if (ROT_IS_ERROR(status)) {
        return status;
    }
    portion_of_device_meta_data.ptr = (const uint8_t *)device_meta_data_buf;
    portion_of_device_meta_data.length = update_info->max_transfer_size;

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_multipart_transfer_resp) + portion_of_device_meta_data.length;

    status = encode_get_device_meta_data_resp(instance_id, rsp_payload_length, rsp, completion_code,
        next_data_transfer_handle, transfer_flag, &portion_of_device_meta_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }
    
    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_GET_DEVICE_METADATA;
    switch_state(state, PLDM_FD_STATE_LEARN_COMPONENTS);
    get_cmd_state->next_data_transfer_handle = next_data_transfer_handle;
    get_cmd_state->transfer_flag = transfer_flag;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
* Process a PassComponentTable request and generate a response.
*
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param update_info - Update information retained by FD.
* @param fw_parameters - FD firmware parameters.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*
* @note For AMI, not every component response code is handled since some depend on other components of Cerberus. 
*/
int pldm_fwup_process_pass_component_table_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_protocol_firmware_parameters *fw_parameters,
    struct cmd_interface_msg *request)
{

    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t transfer_flag = 0;
    uint16_t comp_classification = 0;
    uint16_t comp_identifier = 0;
    uint8_t comp_classification_index = 0;
    uint32_t comp_comparison_stamp = 0;
	uint8_t comp_ver_str_type = 0; 
    uint8_t comp_ver_str_len = 0;
	struct variable_field comp_ver_str;

    int status = decode_pass_component_table_req(rq, rq_payload_length, &transfer_flag, &comp_classification,
        &comp_identifier, &comp_classification_index, &comp_comparison_stamp, &comp_ver_str_type, &comp_ver_str_len, &comp_ver_str);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    static uint8_t comp_num = 0;
    if (transfer_flag == PLDM_START || transfer_flag == PLDM_START_AND_END) {
        comp_num = 0;
    }
    else if (transfer_flag == PLDM_MIDDLE || transfer_flag == PLDM_END) {
        comp_num += 1;
    }

    uint8_t completion_code = PLDM_SUCCESS;
	uint8_t comp_resp = 0;
	uint8_t comp_resp_code = 0;
    static uint8_t instance_id = 1;

    if (!state->update_mode) {
        completion_code = PLDM_FWUP_NOT_IN_UPDATE_MODE;
        goto exit;
    } else if (state->current_state != PLDM_FD_STATE_LEARN_COMPONENTS) {
        completion_code = PLDM_FWUP_INVALID_STATE_FOR_COMMAND;
        goto exit;
    }

    int fw_parameters_idx;
    bool comp_supported = 0;
    for (fw_parameters_idx = 0; fw_parameters_idx < fw_parameters->count; fw_parameters_idx++) {
        if (comp_classification_index == fw_parameters->entries[fw_parameters_idx].comp_classification_index) {
            comp_supported = 1;
            break;
        }
    }

    if (!comp_supported) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_NOT_SUPPORTED;
        goto exit;
    }

    update_info->comp_entries[comp_num].comp_classification = comp_classification;
    update_info->comp_entries[comp_num].comp_classification_index = comp_classification_index;
    update_info->comp_entries[comp_num].comp_comparison_stamp = comp_comparison_stamp;
    update_info->comp_entries[comp_num].comp_identifier = comp_identifier;
    memcpy(update_info->comp_entries[comp_num].comp_ver.version_str, comp_ver_str.ptr, comp_ver_str.length);
    update_info->comp_entries[comp_num].comp_ver.version_str_length = comp_ver_str_len;
    update_info->comp_entries[comp_num].comp_ver.version_str_type = comp_ver_str_type;
    if (transfer_flag == PLDM_END) {
        comp_num = 0;
    }

    if (comp_comparison_stamp == fw_parameters->entries[fw_parameters_idx].active_comp_comparison_stamp) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_COMPARISON_STAMP_IDENTICAL;
    } 
    else if (comp_comparison_stamp < fw_parameters->entries[fw_parameters_idx].active_comp_comparison_stamp) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_COMPARISON_STAMP_LOWER;
    } 
    else if (buffer_compare(comp_ver_str.ptr, (const uint8_t *)fw_parameters->entries[fw_parameters_idx].active_comp_ver.version_str, 
        comp_ver_str.length) == 0) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_VER_STR_IDENTICAL;
    } else {
        comp_resp = PLDM_CR_COMP_CAN_BE_UPDATED;
        comp_resp_code = PLDM_CRC_COMP_CAN_BE_UPDATED;
    }

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_pass_component_table_resp);

    status = encode_pass_component_table_resp(instance_id, rsp, rsp_payload_length, completion_code, comp_resp, comp_resp_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    update_info->comp_transfer_flag = transfer_flag;
    if (transfer_flag == PLDM_START || transfer_flag == PLDM_MIDDLE) {
        switch_state(state, PLDM_FD_STATE_LEARN_COMPONENTS);
    } 
    else if (transfer_flag == PLDM_END || transfer_flag == PLDM_START_AND_END) {
        switch_state(state, PLDM_FD_STATE_READY_XFER);
    }
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
* Process a UpdateComponent request and generate a response.
*
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param update_info - Update information retained by FD.
* @param comp_entries - Component table. 
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*
* @note For AMI, not every component response code is handled since some depend on the external state/status of Cerberus. Also the FD enabled update options flags is simply set to
*       what the UA requested without any additional checks, configuration, etc. The time before RequestFirmwareData field can be changed via the platform config. 
*/
int pldm_fwup_process_update_component_request(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_protocol_component_entry *comp_entries, 
    struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint16_t comp_classification;
	uint16_t comp_identifier;
    uint8_t comp_classification_index;
	uint32_t comp_comparison_stamp;
    uint32_t comp_image_size;
	bitfield32_t update_option_flags;
    uint8_t comp_ver_str_type;
	uint8_t comp_ver_str_len;
    struct variable_field comp_ver;

    int status = decode_update_component_req(rq, rq_payload_length, &comp_classification, &comp_identifier, &comp_classification_index,
        &comp_comparison_stamp, &comp_image_size, &update_option_flags, &comp_ver_str_type, &comp_ver_str_len, &comp_ver);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    uint8_t completion_code = PLDM_SUCCESS;
    uint8_t comp_compatibility_resp = 0;
	uint8_t comp_compatibility_resp_code = 0;
	bitfield32_t update_option_flags_enabled;
    update_option_flags_enabled.value = update_option_flags.value;
	uint16_t time_before_req_fw_data = PLDM_FWUP_PROTOCOL_TIME_BERFORE_REQ_FW_DATA;
    static uint8_t instance_id = 1;

    if (!state->update_mode) {
        completion_code = PLDM_FWUP_NOT_IN_UPDATE_MODE;
        goto exit;
    }
    int comp_num;
    bool comp_supported = 0;
    for (comp_num = 0; comp_num < update_info->num_components; comp_num++) {
        if (comp_classification_index == comp_entries[comp_num].comp_classification_index && comp_identifier == comp_entries[comp_num].comp_identifier &&
            comp_classification == comp_entries[comp_num].comp_classification) {
            comp_supported = 1;
            break;
        }
    }

    if (!comp_supported) {
        comp_compatibility_resp = PLDM_CCR_COMP_CANNOT_BE_UPDATED;
        comp_compatibility_resp_code = PLDM_CCRC_COMP_NOT_SUPPORTED;
    }
    else if (comp_comparison_stamp == comp_entries[comp_num].comp_comparison_stamp && !update_option_flags_enabled.bits.bit0) {
        comp_compatibility_resp = PLDM_CCR_COMP_CANNOT_BE_UPDATED;
        comp_compatibility_resp_code = PLDM_CCRC_COMP_COMPARISON_STAMP_IDENTICAL;
    } 
    else if (comp_comparison_stamp < comp_entries[comp_num].comp_comparison_stamp && !update_option_flags_enabled.bits.bit0) {
        comp_compatibility_resp = PLDM_CCR_COMP_CANNOT_BE_UPDATED;
        comp_compatibility_resp_code = PLDM_CCRC_COMP_COMPARISON_STAMP_LOWER;
    } 
    else if (buffer_compare(comp_ver.ptr, (const uint8_t *)comp_entries[comp_num].comp_ver.version_str, 
        comp_ver.length) == 0 && !update_option_flags_enabled.bits.bit0) {
        comp_compatibility_resp = PLDM_CCR_COMP_CANNOT_BE_UPDATED;
        comp_compatibility_resp_code = PLDM_CCRC_COMP_VER_STR_IDENTICAL;
    } else {
        comp_compatibility_resp = PLDM_CCR_COMP_CAN_BE_UPDATED;
        comp_compatibility_resp_code = PLDM_CCRC_NO_RESPONSE_CODE;
    }

    update_info->current_comp_img_size = comp_image_size;
    update_info->current_comp_update_option_flags = update_option_flags_enabled;
    update_info->current_comp_num = comp_num;


exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_update_component_resp);

    status = encode_update_component_resp(instance_id, rsp, rsp_payload_length, completion_code, comp_compatibility_resp,
        comp_compatibility_resp_code, update_option_flags_enabled, time_before_req_fw_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_UPDATE_COMPONENT;
    if (completion_code == PLDM_SUCCESS) {
        switch_state(state, PLDM_FD_STATE_DOWNLOAD);
    } else {
        switch_state(state, PLDM_FD_STATE_READY_XFER);
    }
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
* Generate a RequestFirmwareData request.
*
* @param state - Variable context for a PLDM FWUP.
* @param update_info - Update information retained by FD.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_request_firmware_data_request(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_fd_update_info *update_info, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint32_t offset = update_info->current_comp_img_offset;
    uint32_t length = update_info->max_transfer_size;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_request_firmware_data_req);
    printf("This might seg fault: %u\n", rq->hdr.command);

    int status = encode_request_firmware_data_req(instance_id, rq, rq_payload_length,
        offset, length);
    if (status != PLDM_SUCCESS) {
        printf("Request transport error: %u\n", status);
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }


    switch_state(state, PLDM_FD_STATE_DOWNLOAD);
    state->previous_cmd = PLDM_REQUEST_FIRMWARE_DATA;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
* Process a RequestFirmwareData response.
*
* @param state - Variable context for a PLDM FWUP.
* @param update_info - Update information retained by FD.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_request_firmware_data_response(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_flash_manager *flash_mgr, 
    struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_REQUEST_FIRMWARE_DATA) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    
    int status = decode_request_firmware_data_resp(rsp, rsp_payload_length, &completion_code);
    if (status != PLDM_SUCCESS) {
        printf("Response transport error: %u\n", status);
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    switch_state(state, PLDM_FD_STATE_DOWNLOAD);
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    status = flash_mgr->flash->write(flash_mgr->flash, 
        flash_mgr->comp_regions[update_info->current_comp_num].start_addr + update_info->current_comp_img_offset,
        rsp->payload + 1, rsp_payload_length - sizeof (completion_code));
    if (ROT_IS_ERROR(status)) {
        return status;
    } else {
        status = 0;
    }

    return status;
}

/**
* Generate a TransferComplete request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*
* @note For AMI, the transfer result is based on what the last completion code received during RequestFirmwareData was set to. 
*       Checks for all transfer results should be implemented based on specific needs. 
*/
int pldm_fwup_generate_transfer_complete_request(struct pldm_fwup_fd_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint8_t transfer_result = 0;
    if (state->previous_completion_code != 0) {
        transfer_result = PLDM_FWUP_FD_GENERIC_TRANSFER_ERROR;
        switch_state(state, PLDM_FD_STATE_DOWNLOAD);
    } else {
        transfer_result = PLDM_FWUP_TRANSFER_SUCCESS;
        switch_state(state, PLDM_FD_STATE_VERIFY);
    }

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (transfer_result);

    int status = encode_transfer_complete_req(instance_id, rq, rq_payload_length, transfer_result);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_TRANSFER_COMPLETE;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
* Process a TransferComplete response.
*
* @param state - Variable context for a PLDM FWUP.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_transfer_complete_response(struct pldm_fwup_fd_state *state, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_TRANSFER_COMPLETE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    
    int status = decode_transfer_complete_resp(rsp, rsp_payload_length, &completion_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        switch_state(state, PLDM_FD_STATE_DOWNLOAD);
        return 0;
    }
    
    return 0;
}

/**
* Generate a VerifyComplete request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*
* @note For AMI, this is skeleton code. Verification of the requested firmware image is left up to the AMI team and their specific requirements. 
* 
*/
int pldm_fwup_generate_verify_complete_request(struct pldm_fwup_fd_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint8_t verify_result = PLDM_FWUP_VERIFY_SUCCESS;
    switch_state(state, PLDM_FD_STATE_APPLY);

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (verify_result);

    int status = encode_verify_complete_req(instance_id, rq, rq_payload_length, verify_result);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_VERIFY_COMPLETE;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
* Process a VerifyComplete response.
*
* @param state - Variable context for a PLDM FWUP.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_verify_complete_response(struct pldm_fwup_fd_state *state, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_VERIFY_COMPLETE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    
    int status = decode_verify_complete_resp(rsp, rsp_payload_length, &completion_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        switch_state(state, PLDM_FD_STATE_VERIFY);
        return 0;
    }
    
    return 0;
}

/**
* Generate a ApplyComplete request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*
* @note For AMI, this is skeleton code. DMTF specifies that during RequestFirmwareData the received firmware image is stored in volatile memory. However,
*       the RequestFirmwareData API immediately writes to a designated flash region. This means that ApplyComplete can either be skipped or the flash region
*       can be treated as temporary and ApplyComplete could transfer to another more permanent region. 
*/
int pldm_fwup_generate_apply_complete_request(struct pldm_fwup_fd_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint8_t apply_result = PLDM_FWUP_APPLY_SUCCESS;
    bitfield16_t comp_activation_methods_modification;
    comp_activation_methods_modification.value = 0;
    switch_state(state, PLDM_FD_STATE_READY_XFER);

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_apply_complete_req);

    int status = encode_apply_complete_req(instance_id, rq, rq_payload_length, apply_result, comp_activation_methods_modification);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_APPLY_COMPLETE;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
* Process a ApplyComplete response.
*
* @param state - Variable context for a PLDM FWUP.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_apply_complete_response(struct pldm_fwup_fd_state *state, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_APPLY_COMPLETE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    
    int status = decode_apply_complete_resp(rsp, rsp_payload_length, &completion_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        switch_state(state, PLDM_FD_STATE_APPLY);
        return 0;
    }
    
    return 0;
}

/**
* Generate a GetMetaData request.
*
* @param state - Variable context for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_meta_data_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    uint8_t *buffer, size_t buf_len)
{
    if (state->current_state == PLDM_FD_STATE_IDLE || state->current_state == PLDM_FD_STATE_LEARN_COMPONENTS) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint32_t data_transfer_handle = get_cmd_state->data_transfer_handle;
    uint8_t transfer_operation_flag = get_cmd_state->transfer_op_flag;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_multipart_transfer_req);

    int status = encode_get_meta_data_req(instance_id, rq_payload_length, rq, data_transfer_handle, transfer_operation_flag);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    switch_state(state, state->current_state);
    state->previous_cmd = PLDM_GET_META_DATA;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD + rq_payload_length;
}

/**
* Process a GetMetaData response.
*
* @param state - Variable context for a PLDM FWUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_meta_data_response(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_GET_META_DATA) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code;
    uint32_t next_data_transfer_handle;
    uint8_t transfer_flag;
	struct variable_field portion_of_meta_data;

    int status = decode_get_meta_data_resp(rsp, &completion_code, &next_data_transfer_handle, 
        &transfer_flag, &portion_of_meta_data, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    switch_state(state, state->current_state);
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    status = flash_mgr->flash->write(flash_mgr->flash, 
        flash_mgr->device_meta_data_region.start_addr + get_cmd_state->data_transfer_handle, 
        (uint8_t *)portion_of_meta_data.ptr, portion_of_meta_data.length);
    if (ROT_IS_ERROR(status)) {
        return status;
    } else {
        status = 0;
    }

    if (transfer_flag == PLDM_START || transfer_flag == PLDM_MIDDLE) {
        get_cmd_state->transfer_op_flag = PLDM_GET_NEXTPART;
        get_cmd_state->data_transfer_handle = next_data_transfer_handle;
    } 
    else if (transfer_flag == PLDM_END || transfer_flag == PLDM_START_AND_END) {
        get_cmd_state->transfer_op_flag = PLDM_GET_FIRSTPART;
        get_cmd_state->data_transfer_handle = 0;
    }

    return status;
}

/**
* Process a ActivateFirmware request and generate a response.
*
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param update_info - Update information retained by FD.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*
* @note For AMI, this is skeleton code. The handling of the self contained activation request, determining whether an incomplete update has occurred, 
*       and determining is self contained activation is supported is left to the AMI team. The estimated time for activation can be set in platform config.
*/
int pldm_fwup_process_activate_firmware_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    bool8_t self_contained_activation_req = 0;

    int status = decode_activate_firmware_req(rq, rq_payload_length, &self_contained_activation_req);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->self_contained_activation_req = self_contained_activation_req;

    static uint8_t instance_id = 1;
    uint16_t estimated_time_activation = PLDM_FWUP_PROTOCOL_EST_TIME_SELF_CONTAINED_ACTIVATION;
    uint8_t completion_code = PLDM_SUCCESS;

    if (state->current_state != PLDM_FD_STATE_READY_XFER) {
        completion_code = PLDM_FWUP_INVALID_STATE_FOR_COMMAND;
    }
    else if (!state->update_mode) {
        completion_code = PLDM_FWUP_NOT_IN_UPDATE_MODE;
    }
    else if (state->previous_cmd == PLDM_ACTIVATE_FIRMWARE) {
        completion_code = PLDM_FWUP_ACTIVATION_NOT_REQUIRED;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_activate_firmware_resp );

    status = encode_activate_firmware_resp(instance_id, rsp, rsp_payload_length, completion_code, estimated_time_activation);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_ACTIVATE_FIRMWARE;
    switch_state(state, PLDM_FD_STATE_ACTIVATE);
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/**
 * Process a GetStatus request.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param update_info - Update information retained by FD.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
 * @note For AMI, assumes progress percent is not supported and timeouts are not checked for the aux state status and reason code. 
*/
int pldm_fwup_process_get_status_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request)
{
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_get_status_resp);

    static uint8_t instance_id = 1;

    uint8_t completion_code = PLDM_SUCCESS;
	uint8_t current_state = state->current_state;
	uint8_t previous_state = state->previous_state;
    uint8_t aux_state = 0;
    if (state->current_state == PLDM_FD_STATE_IDLE ||
        state->current_state == PLDM_FD_STATE_LEARN_COMPONENTS ||
        state->current_state == PLDM_FD_STATE_READY_XFER) {
        aux_state = PLDM_FD_IDLE_LEARN_COMPONENTS_READ_XFER;
    } else {
        aux_state = state->previous_completion_code == 0 ? PLDM_FD_OPERATION_SUCCESSFUL : PLDM_FD_OPERATION_FAILED;
    }
	uint8_t aux_state_status = aux_state == PLDM_FD_OPERATION_SUCCESSFUL || aux_state == PLDM_FD_OPERATION_IN_PROGRESS || aux_state == PLDM_FD_IDLE_LEARN_COMPONENTS_READ_XFER
        ? PLDM_FD_AUX_STATE_IN_PROGRESS_OR_SUCCESS : PLDM_FD_GENERIC_ERROR;
	uint8_t progress_percent = PLDM_FWUP_MAX_PROGRESS_PERCENT;
	uint8_t reason_code = 0;
    if (state->current_state == PLDM_FD_STATE_IDLE && state->previous_cmd == PLDM_ACTIVATE_FIRMWARE) {
        reason_code = PLDM_FD_ACTIVATE_FW;
    }
    else if (state->current_state == PLDM_FD_STATE_IDLE && state->previous_cmd == PLDM_CANCEL_UPDATE) {
        reason_code = PLDM_FD_CANCEL_UPDATE;
    } else {
        reason_code = PLDM_FD_INITIALIZATION;
    }
	bitfield32_t update_option_flags_enabled;
    update_option_flags_enabled.value = update_info->current_comp_update_option_flags.value;

    int status = encode_get_status_resp(instance_id, rsp, rsp_payload_length, completion_code, current_state, previous_state, aux_state,
        aux_state_status, progress_percent, reason_code, update_option_flags_enabled);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_GET_STATUS;
    switch_state(state, state->current_state);
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/**
 * Process a CancelUpdateComponent request.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param update_info - Update information retained by FD.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
 * @note For AMI, the busy in background completion code is not checked and will be left to the AMI team. 
*/
int pldm_fwup_process_cancel_update_component_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request)
{
    switch_state(state, PLDM_FD_STATE_READY_XFER);
    update_info->current_comp_num = 0;
    update_info->current_comp_img_size = 0;
    update_info->current_comp_img_offset = 0;
    update_info->current_comp_update_option_flags.value = 0;

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    size_t rsp_payload_length = sizeof (completion_code);

    if (!state->update_mode) {
        completion_code = PLDM_FWUP_NOT_IN_UPDATE_MODE;
    }

    int status = encode_cancel_update_component_resp(instance_id, rsp, rsp_payload_length, completion_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_CANCEL_UPDATE_COMPONENT;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
 * Process a CancelUpdate request.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param update_info - Update information retained by FD.
 * @param flash_mgr - The flash manager for a PLDM FWUP.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
 * @note For AMI, this is skeleton code. Some reset is done upon receiving a CancelUpdate command, but additional implementation is left to the AMI team.
 *       The assignment of the non functioning component indication and bitmap and the busy in background completion code is left to the AMI team.
*/
int pldm_fwup_process_cancel_update_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_flash_manager *flash_mgr, struct cmd_interface_msg *request)
{
    switch_state(state, PLDM_FD_STATE_IDLE);
    memset(update_info->comp_entries, 0, update_info->num_components * sizeof (struct pldm_fwup_protocol_component_entry));
    memset(update_info, 0, sizeof (struct pldm_fwup_fd_update_info));
    flash_mgr->package_data_size = 0;

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_cancel_update_resp);

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
	bool8_t non_functioning_component_indication = 0;
    bitfield64_t non_functioning_component_bitmap;
    non_functioning_component_bitmap.value = 0;

    if (!state->update_mode) {
        completion_code = PLDM_FWUP_NOT_IN_UPDATE_MODE;
    }

    int status = encode_cancel_update_resp(instance_id, rsp, rsp_payload_length, completion_code, non_functioning_component_indication,
        non_functioning_component_bitmap);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_CANCEL_UPDATE;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/*******************
 * UA Inventory commands
 *******************/

/**
* Generate a QueryDeviceIdentifiers request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_query_device_identifiers_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len)
{

    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_query_device_identifiers_req(instance_id, rq_payload_length, rq);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_QUERY_DEVICE_IDENTIFIERS;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}


/**
 * Process a QueryDeviceIdentifiers response.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param device_mgr - The device manager linked to command interface.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_query_device_identifiers_response(struct pldm_fwup_ua_state *state, 
    struct device_manager *device_mgr, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_QUERY_DEVICE_IDENTIFIERS) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;
    
    uint8_t completion_code = 0;
	uint32_t device_identifiers_len = 0;
	uint8_t descriptor_count = 0;
	uint8_t *descriptor_data = 0;

    int status = decode_query_device_identifiers_resp(rsp, rsp_payload_length, 
        &completion_code, &device_identifiers_len, &descriptor_count, &descriptor_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    for (int i = 0; i < device_mgr->num_devices; i++) {
        int device_eid = device_manager_get_device_eid(device_mgr, i);
        if(device_eid == response->source_eid) {
            memcpy(&device_mgr->entries[i].pci_vid, 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_mgr->entries[i].pci_device_id, 
            descriptor_data + sizeof (uint16_t), sizeof (uint16_t));
            memcpy(&device_mgr->entries[i].pci_subsystem_vid, 
            descriptor_data + (2 * sizeof (uint16_t)), sizeof (uint16_t));
            memcpy(&device_mgr->entries[i].pci_subsystem_id, 
            descriptor_data + (3 * sizeof (uint16_t)), sizeof (uint16_t));
            break;
        } else if (ROT_IS_ERROR(device_eid)) {
            return device_eid;
        }
    }

    response->length = 0;
    return status;
}


/**
* Generate a GetFirmwareParameters request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_get_firmware_parameters_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len)
{   

    static uint8_t instance_id;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_get_firmware_parameters_req(instance_id, rq_payload_length, rq);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_GET_FIRMWARE_PARAMETERS;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}


/**
 * Process a GetFirmwareParameters response.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param rec_fw_parameters - Pointer to the firmware parameters received from the FD.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_get_firmware_parameters_response(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_protocol_firmware_parameters *rec_fw_parameters, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_GET_FIRMWARE_PARAMETERS) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    struct pldm_get_firmware_parameters_resp rsp_data = {0};
	struct variable_field active_comp_img_set_ver = {0};
	struct variable_field pending_comp_img_set_ver = {0};
	struct variable_field comp_parameter_table = {0};

    int status = decode_get_firmware_parameters_resp(rsp, rsp_payload_length, &rsp_data, &active_comp_img_set_ver,
        &pending_comp_img_set_ver, &comp_parameter_table);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = rsp_data.completion_code;
    if (rsp_data.completion_code != PLDM_SUCCESS) {
        return 0;
    }

    rec_fw_parameters->count = rsp_data.comp_count;
    rec_fw_parameters->capabilities_during_update = rsp_data.capabilities_during_update;
    memcpy(rec_fw_parameters->active_comp_img_set_ver.version_str, active_comp_img_set_ver.ptr, active_comp_img_set_ver.length);
    rec_fw_parameters->active_comp_img_set_ver.version_str_length = rsp_data.active_comp_image_set_ver_str_len;
    rec_fw_parameters->active_comp_img_set_ver.version_str_type = rsp_data.active_comp_image_set_ver_str_type;
    memcpy(rec_fw_parameters->pending_comp_img_set_ver.version_str, pending_comp_img_set_ver.ptr, pending_comp_img_set_ver.length);
    rec_fw_parameters->pending_comp_img_set_ver.version_str_length = rsp_data.pending_comp_image_set_ver_str_len;
    rec_fw_parameters->pending_comp_img_set_ver.version_str_type = rsp_data.pending_comp_image_set_ver_str_type;

    rec_fw_parameters->entries = platform_calloc(rsp_data.comp_count, sizeof (struct pldm_fwup_protocol_component_parameter_entry));

    size_t offset = 0;
    for (int i = 0; i < rsp_data.comp_count; i++) {
        size_t comp_parameter_table_fixed_len = sizeof (struct pldm_fwup_protocol_component_parameter_entry) - 
            (2 * sizeof (struct pldm_fwup_protocol_version_string));
        size_t comp_ver_fixed_len = sizeof (struct pldm_fwup_protocol_version_string) - 255;

        memcpy(&rec_fw_parameters->entries[i], comp_parameter_table.ptr + offset, comp_parameter_table_fixed_len);
        memcpy(&rec_fw_parameters->entries[i].active_comp_ver, comp_parameter_table.ptr + offset + comp_parameter_table_fixed_len,
            comp_ver_fixed_len);
        memcpy(rec_fw_parameters->entries[i].active_comp_ver.version_str, comp_parameter_table.ptr + offset 
        + comp_parameter_table_fixed_len + comp_ver_fixed_len, rec_fw_parameters->entries[i].active_comp_ver.version_str_length);

        memcpy(&rec_fw_parameters->entries[i].pending_comp_ver, comp_parameter_table.ptr + offset + 
            comp_parameter_table_fixed_len + comp_ver_fixed_len + rec_fw_parameters->entries[i].active_comp_ver.version_str_length,
            comp_ver_fixed_len);
        memcpy(rec_fw_parameters->entries[i].pending_comp_ver.version_str, comp_parameter_table.ptr + offset + 
            comp_parameter_table_fixed_len + comp_ver_fixed_len + 
            rec_fw_parameters->entries[i].active_comp_ver.version_str_length + comp_ver_fixed_len,
            rec_fw_parameters->entries[i].pending_comp_ver.version_str_length);

        offset += comp_parameter_table_fixed_len + (2 * comp_ver_fixed_len) + rec_fw_parameters->entries[i].pending_comp_ver.version_str_length +
            rec_fw_parameters->entries[i].active_comp_ver.version_str_length;
    }

    response->length = 0;
    return status;
}

/*******************
 * UA Update commands
 *******************/

/**
* Generate a RequestUpdate request.
*
* @param ua_mgr The manager for the UA.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_request_update_request(struct pldm_fwup_ua_manager *ua_mgr, 
    uint8_t *buffer, size_t buf_len)
{
    
    static uint8_t instance_id;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    uint32_t max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
	uint8_t max_outstanding_transfer_req = PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ;
    uint16_t num_of_comp = ua_mgr->num_components;

	struct variable_field comp_img_set_ver;
    comp_img_set_ver.length = ua_mgr->fup_comp_img_set_ver->version_str_length;
    comp_img_set_ver.ptr = (const uint8_t *)ua_mgr->fup_comp_img_set_ver->version_str;

   	uint16_t pkg_data_len = ua_mgr->flash_mgr->package_data_size;
    uint8_t comp_img_set_ver_str_type = ua_mgr->fup_comp_img_set_ver->version_str_type;
    uint8_t comp_img_set_ver_str_len = ua_mgr->fup_comp_img_set_ver->version_str_length;

    size_t rq_payload_length = sizeof (struct pldm_request_update_req) + comp_img_set_ver.length;

    int status = encode_request_update_req(instance_id, max_transfer_size, num_of_comp, max_outstanding_transfer_req,
        pkg_data_len, comp_img_set_ver_str_type, comp_img_set_ver_str_len, &comp_img_set_ver, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    ua_mgr->state.previous_cmd = PLDM_REQUEST_UPDATE;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}




/**
 * Process a RequestUpdate response.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param update_info - Update information retained by UA.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_request_update_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_REQUEST_UPDATE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t fd_will_send_pkg_data_cmd = 0;
    uint16_t fd_meta_data_len = 0;
    uint8_t completion_code = 0;

    int status = decode_request_update_resp(rsp, rsp_payload_length, &completion_code,
    &fd_meta_data_len, &fd_will_send_pkg_data_cmd);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }
    
    update_info->fd_will_send_pkg_data_cmd = fd_will_send_pkg_data_cmd;
    update_info->fd_meta_data_len = fd_meta_data_len;

    return status;
}


/**
* Process a GetPackageData request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_get_package_data_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint32_t data_transfer_handle;
    uint8_t transfer_operation_flag;

    int status = decode_get_package_data_req(rq, rq_payload_length, &data_transfer_handle, &transfer_operation_flag);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    uint8_t transfer_flag = 0;
    struct variable_field portion_of_package_data;
    uint32_t next_data_transfer_handle = 0;
    portion_of_package_data.ptr = (const uint8_t *)&completion_code;
    portion_of_package_data.length = sizeof (completion_code);
    uint8_t package_data_buf[PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE];

    if (state->previous_cmd != PLDM_REQUEST_UPDATE && state->previous_cmd != PLDM_GET_PACKAGE_DATA) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
        goto exit;
    }
    else if (flash_mgr->package_data_size <= 0) {
        completion_code = PLDM_FWUP_NO_PACKAGE_DATA;
        goto exit;
    }
    else if (get_cmd_state->transfer_flag == PLDM_MIDDLE && transfer_operation_flag == PLDM_GET_FIRSTPART) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_OPERATION_FLAG;
        goto exit;
    } 
    else if (data_transfer_handle != get_cmd_state->next_data_transfer_handle) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_HANDLE;
        goto exit;
    }

    if (transfer_operation_flag == PLDM_GET_FIRSTPART) {
        if (flash_mgr->package_data_size < PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE) {
            transfer_flag = PLDM_START_AND_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_START;
            next_data_transfer_handle = data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
        }
    }
    else {
        if (data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE >= flash_mgr->package_data_size) {
            transfer_flag = PLDM_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_MIDDLE;
            next_data_transfer_handle = data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
        }
    }

    status = flash_mgr->flash->read(flash_mgr->flash, flash_mgr->package_data_region.start_addr + data_transfer_handle,
        package_data_buf, PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE);
    if (ROT_IS_ERROR(status)) {
        return status;
    }
    portion_of_package_data.ptr = (const uint8_t *)package_data_buf;
    portion_of_package_data.length = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_multipart_transfer_resp) + portion_of_package_data.length;

    status = encode_get_package_data_resp(instance_id, rsp_payload_length, rsp, completion_code,
        next_data_transfer_handle, transfer_flag, &portion_of_package_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }
    
    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_GET_PACKAGE_DATA;
    get_cmd_state->next_data_transfer_handle = next_data_transfer_handle;
    get_cmd_state->transfer_flag = transfer_flag;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/**
* Generate a GetDeviceMetaData request.
*
* @param state - Variable context for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_device_meta_data_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint32_t data_transfer_handle = get_cmd_state->data_transfer_handle;
    uint8_t transfer_operation_flag = get_cmd_state->transfer_op_flag;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_multipart_transfer_req);

    int status = encode_get_device_meta_data_req(instance_id, rq_payload_length, rq, data_transfer_handle, transfer_operation_flag);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_GET_DEVICE_METADATA;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD  + rq_payload_length;
    
}


/**
* Process a GetDeviceMetaData response.
*
* @param state - Variable context for a PLDM FWUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_device_meta_data_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_GET_DEVICE_METADATA) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code;
    uint32_t next_data_transfer_handle;
    uint8_t transfer_flag;
	struct variable_field portion_of_device_meta_data;

    int status = decode_get_device_meta_data_resp(rsp, &completion_code, &next_data_transfer_handle, 
        &transfer_flag, &portion_of_device_meta_data, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    status = flash_mgr->flash->write(flash_mgr->flash, 
        flash_mgr->device_meta_data_region.start_addr + get_cmd_state->data_transfer_handle, 
        (uint8_t *)portion_of_device_meta_data.ptr, portion_of_device_meta_data.length);
    if (ROT_IS_ERROR(status)) {
        return status;
    } else {
        status = 0;
    }

    if (transfer_flag == PLDM_START || transfer_flag == PLDM_MIDDLE) {
        get_cmd_state->transfer_op_flag = PLDM_GET_NEXTPART;
        get_cmd_state->data_transfer_handle = next_data_transfer_handle;
    } else {
        get_cmd_state->transfer_op_flag = PLDM_GET_FIRSTPART;
    }

    return status;
}


/**
* Generate a PassComponentTable request.
*
* @param ua_mgr - Manager for the UA. 
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_pass_component_table_request(struct pldm_fwup_ua_manager *ua_mgr,
    uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint8_t comp_num = ua_mgr->current_comp_num;
    uint8_t transfer_flag;
    if (comp_num == 0) {
        if (comp_num == ua_mgr->num_components - 1) {
            transfer_flag = PLDM_START_AND_END;
        } else {
            transfer_flag = PLDM_START;
        }
    }
    else if (comp_num > 0) {
        if (comp_num == ua_mgr->num_components - 1) {
            transfer_flag = PLDM_END;
        } else {
            transfer_flag = PLDM_MIDDLE;
        }
    }

    uint16_t comp_classification = ua_mgr->comp_img_entries[comp_num].comp_classification;
	uint16_t comp_identifier = ua_mgr->comp_img_entries[comp_num].comp_identifier;
	uint8_t comp_classification_index = 0;
    for (int i = 0; i < ua_mgr->rec_fw_parameters.count; i++) {
        if (comp_identifier == ua_mgr->rec_fw_parameters.entries[i].comp_identifier) {
            comp_classification_index = ua_mgr->rec_fw_parameters.entries[i].comp_classification_index;
            break;
        }
    }
	uint32_t comp_comparison_stamp = ua_mgr->comp_img_entries[comp_num].comp_comparison_stamp;
	uint8_t comp_ver_str_type = ua_mgr->comp_img_entries[comp_num].comp_ver.version_str_type;
	uint8_t comp_ver_str_len = ua_mgr->comp_img_entries[comp_num].comp_ver.version_str_length;
	struct variable_field comp_ver;
    comp_ver.ptr = ua_mgr->comp_img_entries[comp_num].comp_ver.version_str;
    comp_ver.length = comp_ver_str_len;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_pass_component_table_req) + comp_ver.length;

    int status = encode_pass_component_table_req(instance_id, transfer_flag, comp_classification, comp_identifier, 
        comp_classification_index, comp_comparison_stamp, comp_ver_str_type, comp_ver_str_len, &comp_ver, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    ua_mgr->state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    instance_id += 1;
    comp_num += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}




/**
* Process a PassComponentTable response.
*
* @param state - Variable context for a PLDM FWUP.
* @param comp_img_entries - Component image information entries from the FUP.
* @param current_comp_num - Current component to be updated. 
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_pass_component_table_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_fup_component_image_entry *comp_img_entries, uint16_t current_comp_num, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_PASS_COMPONENT_TABLE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
	uint8_t comp_resp = 0;
	uint8_t comp_resp_code = 0;
    int status = decode_pass_component_table_resp(rsp, rsp_payload_length, &completion_code, &comp_resp, &comp_resp_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    comp_img_entries[current_comp_num].comp_resp = comp_resp;
    comp_img_entries[current_comp_num].comp_resp_code = comp_resp_code;

    return status;
}


/**
* Generate a UpdateComponent request.
*
* @param state - Variable context for a PLDM FWUP.
* @param current_comp_num - Current component to be updated. 
* @param comp_img_entries - Component image information entries from the FUP.
* @param rec_fw_parameters - Received FD firmware parameters. 
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*
* @note For AMI, any additional reserved update options should be handled by the AMI team. 
*/
int pldm_fwup_generate_update_component_request(struct pldm_fwup_ua_state *state,
    uint16_t current_comp_num, struct pldm_fwup_fup_component_image_entry *comp_img_entries,
    struct pldm_fwup_protocol_firmware_parameters *rec_fw_parameters, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint16_t comp_classification = comp_img_entries[current_comp_num].comp_classification;
	uint16_t comp_identifier = comp_img_entries[current_comp_num].comp_identifier;
    uint8_t comp_classification_index = 0;
    for (int i = 0; i < rec_fw_parameters->count; i++) {
        if (comp_identifier == rec_fw_parameters->entries[i].comp_identifier) {
            comp_classification_index = rec_fw_parameters->entries[i].comp_classification_index;
            break;
        }
    }
	uint32_t comp_comparison_stamp = comp_img_entries[current_comp_num].comp_comparison_stamp;
    uint32_t comp_img_size = comp_img_entries[current_comp_num].comp_size;
	bitfield32_t update_option_flags;
    update_option_flags.value = 0;
    if (comp_img_entries[current_comp_num].comp_options.bits.bit0) {
        update_option_flags.bits.bit0 = 1;
    }
    uint8_t comp_ver_str_type = comp_img_entries[current_comp_num].comp_ver.version_str_type;
	uint8_t comp_ver_str_len = comp_img_entries[current_comp_num].comp_ver.version_str_length;
    struct variable_field comp_ver;
    comp_ver.ptr = comp_img_entries[current_comp_num].comp_ver.version_str;
    comp_ver.length = comp_ver_str_len;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_update_component_req) + comp_ver.length;

    int status = encode_update_component_req(instance_id, comp_classification, comp_identifier, comp_classification_index,
        comp_comparison_stamp, comp_img_size, update_option_flags, comp_ver_str_type, comp_ver_str_len, &comp_ver, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }
    
    state->previous_cmd = PLDM_UPDATE_COMPONENT;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}


/**
* Process a UpdateComponent response.
*
* @param state - Variable context for a PLDM FWUP.
* @param comp_img_entries - Component image information entries from the FUP.
* @param current_comp_num - Current component to be updated. 
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_update_component_response(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_fup_component_image_entry *comp_img_entries, uint16_t current_comp_num, 
    struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_UPDATE_COMPONENT) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
	uint8_t comp_compatibility_resp = 0;
	uint8_t comp_compatibility_resp_code = 0;
	bitfield32_t update_option_flags_enabled;
    update_option_flags_enabled.value = 0;
	uint16_t time_before_req_fw_data = 0;

    int status = decode_update_component_resp(rsp, rsp_payload_length, &completion_code, &comp_compatibility_resp, 
        &comp_compatibility_resp_code, &update_option_flags_enabled, &time_before_req_fw_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    comp_img_entries[current_comp_num].comp_compatibility_resp = comp_compatibility_resp;
    comp_img_entries[current_comp_num].comp_compatibility_resp_code = comp_compatibility_resp_code;
    comp_img_entries[current_comp_num].update_option_flags_enabled = update_option_flags_enabled;
    comp_img_entries[current_comp_num].time_before_req_fw_data = time_before_req_fw_data;

 
    return status;
}

/**
* Process a RequestFirmwareData request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param current_comp_num - Current component to be updated. 
* @param comp_img_entries - Component image information entries from the FUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*
* @note For AMI, RETRY_REQUEST_FW_DATA is returned if the flash region has a length set to zero. This should be changed to a more
*       robust check in the future. 
*/
int pldm_fwup_process_request_firmware_data_request(struct pldm_fwup_ua_state *state,
    uint16_t current_comp_num, struct pldm_fwup_fup_component_image_entry *comp_img_entries,
    struct pldm_fwup_flash_manager *flash_mgr, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint32_t offset = 0;
    uint32_t length = 0;

    int status = decode_request_firmware_data_req(rq, rq_payload_length, &offset, &length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (completion_code);

    if (length > PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_LENGTH;
    }
    else if (state->previous_cmd != PLDM_UPDATE_COMPONENT && state->previous_cmd != PLDM_REQUEST_FIRMWARE_DATA) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
    }
    else if (offset > comp_img_entries[current_comp_num].comp_size) {
        completion_code = PLDM_FWUP_DATA_OUT_OF_RANGE;
    }
    else if (flash_mgr->comp_regions[current_comp_num].length <= 0) {
        completion_code = PLDM_FWUP_RETRY_REQUEST_FW_DATA;
    }
    else if (state->previous_cmd == PLDM_CANCEL_UPDATE_COMPONENT || 
        state->previous_cmd == PLDM_CANCEL_UPDATE) {
        completion_code = PLDM_FWUP_CANCEL_PENDING;
    } else {
        status = flash_mgr->flash->read(flash_mgr->flash, 
            flash_mgr->comp_regions[current_comp_num].start_addr + offset, rsp->payload + 1, length);
        if (ROT_IS_ERROR(status)) {
            return status;
        } else {
            rsp_payload_length += length;
        }
    }

    status = encode_request_firmware_data_resp(instance_id, completion_code, rsp, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_REQUEST_FIRMWARE_DATA;
    state->previous_completion_code = completion_code;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}


/**
* Process a TransferComplete request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param update_info - Update information retained by UA.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_transfer_complete_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t transfer_result = 0;
    
    int status = decode_transfer_complete_req(rq, rq_payload_length, &transfer_result);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->transfer_result = transfer_result;

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    if (state->previous_cmd != PLDM_REQUEST_FIRMWARE_DATA) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (completion_code);

    status = encode_transfer_complete_resp(instance_id, completion_code, rsp, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_TRANSFER_COMPLETE;
    state->previous_completion_code = completion_code;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;

}

/**
* Process a VerifyComplete request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param update_info - Update information retained by UA.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_verify_complete_request(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t verify_result = 0;
    
    int status = decode_verify_complete_req(rq, rq_payload_length, &verify_result);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->verify_result = verify_result;

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    if (state->previous_cmd != PLDM_TRANSFER_COMPLETE) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (completion_code);

    status = encode_verify_complete_resp(instance_id, completion_code, rsp, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_VERIFY_COMPLETE;
    state->previous_completion_code = completion_code;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/**
* Process a ApplyComplete request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param update_info - Update information retained by UA.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_apply_complete_request(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t apply_result = 0;
    bitfield16_t comp_activation_methods_modification;
    
    int status = decode_apply_complete_req(rq, rq_payload_length, &apply_result, &comp_activation_methods_modification);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->apply_result = apply_result;
    update_info->comp_activation_methods_modification = comp_activation_methods_modification.value;

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    if (state->previous_cmd != PLDM_VERIFY_COMPLETE) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (completion_code);

    status = encode_apply_complete_resp(instance_id, completion_code, rsp, rsp_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_APPLY_COMPLETE;
    state->previous_completion_code = completion_code;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/**
* Process a GetMetaData request and generate a response.
*
* @param state - Variable context for a PLDM FWUP.
* @param flash_mgr - The flash manager for a PLDM FWUP.
* @param get_cmd_state - Variable context for a multipart transfer. 
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_get_meta_data_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *request)
{
    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint32_t data_transfer_handle;
    uint8_t transfer_operation_flag;

    int status = decode_get_meta_data_req(rq, rq_payload_length, &data_transfer_handle, &transfer_operation_flag);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    uint8_t transfer_flag = 0;
    struct variable_field portion_of_meta_data;
    uint32_t next_data_transfer_handle = 0;
    portion_of_meta_data.ptr = (const uint8_t *)&completion_code;
    portion_of_meta_data.length = sizeof (completion_code);
    uint8_t meta_data_buf[PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE];

    if (flash_mgr->device_meta_data_size <= 0) {
        completion_code = PLDM_FWUP_NO_PACKAGE_DATA;
        goto exit;
    }
    else if (get_cmd_state->transfer_flag == PLDM_MIDDLE && transfer_operation_flag == PLDM_GET_FIRSTPART) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_OPERATION_FLAG;
        goto exit;
    } 
    else if (data_transfer_handle != get_cmd_state->next_data_transfer_handle) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_HANDLE;
        goto exit;
    }

    if (transfer_operation_flag == PLDM_GET_FIRSTPART) {
        if (flash_mgr->device_meta_data_size < PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE) {
            transfer_flag = PLDM_START_AND_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_START;
            next_data_transfer_handle = data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
        }
    }
    else {
        if (data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE >= flash_mgr->device_meta_data_size) {
            transfer_flag = PLDM_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_MIDDLE;
            next_data_transfer_handle = data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
        }
    }

    status = flash_mgr->flash->read(flash_mgr->flash, flash_mgr->device_meta_data_region.start_addr + data_transfer_handle,
        meta_data_buf, PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE);
    if (ROT_IS_ERROR(status)) {
        return status;
    }
    portion_of_meta_data.ptr = (const uint8_t *)meta_data_buf;
    portion_of_meta_data.length = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_multipart_transfer_resp) + portion_of_meta_data.length;

    status = encode_get_meta_data_resp(instance_id, rsp_payload_length, rsp, completion_code,
        next_data_transfer_handle, transfer_flag, &portion_of_meta_data);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }
    
    state->previous_completion_code = completion_code;
    state->previous_cmd = PLDM_GET_META_DATA;
    get_cmd_state->next_data_transfer_handle = next_data_transfer_handle;
    get_cmd_state->transfer_flag = transfer_flag;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}

/**
* Generate a ActivateFirmware request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*
* @note For AMI, this is skeleton code. The activation request field should be set based on the specifics required by the AMI team. 
*/
int pldm_fwup_generate_activate_firmware_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    bool8_t self_contained_activation_req = 1;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_activate_firmware_req);

    int status = encode_activate_firmware_req(instance_id, self_contained_activation_req, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }
    
    state->previous_cmd = PLDM_ACTIVATE_FIRMWARE;
    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;

}

/**
* Process a ActivateFirmware response.
*
* @param state - Variable context for a PLDM FWUP.
* @param update_info - Update information retained by UA.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*
*/
int pldm_fwup_process_activate_firmware_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_ACTIVATE_FIRMWARE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    uint16_t estimated_time_activation = 0;
    
    int status = decode_activate_firmware_resp(rsp, rsp_payload_length, &completion_code, &estimated_time_activation);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->estimated_time_activation = estimated_time_activation;
    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }
    
    return 0;
}

/**
* Generate a GetStatus request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_get_status_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_get_status_req(instance_id, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_GET_STATUS;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
 * Process a GetStatus response.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param update_info - Update information retained by UA.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
 * @note For AMI, this is skeleton code. The current implementation of the firmware update flow does not ever call GetStatus. 
*/
int pldm_fwup_process_get_status_response(struct pldm_fwup_ua_state *state, struct pldm_fwup_ua_update_info *update_info,
     struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_GET_STATUS) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    uint8_t current_state = 0;
	uint8_t previous_state = 0;
    uint8_t aux_state = 0;
	uint8_t aux_state_status = 0;
    uint8_t progress_percent = 0;
	uint8_t reason_code = 0;
	bitfield32_t update_option_flags_enabled;
    update_option_flags_enabled.value = 0;

    int status = decode_get_status_resp(rsp, rsp_payload_length, &completion_code, &current_state, &previous_state,
        &aux_state, &aux_state_status, &progress_percent, &reason_code, &update_option_flags_enabled);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->fd_status.aux_state = aux_state;
    update_info->fd_status.aux_state_status = aux_state_status;
    update_info->fd_status.current_state = current_state;
    update_info->fd_status.previous_state = previous_state;
    update_info->fd_status.progress_percent = progress_percent;
    update_info->fd_status.reason_code = reason_code;
    update_info->fd_status.update_option_flags_enabled = update_option_flags_enabled.value;


    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }


    return 0;

}


/**
* Generate a CancelUpdateComponent request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_cancel_update_component_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_cancel_update_component_req(instance_id, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_CANCEL_UPDATE_COMPONENT;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
 * Process a CancelUpdateComponent response.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
 */
int pldm_fwup_process_cancel_update_component_response(struct pldm_fwup_ua_state *state, struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_CANCEL_UPDATE_COMPONENT) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;

    int status = decode_cancel_update_component_resp(rsp, rsp_payload_length, &completion_code);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    return 0;
}

/**
* Generate a CancelUpdate request.
*
* @param state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_cancel_update_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len)
{
    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    size_t rq_payload_length = 0;

    int status = encode_cancel_update_req(instance_id, rq, rq_payload_length);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    state->previous_cmd = PLDM_CANCEL_UPDATE;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD;
}

/**
 * Process a CancelUpdate response.
 * 
 * @param state - Variable context for a PLDM FWUP.
 * @param update_info - Update information retained by UA.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
 * @note For AMI, this is skeleton code. The handling of the non functioning component indication and bitmap is left to the AMI team. 
 */
int pldm_fwup_process_cancel_update_response(struct pldm_fwup_ua_state *state, struct pldm_fwup_ua_update_info *update_info, 
    struct cmd_interface_msg *response)
{
    if (state->previous_cmd != PLDM_CANCEL_UPDATE) {
        return CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED;
    }
    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t completion_code = 0;
    bool8_t non_functioning_component_indication = 0;
	bitfield64_t non_functioning_component_bitmap;
    non_functioning_component_bitmap.value = 0;

    int status = decode_cancel_update_resp(rsp, rsp_payload_length, &completion_code, &non_functioning_component_indication, 
        &non_functioning_component_bitmap);
    if (status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    }

    update_info->non_functioning_component_indication = non_functioning_component_indication;
    update_info->non_functioning_component_bitmap.value = non_functioning_component_bitmap.value;

    state->previous_completion_code = completion_code;
    response->length = 0;
    if (completion_code != PLDM_SUCCESS) {
        return 0;
    }

    return 0;
}