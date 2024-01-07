#ifndef PLDM_FWUP_COMMANDS_H_
#define PLDM_FWUP_COMMANDS_H_


#include <stdint.h>
#include "cmd_interface/cmd_interface.h"


/**
 * Some PLDM commands are either recieved or requested by the Firmware Device.
 * The following API are either called by mctp_interface_process_packet() to process the PLDM payloads of recieved MCTP packets
 * or independently create PLDM payloads for mctp_interface_issue_request().
 * 
 * These API make use of openbmc's libpldm to construct or deconstruct PLDM messages.
*/

//Requested by the UA, FD will issue a response.
int query_device_identifiers(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the UA, FD will issue a response.
int get_firmware_parameters(struct cmd_interface *intf, struct cmd_interface_msg *request);


//Requested by the UA, FD will issue a response
int request_update(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the UA, FD will issue a response
int pass_component_table(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the UA, FD will issue a response
int update_component(struct cmd_interface *intf, struct cmd_interface_msg *request);

//Requested by the FD, UA will issue a response
int issue_request_firmware_data(uint8_t *request, size_t *payload_length);

int request_firmware_data(struct cmd_interface *intf, struct cmd_interface_msg *response);

//Requested by the FD, UA will issue a response
int issue_transfer_complete(uint8_t *request, size_t *payload_length);

int transfer_complete(struct cmd_interface *intf, struct cmd_interface_msg *response);

#endif /* PLDM_FWUP_COMMANDS_H_ */