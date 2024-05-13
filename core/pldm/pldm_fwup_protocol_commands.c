#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "pldm_fwup_protocol_commands.h"
#include "cmd_interface_pldm.h"
#include "status/rot_status.h"
#include "common/unused.h"
#include "platform.h"

#include "libpldm/firmware_update.h"
#include "libpldm/utils.h"

/*******************
 * Global Helper functions
 *******************/
void switch_command(struct pldm_fwup_state *fwup_state, enum pldm_firmware_update_commands new_command) 
{
    fwup_state->previous_command = fwup_state->command;
    fwup_state->command = new_command;
}


#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
/*******************
 * FD Helper functions
 *******************/
void switch_state(struct pldm_fwup_state *fwup_state, enum pldm_firmware_device_states new_state) 
{
    fwup_state->previous_state = fwup_state->state;
    fwup_state->state = new_state;
}

void reset_multipart_transfer(struct pldm_fwup_state *fwup_state) 
{
    fwup_state->multipart_transfer.transfer_op_flag = PLDM_GET_FIRSTPART;
    fwup_state->multipart_transfer.data_transfer_handle = 0x00;
}


/*******************
 * FD Inventory commands
 *******************/

/**
 * Process a QueryDeviceIdentifiers request.
 * 
 * @param fwup_state - Variable context for a PLDM FWUP.
 * @param device_mgr - The device manager linked to command interface.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
 * @note A QueryDeviceIdentifiers request does not contain payload data.
*/
int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_mgr, struct cmd_interface_msg *request)
{
    switch_command(fwup_state, PLDM_QUERY_DEVICE_IDENTIFIERS);

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

    fwup_state->completion_code = completion_code;
    switch_state(fwup_state, PLDM_FD_STATE_IDLE);
    instance_id += 1;
    return status;
}


/**
 * Process a GetFirmwareParameters request.
 * 
 * @param fwup_state - Variable context for a PLDM FWUP.
 * @param device_mgr - The device manager linked to command interface.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_prcocess_get_firmware_parameters_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_mgr, struct cmd_interface_msg *request)
{
    switch_command(fwup_state, PLDM_GET_FIRMWARE_PARAMETERS);

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);

    static uint8_t instance_id = 1;

    struct variable_field active_comp_image_set_ver_str;
    active_comp_image_set_ver_str.length = device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.request.active_comp_img_set_ver_str_len;
    active_comp_image_set_ver_str.ptr = (const uint8_t *)device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.active_comp_img_set_ver_str;

    struct variable_field pending_comp_image_set_ver_str;
    pending_comp_image_set_ver_str.length = device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.request.pending_comp_img_set_ver_str_len;
    pending_comp_image_set_ver_str.ptr = (const uint8_t *)device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.pending_comp_img_set_ver_str;

    uint8_t comp_parameter_table_buf[device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.request.comp_count *
    sizeof (struct pldm_fwup_protocol_full_component_parameter_entry)];

    size_t comp_parameter_table_length;
    for (int i = 0; i < device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.request.comp_count; i++) {
        size_t active_comp_ver_str_len = device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[i].request.active_comp_ver_str_len;
        size_t pending_comp_ver_str_len = device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[i].request.pending_comp_ver_str_len;

        memcpy(comp_parameter_table_buf, &device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[i].request,
        sizeof (struct pldm_component_parameter_entry));
        memcpy(comp_parameter_table_buf + sizeof (struct pldm_component_parameter_entry), 
        device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[i].active_comp_ver_str, active_comp_ver_str_len);
        memcpy(comp_parameter_table_buf + sizeof (struct pldm_component_parameter_entry) + active_comp_ver_str_len, 
        device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.entries[i].pending_comp_ver_str, pending_comp_ver_str_len);

        comp_parameter_table_length += (sizeof (struct pldm_component_parameter_entry) + active_comp_ver_str_len + pending_comp_ver_str_len);
    }

    struct variable_field comp_parameter_table;
    comp_parameter_table.length = comp_parameter_table_length;
    comp_parameter_table.ptr = (const uint8_t *)comp_parameter_table_buf;

    struct pldm_get_firmware_parameters_resp rsp_data;
    memcpy(&rsp_data + sizeof (uint8_t), &device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].fw_parameters.request, 
    sizeof (struct pldm_fwup_protocol_firmware_parameters));
    rsp_data.completion_code = PLDM_SUCCESS;

    size_t rsp_payload_length = sizeof (rsp_data) + active_comp_image_set_ver_str.length + 
        pending_comp_image_set_ver_str.length + comp_parameter_table.length;

    int status = encode_get_firmware_parameters_resp(instance_id, rsp, rsp_payload_length,
        &rsp_data, &active_comp_image_set_ver_str, &pending_comp_image_set_ver_str, &comp_parameter_table);

    fwup_state->completion_code = rsp_data.completion_code;
    switch_state(fwup_state, PLDM_FD_STATE_IDLE);
    instance_id += 1;
    return status;
}   


/*******************
 * FD Update commands
 *******************/

