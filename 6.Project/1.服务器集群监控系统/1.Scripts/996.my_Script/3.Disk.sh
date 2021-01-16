#!/bin/bash
Time=`date +"%Y:%m:%d__%H:%M:%S"`

DiskSum=0
DiskLeft=0
for (( i = 0; i < 2; i++ )); do
    DiskSum=`df -T -m -x tmpfs -x devtmpfs | tail -n +2 | awk '{printf $2}'`
    DiskLeft=`df -T -m -x tmpfs -x devtmpfs | tail -n +2 | awk '{printf $4}'`
    DiskUsed=`df -T -m -x tmpfs -x devtmpfs | tail -n +2 | awk '{printf "%d%%"$5}'`
    DiskMount=`df -T -m -x tmpfs -x devtmpfs | tail -n +2 | awk '{printf $6}'`
    echo "${Time} 1 ${DiskMount} ${DiskSum} ${DiskLeft} ${DiskUsed}"
done
