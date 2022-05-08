#include <boost/type_index.hpp>
#include <iostream>

struct node {
	int data;
	node* next;
};

void test(node* root) {
	node** ptr = &root;
	*ptr = root->next;
}

int main()
{
	node* pn1 = new node();
	pn1->data = 1;
	node* pn2 = new node();
	pn2->data = 2;
	node* pn3 = new node();
	pn3->data = 3;

	pn1->next = pn2;
	pn2->next = pn3;
	
	test(pn1);

	delete pn1;
	delete pn2;
	delete pn3;

	return 0;
}