#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "treeint_xt.h"
#include "xtree.h"

#define treeint_xt_entry(ptr) container_of(ptr, struct treeint_st, xt_n)

struct treeint_st {
    int value;
    struct xt_node xt_n;
};

static int treeint_xt_cmp(struct xt_node *node, void *key)
{
    struct treeint_st *n = treeint_xt_entry(node);
    int value = *(int *) key;

    return n->value - value;
}

static struct xt_node *treeint_xt_node_create(void *key)
{
    int value = *(int *) key;
    struct treeint_st *i = calloc(sizeof(struct treeint_st), 1);
    assert(i);

    i->value = value;
    /* return the xt_node reference of the new node */
    return &i->xt_n;
}

static void treeint_xt_node_destroy(struct xt_node *n)
{
    struct treeint_st *i = treeint_xt_entry(n);
    free(i);
}

void *treeint_xt_init()
{
    struct xt_tree *tree;
    tree = xt_create(treeint_xt_cmp, treeint_xt_node_create,
                     treeint_xt_node_destroy);
    assert(tree);
    return tree;
}

int treeint_xt_destroy(void *ctx)
{
    struct xt_tree *tree = (struct xt_tree *) ctx;

    assert(tree);
    xt_destroy(tree);
    return 0;
}

int treeint_xt_insert(void *ctx, int a)
{
    struct xt_tree *tree = (struct xt_tree *) ctx;
    return xt_insert(tree, (void *) &a);
}

void *treeint_xt_find(void *ctx, int a)
{
    struct xt_tree *tree = (struct xt_tree *) ctx;
    struct xt_node *n = xt_find(tree, (void *) &a);
    return n ? treeint_xt_entry(n) : NULL;
}

int treeint_xt_remove(void *ctx, int a)
{
    struct xt_tree *tree = (struct xt_tree *) ctx;
    return xt_remove(tree, (void *) &a);
}