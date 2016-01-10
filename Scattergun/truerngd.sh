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

if [ $# -lt 1 ]; then
	echo "usage: ${ZERO} /dev/cu.usbmodem1234 [ /dev/random [ /var/run/${LABEL}.pid ] ]" 1>&2
	exit 1
fi

SOURCE=${1} # /dev/cu.usbmodem1431
SINK=${2:-"/dev/random"}
FILE=${3:-"/var/run/${LABEL}.pid"}

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
	echo "${ZERO}: ${PID}: should be no such process" 1>&2
	exit 2
fi

(
	sh -c 'echo ${PPID}' > ${FILE}
	trap "rm -f ${FILE}; echo TRAPPED ${FILE}! 1>&2" HUP INT QUIT TERM
	dd if=${SOURCE} of=${SINK}
	rm -f ${FILE}

) < /dev/null > /dev/null &

exit 0
