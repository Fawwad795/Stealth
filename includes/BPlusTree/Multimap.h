#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "Mpair.h"
#include "../STLUtilities/vector_utilities.h"
#include "bplustree.h"
using namespace std;

//implementing a multimap container that allows duplicate keys using BPlusTree
template <typename K, typename V>
class MMap {
public:
    typedef BPlusTree<MPair<K, V> > map_base;

    //implementing iterator class for traversing multimap elements
    class Iterator {
    public:
        friend class MMap;

        //constructing empty iterator
        Iterator() {
            _it = nullptr;
        }

        //constructing iterator from BPlusTree iterator
        Iterator(typename map_base::Iterator it) {
            _it = it;
        }

        //implementing postfix increment operator
        Iterator operator ++(int unused) {
            return Iterator(_it++);
        }

        //implementing prefix increment operator
        Iterator operator ++() {
            return Iterator(++_it);
        }

        //dereferencing iterator to access MPair
        MPair<K, V> operator *() {
            return *_it;
        }

        //comparing iterators for equality
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs) {
            return lhs._it == rhs._it;
        }

        //comparing iterators for inequality
        friend bool operator !=(const Iterator& lhs, const Iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        typename map_base::Iterator _it;
    };

    //initializing empty multimap with duplicates enabled
    MMap():mmap(true) { }

    //getting iterator to first element
    Iterator begin() {
        return Iterator(mmap.begin());
    }

    //getting iterator to position after last element
    Iterator end() {
        return Iterator(mmap.end());
    }

    //checking if multimap is empty
    int size() const {
        return mmap.elements_count();
    }

    //getting number of elements in multimap
    bool empty() const {
        return !size();
    }

    //accessing value list by key (const version)
    const vector<V>& operator[](const K& key) const {
        return mmap.get(key).value_list;
    }

    //accessing value list by key
    vector<V>& operator[](const K& key) {
        return mmap.get(key).value_list;
    }

    //inserting key-value pair, allowing duplicates
    void insert(const K& k, const V& v) {
        MPair<K, V> p(k, v);
        mmap.insert(p);
    }

    //removing all elements with given key
    void erase(const K& key) {
        mmap.remove();
    }

    //removing all elements
    void clear() {
        mmap.clear_tree();
    }

    //checking if key exists in multimap
    bool contains(const K& key) const {
        return mmap.contains(key);
    }

    //getting value list associated with key
    vector<V>& get(const K& key) {
        return mmap.get(key).value_list;
    }

    //accessing value list by key with bounds checking
    vector<V>& at(const K& key) {
        return mmap.get(key).value_list;
    }

    //finding element by key and returning iterator
    Iterator find(const K& key) {
        return Iterator(mmap.find(key));
    }

    //counting number of elements
    int count(const K& key) {
        return mmap.elements_count();
    }

    //getting iterator to first element not less than key
    Iterator lower_bound(const K& key) {
        return Iterator(mmap.lower_bound(key));
    }

    //getting iterator to first element greater than key
    Iterator upper_bound(const K& key) {
        return Iterator(mmap.upper_bound(key));
    }

    //getting range of elements equal to key
    Iterator equal_range(const K& key) {
        return Iterator();
    }

    //validating multimap structure
    bool is_valid() {
        return mmap.is_valid();
    }

    //outputting multimap contents
    friend ostream& operator<<(ostream& outs, const MMap<K, V>& print_me) {
        outs<<print_me.mmap<<endl;
        return outs;
    }

    //displaying lookup table of key-value pairs
    void print_lookup() {
        const bool debug = false;
        if(debug) {
            cout<<"entered print_lookup\n";
            cout<<"mmap.size(): "<<size()<<"\n";
        }
        if(!empty()) {
            for(Iterator it = mmap.begin(); it != mmap.end(); it++) {
                cout<<setw(20)<<*it<<"\n";
            }
        }
        if(debug)
            cout<<"mmap.begin(): "<<*(mmap.begin())<<"\n";
    }

private:
    BPlusTree<MPair<K, V> > mmap;
};

#endif //MULTIMAP_H