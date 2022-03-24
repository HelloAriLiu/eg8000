#!/bin/bash
###
 # @Description :  
 # @FilePath: /app_code/mainRtu/usrapp_auto_startup.sh
 # @Author:  LR
 # @Date: 2021-12-01 11:11:48
### 
# 启动脚本



brctl addbr br-lan
brctl addif br-lan eth1
ifconfig eth1 0.0.0.0
ifconfig br-lan up

sudo iptables -t nat -A PREROUTING -p tcp --dport 80 -j REDIRECT --to-port 8000

sudo mv  /dev/random /dev/random.orig
sudo ln  -s  /dev/urandom /dev/random
#dhcp
sudo rm -rf /var/lib/misc/*
sudo touch /var/lib/misc/udhcpd.leases


APP_SERVICE_PATH="/home/pi/edge8000/app/"
cd $APP_SERVICE_PATH

CPUINFO_SHELL="usr_get_cpuInfo.sh"
sudo chmod +x $CPUINFO_SHELL
ps -efww|grep -w 'usr_get_cpuInfo.sh'|grep -v grep|awk '{print $2}'|xargs sudo kill -9
sudo ./$CPUINFO_SHELL &


#加载启动APP
APP_SERVICE_NAME="zh_node.app"
APP_START_CMD="./$APP_SERVICE_NAME &"
sudo chmod +x $APP_SERVICE_NAME
sudo ps -efww|grep -w 'zh_node.app' |grep -v grep|awk '{print $2}'|xargs sudo kill -9

while true
do
    app_procnum=`ps -ef|grep $APP_SERVICE_NAME |grep -v grep|wc -l`
    if [ $app_procnum -eq 0 ]
    then
        echo "\n<<<start App service>>>"
        sudo ./$APP_SERVICE_NAME &
    fi
    sleep 600
    
done
