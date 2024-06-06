#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include "platform_api.h"
#include "cmd_channel_tcp.h"
#include "platform_io.h"


void print_packet_data(const uint8_t *data, size_t len) {
    printf("Packet bytes:");
    for (size_t i = 0; i < len; ++i) {
        printf(" %02x", data[i]);
    }
    printf("\n");
}


int global_server_fd = -1;

int initialize_global_server_socket() {
    if (global_server_fd != -1) {
        return 0;
    }

    struct sockaddr_in address;
    int opt = 1;

    if ((global_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        return CMD_CHANNEL_CREATE_SOC_ERROR;
    }

    if (setsockopt(global_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        close(global_server_fd);
        global_server_fd = -1;
        return CMD_CHANNEL_SET_SOC_OPT_ERROR;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
#if PLDM_TESTING_ENABLE_FIRMWARE_DEVICE == 1
    address.sin_port = htons(PLDM_TESTING_FIRMWARE_DEVICE_PORT);
#else
    address.sin_port = htons(PLDM_TESTING_UPDATE_AGENT_PORT);
#endif

    if (bind(global_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(global_server_fd);
        global_server_fd = -1;
        return CMD_CHANNEL_SOC_BIND_ERROR;
    }

    if (listen(global_server_fd, 100) < 0) {
        close(global_server_fd);
        global_server_fd = -1;
        return CMD_CHANNEL_SOC_LISTEN_ERROR;
    }

    return 0;
}


void close_global_server_socket() {
    close(global_server_fd);
    global_server_fd = -1;
}

/**
* Receive a command packet from a communication channel.  This call will block until a packet
* has been received or the timeout has expired.
*
* @param channel The channel to receive a packet from.
* @param packet Output for the packet data being received.
* @param ms_timeout The amount of time to wait for a received packet, in milliseconds.  A
* negative value will wait forever, and a value of 0 will return immediately.
*
* @return 0 if a packet was successfully received or an error code.
*/
int receive_packet(struct cmd_channel *channel, struct cmd_packet *packet, int ms_timeout) {
    printf("Receiving packet.\n");
    if (global_server_fd == -1) {
        return CMD_CHANNEL_CREATE_SOC_ERROR;
    }

    int client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    struct timeval start, now;
    gettimeofday(&start, NULL);
    long elapsed_ms;

    printf("channel id: %d.\n", channel->id);
    printf("channel overflow: %d.\n", channel->overflow);
    printf("packet size: %zu.\n", packet->pkt_size);
    printf("packet dest_addr: %u.\n", packet->dest_addr);
    printf("packet state: %u.\n", packet->state);
    printf("packet timeout_valid: %d.\n", packet->timeout_valid);

    do {
        client_socket = accept(global_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        gettimeofday(&now, NULL);
        elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;
    } while (client_socket < 0 && elapsed_ms < ms_timeout);
    printf("Accepted connection.\n");

    if (client_socket < 0) {
        platform_printf("Time-out reached.\n");
        return CMD_CHANNEL_SOC_ACCEPT_ERROR;
    }

    ssize_t valread = read(client_socket, packet->data, MCTP_BASE_PROTOCOL_MAX_PACKET_LEN);
    printf("Read packet.\n");
    packet->pkt_size = valread;
    packet->dest_addr = (uint8_t)cmd_channel_get_id(channel);

    close(client_socket);

    return 0;
}



/**
* Send a command packet over a communication channel.
*
* Returning from this function does not guarantee the packet has been fully transmitted.
* Depending on the channel implementation, it is possible the packet is still in flight with
* the data buffered in the channel driver.
*
* @param channel The channel to send a packet on.
* @param packet The packet to send.
*
* @return 0 if the the packet was successfully sent or an error code.
*/
int send_packet(struct cmd_channel *channel, struct cmd_packet *packet) {
    printf("Sending packet.\n");
    struct sockaddr_in serv_addr;
    int sock = 0;
    int ms_timeout = PLDM_TESTING_MS_TIMEOUT;

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

// If Cerberus is the FD then it should connect to the UA port
#if PLDM_TESTING_ENABLE_FIRMWARE_DEVICE == 1
    serv_addr.sin_port = htons(PLDM_TESTING_UPDATE_AGENT_PORT);
#else
    serv_addr.sin_port = htons(PLDM_TESTING_FIRMWARE_DEVICE_PORT);
#endif

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        return CMD_CHANNEL_SOC_INET_ERROR;
    }

    struct timeval start, now;
    gettimeofday(&start, NULL);
    long elapsed_ms = 0;

    printf("channel id: %d.\n", channel->id);
    printf("channel overflow: %d.\n", channel->overflow);
    printf("packet size: %zu.\n", packet->pkt_size);
    printf("packet dest_addr: %u.\n", packet->dest_addr);
    printf("packet state: %u.\n", packet->state);
    printf("packet timeout_valid: %d.\n", packet->timeout_valid);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return CMD_CHANNEL_CREATE_SOC_ERROR;
    }

    while (elapsed_ms < ms_timeout) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return CMD_CHANNEL_CREATE_SOC_ERROR;
        }

        int result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("Connected to server.\n");
        if (result == 0) {
            send(sock, packet->data, packet->pkt_size, 0);
            printf("Sent packet.\n");
            close(sock);
            return 0;
        } else {
            close(sock);
            usleep(100000);
        }

        gettimeofday(&now, NULL);
        elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;
    }

    platform_printf("Time-out reached.\n");
    return CMD_CHANNEL_SOC_CONNECT_ERROR;
}
