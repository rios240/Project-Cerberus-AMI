#ifndef COMMAND_CHANNEL_TCP_H_
#define COMMAND_CHANNEL_TCP_H_


#include <stdint.h>
#include "cmd_interface/cmd_channel.h"
#include "cmd_interface/cmd_interface.h"

int send_packet(struct cmd_channel *channel, struct cmd_packet *packet);
int receive_packet(struct cmd_channel *channel, struct cmd_packet *packet, int ms_timeout);

#endif /* COMMAND_CHANNEL_TCP_H_ */