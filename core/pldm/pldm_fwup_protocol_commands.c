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
 * Global Helper functions
 *******************/

void print_buffer_data(const uint8_t *data, size_t len) {
    printf("Bytes:");
    for (size_t i = 0; i < len; ++i) {
        printf(" %02x", data[i]);
    }
    printf("\n");
}

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
 * @note A QueryDeviceIdentifiers request does not contain payload data.
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
 * @note The function does not yet handle UNABLE_TO_INITIATE_UPDATE and RETRY_REQUEST_UPDATE errors and corresponding state changes.
 *       This should be implemented later on according to some other cerberus criteria.
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
    if (status != 0) {
        return status;
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

    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint32_t data_transfer_handle = get_cmd_state->data_transfer_handle;
    uint8_t transfer_operation_flag = get_cmd_state->transfer_op_flag;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_multipart_transfer_req);

    int status = encode_get_package_data_req(instance_id, rq_payload_length, rq, data_transfer_handle, transfer_operation_flag);
    if (status != 0) {
        return status;
    }

    printf("REQUEST | instance: %d, data transfer handle: %d, transfer op flag: %d.\n", instance_id, data_transfer_handle, transfer_operation_flag);

    switch_state(state, PLDM_FD_STATE_LEARN_COMPONENTS);
    state->previous_cmd = PLDM_GET_PACKAGE_DATA;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD + sizeof (struct pldm_multipart_transfer_req);
    
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
    if (status != 0) {
        return status;
    }
    state->previous_completion_code = completion_code;
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

    printf("RESPONSE | next data transfer handle: %d, transfer flag: %d, CRC: %d.\n", 
        next_data_transfer_handle, transfer_flag, crc32(portion_of_package_data.ptr, portion_of_package_data.length));

    response->length = 0;
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
    if (status != 0) {
        return status;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    uint8_t transfer_flag = 0;
    struct variable_field portion_of_device_meta_data;
    uint32_t next_data_transfer_handle = 0;
    portion_of_device_meta_data.ptr = (const uint8_t *)&completion_code;
    portion_of_device_meta_data.length = sizeof (completion_code);
    uint8_t device_meta_data_buf[(size_t)update_info->max_transfer_size];

    if (state->previous_cmd != PLDM_GET_PACKAGE_DATA || state->previous_cmd != PLDM_GET_DEVICE_METADATA) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
        goto exit;
    }
    else if (update_info->package_data_len <= 0) {
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
* @param device_mgr - The device manager linked to command interface.
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*
int pldm_fwup_process_pass_component_table_request(struct device_manager *device_mgr, 
    struct pldm_fwup_state *fwup_state, struct cmd_interface_msg *request)
{
    switch_command(fwup_state, PLDM_PASS_COMPONENT_TABLE);

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
    if (status != 0) {
        return status;
    }

    uint8_t completion_code = PLDM_SUCCESS;
	uint8_t comp_resp = 0;
	uint8_t comp_resp_code = 0;
    static uint8_t instance_id = 1;

    if (!fwup_state->update_mode) {
        completion_code = PLDM_FWUP_NOT_IN_UPDATE_MODE;
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_PREREQUISITES_NOT_MET;
        goto exit;
    } else if (fwup_state->state != PLDM_FD_STATE_LEARN_COMPONENTS) {
        completion_code = PLDM_FWUP_INVALID_STATE_FOR_COMMAND;
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_PREREQUISITES_NOT_MET;
        goto exit;
    }

    static uint8_t comp_table_num = 0;
    if (transfer_flag == PLDM_START || transfer_flag == PLDM_START_AND_END) {
        comp_table_num = 0;
        fwup_state->device_ctx.entries[comp_table_num].comp_classification = comp_classification;
        fwup_state->device_ctx.entries[comp_table_num].comp_comparison_stamp = comp_comparison_stamp;
        fwup_state->device_ctx.entries[comp_table_num].comp_identifier = comp_identifier;
        fwup_state->device_ctx.entries[comp_table_num].comp_ver_str_type = comp_ver_str_type;
        fwup_state->device_ctx.entries[comp_table_num].comp_ver_str_len = comp_ver_str_len;
        memcpy(fwup_state->device_ctx.entries[comp_table_num].comp_ver_str, comp_ver_str.ptr, comp_ver_str.length);
    } 
    else if (transfer_flag == PLDM_MIDDLE) {
        comp_table_num += 1;
        fwup_state->device_ctx.entries[comp_table_num].comp_classification = comp_classification;
        fwup_state->device_ctx.entries[comp_table_num].comp_comparison_stamp = comp_comparison_stamp;
        fwup_state->device_ctx.entries[comp_table_num].comp_identifier = comp_identifier;
        fwup_state->device_ctx.entries[comp_table_num].comp_ver_str_type = comp_ver_str_type;
        fwup_state->device_ctx.entries[comp_table_num].comp_ver_str_len = comp_ver_str_len;
        memcpy(fwup_state->device_ctx.entries[comp_table_num].comp_ver_str, comp_ver_str.ptr, comp_ver_str.length);
    } else if (transfer_flag == PLDM_END) {
        comp_table_num += 1;
        fwup_state->device_ctx.entries[comp_table_num].comp_classification = comp_classification;
        fwup_state->device_ctx.entries[comp_table_num].comp_comparison_stamp = comp_comparison_stamp;
        fwup_state->device_ctx.entries[comp_table_num].comp_identifier = comp_identifier;
        fwup_state->device_ctx.entries[comp_table_num].comp_ver_str_type = comp_ver_str_type;
        fwup_state->device_ctx.entries[comp_table_num].comp_ver_str_len = comp_ver_str_len;
        memcpy(fwup_state->device_ctx.entries[comp_table_num].comp_ver_str, comp_ver_str.ptr, comp_ver_str.length);
        comp_table_num = 0;
    }

    int num_components = device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.request.comp_count;
    int device_mgr_comp_num;
    bool comp_supported = 0;
    for (device_mgr_comp_num = 0; device_mgr_comp_num < num_components; device_mgr_comp_num++) {
        if (comp_classification_index == device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[device_mgr_comp_num].request.comp_classification_index) {
            comp_supported = 1;
            break;
        }
    }

    if (!comp_supported) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_NOT_SUPPORTED;
    }
    else if (comp_comparison_stamp == device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[device_mgr_comp_num].request.pending_comp_comparison_stamp) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_COMPARISON_STAMP_IDENTICAL;
    } 
    else if (comp_comparison_stamp < device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[device_mgr_comp_num].request.pending_comp_comparison_stamp) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_COMPARISON_STAMP_LOWER;
    } 
    else if (buffer_compare(comp_ver_str.ptr, (const uint8_t *)device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[device_mgr_comp_num].pending_comp_ver_str, 
        (size_t)comp_ver_str_len)) {
        comp_resp = PLDM_CR_COMP_MAY_BE_UPDATEABLE;
        comp_resp_code = PLDM_CRC_COMP_VER_STR_IDENTICAL;

    } else {
        comp_resp = PLDM_CR_COMP_CAN_BE_UPDATED;
        comp_resp = PLDM_CRC_COMP_CAN_BE_UPDATED;
    }

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_pass_component_table_resp);

    status = encode_pass_component_table_resp(instance_id, rsp, rsp_payload_length, completion_code, comp_resp, comp_resp_code);
    fwup_state->completion_code = completion_code;
    if (transfer_flag == PLDM_START || transfer_flag == PLDM_MIDDLE) {
        switch_state(fwup_state, PLDM_FD_STATE_LEARN_COMPONENTS);
    } 
    else if (transfer_flag == PLDM_END || transfer_flag == PLDM_START_AND_END) {
        switch_state(fwup_state, PLDM_FD_STATE_READY_XFER);
    }
    fwup_state->comp_transfer_flag = transfer_flag;
    request->length = rsp_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
    instance_id += 1;
    return status;
}*/


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
    if (status != 0) {
        return status;
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
    if (status != 0) {
        return status;
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
            memcpy(&device_mgr->entries[i].pci_device_id + sizeof (uint16_t), 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_mgr->entries[i].pci_vid + (2 * sizeof (uint16_t)), 
            descriptor_data, sizeof (uint16_t));
            memcpy(&device_mgr->entries[i].pci_vid + (3 * sizeof (uint16_t)), 
            descriptor_data, sizeof (uint16_t));
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
    if (status != 0) {
        return status;
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
    if (status != 0) {
        return status;
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
    comp_img_set_ver.length = ua_mgr->comp_img_set_ver.version_str_length;
    comp_img_set_ver.ptr = (const uint8_t *)ua_mgr->comp_img_set_ver.version_str;

   	uint16_t pkg_data_len = ua_mgr->flash_mgr->package_data_size;
    uint8_t comp_img_set_ver_str_type = ua_mgr->comp_img_set_ver.version_str_type;
    uint8_t comp_img_set_ver_str_len = ua_mgr->comp_img_set_ver.version_str_length;

    size_t rq_payload_length = sizeof (struct pldm_request_update_req) + comp_img_set_ver.length;

    int status = encode_request_update_req(instance_id, max_transfer_size, num_of_comp, max_outstanding_transfer_req,
        pkg_data_len, comp_img_set_ver_str_type, comp_img_set_ver_str_len, &comp_img_set_ver, rq, rq_payload_length);
    if (status != 0) {
        return status;
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
    struct pldm_msg *rsp = (struct pldm_msg *)response->data + PLDM_MCTP_BINDING_MSG_OFFSET;
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint8_t fd_will_send_pkg_data_cmd = 0;
    uint16_t fd_meta_data_len = 0;
    uint8_t completion_code = 0;

    int status = decode_request_update_resp(rsp, rsp_payload_length, &completion_code,
    &fd_meta_data_len, &fd_will_send_pkg_data_cmd);
    if (status != 0) {
        return status;
    }
    state->previous_completion_code = completion_code;
    if (completion_code!= PLDM_SUCCESS) {
        return 0;
    }
    
    update_info->fd_will_send_pkg_data_cmd = fd_will_send_pkg_data_cmd;
    update_info->fd_meta_data_len = fd_meta_data_len;

    response->length = 0;
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
    if (status != 0) {
        return status;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    uint8_t transfer_flag = 0;
    struct variable_field portion_of_pkg_data;
    uint32_t next_data_transfer_handle = 0;
    portion_of_pkg_data.ptr = (const uint8_t *)&completion_code;
    portion_of_pkg_data.length = sizeof (completion_code);
    uint8_t device_meta_data_buf[PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE];

    if (state->previous_cmd != PLDM_GET_PACKAGE_DATA || state->previous_cmd != PLDM_GET_DEVICE_METADATA) {
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
        device_meta_data_buf, PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE);
    if (ROT_IS_ERROR(status)) {
        return status;
    }
    portion_of_pkg_data.ptr = (const uint8_t *)device_meta_data_buf;
    portion_of_pkg_data.length = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

    printf("REQUEST/RESPONSE | instance id: %d, data transfer handle: %d, transfer op flag: %d, next data transfer handle: %d, transfer flag: %d\n",
        instance_id, data_transfer_handle, transfer_operation_flag, next_data_transfer_handle, transfer_flag, crc32(portion_of_pkg_data.ptr, portion_of_pkg_data.length));

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_multipart_transfer_resp) + portion_of_pkg_data.length;

    status = encode_get_package_data_resp(instance_id, rsp_payload_length, rsp, completion_code,
        next_data_transfer_handle, transfer_flag, &portion_of_pkg_data);
    
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
    if (status != 0) {
        return status;
    }

    state->previous_cmd = PLDM_GET_PACKAGE_DATA;
    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD + sizeof (struct pldm_multipart_transfer_req);
    
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
    if (state->previous_cmd != PLDM_GET_PACKAGE_DATA) {
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
    if (status != 0) {
        return status;
    }

    state->previous_completion_code = completion_code;
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
    }


    response->length = 0;
    return status;
}


/**
* Generate a PassComponentTable request.
*
* @param device_mgr -The device manager linked to command interface.
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*
int pldm_fwup_generate_pass_component_table_request(struct device_manager *device_mgr, 
struct pldm_fwup_state *fwup_state, uint8_t *buffer, size_t buf_len)
{
    switch_command(fwup_state, PLDM_PASS_COMPONENT_TABLE);

    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_pass_component_table_req);

    uint8_t transfer_flag = 0;
    if (fwup_state->device_ctx.current_component == 0 && fwup_state->device_ctx.num_of_components == 1) {
        transfer_flag == PLDM_START_AND_END;
    } else {
        transfer_flag == PLDM_START;
    }
    
    if (fwup_state->device_ctx.current_component > 0 && fwup_state->device_ctx.current_component != fwup_state->device_ctx.num_of_components - 1) {
        transfer_flag == PLDM_MIDDLE;
    } else {
        transfer_flag = PLDM_END;
    }

    uint16_t comp_classification = fwup_state->device_ctx.entries[fwup_state->device_ctx.current_component].comp_classification;
	uint16_t comp_identifier = fwup_state->device_ctx.entries[fwup_state->device_ctx.current_component].comp_identifier;
	uint8_t comp_classification_index = 0;
	uint32_t comp_comparison_stamp = 0;
	uint8_t comp_ver_str_type = 0;
	uint8_t comp_ver_str_len = 0;
	const struct variable_field comp_ver_str;

    return 0;
}*/
