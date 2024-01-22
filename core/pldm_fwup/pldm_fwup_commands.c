#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "utils.h"
#include "pldm_fwup_commands.h"
#include "firmware_update.h"


//Helper function that returns ComponentParameterTable
void get_comp_parameter_table(struct variable_field *comp_parameter_table)
{

    const char *active_comp_ver_str_arr = "BIOS_v1.0";
    const char *pending_comp_ver_str_arr = "BIOS_v2.0";

    bitfield16_t comp_activation_methods;
    comp_activation_methods.value = 0xDEAD;

    bitfield32_t capabilities_during_update;
    capabilities_during_update.value = 0xDEADDAED;
    capabilities_during_update.bits.bit0 = 0;
    capabilities_during_update.bits.bit1 = 0;
    capabilities_during_update.bits.bit2 = 0;
    capabilities_during_update.bits.bit3 = 0;
    capabilities_during_update.bits.bit4 = 0;

    struct pldm_component_parameter_entry bios_fw_comp;
    bios_fw_comp.comp_classification = PLDM_COMP_FIRMWARE_OR_BIOS;
    bios_fw_comp.comp_identifier = 0xDEAD;
    bios_fw_comp.comp_classification_index = 0xDE;
    bios_fw_comp.active_comp_comparison_stamp = 0xDEADBEEF;
    bios_fw_comp.active_comp_ver_str_type = PLDM_STR_TYPE_ASCII;
    bios_fw_comp.active_comp_ver_str_len = strlen(active_comp_ver_str_arr);
    bios_fw_comp.active_comp_release_date[0] = 2;
    bios_fw_comp.active_comp_release_date[1] = 0;
    bios_fw_comp.active_comp_release_date[2] = 2;
    bios_fw_comp.active_comp_release_date[3] = 3;
    bios_fw_comp.active_comp_release_date[4] = 0;
    bios_fw_comp.active_comp_release_date[5] = 1;
    bios_fw_comp.active_comp_release_date[6] = 0;
    bios_fw_comp.active_comp_release_date[7] = 4;
    bios_fw_comp.pending_comp_comparison_stamp = 0xDEADF00D;
    bios_fw_comp.pending_comp_ver_str_type = PLDM_STR_TYPE_ASCII;
    bios_fw_comp.pending_comp_ver_str_len = strlen(pending_comp_ver_str_arr);
    bios_fw_comp.pending_comp_release_date[0] = 2;
    bios_fw_comp.pending_comp_release_date[1] = 0;
    bios_fw_comp.pending_comp_release_date[2] = 2;
    bios_fw_comp.pending_comp_release_date[3] = 4;
    bios_fw_comp.pending_comp_release_date[4] = 0;
    bios_fw_comp.pending_comp_release_date[5] = 1;
    bios_fw_comp.pending_comp_release_date[6] = 0;
    bios_fw_comp.pending_comp_release_date[7] = 4;
    bios_fw_comp.comp_activation_methods = comp_activation_methods;
    bios_fw_comp.capabilities_during_update = capabilities_during_update;

    uint8_t comp_parameter_table_buf[sizeof (struct pldm_component_parameter_entry) + 
                            bios_fw_comp.active_comp_ver_str_len +
                            bios_fw_comp.pending_comp_ver_str_len];
    memcpy(comp_parameter_table_buf, &bios_fw_comp, sizeof (struct pldm_component_parameter_entry));
    memcpy(comp_parameter_table_buf + sizeof (struct pldm_component_parameter_entry),
            active_comp_ver_str_arr, bios_fw_comp.active_comp_ver_str_len);
    memcpy(comp_parameter_table_buf + sizeof (struct pldm_component_parameter_entry)
            + bios_fw_comp.active_comp_ver_str_len, pending_comp_ver_str_arr,
            bios_fw_comp.pending_comp_ver_str_len);

    comp_parameter_table->ptr = comp_parameter_table_buf;
    comp_parameter_table->length = sizeof (comp_parameter_table_buf);

}


