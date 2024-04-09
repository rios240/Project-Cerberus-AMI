#include <stdint.h>
#include <string.h>
#include "testing.h"
#include "common/unused.h"
#include "pldm_fwup/cmd_interface_pldm_fwup.h"
#include "pldm_fwup/cmd_channel/cmd_channel_tcp.h"
#include "mctp/mctp_interface.h"
#include "flash/flash_virtual_disk.h"
#include "cmd_interface/cmd_channel.h"

#ifdef PLDM_FWUP_FD_ENABLE
#include "libpldm/firmware_update.h"
#elif defined(PLDM_FWUP_UA_ENABLE)
#include "firmware_update.h"
#endif


TEST_SUITE_LABEL ("pldm_fwup_learn_components_state_commands");

//Function to assign to cmd_cerberus to avoid memory error
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

void initialize_mctp_interface_test(struct mctp_interface *mctp, struct cmd_interface_pldm_fwup *cmd_pldm, struct pldm_fwup_state *state_ptr, 
    struct cmd_channel *channel, struct device_manager *device_mgr, struct cmd_interface *cmd_cerberus, 
    struct pldm_fwup_flash_map *flash_map, uint8_t eid, uint8_t id, CuTest *test)
{
    struct firmware_update_control control;

    struct cmd_interface cmd_spdm;
    struct cmd_interface cmd_mctp;

    int status = cmd_interface_pldm_fwup_init(cmd_pldm, flash_map, state_ptr, &control, 0);
    CuAssertIntEquals(test, 0, status);

    status = device_manager_init(device_mgr, 1, 1, DEVICE_MANAGER_AC_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE, 1000, 1000, 1000, 0, 0, 0, 0);
    CuAssertIntEquals(test, 0, status);
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = eid;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = id;


    status = cmd_channel_init(channel, id);
    CuAssertIntEquals(test, 0, status);
    channel->receive_packet = receive_packet;
    channel->send_packet = send_packet;

    cmd_cerberus->generate_error_packet = generate_error_packet;

    status = mctp_interface_init(mctp, cmd_cerberus, &cmd_mctp, &cmd_spdm, &cmd_pldm->base, device_mgr);
    CuAssertIntEquals(test, 0, status);
}

/*******************
 * Test cases
 *******************/

#ifdef PLDM_FWUP_FD_ENABLE
static void pldm_fwup_learn_components_state_commands_test_get_package_data(CuTest *test) 
{
    struct mctp_interface mctp;
    struct cmd_interface_pldm_fwup cmd_pldm;
    struct cmd_channel channel;
    struct cmd_interface cmd_cerberus;
    struct device_manager device_mgr;
    struct pldm_fwup_flash_map flash_map;
    struct pldm_fwup_state pldm_state_ptr;
    uint8_t device_eid = 0xAA;
    uint8_t device_addr = 0xAB;

    const char *flash_disk_region = "/s/bach/j/under/tylerios/Research/Project-Cerberus-AMI/Cerberus-Dev/core/testing/pldm_fwup/utils/disk_flash_region_test.bin";
    struct flash_virtual_disk package_data;
    struct flash_virtual_disk_state flash_state_ptr;
    size_t package_data_size = 32 * 32768;

    TEST_START;

    int status = flash_virtual_disk_init (&package_data, flash_disk_region, &flash_state_ptr, package_data_size);
    CuAssertIntEquals(test, 0, status);

    flash_map.package_data = &package_data.base;
    flash_map.package_data_addr = 0;
    flash_map.package_data_size = package_data_size;


    initialize_mctp_interface_test(&mctp, &cmd_pldm, &pldm_state_ptr, &channel, 
        &device_mgr, &cmd_cerberus, &flash_map, device_eid, device_addr, test);

    uint8_t request[MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN];

    do {
        size_t request_size = cmd_pldm.generate_request(&cmd_pldm.base, 0x11, request, MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN);

        status = mctp_interface_issue_request(&mctp, &channel, 0xFB, 0xFA, request, request_size, request, MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN, 0);
        CuAssertIntEquals(test, 0, status);

        status = cmd_channel_receive_and_process(&channel, &mctp, 10000);
        CuAssertIntEquals(test, 0, status);
    } while (cmd_pldm.multipart_transfer.transfer_op_flag != 0);

}

#elif defined(PLDM_FWUP_UA_ENABLE)
static void pldm_fwup_learn_components_state_commands_test_get_package_data(CuTest *test) 
{
    struct mctp_interface mctp;
    struct cmd_interface_pldm_fwup cmd_pldm;
    struct cmd_channel channel;
    struct cmd_interface cmd_cerberus;
    struct device_manager device_mgr;
    struct pldm_fwup_flash_map flash_map;
    struct pldm_fwup_state pldm_state_ptr;
    uint8_t device_eid = 0xFA;
    uint8_t device_addr = 0xFB;

    const char *flash_disk_region = "/s/bach/j/under/tylerios/Research/Project-Cerberus-AMI/Cerberus-Dev/core/testing/pldm_fwup/utils/disk_flash_region_test.bin";
    struct flash_virtual_disk fup;
    struct flash_virtual_disk_state flash_state_ptr;
    size_t fup_size = 32 * 32768;

    TEST_START;

    int status = flash_virtual_disk_init (&fup, flash_disk_region, &flash_state_ptr, fup_size);
    CuAssertIntEquals(test, 0, status);

    flash_map.firmware_update_package = &fup.base;
    flash_map.firmware_update_package_addr = 32 * 32768 + 1;
    flash_map.firmware_update_package_size = fup_size;


    initialize_mctp_interface_test(&mctp, &cmd_pldm, &pldm_state_ptr, &channel, 
        &device_mgr, &cmd_cerberus, &flash_map, device_eid, device_addr, test);

    do {
        status = cmd_channel_receive_and_process(&channel, &mctp, 10000);
        CuAssertIntEquals(test, 0, status);
    } while (cmd_pldm.multipart_transfer.transfer_flag != 0x04 || cmd_pldm.multipart_transfer.transfer_flag != 0x05);

}
#endif


TEST_SUITE_START (pldm_fwup_learn_components_state_commands);

TEST (pldm_fwup_learn_components_state_commands_test_get_package_data);

TEST_SUITE_END;