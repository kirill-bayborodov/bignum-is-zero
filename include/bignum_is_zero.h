/**
 * @file    bignum_is_zero.h
 * @brief   Проверка нулевого значения bignum_t.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_IS_ZERO_H
#define BIGNUM_IS_ZERO_H

#include "bignum.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Типизированный результат проверки нулевого значения. */
typedef enum {
    BIGNUM_IS_ZERO_FALSE          = 0,
    BIGNUM_IS_ZERO_TRUE           = 1,
    BIGNUM_IS_ZERO_ERROR_NULL_ARG = -1
} bignum_is_zero_result_t;

/**
 * @brief Возвращает типизированный результат проверки x.
 *
 * Возвращается BIGNUM_IS_ZERO_ERROR_NULL_ARG, если x == NULL.
 * Для ненулевого x возвращается BIGNUM_IS_ZERO_TRUE при x->len == 0U,
 * иначе возвращается BIGNUM_IS_ZERO_FALSE. Функция проверяет только указатель
 * и поле len; содержимое words не анализируется.
 *
 * @param[in] x Проверяемый объект.
 * @return BIGNUM_IS_ZERO_TRUE, BIGNUM_IS_ZERO_FALSE или
 *         BIGNUM_IS_ZERO_ERROR_NULL_ARG.
 */
bignum_is_zero_result_t bignum_is_zero(const bignum_t *x);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_IS_ZERO_H */

/* SPDX-License-Identifier: MIT */
