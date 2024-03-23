/*
 * XTree: A self-balancing binary search tree.
 *
 * AVL-trees promise a close-to-optimal tree layout for lookup, but they
 * consume a significant amount of memory and require relatively slow
 * balancing operations. Red-black trees offer quicker manipulation with
 * a slightly less optimal tree layout, and the proposed XTree offers
 * fast insertion and deletion by balancing trees during lookup.
 *
 * XTrees rely on four fundamental Binary Search Tree (BST) operations:
 * rotate_left, rotate_right, replace_right, and replace_left. The latter
 * two, replace_right and replace_left, are exclusively employed during node
 * removal, following the conventional BST approach. They identify the
 * next/last node in the right/left subtree, respectively, and perform the
 * substitution of the node scheduled for deletion with the identified node.
 *
 * In contrast, rotate_left and rotate_right are integral to a dedicated update
 * phase aimed at rebalancing the tree. This update phase follows both insert
 * and remove phases in the current implementation. Nonetheless, it is
 * theoretically possible to have arbitrary sequences comprising insert,
 * remove, lookup, and update operations. Notably, the frequency of updates
 * directly influences the extent to which the tree layout approaches
 * optimality. However, it is important to consider that each update operation
 * incurs a certain time penalty.
 *
 * The update function exhibits a relatively straightforward process: When a
 * specific node leans to the right or left beyond a defined threshold, a left
 * or right rotation is performed on the node, respectively. Concurrently, the
 * node's hint is consistently updated. Additionally, if the node's hint becomes
 * zero or experiences a change compared to its previous state during the
 * update, modifications are made to the node's parent, as it existed before
 * these update operations.
 */

#include <assert.h>
#include <stdlib.h>

#include "xtree.h"

enum xt_dir { LEFT, RIGHT, NONE };

struct xt_tree *xt_create(cmp_t *cmp,
                          struct xt_node *(*create_node)(),
                          void (*destroy_node)(struct xt_node *n))
{
    struct xt_tree *tree = calloc(sizeof(struct xt_tree), 1);
    tree->root = NULL;
    tree->cmp = cmp;
    tree->create_node = create_node;
    tree->destroy_node = destroy_node;
    return tree;
}

static void __xt_destroy(struct xt_tree *tree, struct xt_node *n)
{
    if (xt_left(n))
        __xt_destroy(tree, xt_left(n));

    if (xt_right(n))
        __xt_destroy(tree, xt_right(n));

    tree->destroy_node(n);
}

void xt_destroy(struct xt_tree *tree)
{
    if (xt_root(tree))
        __xt_destroy(tree, xt_root(tree));

    free(tree);
}

struct xt_node *xt_first(struct xt_node *n)
{
    if (!xt_left(n))
        return n;

    return xt_first(xt_left(n));
}

struct xt_node *xt_last(struct xt_node *n)
{
    if (!xt_right(n))
        return n;

    return xt_last(xt_right(n));
}

static inline void xt_rotate_left(struct xt_node *n)
{
    struct xt_node *l = xt_left(n), *p = xt_parent(n);

    xt_parent(l) = xt_parent(n);
    xt_left(n) = xt_right(l);
    xt_parent(n) = l;
    xt_right(l) = n;

    if (p && xt_left(p) == n)
        xt_left(p) = l;
    else if (p)
        xt_right(p) = l;

    if (xt_left(n))
        xt_lparent(n) = n;
}

static inline void xt_rotate_right(struct xt_node *n)
{
    struct xt_node *r = xt_right(n), *p = xt_parent(n);

    xt_parent(r) = xt_parent(n);
    xt_right(n) = xt_left(r);
    xt_parent(n) = r;
    xt_left(r) = n;

    if (p && xt_left(p) == n)
        xt_left(p) = r;
    else if (p)
        xt_right(p) = r;

    if (xt_right(n))
        xt_rparent(n) = n;
}

static inline int xt_balance(struct xt_node *n)
{
    int l = 0, r = 0;

    if (xt_left(n))
        l = xt_left(n)->hint + 1;

    if (xt_right(n))
        r = xt_right(n)->hint + 1;

    return l - r;
}

static inline int xt_max_hint(struct xt_node *n)
{
    int l = 0, r = 0;

    if (xt_left(n))
        l = xt_left(n)->hint + 1;

    if (xt_right(n))
        r = xt_right(n)->hint + 1;

    return l > r ? l : r;
}

static inline void xt_update(struct xt_node **root, struct xt_node *n)
{
    if (!n)
        return;

    int b = xt_balance(n);
    int prev_hint = n->hint;
    struct xt_node *p = xt_parent(n);

    if (b < -1) {
        /* leaning to the right */
        if (n == *root)
            *root = xt_right(n);
        xt_rotate_right(n);
    }

    else if (b > 1) {
        /* leaning to the left */
        if (n == *root)
            *root = xt_left(n);
        xt_rotate_left(n);
    }

    n->hint = xt_max_hint(n);
    if (n->hint == 0 || n->hint != prev_hint)
        xt_update(root, p);
}

static struct xt_node *__xt_find(struct xt_tree *tree,
                                 void *key,
                                 struct xt_node **p,
                                 enum xt_dir *d)
{
    assert(p && d);

