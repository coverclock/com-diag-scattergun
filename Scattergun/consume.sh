#!/bin/bash
# Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.

ZERO=$(basename $0)
SOURCE=${1:-"/dev/random"}

read AVAILABLE < /proc/sys/kernel/random/entropy_avail
read TOTAL < /proc/sys/kernel/random/poolsize

FIRST=$(( ( ${AVAILABLE} + 7 ) / 8 ))
SECOND=$(( ${TOTAL} / 8 ))

echo ${AVAILABLE} ${FIRST} ${TOTAL} ${SECOND}

dd if=${SOURCE} of=/dev/null bs=${FIRST} count=1 iflag=fullblock
time dd if=${SOURCE} of=/dev/null bs=${SECOND} count=1 iflag=fullblock
