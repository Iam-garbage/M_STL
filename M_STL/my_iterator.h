#ifndef MY_ITERATOR
#define MY_ITERATOR
#include<cstddef>
#include<iostream>
//��ȡ����һ��
struct output_iterator_tag{};
struct input_iterator_tag{};
struct forward_iterator_tag :public input_iterator_tag{};
struct bidirectional_iterator_tag: public forward_iterator_tag{};
struct random_access_iterator_tag:public bidirectional_iterator_tag{};



//�ڶ��㽨�����ֵ��������͵�ģ��
template<typename T,typename distance>//distance ��������ptrdiff_t����
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

template<typename T, typename distance>//distance ��������ptrdiff_t����
struct forward_iterator {
	using iterator_category  = forward_iterator_tag;
	using value_type         = T;
	using pointer            = T*;
	using reference          = value_type&;
	using difference_type    = distance;
};

template<typename T, typename distance>//distance ��������ptrdiff_t����
struct bidirectional_iterator {
	using iterator_category  = bidirectional_iterator_tag;
	using value_type         = T;
	using pointer            = T*;
	using reference          = value_type&;
	using difference_type    = distance;
};

template<typename T, typename distance>//distance ��������ptrdiff_t����
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

//��������ȡ����
template<typename iterator>
struct iterator_traits {
	
	using iterator_category = typename iterator::iterator_category;
	using value_type = typename iterator::value_type;
	using pointer = typename iterator::pointer;
	using reference = typename iterator::reference;
	using difference_type = typename iterator::difference_type;
};
template<typename T>//�ڽ�����ָ���ػ�
struct iterator_traits<T*> {
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using pointer           = T*;
	using reference         = T&;
	using difference_type   = std::ptrdiff_t;
};
template<typename T>//�ڽ����ͳ���ָ���ػ�
struct iterator_traits<const T*> {
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using pointer           = T*;
	using reference         = T&;
	using difference_type   = std::ptrdiff_t;
};


template<typename iterator>//�ǵò���
inline decltype(auto) iterator_category(iterator&) {
	return typename iterator_traits<iterator>::iterator_category();//����д��
}
template<typename iterator>
inline decltype(auto) value_type(iterator&) {
	return static_cast<typename iterator_traits<iterator>::value_type*>(0);
}
template<typename iterator>
inline decltype(auto) distance_type(iterator&) {
	return static_cast<typename iterator_traits<iterator>::difference_type*>(0);
}

//�˼�Դ����д�� output_iteratorû��value_type��distance_type����������
//��Ϊoutput_iterator������ͨ���Ͳ���value_type��distance_type���ֶ�����void
//����ʹ�ú��������Ƶ�û������
//iterator_category����
template<typename T,typename distance>
inline input_iterator_tag
iterator_category(const input_iterator<T, distance>&) {
	return input_iterator_tag();
}

inline output_iterator_tag
iterator_category(const output_iterator&) {//output_iterator��һ����ģ����
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

template<typename T>//���ڽ����ͽ�������
inline random_access_iterator_tag
iterator_category(const T*) {
	return random_access_iterator_tag();
}

//value_type����
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

//����Ϊdistance_type����
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

template<typename T>//��������ָ��
inline ptrdiff_t*  
distance_type(const T*) {//�����еĲ�������û������ ֻ��Ϊ��ģ�����������ڽ�����
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
}//����ʹ��ȫ�ֺ��� iterator_category()

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
}//advance()ʹ����iterator_category()


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
template<typename randomiterator, typename distance>//nΪ��ֵ������ǰ�ƶ�
inline randomiterator _prev(randomiterator& begin, distance n,
	random_access_iterator_tag) {
	return (begin += n);
}
template<typename bidirectionaliterator, typename distance>
inline bidirectionaliterator prev(bidirectionaliterator& begin, distance n=1) {
	using category = typename iterator_traits<bidirectionaliterator>::iteraot_category;
	return _prev(begin, -n, category());
}//�˴�ֱ��ʹ������ȡ�� ��û��ʹ��iterator_category()

template<typename forwarditerator1,typename forwarditerator2>
inline void iter_swap(forwarditerator1& a, forwarditerator2& b) {
	std::swap(*a, *b);//�����˼���Ƶ�swap�� ��Ϊ���ͶԱȱȽ��鷳ֱ�ӽ���
}


//����Ϊȫ�ֵ����� back front insert
template<typename container>//֧��vector deque list string
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
		n(&x);//������ַ
	}

	back_insert_iterator&
		operator=(const typename container::value_type& x) {
		n->push_back(x);//n������ָ��
		return *this;
	}

	back_insert_iterator<container>& operator*() { return *this; }
	back_insert_iterator<container>& operator++() { return *this; }//ǰ׺����
	back_insert_iterator<container>& operator++(int) { return *this; }//��׺����
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
		cr++;//insert_iteraot ��Ҫ���ر�������Ԫ��λ�� Ҳ����ԭ��Ԫ�ص���λ��
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
	using self = reverse_iterator<iterator>;//���ƹ���ʹ��
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
	}//ǰ׺-- ++��*�������ã�����ͬһ���õȼ�
    //����˳����R-L
    //��������൱�ڵ���ǰ׺�ݼ� Ȼ������� ��󷵻ص�����ָ���ֵ��copy

	//����������ļӼ��������Ա���ָ������޸�
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
		return *((*this) + n);//�������ñ�ʾ����operator*()
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

//����Ϊ����������Ĺ�ϵ�����
template<typename iterator1,typename iterator2>
bool operator==(const reverse_iterator<iterator1>& x,
	            const reverse_iterator<iterator2>& y) {
	return (x.base() == y.base());//��Ҫʹ��base ������Ϊ������Ա
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
reverse_iterator<iterator> operator+ (//����������߲���λ�ĺ��� ʹ�ó�Ա����ʵ��
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
template <class _Iterator>//��������������֮��ľ��루Ԫ�ظ�����
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
	T myvalue;//��¼��ǰ��ȡֵ ͬʱ�����õ�������һ����ȡֵ�Ĺ�����
	bool end_mark;//�ж��Ƿ���Լ�����ȡ
	//���bool���Ϊ1��ʾ���Լ�����ȡ ������Ϊ0��ô�ͱ�ʾ
	//�Ѿ��������ļ�β��������ԭ�����޷�������ȡ
	void read() {//��ȡ����
		end_mark = (*stream) ? true : false;
		if (end_mark)*stream >> myvalue;//д���� ��ȡ����ֵ��value
		//������ȡ����ֵ
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

	//ʵ����Ԫģ�庯������İ� ��ʽʵ����
	//�����Ĳ����б�����Ҫд��֮�󶨵Ķ�Ӧ���� ������ʵ����һ��һ�İ�
	//��������� ��ϵ�����Ҫ����ͬ���͵ıȽ� ���Ͳ�ͬ���ܱȽ�
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
//��=����Ҫ�������Ԫ ����==����
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
	}//������������� ʹ�������������Բ�����ʽ����ģ������б�<.....>

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



