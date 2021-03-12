#ifndef MY_DEQUE
#define MY_DEQUE

#include"my_allocator.h"
#include"my_iterator.h"
#include"mytype_traits.h"
#include<iostream>//已无误
//修改时间:2020/11/26/20:01
//deque的空间配置有默认大小 如果空间不够需要自己调整
inline size_t deque_buf_size(size_t n, size_t size) {//size是容器内单个缓冲区元素的大小
	return n != 0 ? n : (size < 512 ? size_t(512 / size) : size_t(1));
}
//deque_iterator的构造
template<class T, class ref, class ptr, size_t bufsize>
struct _deque_iterator {
	//deque的迭代器和vector不同 是使用类表示
	using iterator_category = random_access_iterator_tag;
	using value_type = T;
	using pointer = ptr;
	using reference = ref;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using map_pointer = T**;
	using iterator = _deque_iterator<T, T&, T*, bufsize>;
	using const_iterator = _deque_iterator<T, const T&, const T*, bufsize>;
	using self = _deque_iterator;

	static size_t buffer_size() {
		return deque_buf_size(bufsize, sizeof(T));
	}

	pointer my_cur;
	pointer my_first;
	pointer my_last;
	map_pointer node;//中控器指针
	_deque_iterator(T* x, map_pointer mp) :
		my_cur(x), my_first(*mp),
		node(mp), my_last(*mp + buffer_size()) {}
	_deque_iterator() :my_cur(0), my_first(0), my_last(0), node(0) {}
	_deque_iterator(const iterator& x) :my_cur(x.my_cur),
		my_first(x.my_first), my_last(x.my_last), node(x.node) {}

	void set_node(map_pointer newnode) {
		node = newnode;
		my_first = *newnode;
		my_last = my_first + difference_type(buffer_size());
	}


	reference operator*()const { return *my_cur; }
	pointer operator->()const { return &(operator*()); }
	difference_type operator-(const self& x)const {
		return difference_type(buffer_size()) * (node - x.node - 1) +
			(my_cur - my_first) + (x.my_last - x.my_cur);
	}
	self& operator++() {
		++my_cur;
		if (my_cur == my_last) {
			set_node(node + 1);
			my_cur = my_first;
		}
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		++* this;
		return tmp;
	}
	self& operator--() {
		if (my_cur == my_first) {
			set_node(node - 1);
			my_cur = my_last;
		}
		--my_cur;
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--* this;
		return tmp;
	}
	self& operator+=(difference_type n) {
		difference_type len = n + (my_cur - my_first);
		if (len >= 0 && len < difference_type(buffer_size()))
			my_cur += n;
		else {
			difference_type node_len =
				len > 0 ? len / difference_type(buffer_size()):
				-difference_type((-len - 1) / buffer_size()) - 1;
			set_node(node + node_len);
			my_cur = my_first + (len - node_len * difference_type(buffer_size()));
		}
		return *this;
	}
	self operator+(difference_type n) const {
		self tmp = *this;
		return tmp += n;
	}
	self& operator-=(difference_type n)const {
		return *this += -n;
	}
	self operator-(difference_type n)const {
		self tmp = *this;
		return tmp -= n;
	}
	reference operator[](difference_type n) const {
		return *(*this + n);//不改变原先指针的值
	}
	bool operator==(const self& x)const {
		return (my_cur == x.my_cur);
	}
	bool operator!=(const self& x)const {
		return (*this == x);
	}
	bool operator<(const self& x)const {
		return node == x.node ? my_cur < x.my_cur :
			(node < x.node);
	}

};

template <class _Tp, class _Ref, class _Ptr, size_t __bufsiz>
inline random_access_iterator_tag
iterator_category(const _deque_iterator<_Tp, _Ref, _Ptr, __bufsiz>&) {
	return random_access_iterator_tag();
}

