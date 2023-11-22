#ifndef PLDM_FIRMWARE_UPDATE_FSM_H_
#define PLDM_FIRMWARE_UPDATE_FSM_H_



#include <stdint.h>
#include "base.h"
#include "cmd_interface/cmd_interface.h"

#define MAX_ROW 1

enum pldm_firmware_device_states {
	PLDM_FD_STATE_IDLE = 0,
	PLDM_FD_STATE_LEARN_COMPONENTS = 1,
	PLDM_FD_STATE_READY_XFER = 2,
	PLDM_FD_STATE_DOWNLOAD = 3,
	PLDM_FD_STATE_VERIFY = 4,
	PLDM_FD_STATE_APPLY = 5,
	PLDM_FD_STATE_ACTIVATE = 6
};

struct pldm_firmware_device_state_info {
    int (*pldm_command_controller)(struct pldm_firmware_device_state_info *state_info, 
        struct cmd_interface_msg *message, int command);

    int state;
    int update_mode;
    int response;
    int next_state;
};

struct pldm_firmware_device_fsm {
    enum pldm_firmware_device_states current_state;
    struct pldm_firmware_device_state_info sst[MAX_ROW];
};


#endif /* PLDM_FIRMWARE_UPDATE_FSM_H_ */