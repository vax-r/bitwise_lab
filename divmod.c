#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>

static uint32_t mod5(uint32_t n)
{
    static char table[62] = {0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0,
                             1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1,
                             2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2,
                             3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1};
    n = (n >> 16) + (n & 0xFFFF);
    n = (n >> 8) + (n & 0x00FF);
    n = (n >> 4) + (n & 0x000F);
    return table[n];
}

static uint32_t mod9(uint32_t n)
{
    static char table[75] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5,
                             6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2,
                             3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8,
                             0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5,
                             6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2};
    int r = (n & 0x7FFF) - (n >> 15);
    r = (r & 0x01FF) - (r >> 9);
    r = (r & 0x003F) + (r >> 6);
    return table[r];
}

int main(void)
{
    for (uint32_t i = 0; i < 0xFFFFFFFF - 1; i++) {
        assert((i % 5) == mod5(i));
        assert((i % 9) == mod9(i));
    }

    return 0;
}