/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * ID Quantique Quantis tool<BR>
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA.<BR>
 * "Digital Aggregates Corporation" is a registered trademark.<BR>
 * Licensed under the terms of the Scattergun license.<BR>
 * author:Chip Overclock<BR>
 * mailto:coverclock@diag.com<BR>
 * http://www.diag.com/nagivation/downloads/Scattergun.html<BR>
 * http://github.com/coverclock/com-diag-scattergun<BR>
 *
 * USAGE
 *
 * quantistool [ -? ] [ -d ] [ -i ] [ -u UNIT | -p UNIT ] [ -r BYTES ]
 *
 * EXAMPLES
 *
 * ABSTRACT
 *
 * Continuously reads random data from a Quantis hardware entropy generator
 * and writes it to standard output. Optionally does some other useful stuff
 * with the Quantis. This is part of the Scattergun project. This must be
 * linked with the Quantis library.
 *
 * The program incorporates portions of the source code of the Diminuto
 * library, which is also a product of the Digital Aggregates Corporation.
 * The copied portions of Diminuto are licensed under the Scattergun license,
 * or, at your options, the original Diminuto license (which is less
 * restrictive).
 *
 * The Quantis software library I have restricts reads to sizes of no more
 * than 16 megabytes (16 * 1024 * 1024). But it restricts each individual
 * read from the device to the block transfer size advertised by the device.
 * The lsusb command suggests that the max packet size for my Quantis USB
 * device is 512 bytes. There doesn't seem to be any mechanism to just "read
 * what you got" so that we get all available random bits without possibly
 * blocking to wait for more or leaving some behind.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "Quantis.h"
#include "com/diag/diminuto/diminuto_dump.h"

static const QuantisDeviceType TYPES[] = { QUANTIS_DEVICE_PCI, QUANTIS_DEVICE_USB };
static const char * NAMES[] = { "PCI", "USB" };

static const char * program = "quantistool";
static int debug = 0;
static int verbose = 0;
static int done = 0;
static int report = 0;

static void handler(int signum)
{
    if (signum == SIGPIPE) {
        done = !0;
    } else if (signum == SIGINT) {
        done = !0;
    } else if (signum == SIGHUP) {
        report = !0;
    } else {
        /* Do nothing. */
    }
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -? ] [ -d ] [ -v ] [ -u UNIT | -p UNIT ] [ -r BYTES ]\n", program);
    fprintf(stderr, "       -d            Enable debug mode\n");
    fprintf(stderr, "       -v            Enable verbose mode\n");
    fprintf(stderr, "       -u UNIT       Use USB card UNIT\n");
    fprintf(stderr, "       -p UNIT       Use PCI card UNIT\n");
    fprintf(stderr, "       -r BYTES      Read at most BYTES bytes at a time\n");
    fprintf(stderr, "       -?            Print menu\n");
}

static int query(void)
{
    int ii;

    for (ii = 0; ii < (sizeof(TYPES) / sizeof(TYPES[0])); ++ii) {
        QuantisDeviceType type = 0;
        float software = 0.0;
        int detected = 0;
        int jj;

        type = TYPES[ii];
        software = QuantisGetDriverVersion(type);
        detected = QuantisCount(type);

        fprintf(stderr, "%s: type         %s\n", program, NAMES[ii]);
        fprintf(stderr, "%s: software     %f\n", program, software);
        fprintf(stderr, "%s: detected     %d\n", program, detected);

        for (jj = 0; jj < detected; ++jj) {
            int hardware = 0;
            const char * serial = (const char *)0;
            const char * manufacturer = (const char *)0;
            int power = 0;
            int mask = 0;
            int status = 0;

            hardware = QuantisGetBoardVersion(type, jj);
            serial = QuantisGetSerialNumber(type, jj);
            manufacturer = QuantisGetManufacturer(type, jj);
            power = QuantisGetModulesPower(type, jj);
            mask = QuantisGetModulesMask(type, jj);
            status = QuantisGetModulesStatus(type, jj);

            fprintf(stderr, "%s: unit         %d\n", program, jj);
            fprintf(stderr, "%s: hardware     %d\n", program, hardware);
            fprintf(stderr, "%s: serial       \"%s\"\n", program, serial);
            fprintf(stderr, "%s: manufacturer \"%s\"\n", program, manufacturer);
            fprintf(stderr, "%s: power        %d\n", program, power);
            fprintf(stderr, "%s: modules      0x%8.8x\n", program, mask);
            fprintf(stderr, "%s: status       0x%8.8x\n", program, status);
        }
    }
}

