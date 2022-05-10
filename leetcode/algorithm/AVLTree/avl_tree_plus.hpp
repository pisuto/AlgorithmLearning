#pragma once
#include <iostream>
#include <string>
#include <type_traits>
#include <functional>
#include <queue>

namespace avl {

	template<typename T> struct tree_node_base;
	template<typename T> struct tree_node;

	template<typename T>
	struct node_traits {
		using base_ptr = tree_node_base<T>*;
		using node_ptr = tree_node<T>*;
	};
	
	template<typename T>
	struct tree_node_base {
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;

		int height;
		base_ptr left;
		base_ptr right;
		base_ptr parent;

		tree_node_base() noexcept : 
			height(1) {}

		base_ptr self() {
			return static_cast<base_ptr>(&*this);
		}

		node_ptr as_node() {
			return static_cast<node_ptr>(self());
		}

		int update_height() {
			height = 1 + std::max(this->left ? this->left->height : 0,
				this->right ? this->right->height : 0);
			return height;
		}
	};

	template<typename T>
	struct tree_node : public tree_node_base<T> {
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;
		
		/* Data domain */
		T data;

		/* 
		 * If base class has non-paramter constructor, subclass 
		 * will use this constructor to initialize base class.
		 */
		tree_node() noexcept = default;

		tree_node(const T& t) noexcept :
			data(t) {}

		tree_node(T&& t) noexcept :
			data(std::move(t)) {}

		base_ptr as_base() {
			return static_cast<base_ptr>(self());
		}

		node_ptr self() {
			return static_cast<node_ptr>(&*this);
		}
	};

	template<typename T>
	class tree_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
	public:
		using value_type = T;
		using pointer    = T*;
		using reference  = T&;
		using self       = tree_iterator<T>;
		using base_ptr   = typename node_traits<T>::base_ptr;
		using node_ptr   = typename node_traits<T>::node_ptr;

	public:
		tree_iterator() = default;
		tree_iterator(base_ptr x) : 
			node_(x) {}
		tree_iterator(node_ptr x) : 
			node_(x->as_base()) {}
		tree_iterator(const self& rhs) : 
			node_(rhs.node_) {}
		tree_iterator(self&& rhs) : 
			node_(std::move(rhs.node_)) { rhs.node_ = nullptr; }
		/*
		 * The overload operators '*' and '->' return the address of
		 * data domain. It is same in other container like list.
		 */
		reference operator*() const { return node_->as_node()->data; }
		pointer operator->() const { return &(operator*()); }

		bool operator==(const self& rhs) const { 
			return node_ == rhs.node_; 
		}

		bool operator!=(const self& rhs) const {
			return !(*this == rhs);
		}

		self& operator++() { /* post-increment */
			/*
			 *         A
			 *        / \
			 *       B   C
			 *          /
			 *         D
			 * 
			 * Assume node_ now is 'A', in inorder traversal which sort 
			 * elements in ascending order, its next node is 'D'. So the
			 * minimal node greater than it is leftmost node in its right 
			 * subtree. 
			 */
			if (node_->right) {
				node_ = node_->right;
				while (node_->left) {
					node_ = node_->left;
				}
			}
			else {
				/*
				 * In order to find parent node, it's necessary to store
				 * the pointer to parent node, because iterator only save
				 * a lonely node, the root node is unknown.
				 * 
				 *              A
				 *             / \
				 *            B   C
				 *           / \
				 *          D   E
				 *               \
				 *                F
				 * 
				 * Assume node_ is 'D', then its next node_ is 'B' if it's
				 * the left node of its parent.
				 * When node_ is the right child of its parent like 'F', its 
				 * next node is 'A'  
				 */
#if 0
				auto temp = node_->parent;
				if (temp) {
					if (temp->left == node_) {
						node_ = temp;
					}
					else {
						while (temp && temp->right = node_) {
							node_ = temp;
							temp = node_->parent;
						}
						node_ = temp;
					}
				}
				else {
					node_ = temp;
				}
#else 
				/*
				 * Optimize the above code. When node_->right isn't equal
				 * to temp, this means temp is its left child, then break
				 * the loop.
				 */
				base_ptr temp;
				do {
					temp = node_;
					node_ = node_->parent;
				} while (node_ && node_->right == temp);
#endif
			}
			return *this;
		}

