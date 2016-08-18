#!/bin/bash

###############################################
# script generation tool for setting emmc gpt
# author: sunny an
###############################################

start_off=0
prev_off=2048
IFS=,
no=1
emmc_size="8GB"

gpt_tbl=$1

set_emmc_sh="set_emmc_part.sh"

usage()
{
    echo "Usage: gpt_maker.sh GPT.TBL"
}

generate_script()
{
    echo "#!/bin/bash"
    echo ""
    echo "echo \"# Reset eMMC partitions and generate new GPT.......\""
    echo "sgdisk -g -o /dev/mmcblk0"
    echo "echo \"done.\""
    echo ""
    echo ""
    echo "echo \"# Set $emmc_size eMMC partitions.......\""
    echo ""
    while read part_name size
    do
        sector=$(($prev_off + $size * 1048576 / 512))
        _part_name=`printf "\"%s\"" $part_name`
        gpt=`printf "sgdisk -n %2d:%08d:%08d -c %2d:%-20s \t /dev/mmcblk0 \t # %4d MB \n" $no $prev_off $(($sector-1)) $no $_part_name $size`
        echo $gpt
        prev_off=$sector
        no=$(($no+1))
    done < $gpt_tbl

    echo ""
    echo ""
    echo "sgdisk -p /dev/mmcblk0"
    echo ""
    echo "echo \"eMMC partitions are generated successfully.\""
    echo ""

    echo "dd if=/dev/mmcblk0 of=gpt.dump bs=1048576 count=1"
    if [ $emmc_size = "4GB" ]; then
        echo "dd if=/dev/mmcblk0 of=gpt_backup.dump bs=512 skip=7632896"
    else
        echo "dd if=/dev/mmcblk0 of=gpt_backup.dump bs=512 skip=15267840"
    fi
    echo "echo \"$emmc_size eMMC gpt dump are generated successfully.\""
}

if [ -z $gpt_tbl ]; then
    echo "GPT maker script. v1.0"
    echo ""
    echo "Warning: need a paramter!"
    echo ""
    usage
    exit 1
fi

generate_script > $set_emmc_sh