template <class _Tp, class _Ref, class _Ptr, size_t __bufsiz>
inline _Tp*
value_type(const _deque_iterator<_Tp, _Ref, _Ptr, __bufsiz>&) {
	return 0;
}

template <class _Tp, class _Ref, class _Ptr, size_t __bufsiz>
inline ptrdiff_t*
distance_type(const _deque_iterator<_Tp, _Ref, _Ptr, __bufsiz>&) {
	return 0;
}


//以下为deque的内存分配与回收部分 使用类进行包装 
//需要两种类型的内存分配器
//bufsize 为单个缓冲区尺寸 
//这个逼玩意_deque_base类没有类型的构造 只有你妈的空间分配回收
template<class T, class allocator, size_t bufsize>
class _deque_base {

public:
	using iterator = _deque_iterator<T, T&, T*, bufsize>;
	using const_iterator = _deque_iterator<T, const T&, const T*, bufsize>;
	using allocator_type = allocator;

	allocator_type get_allocator()const {
		return allocator_type();
	}
	_deque_base(_deque_base&& x) {
		start(std::move(x.start));
		finish(std::move(x.finish));
		map(std::move(x.map));
		map_size = x.map_size;
	}
	_deque_base(const allocator_type&, size_t n) :map(0), map_size(0), start(), finish()
	{
		initialize_map(n);
	}
	_deque_base(const allocator_type&) :map(0), map_size(0), start(), finish()
	{}
	~_deque_base() {
		if (map) {
			destroy_nodes(start.node, (finish.node + 1));
			deallocate_map(map, map_size);
		}
	}
protected:
	iterator start;//指向deque的头部
	iterator finish;//指向deque的尾部
	T** map;//中控器指针
	size_t map_size;//中控器尺寸
	enum { initial_map_size = 8 };
	using node_alloc_type = my_allocator<T>;//节点分配器
	using map_alloc_type = my_allocator<T*>;//控制器指针分配器

	void initialize_map(size_t n) {
		size_t n_node = n / deque_buf_size(n, sizeof(T)) ;
		//n_node的计算不需要+1 如果额外加1 那么deque_buf_size算出的节点数就多了1 
		//导致后面的内存空间分配过多
		//最终导致错误的start和finish 于是初始化超额
		map_size = std::max((size_t)initial_map_size, n_node + 2);
		//以上两句求缓冲区个数
		map = allocate_map(map_size);

		T** newstart = map + (map_size - n_node) / 2;
		T** newfinish = newstart + n_node;

		create_nodes(newstart, newfinish);//构建相应空间
		start.set_node(newstart);
		finish.set_node(newfinish-1);
		start.my_cur = start.my_first;
		finish.my_cur = finish.my_first + n % deque_buf_size(bufsize, sizeof(T));
	}//上述五条语句是为了构建对应数量类型的默认值 和对应的指针



	//以下语句对应分配器空间分配回收	
	void create_nodes(T** newstart, T** newfinish) {
		T** cur;
		for (cur = newstart; cur < newfinish; cur++) {
			*cur = allocate_node();
		}
		//异常没写 正常是如果失败就回滚 全部销毁
	}
	void destroy_nodes(T** newstart, T** newfinish) {
		T** cur;
		for (cur = newstart; cur < newfinish; cur++)
			deallocate_node(*cur);
	}

	//空间分配回收分两种
	//此处构建的时候 一次性构造一个缓冲区内所有元素个数的默认值
	T* allocate_node() {
		return node_alloc_type::allocate(deque_buf_size(bufsize, sizeof(T)));
	}
	//析构的函数同样如此 
	//最后会一个个的转调用类型的析构函数
	void deallocate_node(T* x) {
		node_alloc_type::deallocate(x, deque_buf_size(bufsize, sizeof(T)));
	}
	//单个缓冲区所有结点的构造 和中控器指针构造 使用不同类型的分配器进行
	T** allocate_map(size_t n) {
		return (map_alloc_type::allocate(n));
	}
	void deallocate_map(T** p, size_t n) {
		map_alloc_type::deallocate(p, n);
	}
};