int main(int argc, char * argv[])
{
    int xc = 1;
    int error = 0;
    QuantisDeviceType type = QUANTIS_DEVICE_USB;
    unsigned int unit = 0;
    size_t size = 512;
    size_t written = 0;
    char * end = (char *)0;
    QuantisDeviceHandle * handle = (QuantisDeviceHandle *)0;
    unsigned char * buffer = (unsigned char *)0;
    int rc = 0;
    FILE * fp = stdout;
    uintptr_t offset = 0;
    struct sigaction sigpipe = { 0 };
    struct sigaction sighup = { 0 };
    struct sigaction sigint = { 0 };
    size_t opens = 0;
    size_t total = 0;
    size_t reads = 0;
    int opt;
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "dvu:p:r:?")) >= 0) {

        switch (opt) {

        case 'd':
            debug = !0;
            break;

        case 'v':
            verbose = !0;
            break;

        case 'u':
            type = QUANTIS_DEVICE_USB;
            unit = strtoul(optarg, &end, 0);
            break;

        case 'p':
            type = QUANTIS_DEVICE_PCI;
            unit = strtoul(optarg, &end, 0);
            break;

        case 'r':
            size = strtoul(optarg, &end, 0);
            if ((*end != '\0') || (!((0 < size) && (size <= QUANTIS_MAX_READ_SIZE)))) {
                errno = EINVAL;
                perror(optarg);
                error = !0;
            }
            break;

        case '?':
            usage();
            break;

        default:
            error = !0;
            break;

        }

        if (error) {
            break;
        }

    }

    do {

        if (error) {
            usage();
            break;
        }

        if (verbose) {
            fprintf(stderr, "%s: pid          %d\n", program, getpid());
        }

        if (verbose) {  
            query();
        }

        if (size > QUANTIS_MAX_READ_SIZE) {
            size = QUANTIS_MAX_READ_SIZE;
        }

        if (verbose) {
            int ii;

            for (ii = 0; ii < (sizeof(TYPES) / sizeof(TYPES[0])); ++ii) {
                if (TYPES[ii] == type) {
                    fprintf(stderr, "%s: type         %s\n", program, NAMES[ii]);
                }
            }
            fprintf(stderr, "%s: unit         %d\n", program, unit);
            fprintf(stderr, "%s: size         %zu\n", program, size);
        }

        buffer = malloc(size);
        if (buffer == (unsigned char *)0) {
            perror("malloc");
            break;
        }

        sigpipe.sa_handler = handler;
        sigpipe.sa_flags = 0;
        rc = sigaction(SIGPIPE, &sigpipe, (struct sigaction *)0);
        if (rc < 0) {
            perror("sigaction");
            break;
        }

        sighup.sa_handler = handler;
        sighup.sa_flags = SA_RESTART;
        rc = sigaction(SIGHUP, &sighup, (struct sigaction *)0);
        if (rc < 0) {
            perror("sigaction");
            break;
        }

        sigint.sa_handler = handler;
        sigint.sa_flags = 0;
        rc = sigaction(SIGINT, &sighup, (struct sigaction *)0);
        if (rc < 0) {
            perror("sigaction");
            break;
        }

        while (!done) {

            rc = QuantisOpen(type, unit, &handle);
            if (rc < QUANTIS_SUCCESS) {
                fprintf(stderr, "%s: QuantisOpen(%d,%d,%p)=%d=\"%s\"\n", program, type, unit, handle,  rc, QuantisStrError(rc));
                break;
            }
            ++opens;

            if (verbose) {
                fprintf(stderr, "%s: handle       %p\n", program, handle);
            }

            while (!done) {
                if (report) {
                    fprintf(stderr, "%s: opens=%zu size=%zu reads=%zu total=%zu\n", program, opens, size, reads, total);
                    report = 0;
                }
                rc = QuantisReadHandled(handle, buffer, size);
                if (rc < QUANTIS_SUCCESS) {
                    fprintf(stderr, "%s: QuantisReadHandled(%p,%p,%zu)=%d=\"%s\"\n", program, handle, buffer, size, rc, QuantisStrError(rc));
                    break;
                }
                ++reads;
                total += size;
                if (debug) {
                    diminuto_dump_virtual(stderr, buffer, size, offset);
                    fputc('\n', stderr);
                    offset += size;
                }
                written = fwrite(buffer, size, 1, fp);
                if (written < 1) {
                    perror("fwrite");
                    break;
                }
            }

            if (handle != (QuantisDeviceHandle *)0) {
                QuantisClose(handle);
            }

        }

        xc = 0;

    } while (0);

    if (buffer != (unsigned char *)0) {
        free(buffer);
    }

    if (verbose) {
        fprintf(stderr, "%s: opens=%zu size=%zu reads=%zu total=%zu\n", program, opens, size, reads, total);
    }

    return xc;
}
