#pragma once
#ifndef MY_STL_ALGOBASE
#define MY_STL_ALGOBASE
#include"mytype_traits.h"
#include"my_iterator.h"
#include<algorithm>
#include"my_numeric.h"
#include"stl_heap.h"

template<class Inputiterator1,class Inputiterator2>
inline bool equal(Inputiterator1 first1, Inputiterator1 last, Inputiterator2 first2) {
	for (; first1 != last; ++first1, ++first2)
		if (*first1 != *first2)
			return false;
	return true;
}
template<class Inputiterator1, class Inputiterator2,class Binaryoperation>
inline bool equal(Inputiterator1 first1, Inputiterator1 last, Inputiterator2 first2
,Binaryoperation binary_op) {
	for (; first1 != last; ++first1, ++first2)
		if (!binary_op(*first1,*first2))
			return false;
	return true;
}

template<class Forwarditerator,class T>
void fill(Forwarditerator first, Forwarditerator last, const T& value) {
	for (; first != last; ++first)
		*first = value;
}

template<class Outputiterator,class Size,class T>
void fill_n(Outputiterator first, Size n, const T& value) {
	for (; n>0; ++first, --n)
		*first = value;
}


template<class Forwarditerator1,class Forwarditerator2>
inline void iter_swap(Forwarditerator1 a, Forwarditerator2 b) {
	typename iterator_traits<Forwarditerator1>::value_type tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

template<class Inputiterator1,class Inputiterator2>
bool lexicographical_compare(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (*first1 < *first2)
			return true;
		else if (*first2 < *first1)
			return false;
	}
	return first1 == last1 && first2 != last2;
}
template<class Inputiterator1, class Inputiterator2,class Binaryoperation>
bool lexicographical_compare(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,Binaryoperation binary_op) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (binary_op(*first1,*first2))
			return true;
		else if (binary_op(*first2,*first1))
			return false;
	}
	return first1 == last1 && first2 != last2;
}

template<class T>
inline const T& max(const T& a, const T& b) {
	return a < b ? b : a;
}
template<class T,class Compare>
inline const T& max(const T& a, const T& b,Compare comp) {
	return comp(a, b) ? b : a;
}
template<class T>
inline const T& min(const T& a, const T& b) {
	return a < b ? a : b;
}
template<class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
	return comp(a, b) ? a : b;
}

template<class Inputiterator1,class Inputiterator2>
std::pair<Inputiterator1, Inputiterator2> mismatch(Inputiterator1 first1,
	Inputiterator1 last, Inputiterator2 first2) {
	while (first1 != last && *first1 == *first2) {
		++first1;
		++first2;
	}
	return std::pair<Inputiterator1, Inputiterator2>(first1, first2);
}
template<class Inputiterator1, class Inputiterator2,class Binarypredicate>
std::pair<Inputiterator1, Inputiterator2> mismatch(Inputiterator1 first1,
	Inputiterator1 last, Inputiterator2 first2,Binarypredicate pred) {
	while (first1 != last && pred(*first1,*first2)) {
		++first1;
		++first2;
	}
	return std::pair<Inputiterator1, Inputiterator2>(first1, first2);
}

template<class T>
inline void swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}





template<class Randomaccessit,class Outputiterator,class Distance>
inline Outputiterator _copy_d(Randomaccessit first, Randomaccessit last
	, Outputiterator result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first)
		*result = *first;//operator assignment
	return result;
}
template<class T>
inline T* _copy_t(const T* first, const T* last, T* result, true_type) {
	::memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}
template<class T>
inline T* _copy_t(const T* first, const T* last, T* result, false_type) {
	return ::_copy_d(first, last, result,(ptrdiff_t*)0);//distance_type(T*)
}

template<class Inputiterator,class Outputiterator>
inline Outputiterator _copy(Inputiterator first, Inputiterator last,
	Outputiterator result, input_iterator_tag) {
	for (; first != last; ++first,++result) {
		*result = *first;//operator assignment
	}
	return result;
}
template<class Randomaccessit, class Outputiterator>
inline Outputiterator _copy(Randomaccessit first, Randomaccessit last,
	Outputiterator result, random_access_iterator_tag) {
	return ::_copy_d(first, last, result, distance_type(first));
}


template<class Inputiterator,class Outputiterator>
struct _copy_dispatch {
	Outputiterator operator()(Inputiterator first, Inputiterator last,
		Outputiterator result) {
		return ::_copy(first, last, result,iterator_category(first));
	}
};
template<class T>
struct _copy_dispatch<T*, T*> {
	T* operator()(T* first, T* last, T* result) {
		using has_t = typename mytype_traits<T>::has_trivial_assignment;
		return ::_copy_t(first, last, result, has_t());
	}
};
template<class T>
struct _copy_dispatch<const T*, T*> {
	T* operator()(const T* first, const T* last, T* result) {
		using has_t = typename mytype_traits<T>::has_trivial_assignment;
		return ::_copy_t(first, last, result, has_t());
	}
};


template<class Inputiterator,class Outputiterator>
inline Outputiterator copy(Inputiterator first, Inputiterator last,
	Outputiterator result) {
	return _copy_dispatch<Inputiterator, Outputiterator>()(first, last, result);
}
inline char* copy(char* first, char* last,
	char* result) {
	::memmove(result, first, last - first);
	return result + (last - first);
}
inline wchar_t* copy(wchar_t* first, wchar_t* last,
	wchar_t* result) {
	::memmove(result, first, sizeof(wchar_t)*(last - first));
	return result + (last - first);
}







template<class Randomaccessit, class Biderectionaliterator, class Distance>
inline Biderectionaliterator _copy_backward_d(Randomaccessit first, Randomaccessit last
	, Biderectionaliterator result, Distance*) {
	for (Distance n = last - first; n > 0; --n, --result)
		*result = *(--last);//operator assignment
	return result;
}
template<class T>
inline T* _copy_backward_t(const T* first, const T* last, T* result, true_type) {
	::memmove((result+(last-first)), first, (sizeof(T) * (last - first)));
	return result + (last - first);
}
template<class T>
inline T* _copy_backward_t(const T* first, const T* last, T* result, false_type) {
	return ::_copy_backward_d(first, last, result, (ptrdiff_t*)0);//distance_type(T*)
}

template<class Biderectionaliterator1, class Biderectionaliterator2>
inline Biderectionaliterator2 _copy_backward(Biderectionaliterator1 first, Biderectionaliterator1 last,
	Biderectionaliterator2 result, bidirectional_iterator_tag) {
	for (; first != last; --result) {
		*result = *(--last);//operator assignment
	}
	return result;
}
template<class Randomaccessit, class Biderectionaliterator2>
inline Biderectionaliterator2 _copy_backward(Randomaccessit first, Randomaccessit last,
	Biderectionaliterator2 result, random_access_iterator_tag) {
	return ::_copy_backward_d(first, last, result, distance_type(first));
}


template<class Biderectionaliterator1, class Biderectionaliterator2>
struct _copy_backward_dispatch {
	Biderectionaliterator2 operator()(Biderectionaliterator1 first, Biderectionaliterator1 last,
		Biderectionaliterator2 result) {
		return ::_copy_backward(first, last, result, iterator_category(first));
	}
};
template<class T>
struct _copy_backward_dispatch<T*, T*> {
	T* operator()(T* first, T* last, T* result) {
		using has_t = typename mytype_traits<T>::has_trivial_assignment;
		return ::_copy_backward_t(first, last, result, has_t());
	}
};
template<class T>
struct _copy_backward_dispatch<const T*, T*> {
	T* operator()(const T* first, const T* last, T* result) {
		using has_t = typename mytype_traits<T>::has_trivial_assignment;
		return ::_copy_backward_t(first, last, result, has_t());
	}
};

