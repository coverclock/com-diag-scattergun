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
# entropy.sh [ SOURCE ]
#
# EXAMPLES
#
# entropy.sh /dev/TrueRNGpro
# cat /dev/TrueRNGpro | entropy.sh
#
# ABSTRACT
#
# Quick entropy check. Useful for unit testing.
#

RC=0
ZERO=$(basename ${0})
BASENAME=${ZERO%\.sh}
SOURCE=${1}
if [[ -n "${SOURCE}" ]]; then
	INPUT="if=${SOURCE}"
else
	INPUT=""
fi

if ENT=$(which ent); then
	DATA=$(mktemp /tmp/${BASENAME}.XXXXXXXXXX)
	dd ${INPUT} of=${DATA} bs=1024 count=4096 iflag=fullblock
	${ENT} ${DATA}
	rm ${DATA}
fi

exit 1
