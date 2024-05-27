#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pldm/cmd_interface_pldm.h"
#include "pldm/cmd_channel/cmd_channel_tcp.h"
#include "pldm/pldm_fwup_handler.h"
#include "mctp/mctp_interface.h"
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/device_manager.h"
#include "flash/flash_virtual_disk.h"
#include "common/unused.h"
#include "status/rot_status.h"
#include "testing.h"


TEST_SUITE_LABEL ("pldm_fwup_transfers");

struct pldm_fwup_testing {
    struct mctp_interface mctp;
    struct cmd_interface_pldm cmd_pldm;
    struct cmd_interface cmd_mctp;
    struct cmd_interface cmd_spdm;
    struct cmd_interface cmd_cerberus;
    struct cmd_channel channel;
    struct device_manager device_mgr;
    struct flash_virtual_disk flash;
    struct flash_virtual_disk_state flash_state;
    struct pldm_fwup_manager fwup_mgr;
    struct pldm_fwup_fup_component_image_entry *fup_comp_img_list;
    struct pldm_fwup_flash_manager fd_flash_mgr;
    struct pldm_fwup_flash_manager ua_flash_mgr;
    struct pldm_fwup_protocol_firmware_parameters fd_fw_parameters;
    struct pldm_fwup_protocol_version_string fup_comp_img_set_ver;
    uint16_t num_components;

};

/*******************
 * Helper Functions
 *******************/

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

static void initialize_base_flash(CuTest *test, struct flash_virtual_disk *base_flash, struct flash_virtual_disk_state *base_flash_state,
    const char *region, size_t size)
{
    int status = flash_virtual_disk_init(base_flash, region, base_flash_state, size);
    CuAssertIntEquals(test, 0, status);
}

static void initialize_cmd_channel_and_mctp_interface(CuTest *test, struct pldm_fwup_testing *testing, uint8_t channel_id) {
    int status = cmd_channel_init(&testing->channel, channel_id);
    CuAssertIntEquals(test, 0, status);

    testing->channel.send_packet = send_packet;
    testing->channel.receive_packet = receive_packet;

    testing->cmd_cerberus.generate_error_packet = generate_error_packet;
    testing->cmd_mctp.generate_error_packet = generate_error_packet;
    testing->cmd_spdm.generate_error_packet = generate_error_packet;

    status = mctp_interface_init(&testing->mctp, &testing->cmd_cerberus, &testing->cmd_mctp,
        &testing->cmd_spdm, &testing->cmd_pldm.base, &testing->device_mgr);
    CuAssertIntEquals(test, 0, status);
}

static void deinitialize_testing(struct pldm_fwup_testing *testing) {
    mctp_interface_deinit(&testing->mctp);
    cmd_channel_release(&testing->channel);
    cmd_interface_pldm_deinit(&testing->cmd_pldm);
    flash_virtual_disk_release(&testing->flash);
    device_manager_release(&testing->device_mgr);
    pldm_fwup_manager_deinit(&testing->fwup_mgr);
    memset (testing, 0, sizeof (struct pldm_fwup_testing));
}


/*******************
 * Tests
 *******************/

//#define PLDM_FWUP_ENABLE_FIRMWARE_DEVICE_TESTS

