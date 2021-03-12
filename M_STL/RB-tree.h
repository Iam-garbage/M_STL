#ifndef RB_TREE
#define RB_TREE
#include"my_allocator.h"
#include"mytype_traits.h"
#include"my_iterator.h"
using rb_tree_color_type = bool;
rb_tree_color_type tree_red = false;
rb_tree_color_type tree_black = true;//������
//�޸�ʱ��;2020//11/28
//����汾���map���� key��������ȡ�����в�ͬ
//���� map��allocatorͷ�ļ� ��ת�����Ѿ���Ϊ< ����ע�⣩

struct rb_tree_node_base {
	using color_type = rb_tree_color_type;
	using base_ptr	 = rb_tree_node_base*;

	color_type color;
	base_ptr parent;
	base_ptr left;
	base_ptr right;
	static base_ptr minimun(base_ptr x) {
		while (x->left!=0)x = x->left;
		return x;
	}
	static base_ptr maximum(base_ptr x) {
		while (x->right != 0)x = x->right;
		return x;
	}
};

template<class T>
struct _rb_tree_node :public rb_tree_node_base{
	using link_type = _rb_tree_node<T>*;
	T value;
};

struct rb_tree_iterator_base {
	using iterator_category = bidirectional_iterator_tag;
	using base_ptr = rb_tree_node_base::base_ptr;
	using difference_type = ptrdiff_t;
	base_ptr node;
	void increment() {
		if (node->right != 0) {//right->left
			node = node->right;
			while (node->left != 0) {
				node = node->left;
			}
		}
		else {//parent->right
			base_ptr y = node->parent;
			while (node == y->right) {
				node = y;
				y = y->parent;
			}
			if (node->right != y)
				node = y;
		}
	}
	void decrement() {
		if (node->color == tree_red && node->parent->parent == node)
			node = node->right;//�˴�Ϊͷ�ڵ����� header�ڵ�
		else if (node->left != 0) {//left->right
			base_ptr y = node->left;
			while (y->right != 0) {
				y = y->right;
			}
			node = y;
		}
		else {
			base_ptr y = node->parent;
			while (node == y->left) {//parent->left
				node = y;
				y = y->parent;
			}
			node = y;
		}
	}
};


template<class T,class ref,class ptr>
struct rb_tree_iterator :public rb_tree_iterator_base{
	using value_type = T;
	using reference = ref;
	using pointer = ptr;
	using iterator = rb_tree_iterator<T, T&, T*>;
	using const_iterator = rb_tree_iterator<T, const T&, const T*>;
	using self = rb_tree_iterator<T, ref, ptr>;
	using link_type = _rb_tree_node<T>*;
	rb_tree_iterator() {}
	rb_tree_iterator(link_type x) { node = x; }
	rb_tree_iterator(const rb_tree_iterator& x) { node = x.node; }
	reference operator*()const {
		return link_type(node)->value;
	}
	pointer operator->() const {
		return &(operator*());
	}
	self& operator++() {
		increment();
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		increment();
		return tmp;
	}
	self& operator--() {
		decrement();
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		decrement();
		return tmp;
	}
	bool operator==(const rb_tree_iterator& x) {
		return node == x.node;
	}
	bool operator!=(const rb_tree_iterator& x) {
		return node != x.node;
	}
};

inline bidirectional_iterator_tag
iterator_category(const rb_tree_iterator_base&) {
	return bidirectional_iterator_tag();
}

inline rb_tree_iterator_base::difference_type*
distance_type(const rb_tree_iterator_base&) {
	return (rb_tree_iterator_base::difference_type*) 0;
}

template <class _Value, class _Ref, class _Ptr>
inline _Value* value_type(const rb_tree_iterator<_Value, _Ref, _Ptr>&) {
	return (_Value*)0;
}


template<class Key, class value, class keyofvalue,
	class compare, class Alloc = my_allocator<value>>
