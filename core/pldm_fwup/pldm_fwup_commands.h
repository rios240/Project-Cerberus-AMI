#ifndef PLDM_FWUP_COMMANDS_H_
#define PLDM_FWUP_COMMANDS_H_


#include <stdint.h>
#include "cmd_interface_pldm_fwup.h"
#include "cmd_interface/cmd_interface.h"


#ifdef PLDM_FWUP_FD_ENABLE
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_multipart_transfer_handler *multipart_transfer, 
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_multipart_transfer_handler *multipart_transfer,
    struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *response);
#elif defined(PLDM_FWUP_UA_ENABLE)
int pldm_fwup_process_get_package_data_request(struct pldm_fwup_flash_map *flash_map, struct cmd_interface_msg *request);
#endif



#endif /* PLDM_FWUP_COMMANDS_H_ */