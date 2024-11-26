#include "DoublyLinkedList.h"

//constructor implementation to initialize an empty list
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(){
    head = nullptr;
    tail = nullptr;
}

//constant empty function implementation to check whether the list is empty or not
template <typename T>
bool DoublyLinkedList<T>::empty() const {
    return head == nullptr;
}

//insertToHead function implementation to insert a new element at the head of the list
template <typename T>
void DoublyLinkedList<T>::insertToHead(T value) {
    if (empty()) {      //if list is empty, initialize head and tail of the list to point at the same new node
        head = tail = new Node<T>(value);
    } else {            //if list is not empty, then insert at head, and set the pointer to the new head
        head = new Node<T>(value, head);
        head->next->prev = head;
    }
}

//popFront function implementation to remove an element from the head of the list
template <typename T>
void DoublyLinkedList<T>::popFront() {
    if (empty()) {      //if list is empty, print error
        std::cerr << "List is empty!\n";
    } else {            //else if list is not empty and...
        if (head == tail) {     //only one element exists in the list
            delete head;
            head = tail = nullptr;
        } else {        //multiple elements exist in the list, so delete the head
            head = head->next;
            delete head->prev;
            head->prev = nullptr;
        }
    }
}

//front function to get value at the head of the list
template <typename T>
T DoublyLinkedList<T>::front() const {
    if (empty()) {
        throw std::runtime_error("List is empty!");
    }
    return head->data;
}

//function remove to remove a specific element from the list
template <typename T>
void DoublyLinkedList<T>::remove(T value) {
    if (empty()) {
        std::cout << "List is empty!\n";
        return;
    }

    Node<T>* current = head;
    while (current != nullptr) {
        if (current->data == value) {
            // If it's the only node
            if (current == head && current == tail) {
                delete current;
                head = tail = nullptr;
            }
            // If it's the head node
            else if (current == head) {
                head = head->next;
                head->prev = nullptr;
                delete current;
            }
            // If it's the tail node
            else if (current == tail) {
                tail = tail->prev;
                tail->next = nullptr;
                delete current;
            }
            // If it's in the middle
            else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                delete current;
            }
            return;
        }
        current = current->next;
    }

    std::cout << "Value not found in list!\n";
}

// Display the list from head to tail
template <typename T>
void DoublyLinkedList<T>::display() const {
    if (empty()) {
        std::cout << "List is empty!\n";
    } else {
        Node<T>* current = head;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << "\n";
    }
}

// Display the list from tail to head
template <typename T>
void DoublyLinkedList<T>::displayReversed() const {
    if (empty()) {
        std::cout << "List is empty!\n";
    } else {
        Node<T>* current = tail;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->prev;
        }
        std::cout << "\n";
    }
}

// Explicit template instantiation (for int type)
template class DoublyLinkedList<int>;
