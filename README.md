com-diag-scattergun
-------------------

Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
Licensed under the terms of the GNU GPL v2.

This project represents my puttering around with hardware entropy sources and
random number generation in Linux/GNU, and to a lesser extent, Mac OS X.

Many thanks to my friend, occasional colleague, former office mate, and
mentor REDACTED, who has a wealth of deep expertise in this domain.

Scattergun includes a variety of artifacts that might be useful.

TEST SUITE

    ./Scattergun/bin/scattergun.sh
    ./Scattergun/Makefile
    ./Scattergun/out
    ./Scattergun/results

It has a script that runs a suite of tests (none of which I wrote) that
assess the randomness of an entropy source, a Makefile to drive the tests,
and the output of the test suite when it was run on a variety of hardware
entropy generators.

ID QUANTIQUE QUANTIS

    ./Scattergun/src/quantistool.c
    ./Scattergun/fs/etc/init.d/quantis
    ./Scattergun/fs/etc/udev/rules.d/99-idq-quantis.rules
    ./Scattergun/fs/etc/init.d/rng-tools.diff
    ./Scattergun/fs/etc/default/rng-tools-quantis

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

INTEL RDRAND AND RDSEED

    ./Scattergun/src/seventool.c
    ./Scattergun/fs/etc/init.d/rdrand
    ./Scattergun/fs/etc/default/rng-tools-rdrand

It has a utility, written in C, that examines the host processor using the
Intel cpuid instruction, and if it indicates that the processor implements
either the rdrand or rdseed instruction, extracts random bits using the
specified instruction and writes them to standard output, or to a file which
can be a named pipe.

UBLD.IT TRUERNG AND TRUERNGPRO

    ./Scattergun/fs/etc/udev/rules.d/99-TrueRNG.rules
    ./Scattergun/fs/etc/default/rng-tools-TrueRNG

    ./Scattergun/fs/etc/udev/rules.d/99-TrueRNGpro.rules
    ./Scattergun/fs/etc/default/rng-tools-TrueRNGpro

It has some udev rules and rng-tools configuration files that make it easy to
use a ubld.it TrueRNG or TrueRNGpro hardware entropy generators to fill the
system entropy pool on a GNU/Linux system.

MOONBASE OTAGO ONERNG

    ./Scattergun/fs/etc/udev/rules.d/99-OneRNG.rules
    ./Scattergun/fs/etc/default/rng-tools-OneRNG

It has some udev rules and rng-tools configuration files that make it easy to
use a Moonbase Otago OneRNG hardware entropy generators to fill the system
entropy pool on a GNU/Linux system.

GNU FST-1 NEUG

    ./Scattergun/fs/etc/udev/rules.d/99-NeuG.rules

It has some udev files and rng-tools configuration files.

RASPBERRY PI BCM2708

    ./Scattergun/fs/etc/default/rng-tools-bcm2708

It has an rng-tools configuration file so that the option Broadcom hardware
random number generator device driver module can be used to fill the system
entropy pool on a Raspberry Pi 2 (and perhaps others).

MAC OS X

    ./Scattergun/bin/truerngd.sh
    ./Scattergun/fs/etc/default/truerngd.conf

It has a simple script (admittedly experimental) to use a ubld.it TrueRNG USB
entropy generator (which is about the size and shape of a thumb drive) to fill
the system entropy pool on a Mac OS X system.

OTHER STUFF

    ./Scattergun/src/bytes.c
    ./Scattergun/src/cmrand48.c
    ./Scattergun/src/crandom.c

It includes some programs that emit constant bytes, random numbers
generated using the C library's mrand48(3) function, or with the random(3)
function. It is informative to compare the results of the hardware entropy
generators with those of these two pseudo-random number generators.
