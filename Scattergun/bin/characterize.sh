#!/bin/bash
# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# https://github.com/coverclock/com-diag-scattergun
# mailto:coverclock@diag.com

RC=0
ZERO=$(basename $0)
ISO8601=$(date -u +%Y-%m-%dT%H:%M:%S)
STAMP=${1-"${ISO8601}"}
SOURCE=${2:-"/dev/random"}
LABEL=${ZERO%\.sh}

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin ${STAMP} ${SOURCE}"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) platform"

uname -a
[[ -x /usr/bin/lsb_release ]] && /usr/bin/lsb_release -a

[[ -c /dev/random ]] && ls -l /dev/random
[[ -c /dev/urandom ]] && ls -l /dev/urandom
[[ -c /dev/hwrng ]] && ls -l /dev/hwrng
[[ -c /dev/TrueRNG ]] && ls -l /dev/TrueRNG
[[ -c /dev/TrueRNGpro ]] && ls -l /dev/TrueRNGpro
[[ -c /dev/OneRNG ]] && ls -l /dev/OneRNG

# modprobe bcm2708_rng # Raspberry Pi 2 Model B v1.1 2014

lsmod | grep bcm2708_rng

# modprobe intel-rng # Intel 82802 "Firmware Hub"

lsmod | grep intel-rng

# Intel rdrand (Bull Mountain): Ivy Bridge CPUs

cat /proc/cpuinfo | grep rdrand

# Intel rdseed: Broadwell CPUs

cat /proc/cpuinfo | grep rdseed

ps -ef | grep rngd | grep -v grep

if [[ -r /etc/default/rng-tools ]]; then
	. /etc/default/rng-tools
	echo HRNGDEVICE=${HRNGDEVICE}
	ls -l ${HRNGDEVICE}
fi

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) pool"

if [[ ! -f /proc/sys/kernel/random/poolsize ]]; then
	:
elif [[ ! -f /proc/sys/kernel/random/entropy_avail ]]; then
	:
else
	POOLSIZE=$(cat /proc/sys/kernel/random/poolsize)
	echo poolsize=${POOLSIZE}bits
	POOLBYTES=$(( ( ${POOLSIZE} + 7 ) / 8))
	echo poolsize=${POOLBYTES}bytes
	ENTROPYAVAIL=$(cat /proc/sys/kernel/random/entropy_avail)
	echo entropy_avail=${ENTROPYAVAIL}bits
	ENTROPYBYTES=$(( ( ${ENTROPYAVAIL} + 7 ) / 8 ))
	echo entropy_avail=${ENTROPYBYTES}bytes
	dd if=/dev/random of=/dev/null bs=1 count=${ENTROPYBYTES}
	time dd if=/dev/random of=/dev/null bs=1 count=${POOLBYTES}
	time dd if=/dev/urandom of=/dev/null bs=1 count=${POOLBYTES}
fi

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end ${RC}"

exit ${RC}
