/* ------------------------------------------------------------------ */
/**
 * @file    test_bignum_is_zero_runner.c
 * @brief   Интеграционный runner для bignum_is_zero.
 */
/* ------------------------------------------------------------------ */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bignum_is_zero.h"

int main(void)
{
    bignum_t value;

    printf("Running test: test_bignum_is_zero_runner... ");

    assert(bignum_is_zero(NULL) == BIGNUM_IS_ZERO_ERROR_NULL_ARG);

    memset(&value, 0, sizeof(value));
    assert(bignum_is_zero(&value) == BIGNUM_IS_ZERO_TRUE);

    value.words[0] = UINT64_C(1);
    value.len = 1U;
    assert(bignum_is_zero(&value) == BIGNUM_IS_ZERO_FALSE);

    value.len = 0U;
    assert(bignum_is_zero(&value) == BIGNUM_IS_ZERO_TRUE);

    puts("PASSED");
    return 0;
}
