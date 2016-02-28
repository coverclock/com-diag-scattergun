# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# https://github.com/coverclock/com-diag-scattergun
# mailto:coverclock@diag.com

# Depending on the device being tested, three systems were used.
# 
# "Silver"
# Dell
# Intel Core 2 Q6600 2.4GHz x4
# Ubuntu 14.04 "trusty"
# Linux 3.13.0
# gcc 4.8.2
#
# "Mercury"
# Dell
# Intel Core i7-6700T 2.8GHz x8
# Ubuntu 14.04.4 "trusty"
# Linux 4.2.0
# gcc 4.8.4
#
# "Betty"
# Raspberry Pi 2 Model B v1.1
# Broadcom BCM2836 Cortex-A7 ARMv7 x4
# Raspbian (Debian) 7.8
# Linux 3.8.11
# gcc 4.6.3

################################################################################

ROOT=$(HOME)/src

USNISTGOV_ROOT=$(ROOT)/usnistgov/SP800-90B_EntropyAssessment
QUANTIS_ROOT=$(ROOT)/idq-quantis

export PATH:=$(shell pwd)/bin:$(shell pwd)/src:$(USNISTGOV_ROOT):$(PATH)
export LD_LIBRARY_PATH:=$(QUANTIS_LIBPATH):$(LD_LIBRARY_PATH)

################################################################################

all:

clean:
	rm -f src/quantistool src/seventool

.PHONY: all clean

################################################################################

QUANTIS_INCPATH=$(QUANTIS_ROOT)/Libs-Apps/Quantis
QUANTIS_LIBPATH=$(QUANTIS_ROOT)/Libs-Apps/build/Quantis

QUANTIS_CFLAGS += -I$(QUANTIS_INCPATH)
#QUANTIS_LDFLAGS += -L$(QUANTIS_LIBPATH)
#QUANTIS_LDFLAGS += -lQuantis
QUANTIS_LDFLAGS += $(QUANTIS_LIBPATH)/libQuantis.a
QUANTIS_LDFLAGS += -lusb-1.0
QUANTIS_LDFLAGS += -lpthread

src/quantistool: src/quantistool.c
	$(CC) $(QUANTIS_CFLAGS) -o $@ $^ $(QUANTIS_LDFLAGS)

################################################################################

#SEVEN_CFLAGS += -DSCATTERGUN_HAS_RDRAND_INLINE
#SEVEN_CFLAGS += -DSCATTERGUN_HAS_RDRAND_INTRINSIC
SEVEN_CFLAGS += -DSCATTERGUN_HAS_RDRAND_MNEMONIC
#SEVEN_CFLAGS += -DSCATTERGUN_HAS_RDSEED_INLINE
#SEVEN_CFLAGS += -DSCATTERGUN_HAS_RDSEED_INTRINSIC
SEVEN_CFLAGS += -DSCATTERGUN_HAS_RDSEED_MNEMONIC
SEVEN_LDFLAGS += 

src/seventool: src/seventool.c
	$(CC) $(SEVEN_CFLAGS) -o $@ $^ $(SEVEN_LDFLAGS)

################################################################################

environment:
	echo export PATH=$(shell pwd)/bin:$(shell pwd)/src:$(USNISTGOV_ROOT):$(PATH)
	echo export LD_LIBRARY_PATH=$(QUANTISLIBPATH):$(LD_LIBRARY_PATH)

.PHONY: environment

################################################################################

TRUERNG=scattergun_silver_TrueRNG

TrueRNG:
	mkdir -p $(TRUERNG)
	( ( dd if=/dev/TrueRNG | bash scattergun.sh $(TRUERNG) ) > $(TRUERNG)/scattergun.log 2>&1 ) &

.PHONY: TrueRNG

################################################################################

TRUERNGPRO=scattergun_silver_TrueRNGPro

TrueRNGPro:
	mkdir -p $(TRUERNGPRO)
	( ( dd if=/dev/TrueRNGPro | bash scattergun.sh $(TRUERNGPRO) ) > $(TRUERNGPRO)/scattergun.log 2>&1 ) &

.PHONY:	TrueRNGPro

################################################################################

BCM2708=scattergun_betty_bcm2708

bcm2708:
	mkdir -p $(BCM2708)
	( ( dd if=/dev/random | bash scattergun.sh $(BCM2708) ) > $(BCM2708)/scattergun.log 2>&1 ) &

.PHONY:	bcm2708

################################################################################

QUANTIS=scattergun_silver_quantis

quantis:	src/quantistool
	mkdir -p $(QUANTIS)
	( ( quantistool -v | bash scattergun.sh $(QUANTIS) ) > $(QUANTIS)/scattergun.log 2>&1 ) &

.PHONY:	quantis

################################################################################

RDRAND=scattergun_mercury_rdrand

rdrand:	src/seventool
	mkdir -p $(RDRAND)
	( ( seventool -v -R | bash scattergun.sh $(RDRAND) ) > $(RDRAND)/scattergun.log 2>&1 ) &

.PHONY: rdrand

################################################################################

RDSEED=scattergun_mercury_rdseed

rdseed:	src/seventool
	mkdir -p $(RDSEED)
	( ( seventool -v -S | bash scattergun.sh $(RDSEED) ) > $(RDSEED)/scattergun.log 2>&1 ) &

.PHONY: rdseed

################################################################################

URANDOM=scattergun_silver_urandom

urandom:
	mkdir -p $(URANDOM)
	( ( dd if=/dev/urandom | bash scattergun.sh $(URANDOM) ) > $(URANDOM)/scattergun.log 2>&1 ) &

.PHONY:	urandom

################################################################################

RANDOM=scattergun_silver_random

random:
	mkdir -p $(RANDOM)
	( ( dd if=/dev/random | bash scattergun.sh $(RANDOM) ) > $(RANDOM)/scattergun.log 2>&1 ) &

.PHONY:	random

################################################################################
