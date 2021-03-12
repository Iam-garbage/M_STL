#ifndef LIST
#define LIST
#include"my_allocator.h"
#include"mytype_traits.h"
#include"my_iterator.h"
#include<iostream>
template<class T>
struct _list_node {
	using void_pointer = _list_node<T>*;
	void_pointer prev;
	void_pointer next;
	T data;
};

template<class T,class ref,class ptr>
struct _list_iterator {
	using self = _list_iterator<T, ref, ptr>;
	using iterator = _list_iterator<T, T&, T*>;
	using iterator_category = bidirectional_iterator_tag;
	using value_type = T;
	using pointer = ptr;
	using reference = ref;
	using difference_type = ptrdiff_t;
	using size_type = size_t;
	using link_type = _list_node<T>*;//节点的指针

	link_type node;

	_list_iterator(link_type x):node(x){}
	_list_iterator() :node((link_type)0) {}
	_list_iterator(const iterator& x):node(x.node){}

	bool operator==(const self& x)const {
		return node == x.node;
	}
	bool operator!=(const self& x)const {
		return !(operator==(x));
	}
	reference operator*()const {
		return node->data;
	}
	pointer operator->()const {
		return &(operator*());
	}
	self& operator++() {
		node = (link_type)node->next;
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		++(*this);
		return tmp;
	}
	self& operator--() {
		node = (link_type)node->prev;
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--(*this);
		return tmp;
	}
};

template<class T,class ref,class ptr>
inline bidirectional_iterator_tag
iterator_category(const _list_iterator<T, ref, ptr>&) {
	return bidirectional_iterator_tag();
}

template<class T, class ref, class ptr>
inline T* 
value_type(const _list_iterator<T, ref, ptr>&) {
	return (T*)(0);
}

template<class T, class ref, class ptr>
inline ptrdiff_t*
distance_type(const _list_iterator<T, ref, ptr>&) {
	return ((ptrdiff_t * )0);
}

template<class T, class Alloc = my_allocator<T>>
class list {
protected:
	using list_node              = _list_node<T>;//节点
	using link_type              = list_node*;//节点指针
	using list_node_allocator    = my_allocator<list_node>;//产生link_type 
	using allocator_type         = Alloc;//my_allocator<T>

public:
	using value_type             = T;
	using reference              = value_type&;
	using const_reference        = const reference;
	using pointer                = typename allocator_type::pointer;//T*
	using const_pointer          = const pointer;
	using iterator               = _list_iterator<T,T&,T*>;//link_type和iterator之间有构造函数
	using const_iterator         = const iterator;
	using reverse_iterator       = reverse_iterator<iterator>;
	using const_reverse_iterator = const reverse_iterator;
	using difference_type        = typename iterator_traits<iterator>::difference_type;
	using size_type              = size_t;
	
	allocator_type get_allocator()const {
		return allocator_type();
	}

protected:

	link_type node;//头指针 指向超尾节点 同时表达成头节点 实习前闭后开区间



	link_type get_node() {
		return (link_type)list_node_allocator::allocate(sizeof(list_node));
	}
	void put_node(link_type p) {
		list_node_allocator::deallocate(p);
	}

	link_type create_node( const T& x=T()) {
		link_type p = get_node();
		construct(&(p->data), x);
		return p;
	}
	link_type create_node(T&& x) {
		//emmmmm 虽然使用了移动语义但是construct没有定义移动语义
		link_type p = get_node();
		//construct(&p->data, std::move(x)); 不使用construct 直接用移动赋值语义
		p->data = std::move(x);
		return p;
	}
	void destory_node(link_type p) {
		destory(&p->data);
		put_node(p);
	}

