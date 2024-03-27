#pragma once

extern void *rbtree_init();
extern int rbtree_insert(void *ctx, int a);
extern void *rbtree_find(void *ctx, int a);
extern int rbtree_remove(void *ctx, int a);