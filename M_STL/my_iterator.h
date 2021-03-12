#ifndef MY_ITERATOR
#define MY_ITERATOR
#include<cstddef>
#include<iostream>
//萃取器第一层
struct output_iterator_tag{};
struct input_iterator_tag{};
struct forward_iterator_tag :public input_iterator_tag{};
struct bidirectional_iterator_tag: public forward_iterator_tag{};
struct random_access_iterator_tag:public bidirectional_iterator_tag{};



//第二层建立五种迭代器类型的模板
template<typename T,typename distance>//distance 基本都是ptrdiff_t类型
struct input_iterator {
	using iterator_category    = input_iterator_tag;
	using value_type           = T;
	using pointer              = T*;
	using reference            = value_type&;
	using difference_type      = distance;
};

struct output_iterator {
	using iterator_category   = output_iterator_tag;
	using value_type          = void;
	using pointer             = void*;
	using reference           = void;
	using difference_type     = void;
};

template<typename T, typename distance>//distance 基本都是ptrdiff_t类型
struct forward_iterator {
	using iterator_category  = forward_iterator_tag;
	using value_type         = T;
	using pointer            = T*;
	using reference          = value_type&;
	using difference_type    = distance;
};

template<typename T, typename distance>//distance 基本都是ptrdiff_t类型
struct bidirectional_iterator {
	using iterator_category  = bidirectional_iterator_tag;
	using value_type         = T;
	using pointer            = T*;
	using reference          = value_type&;
	using difference_type    = distance;
};

template<typename T, typename distance>//distance 基本都是ptrdiff_t类型
struct random_access_iterator {
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using pointer           = T*;
	using reference         = value_type&;
	using difference_type   = distance;
};
template <class Category, class T, class Distance = ptrdiff_t,
	class Pointer = T*, class Reference = T&>
	struct iterator {
	typedef T         value_type;
	typedef Distance  difference_type;
	typedef Pointer   pointer;
	typedef Reference reference;
	typedef Category  iterator_category;
};

//迭代器萃取功能
template<typename iterator>
struct iterator_traits {
	
	using iterator_category = typename iterator::iterator_category;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;
	using difference_type = typename iterator::difference_type;
};
template<typename T>//内建类型指针特化
struct iterator_traits<T*> {
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using pointer           = T*;
	using reference         = T&;
	using difference_type   = std::ptrdiff_t;
};
template<typename T>//内建类型常量指针特化
struct iterator_traits<const T*> {
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using pointer           = T*;
	using reference         = T&;
	using difference_type   = std::ptrdiff_t;
};


template<typename iterator>//记得测试
inline decltype(auto) iterator_category(iterator&) {
	return typename iterator_traits<iterator>::iterator_category();//两种写法
}
template<typename iterator>
inline decltype(auto) value_type(iterator&) {
	return static_cast<typename iterator_traits<iterator>::value_type*>(0);
}
template<typename iterator>
inline decltype(auto) distance_type(iterator&) {
	return static_cast<typename iterator_traits<iterator>::difference_type*>(0);
}

//人家源代码写法 output_iterator没有value_type和distance_type函数的重载
//因为output_iterator属于普通类型并且value_type和distance_type部分都属于void
//所以使用函数进行推导没有意义
//iterator_category部分
template<typename T,typename distance>
inline input_iterator_tag
iterator_category(const input_iterator<T, distance>&) {
	return input_iterator_tag();
}

inline output_iterator_tag
iterator_category(const output_iterator&) {//output_iterator是一个非模板类
	return output_iterator_tag();
}

template<typename T,typename distance>
inline forward_iterator_tag
iterator_category(const forward_iterator<T, distance>&) {
	return forward_iterator_tag();
}

template<typename T, typename distance>
inline bidirectional_iterator_tag
iterator_category(const bidirectional_iterator<T, distance>&) {
	return bidirectional_iterator_tag();
}

template<typename T, typename distance>
inline random_access_iterator_tag
iterator_category(const random_access_iterator<T, distance>&) {
	return random_access_iterator_tag();
}

