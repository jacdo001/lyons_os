#!/bin/bash
. ./config_cross.sh

SYSTEM_HEADER_PROJECTS="kernel"
PROJECTS="kernel"

export MAKE="${MAKE:-make}"
export HOST="${HOST:-$(./default-host.sh)}"

export AR="${HOST}-ar"
export AS="${HOST}-as"
export CC="${HOST}-gcc"

export PREFIX="/usr"
export EXEC_PREFIX="$PREFIX"
export BOOTDIR="/boot"
export LIBDIR="$EXEC_PREFIX/lib"
export INCLUDEDIR="$PREFIX/include"

export CFLAGS=""
export CPPFLAGS=""

export CC="$CC --sysroot=$PWD/sysroot"

if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi

