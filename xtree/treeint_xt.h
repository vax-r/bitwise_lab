#pragma once

extern void *treeint_xt_init();
extern int treeint_xt_destroy(void *ctx);
extern int treeint_xt_insert(void *ctx, int a);
extern void *treeint_xt_find(void *ctx, int a);
extern int treeint_xt_remove(void *ctx, int a);