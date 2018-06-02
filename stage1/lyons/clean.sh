#!/bin/sh
set -e
. ./config.sh

$MAKE clean

rm -rfv sysroot
