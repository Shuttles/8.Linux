#!/bin/bash

time=$(date "+%Y/%m/%d__%H.%M.%S")
echo -n "${time} "
echo -n "${hostname}"
os_version=$(cat /etc/os-release | grep 'PRETTY_NAME'  | awk -F= '{print($2)}' | tr -d """")
echo -n "${os_version} "
start_time=$(w | cut -c 14-26 | head -1)
echo -n "${start_time} "
load_avg=$(cat /proc/loadavg | awk '{print($1,$2,$3)}')
echo -n "${load_avg}"
Disk_total=$(df -h -T -x devtmpfs -x tmpfs -x fuseblk -m | tail -n +2 | awk '{sum1+=$3} END {print sum1}')
Disk_unuse=$(df -h -T -x devtmpfs -x tmpfs -x fuseblk -m | tail -n +2 | awk '{sum2+=$5} END {print sum2}')
#echo -n $UNUSE " "
echo -n "${Disk_total} "
array=($Disk_total $Disk_unuse)
disk=$(echo array | awk -v total=$Disk_total -v unuse=$Disk_unuse '{printf("%.2f", unuse/total*100)}')
echo -n $disk "% "
Mem_Total=$(free -m | grep 'Mem' | awk '{print($2)}')
Mem_Free=$(free -m | grep 'Mem' | awk '{print($4)}')
rate=`echo "scale=2;1-${Mem_Free}/${Mem_Total}*1" | bc`

echo -n "${Mem_Total} "
echo -n "${rate} "

TEMPERATURE=$(cat /sys/class/thermal/thermal_zone0/temp)
DUE=$(echo "${TEMPERATURE}")
DUE=$[$DUE / (1000)]
echo -n $DUE "℃  "

#if [ ($disk>90)>0 | bc]
#then 
#    echo "warning"
#fi
#if [ ($disk>80)>0 | bc] && [ ($disk<90)>0 | bc]
#then
#    echo "note"
#fi
#if [ ($disk<80)>0 ]
#then
#    echo "normal"
#fi


#if [ $DUE -gt 70 ]
#then 
#    echo "warning"
#fi
#if [ $DUE -gt 50 ] && [ $DUE -lt 70 ]
#then
#    echo "note"
#fi
#if [ $DUE -lt 50 ]
#then
#    echo "normal"
#fi
#
#if [ $rate -gt 80 ]
#then 
#    echo "warning"
#fi
#if [ $rate -gt 70 ] && [ $rate -lt 80 ]
#then
#    echo "note"
#fi
#if [ $rate -lt 70 ]
#then
#    echo "normal"
#fi