/**
 * Process a RequestUpdate request.
 * 
 * @param fwup_state - Variable state context for a PLDM FWUP.
 * @param fwup_flash - The flash map for a PLDM FWUP.
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
 * @note The function does not yet handle UNABLE_TO_INITIATE_UPDATE and RETRY_REQUEST_UPDATE errors and corresponding state changes.
 *       This should be implemented later on according to some other cerberus criteria.
*/
int pldm_fwup_process_request_update_request(struct pldm_fwup_state *fwup_state, struct pldm_fwup_flash_map *fwup_flash, 
    struct cmd_interface_msg *request)
{   
    switch_command(fwup_state, PLDM_REQUEST_UPDATE);

    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;
    
    struct variable_field comp_img_set_ver_str;
    comp_img_set_ver_str.ptr = (const uint8_t *)fwup_state->updating_device.comp_img_set_set_str;

    int status = decode_request_update_req(rq, rq_payload_length, &fwup_state->max_transfer_size,
    &fwup_state->updating_device.num_of_components, &fwup_state->max_num_outstanding_transfer_req,
    (uint16_t *)&fwup_flash->package_data_size, &fwup_state->updating_device.comp_img_set_ver_str_type,
    &fwup_state->updating_device.comp_img_set_ver_str_len, &comp_img_set_ver_str);
    if (status != 0) {
        return status;
    }

    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_request_update_resp);

    static uint8_t instance_id = 1;

    uint8_t completion_code = 0;
	uint16_t fd_meta_data_len = fwup_flash->device_meta_data_size;
	uint8_t fd_will_send_pkg_data = 0;

    if (fwup_flash->package_data_size > 0) {
        fd_will_send_pkg_data = 1;
    } else {
        fd_will_send_pkg_data = 0;
    }

    if (fwup_state->update_mode) {
        completion_code = PLDM_FWUP_ALREADY_IN_UPDATE_MODE;
        switch_state(fwup_state, PLDM_FD_STATE_IDLE);
    } else {
        completion_code = PLDM_SUCCESS;
        switch_state(fwup_state, PLDM_FD_STATE_LEARN_COMPONENTS);
        fwup_state->update_mode = 1;
    }

    status = encode_request_update_resp(instance_id, rsp_payload_length, rsp, completion_code, 
        fd_meta_data_len, fd_will_send_pkg_data);
   
        
    fwup_state->completion_code = completion_code;
    instance_id += 1;
    return status;
}


