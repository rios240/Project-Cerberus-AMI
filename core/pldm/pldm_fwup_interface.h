#ifndef PLDM_FWUP_INTERFACE_H_
#define PLDM_FWUP_INTERFACE_H_


#include "cmd_interface_pldm.h"
#include "mctp/mctp_interface.h"
#include "cmd_interface/cmd_channel.h"




int pldm_fwup_run_update(struct mctp_interface *mctp, struct cmd_channel *channel, uint8_t inventory_cmds, uint8_t device_eid, int ms_timeout);









#endif /* PLDM_FWUP_INTERFACE_H_ */
