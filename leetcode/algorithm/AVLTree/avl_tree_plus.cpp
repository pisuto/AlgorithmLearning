#include "avl_tree_plus.hpp"

#if 1

int main()
{
	avl::tree<int> t;
	t.insert(1);
	t.insert(2);
	t.insert(3);
	t.insert(4);
	t.insert(5);
	t.insert(6);
	t.insert(7);
	t.insert(8);
	t.insert(9);
	std::cout << t;

	t.remove(6);
	std::cout << t;

	t.remove(3);
	std::cout << t;

	t.remove(4);
	std::cout << t;

	for (auto it = t.begin(); it != t.end(); ++it) {
		std::cout << " " << *it;
	}

	std::cout << std::endl << " " << t[0] << " " << t[1] << std::endl;
}

#endif