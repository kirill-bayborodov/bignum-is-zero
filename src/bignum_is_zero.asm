; -----------------------------------------------------------------------------
; @file    bignum_is_zero.asm
; @brief   Проверка bignum_t на нулевое значение на x86-64 YASM.
; @details System V ABI: rdi = x.
; -----------------------------------------------------------------------------
; SPDX-License-Identifier: MIT
; -----------------------------------------------------------------------------

default rel
section .text
    align 16
    global bignum_is_zero

BIGNUM_WORD_SIZE    equ 8
BIGNUM_CAPACITY     equ 32
BIGNUM_OFFSET_LEN   equ BIGNUM_CAPACITY * BIGNUM_WORD_SIZE
BIGNUM_IS_ZERO_FALSE          equ 0
BIGNUM_IS_ZERO_TRUE           equ 1
BIGNUM_IS_ZERO_ERROR_NULL_ARG equ -1

; bignum_is_zero_result_t bignum_is_zero(const bignum_t *x)
bignum_is_zero:
    ; NULL is an invalid argument.
    mov     eax, BIGNUM_IS_ZERO_ERROR_NULL_ARG
    test    rdi, rdi
    jz      .ret

    ; Return the typed TRUE/FALSE value according to len.
    xor     eax, eax
    cmp     qword [rdi + BIGNUM_OFFSET_LEN], 0
    sete    al

.ret:
    ret
