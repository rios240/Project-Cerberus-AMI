// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
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

int PLDM_FW_UPDATE_STATE = PLDM_FWUP_NOT_IN_UPDATE_MODE;

void setPLDM_FW_UPDATE_STATE(int new_state) {
    PLDM_FW_UPDATE_STATE = new_state;
}

static void platform_printbuf(uint8_t *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        platform_printf("%02x ", buf[i]);
    }
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

    platform_printf("After processing the MCTP payload/PLDM message the client will send back a response message.\n");
    platform_printf("The client sends the following MCTP message: MCTP header->{");
    platform_printbuf(packet->data, sizeof(struct mctp_base_protocol_transport_header));
    platform_printf("}, MCTP payload/PLDM message->{");
    platform_printbuf(&packet->data[sizeof(struct mctp_base_protocol_transport_header)], sizeof (struct pldm_msg_hdr) + sizeof (struct pldm_request_update_resp) + 1);
    platform_printf("}, and Checksum->{");
    platform_printbuf(&packet->data[sizeof(struct mctp_base_protocol_transport_header) + sizeof (struct pldm_msg_hdr) + sizeof (struct pldm_request_update_resp) + 1], 1);
    platform_printf("}.\n\n");
    
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

    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = ms_timeout / 1000; 
    timeout.tv_usec = (ms_timeout % 1000) * 1000; 

    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    int selectResult = select(sock + 1, &readfds, NULL, NULL, &timeout);

    if (selectResult == -1) {
        close(sock);
        return CMD_CHANNEL_SOC_SELECT_FAILURE;
    } else if (selectResult == 0) {
        close(sock);
        return CMD_CHANNEL_SOC_TIMEOUT;
    }


    ssize_t bytes = recv(sock, packet->data, MCTP_BASE_PROTOCOL_MAX_PACKET_LEN, 0);

    if (bytes < 0) {
        close(sock);
        return CMD_CHANNEL_SOC_SEND_FAILURE;
    }

    packet->pkt_size = bytes;
    packet->dest_addr = 0xAA;

    platform_printf("\nClient received the following MCTP message from the server: \n");
    platform_printbuf(packet->data, packet->pkt_size);
    platform_printf(NEWLINE);

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

    platform_printf("After processing the MCTP message and extracting the payload, the client enters the process request phase.\n\n");
    platform_printf("Decoding the payload/PLDM gives the following fields.\n");
    status = decode_request_update_req(reqMsg, sizeof(struct pldm_request_update_req) + sizeof (struct variable_field), 
                                &completion_code, &max_transfer_size, &num_of_comp, &max_outstanding_transfer_req,
                                &pkg_data_len, &comp_image_set_ver_str_type, &comp_image_set_ver_str_len,
                                &comp_img_set_ver_str);
    if (status != 0) {
        return status;
    }

    platform_printf("MaximumTransferSize: %d, \nNumberOfComponents: %d," 
            "\nMaximumOutstandingTransferRequests: %d, \nPackageDataLength: %d," 
            "\nComponentImageSetVersionStringType: %d, \nComponentImageSetVersionStringLength: %d,"
            "\nComponentImageSetVersionString: %s\n\n",
                max_transfer_size, num_of_comp, 
                max_outstanding_transfer_req, pkg_data_len, 
                comp_image_set_ver_str_type, comp_img_set_ver_str.length, comp_img_set_ver_str.ptr);

    platform_printf("Is the client already in update mode? %s\n", PLDM_FW_UPDATE_STATE == PLDM_FWUP_NOT_IN_UPDATE_MODE ? "False" : "True");
    if (PLDM_FW_UPDATE_STATE == PLDM_FWUP_NOT_IN_UPDATE_MODE) {
        platform_printf("Then the client will enter update mode and set the response completion code to PLDM_BASE_CODES.\n");
        setPLDM_FW_UPDATE_STATE(PLDM_FWUP_ALREADY_IN_UPDATE_MODE);
        completion_code = PLDM_SUCCESS;
    }
    else if (PLDM_FW_UPDATE_STATE == PLDM_FWUP_ALREADY_IN_UPDATE_MODE) {
        platform_printf("Then client sets the PLDM response completion code to PLDM_FWUP_ALREADY_IN_UPDATE_MODE (129).\n");
        completion_code = PLDM_FWUP_ALREADY_IN_UPDATE_MODE;
    }
    else if (PLDM_FW_UPDATE_STATE == PLDM_FWUP_UNABLE_TO_INITIATE_UPDATE) {
        completion_code = PLDM_FWUP_UNABLE_TO_INITIATE_UPDATE;
    }
    else if (PLDM_FW_UPDATE_STATE == PLDM_FWUP_RETRY_REQUEST_UPDATE) {
        completion_code = PLDM_FWUP_RETRY_REQUEST_UPDATE;
    }



    platform_printf("Additionally, was the PackageDataLength > 0? %s\n", pkg_data_len > 0 ? "True" : "False");
    uint8_t fd_will_send_pkg_data;
    if (pkg_data_len > 0) {
        platform_printf("Then the client will set FDWillSendGetPackageDataCommand to 0x01 in the response and send the GetPackageData PLDM command to the server.\n");
        fd_will_send_pkg_data = 0x01;
    } else {
        platform_printf("Then the client will set FDWillSendGetPackageDataCommand to 0x00.\n");
        fd_will_send_pkg_data = 0x00;
    }

    uint8_t instance_id = 2; 
	uint16_t fd_meta_data_len = 0x555;

    uint8_t respBuf[sizeof (struct pldm_msg_hdr) + sizeof (struct pldm_request_update_resp)+1];
    respBuf[0] = MCTP_BASE_PROTOCOL_MSG_TYPE_PLDM;
    struct pldm_msg *respMsg = (struct pldm_msg *)&respBuf[1];

    status = encode_request_update_resp(instance_id, fd_meta_data_len, 
                                fd_will_send_pkg_data, completion_code, respMsg, sizeof(struct pldm_request_update_resp));
    
    memcpy(request->data, respBuf, sizeof (respBuf));
    request->length = sizeof (respBuf);

    platform_printf("After encoding, the PLDM response message will be: PLDM header->{");
    platform_printbuf(&respBuf[1], sizeof (struct pldm_msg_hdr));
    platform_printf("} and PLDM payload->{");
    platform_printbuf(&respBuf[1 + sizeof (struct pldm_msg_hdr)], sizeof (struct pldm_request_update_resp));
    platform_printf("}.\n\n");

    platform_printf("And the payload fields are: \nCompletionCode: %d, \nFirmwareDeviceMetaDataLength: %d," 
            "\nFDWillSendGetPackageDataCommand: %d\n\n",
                completion_code, fd_meta_data_len, fd_will_send_pkg_data);

    
    return status;
}