/**
* Generate a GetPackageData request.
*
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_state *fwup_state, uint8_t *buffer, size_t buf_len)
{
    fwup_state->command = PLDM_GET_PACKAGE_DATA;

    static uint8_t instance_id = 1;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    uint32_t data_transfer_handle = fwup_state->multipart_transfer.data_transfer_handle;
    uint8_t transfer_operation_flag = fwup_state->multipart_transfer.transfer_op_flag;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = sizeof (struct pldm_multipart_transfer_req);

    int status = encode_get_package_data_req(instance_id, rq_payload_length, rq, data_transfer_handle, transfer_operation_flag);
    if (status != 0) {
        return status;
    }

    printf("REQUEST | instance: %d, data transfer handle: %d, transfer op flag: %d.\n", instance_id, data_transfer_handle, transfer_operation_flag);

    instance_id += 1;
    return PLDM_MCTP_BINDING_MSG_OVERHEAD + sizeof (struct pldm_multipart_transfer_req);
    
}


/**
* Process a GetPackageData response.
*
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param fwup_flash - The flash map for a PLDM FWUP.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_state *fwup_state, 
    struct pldm_fwup_flash_map *fwup_flash, struct cmd_interface_msg *response)
{
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
    fwup_state->completion_code = completion_code;
    if (completion_code != PLDM_SUCCESS) {
        switch_state(fwup_state, PLDM_FD_STATE_LEARN_COMPONENTS);
        return 0;
    }

    status = fwup_flash->package_data_flash->write(fwup_flash->package_data_flash, 
        fwup_flash->package_data_addr + fwup_state->multipart_transfer.data_transfer_handle, 
        (uint8_t *)portion_of_package_data.ptr, portion_of_package_data.length);
    if (ROT_IS_ERROR(status)) {
        return status;
    }

    if (transfer_flag == PLDM_END || transfer_flag == PLDM_START_AND_END) {
        reset_multipart_transfer(fwup_state);
    } else if (transfer_flag == PLDM_MIDDLE) {
        fwup_state->multipart_transfer.transfer_op_flag = PLDM_GET_NEXTPART;
        fwup_state->multipart_transfer.data_transfer_handle = next_data_transfer_handle;
    }

    printf("RESPONSE | next data transfer handle: %d, transfer flag: %d, CRC: %d.\n", 
        next_data_transfer_handle, transfer_flag, crc32(portion_of_package_data.ptr, portion_of_package_data.length));

    switch_state(fwup_state, PLDM_FD_STATE_LEARN_COMPONENTS);
    response->length = 0;
    return status;
}


#else

/*******************
 * UA Inventory commands
 *******************/

