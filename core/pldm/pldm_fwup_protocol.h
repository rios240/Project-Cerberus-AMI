#ifndef PLDM_FWUP_PROTOCOL_H_
#define PLDM_FWUP_PROTOCOL_H_

#include <stdint.h>
#include "libpldm/firmware_update.h"
#include "mctp/mctp_base_protocol.h"
#include "platform_config.h"


#if !PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE 
#define PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE                                PLDM_FWUP_BASELINE_TRANSFER_SIZE
#endif

#if !PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ 
#define PLDM_FWUP_PROTOCOL_MAX_OUTSTANDING_TRANSFER_REQ                     1                     
#endif

#define PLDM_MCTP_BINDING_MSG_OVERHEAD                                      (sizeof (struct pldm_msg_hdr) + MCTP_BASE_PROTOCOL_MSG_TYPE_SIZE)
#define PLDM_MCTP_BINDING_MSG_OFFSET                                        MCTP_BASE_PROTOCOL_MSG_TYPE_SIZE

#pragma pack(push, 1)
/**
 * Information for a versioning string
 */
struct pldm_fwup_protocol_version_string {
    enum pldm_firmware_update_string_type version_str_type;                 /**< The version string format. */
    uint8_t version_str_length;                                             /**< The length of the version string. */
    uint8_t version_str[255];                                               /**< The version string. */
};
#pragma pack(pop)

/**
 * Variable context for performing a multipart transfer for GetPackageData, GetDeviceMetaData, GetMetaData. 
 */
struct pldm_fwup_protocol_multipart_transfer {
    uint32_t data_transfer_handle;                                          /**< Handle that is used to identify a transfer. */
	uint8_t transfer_op_flag;                                               /**< Operation flag that indicates if a transfer has started. */
    uint32_t next_data_transfer_handle;                                     /**< Handle used to identify the next portion of a transfer. */
	uint8_t transfer_flag;                                                  /**< Flag that indicates which part of the transfer is occurring. */
};

/**
 * Variable context for requesting firmware data.  
 */
struct pldm_fwup_protocol_request_firmware {
    uint32_t offset;                                                        /**< Offset of the firmware image. */
	uint32_t length;                                                        /**< Length of the portion of firmware image. */
};

/**
 * Single entry in the component image information section of the Firmware Update Package
 */
struct pldm_fwup_fup_component_image_entry {
    uint16_t comp_classification;                                           /**< Identifier for the function type of component image. */
	uint16_t comp_identifier;                                               /**< Identifier for distinguishing component images. */
	uint32_t comp_comparison_stamp;                                         /**< Optional value that can be used to compare levels of firmware. */
	bitfield16_t comp_options;                                              /**< Component options. */
	bitfield16_t requested_comp_activation_method;                          /**< Component activation methods the UA should use. */   
	uint32_t comp_size;                                                     /** Size of the component. */
	struct pldm_fwup_protocol_version_string comp_ver;                      /**< Component version. */
};

/**
 * Single entry in the component table.
 * 
 * @note This is different than the component parameter table that resides natively on the FD. 
 *  The FD will receive the component table from the UA. The UA generates the component table via 
 *  the component image information in the Firmware Update Package (FUP) except for the component classification index
 *  from which the UA receives from the FD via GetFirmwareParameters. 
 */
struct pldm_fwup_protocol_component_entry {
    uint16_t comp_classification;                                           /**< Identifier for the function type of component image. */
	uint16_t comp_identifier;                                               /**< Identifier for distinguishing component images. */
	uint8_t comp_classification_index;                                      /**< Value to indicate which firmware component this information is applicable to. */
	uint32_t comp_comparison_stamp;                                         /**< Optional value that can be used to compare levels of firmware. */
	struct pldm_fwup_protocol_version_string comp_ver;                      /**< String describing the component image version. */
};

#pragma pack(push, 1)
/**
 * Single entry in the component parameter table.
 */
struct pldm_fwup_protocol_component_parameter_entry {
    uint16_t comp_classification;                                           /**< Identifier for the function type of component image. */
	uint16_t comp_identifier;                                               /**< Identifier for distinguishing component images. */
	uint8_t comp_classification_index;                                      /**< Value to indicate which firmware component this information is applicable to. */
	uint32_t active_comp_comparison_stamp;                                  /**< Optional value that can be used to compare levels of active firmware. */
	uint8_t active_comp_release_date[8];                                    /**< Release date of the active component. */
    uint32_t pending_comp_comparison_stamp;                                 /**< Optional value that can be used to compare levels of pending firmware. */
	uint8_t pending_comp_release_date[8];                                   /**< Release date of the pending component. */
	bitfield16_t comp_activation_methods;                                   /**< Activation methods of the component. */
	bitfield32_t capabilities_during_update;                                /**< Capabilities of the component during an update. */
    struct pldm_fwup_protocol_version_string active_comp_ver;               /**< Active component version. */
    struct pldm_fwup_protocol_version_string pending_comp_ver;              /**< Pending component version. */
};
#pragma pack(pop)

/**
 * The firmware components details for a single FD.
 * 
 * @note The firmware parameters are inline with the GetFirmwareParameters command that the UA 
 *  issues to get component details from the FD. The instantiation and population 
 *  is implementation specific, i.e. the firmware parameters could come from the PFM, PCD, CFM, etc.
 */
struct pldm_fwup_protocol_firmware_parameters {
    bitfield32_t capabilities_during_update;                                /**< The capabilities of the FD during an update. */
	uint16_t count;                                                         /**< The number firmware components. */
	struct pldm_fwup_protocol_version_string active_comp_img_set_ver;       /**< The active component image set version. */
    struct pldm_fwup_protocol_version_string pending_comp_img_set_ver;      /**< The pending component image set version. */
    struct pldm_fwup_protocol_component_parameter_entry *entries;           /**< The component parameter table. */
};


#define	CMD_HANDLER_PLDM_ERROR(code)										ROT_ERROR (ROT_MODULE_CMD_HANDLER_PLDM, code)

/**
 * Error codes that can be generated by the command handler 
 */
enum {
    CMD_HANDLER_PLDM_INVALID_ARGUMENT = CMD_HANDLER_PLDM_ERROR (0x00),      /**< Input parameter is null or not valid. */
    CMD_HANDLER_PLDM_PAYLOAD_TOO_SHORT = CMD_HANDLER_PLDM_ERROR (0x01),     /**< The message does not contain the minimum amount of data. */
    CMD_HANDLER_PLDM_UNKNOWN_REQUEST = CMD_HANDLER_PLDM_ERROR (0x02),	    /**< A command does not represent a known request. */
    CMD_HANDLER_PLDM_UNKNOWN_RESPONSE = CMD_HANDLER_PLDM_ERROR (0x03),	    /**< A command does not represent a known response. */
    CMD_HANDLER_PLDM_UNSUPPORTED_OPERATION = CMD_HANDLER_PLDM_ERROR (0x04),	/**< The requested operation is not supported. */
    CMD_HANDLER_PLDM_OPERATION_NOT_EXPECTED = CMD_HANDLER_PLDM_ERROR (0x05) /**< The requested operation was not expected. */
};

#endif /* PLDM_FWUP_PROTOCOL_H_ */