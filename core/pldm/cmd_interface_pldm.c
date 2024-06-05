#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "pldm_fwup_protocol_commands.h"
#include "pldm_fwup_manager.h"
#include "pldm_fwup_protocol.h"
#include "common/unused.h"
#include "libpldm/firmware_update.h"
#include "libpldm/base.h"

/**
 * Pre-process received PLDM FWUP protocol message.
 *
 * @param intf The command interface that will process the message.
 * @param message The message being processed.
 * @param pldm_command Pointer to hold command ID of incoming message.
 *
 * @return 0 if the message was successfully processed or a pldm error code.
 */
static int cmd_interface_pldm_process_pldm_protocol_message (
    struct cmd_interface_pldm *intf, struct cmd_interface_msg *message, uint8_t *pldm_command)
{
    struct pldm_msg *msg = (struct pldm_msg *)&message->data[PLDM_MCTP_BINDING_MSG_OFFSET];
    struct pldm_header_info header = { 0 };

    UNUSED (intf);

    message->crypto_timeout = false;

    if (message->length < sizeof (struct pldm_msg)) {
        return CMD_HANDLER_PLDM_PAYLOAD_TOO_SHORT;
    }

    
    int status = unpack_pldm_header((const struct pldm_msg_hdr *)&msg->hdr, &header);
    if (status != 0) {
        return status;
    }

    *pldm_command = header.command;

    return 0;

}

/**
* Process a PLDM FWUP received request.
*
* @param intf The command interface that will process the request.
* @param request The request data to process.  This will be updated to contain a response, if
* necessary.
*
* @return 0 if the request was successfully processed or an error code.
*/
static int cmd_interface_pldm_process_request (struct cmd_interface *intf,
    struct cmd_interface_msg *request)
{
    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)intf;
    uint8_t pldm_command;
    int status;

    if (request == NULL) {
        return CMD_HANDLER_PLDM_INVALID_ARGUMENT;
    }

    status = cmd_interface_pldm_process_pldm_protocol_message(interface, request, &pldm_command);
    if (status != 0) {
        return status;
    }

    switch (pldm_command) {
        //Firmware Device
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = pldm_fwup_process_query_device_identifiers_request(&interface->fwup_mgr->fd_mgr.state, interface->device_mgr, request);
            break;
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = pldm_fwup_prcocess_get_firmware_parameters_request(&interface->fwup_mgr->fd_mgr.state, interface->fwup_mgr->fd_mgr.fw_parameters, request);
            break;
        case PLDM_REQUEST_UPDATE:
            status = pldm_fwup_process_request_update_request(&interface->fwup_mgr->fd_mgr.state, interface->fwup_mgr->fd_mgr.flash_mgr,
                &interface->fwup_mgr->fd_mgr.update_info, request);
            break;
        case PLDM_GET_DEVICE_METADATA:
            status = pldm_fwup_process_get_device_meta_data_request(&interface->fwup_mgr->fd_mgr.state, interface->fwup_mgr->fd_mgr.flash_mgr,
                &interface->fwup_mgr->fd_mgr.get_cmd_state, &interface->fwup_mgr->fd_mgr.update_info, request);
            break;
        case PLDM_PASS_COMPONENT_TABLE:
            status = pldm_fwup_process_pass_component_table_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info,
                interface->fwup_mgr->fd_mgr.fw_parameters, request);
            break;
        case PLDM_UPDATE_COMPONENT:
            status = pldm_fwup_process_update_component_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info,
                interface->fwup_mgr->fd_mgr.update_info.comp_entries, request);
            break;
        case PLDM_ACTIVATE_FIRMWARE:
            status = pldm_fwup_process_activate_firmware_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info, request);
            break;
        case PLDM_GET_STATUS:
            status = pldm_fwup_process_get_status_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info, request);
            break;
        case PLDM_CANCEL_UPDATE_COMPONENT:
            status = pldm_fwup_process_cancel_update_component_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info, request);
            break;
        case PLDM_CANCEL_UPDATE:
            status = pldm_fwup_process_cancel_update_request(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info,
                interface->fwup_mgr->fd_mgr.flash_mgr, request);
            break;
        //Update Agent
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_process_get_package_data_request(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.flash_mgr,
                &interface->fwup_mgr->ua_mgr.get_cmd_state, request);
            break;
        case PLDM_REQUEST_FIRMWARE_DATA:
            status = pldm_fwup_process_request_firmware_data_request(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.current_comp_num,
                interface->fwup_mgr->ua_mgr.comp_img_entries, interface->fwup_mgr->ua_mgr.flash_mgr, request);
            break;
        case PLDM_TRANSFER_COMPLETE:
            status = pldm_fwup_process_transfer_complete_request(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.update_info, request);
            break;
        case PLDM_VERIFY_COMPLETE:
            status = pldm_fwup_process_verify_complete_request(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.update_info, request);
            break;
        case PLDM_APPLY_COMPLETE:
            status = pldm_fwup_process_apply_complete_request(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.update_info, request);
            break;
        case PLDM_GET_META_DATA:
            status = pldm_fwup_process_get_meta_data_request(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.flash_mgr,
                &interface->fwup_mgr->ua_mgr.get_cmd_state, request);
            break;
        default:
            status = CMD_HANDLER_PLDM_UNKNOWN_REQUEST;
    }

    return status;
}

