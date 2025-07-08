#ifndef QUEUE_H
#define QUEUE_H

#include "../DoublyLinkedList/DoublyLinkedList.h"
#include "../Token/token.h"

//implementing a queue data structure using a doubly linked list
template <typename T>
class Queue {
public:
    //implementing iterator class for traversing queue elements
    class Iterator {
    public:
        friend class Queue;
        //constructing empty iterator
        Iterator() { _ptr = NULL; }
        //constructing iterator pointing to specified node
        Iterator(node<T>* p) { _ptr = p; }

        operator bool() { return _ptr; }

        //accessing the value at current position
        T& operator *() { return _ptr->_item; }

        //accessing members of current item
        T* operator ->() {
            T* item_ptr = &_ptr->_item;
            return item_ptr;
        }

        //checking if iterator is null
        bool is_null() { return !_ptr; }

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

    //constructing empty queue
    Queue();
    //copying existing queue
    Queue(const Queue<T>& copyMe);
    //cleaning up queue resources
    ~Queue();
    //assigning from another queue
    Queue& operator=(const Queue<T>& RHS);

    //checking if queue is empty
    bool empty();
    //accessing first element
    T front();
    //accessing last element
    T back();

    //adding element to end of queue
    void push(T item);
    //removing and returning first element
    T pop();
    //removing all elements
    void clear();

    //getting iterator to first element
    Iterator begin() const;
    //getting iterator to end (null)
    Iterator end() const;
    //debugging pointer structure
    void print_pointers();
    //getting number of elements
    int size() const { return _size; }

    //outputting queue contents
    template<typename TT>
    friend ostream& operator << (ostream& outs, const Queue<TT>& printMe);
private:
    node<T>* _front;    //pointer to first element
    node<T>* _rear;     //pointer to last element
    int _size;          //number of elements
};

//constructing empty queue
template <typename T>
Queue<T>::Queue() {
    _front = nullptr;
    _rear = nullptr;
    _size = 0;
}

//copying existing queue
template <typename T>
Queue<T>::Queue(const Queue<T>& copyMe) {
    if(copyMe._front == nullptr) {
        assert(copyMe._rear == nullptr && "Rear should be nullptr when front is nullptr.");
        _front = nullptr;
        _rear = nullptr;
        _size = 0;
    }
    else {
        _front = nullptr;
        _rear = _copy_list(_front, copyMe._front);
        _size = copyMe._size;
    }
}

//cleaning up queue resources
template <typename T>
Queue<T>::~Queue() {
    const bool debug = false;
    _clear_list(_front);
    _front = nullptr;
    _rear = nullptr;
    _size = 0;
}

//assigning from another queue
template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& RHS) {
    if(this == &RHS)
        return *this;
    else {
        _clear_list(_front);
        _front = nullptr;
        _rear = nullptr;
        _rear = _copy_list(_front, RHS._front);
        _size = RHS._size;
        return *this;
    }
}

//checking if queue is empty
template <typename T>
bool Queue<T>::empty() {
    if(_front == nullptr) {
        assert(_rear == nullptr && "Rear should be nullptr when front is nullptr.");
        return true;
    }
    else
        return false;
}

//accessing first element
template <typename T>
T Queue<T>::front() {
    return _front->_item;
}

//accessing last element
template <typename T>
T Queue<T>::back() {
    return _rear->_item;
}

//adding element to end of queue
template <typename T>
void Queue<T>::push(T item) {
    if(_front == nullptr) {
        assert(_rear == nullptr && "Rear should be nullptr when front is nullptr.");
        _size++;
        _front = _insert_head(_rear, item);
    }
    else {
        _size++;
        _rear = _insert_after(_rear, _rear, item);
    }
}

//removing and returning first element
template <typename T>
T Queue<T>::pop() {
    assert(_front != nullptr && "Cannot pop an Empty Stack.");
    _size--;
    if(_front == _rear) {
        _rear = nullptr;
        return _remove_head(_front);
    }
    else {
        return _remove_head(_front);
    }
}

//removing all elements
template <typename T>
void Queue<T>::clear() {
    _clear_list(_front);
    _front = nullptr;
    _rear = nullptr;
    _size = 0;
}

//getting iterator to first element
template <typename T>
typename Queue<T>::Iterator Queue<T>::begin() const {
    return Iterator(_front);
}

//getting iterator to end (null)
template <typename T>
typename Queue<T>::Iterator Queue<T>::end() const {
    return nullptr;
}

//debugging pointer structure
template <typename T>
void Queue<T>::print_pointers() {
    _print_list(_front);
}

//outputting queue contents
template<typename TT>
ostream& operator << (ostream& outs, const Queue<TT>& printMe) {
    outs << "Queue:Head->";
    for(node<TT>* walker = printMe._front; walker != nullptr; walker = walker->_next) {
        outs << *walker;
    }
    outs << "|||\n";
    return outs;
}

#endif //QUEUE_H