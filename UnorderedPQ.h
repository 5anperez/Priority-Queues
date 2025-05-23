
// #ifndef UNORDEREDPQ_H
// #define UNORDEREDPQ_H

// #include "Eecs281PQ.h"


// // A specialized version of the 'heap' ADT that is implemented with an
// // underlying unordered array-based container that is linearly searched
// // for the most extreme element every time it is needed.

// // TODO: Read and understand this priority queue implementation!
// // Pay particular attention to how the constructors and findExtreme()
// // are written, especially the use of this->compare.

// template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
// class UnorderedPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
// {

//     // This is a way to refer to the base class object.
//     using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

// public:
    
//     // Default ctor (COMP_FUNCTOR() provides a default argument by creating a default-constructed instance of the functor)
//     // Description: Construct an empty heap with an optional comparison functor.
//     // Runtime: O(1)
//     explicit UnorderedPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
//         BaseClass{ comp } // call the base class ctor
//     {} // UnorderedPQ()



//     // Range-based ctor
//     // Description: Construct a heap out of an iterator range with an optional
//     //              comparison functor.
//     // Runtime: O(n) where n is number of elements in range.
//     template<typename InputIterator>
//     UnorderedPQ(InputIterator start, InputIterator end,
//                 COMP_FUNCTOR comp = COMP_FUNCTOR()) : // WHAT IS THIS LINE DOING?
//         BaseClass{ comp }, data{ start, end }
//     {} // UnorderedPQ()



//     // Description: Destructor doesn't need any code, the data vector will
//     //              be destroyed automatically.
//     virtual ~UnorderedPQ()
//     {} // ~UnorderedPQ()


//     // Description: Does nothing for this implementation, as items can never be
//     //              'out of order'.
//     // Runtime: O(1)
//     virtual void updatePriorities()
//     {} // updatePriorities()


//     // Description: Add a new element to the heap.
//     // Runtime: Amortized O(1)
//     virtual void push(const TYPE &val)
//     {
//         data.push_back(val);
        
//         // if this was an ordered pq we would call fixUp(data, data.size()) here to
//         // to maintain the sorting invariants, which would be O(log n).

//         // NEW APPROACH USING THE HOMEMADE CACHE FOR THE EXTREME'S INDEX
//         // The most extreme elt might change.
//         invalidateCache();
        
//     } // push()


//     // Description: Remove the most extreme (defined by 'compare') element from
//     //              the heap.
//     // Note: We will not run tests on your code that would require it to pop an
//     // element when the heap is empty. Though you are welcome to if you are
//     // familiar with them, you do not need to use exceptions in this project.
//     // Runtime: O(n)
//     // Note: If the most extreme element is already known (as would happen if
//     //       .top() was called before .pop()), this function is O(1).
//     virtual void pop()
//     {
//         // Replace (overwrite) the most extreme element with the element at the back,
//         // then pop_back().  This is much faster than erasing from the middle of a
//         // vector.
//         // data[findExtreme()] = data.back();
//         // data.pop_back();
        
//         // if this was an ordered pq, we would call fixDown(data, data.size(), 1)
//         // here to maintain the sorting invariants, which would be O(log n).

//         // NEW APPROACH USING THE HOMEMADE CACHE FOR THE EXTREME'S INDEX
//         // Use cahced value if available.
//         if (data.size() > 1)
//             data[findExtreme()] = data.back();
            
//         data.pop_back();

//         // Data changed, so invalidate the cache.
//         invalidateCache();
        
//     } // pop()


//     // Description: Return the most extreme (defined by 'compare') element of
//     //              the vector.  This should be a reference for speed.  It MUST
//     //              be const because we cannot allow it to be modified, as that
//     //              might make it no longer be the most extreme element.
//     // Runtime: O(n)
//     virtual const TYPE &top() const
//     {
//         if (cachedExtremeIdx == size())
//             // Find the most extreme element and return it by const reference.
//             return data[findExtreme()];
//         else
//             return data[cachedExtremeIdx];
//     } // top()


//     // Description: Get the number of elements in the heap.
//     // Runtime: O(1)
//     virtual std::size_t size() const
//     {
//         return data.size();
//     } // size()

//     // Description: Return true if the heap is empty.
//     // Runtime: O(1)
//     virtual bool empty() const
//     {
//         return data.empty();
//     } // empty()


