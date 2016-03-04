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

OUT=out/host/bin

################################################################################

ALL  = $(OUT)/setup
ALL += $(OUT)/quantistool
ALL += $(OUT)/seventool
ALL += $(OUT)/seventool-binary
ALL += $(OUT)/seventool-mnemonic
ALL += $(OUT)/characterize.sh
ALL += $(OUT)/consume.sh
ALL += $(OUT)/monitor.sh
ALL += $(OUT)/onernginit.sh
ALL += $(OUT)/scattergun.sh
ALL += $(OUT)/truerngd.sh

all:	$(ALL)

clean:
	rm -rf $(ALL)

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

$(OUT)/quantistool: src/quantistool.c
	$(CC) $(CFLAGS) $(QUANTIS_CFLAGS) -o $@ $^ $(LDFLAGS) $(QUANTIS_LDFLAGS)

################################################################################

$(OUT)/seventool:	$(OUT)/seventool-mnemonic
	cp $^ $@

$(OUT)/seventool-binary: src/seventool.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

SEVEN_MNEMONIC += -DSCATTERGUN_HAS_RDRAND_MNEMONIC
SEVEN_MNEMONIC += -DSCATTERGUN_HAS_RDSEED_MNEMONIC

$(OUT)/seventool-mnemonic: src/seventool.c
	$(CC) $(CFLAGS) $(SEVEN_MNEMONIC) -o $@ $^ $(LDFLAGS)

SEVEN_INTRINSIC += -DSCATTERGUN_HAS_RDRAND_INTRINSIC
SEVEN_INTRINSIC += -DSCATTERGUN_HAS_RDSEED_INTRINSIC

$(OUT)/seventool-intrinsic: src/seventool.c
	$(CC) $(CFLAGS) $(SEVEN_INTRINSIC) -o $@ $^ $(LDFLAGS)

SEVEN_INLINE += -DSCATTERGUN_HAS_RDRAND_INLINE
SEVEN_INLINE += -DSCATTERGUN_HAS_RDSEED_INTRINSIC

$(OUT)/seventool-inline: src/seventool.c
	$(CC) $(CFLAGS) $(SEVEN_INLINE) -o $@ $^ $(LDFLAGS)

################################################################################

$(OUT)/characterize.sh:	bin/characterize.sh
	cp $^ $@
	chmod 775 $@

$(OUT)/consume.sh:	bin/consume.sh
	cp $^ $@
	chmod 775 $@

$(OUT)/monitor.sh:	bin/monitor.sh
	cp $^ $@
	chmod 775 $@

$(OUT)/onernginit.sh:	bin/onernginit.sh
	cp $^ $@
	chmod 775 $@

$(OUT)/scattergun.sh:	bin/scattergun.sh
	cp $^ $@
	chmod 775 $@

$(OUT)/truerngd.sh:	bin/truerngd.sh
	cp $^ $@
	chmod 775 $@


################################################################################

$(OUT)/setup:
	cp /dev/null $@
	echo export PATH=$(shell pwd)/$(OUT):$(USNISTGOV_ROOT):$(PATH) >> $@
	echo export LD_LIBRARY_PATH=$(QUANTIS_LIBPATH):$(LD_LIBRARY_PATH) >> $@

export PATH:=$(shell pwd)/$(OUT):$(USNISTGOV_ROOT):$(PATH)
export LD_LIBRARY_PATH:=$(QUANTIS_LIBPATH):$(LD_LIBRARY_PATH)

################################################################################

TRUERNG=scattergun_silver_TrueRNG

TrueRNG:	/dev/TrueRNG
	mkdir -p $(TRUERNG)
	( ( dd if=/dev/TrueRNG | scattergun.sh $(TRUERNG) ) > $(TRUERNG)/scattergun.log 2>&1 ) &

.PHONY: TrueRNG

################################################################################

TRUERNGPRO=scattergun_silver_TrueRNGpro

TrueRNGpro:	/dev/TrueRNGpro
	mkdir -p $(TRUERNGPRO)
	( ( dd if=/dev/TrueRNGpro | scattergun.sh $(TRUERNGPRO) ) > $(TRUERNGPRO)/scattergun.log 2>&1 ) &

.PHONY:	TrueRNGpro

################################################################################

QUANTIS=scattergun_silver_quantis

quantis:	src/quantistool
	mkdir -p $(QUANTIS)
	( ( quantistool -v | scattergun.sh $(QUANTIS) ) > $(QUANTIS)/scattergun.log 2>&1 ) &

.PHONY:	quantis

################################################################################

ONERNG=scattergun_silver_onerng

onerng:	/dev/OneRNG
	mkdir -p $(ONERNG)
	( ( dd if=/dev/OneRNG | scattergun.sh $(ONERNG) ) > $(ONERNG)/scattergun.log 2>&1 ) &

.PHONY:	onerng

################################################################################

URANDOM=scattergun_silver_urandom

urandom:	/dev/urandom
	mkdir -p $(URANDOM)
	( ( dd if=/dev/urandom | scattergun.sh $(URANDOM) ) > $(URANDOM)/scattergun.log 2>&1 ) &

.PHONY:	urandom

################################################################################

RANDOM=scattergun_silver_random

random:	/dev/random
	mkdir -p $(RANDOM)
	( ( dd if=/dev/random | scattergun.sh $(RANDOM) ) > $(RANDOM)/scattergun.log 2>&1 ) &

.PHONY:	random

################################################################################

BCM2708=scattergun_betty_bcm2708

bcm2708:	/dev/random
	mkdir -p $(BCM2708)
	( ( dd if=/dev/hwrng | scattergun.sh $(BCM2708) ) > $(BCM2708)/scattergun.log 2>&1 ) &

.PHONY:	bcm2708

################################################################################

RDRAND=scattergun_mercury_rdrand

rdrand:	$(OUT)/seventool
	mkdir -p $(RDRAND)
	( ( seventool -v -R -r -c | scattergun.sh $(RDRAND) ) > $(RDRAND)/scattergun.log 2>&1 ) &

.PHONY: rdrand

################################################################################

RDSEED=scattergun_mercury_rdseed

rdseed:	$(OUT)/seventool
	mkdir -p $(RDSEED)
	( ( seventool -v -S -c | scattergun.sh $(RDSEED) ) > $(RDSEED)/scattergun.log 2>&1 ) &

.PHONY: rdseed

################################################################################

FAIL=scattergun_mercury_fail

fail:	$(OUT)/seventool
	mkdir -p $(FAIL)
	( ( seventool -v | scattergun.sh $(FAIL) ) > $(FAIL)/scattergun.log 2>&1 ) &

.PHONY:	fail

################################################################################