int query_device_identifiers(struct cmd_interface *intf, struct cmd_interface_msg *request)
{

    uint32_t iana_number = 135;
    struct pldm_descriptor_tlv iana;
    iana.descriptor_type = PLDM_FWUP_IANA_ENTERPRISE_ID;
    iana.descriptor_length = PLDM_FWUP_IANA_ENTERPRISE_ID_LENGTH;
    memcpy(iana.descriptor_data, &iana_number, iana.descriptor_length);
    size_t iana_size = sizeof (iana.descriptor_type) + sizeof (iana.descriptor_length) + iana.descriptor_length;

    const char *uuid_string = "8d759454a78111eea5060242ac120002";
    struct pldm_descriptor_tlv uuid;
    uuid.descriptor_type = PLDM_FWUP_UUID;
    uuid.descriptor_length = PLDM_FWUP_UUID_LENGTH;
    memcpy(uuid.descriptor_data, uuid_string, uuid.descriptor_length);
    size_t uuid_size = sizeof (uuid.descriptor_type) + sizeof (uuid.descriptor_length) + uuid.descriptor_length;

    uint8_t instance_id = 1;
    uint8_t completion_code = PLDM_SUCCESS;
    uint32_t device_identifiers_len = iana_size + uuid_size;
    uint8_t descriptor_count = 2;

    uint8_t descriptors_buf[device_identifiers_len];
    memcpy(descriptors_buf, &iana, iana_size);
    memcpy(descriptors_buf + iana_size, &uuid, uuid_size);

    struct variable_field descriptors;
    descriptors.length = sizeof (descriptors_buf);
    descriptors.ptr = descriptors_buf;
    
    size_t payload_length = sizeof (struct pldm_msg_hdr) 
                + sizeof (struct pldm_query_device_identifiers_resp)
                + descriptors.length 
                + 1;

    request->length = payload_length;
    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)(&request->data[1]);
    int status = encode_query_device_identifiers_resp(instance_id, respMsg, 
                                    completion_code, device_identifiers_len, 
                                    descriptor_count, descriptors);
    return status;
    

}


int get_firmware_parameters(struct cmd_interface *intf, struct cmd_interface_msg *request)
{

    const char *active_comp_image_set_ver_str_arr = "cerberus_v1.0";
    const char *pending_comp_image_set_ver_str_arr = "cerberus_v2.0";

    bitfield32_t capabilities_during_update;
    capabilities_during_update.value = 0xDEADBEEF;
    capabilities_during_update.bits.bit0 = 0;
    capabilities_during_update.bits.bit1 = 0;
    capabilities_during_update.bits.bit2 = 0;
    capabilities_during_update.bits.bit3 = 0;
    capabilities_during_update.bits.bit4 = 0;

    struct pldm_get_firmware_parameters_resp resp_data;
    resp_data.completion_code = PLDM_SUCCESS;
    resp_data.capabilities_during_update = capabilities_during_update;
    resp_data.comp_count = 1;
    resp_data.active_comp_image_set_ver_str_type = PLDM_STR_TYPE_ASCII;
    resp_data.pending_comp_image_set_ver_str_type = PLDM_STR_TYPE_ASCII;
    resp_data.active_comp_image_set_ver_str_len = strlen(active_comp_image_set_ver_str_arr);
    resp_data.pending_comp_image_set_ver_str_len = strlen(pending_comp_image_set_ver_str_arr);

    struct variable_field active_comp_image_set_ver_str;
    active_comp_image_set_ver_str.ptr = (const uint8_t *)active_comp_image_set_ver_str_arr;
    active_comp_image_set_ver_str.length =  resp_data.active_comp_image_set_ver_str_len;

    struct variable_field pending_comp_image_set_ver_str;
    pending_comp_image_set_ver_str.ptr = (const uint8_t *)pending_comp_image_set_ver_str_arr;
    pending_comp_image_set_ver_str.length = resp_data.pending_comp_image_set_ver_str_len;

    struct variable_field *comp_parameter_table = { 0 };
    get_comp_parameter_table(comp_parameter_table);

    uint8_t instance_id = 1;
    size_t payload_length = sizeof (struct pldm_msg_hdr) 
                    + sizeof (struct pldm_get_firmware_parameters_resp)
                    + active_comp_image_set_ver_str.length
                    + pending_comp_image_set_ver_str.length
                    + comp_parameter_table->length 
                    + 1;

    request->length = payload_length;
    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)(&request->data[1]);
    int status = encode_get_firmware_parameters_resp(instance_id, respMsg, 
                                        &resp_data,
                                        &active_comp_image_set_ver_str,
                                        &pending_comp_image_set_ver_str,
                                        comp_parameter_table);
    return status;

}