	void empty_initialize() {
		node = get_node();
		node->next = node;
		node->prev = node;
	}
public:
	iterator begin()noexcept { return (iterator)node->next; }
	const_iterator begin()const noexcept { return (const_iterator)node->next; }
	const_iterator cbegin() const noexcept{ return (const_iterator)node->next; }
	iterator end() noexcept { return (iterator)node; }
	const_iterator end()const noexcept { return (const_iterator)node; }
	const_iterator cend()const noexcept { return (const_iterator)node; }
	reverse_iterator rbegin()noexcept {return (reverse_iterator)node->next;}
	const_reverse_iterator rbegin()const noexcept { return (reverse_iterator)node->next; }
	const_reverse_iterator crbegin()const noexcept{ return (reverse_iterator)node->next; }
	reverse_iterator rend()  { return (reverse_iterator)node; }
	const_reverse_iterator rend() const { return (reverse_iterator)node; }
	const_reverse_iterator crend()const { return (reverse_iterator)node; }
	reference front() {
		return begin().node->data;
	}
	const_reference front()const {
		return (*begin());
	}
	reference back() {
		return *(--end());
	}
	const_reference back()const {
		return *(--end());
	}
	void clear()noexcept {
		for (iterator cur = begin(); cur != end(); ++cur)
			destory_node(cur);
	}

	explicit list() {
		empty_initialize();
	}
	explicit list(size_type n) {
		empty_initialize();
		insert(begin(), n, value_type());
	}
	list(size_type n, const value_type& val)  {
		empty_initialize();
		insert(begin(), n, val);
	}
	list(iterator first, iterator last)  {
		empty_initialize();
		insert(begin(), first, last);
	}
	list(const list& x) {
		empty_initialize();
		insert(cbegin(), x.begin(), x.end());
	}
	list(list&& x) {
		empty_initialize();
		for (iterator cur = x.begin(); cur != x.end(); ++cur) {
			insert(begin(), std::move(*cur));
		}
	}
	list(std::initializer_list<value_type>& x)  {
		empty_initialize();
		insert(begin(), x);
	}
	~list() {
		for (iterator cur = begin(); cur != end(); ++cur)
			destory_node(cur.node);
	}
	list& operator= (const list& x)  {
		empty_initialize();
		insert(cbegin(), x.cbegin(), x.cend());
	}
	list& operator= (list&& x) {
		empty_initialize();
		for (iterator cur = x.begin(); cur != x.end(); ++cur) {
			insert(begin(), std::move(*cur));
		}
	}
	list& operator= (std::initializer_list<value_type> il) {
		empty_initialize();
		insert(begin(), il);
	}
public:
	iterator insert(const_iterator position, const value_type& val) {
		link_type tmp = create_node(val);//类型直接设置为link_type方便操作
		tmp->next = position.node;       //iterator tmp.node->next太麻烦
		tmp->prev = position.node->prev;
		(link_type(position.node->prev))->next = tmp;
		position.node->prev = tmp;
		return iterator(tmp);
	}
	iterator insert(const_iterator position, size_type n, const value_type val = value_type()) {
		iterator tmp = insert(position, val);
		for (--n; n>0; --n) {
			link_type cur = create_node(val);
			cur->next = position.node;
			cur->prev = position.node->prev;
			(link_type(position.node->prev))->next = cur;
			position.node->prev = cur;
		}
		return tmp;
	}
	
