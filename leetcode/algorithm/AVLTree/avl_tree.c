#include "avl_tree.h"
#include "stddef.h"

int avl_max(int t1, int t2) {
	return t1 > t2 ? t1 : t2;
}

int avl_height(AVL_TREE tree) {
	return tree ? tree->height : 0;
}

int avl_balanced_factor(AVL_TREE tree) {
	return tree ? avl_height(tree->left) - avl_height(tree->right) : 0;
}

AVL_TREE avl_ll_rotate(AVL_TREE node) {
	AVL_TREE temp = node->left;
	node->left = temp->right;
	temp->right = node;

	node->height = 1 + avl_max(avl_height(node->left),
		avl_height(node->right));
	temp->height = 1 + avl_max(avl_height(temp->left),
		avl_height(temp->right));
	return temp;
}

AVL_TREE avl_rr_rotate(AVL_TREE node) {
	AVL_TREE temp = node->right;
	node->right = temp->left;
	temp->left = node;

	node->height = 1 + avl_max(avl_height(node->left),
		avl_height(node->right));
	temp->height = 1 + avl_max(avl_height(temp->left),
		avl_height(temp->right));
	return temp;
}

AVL_TREE avl_lr_rotate(AVL_TREE node) {
	node->left = avl_rr_rotate(node->left);
	node = avl_ll_rotate(node);
	return node;
}

AVL_TREE avl_rl_rotate(AVL_TREE node) {
	node->right = avl_ll_rotate(node->right);
	node = avl_rr_rotate(node);
	return node;
}

void avl_rebalance(AVL_NODE*** ancestors, int count) {
	AVL_NODE* temp;
	AVL_NODE** temp_ptr;

	while (count--) {
		/* 
		 * 'temp' here means the current node.
		 * 'temp_ptr' means the address of subtree of 
		 * the parent of 'temp'.
		 */
		temp_ptr = ancestors[count];
		temp = *temp_ptr;
		int factor = avl_balanced_factor(temp);
		/* 
		 *		    *
		 *		   / \
		 *		 h+2  h
		 * 
		 * The current subtree violates the balance by its 
		 * left subtree being too high. In this situation,
		 * there are two configurations of the left substree.
		 */
		if (factor >= AVL_LEFT_UNBALANCED)
		{
			/*
			 * ll: 
			 *          A               B
			 *         / \             / \
			 *        B   C	  left	  D   A
			 *       h+2  h   ====>  h+1 h+2|h+1
			 *       / \                 / \
			 *      D   E               E   C
			 *     h+1  h+1|h         h+1|h h
			 * 
			 * As 'h' means height, it can be 0. And the tree
			 * above shows the heights of all nodes except the
			 * unbalanced nodes are same as before.
			 * 'left' means 'left-handed rotate'.
			 * 'right' means 'right-handed rotate'.
			 */
			if (avl_balanced_factor(temp->left) >= 0) {
				/*
				 * parent->left or right = node after rotation
				 */
				*temp_ptr = avl_ll_rotate(temp);
			}
			/*
			 * lr: 
			 *          A               A                 E
			 *         / \             / \              /   \
			 *        B   C	  left    E   C   right    B     A
			 *       h+2  h   ====>  h+2  h   ====>   h+1    h+1    
			 *       / \             / \             / \     /  \
			 *      D   E           B   G           D   F   G    C
			 *      h  h+1         h+1 h|h-1        h h|h-1 h|h-1 h
			 *         / \         / \  
			 *        F   G       D   F 
			 *     h|h-1 h|h-1    h  h|h-1
			 * 
			 */
			else {
				*temp_ptr = avl_lr_rotate(temp);
			}

		}
		/* 
		 * When the right subtree is unbalanced, the method is similiar
		 * to the left subtree.
		 */
		else if (factor <= AVL_RIGHT_UNBALANCED) {
			if (avl_balanced_factor(temp->right) <= 0) {
				*temp_ptr = avl_rr_rotate(temp);
			}
			else {
				*temp_ptr = avl_rl_rotate(temp);
			}
		}
		else {
			/* 
			 * If the tree isn't balanced, its height already
			 * update by rotate funcitons. When it's balanced
		     * and its height has no change, it means the tree
			 * and its parents are balanced, no need to check 
			 * their factors. 
			 */
			int height = 1 + avl_max(avl_height(temp->left),
				avl_height(temp->right));
			if (height == temp->height) {
				break;
			}
			temp->height = height;
		}
	}
}

