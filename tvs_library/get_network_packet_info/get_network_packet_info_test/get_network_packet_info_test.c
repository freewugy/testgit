// get_network_packet_info_test.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

typedef struct _NetworkPacketInfo
{
	long long	network_total_packets;
	long long	network_lost_packets;
} NetworkPacketInfo;

NetworkPacketInfo get_network_packet_info(const char *ifName);

int main()
{
	char buf[10];

	void *handle;

	NetworkPacketInfo	(*get_network_packet_info_func)(char*);

	handle = dlopen("/system/lib/libgetnetworkpacketinfo.so", RTLD_NOW);

	get_network_packet_info_func = (NetworkPacketInfo(*)(char*)) dlsym( handle, "get_network_packet_info" );

	memset(buf, 0x00, 10);
	printf(">> ");
	while(fgets(buf, 10, stdin) != NULL)
	{
		switch(atoi(buf))
		{
	
			case 1:
				{
					NetworkPacketInfo networkPacketInfo = get_network_packet_info_func("eth0");
					printf("------------------> get_network_packet_info\n");
					printf("---------> network_total_packets [%lld]\n", networkPacketInfo.network_total_packets);
					printf("---------> network_lost_packets [%lld]\n", networkPacketInfo.network_lost_packets);
				}
				break;
			default:
				break;
		}
		memset(buf, 0x00, 10);
		printf(">> ");
	}

	if(ferror(stdin))
		perror("input error");

	dlclose(handle);

	return 0;
}
