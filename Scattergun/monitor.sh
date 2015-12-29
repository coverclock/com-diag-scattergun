#!/bin/bash
# Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.

PERIOD=${1-"0.25"}

POOLFILE="/proc/sys/kernel/random/poolsize"
ENTRFILE="/proc/sys/kernel/random/entropy_avail"

read POOLSIZE < ${POOLFILE}

GRAPH="===================================================================================================="

while true; do

	read ENTRSIZE < ${ENTRFILE}

	PERCENT="$(( ( ${ENTRSIZE}  * 100 ) / ${POOLSIZE} ))"

	printf "%3d%% %s\n" ${PERCENT} ${GRAPH:0:${PERCENT}}

	sleep ${PERIOD}

done
