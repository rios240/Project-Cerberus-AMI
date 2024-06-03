#include <stdint.h>
#include <string.h>
#include "testing.h"
#include "pldm/cmd_channel/cmd_channel_tcp.h"
#include "cmd_interface/cmd_channel.h"
#include "mctp/mctp_interface.h"
#include "pldm/cmd_interface_pldm.h"
#include "pldm/pldm_fwup_manager.h"
#include "testing/pldm/fwup_testing.h"
#include "pldm/pldm_fwup_handler.h"
#include "platform_api.h"
#include "platform_pldm_testing.h"

TEST_SUITE_LABEL ("pldm_fwup_protocol_fd_commands");

struct pldm_fwup_protocol_fd_commands_testing {
    struct mctp_interface mctp;
    struct cmd_interface_pldm pldm;
    struct device_manager device_mgr;
    struct pldm_fwup_manager fwup_mgr;
    struct cmd_channel channel;
    int timeout_ms;
    uint8_t req_buffer[MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN];
};

/**
 * Helper Functions
*/

static void setup_testing(struct pldm_fwup_protocol_fd_commands_testing *testing, struct pldm_fwup_protocol_testing_ctx *testing_ctx, CuTest *test)
{
    int status = pldm_fwup_manager_init(&testing->fwup_mgr, &testing_ctx->fd_fw_parameters, testing_ctx->fup_comp_img_list,
        &testing_ctx->fd_flash_mgr, &testing_ctx->ua_flash_mgr, &testing_ctx->fup_comp_img_set_ver, PLDM_FWUP_NUM_COMPONENTS);
    CuAssertIntEquals(test, 0, status);

    status = cmd_interface_pldm_init(&testing->pldm, &testing->fwup_mgr, &testing->device_mgr);
    CuAssertIntEquals(test, 0, status);

    status = cmd_channel_init(&testing->channel, testing->device_mgr.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    testing->channel.send_packet = send_packet;
    testing->channel.receive_packet = receive_packet;

    status = mctp_interface_init(&testing->mctp, &testing_ctx->cmd_cerberus, &testing_ctx->cmd_mctp, &testing_ctx->cmd_spdm,
        &testing->pldm.base, &testing->device_mgr);
    CuAssertIntEquals(test, 0, status);

    testing->timeout_ms = PLDM_TESTING_MS_TIMEOUT;
}

int receive_and_respond_full_mctp_message(struct cmd_channel *channel, struct mctp_interface *mctp, int timeout_ms)
{
    int status;
    do {
        status = cmd_channel_receive_and_process(channel, mctp, timeout_ms);
        if (status != 0) {
            return status;
        }
    } while (mctp->req_buffer.length != 0);
    
    return status;
}

int send_and_receive_full_mctp_message(struct pldm_fwup_protocol_fd_commands_testing *testing, int command)
{
    int status;
    size_t req_length = pldm_fwup_handler_generate_request(testing->mctp.cmd_pldm, command, testing->req_buffer, sizeof (testing->req_buffer));
    if (req_length == CMD_HANDLER_PLDM_TRANSPORT_ERROR) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    } 
    else if (req_length == PLDM_FWUP_HANDLER_UNKNOWN_REQUEST) {
        return PLDM_FWUP_HANDLER_UNKNOWN_REQUEST;
    }

    uint8_t addr = testing->device_mgr.entries[2].smbus_addr;
    uint8_t eid = testing->device_mgr.entries[2].eid;
    
    status = mctp_interface_issue_request(testing->mctp, testing->channel, addr, eid, testing->req_buffer, 
        req_length, testing->req_buffer, sizeof (testing->req_buffer), 0);
    if (status != 0) {
        return status;
    }

    status = pldm_fwup_handler_receive_and_respond_full_mctp_message(&testing->channel, &testing->mctp, testing->timeout_ms);
    return status;
}

static void release_testing(struct pldm_fwup_protocol_fd_commands_testing *testing) {
    mctp_interface_deinit(&testing->mctp);
    cmd_channel_release(&testing->channel);
    cmd_interface_pldm_deinit(&testing->pldm);
    pldm_fwup_manager_deinit(&testing->fwup_mgr);

}

/**
 * Testing Functions
*/

static void pldm_fwup_protocol_fd_commands_test_query_device_identifiers(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_fd_commands_testing testing;

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    int status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_IDLE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_QUERY_DEVICE_IDENTIFIERS, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
}

TEST_SUITE_START (pldm_fwup_protocol_fd_commands);

TEST (pldm_fwup_protocol_fd_commands_test_query_device_identifiers);

TEST_SUITE_END;