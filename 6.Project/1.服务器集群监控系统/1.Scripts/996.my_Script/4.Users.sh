#!/bin/bash
Time=`date +"%Y-%m-%d__%H:%M:%S"`


eval `who  | awk -v UserSum=0 '{printf("Users["NR"]=%s; UserIP["NR"]=%s; UserTTY["NR"]=%s;", $1, $5, $2)}'`

