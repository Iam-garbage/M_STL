#pragma once
#ifndef STL_HEAP
#define STL_HEAP
#include"mytype_traits.h"
#include"my_iterator.h"

template<class Randomaccessiterator,class Distance,class T>
void _push_heap(Randomaccessiterator first, Distance index, Distance root, T value) {
	Distance parent = parent = (index - 1) / 2;
	while (index > root && *(first + parent) < value) {
		*(first + index) = *(first + parent);
		index = parent;
		parent = (index-1) / 2;
	}
	*(first + index) = value;
}

template<class Randomaccessiterator,class Distance,class T>
inline void _push_heap_aux(Randomaccessiterator first,Randomaccessiterator last
,Distance*,T*){
	_push_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)));
}

template<class Randomaccessiterator>
inline void push_heap(Randomaccessiterator first, Randomaccessiterator last) {
	_push_heap_aux(first, last, distance_type(first), value_type(first));
}

template<class Randomaccessiterator,class Distance,class T>
void _adjust_heap(Randomaccessiterator first, Distance index, Distance len, T value) {
	Distance root = index;
	Distance bigchild = 2 * index + 2;//�˴��ӶѶ�0��ʼ �����Һ��ӽڵ�
	while (bigchild < len) {
		//С���ж�û�����Һ��� ��������ж����� �൱�����һ���ڵ����Һ���
		if (*(first + bigchild) < *(first + bigchild - 1))//if���ұ�<��ߣ�
			--bigchild;//������޸�ָ�� ָ�������
		*(first + index) = *(first + bigchild);//���¸��ڵ�ֵ �ϴ������
		
		index = bigchild;
		bigchild = 2 * (bigchild + 1);
	}
	//��Ϊwhile����bigchildһֱ���Ǽ����Һ��� 
	//��������Ѵ�С��� �ͱ�ʾ���һ���ڵ����Һ���
	if (bigchild == len) {
		*(first + index) = *(first + bigchild - 1);
		//��Ϊ���һ���Һ�����Ҫ���׳� ����ֱ�Ӽ�һ ʹ��������丸�ڵ�
		index = bigchild - 1;
	}
	_push_heap(first, index, root, value);
}

template<class Randomaccessiterator,class T,class Distance>
inline void _pop_heap(Randomaccessiterator first, Randomaccessiterator last,
	Randomaccessiterator result, T value,Distance*) {
	*result = *first;//��¼���ѵ�ֵ
	_adjust_heap(first, Distance(0), Distance(last - first), value);
}
template<class Randomaccessiterator,class T>
inline void _pop_heap_aux(Randomaccessiterator first, Randomaccessiterator last,
	T*) {
	_pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}
template<class Randomaccessiterator>
inline void pop_heap(Randomaccessiterator first, Randomaccessiterator last) {
	_pop_heap_aux(first, last, value_type(first));
}

template<class Randomaccessiterator>
void sort_heap(Randomaccessiterator first, Randomaccessiterator last) {
	while (last - first > 1) {
		pop_heap(first, last--);
	}
}

template<class Randomaccessiterator,class T,class Distance>
inline void _make_heap(Randomaccessiterator first, Randomaccessiterator last
,T*,Distance*) {
	if (last - first < 2)return;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;//�����±�0����
	while (true) {
		_adjust_heap(first, parent, len, T(*(first + parent)));
		if (parent == 0)return;
		--parent;
	}
}
template<class Randomaccessiterator>
inline void make_heap(Randomaccessiterator first, Randomaccessiterator last) {
	_make_heap(first, last, value_type(first), distance_type(first));
}

#endif // !STL_HEAP

