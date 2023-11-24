#ifndef PLDM_FIRMWARE_UPDATE_FSM_H_
#define PLDM_FIRMWARE_UPDATE_FSM_H_



#include <stdint.h>
#include "base.h"
#include "firmware_update.h"
#include "cmd_interface/cmd_interface.h"

#define MAX_ROW 1

struct pldm_firmware_device_state_info {
    int (*pldm_command_controller)(struct pldm_firmware_device_state_info *state_info, 
        struct cmd_interface_msg *message);

    enum pldm_firmware_device_states state;
    bool update_mode;
    int response;
    enum pldm_firmware_device_states next_state;
};

struct pldm_firmware_device_fsm {
    enum pldm_firmware_device_states current_state;
    struct pldm_firmware_device_state_info stt[MAX_ROW];
};


#endif /* PLDM_FIRMWARE_UPDATE_FSM_H_ */