static void pldm_fwup_transfers_test_10_kb_transfer(CuTest *test)
{
    struct pldm_fwup_testing testing;
    memset(&testing, 0, sizeof (struct pldm_fwup_testing));
    uint8_t request_buf[MCTP_BASE_PROTOCOL_MAX_MESSAGE_LEN];

    const char *disk_region = "/s/bach/j/under/tylerios/Research/Project-Cerberus-AMI/common_flash_10_kb.bin";
    size_t disk_size = 5242880;

    TEST_START;

    initialize_base_flash(test, &testing.flash, &testing.flash_state, disk_region, disk_size);

    int status = device_manager_init(&testing.device_mgr, 1, 2, DEVICE_MANAGER_AC_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE,
        0, 0, 0, 0, 0, 0, 0);
    CuAssertIntEquals(test, 0, status);

    testing.ua_flash_mgr.flash = &testing.flash.base;
    testing.ua_flash_mgr.package_data_region.start_addr = 0;
    testing.ua_flash_mgr.package_data_region.length = 10240;
    testing.ua_flash_mgr.package_data_size = 10240;

    testing.fd_flash_mgr.flash = &testing.flash.base;
    testing.fd_flash_mgr.package_data_region.start_addr = 10240;
    testing.fd_flash_mgr.package_data_region.length = 10240;

    struct pldm_fwup_fup_component_image_entry dummy = {0};
    testing.fup_comp_img_list = &dummy;

    status = pldm_fwup_manager_init(&testing.fwup_mgr, &testing.fd_fw_parameters, testing.fup_comp_img_list,
        &testing.fd_flash_mgr, &testing.ua_flash_mgr, &testing.fup_comp_img_set_ver, testing.num_components);
    CuAssertIntEquals(test, 0, status);

#ifdef PLDM_FWUP_ENABLE_FIRMWARE_DEVICE_TESTS
    testing.device_mgr.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = 0x40;
    testing.device_mgr.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = 0x4E;
    testing.device_mgr.entries[2].eid = 0x60;
    testing.device_mgr.entries[2].smbus_addr = 0x6E;

    testing.device_mgr.entries[2].capabilities.request.max_message_size = MCTP_BASE_PROTOCOL_MAX_MESSAGE_BODY;
    testing.device_mgr.entries[2].capabilities.request.max_packet_size = MCTP_BASE_PROTOCOL_MAX_TRANSMISSION_UNIT;

    status = cmd_interface_pldm_init(&testing.cmd_pldm, &testing.fwup_mgr, &testing.device_mgr);
    CuAssertIntEquals(test, 0, status);

    initialize_cmd_channel_and_mctp_interface(test, &testing, 0x4E);

    testing.fwup_mgr.fd_mgr.state.previous_state = PLDM_FD_STATE_IDLE;
    testing.fwup_mgr.fd_mgr.state.current_state = PLDM_FD_STATE_LEARN_COMPONENTS;
    testing.fwup_mgr.fd_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;
    testing.fwup_mgr.fd_mgr.flash_mgr->package_data_size = 10240;
    testing.fwup_mgr.fd_mgr.update_info.package_data_len = 10240;

    do {
        status = pldm_fwup_handler_generate_request(testing.mctp.cmd_pldm, PLDM_GET_PACKAGE_DATA, request_buf, sizeof (request_buf));
        status = mctp_interface_issue_request(&testing.mctp, &testing.channel, 0x6E, 0x60, request_buf, 
            (size_t) status, request_buf, sizeof (request_buf), 0);
        CuAssertIntEquals(test, 0, status);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.fd_mgr.state.previous_cmd);

        do {
            status = cmd_channel_receive_and_process(&testing.channel, &testing.mctp, 10000);
            CuAssertIntEquals(test, 0, status);

        } while (testing.mctp.rsp_state != MCTP_INTERFACE_RESPONSE_SUCCESS);
        mctp_interface_reset_message_processing(&testing.mctp);
        CuAssertIntEquals(test, PLDM_SUCCESS, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.fd_mgr.get_cmd_state.transfer_op_flag != PLDM_GET_FIRSTPART);

#else
    UNUSED(request_buf);

    testing.device_mgr.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = 0x60;
    testing.device_mgr.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = 0x6E;
    testing.device_mgr.entries[2].eid = 0x40;
    testing.device_mgr.entries[2].smbus_addr = 0x4E;

    testing.device_mgr.entries[2].capabilities.request.max_message_size = MCTP_BASE_PROTOCOL_MAX_MESSAGE_BODY;
    testing.device_mgr.entries[2].capabilities.request.max_packet_size = MCTP_BASE_PROTOCOL_MAX_TRANSMISSION_UNIT;

    status = cmd_interface_pldm_init(&testing.cmd_pldm, &testing.fwup_mgr, &testing.device_mgr);
    CuAssertIntEquals(test, 0, status);
    
    initialize_cmd_channel_and_mctp_interface(test, &testing, 0x6E);

    testing.fwup_mgr.ua_mgr.state.previous_cmd = PLDM_REQUEST_UPDATE;

    do {
        do {
            status = cmd_channel_receive_and_process(&testing.channel, &testing.mctp, 10000);
            CuAssertIntEquals(test, 0, status);
        } while (testing.mctp.req_buffer.length != 0);
        mctp_interface_reset_message_processing(&testing.mctp);
        CuAssertIntEquals(test, PLDM_GET_PACKAGE_DATA, testing.fwup_mgr.ua_mgr.state.previous_cmd);
        CuAssertIntEquals(test, PLDM_SUCCESS, testing.fwup_mgr.ua_mgr.state.previous_completion_code);
    } while (testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_END && testing.fwup_mgr.ua_mgr.get_cmd_state.transfer_flag != PLDM_START_AND_END);
#endif

    deinitialize_testing(&testing);
}


TEST_SUITE_START (pldm_fwup_transfers);

TEST (pldm_fwup_transfers_test_10_kb_transfer);

TEST_SUITE_END;