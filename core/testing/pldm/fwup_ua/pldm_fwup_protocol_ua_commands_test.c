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


TEST_SUITE_LABEL ("pldm_fwup_protocol_ua_commands");

/**
 * Testing Functions
*/

static void pldm_fwup_protocol_ua_commands_test_query_device_identifiers(CuTest *test) {
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

    status = send_and_receive_full_mctp_message(&testing, PLDM_QUERY_DEVICE_IDENTIFIERS);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_QUERY_DEVICE_IDENTIFIERS, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, 8765, testing.device_mgr.entries[2].pci_device_id);
    CuAssertIntEquals(test, 4321, testing.device_mgr.entries[2].pci_vid);
    CuAssertIntEquals(test, 2109, testing.device_mgr.entries[2].pci_subsystem_id);
    CuAssertIntEquals(test, 6789, testing.device_mgr.entries[2].pci_subsystem_vid);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_ua_commands_test_get_firmware_parameters(CuTest *test) {
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

    status = send_and_receive_full_mctp_message(&testing, PLDM_GET_FIRMWARE_PARAMETERS);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_GET_FIRMWARE_PARAMETERS, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_NUM_COMPONENTS, testing.fwup_mgr.ua_mgr.rec_fw_parameters.count);
    CuAssertIntEquals(test, PLDM_COMP_FIRMWARE, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[0].comp_classification);
    CuAssertIntEquals(test, 187, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[0].comp_classification_index);
    CuAssertIntEquals(test, 29485, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[0].comp_identifier);
    CuAssertIntEquals(test, 3780935207, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[0].active_comp_comparison_stamp);
    CuAssertStrEquals(test, PLDM_FWUP_ACTIVE_FIRMWARE_COMP_VER, (const char *)testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[0].active_comp_ver.version_str);
    CuAssertIntEquals(test, PLDM_COMP_MIDDLEWARE, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[1].comp_classification);
    CuAssertIntEquals(test, 190, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[1].comp_classification_index);
    CuAssertIntEquals(test, 29490, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[1].comp_identifier);
    CuAssertIntEquals(test, 3780935210, testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[1].active_comp_comparison_stamp);
    CuAssertStrEquals(test, PLDM_FWUP_ACTIVE_MIDDLEWARE_COMP_VER, (const char *)testing.fwup_mgr.ua_mgr.rec_fw_parameters.entries[1].active_comp_ver.version_str);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_ua_commands_test_request_update(CuTest *test) {
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

    status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_UPDATE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_REQUEST_UPDATE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_META_DATA_SIZE, testing.fwup_mgr.ua_mgr.update_info.fd_meta_data_len);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.ua_mgr.update_info.fd_will_send_pkg_data_cmd);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_ua_commands_test_get_package_data(CuTest *test) {
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

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

TEST_SUITE_START (pldm_fwup_protocol_ua_commands);

TEST (pldm_fwup_protocol_ua_commands_test_query_device_identifiers);
TEST (pldm_fwup_protocol_ua_commands_test_get_firmware_parameters);
TEST (pldm_fwup_protocol_ua_commands_test_request_update);
TEST (pldm_fwup_protocol_ua_commands_test_get_package_data);

TEST_SUITE_END;