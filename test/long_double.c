/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2022 Keith Packard
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <float.h>

#ifdef LDBL_MANT_DIG

static long double max_error;

bool
within_error(long double expect, long double result, long double error)
{
    long double difference;
    long double e = 1.0L;

    if (isnan(expect) && isnan(result))
        return true;

    if (expect == result)
        return true;

    if (expect != 0)
        e = scalbnl(1.0L, -ilogbl(expect));

    difference = fabsl(expect - result) * e;

    if (difference > max_error)
        max_error = difference;

    return difference <= error;
}

int
check_long_double(const char *name, int i, long double prec, long double expect, long double result)
{
    if (!within_error(expect, result, prec)) {
        long double diff = fabsl(expect - result);
#ifdef __PICOLIBC__
        printf("%s test %d got %.15g expect %.15g diff %.15g\n", name, i, (double) result, (double) expect, (double) diff);
#else
//        printf("%s test %d got %.33Lg expect %.33Lg diff %.33Lg\n", name, i, result, expect, diff);
        printf("%s test %d got %La expect %La diff %La\n", name, i, result, expect, diff);
#endif
        return 1;
    }
    return 0;
}

typedef struct {
    const char *name;
    int (*test)(void);
} long_double_test_t;

typedef struct {
    int line;
    long double x;
    long double y;
} long_double_test_f_f_t;

typedef struct {
    int line;
    long double x0;
    long double x1;
    long double y;
} long_double_test_f_ff_t;

/*
 * sqrtl is "exact", but can have up to one bit of error as it might
 * not have enough guard bits to correctly perform rounding, leading
 * to some answers getting rounded to an even value instead of the
 * (more accurate) odd value
 */
#if LDBL_MANT_DIG == 64
#define DEFAULT_PREC 1e-16L
#define SQRTL_PREC 0x8.0p-63
#define FULL_LONG_DOUBLE
#elif LDBL_MANT_DIG == 113
#define FULL_LONG_DOUBLE
#define DEFAULT_PREC 1e-31L
#define SQRTL_PREC 0x8.0p-112
#elif LDBL_MANT_DIG == 106
#define DEFAULT_PREC 1e-29L
#define SQRTL_PREC 0x8.0p-105
#endif

#define CEILL_PREC      0
#define FLOORL_PREC     0
#define LOGBL_PREC      0
#define NEARBYINTL_PREC 0
#define ROUNDL_PREC     0
#define TRUNCL_PREC     0

#include "long_double_vec.h"

int main(void)
{
    int result = 0;
    unsigned int i;

    printf("LDBL_MANT_DIG %d\n", LDBL_MANT_DIG);
    for (i = 0; i < sizeof(long_double_tests) / sizeof(long_double_tests[0]); i++) {
        result += long_double_tests[i].test();
    }
    return result != 0;
}

#else
int main(void)
{
    printf("no long double support\n");
    return 0;
}
#endif
