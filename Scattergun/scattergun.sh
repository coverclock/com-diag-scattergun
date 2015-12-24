#!/bin/bash
# Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.

# sudo /etc/init.d/rng-tools [ start | stop ]

ZERO=$(basename $0)
SOURCE=${1:-"/dev/random"}
STAMP=$(date -u +%Y%m%dT%H%M%S.%N%:z)

uname -a
lsb_release -a

ls -l /dev/random
ls -l /dev/urandom
ls -l /dev/hwrng
ls -l /dev/hw_random
ls -l /dev/hwrandom
ls -l /dev/intel_rng
ls -l /dev/i810_rng

pgrep -a rngd

test -r /etc/default/rng-tools && ( . /etc/default/rng-tools && echo HRNGDEVICE=${HRNGDEVICE} && ls -l ${HRNGDEVICE} )

for STEP in 0 1 2 3 4 5 6 7 8 9; do
	echo -n "${STEP}: "
	cat /proc/sys/kernel/random/entropy_avail
	sleep 1
done

POOLSIZE=$(cat /proc/sys/kernel/random/poolsize)
BLOCKSIZE=$((${POOLSIZE} / 8))
time dd if=${SOURCE} of=/dev/null bs=${BLOCKSIZE} count=1 iflag=fullblock
time dd if=${SOURCE} of=/dev/null bs=${BLOCKSIZE} count=1 iflag=fullblock

# sudo apt-get install rng-tools

test -x /usr/bin/rngtest && ( cat ${SOURCE} | time rngtest -c 1000 )

# sudo apt-get install ent

test -x /usr/bin/ent && ( FILE=$(mktemp ./${ZERO}.XXXXXXXXXX); dd if=${SOURCE} of=${FILE} bs=1024 count=128 iflag=fullblock; time ent ${FILE}; rm -f ${FILE} )

# sudo apt-get install netpbm

test -x /usr/bin/rawtoppm && test -x /usr/bin/pnmtopng && ( FILE=$(mktemp ./${ZERO}.XXXXXXXXXX); cat ${SOURCE} | time rawtoppm -rgb 256 256 | pnmtopng > ${FILE}; mv -f ${FILE} ${ZERO}-${STAMP}.png )

# git clone http://github.com/usnistgov/SP800-90B_EntropyAssessment
