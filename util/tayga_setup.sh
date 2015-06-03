#!/bin/bash

## USAGE: ./tayga_setup.sh
#  Setup tayga

cat >/usr/local/etc/tayga.conf <<EOD
tun-device nat64
ipv4-addr 192.168.255.1
prefix 2001:db8:1:ffff::/96
dynamic-pool 192.168.255.0/24
EOD

tayga --mktun
ip link set nat64 up
ip addr add 192.168.255.1/24 dev nat64
ip addr add 2001:db8:1:fff0::1/96 dev nat64
ip route add 2001:db8:1:ffff::/96 dev nat64
tayga
ping 192.168.255.2