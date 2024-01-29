#ifndef PLDM_FWUP_INTERFACE_H_
#define PLDM_FWUP_INTERFACE_H_

#include "mctp/mctp_interface.h"
#include "cmd_interface/cmd_channel.h"
#include "pldm_types.h"

enum multipart_transfer_status {
    MPT_START,
    MPT_IN_PROGRESS,
    MPT_END
};

struct pldm_fwup_multipart_transfer {
    uint8_t transfer_status;
    uint32_t last_data_transfer_handle;
};

struct pldm_fwup_error_testing {
    bool8_t unable_to_initiate_update;
    bool8_t retry_request_update;
};

struct pldm_fwup_interface {
    uint8_t current_fd_state;
    uint8_t current_fd_command;
    uint8_t completion_code;
    struct pldm_fwup_multipart_transfer multipart_transfer;
    struct pldm_fwup_error_testing error_testing;
    uint32_t package_data_size;
    uint8_t *package_data;

};

struct pldm_fwup_interface *get_fwup_interface();


int initialize_firmware_update(struct mctp_interface *mctp, struct cmd_channel *cmd_channel, 
                        struct cmd_interface *cmd_mctp, 
                        struct cmd_interface *cmd_spdm, struct cmd_interface *cmd_cerberus,
                        struct device_manager *device_mgr, struct pldm_fwup_interface *fwup);

void firmware_update_check_state(struct pldm_fwup_interface *fwup);

void clean_up_and_reset_firmware_update(struct mctp_interface *mctp, struct pldm_fwup_interface *fwup);


#endif /* PLDM_FWUP_INTERFACE_H_ */