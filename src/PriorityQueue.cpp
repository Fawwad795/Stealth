// PriorityQueue.cpp
#include "PriorityQueue.h"
#include <stdexcept>
using namespace std;

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue()
    : comparator(Compare()) {
}

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue(const Compare& comp)
    : comparator(comp) {
}

template <typename T, typename Compare>
PriorityQueue<T, Compare>::PriorityQueue(const vector<T>& items, const Compare& comp)
    : heap(items)
    , comparator(comp) {
    // Build heap from bottom up
    for (int i = size() / 2 - 1; i >= 0; --i) {
        heapifyDown(i);
    }
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::push(const T& item) {
    heap.push_back(item);
    heapifyUp(heap.size() - 1);
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::pop() {
    if (empty()) {
        throw runtime_error("Priority queue is empty");
    }

    heap[0] = heap.back();
    heap.pop_back();

    if (!empty()) {
        heapifyDown(0);
    }
}

template <typename T, typename Compare>
const T& PriorityQueue<T, Compare>::top() const {
    if (empty()) {
        throw runtime_error("Priority queue is empty");
    }
    return heap[0];
}

template <typename T, typename Compare>
bool PriorityQueue<T, Compare>::empty() const {
    return heap.empty();
}

template <typename T, typename Compare>
size_t PriorityQueue<T, Compare>::size() const {
    return heap.size();
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::clear() {
    heap.clear();
}

template <typename T, typename Compare>
const vector<T>& PriorityQueue<T, Compare>::getContainer() const {
    return heap;
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::heapifyUp(size_t index) {
    while (index > 0) {
        size_t parentIndex = parent(index);
        if (comparator(heap[parentIndex], heap[index])) {
            swap(index, parentIndex);
            index = parentIndex;
        } else {
            break;
        }
    }
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::heapifyDown(size_t index) {
    size_t size = heap.size();

    while (true) {
        size_t largest = index;
        size_t left = leftChild(index);
        size_t right = rightChild(index);

        if (left < size && comparator(heap[largest], heap[left])) {
            largest = left;
        }

        if (right < size && comparator(heap[largest], heap[right])) {
            largest = right;
        }

        if (largest == index) {
            break;
        }

        swap(index, largest);
        index = largest;
    }
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::swap(size_t i, size_t j) {
    std::swap(heap[i], heap[j]);
}

// Explicit template instantiation for common types
template class PriorityQueue<int>;
template class PriorityQueue<double>;
template class PriorityQueue<float>;