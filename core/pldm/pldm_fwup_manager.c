#include <string.h>
#include "pldm_fwup_manager.h"



/**
 * Initialize a PLDM FWUP manager instance. 
 * 
 * @param fwup_mgr The PLDM FWUP manager instance to initialize.
 * @param fd_fw_parameters The FD firmware parameters
 * @param fup_comp_img_list The FUP component image information list
 * @param fd_flash_mgr The FWUP flash manager for the FD
 * @param ua_flash_mgr The FWUP flash manager for the UA
 * @param fup_comp_img_set_ver The component image set obtained from the FUP.
 * @param num_components The number of components in the FUP.
 * 
 * @return 0 on success otherwise an error code.
*/
int init_pldm_fwup_manager(struct pldm_fwup_manager *fwup_mgr, 
    struct pldm_fwup_protocol_firmware_parameters *fd_fw_parameters,  struct pldm_fwup_fup_component_image_entry *fup_comp_img_list,
    struct pldm_fwup_flash_manager *fd_flash_mgr, struct pldm_fwup_flash_manager *ua_flash_mgr, 
    struct pldm_fwup_protocol_version_string *fup_comp_img_set_ver, uint16_t num_components)
{
    if (fwup_mgr == NULL || fd_fw_parameters == NULL || fup_comp_img_list == NULL ||
        fd_flash_mgr == NULL || ua_flash_mgr == NULL) {
        return PLDM_FWUP_MANAGER_INVALID_ARGUMENT;
    }

    memset(fwup_mgr, 0, sizeof (struct pldm_fwup_manager));

    fwup_mgr->fd_mgr.flash_mgr = fd_flash_mgr;
    fwup_mgr->fd_mgr.fw_parameters = fd_fw_parameters;
    fwup_mgr->fd_mgr.get_cmd_state.transfer_op_flag = PLDM_GET_FIRSTPART;
    fwup_mgr->fd_mgr.get_cmd_state.transfer_flag = PLDM_START;
    fwup_mgr->fd_mgr.state.current_state = PLDM_FD_STATE_IDLE;

    fwup_mgr->ua_mgr.comp_img_entries = fup_comp_img_list;
    fwup_mgr->ua_mgr.flash_mgr = ua_flash_mgr;
    fwup_mgr->ua_mgr.get_cmd_state.transfer_op_flag = PLDM_GET_FIRSTPART;
    fwup_mgr->ua_mgr.get_cmd_state.transfer_flag = PLDM_START;
    fwup_mgr->ua_mgr.fup_comp_img_set_ver = fup_comp_img_set_ver;
    fwup_mgr->ua_mgr.num_components = num_components;

    return 0;
}

/**
 * Deinitialize a PLDM FWUP manager instance. 
 * 
 * @param fwup_mgr The PLDM FWUP manager instance to deinitialize.
*/
void deinit_pldm_fwup_manager(struct pldm_fwup_manager *fwup_mgr)
{
    if (fwup_mgr != NULL) {
        memset(fwup_mgr, 0, sizeof (struct pldm_fwup_manager));
    }
}