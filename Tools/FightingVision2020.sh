#!/bin/sh

#<the exe file's absolute path and its shell parameter>"
# And then when ever the exe shutdown it will be automatically restart

exe=/home/dji/FightingVision2020/build/FightingVision2020
while true; do
	state=`ps aux | grep "$exe" | grep -v grep | grep -v $0`
	if [ ! "$state" ]; then
		exec $exe &
		echo "restart $exe"
	fi
	sleep 0.5
done
