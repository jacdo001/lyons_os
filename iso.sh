#!/bin/sh
set -e
set -x

OS_SYSROOT="$PWD/lyons/sysroot"
GRUB_CFG="$PWD/grub/grub.cfg"

mkdir -p isodir/boot/grub

cp -a "$OS_SYSROOT/." "$PWD/isodir/"
cp shell/bin/shell isodir/boot/shell
cp "$GRUB_CFG" "$PWD/isodir/boot/grub/grub.cfg"
grub-mkrescue -o lyons.iso isodir
