#!/usr/bin/env bash

set -e

SDIR=$(dirname $0)

$SDIR/build.sh | xargs python3 $SDIR/autotest.py
