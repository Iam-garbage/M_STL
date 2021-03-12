#ifndef  MY_FORWARD_LIST
#define MY_FORWARD_LIST
#include"my_allocator.h"
#include"mytype_traits.h"
#include"my_iterator.h"
#include<iostream>//已无误
//修改时间:2020/11/27/18:22
struct flist_node_base {
	flist_node_base* next;
};
template<class T>
struct flist_node:public flist_node_base {
	T data;
};

inline flist_node_base* flist_insert(flist_node_base* cur, flist_node_base* newnode) {
	newnode->next = cur->next;
	cur->next = newnode;
	return newnode;

}

inline size_t flist_size(flist_node_base* begin) {
	size_t n = 0;
	for (; begin != 0; begin->next) {
		++n;
	}
	return n;
}

struct flist_iterator_base {
	using size_type           = size_t;
	using iterator_category   = forward_iterator_tag;
	using difference_type     = ptrdiff_t;

	flist_node_base* node;
	flist_iterator_base(flist_node_base* x):node(x){}
	
	void inc() { node = node->next; }

	bool operator==(const flist_iterator_base& x) {
		return x.node == node;
	}

	bool operator!=(const flist_iterator_base& x) {
		return x.node != node;
	}
};

template<class T,class ref,class ptr>
struct flist_iterator :public flist_iterator_base{
	using value_type                  = T;
	using pointer                     = ptr;
	using const_pointer               = const pointer;
	using reference                   = ref;
	using conset_reference            = const reference;
	using list_node                   = flist_node<T>;
	using iterator                    = flist_iterator<T, T&, T*>;
	using const_iterator              = const iterator;
	using self						  = flist_iterator<T, ref, ptr>;

	flist_iterator(list_node* x):flist_iterator_base(x){}
	flist_iterator():flist_iterator_base(0){}
	flist_iterator(const flist_iterator& x):flist_iterator_base(x.node){}

	reference operator*()const {
		return ((list_node*)node)->data;
	}
	pointer operator->()const {
		return &(operator*());
	}

	self& operator++() {
		inc();
		return *this;
	}

	self operator++(int) {
		self tmp = *this;
		inc();
		return tmp;
	}
};

template<class T,class Alloc=my_allocator<T>>
class forward_list {
public:
	using value_type             = T;
	using allocator_type         = Alloc;
	using reference              = T&;
	using const_reference        = const reference;
	using pointer                = T*;
	using const_pointer          = const pointer;
	using iterator               = flist_iterator<T,T&,T*>;
	using const_iterator		 = const iterator;
	using size_type				 = size_t;
	using difference_type		 = ptrdiff_t;

protected:
	using list_node				= flist_node<T>;
	using list_node_base		= flist_node_base;
	using iterator_base			= flist_iterator_base;
	using list_node_allocator   = my_allocator<list_node>;
	list_node_base head;//头节点 只有指针部分
	static list_node* create_node(const value_type& x=value_type()) {
		
		list_node* node = (list_node*)list_node_allocator::allocate(sizeof(list_node));
		construct(&(node->data), x);
		node->next = nullptr;
		return node;
	}
	static list_node* create_node(value_type&& x) {//此处针对右值
		list_node* node = list_node_allocator::allocate(sizeof(list_node)); //list_node_allocator::allocate();
		construct(&(node->data), std::move(x));
		node->next = nullptr;
		return node;
	}

	static void destory_node(list_node* p){
		destory(&(p->data));
		list_node_allocator::deallocate(p);
	}
public:
	allocator_type get_allocator() const{
		return allocator_type();
	}
	iterator insert_after(iterator position, const value_type& val) {
		list_node* cur=create_node(val);
		cur->next = position.node->next;// <-------
		//不用特意转型list_node* 因为它本身只是针对
		//基类指针进行操作 数据部分并没有干扰 
		//除非出现转型或者说访问数据才需要强制类型转换
		position.node->next = cur;
		return iterator(cur);

	}
	