int request_update(struct cmd_interface *intf, struct cmd_interface_msg *request)
{
    struct pldm_msg *reqMsg = (struct pldm_msg *)(&request->data[1]);
    struct pldm_request_update_req req_data;
    struct variable_field comp_img_set_ver_str;

    int status = decode_request_update_req(reqMsg, &req_data, &comp_img_set_ver_str);

    struct pldm_request_update_resp resp_data;
    resp_data.fd_will_send_pkg_data = 0x00;
    resp_data.fd_meta_data_len = 0x0000;
    resp_data.completion_code = PLDM_SUCCESS;

    uint8_t instance_id = 1;

    size_t payload_length = sizeof (struct pldm_msg_hdr)
                        + sizeof (struct pldm_request_update_resp)
                        + 1;
    request->length = payload_length;
    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)(&request->data[1]);
    status = encode_request_update_resp(instance_id, respMsg, &resp_data);

    return status;
}

int pass_component_table(struct cmd_interface *intf, struct cmd_interface_msg *request)
{
    struct pldm_msg *reqMsg = (struct pldm_msg *)(&request->data[1]);
    struct pldm_pass_component_table_req req_data;
    struct variable_field comp_ver_str;

    int status = decode_pass_component_table_req(reqMsg, &req_data, &comp_ver_str);

    struct pldm_pass_component_table_resp resp_data;
    resp_data.comp_resp = PLDM_CR_COMP_CAN_BE_UPDATED;
    resp_data.comp_resp_code = PLDM_CRC_COMP_CAN_BE_UPDATED;
    resp_data.completion_code = PLDM_SUCCESS;

    uint8_t instance_id = 1;

    size_t payload_length = sizeof (struct pldm_msg_hdr)
                        + sizeof (struct pldm_pass_component_table_resp)
                        + 1;
    request->length = payload_length;
    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)(&request->data[1]);
    status = encode_pass_component_table_resp(instance_id, respMsg, &resp_data);

    return status;
}

int update_component(struct cmd_interface *intf, struct cmd_interface_msg *request)
{
    struct pldm_msg *reqMsg = (struct pldm_msg *)(&request->data[1]);
    struct pldm_update_component_req req_data;
    struct variable_field comp_ver_str;

    int status = decode_update_component_req(reqMsg, &req_data, &comp_ver_str);

    bitfield32_t update_option_flags_enabled;
    update_option_flags_enabled.value = 0xDEADBEEF;


    struct pldm_update_component_resp resp_data;
    resp_data.comp_compatibility_resp = PLDM_CCR_COMP_CAN_BE_UPDATED;
    resp_data.comp_compatibility_resp_code = PLDM_CCRC_NO_RESPONSE_CODE;
    resp_data.update_option_flags_enabled = update_option_flags_enabled;
    resp_data.time_before_req_fw_data = 0x64;
    resp_data.completion_code = PLDM_SUCCESS;

    size_t payload_length = sizeof (struct pldm_msg_hdr)
                        + sizeof (struct pldm_update_component_resp)
                        + 1;
    
    uint8_t instance_id = 1;

    request->length = payload_length;
    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)(&request->data[1]);
    status = encode_update_component_resp(instance_id, respMsg, &resp_data);

    return status;
}

