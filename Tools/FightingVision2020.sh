#!/bin/sh
exe=/home/husky-rye/FightingVision2020/build/FightingVision2020
cd /home/husky-rye/FightingVision2020/build
while true; do
	state=`ps aux | grep "$exe" | grep -v grep | grep -v $0`
	if [ ! "$state" ]; then
		exec $exe &
		echo "restart $exe"
	fi
	sleep 0.5
done
