#!/bin/bash

## USAGE: ./clatd_deploy.sh
#  Installs dependencies, builds, generates config file & runs clatd

yaourt --force --noconfirm -Sy tayga perl-net-dns perl-io-socket-inet6 perl-net-ip;

git clone https://github.com/toreanderson/clatd
cd clatd/
sudo make install installdeps

