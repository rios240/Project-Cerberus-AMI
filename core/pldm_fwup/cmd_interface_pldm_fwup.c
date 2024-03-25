#include <stdint.h>
#include <string.h>
#include "common/unused.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface_pldm_fwup.h"
#include "pldm_fwup_depricated/pldm_fwup_commands.h"
#include "firmware_update.h"

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
    struct pldm_header_info *header;
    int status;

    UNUSED (intf);

    message->crypto_timeout = false;

    if (message->length < sizeof (struct pldm_msg) + 1) {
        return PLDM_ERROR_INVALID_LENGTH;
    }

    status = unpack_pldm_header((const struct pldm_msg_hdr *)&msg->hdr, header);

    if (header->pldm_type != PLDM_FWUP) {
        return PLDM_ERROR_INVALID_PLDM_TYPE;
    }

    *command = header->command;

    return 0;

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
#ifdef PLDM_FWUP_ENABLE_FD
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = process_and_respond_query_device_identifiers(intf, request);
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = process_and_respond_get_firmware_parameters(intf, request);
        case PLDM_REQUEST_UPDATE:
            status = process_and_response_request_update(intf, request);
        case PLDM_GET_DEVICE_METADATA:
            status = process_and_respond_get_device_meta_data(intf, request);
        case PLDM_PASS_COMPONENT_TABLE:
            status = process_and_respond_ua_pass_component_table(intf, request);
        case PLDM_UPDATE_COMPONENT:
            status = process_and_respond_update_component(intf, request);
        case PLDM_ACTIVATE_FIRMWARE:
            status = process_and_respond_activate_firmware(intf, request);
#elif PLDM_FWUP_ENABLE_UA
        case PLDM_GET_PACKAGE_DATA:
            status = process_and_respond_get_package_data(intf, request);
        case PLDM_REQUEST_FIRMWARE_DATA;
            status = process_and_respond_request_firmware_data(intf, request);
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
#ifdef PLDM_FWUP_ENABLE_FD
        case PLDM_GET_PACKAGE_DATA:
            status = process_get_package_data(intf, response);
        case PLDM_REQUEST_FIRMWARE_DATA:
            status = process_request_firmware_data(intf, response);
        case PLDM_TRANSFER_COMPLETE:
            status = process_transfer_complete(intf, response);
        case PLDM_VERIFY_COMPLETE:
            status = process_verify_complete(intf, response);
        case PLDM_APPLY_COMPLETE:
            status = process_apply_complete(intf, response);
#elif PLDM_FWUP_ENABLE_UA
        case PLDM_QUERY_DEVICE_IDENTIFIERS:
            status = process_query_device_identifiers(intf, reponse);
        case PLDM_GET_FIRMWARE_PARAMETERS:
            status = PLDM_GET_FIRMWARE_PARAMETERS(intf, response);
        case PLDM_REQUEST_UPDATE:
            status = process_request_update(intf, response);
        case PLDM_GET_DEVICE_METADATA:
            status = process_get_device_meta_data(intf, response);
        case PLDM_PASS_COMPONENT_TABLE:
            status = process_pass_component_table_resp(intf, response);
        case PLDM_UPDATE_COMPONENT:
            status = process_update_component_resp(intf, response);
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


/**
 * Initialize PLDM FWUP command interface instance
 *
 * @param intf The MCTP control command interface instance to initialize
 *
 * @return Initialization status, 0 if success or an error code.
 */
int cmd_interface_mctp_control_init (struct cmd_interface_pldm_fwup *intf)
{

    if ((intf == NULL)) {
        return PLDM_ERROR_INVALID_DATA;
    }

    memset (intf, 0, sizeof (struct cmd_interface_pldm_fwup));
    
    intf->base.process_request = cmd_interface_pldm_fwup_process_request;
#ifdef CMD_ENABLE_ISSUE_REQUEST
    intf->base.process_response = cmd_interface_pldm_fwup_process_response;
#endif
    intf->base.generate_error_packet = cmd_interface_pldm_fwup_generate_error_packet;
}

/*
void cmd_interface_mctp_control_deinit (struct cmd_interface_pldm_fwup *intf)
{
    
}
*/