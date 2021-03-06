/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * C MRand48<BR>
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
 * cmrand48 [ SEED ]
 *
 * EXAMPLES
 *
 * cmrand48 | dd of=random.dat bs=4096 count=1024 iflag=fullblock
 *
 * cmrand48 0xDEADBEEF | dd of=random.dat bs=4096 count=1024 iflag=fullblock
 * 
 * ABSTRACT
 *
 * Continuously output thirty-two bit binary numbers generated by the C
 * library's mrand48(3) function. These numbers will range in value from
 * -2^31 to 2^31. If an argument is specified, it is used to seed the
 * pseudo-random number generator.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    int32_t value;
    if (argc >= 2) {
        char * end;
        unsigned long seed;
        seed = strtoul(argv[1], &end, 0);
        if ((*end != '\0') || (seed == 0)) {
            errno=EINVAL;
            perror(argv[1]);
            return 1;
        }
        srand48(seed);
    }
    while (!0) {
        value = mrand48();
        if (fwrite(&value, sizeof(value), 1, stdout) == 1) {
            /* Do nothing. */
        } else if (ferror(stdout)) {
            perror("fwrite");
            return 1;
        } else {
            return 0;
        }
    }
}
