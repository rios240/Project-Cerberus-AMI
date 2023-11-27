#ifndef PLDM_FIRMWARE_UPDATE_UPDATE_TRIGGERS_H_
#define PLDM_FIRMWARE_UPDATE_TRIGGERS_H_

#include "base.h"
#include "cmd_interface/cmd_interface.h"
#include "pldm_firmware_update_fsm.h"

int generate_error_packet(struct cmd_interface *intf, struct cmd_interface_msg *request,
		uint8_t error_code, uint32_t error_data, uint8_t cmd_set);

int requestUpdate(struct pldm_firmware_device_state_info *state_info, struct pldm_msg *pldmMsg, struct cmd_interface_msg *message);


#endif /* PLDM_FIRMWARE_UPDATE_TRIGGERS_H_ */