#!/bin/bash
# Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.

ZERO=$(basename $0)
SOURCE=${1:-"/dev/random"}
SINK=${2:-"/dev/null"}
COUNT=${3:-"1"}

echo ${ZERO}: ${SOURCE} ${SINK} ${COUNT}

echo ${ZERO}: $(uname -a)

echo ${ZERO}: $(ps -ef | grep rngd | grep -v grep)

HRNGDEVICE=""
if [[ -r /etc/default/rng-tools ]]; then
	. /etc/default/rng-tools
	echo ${ZERO}: HRNGDEVICE=${HRNGDEVICE}
	echo ${ZERO}: $(ls -l ${HRNGDEVICE})
fi
if [[ -z "${HRNGDEVICE}" ]]; then
	HRNGDEVICE="/dev/null"
fi

read AVAILABLE < /proc/sys/kernel/random/entropy_avail
read TOTAL < /proc/sys/kernel/random/poolsize

FIRST=$(( ( ${AVAILABLE} + 7 ) / 8 ))
SECOND=$(( ${TOTAL} / 8 ))

echo ${ZERO}: ${AVAILABLE} ${FIRST} ${TOTAL} ${SECOND}

dd if=${SOURCE} of=/dev/null bs=${FIRST} count=1 iflag=fullblock
time dd if=${SOURCE} of=${SINK} bs=${SECOND} count=${COUNT} iflag=fullblock
