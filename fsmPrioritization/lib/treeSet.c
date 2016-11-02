/*
 * treeSet.c
 *
 *	Code based on http://www.cprogramming.com/tutorial/c/lesson18.html
 *
 *  Created on: 24 de out de 2016
 *      Author: damasceno
 */

#include "treeSet.h"

#include <stdio.h>
#include <stdlib.h>

void destroy_tree(struct node *leaf) {
	if( leaf != 0 )	{
		destroy_tree(leaf->left);
		destroy_tree(leaf->right);
		free( leaf );
	}
}

struct node *insert(void* key, struct node **leaf){
	if( *leaf == 0 )	{
		*leaf = (struct node*) malloc( sizeof( struct node ) );
		(*leaf)->key = key;
		/* initialize the children to null */
		(*leaf)->left = 0;
		(*leaf)->right = 0;
		return (*leaf);
	}else if(key <= (*leaf)->key)	{
		return insert( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		return insert( key, &(*leaf)->right );
	}
}

struct node_float *insert_float(float key, struct node_float **leaf){
	if( *leaf == 0 )	{
		*leaf = (struct node_float*) malloc( sizeof( struct node_float ) );
		(*leaf)->key = key;
		/* initialize the children to null */
		(*leaf)->left = 0;
		(*leaf)->right = 0;
		return (*leaf);
	}else if(key <= (*leaf)->key)	{
		return insert_float( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		return insert_float( key, &(*leaf)->right );
	}
}

struct node *search(void* key, struct node **leaf){
	if( leaf != NULL && (*leaf)!= NULL )	{
		if(key==(*leaf)->key)		{
			return (*leaf);
		}		else if(key<(*leaf)->key)		{
			return search(key, &(*leaf)->left);
		}
		else		{
			return search(key, &(*leaf)->right);
		}
	}
	return NULL;
}

struct node_float *search_float(float key, struct node_float *leaf){
	if( leaf != 0 )	{
		if(key==leaf->key)		{
			return leaf;
		}		else if(key<leaf->key)		{
			return search_float(key, leaf->left);
		}
		else		{
			return search_float(key, leaf->right);
		}
	}
	return NULL;
}

struct node *delete(void* key, struct node **leaf){
	if( *leaf == 0 ){
		return NULL;
	}else if(key < (*leaf)->key)	{
		//inorder_toString(*leaf);printf("\n");
		(*leaf)->left = delete( key, &(*leaf)->left );
	}	else if(key > (*leaf)->key)	{
		//inorder_toString(*leaf);printf("\n");
		(*leaf)->right = delete( key, &(*leaf)->right );
	}else{
		//inorder_toString(*leaf);printf("\n");
		if((*leaf)->right == NULL) return (*leaf)->left;
		if((*leaf)->left == NULL) return (*leaf)->right;
		struct node *t = *leaf;
		(*leaf) = min(&(t)->right);
		(*leaf)->right = deleteMin(&(t)->right);
		(*leaf)->left  = (t)->left;
	}
	//inorder_toString(*leaf);printf("\n");
	return (*leaf);

}

struct node *min(struct node **leaf){
	if((*leaf) == NULL) return (*leaf);
	if((*leaf)->left == NULL) return (*leaf);
	return min(&(*leaf)->left);
}
struct node *deleteMin(struct node **leaf){
	if((*leaf) == NULL) return (*leaf);
	if((*leaf)->left == NULL) return (*leaf)->right;
	(*leaf)->left  = deleteMin(&(*leaf)->left);
	return (*leaf);
}

void removeAll(struct node **t1,struct node *toRemove) {
	if( toRemove != NULL )	{
		if(search(toRemove->key,t1) != NULL){
			inorder_toString(*t1);printf("\n");
			delete(toRemove->key,t1);
			inorder_toString(*t1);printf("\n");
		}
		removeAll(t1,toRemove->left);
		removeAll(t1,toRemove->right);
	}
}


void inorder(struct node *leaf){
	if( leaf != 0 ) {
		inorder(leaf->left);
		printf("%d\n",leaf->key);
		inorder(leaf->right);
	}
}

void inorder_rev(struct node *leaf){
	if( leaf != 0 ) {
		inorder_rev(leaf->right);
		printf("%d\n",leaf->key);
		inorder_rev(leaf->left);
	}
}

void inorder_float_rev(struct node_float *leaf){
	if( leaf != 0 ) {
		inorder_float_rev(leaf->right);
		printf("node id @ %p: %f \n",leaf,leaf->key);
		inorder_float_rev(leaf->left);
	}
}

void inorder_toString(struct node *root){
	if(root ==NULL) return;

	printf("[%p",root->key);
	printf(" ");
	if(root->left != NULL){
		inorder_toString(root->left);
	}else{
		printf("[ ]");
	}
	printf(" ");
	if(root->right != NULL){
		inorder_toString(root->right);
	}else{
		printf("[ ]");
	}


	printf(" ]");
	fflush(stdout);
}
