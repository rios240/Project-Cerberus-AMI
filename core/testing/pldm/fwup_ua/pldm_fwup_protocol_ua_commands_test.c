#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
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

static void pldm_fwup_protocol_ua_commands_test_query_device_identifiers_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_QUERY_DEVICE_IDENTIFIERS);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_QUERY_DEVICE_IDENTIFIERS, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, 8765, testing.device_mgr.entries[2].pci_device_id);
    CuAssertIntEquals(test, 4321, testing.device_mgr.entries[2].pci_vid);
    CuAssertIntEquals(test, 2109, testing.device_mgr.entries[2].pci_subsystem_id);
    CuAssertIntEquals(test, 6789, testing.device_mgr.entries[2].pci_subsystem_vid);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);

}

static void pldm_fwup_protocol_ua_commands_test_get_firmware_parameters_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

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

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}


static void pldm_fwup_protocol_ua_commands_test_request_update_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_UPDATE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_REQUEST_UPDATE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB, testing.fwup_mgr.ua_mgr.update_info.fd_meta_data_len);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.ua_mgr.update_info.fd_will_send_pkg_data_cmd);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_update_already_in_update_mode(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_REQUEST_UPDATE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_REQUEST_UPDATE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_ALREADY_IN_UPDATE_MODE, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_pass_component_table_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_DEVICE_METADATA;
    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    CuAssertIntEquals(test, PLDM_FWUP_NUM_COMPONENTS, testing.fwup_mgr.ua_mgr.num_components);

    int num_components = testing.fwup_mgr.ua_mgr.num_components;
    for (testing.fwup_mgr.ua_mgr.current_comp_num = 0; testing.fwup_mgr.ua_mgr.current_comp_num < num_components; 
        testing.fwup_mgr.ua_mgr.current_comp_num++) {
        status = send_and_receive_full_mctp_message(&testing, PLDM_PASS_COMPONENT_TABLE);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_PASS_COMPONENT_TABLE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    }

    CuAssertIntEquals(test, PLDM_CR_COMP_CAN_BE_UPDATED, testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_resp);
    CuAssertIntEquals(test, PLDM_CRC_COMP_CAN_BE_UPDATED, testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_resp_code);
    CuAssertIntEquals(test, PLDM_CR_COMP_CAN_BE_UPDATED, testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_resp);
    CuAssertIntEquals(test, PLDM_CRC_COMP_CAN_BE_UPDATED, testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_resp_code);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_pass_component_table_not_in_update_mode(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_DEVICE_METADATA;
    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    CuAssertIntEquals(test, PLDM_FWUP_NUM_COMPONENTS, testing.fwup_mgr.ua_mgr.num_components);

    int num_components = testing.fwup_mgr.ua_mgr.num_components;
    for (testing.fwup_mgr.ua_mgr.current_comp_num = 0; testing.fwup_mgr.ua_mgr.current_comp_num < num_components; 
        testing.fwup_mgr.ua_mgr.current_comp_num++) {
        status = send_and_receive_full_mctp_message(&testing, PLDM_PASS_COMPONENT_TABLE);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_PASS_COMPONENT_TABLE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, PLDM_FWUP_NOT_IN_UPDATE_MODE, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    }

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_update_component_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.current_comp_num = 0;
    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = send_and_receive_full_mctp_message(&testing, PLDM_UPDATE_COMPONENT);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_UPDATE_COMPONENT, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_CCR_COMP_CAN_BE_UPDATED, testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_compatibility_resp);
    CuAssertIntEquals(test, PLDM_CCRC_NO_RESPONSE_CODE, testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_compatibility_resp_code);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.ua_mgr.comp_img_entries[0].update_option_flags_enabled.bits.bit0);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_update_component_not_in_update_mode(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.current_comp_num = 0;
    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = send_and_receive_full_mctp_message(&testing, PLDM_UPDATE_COMPONENT);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_UPDATE_COMPONENT, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_NOT_IN_UPDATE_MODE, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}


static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);


    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_100_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_MB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);


    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_MB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_500_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_MB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);


    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_MB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_request_firmware_data_1_gb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_GB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);


    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;
    testing.fwup_mgr.ua_mgr.current_comp_num = 0;

    int iterations = (PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_GB + PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE - 1) / PLDM_FWUP_PROTOCOL_MAX_TRANSFER_SIZE;
    int i = 0;
    while (i < iterations) {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_REQUEST_FIRMWARE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
        i++;
    }

    gettimeofday(&end, NULL);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}


