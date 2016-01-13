#!/bin/bash
# vi: set ts=4:
# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# WORK IN PROGRESS
# Used with Mac OS X 10.11.2 "El Capitan" and the TrueRNG v2.
# Unlike the Linux rngd from rng-tools, this simple script does
# not to any testing of the entropy stream, so cannot meet the
# FIPS 140-2 requirements.

ZERO=$(basename ${0})
LABEL=${ZERO%\.sh}

. ${LABEL}.conf

FILE=${RUNDIR}/${LABEL}.pid

if [ ! -r ${SOURCE} ]; then
	echo "${ZERO}: ${SOURCE}: no such file or directory" 1>&2
	exit 2
fi

if [ ! -w ${SINK} ]; then
	echo "${ZERO}: ${SINK}: no such file or directory" 1>&2
	exit 2
fi

if [ ! -r ${FILE} ]; then
	:
elif ! read PID < ${FILE}; then
	:
elif [ -z "${PID}" ]; then
	:
elif ! kill -0 ${PID} 2> /dev/null; then
	:
else
	echo "${ZERO}: ${PID}: already running" 1>&2
	exit 2
fi

dd if=${SOURCE} of=${SINK} &
echo $! > ${FILE}

exit 0
