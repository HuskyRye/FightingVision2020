#!/bin/sh
systemctl disable FightingVision2020.service
sudo rm /etc/systemd/system/FightingVision2020.service
sudo rm /bin/FightingVision2020.sh
sudo systemctl daemon-reload