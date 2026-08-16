/* ------------------------------------------------------------------ */
/**
 * @file    test_bignum_is_zero_extra.c
 * @brief   Расширенные и fuzz-style тесты bignum_is_zero.
 */
/* ------------------------------------------------------------------ */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bignum_is_zero.h"

typedef struct {
    uint64_t left;
    bignum_t value;
    uint64_t right;
} guarded_bignum_t;

static uint64_t next_value(uint64_t *state)
{
    *state ^= *state << 7;
    *state ^= *state >> 9;
    *state ^= *state << 8;
    return *state;
}

static void test_guard_canaries_and_boundaries(void)
{
    guarded_bignum_t guarded;

    guarded.left = UINT64_C(0x1122334455667788);
    guarded.right = UINT64_C(0x8877665544332211);
    memset(&guarded.value, 0, sizeof(guarded.value));
    guarded.value.len = BIGNUM_CAPACITY;

    assert(bignum_is_zero(&guarded.value) == BIGNUM_IS_ZERO_FALSE);
    assert(guarded.left == UINT64_C(0x1122334455667788));
    assert(guarded.right == UINT64_C(0x8877665544332211));
    puts("test_guard_canaries_and_boundaries: PASSED");
}

static void test_len_contract(void)
{
    bignum_t value;

    memset(&value, 0xA5, sizeof(value));
    for (size_t len = 0; len <= BIGNUM_CAPACITY; ++len) {
        value.len = len;
        assert(bignum_is_zero(&value) == (len == 0U ? BIGNUM_IS_ZERO_TRUE : BIGNUM_IS_ZERO_FALSE));
    }
    puts("test_len_contract: PASSED");
}

static void test_fuzz_reference_equivalence(void)
{
    uint64_t state = UINT64_C(0x9E3779B97F4A7C15);
    bignum_t value;

    for (size_t iteration = 0; iteration < 100000U; ++iteration) {
        size_t len = (size_t)(next_value(&state) % (BIGNUM_CAPACITY + 1U));

        for (size_t i = 0; i < BIGNUM_CAPACITY; ++i) {
            value.words[i] = next_value(&state);
        }
        value.len = len;
        assert(bignum_is_zero(&value) == (len == 0U ? BIGNUM_IS_ZERO_TRUE : BIGNUM_IS_ZERO_FALSE));
    }
    puts("test_fuzz_reference_equivalence: PASSED (100000 cases)");
}

static void test_unnormalized_values(void)
{
    bignum_t value;

    memset(&value, 0, sizeof(value));
    value.words[0] = 0U;
    value.len = 1U;
    assert(bignum_is_zero(&value) == BIGNUM_IS_ZERO_FALSE);
    value.len = 0U;
    assert(bignum_is_zero(&value) == BIGNUM_IS_ZERO_TRUE);
    puts("test_unnormalized_values: PASSED");
}

int main(void)
{
    puts("--- Starting extended bignum_is_zero tests ---");
    test_guard_canaries_and_boundaries();
    test_len_contract();
    test_fuzz_reference_equivalence();
    test_unnormalized_values();
    puts("--- All extended bignum_is_zero tests passed ---");
    return 0;
}
