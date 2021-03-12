#ifndef MY_SET
#define MY_SET
#include"mytype_traits.h"
#include"my_allocator.h"
#include"my_iterator.h"
#include"RB-tree.h"//已无误
//修改时间:2020/11/28/19:38
template<class arg,class result>
struct unary_function {
    using argument_type = arg;
    using result_type = result;
};
template<class arg1,class arg2,class result>
struct binary_function {
    using first_argument_type = arg1;
    using seconde_argument_type = arg2;
    using result_type = result;

};
template<class T>
struct my_less :public binary_function<T,T,T>{
    bool operator()(const T& x,const T& y)const {
        T Y(y);
        return x < Y;
    }
};


template < class T,                        // set::key_type/value_type
    class compare = my_less<T>,        // set::key_compare/value_compare
    class Alloc = my_allocator<T>>      // set::allocator_type
class set {
protected:
    template<class T>
    struct identity :public unary_function<T,T> {
         const T& operator()(const T& x)const { return x; }
    };
    
    using rep_type = rb_tree<T, T, identity<T>,compare,Alloc>;
    rep_type t;//真正的底层结构

public:
    using key_type                      = T;
    using value_type                    = T;
    using key_compare                   = compare;
    using value_compare                 = compare;
    using allocator_type                = typename rep_type::allocator_type;
    using reference                     = typename rep_type::const_reference;
    using const_reference               = const reference;
    using pointer                       = typename rep_type::const_pointer;
    using const_pointer                 = const pointer;
    using iterator                      = typename rep_type::const_iterator;
    using const_iterator                = const iterator;
    using reverse_iterator              = typename rep_type::const_reverse_iterator;
    using const_reverse_iterator        = const reverse_iterator;
    using size_type                     = typename rep_type::size_type;
    using difference_type               = typename rep_type::difference_type;

public:
    explicit set(const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()): t(comp, alloc) {
     
    }
    explicit set(const allocator_type& alloc):t(alloc) {
        
    }
    set(pointer first, pointer last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :t(comp,alloc){
        t.insert_unique(first, last);
    }
    set(iterator first, iterator last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :t(comp, alloc) {
        t.insert_unique(first, last);
    }

    set(const set& x) {
        t(x);
    }
    set(set&& x) {
        t(std::move(x));
    }
    set(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()):t(comp,alloc) {
        t.insert_unique(il);
    }
    ~set() { t.~rb_tree(); }

    set& operator=(const set& x) {
        t = x.t;
    }
    set& operator=(set&& x) {
        t.clear();
        t.header = std::move(x.t.header);
        t.node_count = x.t.node_count;
        t.key_compare = std::move(x.t.key_compare);
        x.t.empty_initialize();
    }
    set& operator= (std::initializer_list<value_type> il) {
        t.clear();
        t.insert_unique(il);
    }


public:
    allocator_type get_allocator()const { return allocator_type(); }
    
    iterator begin()noexcept { return t.cbegin(); }
    const_iterator begin()const noexcept{ return t.cbegin(); }
    const_iterator cbegin()const noexcept { return t.cbegin(); }
    reverse_iterator rbegin() noexcept { return t.crbegin(); }
    const_reverse_iterator crbegin()const noexcept { return t.crbegin(); }
    
    iterator end()noexcept { return t.cend(); }
    const_iterator end()const noexcept { return t.cend(); }
    const_iterator cend()const noexcept { return t.cend(); }
    reverse_iterator rend() noexcept { return t.crend(); }
    const_reverse_iterator crend()const noexcept { return t.crend(); }

public:
    bool empty() const { return t.empty(); }
    size_type size()const { return t.size(); }
    size_type max_size()const { return t.max_size(); }
    //插入右值部分不实现了
    std::pair<iterator, bool> insert(const value_type& val) {
        return t.insert_unique(val);
    }
    iterator insert(const_iterator position, const value_type& val) {
        return t.insert_unique(position,val);
    }
    void insert(iterator first, iterator last) {
        t.insert_unique(first, last);
    }
    void insert(value_type* first, value_type* last) {
        t.insert_unique(first, last);
    }
    void insert(std::initializer_list<value_type> il) {
        t.insert_unique(il);
    }
    void erase(iterator position) {
        t.erase(position);
    }
    size_type erase(const value_type& val) {
        return t.erase(val);
    }
    void erase(iterator first, iterator last) {
        t.erase(first, last);
    }
    void swap(set& x) {
        t.swap(x);
    }
    void clear() {
        t.clear();
    }


public:
    key_compare key_comp()const { return key_compare(); }
    value_compare value_comp()const { return value_compare(); }
    const_iterator find(const value_type& val) const {
        return t.find(val);
    }
    iterator find(const value_type& val) {
       return t.find(val);
    }
    size_type count(const value_type& val) const {
        return t.count(val);
    }
    iterator lower_bound(const key_type& val)const {//查找这块value和key是相同值
        return t.lower_bound(val);
    }
    iterator upper_bound(const key_type& val)const  {
        return t.upper_bound(val);
    }
    std::pair<iterator, iterator> equal_range(const key_type& val)const {
        return t.equal_range(val);
    }
};

template < class T,                        
    class compare = std::less<T>,        
    class alloc = my_allocator<T>>
bool operator==(const set<T, compare, alloc>& x, const set<T, compare, alloc>& y) {
    return x.t == x.t;
}
template < class T,
    class compare = std::less<T>,
    class alloc = my_allocator<T>>
    bool operator!=(const set<T, compare, alloc>& x, const set<T, compare, alloc>& y) {
    return !(operator==(x, y));
}
template < class T,
    class compare = std::less<T>,
    class alloc = my_allocator<T>>
    bool operator<(const set<T, compare, alloc>& x, const set<T, compare, alloc>& y) {
    return x.t < y.t;
}
template < class T,
    class compare = std::less<T>,
    class alloc = my_allocator<T>>
    bool operator>(const set<T, compare, alloc>& x, const set<T, compare, alloc>& y) {
    return (operator==(y, x));
}
template < class T,
    class compare = std::less<T>,
    class alloc = my_allocator<T>>
    bool operator>=(const set<T, compare, alloc>& x, const set<T, compare, alloc>& y) {
    return !(operator==(x, y));
}
template < class T,
    class compare = std::less<T>,
    class alloc = my_allocator<T>>
    bool operator<=(const set<T, compare, alloc>& x, const set<T, compare, alloc>& y) {
    return !(operator==(y, x));
}

#endif // !MY_SET
