#!/usr/bin/env bash

set -e

SDIR=$(dirname $0)
BDIR=$SDIR/../build
APATH=$BDIR/src/autotester/autotester

mkdir -p $BDIR
cd $BDIR
cmake .. > /dev/null 2>&1
cmake --build . --target autotester --config Release > /dev/null 2>&1

if [[ -e $APATH ]]; then
  echo "Error while building autotester binary!" >&2
  exit 1
fi

echo $APATH
