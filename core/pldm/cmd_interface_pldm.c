#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common/unused.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface_pldm.h"


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
    struct pldm_msg *msg = (struct pldm_msg *)&message->data[1];
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
//#ifdef PLDM_FWUP_FD_ENABLE


//#elif defined(PLDM_FWUP_UA_ENABLE)

//#endif
        default:
            status = PLDM_ERROR_UNSUPPORTED_PLDM_CMD;
    }

    return 0;
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
//#ifdef PLDM_FWUP_FD_ENABLE
        
//#elif defined(PLDM_FWUP_UA_ENABLE)

//#endif
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
* Generate PLDM FWUP request for use in mctp_interface_issue_request
* 
* @param intf The PLDM FWUP control command interface instance
* @param pldm_command The PLDM FWUP command
* @param buffer The buffer to store the PLDM message
* @param buf_len The length of the buffer
* 
* @return size of the request or pldm_completion_codes
*/
int cmd_interface_pldm_generate_request(struct cmd_interface *intf, uint8_t pldm_command, uint8_t *buffer, size_t buf_len) 
{
    struct cmd_interface_pldm *interface = (struct cmd_interface_pldm *)intf;
    int status;

    UNUSED (interface);   

    switch(pldm_command) {
//#ifdef PLDM_FWUP_FD_ENABLE
        
//#elif defined(PLDM_FWUP_UA_ENABLE)

//#endif
    default:
        status =  PLDM_ERROR_UNSUPPORTED_PLDM_CMD;    
    }

    return status;
}

/**
 * Initialize only the multipart transfer for the cmd_interface. 
 * The rest of the cmd interface instance is assumed to have already been initialized.
 * 
 * This would generally be used with a statically initialized instance.
 * 
 * @param fwup_multipart_transfer The multipart transfer to initialized
 * 
 * @return 0 if the multipart transfer was successfully initialized or an error code.
*/
int cmd_interface_pldm_init_fwup_multipart_transfer(struct pldm_fwup_multipart_transfer *fwup_multipart_transfer)
{
    if ((fwup_multipart_transfer == NULL)) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (fwup_multipart_transfer, 0, sizeof (struct pldm_fwup_multipart_transfer));

#ifdef PLDM_FWUP_FD_ENABLE
    fwup_multipart_transfer->transfer_op_flag = PLDM_GET_FIRSTPART;
#elif defined(PLDM_FWUP_UA_ENABLE)
    fwup_multipart_transfer->transfer_flag = PLDM_START;
#endif

    return 0;
}



/**
 * Initialize only the variable FWUP state for the cmd interface.  The rest of the cmd
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

#ifdef PLDM_FWUP_FD_ENABLE
    fwup_state->fwup_state = PLDM_FD_STATE_IDLE;
#endif

    return cmd_interface_pldm_init_fwup_multipart_transfer(&fwup_state->fwup_multipart_transfer);
}



/**
 * Initialize PLDM command interface instance
 *
 * @param intf The PLDM control command interface instance to initialize
 * @param fwup_flash_ptr The flash address mapping to use for FWUP
 * @param fwup_state_ptr Variable FWUP context for the cmd interface.
 *
 * @return Initialization status, 0 if success or an error code.
 */
int cmd_interface_pldm_init (struct cmd_interface_pldm *intf, 
    struct pldm_fwup_flash_map *fwup_flash_ptr, struct pldm_fwup_state *fwup_state_ptr)
{

    if ((intf == NULL) || fwup_flash_ptr == NULL) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (intf, 0, sizeof (struct cmd_interface_pldm));

    intf->fwup_flash = fwup_flash_ptr;

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
void cmd_interface_pldm_fwup_deinit (struct cmd_interface_pldm *intf)
{
	if (intf != NULL) {
		memset (intf, 0, sizeof (struct cmd_interface_pldm));
	}
}

