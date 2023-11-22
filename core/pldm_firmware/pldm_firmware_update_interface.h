#ifndef PLDM_FIRMWARE_UPDATE_INTERFACE_H_
#define PLDM_FIRMWARE_UPDATE_INTERFACE_H_


#include "mctp/mctp_interface.h"
#include "cmd_interface/cmd_channel.h"


int pldm_firmware_update_init(struct mctp_interface *mctp, struct cmd_channel *channel, struct device_manager *device_mgr,
                                uint8_t device_eid, uint8_t device_smbus_addr);

int pldm_firmware_update_run_workflow(struct mctp_interface *mctp, struct cmd_channel *channel, int ms_timeout);








#endif /* PLDM_FIRMWARE_UPDATE_INTERFACE_H_ */