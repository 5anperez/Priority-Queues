// #ifndef UNORDERED_PQ_H
// #define UNORDERED_PQ_H

// #include "SPsPQ.h"  // Base class header

// template<typename T, typename Compare = std::less<T>>
// class UnorderedPQ : public SPsPQ<T, Compare> 
// {

// public:

//     // Default constructor
//     explicit UnorderedPQ(const Compare& comp = Compare())
//         : SPsPQ<T, Compare>(comp), data(nullptr), capacity(0), size(0) {}


//     // Range-based constructor
//     template<typename Iterator>
//     UnorderedPQ(Iterator begin, Iterator end, const Compare& comp = Compare())
//         : SPsPQ<T, Compare>(comp), data(nullptr), capacity(0), size(0) 
//     {
//         // Calculate the size of the range
//         std::size_t rangeSize = 0;
//         for (Iterator it = begin; it != end; ++it) {
//             ++rangeSize;
//         }

//         // Allocate and copy
//         if (rangeSize > 0) {
//             capacity = rangeSize;
//             data = new T[capacity];
//             size = rangeSize;
//             std::size_t i = 0;
//             for (Iterator it = begin; it != end; ++it) {
//                 data[i++] = *it;
//             }
//         }
//     }



//     // Destructor
//     ~UnorderedPQ() override {
//         delete[] data;
//     }



//     // Push a new value into the queue
//     void push(const T& value) override {
//         if (size == capacity) {
//             resize();
//         }
//         data[size++] = value;
//     }



//     // Remove the top element
//     void pop() override {
//         if (isEmpty()) return;
//         std::size_t topIdx = findTopIndex();
//         // Move the last element to the top position and decrease size
//         data[topIdx] = data[--size];
//     }



//     // Get the top element (highest priority)
//     const T& getTop() const override {
//         if (isEmpty()) {
//             throw std::runtime_error("Priority queue is empty");
//         }
//         return data[findTopIndex()];
//     }



//     // Get the current size
//     std::size_t getSize() const override {
//         return size;
//     }



//     // Check if the queue is empty
//     bool isEmpty() const override {
//         return size == 0;
//     }

//     // Update function (for unordered, this is a no-op since we search on demand)
//     void updatePQ() override {
//         // No need to update in an unordered PQ; top is found dynamically
//     }



// private:

//     T* data;                // Dynamic array to store elements
//     std::size_t capacity;   // Total allocated size
//     std::size_t size;       // Current number of elements


//     // Helper to resize the internal array when full
//     void resize() 
//     {
//         std::size_t newCapacity = capacity == 0 ? 1 : capacity * 2;
//         T* newData = new T[newCapacity];
//         for (std::size_t i = 0; i < size; ++i) {
//             newData[i] = data[i];
//         }
//         delete[] data;
//         data = newData;
//         capacity = newCapacity;
//     }


//     // Helper to find the index of the "top" element based on Compare
//     std::size_t findTopIndex() const 
//     {
//         if (size == 0) return 0;  // Shouldn't happen due to isEmpty check in getTop/pop
//         std::size_t topIdx = 0;
//         for (std::size_t i = 1; i < size; ++i) {
//             if (this->compareFunctor(data[topIdx], data[i])) {
//                 topIdx = i;
//             }
//         }
//         return topIdx;
//     }
// };

// #endif // UNORDERED_PQ_H
















































#ifndef UNORDERED_PQ_VECTOR_H
#define UNORDERED_PQ_VECTOR_H

#include "SPsPQ.h"  // Base class header
#include <vector>

template<typename T, typename Compare = std::less<T>>
class UnorderedPQ : public SPsPQ<T, Compare> 
{

private:

    std::vector<T> data;  // Vector to store elements


    // Helper to find the index of the "top" element based on Compare
    std::size_t findTopIndex() const 
    {
        // Handled by isEmpty check in getTop/pop
        if (data.empty()) return 0;  

        std::size_t topIdx = 0;
        for (std::size_t i = 1; i < data.size(); ++i) 
        {
            if (this->compareFunctor(data[topIdx], data[i])) {
                topIdx = i;
            }
        }
        return topIdx;
    }



public:

    // Default constructor
    explicit UnorderedPQ(const Compare& comp = Compare())
        : SPsPQ<T, Compare>(comp) {}


    // Range-based constructor
    template<typename Iterator>
    UnorderedPQ(Iterator begin, Iterator end, const Compare& comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end) {}


    // Push a new value into the queue
    void push(const T& value) override {
        data.push_back(value);
    }


    // Remove the top element
    void pop() override {
        if (isEmpty()) return;
        std::size_t topIdx = findTopIndex();
        data[topIdx] = data.back();  // Replace top with last element
        data.pop_back();             // Remove last element
    }


    // Get the top element (highest priority)
    const T& getTop() const override {
        if (isEmpty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return data[findTopIndex()];
    }


    // Get the current size
    std::size_t getSize() const override {
        return data.size();
    }


    // Check if the queue is empty
    bool isEmpty() const override {
        return data.empty();
    }


    // Update function (no-op for unordered PQ)
    void updatePQ() override {
        // No need to update; top is found dynamically
    }
};


#endif // UNORDERED_PQ_VECTOR_H

















































































