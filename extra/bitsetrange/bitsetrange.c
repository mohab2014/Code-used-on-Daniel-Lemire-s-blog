// gcc -O3 -o bitsetrange bitsetrange.c

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

__attribute__ ((noinline))
static void bitset_set_range_noinline(uint64_t *bitmap, uint32_t start, uint32_t end) {
    if (start == end) return;
    uint32_t firstword = start / 64;
    uint32_t endword = (end - 1) / 64;
    if (firstword == endword) {
        bitmap[firstword] |= ((~UINT64_C(0)) << (start % 64)) &
                             ((~UINT64_C(0)) >> ((-end) % 64));
        return;
    }
    bitmap[firstword] |= (~UINT64_C(0)) << (start % 64);
    for (uint32_t i = firstword + 1; i < endword; i++) bitmap[i] = ~UINT64_C(0);
    bitmap[endword] |= (~UINT64_C(0)) >> ((-end) % 64);
}

__attribute__((always_inline))
static inline void bitset_set_range(uint64_t *bitmap, uint32_t start, uint32_t end) {
    if (start == end) return;
    uint32_t firstword = start / 64;
    uint32_t endword = (end - 1) / 64;
    if (firstword == endword) {
        bitmap[firstword] |= ((~UINT64_C(0)) << (start % 64)) &
                             ((~UINT64_C(0)) >> ((-end) % 64));
        return;
    }
    bitmap[firstword] |= (~UINT64_C(0)) << (start % 64);
    for (uint32_t i = firstword + 1; i < endword; i++) bitmap[i] = ~UINT64_C(0);
    bitmap[endword] |= (~UINT64_C(0)) >> ((-end) % 64);
}

__attribute__((always_inline))
static inline void bitset_set_range_opti(uint64_t *bitmap, uint32_t start, uint32_t lenminusone) {
    uint32_t firstword = start / 64;
    uint32_t endword = (start + lenminusone) / 64;
    if (firstword == endword) {

        bitmap[firstword] |=
            ((~UINT64_C(0)) >> ((63-lenminusone)  ))<< (start %64);
        return;
    }
    bitmap[firstword] |= (~UINT64_C(0)) << (start % 64);
    for (uint32_t i = firstword + 1; i < endword; i++) bitmap[i] = ~UINT64_C(0);
    bitmap[endword] |= (~UINT64_C(0)) >> ((-start - lenminusone - 1) % 64);
}


/* struct rle16_s - run length pair
 *
 * @value:  start position of the run
 * @length: length of the run is `length + 1`
 *
 * An RLE pair {v, l} would represent the integers between the interval
 * [v, v+l+1], e.g. {3, 2} = [3, 4, 5].
 */
struct rle16_s {
    uint16_t value;
    uint16_t length;
};

typedef struct rle16_s rle16_t;



__attribute__ ((noinline))
void run_bitset_container_union(uint64_t *src_1,
                                const rle16_t *src_2,
                                size_t n_runs) {
    for (int32_t rlepos = 0; rlepos < n_runs; ++rlepos) {
        rle16_t rle = src_2[rlepos];
        bitset_set_range(src_1, rle.value,
                         rle.value + rle.length + UINT32_C(1));
    }
}

__attribute__ ((noinline))
void run_bitset_container_union_opti(uint64_t *src_1,
                                      const rle16_t *src_2,
                                      size_t n_runs) {
    for (int32_t rlepos = 0; rlepos < n_runs; ++rlepos) {
        rle16_t rle = src_2[rlepos];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
    }
}

__attribute__ ((noinline))
void run_bitset_container_union_optiu(uint64_t *src_1,
                                       const rle16_t *src_2,
                                       size_t n_runs) {
    rle16_t rle;
    int32_t rlepos = 0;
    for (; rlepos + 1 < n_runs; rlepos += 2) {
        rle = src_2[rlepos];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
        rle = src_2[rlepos + 1];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
    }
    for (; rlepos < n_runs; ++rlepos) {
        rle16_t rle = src_2[rlepos];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
    }
}

__attribute__ ((noinline))
void run_bitset_container_union_optiuu(uint64_t *src_1,
                                        const rle16_t *src_2,
                                        size_t n_runs) {
    rle16_t rle;
    int32_t rlepos = 0;
    for (; rlepos + 3 < n_runs; rlepos += 4) {
        rle = src_2[rlepos];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
        rle = src_2[rlepos + 1];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
        rle = src_2[rlepos + 2];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
        rle = src_2[rlepos + 3];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
    }
    for (; rlepos < n_runs; ++rlepos) {
        rle16_t rle = src_2[rlepos];
        bitset_set_range_opti(src_1, rle.value,
                               rle.length);
    }
}


__attribute__ ((noinline))
void run_bitset_container_union_uu(uint64_t *src_1,
                                        const rle16_t *src_2,
                                        size_t n_runs) {
    rle16_t rle;
    int32_t rlepos = 0;
    for (; rlepos + 3 < n_runs; rlepos += 4) {
        rle = src_2[rlepos];
        bitset_set_range(src_1, rle.value,
                               rle.value + rle.length + UINT32_C(1));
        rle = src_2[rlepos + 1];
        bitset_set_range(src_1, rle.value,
                               rle.value + rle.length + UINT32_C(1));
        rle = src_2[rlepos + 2];
        bitset_set_range(src_1, rle.value,
                               rle.value + rle.length + UINT32_C(1));
        rle = src_2[rlepos + 3];
        bitset_set_range(src_1, rle.value,
                               rle.value + rle.length + UINT32_C(1));
    }
    for (; rlepos < n_runs; ++rlepos) {
        rle16_t rle = src_2[rlepos];
        bitset_set_range(src_1, rle.value,
                               rle.value + rle.length + UINT32_C(1));
    }
}



