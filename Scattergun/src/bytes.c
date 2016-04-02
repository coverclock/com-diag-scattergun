/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * Bytes<BR>
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
 * bytes
 *
 * EXAMPLES
 *
 * bytes | dd of=random.dat bs=4096 count=1024 iflag=fullblock
 *
 * bytes 0xff | dd of=random.dat bs=4096 count=1024 iflag=fullblock
 *
 * ABSTRACT
 *
 * Continuously output eight-bit binary bytes with the value zero. If an
 * argument is specified, a byte with that value is output instead.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    uint8_t value = 0;
	if (argc >= 2) {
        char * end;
        value = strtoul(argv[1], &end, 0);
        if (*end != '\0') {
            errno=EINVAL;
            perror(argv[1]);
            return 1;
        }
    }
    while (!0) {
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
