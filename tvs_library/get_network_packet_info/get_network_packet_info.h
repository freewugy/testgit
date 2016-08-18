// get_network_packet_info.h

#ifndef GET_NETWORK_PACKET_INFO_H
#define GET_NETWORK_PACKET_INFO_H

typedef struct _NetworkPacketInfo
{
	long long	network_total_packets;
	long long	network_lost_packets;
} NetworkPacketInfo;

NetworkPacketInfo get_network_packet_info(const char *ifName);

#endif	// GET_NETWORK_PACKET_INFO_H