//关于deque的警惕  迭代器部分都是已经重载过相应的运算符了 对于迭代器内部指向
//当前元素的cur指针 重载的运算符都是有对这部分进行考虑 不需要自作聪明去
//使用域运算符进行计算
//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换

template<class T, class alloc = my_allocator<T>, size_t bufsize = 0>
class deque :protected _deque_base<T, alloc, bufsize> {
protected:
	using base = _deque_base<T, alloc, bufsize>;

public:
	using value_type = T;
	using pointer = T*;

	using const_pointer = const pointer;
	using reference = T&;
	using const_reference = const reference;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using allocator_type = typename base::allocator_type;
	using iterator = typename base::iterator;
	using const_iterator = typename base::const_iterator;
	using const_reverse_iterator = reverse_iterator<const_iterator>;
	using reverse_iterator = reverse_iterator<iterator>;

	allocator_type get_allocator() const { return base::get_allocator(); }
protected:
	//数据指针部分
	using base::map;
	using base::map_size;
	using base::start;
	using base::finish;
	//接口
	using base::initialize_map;
	using base::create_nodes;
	using base::destroy_nodes;
	using base::allocate_node;
	using base::deallocate_node;
	using base::allocate_map;
	using base::deallocate_map;
	//using _deque_iterator::node; //此处给fill_initialize里面的node做备用
	
	using map_pointer = pointer*;//protected 内容  
	static size_t buffer_size() {
		return deque_buf_size(bufsize, sizeof(T));
	}
	void fill_initialize(const value_type& value) {
		map_pointer cur;
		for (cur = start.node; cur < finish.node; ++cur) {
			::uninitialized_fill(*cur, (*cur + buffer_size()), value);
		}
		uninitialized_fill(finish.my_first, finish.my_cur, value);
	}
public:
	
	iterator begin()noexcept { return start; }
	const_iterator begin()const noexcept { return start; }
	const_iterator cbegin()const noexcept { return start; }
	reverse_iterator rbegin()noexcept { return reverse_iterator(start); }
	const_reverse_iterator rebegin()const noexcept { return reverse_iterator(start); }
	const_reverse_iterator crbegin()const noexcept { return reverse_iterator(start); }

	iterator end()noexcept { return finish; }
	const_iterator end()const noexcept { return finish; }
	const_iterator cend()const noexcept { return finish; }
	reverse_iterator rend()noexcept { return reverse_iterator(finish); }
	const_reverse_iterator rend()const noexcept { return reverse_iterator(finish); }
	const_reverse_iterator crend()const noexcept { return reverse_iterator(finish); }

	size_type size() const noexcept { return end() - begin(); }
	size_type max_size() const noexcept { return size_type(-1); }
	bool empty()const { return begin() = end(); }
	void resize(size_type n, const value_type& val = value_type()) {
		if (n < size()) {
			erase(start + n, finish);
		}
		else {
			insert(finish, n-size(), val);
		}
	}

	void shrink_to_fit() {
		resize(finish - start);
	}

	reference front() { return *begin(); }
	const_reference front()const { return *begin(); }
	reference back() {
		iterator tmp = finish;
		return -- * tmp;
	}
	const_reference back()const {
		iterator tmp = finish;
		return -- * tmp;
	}


