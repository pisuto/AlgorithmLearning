#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"

/*
 * In a rb-tree, it follows these rules:
 *  1. each node has black or red color.
 *  2. root node and nil nodes must be black.
 *  3. red node has no red neighbor, but black node can 
 *     have black neighbor.
 *  4. each path from a node to any child nodes has the 
 *     same number of black nodes.
 * when a node is inserted, this node will be considered 
 * to be red, otherwise this tree will be a normal BST.
 * Meanwhile, the length of the longest path won't be twice
 * the shortest.
 */

typedef int   TYPE;
typedef char  COLOR;

typedef enum {
	BLACK,
	RED
} RB_NODE_COLOR;

typedef struct RB_NODE_SIMPLE {
	TYPE  data;
	COLOR color;

	struct RB_NODE_SIMPLE* left;
	struct RB_NODE_SIMPLE* right;
	struct RB_NODE_SIMPLE* parent;
} NODE;

typedef struct RB_TREE_SIMPLE {
	NODE* root;
	NODE* nil;
} TREE;


#define safe_malloc   malloc

TREE* simple_rb_tree_create() {
	TREE* tree;
	NODE* temp;
	/* Don't consider the situation that malloc allocates memmory failed. */
	tree = (TREE*)safe_malloc(sizeof(TREE));
	/* Create nil node */
	temp = tree->nil = (NODE*)safe_malloc(sizeof(NODE));
	temp->parent = temp->left = temp->right = temp;
	temp->color = BLACK;
	temp->data = 0;
	/* Create root node */
	tree->root = tree->nil;
	return tree;
}

NODE* simple_rb_node_create(COLOR color, TYPE data, NODE* parent, NODE* nil)
{
	NODE* temp = (NODE*)safe_malloc(sizeof(NODE));
	temp->left = temp->right = nil;
	temp->parent = parent;
	temp->color = color;
	temp->data = data;
	return temp;
}

void simple_rb_ll_rotate(TREE* t, NODE* n)
{
	/*
	 *           .                 .    
	 *          /                 / 
	 *         n                 x
	 *        / \    right      / \
	 *       x   k   =====>    z   n 
	 *      / \                   / \
	 *     z   y                 y   k
	 * 
	 * In right rotation, don't forget x's parent.
	 */
	NODE* nil = t->nil;
	NODE* x = n->left;
	NODE* y = x->right;

	if (n == nil) {
		return;
	}

	n->left = y;
	x->right = n;
	x->parent = n->parent;
	n->parent = x;
	y->parent = (y != nil ? n : nil);

	/* If node is root, change root */
	if (t->root == n) {
		t->root = x;
		x->parent = nil;
		return;
	}

	if (x->parent->left == n) {
		x->parent->left = x;
	}
	else {
		x->parent->right = x;
	}
	
}

void simple_rb_rr_rotate(TREE* t, NODE* n)
{
	NODE* nil = t->nil;
	NODE* x = n->right;
	NODE* y = x->left;

	if (n == nil) {
		return;
	}

	n->right = y;
	x->left = n;
	x->parent = n->parent;
	n->parent = x;
	y->parent = (y != nil ? n : nil);

	/* If node is root, change root */
	if (t->root == n) {
		t->root = x;
		x->parent = nil;
		return;
	}

	if (x->parent->left == n) {
		x->parent->left = x;
	}
	else {
		x->parent->right = x;
	}
}

NODE* simple_rb_predecessor(TREE* tree, NODE* node)
{
	NODE* nil = tree->nil;
	NODE* temp = node->left;
	if (temp != nil) {
		while (temp->right != nil) {
			temp = temp->right;
		}
	}
	else {
		temp = node;
		do {
			node = temp;
			temp = node->parent;
		} while (temp != nil && temp->left == node);
	}
	return temp;
}

NODE* simple_rb_successor(TREE* tree, NODE* node)
{
	NODE* nil = tree->nil;
	NODE* temp = node->right;
	if (temp != nil) {
		while (temp->left != nil) {
			temp = temp->left;
		}
	}
	else {
		temp = node;
		do {
			node = temp;
			temp = node->parent;
		} while (temp != nil && temp->right == node);
	}
	return temp;
}

