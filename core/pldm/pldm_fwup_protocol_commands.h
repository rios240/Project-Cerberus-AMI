#ifndef PLDM_FWUP_PROTOCOL_COMMANDS_H_
#define PLDM_FWUP_PROTOCOL_COMMANDS_H_


#include <stdint.h>
#include "cmd_interface_pldm.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface/device_manager.h"
#include "mctp/mctp_base_protocol.h"
#include "platform_config.h"

#include "libpldm/base.h"


#ifndef PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE
#define PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE                        MCTP_BASE_PROTOCOL_MAX_TRANSMISSION_UNIT
#endif

#ifndef PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ
#define PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ             1
#endif                         

#define PLDM_MCTP_BINDING_MSG_OVERHEAD                              (sizeof (struct pldm_msg_hdr) + MCTP_BASE_PROTOCOL_MSG_TYPE_SIZE);
#define PLDM_MCTP_BINDING_MSG_OFFSET                                MCTP_BASE_PROTOCOL_MSG_TYPE_SIZE


/*

int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request);
int pldm_fwup_prcocess_get_firmware_parameters_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request);
int pldm_fwup_process_request_update_request(struct cmd_interface_pldm *cmd_pldm, struct cmd_interface_msg *request);

    
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_multipart_transfer *multipart_transfer, 
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_multipart_transfer *multipart_transfer,
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *response);



int pldm_fwup_process_get_package_data_request(struct pldm_fwup_multipart_transfer *multipart_transfer, 
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *request);


int pldm_fwup_generate_query_device_identifiers_request(uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_query_device_identifiers_response(struct pldm_fwup_state *fwup_state, 
    struct device_manager *device_manager, struct cmd_interface_msg *response);

int pldm_fwup_generate_get_firmware_parameters_request(uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_firmware_parameters_response(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *response);


int pldm_fwup_generate_request_update_request(struct pldm_fwup_flash_map *fwup_flash, struct device_manager *device_manager, uint8_t device_eid, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_request_update_response(struct cmd_interface_pldm *cmd_pldm, struct cmd_interface_msg *response);

*/



/**
 * Process a QueryDeviceIdentifiers request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_query_device_identifiers_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
 * Process a GetFirmwareParameters request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_prcocess_get_firmware_parameters_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
 * Process a RequestUpdate request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_request_update_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);


/**
* Generate a GetPackageData request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_package_data_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a GetPackageData response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_package_data_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);


/**
 * Process a GetDeviceMetaData request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_get_device_meta_data_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
 * Process a PassComponentTable request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_pass_component_table_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
 * Process a UpdateComponent request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_update_component_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);


/**
* Generate a RequestFirmwareData request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_request_firmware_data_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a RequestFirmwareData response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_request_firmware_data_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
* Generate a TransferComplete request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_transfer_complete_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a TransferComplete response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_transfer_complete_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
* Generate a VerifyComplete request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_verify_complete_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a VerifyComplete response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_verify_complete_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);


/**
* Generate a ApplyComplete request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_apply_complete_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a ApplyComplete response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_apply_complete_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);


/**
 * Process a ActivateFirmware request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_activate_firmware_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);



/**
* Generate a QueryDeviceIdentifiers request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_query_device_identifiers_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a QueryDeviceIdentifiers response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_query_device_identifiers_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
* Generate a GetFirmwareParameters request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_firmware_parameters_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a GetFirmwareParameters response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_firmware_parameters_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
* Generate a RequestUpdate request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_request_update_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a RequestUpdate response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_request_update_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
 * Process a GetPackageData request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_get_package_data_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
* Generate a GetDeviceMetaData request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_get_device_meta_data_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a GetDeviceMetaData response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_get_device_meta_data_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);


/**
* Generate a PassComponentTable request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_pass_component_table_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a PassComponentTable response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_pass_component_table_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
* Generate a UpdateComponent request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_update_component_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a UpdateComponent response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_update_component_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);

/**
 * Process a RequestFirmwareData request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_request_firmware_data_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
 * Process a TransferComplete request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_transfer_complete_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);

/**
 * Process a VerifyComplete request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_verify_complete_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);


/**
 * Process a ApplyComplete request.
 * 
 * @param interface - The command interface for PLDM FWUP
 * @param request - The request data to process. This will be updated to contain a response
 * 
 * @return 0 on success or an error code.
 * 
*/
int pldm_fwup_process_apply_complete_request(struct cmd_interface_pldm *interface, struct cmd_interface_msg *request);


/**
* Generate a ActivateFirmware request.
*
* @param interface - The command interface for PLDM FWUP
* @param buffer The buffer to contain the request data.
* @param buf_len The buffer length.
*
* @return 0 if the request was successfully generated or an error code.
*/
int pldm_fwup_generate_activate_firmware_request(struct cmd_interface_pldm *interface, 
    uint8_t *buffer, size_t buf_len);

/**
* Process a ActivateFirmware response.
*
* @param interface - The command interface for PLDM FWUP
* @param response The response data to process.
*
* @return 0 if the response was successfully processed or an error code.
*/
int pldm_fwup_process_activate_firmware_response(struct cmd_interface_pldm *interface, struct cmd_interface_msg *response);



#endif /* PLDM_FWUP_PROTOCOL_COMMANDS_H_ */