template<class Biderectionaliterator1,class Biderectionaliterator2>
inline Biderectionaliterator2 copy_backward(Biderectionaliterator1 first, Biderectionaliterator1 last,
	Biderectionaliterator2 result) {
	return _copy_backward_dispatch<Biderectionaliterator1, Biderectionaliterator2>
		()(first, last, result);
}



template<class Inputiterator1,class Inputiterator2,class Outputiterator>
Outputiterator set_union(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2, Outputiterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
		}
		else if (*first2 < *first1) {
			*result = *first2;
			++first2;
		}
		else {//此处判定相等
			*result = *first1;
			++first1;
			++first2;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}
template<class Inputiterator1, class Inputiterator2, class Outputiterator
,class predicate>
Outputiterator set_union(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2, Outputiterator result
,predicate pred) {
	while (first1 != last1 && first2 != last2) {
		if (pred(*first1,*first2)) {
			*result = *first1;
			++first1;
		}
		else if (pred(*first2, *first1)) {
			*result = *first2;
			++first2;
		}
		else {//此处判定相等
			*result = *first1;
			++first1;
			++first2;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}


template<class Inputiterator1,class Inputiterator2,class Outputiterator>
Outputiterator set_intersection(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,
	Outputiterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2)
			++first1;
		else if (*first2 < first1)
			++first2;
		else {//相等情况下 以第一序列为基准
			*result = *first1;
			++first1;
			++first2;
			++result;
		}
	}
	return result;
}
template<class Inputiterator1, class Inputiterator2, class Outputiterator
,class predicate>
Outputiterator set_intersection(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,
	Outputiterator result,predicate pred) {
	while (first1 != last1 && first2 != last2) {
		if (pred(*first1,*first2))
			++first1;
		else if (pred(*first2, *first1))
			++first2;
		else {//相等情况下 以第一序列为基准
			*result = *first1;
			++first1;
			++first2;
			++result;
		}
	}
	return result;
}



template<class Inputiterator1, class Inputiterator2, class Outputiterator >
Outputiterator set_difference(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,
	Outputiterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < *first1)
			++first2;
		else {
			++first1;
			++first2;
		}
	}
	return copy(first1, last1, result);
	}

template<class Inputiterator1, class Inputiterator2, class Outputiterator
	, class predicate>
	Outputiterator set_difference(Inputiterator1 first1, Inputiterator1 last1,
		Inputiterator2 first2, Inputiterator2 last2,
		Outputiterator result, predicate pred) {
	while (first1 != last1 && first2 != last2) {
		if (pred(*first1,*first2)) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (pred(*first2, *first1))
			++first2;
		else {
			++first1;
			++first2;
		}
	}
	return copy(first1,last1,result);
}

template<class Inputiterator1, class Inputiterator2, class Outputiterator>
Outputiterator set_symmetric_difference(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,
	Outputiterator result) {//对称差集实际上就是剔除相同值的有序数列
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < *first1) {
			*result = *first2;
			++first2;
			++result;
		}
		else {
			++first1;
			++first2;
		}
	}
	return copy(first2, last2, copy(first1, last1, result));
}
template<class Inputiterator1, class Inputiterator2, class Outputiterator,
class predicate>
Outputiterator set_symmetric_difference(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,
	Outputiterator result,predicate pred) {//对称差集实际上就是剔除相同值的有序数列
	while (first1 != last1 && first2 != last2) {
		if (pred(*first1, *first2)) {
			*result = *first1;
			++first1;
			++result;
		}
		else if (pred(*first2, *first1)) {
			*result = *first2;
			++first2;
			++result;
		}
		else {
			++first1;
			++first2;
		}
	}
	return copy(first2, last2, copy(first1, last1, result));
}


template<class Forwarditerator>
Forwarditerator adjacent_find(Forwarditerator first, Forwarditerator last) {
	if (first == last)return last;
	Forwarditerator next = first;
	while (++next != last) {
		if (*first == *next)return first;
		first = next;
	}
	return last;
}
template<class Forwarditerator,class Binarypredicate>
Forwarditerator adjacent_find(Forwarditerator first, Forwarditerator last,
	Binarypredicate binary_pred) {
	if (first == last)return last;
	Forwarditerator next = first;
	while (++next != last) {
		if (binary_pred(*first == *next))return first;
		first = next;
	}
	return last;
}

template<class Inputiterator,class T>
typename iterator_traits<Inputiterator>::difference_type
count(Inputiterator first, Inputiterator last,const T& value) {
	typename iterator_traits<Inputiterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (*first == value)++n;
	}
	return n;
}
template<class Inputiterator,class predicate>
typename iterator_traits<Inputiterator>::difference_type
count_if(Inputiterator first, Inputiterator last, predicate pred) {
	typename iterator_traits<Inputiterator>::difference_type n = 0;
	for (; first != last; ++first) {
		if (pred(*first))++n;
	}
	return n;
}

template<class Inputiterator,class T>
Inputiterator find(Inputiterator first, Inputiterator last, const T& value) {
	while (first != last && *first != value)++first;
	return first;
}
template<class Inputiterator, class Predicate>
Inputiterator find_if(Inputiterator first, Inputiterator last, Predicate pred) {
	while (first != last && !pred(*first))++first;
	return first;
}




template<class Bidirectionaliterator1, class Bidirectionaliterator2>
Bidirectionaliterator1 _find_end(Bidirectionaliterator1 first1, Bidirectionaliterator1 last1,
	Bidirectionaliterator2 first2, Bidirectionaliterator2 last2,
	bidirectional_iterator_tag, bidirectional_iterator_tag) {
	using reviter1 = reverse_iterator<Bidirectionaliterator1>;
	using reviter2 = reverse_iterator<Bidirectionaliterator2>;
	reviter1 rfirst1(first1);
	reviter2 rfirst2(first2);
	reviter1 result = ::search(reviter1(last1), rfirst1, reviter2(last2), rfirst2);
	if (result == rfirst1)return last1;
	else {
		Bidirectionaliterator1 real_result = result.base();
		std::advance(real_result, -distance(first2, last2));
		return real_result;
	}
}
template<class Forwarditerator1,class Forwarditerator2>
Forwarditerator1 _find_end(Forwarditerator1 first1, Forwarditerator1 last1,
	Forwarditerator2 first2, Forwarditerator2 last2,
	forward_iterator_tag, forward_iterator_tag) {
	if (first2 == last2)return last1;
	else {
		Forwarditerator1 result = last1;
		while (true) {
			Forwarditerator1 new_result = search(first1, last1, first2, last2);
			if (new_result == last1)
				return result;
			else {
				result = new_result;
				first1 = new_result;
				++first1;
			}
		}
	}
}
template<class Forwarditerator1,class Forwarditerator2>
inline Forwarditerator1
find_end(Forwarditerator1 first1, Forwarditerator1 last1,
	Forwarditerator2 first2, Forwarditerator2 last2) {
	using category1 = typename iterator_traits<Forwarditerator1>::iterator_category;
	using category2 = typename iterator_traits<Forwarditerator2>::iterator_category;
	return ::_find_end(first1, last1, first2, last2,category1(),category2());
}

