/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 32144548 32143180
*	    Student name : 조창연, 이명재
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "lab2_sync_types.h"
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */
int lab2_node_print_inorder(lab2_tree *tree) {
    // You need to implement lab2_node_print_inorder function.
	return inorder(tree->root);
}

int inorder(lab2_node *node) {
	if(node) {
		inorder(node->left);
//		printf("%d\t", node->key);
		inorder(node->right);
		free(node);
		node = NULL;
	}
	return 1;
}

/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
lab2_tree *lab2_tree_create() {
    // You need to implement lab2_tree_create function.
	lab2_tree* tree = (lab2_tree *)malloc(sizeof(lab2_tree));
	tree->root = NULL;
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
lab2_node * lab2_node_create(int key) {
    // You need to implement lab2_node_create function.
	lab2_node* node = (lab2_node *)malloc(sizeof(lab2_node));
	pthread_mutex_init(&node->mutex, NULL);
	node->key = key;
	node->left = NULL;
	node->right = NULL;
	return node;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : satus (success or fail)
 */
int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert function.
	if(!tree->root) {
		tree->root = new_node;
		return 0;
	}
	lab2_node *p = tree->root;
	lab2_node *q = NULL;
	while(p) {
		q = p;
		if(new_node->key == p->key) {
			lab2_node_delete(new_node);
			return -1;
		}
		if(new_node->key < p->key)
			p = p->left;
		else
			p = p->right;
	}
	if(new_node->key < q->key)
		q->left = new_node;
	else
		q->right = new_node;
	return 0;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
      // You need to implement lab2_node_insert_fg function.
	pthread_mutex_lock(&lock);
	if(!tree->root) {
		tree->root = new_node;
		pthread_mutex_unlock(&lock);
		return 0;
	}
	pthread_mutex_unlock(&lock);
	lab2_node *p = tree->root;
	lab2_node *q = NULL;
	while(1) {
		q = p;
		if(new_node->key == p->key) {
			lab2_node_delete(new_node);
			return -1;
		}
		if(new_node->key < p->key)
			p = p->left;
		else
			p = p->right;
		pthread_mutex_lock(&q->mutex);
		if(!p) break;
		pthread_mutex_unlock(&q->mutex);
	}
	if(new_node->key < q->key)
		q->left = new_node;
	else
		q->right = new_node;
	pthread_mutex_unlock(&q->mutex);
	return 0;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert_cg function.
	pthread_mutex_lock(&lock);
	if(!tree->root) {
		tree->root = new_node;
		pthread_mutex_unlock(&lock);
		return 0;
	}
	lab2_node *p = tree->root;
	lab2_node *q = NULL;
	while(p) {
		q = p;
		if(new_node->key == p->key) {
			lab2_node_delete(new_node);
			pthread_mutex_unlock(&lock);
			return -1;
		}
		if(new_node->key < p->key)
			p = p->left;
		else
			p = p->right;
	}
	if(new_node->key < q->key)
		q->left = new_node;
	else
		q->right = new_node;
	pthread_mutex_unlock(&lock);
	return 0;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove function.
	int rm_value = -1;
	lab2_node *del = tree->root;
	lab2_node *p_del = NULL;
	while(del) {
		if(del->key == key) break;
		p_del = del;
		if(del->key > key)
			del = del->left;
		else
			del = del->right;
	}
	if(!del) return -1;
	rm_value = del->key;
	if(del->left && del->right) {
		lab2_node *save = del->left;
		lab2_node *p_save = del;
		while(save->right) {
			p_save = save;
			save = save->right;
		}
		del->key = save->key;
		del = save;
		p_del = p_save;
	}
	lab2_node *p;
	if(!del->left)
		p = del->right;
	else
		p = del->left;
	if(del == tree->root)
		tree->root = p;
	else {
		if(del == p_del ->left)
			p_del->left = p;
		else
			p_del->right = p;
	}
	lab2_node_delete(del);
	return rm_value;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_fg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_fg function.
	int rm_value = -1;
	lab2_node *del = tree->root;
	lab2_node *p_del = del;
	pthread_mutex_lock(&lock);
	while(1) {
		if(!del) break;
		if(del->key == key) {
			pthread_mutex_lock(&lock2);
			break;
		}
		p_del = del;
		if(del->key > key)
			del = del->left;
		else
			del = del->right;
	}
	pthread_mutex_unlock(&lock);
	if(!del) return -1;
	rm_value = del->key;
	lab2_node *kkk = del;
	if(del->left && del->right) {
		lab2_node *save = del->left;
		lab2_node *p_save = del;
		while(save->right) {
			p_save = save;
			save = save->right;
		}
		lab2_node *p;
		if(!del->left)
			p = del->right;
		else
			p = del->left;
		if(del == tree->root)
			tree->root = p;
		else {
			if(del == p_del->left)
				p_del->left = p;
			else
				p_del->right = p;
		}
		lab2_node_delete(del);
		pthread_mutex_unlock(&lock2);
		return rm_value;
	}
}


/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_cg(lab2_tree *tree, int key) {
    // You need to implement lab2_node_remove_cg function.
//	pthread_mutex_lock();
	pthread_mutex_lock(&lock);
	int rm_value = -1;
	lab2_node *del = tree->root;
	lab2_node *p_del = NULL;
	while(del) {
		if(del->key == key) break;
		p_del = del;
		if(del->key > key)
			del = del->left;
		else
			del = del->right;

	}
	if(!del) {
		pthread_mutex_unlock(&lock);
		return -1;
	}
	rm_value = del->key;
	if(del->left && del->right) {
		lab2_node *save = del->left;
		lab2_node *p_save = del;
		while(save->right) {
			p_save = save;
			save = save->right;
		}
		del->key = save->key;
		del = save;
		p_del = p_save;
	}
	lab2_node *p;
	if(!del->left)
		p = del->right;
	else
		p = del->left;
	if(del == tree->root)
		tree->root = p;
	else {
		if(del == p_del->left)
			p_del->left = p;
		else
			p_del->right = p;
	}
	lab2_node_delete(del);
	pthread_mutex_unlock(&lock);
	return rm_value;
}


/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
void lab2_tree_delete(lab2_tree *tree) {
    // You need to implement lab2_tree_delete function.
	free(tree);
	tree = NULL;
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst node which you want to remove. 
 *  @return                 : status(success or fail)
 */
void lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
	free(node);
	node = NULL;
}