	iterator insert(const_iterator position,
		iterator first, iterator last) {
		iterator tmp = insert(position, *first);
		iterator cur = ++first;
		for (; cur !=last ; ++cur) {
			link_type next = create_node(cur.node->data);
			next->next = position.node;
			next->prev = position.node->prev;
			(link_type(position.node->prev))->next = next;
			position.node->prev = next;
		
		}
		return tmp;
	}
	iterator insert(const_iterator position, value_type&& val) {
		link_type tmp = create_node(std::move(val));//类型直接设置为link_type方便操作
		tmp->next = position.node;       //iterator tmp.node->next太麻烦
		tmp->prev = position.node->prev;
		(link_type(position.node->prev))->next = tmp;
		position.node->prev = tmp;
		return iterator(tmp);
	}
	iterator insert(const_iterator position, std::initializer_list<value_type> il) {
		using in_iterator = typename std::initializer_list<value_type>::iterator;
		
		in_iterator cur = il.begin();
		iterator re = insert(position, *cur);
		in_iterator next = ++cur;
		for (; next != il.end(); ++next) {
			insert(position, *next);
		}
		return re;
	}
	iterator erase(const_iterator position) {
		link_type next_node = (link_type)position.node->next;
		link_type prev_node = (link_type)position.node->prev;
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destory_node(position.node);
		return iterator(next_node);
	}
	iterator erase(const_iterator first, const_iterator last) {
		iterator cur = first;
		for (; cur != last.node;) {
			cur=erase(cur);
		}
		return last;
	}
	protected:
		void transfer(iterator position, iterator first, iterator last) {
			if (position != last) {
				last.node->prev->next = position.node;
				first.node->prev->next = last.node;
				position.node->prev->next = first.node;
				link_type tmp = position.node->prev;
				position.node->prev = last.node->prev;
				last.node->prev = first.node->prev;
				first.node->prev = tmp;
			}
		}
public:
	bool empty() const noexcept {
		return node->next==node;
	}
	size_type size()const noexcept {
		size_type n=0;
		iterator cur = begin();
		for (; cur != end(); ++cur) { ++n; }
		return n;
	}
	size_type max_size() const noexcept {
		return size_type(-1);
	}

	template <class InputIterator>
	void assign(InputIterator first, InputIterator last);
	void assign(size_type n, const value_type& val) {
		iterator cur = begin();
		for (; cur != end() && n > 0; ++cur, --n) {
			*cur = val;
		}
		if (n > 0) {
			insert(end(), n, val);
		}
		else {
			erase(cur, end());
		}
	}
	void assign(std::initializer_list<value_type> il) {
		assign(il.begin(), il.end());
	}

	void push_front(const value_type& val) {
		insert(begin(), val);
	}
	void push_front(value_type&& val) {
		insert(begin(), std::move(val));
	}
	void pop_front() {
		erase(begin());
	}
	void push_back(const value_type& val) {
		insert(end(), val);
	}
	void push_back(value_type&& val) {
		insert(end(), std::move(val));
	}
	void pop_back() {
		iterator tmp = end();
		erase(--tmp);
	}
	void swap(list& x) {
		link_type tmp = x.node;
		x.node = node;
		node = tmp;
	}

	void resize(size_type n, value_type val = value_type()) {
		if (n > size()) {
			insert(end(), (n - size()), val);
		}
		else {
			size_type x = size();
			for (; n != x; --x) {
				pop_back();
			}
		}
	}
	void splice(const_iterator position, list& x) {
		transfer(position, x.begin(), x.end());
	}
	void splice(const_iterator position, list& x, const_iterator i) {
		iterator e = i;
		transfer(position, i, ++e);
	}
	void splice(const_iterator position, list& x,
		const_iterator first, const_iterator last) {
		transfer(position, first, last);
	}
	void remove(const value_type& val) {
		iterator cur = begin();
		for (; cur != end(); ++cur) {
			if (*cur == val)
				erase(cur);
		}
	}
	template <class Predicate>
	void remove_if(Predicate pred);
	
	void unique() {
		link_type cur = begin();
		link_type next = ++cur;
		for (; cur != end().node->prev; ++cur, ++next) {
			if (*cur == *next) {
				link_type cp = next++;
				erase(cp);
			}
		}
	}
	template <class BinaryPredicate>
	void unique(BinaryPredicate binary_pred);

	void merge(list& __x) {
		iterator __first1 = begin();
		iterator __last1 = end();
		iterator __first2 = __x.begin();
		iterator __last2 = __x.end();
		while (__first1 != __last1 && __first2 != __last2)
			if (*__first2 < *__first1) {
				iterator __next = __first2;
				transfer(__first1, __first2, ++__next);
				__first2 = __next;
			}
			else
				++__first1;
		if (__first2 != __last2) transfer(__last1, __first2, __last2);
	}
	template <class Compare>
	void merge(list& x, Compare comp);

