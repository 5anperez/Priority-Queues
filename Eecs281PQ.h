/*

    The base class from which all subsequent priority queue classes are derived from. E.g., SortedPQ is a templated class which inherits from this base templated class. This creates a dependent context, therefore, we must always specify this->compare, not just compare.


    Think of the ADT as the interface, which is from the user's perspective, and the data structure as the implementation of that interface, which is from the programmer's perspective.


    The unsorted priority queue is an ADT and the vector is the data structure, i.e., an implementation of it.
    
    The sorted priority queue is an ADT and the sorted vector is the data structure, i.e., an implementation of it.
    
    The priority queue is an ADT and the binary heap is the data structure, i.e., an implementation of it.

    The priority queue is an ADT and the pairing heap is the data structure, i.e., an implementation of it.

*/



#ifndef EECS281_PQ_H
#define EECS281_PQ_H

#include <functional>
#include <iterator>
#include <vector>

// A simple interface that implements a generic priority queue.
// Runtime specifications assume constant time comparison and copying.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class Eecs281PQ
{
public:
    
    // DETRUCTOR
    virtual ~Eecs281PQ() {}
    
    // Description: Add a new element to the priority queue.
    virtual void push(const TYPE &val) = 0;
    
    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    virtual void pop() = 0;
    
    // Description: Return the most extreme (defined by 'compare') element of
    //              the priority queue.
    virtual const TYPE &top() const = 0;
    
    // Description: Get the number of elements in the priority queue.
    virtual std::size_t size() const = 0;
    
    // Description: Return true if the priority queue is empty.
    virtual bool empty() const = 0;
    
    // Description: Assumes that all elements in the priority queue are out of order.
    //              You must reorder the data so that the PQ invariant is restored.
    //              Each derived PQ will have to implement this appropriately.
    virtual void updatePriorities() = 0;
    
protected:
    
    // DEFAULT CTOR
    Eecs281PQ() {}
    
    // CTOR THAT ACCEPTS A COMPARISON FUNCTION (functor)
    explicit Eecs281PQ(const COMP_FUNCTOR &comp) : 
    compare{ comp } {} // initializer list
    
    // MEMBER VARIABLES
    // Note: These data members *must* be used in all of your priority queue
    //       implementations.
    
    // You can use this in the derived classes with:
    // this->compare(thing1, thing2)
    // With the default compare function (std::less), this will
    // tell you if Thing1 is lower priority than Thing2.
    COMP_FUNCTOR compare;
}; // Eecs281PQ


#endif
