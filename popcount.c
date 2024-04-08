#include <stdlib.h>
// #include <random.h>
#include <stdio.h>
#include <time.h>


unsigned popcount_branchless(unsigned v)
{
    unsigned n;
    n = (v >> 1) & 0x77777777;
    v -= n;
    n = (n >> 1) & 0x77777777;
    v -= n;
    n = (n >> 1) & 0x77777777;
    v -= n;

    v = (v + (v >> 4)) & 0x0F0F0F0F;
    v *= 0x01010101;

    return v >> 24;
}

unsigned popcount_v2(unsigned v)
{
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    v = (v + (v >> 4)) & 0x0f0f0f0f;
    v = (v + (v >> 8)) & 0x00ff00ff;
    v = (v + (v >> 16)) & 0x0000ffff;

    return v;
}

int totalHammingDistance(int *nums, int numsSize)
{
    int total = 0;
    for (int i = 0; i < numsSize; i++)
        for (int j = i + 1; j < numsSize; j++)
            total += popcount_v2(nums[i] ^ nums[j]);
    return total;
}

#define SIZE 10000

int main(void)
{
    srand(time(NULL));

    int nums[SIZE];
    for (int i = 0; i < SIZE; i++)
        nums[i] = rand();

    totalHammingDistance(nums, SIZE);
    // int test[2] = {1337, 7331};
    // printf("%d\n", totalHammingDistance(test, 2));
    // totalHammingDistance(test, 2);

    return 0;
}
