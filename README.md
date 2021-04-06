com-diag-scattergun
-------------------

# Copyright

Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.

# License

Licensed under the terms of the GNU GPL v2.

# Say Thanks!

[![Say Thanks!](https://img.shields.io/badge/Say%20Thanks-!-1EAEDB.svg)](https://saythanks.io/to/coverclock)

# CONTACT

Chip Overclock    
Digital Aggregates Corporation    
3440 Youngfield Street, Suite 209    
Wheat Ridge CO 80033 USA    
<http://www.diag.com>    
<mailto:coverclock@diag.com>    

# ABSTRACT

This project represents my puttering around with hardware entropy sources and
random number generation in Linux/GNU, and to a lesser extent, Mac OS X.

For too much information, see my blog article about this project.

<http://coverclock.blogspot.com/2017/02/the-need-for-non-determinacy.html>

Scattergun includes a variety of artifacts that might be useful.

# TEST SUITE

    ./Scattergun/bin/scattergun.sh
    ./Scattergun/dat
    ./Scattergun/Makefile
    ./Scattergun/out

It has a script that runs a suite of tests (none of which I wrote) that
assess the randomness of an entropy source, a Makefile to drive the tests,
and the results of the test suite when it was run on a variety of hardware
entropy generators.

## ID QUANTIQUE QUANTIS

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

## INTEL RDRAND AND RDSEED

    ./Scattergun/src/seventool.c
    ./Scattergun/fs/etc/init.d/rdrand
    ./Scattergun/fs/etc/default/rng-tools-rdrand

It has a utility, written in C, that examines the host processor using the
Intel cpuid instruction, and if it indicates that the processor implements
either the rdrand or rdseed instruction, extracts random bits using the
specified instruction and writes them to standard output, or to a file which
can be a named pipe.

## UBLD.IT TRUERNGV2, TRUERNGPRO, TRUERNGV3

    ./Scattergun/fs/etc/udev/rules.d/99-TrueRNG.rules
    ./Scattergun/fs/etc/default/rng-tools-TrueRNG

    ./Scattergun/fs/etc/udev/rules.d/99-TrueRNGpro.rules
    ./Scattergun/fs/etc/default/rng-tools-TrueRNGpro

It has some udev rules and rng-tools configuration files that make it easy to
use a ubld.it TrueRNG or TrueRNGpro hardware entropy generators to fill the
system entropy pool on a GNU/Linux system.

## MOONBASE OTAGO ONERNG AND ONERNG V3.0

    ./Scattergun/fs/etc/udev/rules.d/99-OneRNG.rules
    ./Scattergun/fs/etc/default/rng-tools-OneRNG

It has some udev rules and rng-tools configuration files that make it easy to
use a Moonbase Otago OneRNG hardware entropy generators to fill the system
entropy pool on a GNU/Linux system.

## GNU FST-1 NEUG

    ./Scattergun/fs/etc/udev/rules.d/99-NeuG.rules

It has some udev files for the GNU FST-1 NEUG hardware entropy generator.

## VOICETRONIX BITBABBLER WHITE AND BITBABBLER BLACK

    ./Scattergun/fs/etc/udev/rules.d/99-BitBabbler.rules

It has some udev files for the Voicetronix BitBabbler White and Black
hardware engropy generators.

## RASPBERRY PI BCM2708

    ./Scattergun/fs/etc/default/rng-tools-bcm2708

It has an rng-tools configuration file so that the optional Broadcom hardware
random number generator device driver module can be used to fill the system
entropy pool on a Raspberry Pi 2 or 3 (and perhaps others).

## ALTUS METRUM CHAOS KEY

    ./Scattergun/fs/etc/udev/rules.d/99-ChaosKey.rules

## WAYWORD GEEK INFINITE NOISE

    ./Scattergun/fs/etc/udev/rules.d/99-InfiniteNoise.rules

## MAC OS X

    ./Scattergun/bin/truerngd.sh
    ./Scattergun/fs/etc/default/truerngd.conf

It has a simple script (admittedly experimental) to use a ubld.it TrueRNG USB
entropy generator (which is about the size and shape of a thumb drive) to fill
the system entropy pool on a Mac OS X system.

## OTHER STUFF

    ./Scattergun/src/bytes.c
    ./Scattergun/src/cmrand48.c
    ./Scattergun/src/crandom.c
    ./Scattergun/src/getrandom.c

It includes some programs that emit constant bytes, random numbers
generated using the C library's mrand48(3) function, with the random(3)
function, or with the getrandom(2) system call. It is informative to
compare the results of the hardware entropy generators with those of
the pseudo-random number generators.

# NOTES

If you send a SIGUSR1 to the dd command process, it will print I/O statistics
and then continue on its merry way.