__attribute__ ((noinline))
void run_bitset_container_union_noinline(uint64_t *src_1,
        const rle16_t *src_2,
        size_t n_runs) {
    for (int32_t rlepos = 0; rlepos < n_runs; ++rlepos) {
        rle16_t rle = src_2[rlepos];
        bitset_set_range_noinline(src_1, rle.value,
                                  rle.value + rle.length + UINT32_C(1));
    }
}




#define RDTSC_START(cycles)                                                   \
    do {                                                                      \
        register unsigned cyc_high, cyc_low;                                  \
        __asm volatile(                                                       \
            "cpuid\n\t"                                                       \
            "rdtsc\n\t"                                                       \
            "mov %%edx, %0\n\t"                                               \
            "mov %%eax, %1\n\t"                                               \
            : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx", "%rdx"); \
        (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                      \
    } while (0)

#define RDTSC_FINAL(cycles)                                                   \
    do {                                                                      \
        register unsigned cyc_high, cyc_low;                                  \
        __asm volatile(                                                       \
            "rdtscp\n\t"                                                      \
            "mov %%edx, %0\n\t"                                               \
            "mov %%eax, %1\n\t"                                               \
            "cpuid\n\t"                                                       \
            : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx", "%rdx"); \
        (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                      \
    } while (0)

/*
 * Prints the best number of operations per cycle where
 * test is the function call, answer is the expected answer generated by
 * test, repeat is the number of times we should repeat and size is the
 * number of operations represented by test.
 */
#define BEST_TIME(test,  repeat, size)                         \
    do {                                                              \
        printf("%s: ", #test);                                        \
        fflush(NULL);                                                 \
        uint64_t cycles_start, cycles_final, cycles_diff;             \
        uint64_t min_diff = (uint64_t)-1;                             \
        for (int i = 0; i < repeat; i++) {                            \
            __asm volatile("" ::: /* pretend to clobber */ "memory"); \
            RDTSC_START(cycles_start);                                \
            test;                     \
            RDTSC_FINAL(cycles_final);                                \
            cycles_diff = (cycles_final - cycles_start);              \
            if (cycles_diff < min_diff) min_diff = cycles_diff;       \
        }                                                             \
        uint64_t S = (uint64_t)size;                                  \
        float cycle_per_op = (min_diff) / (float)S;                   \
        printf(" %.2f cycles per operation", cycle_per_op);           \
        printf("\n");                                                 \
        fflush(NULL);                                                 \
    } while (0)


void demo() {
    size_t bsize = (1<<16)/8;
    uint64_t *bitmap = (uint64_t *) malloc(bsize);
    memset(bitmap,0,bsize);
    size_t n_runs = 1024;
    rle16_t * runs = (rle16_t *) malloc ( n_runs * sizeof(rle16_t));
    int base = 0;
    for(size_t k = 0; k < n_runs; ++k) {
        int len = (rand() % 71) + 1;
        runs[k].value = base;
        runs[k].length = len - 1;
        base += len + (rand() % 128);
    }
    uint64_t *bitmaptmp1 = (uint64_t *) malloc(bsize);
    uint64_t *bitmaptmp2 = (uint64_t *) malloc(bsize);
    memset(bitmaptmp1,0,bsize);
    memset(bitmaptmp2,0,bsize);
    run_bitset_container_union(bitmaptmp1,runs,n_runs);
    run_bitset_container_union_noinline(bitmaptmp2,runs,n_runs);
    assert(memcmp(bitmaptmp1, bitmaptmp2, bsize) == 0);
    memset(bitmaptmp2,0,bsize);
    run_bitset_container_union_opti(bitmaptmp2,runs,n_runs);
    assert(memcmp(bitmaptmp1, bitmaptmp2, bsize) == 0);
    memset(bitmaptmp2,0,bsize);
    run_bitset_container_union_optiu(bitmaptmp2,runs,n_runs);
    assert(memcmp(bitmaptmp1, bitmaptmp2, bsize) == 0);
    memset(bitmaptmp2,0,bsize);
    run_bitset_container_union_optiuu(bitmaptmp2,runs,n_runs);
    assert(memcmp(bitmaptmp1, bitmaptmp2, bsize) == 0);
    free(bitmaptmp1);
    free(bitmaptmp2);
    int repeat = 5000;
    BEST_TIME(run_bitset_container_union(bitmap,runs,n_runs),repeat,n_runs);
    BEST_TIME(run_bitset_container_union_uu(bitmap,runs,n_runs),repeat,n_runs);
    BEST_TIME(run_bitset_container_union_noinline(bitmap,runs,n_runs),repeat,n_runs);
    BEST_TIME(run_bitset_container_union_opti(bitmap,runs,n_runs),repeat,n_runs);
    BEST_TIME(run_bitset_container_union_optiu(bitmap,runs,n_runs),repeat,n_runs);
    BEST_TIME(run_bitset_container_union_optiuu(bitmap,runs,n_runs),repeat,n_runs);





    free(bitmap);
    free(runs);
}

int main() {
    demo();
}
