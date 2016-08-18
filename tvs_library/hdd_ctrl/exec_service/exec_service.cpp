// exec_service.cpp

#include <sys/mount.h>
#include <fcntl.h>

#include "exec_service.h"

#define LOG_TAG "EXEC_SERVICE"
#include "cutils/log.h"

void CheckLknHomeThread::onFirstRef()
{
    //ALOGD("CheckLknHomeThread::onFirstRef()\n");
}

status_t CheckLknHomeThread::readyToRun()
{
	//ALOGD("CheckLknHomeThread::readyToRun()\n");
	return 0;
}

bool CheckLknHomeThread::threadLoop()
{
	//ALOGD("CheckLknHomeThread::threadLoop()\n");

	if (access("/data/lkn_home/cts_mode", F_OK) >= 0)
	{
		if (IsHddCtrlAlive() == 1)
		{
			if (property_set("ctl.stop", "hdd_control") < 0)
				ALOGD("hdd_control stop: Failed\n");
			else
				ALOGD("hdd_control stop: Succeeded\n");
		}
	}
	else
	{
		if (IsHddCtrlAlive() == 0)
		{
			if (property_set("ctl.start", "hdd_control") < 0)
				ALOGD("hdd_control start: Failed\n");
			else
				ALOGD("hdd_control start: Succeeded\n");
		}
	}

	usleep(1500000); // 1500ms
	//sleep(3); // 3 sec

	return true;
}

int CheckLknHomeThread::IsHddCtrlAlive()
{
	//system("ps | grep hdd_ctrl > /data/lkn_home/is_hdd_ctrl_alive.txt");
	//system("ps | grep hdd_ctrl > /mnt/obb/is_hdd_ctrl_alive.txt");
	usleep(1000000);    //1000ms

	FILE *fp;
	char line[256];

	//if (!(fp = fopen("/data/lkn_home/is_hdd_ctrl_alive.txt", "r")))
	if (!(fp = fopen("/mnt/obb/is_hdd_ctrl_alive.txt", "r")))
	{
		//ALOGE("Error opening /data/lkn_home/is_hdd_ctrl_alive.txt (%s)", strerror(errno));
		ALOGE("Error opening /mnt/obb/is_hdd_ctrl_alive.txt (%s)", strerror(errno));
		return -1;
	}

	if (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		if (strstr(line, "hdd_ctrl")) {
			fclose(fp);
			//system("rm -rf /data/lkn_home/is_hdd_ctrl_alive.txt");
			system("rm -rf /mnt/obb/is_hdd_ctrl_alive.txt");
			usleep(1000000);    //1000ms
			ALOGD("=========> hdd_ctrl is alive");
			return 1;
		}
		else
		{
			fclose(fp);
			//system("rm -rf /data/lkn_home/is_hdd_ctrl_alive.txt");
			system("rm -rf /mnt/obb/is_hdd_ctrl_alive.txt");
			usleep(1000000);    //1000ms
			ALOGD("=========> hdd_ctrl is not alive");
			return 0;
		}
	}

	fclose(fp);
	//system("rm -rf /data/lkn_home/is_hdd_ctrl_alive.txt");
	system("rm -rf /mnt/obb/is_hdd_ctrl_alive.txt");
	usleep(1000000);    //1000ms
	ALOGD("IsHddCtrlAlive(): =========> hdd_ctrl is not alive");
	return 0;
}

void TVSFirewallRunThread::onFirstRef()
{
    //ALOGD("TVSFirewallRunThread::onFirstRef()\n");
}

status_t TVSFirewallRunThread::readyToRun()
{
	//ALOGD("TVSFirewallRunThread::readyToRun()\n");
	return 0;
}

bool TVSFirewallRunThread::threadLoop()
{
	//ALOGD("TVSFirewallRunThread::threadLoop()\n");

	while (1)
	{
		char dev_bootcomplete_prop[PROPERTY_VALUE_MAX] = {'\0'};
		if (property_get("dev.bootcomplete", dev_bootcomplete_prop, NULL) &&
			!strcmp(dev_bootcomplete_prop, "1"))
		{
			system("/system/bin/sh /lkndate.sh");
			break;
		}
		usleep(1000000);	//1000ms
	}

	while (1)
	{
		char tvs_tvapp_on_prop[PROPERTY_VALUE_MAX] = {'\0'};
		if (property_get("tvs.tvapp.on", tvs_tvapp_on_prop, NULL) &&
			!strcmp(tvs_tvapp_on_prop, "1"))
		{
			break;
		}
		usleep(1000000);	//1000ms
	}

	char tvs_firewall_prepare_prop[PROPERTY_VALUE_MAX] = {'\0'};
	property_get("tvs.firewall.prepare", tvs_firewall_prepare_prop, NULL);
	if (!strcmp(tvs_firewall_prepare_prop, "1"))
	{
		sleep(60);
		property_set("tvs.firewall.run", "1");
	}

#if 0
    /////////////////////////////////////////////////////////////////////
    // for selinux
	while (1)
	{
		char prop[PROPERTY_VALUE_MAX] = {'\0'};
		if (property_get("sys.boot_completed", prop, NULL) &&
			!strcmp(prop, "1"))
		{
		    sleep(2);
            if(access("/data/lkn_home/cts_mode", F_OK) == 0)
            {
			    system("stop adbd");
			    system("start adbd");
            }
			break;
		}
		usleep(1000000);	//1000ms
	}
#endif
	return false;
}