template<class Inputiterator,class Forwarditerator>
Inputiterator find_first_of(Inputiterator first1, Inputiterator last1,
	Forwarditerator first2, Forwarditerator last2) {
	for (; first1 != last1; ++first1)
		for (Forwarditerator it = first2; it != last2; ++it)
			if (*it == *first1)
				return first1;
}
template<class Inputiterator, class Forwarditerator,class BinaryPredicate>
Inputiterator find_first_of(Inputiterator first1, Inputiterator last1,
	Forwarditerator first2, Forwarditerator last2, BinaryPredicate binary_pred) {
	for (; first1 != last1; ++first1)
		for (Forwarditerator it = first2; it != last2; ++it)
			if (binary_pred(*first1,*it))
				return first1;
}

template<class Inputiterator,class Function>
Function for_each(Inputiterator first, Inputiterator last, Function func) {
	for (; first != last; ++first)
		func(*first);
	return func;
}

template<class Forwarditerator,class Generator>
void generate(Forwarditerator first, Forwarditerator last, Generator g) {
	for (; first != last; ++first) {
		*first = g();
	}
}

template<class Forwarditerator,class Size,class Generator>
void generate_n(Forwarditerator first, Size n, Generator g) {
	for (; n > 0; ++first)
		*first = g();
}


template<class Forwarditerator1,class Forwarditerator2,class Distance1,class Distance2>
Forwarditerator1 _search(Forwarditerator1 first1,Forwarditerator1 last1,
	Forwarditerator2 first2,Forwarditerator2 last2,Distance1*,Distance2*) {
	//先计算长度 避免无效搜索
	Distance1 onelen = distance(first1,last1);
	Distance2 twolen = distance(first2, last2);
	if (onelen < twolen)return last1;

	Forwarditerator1 current1 = first1;
	Forwarditerator2 current2 = first2;
	while (current2 != last2) {//以第二序列长度为基准 在第一序列中查找第二序列
		if (*current1 == *current2) {
			++current1;
			++current2;
		}
		else {
			if (onelen == twolen)
				return last1;
			else {
				current1=++first1;
				current2 = first2;
				--onelen;
			}
		}
	}
	return first1;//返回更新之后的first1;
}

template<class Forwarditerator1,class Forwarditerator2>
inline Forwarditerator1 search(Forwarditerator1 first1,Forwarditerator1 last1,
	Forwarditerator2 first2,Forwarditerator2 last2) {
	return ::_search(first1, last1, first2, last2,
		distance_type(first1), distance_type(first2));
}

template<class Forwarditerator,class Integer,class T>
Forwarditerator search_n(Forwarditerator first, Forwarditerator last, Integer count
	, const T& value) {
	if (count <= 0)return first;
	else {
		first = ::find(first, last, value);
		while (first != last) {
			Integer n = count - 1;
			Forwarditerator i = first;
			++i;
			while (i != last && n != 0 && *i == value) {
				++i;
				--n;
			}
			if (n ==0)
				return first;
			else
				first = ::find(first, last, value);
		}
		return last;
	}
}

template<class Forwarditerator, class Integer, class T,class Binarypredicate>
Forwarditerator search_n(Forwarditerator first, Forwarditerator last, Integer count
	, const T& value,Binarypredicate binary_pred) {
	if (count <= 0)return first;
	else {
		while (first != last)
			if (binary_pred(*first, value))break;
		++first;
		while (first != last) {
			Integer n = count - 1;
			Forwarditerator i = first;
			++i;
			while (i != last && n != 0 && binary_pred(*i,value)) {
				++i;
				--n;
			}
			if (n == 0)
				return first;
			else
				while (first != last) {
					if (binary_pred(*i, value))break;
					++i;
				}
		}
		return last;
	}
}


template<class Inputiterator1,class Inputiterator2>
bool includes(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2) {
	while (first1 != last1 && first2 != last2)//默认是通过小于进行比较 
		if (*first1 < *first2)
			++first1;
		else if (*first2 < *first1)//因为是有序
			return false;
		else {
			++first1;
			++first2;
		}
	return first2 == last2;
}

template<class Inputiterator1, class Inputiterator2,class Compare>
bool includes(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,Compare comp) {
	while (first1 != last1 && first2 != last2)//默认是通过小于进行比较 
		if (comp(*first1,*first2))
			++first1;
		else if (comp(*first2, *first1))//因为是有序
			return false;
		else {
			++first1;
			++first2;
		}
	return first2 == last2;
}


template<class Forwarditerator>//要求读写 最低要求是forward
Forwarditerator max_element(Forwarditerator first, Forwarditerator last) {
	if (first == last)return first;
	Forwarditerator result = first;
	while (++first != last)
		if (*result < *first)//更新岗哨
			result = first;
	return result;
}
template<class Forwarditerator,class Compare>//要求读写 最低要求是forward
Forwarditerator max_element(Forwarditerator first, Forwarditerator last,Compare comp) {
	if (first == last)return first;
	Forwarditerator result = first;
	while (++first != last)
		if (comp(*result,*first))//更新岗哨
			result = first;
	return result;
}


template<class Forwarditerator>//要求读写 最低要求是forward
Forwarditerator min_element(Forwarditerator first, Forwarditerator last) {
	if (first == last)return first;
	Forwarditerator result = first;
	while (++first != last)
		if (*first < *result)//更新岗哨
			result = first;
	return result;
}
template<class Forwarditerator,class Compare>//要求读写 最低要求是forward
Forwarditerator min_element(Forwarditerator first, Forwarditerator last,Compare comp) {
	if (first == last)return first;
	Forwarditerator result = first;
	while (++first != last)
		if (comp(*first,*result))//更新岗哨
			result = first;
	return result;
}