static void pldm_fwup_protocol_ua_commands_test_transfer_complete_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_FIRMWARE_DATA;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_TRANSFER_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_TRANSFER_SUCCESS, testing.fwup_mgr.ua_mgr.update_info.transfer_result);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_transfer_complete_command_not_expected(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_UPDATE_COMPONENT;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_TRANSFER_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_COMMAND_NOT_EXPECTED, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_transfer_complete_generic_transfer_error(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_FIRMWARE_DATA;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_TRANSFER_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_FD_GENERIC_TRANSFER_ERROR, testing.fwup_mgr.ua_mgr.update_info.transfer_result);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_verify_complete_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_TRANSFER_COMPLETE;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_VERIFY_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_VERIFY_SUCCESS, testing.fwup_mgr.ua_mgr.update_info.verify_result);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_verify_complete_command_not_expected(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_FIRMWARE_DATA;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_VERIFY_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_COMMAND_NOT_EXPECTED, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_apply_complete_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_VERIFY_COMPLETE;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_APPLY_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_APPLY_SUCCESS, testing.fwup_mgr.ua_mgr.update_info.apply_result);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.update_info.comp_activation_methods_modification);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_apply_complete_command_not_expected(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_TRANSFER_COMPLETE;

    status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_APPLY_COMPLETE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_COMMAND_NOT_EXPECTED, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_activate_firmware_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_ACTIVATE_FIRMWARE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_ACTIVATE_FIRMWARE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FWUP_PROTOCOL_EST_TIME_SELF_CONTAINED_ACTIVATION, testing.fwup_mgr.ua_mgr.update_info.estimated_time_activation);

    gettimeofday(&end, NULL);
    
    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_activate_firmware_activation_not_required(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_ACTIVATE_FIRMWARE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_ACTIVATE_FIRMWARE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, PLDM_FWUP_ACTIVATION_NOT_REQUIRED, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_status_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_GET_STATUS);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_GET_STATUS, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, PLDM_FD_STATE_LEARN_COMPONENTS, testing.fwup_mgr.ua_mgr.update_info.fd_status.current_state);
    CuAssertIntEquals(test, PLDM_FD_STATE_IDLE, testing.fwup_mgr.ua_mgr.update_info.fd_status.previous_state);
    CuAssertIntEquals(test, PLDM_FD_IDLE_LEARN_COMPONENTS_READ_XFER, testing.fwup_mgr.ua_mgr.update_info.fd_status.aux_state);
    CuAssertIntEquals(test, PLDM_FD_AUX_STATE_IN_PROGRESS_OR_SUCCESS, testing.fwup_mgr.ua_mgr.update_info.fd_status.aux_state_status);
    CuAssertIntEquals(test, PLDM_FWUP_MAX_PROGRESS_PERCENT, testing.fwup_mgr.ua_mgr.update_info.fd_status.progress_percent);
    CuAssertIntEquals(test, PLDM_FD_INITIALIZATION, testing.fwup_mgr.ua_mgr.update_info.fd_status.reason_code);
    CuAssertIntEquals(test, 1, testing.fwup_mgr.ua_mgr.update_info.fd_status.update_option_flags_enabled);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_cancel_update_component_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_CANCEL_UPDATE_COMPONENT);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_CANCEL_UPDATE_COMPONENT, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);

    gettimeofday(&end, NULL);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_cancel_update_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_5_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    status = send_and_receive_full_mctp_message(&testing, PLDM_CANCEL_UPDATE);
    CuAssertIntEquals(test, 0, status);
    CuAssertIntEquals(test, PLDM_CANCEL_UPDATE, testing.fwup_mgr.ua_mgr.state.previous_cmd);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.update_info.non_functioning_component_indication);
    CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.update_info.non_functioning_component_bitmap.value);

    gettimeofday(&end, NULL);
    

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}


static void pldm_fwup_protocol_ua_commands_test_get_package_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_package_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);

}

static void pldm_fwup_protocol_ua_commands_test_get_package_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_package_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}