NODE* simple_rb_find(TREE* tree, TYPE data)
{
	NODE* nil = tree->nil;
	NODE* node = tree->root;
	while (node != nil) {
		if (node->data > data) {
			node = node->left;
		}
		else if (node->data < data) {
			node = node->right;
		}
		else {
			return node;
		}
	}
	return nil;
}

/*
 * When inserting a node, several conditions are considered:
 *  1. it's root node, turning to be black.
 *  2. its parent node is black, then remain unchanged. 
 *  3. its uncle and parent nodes are both red, change their color black
       and its grandparent red.
 *  4. its uncle node is black and its parent is red (occurs in recursion
       due to condition 3):
 *     a. these three are all left nodes, perform right rotation and its
 *        parent turns black, its grandparent turns red.
 *     b. Contrary to (a), these nodes are right nodes, perform left rotation
 *        and its parent turns black, its grandparent turns red.
 *     c. left-right-rotation ...
 *     d. right-left-rotation ...
 */
void simple_rb_insert(TREE* tree, TYPE data)
{
	NODE* node;
	NODE* nil;
	NODE* temp;
	node = tree->root;
	nil = tree->nil;

	if (node == nil) {
		tree->root = simple_rb_node_create(BLACK, data, nil, nil);
		return;
	}

	/* Firstly the leaf position of the inserted data needs to be found. */
	while (1) {
		if (node->data > data) {
			if (node->left != nil) {
				node = node->left;
			}
			else {
				temp = simple_rb_node_create(RED, data, node, nil);
				node->left = temp;
				break;
			}
		}
		else if (node->data < data) {
			if (node->right != nil) {
				node = node->right;
			}
			else {
				temp = simple_rb_node_create(RED, data, node, nil);
				node->right = temp;
				break;
			}
		}
		else {
			return;
		}
	}

	while (RED == temp->parent->color) {
		/* Its parent is the left node of its grandparent. */
		if (temp->parent->parent->left == temp->parent) {
			/* Now 'node' is uncle node. */
			node = temp->parent->parent->right;
			if (RED == node->color) {
				node->color = BLACK;
				temp->parent->color = BLACK;
				temp->parent->parent->color = RED;
				temp = temp->parent->parent;
			}
			else {
				if (temp == temp->parent->right) {
					temp = temp->parent;
					/*
				     * It's the right node of parent:
				     *
				     *           A             A
				     *          / \           / \
				     *       B(temp)  ====>  C
				     *          \           /
				     *           C        B(temp)
				     */
					simple_rb_rr_rotate(tree, temp);
				}
				temp->parent->color = BLACK;
				temp->parent->parent->color = RED;
				simple_rb_ll_rotate(tree, temp->parent->parent);
			}
		}
		/* Its parent is the right node of its grandparent. */
		else {
			/* Now 'node' is uncle node. */
			node = temp->parent->parent->left;
			if (RED == node->color) {
				node->color = BLACK;
				temp->parent->color = BLACK;
				temp->parent->parent->color = RED;
				temp = temp->parent->parent;
			}
			else {
				if (temp == temp->parent->left) {
					temp = temp->parent;
					simple_rb_ll_rotate(tree, temp);
				}
				temp->parent->color = BLACK;
				temp->parent->parent->color = RED;
				simple_rb_rr_rotate(tree, temp->parent->parent);
			}
		}
	}
	/* Keep the root node black. */
	tree->root->color = BLACK;
}

