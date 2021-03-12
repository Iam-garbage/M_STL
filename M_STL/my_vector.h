#ifndef MY_VECTOR
#define MY_VECTOR
#include"my_allocator.h"
#include<iostream>
#include<type_traits>
template<typename T, typename alloc = my_allocator<T>>
class myvector{
public:
	using value_type                 = T;
	using pointer                    = value_type*;
	using const_pointer              = const pointer;
	using iterator                   = T*;
	using const_iterator             = const value_type*;
	using reference                  = T&;
	using const_reference            = const value_type&;
	using difference_type            = ptrdiff_t;
	using size_type                  = size_t;
	using reverse_iterator           = iterator;
	using const_reverse_iterator     = const_iterator;
protected:
	using allocator_type             = my_allocator<value_type>;
	iterator start;
	iterator finish;
	iterator end_of;

	iterator allocate_fill(size_type n,value_type value) {
		iterator result = allocator_type::allocate(n);
		uninitialized_fill_n(result, n, value);
		return result;
	}

	void fii_initialize(size_type n, value_type value) {
		start = allocate_fill(n, value);
		finish = start + n;
		end_of = finish;
	}

	
	void insert_aux(iterator position,  T& x) {
		if (finish != end()) {
			construct(finish, *(finish - 1));
			++finish;
			std::copy_backward(position, finish - 2, finish - 1);
			value_type x(x);
			*position = x;
		}
		else {
			const size_type old = size();
			const size_type newsize = old == 0 ? 0 : 2 * old;
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;
			
			newfinish = ::uninitialized_copy(start, position, newstart);
			construct(newfinish, x);
			++newfinish;//省略了异常
			newfinish = ::uninitialized_copy(position, finish, newfinish);
			
			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
		}
	}
public:
	//以下为构造函数
	explicit myvector(const allocator_type& alloc = allocator_type()):
	start(0),finish(0), end_of(0){};
	explicit myvector(size_type n) {
		fii_initialize(n, T());
	}
	myvector(size_type n, const value_type& value) {
		fii_initialize(n, value);
	}
	//template<typename Inputiterator>
	myvector(iterator first, iterator last,
		const allocator_type& alloc = allocator_type()) {
		size_type n = 0;
		iterator cur = first;
		for (cur; cur != last; first++, cur++)
			n++;
		
		fii_initialize(n, typename myvector<T, my_allocator<T>>::value_type());
		::uninitialized_copy(first, last, begin());
	}
	myvector(const myvector& x, const allocator_type& alloc = allocator_type()) {
		size_type n = x.end() - x.begin();
		fii_initialize(n, T());

		::uninitialized_copy(x.begin(), x.end(), x.begin());
	}
	myvector(myvector&& x, const allocator_type& alloc = allocator_type()) {
		start = std::move(x.start);
		end_of = std::move(x.end_of);
		finish = std::move(x.finish);
	}
	myvector(std::initializer_list<value_type> il,
		const allocator_type& alloc = allocator_type()) {
		size_type n = il.end() - il.begin();
		fii_initialize(n, T());
		::uninitialized_copy(il.begin(), il.end(), begin());

	}

	//以下为析构函数
	~myvector()noexcept {
		destory(start, finish);//销毁对象
		allocator_type::deallocate(start, end_of - start);//回收空间
	}

    //以下为赋值运算符重载函数
	myvector& operator=(const myvector& x) {
		fii_initialize(x.end() - x.begin(), T());
		::uninitialized_copy(x.begin(), x.end(), begin());
	}

	myvector& operator= (myvector&& x) {
		start = std::move(x.start);
		finish = std::move(x.finish);
		end_of = std::move(x.end_of);
	}

	myvector& operator= (std::initializer_list<value_type> il) {
		size_type n = il.end() - il.begin();
		fii_initialize(n, T());
		::uninitialized_copy(il.begin(), il.end(), begin());
	}

	iterator begin()noexcept {
		return start;
	}
	const_iterator cbegin()const noexcept {
		return start;
	}
	const_reverse_iterator crbegin()const noexcept {
		return const_reverse_iterator(begin());
	}
	reverse_iterator rbegin()noexcept {
		return reverse_iterator(begin());
	}
	iterator end()noexcept {
		return finish;
	}
	const_iterator cend()const noexcept {
		return finish;
	}
	reverse_iterator rend()const noexcept {
		return reverse_iterator(end());
	}
	const_reverse_iterator crend()const noexcept {
		return const_reverse_iterator(end());
	}

	size_type size()const noexcept {
		return (end() - begin());
	}
	size_type max_size()const noexcept {
		return size_type(end_of - begin());
	}
	size_type capacity()const noexcept {
		return size_type(end_of - begin());
	}


