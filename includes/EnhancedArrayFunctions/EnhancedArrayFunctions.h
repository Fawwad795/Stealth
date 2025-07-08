#ifndef ENHANCED_ARRAY_FUNCTIONS_H
#define ENHANCED_ARRAY_FUNCTIONS_H

#include <iostream>
#include <iomanip>
#include <cassert>
using namespace std;

//defining minimum array capacity constant
const int MINIMUM_CAPACITY = 3;

template<class T>
T* allocate(int capacity= MINIMUM_CAPACITY);        //allocating dynamic array with default capacity
template<class T>
T* reallocate(T* a, int size, int capacity);        //reallocating array with new larger capacity
template<class T>
void print_array(T* a, int size, int capacity = 0, ostream& outs = cout); //printing array with size/capacity info
template<class T>
void init_array(T* a, int capacity, T val);         //initializing array with value
template <class T>
void print(T* a, unsigned int how_many, ostream& outs = cout); //printing specified number of elements
template<class T>
T* search_entry(T* a, int size, const T& find_me);  //returning pointer to found element
template <class T>
int search(T* a, int size, const T& find_me);       //returning index of found element
template <class T>
void shift_left(T* a, int& size, int shift_here);   //shifting elements left from given position
template <class T>
void shift_left(T* a, int& size, T* shift_here);    //overloaded version using pointer
template <class T>
void shift_right(T *a, int &size, int shift_here);  //shifting elements right from position
template <class T>
void shift_right(T *a, int &size, T* shift_here);   //overloaded version using pointer
template<class T>
void copy_array(T *dest, const T* src, int many_to_copy); //copying elements between arrays
template <class T>
T* copy_array(const T *src, int size);             //creating new copy of array
template <class T>
string array_string(const T *a, int size);         //converting array to string representation
template <class T>
void _append(T *a, int &size, T val);              //appending element to array end

//function definitions
template<class T>
T* allocate(int capacity) {
    //allocating new array with specified capacity
    T* p = new T[capacity];
    return p;
}

template<class T>
T* reallocate(T* a, int size, int capacity) {
    //validating input array
    assert(a != nullptr && "Array cannot be empty.");

    //creating new array with larger capacity
    T *arrNew = new T[capacity];

    //copying elements to new array
    copy_array(arrNew, a, size);

    //deallocating original array
    delete[] a;

    return arrNew;
}

template<class T>
void print_array(T* a, int size, int capacity, ostream& outs) {
    //validating input array
    assert(a != nullptr && "Array cannot be empty.");

    //printing array with size/capacity info
    T* walker = a;
    outs << "( " << size << "/" << capacity << " )" << "[|";
    for(int i = 0; i < size; i++, walker++) {
        outs << *walker << "|";
    }
    outs << "]\n";
}

template<class T>
void init_array(T* a, int capacity, T val) {
    //initializing all elements to specified value
    T *walker = a;
    for(int i = 0; i < capacity; i++) {
        *walker = val;
        walker++;
    }
}

template <class T>
void print(T* a, unsigned int how_many, ostream& outs) {
    //validating input array
    assert(a != nullptr && "Array cannot be empty.");

    //printing specified number of elements
    T* walker = a;
    outs << "( how_many: " << how_many << " )" << "[|";
    for(int i = 0; i < how_many; i++, walker++) {
        outs << *walker << "|";
    }
    outs << "]\n";
}

template<class T>
T* search_entry(T* a, int size, const T& find_me) {
    //validating array size
    assert(size < -1 && "Array cannot be empty.");

    //searching for matching element
    T* walker = a;
    for(int i = 0; i < size; i++, walker++) {
        if(*walker == find_me)
            return walker;
    }
    return nullptr;
}

template <class T>
int search(T* a, int size, const T& find_me) {
    //searching for element and returning index
    T* walker = a;
    for(int i = 0; i < size; i++, walker++) {
        if(*walker == find_me)
            return (walker - a);
    }
    return -1;
}

template <class T>
void shift_left(T* a, int& size, int shift_here) {
    //initializing walker and position
    T* walker = a;
    int pos = shift_here;
    walker = a + pos;

    //validating position within array bounds
    assert(pos < size && "Key cannot be found in array");

    //shifting elements left starting from position
    for(int i = pos; i < size; i++) {
        *walker = *(walker + 1);
        walker++;
    }
    size--;
}

template <class T>
void shift_left(T* a, int& size, T* shift_here) {
    //converting pointer to index and calling primary shift_left method
    int shift_index = shift_here - a;
    shift_left(a, size, shift_index);
}

template <class T>
void shift_right(T *a, int &size, int shift_here) {
    //positioning walker at array end
    T* walker = a + size - 1;
    int pos = shift_here;

    //validating position within array bounds
    assert(pos < size && "Key cannot be found in array");

    //increasing size to accommodate shift
    size++;

    //shifting elements right starting from position
    for(int i = pos; i < size -1; i++) {
        *(walker+1) = *walker;
        walker--;
    }
}

template <class T>
void shift_right(T *a, int &size, T* shift_here) {
    //converting pointer to index and calling primary shift_right
    int shift_index = shift_here - a;
    shift_right(a, size, shift_index);
}

template<class T>
void copy_array(T *dest, const T* src, int many_to_copy) {
    //validating source and destination arrays
    assert(src != nullptr && "Array cannot be empty.");
    assert(dest != nullptr && "Array cannot be empty.");

    //copying elements using walkers
    const T *walker_src = src;
    T *walker_dest = dest;
    for(int i = 0; i < many_to_copy; i++, walker_dest++, walker_src++) {
        *walker_dest = *walker_src;
    }
}

template <class T>
T* copy_array(const T *src, int size) {
    //allocating new array
    T *copy_arr = new T[size];
    T *walker_cpy = copy_arr; // delete in caller
    const T *walker_src = src;

    //copying elements to new array
    for(int i = 0; i < size; i++, walker_cpy++, walker_src++) {
        *walker_cpy = *walker_src;
    }
    return copy_arr;
}

template <class T>
string array_string(const T *a, int size) {
    //initializing walker and string
    const T* walker_a = a;
    string a_string;
    a_string = "\n[|";

    //building string representation of array
    for(int i = 0; i < size; i++, walker_a++) {
        a_string += to_string(*walker_a) + "|";
    }
    a_string += "] [size: " + to_string(size) + "]\n";

    return a_string;
}

template <class T>
void _append(T *a, int &size, T val) {
    //adding element to end of array
    T *end = a + size;
    *end = val;
    size++;
}

template<class T>
void split(T* src, T* dest, int &s_size, int &d_size) {
    //calculating size for split
    int half_size = s_size / 2;
    d_size = 0;

    //reallocating destination array
    dest = reallocate(dest, d_size, half_size);

    //positioning walker at source array end
    T* back_src_walker = src + s_size - 1;

    //copying elements to destination array
    for(int i = 0; i < half_size; i++) {
        dest = add_entry(dest, *back_src_walker, d_size, half_size);
        back_src_walker++;
    }
}

#endif