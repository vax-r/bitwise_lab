#pragma once

#define xt_root(r) (r->root)
#define xt_left(n) (n->left)
#define xt_right(n) (n->right)
#define xt_rparent(n) (xt_right(n)->parent)
#define xt_lparent(n) (xt_left(n)->parent)
#define xt_parent(n) (n->parent)

/* XTree uses hints to decide whether to perform a balancing operation or not.
 * Hints are similar to AVL-trees' height property, but they are not
 * required to be absolutely accurate. A hint provides an approximation
 * of the longest chain of nodes under the node to which the hint is attached.
 */
struct xt_node {
    short hint;
    struct xt_node *parent;
    struct xt_node *left, *right;
};

typedef int cmp_t(struct xt_node *node, void *key);
struct xt_tree {
    struct xt_node *root;
    cmp_t *cmp;
    struct xt_node *(*create_node)(void *key);
    void (*destroy_node)(struct xt_node *n);
};

struct xt_tree *xt_create(cmp_t *cmp,
                          struct xt_node *(*create_node)(void *key),
                          void (*destroy_node)(struct xt_node *n));
void xt_destroy(struct xt_tree *tree);
int xt_insert(struct xt_tree *tree, void *key);
int xt_remove(struct xt_tree *tree, void *key);
struct xt_node *xt_find(struct xt_tree *tree, void *key);