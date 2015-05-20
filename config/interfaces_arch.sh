#!/bin/bash
sudo ip addr add 192.168.254.1/24 broadcast 192.168.254.255 dev enp0s3
sudo ip -6 addr add fd16:6db2:c925:0000:2544:67fa:9e35:2876/64 dev enp0s3