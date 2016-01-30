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
 * idqtool [ -? ] [ -d ] [ -i ] [ -u UNIT | -p UNIT ] [ -r BYTES ]
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

static const char * program = "idqtool";
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

int main(int argc, char * argv[])
{
    int xc = 1;
    int done = 0;
    int error = 0;
    int debug = 0;
    int verbose = 0;
    QuantisDeviceType type = QUANTIS_DEVICE_USB;
    unsigned int unit = 0;
    size_t reading = QUANTIS_MAX_READ_SIZE;
    size_t written = 0;
    char * end = (char *)0;
    QuantisDeviceHandle * handle = (QuantisDeviceHandle *)0;
    unsigned char * buffer = (unsigned char *)0;
    int rc = 0;
    
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
            reading = strtoul(optarg, &end, 0);
            if ((*end != '\0') || (!((0 < reading) && (reading <= QUANTIS_MAX_READ_SIZE)))) {
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

        buffer = malloc(reading);
        if (buffer == (unsigned char *)0) {
            perror("malloc");
            break;
        }

        rc = QuantisOpen(type, unit, &handle);
        if (rc != QUANTIS_SUCCESS) {
            perror("QuantisOpen");
            break;
        }

        for (;;) {
            rc = QuantisReadHandled(handle, buffer, reading);
            if (rc != QUANTIS_SUCCESS) {
                perror("QuantisReadHandled");
                break;
            }
            written = fwrite(buffer, reading, 1, stdout);
            if (written < reading) {
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