/*
 * Node is the one that replace the deleted node, and parent is node's 
 * parent.
 * Go through this function due to black color of the deleted node,
 * otherwise just deleted red node is satisfied. For expamle, as node 
 * is in left subtree:
 * case 1: if brother is red, convert parent red and brother black, then
 * turn left.
 * case 2: if brother is black and two children are black, then convert 
 * brother red and move node upward as parent.
 * case 3: if borther is black left child is red, and right child is black,
 * convert brother red and its left child black. Finally, regard brother as 
 * root, turn right.
 * case 4: if brother is black and right child is red, convert brother's 
 * color same as parent's, convert parent and right child black. Finally,
 * regard parent as root, turn left. The tree is balanced and exit.
 * 
 * Reference to https://blog.csdn.net/v_JULY_v/article/details/6105630
 */
void simple_rb_remove_fixup(TREE* tree, NODE* node, NODE* parent)
{
	NODE* root = tree->root;
	NODE* nil = tree->nil;
	NODE* bro;

	while (node != root && node->color == BLACK)
	{
		if (parent->left == node) {
			bro = parent->right;
			/* Case 1 */
			if (bro->color == RED) {
				bro->color = BLACK;
				parent->color = RED;
				simple_rb_rr_rotate(tree, parent);
				bro = parent->right;
			}
			/* Case 1 is convert to case 2, 3, 4. */
			/* Case 2 */
			if (bro->left->color == BLACK && bro->right->color == BLACK)
			{
				/*
				 * In this case, because of decrease of left subtree's black nodes,
				 * convert brother red. Continue next loop.
				 */
				bro->color = RED;
				node = parent;
				parent = node->parent;
			}
			else {
				/* Case 3*/
				if (bro->right->color == BLACK) {
					bro->left->color = BLACK;
					bro->color = RED;
					simple_rb_ll_rotate(tree, bro);
					bro = parent->right;
				}
				/* Case 4 */
				bro->color = parent->color;
				parent->color = BLACK;
				bro->right->color = BLACK;
				simple_rb_rr_rotate(tree, parent);
				/*
				 * After case 3 and 4, the number of this subtree's black nodes is
				 * the same as node's before deleted. So this tree is already balanced.
				 */
				break;
			}
		}
		else { /* Node is in right. */
			bro = parent->left;
			/* Case 1 */
			if (bro->color == RED) {
				bro->color = BLACK;
				parent->color = RED;
				simple_rb_ll_rotate(tree, parent);
				bro = parent->left;
			}
			/* Case 1 is convert to case 2, 3, 4. */
			/* Case 2 */
			if (bro->left->color == BLACK && bro->right->color == BLACK)
			{
				bro->color = RED;
				node = parent;
				parent = node->parent;
			}
			else {
				/* Case 3*/
				if (bro->left->color == BLACK) {
					bro->right->color = BLACK;
					bro->color = RED;
					simple_rb_rr_rotate(tree, bro);
					bro = parent->left;
				}
				/* Case 4 */
				bro->color = parent->color;
				parent->color = BLACK;
				bro->left->color = BLACK;
				simple_rb_ll_rotate(tree, parent);
				break;
			}
		}
	}
	/* The 'parent' is always black in all cases. */
	node->color = BLACK;
}