bool isMountedWithRWMode(const char *argMountPath)
{
	char device[256];
	char mount_path[256];
	char fsType[256];
	char accessMode[256];
	char rest[256];
	FILE *fp;
	char line[1024];

	if (!(fp = fopen("/proc/mounts", "r")))
	{
		ALOGE("Error opening /proc/mounts (%s)", strerror(errno));
		return false;
	}

	while (fgets(line, sizeof(line), fp))
	{
		line[strlen(line)-1] = '\0';
		sscanf(line, "%255s %255s %255s %255s %255s\n", device, mount_path, fsType, accessMode, rest);
		if (!strcmp(mount_path, argMountPath) && !strcmp("ext4", fsType) && !strncmp("rw", accessMode, 2))
		{
			fclose(fp);
			ALOGD("=========> isMountedWithRWMode(): return true");
			return true;
		}
	}

	fclose(fp);
	return false;
}

bool TVSCheckRouteTableThread::needToCheckEth = true;
bool TVSCheckRouteTableThread::needToCheckWlan = true;

void TVSCheckRouteTableThread::onFirstRef()
{
    //ALOGD("CheckLknHomeThread::onFirstRef()\n");
}

status_t TVSCheckRouteTableThread::readyToRun()
{
	//ALOGD("CheckLknHomeThread::readyToRun()\n");
	return 0;
}

bool TVSCheckRouteTableThread::threadLoop()
{
	char eth0_result[PROPERTY_VALUE_MAX] = {'\0'};
	char wlan0_result[PROPERTY_VALUE_MAX] = {'\0'};

	if ( needToCheckEth && property_get("dhcp.eth0.result", eth0_result, NULL) && strcmp(eth0_result, "ok") == 0)
	{
		needToCheckEth = false;
		char gw_prop[PROPERTY_VALUE_MAX] = {'\0'};
		property_get("dhcp.eth0.gateway", gw_prop, NULL);
		char ip_prop[PROPERTY_VALUE_MAX] = {'\0'};
		property_get("dhcp.eth0.ipaddress", ip_prop, NULL);
		int dot_cnt = 0;
		int index = 0;
		int size = strlen(ip_prop);
		for (index = 0; index < size; index++)
		{
			if (ip_prop[index] == '.') dot_cnt++;
			if (dot_cnt == 3)
			{
				break;
			}
		}
		if (index < size)
			ip_prop[index] = '\0';
	}
	else
	{
		if (property_get("dhcp.eth0.result", eth0_result, NULL) && strcmp(eth0_result, "ok") != 0)
			needToCheckEth = true;
	}

	if (needToCheckWlan && property_get("dhcp.wlan0.result", wlan0_result, NULL) && strcmp(wlan0_result, "ok") == 0 )
	{
		needToCheckWlan = false;
		char gw_prop[PROPERTY_VALUE_MAX] = {'\0'};
		property_get("dhcp.wlan0.gateway", gw_prop, NULL);
		char ip_prop[PROPERTY_VALUE_MAX] = {'\0'};
		property_get("dhcp.wlan0.ipaddress", ip_prop, NULL);
		int dot_cnt = 0;
		int index = 0;
		int size = strlen(ip_prop);
		for (index = 0; index < size; index++)
		{
			if (ip_prop[index] == '.') dot_cnt++;
			if (dot_cnt == 3)
			{
				break;
			}
		}
		if (index < size)
			ip_prop[index] = '\0';
	}
	else
	{
		if (property_get("dhcp.wlan0.result", wlan0_result, NULL) && strcmp(wlan0_result, "ok") != 0)
            needToCheckWlan = true;
	}

	usleep(1000000); // 1000ms
	return true;
}

#define SDCARD_USB_PATH         "/storage/sdcard1/usb_sh960c-ln"
#define SERVERLIST_CONF         SDCARD_USB_PATH"/server-list.conf"
#define CM_MAC_ADDR             SDCARD_USB_PATH"/CM_MAC_ADDR"
#define LKN_HOME_CONFIG         "/data/lkn_home/config"
#define FACTORY_SETTINGS        "/factory_settings"

#ifdef __cplusplus
extern "C"
{
#endif

int TVSTORM_Front_Led_Init(void);
int TVSTORM_Front_Led_Uninit(void);
int TVSTORM_Front_Led_Control(int tvs_led, int state); /* NEXUS_GpioValue gpio_value */

#ifdef __cplusplus
}
#endif

