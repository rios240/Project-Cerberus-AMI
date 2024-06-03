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


TEST_SUITE_LABEL ("pldm_fwup_protocol_fd_commands");

/**
 * Testing Functions
*/

static void pldm_fwup_protocol_fd_commands_test_query_device_identifiers(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

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