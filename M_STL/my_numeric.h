#pragma once
#ifndef MY_NUMERIC
#define MY_NUMERIC
#include"mytype_traits.h"
#include"my_iterator.h"
template<class Inputiterator,class T>
T accumulate(Inputiterator first, Inputiterator last,T init) {
	for (; first != last; ++first) {
		init += *first;
	}
	return init;
}
template<class Inputiterator, class T,class Binaryoperation>
T accumulate(Inputiterator first, Inputiterator last, T init,Binaryoperation binary_op) {
	for (; first != last; ++first) {
		init = binary_op(init, *first);
	}
	return init;
}

template<class Inputiterator,class Outputiterator>
Outputiterator adjacent_difference(Inputiterator first, Inputiterator last,
	Outputiterator result) {
	if (first == last)return result;
	*result = *first;
	typename iterator_traits<Inputiterator>::value prev = *first;//����ǰ��first
	while (++first != last) {
		typename iterator_traits<Inputiterator>::value next = *first;//����֮���first
		*(++result) = next - prev;//����Ԫ�ز�� ��-ǰ
		next = prev;
	}
	return ++result;
}

template<class Inputiterator, class Outputiterator,class Binaryoperation>
Outputiterator adjacent_difference(Inputiterator first, Inputiterator last,
	Outputiterator result,Binaryoperation binary_op) {
	if (first == last)return result;
	*result = *first;
	typename iterator_traits<Inputiterator>::value_type prev = *first;
	while (++first != last) {
		typename iterator_traits<Inputiterator>::value_type next = *first;
		*(++result) = binary_op(next,prev);//����Ԫ�ز�� ��-ǰ
		next = prev;
	}
	return ++result;
}

template<class Inputiterator1,class Inputiterator2,class T>
T inner_product(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, T init) {
	for (; first1 != last1; ++first1, ++first2)
		init = init+ *first1 * (*first2);
	return init;
}
template<class Inputiterator1, class Inputiterator2, class T,
class binaryoperation1,class binaryoperation2>
T inner_product(Inputiterator1 first1, Inputiterator1 last1,
	Inputiterator2 first2, T init,binaryoperation1 op1,binaryoperation2 op2) {
	for (; first1 != last1; ++first1, ++first2)
		init = op1(init, op2(*first1, *first2));
	return init;
}

template<class Inputiterator,class Outputiterator>
Outputiterator partial_sum(Inputiterator first, Inputiterator last,
	Outputiterator result) {
	if (first == last)return result;
	*result = *first;
	typename iterator_traits<Inputiterator>::value_type value = *first;
	while (++first != last) {
		typename iterator_traits<Inputiterator>::value_type next = *first;
		value = value + next;//value��ʾǰn��ĺ�
		*++result= value;
	}
	return ++result;
}
template<class Inputiterator, class Outputiterator,class Binaryoperation>
Outputiterator partial_sum(Inputiterator first, Inputiterator last,
	Outputiterator result,Binaryoperation binary_op) {
	if (first == last)return result;
	*result = *first;
	typename iterator_traits<Inputiterator>::value_type value = *first;
	while (++first != last) {
		typename iterator_traits<Inputiterator>::value_type next = *first;
		value = binary_op(value,next);//value��ʾǰn��ĺ�
		*++result = value;
	}
	return ++result;
}


template<class T>//֤ͬ����
inline T identity_element(std::multiplies<T>) {
	return T(1);
}

template<class T,class integer>
inline T power(T x, integer n) {
	return power(x, n, std::multiplies<T>());
}
template<class T, class integer,class operation>
T power(T x, integer n,operation op) {
	if (n == 0)
		return identity_element<T>(op);
	else {
		while ((n & 1) == 0) {
			n >>= 1;//�൱�� n=n>>1;��������ʽ
			x = op(x, x);
		}
		T result = x;
		n >>= 1;
		while (n != 0) {
			x = op(x, x);
			if ((n & 1) != 0)
				result = op(result, x);
			n >>= 1;
		}
		return result;
	}
}

template<class Forwarditerator,class T>
void iota(Forwarditerator first, Forwarditerator last, T value) {
	while (first != last)
		*(first++) = value++;
}

#endif // !MY_NUMERIC

