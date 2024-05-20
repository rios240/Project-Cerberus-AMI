#include <stdint.h>
#include <string.h>
#include "testing.h"
#include "common/unused.h"
#include "pldm/cmd_interface_pldm.h"
#include "pldm/cmd_channel/cmd_channel_tcp.h"
#include "mctp/mctp_interface.h"
#include "flash/flash_virtual_disk.h"
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/device_manager.h"
#include "libpldm/firmware_update.h"

/*
TEST_SUITE_LABEL ("pldm_fwup_interface_test");


//Error function for cmd_cerberus
static int generate_error_packet(struct cmd_interface *intf, struct cmd_interface_msg *request,
		uint8_t error_code, uint32_t error_data, uint8_t cmd_set) 
{


    UNUSED (intf);
    UNUSED (error_code);
    UNUSED (error_data);
    UNUSED (cmd_set);

    uint8_t buffer[32];
    memset(buffer, 0xFF, 32);
    request->data = buffer;
    return 0;
}

*******************
 * Test cases
 *******************


static void pldm_fwup_run_update_single_device_test(CuTest *test)
{

#ifdef PLDM_FWUP_FD_ENABLE

    TEST_START;
    struct flash_virtual_disk common_flash;
    const char *common_flash_region = "/s/bach/j/under/tylerios/Research/Project-Cerberus-AMI/common_flash.bin";
    struct flash_virtual_disk_state flash_state_ptr;
    int status = flash_virtual_disk_init(&common_flash, common_flash_region, &flash_state_ptr, 1073741824);
    CuAssertIntEquals (test, 0, status);

    struct cmd_interface cmd_cerberus = {0};
    cmd_cerberus.generate_error_packet = generate_error_packet;
	struct cmd_interface cmd_mctp = {0};
    struct cmd_interface cmd_spdm = {0};
	struct device_manager device_manager;
    struct pldm_fwup_flash_map fwup_flash;
    struct pldm_fwup_state fwup_state;
    struct cmd_channel channel;
    struct mctp_interface mctp;
    struct cmd_interface_pldm cmd_pldm;
    uint8_t updating_device_eid = 0xAA;
    uint8_t updating_device_addr = 0xAB;
    uint8_t this_device_eid = 0xBB;
    uint8_t this_device_addr = 0xBA;


    status = device_manager_init(&device_manager, 2, 2, DEVICE_MANAGER_AC_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE,
        0, 0, 0, 0, 0, 0, 0);
    CuAssertIntEquals (test, 0, status);
    device_manager.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = this_device_eid;
    device_manager.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = this_device_addr;
    device_manager.entries[2].eid = updating_device_eid;
    device_manager.entries[2].smbus_addr = updating_device_addr;

    status = cmd_interface_pldm_init(&cmd_pldm, &fwup_flash, &fwup_state, &device_manager, updating_device_eid);
    CuAssertIntEquals (test, 0, status);

    status = cmd_channel_init(&channel, this_device_addr);
    CuAssertIntEquals (test, 0, status);

    status = mctp_interface_init(&mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, &cmd_pldm.base, &device_manager);
    CuAssertIntEquals (test, 0, status);

    status = pldm_fwup_run_update(&mctp, &channel, 1, updating_device_eid, 10000);
    CuAssertIntEquals (test, 0, status);

#elif defined(PLDM_FWUP_UA_ENABLE)

    TEST_START;
    struct flash_virtual_disk common_flash;
    const char *common_flash_region = "/s/bach/j/under/tylerios/Research/Project-Cerberus-AMI/common_flash.bin";
    struct flash_virtual_disk_state flash_state_ptr;
    int status = flash_virtual_disk_init(&common_flash, common_flash_region, &flash_state_ptr, 1073741824);
    CuAssertIntEquals (test, 0, status);

    struct cmd_interface cmd_cerberus = {0};
    cmd_cerberus.generate_error_packet = generate_error_packet;
	struct cmd_interface cmd_mctp = {0};
    struct cmd_interface cmd_spdm = {0};
	struct device_manager device_manager;
    struct pldm_fwup_flash_map fwup_flash;
    struct pldm_fwup_state fwup_state;
    struct cmd_channel channel;
    struct mctp_interface mctp;
    struct cmd_interface_pldm cmd_pldm;
    uint8_t updating_device_eid = 0xBB;
    uint8_t updating_device_addr = 0xBA;
    uint8_t this_device_eid = 0xAA;
    uint8_t this_device_addr = 0xAB;


    status = device_manager_init(&device_manager, 2, 2, DEVICE_MANAGER_AC_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE,
        0, 0, 0, 0, 0, 0, 0);
    CuAssertIntEquals (test, 0, status);
    device_manager.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = this_device_eid;
    device_manager.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = this_device_addr;
    device_manager.entries[2].eid = updating_device_eid;
    device_manager.entries[2].smbus_addr = updating_device_addr;

    status = cmd_interface_pldm_init(&cmd_pldm, &fwup_flash, &fwup_state, &device_manager, updating_device_eid);
    CuAssertIntEquals (test, 0, status);

    status = cmd_channel_init(&channel, this_device_addr);
    CuAssertIntEquals (test, 0, status);

    status = mctp_interface_init(&mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, &cmd_pldm.base, &device_manager);
    CuAssertIntEquals (test, 0, status);

    status = pldm_fwup_run_update(&mctp, &channel, 1, updating_device_eid, 10000);
    CuAssertIntEquals (test, 0, status);
#endif

}

TEST_SUITE_START (pldm_fwup_interface_test);

TEST (pldm_fwup_run_update_single_device_test);

TEST_SUITE_END;

*/