/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * Seven Tool<BR>
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
 * seventool [ -h ] [ -d ] [ -v ] [ -D ] [ -i IDENT ] [ -R [ -r ] | -S ] [ -c ] [ -x ] [ -o PATH ]
 *
 * EXAMPLES
 *
 * ABSTRACT
 *
 * Continuously reads a word of entropy using the rdrand or rdseed instructions
 * available on various Intel processors such as certain models of the i7 and
 * writes it to standard output, or to a specified file system path. This
 * latter object could be a FIFO, which could allow generated entropy to be
 * read by another program, like rngd. Optionally does some other useful stuff
 * regarding examining the capabilities of the host processor. This is part of
 * the Scattergun project.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#define  __RDRND__
#include <immintrin.h>

static const char * program = "seventool";
static const char * ident = "seventool";
static int debug = 0;
static int verbose = 0;
static int done = 0;
static int report = 0;
static int daemonize = 0;

enum mode { FAIL=0, RDRAND=1, RDSEED=2, };
static const char * MODE[] = { "fail", "rdrand", "rdseed", };

/**
 * Emit a formatting string to either the system log or to standard error.
 * @param format is the printf format.
 */
static void lprintf(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    if (daemonize) {
        vsyslog(LOG_DEBUG, format, ap);
    } else {
        vfprintf(stderr, format, ap);
    }
    va_end(ap);
}

/**
 * Emit a formatting string to either the system log or to standard error
 * if verbosity is enabled.
 * @param format is the printf format.
 */
static void lverbosef(const char * format, ...)
{
    if (verbose) {
        va_list ap;
        va_start(ap, format);
        if (daemonize) {
            vsyslog(LOG_DEBUG, format, ap);
        } else {
            vfprintf(stderr, format, ap);
        }
        va_end(ap);
    }
}

/**
 * Emit a caller provider string and an error message string corresponding to
 * the current value of the error number (errno) to either the system log or
 * to standard error.
 * @param string is the string.
 */
static void lerror(const char * string)
{
    if (daemonize) {
        syslog(LOG_ERR, "%s: %s\n", string, strerror(errno));
    } else {
        fprintf(stderr, "%s: %s\n", string, strerror(errno));
    }
}

/**
 * Handle a signal. In the event of a SIGPIPE or a SIGINT, the program shuts
 * down in an orderly fashion. In the event of a SIGHUP, it emits some
 * statistics to standard error.
 * @param signum is the number of the incoming signal.
 */
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

/**
 * Emit a usage message to standard error.
 * @param nomenu if true supresses the printing of the menu.
 */
static void usage(int nomenu)
{
    lprintf("usage: %s [ -h ] [ -d ] [ -v ] [ -D ] [ -i IDENT ] [ -R [ -r ] | -S ] [ -c ] [ -x ] [ -o PATH ]\n", program);
    if (nomenu) { return; }
    lprintf("       -d            Enable debug mode\n");
    lprintf("       -v            Enable verbose mode\n");
    lprintf("       -D            Run as a daemon\n");
    lprintf("       -i IDENT      Use IDENT as the syslog identifier\n");
    lprintf("       -R            Use the rdrand instruction\n");
    lprintf("       -r            Force the rdrand DRNG to reseed beforehand\n");
    lprintf("       -S            Use the rdseed instruction\n");
    lprintf("       -c            Check for instruction, exit if unimplemented\n");
    lprintf("       -x            Perform check only, exit afterwards\n");
    lprintf("       -o PATH       Write to PATH (which may be a fifo) instead of stdout\n");
    lprintf("       -h            Print help menu\n");
}

/**
 * Run the cpuid instruction with the specified leaf and subleaf and return the
 * resulting values of the EAX, EBX, ECX< and EDX registers.
 * @param ap points to the variable into which EAX is returned.
 * @param bp points to the variable into which EBX is returned.
 * @param cp points to the variable into which ECX is returned.
 * @param dp points to the variable into which EDX is returned.
 * @param l is the cpuid leaf to be loaded in register EAX.
 * @param s is the cpuid subleaf to be loaded into register ECX. 
 */
static void cpuid(uint32_t * ap, uint32_t * bp, uint32_t * cp, uint32_t * dp, uint32_t l, uint32_t s)
{
    asm volatile ("cpuid" : "=a" (*ap), "=b" (*bp), "=c" (*cp), "=d" (*dp) : "a" (l), "c" (s) );
 
    lverbosef("%s: cpuid\n", program);
    lverbosef("%s: leaf         %d\n", program, l);
    lverbosef("%s: subleaf      %d\n", program, s);
    lverbosef("%s: eax          0x%8.8x\n", program, *ap);
    lverbosef("%s: ebx          0x%8.8x\n", program, *bp);
    lverbosef("%s: ecx          0x%8.8x\n", program, *cp);
    lverbosef("%s: edx          0x%8.8x\n", program, *dp);
}

