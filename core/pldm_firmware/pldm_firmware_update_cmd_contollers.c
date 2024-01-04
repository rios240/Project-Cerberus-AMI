#include "pldm_firmware_update_cmd_controllers.h"
#include "pldm_firmware_update_triggers.h"
#include "firmware_update.h"
#include "platform_io.h"



int idle_command_controller(struct pldm_firmware_device_state_info *state_info, 
        struct cmd_interface_msg *message)
{
    /*
    int status;
    struct pldm_msg *pldmMsg = (struct pldm_msg *)&message->data[1];
    struct pldm_header_info pldmHdr = { 0 };

    status = unpack_pldm_header((const struct pldm_msg_hdr *) &pldmMsg->hdr, &pldmHdr);

    if (status != PLDM_SUCCESS) {
        return status;
    }

    switch (pldmHdr.command) {
        case PLDM_REQUEST_UPDATE:
            platform_printf("Recieved RequestUpdate command.\n");
            status = requestUpdate(state_info, pldmMsg, message);
            if (status != PLDM_SUCCESS) {
                return status;
            }
            platform_printf("Got response: %d\n", state_info->response);
            if (state_info->response == PLDM_SUCCESS) {
                state_info->next_state = PLDM_FD_STATE_LEARN_COMPONENTS;
            }
            else if (state_info->response == PLDM_FWUP_RETRY_REQUEST_FW_DATA || 
                state_info->response == PLDM_FWUP_UNABLE_TO_INITIATE_UPDATE) {
                    state_info->next_state = PLDM_FD_STATE_IDLE;
            }
            else {
                state_info->next_state = PLDM_FD_STATE_IDLE;
            }
            break;
    }*/

    return 0;

}