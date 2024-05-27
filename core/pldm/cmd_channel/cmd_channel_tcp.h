#ifndef COMMAND_CHANNEL_TCP_H_
#define COMMAND_CHANNEL_TCP_H_


#include <stdint.h>
#include <string.h>
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/cmd_interface.h"

#define CMD_CHANNEL_SOCKET_ERROR            -1000

#define CMD_CHANNEL_SOCKET_PATH             "/s/bach/j/under/tylerios/tmp/cmd_channel.sock"

#define CMD_CHANNEL_PORT                    5000

int send_packet(struct cmd_channel *channel, struct cmd_packet *packet);
int receive_packet(struct cmd_channel *channel, struct cmd_packet *packet, int ms_timeout);

#endif /* COMMAND_CHANNEL_TCP_H_ */