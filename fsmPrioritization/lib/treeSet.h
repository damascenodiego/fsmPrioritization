/*
 * treeSet.h
 *
 *  Created on: 24 de out de 2016
 *      Author: damasceno
 */

#ifndef LIB_TREESET_H__
#define LIB_TREESET_H__

struct node{
	void* key;
	void* value;
	struct node *left;
	struct node *right;
};

void destroy_tree(struct node *leaf);
struct node *insert(void* key, struct node **leaf);
struct node *search(void* key, struct node **leaf);
void inorder_rev(struct node *leaf);
struct node *min(struct node **leaf);
struct node *deleteMin(struct node **leaf);
void removeAll(struct node **t1,struct node *toRemove);

struct node_float {
	float key;
	void* value;
	struct node *left;
	struct node *right;
};

void destroy_tree_float(struct node_float *leaf);
struct node_float *insert_float(float key, struct node_float **leaf);
struct node_float *search_float(float key, struct node_float *leaf);
void inorder_float(struct node_float *leaf);
void inorder_float_rev(struct node_float *leaf);


#endif /* LIB_TREESET_H__ */
