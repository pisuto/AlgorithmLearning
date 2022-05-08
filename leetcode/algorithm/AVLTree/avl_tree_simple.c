#include "stddef.h"
#include "stdio.h"

typedef int type;

struct AVL_NODE_SIMPLE {
	type key;
	int height;
	struct AVL_NODE_SIMPLE* left;
	struct AVL_NODE_SIMPLE* right;
};

typedef struct AVL_NODE_SIMPLE NODE;

#define MAX(x, y) (x > y ? x : y)
#define HEIGHT(n) (n ? n->height : 0)
#define UPDATE_HEIGHT(n) (1 + (MAX(HEIGHT(n->left), HEIGHT(n->right))))
#define BALANCED_FACTOR(n) (n ? (HEIGHT(n->left) - HEIGHT(n->right)) : 0)

NODE* simple_ll_rotate(NODE* node) {
	NODE* temp = node->left;
	node->left = temp->right;
	temp->right = node;

	node->height = UPDATE_HEIGHT(node);
	temp->height = UPDATE_HEIGHT(temp);
	return temp;
}

NODE* simple_rr_rotate(NODE* node) {
	NODE* temp = node->right;
	node->right = temp->left;
	temp->left = node;

	node->height = UPDATE_HEIGHT(node);
	temp->height = UPDATE_HEIGHT(temp);
	return temp;
}

NODE* simple_lr_rotate(NODE* node) {
	node->left = simple_rr_rotate(node->left);
	node = simple_ll_rotate(node);
	return node;
}

NODE* simple_rl_rotate(NODE* node) {
	node->right = simple_ll_rotate(node->right);
	node = simple_rr_rotate(node);
	return node;
}

NODE* simple_rebalance(NODE* root) {

	root->height = UPDATE_HEIGHT(root);
	int delta = BALANCED_FACTOR(root);
	if (delta > 1) {
		if (BALANCED_FACTOR(root->left) >= 0) {
			return simple_ll_rotate(root);
		}
		else {
			return simple_lr_rotate(root);
		}
	}
	else if (delta < -1) {
		if (BALANCED_FACTOR(root->right) <= 0) {
			return simple_rr_rotate(root);
		}
		else {
			return simple_rl_rotate(root);
		}
	}
	return root;
}

NODE* simple_insert(NODE* root, NODE* node) 
{
	if (!root) return node;

	if (root->key < node->key) {
		root->right = simple_insert(root->right, node);
	}
	else if (root->key > node->key) {
		root->left = simple_insert(root->left, node);
	}
	else {
		return node;
	}

	return simple_rebalance(root);
}

NODE* simple_delete(NODE* root, type key)
{
	if (!root) return root;

	if (root->key < key) {
		root->right = simple_delete(root->right, key);
	}
	else if (root->key > key) {
		root->left = simple_delete(root->left, key);
	}
	else {
		if (!root->left && !root->right) {
			return NULL;
		}
		else if (root->left && !root->right) {
			return root->left;
		}
		else if (root->right && !root->left) {
			return root->right;
		}
		else {
			NODE* temp = root->right;
			while (temp->left) {
				temp = temp->left;
			}
			root->key = temp->key;
			root->right = simple_delete(root->right, temp->key);
		}
	}
	
	simple_rebalance(root);
	return root;
}

void simple_traverse(NODE* root) 
{
	if (!root) return;
	int in = 0;
	int out = 0;
	NODE* array[42 * 2];

	array[in++] = root;
	while (in > out) {
		if (array[out]) {
			printf(" %d", array[out]->key);
			array[in++] = array[out]->left;
			array[in++] = array[out]->right;
		}
		out++;
	}
}

#if 0

NODE* make_node(int val) {
	NODE* node = (NODE*)malloc(sizeof(NODE));
	if (node) {
		node->height = 1;
		node->key = val;
		node->left = NULL;
		node->right = NULL;
	}
	return node;
}

int main() 
{
	NODE* root = NULL;
	NODE* node1 = make_node(1);
	NODE* node2 = make_node(2);
	NODE* node3 = make_node(3);
	NODE* node4 = make_node(4);
	NODE* node5 = make_node(5);
	NODE* node6 = make_node(6);
	NODE* node7 = make_node(7);
	NODE* node8 = make_node(8);
	NODE* node9 = make_node(9);
	NODE* node10 = make_node(10);

	root = simple_insert(root, node1);
	root = simple_insert(root, node2);
	root = simple_insert(root, node3);
	root = simple_insert(root, node4);
	root = simple_insert(root, node5);
	root = simple_insert(root, node6);
	root = simple_insert(root, node7);
	root = simple_insert(root, node8);
	root = simple_insert(root, node9);
	root = simple_insert(root, node10);

	simple_traverse(root);
	printf("\n");

	root = simple_delete(root, 6);
	simple_traverse(root);
	printf("\n");

	root = simple_delete(root, 3);
	simple_traverse(root);
	printf("\n");

	root = simple_delete(root, 4);
	simple_traverse(root);
	printf("\n");

	free(node1);
	free(node2);
	free(node3);
	free(node4);
	free(node5);
	free(node6);
	free(node7);
	free(node8);
	free(node9);
	free(node10);
	
	return 0;
}


#endif