#!/bin/bash
git pull
cd kmod/
make fresh
sudo make install
cd ..
./config/interfaces_arch.sh
./util/core_udp_test/client4 192.168.254.1 2000 "hi"
