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

TEST_SUITE_START (pldm_fwup_protocol_ua_commands);

TEST (pldm_fwup_protocol_ua_commands_test_query_device_identifiers);

TEST_SUITE_END;