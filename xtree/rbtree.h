#pragma once

#include "common.h"

struct rb_node {
    unsigned long __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
    struct rb_node *rb_node;
};

/*
 * Please note - only struct rb_augment_callbacks and the prototypes for
 * rb_insert_augmented() and rb_erase_augmented() are intended to be public.
 * The rest are implementation details you are not expected to depend on.
 *
 * See Documentation/core-api/rbtree.rst for documentation and samples.
 */

struct rb_augment_callbacks {
    void (*propagate)(struct rb_node *node, struct rb_node *stop);
    void (*copy)(struct rb_node *old, struct rb_node *new);
    void (*rotate)(struct rb_node *old, struct rb_node *new);
};

#define RB_RED 0
#define RB_BLACK 1

#define __rb_parent(pc) ((struct rb_node *) (pc & ~3))

#define __rb_color(pc) ((pc) &1)
#define __rb_is_black(pc) __rb_color(pc)
#define __rb_is_red(pc) (!__rb_color(pc))
#define rb_color(rb) __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb) __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb) __rb_is_black((rb)->__rb_parent_color)

#define RB_ROOT      \
    (struct rb_root) \
    {                \
        NULL,        \
    }

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define rb_parent(r) ((struct rb_node *) ((r)->__rb_parent_color & ~3))

#define RB_EMPTY_ROOT(root) (((root)->rb_node) == NULL)

#define RB_EMPTY_NODE(node) \
    ((node)->__rb_parent_color == (unsigned long) (node))

#define RB_CLEAR_NODE(node) ((node)->__rb_parent_color = (unsigned long) (node))

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
    rb->__rb_parent_color = rb_color(rb) + (unsigned long) p;
}

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

static inline void rb_link_node(struct rb_node *node,
                                struct rb_node *parent,
                                struct rb_node **rb_link)
{
    node->__rb_parent_color = (unsigned long) parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

/**
 * rb_find_add() - find equivalent @node in @tree, or add @node
 * @node: node to look-for / insert
 * @tree: tree to search / modify
 * @cmp: operator defining the node order
 *
 * Returns the rb_node matching @node, or NULL when no match is found and @node
 * is inserted.
 */
static __always_inline struct rb_node *rb_find_add(
    struct rb_node *node,
    struct rb_root *tree,
    int (*cmp)(struct rb_node *, const struct rb_node *))
{
    struct rb_node **link = &tree->rb_node;
    struct rb_node *parent = NULL;
    int c;

    while (*link) {
        parent = *link;
        c = cmp(node, parent);

        if (c < 0)
            link = &parent->rb_left;
        else if (c > 0)
            link = &parent->rb_right;
        else
            return parent;
    }

    rb_link_node(node, parent, link);
    rb_insert_color(node, tree);
    return NULL;
}

/**
 * rb_find() - find @key in tree @tree
 * @key: key to match
 * @tree: tree to search
 * @cmp: operator defining the node order
 *
 * Returns the rb_node matching @key or NULL.
 */
static __always_inline struct rb_node *rb_find(
    const void *key,
    const struct rb_root *tree,
    int (*cmp)(const void *key, const struct rb_node *))
{
    struct rb_node *node = tree->rb_node;

    while (node) {
        int c = cmp(key, node);

        if (c < 0)
            node = node->rb_left;
        else if (c > 0)
            node = node->rb_right;
        else
            return node;
    }

    return NULL;
}

/**
 * rb_remove() - remove @key in tree @tree
 * @key: key to remove
 * @tree: tree to modify
 * @less: operator defining the (partial) node order
 */
static __always_inline struct rb_node *rb_remove(
    const void *key,
    struct rb_root *tree,
    int (*cmp)(const void *key, const struct rb_node *))
{
    struct rb_node *node = tree->rb_node;

    while (node) {
        int c = cmp(key, node);

        if (c < 0)
            node = node->rb_left;
        else if (c > 0)
            node = node->rb_right;
        else {
            rb_erase(node, tree);
            return node;
        }
    }

    return NULL;
}
