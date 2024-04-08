#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "pldm_fwup_commands.h"

#ifdef PLDM_FWUP_FD_ENABLE
#include "libpldm/firmware_update.h"
#include "libpldm/utils.h"
#elif PLDM_FWUP_UA_ENABLE
#include "firmware_update.h"
#include "utils.h"
#endif


/*
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_flash_map *flash_map, uint8_t *buffer, size_t buf_len)
{
    struct pldm_multipart_transfer_req req_data;
    

}
*/
