#pragma once
#include <iostream>
#include <string>
#include <type_traits>
#include <functional>

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

		base_ptr left;
		base_ptr right;

		tree_node_base() = default;

		base_ptr self() {
			return static_cast<base_ptr>(&*this); /* 实现父类到字类的强制转换 */
		}

		node_ptr as_node() {
			return static_cast<node_ptr>(self()); /* 实现子类到父类的强制转换 */
		}
	};

	template<typename T>
	struct tree_node : public tree_node_base<T> {
		using base_ptr = typename node_traits<T>::base_ptr;
		using node_ptr = typename node_traits<T>::node_ptr;
		
		/* 数据域 */
		int height;
		T data;

		tree_node() noexcept :
			height(1) {}

		tree_node(const T& t) noexcept :
			data(t),
			height(1) {}

		tree_node(T&& t) noexcept :
			data(std::move(t)),
			height(1) {}

		base_ptr as_base() {
			return static_cast<base_ptr>(self());
		}

		node_ptr self() {
			return static_cast<node_ptr>(&*this);
		}

		void update_height() {
			height = 1 + std::max(left ? left->height : 0, 
								right ? right->height : 0);
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



	private:
		base_ptr node_;
	};

	template<typename T, typename Alloc = std::allocator<T>>
	class tree {
		struct NODE;
	public:
		using allocator_type	= Alloc;
		using data_allocator	= Alloc;

		using value_type		= typename allocator_type::value_type;
		using reference			= typename allocator_type::reference;
		using pointer			= typename allocator_type::pointer;
		using const_pointer		= typename allocator_type::const_pointer;
		using const_reference	= typename allocator_type::const_reference;
		using size_type			= typename allocator_type::size_type;
		using difference_type	= typename allocator_type::difference_type;

		using iterator			= NODE*;
		using const_iterator	= const NODE*;
		using reverse_iterator  = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<iterator>;

	private:
		iterator root_;
		size_t size_;

	public:
		tree() noexcept :
			root_(nullptr),
			size_(0) {}

		tree(const T& t) noexcept :
		{
			root_ = data_allocator::allocate();
			data_allocator::construct(root_, NODE(t));
			size_++;
		}

		tree(const tree& rhs) noexcept {
			/* 全部遍历一遍深拷贝 */
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
			auto it = root_;
			while (it->left) {
				it = it->left;
			}
			return it;
		}
		
		const_iterator begin() const noexcept {
			auto it = root_;
			while (it->left) {
				it = it->left;
			}
			return it;
		}

		const_iterator cbegin() const noexcept {
			return begin();
		}

		iterator end() noexcept {
			auto it = begin();
			while (it) {
				it = it->right;
			}
			return it;
		}

		const_iterator end() const noexcept {
			const auto it = begin();
			while (it) {
				it = it->right;
			}
			return it;
		}

		const_iterator cend() const noexcept {
			return end();
		}

		reference back() {
			if (empty()) return nullptr;
			auto it = begin();
			while (it->right) {
				it = it->right;
			}
			return *it;
		}

		bool empty() const noexcept {
			return size_ == 0;
		}

		void clear() const noexcept {
			if (empty()) return;
			clear_node(root_);
			data_allocator::destroy(root_);
			data_allocator::deallocate(root_);
			root_ = nullptr;
		}

		void swap(tree& rhs) {
			std::swap(root_, rhs.root_);
		}

		size_t size() noexcept {
			return size_;
		}

		iterator insert(const T& t) {
			iterator it;
			root_ = insert_native(root_, it, t);
			if (!it) size_++;
			return it;
		}

		iterator insert(T&& t) {
			iterator it;
			root_ = insert_native(root_, it, t);
			if (!it) size_++;
			return it;
		}

		iterator find(const_reference ref) {
			iterator it = root;
			while (it) {
				if (it->data == ref) {
					return it;
				}
				else if (it->data > ref) {
					it = it->left;
				}
				else {
					it = it->right;
				}
			}
			return end();
		}

		iterator erase(iterator it) {
			auto temp = successor(it)
			root_ = erase_native(root_, it);
			size_--;
			return temp;
		}

		void remove(const_reference ref) {
			auto it = find(ref);
			if (it) {
				erase(it);
			}
		}

	private:
		iterator insert_native(iterator it, iterator ret, const T& t) {
			if (!it) {
				auto tmp = data_allocator::allocate();
				data_allocator::construct(tmp, t);
				ret = tmp;
				return tmp;
			}

			if(it->data < t) {
				it->right = insert_native(it->right, ret, t);
			}
			else if (it->data > t) {
				it->left = insert_native(it->left, ret, t);
			}
			else {
				ret = nullptr;
				return it;
			}

			it->update_height();

			int factor = get_balanced_factor(it);
			if (factor >= 2) {
				if (get_balanced_factor(it->left) >= 0) {
					return ll_rotate(it);
				}
				else {
					return lr_rotate(it);
				}
			}
			else if (factor <= -2)
			{
				if (get_balanced_factor(it->right) <= 0)
				{
					return rr_rotate(it);
				}
				else {
					return rl_rotate(it);
				}
			}
			return it;
		}

		iterator insert_native(iterator it, iterator ret, T&& t) {
			if (!it) {
				auto tmp = data_allocator::allocate();
				data_allocator::construct(tmp, std::move(t));
				ret = tmp;
				return tmp;
			}

			if (it->data < t) {
				it->right = insert_native(it->right, ret, t);
			}
			else if (it->data > t) {
				it->left = insert_native(it->left, ret, t);
			}
			else {
				ret = it;
				return it;
			}

			it->update_height();

			int factor = get_balanced_factor(it);
			if (factor >= 2) {
				if (get_balanced_factor(it->left) >= 0) {
					return ll_rotate(it);
				}
				else {
					return lr_rotate(it);
				}
			}
			else if (factor <= -2)
			{
				if (get_balanced_factor(it->right) <= 0)
				{
					return rr_rotate(it);
				}
				else {
					return rl_rotate(it);
				}
			}
			return it;
		}

		iterator erase_native(iterator it, interator key) {
			if (!it) return it;

			if (it->data < key->data) {
				it->right = erase_native(it->right, key);
			}
			else if (it->data > key->data) {
				it->left = erase_native(it->left, key);
			}
			else {
				if (!it->left && !it->right) {
					data_allocator::destroy(it);
					data_allocator::deallocate(it);
					it = nullptr;
				}
				else if (it->left && !it->right) {
					*(it) = *(it->left);
					data_allocator::destroy(it->left);
					data_allocator::deallocate(it->left);
					it->left = nullptr;
				}
				else if (it->right && !it->left) {
					*(it) = *(it->right);
					data_allocator::destroy(it->right);
					data_allocator::deallocate(it->right);
					it->right = nullptr;
				}
				else {
					auto temp = successor(it);
					it->data = temp->data;
					it->right = erase_native(it->right, temp);
				}
			}

			if (!it) return it;

			it->update_height();

			int factor = get_balanced_factor(it);
			if (factor >= 2) {
				if (get_balanced_factor(it->left) >= 0) {
					return ll_rotate(it);
				}
				else {
					return lr_rotate(it);
				}
			}
			else if (factor <= -2)
			{
				if (get_balanced_factor(it->right) <= 0)
				{
					return rr_rotate(it);
				}
				else {
					return rl_rotate(it);
				}
			}
			return it;
		}

		iterator successor(iterator it) {
			if (!it) return it;
			auto temp = it->right;
			if (!temp) return it;
			while (temp->left) {
				temp = temp->left;
			}
			return temp;
		}

		iterator ll_rotate(iterator it) {
			auto tmp = it->left;
			it->left = tmp->right;
			tmp->right = it;
			return tmp;
		}

		iterator rr_rotate(iterator it) {
			auto tmp = it->right;
			it->right = tmp->left;
			tmp->left = it;
			return tmp;
		}

		iterator lr_rotate(iterator it) {
			it->left == rr_rotate(it->left);
			it = ll_rotate(it);
			return it;
		}

		iterator rl_rotate(iterator it) {
			it->right == ll_rotate(it->right);
			it = rr_rotate(it);
			return;
		}

		int get_height(iterator it) {
			return it ? it->height : 0;
		}

		int get_balanced_factor(iterator it) {
			return it ? get_height(it->left) - get_height(it->right) : 0;
		}

		void clear_node(iterator it) noexcept {
			if (it->left) {
				clear_node(it->left);
				data_allocator::destroy(it->left);
				data_allocator::deallocate(it->left);
			}
			
			if (it->right) {
				clear_node(it->right);
				data_allocator::destroy(it->right);
				data_allocator::deallocate(it->right);
			}
		}
	};

	/* 重载avl空间中的swap */
	template<typename T>
	void swap(tree<T>& lhs, tree<T>& rhs) {
		lhs.swap(rhs);
	}
}