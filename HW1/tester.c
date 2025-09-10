#include <stdio.h>
#include <stdint.h>
int main() {
    uint64_t lwhole = 2;
    uint64_t lfrac = 0x80000000;

    uint64_t rwhole = 1;
    uint64_t rfrac = 0x80000000; 
    uint32_t boxes[4];
    
    uint64_t AB = lfrac * rfrac;
    uint64_t CD = (lwhole * rfrac);
    uint64_t EF = (lfrac * rwhole);
    uint64_t GH = (lwhole * rwhole);
    uint64_t box1Carry = CD + EF + (AB >> 32);
    printf("CD: %#08x, EF: %#0x, AB >> 32: %#08x\n", CD, EF, (AB >> 32));
    printf("box1Carry: %#08x\n", box1Carry);
    uint32_t carry = box1Carry >> 32;
    printf("carry: %#08x\n", carry);

    boxes[0] = AB << 32;
    boxes[1] = box1Carry;
    boxes[2] = ((CD & 0xFFFF0000) >> 32) + ((EF & 0xFFFF0000) >> 32) + GH + carry; 
    printf("CD part: %#08x, EF: %#08x, GH: %#08x\n", ((CD & 0xFFFF0000) >> 32), ((EF & 0xFFFF0000) >> 32), GH);
    boxes[3] = GH >> 32;

    uint64_t secondSum = (EF + GH); //add carryon

    //uint64_t finalProduct = boxes[1
    uint32_t whole = boxes[2];
    uint32_t frac = boxes[1];

    printf("whole: %u\n", whole);
    printf("frac: %#08x\n", frac);
    //64bit WX * 32 Z = 96 bits 

    //XY * Z = AB (YZ) + CD(XZ) << 32
    //XY *  = (X) + (Y)



    // uint64_t lowerints = lfrac * rfrac;
    // uint64_t halfmiddleints = (lwhole * rfrac);
    // uint64_t ohalfmiddleints = (rwhole * lfrac);
    // uint64_t upperints = lwhole * rwhole;

    // //get the upperhalf of the lowerints, conversion to u32 ints for the middleintegers will get the bottom 32 ints of those parts.
    // uint32_t carryover = (uint32_t)(((lowerints >> 32) + (uint32_t)halfmiddleints + (uint32_t)ohalfmiddleints) >> 32);

    // printf("upperints: %u\n", upperints);
    // printf("halfmiddleints: %u\n", halfmiddleints);
    // printf("ohalfmiddleints: %u\n", ohalfmiddleints);
    // printf("carryover: %u\n", carryover);
    // printf("lowerints: %u\n", lowerints);
    // //getting the upper 32 bits of the middle ints
    // uint32_t whole = (upperints + (halfmiddleints >> 32) + (ohalfmiddleints >> 32) + carryover);
    // uint32_t frac = lowerints + (halfmiddleints << 32) + (ohalfmiddleints << 32);

    //printf("upperints: %llu\n", upperint);
     //printf("middleprods: %llu\n", middleprods);
     //printf("lowerint: %llu\n", lowerint);
    return 0;
}