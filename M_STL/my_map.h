#ifndef  MY_MAP
#define MY_MAP
#include"mytype_traits.h"
#include"my_allocator.h"
#include"my_iterator.h"
#include"RB-tree.h"
#include<utility>//已无误
//修改时间：2020/11/28
using std::pair;
template<class arg1, class arg2, class result>
struct binary_function {
    using first_argument_type = arg1;
    using seconde_argument_type = arg2;
    using result_type = result;

}; 
template<class arg, class result>
struct unary_function {
    using argument_type = arg;
    using result_type = result;
};
template<class T>
struct my_less :public binary_function<T, T, T> {
    bool operator()(const T& x, const T& y)const {
        T Y(y);
        return x < Y;
    }
};
template<class pair>
struct select1st :public unary_function<pair, typename pair::first_type>
{
    const typename pair::first_type& operator()(const pair& x)const {
        return x.first;
    }
};

template<class Key,class T,class Compare=std::less<Key>,
         class Alloc=alloc>
class map {
public:
    using key_type          = Key;
    using data_type         = T;//mapped_type
    using value_type        = pair<const Key, T>;
    //本身类型限制在了const领域 后续using正常定义即可
    using key_compare       = Compare;

    class value_compare :public binary_function<value_type, value_type, bool> {
        friend class map<Key, T, Compare, Alloc>;
    protected:
        Compare comp;
        value_compare(Compare& x):comp(x){}
    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    };

private:
    using tree_type = rb_tree<key_type, value_type, select1st<value_type>,
                              key_compare, Alloc>;
    tree_type tree;
public:
    using allocator_type            = Alloc;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;
    //引用和指针针对的类型是pair<const Key, T> 键值部分本身已经是const了
    using reference                 = typename tree_type::reference;
    using const_reference           = typename tree_type::const_reference;
    using pointer                   = typename tree_type::pointer;
    using const_pointer             = typename tree_type::const_pointer;
    using iterator                  = typename tree_type::iterator;
    using const_iterator            = typename tree_type::const_iterator;
    using reverse_iterator          = typename tree_type::reverse_iterator;
    using const_reverse_iterator    = typename tree_type::const_reverse_iterator;


public:
    allocator_type get_allocator()const noexcept{
        return allocator_type();
    }

    explicit map(const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()):tree(comp,alloc) {
    }
    explicit map(const allocator_type& alloc):tree(alloc){}

    map(iterator first, iterator last,
        const key_compare& comp = key_compare(),
        const allocator_type &alloc = allocator_type()) :tree(comp,alloc){
        tree.insert_unique(first, last);
    }
    map(value_type* first, value_type* last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :tree(comp, alloc) {
        tree.insert_unique(first, last);
    }
    map(const map& x):tree(x.tree) {}
    map( map&& x) :tree(std::move(x.tree)) {}
    map(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()):tree(comp,alloc) {
        tree.insert_unique(il);
    }
    //~map() { ~tree(); }
    
    map& operator= (const map& x) {
        tree = x.tree;
    }
    map& operator= (map&& x) {
        tree.clear();
        tree.header = std::move(x.tree.header);
        tree.node_count = x.tree.node_count;
        tree.key_compare = x.tree.key_compare;
        x.tree.empty_initialize();
    }
    map& operator= (std::initializer_list<value_type> il) {
        tree.clear();
        tree.insert_unique(il); 
    }
    key_compare key_comp() const {
        return key_compare();
    }
    value_compare value_comp() const {
        return value_compare();
    }
public:
    iterator begin() noexcept { return tree.begin(); }
    const_iterator begin()const noexcept { return tree.begin(); }
    const_iterator cbegin()const noexcept { return tree.cbegin(); }
    reverse_iterator rbegin() noexcept { return tree.rbegin(); }
    reverse_iterator rbegin()const noexcept { return tree.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return tree.crbegin(); }
    iterator end() noexcept { return tree.end(); }
    const_iterator end()const noexcept { return tree.end(); }
    const_iterator cend()const noexcept { return tree.cend(); }
    reverse_iterator rend() noexcept { return tree.rend(); }
    reverse_iterator rend()const noexcept { return tree.rend(); }
    const_reverse_iterator crend() const noexcept { return tree.crend(); }

    bool empty()const noexcept {
        tree.empty();
    }
    size_type size()const noexcept {
        return tree.size();
    }
    size_type max_size()const noexcept {
        tree.max_size();
    }
    data_type& operator[] (const key_type& k) {
        iterator x = tree.lower_bound(k);
        if (x == end() || key_comp()(k, (*x).first)) {
            x = tree.insert_unique(x, value_type(k, data_type()));
        }
        return (*x).second;
    }
    data_type& at(const key_type& k) {
        iterator x = tree.lower_bound(k);
        if (x.node)
            return (*x).second;
        else
            exit(1);//异常没进行处理
    }
    const data_type& at(const key_type& k) const {
        iterator x = tree.lower_bound(k);
        if (x.node)
            return (*x).second;
        else
            exit(1);
    }
    pair<iterator, bool> insert(const value_type& val) {
        return tree.insert_unique(val);
    }
    iterator insert(const_iterator position, const value_type& val) {
        return tree.insert_unique(position, val);
    }
    void insert(iterator first, iterator last) {
        tree.insert_unique(first, last);
    }
    void insert(value_type* first, value_type* last) {
        tree.insert_unique(first, last);
    }
    void insert(std::initializer_list<value_type> il) {
        tree.insert_unique(il);
    }
    void erase(const_iterator position) {
        tree.erase(position);
    }
    size_type erase(const key_type& k) {
        return tree.erase(k);
    }
    void erase(const_iterator first, const_iterator last) {
        tree.erase(first, last);
    }

public:
    void swap(map& x) {
        tree.swap(x.tree);
    }
    void clear() {
        tree.clear();
    }
    iterator find(const key_type& k) {
        return tree.find(k);
    }
    const_iterator find(const key_type& k) const {
        return tree.find(k);
    }
    size_type count(const key_type& k) const {
        return tree.count(k);
    }
    iterator lower_bound(const key_type& k) {
        return tree.lower_bound(k);
    }
    const_iterator lower_bound(const key_type& k)const {
        return tree.lower_bound(k);
    }
    iterator upper_bound(const key_type& k) {
        return tree.upper_bound(k);
    }
    const_iterator upper_bound(const key_type& k) const {
        return tree.upper_bound(k);
    }
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        return (pair<const_iterator, const_iterator>)tree.equal_range(k);
    }
    pair<iterator, iterator> equal_range(const key_type& k) {
        return tree.equal_range(k);
    }

    bool operator==(const map& x) {
        return tree == x.tree;
    }
    bool operator!=(const map& x) {
        return tree != x.tree;
    }
    bool operator<(const map& x) {
        return tree < x.tree;
    }
    bool operator > (const map& x) {
        return tree > x.tree;
    }
    bool operator >= (const map& x) {
        return tree >= x.tree;
    }
    bool operator <= (const map& x) {
        return tree <= x.tree;
    }
};
template<class Key, class T, class Compare = std::less<Key>,
    class Alloc = alloc>
    void swap(map<Key, T, Compare, Alloc>& x, map<Key, T, Compare, Alloc>& y) {
    x.swap(y);
}
#endif // ! MY_MAP
#pragma once