int simple_rb_remove(TREE* tree, TYPE data)
{
	NODE* child;
	NODE* parent; /* it's child's parent. */
	NODE* temp;
	NODE* root = tree->root;
	NODE* nil = tree->nil;
	NODE* node = simple_rb_find(tree, data);
	COLOR color = BLACK;
	if (node == nil) {
		return -1;
	}

	/*
	 * Judge node's position if it has no child, or only one child, or
	 * two children. Depend on different positions, perform different
	 * methods.
	 */
#if 0
	if (node->left == nil) {
		child = node->right;
		temp = node;
	}
	else if (node->right == nil) {
		child = node->left;
		temp = node;
	}
	else {
		/* When node has two children */
		temp = simple_rb_successor(tree, node);
		/* Successor node has no left child. */
		child = temp->right; 
	}
#else
	temp = (node->left == nil || node->right == nil) ? node : simple_rb_successor(tree, node);
	child = temp->left != nil ? temp->left : temp->right;
#endif
	/* 
	 * If node has two children, we should use D to replace B if *1 
	 * doesn't exist. 
	 */
	if (temp != node) {
		/*                A
		 *               /
		 *              B(deleted)
		 *             / \
		 *            C   D
		 *           /   / \
		 *          *1  *2  *3
		 */
		node->left->parent = temp;
		temp->left = node->left;

		/* It means *2 exist. */
		if (temp != node->right) {
			/* Reconnect child and temp's parent due to removal of temp. */
			parent = temp->parent;
			if (child != nil) {
				child->parent = parent;
			}
			temp->parent->left = child;
			/* Replace node with temp. */
			temp->right = node->right;
			node->right->parent = temp;
		}
		/* 
		 * *2 doesn't exist. D is the successor node. And *3 doesn't have
		 *  to be maintained. So temp is always child's parent.
		 */
		else {
			parent = temp;
		}

		/* Store the successor color used for checking deleted node's color. */
		color = temp->color;
		temp->color = node->color;

		/* Reconnect A and the successor. */
		if (root == node) {
			/* 
			 * Remember root just a copy of tree->root. 'root = temp' cannot change 
			 * the value of 'tree->root'.
		     */
			tree->root = temp;
		}
		else if (node->parent->left == node) {
			node->parent->left = temp;
		}
		else {
			node->parent->right = temp;
		}
		temp->parent = node->parent;
	}
	/* 'temp == node' means there has one child at most */
	else {
		/*                A
		 *               /
		 *              B(deleted)
		 *             / 
		 *            C   
		 *           /  
		 *          *1  
		 *  In this tree, *1 is always nil node.
		 */
		color = temp->color;
		parent = temp->parent;
		if (child != nil) {
			child->parent = parent;
		}

		if (root == node) {
			tree->root = child;
		}
		else if (node->parent->left == node) {
			node->parent->left = child;
		}
		else {
			node->parent->right = child;
		}
	}

	free(node);
	if (color == BLACK) {
		simple_rb_remove_fixup(tree, child, parent);
	}
	return 0;

}

void simple_rb_destroy_tree(NODE* node, TREE* tree)
{
	if (node == tree->nil)
		return;
	simple_rb_destroy_tree(node->left, tree);
	simple_rb_destroy_tree(node->right, tree);
	free(node);
	node = NULL;
}

void simple_rb_destroy(TREE* tree)
{
	simple_rb_destroy_tree(tree->root, tree);
	free(tree->nil);
	free(tree);
}

void simple_rb_traverse(TREE* tree)
{
	if (!tree) return;
	int in = 0;
	int out = 0;
	NODE* array[42 * 2];

	array[in++] = tree->root;
	while (in > out) {
		if (array[out] != tree->nil) {
			printf(" %s-%d", array[out]->color == RED ? "ºì" : "ºÚ", array[out]->data);
			array[in++] = array[out]->left;
			array[in++] = array[out]->right;
		}
		out++;
	}
	printf("\n");
}


#if 1

int main()
{
	TREE* tree = simple_rb_tree_create();

	simple_rb_insert(tree, 1);
	simple_rb_insert(tree, 2);
	simple_rb_insert(tree, 3);
	simple_rb_insert(tree, 4);
	simple_rb_insert(tree, 5);
	simple_rb_insert(tree, 6);
	simple_rb_insert(tree, 7);
	simple_rb_insert(tree, 8);
	simple_rb_insert(tree, 9);
	simple_rb_insert(tree, 10);

	simple_rb_traverse(tree);

	/*
	 *               4                              6
	 *              / \                            / \
	 *             /   \                          /   \
	 *            2     6                        4     8
	 *           / \   / \                      / \   / \
	 *          1   3 5   8       ====>        2   5 7   9
	 *                   / \                    \         \
	 *                  7   9                    3         10
	 *                       \
	 *                        10
	 *                
	 */
	simple_rb_remove(tree, 1);
	simple_rb_traverse(tree);

	simple_rb_remove(tree, 6);
	simple_rb_traverse(tree);

	simple_rb_remove(tree, 10);
	simple_rb_traverse(tree);

	simple_rb_destroy(tree);

	return 0;
}

#endif