// private:
    
//     // Note: This vector *must* be used for your heap implementation.
//     std::vector<TYPE> data;

//     // Define a cache to store the most extreme index to reduce redundant 
//     // linear searches. Its mutable to allow const function modifications.
//     // I couldve also made a struct with this and a truth val for checks.
//     mutable size_t cachedExtremeIdx = 0;
    
//     // Description: Find the 'most extreme' element of the data vector, using
//     //              this->compare() to check if one element is 'less than'
//     //              another.
//     // Checks whether the cache is valid before performing the linear search. 
//     //If the cache is invalid, it computes the extreme index, updates the 
//     // cache, and returns the value.
//     // Runtime: O(n)
//     size_t findExtreme() const
//     {
//         if (cachedExtremeIdx >= data.size())
//         {
//             size_t index = 0;

//             for (size_t i = 1; i < data.size(); ++i)
//                 // Is data[index] < data[i]?
//                 if (this->compare(data[index], data[i]))
//                     index = i;

//             cachedExtremeIdx = index;
//         }
        
//         return cachedExtremeIdx;
//     } // findExtreme()

//     // Every time an element is added or removed, the index of the most 
//     // extreme element could change, so the cache must be invalidated.
//     // Hence the call within the pop() and push() functions.
//     void invalidateCache()
//     {
//         // Represents an invalidate state
//         // where the cache neds recalculation.
//         cachedExtremeIdx = size();
//     }
// }; // UnorderedPQ

// #endif // UNORDEREDPQ_H












// OG UNORDERED PQ. THE ABOVE IMPLEMENTS CACHE TO STORE THE INDEX OF THE MOST EXTREME 
// ELEMENT, WHICH ESSENTIALLLY MAKES IT THE SAME AS UNORDEREDFASTPQ.


#ifndef UNORDEREDPQ_H
#define UNORDEREDPQ_H

#include "Eecs281PQ.h"


// A specialized version of the 'heap' ADT that is implemented with an
// underlying unordered array-based container that is linearly searched
// for the most extreme element every time it is needed.

// TODO: Read and understand this priority queue implementation!
// Pay particular attention to how the constructors and findExtreme()
// are written, especially the use of this->compare.

template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class UnorderedPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    
    // default ctor
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit UnorderedPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }
    {} // UnorderedPQ()


    // range-based ctor
    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    UnorderedPQ(InputIterator start, InputIterator end,
                COMP_FUNCTOR comp = COMP_FUNCTOR()) : 
        BaseClass{ comp }, data{ start, end }
    {} // UnorderedPQ()


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~UnorderedPQ()
    {} // ~UnorderedPQ()


    // Description: Does nothing for this implementation, as items can never be
    //              'out of order'.
    // Runtime: O(1)
    virtual void updatePriorities()
    {} // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: Amortized O(1)
    virtual void push(const TYPE &val)
    {
        data.push_back(val);
        
        // if this was an ordered pq we would call fixUp(data, data.size()) here to
        // to maintain the sorting invariants, which would be O(log n).
        
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(n)
    // Note: If the most extreme element is already known (as would happen if
    //       .top() was called before .pop()), this function is O(1).
    virtual void pop()
    {
        // Replace (overwrite) the most extreme element with the element at the back,
        // then pop_back().  This is much faster than erasing from the middle of a
        // vector.
        data[findExtreme()] = data.back();
        data.pop_back();
        
        // if this was an ordered pq, we would call fixDown(data, data.size(), 1)
        // here to maintain the sorting invariants, which would be O(log n).
        
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the vector.  This should be a reference for speed.  It MUST
    //              be const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(n)
    virtual const TYPE &top() const
    {
        // Find the most extreme element and return it by const reference.
        return data[findExtreme()];
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

private:
    
    // Description: Find the 'most extreme' element of the data vector, using
    //              this->compare() to check if one element is 'less than'
    //              another.
    // Runtime: O(n)
    size_t findExtreme() const
    {
        size_t index = 0;

        for (size_t i = 1; i < data.size(); ++i)
            if (this->compare(data[index], data[i]))
                index = i;

        return index;
    } // findExtreme()
}; // UnorderedPQ

#endif // UNORDEREDPQ_H