/**
 * Run the rdrand instruction.
 * @param wp points to the result word.
 * @return the carry bit indicating success.
 */
static inline uint8_t rdrand(uint32_t * wp)
{
#if defined(SCATTERGUN_HAS_RDRAND_INLINE)
    return _rdrand32_step(wp);
#elif defined(SCATTERGUN_HAS_RDRAND_INTRINSIC)
    return __builtin_ia32_rdrand32_step(wp);
#elif defined(SCATTERGUN_HAS_RDRAND_MNEMONIC)
    uint8_t carry = 1;
    asm volatile ("rdrand %0; setc %1" : "=r" (*wp), "=qm" (carry));
    return carry;
#else
    uint8_t carry = 1;
    asm volatile (".byte 0x0f,0xc7,0xf0; setc %0" : "=qm" (carry), "=a" (*wp));
    return carry;
#endif
}

/**
 * Run the rdseed instruction.
 * @param wp points to the result word.
 * @return the carry bit indicating success.
 */
static inline uint8_t rdseed(uint32_t * wp)
{
#if defined(SCATTERGUN_HAS_RDSEED_INTRINSIC)
    return __builtin_ia32_rdseed32_step(wp);
#elif defined(SCATTERGUN_HAS_RDSEED_MNEMONIC)
    uint8_t carry = 1;
    asm volatile ("rdseed %0; setc %1" : "=r" (*wp), "=qm" (carry));
    return carry;
#else
    uint8_t carry = 1;
    asm volatile (".byte 0x0f,0xc7,0xf8; setc %0" : "=qm" (carry), "=a" (*wp));
    return carry;
#endif
}

/**
 * Use the cpuid instruction to query the CPU to see what kind it is, and if
 * it is an Intel, whether it implements the rdrand or the rdseed instruction.
 * (Note that some AMD processors implement these instructions as well.)
 * @return a mask with the RDRAND and/or RDSEED bits set if supported.
 */
static int query(void)
{
    int result = 0;
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;

    cpuid(&a, &b, &c, &d, 0, 0);

    if ((memcmp((char *)&b, "Genu", 4) == 0) && (memcmp((char *)&d, "ineI", 4) == 0) && (memcmp((char *)&c, "ntel", 4) == 0)) {

        lverbosef("%s: cpu          Intel\n", program);

        cpuid(&a, &b, &c, &d, 1, 0);
        if (c & 0x40000000) {
            lverbosef("%s: rdrand       available\n", program);
            result |= 1<<RDRAND;
        } else {
            lverbosef("%s: rdrand       unavailable\n", program);
        }

        cpuid(&a, &b, &c, &d, 7, 0);
        if (b & 0x00040000) {
            lverbosef("%s: rdseed       available\n", program);
            result |= 1<<RDSEED;
        } else {
            lverbosef("%s: rdseed       unavailable\n", program);
        }
    } else {

        lverbosef("%s: cpu          other\n", program);

    }

    return result;
}

/**
 * Force the rdrand mechanism to reseed. We do this by calling rdrand just
 * one more time than its reseed cycle. The rdrand DRNG is guaranteed to
 * produce no more than (511 * 2) or 1022 64-bit results using the same seed.
 * Where exactly in this loop the rdrand reseeds is transparent and unknowable.
 * @return true if the all of the rdrand calls succeeded, false otherwise.
 */