class rb_tree {
public:
		using key_type = Key;
		using value_type = value;
		using pointer = value*;
		using const_pointer = const pointer;
		using reference = value&;
		using const_reference = const reference;
		using link_type = _rb_tree_node<value>*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = rb_tree_iterator<value_type, reference, pointer>;
		using const_iterator = const iterator;
		using reverse_iterator = reverse_iterator<iterator>;
		using const_reverse_iterator = const reverse_iterator;
		using allocator_type = Alloc;
protected:
	using void_pointer = void*;
	using base_ptr = rb_tree_node_base*;
	using rb_tree_node = _rb_tree_node<value>;
	using rb_tree_node_allocator = my_allocator<rb_tree_node>;//ֱ��ʹ���˶��������� ��ʹ����Χ��ģ��
	using color_type = rb_tree_color_type;

protected://��ת ɾ�� ���� ƽ��
	inline void rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root) {
		//if (x != header) {
			rb_tree_node_base* y = x->right;
			x->right = y->left;
			if (y->left != 0) {
				y->left->parent = x;
			}
			y->parent = x->parent;
			if (x == root) {
				root = y;
				header->parent = y;
			}
			else if (x == x->parent->left)
				x->parent->left = y;
			else
				x->parent->right = y;
			y->left = x;
			x->parent = y;
		//}
	}
	inline void rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
		//if (x != header) {
			rb_tree_node_base* y = x->left;
			x->left = y->right;
			if (y->right != 0)
				y->right->parent = x;
			y->parent = x->parent;
			if (x == root) {
				root = y;
				header->parent = y;
			}
			else if (x == x->parent->right)
				x->parent->right = y;
			else
				x->parent->left = y;
			y->right = x;
			x->parent = y;
		//}
	}

	inline rb_tree_node_base* rb_tree_rebalance_for_erase(
		rb_tree_node_base* z, rb_tree_node_base*& root,
		rb_tree_node_base*& leftmost, rb_tree_node_base*& rightmost) {
		rb_tree_node_base* y = z;
		rb_tree_node_base* x = nullptr;
		rb_tree_node_base* x_parent = nullptr;
		if (y && y->left == 0)//ֻ��һ���������
			x = y->right;
		else
			if (y && y->right == 0)//ֻ��һ���������
				x = y->left;
			else
			{
				if(y)y = y->right;

				while (y && y->left != 0) {
					y = y->left;
				}
				x = y->right;
			}//�˴����Һ��ʵ�ɾ���ڵ� y���� xΪ����ڵ�

		if (y != z) {
			z->left->parent = y;
			y->left = z->left;
			if (y != z->right) {
				x_parent = y->parent;
				if (x)x->parent = y->parent;
				y->parent->left = x;
				y->right = z->right;
				z->right->parent = y;
			}
			else
				x_parent = y;

			if (root == z)
				root = y;
			else if (z->parent->left == z)
				z->parent->left = y;
			else 
				z->parent->right = y;

			y->parent = z->parent;
			std::swap(y->color, z->color);
			y = z;
		}
		else {//y==z
			x_parent = y->parent;
			if(x)x->parent = y->parent;
			if (root == z) {
				root = x;//�Ƴ��Ļ� �油�ڵ�Ҫ���µĸ��ڵ�
				header->parent = x;
			}
			else if (z && z->parent->left == z)
				z->parent->left = x;
			else if (z && z->parent->right == z)
				z->parent->right = x;

			//if���ֲ���Ҫ�ж�����Ϊ if����Ľڵ㶼���ڲ�� 
			if (z == leftmost)//���������ֵ ��û�и����ֵ
				if (z->right == 0)//��ֵҲû�� �Ǿ���һ��ֵ�̳�����
					leftmost = z->parent;
				else
					leftmost = rb_tree_node_base::minimun(x);
			if (z == rightmost)
				if (z->left == 0)
					rightmost = z->parent;
				else
					rightmost = rb_tree_node_base::maximum(x);
		}

		//����if else������ڽ�����ɫ���жϺ͵���
		if (y->color != tree_red) {
			while (x != root && (x == 0 || x->color == tree_black)) {
				if (x == x_parent->left) {
					rb_tree_node_base* brother = x_parent->right;
					if (brother->color == tree_red) {
						brother->color = tree_black;
						x_parent->color = tree_red;
						rb_tree_rotate_left(x_parent, root);
						brother = x_parent->right;
					}
					if ((brother->left == 0 || brother->left->color == tree_black) &&
						(brother->right == 0 || brother->right->color == tree_black)) {
						brother->color = tree_red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else {
						//Ҫô����rΪ�� Ҫô������r
						if (brother->right == 0 ||//������ж����жϽ���else���ӽڵ��ĸ��Ǻ�ɫ
							brother->right->color == tree_black) {
							if (brother->left)brother->left->color = tree_black;
							brother->color = tree_red;
							rb_tree_rotate_right(brother, root);
							brother = x_parent->right;
						}
						brother->color = x_parent->color;
						x_parent->color = tree_black;
						if (brother->right)brother->right->color = tree_black;
						rb_tree_rotate_left(x_parent, root);
						break;
					}
				}
				else {
					rb_tree_node_base* brother = x_parent->left;
					if (brother&&brother->color == tree_red) {
						brother->color = tree_black;
						x_parent->color = tree_red;
						rb_tree_rotate_right(x_parent, root);
						brother = x_parent->left;
					}

					if (brother&&(brother->right == 0 || brother->right->color == tree_black) &&
						(brother->left == 0 || brother->left->color == tree_black)) {
						brother->color = tree_red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else {
						if ((brother->left == 0 || brother->left->color == tree_black)) {
							if (brother->right)
								brother->right->color = tree_black;
							brother->color = tree_red;
							rb_tree_rotate_left(brother, root);
							brother = x_parent->left;
						}
						if (brother)brother->color = x_parent->color;
						x_parent->color = tree_black;
						if (brother->left)brother->left->color = tree_black;
						rb_tree_rotate_right(x_parent, root);
						break;
					}
				}
			}
			if (x)x->color = tree_black;
		}
		return y;
	}

	void rb_tree_rebalance(rb_tree_node_base* x, rb_tree_node_base*& root) {
		x->color = tree_red;
		while (x != root && x->parent->color == tree_red) {
			if (x->parent == x->parent->parent->left) {
				rb_tree_node_base* y = x->parent->parent->right;
				if (y && y->color == tree_red) {
					x->parent->color = tree_black;
					y->color = tree_black;
					x->parent->parent->color = tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->right) {
						x = x->parent;
						rb_tree_rotate_left(x, root);
					}
					x->parent->color = tree_black;
					x->parent->parent->color = tree_red;
					rb_tree_rotate_right(x->parent->parent, root);
				}//������x==root��whileѭ��ʱ ����x==root ����ѭ���ͽ��� ͬʱ��ʾ
				//�Ѿ�����������ߵ�
			}
			else {
				rb_tree_node_base* y = x->parent->parent->left;
				if (y && y->color == tree_red) {
					x->parent->color = tree_black;
					y->color = tree_black;
					x->parent->parent->color = tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->left) {
						x = x->parent;
						rb_tree_rotate_right(x, root);
					}
					x->parent->color = tree_black;
					x->parent->parent->color = tree_red;
					rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}
		root->color = tree_black;
		//�����ǹؼ����� ��д�ᵼ���޷�ƽ�������ֻ�ܲ��������ڵ�
	}
	link_type get_node() {
		return (link_type)Alloc::allocate(sizeof(rb_tree_node));
		//�������Alloc�������Ѿ�����õ��������� ֱ�ӵ��ö��������� 
	}//my_allocator<value_type>
	void put_node(link_type p) {
		Alloc::deallocate(p, sizeof(rb_tree_node));//
	}
	link_type create_node(const value_type& x) {
		link_type tmp = get_node();
		construct(&tmp->value, x);//�쳣û��׼��
		return tmp;
	}
	link_type clone_node(link_type x) {
		link_type tmp = create_node(x->value);
		tmp->color = x->color;
		tmp->left = 0;
		tmp->right = 0;
		return tmp;
	}
	void destory_node(link_type p) {
		destory(&p->value);
		put_node(p);
	}
	link_type _copy(link_type x, link_type p) {//xΪ��¡�ڵ� pΪ���ڵ�
		link_type top = clone_node(x);//�����ʾ����top������ʱ�洢���ƵĽڵ�
		top->parent = p;
		if (x->right)
			top->right = _copy(right(x), top);
		p = top;//top����p �Ǳ�ʾx�ĸ��ڵ�λ��
		x = left(x);

		while (x != 0) {
			link_type y = clone_node(x);
			p->left = y;
			y->parent = p;
			if (x->right)
				y->right = _copy(right(x), y);
			p = y;
			x = left(x);
		}
		return top;//����ֵ�ǵ�ǰ�ݹ麯�������Ƶ�x 
	}
protected:
	size_type node_count;
	link_type header;
	compare key_compare;//�º��� ��ֵ�Ƚ�
	
	
	link_type& root()const {
		return (link_type&)header->parent;
	}
	link_type& leftmost()const {
		return (link_type&)header->left;
	}
	link_type& rightmost()const {
		return (link_type&)header->right;
	}

	static link_type& left(link_type x) {
		return (link_type&)x->left;
	}
	static link_type& right(link_type x) {
		return (link_type&)x->right;
	}
	static link_type& parent(link_type x) {
		return(link_type&)x->parent;
	}
	static const Key& key(link_type x) {
		if (x == 0)return keyofvalue()(value_type(0, 0));
		//map��set�в�� ��Ϊmap�̶���Ҫpair���а�װ
		return keyofvalue()(value(x));
	}
	static reference value(link_type x) {
		return x->value;
	}
	static const color_type& color(link_type x) {
		return (color_type&)x->color;
	}

	static link_type& left(base_ptr x) {
		return (link_type&)x->left;
	}
	static link_type& right(base_ptr x) {
		return (link_type&)x->right;
	}
	static link_type& parent(base_ptr x) {
		return(link_type&)x->parent;
	}
	static const Key& key(base_ptr x) {
		if (x == 0)return keyofvalue()(value_type(0, 0));
		//map��set�в�� ��Ϊmap�̶���Ҫpair���а�װ
		return keyofvalue()(value((link_type)x));
	}
	static reference value(base_ptr x) {
		return ((link_type&)x)->value;
	}
	static const color_type& color(base_ptr x) {
		return (color_type&)x->color;
	}

	static link_type minimum(link_type x) {
		return (link_type)rb_tree_node_base::minimun(x);
	}
	static link_type maximum(link_type x) {
		return (link_type)rb_tree_node_base::maximum(x);
	}
	private:
		void empty_initialize() {
			header = get_node();
			header->color = tree_red;
			root() = 0;
			leftmost() = header;
			rightmost() = header;
		}



protected:
	iterator _insert(base_ptr x,base_ptr y,const value_type& value) {
		link_type newnode = (link_type)x;
		link_type father = (link_type)y;
		link_type z;
		if (father == header || newnode != 0 
			|| key_compare(keyofvalue()(value), key(father))) {
			z=create_node(value);
			left(father) = z;//���뵱ǰif֮�� ����ڵ������ڵ�
			if (father == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost()) {
				leftmost() = z;//���ڵ�Ϊ����ֵ �ͼ�����������
			}
		}
		else {
			z = create_node(value);
			right(father) = z;
			if (rightmost() == father) {
				rightmost() = z;
			}
		}
		parent(z) = father;
		left(z) = 0;
		right(z) = 0;
		rb_tree_rebalance(z, header->parent);//�����ĺ���
		++node_count;	
		return iterator(z);
	}
	
public:
	compare key_comp()const {return key_compare;}
	
	iterator begin() {return iterator(leftmost());}
	const_iterator begin()const {return iterator(leftmost());}
	const_iterator cbegin()const {return iterator(leftmost());}
	reverse_iterator rbegin() {return reverse_iterator(end());}//���������
	const_reverse_iterator crbegin(){ return reverse_iterator(end()); }
	iterator end() {return header;}
	const_iterator end()const {return header;}
	const_iterator cend()const { return header; }
	reverse_iterator rend()const { return reverse_iterator(begin()); }
	const_reverse_iterator crend()const { return reverse_iterator(begin()); }


	bool empty()const {return node_count == 0;}
	size_type size()const {return node_count;}
	size_type max_size()const {return size_type(-1);}
	void swap(rb_tree& x) {
		std::swap(header, x.header);
		std::swap(node_count, x.node_count);
		std::swap(key_compare, x.key_compare);
	}

public://����Ϊequal��unique����
	iterator insert_equal(const value_type& v) {
		link_type y = header;
		link_type x = root();
		while (x != 0) {
			y = x;
			x = key_compare(keyofvalue()(v), key(x)) ? left(x) : right(x);
		}
		return _insert(x, y, v);//xΪ����λ�� yΪ����λ�õĸ��ڵ�
	}
	iterator insert_equal(iterator position, const value_type& x) {
		if (position.node == header->left) {//begin()
			if (size()>0 && key_compare((keyofvalue()(x)), key(position.node))) {
				return _insert(position.node, position.node, x);
			}
			else
				return insert_equal(x);//Ҫȡ��Ԫ��λ�� 
		}//end()
		else if (position.node == header) {
			if (!key_compare(keyofvalue()(x), key(rightmost()))) {
				return _insert(0, rightmost(), x);
			}
			else {
				return insert_equal(x);
			}
		}
		else {
			iterator before = position;
			--before;
			if (!key_compare(keyofvalue()(x), key(before.node))
				&& !key_compare(key(position.node), keyofvalue()(x))) {
				if (right(before.node) == 0) {
					return _insert(0, before.node, x);
				}
				else
					return _insert(position.node, position.node, x);
			}
			else
				return insert_equal(x);
		}
		
	}
	void insert_equal(const value_type* first, const value_type* last) {
		iterator one = first;
		for (; one != last; ++one) {
			insert_equal(*one);
		}
	}
	void insert_equal(const_iterator first, const_iterator last) {
		iterator one = first;
		for (; one != last; ++one) {
			insert_equal(*one);
		}
	}
	void insert_equal(std::initializer_list<value_type>& x) {
		using i_iterator = typename std::initializer_list<value_type>::iterator;
		i_iterator one = x.begin();
		for (; one != x.end(); ++one) {
			insert_equal(*one);
		}
	}



	std::pair<iterator,bool> insert_unique(const value_type& v) {
		link_type y = header;
		link_type x = root();
		bool comp = true;
		while (x != 0) {
			y = x;
			comp = key_compare(keyofvalue()(v),key(x));
			x = comp? left(x) : right(x);
		}
		iterator father = iterator(y);
		if (comp)
			if (father== begin())//���뵽����
				return std::pair<iterator, bool>(_insert(x, y, v), true);
			else
				--father;
		if (key_compare(key(father.node), keyofvalue()(v)))
		{
			return std::pair<iterator, bool>(_insert(x, y, v), true);
		}
		return std::pair<iterator, bool>(father, false);

	}
	iterator insert_unique(iterator position, const value_type& x) {
		if (position.node == header->left) {//begin()
			if (size()>0&& key_compare((keyofvalue()(x)), key(position.node))) {
				return _insert(position.node, position.node, x);
			}
			else
				return insert_unique(x).first;//Ҫȡ��Ԫ��λ��
		}//end()
		else if (position.node == header) {
			if (key_compare((key(rightmost())), keyofvalue()(x))) {
				return _insert(0, rightmost(), x);
			}
			else {
				return insert_unique(x).first;
			}
		}
		else {
			iterator before = position;
			--before;
			if (key_compare(key(before.node), keyofvalue()(x))
				&& key_compare(keyofvalue()(x), key(position.node))) {
				if (right(before.node) == 0) {
					return _insert(0, before.node, x);
				}
				else
					return _insert(position.node, position.node, x);
			}
			else
				return insert_unique(x).first;
		}
	}
	void insert_unique(value_type* first,value_type* last) {
		value_type* one = first;
		for (; one != last; ++one) {
			insert_unique(*one);
		}
	}

	void insert_unique(const_iterator first, const_iterator last) {
		iterator one = first;
		for (; one != last; ++one) {
			insert_equal(*one);
		}
	}
	void insert_unique(std::initializer_list<value_type>& x) {
		using i_iterator = typename std::initializer_list<value_type>::iterator;
		i_iterator one = x.begin();
		for (; one != x.end(); ++one) {
			insert_unique(*one);
		}
	}
public://����Ϊerase
	void erase(iterator position) {
		link_type y = (link_type)rb_tree_rebalance_for_erase(
			position.node,
			header->parent,//root
			header->left,
			header->right
		);
		destory_node(y);
		--node_count;
	}
	void erase(iterator first, iterator last) {
		if (first == begin() && last == end())
			clear();
		else
			while (first != last) {
				erase(first);
				++first;
			}
	}

	size_type erase(const Key& x) {
		std::pair<iterator, iterator> rang = equal_range(x);
		size_type n = 0;
		n=std::distance(rang.first, rang.second);//����������
		erase(rang.first, rang.second);
		return n;
	}

	void erase(const Key* first, const Key* last) {
		Key* one = first;
		while (one != last) {
			erase(*one);
			++one;
		}
	}
	void _erase(link_type x) {
		//iterator first=leftmost();
		//iterator last=rightmost();
		while (x!=0) {
			_erase(right(x));
			link_type y = left(x);
			destory_node(x);
			x = y;
		}
	}
	void clear() {
		if (node_count != 0) {
			_erase(root());
			leftmost() = header; 
			root() = 0;
			rightmost() = header;
			node_count = 0;
		}

	}
public:
	rb_tree(const allocator_type& alloc = allocator_type())
		:node_count(0),key_compare() {
		empty_initialize();
	}
	rb_tree(const compare& x, const allocator_type& alloc = allocator_type()) 
		:node_count(0),key_compare(x) {
		empty_initialize();
	}
	rb_tree(const rb_tree& x):node_count(0),key_compare() {
		if (x.root() == 0)
			empty_initialize();
		else
		{
			color(header) == tree_red;
			root() = _copy(x.root(), x.header);
			leftmost() = minimum(root());
			rightmost() = maximum(root());
		}
		node_count = x.node_count;
	}
	rb_tree(rb_tree&& x) :node_count(0), key_compare() {
		if (x.root() == 0)
			empty_initialize();
		else {
		root() = x.root();
		leftmost() = minimum(root());
		rightmost() = maximum(root());
		x.empty_initialize();
		}
		node_count = x.node_count;
	}

	rb_tree& operator=(const rb_tree& x) {
		if(x!=*this){
		clear();
		key_compare = x.key_compare;
		if (x.root() == 0)
			empty_initialize();
		else
		{
			color(header) == tree_red;
			root() = _copy(x.root(), x.header);
			leftmost() = minimum(root());
			rightmost() = maximum(root());
		}
		node_count = x.node_count;
		return *this;
		}
	}



	~rb_tree() { clear(); }


public:
	iterator find(const key_type& k) {
		link_type y = header;
		link_type x = root();
		while (x != 0)
		{
			if (!key_compare(key(x), k))//x>=k
				y = x, x = left(x);
			else
				x = right(x);
		}
		iterator current = iterator(y);//k<current 
		return (current == end() || key_compare(k, key(current.node))) ? end() : current;
	}
	const_iterator find(const key_type& k) const {
		link_type y = header;
		link_type x = root();
		while (x != 0)
		{
			if (!key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);
		}
		const_iterator current = const_iterator(y);
		return (current == end() || key_compare(k, key(current.node))) ? end() : current;
	}

	iterator lower_bound(const Key& k) {
		link_type y = header;
		link_type x = root();
		while (x != 0)//�������while��ѭ����ʾ��Ŀǰ�Ѿ��ǵ�����Ҷ�ӽڵ����Ҷ�ڵ�ĸ��ڵ�
		{
			if (!key_compare(key(x), k))//x>=k 
				y = x, x = left(x);
			else
				x = right(x);
		}
		return iterator(y);//���ؿ��Բ����λ�� һ�����ڵ���k��ֵ
	}
	iterator upper_bound(const Key& k) {
		link_type y = header;
		link_type x = root();
		while (x != 0)
			if (key_compare(k, key(x)))//x>k
				y = x, x = left(x);
			else
				x = right(x);
		return iterator(y);
	}

	std::pair<iterator, iterator> equal_range(const Key& k) {
		return std::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
		//x>=k------x>k ����һ�����ڻ��ߵ���k��ֵ�����һ������k��ֵ ����Χ��
	}

	size_type count(const Key& k) const {
		std::pair<iterator, iterator> rang = equal_range(k);
		size_type n = std::distance(rang.first, rang.second);
		return n;
	}
public:
	bool operator==(const rb_tree& x) {
		return (size() == x.size() && std::equal(begin(), end(), x.begin()));
	}
	
	bool operator!=(const rb_tree& x) {
		return !(operator==(*this, x));
	}
	bool operator<(const rb_tree& x) {
		return std::lexicographical_compare(
			begin(), end(),x.begin(), x.end() );
	}
	bool operator>(const rb_tree& x) {
		return operator<(*this, x);
	}
	bool operator>=(const rb_tree& x) {
		return !(operator<(*this,x));
	}
	bool operator<=(const rb_tree& x) {
		return !(operator<(x, *this));
	}
};



template<class Key, class value, class keyofvalue,
	class compare , class alloc = my_allocator<value >>
	inline bool operator==(const rb_tree<Key, value, keyofvalue, compare, alloc>& x,
		const rb_tree<Key, value, keyofvalue, compare, alloc>& y) {
	return (x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin()));
}
template<class Key, class value, class keyofvalue,
	class compare , class alloc = my_allocator<value >>
	inline bool operator!=(const rb_tree<Key, value, keyofvalue, compare, alloc>& x,
	const rb_tree<Key, value, keyofvalue, compare, alloc>& y){
	return !(operator==(x, y));
}

