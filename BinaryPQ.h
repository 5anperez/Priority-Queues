// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include <utility>
#include "Eecs281PQ.h"

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }
    {
        // TODO: Implement this function.
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end,
             COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{ comp }, data{ start, end }
    {
        
        updatePriorities();
        
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ()
    {} // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities()
    {
        // heapify: proceed from bottom, repeatidly calling fixDown()
        // no need calling it on leaves
        for (size_t i = (size() / 2); i >= 1; i--)
        {
            fixDown(i);
        }
        
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE &val)
    {
        // insert val at the back
        data.push_back(val);
        
        // call fixUp() on the last element
        fixUp(data.size());
        
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop()
    {
        // overwrite the root with back
        data.front() = data.back();
        
        // pop the root
        data.pop_back();
        
        // call fixDown() with the roots index
        fixDown(1);
        
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const
    {
        return getElement(1);
    
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const
    {
        return data.size();
        
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const
    {
        return data.empty();
        
    } // empty()


private:
    // Note: This vector *must* be used for your heap implementation.
    std::vector<TYPE> data;
    // NOTE: You are not allowed to add any member variables.  You don't need
    //       a "heapSize", since you can call your own size() member function,
    //       or check data.size().

    // TODO: Add any additional member functions or data you require here.
    //       For instance, you might add fixUp() and fixDown().
    
    // Description: fixes tree if a priority has increased.
    // Runtime: O(log n)
    void fixUp(size_t index)
    {
        // keep looping and swapping while we are not at the root
        // and while my parents priority is less than mine
        while (index != 1 && this->compare(getElement(index / 2), getElement(index)))
        {
            std::swap(getElement(index), getElement(index / 2));
            
            // keep moving up
            index /= 2;
            
        } // while
    
    } // fixUp()
    
    // Description: fixes tree if a priority has decreased.
    // Runtime: O(log n)
    void fixDown(size_t index)
    {
        // keep looping while I am not at a leaf
        while ((index * 2) <= size())
        {
            // to compare children, start with the left child
            size_t j = index * 2;
            
            // if still in the heaps range i.e. there is a right child, then
            // compare with right child and switch if right is higher priority
            if (j < size() && this->compare(getElement(j), getElement(j + 1)))
                j++;
            
            // if the child (j) is less than or equal to the parents (index)
            // priority, then leave  because the heap is restored.
            if (this->compare(getElement(j), getElement(index)) ||
                (!this->compare(getElement(index), getElement(j)) &&
                 (!this->compare(getElement(j), getElement(index)))))
                break;
            
            // otherwise swap the child and parent
            std::swap(getElement(j), getElement(j / 2));
            
            // and move down
            index = j;
            
        } // while
        
    } // fixDown()
    
    
    // translates base-zero indexing to base-one
    TYPE &getElement(std::size_t i)
    {
        return data[i - 1];
        
    } // getElement()
    
    // translates base-zero indexing to base-one
    const TYPE &getElement(std::size_t i) const
    {
        return data[i - 1];
        
    } // getElement()
    
}; // BinaryPQ


#endif // BINARYPQ_H
