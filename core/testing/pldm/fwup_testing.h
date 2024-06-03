#ifndef FWUP_TESTING_H_
#define FWUP_TESTING_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "pldm/pldm_fwup_manager.h"
#include "pldm/pldm_fwup_protocol.h"
#include "flash/flash_virtual_disk.h"
#include "cmd_interface/device_manager.h"
#include "cmd_interface/cmd_interface.h"
#include "mctp/mctp_interface.h"
#include "pldm/cmd_interface_pldm.h"
#include "testing.h"


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

#define PLDM_FWUP_ACTIVE_FIRMWARE_COMP_VER              "firmware_v1.0"
#define PLDM_FWUP_ACTIVE_FIRMWARE_COMP_VER_LEN          (sizeof (PLDM_FWUP_ACTIVE_FIRMWARE_COMP_VER) - 1)
#define PLDM_FWUP_PENDING_FIRMWARE_COMP_VER             "firmware_v2.0"
#define PLDM_FWUP_PENDING_FIRMWARE_COMP_VER_LEN         (sizeof (PLDM_FWUP_PENDING_FIRMWARE_COMP_VER) - 1)

#define PLDM_FWUP_ACTIVE_MIDDLEWARE_COMP_VER            "middleware_v1.0"
#define PLDM_FWUP_ACTIVE_MIDDLEWARE_COMP_VER_LEN        (sizeof (PLDM_FWUP_ACTIVE_MIDDLEWARE_COMP_VER) - 1)
#define PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER           "middleware_v2.0"
#define PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER_LEN       (sizeof (PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER) - 1)

#define PLDM_FWUP_ACTIVE_COMP_IMG_SET_VER               "cerberus_v1.0"
#define PLDM_FWUP_ACTIVE_COMP_IMG_SET_VER_LEN           (sizeof (PLDM_FWUP_ACTIVE_COMP_IMG_SET_VER) - 1)
#define PLDM_FWUP_PENDING_COMP_IMG_SET_VER              "cerberus_v2.0"
#define PLDM_FWUP_PENDING_COMP_IMG_SET_VER_LEN          (sizeof (PLDM_FWUP_PENDING_COMP_IMG_SET_VER) - 1)

extern const struct pldm_fwup_protocol_component_parameter_entry PLDM_FWUP_PARAMETER_ENTRIES[];
extern const struct pldm_fwup_protocol_firmware_parameters PLDM_FWUP_FD_FIRMWARE_PARAMETERS;
extern const struct pldm_fwup_fup_component_image_entry PLDM_FWUP_UA_FUP_COMP_INFO_LIST[];
extern const struct pldm_fwup_protocol_version_string PLDM_FWUP_UA_FUP_COMP_IMG_SET_VER;
extern const struct flash_region PLDM_FWUP_COMP_REGIONS[];
extern const struct pldm_fwup_flash_manager PLDM_FWUP_UA_FLASH_MANAGER;
extern const struct pldm_fwup_flash_manager PLDM_FWUP_FD_FLASH_MANAGER;

struct pldm_fwup_protocol_testing_ctx {
    struct pldm_fwup_protocol_firmware_parameters fd_fw_parameters;
    struct pldm_fwup_fup_component_image_entry *fup_comp_img_list;
    struct pldm_fwup_flash_manager fd_flash_mgr;
    struct pldm_fwup_flash_manager ua_flash_mgr;
    struct pldm_fwup_protocol_version_string fup_comp_img_set_ver;
    struct flash_virtual_disk flash;
    struct flash_virtual_disk_state flash_state;
    struct cmd_interface cmd_cerberus;
	struct cmd_interface cmd_mctp;
    struct cmd_interface cmd_spdm;
};

struct pldm_fwup_protocol_flash_ctx {
    struct flash_virtual_disk ua_flash;
    struct flash_virtual_disk_state ua_flash_state;
    struct flash_virtual_disk fd_flash;
    struct flash_virtual_disk_state fd_flash_state;
};

struct pldm_fwup_protocol_commands_testing {
    struct mctp_interface mctp;
    struct cmd_interface_pldm pldm;
    struct device_manager device_mgr;
    struct pldm_fwup_manager fwup_mgr;
    struct cmd_channel channel;
    int timeout_ms;
    uint8_t req_buffer[MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN];
};

void setup_flash_ctx(struct pldm_fwup_protocol_flash_ctx *flash_ctx, CuTest *test);
void release_flash_ctx(struct pldm_fwup_protocol_flash_ctx *flash_ctx);
void setup_testing_ctx(struct pldm_fwup_protocol_testing_ctx *testing_ctx, struct pldm_fwup_protocol_flash_ctx *flash_ctx) ;
void release_testing_ctx(struct pldm_fwup_protocol_testing_ctx *testing_ctx);
void setup_ua_device_manager(struct device_manager *device_mgr, CuTest *test);
void setup_fd_device_manager(struct device_manager *device_mgr, CuTest *test);
void release_device_manager(struct device_manager *device_mgr);
void setup_testing(struct pldm_fwup_protocol_commands_testing *testing, struct pldm_fwup_protocol_testing_ctx *testing_ctx, CuTest *test);
void release_testing(struct pldm_fwup_protocol_commands_testing *testing);
int receive_and_respond_full_mctp_message(struct cmd_channel *channel, struct mctp_interface *mctp, int timeout_ms);
int send_and_receive_full_mctp_message(struct pldm_fwup_protocol_commands_testing *testing, int command);


#endif /* FWUP_TESTING_H_ */