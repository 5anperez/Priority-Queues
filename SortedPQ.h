#ifndef SORTEDPQ_H
#define SORTEDPQ_H

#include "Eecs281PQ.h"
#include <algorithm>
#include <iostream>

// A specialized version of the 'heap' ADT that is implemented with an
// underlying sorted array-based container.

// Note: The most extreme element should be found at the end of the
// 'data' container, such that traversing the iterators yields the elements in
// sorted order.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class SortedPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit SortedPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) : 
    BaseClass{ comp }
        {} // SortedPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n log n) where n is number of elements in range.
    template<typename InputIterator>
    SortedPQ(InputIterator start, InputIterator end, 
            COMP_FUNCTOR comp = COMP_FUNCTOR()) : 
            BaseClass{ comp }, data{ start, end }
    { updatePriorities(); } // SortedPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~SortedPQ()
    {} // ~SortedPQ()


    // Description: Add a new element to the heap.
    // Runtime: O(n)
    /*
        lower_bound returns an iterator pointing to the first element in the range [first, last) that is not less than (i.e., greater or equal to) the value specified. If all elements in the range are less than the specified value, the function returns an iterator to the end of the range.
    */
    virtual void push(const TYPE &val)
    {
        // Find the last place val should go
        auto index = std::lower_bound(
            data.begin(), 
            data.end(), 
            val, 
            this->compare
        );
        
        // insert val at its lower bound
        data.insert(index, val);        
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(1)
    virtual void pop()
    {
        data.pop_back();
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the vector.  This should be a reference for speed.  It MUST
    //              be const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const
    {
        return data.back();
    } // top()


    // Description: Get the number of elements in the heap.
    //              This has been implemented for you.
    // Runtime: O(1)
    virtual std::size_t size() const
    {
        return data.size();
    } // size()


    // Description: Return true if the heap is empty.
    //              This has been implemented for you.
    // Runtime: O(1)
    virtual bool empty() const
    {
        return data.empty();
    } // empty()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n log n)
    virtual void updatePriorities()
    {
        // sort the vector
        sort(data.begin(), data.end(), this->compare);
    } // updatePriorities()


private:
    // priority queue's underlying container
    std::vector<TYPE> data;
}; // SortedPQ

#endif // SORTEDPQ_H