#ifdef  AVL_INSERT_RECURSION
void* avl_insert(AVL_NODE* node, void* data, GENERIC_KEY key,
	COMPARE_FUNC compare)
{
	if (!node) {
		((AVL_TREE)data)->left = NULL;
		((AVL_TREE)data)->right = NULL;
		((AVL_TREE)data)->height = 1;
		return data;
	}

	int delta = compare(node, key);
	switch (delta)
	{
	case AVL_NODE_KEY_SMALL:
		node->left = avl_insert((AVL_TREE)node->left, data, key, compare);
		break;
	case AVL_NODE_KEY_BIG:
		node->right = avl_insert((AVL_TREE)node->right, data, key, compare);
		break;
	case AVL_NODE_KEY_EQUAL:
		return node;
	}

	node->height = 1 + avl_max(avl_height(node->left),
		avl_height(node->right));

	int factor = avl_balanced_factor(node);

	if (factor >= AVL_LEFT_UNBALANCED)
	{
		if (avl_balanced_factor(node->left) >= 0)
		{
			return avl_ll_rotate(node);
		}
		else {
			return avl_lr_rotate(node);
		}
	}
	else if(factor <= AVL_RIGHT_UNBALANCED)
	{
		if (avl_balanced_factor(node->right) <= 0)
		{
			return avl_rr_rotate(node);
		}
		else {
			return avl_rl_rotate(node);
		}
	}
	return node;
}
#else
void avl_insert(AVL_NODE** node, void* data, GENERIC_KEY key, 
	COMPARE_FUNC compare)
{
	int count = 0;
	AVL_NODE** ancestors[AVL_TREE_HEIGHT];
	AVL_NODE** temp_ptr = node;
	AVL_NODE* temp;

	while (1) {
		temp = *temp_ptr;
		/* 
		 * Inserting a node means treating the node as a
		 * leaf node. 
		 */
		if (!temp) {
			AVL_NODE* res = (AVL_NODE*)data;
			res->left = NULL;
			res->right = NULL;
			res->height = 1;
			*temp_ptr = res;
			break;
		}

		/*
		 * Don't save the inserted node, because it's a
		 * leaf node whose height is 0. 
		 */
		ancestors[count++] = temp_ptr;

		int delta = compare(temp, key);
		if (delta == AVL_NODE_KEY_SMALL) {
			temp_ptr = &temp->left;
		}
		else if (delta == AVL_NODE_KEY_BIG) {
			temp_ptr = &temp->right;
		}
		else {
			return;
		}
	}

	avl_rebalance(ancestors, count);
}
#endif

/* 
 * Recusion cannot work here, because key is not
 * defined in the node structure. Unless adding 
 * a new function to get the data from the pointer.
 * Therefore, a non-recursive method is used.
 * In this way, argument cannot be a first-level,
 * because of the existence of temporary copy.
 * when using 'AVL_NODE** node = &root', just get
 * the second-level pointer of the copy of the 
 * first-level pointer, not the real pointer of 
 * the root node. 
 */
void* avl_delete(AVL_NODE** node, GENERIC_KEY key, COMPARE_FUNC compare)
{
	int count = 0;
	AVL_NODE** ancestors[AVL_TREE_HEIGHT];
	AVL_NODE** temp_ptr = node;
	AVL_NODE* temp;
	AVL_NODE* deleted   = NULL;

	while (1) {
		temp = *temp_ptr;
		if (!temp) {
			return NULL;
		}

		/* 
		 * The last element of the array is the pointer to
		 * the leaf of the parent of the deleted node. 
		 */
		ancestors[count++] = temp_ptr;

		int delta = compare(temp, key);
		if (delta == AVL_NODE_KEY_SMALL) {
			temp_ptr = &temp->left;
		}	
		else if (delta == AVL_NODE_KEY_BIG) {
			temp_ptr = &temp->right;
		}
		else {
			deleted = temp;
			break;
		}
	}
	/*
	 * When the left node doesn't exist, as the balanced rule,
	 * the right node is null or a leaf node. And this right 
	 * node must be balanced, so perform 'count--'.
	 */
	if (!temp->left) {
		*temp_ptr = temp->right;
		count--;
	}
	/* 
	 * If the left subtree exists, the predecessor (or successor)
	 * node needs to be found to replace the node to be deleted.
	 */
	else {
		int deleted_count = count;
		AVL_NODE** deleted_ptr = temp_ptr;

		temp_ptr = &temp->left;
		while (1) {
			temp = *temp_ptr;
			if (!temp->right) {
				break;
			}
			ancestors[count++] = temp_ptr;
			temp_ptr = &temp->right;
		}
		/* 
		 * Explain the example below to understand the following code
		 * better:
		 *                     A                   
		 *                   /   \
		 *                  B     ... 
		 *                 /
		 *                C            
		 * now delete node A, the array will store node's secondary 
		 * pointer and 'temp_ptr' now means the pointer to 'A->left',
		 * so 'temp' points to node B and as '*temp_ptr' is set to
		 * 'temp->left', now the code means A->left = B->left = C. 
		 */
		*temp_ptr = temp->left;
		/* 
		 * 'deleted' points to node A. After finishing code above, 
		 * the following code is equivalent to 'B->left = A->left = C'
		 * Now the tree:
		 *				    B	A
		 *				    |  / \
		 *				    | /  ...
		 *                   C
		 */
		temp->left = deleted->left;
		temp->right = deleted->right;
		temp->height = deleted->height;
		/* 
		 * 'deleted_ptr' now is the root pointer, so replace node A
		 * by node B. If this pointer isn't the secondary pointer,
		 * then the final reuslt return by 'node' is always the pointer
		 * of node A, not node B:
		 *                      B
		 *                     / \
		 *                    C  ...
		 */
		*deleted_ptr = temp;

		ancestors[deleted_count] = &temp->left;
	}
	
	avl_rebalance(ancestors, count);
	return deleted;
}

