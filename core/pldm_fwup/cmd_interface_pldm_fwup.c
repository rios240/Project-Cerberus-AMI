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
            status = pldm_fwup_process_get_package_data_response(&interface->multipart_transfer, interface->flash_map, response);
            break;
#elif defined(PLDM_FWUP_UA_ENABLE)
#endif
        default:
            status = PLDM_ERROR_UNSUPPORTED_PLDM_CMD;
    }

    return status;
}

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

static int cmd_interface_pldm_fwup_generate_request(struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len) {
    struct cmd_interface_pldm_fwup *interface = (struct cmd_interface_pldm_fwup *)intf;
    int status;


    UNUSED (interface);   

    switch(command) {
#ifdef PLDM_FWUP_FD_ENABLE
        case PLDM_GET_PACKAGE_DATA:
            status = pldm_fwup_generate_get_package_data_request(&interface->multipart_transfer, buffer, buf_len);
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
 * @param init_state The initial PLDM FWUP state.
 *
 * @return 0 if the state was successfully initialized or an error code.
 */
int cmd_interface_pldm_fwup_init_state(struct pldm_fwup_state *state, uint8_t init_state)
{

    if ((state == NULL)) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (state, 0, sizeof (struct pldm_fwup_state));

    state->state = init_state;

    return 0;
}


/**
 * Initialize PLDM FWUP command interface instance
 *
 * @param intf The PLDM FWUP control command interface instance to initialize
 * @param flash_map The flash address mapping to use for PLDM FWUP
 * @param state Variable context for the cmd interface.  This must be uninitialized.
 * @param control The FW update control instance to use.
 * @param init_state The initial state
 *
 * @return Initialization status, 0 if success or an error code.
 */
int cmd_interface_pldm_fwup_init (struct cmd_interface_pldm_fwup *intf, struct pldm_fwup_flash_map *flash_map,
    struct pldm_fwup_state *state_ptr, const struct firmware_update_control *control, uint8_t init_state)
{

    if ((intf == NULL) || flash_map == NULL || control == NULL) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (intf, 0, sizeof (struct cmd_interface_pldm_fwup));

    intf->flash_map = flash_map;
    intf->control = control;

#ifdef PLDM_FWUP_FD_ENABLE
    intf->state = state_ptr;

    intf->multipart_transfer.transfer_handle = 0;
    intf->multipart_transfer.transfer_op_flag = PLDM_GET_FIRSTPART;
#elif defined(PLDM_FWUP_UA_ENABLE)
    intf->multipart_transfer.transfer_flag = PLDM_START;
#endif

    intf->base.process_request = cmd_interface_pldm_fwup_process_request;
#ifdef CMD_ENABLE_ISSUE_REQUEST
    intf->base.process_response = cmd_interface_pldm_fwup_process_response;
    intf->generate_request = cmd_interface_pldm_fwup_generate_request;
#endif
    intf->base.generate_error_packet = cmd_interface_pldm_fwup_generate_error_packet;

#ifdef PLDM_FWUP_FD_ENABLE
    return cmd_interface_pldm_fwup_init_state(intf->state, init_state);
#elif defined(PLDM_FWUP_UA_ENABLE)
    return 0;
#endif

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

