#!/bin/bash

echo "# Reset eMMC partitions and generate new GPT......."
sgdisk -g -o /dev/mmcblk0
echo "done."


echo "# Set 8GB eMMC partitions......."

sgdisk -n  1:00002048:00034815 -c  1:"fts"                	 /dev/mmcblk0 	 #   16 MB 
sgdisk -n  2:00034816:00036863 -c  2:"misc"               	 /dev/mmcblk0 	 #    1 MB 
sgdisk -n  3:00036864:00053247 -c  3:"boot"               	 /dev/mmcblk0 	 #    8 MB 
sgdisk -n  4:00053248:00118783 -c  4:"recovery"           	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  5:00118784:00184319 -c  5:"recovery_backup"    	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  6:00184320:00249855 -c  6:"factory_settings"   	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n  7:00249856:00270335 -c  7:"splash"             	 /dev/mmcblk0 	 #   10 MB 
sgdisk -n  8:00270336:02113535 -c  8:"system"             	 /dev/mmcblk0 	 #  900 MB 
sgdisk -n  9:02113536:03137535 -c  9:"linknet_home"       	 /dev/mmcblk0 	 #  500 MB 
sgdisk -n 10:03137536:03203071 -c 10:"diagnostics"        	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n 11:03203072:03268607 -c 11:"vendordata"         	 /dev/mmcblk0 	 #   32 MB 
sgdisk -n 12:03268608:03270655 -c 12:"security_storage"   	 /dev/mmcblk0 	 #    1 MB 
sgdisk -n 13:03270656:04782079 -c 13:"cache"              	 /dev/mmcblk0 	 #  738 MB 
sgdisk -n 14:04782080:15267839 -c 14:"userdata"           	 /dev/mmcblk0 	 # 5120 MB 


sgdisk -p /dev/mmcblk0

echo "eMMC partitions are generated successfully."

dd if=/dev/mmcblk0 of=gpt.dump bs=1048576 count=1
dd if=/dev/mmcblk0 of=gpt_backup.dump bs=512 skip=15267840
echo "8GB eMMC gpt dump are generated successfully."
