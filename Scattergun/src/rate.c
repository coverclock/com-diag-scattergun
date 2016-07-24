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
 * rate [ -h ] [ -c NANOSECONDS ] [ -v ] [ -f PATH ] [ -r BYTES ] [ -t BYTES ]
 *
 * OPTIONS
 *
 * -c NANOSECONDS  Display CSV output to stdout.
 * -f PATH         Read from here instead of stdin.
 * -h              Display this menu.
 * -r BYTES        Read no more than this at a time.
 * -t BYTES        Read no more than this total.
 * -v              Display verbose output to stderr.
 *
 * EXAMPLES
 *
 * rate -f /dev/TrueRNGpro -r 4096 -t 1000000000
 *
 * ABSTRACT
 *
 * Measures the sustained and peak rates of a data source. Optionally outputs
 * a comma separated value (CSV) file of performance metrics with the specified
 * period.
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
#include <sys/time.h>
#include <fcntl.h>
#include <float.h>

static const char * program = "rate";

static uint64_t watch(void)
{
    int rc;
    uint64_t ticks = ~0;
    struct timespec spec = { 0 };

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

static uint64_t timer(uint64_t ns)
{
    int rc;
    uint64_t ticks = ~0;
    struct itimerval itimer = { 0 };

    itimer.it_value.tv_sec = ns / 1000000000;
    itimer.it_value.tv_usec = (ns % 1000000000) / 1000;
    itimer.it_interval = itimer.it_value;
    rc = setitimer(ITIMER_REAL, &itimer, (struct itimerval *)0);
    if (rc == 0) {
        ticks = itimer.it_value.tv_sec;
        ticks *= 1000000000;
        ticks += (itimer.it_value.tv_usec * 1000);
    } else {
        perror("setitimer");
    }

    return ticks;
}

static int alarmed = 0;

static void alarming(int signum)
{
    if (signum == SIGALRM) {
        alarmed = !0;
    }
}

static int alarmable(void)
{
    int rc;
    struct sigaction action = { 0 };

    action.sa_handler = alarming;
    action.sa_flags = SA_RESTART;
    rc = sigaction(SIGALRM, &action, (struct sigaction *)0);
    if (rc < 0) {
        perror("sigaction");
    }

    return rc;
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -c NANOSECONDS ] [ -f PATH ] [ -h ] [ -r BYTES ] [ -t BYTES ] [ -v ] \n", program);
    fprintf(stderr, "       -c NANOSECONDS  Display CSV output to stdout.\n");
    fprintf(stderr, "       -f PATH         Read from here instead of stdin.\n");
    fprintf(stderr, "       -h              Display this menu.\n");
    fprintf(stderr, "       -r BYTES        Read no more than this at a time.\n");
    fprintf(stderr, "       -t BYTES        Read no more than this total.\n");
    fprintf(stderr, "       -v              Display verbose output to stderr.\n");
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
    int verbose = 0;
    uint64_t period = 0;
    int opt;
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "c:f:ht:r:v")) >= 0) {

        switch (opt) {

        case 'c':
            period = strtoul(optarg, &end, 0);
            if ((*end != '\0') || (size == 0)) {
                errno = EINVAL;
                perror(optarg);
                error = !0;
            }
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
        uint64_t hence = ~0;
        uint64_t now = ~0;
        uint64_t elapsed = 0;
        uint64_t duration = 0;
        ssize_t bytes = 0;
        size_t reads = 0;
        size_t interval = 0;
        size_t total = 0;
        size_t minimum = ~0;
        size_t maximum = 0;
        double instantaneous = 0.0;
        double peak = 0;
        double sustained = 0.0;
        double current = 0.0;

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

        if (size > limit) {
            size = limit;
        }

        fprintf(stderr, "%s: %zu bytes limit\n", program, limit);
        fprintf(stderr, "%s: %zu bytes requested\n", program, size);

        if (period > 0) {
            fprintf(stderr, "%s: %lu nanoseconds period\n", program, period);
            printf("%s,%s,%s,%s,%s,%s\n", "Elapsed", "Minimum", "Maximum", "Current", "Sustained", "Peak");
            alarmable();
            timer(period);
        }

        remaining = limit;
        epoch = watch();

        while (!0) {

            if (remaining < size) {
                xc = 0;
                break;
            } else if ((bytes = read(fd, buffer, size)) == 0) {
                xc = 0;
                break;
            } else if (bytes < 0) {
                perror("read");
                break;
            } else {
                /* Do nothing. */
            }

            reads += 1;
            interval += bytes;
            total += bytes;
            remaining -= bytes;

            if (bytes < minimum) {
                minimum = bytes;
            }

            if (bytes > maximum) {
                maximum = bytes;
            }

            then = now;
            now = watch();

            elapsed = now - epoch;
            sustained = total;
            sustained *= 8;
            sustained *= 1000000;
            sustained /= elapsed;

            if (then != ~0) {

                duration = now - then;
                instantaneous = bytes;
                instantaneous *= 8;
                instantaneous *= 1000000;
                instantaneous /= duration;

                if (instantaneous > peak) {
                    peak = instantaneous;
                }

            }

            if (alarmed) {

                if (hence != ~0) {
                    duration = now - hence;
                    current = interval;
                    current *= 8;
                    current *= 1000000;
                    current /= duration;
                    interval = 0;
                    printf("%lu,%zu,%zu,%lf,%lf,%lf\n", elapsed, minimum, maximum, current, sustained, peak);
                }

                hence = now;
                alarmed = 0;

            }

        }

        if (close(fd) < 0) {
            perror("close");
        }

        fprintf(stderr, "%s: %zu bytes total\n", program, total);
        fprintf(stderr, "%s: %lf milliseconds elapsed\n", program, elapsed / 1000000.0);
        fprintf(stderr, "%s: %zu reads\n", program, reads);

        if (reads <= 0) {
            break;
        }

        fprintf(stderr, "%s: %lf bytes average\n", program, (0.0 + total) / reads);
        if (reads <= 1) {
            break;
        }

        fprintf(stderr, "%s: %zu bytes minimum\n", program, minimum);
        fprintf(stderr, "%s: %zu bytes maximum\n", program, maximum);
        fprintf(stderr, "%s: %lf kilobits/second sustained\n", program, sustained);
        fprintf(stderr, "%s: %lf kilobits/second peak\n", program, peak);

    } while (0);

    return xc;
}
