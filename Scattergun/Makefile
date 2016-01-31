# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# https://github.com/coverclock/com-diag-scattergun
# mailto:coverclock@diag.com

ROOT=$(HOME)/src/idq-quantis

QUANTISINCPATH=$(ROOT)/Libs-Apps/Quantis
QUANTISLIBPATH=$(ROOT)/Libs-Apps/build/Quantis

CFLAGS += -Iinc
CFLAGS += -I$(QUANTISINCPATH)
LDFLAGS += -L$(QUANTISLIBPATH)
LDFLAGS += -lQuantis
LDFLAGS += -lusb-1.0
LDFLAGS += -lpthread

src/quantistool: src/quantistool.c src/diminuto_dump.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/quantistool

LD_LIBRARY_PATH:
	echo export LD_LIBRARY_PATH=$(QUANTISLIBPATH):$(LD_LIBRARY_PATH)