template<typename T>//对内建类型进行重载
inline random_access_iterator_tag
iterator_category(const T*) {
	return random_access_iterator_tag();
}

//value_type部分
template<typename T,typename distance>
inline T* value_type(const input_iterator<T, distance>&) {
	return (T*)(0);
}

template<typename T, typename distance>
inline T* value_type(const forward_iterator<T, distance>&) {
	return (T*)(0);
}

template<typename T, typename distance>
inline T* value_type(const bidirectional_iterator<T, distance>&) {
	return (T*)(0);
}

template<typename T, typename distance>
inline T* value_type(const random_access_iterator<T, distance>&) {
	return (T*)(0);
}

template<typename T>
inline T* value_type(const T*){
	return (T*)(0);
}

//以下为distance_type部分
template<typename T,typename distance>
inline distance*
distance_type(const input_iterator<T, distance>&) {
	return (distance*)(0);
}

template<typename T,typename distance>
inline distance*
distance_type(const forward_iterator<T, distance>&) {
	return (distance*)(0);
}

template<typename T, typename distance>
inline distance*
distance_type(const bidirectional_iterator<T, distance>&) {
	return (distance*)(0);
}

template<typename T, typename distance>
inline distance*
distance_type(const random_access_iterator<T, distance>&) {
	return (distance*)(0);
}

template<typename T>//基类类型指针
inline ptrdiff_t*  
distance_type(const T*) {//函数中的参数基本没有意义 只是为了模板正常接收内建类型
	return (ptrdiff_t*)(0);
}


template<typename inputiterator,typename distance>
inline typename iterator_traits<inputiterator>::difference_type
_distance(inputiterator first, inputiterator last, 
          input_iterator_tag,distance n = 0 ) {
	while (first != last) {
		++n; ++first;
	}
	return n;
}
template<typename randomiterator, typename distance>
inline typename iterator_traits<randomiterator>::difference_type
_distance(randomiterator first, randomiterator last,
	       random_access_iterator_tag,distance n = 0) {
	n = last - first;
	return n;
}
template<typename inputiterator, typename Distance>
inline typename iterator_traits<inputiterator>::difference_type
distance(inputiterator first, inputiterator last,
	     input_iterator_tag, Distance n = 0) {
	using category = typename iterator_traits<inputiterator>::iterator_category;
	return _distance(first, last, category());
}//可以使用全局函数 iterator_category()

template<typename forwarditerator,typename distance>
inline void _advance(forwarditerator& begin, distance n,
	                 forward_iterator_tag) {
	if (n > 0)
		while (--n)
			++begin;
	else
		while (n++)
			--begin;
}
template<typename randomiterator, typename distance>
inline void _advance(randomiterator& begin, distance n,
	random_access_iterator_tag) {
	begin += n;
}
template<typename forwarditerator, typename distance>
inline void advance(forwarditerator& begin, distance n) {
	_advance(begin, n, iterator_category(begin));
}//advance()使用了iterator_category()


template<typename forwarditerator, typename distance>
inline forwarditerator _next(forwarditerator& begin, distance n,
	forward_iterator_tag) {
	if (n > 0)
		while (--n){
			++begin; return begin;
		}
	else
		while (n++){
	     --begin; return begin;
        }
}

template<typename randomiterator, typename distance>
inline randomiterator _next(randomiterator& begin, distance n,
	random_access_iterator_tag) {
	return (begin += n);
}
template<typename forwarditerator,typename distance>
inline forwarditerator next(forwarditerator& begin, distance n=1) {
	return _next(begin, n, iterator_category(begin));
}

template<typename bidirectionaliterator, typename distance>
inline bidirectionaliterator _prev(bidirectionaliterator& begin, distance n,
	bidirectional_iterator_tag) {
	if (n > 0)
		while (--n) {
			++begin; return begin;
		}
	else
		while (n++) {
			--begin; return begin;
		}
}
template<typename randomiterator, typename distance>//n为负值即可向前移动
inline randomiterator _prev(randomiterator& begin, distance n,
	random_access_iterator_tag) {
	return (begin += n);
}
template<typename bidirectionaliterator, typename distance>
inline bidirectionaliterator prev(bidirectionaliterator& begin, distance n=1) {
	using category = typename iterator_traits<bidirectionaliterator>::iteraot_category;
	return _prev(begin, -n, category());
}//此处直接使用了萃取器 而没有使用iterator_category()