	iterator insert_after(iterator position, value_type&& val) {
			list_node* cur = create_node(std::move(val));
			cur->next = position.node->next;
			position.node->next = cur;
			return iterator(cur);
		
	}
	iterator insert_after(iterator position, size_type n, const value_type& val) {
		list_node* ret = (list_node*)position.node;
		for (size_type i = 0; i < n; ++i)
			ret = (list_node*)flist_insert(ret, create_node(val));
		return (iterator)((list_node*)position.node->next);
		/*跟上面一样效果
		iterator newposition = position;
		size_type i = 0;
		while (i < n) {
			position = insert_after(position, val);
			++i;
		}
		return (list_node*)newposition.node->next;
		*/
	}

	iterator insert_after(iterator position,
		iterator first, iterator last) {
		iterator newposition = position;
		while (first != last) {
			position = (list_node*)flist_insert(position, create_node(*first));
			++first;
		}
		return (iterator)((list_node*)newposition.node->next);
	}
	iterator insert_after(iterator position,
		value_type* first, value_type* last) {
		iterator newposition = position;
		while (first != last) {
			position = (list_node*)flist_insert(position, create_node(*first));
			++first;
		}
		return (iterator)((list_node*)newposition.node->next);
	}
	
	iterator insert_after(iterator position, std::initializer_list<value_type>il) {
		return insert_after(position, il.begin(), il.end());
	}


	iterator erase_after(iterator position) {
		iterator tmp = iterator((list_node*)position.node->next);
		position.node->next = tmp.node->next;
		destory_node((list_node*)tmp.node);
		return iterator((list_node*)position.node->next);
	}
	iterator erase_after(iterator position, const_iterator last) {
		iterator cur = iterator((list_node*)position.node->next);
		while (cur != last) {
			iterator tmp = cur;
			cur = (list_node*)cur.node->next;
			destory_node((list_node*)&cur.node);
		}
		position.node->next = last.node;
		return last;
	}


public:
	iterator before_begin() noexcept {
		return iterator(((list_node*)&head));
	}
	const_iterator before_begin()const noexcept {
		return iterator(((list_node*)&head));
	}
	const_iterator cbefore_begin() const noexcept {
		return iterator(((list_node*)&head));
	}
	iterator begin() noexcept {
		return iterator((list_node*)head.next);
	}
	const_iterator begin() const noexcept {
		return iterator((list_node*)head.next);
	}
	const_iterator cbegin() const noexcept {
		return iterator((list_node*)head.next);
	}
	iterator end() noexcept {
		return iterator(0);
	}
	const_iterator end() const noexcept {
		return iterator(0);
	}
	const_iterator cend() const noexcept {
		return iterator(0);
	}
	
	explicit forward_list(const allocator_type& alloc = allocator_type()) {
		head.next = nullptr;
	}
	explicit forward_list(size_type n) {
		head.next = nullptr;
		insert_after(before_begin(), n, value_type());
	}
	explicit forward_list(size_type n, const value_type& val,
		const allocator_type& alloc = allocator_type()) {
		head.next = nullptr;
		insert_after(before_begin(), n, val);
	}

	forward_list(iterator first, iterator last,
		const allocator_type& alloc = allocator_type()) {
		head.next = nullptr;
		insert_after(before_begin(), first, last);
	}
	forward_list(const forward_list& cp, const allocator_type& alloc) {
		head.next = nullptr;
		insert_after(before_begin(), cp.begin(), cp.end);
	}
	forward_list(forward_list&& mo, const allocator_type& alloc) {
		head.next = nullptr;
		iterator first = mo.begin();
		iterator cur = before_begin();
		for (; first != mo.end(); ++first) {
			cur=insert_after(cur, std::move(*first));
		}
	}
	forward_list(std::initializer_list<value_type> il,
		const allocator_type& alloc = allocator_type()) {
		head.next = nullptr;
		insert_after(before_begin(), il.begin(), il.end());
	}
	~forward_list() {
		erase_after(before_begin(), end());
	}

