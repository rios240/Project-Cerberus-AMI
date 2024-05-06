#ifndef CMD_INTERFACE_PLDM_H_
#define CMD_INTERFACE_PLDM_H_


#include <stdint.h>
#include <stdbool.h>
#include "platform_config.h"
#include "cmd_interface/cmd_interface.h"
#include "flash/flash.h"
#include "cmd_interface/device_manager.h"
#include "firmware/firmware_update.h"
#include "libpldm/base.h"
#include "libpldm/firmware_update.h"

/**
 * The flash addresses and devices to use for different PLDM FWUP regions.
 */
struct pldm_fwup_flash_map {
//#ifdef PLDM_FWUP_FD_ENABLE
    const struct flash *package_data_flash;                                     /**< The flash device that contains the PackageData. */
    uint32_t package_data_addr;                                                 /**< The base address of the PackageData region. */
    size_t package_data_size;                                                   /**< The size of the PackageData region. */
    struct firmware_flash_map *firmware_flash;                                  /**< The Cerberus firmware flash map containing active, backup, staging, etc. regions. */
//#elif defined(PLDM_FWUP_UA_ENABLE)
    const struct flash *device_meta_data_flash;                                 /**< The flash device that contains the DeviceMetaData. */
    uint32_t device_meta_data_addr;                                             /**< The base address of the DeviceMetaData region. */
    size_t device_meta_data_size;                                               /**< The size of the DeviceMetaData region. */
    const struct flash *fw_update_package_flash;                                /**< The flash device that contains the Firmware Update Package. */
    uint32_t fw_update_package_addr;                                            /**< The base address of the Firmware Update Package region. */
    size_t fw_update_package_size;                                              /**< The size of the Firmware Update Package region. */
//#endif
};


/**
 * Multipart transfer context for PLDM Get commands
*/
struct pldm_fwup_multipart_transfer {
//#ifdef PLDM_FWUP_FD_ENABLE
    uint32_t transfer_handle;                                                   /**< Handle that is used to identify a package data transfer. */
    uint8_t transfer_op_flag;                                                   /**< Operation flag that indiates whether this is the start of the transfer. */
//#elif defined(PLDM_FWUP_UA_ENABLE)
    uint8_t transfer_flag;                                                      /**< Transfer flag that indiates what part of the transfer this response represents. */
//#endif
};

/**
 * Variable context for a cmd interface PLDM FWUP.
 */
struct pldm_fwup_state {
//#ifdef PLDM_FWUP_FD_ENABLE
    uint8_t state;                                                              /**< Current PLDM FWUP state. */
    uint8_t update_mode:1;                                                      /**< Flag for update mode. */
//#endif
    uint8_t previous_cmd;                                                       /**< The previous PLDM FWUP command that was processed/generated. */
    uint8_t previous_completion_code;                                           /**< The previous PLDM completion code. */
    struct pldm_fwup_multipart_transfer fwup_multipart_transfer;                /**< Context for multipart transfer of Get commands. */
};

/**
 * Command interface for processing received PLDM FWUP protocol requests.
 */
struct cmd_interface_pldm {
    struct cmd_interface base;                                                  /**< Base command interface. */
	const struct pldm_fwup_flash_map *fwup_flash;                               /**< The flash address mapping to use for PLDM FWUP. */
    struct pldm_fwup_state *fwup_state;                                         /**< Variable context for the cmd interface instance. */
    struct device_manager *device_manager;                                      /**< Device instance. */
//#ifdef PLDM_FWUP_FD_ENABLE
    uint8_t num_components;                                                     /**< The number of components the UA will transfer. */
    uint16_t package_data_len;                                                  /**< The length of the package data the UA will transfer. */
    uint32_t max_transfer_size;                                                 /**< The size of the vaiable payload allowed to be requested by the FD. */
    uint8_t max_outstanding_transfer_requests;                                  /**< The number of transfer requests allowed to be sent by the FD. */
    enum pldm_firmware_update_string_type comp_img_set_ver_str_type;            /**< The component image set version string type. */
    uint8_t comp_img_set_ver_str[255];                                          /**< The component image set version string the UA will transfer. */
    uint8_t comp_img_set_ver_str_len;                                           /**< The component image set version string length. */
//#elif defined(PLDM_FWUP_UA_ENABLE)
    uint16_t fw_device_meta_data_len;                                           /**< The length of the meta data the FD will transfer. */
    uint8_t fd_will_send_pkg_data_cmd:1;                                        /**< If set the FD will send the GetPackageData command. */
    int device_eid;                                                             /**< The device that is currently being updated. */
//#endif
};


int cmd_interface_pldm_init (struct cmd_interface_pldm *intf, 
    struct pldm_fwup_flash_map *fwup_flash_ptr, struct pldm_fwup_state *fwup_state_ptr, struct device_manager *device_manager_ptr, uint8_t device_eid);
void cmd_interface_pldm_deinit (struct cmd_interface_pldm *intf);
int cmd_interface_pldm_generate_request(struct cmd_interface *intf, uint8_t pldm_command, uint8_t *buffer, size_t buf_len);


#endif /* CMD_INTERFACE_PLDM_H_ */