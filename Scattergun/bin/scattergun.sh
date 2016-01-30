#!/bin/bash
# vi: set ts=4:
# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# mailto:coverclock@diag.com
# https://github.com/coverclock/com-diag-scattergun

RC=0
ZERO=$(basename $0)
ISO8601=$(date -u +%Y-%m-%dT%H:%M:%S)
STAMP=${1-"${ISO8601}"}
LABEL=${ZERO%\.sh}

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin ${STAMP}"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin png"

# sudo apt-get install netpbm

if [[ ! -x /usr/bin/rawtoppm ]]; then
	:
elif [[ ! -x /usr/bin/pnmtopng ]]; then
	:
else
	DATA="${LABEL}-rawtoppm-${STAMP}.dat"
	IMAGE="${LABEL}-rawtoppm-${STAMP}.png"
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
	DATA="${LABEL}-rngtest-${STAMP}.dat"
	BLOCKSIZE=$(( 20000 / 8 ))
	time dd of=${DATA} bs=${BLOCKSIZE} count=1000 iflag=fullblock
	time /usr/bin/rngtest -c 1000 < ${DATA}
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end rngtest"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin ent"

# sudo apt-get install ent

if [[ -x /usr/bin/ent ]]; then
	DATA="${LABEL}-ent-${STAMP}.dat"
	time dd of=${DATA} bs=1024 count=4096 iflag=fullblock
	time /usr/bin/ent ${DATA}
fi

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end ent"

##################################################

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) begin SP800-90B"

# git clone http://github.com/usnistgov/SP800-90B_EntropyAssessment
# export PATH=$PATH:$(pwd)/SP800-90B_EntropyAssessment

NISTCODE=$(which iid_main.py)
if [[ ! -z "${NISTCODE}" ]]; then
	NISTPATH=$(dirname ${NISTCODE})
	DATA="$(pwd)/${LABEL}-sp800-${STAMP}.dat"
	time dd of=${DATA} bs=1024 count=4096 iflag=fullblock
	( cd ${NISTPATH}; time python iid_main.py ${DATA} 8 1000 -v )
	( cd ${NISTPATH}; time python noniid_main.py ${DATA} 8 -v )
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

echo "${ZERO}: $(date -u +%Y-%m-%dT%H:%M:%S) end ${RC}"

exit ${RC}
