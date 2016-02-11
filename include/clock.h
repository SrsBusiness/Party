#include <stdint.h>

static inline uint64_t RDTSC() {
    uint32_t lo, hi;
    __asm__ __volatile__("rdtsc;":"=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t tsc_elapsed(uint64_t start, uint64_t end) {
    if (start <= end) {
        return end - start;
    } else {
        return UINT64_MAX - start + end;
    }
}
