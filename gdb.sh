#!/bin/sh
set -e

qemu-system-$(./lyons/target-triplet-to-arch.sh $(./lyons/default-host.sh)) -s -S -cdrom lyons.iso &
QEMU_PID=$!

gdb -x script.gdb

kill $QEMU_PID
