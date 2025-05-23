#ifndef BIN_PQ_H
#define BIN_PQ_H

#include "SPsPQ.h"
#include <vector>





/**
 * TODO: 
 * 
 * 1. 
 */










template <typename T, typename Compare = std::less<T>>
class BinPQ : public SPsPQ<T, Compare>
{

public:

    // Default constructor
    explicit BinPQ(const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp) {}


    // Range-based constructor
    template <typename Iterator>
    BinPQ(Iterator begin, Iterator end, 
            const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end)
    { buildHeap(); }


    // Destructor (virtual, inherited from base)
    virtual ~BinPQ() = default;


    // Push a new value into the queue
    void push(const T &value) override
    {
        data.push_back(value);
        // Sift up the new element 
        // to maintain heap property.
        bottomUp(data.size() - 1); 
    }



    // Remove the top element
    void pop() override 
    {
        if (isEmpty())
            return;

        if (data.size() == 1)
        {
            data.pop_back();
            return;
        }

        // Move last element to root and sift down
        data[0] = data.back();
        data.pop_back();
        // Sift down from root to maintain heap property
        topDown(0); 
    }



    // Get the top element (highest priority)
    const T &getTop() const override
    {
        if (isEmpty())
            throw std::runtime_error("Priority queue is empty");
        return data[0]; // Root is always the top in a binary heap
    }



    // Get the current size
    std::size_t getSize() const override
    { return data.size(); }


    // Check if the queue is empty
    bool isEmpty() const override
    { return data.empty(); }


    // Rebuild the heap when invariant is broken
    void updatePQ() override
    { buildHeap(); }


    // Fix heap property bottom-up (sift up) when priority increases at given index
    void bottomUp(std::size_t index)
    {
        if (index >= data.size())
            return;

        // While not at root and parent is worse than current 
        std::size_t parent = (index - 1) / 2;
        while (index > 0 && this->compareFunctor(data[parent], data[index]))
        {
            std::swap(data[parent], data[index]);
            index = parent;
            parent = (index - 1) / 2;
        }
    }



    // Fix heap property top-down (sift down) when priority decreases at given index
    void topDown(std::size_t index) 
    {
        if (index >= data.size()) return;

        std::size_t size = data.size();
        while (true) 
        {
            std::size_t largest = index;
            std::size_t left = 2 * index + 1;
            std::size_t right = 2 * index + 2;

            // Check if left child exists and is better than current largest
            if (left < size && this->compareFunctor(data[largest], data[left])) {
                largest = left;
            }
            // Check if right child exists and is better than current largest
            if (right < size && this->compareFunctor(data[largest], data[right])) {
                largest = right;
            }

            // If largest is not the current index, swap and continue sifting down
            if (largest != index) {
                std::swap(data[index], data[largest]);
                index = largest;
            } else {
                break; // Heap property restored
            }
        }
    }


private:

    std::vector<T> data; // Vector to store binary heap elements

    // Build heap from current data (used in range constructor and updatePQ)
    void buildHeap() 
    {
        if (data.empty()) return;

        // Start from last non-leaf node and sift down each node
        size_t lastNonLeaf = (data.size() / 2) - 1;
        for (size_t i = lastNonLeaf; i < data.size(); ++i) 
            topDown(i);
    }
};

#endif // BIN_PQ_H
































