#!/bin/sh
###
 # @Description :  
 # @FilePath: /mainRtu/usr_get_cpuInfo.sh
 # @Author:  LR
 # @Date: 2021-12-01 16:22:17
### 
#
#脚本功能描述：依据/proc/stat文件获取并计算CPU使用率
#
#CPU时间计算公式：CPU_TIME=user+system+nice+idle+iowait+irq+softirq
#CPU使用率计算公式：cpu_usage=(idle2-idle1)/(cpu2-cpu1)*100







while true
do
    #默认时间间隔
    TIME_INTERVAL=5
    #time=$(date "+%Y-%m-%d %H:%M:%S")
    LAST_CPU_INFO=$(cat /proc/stat | grep -w cpu | awk '{print $2,$3,$4,$5,$6,$7,$8}')
    LAST_SYS_IDLE=$(echo $LAST_CPU_INFO | awk '{print $4}')
    LAST_TOTAL_CPU_T=$(echo $LAST_CPU_INFO | awk '{print $1+$2+$3+$4+$5+$6+$7}')
    sleep ${TIME_INTERVAL}
    NEXT_CPU_INFO=$(cat /proc/stat | grep -w cpu | awk '{print $2,$3,$4,$5,$6,$7,$8}')
    NEXT_SYS_IDLE=$(echo $NEXT_CPU_INFO | awk '{print $4}')
    NEXT_TOTAL_CPU_T=$(echo $NEXT_CPU_INFO | awk '{print $1+$2+$3+$4+$5+$6+$7}')

    #系统空闲时间
    SYSTEM_IDLE=`echo ${NEXT_SYS_IDLE} ${LAST_SYS_IDLE} | awk '{print $1-$2}'`
    #CPU总时间
    TOTAL_TIME=`echo ${NEXT_TOTAL_CPU_T} ${LAST_TOTAL_CPU_T} | awk '{print $1-$2}'`
    CPU_USAGE=`echo ${SYSTEM_IDLE} ${TOTAL_TIME} | awk '{printf "%.2f", 100-$1/$2*100}'`

    sudo echo "${CPU_USAGE}" > /home/pi/edge8000/app/cpu_usage.log
    #echo "CPU=${CPU_USAGE}"

    LAST_MEM_TOTAL=$(free | grep "Mem:" |awk '{print $2}')
    LAST_MEM_IDLE=$(free | grep "Mem:" |awk '{print $7}')
    MEM_USAGE=`echo ${LAST_MEM_IDLE} ${LAST_MEM_TOTAL}  | awk '{printf "%.2f", 100-$1/$2*100}'`
    sudo echo "${MEM_USAGE}" >  /home/pi/edge8000/app/mem_usage.log
    #echo "MEM=${MEM_USAGE}"

    LAST_FLASH_USED=$(df | grep "/dev/root" |awk '{print $5}')
    sudo echo "${LAST_FLASH_USED}" > /home/pi/edge8000/app/flash_usage.log
    #echo "FLASH=${LAST_FLASH_USED}"

done

