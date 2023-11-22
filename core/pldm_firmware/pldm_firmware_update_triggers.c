#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pldm_firmware_update_triggers.h"
#include "utils.h"
#include "firmware_update.h"

#define RETRY false
#define UNABLE false
#define METADATA false


int requestUpdate(struct pldm_msg *pldmMsg, struct cmd_interface_msg *message, 
                    int *completion_code, int command, int current_state, int update_mode) 
{

    if (command != PLDM_REQUEST_UPDATE) {
        return command;
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

    uint8_t completion_code = 0;
    if (current_state == READY_XFER && update_mode) {
        completion_code = PLDM_FWUP_ALREADY_IN_UPDATE_MODE;
    }
    else if (RETRY) {
        completion_code = PLDM_FWUP_RETRY_REQUEST_FW_DATA;
    }
    else if (UNABLE) {
        completion_code = PLDM_FWUP_UNABLE_TO_INITIATE_UPDATE;
    }
   
	uint16_t fd_meta_data_len = 0x00;
    if (METADATA) {
        fd_meta_data_len = 0x5555;
    }
    uint8_t fd_will_send_pkg_data = 0x00;
    if (pkg_data_len != 0) {
        fd_will_send_pkg_data = 0x01;
    }
    uint8_t instance_id = 1; 

    uint8_t respBuf[sizeof (struct pldm_msg_hdr) + sizeof (struct pldm_request_update_resp)+1];
    respBuf[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)&respBuf[1];

    status = encode_request_update_resp(instance_id, fd_meta_data_len, 
                                fd_will_send_pkg_data, completion_code, respMsg, sizeof(struct pldm_request_update_resp));
    
    memcpy(message->data, respBuf, sizeof (respBuf));
    message->length = sizeof (respBuf);

    return status;
    

}