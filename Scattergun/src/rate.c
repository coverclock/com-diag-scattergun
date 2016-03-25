/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * Rate<BR>
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
 * rate [ -h ] [ -d ] [ -v ] [ -f PATH ] [ -r BYTES ] [ -t BYTES ]
 *
 * OPTIONS
 *
 * -f PATH     Read from here instead of stdin.
 * -r BYTES    Read no more than this at a time.
 * -t BYTES    Read no more than this total.
 * -h          Display this menu.
 * -d          Display debug output.
 * -v          Display verbose output.
 *
 * EXAMPLES
 *
 * rate -f /dev/TrueRNGpro -r 4096 -t 1000000000
 *
 * ABSTRACT
 *
 * Measures the sustained and peak rates of a data source.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static const char * program = "rate";

static uint64_t watch(void)
{
    int rc;
    uint64_t ticks = ~0;
    struct timespec spec;

    rc = clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
    if (rc == 0) {
        ticks = spec.tv_sec;
        ticks *= 1000000000;
        ticks += spec.tv_nsec;
    } else {
        perror("clock_gettime");
    }

    return ticks;
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -d ] [ -f PATH ] [ -h ] [ -r BYTES ] [ -t BYTES ] [ -v ] \n", program);
    fprintf(stderr, "       -d          Display debug output.\n");
    fprintf(stderr, "       -f PATH     Read from here instead of stdin.\n");
    fprintf(stderr, "       -h          Display this menu.\n");
    fprintf(stderr, "       -r BYTES    Read no more than this at a time.\n");
    fprintf(stderr, "       -t BYTES    Read no more than this total.\n");
    fprintf(stderr, "       -v          Display verbose output.\n");
}

/**
 * This is the main program.
 * @param argc is the count of command line arguments.
 * @param argv is a vector of pointers to the command line arguments.
 */
int main(int argc, char * argv[])
{
    int xc = 1;
    int error = 0;
    size_t size = 4096;
    const char * path = (const char *)0;
    int fd = STDIN_FILENO;
    uint8_t * buffer = (uint8_t *)0;
    size_t limit = ~0;
    size_t remaining = 0;
    int rc = 0;
    char * end = (char *)0;
    int debug = 0;
    int verbose = 0;
    int opt;
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "df:ht:r:v")) >= 0) {

        switch (opt) {

        case 'd':
            debug = !0;
            break;

        case 'f':
            path = optarg;
            break;

        case 'h':
            usage();
            break;

        case 'r':
            size = strtoul(optarg, &end, 0);
            if ((*end != '\0') || (size == 0)) {
                errno = EINVAL;
                perror(optarg);
                error = !0;
            }
            break;

        case 't':
            limit = strtoul(optarg, &end, 0);
            if (*end != '\0') {
                errno = EINVAL;
                perror(optarg);
                error = !0;
            }
            break;

        case 'v':
            verbose = !0;
            break;

        default:
            usage();
            error = !0;
            break;

        }

        if (error) {
            break;
        }

    }

    do {
        uint64_t epoch = 0;
        uint64_t then = 0;
        uint64_t now = 0;
        uint64_t elapsed = 0;
        uint64_t duration = 0;
        ssize_t bytes = 0;
        size_t reads = 0;
        size_t total = 0;
        size_t sustained = 0;
        size_t burst = 0;
        size_t average = 0;
        size_t minimum = ~0;
        size_t maximum = 0;
        size_t low = ~0;
        size_t peak = 0;

        if (error) {
            break;
        }

        buffer = malloc(size);
        if (buffer == (unsigned char *)0) {
            perror("malloc");
            break;
        }

        if (path != (const char *)0) {
            fd = open(path, O_RDONLY);
            if (fd < 0) {
                perror(path);
                break;
            }
        }

        fprintf(stderr, "%s: %zu bytes limit\n", program, limit);
        fprintf(stderr, "%s: %zu bytes requested\n", program, size);

        remaining = limit;
        epoch = watch();
        while (remaining >= size) {

            then = watch();
            bytes = read(fd, buffer, size);
            now = watch();
            if (bytes < 0) {
                perror("read");
                break;
            } else if (bytes == 0) {
                xc = 0;
                break;
            } else {
                /* Do nothing. */
            }

            reads += 1;
            total += bytes;
            remaining -= bytes;

            average = (total + (reads / 2)) / reads;

            elapsed = now - epoch;
            sustained = (total * 1000000) / elapsed;

            duration = now - then;
            burst = (bytes * 1000000) / duration;

            if (debug) {
                printf("%lu %zu %lu %zu %zu\n", elapsed, bytes, duration, burst, sustained);
            }

            if (verbose && ((burst > peak) || (burst < low))) {
                fprintf(stderr, "%s: %lu nanoseconds elapsed\n", program, elapsed);
                fprintf(stderr, "%s: %lu nanoseconds burst\n", program, duration);
                fprintf(stderr, "%s: %zu bytes read\n", program, bytes);
            }

            if (bytes < minimum) {
                minimum = bytes;
            }

            if (bytes > maximum) {
                maximum = bytes;
            }

            if (burst < low) {
                low = burst;
                if (verbose) {
                    fprintf(stderr, "%s: %zu kilobytes/second low\n", program, low);
                }
            }

            if (burst > peak) {
                peak = burst;
                if (verbose) {
                    fprintf(stderr, "%s: %zu kilobytes/second peak\n", program, peak);
                }
            }

        }

        if (close(fd) < 0) {
            perror("close");
        }

        fprintf(stderr, "%s: %zu bytes total\n", program, total);
        fprintf(stderr, "%s: %lu milliseconds elapsed\n", program, elapsed / 1000000);
        fprintf(stderr, "%s: %zu reads\n", program, reads);
        fprintf(stderr, "%s: %zu bytes minimum\n", program, minimum);
        fprintf(stderr, "%s: %zu bytes average\n", program, average);
        fprintf(stderr, "%s: %zu bytes maximum\n", program, maximum);
        fprintf(stderr, "%s: %zu kilobytes/second low\n", program, low);
        fprintf(stderr, "%s: %zu kilobytes/second sustained\n", program, sustained);
        fprintf(stderr, "%s: %zu kilobytes/second peak\n", program, peak);

    } while (0);

    return xc;
}
