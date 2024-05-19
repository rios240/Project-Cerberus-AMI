#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cmd_interface_pldm.h"
#include "pldm_fwup_protocol_commands.h"
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
        return PLDM_ERROR_INVALID_LENGTH;
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
        return PLDM_ERROR_INVALID_DATA;
    }

    status = cmd_interface_pldm_process_pldm_protocol_message(interface, request, &pldm_command);
    if (status != 0) {
        return status;
    }

    switch (pldm_command) {
#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = pldm_fwup_process_query_device_identifiers_request(interface->fwup_state, interface->device_mgr, request);
            break;
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = pldm_fwup_prcocess_get_firmware_parameters_request(interface->fwup_state, interface->device_mgr, request);
            break;
        case PLDM_REQUEST_UPDATE:
            status = pldm_fwup_process_request_update_request(interface->fwup_state, interface->fwup_flash, request);
            break;
        case PLDM_GET_DEVICE_METADATA:
            status = pldm_fwup_process_get_device_meta_data_request(interface->fwup_state, interface->fwup_flash, request);
            break;
#else
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_process_get_package_data_request(interface->fwup_state, interface->fwup_flash, request);
            break;
#endif
        default:
            status = PLDM_ERROR_UNSUPPORTED_PLDM_CMD;
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
        return PLDM_ERROR_INVALID_DATA;
    }

    status = cmd_interface_pldm_process_pldm_protocol_message(interface, response, &command);
    if (status != 0) {
        return status;
    }

    switch (command) {
#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_process_get_package_data_response(interface->fwup_state, interface->fwup_flash, response);
            break;
#else
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = pldm_fwup_process_query_device_identifiers_response(interface->fwup_state, interface->device_mgr, response);
            break;
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = pldm_fwup_process_get_firmware_parameters_response(interface->fwup_state, interface->device_mgr, response);
            break;
        case PLDM_REQUEST_UPDATE:
            status = pldm_fwup_process_request_update_response(interface->fwup_flash, interface->fwup_state, response);
            break;
        case PLDM_GET_DEVICE_METADATA:
            status = pldm_fwup_process_get_device_meta_data_response(interface->fwup_state, interface->fwup_flash, response);
            break;
#endif
        default:
            status = PLDM_ERROR_UNSUPPORTED_PLDM_CMD;
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

    return PLDM_ERROR_INVALID_DATA;
}

/**
 * Initialize only the variable fwup state for the command interface.  The rest of the command
 * interface instance is assumed to have already been initialized.
 *
 * This would generally be used with a statically initialized instance.
 *
 * @param fwup_state The the FWUP state to initialize.
 *
 * @return 0 if the state was successfully initialized or an error code.
 */
int cmd_interface_pldm_init_fwup_state(struct pldm_fwup_state *fwup_state)
{

    if ((fwup_state == NULL)) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (fwup_state, 0, sizeof (struct pldm_fwup_state));
#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
    fwup_state->state = PLDM_FD_STATE_IDLE;
#endif
    fwup_state->multipart_transfer.transfer_op_flag = PLDM_GET_FIRSTPART;
    fwup_state->multipart_transfer.data_transfer_handle = 0;
    fwup_state->multipart_transfer.transfer_flag = PLDM_START;
    fwup_state->multipart_transfer.next_data_transfer_handle = 0;

    return 0;
}



/**
 * Initialize a PLDM command interface instance
 *
 * @param intf The PLDM control command interface instance to initialize.
 * @param fwup_flash The flash address mapping to use for a FWUP.
 * @param fwup_state Variable FWUP context for the command interface.
 * @param device_mgr The device manager linked to command interface.
 *
 * @return Initialization status, 0 if success or an error code.
 */
int cmd_interface_pldm_init (struct cmd_interface_pldm *intf, 
    struct pldm_fwup_flash_map *fwup_flash, struct pldm_fwup_state *fwup_state,
    struct device_manager *device_mgr)
{

    if ((intf == NULL) || fwup_flash == NULL || device_mgr == NULL) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (intf, 0, sizeof (struct cmd_interface_pldm));

    intf->fwup_flash = fwup_flash;
    intf->device_mgr = device_mgr;
    intf->fwup_state = fwup_state;

    intf->base.process_request = cmd_interface_pldm_process_request;
#ifdef CMD_ENABLE_ISSUE_REQUEST
    intf->base.process_response = cmd_interface_pldm_process_response;
#endif
    intf->base.generate_error_packet = cmd_interface_pldm_generate_error_packet;

    return cmd_interface_pldm_init_fwup_state(intf->fwup_state);

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

