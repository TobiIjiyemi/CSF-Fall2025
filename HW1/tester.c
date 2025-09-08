#include <stdio.h>
#include <stdint.h>
int main() {
    uint32_t lwhole= 100;
    uint32_t lfrac = 5;

    uint32_t rwhole = 1;
    uint32_t rfrac = 5;
    
    uint64_t upperint = ((uint64_t)lwhole * rwhole) << 32;
    uint64_t middleprods = lfrac * rwhole + lwhole * rfrac;
    // uint32_t mid = (middleprods >> 32);
    // upperint += mid;
    uint64_t lowerint = ((uint64_t)lfrac * rfrac) >> 32;

    printf("upperints: %llu\n", upperint);
    printf("middleprods: %llu\n", middleprods);
    printf("lowerint: %llu\n", lowerint);
    uint64_t product = (upperint + middleprods + lowerint) << 32;
    uint32_t whole = product >> 32;
    uint32_t frac = (uint32_t)product;
    printf("product: %llu\n", product);
    printf("whole: %u\n", whole);
    printf("frac: %u\n", frac);
    return 0;
}