    for (struct xt_node *n = xt_root(tree); n;) {
        int cmp = tree->cmp(n, key);
        if (cmp == 0)
            return n;

        *p = n;

        if (cmp > 0) {
            n = xt_left(n);
            *d = LEFT;
        } else if (cmp < 0) {
            n = xt_right(n);
            *d = RIGHT;
        }
    }

    return NULL;
}

static struct xt_node *__xt_find2(struct xt_tree *tree, void *key)
{
    for (struct xt_node *n = xt_root(tree); n;) {
        int cmp = tree->cmp(n, key);
        if (cmp == 0)
            return n;

        if (cmp > 0) {
            n = xt_left(n);
        } else if (cmp < 0) {
            n = xt_right(n);
        }
    }

    return NULL;
}

/* The process of insertion is straightforward and follows the standard approach
 * used in any BST. After inserting a new node into the tree using conventional
 * BST insertion techniques, an update operation is invoked on the newly
 * inserted node.
 */
static void __xt_insert(struct xt_node **root,
                        struct xt_node *p,
                        struct xt_node *n,
                        enum xt_dir d)
{
    if (d == LEFT)
        xt_left(p) = n;
    else
        xt_right(p) = n;

    xt_parent(n) = p;
    xt_update(root, n);
}

int xt_insert(struct xt_tree *tree, void *key)
{
    struct xt_node *p = NULL;
    enum xt_dir d = NONE;
    struct xt_node *n = __xt_find(tree, key, &p, &d);
    if (n != NULL)
        return -1;

    n = tree->create_node(key);
    if (xt_root(tree)) {
        assert(d != NONE);
        __xt_insert(&xt_root(tree), p, n, d);
    } else
        xt_root(tree) = n;

    return 0;
}

static inline void xt_replace_right(struct xt_node *n, struct xt_node *r)
{
    struct xt_node *p = xt_parent(n), *rp = xt_parent(r);

    if (xt_left(rp) == r) {
        xt_left(rp) = xt_right(r);
        if (xt_right(r))
            xt_rparent(r) = rp;
    }

    if (xt_parent(rp) == n)
        xt_parent(rp) = r;

    xt_parent(r) = p;
    xt_left(r) = xt_left(n);

    if (xt_right(n) != r) {
        xt_right(r) = xt_right(n);
        xt_rparent(n) = r;
    }

    if (p && xt_left(p) == n)
        xt_left(p) = r;
    else if (p)
        xt_right(p) = r;

    if (xt_left(n))
        xt_lparent(n) = r;
}

static inline void xt_replace_left(struct xt_node *n, struct xt_node *l)
{
    struct xt_node *p = xt_parent(n), *lp = xt_parent(l);

    if (xt_right(lp) == l) {
        xt_right(lp) = xt_left(l);
        if (xt_left(l))
            xt_lparent(l) = lp;
    }

    if (xt_parent(lp) == n)
        xt_parent(lp) = l;

    xt_parent(l) = p;
    xt_right(l) = xt_right(n);

    if (xt_left(n) != l) {
        xt_left(l) = xt_left(n);
        xt_lparent(n) = l;
    }

    if (p && xt_left(p) == n)
        xt_left(p) = l;
    else if (p)
        xt_right(p) = l;

    if (xt_right(n))
        xt_rparent(n) = l;
}

/* The process of deletion in this tree structure is relatively more intricate,
 * although it shares similarities with deletion methods employed in other BST.
 * When removing a node, if the node to be deleted has a right child, the
 * deletion process entails replacing the node to be removed with the first node
 * encountered in the right subtree. Following this replacement, an update
 * operation is invoked on the right child of the newly inserted node.
 *
 * Similarly, if the node to be deleted does not have a right child, the
 * replacement process involves utilizing the first node found in the left
 * subtree. Subsequently, an update operation is called on the left child of th
 * replacement node.
 *
 * In scenarios where the node to be deleted has no children (neither left nor
 * right), it can be directly removed from the tree, and an update operation is
 * invoked on the parent node of the deleted node.
 */
static void __xt_remove(struct xt_node **root, struct xt_node *del)
{
    if (xt_right(del)) {
        struct xt_node *least = xt_first(xt_right(del));
        if (del == *root)
            *root = least;

        xt_replace_right(del, least);
        xt_update(root, xt_right(least));
        return;
    }

    if (xt_left(del)) {
        struct xt_node *most = xt_last(xt_left(del));
        if (del == *root)
            *root = most;

        xt_replace_left(del, most);
        xt_update(root, xt_left(most));
        return;
    }

    if (del == *root) {
        *root = 0;
        return;
    }

    /* empty node */
    struct xt_node *parent = xt_parent(del);

    if (xt_left(parent) == del)
        xt_left(parent) = 0;
    else
        xt_right(parent) = 0;

    xt_update(root, parent);
}

struct xt_node *xt_find(struct xt_tree *tree, void *key)
{
    return __xt_find2(tree, key);
}

int xt_remove(struct xt_tree *tree, void *key)
{
    struct xt_node *n = xt_find(tree, key);
    if (!n)
        return -1;

    __xt_remove(&xt_root(tree), n);
    tree->destroy_node(n);

    return 0;
}