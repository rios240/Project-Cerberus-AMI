#ifndef CMD_INTERFACE_PLDM_H_
#define CMD_INTERFACE_PLDM_H_

#include "pldm_fwup_manager.h"
#include "cmd_interface/cmd_interface.h"


/**
 * Command interface for processing received PLDM protocol requests.
 */
struct cmd_interface_pldm {
    struct cmd_interface base;                                                  /**< Base command interface. */
	struct pldm_fwup_manager *fwup_mgr;                                         /**< Firmware update manager. */
    struct device_manager *device_mgr;                                          /**< Device manager instance. */
};


int cmd_interface_pldm_init (struct cmd_interface_pldm *intf, 
    struct pldm_fwup_manager *fwup_mgr, struct device_manager *device_mgr);
void cmd_interface_pldm_deinit (struct cmd_interface_pldm *intf);

#endif /* CMD_INTERFACE_PLDM_H_ */