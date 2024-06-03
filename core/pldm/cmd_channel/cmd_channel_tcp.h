#ifndef COMMAND_CHANNEL_TCP_H_
#define COMMAND_CHANNEL_TCP_H_


#include <stdint.h>
#include <string.h>
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/cmd_interface.h"

enum {
    CMD_CHANNEL_CREATE_SOC_ERROR = -1000,
    CMD_CHANNEL_SET_SOC_OPT_ERROR,
    CMD_CHANNEL_SOC_BIND_ERROR,
    CMD_CHANNEL_SOC_LISTEN_ERROR,
    CMD_CHANNEL_SOC_ACCEPT_ERROR,
    CMD_CHANNEL_SOC_INET_ERROR,
    CMD_CHANNEL_SOC_CONNECT_ERROR,
    CMD_CHANNEL_FCNTL_ERROR,
};

int initialize_global_server_socket();
int send_packet(struct cmd_channel *channel, struct cmd_packet *packet);
int receive_packet(struct cmd_channel *channel, struct cmd_packet *packet, int ms_timeout);
void close_global_server_socket();

#endif /* COMMAND_CHANNEL_TCP_H_ */