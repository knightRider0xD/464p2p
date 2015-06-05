#!/bin/bash

## USAGE: ./clatd_deploy.sh mode node_number gateway_dev_name
#  Installs dependencies, builds, generates config file & runs clatd
#  mode 0 = install & run, other values run only.
if $1 == '0'; then
    yaourt --force --noconfirm -Sy tayga perl-net-dns perl-io-socket-inet6 perl-net-ip;

    git clone https://github.com/toreanderson/clatd
    cd clatd/
    sudo make install installdeps
fi
a=$2
a=$((3-a))
echo "clat-v4-addr=192.0.0.$2
clat-v6-addr=fd16:6db2:c925:$2::C000:$2
plat-dev=$3
plat-prefix=fd16:6db2:c925:$a::/64
tayga-v4-addr=192.0.1.$2
v4-conncheck-enable=no" > clatd.conf

./clatd -c clatd.conf