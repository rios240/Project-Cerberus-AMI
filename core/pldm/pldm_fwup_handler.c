#include <string.h>
#include "pldm_fwup_handler.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface_pldm.h"
#include "pldm_fwup_protocol_commands.h"
#include "pldm_fwup_protocol.h"
#include "common/unused.h"
#include "cmd_interface/cmd_channel.h"
#include "mctp/mctp_interface.h"
#include "platform_api.h" 
#include "platform_io.h"
#include "status/rot_status.h"

#include "libpldm/firmware_update.h"

const char *print_state(uint8_t state) {
    switch (state) {
        case PLDM_FD_STATE_IDLE:
            return "Idle";
        case PLDM_FD_STATE_LEARN_COMPONENTS:
            return "Learn Components";
        case PLDM_FD_STATE_READY_XFER:
            return "Ready Xfer";
        case PLDM_FD_STATE_DOWNLOAD:
            return "Download";
        case PLDM_FD_STATE_VERIFY:
            return "Verify";
        case PLDM_FD_STATE_APPLY:
            return "Apply";
        case PLDM_FD_STATE_ACTIVATE:
            return "Activate";
        default:
            return "";
    }
}

/**
* Generate PLDM FWUP request for use in the MCTP interface
* 
* @param intf The command interface that will generate the request.
* @param command The PLDM FWUP command
* @param buffer The buffer to store the PLDM message
* @param buf_len The length of the buffer
* 
* @return size of the request or pldm_completion_codes
*/
int pldm_fwup_handler_generate_request(struct cmd_interface *intf, int command, uint8_t *buffer, size_t buf_len) 
{
    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)intf;

    int status; 
    switch(command) {
        // Firmware Device
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_generate_get_package_data_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.get_cmd_state, buffer, buf_len);
            break;
        case PLDM_REQUEST_FIRMWARE_DATA:
            status = pldm_fwup_generate_request_firmware_data_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info, buffer, buf_len);
            break;
        case PLDM_TRANSFER_COMPLETE:
            status = pldm_fwup_generate_transfer_complete_request(&interface->fwup_mgr->fd_mgr.state, buffer, buf_len);
            break;
        case PLDM_VERIFY_COMPLETE:
            status = pldm_fwup_generate_verify_complete_request(&interface->fwup_mgr->fd_mgr.state, buffer, buf_len);
            break;
        case PLDM_APPLY_COMPLETE:
            status = pldm_fwup_generate_apply_complete_request(&interface->fwup_mgr->fd_mgr.state, buffer, buf_len);
            break;
        case PLDM_GET_META_DATA:
            status = pldm_fwup_generate_get_meta_data_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.get_cmd_state, buffer, buf_len);
            break;
        // Update Agent
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = pldm_fwup_generate_query_device_identifiers_request(&interface->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = pldm_fwup_generate_get_firmware_parameters_request(&interface->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_REQUEST_UPDATE:
            status = pldm_fwup_generate_request_update_request(&interface->fwup_mgr->ua_mgr, buffer, buf_len);
            break;
        case PLDM_GET_DEVICE_METADATA:
            status = pldm_fwup_generate_get_device_meta_data_request(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.get_cmd_state, 
                buffer, buf_len);
            break;
        case PLDM_PASS_COMPONENT_TABLE:
            status = pldm_fwup_generate_pass_component_table_request(&interface->fwup_mgr->ua_mgr, buffer, buf_len);
            break;
        case PLDM_UPDATE_COMPONENT:
            status = pldm_fwup_generate_update_component_request(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.current_comp_num, 
                interface->fwup_mgr->ua_mgr.comp_img_entries, &interface->fwup_mgr->ua_mgr.rec_fw_parameters, buffer, buf_len);
            break;
        case PLDM_ACTIVATE_FIRMWARE:
            status = pldm_fwup_generate_activate_firmware_request(&interface->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_GET_STATUS:
            status = pldm_fwup_generate_get_status_request(&interface->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_CANCEL_UPDATE_COMPONENT:
            status = pldm_fwup_generate_cancel_update_component_request(&interface->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_CANCEL_UPDATE:
            status = pldm_fwup_generate_cancel_update_request(&interface->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        default:
            status = PLDM_FWUP_HANDLER_UNKNOWN_REQUEST;    
    }
    return status;
}
 
/**
 * Receive all the packets for a MCTP message, process it, and send back a response.
 * 
 * @param channel The command channel for sending and receiving packets.
 * @param mctp The MCTP protocol handler to use for packet processing.
 * @param timeout_ms The time to wait in miliseconds for receiving a single packet.
 * 
 * @return 0 if all the packets were received and the message processed otherwise an error code.
*/
int pldm_fwup_handler_receive_and_respond_full_mctp_message(struct cmd_channel *channel, struct mctp_interface *mctp, int timeout_ms)
{
    int status;
    do {
        status = cmd_channel_receive_and_process(channel, mctp, timeout_ms);
        if (status != 0) {
            return status;
        }
    } while (mctp->req_buffer.length != 0);
    
    mctp_interface_reset_message_processing(mctp);
    return status;
}

/**
 * Send a full MCTP message, receive a response, and process it.  
 * 
 * @param handler The firmware update handler.
 * @param command The PLDM firmware update command
 * @param fd_eid The endpoint ID of the device to send the message to.
 * @param fd_addr The SMBus address of the firmware device to send the message to.
 * 
 * @return 0 if the MCTP message was sent and a response was processed otherwise an error code.
 * 
*/
int pldm_fwup_handler_send_and_receive_full_mctp_message(struct pldm_fwup_handler *handler, int command, uint8_t fd_eid, uint8_t fd_addr)
{
    int status;
    int req_length = pldm_fwup_handler_generate_request(handler->mctp->cmd_pldm, command, handler->req_buffer, sizeof (handler->req_buffer));
    if (ROT_IS_ERROR(req_length) && req_length == CMD_HANDLER_PLDM_TRANSPORT_ERROR) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    } 
    else if (ROT_IS_ERROR(req_length) && req_length == PLDM_FWUP_HANDLER_UNKNOWN_REQUEST) {
        return PLDM_FWUP_HANDLER_UNKNOWN_REQUEST;
    }
    
    status = mctp_interface_issue_request(handler->mctp, handler->channel, fd_addr, fd_eid, handler->req_buffer, 
        req_length, handler->req_buffer, sizeof (handler->req_buffer), 0); // Not performing parallel operations so set time out to zero.
    if (status != 0) {
        return status;
    }

    status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
    return status;
}

/**
 * Check the status of an MCTP operation.
 * 
 * @param transport_status The transport layer status of the operation.
 * @param protocol_status The protocol layer status of the operation.
 * 
 * @return 0 if the MCTP operation was successful otherwise an error code.
*/
int pldm_fwup_handler_check_operation_status(int transport_status, int protocol_status) {
    if (transport_status != 0) {
        return transport_status;
    }
    else if (protocol_status != PLDM_SUCCESS) {
        return CMD_HANDLER_PLDM_PROTOCOL_ERROR;
    } else {
        return 0;
    }
}

/**
 * Internal reference to the function that will execute a firmware update when Cerberus is operating as the Update Agent
 * 
 * This function will update the Firmware Device with the component images contained in the FWUP UA manager.
 * 
 * @param handler The firmware update handler.
 * @param inventory_cmds A flag indicating that inventory commands should be issued to the device being updated. 
 * @param fd_eid The endpoint ID of the firmware device to be updated. 
 * @param fd_addr The SMBus address of the firmware device to be updated.
 * 
 * @return 0 if the update was successful otherwise an error code.
 */ 
int pldm_fwup_handler_run_update_ua(struct pldm_fwup_handler *handler, bool inventory_cmds, uint8_t fd_eid, uint8_t fd_addr)
{
    if (handler->mode != PLDM_FWUP_HANDLER_UA_MODE) {
        return PLDM_FWUP_HANDLER_INCORRECT_MODE;
    }

    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)handler->mctp->cmd_pldm;
    struct pldm_fwup_ua_manager *ua_mgr = &interface->fwup_mgr->ua_mgr;

    if (ua_mgr->num_components == 0 || ua_mgr->comp_img_entries == NULL ||
        ua_mgr->fup_comp_img_set_ver == NULL || ua_mgr->flash_mgr->device_meta_data_region.length == 0 ||
        ua_mgr->flash_mgr->package_data_region.length == 0 || ua_mgr->flash_mgr->comp_regions == NULL ||
        ua_mgr->flash_mgr->flash == NULL) {
        return PLDM_FWUP_HANDLER_INVALID_UA_MANAGER_STATE;
    }

    int status;

    /* First we check if the UA needs to send inventory commands. If so we proceed with sending
     * the QueryDeviceIdentifier command and the GetFirmwareParameter command so that Cerberus
     * has the most recent configuration from the FD it is trying to update. */
    if (inventory_cmds) {
        platform_printf("Cerberus UA issues: QueryDeviceIdentifiers.\n");
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_QUERY_DEVICE_IDENTIFIERS, fd_eid, fd_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }

        platform_printf("Cerberus UA issues: GetFirmwareParameters.\n");
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_GET_FIRMWARE_PARAMETERS, fd_eid, fd_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
    }

    platform_printf("Cerberus UA issues: RequestUpdate.\n");
    /* We now proceed in sending the RequestUpdate command starting the firmware update process. */
    status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_REQUEST_UPDATE, fd_eid, fd_addr);
    if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
        return status;
    }

    /* If there was package data present in the Firmware Device Identification Area of the Firmware Update Package then 
     * the FD will specify to the UA that it will send the GetPackageData command. */
    if (ua_mgr->update_info.fd_will_send_pkg_data_cmd) {
        platform_printf("Cerberus UA receives: GetPackageData.\n");
        /* Since this is a multipart transfer we will receive and respond to GetPackageData commands
         * so long as the transfer flag does not indicate the end of the transfer. */
        do {
            status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
            if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        } while (ua_mgr->get_cmd_state.transfer_flag != PLDM_END && ua_mgr->get_cmd_state.transfer_flag != PLDM_START_AND_END);
        reset_get_cmd_state(&ua_mgr->get_cmd_state);
    }

    /* If the FD has specified that it has additional meta data it needs to UA to retain
     * then the UA will send the GetDeviceMetaData command. */
    if (ua_mgr->update_info.fd_meta_data_len > 0) {
        platform_printf("Cerberus UA issues: GetDeviceMetaData.\n");
        /* Since this is a multipart transfer we will send GetDeviceMetaData commands
         * so long as the transfer flag does not indicate the end of the transfer. After the first
         * GetDeviceMetaData command is sent the transfer operation flag will switch to indicate to
         * the FD that it needs to send back the next part of the meta data. Once all of the meta data
         * is transferred the operation flag will switch back. */
        do {
            status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_GET_DEVICE_METADATA, fd_eid, fd_addr);
            if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        } while (ua_mgr->get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
        reset_get_cmd_state(&ua_mgr->get_cmd_state);
    }

    platform_printf("Cerberus UA issues: PassComponentTable.\n");
    /* Now we pass the component table to the FD. The component table is a list of component information
     * from the Component Image Information Area from the Firmware Update Package. */
    int num_components = ua_mgr->num_components;
    for (ua_mgr->current_comp_num = 0; ua_mgr->current_comp_num < num_components; ua_mgr->current_comp_num++) {
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_PASS_COMPONENT_TABLE, fd_eid, fd_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
    }

    /* After passing the component parameter table we can begin to update each firmware component. */
    for (ua_mgr->current_comp_num = 0; ua_mgr->current_comp_num < num_components; ua_mgr->current_comp_num++) {

        platform_printf("Cerberus UA issues: UpdateComponent %u.\n", ua_mgr->current_comp_num + 1);
        /* We first send the UpdateComponent command to specify to the FD which component should be updated. 
         * We do this by sequentially traversing the component list although this can be changed to be any order. */
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_UPDATE_COMPONENT, fd_eid, fd_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }

        platform_printf("Cerberus UA waits for FD to prepare.\n");
        /* We then sleep for as long as the FD specified durng the UpdateComponent command
         * before receiving RequestFirmwareData command. */
        platform_msleep(ua_mgr->comp_img_entries[ua_mgr->current_comp_num].time_before_req_fw_data * 1000);

        platform_printf("Cerberus UA receives: RequestFirmwareData until TransferComplete.\n");
        /* After receiving the UpdateComponent command the FD will begin to request parts of the 
         * firmware image via the RequestFirmwareData command. The UA will continue to receive until the FD
         * sends the TransferComplete command. */
        do {
            status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
            if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        } while (ua_mgr->state.previous_cmd != PLDM_TRANSFER_COMPLETE);


        platform_printf("Cerberus UA receives: VerifyComplete.\n");
        /* After the firmware component image has been transferred the FD will verify the image. We leave the implementation of the
         * verify stage up to the AMI team including the mechanism by which the UA will wait for verification to complete.
         * In lieu of the implementation we immediately receive the VerifyComplete command. A few examples of the mechanism 
         * by which the UA can wait are: the UA switches the time-out in the handler to a negative number in which the channel will wait
         * indefinitely until it receives the VerifyComplete command; the UA periodically sends a GetStatus command to check
         * if the FD has transitioned out of the Verify state or to check the progress percent; the FD supplies the UA with the
         * maximum time it should wait for verficiation during the GetDeviceMetaData phase, the UA sets the time-out in the handler
         * to this value. */
        status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }

        platform_printf("Cerberus UA receives: ApplyComplete.\n");
        /* After verification is completed the FD will then transfer the component to a storage area and send ApplyComplete
         * command once it is done. Similarly to the verify stage, the FD may take significant time to transfer the component
         * in which case the UA needs a mechanism to wait. In lieu of this we immediately receive the ApplyComplete command. */
        status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
    }

    platform_printf("Cerberus UA issues: ActivateFirmware.\n");
    /* After all the firmware components have been downloaded, verified, and applied the UA sends the ActivateFirmware command
     * with a boolean flag telling the FD whether to activate any self-contained component or not. Setting this flag is to the
     * chosen value is left up to the AMI team. */
    status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_ACTIVATE_FIRMWARE, fd_eid, fd_addr);
    if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
        return status;
    }

    return 0;
}

