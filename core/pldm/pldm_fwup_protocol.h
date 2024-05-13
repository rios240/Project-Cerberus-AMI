#ifndef PLDM_FWUP_PROTOCOL_H_
#define PLDM_FWUP_PROTOCOL_H_

#include <stdint.h>
#include "libpldm/firmware_update.h"
#include "mctp/mctp_base_protocol.h"
#include "platform_config.h"


#ifndef PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE && !PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
#define PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE                                MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN
#endif

#ifndef PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ && !PLDM_FWUP_ENABLE_FIRMWARE_DEVICE
#define PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ                     1                     
#endif

#define PLDM_MCTP_BINDING_MSG_OVERHEAD                                      (sizeof (struct pldm_msg_hdr) + MCTP_BASE_PROTOCOL_MSG_TYPE_SIZE)
#define PLDM_MCTP_BINDING_MSG_OFFSET                                        MCTP_BASE_PROTOCOL_MSG_TYPE_SIZE

/**
 * Contains the full set of parameters for a single FW component 
 */
struct pldm_fwup_protocol_full_component_parameter_entry {
    struct pldm_component_parameter_entry request;                          /**< Requested component fixed information. */
    uint8_t active_comp_ver_str[255];                                       /**< Firmware component version which is currently active. */
    uint8_t pending_comp_ver_str[255];                                      /**< Firmware component version which is pending activation. */

};

/**
 * Parameters for a device's firmware 
 */
struct pldm_fwup_protocol_firmware_parameters {
    bitfield32_t capabilities_during_update;                                /**< 32 bit field specifying the capability of the firmware device. */
	uint16_t comp_count;                                                    /**< Number of firmware components that reside within the FD. */
	uint8_t active_comp_img_set_ver_str_type;                               /**< The type of string format used for the active component image set version information. */
	uint8_t active_comp_img_set_ver_str_len;                                /**< The length of the active component image set version information. */
	uint8_t pending_comp_img_set_ver_str_type;                              /**< The type of string format used for the pending component image set version information. */
	uint8_t pending_comp_img_set_ver_str_len;                               /**< The length of the pending component image set version information. */
};


/**
 * Full set of parameters for a single device's firmware
 */
struct pldm_fwup_protocol_full_firmware_parameters {
    struct pldm_fwup_protocol_firmware_parameters request;                  /**< Requested FW parameters fixed information. */
    uint8_t active_comp_img_set_ver_str[255];                               /**< Version of the set of component images that are currently active. */
    uint8_t pending_comp_img_set_ver_str[255];                              /**< Component image set version which is pending activation. */
    struct pldm_fwup_protocol_full_component_parameter_entry *entries;      /**< Table of component entries for all of the updateable components that reside on the FD. */

};

#endif /* PLDM_FWUP_PROTOCOL_H_ */