template<class Inputiterator1,class Inputiterator2,class Outputiterator>
Outputiterator merge(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2, Outputiterator result) {
	while (first1 != last1 && first2 != last2) {
		if (*first2 < *first1) {//先比较first2 然后再去比较first1 
			//可以减少代码 并且保证稳定
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}
template<class Inputiterator1, class Inputiterator2, class Outputiterator,
class Compare>
Outputiterator merge(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2, Outputiterator result,Compare comp) {
	while (first1 != last1 && first2 != last2) {
		if (comp(*first2,*first1)) {//先比较first2 然后再去比较first1 
			//可以减少代码 并且保证稳定
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}


template<class Bidirectionaliterator,class Predicate>
Bidirectionaliterator partition(Bidirectionaliterator first, Bidirectionaliterator last,
	Predicate pred) {
	while (true) {
		while (true)//此处while判定符合条件部分 
			if (first == last)//这里表示比较完了
				return first;
			else if (pred(*first))//符合前半部分不移动条件
				++first;
			else
				break;
		
		--last;//修改last至暂未比较的最后一个元素
		
		while (true)
			if (first == last)
				return first;
			else if (!pred(*last))
				--last;
			else
				break;

		::iter_swap(first, last);
		++first;//更新first继续进行比较
	}
}//非stable 没有保持稳定


template<class Inputiterator,class Outputiterator,class T>
Outputiterator remove_copy(Inputiterator first, Inputiterator last,
	Outputiterator result, const T& value) {
	while (first != last) {
		if (*first != value) {
			*result = *first;
			++result;
		}
		++first;
	}
	return result;
}
template<class Inputiterator, class Outputiterator,class Predicate>
Outputiterator remove_copy_if(Inputiterator first, Inputiterator last,
	Outputiterator result,Predicate pred) {
	while (first != last) {
		if (pred(*first)) {
			*result = *first;
			++result;
		}
		++first;
	}
	return result;
}


template<class Forwarditerator,class T>
Forwarditerator remove(Forwarditerator first, Forwarditerator last, const T& value) {
	first = ::find(first, last, value);//first属于被输出的新位置
	Forwarditerator next = first;//next属于新比较位置

	//已经查找出来的first不需要管理 直接当作remove_copy的新位置进行输出即可
	return first == last ? first : ::remove_copy(++next, last, first, value);
}

template<class Forwarditerator, class Predicate>
Forwarditerator remove_if(Forwarditerator first, Forwarditerator last, Predicate pred) {
	first = ::find_if(first, last, pred);//first属于被输出的新位置
	Forwarditerator next = first;//next属于新比较位置

	//已经查找出来的first不需要管理 直接当作remove_copy_if的新位置进行输出即可
	return first == last ? first : ::remove_copy_if(++next, last, first, pred);
}


template<class Forwarditerator,class T>
void replace(Forwarditerator first, Forwarditerator last, const T& old_value
	,const T& new_value) {
	for (; first != last; ++first)
		if (*first == old_value)
			*first = new_value;
}

template<class Forwarditerator, class T,class Predicate>
void replace_if(Forwarditerator first, Forwarditerator last 
	,const T& new_value,Predicate pred) {
	for (; first != last; ++first)
		if (pred(*first))
			*first = new_value;
}

template<class Inputiterator,class Outputiterator,class T>
Outputiterator replace_copy(Inputiterator first, Inputiterator last
	, Outputiterator result, const T& old_value,const T& new_value) {
	for (; first != last; ++first, ++result)
		*result = (*first == old_value ? new_value : *first);
	return result;
}

template<class Inputiterator,class Outputiterator,class T,class Predicate>
Outputiterator replace_copy_if(Inputiterator first, Inputiterator last
	, Outputiterator result, const T& new_value, Predicate pred) {
	for (; first != last; ++first, ++result)
		*result = (pred(*first) ? new_value : *first);
	return result;
}


template<class Bidirectionaliterator>
void _reverse(Bidirectionaliterator first, Bidirectionaliterator last, bidirectional_iterator_tag) {
	while (true)
		if (first == last )
			return;
		else
			iter_swap(first++, --last);
}
template<class Randomaccessiterator>
void _reverse(Randomaccessiterator first, Randomaccessiterator last, random_access_iterator_tag) {
	while (first < last)
		iter_swap(first++, --last);
}
template<class Bidirectionaliterator>
inline void reverse(Bidirectionaliterator first, Bidirectionaliterator last) {
	_reverse(first, last, iterator_category(first));
}


template<class Bidirectionaliterator,class Outputiterator>
Outputiterator reverse_copy(Bidirectionaliterator first, Bidirectionaliterator last,
	Outputiterator result) {
	while (first != last) {
		*result = *(--last);
		++result;
	}
	return result;
		
}


template<class Forwarditerator, class Distance>
void _rotate(Forwarditerator first, Forwarditerator middle, Forwarditerator last
	, Distance*, forward_iterator_tag) {
	for (Forwarditerator i = middle;;) {
		iter_swap(first, i);
		++first;
		++i;
		if (first == middle) {
			if (i == last)return;
			middle = i;
		}
		else if (i == last)
			i = middle;
	}
}

template<class Bidirectional, class Distance>
void _rotate(Bidirectional first, Bidirectional middle, Bidirectional last
	, Distance*, bidirectional_iterator_tag) {
	reverse(first, middle);
	reverse(middle, last);
	reverse(first, last);
}



template<class EuclideanRingElement>
EuclideanRingElement _gcd(EuclideanRingElement m, EuclideanRingElement n) {
	while (n != 0) {
		EuclideanRingElement t = m % n;
		m = n;
		n = t;
	}
	return m;
}
template<class Randomaccessiterator,class Distance,class T>
void _rotate_cycle(Randomaccessiterator first, Randomaccessiterator last
	, Randomaccessiterator initial, Distance shift, T*) {
	T value = *initial;
	Randomaccessiterator p1 = initial;
	Randomaccessiterator p2 = p1 + shift;
	while (p2 != initial) {
		*p1 = *p2;
		p1 = p2;
		if (last - p2 > shift)
			p2 += shift;
		else
			p2 = first + (shift - (last - p2));
	}
	*p1 = value;
}


template<class Randomaccessiterator, class Distance>
void _rotate(Randomaccessiterator first, Randomaccessiterator middle
	, Randomaccessiterator last, Distance*, random_access_iterator_tag) {
	Distance n = _gcd(last - first, middle - first);
	while (n--)
		_rotate_cycle(first, last, (first + n), (middle - first), value_type(first));
}

template<class Forwarditerator>
inline void rotate(Forwarditerator first, Forwarditerator middle,
	Forwarditerator last) {
	if (first == middle || middle == last)return;
	_rotate(first, middle, last, distance_type(first), iterator_category(first));
}


template<class Forwarditerator,class Outputiterator>
Outputiterator rotate_copy(Forwarditerator first, Forwarditerator middle,
	Forwarditerator last, Outputiterator result) {
	return copy(first, middle, copy(middle, last, result));
}


template<class Forwarditerator1,class Forwarditerator2>
Forwarditerator2 swap_range(Forwarditerator1 first1, Forwarditerator1 last,
	Forwarditerator2 first2) {
	for (; first1 != last; ++first1, ++first2)
		iter_swap(first1, first2);
	return first2;
}


template<class Inputiterator,class Outputiterator,class unaryoperation>
Outputiterator transform(Inputiterator first, Inputiterator last,
	Outputiterator result, unaryoperation op) {
	for (; first != last; ++first, ++result)
		*result = op(*first);
	return result;
}

template<class Inputiterator1,class Inputiterator2,class Outputiterator,
class Binaryoperation>
	Outputiterator transform(Inputiterator1 first1, Inputiterator1 last1,
		Inputiterator2 first2, Outputiterator result, Binaryoperation binary_op) {
	for (; first1 != last1; ++first1, ++first2, ++result) {
		*result = binary_op(*first1, *first2);
	}
	return result;
}




template<class Inputiterator,class Forwarditerator>
Forwarditerator _unique_copy(Inputiterator first, Inputiterator last,
	Forwarditerator result, forward_iterator_tag) {
	*result = *first;//前值
	while (++first != last) {//此处first相当于result的后值
		if (*first != *result)//此处表示满足不重复的要求
			*(++result) = *first;//result变为后值然后进行first的复制
	}
	return result;
}
template<class Inputiterator,class Outputiterator>
Outputiterator _unique_copy(Inputiterator first, Inputiterator last,
	Outputiterator result, output_iterator_tag) {
	typename iterator_traits<Outputiterator>::value_type value = *first;//前值
	*result = value;
	while (++first != last) {//++first 后值
		if (*first != *result) {//此处表示满足不重复的要求
			value = *first;
			*result = value;
		}
	}
	return result;
}

template<class Inputiterator,class Outputiterator>
inline Outputiterator unique_copy(Inputiterator first, Inputiterator last,
	Outputiterator result) {
	if (first == last)return result;
	return _unique_copy(first, last, result, iterator_category(result));
}




template<class Inputiterator, class Forwarditerator,class Binarypredicate>
Forwarditerator _unique_copy(Inputiterator first, Inputiterator last,
	Forwarditerator result,Binarypredicate binary_pred, forward_iterator_tag) {
	*result = *first;//前值
	while (++first != last) {//此处first相当于result的后值
		if (!binary_pred(*first,*result))//此处表示满足不重复的要求
			*(++result) = *first;//result变为后值然后进行first的复制
	}
	return result;
}
template<class Inputiterator, class Outputiterator,class Binarypredicate>
Outputiterator _unique_copy(Inputiterator first, Inputiterator last,
	Outputiterator result,Binarypredicate binary_pred, output_iterator_tag) {
	typename iterator_traits<Outputiterator>::value_type value = *first;//前值
	*result = value;
	while (++first != last) {//++first 后值
		if (!binary_pred(*first,*result)) {//此处表示满足不重复的要求
			value=*first;
			*result = value;
		}
	}
	return result;
}

template<class Inputiterator, class Outputiterator,class Binarypredicate>
inline Outputiterator unique_copy(Inputiterator first, Inputiterator last,
	Outputiterator result,Binarypredicate binary_pred) {
	if (first == last)return result;
	return _unique_copy(first, last, result,binary_pred, iterator_category(result));
}


template<class Forwarditerator>
Forwarditerator unique(Forwarditerator first, Forwarditerator last) {
	return unique_copy(first, last, first);
}





template<class Forwarditerator,class Distance,class T>
Forwarditerator _lower_bound(Forwarditerator first, Forwarditerator last,
	const T& value,Distance *, forward_iterator_tag) {
	Distance len = distance(first, last);
	Distance half;
	Forwarditerator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		std::advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}
template<class Randomaccessiterator, class Distance, class T>
Randomaccessiterator _lower_bound(Randomaccessiterator first, Randomaccessiterator last,
	const T& value, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	Randomaccessiterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}


template<class Forwarditerator, class Distance, class T,class Compare>
Forwarditerator _lower_bound(Forwarditerator first, Forwarditerator last,
	const T& value,Compare comp, Distance*, forward_iterator_tag) {
	Distance len = distance(first, last);
	Distance half;
	Forwarditerator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		std::advance(middle, half);
		if (comp(*middle,value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}
template<class Randomaccessiterator, class Distance, class T,class Compare>
Randomaccessiterator _lower_bound(Randomaccessiterator first, Randomaccessiterator last,
	const T& value,Compare comp, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	Randomaccessiterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(*middle,value)) {
			first = middle + 1;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}
template<class Forwarditerator,class T>
inline Forwarditerator lower_bound(Forwarditerator first, Forwarditerator last,
	const T& value) {
	return _lower_bound(first, last, value,distance_type(first), iterator_category(first));
}


template<class Forwarditerator, class T,class Compare>
inline Forwarditerator lower_bound(Forwarditerator first, Forwarditerator last,
	const T& value,Compare comp) {
	return _lower_bound(first, last, value, comp,
		distance_type(first), iterator_category(first));
}



template<class Randomaccessiterator, class Distance, class T>
Randomaccessiterator _upper_bound(Randomaccessiterator first, Randomaccessiterator last,
	const T& value, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	Randomaccessiterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (value < *middle) {//lower_bound 跟 upper_bound 差别就在于if的比较参数的位置
			len = half;
		}
		else
			first = middle + 1;
			len = len - half - 1;
	}
	return first;
}


template<class Forwarditerator,class Distance,class T>
Forwarditerator _upper_bound(Forwarditerator first, Forwarditerator last,
	const T& value, Distance*, forward_iterator_tag) {
	Distance len = distance(first, last);
	Distance half;
	Forwarditerator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		std::advance(middle, half);
		if (value < *middle) {//lower_bound 跟 upper_bound 差别就在于if的比较参数的位置
			len = half;
		}
		else
			first = middle;
			++first;
			len = len - half - 1;
	}
	return first;
}


template<class Forwarditerator, class Distance, class T, class Compare>
Forwarditerator _upper_bound(Forwarditerator first, Forwarditerator last,
	const T& value, Compare comp, Distance*, forward_iterator_tag) {
	Distance len = distance(first, last);
	Distance half;
	Forwarditerator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		std::advance(middle, half);
		if (comp(value, * middle)) {
			len = half;
		}
		else
			first = middle;
			++first;
			len = len - half - 1;
	}
	return first;
}
template<class Randomaccessiterator, class Distance, class T, class Compare>
Randomaccessiterator _upper_bound(Randomaccessiterator first, Randomaccessiterator last,
	const T& value, Compare comp, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	Randomaccessiterator middle;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(value ,*middle)) {
			len = half;
		}
		else
			first = middle + 1;
			len = len - half - 1;
	}
	return first;
}

template<class Forwarditerator,class T>
inline Forwarditerator upper_bound(Forwarditerator first, Forwarditerator last,
	const T& value) {
	return _upper_bound(first, last, value, distance_type(first), iterator_category(first));
}
template<class Forwarditerator, class T,class Compare>
inline Forwarditerator upper_bound(Forwarditerator first, Forwarditerator last,
	const T& value,Compare comp) {
	return _upper_bound(first, last, value,comp, distance_type(first), iterator_category(first));
}


template<class Forwarditerator,class T>
bool binary_search(Forwarditerator first, Forwarditerator last, const T& value) {
	Forwarditerator i = lower_bound(first, last, value);
	return i != last && !(value < *i);
}
template<class Forwarditerator, class T,class Compare>
bool binary_search(Forwarditerator first, Forwarditerator last, const T& value,Compare comp) {
	Forwarditerator i = lower_bound(first, last, value);
	return i != last && !comp(value,*i);
}


template<class Bidirectionaliterator>
bool next_permutation(Bidirectionaliterator first, Bidirectionaliterator last) {
	if (first == last)return false;
	Bidirectionaliterator i = first;
	++i;
	if (i == last)return false;
	i = last;
	--i;

	for (;;) {
		Bidirectionaliterator ii = i;
		--i;
		if (*i < *ii) {
			Bidirectionaliterator j = last;
			while (!(*i < *--j));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}


template<class Bidirectionaliterator,class Compare>
bool next_permutation(Bidirectionaliterator first, Bidirectionaliterator last
,Compare comp) {
	if (first == last)return false;
	Bidirectionaliterator i = first;
	++i;
	if (i == last)return false;
	i = last;
	--i;

	for (;;) {
		Bidirectionaliterator ii = i;
		--i;
		if (comp(*i,*ii)) {
			Bidirectionaliterator j = last;
			while (!comp(*i,*--j));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

template<class Bidirectionaliterator>
bool prev_permutation(Bidirectionaliterator first, Bidirectionaliterator last) {
	if (first == last)return false;
	Bidirectionaliterator i = first;
	++i;
	if (i == last)return false;
	i = last;
	--i;

	for (;;) {
		Bidirectionaliterator ii = i;
		--i;
		if (*ii < *i) {
			Bidirectionaliterator j = last;
			while (!(*--j < *i));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}

template<class Bidirectionaliterator, class Compare>
bool prev_permutation(Bidirectionaliterator first, Bidirectionaliterator last
	, Compare comp) {
	if (first == last)return false;
	Bidirectionaliterator i = first;
	++i;
	if (i == last)return false;
	i = last;
	--i;

	for (;;) {
		Bidirectionaliterator ii = i;
		--i;
		if (comp(*ii, *i)) {
			Bidirectionaliterator j = last;
			while (!comp(*--j, *i));
			iter_swap(i, j);
			reverse(ii, last);
			return true;
		}
		if (i == first) {
			reverse(first, last);
			return false;
		}
	}
}



template<class Randomaccessiterator,class Distance>
void _random_shuffle(Randomaccessiterator first, Randomaccessiterator,
	Distance*) {
	if (first == last)return;
	for (Randomaccessiterator i = first + 1; i != last; ++i)
		iter_swap(i, first + Distance(srand() % ((i - first) + 1));
}
template<class Randomaccessiterator,class Rand>
void random_shuffle(Randomaccessiterator first, Randomaccessiterator,
	Rand r) {
	if (first == last)return;
	for (Randomaccessiterator i = first + 1; i != last; ++i)
		iter_swap(i, first + r((i - first) + 1));
}
template<class Randomaccessiterator>
inline void random_shuffle(Randomaccessiterator first, Randomaccessiterator last) {
	_random_shuffle(first, last, distance_type(first));
}






//两种sort

template<class Randomaccessiterator,class T>
void _partial_sort(Randomaccessiterator first, Randomaccessiterator middle
	, Randomaccessiterator last,T*) {
	make_heap(first, middle);
	for (Randomaccessiterator i = middle; i < last; ++i)
		if(*i<*first)
		_pop_heap(first, middle, i, T(*i), distance_type(first));
	//_pop_heap 已经选定出堆的目标和出堆存放数据位置（i）同时还有入堆的数据
	//内部的adjust_heap会自动进行调整
	sort_heap(first, middle);
}

template<class Randomaccessiterator, class T,class Compare>
void _partial_sort(Randomaccessiterator first, Randomaccessiterator middle
	, Randomaccessiterator last,Compare comp, T*) {
	make_heap(first, middle);
	for (Randomaccessiterator i = middle; i < last; ++i)
		if(comp(*i,*first))
		_pop_heap(first, middle, i, T(*i), distance_type(first));
	//_pop_heap 已经选定出堆的目标和出堆存放数据位置（i）同时还有入堆的数据
	//内部的adjust_heap会自动进行调整
	sort_heap(first, middle);
}

template<class Randomaccessiterator>
inline void partial_sort(Randomaccessiterator first, Randomaccessiterator middle
	,Randomaccessiterator last) {
	_partial_sort(first, middle, last, value_type(first));
}
template<class Randomaccessiterator,class Compare>
inline void partial_sort(Randomaccessiterator first, Randomaccessiterator middle
	, Randomaccessiterator last,Compare comp) {
	_partial_sort(first, middle, last,comp, value_type(first));
}



template<class Inputiterator,class Randomaccessiterator>
inline Randomaccessiterator
partial_sort_copy(Inputiterator first, Inputiterator last
	, Randomaccessiterator result_first, Randomaccessiterator result_last) {
	partial_sort(first, last, last);
	result_last=copy(first, last, result_first);
	return result_last;
}

template<class Inputiterator, class Randomaccessiterator,class Compare>
inline Randomaccessiterator
partial_sort_copy(Inputiterator first, Inputiterator last
	, Randomaccessiterator result_first, Randomaccessiterator result_last,Compare comp) {
	partial_sort(first, last, last,comp);
	result_last = copy(first, last, result_first);
	return result_last;
}



template<class Randomaccessiterator,class T>
void _unguarded_linear_insert(Randomaccessiterator last, const T& value) {
	Randomaccessiterator next = last;
	--next;
	while (value < *next) {
		*last = *next;
		last = next;
		--next;
	}
	*last = value;
}

template<class Randomaccessiterator, class T,class Compare>
void _unguarded_linear_insert(Randomaccessiterator last, const T& value,Compare comp) {
	Randomaccessiterator next = last;
	--next;
	while (comp(value,*next)) {
		*last = *next;
		last = next;
		--next;
	}
	*last = value;
}
template<class Randomaccessiterator,class T,class Compare>
void _linear_insert(Randomaccessiterator first,Randomaccessiterator last,
	T*,Compare comp) {
	T value = *last;
	if (comp(value , *first)) {
		copy_backward(first, last, last);
		*first = value;
	}
	else
		_unguarded_linear_insert(last, value,comp);
}
template<class Randomaccessiterator, class T>
void _linear_insert(Randomaccessiterator first, Randomaccessiterator last,
	T*) {
	T value = *last;
	if (value < *first) {
		copy_backward(first, last, last);
		*first = value;
	}
	else
		_unguarded_linear_insert(last, value);
}

template<class Randomaccessiterator>
void _insertion_sort(Randomaccessiterator first, Randomaccessiterator last) {
	if (first == last)return;
	for (Randomaccessiterator i = first + 1; i < last; ++i)
		_linear_insert(first, i, value_type(first));
}
template<class Randomaccessiterator,class Compare>
void _insertion_sort(Randomaccessiterator first, Randomaccessiterator last,Compare comp) {
	if (first == last)return;
	for (Randomaccessiterator i = first + 1; i < last; ++i)
		_linear_insert(first, i, value_type(first),comp);
}





template<class T>
inline const T& _median(const T& a, const T& b, const T& c) {
	if (a < b)
		if (b < c)//a<b<c
			return b;
		else if (a < c)//a<b b>=c a<c
			return c;
		else//a<b b>=c a>=c
			return a;
	else if (a < c)//c>a>=b
		return a;
	else if (b < c)//a>=c b<c a>=b
		return c;
	else
		return b;
}

//这个函数属于快速排序的一部分
template<class Randomaccessiterator,class T>
Randomaccessiterator _unguarded_partion(Randomaccessiterator first,
	Randomaccessiterator last,
	T pivot) {
	while (true) {
		while (*first < pivot)++first;
		--last;
		while (pivot < *last)--last;
		if (!(first < last))return first;
		iter_swap(first, last);
		++first;
	}
}

template<class Randomaccessiterator, class T,class Compare>
Randomaccessiterator _unguarded_partion(Randomaccessiterator first,
	Randomaccessiterator last,
	T pivot,Compare comp) {
	while (true) {
		while (comp(*first , pivot))++first;
		--last;
		while (comp(pivot , *last))--last;
		if (!(first < last))return first;
		iter_swap(first, last);
		++first;
	}
}


template<class Size>
Size _lg(Size x) {
	Size n;
	for (n = 0; x > 1; x >>= 1)
		++n;
	return n;
}
const int threshold = 16;

template<class Randomaccessiterator,class T,class Size>
void _introsort_loop(Randomaccessiterator first, Randomaccessiterator last,
	T*, Size limit) {
	while (last - first > threshold) {
		if (limit == 0) {
			partial_sort(first, last, last);
			return;
		}
		--limit;
		Randomaccessiterator cut = _unguarded_partion(first, last, T(_median(*first,
													*(first + (last - first) / 2),
													*(last-1))));
		_introsort_loop(cut, last, value_type(first), limit);
		last = cut;
	}
}

template<class Randomaccessiterator>
void _unguarded_insertion_sort(Randomaccessiterator first, Randomaccessiterator last) {
	using value_type=typename iterator_traits<Randomaccessiterator>::value_type;
	for (Randomaccessiterator i = first; first != last; ++i) {
		_unguarded_linear_insert(i, value_type(*i));
	}
}

template<class Randomaccessiterator,class Randomaccessiterator>
void fina_insetion_sort(Randomaccessiterator first, Randomaccessiterator last) {
	if (last - first > threshold) {
		_insertion_sort(first, first + threshold);
		_unguarded_insertion_sort(first + threshold, last);
	}
	else
		_insertion_sort(first, last);
}

template<class Randomaccessiterator>
inline void sort(Randomaccessiterator first, Randomaccessiterator last) {
	if (first != last) {
		_introsort_loop(first, last, value_type(first), _lg(last - first) * 2);
		final_insertion_sort(first, last);
	}
}

//threshold=16
//introspective sort 内省式排序：
//首先使用_lg函数计算分割次数防止恶化，
//随后进入introsort_loop函数，针对序列长度超过16个元素的序列进行unguarded_partition排序；
//里面使用的是快速排序，但是没有递归 相当于不完整快速排序，
//随后对切割的右半部分序列进行introsort_loop递归调用，
//返回之后修改last值在当前层次进行循环；
//当分割出来的序列小于16或者分割次数归零时使用partial_sort函数进行排序，
//确保所有元素都排序过，无论是否完全排序正确，先确保整个序列中已经全部是排序过的子序列。
//最后调用final_insertion_sort 进行最后排序，元素个数小于16直接使用insertion_sort排序完
//由于个数不多使用insertion_sort排序不影响效率；
//如果大于16前十六个元素进行insertion_sort排序，
//随后元素进行insertion_sort排序中的内循环函数unguarded_linear_insert，
//为了将后续元素可能出现的逆转对解决； ---至此解决

template<class Randomaccessiterator, class T, class Size,class Compare>
void _introsort_loop(Randomaccessiterator first, Randomaccessiterator last,
	Compare comp,T*, Size limit) {
	while (last - first > threshold) {
		if (limit == 0) {
			partial_sort(first, last, last,comp);
			return;
		}
		--limit;
		Randomaccessiterator cut = _unguarded_partion(first, last, T(_median(*first,
			*(first + (last - first) / 2),
			*(last - 1))),comp);
		_introsort_loop(cut, last,comp, value_type(first), limit);
		last = cut;
	}
}

template<class Randomaccessiterator,class Compare>
void _unguarded_insertion_sort(Randomaccessiterator first, Randomaccessiterator last,
	Compare comp) {
	using value_type = typename iterator_traits<Randomaccessiterator>::value_type;
	for (Randomaccessiterator i = first; first != last; ++i) {
		_unguarded_linear_insert(i, value_type(*i),comp);
	}
}

template<class Randomaccessiterator, class Randomaccessiterator,class Compare>
void fina_insetion_sort(Randomaccessiterator first, Randomaccessiterator last,Compare comp) {
	if (last - first > threshold) {
		_insertion_sort(first, first + threshold,comp);
		_unguarded_insertion_sort(first + threshold, last,comp);
	}
	else
		_insertion_sort(first, last,comp);
}
template<class Randomaccessiterator,class Compare>
inline void sort(Randomaccessiterator first, Randomaccessiterator last,Compare comp) {
	if (first != last) {
		_introsort_loop(first, last,comp, value_type(first), _lg(last - first) * 2);
		final_insertion_sort(first, last,comp);
	}
}


template<class Randomaccessiterator,class T,class Distance>
std::pair<Randomaccessiterator, Randomaccessiterator>
_equal_range(Randomaccessiterator first, Randomaccessiterator last,
	const T& value,Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	Randomaccessiterator middle, left, right;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = lower_bound(first, middle, value);
			right = upper_bound(++middle, last, value);
			return std::piar<Randomaccessiterator, Randomaccessiterator>(left, right);
		}
	}
	return std::piar<Randomaccessiterator, Randomaccessiterator>(first, first);
}
template<class Forwarditerator, class T, class Distance>
std::pair<Forwarditerator, Forwarditerator>
_equal_range(Forwarditerator first, Forwarditerator last,
	const T& value, Distance*, forward_iterator_tag) {
	Distance len = ::distance(first, last);
	Distance half;
	Forwarditerator middle, left, right;
	while (len > 0) {
		half = len >> 1;
		middle = first ;
		std::advance(middle, half);
		if (*middle < value) {
			first = middle ;
			++first;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else {
			left = lower_bound(first, middle, value);
			right = upper_bound(++middle, last, value);
			return std::piar<Forwarditerator, Forwarditerator>(left, right);
		}
	}
	return std::piar<Forwarditerator, Forwarditerator>(first, first);
}
template<class Forwarditerator,class T>
inline std::pair<Forwarditerator,Forwarditerator> 
equal_range(Forwarditerator first, Forwarditerator last, const T& value) {
	return _equal_range(first, last, value, distance_type(first), iterator_category(first));
}


template<class Randomaccessiterator, class T, class Distance,class Compare>
std::pair<Randomaccessiterator, Randomaccessiterator>
_equal_range(Randomaccessiterator first, Randomaccessiterator last,
	const T& value,Compare comp, Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	Randomaccessiterator middle, left, right;
	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(*middle,value )) {
			first = middle + 1;
			len = len - half - 1;
		}
		else if (comp(value , *middle))
			len = half;
		else {
			left = lower_bound(first, middle, value,comp);
			right = upper_bound(++middle, last, value,comp);
			return std::piar<Randomaccessiterator, Randomaccessiterator>(left, right);
		}
	}
	return std::piar<Randomaccessiterator, Randomaccessiterator>(first, first);
}
template<class Forwarditerator, class T, class Distance,class Compare>
std::pair<Forwarditerator, Forwarditerator>
_equal_range(Forwarditerator first, Forwarditerator last,
	const T& value,Compare comp, Distance*, forward_iterator_tag) {
	Distance len = ::distance(first, last);
	Distance half;
	Forwarditerator middle, left, right;
	while (len > 0) {
		half = len >> 1;
		middle = first;
		std::advance(middle, half);
		if (comp(*middle , value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (comp(value, *middle))
			len = half;
		else {
			left = lower_bound(first, middle, value,comp);
			right = upper_bound(++middle, last, value,comp);
			return std::piar<Forwarditerator, Forwarditerator>(left, right);
		}
	}
	return std::piar<Forwarditerator, Forwarditerator>(first, first);
}

template<class Forwarditerator, class T,class Compare>
inline std::pair<Forwarditerator, Forwarditerator>
equal_range(Forwarditerator first, Forwarditerator last, const T& value,Compare comp) {
	return _equal_range(first, last, value, comp,distance_type(first), iterator_category(first));
}


template<class Biderectionaliterator,class Distance>
void _merge_without_buffer(Biderectionaliterator first,Biderectionaliterator middle
,Biderectionaliterator last,Distance len1,Distance len2) {
	_insertion_sort(first, last);
}
template<class Inputiterator1,class Inputiterator2,class Outputiterator>
void _merge_backward(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2, Outputiterator result_last) {
	--last1; --last2; --result_last;
	while (first1 != last1 && first2 != last2) {
		if (*last2 < *last1) {//先比较last2 然后再去比较last1 
			//可以减少代码 并且保证稳定
			*result_last = *last1;
			--last1;
		}
		else {
			*(--result_last) = *last2;
			--last2;
		}
		--result_last;
	}
	copy_backward(first1,last1,copy_backward(first2, last2, result_last));
}

template<class Bidirectionaliterator1,class Bidirectionaliterator2,class Distance>
Bidirectionaliterator1 _rotate_adaptive(Bidirectionaliterator1 first,
	Bidirectionaliterator1 middle,Bidirectionaliterator1 last,
	Distance len1,Distance len2,Bidirectionaliterator2 buffer,Distance b_size ) {
	Bidirectionaliterator2 buffer_end;
	if (len1 > len2 && len2 <= b_size) {
		buffer_end = copy(middle, last, buffer);
		copy_backward(first, middle, last);
		return copy(buffer, buffer_end, first);
	}
	else if (len1 <= b_size) {
		buffer_end = copy(first, middle, buffer);
		copy(middle, last, first);
		return copy_backward(buffer, buffer_end, last);
	}
	else {
		rotate(first, middle, last);
		advance(first, len2);
		return first;
	}
}
template<class Bidirectionaliterator,class Distance,class Pointer>
void _merge_adaptive(Bidirectionaliterator first,Bidirectionaliterator middle
,Bidirectionaliterator last,Distance len1,Distance len2,Pointer buffer,Distance b_size) {
	if (len1 <= len2 && len1 <= b_size) {
		Pointer end_buffer = copy(first, middle, buffer);
		merge(buffer, end_buffer, middle, last, first);
	}
	else if (len2 <= b_size) {
		Pointer end_buffer = copy(middle, last, buffer);
		_merge_backward(first, middle, buffer, end_buffer, last);
	}
	else {
		Bidirectionaliterator first_cut = first;
		Bidirectionaliterator second_cut = middle;
		Distance len11 = 0;
		Distance len22 = 0;
		if (len1 > len2) {
			len11 = len1 / 2;
			std::advance(first_cut, len11);
			second_cut = lower_bound(middle, last, *first_cut);
			len22=distance(middle, second_cut);
		}
		else {
			len22 = len2 / 2;
			std::advance(second_cut, len22);
			first_cut = upper_bound(first, middle, *second_cut);
			len11 = distance(first, first_cut);
		}
		Bidirectionaliterator new_middle =
			_rotate_adaptive(first_cut, middle, second_cut, 
				len1 - len11, len22, buffer, b_size);

		_merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, b_size);
		_merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, b_size);

	}
}

template<class Bidirectionaliterator,class T,class Distance>
void _inplace_merge(Bidirectionaliterator first,Bidirectionaliterator middle
,Bidirectionaliterator last,T*,Distance*) {
	Distance len1 = distance(first, middle);
	Distance len2 = distance(middle, last);
	std::pair<Bidirectionaliterator,ptrdiff_t>buf=
		std::get_temporary_buffer<Bidirectionaliterator>(len1 + len2);
	if (buf.second() == 0)
		_merge_without_buffer(first, middle, last, len1, len2);
	else
		_merge_adaptive(first, middle, last, len1, len2,buf.first(),(Distance)buf.second());
}

template<class Bidirectionaliterator>
inline void inplace_merge(Bidirectionaliterator first, Bidirectionaliterator middle,
	Bidirectionaliterator last) {
	if (first == middle||middle=last)return;
	_inplace_merge(first, middle, last, value_type(first), distance_type(first));
}




template<class Biderectionaliterator, class Distance,class Compare>
void _merge_without_buffer(Biderectionaliterator first, Biderectionaliterator middle
	, Biderectionaliterator last,Compare comp, Distance len1, Distance len2) {
	_insertion_sort(first, last,comp);
}
template<class Inputiterator1, class Inputiterator2, class Outputiterator,class Compare>
void _merge_backward(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, Inputiterator2 last2,Compare comp, Outputiterator result_last) {
	--last1; --last2; --result_last;
	while (first1 != last1 && first2 != last2) {
		if (comp(*last2 , *last1)) {//先比较last2 然后再去比较last1 
			//可以减少代码 并且保证稳定
			*result_last = *last1;
			--last1;
		}
		else {
			*(--result_last) = *last2;
			--last2;
		}
		--result_last;
	}
	copy_backward(first1, last1, copy_backward(first2, last2, result_last));
}

template<class Bidirectionaliterator1, class Bidirectionaliterator2, class Distance>
Bidirectionaliterator1 _rotate_adaptive(Bidirectionaliterator1 first,
	Bidirectionaliterator1 middle, Bidirectionaliterator1 last,
	Distance len1, Distance len2, Bidirectionaliterator2 buffer, Distance b_size) {
	Bidirectionaliterator2 buffer_end;
	if (len1 > len2 && len2 <= b_size) {
		buffer_end = copy(middle, last, buffer);
		copy_backward(first, middle, last);
		return copy(buffer, buffer_end, first);
	}
	else if (len1 <= b_size) {
		buffer_end = copy(first, middle, buffer);
		copy(middle, last, first);
		return copy_backward(buffer, buffer_end, last);
	}
	else {
		rotate(first, middle, last);
		advance(first, len2);
		return first;
	}
}
template<class Bidirectionaliterator, class Distance, class Pointer,class Compare>
void _merge_adaptive(Bidirectionaliterator first, Bidirectionaliterator middle
	, Bidirectionaliterator last, Compare comp,
	Distance len1, Distance len2, Pointer buffer, Distance b_size) {
	if (len1 <= len2 && len1 <= b_size) {
		Pointer end_buffer = copy(first, middle, buffer);
		merge(buffer, end_buffer, middle, last, first,comp);
	}
	else if (len2 <= b_size) {
		Pointer end_buffer = copy(middle, last, buffer);
		_merge_backward(first, middle, buffer, end_buffer,comp, last);
	}
	else {
		Bidirectionaliterator first_cut = first;
		Bidirectionaliterator second_cut = middle;
		Distance len11 = 0;
		Distance len22 = 0;
		if (len1 > len2) {
			len11 = len1 / 2;
			std::advance(first_cut, len11);
			second_cut = lower_bound(middle, last, *first_cut,comp);
			len22 = distance(middle, second_cut);
		}
		else {
			len22 = len2 / 2;
			std::advance(second_cut, len22);
			first_cut = upper_bound(first, middle, *second_cut,comp);
			len11 = distance(first, first_cut);
		}
		Bidirectionaliterator new_middle =
			_rotate_adaptive(first_cut, middle, second_cut,
				len1 - len11, len22, buffer, b_size);

		_merge_adaptive(first,first_cut, new_middle,comp,len11, len22,buffer,b_size);
		_merge_adaptive(new_middle, second_cut,last,comp,len1 - len11,len2 - len22, buffer, b_size);

	}
}

template<class Bidirectionaliterator, class T, class Distance,class Compare>
void _inplace_merge(Bidirectionaliterator first, Bidirectionaliterator middle
	, Bidirectionaliterator last,Compare comp, T*, Distance*) {
	Distance len1 = distance(first, middle);
	Distance len2 = distance(middle, last);
	std::pair<Bidirectionaliterator, ptrdiff_t>buf =
		std::get_temporary_buffer<Bidirectionaliterator>(len1 + len2);
	if (buf.second() == 0)
		_merge_without_buffer(first, middle, last, comp,len1, len2);
	else
		_merge_adaptive(first, middle, last,comp, len1, len2, buf.first(), (Distance)buf.second());
}


template<class Bidirectionaliterator,class Compare>
inline void inplace_merge(Bidirectionaliterator first, Bidirectionaliterator middle,
	Bidirectionaliterator last,Compare comp) {
	if (first == middle || middle = last)return;
	_inplace_merge(first, middle, last,comp, value_type(first), distance_type(first));
}

template<class Randomaccessiterator>
inline void nth_element(Randomaccessiterator first, Randomaccessiterator nth
	, Randomaccessiterator last) {
	while (last - first > 3) {
		Randomaccessiterator cut = _unguarded_partion(
			first, last, T(_median(*first,
				*(first + (last - first) / 2),
				*(last - 1))));
		if (cut <= nth)
			first = cut;
		else
			last = cut;
	}
	_insertion_sort(first, last);
}

template<class Randomaccessiterator,class Compare>
inline void nth_element(Randomaccessiterator first, Randomaccessiterator nth
	, Randomaccessiterator last,Compare comp) {
	while (last - first > 3) {
		Randomaccessiterator cut = _unguarded_partion(
			first, last, T(_median(*first,
				*(first + (last - first) / 2),
				*(last - 1))),comp);
		if (cut <= nth)
			first = cut;
		else
			last = cut;
	}
	_insertion_sort(first, last,comp);
}



template<class Biderectionaliterator>
void mergesort(Biderectionaliterator first, Biderectionaliterator last) {
	typename iterator_traits<Biderectionaliterator>::difference_type n =
		distance(first, last);
	if (n == 0 || n = 1)return;
	else {
		Biderectionaliterator mid = first;
		std::advance(mid, n / 2);
		mergesort(first, mid);
		mergesort(mid.last);
		inplace_merge(first, mid, last);
	}
}

#endif // !MY_STL_ALGOBASE