int issue_request_firmware_data(uint8_t *request, size_t *payload_length)
{
    struct pldm_request_firmware_data_req req_data;
    req_data.offset = 0;
    req_data.length = PLDM_FWUP_BASELINE_TRANSFER_SIZE;

    uint8_t instance_id = 1;

    *payload_length = sizeof (struct pldm_msg_hdr)
                + sizeof (struct pldm_request_firmware_data_req)
                + 1;
    
    request[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *reqMsg = (struct pldm_msg *)(request + 1);
    int status = encode_request_firmware_data_req(instance_id, reqMsg, &req_data);

    return status;
    
}

int request_firmware_data(struct cmd_interface *intf, struct cmd_interface_msg *response)
{
    struct pldm_msg *respMsg = (struct pldm_msg *)(&response->data[1]);
    uint8_t completion_code = 0;
    struct variable_field comp_image_portion;
    comp_image_portion.length = PLDM_FWUP_BASELINE_TRANSFER_SIZE;

    int status = decode_request_firmware_data_resp(respMsg, &completion_code, &comp_image_portion);

    response->length = 0;

    return status;
}

int issue_transfer_complete(uint8_t *request, size_t *payload_length)
{
    uint8_t transfer_result = PLDM_FWUP_TRANSFER_SUCCESS;
    uint8_t instance_id = 1;

    *payload_length = sizeof (struct pldm_msg) + 1;

    request[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *reqMsg = (struct pldm_msg *)(request + 1);
    int status = encode_transfer_complete_req(instance_id, reqMsg, transfer_result);

    return status;

}

int transfer_complete(struct cmd_interface *intf, struct cmd_interface_msg *response)
{
    struct pldm_msg *respMsg = (struct pldm_msg *)(&response->data[1]);
    uint8_t completion_code = 0;

    int status = decode_transfer_complete_resp(respMsg, &completion_code);

    response->length = 0;

    return status;
}

int issue_verify_complete(uint8_t *request, size_t *payload_length)
{
    uint8_t verify_result = PLDM_FWUP_VERIFY_SUCCESS;
    uint8_t instance_id = 1;

    *payload_length = sizeof (struct pldm_msg) + 1;

    request[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *reqMsg = (struct pldm_msg *)(request + 1);
    int status = encode_verify_complete_req(instance_id, reqMsg, verify_result);
    
    return status;
}

int verify_complete(struct cmd_interface *intf, struct cmd_interface_msg *response)
{
    struct pldm_msg *respMsg = (struct pldm_msg *)(&response->data[1]);
    uint8_t completion_code = 0;

    int status = decode_verify_complete_resp(respMsg, &completion_code);

    response->length = 0;

    return status;

}

int issue_apply_complete(uint8_t *request, size_t *payload_length)
{
    bitfield16_t comp_activation_methods_modification;
    comp_activation_methods_modification.value = 0;

    struct pldm_apply_complete_req req_data;
    req_data.apply_result = PLDM_FWUP_APPLY_SUCCESS;
    req_data.comp_activation_methods_modification = comp_activation_methods_modification;
    
    uint8_t instance_id = 1;

    *payload_length = sizeof (struct pldm_msg_hdr) 
                + sizeof (struct pldm_apply_complete_req)
                + 1;
    
    request[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *reqMsg = (struct pldm_msg *)(request + 1);
    int status = encode_apply_complete_req(instance_id, reqMsg, &req_data);

    return status;
}

int apply_complete(struct cmd_interface *intf, struct cmd_interface_msg *response)
{
    struct pldm_msg *respMsg = (struct pldm_msg *)(&response->data[1]);
    uint8_t completion_code = 0;

    int status = decode_apply_complete_resp(respMsg, &completion_code);

    response->length = 0;

    return status;   
}

int activate_firmware(struct cmd_interface *intf, struct cmd_interface_msg *request)
{
    struct pldm_msg *reqMsg = (struct pldm_msg *)(&request->data[1]);
    struct pldm_activate_firmware_req req_data;

    int status = decode_activate_firmware_req(reqMsg, &req_data);

    struct pldm_activate_firmware_resp resp_data;
    resp_data.completion_code = PLDM_SUCCESS;
    resp_data.estimated_time_activation = 0x64;

    size_t payload_length = sizeof (struct pldm_msg_hdr)
                        + sizeof (struct pldm_activate_firmware_resp)
                        + 1;

    uint8_t instance_id = 1;

    request->length = payload_length;
    request->data[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)(&request->data[1]);
    status = encode_activate_firmware_resp(instance_id, respMsg, &resp_data);

    return status;

}