/**
* Generate a QueryDeviceIdentifiers request.
*
* @param fwup_state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_query_device_identifiers_request(struct pldm_fwup_state *fwup_state, uint8_t *buffer, size_t buf_len)
{
    switch_command(fwup_state, PLDM_QUERY_DEVICE_IDENTIFIERS);

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


/**
 * Process a QueryDeviceIdentifiers response.
 * 
 * @param fwup_state - Variable context for a PLDM FWUP.
 * @param device_mgr - The device manager linked to command interface.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_query_device_identifiers_response(struct pldm_fwup_state *fwup_state, 
    struct device_manager *device_mgr, struct cmd_interface_msg *response)
{

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

    fwup_state->completion_code = completion_code;
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
* @param fwup_state - Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_get_firmware_parameters_request(struct pldm_fwup_state *fwup_state, uint8_t *buffer, size_t buf_len)
{   
    switch_command(fwup_state, PLDM_GET_FIRMWARE_PARAMETERS);

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


/**
 * Process a GetFirmwareParameters response.
 * 
 * @param fwup_state - Variable context for a PLDM FWUP.
 * @param device_mgr -The device manager linked to command interface.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/

int pldm_fwup_process_get_firmware_parameters_response(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_mgr, struct cmd_interface_msg *response)
{

    struct pldm_msg *rsp = (struct pldm_msg *)(response->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = response->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    struct pldm_get_firmware_parameters_resp rsp_data = {0};
	struct variable_field active_comp_img_set_ver_str = {0};
	struct variable_field pending_comp_img_set_ver_str = {0};
	struct variable_field comp_parameter_table = {0};

    int status = decode_get_firmware_parameters_resp(rsp, rsp_payload_length, &rsp_data, &active_comp_img_set_ver_str,
        &pending_comp_img_set_ver_str, &comp_parameter_table);
    if (status != 0) {
        return status;
    }

    fwup_state->completion_code = rsp_data.completion_code;
    if (rsp_data.completion_code != PLDM_SUCCESS) {
        return 0;
    }

    for (int i = 0; i < device_mgr->num_devices; i++) {
        int device_eid = device_manager_get_device_eid(device_mgr, i);
        if(device_eid == response->source_eid) {
            memcpy(&device_mgr->entries[i].fw_parameters.request, &rsp_data + sizeof (uint8_t), sizeof (struct pldm_fwup_protocol_firmware_parameters));
            memcpy(&device_mgr->entries[i].fw_parameters.active_comp_img_set_ver_str, active_comp_img_set_ver_str.ptr, 
            active_comp_img_set_ver_str.length);
            memcpy(&device_mgr->entries[i].fw_parameters.pending_comp_img_set_ver_str, pending_comp_img_set_ver_str.ptr, 
            pending_comp_img_set_ver_str.length);

            size_t offset = 0;
            for (int j = 0; j < device_mgr->entries[i].fw_parameters.request.comp_count; j++) {
                memcpy(&device_mgr->entries[i].fw_parameters.entries[j].request, comp_parameter_table.ptr + offset, sizeof (struct pldm_component_parameter_entry));
                size_t active_comp_ver_str_len = device_mgr->entries[i].fw_parameters.entries[j].request.active_comp_ver_str_len;
                size_t pending_comp_ver_str_len = device_mgr->entries[i].fw_parameters.entries[j].request.pending_comp_ver_str_len;

                memcpy(device_mgr->entries[i].fw_parameters.entries[j].active_comp_ver_str, comp_parameter_table.ptr + offset + 
                sizeof (struct pldm_component_parameter_entry), active_comp_ver_str_len);
                memcpy(device_mgr->entries[i].fw_parameters.entries[j].active_comp_ver_str, comp_parameter_table.ptr + offset + 
                sizeof (struct pldm_component_parameter_entry) + active_comp_ver_str_len, pending_comp_ver_str_len);

                offset += (sizeof (struct pldm_component_parameter_entry) + active_comp_ver_str_len + pending_comp_ver_str_len);
            }
            break;
        } else if (ROT_IS_ERROR(device_eid)) {
            return device_eid;
        }
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
* @param fwup_flash The flash addresses and devices to use for different PLDM FWUP regions.
* @param fwup_state Variable context for a PLDM FWUP.
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return size of the message payload or an error code.
*/
int pldm_fwup_generate_request_update_request(struct pldm_fwup_flash_map *fwup_flash, struct pldm_fwup_state *fwup_state, 
    uint8_t *buffer, size_t buf_len)
{
    switch_command(fwup_state, PLDM_REQUEST_UPDATE);
    
    static uint8_t instance_id;
    buffer[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *rq = (struct pldm_msg *)(buffer + PLDM_MCTP_BINDING_MSG_OFFSET);

    uint32_t max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
	uint8_t max_outstanding_transfer_req = PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ;
    uint16_t num_of_comp = fwup_state->updating_device.num_of_components;

	struct variable_field comp_img_set_ver_str;
    comp_img_set_ver_str.length = fwup_state->updating_device.comp_img_set_ver_str_len;
    comp_img_set_ver_str.ptr = (const uint8_t *)fwup_state->updating_device.comp_img_set_set_str;

   	uint16_t pkg_data_len = fwup_flash->package_data_size;
    uint8_t comp_image_set_ver_str_type = fwup_state->updating_device.comp_img_set_ver_str_type;
    uint8_t comp_image_set_ver_str_len = fwup_state->updating_device.comp_img_set_ver_str_len;

    size_t rq_payload_length = sizeof (struct pldm_request_update_req) + comp_img_set_ver_str.length;

    int status = encode_request_update_req(instance_id, max_transfer_size, num_of_comp, max_outstanding_transfer_req,
        pkg_data_len, comp_image_set_ver_str_type, comp_image_set_ver_str_len, &comp_img_set_ver_str, rq, rq_payload_length);
    if (status != 0) {
        return status;
    }

    instance_id += 1;
    return rq_payload_length + PLDM_MCTP_BINDING_MSG_OVERHEAD;
}




/**
 * Process a RequestUpdate response.
 * 
 * @param fwup_flash The flash addresses and devices to use for different PLDM FWUP regions.
 * @param fwup_state Variable context for a PLDM FWUP.
 * @param response - The response data to process.
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_request_update_response(struct pldm_fwup_flash_map *fwup_flash, 
    struct pldm_fwup_state *fwup_state, struct cmd_interface_msg *response)
{
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
    fwup_state->completion_code = completion_code;
    if (completion_code!= PLDM_SUCCESS) {
        return 0;
    }
    
    fwup_state->get_pkg_data_cmd = fd_will_send_pkg_data_cmd;
    fwup_flash->device_meta_data_size = fd_meta_data_len;

    response->length = 0;
    return status;
}


/**
* Process a GetPackageData request and generate a response.
*
* @param fwup_state - Variable state context for a PLDM FWUP.
* @param fwup_flash - The flash map for a PLDM FWUP.
* @param request The request data to process.  This will be updated to contain a response.
*
* @return 0 if the request was successfully processed and a request was generated or an error code.
*/
int pldm_fwup_process_get_package_data_request(struct pldm_fwup_state *fwup_state, 
    struct pldm_fwup_flash_map *fwup_flash, struct cmd_interface_msg *request)
{
    fwup_state->command = PLDM_GET_PACKAGE_DATA;

    struct pldm_msg *rq = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rq_payload_length = request->length - PLDM_MCTP_BINDING_MSG_OVERHEAD;

    uint32_t data_transfer_handle;
    uint8_t transfer_operation_flag;

    int status = decode_get_package_data_req(rq, rq_payload_length, &data_transfer_handle, &transfer_operation_flag);
    if (status != 0) {
        return status;
    }

    static uint8_t instance_id = 1;
    uint8_t completion_code = 0;
    uint8_t transfer_flag = 0;
    struct variable_field portion_of_pkg_data;
    uint32_t next_data_transfer_handle = 0;
    portion_of_pkg_data.ptr = (const uint8_t *)&completion_code;
    portion_of_pkg_data.length = sizeof (completion_code);

    if (fwup_state->previous_command != PLDM_REQUEST_UPDATE) {
        completion_code = PLDM_FWUP_COMMAND_NOT_EXPECTED;
        goto exit;
    }
    else if (fwup_flash->package_data_size <= 0) {
        completion_code = PLDM_FWUP_NO_PACKAGE_DATA;
        goto exit;
    }
    else if (fwup_state->multipart_transfer.transfer_flag == PLDM_MIDDLE && transfer_operation_flag == PLDM_GET_FIRSTPART) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_OPERATION_FLAG;
        goto exit;
    } 
    else if (data_transfer_handle != fwup_state->multipart_transfer.next_data_transfer_handle) {
        completion_code = PLDM_FWUP_INVALID_TRANSFER_HANDLE;
        goto exit;
    }

    if (transfer_operation_flag == PLDM_GET_FIRSTPART) {
        if (fwup_flash->package_data_size < PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE) {
            transfer_flag = PLDM_START_AND_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_START;
            next_data_transfer_handle = data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
        }
    }
    else {
        if (data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE >= fwup_flash->package_data_size) {
            transfer_flag = PLDM_END;
            next_data_transfer_handle = 0;
        }
        else {
            transfer_flag = PLDM_MIDDLE;
            next_data_transfer_handle = data_transfer_handle + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
        }
    }

    uint8_t pkg_data_buf[PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE] = {0};
    status = fwup_flash->package_data_flash->read(fwup_flash->package_data_flash, fwup_flash->package_data_addr + data_transfer_handle,
        pkg_data_buf, PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE);
    if (ROT_IS_ERROR(status)) {
        return status;
    }
    portion_of_pkg_data.ptr = (const uint8_t *)pkg_data_buf;
    portion_of_pkg_data.length = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

exit:;
    struct pldm_msg *rsp = (struct pldm_msg *)(request->data + PLDM_MCTP_BINDING_MSG_OFFSET);
    size_t rsp_payload_length = sizeof (struct pldm_multipart_transfer_resp) + portion_of_pkg_data.length;

    status = encode_get_package_data_resp(instance_id, rsp_payload_length, rsp, completion_code,
        next_data_transfer_handle, transfer_flag, &portion_of_pkg_data);

    printf("REQUEST/RESPONSE | instance: %d, data transfer handle %d, transfer op flag %d, next data transfer handle: %d, transfer flag: %d, CRC: %d.\n", 
        instance_id, data_transfer_handle, transfer_operation_flag, next_data_transfer_handle, 
        transfer_flag, crc32(portion_of_package_data.ptr, portion_of_package_data.length));
    
    fwup_state->completion_code = completion_code;
    fwup_state->multipart_transfer.next_data_transfer_handle = next_data_transfer_handle;
    fwup_state->multipart_transfer.transfer_flag = transfer_flag;
    request->length = rsp_payload_length;
    return status;
}

#endif