#!/bin/bash

echo "# Reset eMMC partitions and generate new GPT......."
sgdisk -g -o /dev/mmcblk0
echo "done."


echo "# Set 8GB eMMC partitions......."

sgdisk -n  1:00002048:00677887 -c  1:"boot"               	 /dev/mmcblk0 	 #  330 MB 
sgdisk -n  2:00677888:00743423 -c  2:"recovery"           	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  3:00743424:00808959 -c  3:"recovery_backup"    	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  4:00808960:00874495 -c  4:"factory_settings"   	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  5:00874496:00878591 -c  5:"splash"             	 /dev/mmcblk0 	 #    2 MB 
sgdisk -n  6:00878592:02107391 -c  6:"system"             	 /dev/mmcblk0 	 #  600 MB 
sgdisk -n  7:02107392:03131391 -c  7:"linknet_home"       	 /dev/mmcblk0 	 #  500 MB 
sgdisk -n  8:03131392:03196927 -c  8:"diagnostics"        	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  9:03196928:03262463 -c  9:"vendordata"         	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n 10:03262464:03264511 -c 10:"security_storage"   	 /dev/mmcblk0 	 #    1 MB 
sgdisk -n 11:03264512:04775935 -c 11:"cache"              	 /dev/mmcblk0 	 #  738 MB 
sgdisk -n 12:04775936:15267839 -c 12:"userdata"           	 /dev/mmcblk0 	 # 5123 MB 


sgdisk -p /dev/mmcblk0

echo "eMMC partitions are generated successfully."

dd if=/dev/mmcblk0 of=gpt.dump bs=1048576 count=1
dd if=/dev/mmcblk0 of=gpt_backup.dump bs=512 skip=15267840
echo "8GB eMMC gpt dump are generated successfully."
