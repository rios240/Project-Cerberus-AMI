#ifndef PLDM_FWUP_COMMANDS_H_
#define PLDM_FWUP_COMMANDS_H_

#include <stdint.h>
#include "cmd_interface/cmd_interface.h"

struct pldm_firmware_data_transfer {
    uint32_t offset;
    uint32_t length;
};

//Requested by the UA, FD will issue a response.
int process_and_respond_query_device_identifiers(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the UA, FD will issue a response.
int process_and_respond_get_firmware_parameters(struct cmd_interface *intf, struct cmd_interface_msg *request);


//Requested by the UA, FD will issue a response
int process_and_response_request_update(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by FD, UA will issue a response
int request_get_package_data(uint8_t *request, size_t *payload_length);

int process_get_package_data(struct cmd_interface *intf, struct cmd_interface_msg *response);

//Requested by the UA, FA will issue a response
int process_and_respond_get_device_meta_data(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the UA, FD will issue a response
int process_and_respond_ua_pass_component_table(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the UA, FD will issue a response
int process_and_respond_update_component(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the FD, UA will issue a response
//int request_firmware_data(uint8_t *request, size_t *payload_length);

//int process_request_firmware_data(struct cmd_interface *intf, struct cmd_interface_msg *response);

//Requested by the FD, UA will issue a response
int request_transfer_complete(uint8_t *request, size_t *payload_length);

int process_transfer_complete(struct cmd_interface *intf, struct cmd_interface_msg *response);

//Requested by the FD, UA will issue a response
int request_verify_complete(uint8_t *request, size_t *payload_length);

int process_verify_complete(struct cmd_interface *intf, struct cmd_interface_msg *response);

//Requested by the FD, UA will issue a response
int request_apply_complete(uint8_t *request, size_t *payload_length);

int process_apply_complete(struct cmd_interface *intf, struct cmd_interface_msg *response);

//Requested by the UA, FD will issue a response
int process_and_respond_activate_firmware(struct cmd_interface *intf, struct cmd_interface_msg *request);


int request_query_device_identifiers(uint8_t *request, size_t *payload_length);

int process_query_device_identifiers(struct cmd_interface *intf, struct cmd_interface_msg *response);

int request_get_firmware_parameters(uint8_t *request, size_t *payload_length);

int process_get_firmware_parameters(struct cmd_interface *intf, struct cmd_interface_msg *response);

int request_update(uint8_t *request, size_t *payload_length);

int process_request_update(struct cmd_interface *intf, struct cmd_interface_msg *response);

int process_and_respond_get_package_data(struct cmd_interface *intf, struct cmd_interface_msg *request);

int request_get_device_meta_data(uint8_t *request, size_t *payload_length);

int process_get_device_meta_data(struct cmd_interface *intf, struct cmd_interface_msg *response);

int pass_component_table(uint8_t *request, size_t *mctp_payload_length);

int process_pass_component_table_resp(struct cmd_interface *intf, struct cmd_interface_msg *response);

int update_component(uint8_t *request, size_t *mctp_payload_length);

int process_update_component_resp(struct cmd_interface *intf, struct cmd_interface_msg *response);

//int process_and_respond_request_firmware_data(struct cmd_interface *intf, struct cmd_interface_msg *request);


#endif /* PLDM_FWUP_COMMANDS_H_ */