	reference operator[](size_type n) {
		return start[difference_type(n)];
	}
	const_reference operator[](size_type n)const {
		return start[difference_type(n)];
	}
	reference at(size_type n) {
		if (n > size())
			return 0;//此处本该是异常 range_error
		else
			return operator[](n);
	}
	const_reference at(size_type n)const {
		if (n > size())
			return 0;//此处本该是异常 range_error
		else
			return operator[](n);
	}
public:
	//copy部分不需要解引用或者使用.运算符  已经针对deque_iterator整体进行过++重载
	//所以直接使用这个迭代器类型即可
	explicit deque(const allocator_type& alloc = allocator_type()) :base(alloc, 0) {}
	explicit deque(size_type n) :base(allocator_type(), n) {}
	deque(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
		:base(alloc, n) {
		fill_initialize(val);
	}
	//template <class InputIterator> //可能又跟vector一样了 不行就算了
	deque(iterator first, iterator last, const
		allocator_type& alloc = allocator_type())
		: base(alloc, last - first) {
		::uninitialized_copy(first, last, begin());
	}
	deque(const deque& x, const allocator_type& alloc = x.allocator_type())
		:base(alloc, x.size()) {
		::uninitialized_copy(x.begin(), x.end(), begin());
	}
	deque(deque&& x, const allocator_type& alloc = x.allocator_type())
		:base(std::move(x.base())) {}
	deque(std::initializer_list<value_type> il,
		const allocator_type& alloc = allocator_type()) :base(alloc, il.size()) {
		::uninitialized_copy(il.begin(), il.end(), begin());
	}
	//copy部分不需要解引用或者使用.运算符  已经针对deque_iterator整体进行过++重载
 //所以直接使用这个指针即可
 //与构造同样的思路
	~deque() {
		destory(begin(), end());
		deallocate_map(map, map_size); 
	}

	deque& operator=(const deque& x) {
		const size_type len = size();
		if (this != &x) {
			if (len >= x.size()) {
				earse(uninitialized_copy(x.begin(), x.end()), end());
			}
			else {
				const_iterator mid = x.begin() + difference_type(len);
				::uninitialized_copy(x.begin(), mid, start);
				insert(finish, mid, x.end());
			}
		}
		return *this;
	}
	deque& operator= (std::initializer_list<value_type> x) {
		const size_type len = size();
		if (len >= x.size()) {
			earse(uninitialized_copy(x.begin(), x.end()), end());
		}
		else {
			const_iterator mid = x.begin() + difference_type(len);
			::uninitialized_copy(x.begin(), mid, start);
			insert(finish, mid, x.end());
		}
	}
	deque& operator= (deque&& x) {
		base(std::move(x.base()));
		return *this;

	}

public:
	void assign(iterator first, iterator last) {
		size_type len = last - first;
		if (len < size()) {
			uninitialized_copy(first, last, start);
			destory(begin() + len, end());
			finish -= len;
		}
		else if(len<(map_size*buffer_size())){
			destory(begin(), end());
			iterator one;
			one.set_node(map);
			one.my_cur = one.my_first;
			uninitialized_copy(first, last, one);
			start = one;
			finish += len;
		}
		else {
			size_type nodesize = len / buffer_size() + 1;
			size_type mapsize = std::max(2 * map_size, nodesize + 2);
			map = allocate_map(mapsize);//替换中控器空间
			map_pointer nstart = map + (mapsize - nodesize) / 2;
			map_pointer nfinish = nstart + nodesize - 1;
			map_pointer cur;
			for (cur = nstart; cur <= nfinish; ++cur) {
				*cur = allocate_node();
			}
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.my_cur = start.my_first;
			finish.my_cur = finish.my_first + len % buffer_size();
			uninitialized_copy(first, last, start);
		}
	}

	void assign(size_type n, const value_type& val) {
		if (n < size()) {
			uninitialized_fill_n(start,n,val);
			destory(begin() + n, end());
			finish -= n;
		}
		else if (n < (map_size * buffer_size())) {
			destory(begin(), end());
			iterator one;
			one.set_node(map);
			one.my_cur = one.my_first;
			uninitialized_fill_n(one, n, val);
			start = one;
			finish += n;
		}
		else {
			size_type nodesize = n / buffer_size() + 1;
			size_type mapsize = std::max(2 * map_size, nodesize + 2);
			map = allocate_map(mapsize);//替换中控器空间
			map_pointer nstart = map + (mapsize - nodesize) / 2;
			map_pointer nfinish = nstart + nodesize - 1;
			map_pointer cur;
			for (cur = nstart; cur <= nfinish; ++cur) {
				*cur = allocate_node();
			}
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.my_cur = start.my_first;
			finish.my_cur = finish.my_first + n % buffer_size();
			uninitialized_fill_n(start, n, val);
		}
	}

	void assign(std::initializer_list<value_type> il) {
		size_type len = il.size();
		if (len < size()) {
			uninitialized_copy(il.begin(), il.end(), start);
			destory(begin() + len, end());
			finish -= len;
		}
		else if (len < (map_size * buffer_size())) {
			destory(begin(), end());
			iterator one;
			one.set_node(map);
			one.my_cur = one.my_first;
			uninitialized_copy(il.begin(), il.end(), one);
			start = one;
			finish += len;
		}
		else {
			destory(begin(), end());//销毁旧空间
			deallocate_map(map, map_size);
			size_type nodesize = len / buffer_size() + 1;//分配新空间
			size_type mapsize = std::max(2 * map_size, nodesize + 2);
			map = allocate_map(mapsize);//替换中控器空间
			map_pointer nstart = map + (mapsize - nodesize) / 2;
			map_pointer nfinish = nstart + nodesize - 1;
			map_pointer cur;
			for (cur = nstart; cur <= nfinish; ++cur) {
				*cur = allocate_node();
			}
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.my_cur = start.my_first;
			finish.my_cur = finish.my_first + len % buffer_size();
			uninitialized_copy(il.begin(), il.end(), start);
			
		}
	}

	void clear() noexcept {
		destory(start, finish);
	}

	void pop_back() {
		if (finish.my_cur != finish.my_first) {
			--finish.my_cur;
			destory(finish.my_cur);
		}
		else {
			pop_back_aux();
		}
	}
	void pop_front() {
		if (start.my_cur != start.my_last - 1) {
			destory(start.my_cur);
			++start.my_cur;
		}
		else {
			pop_front_aux();
		}
	}

	void push_back(const value_type& val) {
		value_type x = val;
		if (finish.my_cur != finish.my_last - 1) {
			construct(finish.my_cur, x);
			++finish.my_cur;
		}
		else {
			push_back_aux(x);
		}
	}
	void push_back(value_type&& val) {
		if (finish.my_cur != finish.my_last - 1) {
			construct(finish.my_cur, std::move(val));
			++finish.my_cur;
		}
		else {
			push_back_aux(std::move(val));
		}
	}

	void push_front(const value_type& val) {
		if (start.my_cur != start.my_first) {
			construct(start.my_cur - 1, val);
			--start.my_cur;
		}
		else {
			push_front_aux(val);
		}
	}
	void push_front(value_type&& val) {
		if (start.my_cur != start.my_first) {
			construct(start.my_cur - 1, std::move(val));
			--start.my_cur;
		}
		else {
			push_front_aux(std::move(val));
		}
	}
protected:
	void reserve_map_back(size_type add = 1) {
		if (add + 1 > map_size - (finish.node - map)) {
			reallocate_map(add, false);
		}
	}
	void reserve_map_front(size_type add = 1) {
		if (add > start.node - map) {
			reallocate_map(add, true);
		}
	}
	void reallocate_map(size_type add, bool front_or_back) {
		size_type oldnode = finish.node - start.node + 1;
		size_type newnode = oldnode + add;
		map_pointer newstart;
		if (map_size > 2 * newnode) {
			newstart = map + (map_size - newnode) / 2 + (front_or_back ? add : 0);
			if (newstart < start.node) {
				uninitialized_copy(start.node, finish.node + 1, newstart);
			}//此处改变的是缓冲区里面指向数据的指针
			else {
				std::copy_backward(start.node, finish + 1, newstart + oldnode);
			}
		}
		else {
			size_type newmap_size = map_size + std::max(map_size, newnode) + 2;
			map_pointer newmap = allocate_map(newmap_size);
			newstart = map + (map_size - newnode) / 2 + (front_or_back ? add : 0);
			uninitialized_copy(start.node, finish.node + 1, newstart);
			deallocate_map(map, map_size);
		}
		start.set_node(newstart);
		finish.set_node(newstart + oldnode - 1);
	}
protected:
	void push_front_aux(const value_type& x) {
		value_type val = x;
		reserve_map_front();
		*(start.node - 1) = allocate_node();//添加新的缓冲区
		start.set_node(start.node - 1);
		start.my_cur = start.my_last - 1;//更新mycur
		construct(start.my_cur, val);
	}
	void push_front_aux( value_type&& x) {
		reserve_map_front();
		*(start.node - 1) = allocate_node();//添加新的缓冲区
		start.set_node(start.node - 1);
		start.my_cur = start.my_last - 1;//更新mycur
		construct(start.my_cur, std::move(x));
	}
	void push_back_aux(const value_type& x) {
		value_type n = x;
		reserve_map_back();
		*(finish.node + 1) = allocate_node();
		construct(finish.my_cur, x);
		//cur指针判定是原last位置前一个元素 也就是最后一个可填写元素
		finish.set_node(finish.node + 1);
		finish.my_cur = finish.my_first;
	}
	void push_back_aux( value_type&& x) {//移动语义
		reserve_map_back();
		*(finish.node + 1) = allocate_node();
		construct(finish.my_cur, std::move(x));
		//cur指针判定是原last位置前一个元素 也就是最后一个可填写元素
		finish.set_node(finish.node + 1);
		finish.my_cur = finish.my_first;
	}
	void pop_back_aux() {
		deallocate_node(finish);
		finish.set_node(finish.node - 1);
		finish.mt_cur = finish.my_last - 1;
		destory(finish.my_cur);
	}
	void pop_front_aux() {
		destory(start.my_cur);
		deallocate_node(start.my_first);
		start.set_node(start.node + 1);
		start.my_cur = start.first;
	}
public:
	void swap(deque& x) {
		std::swap(start, x.start);
		std::swap(finish, x.finish);
		std::swap(map, x.map);
		std::swap(map_size,x.map_size);
	}
	iterator erase(const_iterator first, const_iterator last) {
		if (last == end() && first == begin()) {
			clear();
			return finish;
		}
		else {
			difference_type n = last - first;
			difference_type before = first - start;
			if (before < (size() - n) / 2) {
				std::copy_backward(start, first, last);
				iterator newstart = start + n;//+n表示移除的数据数量
				destory(start, newstart);
				for (map_pointer cur = start.node; cur < newstart.node; ++cur)
					deallocate_node(*cur, buffer_size());
				//此处for是为了回收分配出去的空间
				start = newstart;
			}
			else {
				uninitialized_copy(last, finish, first);
				iterator newfinish = finish - n;
				destory(newfinish, finish);
				for (map_pointer cur = newfinish.node; cur <= finish.node; ++cur)
					deallocate_node(*cur, buffer_size());
				finish = newfinish;
			}
			return start + before;
		}
	}
	iterator erase(const_iterator position) {
		using std::copy_backward;
		iterator tmp = position;
		++tmp;
		difference_type index = position - start;
		if (index < (size() / 2)) {
			copy_backward(start, position, next);
			pop_front();
		}
		else {
			uninitialized_copy(next, finish, position);
			pop_back();
		}
		return (start + index);
	}

	iterator insert(const_iterator position, const value_type& val) {
		if (position.my_cur == start.my_cur) {
			push_front(val);
			return begin();
		}
		else if (position.my_cur == finish.my_cur) {
			push_back(val);
			return end() - 1;
		}
		else
			return insert_aux(position, val);
	}

	iterator insert(const_iterator position, size_type n, 
		            const value_type& val = value_type()) {
		if (position.my_cur == start.my_cur) {
			while(n>0){
               push_front(val);
			   n--;
			}
			return begin();
		}
		else if (position.my_cur == finish.my_cur) {
			while (n > 0) {
				push_back(val);
				n--;
			}
			return end() - 1;
		}
		else
			return insert_aux(position,n, val);
	}

	iterator insert(const_iterator position, iterator first, iterator last) {
		size_type n = last - first;
		if (position.my_cur == start.my_cur) {
			size_type befor = position - start;
			reserve_map_front(n / buffer_size());
			iterator newstart = start - n;
			iterator newposition=uninitialized_copy(start, position, newstart);
			uninitialized_copy(first, last, newposition);
			start -= n;
			return newposition;
		}
		else if (position.my_cur == finish.my_cur) {
			size_type n = finish - position;
			reserve_map_back(n / buffer_size());
			iterator newfinish = finish + n;
			std::copy_backward(position, finish, newfinish);
			uninitialized_copy(first, last, position);
			return position;
		}
		else
			return insert_aux(position, first, last);
	}
	iterator insert(const_iterator position, value_type&& val) {
		if (position.my_cur == start.my_cur) {
			push_front(std::move(val));
			return begin();
		}
		else if (position.my_cur == finish.my_cur) {
			push_back(std::move(val));
			return end() - 1;
		}
		else
			return insert_aux(position, std::move(val));
	}
	iterator insert(const_iterator position, std::initializer_list<value_type> il) {
		size_type n = il.end() - il.begin();
		if (position.my_cur == start.my_cur) {
			size_type befor = position - start;
			reserve_map_front(n / buffer_size());
			iterator newstart = start - n;
			iterator newposition = uninitialized_copy(start, position, newstart);
			uninitialized_copy(il.begin(), il.end(), newposition);
			start -= n;
			return newposition;
		}
		else if (position.my_cur == finish.my_cur) {
			size_type n = finish - position;
			reserve_map_back(n / buffer_size());
			iterator newfinish = finish + n;
			std::copy_backward(position, finish, newfinish);
			uninitialized_copy(il.begin(), il.end(), position);
			return position;
		}
		else
			return insert_aux(position, il.begin(), il.end());
	
	}
protected://以下为emplace和insert的接口定义
	iterator insert_aux(const_iterator position, value_type&& val) {//移动语义
		difference_type len = position - start;
		iterator newposition;
		if (len < size() / 2) {
			push_front(front());
			iterator oldstart = start;
			++oldstart;
			newposition = start + len;//更新返回位置
			iterator old_position = newposition;
			++old_position;
			uninitialized_copy(oldstart, old_position, start);//覆盖掉旧start
		}
		else {
			push_back(back());
			iterator oldfinish = finish;
			--oldfinish;
			newposition = start + len;//else里面的newposition和position位置其实同步
			std::copy_backward(position, oldfinish, finish);
		}
		*newposition(std::move(val));
		return newposition;
		
	}
	iterator insert_aux(iterator position, const value_type& val=value_type()) {
		difference_type len = position - start;
		value_type x = val;
		if (len < size() / 2) {
			push_front(front());
			iterator oldstart = start;
			++oldstart;
			position = start + len;//更新返回位置
			iterator old_position = position;
			++old_position;
			uninitialized_copy(oldstart, old_position, start);//覆盖掉旧start
		}
		else {
			push_back(back());
			iterator oldfinish = finish;
			--oldfinish;
			position = start + len;
			std::copy_backward(position, oldfinish, finish);
		}
		*position = x;
		return position;
	}

	iterator insert_aux(iterator position, size_type n, const value_type& val) {
		difference_type len = position - start;
		value_type x = val;
		if ((map_size*buffer_size()-(finish-start))>n) {
			difference_type before = ((start.node - map) * buffer_size()
				                      + (start.my_cur - start.my_first));
			//before 记录前端可以往前移的剩余空间
			iterator newstart = start - before;
			uninitialized_copy(start, position, newstart);
			difference_type after = n-before;
			iterator newfinish = finish + after;
			std::copy_backward(position, finish, newfinish);
			position = newstart + before;
			uninitialized_fill_n(position, n, x);
			//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换
			start = start - before;
			finish = finish + after;
			return position;
		}
		else {
			size_type newnode = n / buffer_size() + 2;
			reallocate_map(newnode, false);
			if (n < size() / 2) {
				iterator newstart = start - n;
				uninitialized_copy(start, position, newstart);
				position = newstart + n;
				uninitialized_fill_n(position, n, x);

				//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换
				start = start - n;//只修改被移动过的那部分数据的cur即可
				return position;
			}
			else {
				iterator newfinish = finish + n;
				std::copy_backward(position, finish, newfinish);
				uninitialized_fill_n(position, n, x);
				//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换
				finish = finish + n;
				return position;
			}
		}
	}

	template<class randomiterator>
	iterator insert_aux(iterator position, randomiterator first, randomiterator last) {
		difference_type len = position - start;
		difference_type n = last - first;
		if ((map_size * buffer_size() - (finish - start)) > n) {
			difference_type before = ((start.node - map) * buffer_size()
				+ (start.my_cur - start.my_first));
			//before 记录前端可以往前移的剩余空间
			iterator newstart = start - before;
			iterator newposition=uninitialized_copy(start, position, newstart);
			difference_type after = n - before;
			iterator newfinish = finish + after;
			std::copy_backward(position, finish, newfinish);
			uninitialized_copy(first, last, start);
			
			//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换
			start  -= before;
			finish += after;
			return newposition;
		}
		else {
			size_type newnode = n / buffer_size() + 2;
			reallocate_map(newnode, false);
			if (n < size() / 2) {
				iterator newstart = start - n;
				iterator newposition=uninitialized_copy(start, position, newstart);
				uninitialized_copy(first, last, newposition);

				//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换
				start -= n;//只修改被移动过的那部分数据的cur即可
				return position;
			}
			else {
				iterator newfinish = finish + n;
				std::copy_backward(position, finish, newfinish);
				uninitialized_copy(first, last, position);
				//牢记 调用迭代器的运算符函数 重载中已经考虑到了缓冲区的切换
				finish += n;
				return position;
			}
		}
	}

};
template <class T, class Alloc>
bool operator== (const deque<T, Alloc>& x, const deque<T, Alloc>& y) {
	return (x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin()));
}
template <class T, class Alloc>
bool operator!= (const deque<T, Alloc>& x, const deque<T, Alloc>& y) {
	return !(operator==(x, y));
}
template <class T, class Alloc>
bool operator< (const deque<T, Alloc>& x, const deque<T, Alloc>& y) {
	return(x.size() < y.size() ||
		std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end()));
}
template <class T, class Alloc>
bool operator> (const deque<T, Alloc>& x, const deque<T, Alloc>& y) {
	return operator<(y, x);
}
template <class T, class Alloc>
bool operator>= (const deque<T, Alloc>& x, const deque<T, Alloc>& y) {
	return !(operator<(x, y));
}
template <class T, class Alloc>
bool operator<= (const deque<T, Alloc>& x, const deque<T, Alloc>& y) {
	return !(operator<(y,x));
}
template <class T, class Alloc>
void swap(deque<T, Alloc>& x, deque<T, Alloc>& y) {
	x.swap(y);
}
/*template <class T, class alloc>
std::ostream& operator<<(std::ostream& os, typename deque<T, alloc>::iterator& x) {
	os << *x << std::endl;
	return os;
}*/
#endif