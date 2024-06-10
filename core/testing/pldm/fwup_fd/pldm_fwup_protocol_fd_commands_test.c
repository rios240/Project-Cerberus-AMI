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

static void pldm_fwup_protocol_fd_commands_test_query_device_identifiers_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
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

static void pldm_fwup_protocol_fd_commands_test_get_firmware_parameters_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
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


static void pldm_fwup_protocol_fd_commands_test_request_update_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_REQUEST_UPDATE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB, testing.fwup_mgr.fd_mgr.update_info.package_data_len);
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

static void pldm_fwup_protocol_fd_commands_test_request_update_already_in_update_mode(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.update_mode = 1;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_IDLE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_REQUEST_UPDATE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_ALREADY_IN_UPDATE_MODE, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_pass_component_table_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_DEVICE_METADATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_entry));
    testing.fwup_mgr.fd_mgr.update_info.num_components = PLDM_FWUP_NUM_COMPONENTS;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_PASS_COMPONENT_TABLE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.update_info.comp_transfer_flag != PLDM_START_AND_END && 
        testing.fwup_mgr.fd_mgr.update_info.comp_transfer_flag != PLDM_END);
    
    CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_COMP_FIRMWARE, testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_classification);
    CuAssertIntEquals(test, 29485, testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_identifier);
    CuAssertIntEquals(test, 3780935208, testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_comparison_stamp);
    CuAssertIntEquals(test, testing.fwup_mgr.fd_mgr.fw_parameters->entries[0].comp_classification_index, 
        testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_classification_index);
    CuAssertIntEquals(test, PLDM_COMP_MIDDLEWARE, testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_classification);
    CuAssertIntEquals(test, 29490, testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_identifier);
    CuAssertIntEquals(test, 3780935211, testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_comparison_stamp);
    CuAssertIntEquals(test, testing.fwup_mgr.fd_mgr.fw_parameters->entries[1].comp_classification_index, 
        testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_classification_index);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_pass_component_table_not_in_update_mode(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_DEVICE_METADATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_entry));
    testing.fwup_mgr.fd_mgr.update_info.num_components = PLDM_FWUP_NUM_COMPONENTS;
    testing.fwup_mgr.fd_mgr.state.update_mode = 0;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_PASS_COMPONENT_TABLE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, PLDM_FWUP_NOT_IN_UPDATE_MODE, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.update_info.comp_transfer_flag != PLDM_START_AND_END && 
        testing.fwup_mgr.fd_mgr.update_info.comp_transfer_flag != PLDM_END);
    
    CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_update_component_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_entry));
    testing.fwup_mgr.fd_mgr.update_info.num_components = PLDM_FWUP_NUM_COMPONENTS;

    testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_classification = PLDM_COMP_FIRMWARE;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_classification_index = 187;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_identifier = 29485;

    testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_classification = PLDM_COMP_MIDDLEWARE;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_classification_index = 190;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_identifier = 29490;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_UPDATE_COMPONENT, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB, testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.fd_mgr.update_info.current_comp_update_option_flags.bits.bit0);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_num);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_update_component_not_in_update_mode(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;
    testing.fwup_mgr.fd_mgr.state.update_mode = 0;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_entry));
    testing.fwup_mgr.fd_mgr.update_info.num_components = PLDM_FWUP_NUM_COMPONENTS;

    testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_classification = PLDM_COMP_FIRMWARE;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_classification_index = 187;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[0].comp_identifier = 29485;

    testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_classification = PLDM_COMP_MIDDLEWARE;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_classification_index = 190;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries[1].comp_identifier = 29490;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_UPDATE_COMPONENT, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_NOT_IN_UPDATE_MODE, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}


static void pldm_fwup_protocol_fd_commands_test_request_firmware_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_num = 0;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0;

    uint32_t current_comp_img_size =  testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size;
    uint32_t max_transfer_size = testing.fwup_mgr.fd_mgr.update_info.max_transfer_size;
    for (testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0; 
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset < current_comp_img_size;
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset += max_transfer_size) {
        status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_FIRMWARE_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    }
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_request_firmware_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_num = 0;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0;

    uint32_t current_comp_img_size =  testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size;
    uint32_t max_transfer_size = testing.fwup_mgr.fd_mgr.update_info.max_transfer_size;
    for (testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0; 
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset < current_comp_img_size;
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset += max_transfer_size) {
        status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_FIRMWARE_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    }
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_request_firmware_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_num = 0;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0;

    uint32_t current_comp_img_size =  testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size;
    uint32_t max_transfer_size = testing.fwup_mgr.fd_mgr.update_info.max_transfer_size;
    for (testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0; 
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset < current_comp_img_size;
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset += max_transfer_size) {
        status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_FIRMWARE_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    }
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_request_firmware_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_num = 0;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0;

    uint32_t current_comp_img_size =  testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size;
    uint32_t max_transfer_size = testing.fwup_mgr.fd_mgr.update_info.max_transfer_size;
    for (testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset = 0; 
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset < current_comp_img_size;
            testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset += max_transfer_size) {
        status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_FIRMWARE_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    }
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}