/**
* Process a PLDM FWUP received response.
*
* @param intf The command interface that will process the response.
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
static int cmd_interface_pldm_process_response (struct cmd_interface *intf,
    struct cmd_interface_msg *response)
{
    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)intf;
    uint8_t command;
    int status;

    if (response == NULL) {
        return CMD_HANDLER_PLDM_INVALID_ARGUMENT;
    }

    status = cmd_interface_pldm_process_pldm_protocol_message(interface, response, &command);
    if (status != 0) {
        return status;
    }

    switch (command) {
        // Firmware Device
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_process_get_package_data_response(&interface->fwup_mgr->fd_mgr.state, interface->fwup_mgr->fd_mgr.flash_mgr,
                &interface->fwup_mgr->fd_mgr.get_cmd_state, response);
            break;
        case PLDM_REQUEST_FIRMWARE_DATA:
            status = pldm_fwup_process_request_firmware_data_response(&interface->fwup_mgr->fd_mgr.state, &interface->fwup_mgr->fd_mgr.update_info,
                interface->fwup_mgr->fd_mgr.flash_mgr, response);
            break;
        case PLDM_TRANSFER_COMPLETE:
            status = pldm_fwup_process_transfer_complete_response(&interface->fwup_mgr->fd_mgr.state, response);
            break;
        case PLDM_VERIFY_COMPLETE:
            status = pldm_fwup_process_verify_complete_response(&interface->fwup_mgr->fd_mgr.state, response);
            break;
        case PLDM_APPLY_COMPLETE:
            status = pldm_fwup_process_apply_complete_response(&interface->fwup_mgr->fd_mgr.state, response);
            break;
        case PLDM_GET_META_DATA:
            status = pldm_fwup_process_get_meta_data_response(&interface->fwup_mgr->fd_mgr.state, interface->fwup_mgr->fd_mgr.flash_mgr,
                &interface->fwup_mgr->fd_mgr.get_cmd_state, response);
            break;
        // Update Agent
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = pldm_fwup_process_query_device_identifiers_response(&interface->fwup_mgr->ua_mgr.state, interface->device_mgr, response);
            break;
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = pldm_fwup_process_get_firmware_parameters_response(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.rec_fw_parameters, response);
            break;
        case PLDM_REQUEST_UPDATE:
            status = pldm_fwup_process_request_update_response(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.update_info, response);
            break;
        case PLDM_GET_DEVICE_METADATA:
            status = pldm_fwup_process_get_device_meta_data_response(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.flash_mgr,
                &interface->fwup_mgr->ua_mgr.get_cmd_state, response);
            break;
        case PLDM_PASS_COMPONENT_TABLE:
            status = pldm_fwup_process_pass_component_table_response(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.comp_img_entries, 
                interface->fwup_mgr->ua_mgr.current_comp_num, response);
            break;
        case PLDM_UPDATE_COMPONENT:
            status = pldm_fwup_process_update_component_response(&interface->fwup_mgr->ua_mgr.state, interface->fwup_mgr->ua_mgr.comp_img_entries, 
                interface->fwup_mgr->ua_mgr.current_comp_num, response);
            break;
        case PLDM_ACTIVATE_FIRMWARE:    
            status = pldm_fwup_process_activate_firmware_response(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.update_info, response);
            break;
        case PLDM_GET_STATUS:
            status = pldm_fwup_process_get_status_response(&interface->fwup_mgr->ua_mgr.state, &interface->fwup_mgr->ua_mgr.update_info, response);
            break;
        case PLDM_CANCEL_UPDATE_COMPONENT:
            status = pldm_fwup_process_cancel_update_component_response(&interface->fwup_mgr->ua_mgr.state, response);
            break;
        case PLDM_CANCEL_UPDATE:
            status = pldm_fwup_process_cancel_update_response(&interface->fwup_mgr->ua_mgr.state, response);
            break;
        default:
            status = CMD_HANDLER_PLDM_UNKNOWN_RESPONSE;
    }

    return status;
}

/**
* Generate a message to indicate an error condition.
*
* Unused by cmd_interface_pldm_fwup
*
* @param intf The command interface to utilize.
* @param request The request container to utilize.
* @param error_code Identifier for the error.
* @param error_data Data for the error condition.
* @param cmd_set Command set to respond on.
*
* @return PLDM_ERROR_INVALID_DATA
*/
static int cmd_interface_pldm_generate_error_packet (struct cmd_interface *intf,
	struct cmd_interface_msg *request, uint8_t error_code, uint32_t error_data, uint8_t cmd_set)
{
	UNUSED (intf);
	UNUSED (request);
	UNUSED (error_code);
	UNUSED (error_data);
	UNUSED (cmd_set);