template<typename forwarditerator1,typename forwarditerator2>
inline void iter_swap(forwarditerator1& a, forwarditerator2& b) {
	std::swap(*a, *b);//调用人家设计的swap吧 因为类型对比比较麻烦直接借用
}


//以下为全局迭代器 back front insert
template<typename container>//支持vector deque list string
class back_insert_iterator {
protected:container* n;

public:
	using container_type    = container;
	using iterator_category = output_iterator_tag;
	using value_type        = void;
	using pointer           = void;
	using reference         = void;
	using difference_type   = void;
	explicit back_insert_iterator(container& x) {
		n(&x);//容器地址
	}

	back_insert_iterator&
		operator=(const typename container::value_type& x) {
		n->push_back(x);//n是容器指针
		return *this;
	}

	back_insert_iterator<container>& operator*() { return *this; }
	back_insert_iterator<container>& operator++() { return *this; }//前缀重载
	back_insert_iterator<container>& operator++(int) { return *this; }//后缀重载
};

template<typename container>
inline output_iterator_tag 
iterator_category(const container&) {
	return output_iterator_tag();
}

template<typename container>
inline back_insert_iterator<container>
back_insert(container& x) {
	return back_insert_iterator<container>(x);
}

template<typename container>
class front_insert_iterator {
protected:container* n;
public:
	using container_type = container;
	using iterator_category = output_iterator_tag;
	using value_type = void;
	using pointer = void;
	using reference = void;
	using difference_type = void;
	explicit front_insert_iterator(container& x) :n(&x) {}

	front_insert_iterator<container>&
		operator=(const typename container::value_type& value) {
		n->push_front(value);
		return *this;
	}
	front_insert_iterator<container>& operator*() { return *this; }
	front_insert_iterator<container>& operator++() { return *this; }
	front_insert_iterator<container>& operator++(int) { return *this; }
};

template<typename container>
inline output_iterator_tag
iterator_category(const front_insert_iterator<container>&) {
	return output_iterator_tag();
}

template<class container>
inline front_insert_iterator<container>&
front_insert(container& x) {
	return front_insert_iterator<container>(x);
}

template<typename container>
class insert_iterator {
protected:container* n;
		 typename container::iterator cr;
public:
	using container_type = container;
	using iterator_category = output_iterator_tag;
	using value_type = void;
	using pointer = void;
	using reference = void;
	using difference_type = void;
	explicit insert_iterator(container &x, typename container::iterator&p)
		:n(&x),cr(p){}

	insert_iterator<container>&
		operator=(const typename container::value_type& value) {
		cr=n->insert(cr,value);
		cr++;//insert_iteraot 需要返回被插入后的元素位置 也就是原先元素的新位置
		return *this;
	}
	insert_iterator<container>& operator*() { return *this; }
	insert_iterator<container>& operator++() { return *this; }
	insert_iterator<container>& operator++(int) { return *this; }
};

template<typename container>
inline output_iterator_tag
iterator_category(const insert_iterator<container>&) {
	return output_iterator_tag();
}

template<typename container,typename iterator>
inline insert_iterator<container>&
inserter(container& x, iterator it) {
	using iterator = typename container::iterator;
	return insert_iterator<container>(x, iterator(it));
}

template<typename iterator>
class reverse_iterator {
protected:iterator current;

public:
	using self = reverse_iterator<iterator>;//复制构造使用
	using iterator_type = iterator;
	using iterator_category = typename iterator_traits<iterator>::iterator_category;
	using value_type = typename iterator_traits<iterator>::value_type;
	using difference_type = typename iterator_traits<iterator>::difference_type;
	using pointer = typename iterator_traits<iterator>::pointer;
	using reference = typename iterator_traits<iterator>::reference;
public:
	reverse_iterator(){}

