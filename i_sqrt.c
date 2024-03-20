#include <assert.h>
#include <stdio.h>
#include <string.h>

int i_sqrt(int x)
{
    if (x <= 1) /* Assume x is always positive */
        return x;

    int z = 0;
    for (int m = 1UL << ((31 - __builtin_clz(x)) & ~1UL); m; m >>= 2) {
        int b = z + m;
        z >>= 1;
        if (x >= b)
            x -= b, z += m;
    }
    return z;
}

int i_sqrt_ffs(int x)
{
    if (x <= 1)
        return x;

    int tmp = x, msb = 0;
    while (tmp) {
        int i = ffs(tmp);
        msb += i;
        tmp >>= i;
    }
    msb--;

    int z = 0;
    for (int m = 1UL << (msb & ~1UL); m; m >>= 2) {
        int b = z + m;
        z >>= 1;
        if (x >= b) {
            x -= b;
            z += m;
        }
    }
    return z;
}

int main(void)
{
    for (int i = 0; i < 10000000; i++)
        assert(i_sqrt(i) == i_sqrt_ffs(i));

    return 0;
}