	void sort() {
		iterator cur = begin();
		iterator next = ++cur;
		for (; cur != end(); ++cur) {
			value_type low = *cur;
			next = ++cur;
			for (; next != end(); ++next) {
				if (low > *next)
					low = *next;
			}
			*cur = low;
		}
	}
	template <class Compare>
	void sort(Compare comp);

	void reverse()noexcept {
		reverse_iterator(begin());
	}

};






template<class T, class alloc>
template <class InputIterator>
void list<T, alloc>::assign(InputIterator first, InputIterator last) {
	InputIterator cur = first;
	iterator i = begin();
	for (; i != end() && cur != last; ++cur, ++i) {
		*i = *first;
	}
	if (cur != last) {
		insert(end(), cur, last);
	}
	else {
		erase(i, end());
	}
}

template<class T, class alloc>
template <class Predicate>
void list<T, alloc>::remove_if(Predicate pred) {
	iterator cur = begin();
	for (; cur != end(); ++cur) {
		if (pred(*cur))
			erase(cur);
	}
}

template<class T, class alloc>
template <class BinaryPredicate>
void list<T, alloc>::unique(BinaryPredicate binary_pred) {
	link_type cur = begin();
	link_type next = ++cur;
	for (; cur != end().node->prev; ++cur, ++next) {
		if (binary_pred(*cur,*next)) {
			link_type cp = next++;
			erase(cp);
		}
	}
}

template<class T, class alloc>
template <class Compare>
void list<T, alloc>::merge(list& __x, Compare comp) {
	iterator __first1 = begin();
	iterator __last1 = end();
	iterator __first2 = __x.begin();
	iterator __last2 = __x.end();
	while (__first1 != __last1 && __first2 != __last2)
		if (comp(*__first2,*__first1)) {
			iterator __next = __first2;
			transfer(__first1, __first2, ++__next);
			__first2 = __next;
		}
		else
			++__first1;
	if (__first2 != __last2) transfer(__last1, __first2, __last2);
}


template<class T, class alloc>
template <class Compare>
void list<T,alloc>::sort(Compare comp) {
	iterator cur = begin();
	iterator next = ++cur;
	for (; cur != end(); ++cur) {
		value_type index = *cur;
		next = ++cur;
		for (; next != end(); ++next) {
			if (comp(*next,index))
				index = *next;
		}
		*cur = index;
	}
}


template <class T, class Alloc>
void swap(list<T, Alloc>& x, list<T, Alloc>& y) {
	x.swap(y);
}


template <class T, class Alloc>
bool operator== (const list<T, Alloc>& x, const list<T, Alloc>& y) {
	using iterator = typename list<T, Alloc>::iterator;
	iterator last1(x.end());
	iterator first1 = x.begin();
	iterator last2(y.end());
	iterator first2 = y.begin();
	for (; first1 != last1 && first2 != last2;
		++first1,++first2) {
		if (*first1 != *first2)
			return false;
	}
	return first1 == x.end() && first2 == y.end();
}
template <class T, class Alloc>
bool operator!= (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
	return operator==(lhs, rhs);
}

template <class T, class Alloc>
bool operator<  (const list<T, Alloc>& x, const list<T, Alloc>& y) {
	return std::lexicographical_compare(x.begin(), x.end(),
										y.begin(), y.end());
}

template <class T, class Alloc>
bool operator<= (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
	return !(operator < (rhs, lhs));
}

template <class T, class Alloc>
bool operator>  (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
	return (operator < (rhs, lhs));
}
template <class T, class Alloc>
bool operator>= (const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) {
	return !(operator < (lhs, rhs));
}





#endif