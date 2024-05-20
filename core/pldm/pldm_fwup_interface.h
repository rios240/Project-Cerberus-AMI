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


int pldm_fwup_interface_generate_request(struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len);

//int pldm_fwup_run_update(struct mctp_interface *mctp, struct cmd_channel *channel, uint8_t inventory_cmds, uint8_t device_eid, int ms_timeout);









#endif /* PLDM_FWUP_INTERFACE_H_ */
