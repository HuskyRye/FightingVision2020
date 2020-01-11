#!/bin/sh

shdir=$(pwd)/FightingVision2020.sh
cd ../build/

echo "#!/bin/sh" > $shdir
echo "nvpmodel -m0" >> $shdir
echo "/home/dji/jetson_clocks.sh" >> $shdir
echo "$(pwd)/FightingVision2020" >> $shdir

cd ../Tools/
sudo cp $shdir /bin/
sudo chmod +x /bin/FightingVision2020.sh
sudo cp $(pwd)/rc.local /etc/rc.local
