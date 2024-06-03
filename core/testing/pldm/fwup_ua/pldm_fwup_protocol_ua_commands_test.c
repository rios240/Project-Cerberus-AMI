#include <stdint.h>
#include <string.h>
#include "testing.h"
#include "flash/flash_virtual_disk.h"
#include "pldm/cmd_channel/cmd_channel_tcp.h"
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/device_manager.h"
#include "mctp/mctp_interface.h"


TEST_SUITE_LABEL ("pldm_fwup_protocol_ua_commands");


static void pldm_fwup_protocol_ua_commands_test_query_device_identifiers(CuTest *test) {
    
}

TEST_SUITE_START (pldm_fwup_protocol_ua_commands);

TEST (pldm_fwup_protocol_ua_commands_test_query_device_identifiers);

TEST_SUITE_END;