	forward_list& operator= (const forward_list& cp) {
		erase_after(before_begin(), end());
		insert_after(before_begin(), cp.begin(), cp.end());
		return *this;
	}
	forward_list& operator= (forward_list&& mo) {
		erase_after(before_begin(), end());
		iterator first = mo.begin();
		iterator cur = before_begin();
		for (; first != mo.end(); ++first) {
			cur = insert_after(cur, std::move(*first));
		}
		return *this;
	}
	forward_list& operator= (std::initializer_list<value_type> il) {
		erase_after(before_begin(), end());
		insert_after(before_begin(), il.begin(), il.end());
		return *this;
	}

public:
	bool empty() const noexcept {
		return head.next== 0;
	}
	size_type max_size() const noexcept {
		return size_type(-1);
	}
	reference front() {
		return ((list_node*)head.next)->data;
	}
	const_reference front()const {
		return ((list_node*)head.next)->data;
	}
	void push_front(const value_type& val) {
		insert_after(before_begin(),val);
	}
	void push_front(value_type&& val) {
		insert_after(before_begin(), std::move(val));
	}
	void pop_front() {
		if (!empty()) {
			erase_after(before_begin());
		}
	}

	void clear() noexcept {
		erase_after(before_begin(), end());
	}

	void resize(size_type n, value_type& val = value_type()) {
		size_type x = flist_size(head.next);
		if (n < x) {
			iterator cur = begin();
			for (int cum=0;cum<n; ++cum) {
				++cur;
			}
			erase_after(cur, end());
		}
		else {
			iterator cur = begin();
			for (int cum = 0; cum < x; ++cum) {
				++cur;
			}
			insert_after(cur, x - n, val);
		}
	}

	void swap(forward_list& x) {
		list_node_base tmp = head;
		head = x;
		x = tmp;
	}
public:
	template <class InputIterator>
	void assign(InputIterator first, InputIterator last) {
		erase_after(before_begin(), end());
		insert_after(before_begin(), first, last);
	}

	void assign(size_type n, const value_type& val) {
		erase_after(before_begin(), end());
		insert_after(before_begin(), n, val);
	}
	void assign(std::initializer_list<value_type> il) {
		erase_after(before_begin(), end());
		insert_after(before_begin(), il.begin(), il.end());
	}
public:
	void splice_after(const_iterator position, forward_list& x){
		insert_after(position, x.begin(), x.end());//[begin,end)
	}
	void splice_after(const_iterator position, forward_list& x, const_iterator i) {
		iterator x_prev = x.begin();//这里是相当于正常插入 
		iterator x_cur = x_prev.node->next;//将i插入到position前面位置
		while (x_cur != i) {       
			++x_prev;
			x_cur = x_prev.node->next;
		}
		iterator my_prev = begin();
		iterator my_cur = my_prev.node->next;
		while (my_cur != position) {
			++my_prev;
			my_cur = my_prev.node->next;
		}


		my_prev.node->next = i.node;
		x_prev.node->next = i.node->next;
		i.node->next = position.node;
	}
	void splice_after(const_iterator position, forward_list& x,
		const_iterator first, const_iterator last) {
		iterator next = ++first;//(first,last)
		insert_after(position, next, last);//[next,last)
	}

	void remove(const value_type& val) {
		iterator prev = before_begin();
		iterator next = prev.node->next;
		while(next !=end()){
			if (*next == val) {
				next=erase_after(prev);
			}
			else {
				++prev;
				next = prev.node->next;
			}
		}
	}

	template <class Predicate>
	void remove_if(Predicate pred) {
		iterator prev = before_begin();
		iterator next = prev.node->next;
		while (next != end()) {
			if (pred(*next)) {
				next = erase_after(prev);
			}
			else {
				++prev;
				next = prev.node->next;
			}
		}
	}

	void unique() {
		iterator prev = before_begin();
		iterator next = prev.node->next;
		while (next != end()) {
			if (*prev==*next) {
				next = erase_after(prev);
			}
			else {
				++prev;
				next = prev.node->next;
			}
		}
	}
	template <class BinaryPredicate>
	void unique(BinaryPredicate binary_pred) {
		iterator prev = before_begin();
		iterator next = prev.node->next;
		while (next != end()) {
			if (binary_pred(*prev,*next)) {
				next = erase_after(prev);
			}
			else {
				++prev;
				next = prev.node->next;
			}
		}
	}