	iterator erase(iterator position) {
		if (position + 1 != end())
			std::copy(position + 1, finish, position);
		--finish;
		destory(finish);
		return position;
	}
	iterator erase(iterator first, iterator last) {
		if (first == end())
			return end();
		std::copy(last, finish, first);
		destory(first + (finish - last), finish);
		finish = finish - (last - first);
		return first;
	}
	void resize(size_type n, const value_type& value=T()) {
		if (n < size())
			erase(begin() + n, end());
		else
			insert(end(), n - size(), value);
	}
	bool empty() const noexcept {
		return begin() == end();
	}
	void reserve(size_type n) {
		if (capacity() < n) {
			const size_type old = size();
			iterator newstart = allocator_type::allocate(n);
			::uninitialized_copy(begin(), end(), newstart);
			destory(begin(), end());
			start = newstart;
			finish = start + old;
			end_of = start + n;
		}
	}
	void shrink_to_fit() {
		size_type n = size();
		resize(n);
	}

	allocator_type get_allocator() const noexcept {
		return allocator_type();
	}

	//以下为元素访问函数 []重载 at（） front（） back()
	reference operator [] (size_type n) {
		return *(begin()+n);//n的大小会根据指针指向类型大小进行计算
	}
	const_reference operator [] (size_type n)const {
		return *(begin() + n);
	}
	reference at(size_type n) {//at应该具备异常
		if (n > (end() - begin()))
			return 0;
		else
			return *(begin() + n);
	}
	const_reference at(size_type n) const{
		if (n > (end() - begin()))
			return 0;
		else
			return *(begin() + n);
	}
	reference front() {
		return *(begin());
	}
	const_reference front()const {
		return *(begin());
	}
	reference back() {
		return *(end() - 1);
	}
	const_reference back() const{
		return *(end() - 1);
	}

	template <class InputIterator>
	void assign(InputIterator first, InputIterator last) {//assign旧元素都会被销毁
		destory(start, finish);
		::uninitialized_copy(first, last, begin());
	}
	void assign(size_type n, const value_type& val) {
		destory(start, finish);
		uninitialized_fill_n(start, n, val);
	}
	void assign(std::initializer_list<value_type> il) {
		destory(start, finish);
		::uninitialized_copy(il.begin(), il.end(), begin());
	}

	//push_back & pop_back
	void push_back(const value_type& val) {
		if (end() != end_of)
			construct(end(), val);
		else
			insert_aux(end(), val);
	}
	void push_back(value_type&& val) {
		if (end() != end_of)
			construct(end(), std::move(val));
		else
			insert_aux(end(), std::move(val));
	}
	void pop_back() {
		destory(end() - 1);
		finish--;
	}
	
	//以下为insert函数
	iterator insert(const_iterator position,  const value_type& val) {
		if (finish != end_of) {
			construct(finish, *(finish - 1));
			++finish;
			std::copy_backward(position, finish - 2, finish - 1);
			value_type x = val;
			*position = x;
			return position;
		}
		else {
			const size_type old = size();
			const size_type newsize = old == 0 ? 0 : 2 * old;
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;

			iterator re=newfinish = ::uninitialized_copy(begin(), position, newstart);
			construct(newfinish, val);
			++newfinish;//省略了异常
			newfinish = ::uninitialized_copy(position, end(), newfinish);

			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
			return re;
		}
	}

