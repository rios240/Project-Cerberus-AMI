#ifndef CMD_INTERFACE_PLDM_FWUP_H_
#define CMD_INTERFACE_PLDM_FWUP_H_


#include <stdint.h>
#include "platform_config.h"
#include "common/observable.h"
#include "cmd_interface/cmd_interface.h"

/**
 * Command interface for processing received PLDM FWUP protocol requests.
 */
struct cmd_interface_pldm_fwup {
    struct cmd_interface base;                      /**< Base command interface */    
    struct observable observable;					/**< Observer manager for the interface. */
	struct device_manager *device_manager;			/**< Device manager instance */ 
};


int cmd_interface_mctp_control_init (struct cmd_interface_pldm_fwup *intf);
// void cmd_interface_mctp_control_deinit (struct cmd_interface_pldm_fwup *intf);


#endif /* CMD_INTERFACE_PLDM_FWUP_H_ */