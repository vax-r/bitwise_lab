#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "rbtree.h"


struct rbtree_node {
    struct rb_node node;
    int value;
};

struct rbtree_head {
    struct rb_root root;
};

static int rbtree_node_cmp(struct rb_node *a, const struct rb_node *b)
{
    struct rbtree_node *na = rb_entry(a, struct rbtree_node, node);
    struct rbtree_node *nb = rb_entry(b, struct rbtree_node, node);

    return (na->value - nb->value) < 0;
}

static int rbtree_find_cmp(const void *key, const struct rb_node *n)
{
    struct rbtree_node *na = rb_entry(n, struct rbtree_node, node);
    int value = *(int *) key;
    return value - na->value;
}

void *rbtree_init()
{
    struct rbtree_head *tree = calloc(sizeof(struct rbtree_head), 1);
    tree->root = RB_ROOT;
    return tree;
}

int rbtree_insert(void *ctx, int a)
{
    struct rbtree_head *tree = (struct rbtree_head *) ctx;
    struct rbtree_node *n = calloc(sizeof(struct rbtree_node), 1);
    n->value = a;

    if (rb_find_add(&n->node, &tree->root, rbtree_node_cmp))
        return -1;

    return 0;
}

void *rbtree_find(void *ctx, int a)
{
    struct rbtree_head *tree = (struct rbtree_head *) ctx;
    struct rb_node *f = rb_find(&a, &tree->root, rbtree_find_cmp);
    return f ? rb_entry(f, struct rbtree_node, node) : NULL;
}

int rbtree_remove(void *ctx, int a)
{
    struct rbtree_head *tree = (struct rbtree_head *) ctx;
    struct rb_node *r = rb_find(&a, &tree->root, rbtree_find_cmp);
    if (!r)
        return -1;

    struct rbtree_node *rn = rb_entry(r, struct rbtree_node, node);
    rb_erase(r, &tree->root);
    free(rn);
    return 0;
}