		self& operator--() { /* pre-increment */
			if (node_->left) {
				node_ = node_->left;
				while (node_->right) {
					node_ = node_->right;
				}
			}
			else {
				base_ptr temp;
				do {
					temp = node_;
					node_ = node_->parent;
				} while (node_ && node_->left == temp);
			}
			return *this;
		}

		self operator++(int) {
			auto temp = *this;
			++(*this);
			return temp;
		}

		self operator--(int) {
			auto temp = *this;
			--(*this);
			return temp;
		}

		reference operator=(const self& rhs) { node_ = rhs.node_; return *this; }

		bool operator>(const self& rhs) { 
			return **this > *rhs; 
		}

		bool operator<(const self& rhs) {
			return **this < *rhs;
		}

		bool operator>=(const self& rhs) {
			return **this >= *rhs;
		}

		bool operator<=(const self& rhs) {
			return **this <= *rhs;
		}

		base_ptr node_;
	};

	template<typename T>
	struct const_tree_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
	public:
		using value_type = T;
		using pointer    = const T*;
		using reference  = const T&;
		using self       = const_tree_iterator<T>;
		using base_ptr   = typename node_traits<T>::base_ptr;
		using node_ptr   = typename node_traits<T>::node_ptr;

	public:
		const_tree_iterator() = default;
		const_tree_iterator(base_ptr x) :
			node_(x) {}
		const_tree_iterator(node_ptr x) :
			node_(x->as_base()) {}
		const_tree_iterator(const self& rhs) :
			node_(rhs.node_) {}
		const_tree_iterator(self&& rhs) :
			node_(std::move(rhs.node_)) {
			rhs.node_ = nullptr;
		}
		const_tree_iterator(const tree_iterator<T>& rhs) :
			node_(rhs.node_) {}
		const_tree_iterator(tree_iterator<T>&& rhs) :
			node_(std::move(rhs.node_)) {
			rhs.node_ = nullptr;
		}
		
		reference operator*() const { return node_->as_node()->data; }
		pointer operator->() const { return &(operator*()); }

		bool operator==(const self& rhs) const {
			return node_ == rhs.node_;
		}

		bool operator!=(const self& rhs) const {
			return !(*this == rhs);
		}

		self& operator++() { /* post-increment */
			if (node_->right) {
				node_ = node_->right;
				while (node_->left) {
					node_ = node_->left;
				}
			}
			else {
				base_ptr temp;
				do {
					temp = node_;
					node_ = node_->parent;
				} while (node_ && node_->right == temp);
			}
			return *this;
		}

		self& operator--() { /* pre-increment */
			if (node_->left) {
				node_ = node_->left;
				while (node_->right) {
					node_ = node_->right;
				}
			}
			else {
				base_ptr temp;
				do {
					temp = node_;
					node_ = node_->parent;
				} while (node_ && node_->left == temp);
			}
			return *this;
		}

		self operator++(int) {
			auto temp = *this;
			++(*this);
			return temp;
		}

		self operator--(int) {
			auto temp = *this;
			--(*this);
			return temp;
		}

		reference operator=(const self& rhs) { node_ = rhs.node_; return *this; }

		bool operator>(const self& rhs) {
			return **this > *rhs;
		}

		bool operator<(const self& rhs) {
			return **this < *rhs;
		}

		bool operator>=(const self& rhs) {
			return **this >= *rhs;
		}

		bool operator<=(const self& rhs) {
			return **this <= *rhs;
		}

		base_ptr node_;
	};

	template<typename T>
	class tree {
	public:
		using allocator_type	= std::allocator<T>;
		using data_allocator	= std::allocator<T>;
		using node_allocator    = std::allocator<tree_node<T>>;
		using base_allocator    = std::allocator<tree_node_base<T>>;

		using value_type		= typename allocator_type::value_type;
		using reference			= typename allocator_type::reference;
		using pointer			= typename allocator_type::pointer;
		using const_pointer		= typename allocator_type::const_pointer;
		using const_reference	= typename allocator_type::const_reference;
		using size_type			= typename allocator_type::size_type;
		using difference_type	= typename allocator_type::difference_type;

		using iterator			= tree_iterator<T>;
		using const_iterator	= const_tree_iterator<T>;
		using reverse_iterator  = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;

	private:
		base_ptr root_; /* tree root node */
		size_t size_;   /* tree node count */
		node_allocator node_alloc_;
		data_allocator data_alloc_;

	public:
		node_allocator get_allocator() { return node_alloc_; }

		tree() noexcept :
			root_(nullptr),
			size_(0) {}

		tree(const T& t) noexcept 
		{
			root_ = create_node(t);
			size_++;
		}

		tree(const tree& rhs) noexcept :
			size_(rhs.size_) 
		{
			/* 全部遍历一遍深拷贝 */
			root_ = deep_copy(rhs.root_->as_node());
		}

		tree(tree&& rhs) noexcept :
			root_(rhs.root_),
			size_(rhs.size_) {}

		~tree() noexcept {
			clear();
		}

		reference front() noexcept {
			return *(begin());
		}

		const_reference front() const noexcept {
			return *(begin());
		}

		iterator begin() noexcept {
			auto temp = root_;
			while (temp->left) {
				temp = temp->left;
			}
			return iterator(temp);
		}
		
		const_iterator begin() const noexcept {
			const auto temp = root_;
			while (temp->left) {
				temp = temp->left;
			}
			return const_iterator(temp);
		}

		const_iterator cbegin() const noexcept {
			return begin();
		}

		iterator end() noexcept {
			return iterator(root_->parent);
		}

		const_iterator end() const noexcept {
			return const_iterator(root_->parent);
		}

		const_iterator cend() const noexcept {
			return end();
		}

		reference back() {
			const auto temp = root_;
			while (temp->right) {
				temp = temp->right;
			}
			return *temp;
		}

		bool empty() const noexcept {
			return size_ == 0;
		}

		void clear() noexcept {
			if (empty()) return;
			clear_node(root_);
			destroy_node(root_);
			root_ = nullptr;
			size_ = 0;
		}

		void swap(tree& rhs) {
			std::swap(root_, rhs.root_);
			std::swap(size_, rhs.size_);
		}

		size_t size() noexcept {
			return size_;
		}

		iterator insert(const T& t) {
			if (empty()) {
				root_ = create_node(t);
				size_++;
				return end();
			}
			return iterator(insert_native(root_, t));
		}

		iterator insert(T&& t) {
			if (empty()) {
				root_ = create_node(t);
				size_++;
				return end();
			}
			return iterator(insert_native(root_, std::forward<T>(t)));
		}

		iterator erase(iterator it) {
			auto node = it.node_->as_node();
			auto next = ++it;
			erase_native(node);
			return next;
		}

		iterator find(const_reference ref) {
			auto temp = root_;
			while (temp) {
				if (temp->as_node()->data == ref) {
					return temp;
				}
				else if (temp->as_node()->data > ref) {
					temp = temp->left;
				}
				else {
					temp = temp->right;
				}
			}
			return end();
		}

		void remove(const_reference ref) {
			auto it = find(ref);
			if (it != end()) {
				erase(it);
			}
		}

		reference operator[](size_type i) {
			return *(at(i));
		}

		const_reference operator[](size_type i) const {
			return *(at(i));
		}

		iterator at(size_type i) {
			if (i >= size()) {
				throw std::out_of_range("index is out of tree[]");
			}
			auto it = begin();
			size_type pos = 0;
			while (pos++ < i) {
				it++;
			}
			return it;
		}

		const_iterator at(size_type i) const {
			if (i >= size()) {
				throw std::out_of_range("index is out of tree[]");
			}
			auto it = cbegin();
			size_type pos = 0;
			while (pos++ < i) {
				it++;
			}
			return it;
		}

		template<typename U>
		friend std::ostream& operator<<(std::ostream&, const tree<U>&);

	private:
		void tree_rebalance(base_ptr node) { 
			while (1) {
				if (!node) {
					break; /* Null means this node is root_'s parent. */
				}
				int delta = get_balanced_factor(node);
				if (delta > 1) {
					if (get_balanced_factor(node->left) >= 0) {
						node = ll_rotate(node);
					}
					else {
						node = lr_rotate(node);
					}
				}
				else if (delta < -1) {
					if (get_balanced_factor(node->right) <= 0) {
						node = rr_rotate(node);
					}
					else {
						node = rl_rotate(node);
					}
				}
				else {
					int height = node->height;
					node->update_height();
					if (height == node->height) {
						break;
					}
				}
				node = node->parent;
			}
		}

		node_ptr insert_native(node_ptr node, const T& t) {
			iterator res;
			while (1) {
				if (node->data < t) {
					if (node->right) {
						node = node->right;
					}
					else {
						node->right = create_node(t);
						node->right->parent = node;
						res = node->right;
						break;
					}

				}
				else if (node->data > t) {
					if (node->left) {
						node = node->left;
					}
					else{
						node->left = create_node(t);
						node->left->parent = node;
						res = node->left;
						break;
					}
				}
				else {
					return node;
				}
			}

			tree_rebalance(node);
			size_++;
			return res;
		}

		base_ptr insert_native(base_ptr node, T&& t) {
			base_ptr res;
			while (1) {
				if (node->as_node()->data < t) {
					if (node->right) {
						node = node->right;
					}
					else {
						node->right = create_node(std::forward<T>(t));
						node->right->parent = node;
						res = node->right;
						break;
					}

				}
				else if (node->as_node()->data > t) {
					if (node->left) {
						node = node->left;
					}
					else {
						node->left = create_node(std::forward<T>(t));
						node->left->parent = node;
						res = node->left;
						break;
					}
				}
				else {
					return node;
				}
			}

			tree_rebalance(node);
			size_++;
			return res;
		}

		void erase_native(base_ptr node) {
			base_ptr unbalanced_node;
			if (!node->left) {
				unbalanced_node = node->parent;
				reconnect_parent_with_new_child(node->right, node);
				destroy_node(node);
			}
			else {
				/* Find precessor node */
				auto temp = node->left;
				while (temp->right) {
					temp = temp->right;
				}
				unbalanced_node = temp->parent;
#if 0
				/*
				 * Copy the data of the leaf node to the deleted node,
				 * in order to retain the node which should have been
				 * deleted. This is the easist way to treat two nodes'
				 * swap of their position in the tree.
				 */
				node->as_node()->data = temp->as_node()->data;
				reconnect_parent_with_new_child(temp->left, temp);
				destroy_node(temp);
#else
				/* 
				 * If temp isn't the left child of node, reconnect temp's
				 * parent with its left child. Otherwise, it means node
				 * is the parent of temp. So unbalanced state starts from
				 * itself.
				 */
				if (temp->parent != node) {
					reconnect_parent_with_new_child(temp->left, temp);
				}
				else {
					unbalanced_node = temp;
				}
				/* Set node's right as temp's right. */
				if (node->right) {
					node->right->parent = temp;
				}
				temp->right = node->right;
				/* Set node's parent as temp's parent. */
				reconnect_parent_with_new_child(temp, node);
				destroy_node(node);
#endif
			}

			tree_rebalance(unbalanced_node);
			size_--;
			return;
		}

		/*
		 * This function reconnect node2's parent with node1 as new 
		 * child. 
		 */
		void reconnect_parent_with_new_child(base_ptr node1, base_ptr node2) {
			if (node2->parent) {
				if (node2->parent->left == node2) {
					node2->parent->left = node1;
				}
				else {
					node2->parent->right = node1;
				}
			}
			if (node1 && node1->parent) {
				node1->parent = node2->parent;
			}
		}

		base_ptr ll_rotate(base_ptr node) {
			/* Change child's relationship */
			auto temp = node->left;
			node->left = temp->right;
			temp->right = node;
			/* Change parent's relationship */
			reconnect_parent_with_new_child(temp, node);
			node->parent = temp;
			if (node->left) {
				node->left->parent = node;
			}
			/* Update node's height */
			node->update_height();
			temp->update_height();
			/* Update root node */
			if (!temp->parent) {
				root_ = temp;
			}
			return temp;
		}

		base_ptr rr_rotate(base_ptr node) {
			/* Change child's relationship */
			auto temp = node->right;
			node->right = temp->left;
			temp->left = node;
			/* Change parent's relationship */
			reconnect_parent_with_new_child(temp, node);
			node->parent = temp;
			if (node->right) {
				node->right->parent = node;
			}
			/* Update node's height */
			node->update_height();
			temp->update_height();
			/* Update root node */
			if (!temp->parent) {
				root_ = temp;
			}
			return temp;
		}

		base_ptr lr_rotate(base_ptr node) {
			node->left = rr_rotate(node->left);
			node = ll_rotate(node);
			return node;
		}

		base_ptr rl_rotate(base_ptr node) {
			node->right = ll_rotate(node->right);
			node = rr_rotate(node);
			return node;
		}

		int get_height(base_ptr node) {
			return node ? node->height : 0;
		}

		int get_balanced_factor(base_ptr node) {
			return node ? get_height(node->left) - get_height(node->right) : 0;
		}

		template<typename ...Args>
		node_ptr create_node(Args&&... args) {
			auto temp = node_alloc_.allocate(1);
			try {
				data_alloc_.construct(std::addressof(temp->data), std::forward<Args>(args)...);
				temp->as_base()->height = 1;
				temp->as_base()->left = nullptr;
				temp->as_base()->right = nullptr;
				temp->as_base()->parent = nullptr;
			}
			catch (...) {
				node_alloc_.deallocate(temp, 1);
				throw;
			}
			return temp;
		}

		node_ptr deep_copy(const node_ptr root) {
			if (!root) return root;
			auto temp = create_node(root->data);
			temp->height = root->height;
			if (root->left) {
				temp->left = deep_copy(root->left);
				temp->left->parent = temp;
			}
			if (root->right) {
				temp->right = deep_copy(root->right);
				temp->right->parent = temp;
			}
			return temp;
		}

		void destroy_node(base_ptr node) {
			data_alloc_.destroy(&node->as_node()->data);
			node_alloc_.deallocate(node->as_node(), 1);
		}

		void clear_node(base_ptr node) noexcept {
			if (node->left) {
				clear_node(node->left);
				destroy_node(node->left);
			}
			
			if (node->right) {
				clear_node(node->right);
				destroy_node(node->right);
			}
		}
	};

	/* Overload swap */
	template<typename T>
	void swap(tree<T>& lhs, tree<T>& rhs) {
		lhs.swap(rhs);
	}

	template<typename U>
	std::ostream& operator<<(std::ostream& os, const tree<U>& t)
	{
		std::queue<typename node_traits<U>::base_ptr> que;
		que.push(t.root_);
		while (!que.empty()) {
			auto e = que.front();
			que.pop();
			if (e) {
				os << " " << e->as_node()->data;
				que.push(e->left);
				que.push(e->right);
			}
		}
		os << std::endl;
		return os;
	}
}