#!/bin/bash
# vi: set ts=4:
# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# mailto:coverclock@diag.com
# https://github.com/coverclock/com-diag-scattergun
#
# USAGE
#
# scattergun.sh
#
# EXAMPLES
#
# dd if=/dev/random | scattergun.sh
#
# ABSTRACT
#
# Runs a battery of tests on a random number generator by
# reading ramdom bits from standard input. Saves generated
# data files and other artifacts in the current directory.
# 

RC=0
ZERO=$(basename $0)
LABEL=${ZERO%\.sh}
HOSTNAME=$(uname -n)
SYSTEM=$(uname -r)
ISO8601=$(date -u +%Y-%m-%dT%H:%M:%S)
ROOT=$(basename $(pwd))

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin ${ROOT}"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin png"

# sudo apt-get install netpbm

if [[ ! -x /usr/bin/rawtoppm ]]; then
	:
elif [[ ! -x /usr/bin/pnmtopng ]]; then
	:
else
	DATA="rawtoppm.dat"
	IMAGE="rawtoppm.png"
	time dd of=${DATA} bs=3 count=65536 iflag=fullblock
	/usr/bin/rawtoppm -rgb 256 256 < ${DATA} | /usr/bin/pnmtopng > ${IMAGE}
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end png"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin rngtest"

# sudo apt-get install rng-tools
# ${EDITOR} /etc/default/rng-tools
# sudo /etc/init.d/rng-tools start

if [[ -x /usr/bin/rngtest ]]; then
	DATA="rngtest.dat"
	time dd of=${DATA} bs=2508 count=1000 iflag=fullblock
	time /usr/bin/rngtest -c 1000 < ${DATA}
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end rngtest"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin ent"

# sudo apt-get install ent
# http://www.fourmilab.ch/random/random.zip

if [[ -x /usr/bin/ent ]]; then
	DATA="ent.dat"
	time dd of=${DATA} bs=1024 count=4096 iflag=fullblock
	time /usr/bin/ent ${DATA}
elif [[ -x ${HOME}/bin/ent ]]; then
	DATA="ent.dat"
	time dd of=${DATA} bs=1024 count=4096 iflag=fullblock
	time ${HOME}/bin/ent ${DATA}
else
	:
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end ent"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin SP800-90B"

# git clone http://github.com/usnistgov/SP800-90B_EntropyAssessment
# export PATH=$PATH:$(pwd)/SP800-90B_EntropyAssessment

NISTCODE=$(which iid_main.py)
if [[ -n "${NISTCODE}" ]]; then
	NISTPATH=$(dirname ${NISTCODE})
	DATA="$(pwd)/sp800.dat"
	time dd of=${DATA} bs=1024 count=4096 iflag=fullblock
	( cd ${NISTPATH}; time python iid_main.py    -v ${DATA} 8 )
	( cd ${NISTPATH}; time python noniid_main.py -v ${DATA} 8 )
fi

NISTCODE=$(which ea_iid)
if [[ -n "${NISTCODE}" ]]; then
	NISTPATH=$(dirname ${NISTCODE})
	DATA="$(pwd)/sp800.dat"
	time dd of=${DATA} bs=1024 count=4096 iflag=fullblock
	( cd ${NISTPATH}; time ea_iid     -v ${DATA} 8 )
	( cd ${NISTPATH}; time ea_non_iid -v ${DATA} 8 )
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end SP800-90B"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin dieharder"

# sudo apt-get install dieharder

if [[ -x /usr/bin/dieharder ]]; then
	dieharder -a -g 200
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end dieharder"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end ${ROOT} ${RC}"

exit ${RC}
