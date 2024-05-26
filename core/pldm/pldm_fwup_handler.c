#include "pldm_fwup_handler.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface_pldm.h"
#include "pldm_fwup_protocol_commands.h"
#include "common/unused.h"
#include "cmd_interface/cmd_channel.h"
#include "mctp/mctp_interface.h"
#include "platform_api.h" 

#include "libpldm/firmware_update.h"


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
    size_t req_length = pldm_fwup_handler_generate_request(handler->mctp->cmd_pldm, command, handler->req_buffer, sizeof (handler->req_buffer));
    if (req_length == CMD_HANDLER_PLDM_TRANSPORT_ERROR) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    } 
    else if (req_length == PLDM_FWUP_HANDLER_UNKNOWN_REQUEST) {
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
pldm_fwup_handler_check_operation_status(int transport_status, int protocol_status) {
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
    int status;

    /* First we check if the UA needs to send inventory commands. If so we proceed with sending
     * the QueryDeviceIdentifier command and the GetFirmwareParameter command so that Cerberus
     * has the most recent configuration from the FD it is trying to update. */
    if (inventory_cmds) {
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_QUERY_DEVICE_IDENTIFIERS, fd_eid, fd_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }

        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_GET_FIRMWARE_PARAMETERS, fd_eid, fd_eid);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }
    }

    /* We now proceed in sending the RequestUpdate command starting the firmware update process. */
    status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_REQUEST_UPDATE, fd_eid, fd_eid);
    if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
        return status;
    }

    /* If there was package data present in the Firmware Device Identification Area of the Firmware Update Package then 
     * the FD will specify to the UA that it will send the GetPackageData command. */
    if (ua_mgr->update_info.fd_will_send_pkg_data_cmd) {
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
        /* We first send the UpdateComponent command to specify to the FD which component should be updated. 
         * We do this by sequentially traversing the component list although this can be changed to be any order. */
        status = pldm_fwup_handler_send_and_receive_full_mctp_message(handler, PLDM_UPDATE_COMPONENT, fd_eid, fd_addr);
        if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
            return status;
        }

        /* We then sleep for as long as the FD specified durng the UpdateComponent command
         * before receiving RequestFirmwareData command. */
        platform_msleep(ua_mgr->comp_img_entries[ua_mgr->current_comp_num].time_before_req_fw_data * 1000);

        /* After receiving the UpdateComponent command the FD will begin to request parts of the 
         * firmware image via the RequestFirmwareData command. The UA will continue to receive until the FD
         * sends the TransferComplete command. */
        do {
            status = pldm_fwup_handler_receive_and_respond_full_mctp_message(handler->channel, handler->mctp, handler->timeout_ms);
            if ((status = pldm_fwup_handler_check_operation_status(status, ua_mgr->state.previous_completion_code)) != 0) {
                return status;
            }
        } while (ua_mgr->state.previous_cmd != PLDM_TRANSFER_COMPLETE);

        /* After the firmware image has been transferred the FD will verify the  */
    }
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
    handler->timeout_ms = timeout_ms;

    return 0;
}


/**
 * Release the resources used for handling PLDM firmware update.
 *
 * @param handler The firmware update handler to release.
 */
int pldm_fwup_handler_release(struct pldm_fwup_handler *handler)
{
    UNUSED(handler);
}


