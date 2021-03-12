#pragma once
#ifndef FUNCTIONAL
#define FUNCTIONAL

template<class Arg,class Result>
struct unary_function {
	using argument_type = Arg;
	using result_type = result;
};

template<class Arg1,class Arg2,class Result>
struct binary_function {
	using first_argument_type = Arg1;
	using second_argument_type = Arg2;
	using result_type = Result;
};


//算术类仿函数
template<class T>
struct plus :public binary_function<T,T,T> {
	T operator()(const T& x, const T& y)const {
		return x + y;
	}
};
template<class T>
struct minus :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const {
		return x - y;
	}
};
template<class T>
struct multiplies :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const {
		return x*y;
	}
};
template<class T>
struct divides :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const {
		return x / y;
	}
};
template<class T>
struct modulus :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const {
		return x % y;
	}
};
template<class T>
struct negate :public unary_function<T, T> {
	T operator()(const T& x)const {
		return -x;
	}
};

//证同
template<class T>
inline
T identity_element(plus<T>) {
	return T(0);
}
template<class T>
inline 
T identity_element(multiplies<T>) {
	return T(1);
}
template<class T>
struct identity :public unary_function<T, T> {
	const T& operator()(const T& x)const {
		return x;	
	}
};


//关系运算符仿函数
template<class T>
struct equal_to :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const{
		return x == y;
	}
};
template<class T>
struct not_equal_to :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const {
		return x == y;
	}
};
template<class T>
struct greater :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const {
		return x > y;
	}
};
template<class T>
struct less :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const {
		return x < y;
	}
};
template<class T>
struct greater_equal :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const {
		return x >= y;
	}
};
template<class T>
struct less_equal :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const {
		return x <= y;
	}
};



//逻辑运算符仿函数
template<class T>
struct logical_and :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const{
		return x && y;
	}
};
template<class T>
struct logical_or :public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const {
		return x || y;
	}
};
template<class T>
struct logical_not :public unary_function<T,bool> {
	bool operator()(const T& x) const {
		return !x;
	}
};

template<class Pair>
struct select1st :public unary_function<Pair, typename Pair::first_type> {
	const typename Pair::first_type& operator()(const Pair& x)const {
		return x.first;
	}
};
template<class Pair>
struct select2nd :public unary_function<Pair, typename Pair::second_type> {
	const typename Pair::second_type& operator()(const Pair& x)const {
		return x.second;
	}
};


template<class Predicate>
class unary_negate :public unary_function<typename Predicate::argument_type, bool> {
protected:
	Predicate pred;
public:
	explicit unary_negate(const Predicate& x):pred(x){}
	bool operator()(const typename Predicate::argument_type& x)const {
		return !pred(x);
	}
};
template<class Predicate>//factory functional
inline unary_negate<Predicate> not1(const Predicate& x) {
	return unary_negate<Predicate>(x);
}



template<class Predicate>
class binary_negate :public binary_function
	<typename Predicate::first_argument_type, 
	 typename Predicate::second_argument_type,
	 bool> {
protected:
	Predicate pred;
public:
	explicit binary_negate(const Predicate& x) :pred(x) {}
	bool operator()(const typename Predicate::first_argument_type& x,
					const typename Predicate::second_argument_type& y)const {
		return !pred(x,y);
	}
};
template<class Predicate>//factory functional
inline binary_negate<Predicate> not2(const Predicate& x) {
	return binary_negate<Predicate>(x);
}


//98--bind
template<class Operation>
class binder1st :public unary_function<typename Operation::second_argument_type,
	typename Operation::result_type> {
protected:
	Operation op;
	typename Operation::first_argument_type value;
public:
	binder1st(const Operation& x,
		const typename Operation::first_argument_type& y):op(x),value(y){}

	typename Operation::result_type
		operator()(const typename Operation::second_argument_type& x)const {
		return op(value, x);
	}
};
template<class Operation,class T>
inline binder1st<Operation>
bind1st(const Operation& op, const T& value) {
	using first_type = typename Operation::first_argument_type;
	return binder1st<Operation>(op, first_type(value));
}

template<class Operation>
class binder2nd :public unary_function<typename Operation::first_argument_type,
	typename Operation::result_type> {
protected:
	Operation op;
	typename Operation::second_argument_type value;
public:
	typename Operation::result_type
		operator()(const typename Operation::first_argument_type& x)const {
		return op(value, x);
	}
	bind2nd(const Operation& x,
		const typename Operation::second_argument_type& y) :op(x), value(y) {

	}
};
template<class Operation, class T>
inline binder2nd<Operation>
bind2nd(const Operation& op, const T& value) {
	using second_type = typename Operation::second_argument_type;
	return binder1st<Operation>(op, second_type(value));
}





#endif // !FUNCTIONAL
