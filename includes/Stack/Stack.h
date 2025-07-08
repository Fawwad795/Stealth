#ifndef STACK_H
#define STACK_H

#include "../DoublyLinkedList/DoublyLinkedList.h"
using namespace std;

//implementing a stack data structure using a doubly linked list
template <typename T>
class Stack {
public:
    //implementing iterator class for traversing stack elements
    class Iterator {
    public:
        friend class Stack;
        //constructing empty iterator
        Iterator() { _ptr = NULL; }
        //constructing iterator pointing to specified node
        Iterator(node<T>* p) { _ptr = p; }

        //enabling boolean context usage (if(iterator))
        operator bool() {
            return _ptr;
        }

        //accessing the value at current position
        T& operator *() {
            return _ptr->_item;
        }

        //accessing members of current item
        T* operator ->() {
            T* item_ptr = &_ptr->_item;
            return item_ptr;
        }

        //checking if iterator is null
        bool is_null() {
            return !_ptr;
        }

        //comparing iterators for inequality
        friend bool operator !=(const Iterator& left, const Iterator& right) {
            return !(left == right);
        }

        //comparing iterators for equality
        friend bool operator ==(const Iterator& left, const Iterator& right) {
            return left._ptr == right._ptr;
        }

        //moving iterator to next position (prefix)
        Iterator& operator++() {
            _ptr = _ptr->_next;
            return *this;
        }

        //moving iterator to next position (postfix)
        friend Iterator operator++(Iterator& it, int unused) {
            it._ptr = it._ptr->_next;
            return Iterator(it._ptr);
        }

    private:
        node<T>* _ptr;                          //pointer to current node
    };

    //constructing empty stack
    Stack();
    //copying existing stack
    Stack(const Stack<T>& copyMe);
    //cleaning up stack resources
    ~Stack();
    //assigning from another stack
    Stack<T>& operator=(const Stack<T>& RHS);

    //accessing top element of stack
    T top();
    //checking if stack is empty
    bool empty();
    //adding element to top of stack
    void push(T item);
    //removing and returning top element
    T pop();

    //outputting stack contents
    template<typename TT>
    friend ostream& operator<<(ostream& outs, const Stack<TT>& printMe);

    //getting iterator to first element
    Iterator begin() const;
    //getting iterator to end (null)
    Iterator end() const;
    //getting number of elements
    int size() const { return _size; }

private:
    node<T>* _top;     //pointer to top element
    int _size;         //number of elements
};

//constructing empty stack
template <typename T>
Stack<T>::Stack() {
   const bool debug = false;
    if(debug) {
        cout << "Stack CTOR() Fired.\n";
    }
    _top = nullptr;
    _size = 0;
}

//copying existing stack
template <typename T>
Stack<T>::Stack(const Stack<T>& copyMe) {
    const bool debug = false;
    if(debug) {
        cout << "Stack Copy CTOR() Fired.\n";
    }
    if(copyMe._top == nullptr) {
        _top = nullptr;
        _size = 0;
    }
    else {
        _top = nullptr;
        _copy_list(_top, copyMe._top);
        _size = copyMe._size;
    }
}

//cleaning up stack resources
template <typename T>
Stack<T>::~Stack() {
    const bool debug = false;
    if(debug) {
        cout << "Stack DTOR() Fired.\n";
    }
    _clear_list(_top);
    _top = nullptr;
    _size = 0;
}

//assigning from another stack
template <typename T>
Stack<T>& Stack<T>::operator=(const Stack<T>& RHS) {
    const bool debug = false;
    if(debug) {
        cout << "Stack Assignment Operator Fired.\n";
    }
    if(this == &RHS)
        return *this;
    else {
        _clear_list(_top);
        _top = nullptr;
        _copy_list(_top, RHS._top);
        _size = RHS._size;
        return *this;
    }
}

//accessing top element
template <typename T>
T Stack<T>::top() {
    return _top->_item;
}

//checking if stack is empty
template <typename T>
bool Stack<T>::empty() {
    return _top == nullptr;
}

//adding element to top of stack
template <typename T>
void Stack<T>::push(T item) {
    _size++;
    _insert_head(_top, item);
}

//removing and returning top element
template <typename T>
T Stack<T>::pop() {
    assert(_top != nullptr && "Cannot pop an Empty Stack.");
    _size--;
    return _remove_head(_top);
}

//getting iterator to first element
template<typename T>
typename Stack<T>::Iterator Stack<T>::begin() const {
    return Iterator(_top);
}

//getting iterator to end (null)
template<typename T>
typename Stack<T>::Iterator Stack<T>::end() const {
    return nullptr;
}

//outputting stack contents
template<typename TT>
ostream& operator<<(ostream& outs, const Stack<TT>& printMe) {
    outs << "Stack:Head->";
    for(node<TT>* walker = printMe._top; walker != nullptr; walker = walker->_next) {
        outs << *walker;
    }
    outs << "|||\n";
    return outs;
}

#endif //STACK_H