#pragma once
#ifndef HASHTABLE
#define HASHTABLE
#include"mytype_traits.h"
#include"my_allocator.h"
#include"my_iterator.h"
#include"my_vector.h"
inline size_t hash_string(const char* s) {
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5 * h + *s;
	return size_t(h);
}


template<class key>
struct hash {};//这里只写基本模板然后需要针对各种类型进行特化

//特化部分进行返回就好了 内部进行取值已经使用了除留余数法

template<>
struct hash<char*> {
	size_t operator()(const char* x) {
		return hash_string(x);
	}
};
template<>
struct hash<const char> {
	size_t operator()(const char* x) {
		return hash_string(x);
	}
};
template<>
struct hash<int> {
	size_t operator()(int x) {
		return (size_t)x;
	}
};
template<>
struct hash<unsigned int> {
	size_t operator()(unsigned int x) {
		return (size_t)x;
	}
};
template<>
struct hash<long> {
	size_t operator()(long x) {
		return (size_t)x;
	}
};
template<>
struct hash<unsigned long> {
	size_t operator()(unsigned long x) {
		return (size_t)x;
	}
};
template<>
struct hash<char> {
	size_t operator()(char x) {
		return (size_t)x;
	}
};
template<>
struct hash<unsigned char> {
	size_t operator()(unsigned char x) {
		return (size_t)x;
	}
};
template<>
struct hash<signed char> {
	size_t operator()(signed char x) {
		return (size_t)x;
	}
};
template<>
struct hash<short> {
	size_t operator()(short x) {
		return (size_t)x;
	}
};
template<>
struct hash<unsigned short> {
	size_t operator()(unsigned short x) {
		return (size_t)x;
	}
};
template<>
struct hash<float> {
	size_t operator()(float x) {
		return (size_t)x;
	}
};
template<>
struct hash<double> {
	size_t operator()(double x) {
		return (size_t)x;
	}
};



template<class T>
struct hashtable_node {
	T value;
	hashtable_node* next;
};
template<class value, class Key, class Hashfcn,
	class Extractkey, class Equalkey, class Alloc>class hashtable;


template<class value,class Key,class Hashfcn,
		 class Extractkey,class Equalkey,class Alloc>
struct hashtable_iterator {
	using hashtable			= hashtable<value, Key,Hashfcn,
								Extractkey,Equalkey,Alloc>;
	using iterator			=hashtable_iterator<value, Key, Hashfcn,
								Extractkey, Equalkey, Alloc>;
	using const_iterator	= const iterator;
	using node				= hashtable_node<value>;

	using iterator_category = forward_iterator_tag;
	using value_type		= value;
	using difference_type	= ptrdiff_t;
	using size_type			= size_t;
	using reference			= value&;
	using pointer			= value*;
	using const_reference	= const reference;
	using const_pointer		= const pointer;
	node* cur;//指向当前节点 
	hashtable* table;
	hashtable_iterator(){}
	hashtable_iterator(node* n,hashtable * tab):cur(n),table(tab){}
	hashtable_iterator(const iterator& x):cur(x.cur),table(x.table){}
	reference operator*()const {
		return cur->value;
	}
	pointer operator->()const {
		return &(operator*());
	}
	bool operator==(const iterator& x)const {
		return cur == x.cur;
	}
	bool operator!=(const iterator& x)const {
		return cur != x.cur;
	}
	
	iterator& operator++() {
		const node* old = cur;
		cur = cur->next;
		if (!cur) {
			size_type bucket = table->bkt_num(old->val);
			while (!cur && ++bucket < table->buckets.size())
				cur = ht->buckets(bucket);
		}
		return *this;
	}
	iterator operator++(int) {
		iterator tmp = *this;
		++(*this);
		return tmp;
	}
};

