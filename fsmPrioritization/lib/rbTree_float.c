/*
 * rbTree.c
 *
 *  Created on: 30 de out de 2016
 *      Author: damasceno
 */

#include <stdio.h>
#include <stdlib.h>
#include "rbTree_float.h"
#include "fsmLib.h"


struct _rbNodeFloat* createRbfNode(float k, void* v, int n, int col){
	struct _rbNodeFloat* out = malloc(sizeof(struct _rbNodeFloat));
	out->key = k;
	out->value = v;
	out->n = n;
	out->color = col;
	out->left  = NULL;
	out->right = NULL;
	return out;
}

//struct _rbNodeFloat** createRbTree(int k){
//	struct _rbNodeFloat** out = malloc(sizeof(struct _rbNodeFloat*));
//	out[0] = createRbNode(k,NULL,1,RB_COLOR_BLACK);
//	return out;
//}
void destroy_rbfTree(struct _rbNodeFloat *leaf) {
	if( leaf != NULL )	{
		destroy_rbfTree(leaf->left);
		destroy_rbfTree(leaf->right);
		free( leaf );
	}
}

void removeAll_rbf(struct _rbNodeFloat **t1,struct _rbNodeFloat *toRemove) {
	if( toRemove != NULL )	{
		removeAll_rbf(t1,toRemove->left);
		removeAll_rbf(t1,toRemove->right);
		if(get_rbf(toRemove->key,t1) != NULL){
			inorder_rbf_toString(*t1);printf("\n");
			delete_rbf(toRemove->key,t1);
			inorder_rbf_toString(*t1);printf("\n");
		}
	}
}

struct _rbNodeFloat *insert_rbf(float key, void* val, struct _rbNodeFloat **leaf){
	struct _rbNodeFloat * tmp= NULL;
	if( *leaf == 0 )	{
		*leaf = createRbfNode(key,val,1,RB_COLOR_RED);
		//printf("node id @ %p: %d \n",(*leaf),(*leaf)->key);
	}else if(key <= (*leaf)->key)	{
		(*leaf)->left = insert_rbf( key, val,&(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		(*leaf)->right = insert_rbf( key, val, &(*leaf)->right );
	}
	if(	(isRed_rbf((*leaf)->right)) && !(isRed_rbf((*leaf)->left)) ){
		(*leaf) = rotateLeft_rbf(&(*leaf));
	}
	if(	(isRed_rbf((*leaf)->left)) && (isRed_rbf((*leaf)->left->left)) ){
		(*leaf) = rotateRight_rbf(&(*leaf));
	}
	if(	(isRed_rbf((*leaf)->left)) && (isRed_rbf((*leaf)->right)) ){
		flipColors_rbf((*leaf));
	}
	(*leaf)->n = size_rbf((*leaf)->left) + size_rbf((*leaf)->right) +1;
	//inorder_rb_toString(*leaf);printf("\n");
	return (*leaf);
}

void inorder_rbf_toString(struct _rbNodeFloat *root){
	if(root == NULL) return;

	TestPair* tp = (TestPair*)root->value;

	inorder_rbf_toString(root->right);
	printf("Distance: %f \t Test i: %p \t Test j: %p \n",tp->ds,tp->ti,tp->tj);
	inorder_rbf_toString(root->left);

	fflush(stdout);
}



struct _rbNodeFloat *get_rbf(float key, struct _rbNodeFloat **leaf){
	if( *leaf == 0 ){
		return NULL;
	}else if(key < (*leaf)->key)	{
		return get_rbf( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		return get_rbf( key, &(*leaf)->right );
	}else{
		return (*leaf);
	}
}

struct _rbNodeFloat *delete_rbf(float key, struct _rbNodeFloat **leaf){
	if( *leaf == 0 ){
		return NULL;
	}else if(key < (*leaf)->key)	{
		//inorder_rb_toString(*leaf);printf("\n");
		(*leaf)->left = delete_rbf( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		//inorder_rb_toString(*leaf);printf("\n");
		(*leaf)->right = delete_rbf( key, &(*leaf)->right );
	}else{
		//inorder_rb_toString(*leaf);printf("\n");
		if((*leaf)->right == NULL) return (*leaf)->left;
		if((*leaf)->left == NULL) return (*leaf)->right;
		struct _rbNodeFloat *t = *leaf;
		(*leaf) = min_rbf(&(t)->right);
		(*leaf)->right = deleteMin_rbf(&(t)->right);
		(*leaf)->left  = (t)->left;
	}
	(*leaf)->n = size_rbf((*leaf)->left) + size_rbf((*leaf)->right) +1;
	//inorder_rb_toString(*leaf);printf("\n");
	return (*leaf);

}

struct _rbNodeFloat *rotateLeft_rbf(struct _rbNodeFloat **leaf){
	struct _rbNodeFloat *x = (*leaf)->right;
	//
	(*leaf)->right = ((x)->left);
	(x)->left = (*leaf);
	(x)->color = ((*leaf)->color);

	(*leaf)->color = RB_COLOR_RED;

	(x)->n = ((*leaf)->n);
	(*leaf)->n = size_rbf((*leaf)->left) + size_rbf((*leaf)->right) + 1;
	return (x);
}

struct _rbNodeFloat *rotateRight_rbf(struct _rbNodeFloat **leaf){
	struct _rbNodeFloat *x = (*leaf)->left;
	//
	(*leaf)->left = ((x)->right);
	(x)->right = (*leaf);
	(x)->color = ((*leaf)->color);

	(*leaf)->color = RB_COLOR_RED;

	(x)->n = ((*leaf)->n);
	(*leaf)->n = size_rbf((*leaf)->left) + size_rbf((*leaf)->right) + 1;
	return (x);
}

void flipColors_rbf(struct _rbNodeFloat *h){
	h->color = RB_COLOR_RED;
	h->left->color = RB_COLOR_BLACK;
	h->right->color = RB_COLOR_BLACK;
}

int isRed_rbf(struct _rbNodeFloat *h){
	if(h == NULL) return RB_COLOR_BLACK;
	return h->color == RB_COLOR_RED;
}

int size_rbf(struct _rbNodeFloat *h){
	if(h == NULL) return 0;
	return h->n;
}

//
//	public void put(Key k, Value v){
//		root = put(root, k, v);
//		root.setColor(BLACK);
//	}
//
//	@Override
//	public String toString() {
//
//		StringBuffer child = new StringBuffer("["+key.toString());
//
//		child.append(" "+((getLeft()!=null)?getLeft().toString():"[ ]"));
//		child.append(" ");
//
//		child.append(" "+((getRight()!=null)?getRight().toString():"[ ]"));
//
//		child.append(" ]");
//
//		return child.toString();
//	}
struct _rbNodeFloat *max_rbf(struct _rbNodeFloat **leaf){
	if((*leaf)->right == NULL) return (*leaf);
	return max_rbf(&(*leaf)->right );
}

struct _rbNodeFloat *min_rbf(struct _rbNodeFloat **leaf){
	if((*leaf)->left == NULL) return (*leaf);
	return min_rbf(&(*leaf)->left);
}
//	//TODO falta ajustar metodos para deletar Node em RB Trees
struct _rbNodeFloat *deleteMin_rbf(struct _rbNodeFloat **leaf){
	if((*leaf) == NULL) return (*leaf);
	if((*leaf)->left == NULL) return (*leaf)->right;
	(*leaf)->left  = deleteMin_rbf(&(*leaf)->left);
	(*leaf)->n = size_rbf((*leaf)->left) + size_rbf((*leaf)->right) + 1;
	return (*leaf);
}

