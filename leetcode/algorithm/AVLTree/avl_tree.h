#include "stdio.h"
#include "stdlib.h"

typedef struct {
	void* left;
	void* right;
	int height;
} AVL_NODE;

typedef union {
	int i;
	void* p;
} GENERIC_KEY;

typedef enum {
	AVL_NODE_KEY_EQUAL = 0,
	AVL_NODE_KEY_SMALL = 1,
	AVL_NODE_KEY_BIG = 2,
} AVL_COMP_NODE_KEY;

typedef enum {
	AVL_LEFT_UNBALANCED = 2,
	AVL_RIGHT_UNBALANCED = -2,
} AVL_COMP_NODE_BALANCED;

typedef AVL_NODE* AVL_TREE;

typedef int COMPARE_FUNC(void*, GENERIC_KEY);
typedef int TRAVERSE_FUNC(AVL_NODE* node);

#define AVL_INSERT_RECURSION
#undef AVL_INSERT_RECURSION

#define AVL_TREE_HEIGHT 42

#define AVL_TREE_ENTRY(entry, type, member) \
		((type*) ((char *)(entry) - offsetof(type, member)))