void avl_traverse(AVL_TREE root, TRAVERSE_FUNC traverse)
{
	if (!root) return;
	int in = 0;
	int out = 0;
	AVL_NODE* array[AVL_TREE_HEIGHT * 2];
	
	array[in++] = root;
	while (in > out) {
		if (array[out]) {
			traverse(array[out]);
			array[in++] = array[out]->left;
			array[in++] = array[out]->right;
		}
		out++;
	}
}

#if 0 /* used for test */
typedef struct {
	AVL_NODE node;
	int key;
} TEST;

TEST* make_test(int val) {
	TEST* temp = (TEST*)malloc(sizeof(TEST));
	if (temp) {
		temp->key = val;
	}
	return temp;
}

int test_compare(void* p, GENERIC_KEY u)
{
	TEST* t = (TEST*)p;
	if (t->key < u.i)
		return AVL_NODE_KEY_BIG;
	else if (t->key > u.i)
		return AVL_NODE_KEY_SMALL;
	else
		return AVL_NODE_KEY_EQUAL;
}

AVL_TREE test_insert(AVL_TREE root, TEST* test, int val)
{
	GENERIC_KEY key;
	key.i = val;
#ifdef AVL_INSERT_RECURSION
	return avl_insert(root, test, key, test_compare);
#else 
	avl_insert(root, test, key, test_compare);
	return root;
#endif
}

AVL_TREE test_delete(AVL_TREE root, int val)
{
	GENERIC_KEY key;
	key.i = val;
	return avl_delete(root, key, test_compare);
}

void test_print(AVL_TREE node)
{
	if (!node) return;
	
	TEST* test = AVL_TREE_ENTRY(node, TEST, node);
	printf(" %u", test->key);
}

int main()
{
	TEST* test1 = make_test(1);
	TEST* test2 = make_test(2);
	TEST* test3 = make_test(3);
	TEST* test4 = make_test(4);
	TEST* test5 = make_test(5);
	TEST* test6 = make_test(6);
	TEST* test7 = make_test(7);
	TEST* test8 = make_test(8);
	TEST* test9 = make_test(9);
	TEST* test10 = make_test(10);

	AVL_TREE root = NULL;
#ifdef AVL_INSERT_RECURSION
	root = test_insert(root, test1, test1->key);
	root = test_insert(root, test2, test2->key);
	root = test_insert(root, test3, test3->key);
	root = test_insert(root, test4, test4->key);
	root = test_insert(root, test5, test5->key);
	root = test_insert(root, test6, test6->key);
	root = test_insert(root, test7, test7->key);
	root = test_insert(root, test8, test8->key);
	root = test_insert(root, test9, test9->key);
#else
	test_insert(&root, test1, test1->key);
	test_insert(&root, test2, test2->key);
	test_insert(&root, test3, test3->key);
	test_insert(&root, test4, test4->key);
	test_insert(&root, test5, test5->key);
	test_insert(&root, test6, test6->key);
	test_insert(&root, test7, test7->key);
	test_insert(&root, test8, test8->key);
	test_insert(&root, test9, test9->key);
	test_insert(&root, test10, test10->key);
#endif
	avl_traverse(root, test_print);
	printf("\n");

	AVL_NODE* deleted1 = test_delete(&root, 6);
	avl_traverse(root, test_print);
	printf("\n %u \n", ((TEST*)deleted1)->key);

	AVL_NODE* deleted2 = test_delete(&root, 3);
	avl_traverse(root, test_print);
	printf("\n %u \n", ((TEST*)deleted2)->key);

	AVL_NODE* deleted3 = test_delete(&root, 4);
	avl_traverse(root, test_print);
	printf("\n %u", ((TEST*)deleted3)->key);

	free(test1);
	free(test2);
	free(test3);
	free(test4);
	free(test5);
	free(test6);
	free(test7);
	free(test8);
	free(test9);
	free(test10);
	return 0;
}
#endif