#!/bin/bash

# has to run as sudo!

locationOfScript=$(dirname "$(readlink -e "$0")")

LIBDIR="$locationOfScript""/../lib"

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBDIR
echo $LIBDIR > /etc/ld.so.conf.d/opencv.conf
updatedb
ldconfig
