#ifndef PLDM_FIRMWARE_UPDATE_CMD_CONTROLLERS_H_
#define PLDM_FIRMWARE_UPDATE_CMD_CONTROLLERS_H_

#include <stdint.h>
#include "pldm_firmware_update_fsm.h"

int idle_command_controller(struct pldm_firmware_device_state_info *state_info, 
        struct cmd_interface_msg *message);








#endif /* PLDM_FIRMWARE_UPDATE_CMD_CONTROLLERS_H_*/