/*
 * rbTree.h
 *
 *  Created on: 30 de out de 2016
 *      Author: damasceno
 */

#ifndef LIB_RBTREE_H__
#define LIB_RBTREE_H__

#define RB_COLOR_RED 1
#define RB_COLOR_BLACK 0

typedef struct _rbNode{
	void* key;
	void* value;
	struct _rbNode *left;
	struct _rbNode *right;
	int n;
	int color;
} RedBlackNode;

struct _rbNode* createRbNode(void * k, void* v, int n, int col);
//struct _rbNode** createRbTree(int k);
void 			destroy_rbTree(struct _rbNode *leaf);
struct _rbNode *insert_rb(void* key, struct _rbNode **leaf);
struct _rbNode *get_rb(void* key, struct _rbNode **leaf);
void 			flipColors(struct _rbNode *h);
int 			size_rb(struct _rbNode *h);
int 			isRed_rb(struct _rbNode *h);
struct _rbNode *min_rb(struct _rbNode **leaf);
struct _rbNode *max_rb(struct _rbNode **leaf);
struct _rbNode *rotateRight(struct _rbNode **leaf);
struct _rbNode *rotateLeft(struct _rbNode **leaf);
void 			inorder_rb_toString(struct _rbNode *root);

#endif /* LIB_RBTREE_H__ */
