#ifndef SORTED_PQ_H
#define SORTED_PQ_H

#include "SPsPQ.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

// A specialized version of the 'heap' ADT that is implemented with an
// underlying sorted array-based container.

/// NOTE: The most extreme element should be found at the end of the
/// 'data' container, such that traversing the iterators yields the elements in
/// sorted order.

template <typename T, typename Compare = std::less<T>>
class SortedPQ : public SPsPQ<T, Compare>
{

public:

    // Default constructor
    explicit SortedPQ(const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp) {}


    // Range-based constructor
    template <typename Iterator>
    SortedPQ(Iterator begin, Iterator end, const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end) {
        updatePQ();
    }


    // Copy constructor
    SortedPQ(const SortedPQ& other) 
        : SPsPQ<T, Compare>(other), data(other.data) {}

    // Move constructor
    SortedPQ(SortedPQ&& other) noexcept
        : SPsPQ<T, Compare>(std::move(other)), data(std::move(other.data)) {}

    // Copy assignment
    SortedPQ& operator=(const SortedPQ& other) {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(other);
            data = other.data;
        }
        return *this;
    }

    // Move assignment
    SortedPQ& operator=(SortedPQ&& other) noexcept {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(std::move(other));
            data = std::move(other.data);
        }
        return *this;
    }


    virtual ~SortedPQ() = default;


    /// @brief Find the correct position to insert while maintaining sorted order. 
    /// @param value 
    void push(const T &value) override 
    {
        // Use binary search to find insertion point, i.e., 
        // the first element that has higher priority than value
        auto pos = std::lower_bound( // FIFO behavior
            data.begin(), 
            data.end(), 
            value, 
            this->compareFunctor
        );

        data.insert(pos, value);
    } // push()


    /// @brief Remove the most extreme element from the priority queue.
    /// @note The most extreme element is at the back of the data container.
    void pop() override {
        if (!isEmpty()) {
            data.pop_back(); // Remove highest priority element from back
        }
    } // pop()


    /// @brief Get the most extreme element from the priority queue.
    /// @note The most extreme element is at the back of the data container.
    const T &getTop() const override {
        if (isEmpty())
            throw std::runtime_error("Priority queue is empty");
        return data.back(); // Highest priority element is at the back
    } // getTop()


    /// @brief Get the number of elements in the priority queue.
    std::size_t getSize() const override { 
        return data.size(); 
    } // getSize()


    /// @brief Check if the priority queue is empty.
    bool isEmpty() const override { 
        return data.empty(); 
    } // isEmpty()


    /// @brief Update the priority queue to maintain sorted order.
    void updatePQ() override 
    {
        if (getSize() <= 1) return;
        sort(
            data.begin(), 
            data.end(), 
            this->compareFunctor
        );
    } // updatePQ()


    /// @brief Get the element at the given index.
    /// @param index The index of the element to get.
    /// @note The index is 0-based.
    // Additional method for testing: get element at index (for verification)
    const T& at(std::size_t index) const {
        if (index >= getSize())
            throw std::out_of_range("Index out of range");
        return data[index];
    } // at()

private:
    
    // Priority queue's underlying container
    std::vector<T> data;
}; // SortedPQ

#endif // SORTED_PQ_H



