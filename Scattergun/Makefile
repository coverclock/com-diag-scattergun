# Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
# "Digital Aggregates Corporation" is a registered trademark.
# Licensed under the terms of the GNU GPL v2.
# https://github.com/coverclock/com-diag-scattergun
# mailto:coverclock@diag.com

ROOT=$(HOME)/src/idq-quantis

QUANTISINCPATH=$(ROOT)/Lib-Apps/Quantis
QUANTISLIBPATH=$(ROOT)/Lib-Apps/build/Quantis

CFLAGS += -I$(QUANTISINCPATH)
LDFLAGS += -lusb-1.0
LDFLAGS += -L$(QUANTISLIBPATH)
LDFLAGS += -lQuantis
LDFLAGS += -lpthread

quantistool: quantistool.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f quantistool