int power_led_blink(int ok)
{
#define LED_POWER_GREEN     0
#define LED_POWER_RED       1
    
#define TVS_LED_OFF         0
#define TVS_LED_ON          1

#define BLINK_INTERVAL      (250*1000)

    ALOGI("power_led_blink %d", ok);

    property_set("ctl.stop", "tvs_front_led");
	if (TVSTORM_Front_Led_Init() != 0) {
	    ALOGE("front init failed");
	    return -1;
    }

    TVSTORM_Front_Led_Control(LED_POWER_RED, TVS_LED_OFF);
    TVSTORM_Front_Led_Control(LED_POWER_GREEN, TVS_LED_OFF);
    for(int i=0; i<20; i++)
    {
        TVSTORM_Front_Led_Control(ok?LED_POWER_GREEN:LED_POWER_RED, TVS_LED_ON);
        usleep(BLINK_INTERVAL);
        TVSTORM_Front_Led_Control(ok?LED_POWER_GREEN:LED_POWER_RED, TVS_LED_OFF);
        usleep(BLINK_INTERVAL);
    }

    TVSTORM_Front_Led_Uninit();
    property_set("ctl.start", "tvs_front_led");
    return 0;
}

/*
 * 양산 소프트웨어인 경우 sus server를 통한 online upgrade 테스트가 국내에서는 불가하므로
 * 국내에서 테스트가 가능하도록 CM mac address와 sus server address를 수정할 수 있도록 함.
 * dev_mode 이고 USB 메모리에 CM_MAC_ADDR 파일과 server-list.conf 파일이 존재하는 경우
 * 2 파일을 각각 /factory_settings 디렉토리와 /data/lkn_home/config 디렉토리 복사함
 */
int CheckDevModeThread::prepare_sus_test(void)
{
    int result, timeout = 120;
	char value[PROPERTY_VALUE_MAX] = {'\0'};
    char cmd[512];
    int serverlist_ok = 0, cm_ok = 0;

    while(1)
    {
        if (property_get("tvs.tvapp.initialize", value, NULL) && !strcmp(value, "true"))
        {
            if (property_get("init.svc.fuse_sdcard1", value, NULL) && !strcmp(value, "running"))
            {
                ALOGI("sdcard1 inserted");
                break;
            }
        }
        //ALOGI("waiting sys.boot_completed \n");
        sleep(1);
    }

#if 0
    if(!timeout)
    {
        ALOGE("timeout !!!");
        return -1;
    }
#endif

    if(!access(SERVERLIST_CONF, F_OK))
    {
        sprintf(cmd, "/system/bin/cp -f %s %s", SERVERLIST_CONF, LKN_HOME_CONFIG);
        result = system(cmd);
        if (result != 0) {
            ALOGE("cp server-list.conf failed on %s with %d(%s)\n", result, errno, strerror(errno));
        }
        else
        {
            serverlist_ok = 1;
        }
    }
    else
    {
        ALOGE("File1 not found!");
    }

    if(!access(CM_MAC_ADDR, F_OK))
    {
        sprintf(cmd, "vdc tvs remount rw %s", FACTORY_SETTINGS);
        result = system(cmd);
        if(result != 0)
        {
            ALOGE("remount return %d (%s)\n", result, strerror(errno));
        }
        else
        {
            sprintf(cmd, "/system/bin/cp -f %s %s", CM_MAC_ADDR, FACTORY_SETTINGS);
            result = system(cmd);
            if (result != 0) {
                ALOGE("cp CM_MAC_ADDR failed on %s with %d(%s)\n", result, strerror(errno));
            }
            else
            {
                cm_ok = 1;
            }
        }
    }
    else
    {
        ALOGE("File2 not found!");
    }

    power_led_blink(serverlist_ok && cm_ok);

    return 0;
}

void CheckDevModeThread::onFirstRef()
{
}

status_t CheckDevModeThread::readyToRun()
{
    return 0;
}

bool CheckDevModeThread::threadLoop()
{
    if(access("/data/lkn_home/dev_mode", F_OK)) return false;

    prepare_sus_test();
    return false;
}

int main()
{
	bool flag = false;

	sp<CheckDevModeThread> pCheckDevModeThread = new CheckDevModeThread;
	pCheckDevModeThread->run();

	sp<TVSFirewallRunThread> pTVSFirewallRun = new TVSFirewallRunThread;
	pTVSFirewallRun->run();
	//sp<TVSCheckRouteTableThread> pCheckRouteTableRun = new TVSCheckRouteTableThread;
	//pCheckRouteTableRun->run();

	pTVSFirewallRun->join();
	pCheckDevModeThread->join();
	//pCheckRouteTableRun->join();

	return 0;
}
