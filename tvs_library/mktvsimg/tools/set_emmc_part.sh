#!/bin/bash

echo "# Reset eMMC partitions and generate new GPT......."
sgdisk -g -o /dev/mmcblk0
echo "done."


echo "# Set 8GB eMMC partitions......."

sgdisk -n  1:00002048:00067583 -c  1:"recovery"           	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  2:00067584:00133119 -c  2:"recovery_backup"    	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  3:00133120:00788479 -c  3:"boot"               	 /dev/mmcblk0 	 #  320 MB 
sgdisk -n  4:00788480:00854015 -c  4:"diagnostics"        	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  5:00854016:00858111 -c  5:"splash"             	 /dev/mmcblk0 	 #    2 MB 
sgdisk -n  6:00858112:00923647 -c  6:"factory_settings"   	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  7:00923648:02254847 -c  7:"system"             	 /dev/mmcblk0 	 #  650 MB 
sgdisk -n  8:02254848:02256895 -c  8:"security_storage"   	 /dev/mmcblk0 	 #    1 MB 
sgdisk -n  9:02256896:02625535 -c  9:"linknet_home"       	 /dev/mmcblk0 	 #  180 MB 
sgdisk -n 10:02625536:04782079 -c 10:"cache"              	 /dev/mmcblk0 	 # 1053 MB 
sgdisk -n 11:04782080:15267839 -c 11:"userdata"           	 /dev/mmcblk0 	 # 5120 MB 


sgdisk -p /dev/mmcblk0

echo "eMMC partitions are generated successfully."

dd if=/dev/mmcblk0 of=gpt.dump bs=1048576 count=1
dd if=/dev/mmcblk0 of=gpt_backup.dump bs=512 skip=15267840
echo "8GB eMMC gpt dump are generated successfully."
