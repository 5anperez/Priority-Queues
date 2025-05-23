#ifndef BIN_PQ_H
#define BIN_PQ_H

#include "SPsPQ.h"
#include <vector>
#include <stdexcept>
#include <algorithm>

template <typename T, typename Compare = std::less<T>>
class BinPQ : public SPsPQ<T, Compare>
{

private:

    static constexpr std::size_t ROOT = 0;
    static constexpr std::size_t NUM_CHILDREN = 2;
    
    std::vector<T> data;

    // Helper to calculate parent index
    std::size_t getParentIndex(std::size_t index) const {
        return (index - 1) / NUM_CHILDREN;
    }

    // Helper to calculate left child index
    std::size_t getLeftChildIndex(std::size_t index) const {
        return NUM_CHILDREN * index + 1;
    }

    // Helper to calculate right child index  
    std::size_t getRightChildIndex(std::size_t index) const {
        return NUM_CHILDREN * index + NUM_CHILDREN;
    }

    void topDown(std::size_t index) 
    {
        while (true) 
        {
            std::size_t largest = index;
            const std::size_t left = getLeftChildIndex(index);
            const std::size_t right = getRightChildIndex(index);

            if (left < getSize() && this->compareFunctor(data[largest], data[left]))
                largest = left;
            if (right < getSize() && this->compareFunctor(data[largest], data[right]))
                largest = right;

            if (largest != index) {
                std::swap(data[index], data[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

    void bottomUp(std::size_t index) {
        while (index > ROOT) {
            const std::size_t parent = getParentIndex(index);
            if (this->compareFunctor(data[parent], data[index])) {
                std::swap(data[parent], data[index]);
                index = parent;
            } else {
                break;
            }
        }
    }

public:
    // Default constructor
    explicit BinPQ(const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp) {}

    // Range-based constructor with move semantics
    template <typename Iterator>
    BinPQ(Iterator begin, Iterator end, const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end) { 
        updatePQ();
    }

    // Copy constructor
    BinPQ(const BinPQ& other) 
        : SPsPQ<T, Compare>(other), data(other.data) {}

    // Move constructor
    BinPQ(BinPQ&& other) noexcept
        : SPsPQ<T, Compare>(std::move(other)), data(std::move(other.data)) {}

    // Copy assignment
    BinPQ& operator=(const BinPQ& other) {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(other);
            data = other.data;
        }
        return *this;
    }

    // Move assignment
    BinPQ& operator=(BinPQ&& other) noexcept {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(std::move(other));
            data = std::move(other.data);
        }
        return *this;
    }

    virtual ~BinPQ() = default;

    void updatePQ() override { 
        if (getSize() <= 1) return;
        
        // Start from last non-leaf node and sift down
        std::size_t lastNonLeaf = getParentIndex(getSize() - 1);
        for (int i = static_cast<int>(lastNonLeaf); i >= static_cast<int>(ROOT); --i) {
            topDown(static_cast<std::size_t>(i)); 
        }
    }

    void push(const T &value) override {
        data.push_back(value);
        bottomUp(getSize() - 1); 
    }

    void pop() override 
    {
        if (isEmpty()) return;
        
        data[ROOT] = std::move(data.back());
        data.pop_back();
        if (!isEmpty()) topDown(ROOT);
    }

    const T &getTop() const override {
        if (isEmpty())
            throw std::runtime_error("Priority queue is empty");
        return data[ROOT];
    }

    std::size_t getSize() const override { 
        return data.size(); 
    } 

    bool isEmpty() const override { 
        return data.empty(); 
    } 
};

#endif // BIN_PQ_H




































