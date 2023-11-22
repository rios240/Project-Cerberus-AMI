#ifndef PLDM_FIRMWARE_UPDATE_UPDATE_TRIGGERS_H_
#define PLDM_FIRMWARE_UPDATE_TRIGGERS_H_

#include "base.h"
#include "cmd_interface/cmd_interface.h"
#include "pldm_firmware_update_fsm.h"


int requestUpdate(struct pldm_msg *pldmMsg, struct cmd_interface_msg *message, 
                    int *completion_code, int command, int current_state, int update_mode);


#endif /* PLDM_FIRMWARE_UPDATE_TRIGGERS_H_ */