template<class Key, class value, class keyofvalue,
	class compare , class alloc = my_allocator<value >>
	inline bool operator<(const rb_tree<Key, value, keyofvalue, compare, alloc>& x,
		const rb_tree<Key, value, keyofvalue, compare, alloc>& y) {
	return std::lexicographical_compare(x.begin(), x.end(),
										y.begin(), y.end());
}
template<class Key, class value, class keyofvalue,
	class compare, class alloc = my_allocator<value >>
	inline bool operator>(const rb_tree<Key, value, keyofvalue, compare, alloc>& x,
		const rb_tree<Key, value, keyofvalue, compare, alloc>& y) {
	return operator<(y, x);
}
template<class Key, class value, class keyofvalue,
	class compare, class alloc = my_allocator<value >>
	inline bool operator<=(const rb_tree<Key, value, keyofvalue, compare, alloc>& x,
		const rb_tree<Key, value, keyofvalue, compare, alloc>& y) {
	return !(operator<(y, x));
}
template<class Key, class value, class keyofvalue,
	class compare, class alloc = my_allocator<value >>
	inline bool operator>=(const rb_tree<Key, value, keyofvalue, compare, alloc>& x,
		const rb_tree<Key, value, keyofvalue, compare, alloc>& y) {
	return !(operator<(x, y));
}




#endif