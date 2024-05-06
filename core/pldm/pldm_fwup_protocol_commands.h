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

//#ifdef PLDM_FWUP_FD_ENABLE

int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request);
int pldm_fwup_prcocess_get_firmware_parameters_request(struct pldm_fwup_state *fwup_state,
    struct device_manager *device_manager, struct cmd_interface_msg *request);
int pldm_fwup_process_request_update_request(struct cmd_interface_pldm *cmd_pldm, struct cmd_interface_msg *request);

    
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_multipart_transfer *multipart_transfer, 
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_multipart_transfer *multipart_transfer,
    const struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *response);

//#elif defined(PLDM_FWUP_UA_ENABLE)

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

//#endif


#endif /* PLDM_FWUP_PROTOCOL_COMMANDS_H_ */