int process_response(struct cmd_interface *intf, struct cmd_interface_msg *response) {
    return PLDM_ERROR;
}

/**
 * Test Functions
*/

static void pldm_fw_update_test_place_holder(CuTest *test)
{
    TEST_START;

    CuAssertIntEquals(test, 0, 0);
}

static void pldm_fw_update_test_request_update_resp_good_req(CuTest *test)
{
    int status;
    struct mctp_interface mctp;
    struct cmd_interface cmd_cerberus;
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
    CuAssertIntEquals(test, 0, status);

    device_mgr.entries->eid = 0xCC;
    device_mgr.entries->smbus_addr = 0xAA;

    platform_printf("\nClient initializes the MCTP interface.\n");
    status = mctp_interface_init(&mctp, &cmd_cerberus, &cmd_mctp, &cmd_spdm, &device_mgr);
    CuAssertIntEquals(test, 0, status);

    platform_printf("Client then waits to receive the PLDM over MCTP binding message from the server to process and send back a response.");
    status = cmd_channel_receive_and_process(&channel, &mctp, 30000);
    CuAssertIntEquals(test, 0, status);


}

TEST_SUITE_START (pldm_fw_update);

TEST (pldm_fw_update_test_place_holder);
TEST (pldm_fw_update_test_request_update_resp_good_req);

TEST_SUITE_END;