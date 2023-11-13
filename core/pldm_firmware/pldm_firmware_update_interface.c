#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "mctp/mctp_interface.h"
#include "pldm_firmware_update_interface.h"
#include "pldm_firmware_update_fsm.h"
#include "pldm_firmware_cmd_channel.h"
#include "base.h"
#include "utils.h"
#include "firmware_update.h"




int requestUpdate(struct pldm_msg *pldmMsg, struct cmd_interface_msg *message, 
                    MessageType msg_type, uint8_t pldm_type, uint8_t command) 
{
    if (msg_type != PLDM_REQUEST || pldm_type != PLDM_FWUP || command != PLDM_REQUEST_UPDATE) {
        return -1;
    }

    uint32_t max_transfer_size = 0;
    uint16_t num_of_comp = 0;
	uint8_t max_outstanding_transfer_req = 0;
	uint16_t pkg_data_len = 0;
	uint8_t comp_image_set_ver_str_type = 0;
    uint8_t comp_image_set_ver_str_len = 0;
    struct variable_field comp_img_set_ver_str = { 0 };

    int status = decode_request_update_req(pldmMsg, sizeof(struct pldm_request_update_req) + sizeof (struct variable_field), 
                                &max_transfer_size, &num_of_comp, &max_outstanding_transfer_req,
                                &pkg_data_len, &comp_image_set_ver_str_type, &comp_image_set_ver_str_len,
                                &comp_img_set_ver_str);

    if (status != PLDM_SUCCESS) {
        return status;
    }


    


}


int process_message(struct cmd_interface *intf, struct cmd_interface_msg *message) 
{
    int status;
    struct pldm_msg *pldmMsg = (struct pldm_msg *)&message->data[1];
    struct pldm_header_info pldmHdr = { 0 };

    status = unpack_pldm_header((const struct pldm_msg_hdr *) &pldmMsg->hdr, &pldmHdr);

    if (status != PLDM_SUCCESS) {
        return status;
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

    status = mctp_interface_init(&mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, &device_mgr);

    return status;

}

int pldm_firmware_update_run(struct mctp_interface *mctp, struct cmd_channel *channel, int ms_timeout) 
{
    int status = cmd_channel_receive_and_process(&channel, &mctp, ms_timeout);
    return status;
}
