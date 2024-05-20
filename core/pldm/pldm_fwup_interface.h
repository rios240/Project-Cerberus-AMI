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


int pldm_fwup_generate_request(struct cmd_interface *intf, uint8_t command, uint8_t *buffer, size_t buf_len);









#endif /* PLDM_FWUP_INTERFACE_H_ */
