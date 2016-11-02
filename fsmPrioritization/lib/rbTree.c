/*
 * rbTree.c
 *
 *  Created on: 30 de out de 2016
 *      Author: damasceno
 */

#include <stdio.h>
#include <stdlib.h>
#include "rbTree.h"


struct _rbNode* createRbNode(void * k, void* v, int n, int col){
	struct _rbNode* out = malloc(sizeof(struct _rbNode));
	out->key = k;
	out->value = v;
	out->n = n;
	out->color = col;
	out->left  = NULL;
	out->right = NULL;
	return out;
}

struct _rbNode** createRbTree(){
	struct _rbNode** out = malloc(sizeof(struct _rbNode*));
	*out = NULL;
	return out;
}
void destroy_rbTree(struct _rbNode *leaf) {
	if( leaf != NULL )	{
		destroy_rbTree(leaf->left);
		destroy_rbTree(leaf->right);
		free( leaf );
	}
}

struct _rbNode *insert_rb(void* key, struct _rbNode **leaf){
	struct _rbNode * tmp= NULL;
	if( *leaf == 0 )	{
		*leaf = createRbNode(key,NULL,1,RB_COLOR_RED);
		//printf("node id @ %p: %d \n",(*leaf),(*leaf)->key);
	}else if(key < (*leaf)->key)	{
		(*leaf)->left = insert_rb( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		(*leaf)->right = insert_rb( key, &(*leaf)->right );
	}
	if(	(isRed_rb((*leaf)->right)) && !(isRed_rb((*leaf)->left)) ){
		(*leaf) = rotateLeft(&(*leaf));
	}
	if(	(isRed_rb((*leaf)->left)) && (isRed_rb((*leaf)->left->left)) ){
		(*leaf) = rotateRight(&(*leaf));
	}
	if(	(isRed_rb((*leaf)->left)) && (isRed_rb((*leaf)->right)) ){
		flipColors((*leaf));
	}
	(*leaf)->n = size_rb((*leaf)->left) + size_rb((*leaf)->right) +1;
	//inorder_rb_toString(*leaf);printf("\n");
	return (*leaf);
}

void inorder_rb_toString(struct _rbNode *root){
	if(root ==NULL) return;

	printf("[%p",root->key);
	printf(" ");
	if(root->left != NULL){
		inorder_rb_toString(root->left);
	}else{
		printf("[ ]");
	}
	printf(" ");
	if(root->right != NULL){
		inorder_rb_toString(root->right);
	}else{
		printf("[ ]");
	}


	printf(" ]");
	fflush(stdout);
}



struct _rbNode *get_rb(void* key, struct _rbNode **leaf){
	if( *leaf == 0 ){
		return NULL;
	}else if(key < (*leaf)->key)	{
		return get_rb( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		return get_rb( key, &(*leaf)->right );
	}else{
		return (*leaf);
	}
}

struct _rbNode *rotateLeft(struct _rbNode **leaf){
	struct _rbNode *x = (*leaf)->right;
	//
	(*leaf)->right = ((x)->left);
	(x)->left = (*leaf);
	(x)->color = ((*leaf)->color);

	(*leaf)->color = RB_COLOR_RED;

	(x)->n = ((*leaf)->n);
	(*leaf)->n = size_rb((*leaf)->left) + size_rb((*leaf)->right) + 1;
	return (x);
}

struct _rbNode *rotateRight(struct _rbNode **leaf){
	struct _rbNode *x = (*leaf)->left;
	//
	(*leaf)->left = ((x)->right);
	(x)->right = (*leaf);
	(x)->color = ((*leaf)->color);

	(*leaf)->color = RB_COLOR_RED;

	(x)->n = ((*leaf)->n);
	(*leaf)->n = size_rb((*leaf)->left) + size_rb((*leaf)->right) + 1;
	return (x);
}

void flipColors(struct _rbNode *h){
	h->color = RB_COLOR_RED;
	h->left->color = RB_COLOR_BLACK;
	h->right->color = RB_COLOR_BLACK;
}

int isRed_rb(struct _rbNode *h){
	if(h == NULL) return RB_COLOR_BLACK;
	return h->color == RB_COLOR_RED;
}

int size_rb(struct _rbNode *h){
	if(h == NULL) return 0;
	return h->n;
}

struct _rbNode *max_rb(struct _rbNode **leaf){
	if((*leaf)->right == NULL) return (*leaf);
	return max_rb(&(*leaf)->right );
}

struct _rbNode *min_rb(struct _rbNode **leaf){
	if((*leaf)->left == NULL) return (*leaf);
	return min_rb(&(*leaf)->left);
}
