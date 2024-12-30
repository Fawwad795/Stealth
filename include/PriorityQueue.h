#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <vector>
#include <functional>
using namespace std;

template <typename T, typename Compare = less<T>>
class PriorityQueue {
public:
    // Constructors
    PriorityQueue();
    explicit PriorityQueue(const Compare& comp);
    PriorityQueue(const vector<T>& items, const Compare& comp = Compare());

    // Core operations
    void push(const T& item);
    void pop();
    const T& top() const;

    // Status checks
    bool empty() const;
    size_t size() const;

    // Utility functions
    void clear();

    // Allows access to underlying container for iteration
    const vector<T>& getContainer() const;

private:
    vector<T> heap;
    Compare comparator;

    // Helper functions for maintaining heap property
    void heapifyUp(size_t index);
    void heapifyDown(size_t index);

    // Helper functions for index calculations
    size_t parent(size_t index) const { return (index - 1) / 2; }
    size_t leftChild(size_t index) const { return 2 * index + 1; }
    size_t rightChild(size_t index) const { return 2 * index + 2; }

    // Swap elements
    void swap(size_t i, size_t j);
};

#endif // PRIORITY_QUEUE_H