#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage : bash List.sh num"
    exit
fi

Cnt=$1


Names=(`cat names`)

function rand(){
    Min=1
    Max=${#Names[*]}
    Num=`cat /dev/urandom | head -n 10 | cksum | cut -d ' ' -f 1`
    echo $[ $Num % $Max + $Min ]
}

for i in `seq 1 $Cnt`; do
    RandNum=`rand`
    echo "${Names[$RandNum]} "
done

Ans=`rand`
