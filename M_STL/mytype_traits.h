#ifndef MYTYPE_TRAITS
#define MYTYPE_TRAITS
struct true_type{};
struct false_type {};
template<typename T>
struct mytype_traits {
	using has_trivial_default_ctor = false_type;
	using has_trivial_copy_ctor    = false_type;
	using has_trivial_assignment   = false_type;
	using has_trivial_dector       = false_type;
	using is_POD_type              = false_type;
};

template<>//cahr����
struct mytype_traits<char> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor    = ::true_type;
	using has_trivial_assignment   = ::true_type;
	using has_trivial_dector       = ::true_type;
	using is_POD_type              = ::true_type;
};

template<>//signed char ����
struct mytype_traits<signed char> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//unsigned cahr����
struct mytype_traits<unsigned char> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//short����
struct mytype_traits<short> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//unsigned short����
struct mytype_traits<unsigned short> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//int����
struct mytype_traits<int> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//unsigned int����
struct mytype_traits<unsigned int> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//long����
struct mytype_traits<long> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//unsigned long����
struct mytype_traits<unsigned long> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//float����
struct mytype_traits<float> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//double����
struct mytype_traits<double> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<>//long double����
struct mytype_traits<long double> {
	using has_trivial_default_ctor = ::true_type;
	using has_trivial_copy_ctor = ::true_type;
	using has_trivial_assignment = ::true_type;
	using has_trivial_dector = ::true_type;
	using is_POD_type = ::true_type;
};

template<typename T>
struct mytype_traits<T*> {
	using has_trivial_default_ctor = true_type;
	using has_trivial_copy_ctor = true_type;
	using has_trivial_assignment = true_type;
	using has_trivial_dector = true_type;
	using is_POD_type = true_type;
};
#endif