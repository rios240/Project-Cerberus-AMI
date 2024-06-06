#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "testing.h"
#include "pldm/cmd_channel/cmd_channel_tcp.h"
#include "cmd_interface/cmd_channel.h"
#include "mctp/mctp_interface.h"
#include "pldm/cmd_interface_pldm.h"
#include "pldm/pldm_fwup_manager.h"
#include "testing/pldm/fwup_testing.h"
#include "pldm/pldm_fwup_handler.h"
#include "platform_api.h"


TEST_SUITE_LABEL ("pldm_fwup_get_commands_ua");

/**
 * Testing Functions
*/

static void pldm_fwup_get_commands_ua_test_get_package_data(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;

    //do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    //} while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}


static void pldm_fwup_get_commands_ua_test_get_device_meta_data(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;

    //do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_DEVICE_METADATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    //} while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

TEST_SUITE_START (pldm_fwup_get_commands_ua);

TEST (pldm_fwup_get_commands_ua_test_get_package_data);
TEST (pldm_fwup_get_commands_ua_test_get_device_meta_data);

TEST_SUITE_END;