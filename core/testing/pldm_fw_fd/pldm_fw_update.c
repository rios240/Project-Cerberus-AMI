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
#include "platform_io.h"


TEST_SUITE_LABEL ("pldm_fw_update");

/**
 * Helper Functions
*/

/*static void printBuf(uint8_t *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        platform_printf("%02x ", buf[i]);
    }
    platform_printf("\n");
}*/

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
    
    ssize_t bytes = recv(sock, packet->data, CMD_MAX_PACKET_SIZE, 0);

    if (bytes < 0) {
        close(sock);
        return CMD_CHANNEL_SOC_SEND_FAILURE;
    }

    packet->pkt_size = bytes;

    close(sock);

    return 0;
}

//int process_request(struct cmd_interface *intf, struct cmd_interface_msg *request) {

//}

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
    int status;
    struct mctp_interface mctp;
    struct cmd_interface cmd_cerberus;
    struct cmd_interface cmd_mctp;
    struct cmd_interface cmd_spdm;
    struct device_manager device_mgr;
    struct cmd_channel channel;
    channel.send_packet = send_packet;
    channel.receive_packet = receive_packet;
    channel.id = 2;

    TEST_START;

    status = device_manager_init(&device_mgr, 1, 1, DEVICE_MANAGER_PA_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE, 
                1000, 1000, 1000, 1000, 1000, 1000, 5);

    CuAssertIntEquals(test, 0, status);

    status = mctp_interface_init(&mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, &device_mgr);

    CuAssertIntEquals(test, 0, status);

    status = cmd_channel_receive_and_process(&channel, &mctp, 5000);

    CuAssertIntEquals(test, 0, status);


}

TEST_SUITE_START (pldm_fw_update);

TEST (pldm_fw_update_test_place_holder);
TEST (pldm_fw_update_test_request_update_resp);

TEST_SUITE_END;