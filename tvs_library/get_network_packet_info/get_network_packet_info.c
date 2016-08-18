// get_network_packet_info.c 

#include "get_network_packet_info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#undef LOG_TAG
#define LOG_TAG	"GET_NETWORK_PACKET_INFO"
#include "cutils/log.h"

//#undef ALOGD
//#define ALOGD(...)

NetworkPacketInfo get_network_packet_info(const char *ifName)
{
	NetworkPacketInfo networkPacketInfo;

	memset(&networkPacketInfo, 0, sizeof(NetworkPacketInfo));

	char ifname[256];
	char rx_bytes[256];
	char rx_packets[256];
	char rx_errs[256];
	char rx_drop[256];
	char rest[256];
	FILE *fp;
	char line[1024];

	if (!(fp = fopen("/proc/net/dev", "r")))
	{
		ALOGE("Error opening /proc/net/dev (%s)", strerror(errno));
		return networkPacketInfo;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s %255s %255s %255s\n", ifname, rx_bytes, rx_packets, rx_errs, rx_drop, rest);
		ifname[strlen(ifname)-1] = '\0';
		if (!strcmp(ifname, ifName))
		{
			fclose(fp);
			networkPacketInfo.network_total_packets = atoll(rx_packets);
			networkPacketInfo.network_lost_packets = atoll(rx_errs) + atoll(rx_drop);
			ALOGD("=========> get_network_packet_info(%s): network_total_packets=[%lld], network_lost_packets=[%lld]", ifName, networkPacketInfo.network_total_packets, networkPacketInfo.network_lost_packets);
			return networkPacketInfo;
		}
	}

	fclose(fp);
	return networkPacketInfo;
}
