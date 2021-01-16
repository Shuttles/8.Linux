#!/bin/bash

eval $(ps -aux --sort=-%cpu -h | awk -v num=0 \
    '{if ($3 < 50) {exit} else {num++; printf("Cpupid["num"]=%d\n", $2)}} END {printf("CpuNum=%d", num)}')

eval $(ps -aux --sort=-%cpu -h | awk -v num=0 \
    '{if ($4 < 50) {exit} else {num++; printf("Mempid["num"]=%d\n", $2)}} END {printf("MemNum=%d", num)}')

if [[ ${CpuNum} -gt 0 || ${MemNum} -gt 0 ]];then
    sleep 5
else
    exit 0
fi

Time=`date +"%Y-%m-%d__%H:%M:%S"`
cnt=0

if [[ ${CpuNum} -gt 0 ]];then
    for i in ${Cpupid[*]};do
        eval `ps -aux -h -q $i | awk -v num=${cnt}\
            '{if ($3 < 50) {exit} else {printf("Pname["num"]=%s; Pid["num"]=%d;User["num"]=%s;CpuPer["num"]=%.2f;MemPer["num"]=%.2f", $11, $2, $1, $3, $4)} }'`
        cnt=$[cnt+1]
    done
fi


if [[ ${MemNum} -gt 0 ]];then
    for i in ${Mempid[*]};do
        eval `ps -aux -h -q $i | awk -v num=${cnt}\
            '{if ($4 < 50) {exit} else {printf("Pname["num"]=%s;Pid["num"]=%d;User["num"]=%s;CpuPer["num"]=%.2f;MemPer["num"]=%.2f", $11, $2, $1, $3, $4)} }'`
        cnt=$[cnt+1]
    done
fi

for (( i = 0; i < ${cnt}; i++ ));do
    echo "${Time} ${Pname[$i]} ${Pid[$i]} ${User[$i]} ${CpuPer[$i]}% ${MemPer[$i]}%"
done
