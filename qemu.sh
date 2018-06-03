#!/bin/sh
set -e

HOST=$(./lyons/default-host.sh)

qemu-system-$(./lyons/target-triplet-to-arch.sh $HOST) -display curses -cdrom lyons.iso