static void pldm_fwup_protocol_ua_commands_test_get_device_meta_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_DEVICE_METADATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_op_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag);
    printf("data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_device_meta_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_DEVICE_METADATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_op_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag);
    printf("data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.data_transfer_handle);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}


static void pldm_fwup_protocol_ua_commands_test_get_device_meta_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_DEVICE_METADATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_op_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag);
    printf("data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.data_transfer_handle);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_device_meta_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_GET_PACKAGE_DATA;

    do {
        status = send_and_receive_full_mctp_message(&testing, PLDM_GET_DEVICE_METADATA);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_DEVICE_METADATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_op_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_op_flag);
    printf("data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.data_transfer_handle);


    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_meta_data_50_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
     testing.fwup_mgr.ua_mgr.flash_mgr->device_meta_data_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_meta_data_100_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
     testing.fwup_mgr.ua_mgr.flash_mgr->device_meta_data_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_meta_data_500_kb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
    testing.fwup_mgr.ua_mgr.flash_mgr->device_meta_data_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

    gettimeofday(&end, NULL);

    printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_protocol_ua_commands_test_get_meta_data_1_mb_success(CuTest *test) {
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;

    struct timeval start, end;
    long seconds, useconds;
    double mtime;


    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx, PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB);
    setup_ua_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    gettimeofday(&start, NULL);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_PASS_COMPONENT_TABLE;
     testing.fwup_mgr.ua_mgr.flash_mgr->device_meta_data_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB;

    do {
        status = receive_and_respond_full_mctp_message(&testing.channel, &testing.mctp, testing.timeout_ms);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_META_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, 0, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
    //reset_get_cmd_state(&testing.fwup_mgr.ua_mgr.get_cmd_state);

     gettimeofday(&end, NULL);

     printf("transfer_flag: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag);
    printf("next_data_transfer_handle: %u\n", testing.fwup_mgr.ua_mgr.get_cmd_state.next_data_transfer_handle);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

TEST_SUITE_START (pldm_fwup_protocol_ua_commands);

TEST (pldm_fwup_protocol_ua_commands_test_query_device_identifiers_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_firmware_parameters_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_update_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_update_already_in_update_mode);
TEST (pldm_fwup_protocol_ua_commands_test_pass_component_table_success);
TEST (pldm_fwup_protocol_ua_commands_test_pass_component_table_not_in_update_mode);
TEST (pldm_fwup_protocol_ua_commands_test_update_component_success);
TEST (pldm_fwup_protocol_ua_commands_test_update_component_not_in_update_mode);
TEST (pldm_fwup_protocol_ua_commands_test_transfer_complete_success);
TEST (pldm_fwup_protocol_ua_commands_test_transfer_complete_command_not_expected);
TEST (pldm_fwup_protocol_ua_commands_test_transfer_complete_generic_transfer_error);
TEST (pldm_fwup_protocol_ua_commands_test_verify_complete_success);
TEST (pldm_fwup_protocol_ua_commands_test_verify_complete_command_not_expected);
TEST (pldm_fwup_protocol_ua_commands_test_apply_complete_success);
TEST (pldm_fwup_protocol_ua_commands_test_apply_complete_command_not_expected);
TEST (pldm_fwup_protocol_ua_commands_test_activate_firmware_success);
TEST (pldm_fwup_protocol_ua_commands_test_activate_firmware_activation_not_required);
TEST (pldm_fwup_protocol_ua_commands_test_get_status_success);
TEST (pldm_fwup_protocol_ua_commands_test_cancel_update_component_success);
TEST (pldm_fwup_protocol_ua_commands_test_cancel_update_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_50_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_100_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_500_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_1_mb_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_100_mb_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_500_mb_success);
TEST (pldm_fwup_protocol_ua_commands_test_request_firmware_data_1_gb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_package_data_50_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_package_data_100_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_package_data_500_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_package_data_1_mb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_device_meta_data_50_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_device_meta_data_100_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_device_meta_data_500_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_device_meta_data_1_mb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_meta_data_50_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_meta_data_100_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_meta_data_500_kb_success);
TEST (pldm_fwup_protocol_ua_commands_test_get_meta_data_1_mb_success);


TEST_SUITE_END;