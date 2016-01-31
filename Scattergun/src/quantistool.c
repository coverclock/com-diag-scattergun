/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * ID Quantique Quantis tool<BR>
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA.<BR>
 * "Digital Aggregates Corporation" is a registered trademark.<BR>
 * Licensed under the terms of the GNU GPL v2.<BR>
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
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include "Quantis.h"
#include "com/diag/diminuto/diminuto_dump.h"

static const QuantisDeviceType TYPES[] = { QUANTIS_DEVICE_PCI, QUANTIS_DEVICE_USB };
static const char * NAMES[] = { "PCI", "USB" };

static const char * program = "quantistool";
static int debug = 0;

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
    int done = 0;
    int error = 0;
    int debug = 0;
    int verbose = 0;
    QuantisDeviceType type = QUANTIS_DEVICE_USB;
    unsigned int unit = 0;
    size_t size = QUANTIS_MAX_READ_SIZE;
    size_t written = 0;
    char * end = (char *)0;
    QuantisDeviceHandle * handle = (QuantisDeviceHandle *)0;
    unsigned char * buffer = (unsigned char *)0;
    int rc = 0;
    FILE * fp = (FILE *)0;
    uintptr_t offset = 0;

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

        if (done) {
            break;
        }

    }

    do {

        if (error) {
            usage();
            break;
        }

        if (verbose) {  
            query();
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

        rc = QuantisOpen(type, unit, &handle);
        if (rc < QUANTIS_SUCCESS) {
            fprintf(stderr, "%s: QuantisOpen %d \"%s\"\n", program, rc, QuantisStrError(rc));
            break;
        }

        if (verbose) {
            fprintf(stderr, "%s: handle       %p\n", program, handle);
        }

        fp = stdout;

        for (;;) {
            rc = QuantisReadHandled(handle, buffer, size);
            if (rc < QUANTIS_SUCCESS) {
                fprintf(stderr, "%s: QuantisReadHandled %d \"%s\"\n", program, rc, QuantisStrError(rc));
                break;
            }
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

        xc = 0;

    } while (0);

    if (handle != (QuantisDeviceHandle *)0) {
        QuantisClose(handle);
    }

    if (buffer != (unsigned char *)0) {
        free(buffer);
    }

    return xc;
}
