#!/bin/bash
# Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# Used with Mac OS X 10.11.2 "El Capitan" and the TrueRNG v2.
# Unlike the Linux rngd from rng-tools, this simple script does
# not to any testing of the entropy stream, so cannot meet the
# FIPS 140-2 requirements. This is also highly experimental.

ZERO=$(basename ${0})
LABEL=${ZERO%\.sh}
SOURCE=${1:-"/dev/cu.usbmodem1431"}
SINK=${2:-"/dev/random"}
FILE=${3:-"/var/run/${LABEL}.pid}

dd if=${SOURCE} of=${SINK} &

PID=$!
echo ${PID} > ${FILE}

exit 0
