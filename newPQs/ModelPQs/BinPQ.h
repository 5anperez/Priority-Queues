#ifndef BIN_PQ_H
#define BIN_PQ_H

#include "SPsPQ.h"
#include <vector>

/// @brief A binary heap implementation of a priority queue.
/// @tparam T: The type of the elements in the queue.
/// @tparam Compare: The comparison functor to use for the priority queue.
/// @note This implementation uses a vector to store the elements, and 
///       a heap property to maintain the order of the elements.
///       The heap property is maintained by the topDown() and bottomUp() 
///       functions.
template <typename T, typename Compare = std::less<T>>
class BinPQ : public SPsPQ<T, Compare>
{
    using BaseClass = SPsPQ<T, Compare>;

public:

    // Default constructor
    explicit BinPQ(const Compare &comp = Compare())
        : BaseClass(comp) 
    {}
    // DEFAULT CTOR


    // Range-based constructor
    template <typename Iterator>
    BinPQ(Iterator begin, Iterator end, 
        const Compare &comp = Compare())
            : BaseClass(comp), data(begin, end) 
    { updatePQ(); }
    // R-B CTOR


    // Copy constructor
    BinPQ(const BinPQ& other) 
        : BaseClass(other), 
        data(other.data) 
    {}
    // COPY CTOR


    // Move constructor
    BinPQ(BinPQ&& other) noexcept
        : BaseClass(
            std::move(other)),      // move the entire base class
        data(std::move(other.data)) // move the derived class's vector
    {}
    // MOVE CTOR


    // Copy assignment
    BinPQ& operator=(const BinPQ& other) 
    {
        if (this != &other) 
        {
            // Call the base class's assignment operator,
            // then assign the derived class's vector.
            BaseClass::operator=(other);
            data = other.data;
        }

        // Return the current BinPQ object.
        return *this;
    } // COPY ASSIGNMENT


    // Move assignment
    BinPQ& operator=(BinPQ&& other) noexcept 
    {
        if (this != &other) 
        {
            BaseClass::operator=(std::move(other));
            data = std::move(other.data);
        }

        return *this;
    } // MOVE ASSIGNMENT


    // Destructor
    virtual ~BinPQ() = default;


    /// @brief Update the priority queue to maintain the heap property.
    /// @note This function uses a post-decrement approach to overcome 
    ///       unsigned integer overflow and the use of static_cast. 
    ///       Read more about it in the notes!
    void updatePQ() override
    { 
        if (getSize() <= 1) return;

        // Start from last non-leaf node and sift down each node
        const size_t lastParentIdx = getParentIndex(getSize() - 1);
        for (size_t i = (lastParentIdx + 1); i-- > ROOT;)
            topDown(i); 
    } // updatePQ()



    /// @brief Push a new value into the queue, and call 
    ///        bottomUp() to maintain the heap property 
    ///        by bubbling up the new element.
    /// @param value: The value to push into the queue.
    void push(const T &value) override
    {
        data.push_back(value);
        bottomUp(getSize() - 1); 
    } // push()



    /// @brief Remove the top element, and call topDown() 
    ///        to maintain the heap property by sifting down 
    ///        the new root.
    void pop() override 
    {
        if (isEmpty()) return;
        data[ROOT] = std::move(data.back());
        data.pop_back();
        if (!isEmpty()) topDown(ROOT); 
    } // pop()



    /// @brief Get the top element (highest priority)
    /// @return The top element of the queue.
    const T &getTop() const override
    {
        if (isEmpty())
            throw std::runtime_error("Priority queue is empty");
        return data[ROOT]; // Root is always the top in a binary heap
    } // getTop()



    /// @brief Get the current size
    /// @return The current size of the queue.
    size_t getSize() const override { 
        return data.size(); 
    } // getSize()


    /// @brief Check if the queue is empty
    /// @return True if the queue is empty, false otherwise.
    bool isEmpty() const override { 
        return data.empty(); 
    } // isEmpty()


private:

    /// @brief Constants 
    static constexpr size_t ROOT = 0;
    static constexpr size_t NUM_CHILDREN = 2;
    
    /// @brief Heap container
    std::vector<T> data;


    /// @brief Helper to calculate parent index
    /// @param index: The index of the node to get the parent of.
    /// @return The index of the parent of the node at index.
    size_t getParentIndex(size_t index) const {
        return (index - 1) / NUM_CHILDREN;
    } // getParentIndex()


    /// @brief Helper to calculate left child index
    /// @param index: The index of the node to get the left child of.
    /// @return The index of the left child of the node at index.
    size_t getLeftChildIndex(size_t index) const {
        return NUM_CHILDREN * index + 1;
    } // getLeftChildIndex()


    /// @brief Helper to calculate right child index  
    /// @param index: The index of the node to get the right child of.
    /// @return The index of the right child of the node at index.
    size_t getRightChildIndex(size_t index) const {
        return NUM_CHILDREN * index + NUM_CHILDREN;
    } // getRightChildIndex()


    /// @brief Helper to check if compareFunctor says a < b 
    ///         (a has lower priority than b)
    /// @param a: The first element to compare.
    /// @param b: The second element to compare.
    /// @return True if a has lower priority than b, false otherwise.
    bool hasLowerPriority(const T& a, const T& b) const {
        return this->compareFunctor(a, b);
    } // hasLowerPriority()



    /// @brief Fix heap property top-down (fix down) when 
    ///         priority decreases at given index.
    /// @param index: The index of the node to sift down.
    void topDown(size_t index) 
    {
        const size_t size = getSize();
        while (true) 
        {
            size_t largest = index;
            const size_t left = getLeftChildIndex(index);
            const size_t right = getRightChildIndex(index);

            // Check if children exist, and if so, grab the larger one.
            // Then check if it is larger than the current largest, i.e., the parent.
            if (left < size && hasLowerPriority(data[largest], data[left])) 
                largest = left;
            
            if (right < size && hasLowerPriority(data[largest], data[right])) 
                largest = right;
            

            // If largest is not the current index, swap and continue sifting down
            if (largest != index) {
                std::swap(data[index], data[largest]);
                index = largest;
            } else {
                break; // Heap property restored
            }
        }
    } // topDown()



    /// @brief Fix heap property bottom-up (fix up) when 
    ///         priority increases at given index.
    /// @param index: The index of the node to sift up.
    void bottomUp(size_t index)
    {
        // While not at root, check if the parent is 
        // lower priority than the current node.
        while (index > ROOT)
        {
            const size_t parent = getParentIndex(index);
            if (hasLowerPriority(data[parent], data[index]))
            {
                std::swap(data[parent], data[index]);
                index = parent; // move up to the parent
            } else {
                break; // Heap property restored
            }
        }
    } // bottomUp()


}; // class BinPQ

#endif // BIN_PQ_H














































