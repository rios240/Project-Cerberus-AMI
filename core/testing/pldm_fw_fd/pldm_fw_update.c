// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "testing.h"
#include "base.h"
#include "utils.h"
#include "firmware_update.h"
#include "mctp/mctp_interface.h"
#include "mctp/mctp_base_protocol.h"
#include "platform_io.h"


TEST_SUITE_LABEL ("pldm_fw_update");

/**
 * Helper Functions
*/

static void platform_printbuf(uint8_t *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        platform_printf("%02x ", buf[i]);
    }
    platform_printf("\n");
}

int send_packet(struct cmd_channel *channel, struct cmd_packet *packet) {
    const int port = 5000;
    const char *address = "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return CMD_CHANNEL_SOC_INIT_FAILURE;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &(server.sin_addr)) <= 0) {
        close(sock);
        return CMD_CHANNEL_SOC_NET_ADDRESS_FAILURE;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        close(sock);
        return CMD_CHANNEL_SOC_CONNECT_FAILURE;
    }


    if (send(sock, packet->data, packet->pkt_size, 0) < 0) {
        close(sock);
        return CMD_CHANNEL_SOC_SEND_FAILURE;
    }

    
    close(sock);

    return 0;
}

int receive_packet(struct cmd_channel *channel, struct cmd_packet *packet, int ms_timeout) {
    const int port = 5000;
    const char *address = "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return CMD_CHANNEL_SOC_INIT_FAILURE;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &(server.sin_addr)) <= 0) {
        close(sock);
        return CMD_CHANNEL_SOC_NET_ADDRESS_FAILURE;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        close(sock);
        return CMD_CHANNEL_SOC_CONNECT_FAILURE;
    }
    platform_printf("Step recv\n");
    ssize_t bytes = recv(sock, packet->data, MCTP_BASE_PROTOCOL_MAX_PACKET_LEN, 0);

    if (bytes < 0) {
        close(sock);
        return CMD_CHANNEL_SOC_SEND_FAILURE;
    }

    packet->pkt_size = bytes;
    packet->dest_addr = 0xAA;

    platform_printbuf(packet->data, packet->pkt_size);

    close(sock);

    return 0;
}

int process_request(struct cmd_interface *intf, struct cmd_interface_msg *request) {
    uint32_t max_transfer_size = 0;
    uint16_t num_of_comp = 0;
	uint8_t max_outstanding_transfer_req = 0;
	uint16_t pkg_data_len = 0;
	uint8_t comp_image_set_ver_str_type = 0;
    uint8_t comp_image_set_ver_str_len = 0;
    struct variable_field comp_img_set_ver_str = { 0 };

    uint8_t completion_code = 0;
    struct pldm_msg *reqMsg = (struct pldm_msg *)&request->data[1];
    int status;

    status = decode_request_update_req(reqMsg, sizeof(struct pldm_request_update_req) + sizeof (struct variable_field), 
                                &completion_code, &max_transfer_size, &num_of_comp, &max_outstanding_transfer_req,
                                &pkg_data_len, &comp_image_set_ver_str_type, &comp_image_set_ver_str_len,
                                &comp_img_set_ver_str);

    platform_printf("pkg_data_len: %d\n", pkg_data_len);

    if (status != 0) {
        return status;
    }

    uint8_t instance_id = 2; 
	uint16_t fd_meta_data_len = 0x555;
	uint8_t fd_will_send_pkg_data;
    if (pkg_data_len != 0) {
        fd_will_send_pkg_data = 0x01;
    } else {
        fd_will_send_pkg_data = 0x00;
    }

    uint8_t respBuf[sizeof (struct pldm_msg_hdr) + sizeof (struct pldm_request_update_resp)+1];
    respBuf[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)&respBuf[1];

    status = encode_request_update_resp(instance_id, fd_meta_data_len, 
                                fd_will_send_pkg_data, respMsg, sizeof(struct pldm_request_update_resp));
    
    memcpy(request->data, respBuf, sizeof (respBuf));
    request->length = sizeof (respBuf);

    struct pldm_request_update_resp *response =
		(struct pldm_request_update_resp *)respMsg->payload;


    platform_printf("Meta data: %02x\n", response->fd_meta_data_len);
    platform_printbuf(respMsg->payload, sizeof (struct pldm_request_update_resp));
    platform_printbuf(request->data, request->length);
    
    return status;
}

int process_response(struct cmd_interface *intf, struct cmd_interface_msg *response) {
    return PLDM_ERROR;
}

int generate_error_packet(struct cmd_interface *intf, struct cmd_interface_msg *request,
		            uint8_t error_code, uint32_t error_data, uint8_t cmd_set) {
    
    uint8_t data[10];

    memset(data, 0xFF, sizeof(data));

    request->data = data;

    return 0;

}

/**
 * Test Functions
*/

static void pldm_fw_update_test_place_holder(CuTest *test)
{
    TEST_START;

    CuAssertIntEquals(test, 0, 0);
}

static void pldm_fw_update_test_request_update_resp(CuTest *test)
{
    platform_printf("Step 0\n");
    int status;
    struct mctp_interface mctp;
    struct cmd_interface cmd_cerberus;
    cmd_cerberus.generate_error_packet = generate_error_packet;
    struct cmd_interface cmd_mctp;
    cmd_mctp.process_request = process_request;
    cmd_mctp.process_response = process_response;
    struct cmd_interface cmd_spdm;
    struct device_manager device_mgr;
    struct cmd_channel channel;
    channel.send_packet = send_packet;
    channel.receive_packet = receive_packet;
    channel.id = 2;

    TEST_START;

    status = device_manager_init(&device_mgr, 2, 0, DEVICE_MANAGER_PA_ROT_MODE, DEVICE_MANAGER_SLAVE_BUS_ROLE, 
                1000, 1000, 1000, 1000, 1000, 1000, 5);

    device_mgr.entries->eid = 0xCC;
    device_mgr.entries->smbus_addr = 0xAA;

    CuAssertIntEquals(test, 0, status);

    status = mctp_interface_init(&mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, &device_mgr);

    CuAssertIntEquals(test, 0, status);

    platform_printf("Step 1\n");
    status = cmd_channel_receive_and_process(&channel, &mctp, 5000);

    platform_printf("Step final\n");
    CuAssertIntEquals(test, 0, status);


}

TEST_SUITE_START (pldm_fw_update);

TEST (pldm_fw_update_test_place_holder);
TEST (pldm_fw_update_test_request_update_resp);

TEST_SUITE_END;