/*
int pldm_fwup_run_update(struct mctp_interface *mctp, struct cmd_channel *channel, uint8_t inventory_cmds, uint8_t device_eid, int ms_timeout)
{
    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)mctp->cmd_pldm;
    interface->updating_device_eid = device_eid;
    uint8_t request_buf[MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN];
    size_t request_size;
    int status;
//#ifdef PLDM_FWUP_FD_ENABLE
    if (inventory_cmds) {
        // QueryDeviceIdentifiers
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        // GetFirmwareParameters
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }
    }

    // RequestUpdate
    status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
    if (status != 0) {
        return status;
    }

    //GetPackageData
    if (interface->package_data_len) {
        do {
            request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_GET_PACKAGE_DATA, request_buf, sizeof (request_buf));

            status = mctp_interface_issue_request(mctp, channel, 
                device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
                request_buf, request_size, request_buf, sizeof (request_buf), 0);
            if (status != 0) {
                return status;
            }

            status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
            if (status != 0) {
                return status;
            }
        } while (interface->fwup_state->fwup_multipart_transfer.transfer_op_flag != PLDM_GET_FIRSTPART);
    }

    //GetDeviceMetaData
    do {
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }
    } while (interface->fwup_state->fwup_multipart_transfer.transfer_flag != PLDM_START && 
        interface->fwup_state->fwup_multipart_transfer.transfer_flag != PLDM_START_AND_END);


    int i = 0;
    while (i < interface->num_components) {
        //PassComponentTable
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //UpdateComponent
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //RequestFirmwareData
        do {
           request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_REQUEST_FIRMWARE_DATA, request_buf, sizeof (request_buf));

            status = mctp_interface_issue_request(mctp, channel, 
                device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
                request_buf, request_size, request_buf, sizeof (request_buf), 0);
            if (status != 0) {
                return status;
            }

            status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
            if (status != 0) {
                return status;
            } 
        } while (interface->fwup_state->comp_offset != interface->fwup_state->comp_size);

        //TransferComplete
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_TRANSFER_COMPLETE, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }

        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //VerifyComplete
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_VERIFY_COMPLETE, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //ApplyComplete
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_APPLY_COMPLETE, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        i++;
    }

    //ActivateFirmware
    status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
    return status;
    
//#elif defined(PLDM_FWUP_UA_ENABLE)
    if (inventory_cmds) {
        //QueryDeviceIdentifiers
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_QUERY_DEVICE_IDENTIFIERS, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //GetFirmwareParameters
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_GET_FIRMWARE_PARAMETERS, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }
    }

    //RequestUpdate
    request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_REQUEST_UPDATE, request_buf, sizeof (request_buf));
    status = mctp_interface_issue_request(mctp, channel, 
        device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
        request_buf, request_size, request_buf, sizeof (request_buf), 0);
    if (status != 0) {
        return status;
    }
    status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
    if (status != 0) {
        return status;
    }

    //GetPackageData
    if (interface->fd_will_send_pkg_data_cmd) {
        do {
            status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
            if (status != 0) {
                return status;
            }
        } while (interface->fwup_state->fwup_multipart_transfer.transfer_flag != PLDM_START && 
            interface->fwup_state->fwup_multipart_transfer.transfer_flag != PLDM_START_AND_END);
    }

    //GetDeviceMetaData
    do {
       request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_GET_DEVICE_METADATA, request_buf, sizeof (request_buf));

        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }

        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }  
    } while (interface->fwup_state->fwup_multipart_transfer.transfer_flag != PLDM_START && 
        interface->fwup_state->fwup_multipart_transfer.transfer_flag != PLDM_START_AND_END);
    

    uint8_t num_components = fup_interface_get_num_components(interface->fwup_flash->fw_update_package_flash,
        interface->fwup_flash->fw_update_package_addr, 
        &interface->device_manager->entries[device_manager_get_device_addr_by_eid(interface->device_manager, device_eid)]);

    int i = 0;
    interface->current_component = 1;
    while (i < num_components) {
        //PassComponentTable
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_PASS_COMPONENT_TABLE, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        } 

        //UpdateComponent
        request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_UPDATE_COMPONENT, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(mctp, channel, 
            device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
            request_buf, request_size, request_buf, sizeof (request_buf), 0);
        if (status != 0) {
            return status;
        }
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //RequestFirmwareData
        do {
            status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
            if (status != 0) {
                return status;
            }
        } while (interface->fwup_state->comp_offset != interface->fwup_state->comp_size);

        //TransferComplete
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //VerifyComplete
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        //ApplyComplete
        status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
        if (status != 0) {
            return status;
        }

        i++;
        interface->current_component++;
    }

    //ActivateFirmware
    request_size = cmd_interface_pldm_generate_request(&interface->base, PLDM_ACTIVATE_FIRMWARE, request_buf, sizeof (request_buf));
    status = mctp_interface_issue_request(mctp, channel, 
        device_manager_get_device_addr_by_eid(interface->device_manager, device_eid), device_eid,
        request_buf, request_size, request_buf, sizeof (request_buf), 0);
    if (status != 0) {
        return status;
    }
    status = cmd_channel_receive_and_process(channel, mctp, ms_timeout);
    return status;
//#endif

}
*/