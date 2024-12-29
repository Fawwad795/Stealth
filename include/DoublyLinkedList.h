#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include <iostream>

//Node class: represents a unit in the list
template <typename T>
class Node {
public:
    //Constructor initializing an empty node
    Node() : next(nullptr), prev(nullptr) {}

    //Constructor for initializing a node with value, a predecessor and a successor (if exists)
    Node(T value, Node* nxt = nullptr, Node* prv = nullptr) {
        data = value;
        next = nxt;
        prev = prv;
    }

    //public data members of the node class
    T data;
    Node* next;
    Node* prev;
};

//Doubly Linked List Class: made up of nodes
template <typename T>
class DoublyLinkedList {
public:
    //Constructor for Doubly Linked List class
    DoublyLinkedList();

    bool empty() const;                     //method to check if the list is empty
    void insertToHead(T value);             //method to insert a new element/value in the list
    void deleteFromHead();                  //method to remove an element from the head of the list
    T peekHead() const;                     //method to peek the element present at the head of the list
    void remove(T value);                   //method to remove a specific element from the list
    void display() const;                   //method to display the list
    void displayReversed() const;           //method to display the list in the reverse order
    T getNext(const T& value) const;        //method to get the next element given a value
    bool contains(const T& value) const;    //method to check if a value exists in the list
    int getPosition(const T& value) const;  //method to get current position of a value

private:
    Node<T>* head;
    Node<T>* tail;
};

#endif // DOUBLYLINKEDLIST_H
