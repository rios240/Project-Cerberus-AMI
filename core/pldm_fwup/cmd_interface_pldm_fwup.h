#ifndef CMD_INTERFACE_PLDM_FWUP_H_
#define CMD_INTERFACE_PLDM_FWUP_H_


#include <stdint.h>
#include "platform_config.h"
#include "common/observable.h"
#include "cmd_interface/cmd_interface.h"
#include "flash/flash_virtual_disk.h"
#include "firmware/firmware_update_control.h"

//#define PLDM_FWUP_UA_ENABLE

/**
 * The flash addresses and devices to use for different PLDM FWUP regions.
 */
struct pldm_fwup_flash_map {
#ifdef PLDM_FWUP_FD_ENABLE
    const struct flash *pkg_data;                                       /**< The flash device that contains the PackageData. */
    uint32_t pkg_data_addr;                                             /**< The base address of the PackageData region. */
    size_t pkg_data_size;                                               /**< The size of the PackageData region. */
#elif defined(PLDM_FWUP_UA_ENABLE)
    const struct flash *device_meta_data;                               /**< The flash device that contains the DeviceMetaData. */
    uint32_t device_meta_data_addr;                                     /**< The base address of the DeviceMetaData region. */
    size_t device_meta_data_size;                                       /**< The size of the DeviceMetaData region. */
    const struct flash *fup;                                            /**< The flash device that contains the Firmware Update Package. */
    uint32_t fup_addr;                                                  /**< The base address of the Firmware Update Package region. */
    size_t fup_size;                                                    /**< The size of the Firmware Update Package region. */
#endif
};


/**
 * Multipart transfer context for PLDM Get commands
*/
struct pldm_fwup_multipart_transfer_context {
#ifdef PLDM_FWUP_FD_ENABLE
    uint32_t transfer_handle;                                           /**< Handle that is used to identify a package data transfer. */
    uint8_t transfer_op_flag;                                           /**< Operation flag that indiates whether this is the start of the transfer. */
#elif defined(PLDM_FWUP_UA_ENABLE)
    uint8_t transfer_flag;                                              /**< Transfer flag that indiates what part of the transfer this response represents. */
#endif
};

/**
 * Variable context for a cmd interface PLDM FWUP.
 */
struct pldm_fwup_state {
#ifdef PLDM_FWUP_FD_ENABLE
    uint8_t fwup_state;                                                 /**< Current PLDM FWUP state. */
    bool update_mode;                                                   /**< Flag for update mode. */
#endif
    uint8_t previous_cmd;                                               /**< The previous PLDM FWUP command that was processed/generated. */
    uint8_t previous_completion_code;                                   /**< The previous PLDM completion code. */
};

/**
 * Command interface for processing received PLDM FWUP protocol requests.
 */
struct cmd_interface_pldm_fwup {
    struct cmd_interface base;                                          /**< Base command interface */
#ifdef PLDM_FWUP_FD_ENABLE
    const struct firmware_update_control *control;		                /**< FW update control instance */
#endif
    struct pldm_fwup_multipart_transfer_context *multipart_transfer;    /**< Context for multipart transfer of Get commands. */    
	const struct pldm_fwup_flash_map *flash_map;                        /**< The flash address mapping to use for PLDM FWUP. */
    struct pldm_fwup_state *state;                                      /**< Variable context for the cmd interface instance. */
};


int cmd_interface_pldm_fwup_init (struct cmd_interface_pldm_fwup *intf, 
    struct pldm_fwup_flash_map *flash_map, struct pldm_fwup_state *state_ptr, 
    const struct firmware_update_control *control, struct pldm_fwup_multipart_transfer_context *multipart_transfer_ptr);
void cmd_interface_pldm_fwup_deinit (struct cmd_interface_pldm_fwup *intf);
int generate_request(struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len);


#endif /* CMD_INTERFACE_PLDM_FWUP_H_ */