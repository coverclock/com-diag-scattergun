#!/bin/bash
# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# https://github.com/coverclock/com-diag-scattergun
# mailto:coverclock@diag.com

ZERO=$(basename $0)
STAMP=${1-"$(date -u +%Y%m%dT%H%M%S)"}
SOURCE=${2:-"/dev/random"}

LABEL=${ZERO%\.sh}

##################################################

uname -a
[[ -x /usr/bin/lsb_release ]] && /usr/bin/lsb_release -a

[[ -c /dev/random ]] && ls -l /dev/random
[[ -c /dev/urandom ]] && ls -l /dev/urandom
[[ -c /dev/hwrng ]] && ls -l /dev/hwrng
[[ -c /dev/TrueRNG ]] && ls -l /dev/TrueRNG

# modprobe bcm2708_rng # Raspberry Pi 2 Model B v1.1 2014

lsmod | grep bcm2708_rng

# modprobe intel-rng # Intel Ivy Bridge RDRAND

lsmod | grep intel-rng

##################################################

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

# sudo apt-get install rng-tools
# ${EDITOR} /etc/default/rng-tools
# sudo /etc/init.d/rng-tools start

ps -ef | grep rngd | grep -v grep

if [[ -r /etc/default/rng-tools ]]; then
	. /etc/default/rng-tools
	echo HRNGDEVICE=${HRNGDEVICE}
	ls -l ${HRNGDEVICE}
fi

if [[ -x /usr/bin/rngtest ]]; then
	DATA="${LABEL}-rngtest-${STAMP}.dat"
	BLOCKSIZE=$(( 20000 / 8 ))
	time dd if=${SOURCE} of=${DATA} bs=${BLOCKSIZE} count=1000 iflag=fullblock
	time /usr/bin/rngtest -c 1000 < ${DATA}
fi

##################################################

# sudo apt-get install ent

if [[ -x /usr/bin/ent ]]; then
	DATA="${LABEL}-ent-${STAMP}.dat"
	time dd if=${SOURCE} of=${DATA} bs=1024 count=128 iflag=fullblock
	time /usr/bin/ent ${DATA}
fi

##################################################

# sudo apt-get install netpbm

if [[ ! -x /usr/bin/rawtoppm ]]; then
	:
elif [[ ! -x /usr/bin/pnmtopng ]]; then
	:
else
	DATA="${LABEL}-rawtoppm-${STAMP}.dat"
	IMAGE="${LABEL}-rawtoppm-${STAMP}.png"
	time dd if=${SOURCE} of=${DATA} bs=3 count=65536 iflag=fullblock
	/usr/bin/rawtoppm -rgb 256 256 < ${DATA} | /usr/bin/pnmtopng > ${IMAGE}
fi

##################################################

# git clone http://github.com/usnistgov/SP800-90B_EntropyAssessment
# export PATH=$PATH:$(pwd)/SP800-90B_EntropyAssessment

NISTCODE=$(which iid_main.py)
if [[ ! -z "${NISTCODE}" ]]; then
	NISTPATH=$(dirname ${NISTCODE})
	DATA="$(pwd)/${LABEL}-sp800-${STAMP}.dat"
	time dd if=/dev/random of=${DATA} bs=1000 count=1000 iflag=fullblock
	( cd ${NISTPATH}; time python iid_main.py ${DATA} 8 1000 -v )
	( cd ${NISTPATH}; time python noniid_main.py ${DATA} 8 -v )
fi

##################################################
