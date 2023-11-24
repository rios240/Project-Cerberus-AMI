#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "mctp/mctp_interface.h"
#include "pldm_firmware_update_interface.h"
#include "pldm_firmware_update_fsm.h"
#include "pldm_firmware_cmd_channel.h"
#include "pldm_firmware_update_triggers.h"
#include "pldm_firmware_update_cmd_controllers.h"
#include "platform_io.h"


struct pldm_firmware_device_fsm pldm_fsm = {
    .current_state = PLDM_FD_STATE_IDLE,
    .stt = {
        [0] = {
            .pldm_command_controller = idle_command_controller,
            .state = PLDM_FD_STATE_IDLE,
            .update_mode = false,
        }
    }
};



int process_message(struct cmd_interface *intf, struct cmd_interface_msg *message) 
{
    int row;
    int status;
    for (row = 0; row < MAX_ROW; row++) {
        if (pldm_fsm.stt[row].state == pldm_fsm.current_state) {
            platform_printf("Current State: %d", pldm_fsm.stt[row].state);
            status = pldm_fsm.stt[row].pldm_command_controller(&pldm_fsm.stt[row], message);
            platform_printf("Transitioning to state: %d", pldm_fsm.stt[row].next_state);
            if (status != PLDM_SUCCESS) {
                break;
            } else {
                pldm_fsm.current_state = pldm_fsm.stt[row].next_state;
                break;
            }
        }

    }
    return status;

}



int pldm_firmware_update_init(struct mctp_interface *mctp, struct cmd_channel *channel, struct device_manager *device_mgr,
                                uint8_t device_eid, uint8_t device_smbus_addr) 
{
    int status;
    struct cmd_interface cmd_cerberus;
    struct cmd_interface cmd_mctp;
    struct cmd_interface cmd_spdm;

    channel->send_packet = send_packet;
    channel->receive_packet = receive_packet;

    cmd_mctp.process_request = process_message;

    status = device_manager_init(device_mgr, 2, 0, DEVICE_MANAGER_PA_ROT_MODE, DEVICE_MANAGER_SLAVE_BUS_ROLE, 
                1000, 1000, 1000, 1000, 1000, 1000, 5);

    if (status != 0) {
        return status;
    }

    device_mgr->entries->eid = device_eid;
    device_mgr->entries->smbus_addr = device_smbus_addr;

    status = mctp_interface_init(mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, device_mgr);

    return status;

}

int pldm_firmware_update_run(struct mctp_interface *mctp, struct cmd_channel *channel, int ms_timeout) 
{
    int status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
    return status;
}