/**
 * Internal reference to the function that will execute a firmware update when Cerberus is operating as the Update Agent
 * 
 * @param handler The firmware update handler.
 * @param ua_eid The endpoint ID of the update agent.  
 * @param ua_addr The SMBus address of the update agent. 
 * 
 * @return 0 if the update was successful otherwise an error code.
 */
int pldm_fwup_handler_start_update_fd(struct pldm_fwup_handler *handler, uint8_t ua_eid, uint8_t ua_addr)
{
    if (handler->mode != PLDM_FWUP_HANDLER_FD_MODE) {
        return PLDM_FWUP_HANDLER_INCORRECT_MODE;
    }

    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)handler->mctp->cmd_pldm;
    struct pldm_fwup_fd_manager *fd_mgr = &interface->fwup_mgr->fd_mgr;

    if (fd_mgr->fw_parameters == NULL || fd_mgr->flash_mgr->device_meta_data_region.length == 0 ||
        fd_mgr->flash_mgr->package_data_region.length == 0 || fd_mgr->flash_mgr->flash == NULL) {
        return PLDM_FWUP_HANDLER_INVALID_FD_MANAGER_STATE;
    }

    int status;

    /* The FD waits to receive the first command issued by the UA. If it was not RequestUpdate then we know the UA is
     * sending the inventory commands first. */
    status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
    if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
        return status;
    }
    platform_printf("Cerberus FD receives: %s.\n", fd_mgr->state.previous_cmd == PLDM_QUERY_DEVICE_IDENTIFIERS ? "QueryDeviceIdentifiers" : "RequestUpdate");
    platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

    if (fd_mgr->state.previous_cmd != PLDM_REQUEST_UPDATE) {
        platform_printf("Cerberus FD receives: GetFirmwareParameters.\n");
        /* If the first command was not RequestUpdate then receive and process the second inventory command. */
        status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

        platform_printf("Cerberus FD receives: RequestUpdate.\n");
        /* After processing both inventory commands the FD should then receive and process the RequestUpdate command. */
        status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));
    }
    /* After RequestUpdate is received the FD transitions into the Learn Components state. */

    
    /* If there is package data present in the Firmware Device Identification Area of the Firmware Update Package 
     * the UA will specify this in RequestUpdate. The FD will then send the GetPackageData command to the UA. */
    if (fd_mgr->update_info.package_data_len > 0) {
        platform_printf("Cerberus FD receives: GetPackageData.\n");
        /* Since this is a multipart transfer we will send GetPackageData commands
         * so long as the transfer flag does not indicate the end of the transfer. After the first
         * GetPackageData command is sent the transfer operation flag will switch to indicate to
         * the UA that it needs to send back the next part of the package data. Once all of the package data
         * is transferred the operation flag will switch back. */
        do {
            status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_GET_PACKAGE_DATA, ua_eid, ua_addr);
            if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        } while (fd_mgr->get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
        reset_get_cmd_state(&fd_mgr->get_cmd_state);
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));
    }

    /* If the FD has meta data to send to the UA then it will indicate that in its response to the 
     * RequestUpdate command. */
    if (fd_mgr->flash_mgr->device_meta_data_size > 0) {
        platform_printf("Cerberus FD issues: GetDeviceMetaData.\n");
        /* Since this is a multipart transfer we will receive and respond to GetDeviceMetaData commands
         * so long as the transfer flag does not indicate the end of the transfer. */
        do {
            status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
            if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        } while (fd_mgr->get_cmd_state.transfer_flag != PLDM_END && fd_mgr->get_cmd_state.transfer_flag != PLDM_START_AND_END);
        reset_get_cmd_state(&fd_mgr->get_cmd_state);
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));
    }

    platform_printf("Cerberus FD receives: PassComponentTable.\n");
    /* After the GetPackageData and GetDeviceMetaData commands the UA will begin to transfer the component parameter table via PassComponentTable. 
     * The number of components the UA will transfer was specified in the RequestUpdate command. The order of components received
     * is determiend by the UA. */
    do {
        status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
    } while (fd_mgr->update_info.comp_transfer_flag != PLDM_START_AND_END && fd_mgr->update_info.comp_transfer_flag != PLDM_END);
    platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));


    /* After receiving the component parameter table the FD transitions into the Ready Xfer state and waits to receive the 
     * UpdateComponent command. Like PassComponentTable the order by which the components are updated is determined by the UA. */
    int components_updated = 0;
    while (components_updated < fd_mgr->update_info.num_components) {
        platform_printf("Cerberus FD receives: UpdateComponent %u.\n", fd_mgr->update_info.current_comp_num + 1);
        /* Receive and respond to a UpdateComponent command. FD will transition to the Download state upon success. */
        status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

        platform_printf("Cerberus FD prepares.\n");
        platform_msleep(PLDM_FWUP_PROTOCOL_TIME_BERFORE_REQ_FW_DATA * 1000);

        platform_printf("Cerberus FD issues: RequestFirmwareData until image is transfered.\n");
        uint32_t current_comp_img_size = fd_mgr->update_info.current_comp_img_size;
        uint32_t max_transfer_size = fd_mgr->update_info.max_transfer_size;
        for (fd_mgr->update_info.current_comp_img_offset = 0; 
            fd_mgr->update_info.current_comp_img_offset < current_comp_img_size;
            fd_mgr->update_info.current_comp_img_offset += max_transfer_size) {
            
            /* The FD will send RequestFirmwareData commands to the UA to transfer parts of the firmware component image.
             * The transfer will continue so long as the offset is less than the size of the image being transferred. */
            status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_REQUEST_FIRMWARE_DATA, ua_eid, ua_addr);
            if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

        platform_printf("Cerberus FD issues: TransferComplete.\n");
        /* After the FD downloads the image it will send a TransferComplete command to the UA and transition into the Verify state. */
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_TRANSFER_COMPLETE, ua_eid, ua_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

        platform_printf("Cerberus FD receives: VerifyComplete.\n");
        /* The specifics on how the FD verifies the component image is left up to the AMI team. For now the FD will immediately send
         * the VerifyComplete command. After verification the FD will transition to the Apply state. */
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_VERIFY_COMPLETE, ua_eid, ua_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

        platform_printf("Cerberus FD receives: ApplyComplete.\n");
        /* During the Apply state the FD is supposed to transfer the firmware image to a storage location. However, this was already
         * done upon each RequestFirmwareData command. Therefore, no operation is currently performed in the Apply state. The AMI team
         * could use this state to write the firmware image to another location in flash than the ones designated in the flash manager of
         * the FD. For now the ApplyComplete command is sent to the UA and the FD transitions back to the Ready Xfer state. */
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_APPLY_COMPLETE, ua_eid, ua_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
        platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

        components_updated++;
    }

    platform_printf("Cerberus FD receives: ActivateFirmware.\n");
    /* After all the firmware components have been updated the UA will send the ActivateFirmware command. How the firmware
     * is activated is left up to the AMI team and their specific requirements. The time for self contained activation can be set
     * in the platform config or through some other configuration. */
    status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
    if ((status = pldm_fwup_handler_check_operation_status(status, fd_mgr->state.previous_completion_code)) != 0) {
        return status;
    }
    platform_printf("State: %s.\n", print_state(fd_mgr->state.current_state));

    return 0;
}