static const int num_primes = 28;
static const unsigned long prime_list[num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long next_prime(unsigned long __n)
{
	const unsigned long* __first = prime_list;
	const unsigned long* __last = prime_list + num_primes;
	const unsigned long* pos = std::lower_bound(__first, __last, __n);
	return pos == __last ? *(__last - 1) : *pos;
}

template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
inline forward_iterator_tag
iterator_category(const hashtable_iterator<value, Key, Hashfcn,
	Extractkey, Equalkey, Alloc>&) {
	return forward_iterator_tag();
}
template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
inline value* 
value_type(const hashtable_iterator<value, Key, Hashfcn,
		Extractkey, Equalkey, Alloc>&) {
	return (value*)0;
}
template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
inline typename hashtable<value, Key, Hashfcn,Extractkey, Equalkey, Alloc>::difference_type*
distance_type(const hashtable_iterator<value, Key, Hashfcn,
	Extractkey, Equalkey, Alloc>&) {
	return (typename hashtable<value, Key, Hashfcn, Extractkey, 
			Equalkey, Alloc>::difference_type*)0;
}



template<class value, class Key, class Hashfcn,class Extractkey
		, class Equalkey, class Alloc>
class hashtable {
public:
	using key_type			= Key;
	using value_type		= value;
	using hasher			= Hashfcn;
	using key_equal			= Equalkey;
	using allocator_type	= Alloc;

	using difference_type	= ptrdiff_t;
	using size_type			= size_t;
	using reference			= value&;
	using pointer			= value*;
	using const_reference	= const reference;
	using const_pointer		= const pointer;

	hasher hash_funct()const { return hash; }
	key_equal key_eq() const { return equal; }
	allocator_type get_allocator()const { return allocator_type(); }

private:
	using node				= hashtable_node<value>;

private:
	hasher hash;//hash函数
	key_equal equal;//键值比较函数
	Extractkey get_key;//键值获取函数
	myvector<node*, Alloc> buckets;	//使用vector构建的hash表
	size_type num_elements;//元素总数
	float factor;//负载系数
public:
	using iterator =
		hashtable_iterator<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>;
	using const_iterator = const iterator;
	friend struct
		hashtable_iterator<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>;
	//这里定义友元类的原因是为了使iterator有访问hashtable里面私有数据的权限
	//如：访问vector的权限
	hashtable(size_type n,
		const hasher& hs,
		const key_equal& eq,
		const Extractkey& ext=Extractkey(),
		const allocator_type& a = allocator_type()):hash(hs),equal(eq),get_key(ext) {
		buckets(a);
		num_elements = 0;
		initialize_buckets(n);
		
	}
	hashtable(size_type n,
		const hasher& hs,
		const key_equal& eq,
		const allocator_type& a = allocator_type()) :hash(hs), equal(eq), buckets(a) {
		num_elements = 0;
		get_key = Extractkey();
		initialize_buckets(n);
	}
	hashtable(const hashtable& x) :hash(x.hash), equal(x.equal), get_key(x.get_key) {
		buckets(x.get_allocator());
		copy_from(x);
	}
	~hashtable() { 
		clear(); 
		factor = 0; 
	}
	hashtable& operator=(const hashtable& x){
		clear();
		copy_from(x);
		return *this;
	}
	hashtable& operator=(hashtable&& x) {
		clear();
		hash = std::move(x.hash);
		equal = std::move(x.equal);
		num_elements = std::move(x.num_elements);
		buckets = std::move(x.buckets);
		get_key = std::move(x.get_key);
		factor = std::move(x.factor);
		return *this;
	}


	void swap(const hashtable& x) {
		std::swap(hash, x.hash);
		std::swap(get_key, x.get_key);
		std::swap(equal, x.equal);
		buckets.swap(x.buckets);
		std::swap(num_elements, x.num_elements);
	}
private:
	using node_allocator = my_allocator<node>;

	node* create_node(const value_type& obj) {
		node* n = node_allocator::allocate(sizeof(node));
		n->next = nullptr;
		construct(&n->value, obj);
		return n;
	}
	void delete_node(node* n) {
		destory(&n->value);
		node_allocator::deallocate(n);
	}

protected:
	//以下求桶的大小 也就是vector的大小
	size_type bucket_count()const { return buckets.size(); }
	//以下求桶的可能最大值
	size_type max_bucket_count()const { return prime_list[num_primes - 1]; }
	//以下求大于或者等于n的质数 用于创建相应大小的桶数
	size_type next_size(size_type n)const { return next_prime(n); }
	//以下计算vector中单个桶内元素个数
	size_type num_elements_bucket(size_type n)const{
		size_type result = 0;
		for (node* cur = buckets[n]; cur; cur = cur->next)
			result += 1;
		return result;
	}

protected:
	void initialize_buckets(size_type n=0) {
		factor = 1;
		const size_type x = next_size(n);
		buckets.reserve(x);
		buckets.insert(buckets.end(), n, (node*)0);
		num_elements = 0;
	}
	//n为bucket个数 通过get_key取得键值 
	//然后使用hash函数并通过求模n 使bucket位置坐落于正确的位置内
	size_type bkt_num_key(const key_type& x, size_t n ) {
		return hash(key) % n;
	}

	size_type bkt_num(const value_type& x, size_t n = 0)const {
		return bkt_num_key(get_key(x), buckets.size());
	}

public:
	size_type size()const {
		return num_elements;
	}
	size_type max_size()const {
		return size_type(-1);
	}
	bool empty()const {
		return size() == 0;
	}
	void clear() {
		for (size_type n = 0; n < buckets.size(); ++n) {
			node* cur = buckets[n];
			while (cur != 0) {
				node* next = cur->next;
				delete_node(cur);
				cur = next;
			}
			buckets[n] = 0;
		}
		num_elements = 0;
	}


	iterator begin() {
		for (size_type n = 0; n < buckets.size(); ++n) {
			if (buckets[n])
				return iterator(buckets[n], this);
		}
		return end();
	}
	iterator end() { return iterator(0, this); }
	const_iterator cbegin() const{
		for (size_type n = 0; n < buckets.size(); ++n) {
			if (buckets[n])
				return const_iterator(buckets[n], this);
		}
		return cend();
	}
	cons_iterator cend()const { return const_iterator(0, this); }

public:
	void resize(size_type list_num) {
		const size_type buckets_num = buckets.size();
		if (buckets_num < list_num) {
			const size_type next_num = next_size(list_num);
			if (next_num > buckets_num) {
				myvector<node*> tmp(next_num, (node*)0);
				for (size_type old_cur = 0; old_cur < buckets_num; ++old_cur)
				{
					node* first = buckets[old_cur];
					while (first) {
						size_type new_cur = bkt_num(first->value, next_num);
						buckets[x] = first->next;
						first->next = tmp[new_cur];
						tmp[new_cur] = first;
						first = buckets[old_cur];
					}
				}
				buckets.swap(tmp);
			}
		}
	}
	
	//insert_unique
	std::pair<iterator, bool> insert_unique_noresize(const value_type& x) {
		const size_type n = bkt_num(x);
		node* first = buckets[n];
		//下面语句判断是否有重复
		for (node* cur = first; cur; cur = cur->next)
			if (equal(get_key(cur->value), get_key(x)))
				return std::pair<iterator, bool>(iterator(cur, this), false);
		
		//没有重复
		node* tmp = create_node(x);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return std::pair<iterator, bool>(iterator(tmp, this), true);
	}

	std::pair<iterator, bool> insert_unique(const value_type& x) {
		resize(num_elements + 1);
		return insert_unique_noresize(x);
	} 
	iterator insert_unique(const_iterator position, const value_type& x) {
		resize(num_elements + 1);
		if (bkt_num(position.cur->value) == bkt_num(x)) {
			for (node* cur = position.cur; cur; cur = cur->next)
				if (equal(get_key(cur->value), get_key(x)))
					return iterator(cur, this);

			node* tmp = create_node(x);
			tmp->next = buckets[n];
			buckets[n] = tmp;
			++num_elements;
			return iterator(tmp, this);
		}
		else
			return insert_unique_noresize(x);
	}
	template <class _InputIterator>
	void insert_unique(_InputIterator first, _InputIterator last) {
		if (iterator_category(first) == forward_iterator_tag) {
			size_type n = std::distance(first, last);;
			resize(num_elements + n);
			for (; n > 0; --n, ++first)
				insert_unique_noresize(*first);
		}
		else if (iterator_category(first) == input_iterator_tag) {
			for (; first != last; ++first)
				insert_unique(*first);
		}
	}

	void insert_unique(value_type* first,  value_type* last) {
		size_type n = last - first;
		resize(num_elements + n);
		for (; first!=last; ++first) {
			insert_unique(*first);
		}
	}
	void insert_unique(iterator __f, iterator __l)
	{
		size_type __n = 0;
		std::distance(__f, __l, __n);
		resize(_M_num_elements + __n);
		for (; __n > 0; --__n, ++__f)
			insert_unique_noresize(*__f);
	}

	void insert_unique(std::initializer_list<value_type>& x) {
		using il_iterator = typename std::initializer_list<value_type>::iterator;
		il_iterator first = x.begin();
		resize(num_elements + x.size());
		for (; first != x.end(); ++first) {
			insert_unique_noresize(*first);
		}
	}




	//insert_equal
	iterator insert_equal_noresize(const value_type& x) {
		const size_type n = bkt_num(x);
		node* first = buckets[n];

		for (node* cur = first; cur; cur = cur->next)
			if (equal(get_key(cur->value), get_key(x))) {
				node* tmp = create_node(x);
				tmp->next = cur->next;
				cur->next = tmp;
				++num_elements;
				return iterator(tmp,this);
			}
		node* tmp = create_node(x);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return iterator(tmp, this);
	}
	iterator insert_equal(const value_type& x) {
		resize(num_elements + 1);
		return insert_equal_noresize(x);
	}
	iterator insert_equal(const_iterator position, const value_type& x) {
		resize(num_elements + 1);
		if (bkt_num(position.cur->value) == bkt_num(x)) {
			for (node* cur = position.cur; cur; cur = cur->next)
				if (equal(get_key(cur->value), get_key(x))) {
					node* tmp = create_node(x);
					tmp->next = cur->next;
					cur->next = tmp;
					++num_elements;
					return iterator(tmp, this);
				}

			node* tmp = create_node(x);
			tmp->next = buckets[n];
			buckets[n] = tmp;
			++num_elements;
			return iterator(tmp, this);
		}
		else
			return insert_equal_noresize(x);
	}

	template <class _InputIterator>
	void insert_equal(_InputIterator first, _InputIterator last) {
		if (iterator_category(first) == forward_iterator_tag) {
			size_type n = std::distance(first, last);;
			resize(num_elements + n);
			for (; n > 0; --n, ++first)
				insert_equal_noresize(*first);
		}
		else if (iterator_category(first) == input_iterator_tag) {
			for (; first != last; ++first)
				insert_equal(*first);
		}
	}
	void insert_equal(value_type* __f, value_type* __l)
	{
		size_type __n = __l - __f;
		resize(_M_num_elements + __n);
		for (; __n > 0; --__n, ++__f)
			insert_equal_noresize(*__f);
	}
	void insert_equal(iterator __f, iterator __l)
	{
		size_type __n = 0;
		std::distance(__f, __l, __n);
		resize(_M_num_elements + __n);
		for (; __n > 0; --__n, ++__f)
			insert_equal_noresize(*__f);
	}
	void insert_equal(std::initializer_list<value_type>& x) {
		using il_iterator = typename std::initializer_list<value_type>::iterator;
		il_iterator first = x.begin();
		resize(num_elements + x.size());
		for (; first != x.end(); ++first) {
			insert_equal_noresize(*first);
		}
	}

public:
	//针对单个bucket的删除
	void erase_bucket(const size_type n, node* last) {
		node* cur = buckets[n];
		while (cur != last) {
			node* next = cur->next;
			delete_node(cur);
			buckets[n] = next;
			cur = next;
			--num_elements;
		}
	}
	void erase_bucket_num(const size_type n, node* first, node* last) {
		node* cur = buckets[n];
		if (cur == first)
			erase_bucket(n, last);
		else {
			node* next;
			for (next = cur->next; next != first; cur = next, next = cur->next);
			while (next != last) {
				cur = next->next;
				delete_node(next);
				next = cur->next;
				--num_elements;
			}
		}
	}
	void erase(const iterator& x) {
		if (node* p = x.cur) {
			const size_type n = bkt_num(p->value);
			node* cur = buckets[n];
			node* re;
			if (cur == p)
			{
				buckets[n] = cur->next;
				re = cur->next;
				delete_node(cur);
				--num_elements;
				return iterator(re, this);
			}
			else {
				node* next = cur->next;
				while (next) {
					if (next == p) {
						cur->next = next->next;
						re = next->next;
						delete_node(next);
						--num_elements;
						return iterator(re, this);
					}
					else {
						cur = next;
						next = next->next;
					}
				}
			}
		}
	}

	size_type erase(const key_type& k) {
		size_type n = 0;
		size_type position = bkt_num_key(k);

		node* next;
		for (node* cur = ht.buckets[position]; cur; ++cur)
			if (get_key(cur->value) == k) {
				next = cur->next;
				delete_node(cur);
				cur = next;
				--num_elements;
				++n;
			}

		return n;
	}

	void erase(iterator first,iterator last) {
		size_type f_bucket = first->cur ? bkt_num(first.cur->value) : buckets.size();
		size_type l_bucket = last->cur ? bkt_num(last.cur->value) : buckets.size();
		if (first.cur == last.cur)
			return;
		else if (f_bucket == l_bucket)
			erase_bucket_num(f_bucket,first.cur,last.cur)
		else {
			erase_bucket_num(f_bucket,first.cur,(node*)0);
			for (size_type n = f_bucket + 1; n < l_bucket; ++n)
				erase_bucket(n, (node*)0);
			if(l_bucket!=buckets.size())
			erase_bucket(l_bucket, last.cur);
		}
	}
	//这个用于处理常量性质的map和set
	void erase(const_iterator __first,const_iterator __last)
	{
		erase(iterator(const_cast<node*>(__first.cur),
			const_cast<hashtable*>(__first.table)),
			iterator(const_cast<node*>(__last.cur),
				const_cast<hashtable*>(__last.table)));
	}
	void erase(const_iterator& it) {
		erase(iterator(const_cast<node*>(it.cur),
			const_cast<hashtable*>(it.table)));
	}

public:
	iterator find(const key_type& key) {
		const size_type n = bkt_num(key);
		node* first;
		for(first=buckets[n];
			first&&!get_key(first->value)==key;
			first=first->next){ }
		return iterator(first, this);
	}
	const_iterator find(const key_type& key)const {
		const size_type n = bkt_num(key);
		node* first;
		for (first = buckets[n];
			first && !get_key(first->value) == key;
			first = first->next) {
		}
		return const_iterator(first, this);
	}
	size_type count(const key_type& key)const {
		const size_type n = bkt_num(key);
		size_type result = 0;
		for (node* cur = buckets[n]; cur; cur = cur->next) {
			if (equal(get_key(cur->value), key))
				++result;
		}
		return result;
	}

	std::pair<iterator, iterator> equal_range(const key_type& key) {
		size_type n = bkt_num_key(key);
		for (node* first = buckets[n]; first; first = first->next) {
			if (equal(get_key(first->value), key)) {
				for (node* cur = first->next; cur; cur = cur->next)
					if (!equal(get_key(cur->value), key))
						return std::pair<iterator, iterator>(
							iterator(first, this), iterator(cur, this));
				for(size_type m=n+1;m<buckets.size();++m)
					if(buckets[m])
						return std::pair<iterator, iterator>(
							iterator(first, this), iterator(buckets[m], this));
				return std::pair<iterator, iterator>(
					iterator(first, this), end());
			}
		}
		return std::pair<iterator, iterator>(end(), end());
	}

	std::pair<const_iterator, const_iterator> equal_range(const key_type& key)const {
		const size_type n = bkt_num_key(key);
		for (const node* first = buckets[n]; first; first = first->next) {
			if (equal(get_key(first->value), key)) {
				for (const node* cur = first->next; cur; cur = cur->next)
					if (!equal(get_key(cur->value), key))
						return std::pair<const_iterator, const_iterator>(
							const_iterator(first, this), const_iterator(cur, this));
				for (size_type m = n + 1; m < buckets.size(); ++m)
					if (buckets[m])
						return std::pair<const_iterator, const_iterator>(
							const_iterator(first, this), const_iterator(buckets[m], this));
				return std::pair<const_iterator, const_iterator>(
					const_iterator(first, this), end());
			}
		}
		return std::pair<iterator, iterator>(end(), end());
	}
protected:
	void copy_from(const hashtable& tab) {
		buckets.clear();
		factor = tab.factor;
		buckets.reserve(tab.buckets.size());
		buckets.insert(buckets.end(),buckets.size(),(node*)0);
		for (size_type n = 0; n < buckets.size(); ++n;) {
			if (const node* cur = tab.buckets[n]) {
				//星号左侧const要求不可以修改指针指向的值
				node* copy = create_node(cur->value);
				buckets[n] = copy;
				for (node* next = cur->next;
					next;
					cur=next,next = cur->next) {
					copy->next=create_node(cur->value);
					copy = copy->next;
				}
			}
		}
		num_elements = tab.num_elements;
	}

	reference find_or_insert(const value& x) {
		resize(num_elements + 1);
		size_type n = bkt_num(x);
		node* first = buckets[n];
		for (node* cur = first; cur; cur = cur->next)
			if (equal(get_key(cur->value), get_key(x)))
				return cur->value;
		
		node* tmp = create_node(x);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return tmp->value;
	}

};


template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
inline bool operator==(
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& x,
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& y) {
	using node = typename hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::node;
	if (x.buckets.size() != y.buckets.size())
		return false;
	for (size_type n = 0; n < x.buckets.size(); ++n) {
		node* x_cur = x.buckets[n];
		node* y_cur = y.buckets[n];
		for(;x_cur&&y_cur&&x_cur->value==y_cur->value;
			x_cur=x_cur->next,y_cur=y_cur->next){ }
		if (x_cur || y_cur)
			return false;
	}
	return true;
}


template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
	inline bool operator!=(
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& x,
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& y) {
	return !(operator==(x, y));
}

template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
	inline bool operator<(
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& x,
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& y) {
	if (x.buckets.size() > y.buckets.size())
		return false;
	for (size_type n = 0; n < x.buckets.size(); ++n) {
		node* x_cur = x.buckets[n];
		node* y_cur = y.buckets[n];
		for (; x_cur && y_cur && x_cur->value < y_cur->value;
			x_cur = x_cur->next, y_cur = y_cur->next) {
		}
		if (x_cur)
			return false;
	}
	return true;
}

template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
	inline bool operator>(
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& x,
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& y) {
	return operator<(y, x);
}
template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
	inline bool operator<=(
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& x,
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& y) {
	return !(operator<(y, x));
}
template<class value, class Key, class Hashfcn, class Extractkey
	, class Equalkey, class Alloc>
	inline bool operator>=(
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& x,
		const hashtable<value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& y) {
	return !(operator<(x, y));
}

#endif 

