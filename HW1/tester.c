#include <stdio.h>
#include <stdint.h>
int main() {
    int32_t tester1 = UINT32_MAX;
    printf("%d\n", tester1);
    uint32_t tester2 = 3;
    printf("%u\n", tester2);
    uint32_t test3 = tester1 * tester2;
    printf("%u\n", test3);
    return 0;
}