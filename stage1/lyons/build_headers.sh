#!/bin/sh
set -e
. ./config.sh

mkdir -p sysroot

DESTDIR="$PWD/sysroot" $MAKE install-headers
