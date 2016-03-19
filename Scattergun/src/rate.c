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
 * EXAMPLES
 *
 * ABSTRACT
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
static size_t reads = 0;
static size_t total = 0;
static int64_t then = 0;
static int hungup = 0;

/**
 * Handle a signal. In the event of a SIGPIPE or a SIGINT, the program shuts
 * down in an orderly fashion. In the event of a SIGHUP, it emits some
 * statistics to standard error.
 * @param signum is the number of the incoming signal.
 */
static void handler(int signum)
{
    if (signum == SIGHUP) {
        hungup = !0;
    } else {
        /* Do nothing. */
    }
}

static int64_t watch(void)
{
    int rc;
    int64_t ticks = -1;
    struct timespec elapsed;

    rc = clock_gettime(CLOCK_MONOTONIC_RAW, &elapsed);
    if (rc == 0) {
        ticks = elapsed.tv_sec;
        ticks *= 1000000000;
        ticks += elapsed.tv_nsec;
    } else {
        perror("clock_gettime");
    }

    return ticks;
}

static void report(void)
{
    int64_t elapsed;
    elapsed = watch() - then;
    printf("%s: %lu milliseconds\n", program, elapsed / 1000000);
    printf("%s: %zu bytes\n", program, total);
    printf("%s: %zu reads\n", program, reads);
    if ((total == 0) || (reads == 0)) { return; }
    printf("%s: %zu bytes/read\n", program, total / reads);
    printf("%s: %zu kilobytes/second\n", program, (total * 1000000) / elapsed);
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
    int debug = 0;
    int verbose = 0;
    size_t size = 4096;
    const char * path = (const char *)0;
    int fd = STDIN_FILENO;
    uint8_t * buffer = (uint8_t *)0;
    ssize_t bytes = 0;
    size_t limit = ~0;
    int rc = 0;
    char * end = (char *)0;
    struct sigaction sighup = { 0 };
    int opt;
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "f:l:r:")) >= 0) {

        switch (opt) {

        case 'f':
            path = optarg;
            break;

        case 'l':
            limit = strtoul(optarg, &end, 0);
            if (*end != '\0') {
                errno = EINVAL;
                perror(optarg);
                error = !0;
            }
            break;

        case 'r':
            size = strtoul(optarg, &end, 0);
            if ((*end != '\0') || (size == 0)) {
                errno = EINVAL;
                perror(optarg);
                error = !0;
            }
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
            break;
        }

        buffer = malloc(size);
        if (buffer == (unsigned char *)0) {
            perror("malloc");
            break;
        }

        sighup.sa_handler = handler;
        sighup.sa_flags = SA_RESTART;
        rc = sigaction(SIGHUP, &sighup, (struct sigaction *)0);
        if (rc < 0) {
            perror("sigaction");
            break;
        }

        if (path != (const char *)0) {
            fd = open(path, O_RDONLY);
            if (fd < 0) {
                perror(path);
                break;
            }
        }

        then = watch();
        while (limit > 0) {

            bytes = read(fd, buffer, (size < limit) ? size : limit);
            if (bytes < 0) {
                perror("read");
                break;
            } else if (bytes == 0) {
                xc = 0;
                break;
            } else {
                reads += 1;
                total += bytes;
                limit -= bytes;
            }

            if (hungup) {
                report();
                hungup = 0;
            }
    
        }

        if (close(fd) < 0) {
            perror("close");
        }

    } while (0);

    report();

    return xc;
}
