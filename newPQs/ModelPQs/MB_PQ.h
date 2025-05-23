#ifndef UNORDERED_PQ_OPTIMIZED_H
#define UNORDERED_PQ_OPTIMIZED_H

#include "SPsPQ.h"  // Base class header
#include <vector>

template<typename T, typename Compare = std::less<T>>
class UnorderedPQOptimized : public SPsPQ<T, Compare> 
{

public:


    // Default constructor
    explicit UnorderedPQOptimized(const Compare& comp = Compare())
        : SPsPQ<T, Compare>(comp), topIdx(0), topValid(false) {}



    // Range-based constructor
    template<typename Iterator>
    UnorderedPQOptimized(Iterator begin, Iterator end, const Compare& comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end), topIdx(0), topValid(false) {
        if (!data.empty()) updateTop();
    }



    // Push a new value into the queue
    void push(const T& value) override {
        data.push_back(value);
        if (!topValid) {
            updateTop();  // Recompute top if invalid
        } else if (this->compareFunctor(data[topIdx], value)) {
            topIdx = data.size() - 1;  // New element is better than current top
            topValid = true;
        }
    }



    // Remove the top element
    void pop() override {
        if (isEmpty()) return;

        if (!topValid) updateTop();

        data[topIdx] = data.back();
        data.pop_back();
        topValid = false;  // Force recheck next time
    }



    // Get the top element (highest priority)
    const T& getTop() const override {
        if (isEmpty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        if (!topValid) {
            const_cast<UnorderedPQOptimized*>(this)->updateTop();  // Lazy update
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

    // Update function (no-op unless forced)
    void updatePQ() override {
        updateTop();  // Explicitly update if needed
    }


private:

    std::vector<T> data;      // Vector to store elements
    std::size_t topIdx;       // Index of current top element
    bool topValid;            // Flag to indicate if topIdx is valid



    // Helper to find and update the top index
    void updateTop() 
    {
        if (data.empty()) 
        {
            topValid = false;
            return;
        }

        topIdx = 0;
        for (std::size_t i = 1; i < data.size(); ++i) 
        {
            if (this->compareFunctor(data[topIdx], data[i])) {
                topIdx = i;
            }
        }
        topValid = true;
    }
};

#endif // UNORDERED_PQ_OPTIMIZED_H




































