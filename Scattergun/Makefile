# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# https://github.com/coverclock/com-diag-scattergun
# mailto:coverclock@diag.com

ROOT=$(HOME)/src

QUANTISROOT=$(ROOT)/idq-quantis
QUANTISINCPATH=$(QUANTISROOT)/Libs-Apps/Quantis
QUANTISLIBPATH=$(QUANTISROOT)/Libs-Apps/build/Quantis

USNISTGOVROOT=$(ROOT)/usnistgov/SP800-90B_EntropyAssessment

################################################################################

CFLAGS += -Iinc
CFLAGS += -I$(QUANTISINCPATH)
#LDFLAGS += -L$(QUANTISLIBPATH)
#LDFLAGS += -lQuantis
LDFLAGS += $(QUANTISLIBPATH)/libQuantis.a
LDFLAGS += -lusb-1.0
LDFLAGS += -lpthread

src/quantistool: src/quantistool.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/quantistool

################################################################################

environment:
	echo export PATH=$(shell pwd)/bin:$(shell pwd)/src:$(USNISTGOVROOT):$(PATH)
	echo export LD_LIBRARY_PATH=$(QUANTISLIBPATH):$(LD_LIBRARY_PATH)

################################################################################

export PATH:=$(shell pwd)/bin:$(shell pwd)/src:$(USNISTGOVROOT):$(PATH)
export LD_LIBRARY_PATH:=$(QUANTISLIBPATH):$(LD_LIBRARY_PATH)

TRUERNG=scattergun_silver_TrueRNG

TrueRNG:
	mkdir -p $(TRUERNG)
	( ( dd if=/dev/TrueRNG | bash scattergun.sh $(TRUERNG) ) > $(TRUERNG)/scattergun.log 2>&1 ) &

TRUERNGPRO=scattergun_silver_TrueRNGPro

TrueRNGPro:
	mkdir -p $(TRUERNGPRO)
	( ( dd if=/dev/TrueRNGPro | bash scattergun.sh $(TRUERNGPRO) ) > $(TRUERNGPRO)/scattergun.log 2>&1 ) &

BCM2708=scattergun_betty_bcm2708

bcm2708:
	mkdir -p $(BCM2708)
	( ( dd if=/dev/random | bash scattergun.sh $(BCM2708) ) > $(BCM2708)/scattergun.log 2>&1 ) &

QUANTIS=scattergun_silver_quantis

quantis:	src/quantistool
	mkdir -p $(QUANTIS)
	( ( quantistool -v | bash scattergun.sh $(QUANTIS) ) > $(QUANTIS)/scattergun.log 2>&1 ) &

RDRAND=scattergun_mercury_rdrand

rdrand:	src/ivybridgetool
	mkdir -p $(RDRAND)
	( ( ivybridgetool -v | bash scattergun.sh $(RDRAND) ) > $(RDRAND)/scattergun.log 2>&1 ) &

URANDOM=scattergun_silver_urandom

urandom:
	mkdir -p $(URANDOM)
	( ( dd if=/dev/urandom | bash scattergun.sh $(URANDOM) ) > $(URANDOM)/scattergun.log 2>&1 ) &

RANDOM=scattergun_silver_random

random:
	mkdir -p $(RANDOM)
	( ( dd if=/dev/random | bash scattergun.sh $(RANDOM) ) > $(RANDOM)/scattergun.log 2>&1 ) &
