#ifndef FWUP_TESTING_H_
#define FWUP_TESTING_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "testing.h"
#include "pldm/pldm_fwup_manager.h"
#include "pldm/pldm_fwup_protocol.h"


#define PLDM_FWUP_FLASH_UA                              "../core/testing/pldm/flash_virtual_disk/ua_flash.bin"
#define PLDM_FWUP_FLASH_FD                              "../core/testing/pldm/flash_virtual_disk/fd_flash.bin"
#define PLDM_FWUP_FLASH_SIZE                            5242880

#define PLDM_FWUP_FLASH_MANAGER_PKG_DATA_ADDR           0
#define PLDM_FWUP_FLASH_MANAGER_META_DATA_ADDR          1048576   
#define PLDM_FWUP_FLASH_MANAGER_COMP_ONE_ADDR           2097152
#define PLDM_FWUP_FLASH_MANAGER_COMP_TWO_ADDR           3145728
#define PLDM_FWUP_FLASH_MANAGER_REGION_SIZE             1048576

#define PLDM_FWUP_NUM_COMPONENTS                        2

#define PLDM_FWUP_COMP_SIZE                             5120
#define PLDM_FWUP_PKG_DATA_SIZE                         5120
#define PLDM_FWUP_META_DATA_SIZE                        5120

#define PLDM_FWUP_FD_EID                                0x40
#define PLDM_FWUP_FD_SMBUS_ADDR                         0x4E
#define PLDM_FWUP_UA_EID                                0x60
#define PLDM_FWUP_UA_SMBUS_ADDR                         0x6E                                

extern const struct pldm_fwup_protocol_firmware_parameters PLDM_FWUP_FD_FIRMWARE_PARAMETERS;
extern const struct pldm_fwup_fup_component_image_entry PLDM_FWUP_UA_FUP_COMP_INFO_LIST[];
extern const struct pldm_fwup_protocol_version_string PLDM_FWUP_UA_FUP_COMP_IMG_SET_VER;
extern const struct pldm_fwup_flash_manager PLDM_FWUP_UA_FLASH_MANAGER;
extern const struct pldm_fwup_flash_manager PLDM_FWUP_FD_FLASH_MANAGER;

#endif /* FWUP_TESTING_H_ */