	void merge(forward_list& x) {
		iterator my_prev = before_begin();
		iterator my_cur = my_prev.node->next;
		//这里多写一个my_prev是为了记录if需要进行后续转移时的转移位置
		
		iterator x_prev = x.before_begin();
		iterator x_cur = x_prev.node->next;
		//被调用方需要一个x_prev存储前一个迭代器 以便转移后可以更新my_cur迭代器

		while (my_cur != end() && x_cur != x.end()) {
			if (*my_cur <= *x_cur) {
				++my_prev;
				my_cur = my_prev.node->next;
			}
			else {
				splice_after(my_cur, x, x_cur);//此处等于更新x_prev
				//splice_after会更新my_cur前面一个迭代器的next 所以直接赋值更新my_cur即可
				x_cur = x_prev.node->next;
			}
		}
		if (x_cur != x.end()) {
			splice_after(my_prev, x, x_cur, x.end());
		}
	}

	template <class Compare>
	void merge(forward_list& x, Compare comp) {
		iterator my_prev = before_begin();
		iterator my_cur = my_prev.node->next;
		//这里多写一个my_prev是为了记录if需要进行后续转移时的转移位置

		iterator x_prev = x.before_begin();
		iterator x_cur = x_prev.node->next;
		//被调用方需要一个x_prev存储前一个迭代器 以便转移后可以更新my_cur迭代器

		while (my_cur != end() && x_cur != x.end()) {
			if (comp(*my_cur,*x_cur)) {
				++my_prev;
				my_cur = my_prev.node->next;
			}
			else {
				splice_after(my_cur, x, x_cur);//此处等于更新x_prev
				//splice_after会更新my_cur前面一个迭代器的next 所以直接赋值更新my_cur即可
				x_cur = x_prev.node->next;
			}
		}
		if (x_cur != x.end()) {
			splice_after(my_prev, x, x_cur, x.end());
		}
	}
	void sort() {
		iterator cur = begin();
		iterator next = cur.node->next;
		for (; cur != end(); ++cur) {
			value_type index = *cur;
			next = cur.node->next;//此处next为更新之后的cur的next
			for (; next != end(); ++next) {
				if (*next<index)
					index = *next;
			}
			*cur = index;
		}
	}
	template <class Compare>
	void sort(Compare comp) {
		iterator cur = begin();
		iterator next = cur.node->next;
		for (; cur != end(); ++cur) {
			value_type index = *cur;
			next = cur.node->next;//此处next为更新之后的cur的next
			for (; next != end(); ++next) {
				if (comp(*next,index))
					index = *next;
			}
			*cur = index;
		}
	}
	void reverse() noexcept {
		forward_list tmp;
		iterator cur=begin();
		for (; cur != end(); ++cur) {
			tmp.push_front(*cur);
		}
		erase_after(before_begin(), end());
		*this = tmp;
	}
};




template <class T, class Alloc>
void swap(forward_list<T, Alloc>& x, forward_list<T, Alloc>& y) {
	x.swap(y);
}
template <class T, class Alloc>
bool operator== (const forward_list<T, Alloc>& x, const forward_list<T, Alloc>& y) {
	using iterator = typename forward_list<T, Alloc>::iterator;
	iterator x_cur = x.begin();
	iterator y_cur = y.begin();
	while (x_cur && y_cur && *y_cur == *x_cur) {
		++x_cur;
		++y_cur;
	}
	return (x_cur == 0 && y_cur == 0);
}
template <class T, class Alloc>
bool operator!= (const forward_list<T, Alloc>& x, const forward_list<T, Alloc>& y) {
	return !(operator==(x, y));
}


template <class T, class Alloc>
bool operator< (const forward_list<T, Alloc>& x, const forward_list<T, Alloc>& y) {
	using iterator = typename forward_list<T, Alloc>::iterator;
	iterator x_cur = x.begin();
	iterator y_cur = y.begin();
	while (x_cur && y_cur ) {
		if (*y_cur < *x_cur) {
			++x_cur;
			++y_cur;
		}
		else {
			return false;
		}	
	}
	return true;
}
template <class T, class Alloc>
bool operator>(const forward_list<T, Alloc>& x, const forward_list<T, Alloc>& y) {
	return operator<(y, x);
}
template <class T, class Alloc>
bool operator<=(const forward_list<T, Alloc>& x, const forward_list<T, Alloc>& y) {
	return !(operator<(y, x));
}
template <class T, class Alloc>
bool operator>=(const forward_list<T, Alloc>& x, const forward_list<T, Alloc>& y) {
	return !(operator<(x, y));
}
#endif // ! MY_FORWARD_LIST
