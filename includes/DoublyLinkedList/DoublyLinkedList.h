#ifndef LIST_H
#define LIST_H

#include "Node.h"
using namespace std;

//implementing general functions for a doubly linked list data structure

//printing list from head to tail
template <typename T>
void _print_list(node<T>* head) {
    cout << "Head->";
    for(node<T>* walker = head; walker != nullptr; walker = walker->_next) {
        cout << *walker;
    }
    cout << "|||\n";
}

//printing list in reverse order (recursively)
template <typename T>
void _print_list_backwards(node<T> *head) {}

//searching for a node containing the given key
template <typename T>
node<T>* _search_list(node<T>* head, T key) {
    for(node<T>* walker = head; walker != nullptr; walker = walker->_next) {
        if(walker->_item == key){
            return walker;
        }
    }
    return nullptr;
}

//inserting new node at head of list
template <typename T>
node<T>* _insert_head(node<T> *&head, T insert_this) {
    node<T>* insert_node_ptr = new node<T>(insert_this);
    //handling empty list vs non-empty list cases differently for doubly linked structure
    if(head == nullptr) {
        insert_node_ptr->_next = head;
        head = insert_node_ptr;
    }
    else {
        insert_node_ptr->_next = head;
        head->_prev = insert_node_ptr;
        head = insert_node_ptr;
    }
    return head;
}

//removing and returning the head node's value
template <typename T>
T _remove_head(node<T>* &head) {
    assert(head != nullptr && "Linked list cannot be empty.");
    T removed_item = head->_item;
    node<T>* remove_ptr = head;
    head = remove_ptr->_next;
    delete remove_ptr;
    remove_ptr = nullptr;
    //maintaining prev pointers when non-empty
    if(head != nullptr)
        head->_prev = nullptr;
    return removed_item;
}

//inserting new node after the given node
template <typename T>
node<T>* _insert_after(node<T>*& head, node<T> *after_this, T insert_this)
{
    if(head == nullptr)
        return _insert_head(head, insert_this);
    else
    {
        if(after_this == nullptr)
            return _insert_head(head, insert_this);
        node<T>* insert_node_ptr = new node<T>(insert_this);
        insert_node_ptr->_next = after_this->_next;
        insert_node_ptr->_prev = after_this;
        after_this->_next = insert_node_ptr;
        //maintaining doubly linked structure by updating next node's prev pointer
        if(insert_node_ptr->_next != nullptr)
            insert_node_ptr->_next->_prev = insert_node_ptr;
        return insert_node_ptr;
    }
}

//inserting new node before the given node
template <typename T>
node<T>* _insert_before(node<T>*& head, node<T>* before_this, T insert_this) {
    if(head == nullptr) {
        return _insert_head(head, insert_this);
    }
    node<T>* previous_node = before_this->_prev;
    return _insert_after(head, previous_node, insert_this);

}

//getting pointer to previous node
template <typename T>
node<T>* _previous_node(node<T>* prev_to_this) {
    assert(prev_to_this != nullptr && "prev_to_this cannot be nullptr");
    return prev_to_this->_prev;
}

//deleting specified node and returning its value
template <typename T>
T _delete_node(node<T>*& head, node<T>* delete_this) {
    if(head == delete_this) return _remove_head(head);

    T deleted_item = delete_this->_item;
    node<T>* previous_node = delete_this->_prev;
    node<T>* next_node = delete_this->_next;
    //handling case where node is last in list
    if(next_node == nullptr)
        previous_node->_next = delete_this->_next;
    else
    {
        previous_node->_next = delete_this->_next;
        next_node->_prev = previous_node;
    }
    delete delete_this;

    return deleted_item;
}

//creating a copy of the entire list
template <typename T>
node<T>* _copy_list(node<T>* head) {
    node<T>* copy_head = nullptr;
    return_copy_list(copy_head, head);
}

//copying source list to destination, returning pointer to last node
template <typename T>
node<T>* _copy_list(node<T> *&dest, node<T> *src){
    _clear_list(dest);
    if(src == nullptr) return nullptr;
    node<T>* src_walker = src;
    dest = new node<T>(src_walker->_item);
    node<T>* dest_walker = dest;
    //copying remaining nodes while maintaining prev pointers
    for(src_walker = src->_next; src_walker != nullptr; src_walker = src_walker->_next) {
        dest_walker->_next = new node<T>(src_walker->_item);
        node<T>* previous_node = dest_walker;
        dest_walker = dest_walker->_next;
        dest_walker->_prev = previous_node;
    }
    return dest_walker;
}

//deleting all nodes in the list
template <typename T>
void _clear_list(node<T>*& head) {
    while(head != nullptr)
        _remove_head(head);
}

//accessing item at specified position
template <typename T>
T& _at(node<T>* head, int pos) {
    int size = 0;
    for(node<T>* walker = head; walker != nullptr; walker = walker->_next)
    {
        if(size == pos)
            return walker->_item;
        size++;
    }
    assert(pos < size && "Index must be less than size");
}

#endif