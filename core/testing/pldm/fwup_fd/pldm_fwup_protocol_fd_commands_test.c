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

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_IDLE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_QUERY_DEVICE_IDENTIFIERS, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_firmware_parameters(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_IDLE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_GET_FIRMWARE_PARAMETERS, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_request_update(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_REQUEST_UPDATE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_PKG_DATA_SIZE, testing.fwup_mgr.fd_mgr.update_info.package_data_len);
    CuAssertIntEquals(test, 1024, testing.fwup_mgr.fd_mgr.update_info.max_transfer_size);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.fd_mgr.update_info.max_outstanding_transfer_req);
    CuAssertIntEquals(test, PLDM_FWUP_NUM_COMPONENTS, testing.fwup_mgr.fd_mgr.update_info.num_components);
    CuAssertStrEquals(test, PLDM_FWUP_PENDING_COMP_IMG_SET_VER, (const char *)testing.fwup_mgr.fd_mgr.update_info.comp_img_set_ver.version_str);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_package_data(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_PACKAGE_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_device_meta_data(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

    //do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    //} while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}


TEST_SUITE_START (pldm_fwup_protocol_fd_commands);

TEST (pldm_fwup_protocol_fd_commands_test_query_device_identifiers);
TEST (pldm_fwup_protocol_fd_commands_test_get_firmware_parameters);
TEST (pldm_fwup_protocol_fd_commands_test_request_update);
TEST (pldm_fwup_protocol_fd_commands_test_get_package_data);
TEST (pldm_fwup_protocol_fd_commands_test_get_device_meta_data);

TEST_SUITE_END;