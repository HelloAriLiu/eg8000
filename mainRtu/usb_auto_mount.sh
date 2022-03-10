#!/bin/bash
###
 # @Description : 
 # @FilePath: /mainRtu/usb_auto_mount.sh
 # @Author:  LR
 # @Date: 2021-05-24 14:33:55
### 
# usb 固件自动升级脚本      @只支持FAT32格式文件系统的U盘

#挂载usb
usb_disk_path="/dev/sdb1"
usb_mount_path="/mnt/usb_disk"
#usb更新标志
usb_is_upgrade=0

#挂载SD
sd_disk_path="/dev/sda1"
sd_mount_path="/mnt/sd_disk"
#usb更新标志
sd_is_upgrade=0


while true
do 
    if [ -b $usb_disk_path ];then   #判断u盘是否插入
        if [ $usb_is_upgrade -eq 0 ];then #判断第一次插入U盘
            echo "检测U盘成功！！！"
            mkdir -p $usb_mount_path
            umount $usb_disk_path
            mount $usb_disk_path  $usb_mount_path  -t  vfat  -o  iocharset=utf8
            echo "U盘挂载成功！！！"
            usb_is_upgrade=1
            #sleep 1
        fi
    else
    	#echo "no usb！！！"
        usb_is_upgrade=0
    fi

    if [ -b $sd_disk_path ];then   #判断sd是否插入
        if [ $sd_is_upgrade -eq 0 ];then #判断第一次插入sd
            echo "检测SD卡成功！！！"
            mkdir -p $sd_mount_path
            umount $sd_disk_path
            mount $sd_disk_path  $sd_mount_path  -t  vfat  -o  iocharset=utf8
            echo "SD卡挂载成功！！！"
            sd_is_upgrade=1
        fi
    else
    	#echo "no sd！！！"
        sd_is_upgrade=0
    fi

    sleep 2
done
