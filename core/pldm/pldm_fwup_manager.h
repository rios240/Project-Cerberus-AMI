#ifndef PLDM_FWUP_MANAGER_H_
#define PLDM_FWUP_MANAGER_H_

#include "pldm_fwup_protocol.h"
#include "flash/flash_updater.h"
#include "flash/flash_util.h"
#include "status/rot_status.h"


/**
 * The flash manager for a PLDM-based firmware update.
 * 
 * @note Depending whether the flash manager belongs to the UA or FD some of the flash regions will be initialized during subsequent commands.  
 */
struct pldm_fwup_flash_manager {
    struct flash *flash;                                                            /**< The flash device that will contain the following flash regions. */
    struct flash_region package_data_region;                                        /**< Flash region to write package data to. */
    size_t package_data_size;                                                       /**< Size of the package data. */
    struct flash_region device_meta_data_region;                                    /**< Flash region that contains the FD meta data. */
    size_t device_meta_data_size;                                                   /**< Size of the FD meta data. */
    struct flash_region *comp_regions;                                              /**< The flash regions to write firmware images to. */
};

/**
 * Variable status of the FD that the UA receives during a GetStatus request.
 * 
 */
struct pldm_fwup_fd_status {
	uint8_t current_state;
	uint8_t previous_state;
	uint8_t aux_state;
	uint8_t aux_state_status;
	uint8_t progress_percent;
	uint8_t reason_code;
	uint32_t update_option_flags_enabled;
};

/**
 * Variable context that the UA needs to retain from the FD during a firmware update. 
 * 
 * @note The component flash regions will be equal to the number of components in the component image list.
 * The current component number will be used as an index to both lists. 
 */
struct pldm_fwup_ua_update_info {
    uint16_t fd_meta_data_len;                                                      /**< Length of the FD meta data the UA needs to retain. */
    bool8_t fd_will_send_pkg_data_cmd;                                              /**< Indicates that the FD will send a GetPackageData command. */
    enum pldm_firmware_update_transfer_result_values transfer_result;               /**< The result of the firmware transfer indicated by the FD. */
    enum pldm_firmware_update_verify_result_values verify_result;                   /**< The result of the verification the FD performs on the firmware image. */
    enum pldm_firmware_update_apply_result_values apply_result;                     /**< The result the FD returns during the apply state. */
    uint16_t comp_activation_methods_modification;                                  /**< Component activation methods modified by the FD.*/
    uint16_t estimated_time_activation;                                             /**< The time the FD needs to activate all firmware. */
    struct pldm_fwup_fd_status fd_status;                                           /**< Status the FD returns after receiving a GetStatus command. */
};

/**
 * Variable context for the UA manager
*/
struct pldm_fwup_ua_state {
    enum pldm_firmware_update_commands previous_cmd;                                /**< Previous FWUP command. */
    enum pldm_firmware_update_completion_codes previous_completion_code;            /**< Last received completion code. */
};

/**
 * Module that is used to manager the Update Agent (UA) during a PLDM-based firmware update.
 */
struct pldm_fwup_ua_manager {
    struct pldm_fwup_protocol_firmware_parameters rec_fw_parameters;                /**< The firmware parameters received from the FD. */
    struct pldm_fwup_ua_state state;                                                /**< Variable context for the UA manager. */
    struct pldm_fwup_flash_manager *flash_mgr;                                      /**< Flash manager for a firmware update. */
    struct pldm_fwup_protocol_multipart_transfer get_cmd_state;                     /**< Variable context for the three Get commands. */
    struct pldm_fwup_ua_update_info update_info;                                    /**< Information retained during the firmware update. */
    struct pldm_fwup_protocol_version_string *fup_comp_img_set_ver;                 /**< The component image set version from the FUP. */
    uint16_t num_components;                                                        /**< The number of firmware components the UA has to transfer. */
    uint16_t current_comp_num;                                                      /**< The component that is currently being updated. */
    struct pldm_fwup_fup_component_image_entry *comp_img_entries;                   /**< The component image information list sent to the FD. */

};

