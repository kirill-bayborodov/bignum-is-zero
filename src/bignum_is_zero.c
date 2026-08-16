/* ------------------------------------------------------------------ */
/**
 * @file    bignum_is_zero.c
 * @brief   Эталонная переносимая C-реализация bignum_is_zero.
 */
/* ------------------------------------------------------------------ */
#include "bignum_is_zero.h"

bignum_is_zero_result_t bignum_is_zero(const bignum_t *x)
{
    if (x == NULL) {
        return BIGNUM_IS_ZERO_ERROR_NULL_ARG;
    }
    return x->len == 0U ? BIGNUM_IS_ZERO_TRUE : BIGNUM_IS_ZERO_FALSE;
}