/**
* Initialize a PLDM firmware update handler instance.
* 
* @param handler The firmware update handler instance to initialize.
* @param channel The command channel for sending and receiving packets.
* @param mctp The MCTP protocol handler to use for packet processing.
* @param timeout_ms The time to wait in miliseconds for receiving a single packet.
* 
* @return 0 if the handler was successfully initialized or an error code.
*/
int pldm_fwup_handler_init(struct pldm_fwup_handler *handler, struct cmd_channel *channel, struct mctp_interface *mctp, int timeout_ms)
{
    if (handler == NULL || channel == NULL || mctp == NULL) {
        return PLDM_FWUP_HANDLER_INVALID_ARGUMENT;
    }

    memset(handler, 0, sizeof (struct pldm_fwup_handler));

    handler->channel = channel;
    handler->mctp = mctp;
    
    handler->run_update_ua = pldm_fwup_handler_run_update_ua;
    handler->start_update_fd = pldm_fwup_handler_start_update_fd;
    handler->timeout_ms = timeout_ms;

    return 0;
}


/**
 * Release the resources used for handling PLDM firmware update.
 *
 * @param handler The firmware update handler to release.
 */
void pldm_fwup_handler_release(struct pldm_fwup_handler *handler)
{
    UNUSED(handler);
}

/**
 * Set the mode of operation for the handler.
 * 
 * @param handler The firmware update handler.
 * @param mode The mode of operation to set to.
 * 
 * @return 0 on success or an error code.
 */
int pldm_fwup_handler_set_mode(struct pldm_fwup_handler *handler, enum pldm_fwup_handler_mode mode)
{
    if (mode != PLDM_FWUP_HANDLER_UA_MODE && mode != PLDM_FWUP_HANDLER_FD_MODE) {
        return PLDM_FWUP_HANDLER_INVALID_ARGUMENT;
    }

    handler->mode = mode;
    return 0;
}