#!/bin/bash
# Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.

ZERO=$(basename $0)
SOURCE=${1:-"/dev/random"}
STAMP=$(date -u +%Y%m%dT%H%M%S%N)

uname -a

if [[ -x /usr/bin/lsb_release ]]; then
	/usr/bin/lsb_release -a
fi

ls -l /dev/random
ls -l /dev/urandom
ls -l /dev/hwrng
ls -l /dev/TrueRNG

if [[ -f /proc/sys/kernel/random/entropy_avail ]]; then
	for STEP in 0 1 2 3 4 5 6 7 8 9; do
		echo -n "${STEP}: "
		cat /proc/sys/kernel/random/entropy_avail
		sleep 1
	done
fi

POOLSIZE=$(cat /proc/sys/kernel/random/poolsize)
BLOCKSIZE=$((${POOLSIZE} / 8))
time dd if=${SOURCE} of=/dev/null bs=${BLOCKSIZE} count=1 iflag=fullblock
time dd if=${SOURCE} of=/dev/null bs=${BLOCKSIZE} count=1 iflag=fullblock

# modprobe bcm2708_rng # Raspberry Pi 2 Model B v1.1 2014

lsmod | grep bcm2708_rng

# sudo apt-get install rng-tools
# ${EDITOR} /etc/default/rng-tools
# sudo /etc/init.d/rng-tools start

ps -ef | grep rngd

if [[ -r /etc/default/rng-tools ]]; then
	. /etc/default/rng-tools
	echo HRNGDEVICE=${HRNGDEVICE}
	ls -l ${HRNGDEVICE}
fi

if [[ -x /usr/bin/rngtest ]]; then
	cat ${SOURCE} | time /usr/bin/rngtest -c 1000
fi

# sudo apt-get install ent

if [[ -x /usr/bin/ent ]]; then
	FILE=$(mktemp ./${ZERO}-ent-${STAMP}.XXXXXXXXXX)
	time dd if=${SOURCE} of=${FILE} bs=1024 count=128 iflag=fullblock
	mv -f ${FILE} ${FILE}.dat
	time /usr/bin/ent ${FILE}.dat
fi

# sudo apt-get install netpbm

if [[ ! -x /usr/bin/rawtoppm ]]; then
	:
elif [[ ! -x /usr/bin/pnmtopng ]]; then
	:
else
	FILE=$(mktemp ./${ZERO}-rawtoppm-${STAMP}.XXXXXXXXXX)
	cat ${SOURCE} | time /usr/bin/rawtoppm -rgb 256 256 | /usr/bin/pnmtopng > ${FILE}
	mv -f ${FILE} ${FILE}.png
fi

# git clone http://github.com/usnistgov/SP800-90B_EntropyAssessment
# export PATH=$PATH:$(pwd)/SP800-90B_EntropyAssessment

NISTCODE=$(which iid_main.py)
if [[ ! -z "${NISTCODE}" ]]; then
	NISTPATH=$(dirname ${NISTCODE})
	FILE=$(mktemp $(pwd)/${ZERO}-sp800-${STAMP}.XXXXXXXXXX)
	time dd if=/dev/random of=${FILE} bs=1000 count=1000 iflag=fullblock
	mv -f ${FILE} ${FILE}.dat
	( cd ${NISTPATH}; time python iid_main.py ${FILE}.dat 8 1000 -v )
	( cd ${NISTPATH}; time python noniid_main.py ${FILE}.dat 8 -v )
fi
