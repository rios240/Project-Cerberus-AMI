#include "pldm_fwup.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface_pldm.h"
#include "pldm_fwup_protocol_commands.h"
#include "pldm_fwup_protocol.h"

#include "libpldm/firmware_update.h"


/**
* Generate PLDM FWUP request for use in the MCTP interface
* 
* @param intf The PLDM FWUP control command interface instance
* @param command The PLDM FWUP command
* @param buffer The buffer to store the PLDM message
* @param buf_len The length of the buffer
* 
* @return size of the request or pldm_completion_codes
*/
int pldm_fwup_generate_request(struct cmd_interface_pldm *intf, uint8_t command, uint8_t *buffer, size_t buf_len) 
{
    int status; 

    switch(command) {
// Firmware Device
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_generate_get_package_data_request(&intf->fwup_mgr->fd_mgr.state, &intf->fwup_mgr->fd_mgr.get_cmd_state,
                buffer, buf_len);
            break;
// Update Agent
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = pldm_fwup_generate_query_device_identifiers_request(&intf->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = pldm_fwup_generate_get_firmware_parameters_request(&intf->fwup_mgr->ua_mgr.state, buffer, buf_len);
            break;
        case PLDM_REQUEST_UPDATE:
            status = pldm_fwup_generate_request_update_request(&intf->fwup_mgr->ua_mgr, buffer, buf_len);
            break;
        case PLDM_GET_DEVICE_METADATA:
            status = pldm_fwup_generate_get_device_meta_data_request(&intf->fwup_mgr->ua_mgr.state, &intf->fwup_mgr->ua_mgr.get_cmd_state, 
                buffer, buf_len);
            break;
        case PLDM_PASS_COMPONENT_TABLE:
            status = pldm_fwup_generate_pass_component_table_request(&intf->fwup_mgr->ua_mgr, buffer, buf_len);
            break;
        case PLDM_UPDATE_COMPONENT:
            status = pldm_fwup_generate_update_component_request(&intf->fwup_mgr->ua_mgr.state, intf->fwup_mgr->ua_mgr.current_comp_num, 
                intf->fwup_mgr->ua_mgr.comp_img_entries, &intf->fwup_mgr->ua_mgr.rec_fw_parameters, buffer, buf_len);
            break;
        default:
        status =  CMD_HANDLER_PLDM_UNKNOWN_REQUEST;    
    }

    return status;
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