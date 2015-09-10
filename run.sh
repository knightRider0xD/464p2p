#!/bin/bash
sudo echo starting
git pull
cd kmod/
make fresh
sync
sudo make install
cd ..
./config/interfaces_arch.sh
sleep 2
cd ./util/core_udp_test/
make fresh
./client4 192.168.254.1 2000 "hi"
cd ../..