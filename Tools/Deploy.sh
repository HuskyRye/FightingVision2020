#!/bin/sh

shdir=$(pwd)/FightingVision2020.sh
cd ../build/
echo "#!/bin/sh" > $shdir
echo "cd $(pwd)" >> $shdir
echo "$(pwd)/FightingVision2020" >> $shdir

cd ../Tools/

sudo cp ./FightingVision2020.service /etc/systemd/system/
sudo cp $shdir /bin/
sudo chmod +x /bin/FightingVision2020.sh
sudo systemctl daemon-reload
sudo systemctl enable FightingVision2020.service

sudo cp ./Clocks.sh /bin/
sudo chmod +x /bin/Clocks.sh
sudo cp $(pwd)/rc.local /etc/rc.local

