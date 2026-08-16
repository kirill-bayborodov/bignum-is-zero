/* ------------------------------------------------------------------ */
/**
 * @file    test_bignum_is_zero_mt.c
 * @brief   Multithreaded correctness tests for bignum_is_zero.
 */
/* ------------------------------------------------------------------ */
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bignum_is_zero.h"

typedef struct {
    size_t id;
    int failed;
} worker_data_t;

static void *worker(void *arg)
{
    worker_data_t *data = arg;
    bignum_t value;

    for (size_t iteration = 0; iteration < 10000U; ++iteration) {
        memset(&value, 0, sizeof(value));
        value.len = (iteration + data->id) % (BIGNUM_CAPACITY + 1U);
        value.words[0] = UINT64_C(0x1000) + iteration + data->id;

        if (bignum_is_zero(&value) != (value.len == 0U ? BIGNUM_IS_ZERO_TRUE : BIGNUM_IS_ZERO_FALSE)) {
            data->failed = 1;
            return NULL;
        }
    }
    return NULL;
}

int main(void)
{
    enum { THREAD_COUNT = 8 };
    pthread_t threads[THREAD_COUNT];
    worker_data_t data[THREAD_COUNT];

    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        data[i].id = i;
        data[i].failed = 0;
        assert(pthread_create(&threads[i], NULL, worker, &data[i]) == 0);
    }
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        assert(pthread_join(threads[i], NULL) == 0);
        assert(data[i].failed == 0);
    }

    puts("--- Multithreaded bignum_is_zero test passed ---");
    return 0;
}
