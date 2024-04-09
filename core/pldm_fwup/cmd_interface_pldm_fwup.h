#ifndef CMD_INTERFACE_PLDM_FWUP_H_
#define CMD_INTERFACE_PLDM_FWUP_H_


#include <stdint.h>
#include "platform_config.h"
#include "common/observable.h"
#include "cmd_interface/cmd_interface.h"
#include "flash/flash_virtual_disk.h"
#include "firmware/firmware_update_control.h"


#define FWUP_BASELINE_TRANSFER_SIZE 32

/**
 * The flash addresses and devices to use for different PLDM FWUP regions.
 */
struct pldm_fwup_flash_map {
#ifdef PLDM_FWUP_FD_ENABLE
    const struct flash *package_data;                                   /**< The flash device that contains the PackageData. */
    uint32_t package_data_addr;                                         /**< The base address of the PackageData region. */
    size_t package_data_size;                                           /**< The size of the PackageData region. */
#elif defined(PLDM_FWUP_UA_ENABLE)
    const struct flash *device_meta_data;                                /**< The flash device that contains the DeviceMetaData. */
    uint32_t device_meta_data_addr;                                     /**< The base address of the DeviceMetaData region. */
    size_t device_meta_data_size;                                       /**< The size of the DeviceMetaData region. */
    const struct flash *firmware_update_package;                        /**< The flash device that contains the Firmware Update Package. */
    uint32_t firmware_update_package_addr;                              /**< The base address of the Firmware Update Package region. */
    size_t firmware_update_package_size;                                /**< The size of the Firmware Update Package region. */
#endif
};


/**
 * Multipart transfer context for PLDM Get commands
*/
struct pldm_fwup_multipart_transfer_handler {
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
    uint8_t state;                                                      /**< Current PLDM FWUP state. */
    uint8_t command;                                                    /**< Last PLDM FWUP command. */
    uint8_t completion_code;                                            /**< Last PLDM completion code. */
};

/**
 * Command interface for processing received PLDM FWUP protocol requests.
 */
struct cmd_interface_pldm_fwup {
    struct cmd_interface base;                                          /**< Base command interface */
    struct pldm_fwup_multipart_transfer_handler multipart_transfer;    /**< Handler for multipart transfer for Get commands. */    
	const struct pldm_fwup_flash_map *flash_map;                        /**< The flash address mapping to use for PLDM FWUP. */
    const struct firmware_update_control *control;		                /**< FW update control instance */
#ifdef PLDM_FWUP_FD_ENABLE
    struct pldm_fwup_state *state;                                      /**< Variable context for the cmd interface instance. */
#endif


    /**
     * Generate PLDM FWUP request for use in mctp_interface_issue_request
     * 
     * @param intf The PLDM FWUP control command interface instance
     * @param command The PLDM FWUP command
     * @param buffer The buffer to store the PLDM message
     * @param buf_len The length of the buffer
     * 
     * @return size of the request or pldm_completion_codes
    */
    int (*generate_request) (struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len);
};


int cmd_interface_pldm_fwup_init (struct cmd_interface_pldm_fwup *intf, struct pldm_fwup_flash_map *flash_map,
    struct pldm_fwup_state *state_ptr, const struct firmware_update_control *control, uint8_t init_state);
void cmd_interface_pldm_fwup_deinit (struct cmd_interface_pldm_fwup *intf);


#endif /* CMD_INTERFACE_PLDM_FWUP_H_ */