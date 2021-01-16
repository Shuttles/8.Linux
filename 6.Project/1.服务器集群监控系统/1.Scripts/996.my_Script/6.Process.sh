#!/bin/bash

time=`date "+%Y/%m/%d__%H.%M.%S"`

eval `ps -aux --sort -%cpu -h | awk -v num=0 '{if($3 < 50) {exit} else {num++; printf("CpuProcId["num"]=%d;", $2)}}'`
#echo ${CpuProcId[*]}
TimeInterval=5
sleep ${TimeInterval}
eval `ps -aux --sort -%cpu -h | awk -v num1=0 '{if($3 < 50) {exit} else {num1++; printf("CpuId["num1"]=%d;", $2)}}'`
#echo ${CpuId[*]}
for i in ${CpuId[*]}
do    
    for j in ${CpuProcId[*]}
    do    
        if [ $i -eq $j ];
        then
            echo -n "${time}"
            name=$(ps -aux | awk -v pid=$i '{if($2==pid)print($11,$1,$2,$3,$4)}')
            echo -n "${name}"
        fi
    done
done

