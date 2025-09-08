#include <stdio.h>
#include <stdint.h>
int main() {
    uint32_t tester1 = 8989895;
    printf("%llu\n", tester1);
    //.332492 * 2389238
    uint32_t tester2 = 524765786;
    printf("%llu\n", tester2);
    uint64_t test3 = (uint64_t)tester1 * tester2;
    uint32_t moved = (uint32_t)(test3 >> 32);
    uint32_t fractional = (test3 >> 16); //only the lowset 32 bits are kept from 64 bits. when you convert from
    //64 bits to 32 bits. 
    printf("%llu\n", test3);
    printf("whole part supposedly: %llu\n", moved);
    printf("fractional part: %llu\n", fractional);
    return 0;
}