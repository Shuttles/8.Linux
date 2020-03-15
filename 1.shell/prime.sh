#!/bin/bash
declare -a prime
End=1000
Sum=0

function init() {
    S=$1
    E=$2
    for (( i = $S; i <= $E; i++ )); do
        prime[$i]=0
    done
}

init 0 1000

for (( i = 2; i <= ${End}; i++ )); do
    if [[ ${prime[$i]}x == x ]]; then #如果数组元素为空，空并不是0！！
        prime[0]=$[ ${prime[0]} + 1 ]
        prime[${prime[0]}]=$i
        Sum=$[ ${Sum} + $i ]
    fi
    for (( j = 1; j <= ${prime[0]}; j++ )); do
        if [[ $[ $i * ${prime[$j]} ] -gt ${End}  ]]; then
            break
        fi
        prime[$[ $i * ${prime[$j]} ]]=1
        if [[ $[ $i % ${prime[$j]} ] -eq 0 ]]; then
            break
        fi
    done
done

echo ${Sum}
