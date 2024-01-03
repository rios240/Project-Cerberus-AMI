#include <string.h>
#include <stdint.h>
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

    uint8_t resp_msg_buf[payload_length];
    resp_msg_buf[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *respMsg = (struct pldm_msg *)&resp_msg_buf[1];
    int status = encode_query_device_identifiers_resp(instance_id, respMsg, 
                                    completion_code, device_identifiers_len, 
                                    descriptor_count, descriptors);
    
    request->data = resp_msg_buf;
    request->length = payload_length;
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

    uint8_t resp_msg_buf[payload_length];
    resp_msg_buf[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;

    struct pldm_msg *respMsg = (struct pldm_msg *)&resp_msg_buf[1];
    int status = encode_get_firmware_parameters_resp(instance_id, respMsg, 
                                        &resp_data,
                                        &active_comp_image_set_ver_str,
                                        &pending_comp_image_set_ver_str,
                                        comp_parameter_table);
    request->data = resp_msg_buf;
    request->length = payload_length;
    return status;

}