	explicit reverse_iterator(iterator_type& it):current(it){}
	
	reverse_iterator(const self& x):current(x.current){}

	iterator_type base() const {
		return current;
	}

	reference operator*() const {
		iterator_type tmp = current;
		return *--tmp;
	}//前缀-- ++和*（解引用）处于同一调用等级
    //操作顺序是R-L
    //所以最后相当于调用前缀递减 然后解引用 最后返回迭代器指向的值的copy

	//反向迭代器的加减操作不对本身指针进行修改
	self operator+ (difference_type n) const {
		return self(current - n);
	}

	self operator- (difference_type n) const {
		return self(current + n);
	}

	self& operator++() {
		return --current;
	}
	self operator++(int) {
		iterator tmp = current;
		current--;
		return tmp;
	}

	self& operator--() {
		return ++current;
	}

	self operator--(int) {
		iterator tmp = current;
		current++;
		return tmp;
	}

	self& operator+= (difference_type n) {
		current -= n;
		return *this;
	}

	self& operator-= (difference_type n) {
		current += n;
		return *this;
	}

	pointer operator->() const {
		return &(operator*());
	}

	reference operator[] (difference_type n) const {
		return *((*this) + n);//外层解引用表示调用operator*()
	}
};

template<typename iterator>
inline random_access_iterator_tag
iterator_category(const reverse_iterator<iterator>&) {
	return random_access_iterator_tag();
}

template<typename iterator>
inline typename iterator::value_type*
value_type(const reverse_iterator<iterator>&) {
	using value_type = typename reverse_iterator<iterator>::value_type;
	return (value_type*)(0);
}

template<typename iterator>
inline typename iterator::difference_type*
distance_type(const reverse_iterator<iterator>&) {
	using difference_type = typename reverse_iterator<iterator>::difference_type;
	return (difference_type*)(0);
}

//以下为反向迭代器的关系运算符
template<typename iterator1,typename iterator2>
bool operator==(const reverse_iterator<iterator1>& x,
	            const reverse_iterator<iterator2>& y) {
	return (x.base() == y.base());//需要使用base 迭代器为保护成员
}
template<typename iterator1, typename iterator2>
bool operator!=(const reverse_iterator<iterator1>& x,
	            const reverse_iterator<iterator2>& y) {
	return !(x == y);
}
template<typename iterator1, typename iterator2>
bool operator<(const reverse_iterator<iterator1>& x,
	            const reverse_iterator<iterator2>& y) {
	return (x.base() < y.base());
}
template<typename iterator1, typename iterator2>
bool operator>(const reverse_iterator<iterator1>& x,
	           const reverse_iterator<iterator2>& y) {
	return (y<x);
}
template<typename iterator1, typename iterator2>
bool operator<=(const reverse_iterator<iterator1>& x,
	            const reverse_iterator<iterator2>& y) {
	return !(y<x);
}
template<typename iterator1, typename iterator2>
bool operator>=(const reverse_iterator<iterator1>& x,
	            const reverse_iterator<iterator2>& y) {
	return !(x<y);
}
template <class iterator>
reverse_iterator<iterator> operator+ (//迭代器在左边操作位的函数 使用成员函数实现
	typename reverse_iterator<iterator>::difference_type n,
	const reverse_iterator<iterator>& x) {
	return (x + n);
}
template <class iterator>
reverse_iterator<iterator> operator- (
	typename reverse_iterator<iterator>::difference_type n,
	const reverse_iterator<iterator>& x) {
	return (x - n);
}
template <class _Iterator>//这个用来求迭代器之间的距离（元素个数）
inline typename reverse_iterator<_Iterator>::difference_type
operator-(const reverse_iterator<_Iterator>& __x,
	const reverse_iterator<_Iterator>& __y) {
	return __y.base() - __x.base();
}

template <class T, class charT = char, class traits = std::char_traits<charT>,
	      class Distance = ptrdiff_t>
