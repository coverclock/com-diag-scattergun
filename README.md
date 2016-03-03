com-diag-scattergun
-------------------

Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
Licensed under the terms of the GNU GPL v2.

This project represents my puttering around with hardware entropy sources and
random number generation in Linux/GNU, and to a lesser extent, Mac OS X.

Many thanks to my friend, occasional colleague, former office mate, and
mentor REDACTED, who has a wealth of deep expertise in this domain.

Scattergun includes a variety of artifacts that might be useful.

    ./Scattergun/bin/scattergun.sh
    ./Scattergun/Makefile
    ./Scattergun/out

It has a script that runs a suite of tests (none of which I wrote) that
assess the randomness of an entropy source, a Makefile to drive the tests,
and the output of the test suite when it was run on a variety of hardware
entropy generators.

    ./Scattergun/src/quantistool.c
    ./Scattergun/etc/init.d/quantistool
    ./Scattergun/etc/udev/rules.d/99-idq-quantis.rules
    ./Scattergun/etc/init.d/rng-tools.diff
    ./Scattergun/etc/default/rng-tools-quantis

It has a utility, written in C, that extracts random bits from the Quantis
random number generator made by ID Quantique and write them to standard output,
or to a file which can be a named pipe. It includes an init script to run
the tool as a daemon, udev rules, and a patch to the rng-tools-4 init script,
that together allows the utility to read random bits from the Quantis, write
them to a named pipe, then have the rng-tools rngd daemon read from the pipe
and write to the system entropy pool. This is one way to integrate the Quantis
into a Linux server so it can be used as an entropy source for the system
entropy pool for the the generation of seeds for cryptographic keys and the
like.

    ./Scattergun/src/seventool.c

It has a utility, written in C, that examines the host processor using the
Intel cpuid instruction, and if it indicates that the processor implements
either the rdrand or rdseed instruction, extracts random bits using the
specified instruction and writes them to standard output, or to a file which
can be a named pipe.

    ./Scattergun/bin/truerngd.sh
    ./Scattergun/etc/truerngd.conf

It has a simple script (admittedly experimental) to use a ubld.it TrueRNG USB
entropy generator (which is about the size and shape of a thumb drive) to fill
the system entropy pool on a Mac OS X system.

    ./Scattergun/etc/udev/rules.d/99-TrueRNG.rules
    ./Scattergun/etc/default/rng-tools-TrueRNG

    ./Scattergun/etc/udev/rules.d/99-TrueRNGpro.rules
    ./Scattergun/etc/default/rng-tools-TrueRNGpro

It has some udev rules and rng-tools configuration files that make it easy to
use a ubld.it TrueRNG or TrueRNGpro hardware entropy generators to fill the
system entropy pool on a GNU/Linux system.