	iterator insert(const_iterator position, size_type n, const value_type& val) {
		if (n<=(end_of -finish)) {//if内代码没有按照原先代码进行实现
			T x = val;
			iterator newfinish=finish+n;
			std::copy_backward(position, finish, newfinish);
			destory(position,finish);
			uninitialized_fill_n(position, n, x);
			finish = newfinish;
			return position;
		}
		else {
			const size_type old = size();
			const size_type newsize = old + std::max(old, n);
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;

			T x = val;
			iterator re=newfinish = ::uninitialized_copy(begin(), position, newstart);
			newfinish = uninitialized_fill_n(newfinish, n, x);
			newfinish = ::uninitialized_copy(position, end(), newfinish);

			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
			return re;
		}
	}
	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last) {
		if ((last - first) <= (end_of - finish)) {
			size_type newsize = last - first;
			iterator newfinish = finish + newsize;
			std::copy_backward(position, finish, newfinish);
			::uninitialized_copy(first, last, position);
			finish = newfinish;
			return position;
		}
		else {
			const size_type old = size();
			const size_type newsize = old + std::max(old, (last-first));
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;
			iterator re=newfinish = ::uninitialized_copy(begin(), position, newstart);
			newfinish = ::uninitialized_copy(first, last, newfinish);
			newfinish = ::uninitialized_copy(position, end(), newfinish);

			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
			return re;
		}
	}
		
	iterator insert(const_iterator position, value_type&& val) {
		if (finish != end_of) {
			construct(finish, *(finish - 1));
			++finish;
			std::copy_backward(position, finish - 2, finish - 1);

			*position = std::move(val);
			return position;
		}
		else {
			const size_type old = size();
			const size_type newsize = old == 0 ? 0 : 2 * old;
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;

			iterator re=newfinish = ::uninitialized_copy(begin(), position, newstart);
			construct(newfinish, std::move(val));
			++newfinish;//省略了异常
			newfinish = ::uninitialized_copy(position, end(), newfinish);

			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
			return re;
		}
	}
	iterator insert(const_iterator position, std::initializer_list<value_type> il) {
		if ((il.end() - il.begin()) <= (end_of - finish)) {
			size_type newsize = size_type(il.end() - il.begin());
			iterator newfinish = finish + newsize;
			std::copy_backward(position, finish, newfinish);
			::uninitialized_copy(il.begin(), il.end(), position);
			finish = newfinish;
			return position;
		}
		else {
			const size_type old = size();
			const size_type newsize = old + std::max(old, (il.end() - il.begin()));
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;

			iterator re=newfinish = ::uninitialized_copy(begin(), position, newstart);
			newfinish = ::uninitialized_copy(il.begin(), il.end(), newfinish);
			newfinish = ::uninitialized_copy(position, end(), newfinish);

			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
			return re;
		}
	}


	iterator erase(const_iterator position) {
		std::copy(position + 1, end(), position);
		destory(finish - 1);
		finish = finish - 1;
		return position;
	}
	iterator erase(const_iterator first, const_iterator last) {
		iterator i=std::copy(last, end(), first);//新finish
		destory(i, finish);
		finish = finish - (last - first);
		return first;
	}
	void swap(myvector& x) {
		iterator newstart = x.start;
		iterator newfinish = x.finish;
		iterator newend = x.end_of;
		
		x.start = start;
		x.finish = finish;
		x.end_of = end_of;

		start = newstart;
		finish = newfinish;
		end_of = newend;
	}
	
	void clear() noexcept {
		destory(begin(), end());
	}
	//emplace函数

	template<typename...arg>//emplace 还需要进行测试
	iterator emplace(const_iterator position, arg&&...agrs) {
		if (finish != end_of) {
			construct(finish, *(finish - 1));
			++finish;
			std::copy_backward(position, finish - 2, finish - 1);
			value_type x(agrs...);
			*position = x;
			return emplace(position,agrs...);
		}
		else {
			const size_type old = size();
			const size_type newsize = old == 0 ? 0 : 2 * old;
			iterator newstart = allocator_type::allocate(newsize);
			iterator newfinish = newstart;

			iterator re = newfinish = ::uninitialized_copy(begin(), position, newstart);
			construct(newfinish, agrs...);
			++newfinish;//省略了异常
			newfinish = ::uninitialized_copy(position, end(), newfinish);

			destory(start, finish);
			allocator_type::deallocate(start, old);

			start = newstart;
			finish = newfinish;
			end_of = start + newsize;
			return emplace(position, agrs...);
		}
	}
 //此函数与emplace一样 需要等待测试
	template <class... Args>
	void emplace_back(Args&&... args) {
		if (end() != end_of)
			construct(end(), std::move(args...));
		else
			insert_aux(end(), std::move(args...));
	}
	
};

template<typename T, typename alloc>
void swap(myvector<T, alloc>& x, myvector<T, alloc>& y) {
	x.swap(y);
}

//以下为运算符函数重载
template<typename T,typename Alloc>
bool operator==(const myvector<T, Alloc>& one, const myvector<T, Alloc>& two) {
	if (one.cbegin() == two.cbegin() && one.end() == two.end())
		return true;
	typedef typename myvector<T, Alloc>::iterator iterator;
	iterator one_cur = one.cbegin();
	iterator two_cur = two.cbegin();
	while (one_cur != one.end() && two_cur != two.end()) {
		if (*(one_cur) != *(two_cur))
			return false;
		one_cur++;
		two_cur++;
	}
	if (one_cur == one.end() && two_cur == two.end())
		return true;
	else
		return false;
}

template <class T, class Alloc>
bool operator!= (const myvector<T, Alloc>& one, const myvector<T, Alloc>& two)
{
	return !(one == two);
}

template <class T, class Alloc>
bool operator<  (const myvector<T, Alloc>& one, const myvector<T, Alloc>& two) {
	if (one.cbegin() == two.cbegin() && one.end() == two.end())
		return false;
	typedef typename myvector<T, Alloc>::iterator iterator;
	iterator one_cur = one.cbegin();
	iterator two_cur = two.cbegin();
	while (one_cur != one.end() && two_cur != two.end()) {
		if (*(one_cur) < *(two_cur))
			return true;
		one_cur++;
		two_cur++;
	}
	if (one_cur == one.end() && two_cur == two.end())
		return false;
	else if (two.cur != two.end())
		return false;
	else
		return true;
}
template <class T, class Alloc>
bool operator<= (const myvector<T, Alloc>& one, const myvector<T, Alloc>& two) {
	return !(two < one);
}
template <class T, class Alloc>
bool operator> (const myvector<T, Alloc>& one, const myvector<T, Alloc>& two) {
	return (two < one);
}
template <class T, class Alloc>
bool operator>= (const myvector<T, Alloc>& one, const myvector<T, Alloc>& two) {
	return !(one < two);
}
#endif