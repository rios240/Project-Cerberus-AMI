#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common/unused.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface_pldm_fwup.h"
#include "pldm_fwup_commands.h"


#ifdef PLDM_FWUP_FD_ENABLE
#include "libpldm/firmware_update.h"
#include "libpldm/base.h"
#elif defined(PLDM_FWUP_UA_ENABLE)
#include "firmware_update.h"
#include "base.h"
#endif

/**
 * Pre-process received PLDM FWUP protocol message.
 *
 * @param intf The command interface that will process the message.
 * @param message The message being processed.
 * @param command_id Pointer to hold command ID of incoming message.
 *
 * @return 0 if the message was successfully processed or an error code.
 */
static int cmd_interface_pldm_fwup_process_pldm_protocol_message (
    struct cmd_interface_pldm_fwup *intf, struct cmd_interface_msg *message, uint8_t *command)
{
    struct pldm_msg *msg = (struct pldm_msg *)&message->data[1];
    struct pldm_header_info header = { 0 };
    int status;

    UNUSED (intf);

    printf("Unpacking PLDM header to get command type.\n");

    message->crypto_timeout = false;

    if (message->length < sizeof (struct pldm_msg) + 1) {
        return PLDM_ERROR_INVALID_LENGTH;
    }

    status = unpack_pldm_header((const struct pldm_msg_hdr *)&msg->hdr, &header);

    if (header.pldm_type != PLDM_FWUP) {
        return PLDM_ERROR_INVALID_PLDM_TYPE;
    }

    *command = header.command;

    return status;

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
static int cmd_interface_pldm_fwup_process_request (struct cmd_interface *intf,
    struct cmd_interface_msg *request)
{
    struct cmd_interface_pldm_fwup *interface = (struct cmd_interface_pldm_fwup *)intf;
    uint8_t command;
    int status;

    if (request == NULL) {
        return PLDM_ERROR_INVALID_DATA;
    }

    status = cmd_interface_pldm_fwup_process_pldm_protocol_message(interface, request, &command);
    if (status != 0) {
        return status;
    }

    switch (command) {
#ifdef PLDM_FWUP_FD_ENABLE
#elif defined(PLDM_FWUP_UA_ENABLE)
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_process_get_package_data_request(&interface->multipart_transfer, interface->flash_map, request);
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
static int cmd_interface_pldm_fwup_process_response (struct cmd_interface *intf,
    struct cmd_interface_msg *response)
{
    struct cmd_interface_pldm_fwup *interface = (struct cmd_interface_pldm_fwup *)intf;
    uint8_t command;
    int status;

    if (response == NULL) {
        return PLDM_ERROR_INVALID_DATA;
    }

    status = cmd_interface_pldm_fwup_process_pldm_protocol_message(interface, response, &command);
    if (status != 0) {
        return status;
    }

    switch (command) {
#ifdef PLDM_FWUP_FD_ENABLE
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_process_get_package_data_response(interface->multipart_transfer, interface->flash_map, response);
            break;
#elif defined(PLDM_FWUP_UA_ENABLE)
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
static int cmd_interface_pldm_fwup_generate_error_packet (struct cmd_interface *intf,
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
* Generate PLDM FWUP request for use in mctp_interface_issue_request
* 
* @param intf The PLDM FWUP control command interface instance
* @param command The PLDM FWUP command
* @param buffer The buffer to store the PLDM message
* @param buf_len The length of the buffer
* 
* @return size of the request or pldm_completion_codes
*/
int cmd_interface_pldm_fwup_generate_request(struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len) {
    struct cmd_interface_pldm_fwup *interface = (struct cmd_interface_pldm_fwup *)intf;
    int status;


    UNUSED (interface);   

    switch(command) {
#ifdef PLDM_FWUP_FD_ENABLE
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_generate_get_package_data_request(interface->multipart_transfer, buffer, buf_len);
            break;
#elif defined(PLDM_FWUP_UA_ENABLE)
#endif
    default:
        status =  PLDM_ERROR_UNSUPPORTED_PLDM_CMD;    
    }

    return status;
}


/**
 * Initialize only the variable state for the cmd interface.  The rest of the cmd
 * interface instance is assumed to have already been initialized.
 *
 * This would generally be used with a statically initialized instance.
 *
 * @param state The the state to initialize.
 *
 * @return 0 if the state was successfully initialized or an error code.
 */
int cmd_interface_pldm_fwup_init_state(struct pldm_fwup_state *state)
{

    if ((state == NULL)) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (state, 0, sizeof (struct pldm_fwup_state));

#ifdef PLDM_FWUP_FD_ENABLE
    state->fwup_state = PLDM_FD_STATE_IDLE;
#endif

    return 0;
}

/**
 * Initialize only the multipart transfer for the cmd_interface. 
 * The rest of the cmd interface instance is assumed to have already been initialized.
 * 
 * This would generally be used with a statically initialized instance.
 * 
 * @param multipart_transfer The multipart transfer to initialized
 * 
 * @return 0 if the multipart transfer was successfully initialized or an error code.
*/
int cmd_interface_pldm_fwup_init_multipart_transfer(struct pldm_fwup_multipart_transfer_context *multipart_transfer)
{
    if ((multipart_transfer == NULL)) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (multipart_transfer, 0, sizeof (struct pldm_fwup_multipart_transfer_context));

#ifdef PLDM_FWUP_FD_ENABLE
    multipart_transfer->transfer_op_flag = PLDM_GET_FIRSTPART;
#elif defined(PLDM_FWUP_UA_ENABLE)
    multipart_transfer->transfer_flag = PLDM_START;
#endif

    return 0;
}


/**
 * Initialize PLDM FWUP command interface instance
 *
 * @param intf The PLDM FWUP control command interface instance to initialize
 * @param flash_map The flash address mapping to use for PLDM FWUP
 * @param state_ptr Variable context for the cmd interface.
 * @param control The FW update control instance to use.
 * @param multipart_transfer_ptr Variable context for a multipart transfer.
 *
 * @return Initialization status, 0 if success or an error code.
 */
int cmd_interface_pldm_fwup_init (struct cmd_interface_pldm_fwup *intf, 
    struct pldm_fwup_flash_map *flash_map, struct pldm_fwup_state *state_ptr, 
    const struct firmware_update_control *control, struct pldm_fwup_multipart_transfer_context *multipart_transfer_ptr)
{

    if ((intf == NULL) || flash_map == NULL || control == NULL) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (intf, 0, sizeof (struct cmd_interface_pldm_fwup));

    intf->flash_map = flash_map;

#ifdef PLDM_FWUP_FD_ENABLE
    intf->control = control;
#endif

    intf->base.process_request = cmd_interface_pldm_fwup_process_request;
#ifdef CMD_ENABLE_ISSUE_REQUEST
    intf->base.process_response = cmd_interface_pldm_fwup_process_response;
#endif
    intf->base.generate_error_packet = cmd_interface_pldm_fwup_generate_error_packet;

    int status = cmd_interface_pldm_fwup_init_multipart_transfer(intf->multipart_transfer);
    if (status != 0) {
        return status;
    }

    return cmd_interface_pldm_fwup_init_state(intf->state);

}


/**
 * Deinitialize PLDM FWUP  command interface instance
 *
 * @param intf The PLDM FWUP command interface instance to deinitialize
 */
void cmd_interface_pldm_fwup_deinit (struct cmd_interface_pldm_fwup *intf)
{
	if (intf != NULL) {
		memset (intf, 0, sizeof (struct cmd_interface_pldm_fwup));
	}
}

