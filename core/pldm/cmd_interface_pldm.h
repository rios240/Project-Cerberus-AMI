#ifndef CMD_INTERFACE_PLDM_H_
#define CMD_INTERFACE_PLDM_H_

#include "pldm_fwup_interface.h"
#include "pldm_fwup_protocol.h"

/**
 * Command interface for processing received PLDM protocol requests.
 */
struct cmd_interface_pldm {
    struct cmd_interface base;                                                  /**< Base command interface. */
	const struct pldm_fwup_flash_map *fwup_flash;                               /**< The flash address mapping to use for a PLDM FWUP. */
    struct pldm_fwup_state *fwup_state;                                         /**< State context for the command interface instance. */
    struct device_manager *device_mgr;                                          /**< Device manager instance. */
};


int cmd_interface_pldm_init (struct cmd_interface_pldm *intf, 
    struct pldm_fwup_flash_map *fwup_flash, struct pldm_fwup_state *fwup_state, struct device_manager *device_mgr);
void cmd_interface_pldm_deinit (struct cmd_interface_pldm *intf);
int cmd_interface_pldm_generate_request(struct cmd_interface *intf, uint8_t pldm_command, uint8_t *buffer, size_t buf_len);


#endif /* CMD_INTERFACE_PLDM_H_ */