    return CMD_HANDLER_PLDM_UNSUPPORTED_OPERATION;
}


/**
 * Initialize a PLDM command interface instance
 *
 * @param intf The PLDM control command interface instance to initialize.
 * @param fwup_mgr The firmware update manager linked to the command interface.
 * @param device_mgr The device manager linked to the command interface.
 *
 * @return Initialization status, 0 if success or an error code.
 */
int cmd_interface_pldm_init (struct cmd_interface_pldm *intf, 
    struct pldm_fwup_manager *fwup_mgr, struct device_manager *device_mgr)
{

    if ((intf == NULL) || fwup_mgr == NULL || device_mgr == NULL) {
        return CMD_HANDLER_PLDM_INVALID_ARGUMENT;
    }

    memset (intf, 0, sizeof (struct cmd_interface_pldm));

    intf->fwup_mgr = fwup_mgr;
    intf->device_mgr = device_mgr;

    intf->base.process_request = cmd_interface_pldm_process_request;
#ifdef CMD_ENABLE_ISSUE_REQUEST
    intf->base.process_response = cmd_interface_pldm_process_response;
#endif
    intf->base.generate_error_packet = cmd_interface_pldm_generate_error_packet;

    return 0;
}


/**
 * Deinitialize PLDM FWUP  command interface instance
 *
 * @param intf The PLDM FWUP command interface instance to deinitialize
 */
void cmd_interface_pldm_deinit (struct cmd_interface_pldm *intf)
{
	if (intf != NULL) {
		memset (intf, 0, sizeof (struct cmd_interface_pldm));
	}
}

