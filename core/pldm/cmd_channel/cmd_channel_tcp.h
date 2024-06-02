#ifndef COMMAND_CHANNEL_TCP_H_
#define COMMAND_CHANNEL_TCP_H_


#include <stdint.h>
#include <string.h>
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/cmd_interface.h"

#define CMD_CHANNEL_SOC_PORT                    5000
#define CMD_CHANNEL_SOC_TIMEOUT                 10000

enum {
    CMD_CHANNEL_CREATE_SOC_ERROR = -1000,
    CMD_CHANNEL_SET_NON_BLOCKING_ERROR,
    CMD_CHANNEL_SET_SOC_OPT_ERROR,
    CMD_CHANNEL_SOC_BIND_ERROR,
    CMD_CHANNEL_SOC_LISTEN_ERROR,
    CMD_CHANNEL_SOC_ACCEPT_ERROR,
    CMD_CHANNEL_SOC_INET_ERROR,
    CMD_CHANNEL_SOC_CONNECT_ERROR,
    CMD_CHANNEL_FCNTL_ERROR,
};


int send_packet(struct cmd_channel *channel, struct cmd_packet *packet);
int receive_packet(struct cmd_channel *channel, struct cmd_packet *packet, int ms_timeout);

#endif /* COMMAND_CHANNEL_TCP_H_ */