/**
 * Variable context for the FD manager
*/
struct pldm_fwup_fd_state {
    enum pldm_firmware_device_states current_state;                                 /**< Current state machine state. */
    enum pldm_firmware_device_states previous_state;                                /**< Previous state machine state. */
    bool8_t update_mode;                                                            /**< Flag indicating if the FD is in update mode. */
    enum pldm_firmware_update_commands previous_cmd;                                /**< Previous FWUP command. */
    enum pldm_firmware_update_completion_codes previous_completion_code;            /**< Previous completion code. */
};

/**
 * Variable context that the FD needs to retain from the UA during a firmware update. 
 * 
 * @note The component flash regions will be equal to the number of components in the component table.
 * The current component number will be used as an index to both lists. 
 */
struct pldm_fwup_fd_update_info {
    uint32_t max_transfer_size;                                                     /**< Maximum data size that the FD can request from the UA. */
    uint8_t max_outstanding_transfer_req;                                           /**< Maximum number of requests the FD can issue to the UA. */
    uint16_t num_components;                                                        /**< The number of firmware components the FD should receive. */
    uint16_t package_data_len;                                                      /**< The length of the package data the FD needs to retain. */
    struct pldm_fwup_protocol_version_string comp_img_set_ver;                      /**< The component image set version being updated. */
    uint16_t current_comp_num;                                                      /**< The component in the component table that is currently being updated. */
    uint32_t current_comp_img_size;                                                 /**< The size of the component image being updated. */
    uint32_t current_comp_img_offset;                                               /**< The current offset of the requested component image. */
    uint8_t comp_transfer_flag;                                                     /**< Flag for the transfer of components for the component table. */
	bitfield32_t current_comp_update_option_flags;                                  /**< The update options requested by the UA for the component being updated. */
    bool8_t self_contained_activation_req;                                          /**< The activation method requested by the UA. */
    struct pldm_fwup_protocol_component_entry *comp_entries;                        /**< The component table received from the UA. */
};

/**
 * Module that is used to manager the Firmware Device (FD) during a PLDM-based firmware update.
 */
struct pldm_fwup_fd_manager {
    struct pldm_fwup_protocol_firmware_parameters *fw_parameters;                   /**< The firmware parameters requested by the UA. */
    struct pldm_fwup_fd_state state;                                                /**< Variable context for the FD manager. */
    struct pldm_fwup_protocol_multipart_transfer get_cmd_state;                     /**< Variable context for the three Get commands. */
    struct pldm_fwup_flash_manager *flash_mgr;                                      /**< Flash manager for a firmware update. */
    struct pldm_fwup_fd_update_info update_info;                                    /**< Information retained during the firmware update. */
};


/**
 * Module that is used by the PLDM command interface and PLDM Firmware Update (FWUP) interface to manage a PLDM-based 
 * firmware update. 
 * 
 * @note The PLDM FWUP manager is divided into the UA and FD specifications. 
 * It is assumed Cerberus can operate in both these modes. 
 */
struct pldm_fwup_manager {
    struct pldm_fwup_fd_manager fd_mgr;                                             /**< The FD manager. */
    struct pldm_fwup_ua_manager ua_mgr;                                             /**< The UA manager. */
};


int pldm_fwup_manager_init(struct pldm_fwup_manager *fwup_mgr, 
    struct pldm_fwup_protocol_firmware_parameters *fd_fw_parameters,  struct pldm_fwup_fup_component_image_entry *fup_comp_img_list,
    struct pldm_fwup_flash_manager *fd_flash_mgr, struct pldm_fwup_flash_manager *ua_flash_mgr, 
    struct pldm_fwup_protocol_version_string *fup_comp_img_set_ver, uint16_t num_components);
void pldm_fwup_manager_deinit(struct pldm_fwup_manager *fwup_mgr);

void reset_get_cmd_state(struct pldm_fwup_protocol_multipart_transfer *get_cmd_state);



#define	PLDM_FWUP_MANAGER_ERROR(code)		ROT_ERROR (ROT_MODULE_PLDM_FWUP_MANAGER, code)

/**
 * Error codes that can be generated by PLDM FWUP manager. 
 */
enum {
    PLDM_FWUP_MANAGER_INVALID_ARGUMENT = PLDM_FWUP_MANAGER_ERROR (0x00)     /**< Input parameter is null or not valid. */
};

#endif /* PLDM_FWUP_MANAGER_H_ */