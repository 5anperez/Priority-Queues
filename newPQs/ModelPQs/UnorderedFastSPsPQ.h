#ifndef UNORDERED_PQ_OPTIMIZED_H
#define UNORDERED_PQ_OPTIMIZED_H

#include "SPsPQ.h"
#include <vector>

template<typename T, typename Compare = std::less<T>>
class UnorderedPQOptimized : public SPsPQ<T, Compare> 
{

public:


    // Default constructor
    explicit UnorderedPQOptimized(const Compare& comp = Compare())
        : SPsPQ<T, Compare>(comp), topIdx(0) {}



    // Range-based constructor
    template<typename Iterator>
    UnorderedPQOptimized(Iterator begin, Iterator end, 
        const Compare& comp = Compare())
    : SPsPQ<T, Compare>(comp), data(begin, end), topIdx(0) {
        if (!data.empty()) updateTop();
    }



    // Push a new value into the queue
    void push(const T& value) override {
        data.push_back(value);
        // If empty before push or new value is better than current top, update topIdx
        if (data.size() == 1 || this->compareFunctor(data[topIdx], value)) {
            topIdx = data.size() - 1;
        }
    }



    // Remove the top element
    void pop() override {
        if (isEmpty()) return;

        // Swap top element with last element and remove last
        if (data.size() > 1 && topIdx != data.size() - 1) {
            data[topIdx] = data.back();
        }
        data.pop_back();

        // If not empty, recompute top since current top was removed
        if (!data.empty()) {
            updateTop();
        }
    }



    // Get the top element (highest priority)
    const T& getTop() const override {
        if (isEmpty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return data[topIdx];
    }



    // Get the current size
    std::size_t getSize() const override {
        return data.size();
    }

    // Check if the queue is empty
    bool isEmpty() const override {
        return data.empty();
    }

    // Update function (explicitly recompute top if needed)
    void updatePQ() override {
        updateTop();
    }

private:
    std::vector<T> data;      // Vector to store elements
    std::size_t topIdx;       // Index of current top element

    // Helper to find and update the top index
    void updateTop() 
    {
        if (data.empty()) {
            return;
        }

        topIdx = 0;
        for (std::size_t i = 1; i < data.size(); ++i) {
            if (this->compareFunctor(data[topIdx], data[i])) {
                topIdx = i;
            }
        }
    }
};

#endif // UNORDERED_PQ_OPTIMIZED_H



















