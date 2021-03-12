#ifndef MY_ALLOCATOR
#define MY_ALLOCATOR
#include<iostream>
#include<new>
#include"mytype_traits.h"
#include"my_iterator.h"
template<typename T1, typename T2>
inline void construct(T1* p, T2& value)
{
	new (p) T1(value);
}
/*template <class U, class... Args>
void construct(U* p, Args&&... args) {
	::new ((U*)p) U(std::forward<Args>(args)...);
	
}//11标准 无头绪*/

	template<typename T>
	inline void destory(T* p)
	{
		p->~T();
	}
	template<typename ForwardIterator>
	inline void destory(ForwardIterator first, ForwardIterator last)
	{
		for (; first < last; first++)
			destory(&(*first));
	}
	inline void destory(char*, char*) {}
	inline void destory(wchar_t*, wchar_t*) {}
	
	template<typename Forwarditerator, typename T, typename T1>
	inline Forwarditerator _uninitialized_fill(Forwarditerator first,
		Forwarditerator last, const T& x, T1*);
	template<typename Forwarditerator, typename T>
	inline Forwarditerator uninitialized_fill(Forwarditerator first, Forwarditerator last, const T& x) {
		return _uninitialized_fill(first, last, x, value_type(first));
	}
	template<typename Forwarditerator, typename T, typename T1>
	inline Forwarditerator _uninitialized_fill(Forwarditerator first,
		Forwarditerator last, const T& x, T1*) {
		using is_pod = typename mytype_traits<T1>::is_POD_type;
		return ufill(first, last, x, is_pod());
	}
	template<typename Forwarditerator, typename T>
	inline Forwarditerator ufill(Forwarditerator first, Forwarditerator last, const T& x, ::true_type) {
		for (; first != last; first++) {
			*first = x;
		}
		return first;
	}
	template<typename Forwarditerator, typename T>
	inline Forwarditerator ufill(Forwarditerator first, Forwarditerator last, const T& x, ::false_type) {
		for (; first != last; first++) {
			construct(&(*first), x);
		}
		return first;
	}
	

	template<typename Forwarditerator, typename size, typename T, typename T1>
	inline Forwarditerator _uninitialized_fill_n(Forwarditerator first,
		size n, const T& x, T1*);
	//此处为前置声明 确保下列函数能正常调用

	template<typename Forwarditerator,typename size,typename T>
	inline Forwarditerator uninitialized_fill_n(
		Forwarditerator first, size n, const T& x
	) {
		return _uninitialized_fill_n(first, n, x,value_type(first));
	}
	template<typename Forwarditerator, typename size, typename T,typename T1>
	inline Forwarditerator _uninitialized_fill_n(Forwarditerator first,
		size n, const T& x, T1*) {
		using is_pod = typename mytype_traits<T1>::is_POD_type;
		return ufill_n(first, n, x, is_pod());
	}
	template<typename Forwarditerator, typename size, typename T>
	inline Forwarditerator ufill_n(Forwarditerator first, size n, const T& x, ::true_type) {
		for (; n > 0; n--, first++) {
			*first = x;
		}
		return first;
	}
	template<typename Forwarditerator, typename size, typename T>
	inline Forwarditerator ufill_n(Forwarditerator first, size n, const T& x, ::false_type) {
		for (; n > 0; n--, first++) {
			construct(&(*first), x);
		}
		return first;
	}


	template<typename inputiterator, typename Forwarditerator, typename T >
	inline Forwarditerator _uninitialized_copy(inputiterator first, inputiterator last,
		Forwarditerator result, T*);
	//此处为前置声明以确保下列函数正常调用

	template<typename inputiterator, typename Forwarditerator>
	inline Forwarditerator uninitialized_copy(inputiterator first, inputiterator last, Forwarditerator result) {
		return  _uninitialized_copy(first, last, result,value_type(result));
	}
	template<typename inputiterator, typename Forwarditerator,typename T >
	inline Forwarditerator _uninitialized_copy(inputiterator first, inputiterator last,
		Forwarditerator result, T*) {
		using is_pod = typename mytype_traits<T>::is_POD_type;
		return ucopy(first, last, result, is_pod());
	}
	/*template<typename Inputiterator, typename outputiterator>
	inline outputiterator copy(Inputiterator first, 
		                       Inputiterator last, outputiterator result) {
		for (; first != last; first++, result++)
			*result(*first);
		return result;
	}*/

	template<typename inputiterator,typename Forwarditerator>
	inline Forwarditerator ucopy(inputiterator first, inputiterator last,
		Forwarditerator result, ::true_type) {
		for (; first != last; first++, result++)
			(*result)=(*first);
		return result;
	}
	template<typename inputiterator, typename Forwarditerator>
	inline Forwarditerator ucopy(inputiterator first, inputiterator last, Forwarditerator result, ::false_type) {
		for (; first != last; first++, result++) {
			construct(&(*result), *first);
		}
		return result;
	}
	//uninitialized_copy这部分省略了char和wchar的特化
	
	template <class InputIterator,class size,class ForwardIterator>
	ForwardIterator uninitialized_copy_n(InputIterator first,size n,
		ForwardIterator result) {
		_uninitialized_copy_n(first,n, result, value_type(result));
	}

	template< class InputIterator, class size,class ForwardIterator, class T >
	ForwardIterator _uninitialized_copy_n(InputIterator first, size n, ForwardIterator result ,T*) {
		using is_pod = typename mytype_traits<T>::is_POD_type;
		return uncopy( first, n, result, is_pod());
	}
	template<class InputIterator, class size, class ForwardIterator >
	ForwardIterator uncopy(InputIterator first, size n, 
		ForwardIterator result, ::true_type) {
		InputIterator cur = first + n;
		for (; first != cur; first++, result++)
			*result = *first;
		return result;
	}
	template<class InputIterator, class size, class ForwardIterator >
	ForwardIterator uncopy(InputIterator first, size n,
		ForwardIterator result, ::false_type) {
			InputIterator cur = first + n;
			for (; first != cur; first++, result++)
				construct(result, *first);
			return result;
	}


	template<int inst>
	class _malloc_alloc//一级空间配置器
	{
	private:
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		
	public:
		static void* allocate(size_t n)
		{
			void* result = malloc(n);
			if (result == 0)
				result = oom_malloc(n);
			return result;
		}

		static void deallocate(void* p, size_t)
		{
			free(p);
		}

		static void* reallocate(void* p, size_t, size_t new_n)
		{
			void* result = realloc(p, new_n);
			if (result == 0)
				result = oom_realloc(p, new_n);
			return result;
		}
	};
	

	template<int inst>
	void* _malloc_alloc<inst>::oom_malloc(size_t n)//初始化
	{
		
		void* result;
		for (;;)
		{
			result = malloc(n);
			if (result)return result;
		}
	}
	template<int inst>
	void* _malloc_alloc<inst>::oom_realloc(void* p, size_t n)//重分配
	{
	
		void* result;
		for (;;)
		{
			
			result = realloc(p, n);
			if (result)return result;
		}
	}
	using malloc_alloc = _malloc_alloc<0>;//一级配置器


	enum { align = 8 };
	enum { max_bytes = 128 };
	enum { counts = max_bytes / 8 };
	
	template<int inst>
	class default_alloc
	{
	private:
		static size_t round_up(size_t bytes)
		{
			return (((bytes)+align-1)&~(align-1));
		}

		union obj {
			union obj* free_list_link;
			char client_data[1];
		};

		static obj* volatile free_list[counts];
		static size_t freelist_index(size_t bytes)
		{
			return (((bytes)+align - 1) / align - 1);//块以下标0开始计算
		}

		static void* refill(size_t n);
		static char* chunk_alloc(size_t size, int& nobjs);
		static char* start_free;
		static char* end_free;
		static size_t heap_size;

	public:

		static void* allocate(size_t n)
		{
			obj* volatile* my_list;
			obj* result;
			
			if (n > max_bytes) { //(n < max_bytes)
				//目前 对于红黑树的内存分配 暂时只能使用一级分配器
				return (malloc_alloc::allocate(n));
			}
			my_list = free_list + freelist_index(n);
			result = *my_list;
			if (result == 0) {
				void* x = refill(round_up(n));
				result = ((obj*)x)->free_list_link;
				return (void*)result;
			}
			
				*my_list = result->free_list_link;
			
			return (void*)result;
		}
		static void deallocate(void* p, size_t n)
		{
			obj* q = (obj*)p;
			obj* volatile* my_free_list;

			if (n > max_bytes)
			{
				malloc_alloc::deallocate(p, n);
				return;
			}
			my_free_list = free_list + freelist_index(n);
			q->free_list_link = *my_free_list;
			*my_free_list = q;

		}
		static void* reallocate(void* p, size_t oldn, size_t newn)
		{
			deallocate(p, oldn);
			return allocate(newn);
		}
	};
	template< int inst>
	char* default_alloc< inst>::start_free = 0;

	template< int inst>
	char* default_alloc< inst>::end_free = 0;

	template<int inst>
	size_t default_alloc< inst>::heap_size = 0;

	template<int inst>
	typename default_alloc<inst>::obj* volatile
		default_alloc<inst>::free_list[counts] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	
		
	

	template< int inst>
	void* default_alloc<inst>::refill(size_t n)
	{
		int cnode = 20;
		char* chunk = chunk_alloc(n, cnode);
		int i;

		obj* result;
		obj* volatile* my_free_list;
		obj* current, * next;

		if (cnode == 1)return (chunk);//基本不会发生 默认为20个块
		my_free_list = free_list + freelist_index(n);
		result = (obj*)chunk;

		*my_free_list = next = (obj*)(chunk + n);
		for (i = 1;; i++) {
			current = (obj*)((char*)next+n);
			//先转换成char 然后把sizeof出来的size_t类型的尺寸相加
			//随后在转换回去
			next =  next + n;
			if (cnode - 1 == i) {
				current->free_list_link = 0;
				break;
			}
			else {
				current->free_list_link = next;
			}
		}
		return(result);
	}
	template< int inst>
	char* default_alloc<inst>::chunk_alloc(size_t size, int& nobjs)//16 20
	{
		char* result;
		size_t total = size * nobjs;
		size_t bytes_left = end_free - start_free;
		if (total <= bytes_left) {
			result = start_free;
			start_free += total;
			return (result);
		}
		else if (bytes_left >= size) {
			nobjs = bytes_left / size;
			total = nobjs * size;
			result = start_free;
			start_free += total;
			return (result);
		}
		else {
			size_t bytes_get = 2 * total + round_up(heap_size >> 4);
			if (bytes_left > 0) {
				obj* volatile* my_free_list = free_list + freelist_index(bytes_left);
				((obj*)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj*)start_free;
			}
			start_free = (char*)malloc(bytes_get);
			if (start_free == 0) {
				int i;
				obj* volatile* my_free_list, * p;

				for (i = size; i < max_bytes; i += align) {
					my_free_list = free_list + freelist_index(i);
					p = *my_free_list;
					if (p != 0) {
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;
						return (chunk_alloc(size, nobjs));
					}
				}
				end_free = 0;//没有找到合适内存情况下调用一级分配器
				start_free = (char*)malloc_alloc::allocate(bytes_get);
			}
			heap_size += bytes_get;//修改堆大小
			end_free = start_free + bytes_get;//新添更多内存之后
			return(chunk_alloc(size, nobjs));//重新递归自身 分配内存给free_list

		}

	}
	using alloc = default_alloc<0>;


	//对一级分配器和次分配器进行包装 统一接口
	template<typename T>
	class my_allocator
	{
		using alloc = default_alloc<0> ;
	public:
		using value_type             = T;
		using pointer                = T*;
		using reference              = T&;
		using const_pointer          = const T*;
		using const_reference        = const T&;
		using size_type              = size_t;
		using difference_type        = ptrdiff_t;

		
		my_allocator()noexcept {};
		my_allocator(const my_allocator&)noexcept {};//同类型创建
		template<typename T1>//不同类型创建
		my_allocator(const my_allocator<T1>&)noexcept {};
		~my_allocator()noexcept {};

		pointer address(reference x)const 
		{
			return &x;
		}
		
		const_pointer address(const_reference x) const
		{
			return &x ;
		}
		
		
		static pointer allocate(size_type n = 1) {//
			return (T*)alloc::allocate(sizeof(T)*n);
		}
		static void deallocate(pointer p, size_type n = 1) {//添加默认值 单参数和双参数版本合并在一起
			alloc::deallocate(p, n * sizeof(T));
		}

		size_type max_size() const noexcept {//没啥用
			return size_t(-1) / sizeof(T);
		}

		template<typename T1, typename T2>
		static void construct(T1* p, T2& value) {
			::construct(p, value);
		}
		void destory(T* p) {
			p->~T();
		}
	};
	template <> class my_allocator<void> {
	public:
		typedef void* pointer;
		typedef const void* const_pointer;
		typedef void value_type;
		
	};

	template<class T,class Alloc>
	class simple_alloc {
	public:
		static T* allocate(size_t n) {
			return 0 == n ? 0 : Alloc::allocate(n * sizeof(T));
		}
		static T* allocate(void ) {
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p, size_t n) {
			if (0 != n)Alloc::deallocate(p,n*sizeof(T));
		}
		static void deallocate(T* p) {
			Alloc::deallocate(p, sizeof(T));
		}
	};


#endif

