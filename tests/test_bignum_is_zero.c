/* ------------------------------------------------------------------ */
/**
 * @file    test_bignum_is_zero.c
 * @brief   Детерминированные тесты bignum_is_zero.
 */
/* ------------------------------------------------------------------ */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bignum_is_zero.h"

static void test_null(void)
{
    assert(bignum_is_zero(NULL) == BIGNUM_IS_ZERO_ERROR_NULL_ARG);
    puts("test_null: PASSED");
}

static void test_null_error(void)
{
    assert(bignum_is_zero(NULL) == BIGNUM_IS_ZERO_ERROR_NULL_ARG);
    puts("test_null_error: PASSED");
}


static void test_initialized_zero(void)
{
    bignum_t b;

    memset(&b, 0, sizeof(b));
    assert(bignum_is_zero(&b) == BIGNUM_IS_ZERO_TRUE);
    puts("test_initialized_zero: PASSED");
}

static void test_len_zero_ignores_words(void)
{
    bignum_t b;

    memset(&b, 0xA5, sizeof(b));
    b.len = 0U;
    assert(bignum_is_zero(&b) == BIGNUM_IS_ZERO_TRUE);
    puts("test_len_zero_ignores_words: PASSED");
}

static void test_len_one_nonzero(void)
{
    bignum_t b;

    memset(&b, 0, sizeof(b));
    b.words[0] = UINT64_C(1);
    b.len = 1U;
    assert(bignum_is_zero(&b) == BIGNUM_IS_ZERO_FALSE);
    puts("test_len_one_nonzero: PASSED");
}

static void test_uint64_max(void)
{
    bignum_t b;

    memset(&b, 0, sizeof(b));
    b.words[0] = UINT64_MAX;
    b.len = 1U;
    assert(bignum_is_zero(&b) == BIGNUM_IS_ZERO_FALSE);
    puts("test_uint64_max: PASSED");
}

static void test_unnormalized_zero_is_nonzero(void)
{
    bignum_t b;

    memset(&b, 0, sizeof(b));
    b.words[0] = 0U;
    b.len = 1U;
    assert(bignum_is_zero(&b) == BIGNUM_IS_ZERO_FALSE);

    b.len = 0U;
    assert(bignum_is_zero(&b) == BIGNUM_IS_ZERO_TRUE);
    puts("test_unnormalized_zero_is_nonzero: PASSED");
}

int main(void)
{
    puts("--- Starting deterministic bignum_is_zero tests ---");
    test_null();
    test_null_error();
    test_initialized_zero();
    test_len_zero_ignores_words();
    test_len_one_nonzero();
    test_uint64_max();
    test_unnormalized_zero_is_nonzero();
    puts("--- All deterministic bignum_is_zero tests passed ---");
    return 0;
}