static int reseed(void)
{
    static const int RESEED = ((511 * 2 * 64) / sizeof(uint32_t)) + 1;
    int count = 0;
    uint32_t word = 0;
    uint8_t carry = 1;
    int ii;

    lverbosef("%s: reseeding    %d\n", program, RESEED);

    for (ii = 0; ii < RESEED; ++ii) {
        carry = rdrand(&word);
        if (carry) {
            ++count;
        }
    }

    lverbosef("%s: reseeded     %d\n", program, count);

    return (count == RESEED);
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
    unsigned int unit = 0;
    size_t written = 0;
    char * end = (char *)0;
    int rc = 0;
    FILE * fp = stdout;
    uintptr_t offset = 0;
    struct sigaction sigpipe = { 0 };
    struct sigaction sighup = { 0 };
    struct sigaction sigint = { 0 };
    struct timespec request = { 0 };
    size_t tries = 0;
    size_t total = 0;
    size_t reads = 0;
    size_t consecutive = 0;
    const char * path = (const char *)0;
    enum mode mode = FAIL;
    int doreseed = 0;
    int docheck = 0;
    int doexit = 0;
    int opt;
    extern char * optarg;
    uint32_t word;
    uint8_t carry;
    static const uint32_t CAFEBEEF = 0xCAFEBEEF;
    static const uint32_t DEADCODE = 0xDEADC0DE;
    static const uint32_t NANOSECONDS = 1000000;
    static const size_t CONSECUTIVE = 10;

    /*
     * Crack open the command line argument vector.
     */

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "dvDo:i:hRrScx")) >= 0) {

        switch (opt) {

        case 'd':
            debug = !0;
            break;

        case 'v':
            verbose = !0;
            break;

        case 'D':
            daemonize = !0;
            break;

        case 'o':
            path = optarg;
            break;

        case 'i':
            ident = optarg;
            break;

        case 'h':
            xc = 0;
            error = !0;
            break;

        case 'R':
            mode = RDRAND;
            break;

        case 'r':
            doreseed = !0;
            break;

        case 'S':
            mode = RDSEED;
            break;

        case 'c':
            docheck = !0;
            break;

        case 'x':
            docheck = !0;
            doexit = !0;
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
            usage(xc);
            break;
        }

        if (daemonize) {
            if (daemon(0, 0) < 0) {
                perror("daemon");
                break;
            }
            openlog(ident, LOG_CONS | LOG_PID, LOG_DAEMON);
            lverbosef("%s: pid          %d\n", program, getpid());
        }

        if (docheck) {  
            rc = query();
            if ((mode == RDRAND) && ((rc & (1 << RDRAND)) == 0)) {
                break;
            } else if ((mode == RDSEED) && ((rc & (1 << RDSEED)) == 0)) {
                break;
            } else if (doexit) {
                xc = 0;
                break;
            } else {
                /* Do nothing. */
            }
        }

        /*
         * Install our signal handlers.
         */

        sigpipe.sa_handler = handler;
        sigpipe.sa_flags = 0;
        rc = sigaction(SIGPIPE, &sigpipe, (struct sigaction *)0);
        if (rc < 0) {
            lerror("sigaction");
            break;
        }

        sighup.sa_handler = handler;
        sighup.sa_flags = SA_RESTART;
        rc = sigaction(SIGHUP, &sighup, (struct sigaction *)0);
        if (rc < 0) {
            lerror("sigaction");
            break;
        }

        sigint.sa_handler = handler;
        sigint.sa_flags = 0;
        rc = sigaction(SIGINT, &sighup, (struct sigaction *)0);
        if (rc < 0) {
            lerror("sigaction");
            break;
        }

        /*
         * Switch from stdout to PATH if so configured.
         */

        if (path != (const char *)0) {
            lverbosef("%s: path         \"%s\"\n", program, path);
            fp = fopen(path, "a");
            if (fp == (FILE *)0) {
                lerror(path);
                break;
            }
        }

        lverbosef("%s: mode         %s\n", program, MODE[mode]);

        /*
         * Force a reseed if requested and if using rdrand.
         */

        if (!doreseed) {
            /* Do nothing. */
        } else if (mode != RDRAND) {
            /* Do nothing. */
        } else if (reseed()) {
            /* Do nothing. */
        } else {
            errno = EBUSY;
            lerror("reseed");
            break;
        }

        /*
         * Enter our work loop.
         */

        request.tv_sec = NANOSECONDS / 1000000000;
        request.tv_nsec = NANOSECONDS % 1000000000;

        xc = 0;

        while (!done) {

            if (report) {
                lprintf("%s: tries=%zu size=%zu reads=%zu total=%zu\n", program, tries, sizeof(word), reads, total);
                report = 0;
            }

            ++tries;

            if (mode == RDRAND) {
                word = CAFEBEEF;
                carry = rdrand(&word);
            } else if (mode == RDSEED) {
                word = CAFEBEEF;
                carry = rdseed(&word);
            } else {
                word = DEADCODE;
                carry = 1;
            }

            if (carry) {
                consecutive = 0;
            } else if ((++consecutive) >= CONSECUTIVE) {
                errno = EBUSY;
                lerror("carry");
                xc = 2;
                break;
            } else if ((rc = nanosleep(&request, (struct timespec *)0)) >= 0) {
                continue;
            } else if (errno == EINTR) {
                continue;
            } else {
                lerror("nanosleep");
                xc = 2;
                break;
            }

            ++reads;
            total += sizeof(word);

            written = fwrite(&word, sizeof(word), 1, fp);
            if (written >= 1) {
                /* Do nothing: nominal. */
            } else if (!ferror(fp)) {
                break;
            } else if (errno == EPIPE) {
                lerror("fwrite");
                break;
            } else {
                lerror("fwrite");
                xc = 2;
                break;
            }

            if (debug) {
                lprintf("%s: tries=%zu size=%zu reads=%zu total=%zu\n", program, tries, sizeof(word), reads, total);
            }

        }

    } while (0);

    /*
     * Clean up after ourselves.
     */

    if (fp != (FILE *)0) {
        fclose(fp);
    }

    lverbosef("%s: tries=%zu size=%zu reads=%zu total=%zu\n", program, tries, sizeof(word), reads, total);

    return xc;
}
