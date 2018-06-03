#!/bin/bash
set -e
. ./build_headers.sh

DESTDIR="$PWD/sysroot" $MAKE install
