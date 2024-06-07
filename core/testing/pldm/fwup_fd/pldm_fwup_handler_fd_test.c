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


TEST_SUITE_LABEL ("pldm_fwup_handler_fd");

/**
 * Testing Functions
*/

static void pldm_fwup_handler_fd_test_start_update_fd(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_FD_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.start_update_fd(&handler, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();
}

static void pldm_fwup_handler_fd_test_start_update_fd_no_inventory_cmds(CuTest *test)
{
    struct pldm_fwup_protocol_testing_ctx testing_ctx;
    struct pldm_fwup_protocol_flash_ctx flash_ctx;
    struct pldm_fwup_protocol_commands_testing testing;
    struct pldm_fwup_handler handler;

    TEST_START;

    int status = initialize_global_server_socket();
    CuAssertIntEquals(test, 0, status);

    setup_flash_ctx(&flash_ctx, test);
    setup_testing_ctx(&testing_ctx, &flash_ctx);
    setup_fd_device_manager(&testing.device_mgr, test);
    setup_testing(&testing, &testing_ctx, test);

    status = pldm_fwup_handler_init(&handler, &testing.channel, &testing.mctp, testing.timeout_ms);
    CuAssertIntEquals(test, 0, status);

    status = pldm_fwup_handler_set_mode(&handler, PLDM_FWUP_HANDLER_FD_MODE);
    CuAssertIntEquals(test, 0, status);

    status = handler.start_update_fd(&handler, testing.device_mgr.entries[2].eid, testing.device_mgr.entries[2].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    release_flash_ctx(&flash_ctx);
    release_testing_ctx(&testing_ctx);
    release_device_manager(&testing.device_mgr);
    release_testing(&testing);
    pldm_fwup_handler_release(&handler);
    close_global_server_socket();
}

TEST_SUITE_START (pldm_fwup_handler_fd);

TEST (pldm_fwup_handler_fd_test_start_update_fd);
TEST (pldm_fwup_handler_fd_test_start_update_fd_no_inventory_cmds);

TEST_SUITE_END;