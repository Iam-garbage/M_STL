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
	Distance bigchild = 2 * index + 2;//此处从堆顶0开始 计算右孩子节点
	while (bigchild < len) {
		//小于判定没有最右孩子 如果后续判定等于 相当于最后一个节点是右孩子
		if (*(first + bigchild) < *(first + bigchild - 1))//if（右边<左边）
			--bigchild;//满足后修改指针 指向最大孩子
		*(first + index) = *(first + bigchild);//更新父节点值 较大孩子填充
		
		index = bigchild;
		bigchild = 2 * (bigchild + 1);
	}
	//因为while里面bigchild一直都是计算右孩子 
	//如果最后跟堆大小相等 就表示最后一个节点是右孩子
	if (bigchild == len) {
		*(first + index) = *(first + bigchild - 1);
		//因为最后一个右孩子需要被抛出 所以直接减一 使用左孩子填充父节点
		index = bigchild - 1;
	}
	_push_heap(first, index, root, value);
}

template<class Randomaccessiterator,class T,class Distance>
inline void _pop_heap(Randomaccessiterator first, Randomaccessiterator last,
	Randomaccessiterator result, T value,Distance*) {
	*result = *first;//记录出堆的值
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
	Distance parent = (len - 2) / 2;//按照下标0计算
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

