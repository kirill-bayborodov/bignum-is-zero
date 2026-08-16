# bignum-is-zero

[![C/ASM CI](https://github.com/kirill-bayborodov/bignum-is-zero/actions/workflows/ci.yml/badge.svg)](https://github.com/kirill-bayborodov/bignum-is-zero/actions/workflows/ci.yml)
[![GitHub release](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-is-zero?label=release)](https://github.com/kirill-bayborodov/bignum-is-zero/releases/latest)

`bignum-is-zero` is a standalone C/ASM module that checks whether a `bignum_t` represents zero. The production path is an x86-64 YASM implementation conforming to the System V AMD64 ABI; a portable C11 implementation is retained as a reference and fallback.

The function checks the pointer and the normalized `len` field only. It does not inspect the `words` array. Therefore, a non-NULL object with `len > 0` is considered non-zero even if its words are all zero. This follows the normalized representation contract provided by `bignum-core`.

## Distribution

The module is intended to be used as a standalone component of the `bignum-lib` family. The required `bignum-core` component is included as a Git submodule at `libs/bignum-core`.

## Features

- **Dual implementation:** x86-64 YASM is the primary implementation and C11 is the reference implementation.
- **Typed result enum:** the public API returns `bignum_is_zero_result_t`.
- **Explicit NULL error:** `x == NULL` returns `BIGNUM_IS_ZERO_ERROR_NULL_ARG` without dereferencing the pointer.
- **Normalized zero semantics:** for a non-NULL object, the result depends only on `x->len`.
- **Deterministic verification:** tests cover NULL, zero, non-zero, maximum-word, and unnormalized states.
- **Extended verification:** canaries, all supported `len` values, and 100,000 deterministic fuzz-style cases are covered.
- **Thread-safety testing:** independent objects are checked concurrently.
- **Reproducible benchmarks:** ST and MT benchmarks support deterministic seeds, fingerprints, checksums, warm-up calls, source lengths, and `all_zero`, `all_nonzero`, and `mixed` modes.
- **Perf workflow:** the unchanged template Makefile provides `perf record`, repeated `perf stat`, raw `perf.data` retention, runtime validation, and comparison targets.

## Dependencies

| Dependency | Purpose |
|---|---|
| `make` | Build, test, lint, benchmark, and distribution targets |
| `gcc` | C compilation and linking |
| `yasm` | x86-64 assembly compilation |
| `cppcheck` | Static analysis |
| `perf` | Performance counters and sampling profiles |
| `taskset` | CPU affinity control |
| `pthread` | Multithreaded tests and benchmarks |

Clone the repository with its submodule:

```bash
git clone --recurse-submodules https://github.com/kirill-bayborodov/bignum-is-zero.git
cd bignum-is-zero
```

For an existing clone, initialize the submodule with:

```bash
git submodule update --init --recursive
```

## API

The public API is declared in `include/bignum_is_zero.h`:

```c
typedef enum {
    BIGNUM_IS_ZERO_FALSE          = 0,
    BIGNUM_IS_ZERO_TRUE           = 1,
    BIGNUM_IS_ZERO_ERROR_NULL_ARG = -1
} bignum_is_zero_result_t;

bignum_is_zero_result_t bignum_is_zero(const bignum_t *x);
```

### Contract

| Condition | Return value | Memory behavior |
|---|---|---|
| `x == NULL` | `BIGNUM_IS_ZERO_ERROR_NULL_ARG` | The pointer is not dereferenced |
| `x != NULL && x->len == 0` | `BIGNUM_IS_ZERO_TRUE` | Only `len` is read |
| `x != NULL && x->len > 0` | `BIGNUM_IS_ZERO_FALSE` | Only `len` is read |

The NULL check is mandatory and is performed before accessing `x->len`. NULL is therefore an invalid argument, not a valid numeric zero. A valid object with `len == 0` is the zero case.

Example:

```c
#include "bignum_is_zero.h"

bignum_is_zero_result_t check_value(const bignum_t *value)
{
    return bignum_is_zero(value);
}
```

The C and ASM implementations return the same typed values. The ASM implementation clears `eax` before executing `sete`, ensuring that the complete register contains either `0` or `1` for valid non-NULL inputs.

## Build and test

Build the release object and submodule:

```bash
make build CONFIG=release
```

The production object is generated at:

```text
build/bignum_is_zero.o
```

Run the full deterministic, extended, multithreaded, and integration-runner suite against the ASM implementation:

```bash
make test CONFIG=release
```

The expected summary is:

```text
=== Summary: 0 / 4 failed ===
```

To test the portable C reference implementation instead of the ASM implementation:

```bash
make clean
make test CONFIG=release USE_ASM=no
```

Run static analysis:

```bash
make lint
```

The test files are organized as follows:

| File | Scope |
|---|---|
| `tests/test_bignum_is_zero.c` | Deterministic NULL, zero, non-zero, and unnormalized-state tests |
| `tests/test_bignum_is_zero_extra.c` | Canaries, `len` matrix, and fuzz-style/reference checks |
| `tests/test_bignum_is_zero_mt.c` | Concurrent independent-object checks |
| `tests/test_bignum_is_zero_runner.c` | Integration smoke test |

## Benchmarks

The benchmark sources are:

```text
benchmarks/bench_bignum_is_zero.c
benchmarks/bench_bignum_is_zero_mt.c
```

The benchmarks measure the predicate call rather than array initialization. Each iteration prepares a `bignum_t` with the requested logical `len`, invokes `bignum_is_zero`, validates that the result is not `BIGNUM_IS_ZERO_ERROR_NULL_ARG`, and incorporates the typed result into the checksum.

| Mode | Logical input | Expected result | Purpose |
|---|---:|---|---|
| `all_zero` | `len == 0` | `BIGNUM_IS_ZERO_TRUE` | Measures the valid zero path |
| `all_nonzero` | `len == src_len` | `BIGNUM_IS_ZERO_FALSE` | Measures the valid non-zero path |
| `mixed` | Alternating `0` and `src_len` | Both valid results | Measures a mixed branch workload |

### Single-thread CLI

```text
bin/bench_bignum_is_zero \
  [--iterations N] \
  [--warmup N] \
  [--data-count N] \
  [--src-len N] \
  [--seed N] \
  [--data-mode all_zero|all_nonzero|mixed]
```

Example:

```bash
./bin/bench_bignum_is_zero \
  --iterations 1000000 \
  --warmup 10000 \
  --data-count 8192 \
  --src-len 32 \
  --seed 0x9e3779b97f4a7c15 \
  --data-mode mixed
```

### Multithread CLI

```text
bin/bench_bignum_is_zero_mt \
  [--threads N] \
  [--iterations N|--total-iterations N] \
  [--warmup N] \
  [--data-count N] \
  [--src-len N] \
  [--seed N] \
  [--data-mode all_zero|all_nonzero|mixed]
```

`--iterations` means iterations per thread. `--total-iterations` specifies total work and must be nonzero and divisible by `--threads`; the benchmark derives the per-thread count internally.

For a fair one-thread/two-thread comparison, hold total work constant:

```bash
./bin/bench_bignum_is_zero_mt \
  --threads 1 \
  --total-iterations 3200000000 \
  --src-len 32 \
  --data-mode mixed

./bin/bench_bignum_is_zero_mt \
  --threads 2 \
  --total-iterations 3200000000 \
  --src-len 32 \
  --data-mode mixed
```

## Perf workflow

The current environment provides two logical CPUs. The corresponding MT settings are:

```make
MT_THREADS=2
MT_CPU_LIST=0-1
MT_TOTAL_ITERATIONS=3200000000
```

Run the complete ST/MT workflow for the supported data modes:

```bash
make bench_full CONFIG=release \
  REPORT_NAME=baseline \
  PERF_RUNS=7 \
  KEEP_PERF=1
```

For targeted repeated counter measurements:

```bash
make bench_stat_st CONFIG=release \
  REPORT_NAME=baseline_st_mixed \
  DATA_MODE=mixed \
  PERF_RUNS=7

make bench_stat_mt CONFIG=release \
  REPORT_NAME=baseline_mt_mixed \
  DATA_MODE=mixed \
  MT_THREADS=2 \
  MT_CPU_LIST=0-1 \
  MT_TOTAL_ITERATIONS=3200000000 \
  PERF_RUNS=7
```

Reports are written to `benchmarks/reports/`. With `KEEP_PERF=1`, raw profiles are retained as `.perf.data` files. Runtime validation checks the dynamic benchmark identifier generated from `LIB_NAME`, the selected data mode, and the elapsed-time field.

A reproducible optimization comparison should keep `CONFIG`, `PERF_RUNS`, `DATA_MODE`, `src_len`, seed, thread count, CPU affinity, and total iterations constant:

```bash
make clean
make test CONFIG=release
make bench_full CONFIG=release REPORT_NAME=baseline PERF_RUNS=7 KEEP_PERF=1

# Change implementation, then repeat the verification.
make clean
make test CONFIG=release
make bench_full CONFIG=release REPORT_NAME=opt_v1 PERF_RUNS=7 KEEP_PERF=1
```

Compare matching reports only:

```bash
diff -u \
  benchmarks/reports/baseline_all_nonzero_st_stat.csv \
  benchmarks/reports/opt_v1_all_nonzero_st_stat.csv
```

If hardware PMU access or a kernel-matched `perf` binary is unavailable, run the benchmark binaries directly and report those measurements separately from `perf` results.

## Installation and distribution

Build the object-file distribution:

```bash
make install CONFIG=release
```

Build the single-header and static-library distribution:

```bash
make dist CONFIG=release
```

Remove generated artifacts:

```bash
make clean
```

## Linking the object file

```bash
make build CONFIG=release

gcc your_app.c \
  build/bignum_is_zero.o \
  -I./include \
  -I./libs/bignum-core/include \
  -o your_app \
  -no-pie
```

The application must use the same System V AMD64 ABI and include the `bignum_t` definition supplied by `bignum-core`.

## Contributing

Contributions should preserve the C/ASM API contract, including the typed NULL error, add or update deterministic and extended tests, and run both `make test CONFIG=release` and `make lint`. Performance changes should include reproducible benchmark parameters and matching ST/MT evidence.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
