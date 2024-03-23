#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "treeint_xt.h"

struct treeint_ops {
    void *(*init)();
    int (*destroy)(void *);
    int (*insert)(void *, int);
    void *(*find)(void *, int);
    int (*remove)(void *, int);
};

static struct treeint_ops *ops;

static struct treeint_ops xt_ops = {
    .init = treeint_xt_init,
    .destroy = treeint_xt_destroy,
    .insert = treeint_xt_insert,
    .find = treeint_xt_find,
    .remove = treeint_xt_remove,
};

#define rand_key(sz) rand() % ((sz) -1)

#define bench(statement)                                                  \
    ({                                                                    \
        struct timespec _tt1, _tt2;                                       \
        clock_gettime(CLOCK_MONOTONIC, &_tt1);                            \
        statement;                                                        \
        clock_gettime(CLOCK_MONOTONIC, &_tt2);                            \
        long long time = (long long) (_tt2.tv_sec * 1e9 + _tt2.tv_nsec) - \
                         (long long) (_tt1.tv_sec * 1e9 + _tt1.tv_nsec);  \
        time;                                                             \
    })

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: treeint <tree size> <seed>\n");
        return -1;
    }

    /* Add an option to specify tree implementation */
    ops = &xt_ops;

    size_t tree_size = 0;
    if (!sscanf(argv[1], "%ld", &tree_size)) {
        printf("Invalid tree size %s\n", argv[1]);
        return -3;
    }

    /* Note: seed 0 is reserved as special value, it will
     * perform linear operatoion. */
    size_t seed = 0;
    if (!sscanf(argv[2], "%ld", &seed)) {
        printf("Invalid seed %s\n", argv[2]);
        return -3;
    }

    srand(seed);

    void *ctx = ops->init();

    long long insert_time = 0;
    for (size_t i = 0; i < tree_size; ++i) {
        int v = seed ? rand_key(tree_size) : i;
        insert_time = bench(ops->insert(ctx, v));
        printf("%lld,", insert_time);
    }
    printf("\n");

    long long find_time = 0;
    for (size_t i = 0; i < tree_size; ++i) {
        int v = seed ? rand_key(tree_size) : i;
        find_time = bench(ops->find(ctx, v));
        printf("%lld, ", find_time);
    }
    printf("\n");

    /* Removing */
    for (size_t i = 0; i < tree_size; ++i) {
        int v = seed ? rand_key(tree_size) : i;
        long long remove_time = bench(ops->remove(ctx, v));
        printf("%lld, ", remove_time);
    }
    printf("\n");

    ops->destroy(ctx);

    return 0;
}