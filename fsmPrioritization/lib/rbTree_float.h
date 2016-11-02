/*
 * rbTree.h
 *
 *  Created on: 30 de out de 2016
 *      Author: damasceno
 */

#ifndef LIB_RBTREE_H_
#define LIB_RBTREE_H_

#define RB_COLOR_RED 1
#define RB_COLOR_BLACK 0

typedef struct _rbNodeFloat{
	float  key;
	void* value;
	struct _rbNodeFloat *left;
	struct _rbNodeFloat *right;
	int n;
	int color;
} RedBlackNodeFloat;

struct _rbNodeFloat* createRbfNode(float k, void* v, int n, int col);
//struct _rbNodeFloat** createRbTree(int k);
void 			destroy_rbfTree(struct _rbNodeFloat *leaf);
struct _rbNodeFloat *insert_rbf(float  key, void* val, struct _rbNodeFloat **leaf);
struct _rbNodeFloat *get_rbf(float  key, struct _rbNodeFloat **leaf);
struct _rbNodeFloat *delete_rbf(float  key, struct _rbNodeFloat **leaf);
void 			flipColors_rbf(struct _rbNodeFloat *h);
int 			size_rbf(struct _rbNodeFloat *h);
int 			isRed_rbf(struct _rbNodeFloat *h);
struct _rbNodeFloat *min_rbf(struct _rbNodeFloat **leaf);
struct _rbNodeFloat *max_rbf(struct _rbNodeFloat **leaf);
struct _rbNodeFloat *deleteMin_rbf(struct _rbNodeFloat **leaf);
struct _rbNodeFloat *rotateRight_rbf(struct _rbNodeFloat **leaf);
struct _rbNodeFloat *rotateLeft_rbf(struct _rbNodeFloat **leaf);
void 			inorder_rbf_toString(struct _rbNodeFloat *root);
void removeAll_rbf(struct _rbNodeFloat **t1,struct _rbNodeFloat *toRemove);

#endif /* LIB_RBTREE_H_ */
