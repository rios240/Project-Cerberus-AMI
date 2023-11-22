#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "mctp/mctp_interface.h"
#include "pldm_firmware_update_interface.h"
#include "pldm_firmware_update_fsm.h"
#include "pldm_firmware_cmd_channel.h"
#include "pldm_firmware_update_triggers.h"


int process_message(struct cmd_interface *intf, struct cmd_interface_msg *message) 
{
    int status;
    struct pldm_msg *pldmMsg = (struct pldm_msg *)&message->data[1];
    struct pldm_header_info pldmHdr = { 0 };

    status = unpack_pldm_header((const struct pldm_msg_hdr *) &pldmMsg->hdr, &pldmHdr);

    if (status != PLDM_SUCCESS) {
        return status;
    }

    int row;
    for (row = 0; row < MAX_ROW; row++) {

    }

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

    device_mgr->entries->eid = 0xCC;
    device_mgr->entries->smbus_addr = 0xAA;

    status = mctp_interface_init(mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, device_mgr);

    return status;

}

int pldm_firmware_update_run_workflow(struct mctp_interface *mctp, struct cmd_channel *channel, int ms_timeout) 
{
    int status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
    return status;
}
