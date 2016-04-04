/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * Seed<BR>
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
 * seed [ -i | -l ]
 *
 * OPTIONS
 *
 * -i              Generate an unsigned integer seed.
 * -l              Generate an unsigned long seed.
 *
 * EXAMPLES
 *
 * seed -l > LONG
 * seed -i > INTEGER
 *
 * ABSTRACT
 *
 * Generates an unsigned long or an unsigned integer seed (one some platforms
 * this might be the same bit-size, on others not so much) and prints it to
 * standard output.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static const char * program = "seed";

/**
 * This is the main program.
 * @param argc is the count of command line arguments.
 * @param argv is a vector of pointers to the command line arguments.
 */
int main(int argc, char * argv[])
{
    int dolong = !0;
    int rc;
    uint64_t ticks = ~0;
    struct timespec spec = { 0 };
    unsigned long ll;
    unsigned int ii;
    int opt;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "il")) >= 0) {

        switch (opt) {

        case 'i':
            dolong = 0;
            break;

        case 'l':
            dolong = !0;
            break;

        default:
            fprintf(stderr, "%s: [ -l | -i ]\n", program);
            exit(1);
            break;

        }

    }

    rc = clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
    if (rc == 0) {
        ticks = spec.tv_sec;
        ticks *= 1000000000;
        ticks += spec.tv_nsec;
    } else {
        perror("clock_gettime");
        exit(1);
    }

    if (dolong) {
        ll = ticks;
        printf("%lu\n", ll);
    } else {
        ii = ticks;
        printf("%u\n", ii);
    }

    exit(0);
}