class istream_iterator {
protected:
	
	std::istream* stream;
	T myvalue;//记录当前读取值 同时可以用到返回上一个读取值的功能上
	bool end_mark;//判定是否可以继续读取
	//这个bool标记为1表示可以继续读取 如果标记为0那么就表示
	//已经到达了文件尾或者其他原因导致无法继续读取
	void read() {//读取函数
		end_mark = (*stream) ? true : false;
		if (end_mark)*stream >> myvalue;//写入流 读取的新值到value
		//保留读取到的值
		end_mark = (*stream) ? true : false;
	}
public:
	
	using istream_type = std::basic_istream<charT, traits>;
	using value_type = T;
	using iterator_category = input_iterator_tag;
	using difference_type = Distance;
	using traits_type = traits;
	using char_type = charT;
	using pointer = T*;
	using reference = T&;
	
	istream_iterator() 
	{
		using std::cin;
		stream=&cin;
		end_mark = false;
	}
	istream_iterator(std::istream& s) :stream(s) { read(); }
	reference operator*() const { return myvalue; }
	pointer operator->() const { return &(operator*()); }

	istream_iterator& operator++() {
		read();
		return *this;
	}
	istream_iterator operator++(int) {
		istream_iterator tmp = *this;
		read();
		return tmp;
	}

	//实现友元模板函数和类的绑定 显式实例化
	//函数的参数列表里面要写与之绑定的对应类型 这样就实现了一对一的绑定
	//输入迭代器 关系运算符要进行同类型的比较 类型不同不能比较
};


template<typename T, typename distance>
bool operator==(istream_iterator<T,distance>& x, istream_iterator<T, distance>& y) {
	if (x.end_mark == false && y.end_mark == false)
		return true;
	else if (x.end_mark == true && y.end_mark == true
		&& (*x.stream) == (*y.stream))
		return true;
	else
		false;
}
//！=不需要定义成友元 调用==即可
template<typename T, typename distance>
bool operator!=(istream_iterator<T, distance>& x, istream_iterator<T, distance>& y) {
	return !(operator==(x,y));
}


template <class T, class charT = char, class traits = std::char_traits<charT>,
	class Distance = ptrdiff_t>
	inline input_iterator_tag iterator_category(
		const istream_iterator<T, charT, traits, Distance>&) {
	return input_iterator_tag();
}

template <class T, class charT = char, class traits = std::char_traits<charT>,
	class Distance = ptrdiff_t>
	inline T* value_type(
		const istream_iterator<T, charT, traits, Distance>&) {
	return (T*)(0);
}

template <class T, class charT = char, class traits = std::char_traits<charT>,
	class Distance = ptrdiff_t>
	inline Distance* distance_type(
		const istream_iterator<T, charT, traits, Distance>&) {
	return (Distance*)(0);
}


template <class T, class charT = char, class traits = std::char_traits<charT> >
class ostream_iterator{
protected:
	std::ostream* stream;
	const char* string;
public:
	typedef output_iterator_tag iterator_category;
	typedef void                value_type;
	typedef void                difference_type;
	typedef void                pointer;
	typedef void                reference;
	typedef traits              traits_type;
	typedef charT               char_type;
	typedef std::basic_ostream<charT, traits>   ostream_type;

	ostream_iterator(ostream_type& s, const char* ch = nullptr)
		:stream(s),string(ch){}
	ostream_iterator(ostream_iterator& x) {
		stream = x.stream;
		string = x.string;
	}//在类的作用域内 使用自身类名可以不用显式声明模板参数列表<.....>

	ostream_iterator operator*() const {
		return *this;
	}
	ostream_iterator& operator++() {
		return *this;
	}
	ostream_iterator  operator++(int) {
		return *this;
	}

	ostream_iterator& operator=(T& value) {
		*stream << value;
		if (string)*stream << string;
		return *this;
	}
};

template<typename T>
inline output_iterator_tag
iterator_category(const ostream_iterator<T>&) {
	return output_iterator_tag();
}



#endif