static void pldm_fwup_protocol_fd_commands_test_transfer_complete_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = send_and_receive_full_mctp_message(&testing, PLDM_TRANSFER_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_VERIFY, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_TRANSFER_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_transfer_complete_command_not_expected(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = send_and_receive_full_mctp_message(&testing, PLDM_TRANSFER_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_TRANSFER_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_COMMAND_NOT_EXPECTED, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_transfer_complete_generic_transfer_error(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_FWUP_INVALID_TRANSFER_LENGTH;


    status = send_and_receive_full_mctp_message(&testing, PLDM_TRANSFER_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_DOWNLOAD, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_TRANSFER_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_verify_complete_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_VERIFY;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = send_and_receive_full_mctp_message(&testing, PLDM_VERIFY_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_APPLY, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_VERIFY_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_verify_complete_command_not_expected(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_VERIFY;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = send_and_receive_full_mctp_message(&testing, PLDM_VERIFY_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_VERIFY, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_VERIFY_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_COMMAND_NOT_EXPECTED, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}


static void pldm_fwup_protocol_fd_commands_test_apply_complete_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_APPLY;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = send_and_receive_full_mctp_message(&testing, PLDM_APPLY_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_APPLY_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_apply_complete_command_not_expected(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_APPLY;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = send_and_receive_full_mctp_message(&testing, PLDM_APPLY_COMPLETE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_APPLY, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_APPLY_COMPLETE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_COMMAND_NOT_EXPECTED, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_activate_firmware_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_ACTIVATE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_ACTIVATE_FIRMWARE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.fd_mgr.update_info.self_contained_activation_req);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_activate_firmware_activation_not_required(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;
    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_ACTIVATE_FIRMWARE;


    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_ACTIVATE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_ACTIVATE_FIRMWARE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_ACTIVATION_NOT_REQUIRED, testing.fwup_mgr.fd_mgr.state.previous_completion_code);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}


static void pldm_fwup_protocol_fd_commands_test_get_status_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.state.previous_state = PLDM_FD_STATE_IDLE;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_update_option_flags.value = 0;
    testing.fwup_mgr.fd_mgr.update_info.current_comp_update_option_flags.bits.bit0 = 1;


    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_GET_STATUS, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_cancel_update_component_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;


    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_CANCEL_UPDATE_COMPONENT, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_num);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_update_option_flags.value);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_cancel_update_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_DOWNLOAD;
    testing.fwup_mgr.fd_mgr.state.update_mode = 1;
    testing.fwup_mgr.fd_mgr.state.previous_completion_code = PLDM_SUCCESS;
    testing.fwup_mgr.fd_mgr.update_info.comp_entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_entry));
    testing.fwup_mgr.fd_mgr.update_info.num_components = PLDM_FWUP_NUM_COMPONENTS;


    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_FD_STATE_IDLE, testing.fwup_mgr.fd_mgr.state.current_state);
    CuAssertIntEquals(test, PLDM_CANCEL_UPDATE, testing.fwup_mgr.fd_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_num);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_img_offset);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_img_size);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.update_info.current_comp_update_option_flags.value);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.flash_mgr->package_data_size);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_package_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
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

static void pldm_fwup_protocol_fd_commands_test_get_package_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
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

static void pldm_fwup_protocol_fd_commands_test_get_package_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
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

static void pldm_fwup_protocol_fd_commands_test_get_package_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
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


static void pldm_fwup_protocol_fd_commands_test_get_device_meta_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_device_meta_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_device_meta_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_device_meta_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.update_info.max_transfer_size = PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_meta_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_META_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_meta_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_META_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_meta_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_META_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

static void pldm_fwup_protocol_fd_commands_test_get_meta_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_READY_XFER;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_META_DATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_FD_STATE_READY_XFER, testing.fwup_mgr.fd_mgr.state.current_state);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.fd_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    reset_get_cmd_state(&testing.fwup_mgr.fd_mgr.get_cmd_state);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();
}

TEST_SUITE_START (pldm_fwup_protocol_fd_commands);

TEST (pldm_fwup_protocol_fd_commands_test_query_device_identifiers_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_firmware_parameters_success);
TEST (pldm_fwup_protocol_fd_commands_test_request_update_success);
TEST (pldm_fwup_protocol_fd_commands_test_request_update_already_in_update_mode);
TEST (pldm_fwup_protocol_fd_commands_test_pass_component_table_success);
TEST (pldm_fwup_protocol_fd_commands_test_pass_component_table_not_in_update_mode);
TEST (pldm_fwup_protocol_fd_commands_test_update_component_success);
TEST (pldm_fwup_protocol_fd_commands_test_update_component_not_in_update_mode);
TEST (pldm_fwup_protocol_fd_commands_test_transfer_complete_success);
TEST (pldm_fwup_protocol_fd_commands_test_transfer_complete_command_not_expected);
TEST (pldm_fwup_protocol_fd_commands_test_transfer_complete_generic_transfer_error);
TEST (pldm_fwup_protocol_fd_commands_test_verify_complete_success);
TEST (pldm_fwup_protocol_fd_commands_test_verify_complete_command_not_expected);
TEST (pldm_fwup_protocol_fd_commands_test_apply_complete_success);
TEST (pldm_fwup_protocol_fd_commands_test_apply_complete_command_not_expected);
TEST (pldm_fwup_protocol_fd_commands_test_activate_firmware_success);
TEST (pldm_fwup_protocol_fd_commands_test_activate_firmware_activation_not_required);
TEST (pldm_fwup_protocol_fd_commands_test_get_status_success);
TEST (pldm_fwup_protocol_fd_commands_test_cancel_update_component_success);
TEST (pldm_fwup_protocol_fd_commands_test_cancel_update_success);
TEST (pldm_fwup_protocol_fd_commands_test_request_firmware_data_50_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_request_firmware_data_100_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_request_firmware_data_500_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_request_firmware_data_1_mb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_package_data_50_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_package_data_100_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_package_data_500_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_package_data_1_mb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_device_meta_data_50_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_device_meta_data_100_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_device_meta_data_500_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_device_meta_data_1_mb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_meta_data_50_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_meta_data_100_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_meta_data_500_kb_success);
TEST (pldm_fwup_protocol_fd_commands_test_get_meta_data_1_mb_success);

TEST_SUITE_END;