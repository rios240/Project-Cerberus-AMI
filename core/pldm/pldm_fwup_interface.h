#ifndef PLDM_FWUP_INTERFACE_H_
#define PLDM_FWUP_INTERFACE_H_

#include <stdint.h>
#include <stdbool.h>
#include "flash/flash.h"
#include "cmd_interface/device_manager.h"
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/cmd_interface.h"
#include "firmware/firmware_update.h"
#include "mctp/mctp_interface.h"
#include "cmd_interface_pldm.h"
#include "libpldm/base.h"
#include "libpldm/firmware_update.h"


/**
 * The flash addresses and devices to use for different pldm fwup regions.
 */
struct pldm_fwup_flash_map {
    const struct flash *package_data_flash;                                     /**< The flash device that contains the packaged data obtained from the FUP. */
    uint32_t package_data_addr;                                                 /**< The base address of the packaged data region. */
    size_t package_data_size;                                                   /**< The size of the packaged data region. */
    const struct flash *device_meta_data_flash;                                 /**< The flash device that contains the device meta data obtained from the FD. */
    uint32_t device_meta_data_addr;                                             /**< The base address of the device meta data region. */
    size_t device_meta_data_size;                                               /**< The size of the device meta data region. */
#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
    struct firmware_flash_map *firmware_flash;                                  /**< The Cerberus firmware flash map to be used by the FD to store FW images. */
#else
    const struct flash *comp_images_flash;                                      /**< The flash device that contains the component images obtained from the FUP. */
    uint32_t comp_images_addr;                                                  /**< The base address of the component images. */
    size_t comp_images_size;                                                    /**< The size of the component images region. */
#endif

};

/**
 * Multipart transfer context for PLDM GET commands
 */
struct pldm_fwup_multipart_transfer {
#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
    uint32_t data_transfer_handle;                                              /**< Handle that is used to identify a package data transfer. */
    uint8_t transfer_op_flag;                                                   /**< Operation flag that indiates whether this is the start of the transfer. */
#else
    uint8_t transfer_flag;                                                      /**< Transfer flag that indiates what part of the transfer this response represents. */
    uint32_t next_data_transfer_handle;                                         /**< Handle that is used to identify the next portion of the transfer. */
#endif
};

/**
 * Variable context for the firmware device being updated
 */
struct pldm_fwup_updating_device_context {
    uint16_t num_of_components;                                                 /**< The number of components for this firmware. */
    uint8_t comp_img_set_ver_str_type;                                          /**< The string format used for the component image set version information. */
    uint8_t comp_img_set_ver_str_len;                                           /**< The length of the component image set version information. */
    uint8_t comp_img_set_set_str[255];                                          /**< The component image set version. */
};

/**
 * Variable context for a PLDM FWUP protocol command interface.
 */
struct pldm_fwup_state {
#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
    uint8_t state;                                                              /**< Current state of operation. */
    uint8_t previous_state;                                                     /**< The previous state of operation. */
    bool update_mode;                                                           /**< Indication if the FD is in update mode. */
    uint32_t max_transfer_size;                                                 /**< Maximum size of the multipart payloads allowed. */
    uint8_t max_num_outstanding_transfer_req;                                   /**< Number of outstanding RequestFirmwareData commands. */
#else
    bool get_pkg_data_cmd;                                                      /**< Indication if the FD will send GetPackageData. */
#endif
    uint8_t command;                                                            /**< The PLDM command currently being processed. */
    uint8_t previous_command;                                                   /**< The previous command that was processed. */
    uint8_t completion_code;                                                    /**< The completion code generated as of the lastest command being processed. */
    struct pldm_fwup_updating_device_context updating_device;                   /**< The firmware in the process of being updated. */
    struct pldm_fwup_multipart_transfer multipart_transfer;                     /**< Context for multipart transfer of GET commands. */
};


int pldm_fwup_interface_generate_request(struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len);

//int pldm_fwup_run_update(struct mctp_interface *mctp, struct cmd_channel *channel, uint8_t inventory_cmds, uint8_t device_eid, int ms_timeout);









#endif /* PLDM_FWUP_INTERFACE_H_ */
