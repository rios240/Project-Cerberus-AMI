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


TEST_SUITE_LABEL ("pldm_fwup_handler_ua");

/**
 * Testing Functions
*/

static void pldm_fwup_handler_ua_test_run_update_ua_50_kb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_100_kb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_500_kb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_1_mb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_100_mb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_MB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_MB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_500_mb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_MB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_MB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_1_gb(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_GB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_GB;

    gettimeofday(&start, NULL);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, true, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_50_kb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_50_KB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_100_kb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_KB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_500_kb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_KB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_1_mb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_MB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_100_mb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_MB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_100_MB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_500_mb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_MB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_500_MB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

static void pldm_fwup_handler_ua_test_run_update_ua_1_gb_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

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

    testing.fwup_mgr.ua_mgr.comp_img_entries[0].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_GB;
    testing.fwup_mgr.ua_mgr.comp_img_entries[1].comp_size = PLDM_FWUP_COMP_PKG_META_DATA_SIZE_1_GB;

    gettimeofday(&start, NULL);

    testing.device_mgr.entries[2].pci_device_id = 8765;
    testing.device_mgr.entries[2].pci_vid = 4321;
    testing.device_mgr.entries[2].pci_subsystem_id = 2109;
    testing.device_mgr.entries[2].pci_subsystem_vid = 6789;

    testing.fwup_mgr.ua_mgr.rec_fw_parameters = *testing.fwup_mgr.fd_mgr.fw_parameters;

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_UA_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.run_update_ua(&handler, false, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    gettimeofday(&end, NULL);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Execution time: %f milliseconds\n", mtime);
}

TEST_SUITE_START (pldm_fwup_handler_ua);

TEST (pldm_fwup_handler_ua_test_run_update_ua_50_kb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_100_kb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_500_kb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_1_mb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_100_mb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_500_mb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_1_gb);
TEST (pldm_fwup_handler_ua_test_run_update_ua_50_kb_no_inventory_cmds);
TEST (pldm_fwup_handler_ua_test_run_update_ua_100_kb_no_inventory_cmds);
TEST (pldm_fwup_handler_ua_test_run_update_ua_500_kb_no_inventory_cmds);
TEST (pldm_fwup_handler_ua_test_run_update_ua_1_mb_no_inventory_cmds);
TEST (pldm_fwup_handler_ua_test_run_update_ua_100_mb_no_inventory_cmds);
TEST (pldm_fwup_handler_ua_test_run_update_ua_500_mb_no_inventory_cmds);
TEST (pldm_fwup_handler_ua_test_run_update_ua_1_gb_no_inventory_cmds);

TEST_SUITE_END;