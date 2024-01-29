#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"
#include "pldm_fwup_commands.h"
#include "pldm_fwup_cmd_channel.h"
#include "cmd_interface/device_manager.h"
#include "pldm_fwup_interface.h"
#include "pldm_fwup_mctp.h"
#include "firmware_update.h"


struct pldm_fwup_interface *get_fwup_interface()
{
    static struct pldm_fwup_interface fwup;
    return &fwup;
}

int initialize_firmware_update(struct mctp_interface *mctp, struct cmd_channel *cmd_channel, 
                        struct cmd_interface *cmd_mctp, 
                        struct cmd_interface *cmd_spdm, struct cmd_interface *cmd_cerberus,
                        struct device_manager *device_mgr, struct pldm_fwup_interface *fwup)
{
    cmd_channel->send_packet = send_packet;
    cmd_channel->receive_packet = receive_packet;
    cmd_channel->id = 1;

    int status = device_manager_init(device_mgr, 2, 0, DEVICE_MANAGER_PA_ROT_MODE, DEVICE_MANAGER_SLAVE_BUS_ROLE, 
                1000, 1000, 1000, 1000, 1000, 1000, 5);

    if (status != 0) {
        return status;
    }

    device_mgr->entries->eid = 0xDA;
    device_mgr->entries->smbus_addr = 0xDA;

    status = mctp_interface_init(mctp, cmd_cerberus, cmd_mctp, cmd_spdm, device_mgr);
    mctp->cmd_cerberus->generate_error_packet = generate_error_packet;
    
    struct pldm_fwup_multipart_transfer multipart_transfer;
    multipart_transfer.transfer_status = MPT_START;
    struct pldm_fwup_error_testing error_testing = { 0 };

    fwup->current_fd_state = PLDM_FD_STATE_IDLE;
    fwup->error_testing = error_testing;
    fwup->multipart_transfer = multipart_transfer;
    fwup->package_data_size = 0;
    fwup->package_data = (uint8_t *)malloc(sizeof(uint8_t));

    return status;
}


void firmware_update_check_state(struct pldm_fwup_interface *fwup)
{
    switch(fwup->current_fd_state) {
        case PLDM_FD_STATE_IDLE:
            if (fwup->current_fd_command == PLDM_REQUEST_UPDATE && fwup->completion_code == PLDM_SUCCESS) {
                fwup->current_fd_state = PLDM_FD_STATE_LEARN_COMPONENTS;
                break;
            } else if (fwup->current_fd_command == PLDM_REQUEST_UPDATE && (fwup->completion_code == PLDM_FWUP_UNABLE_TO_INITIATE_UPDATE
                        || fwup->completion_code == PLDM_FWUP_RETRY_REQUEST_UPDATE)) {
                break;
            } else if (fwup->current_fd_command == PLDM_QUERY_DEVICE_IDENTIFIERS && fwup->completion_code == PLDM_SUCCESS) {
                break;
            } else if (fwup->current_fd_command == PLDM_GET_FIRMWARE_PARAMETERS && fwup->completion_code == PLDM_SUCCESS) {
                break;
            } else {
                break;
            }
        case PLDM_FD_STATE_LEARN_COMPONENTS:
            if (fwup->current_fd_command == PLDM_GET_PACKAGE_DATA && fwup->completion_code == PLDM_SUCCESS) {
                break;
            } else if (fwup->current_fd_command == PLDM_GET_DEVICE_METADATA && fwup->completion_code == PLDM_SUCCESS) {
                break;
            } else {
                break;
            }
    }
}


void clean_up_and_reset_firmware_update(struct mctp_interface *mctp, struct pldm_fwup_interface *fwup) {
    mctp_interface_reset_message_processing(mctp);

    fwup->multipart_transfer.last_data_transfer_handle = 0;
    fwup->multipart_transfer.transfer_status = MPT_START;

    fwup->current_fd_state = PLDM_FD_STATE_IDLE;
    fwup->completion_code = 0;
    fwup->current_fd_command = 0;
    
    fwup->error_testing.retry_request_update = 0;
    fwup->error_testing.unable_to_initiate_update = 0;

    fwup->package_data_size = 0;
    free(fwup->package_data);
}