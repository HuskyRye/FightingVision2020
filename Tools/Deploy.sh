#!/bin/sh

shdir=$(pwd)/FightingVision2020.sh
cd ../build/

echo "#!/bin/sh" > $shdir
echo "exe=$(pwd)/FightingVision2020" >> $shdir
echo "cd $(pwd)" >> $shdir
echo "while true; do" >> $shdir
echo "\tstate=\`ps aux | grep \"\$exe\" | grep -v grep | grep -v \$0\`" >> $shdir
echo "\tif [ ! \"\$state\" ]; then" >> $shdir
echo "\t\texec \$exe &" >> $shdir
echo "\t\techo \"restart \$exe\"" >> $shdir
echo "\tfi" >> $shdir
echo "\tsleep 0.5" >> $shdir
echo "done" >> $shdir

cd ../Tools/
sudo cp ./FightingVision2020.service /etc/systemd/system/
sudo cp $shdir /bin/
sudo chmod +x /bin/FightingVision2020.sh
sudo systemctl daemon-reload
sudo systemctl enable FightingVision2020.service
