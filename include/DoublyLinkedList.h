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

    //function to check if the list is empty
    bool empty() const;

    //function to insert a new element/value in the list
    void insertToHead(T value);

    //function to remove an element from the head of the list
    void popFront();

    //function to peak the element present at the head of the list
    T front() const;

    //function to remove a specific element from the list
    void remove(T value);

    //function to display the list
    void display() const;

    //function to display the list in the reverse order
    void displayReversed() const;

private:
    Node<T>* head;
    Node<T>* tail;
};

#endif // DOUBLYLINKEDLIST_H
