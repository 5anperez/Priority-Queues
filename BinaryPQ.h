//  BinaryPQ.h
//  p2b-priority-queues
//
//  Created by Santiago S. Perez on 2/23/22.
//

/*

    Refer to the heaps lecture slides to find more useful theory information.

    ** However, note that the heap sort portion is irrelevant. **

    Here we are simply using the low-level binary heap to create the abstract binary PQ.

*/

#ifndef BINARYPQ_H
#define BINARYPQ_H

#include <algorithm>
#include <utility>
#include "Eecs281PQ.h"

// A specialized version of the 'heap' ADT
// (abstract data type) implemented as a binary heap.
template <typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) : BaseClass{comp}
    {
    } // BinaryPQ

    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template <typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end,
             COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass{comp}, data{start, end}
    {
        updatePriorities();
    } // BinaryPQ

    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ()
    {
    } // ~BinaryPQ()

    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant, i.e., the heapify approach.
    // Runtime: O(n)
    virtual void updatePriorities()
    {
        // Heapify: proceed from bottom, repeatidly calling fixDown()
        // no need calling it on leaves. ∆ Tree height = size() / 2
        for (size_t i = (size() / 2); i >= 1; i--)
            fixDown(i);
    } // updatePriorities()

    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    virtual void push(const TYPE &val)
    {
        // insert val at the back
        data.push_back(val);

        // call fixUp() on the last element
        fixUp(data.size());
    } // push()

    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: Do not run tests on your code that would require it to pop an
    // element when the heap is empty. Unless you are familiar with them.
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
    //              the heap. This should be a reference for speed. It MUST be
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
    // Under the hood data structure.
    std::vector<TYPE> data;

    /// NOTE: This is from lecture slides 08, pg. 19
    // Description: fixes tree if a priority has increased.
    // Runtime: O(log n)
    void fixUp(size_t index)
    {
        // While my parent's priority is less than mine and
        // we are not at the root, simulate bubble up of the
        // increased priority element via swaps.
        size_t child = index,
               parent = (child / 2); // tree ∆ structure math
        while ((child != 1) &&       // root = 1
               (this->compare(getElement(parent), getElement(child))))
        {
            std::swap(getElement(child), getElement(parent));

            // Move up to parent.
            child = parent;
            parent = (child / 2);
        } // while
    } // fixUp()

    /// NOTE: This is from lecture slides 08, pg. 22
    // Description: fixes tree if a priority has decreased.
    // Runtime: O(log n)
    void fixDown(size_t index)
    {
        // Traverse the tree height.
        size_t heapSize = size();
        while ((index * 2) <= heapSize)
        {
            // Start comparisons on the left child
            size_t j = index * 2;

            // If still in the heaps range i.e. there is a right child, then
            // compare with right child and switch if right is higher priority
            if (j < heapSize && this->compare(getElement(j), getElement(j + 1)))
                j++;

            // If the larger child (j) is less than or equal (≤) to the
            // parent (index), leave because the heap is restored.
            // if (this->compare(getElement(j), getElement(index)) ||      // <
            //     (!this->compare(getElement(index), getElement(j)) &&    // =
            //     !this->compare(getElement(j), getElement(index))))
            //     break;

            // This is logically equivalent to the above.
            // Proof: [a ∨ (¬b ∧ ¬a)] ≡ [a ∨ ¬b] Q.E.D.
            // if (this->compare(getElement(j), getElement(index)) ||
            //     (!this->compare(getElement(index), getElement(j))))
            //     break;

            // Then, if we reduce the logic eq again, we get: [a ∨ ¬b] ≡ ¬b.
            // If j is not greater than index, then we break.
            if (!this->compare(getElement(index), getElement(j)))
                break;

            // O/W, swap the larger child and parent, then move down to
            // check if our node "index" is smaller than anyone else.
            std::swap(getElement(j), getElement(j / 2));
            index = j;
        } // while
    } // fixDown()

    // Translates base-zero indexing to base-one.
    TYPE &getElement(std::size_t i)
    {
        return data[i - 1];
    } // getElement()

    // Translates base-zero indexing to base-one.
    const TYPE &getElement(std::size_t i) const
    {
        return data[i - 1];
    } // getElement()

}; // BinaryPQ

#endif // BINARYPQ_H
