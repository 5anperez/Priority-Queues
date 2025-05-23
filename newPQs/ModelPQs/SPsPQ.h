// SPsPQ.h
//
// Base class for all special-purpose priority queues.
//
//


#ifndef SPSPQ_H
#define SPSPQ_H

#include <functional>  


template<typename T, typename Compare = std::less<T>>
class SPsPQ 
{
public: // INTERFACE
    
    // DTOR
    virtual ~SPsPQ() = default;

    // OPERATIONS
    virtual void push(const T& value) = 0;         
    virtual void pop() = 0;                        
    virtual const T& getTop() const = 0;                              
    virtual std::size_t getSize() const = 0;          
    virtual bool isEmpty() const = 0;
    virtual void updatePQ() = 0;             

protected: // MEMBERS
    
    // CTOR
    explicit SPsPQ(const Compare& comp = Compare())
        : compareFunctor{ comp } {}


    Compare compareFunctor;
}; // class SPsPQ

#endif // SPSPQ_H



















