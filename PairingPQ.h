#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>
#include <iostream>

using namespace std;

// A specialized version of the 'priority queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR>
{
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;
    
public:
    
    // Each node within the pairing heap
    class Node
    {

    public:
        
        // Node ctor
        explicit Node(const TYPE &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
        {}
        
        // Description: Allows access to the element at that Node's position.
        // There are two versions, getElt() and a dereference operator, use
        // whichever one seems more natural to you.
        // Runtime: O(1).
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }
        
        // To access any private data members of this Node class 
        // from within the PairingPQ class. (ie: myNode.elt is a legal
        // statement in PairingPQ's add_node() function).
        friend PairingPQ;
        

    private:

        TYPE elt;
        Node *child;
        Node *sibling;
        Node *parent;
        
    }; // class Node
    
    
    // Description: Construct an empty pairing heap with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {} // PairingPQ()
    
    
    // Description: Construct a pairing heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end,
              COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {
        while (start != end)
        {
            push(*start);
            start++;
        }
        
    } // PairingPQ()
    
    
    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
    BaseClass{ other.compare }, root{ nullptr }, numNodes{ 0 }
    {
        // make a deque and insert root to other
        std::deque<Node*> dq;
        dq.push_back(other.root);
        
        while (!dq.empty())
        {
            // get the next element
            Node *ptr = traversalHelper(dq);
            
            // I now have the current node's child and sibling in the deck
            // waiting to get pushed. I push the current node first.
            push(ptr->elt);
        }
        
    } // PairingPQ()
    
    
    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs)
    {
        // copy-swap method (idiot check not needed from now on)
        
        // use copy ctor to create a copy of the refrenced object on rhs
        PairingPQ temp(rhs);
        
        // begin swapping the current objects (lhs) members with the copy's members
        std::swap(temp.numNodes, numNodes);
        std::swap(temp.root, root);
        
        return *this;
        
    } // operator=()




    // Refactored: A helper function that abstracts node processing.
    Node *traversalHelper(std::deque<Node*> &dq)
    {
        // Get the starting point and pop it.
        Node *ptr = dq.front();
        dq.pop_front();
            
        // Add the child and sibling if their not null.
        if (ptr->sibling)
            dq.push_back(ptr->sibling);
        if (ptr->child)
            dq.push_back(ptr->child);

        return ptr;
    } // traversalHelper()




    // Description: Destructor
    // Runtime: O(n)
    // OG Refactored: Abstracted any code duplication
    ~PairingPQ()
    {
        // make a deque and insert root
        std::deque<Node*> dq;
        
        if (!root)
            return;
        
        dq.push_back(root);
        
        while (!dq.empty())
        {
            // get the next element
            Node *ptr = traversalHelper(dq);
            
            // delete the current node
            delete ptr;
        }
        
        numNodes = 0;
        
    } // ~PairingPQ()




    
    // Description: Assumes that all elements inside the pairing heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    // OG Refactored: abstracted any code duplication
    virtual void updatePriorities()
    {
        // create an aux deck
        std::deque<Node*> dq;
        dq.push_back(root->child);
        root->child = nullptr;
        
        // while not empty we take something out
        while (!dq.empty())
        {
            // get a starting point
            Node *ptr = traversalHelper(dq);
            
            // severe the relationship
            ptr->parent = nullptr;
            //ptr->child = nullptr;
            ptr->sibling = nullptr;
            
            // now were ready to meld
            root = meld(ptr, root);
            
        } // while()
        
    } // updatePriorities()


    
    // Description: Add a new element to the pairing heap. This is already done.
    //              Push functionality is implemented entirely in the addNode()
    //              function, which this function calls.
    // Runtime: O(1)
    virtual void push(const TYPE &val)
    {
        addNode(val);
    } // push()
    
    
    // Description: Remove the most extreme (defined by 'compare') element from
    //              the pairing heap.
    // Note: Do not run tests on your code that would require it to pop an
    // element when the pairing heap is empty. Unless you are familiar with 
    // them, of course.
    // Runtime: Amortized O(log(n))
    virtual void pop()
    {
        // placeholder
        Node *temp = root->child;
        
        // set child to null as well
        root->child = nullptr;
        
        // pop the root
        delete root;
        numNodes--;
        
        // two cases: empty and not empty
        if (!temp)
        {
            // update the root
            root = nullptr;
        }
        else if (size() == 1)
        {
            temp->parent = nullptr;
            root = temp;
        }
        // not empty case
        else
        {
            // create a deque of pointers
            std::deque<Node*> dq;
        
            // fill it with the remaining heap
            while (temp)
            {
                dq.push_back(temp);
                temp = temp->sibling;
            }
            
            // we now perform a multi-pass on the dq
            while (dq.size() != 1)
            {
                size_t index = 0;
                
                // break the sibling parent relation
                dq[index]->parent = nullptr;
                dq[index]->sibling = nullptr;
                dq[index + 1]->parent = nullptr;
                dq[index + 1]->sibling = nullptr;
                
                // meld two elements and push them to back
               dq.push_back(meld(dq[index], dq[index + 1]));
                    
                // pop the ones that were just melded
                dq.pop_front();
                dq.pop_front();
            }
            
            // heap restored
            root = dq.front();
            dq.pop_front();
        }
        
    } // pop()
    
    
    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const
    {
        return root->elt;
        
    } // top()
    
    
    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const
    {
        return numNodes;
        
    } // size()
    
    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const
    {
        // if root = null, empty
        return !root;
        
    } // empty()
    
    
    // Description: Updates the priority of an element already in the pairing heap by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // NEED TO TEST THIS!!
    void updateElt(Node* node, const TYPE &new_value)
    {
        // check for precondition
        if (this->compare(node->elt, new_value))
        {
            // update elt
            node->elt = new_value;
            
            // three main cases:
            // 1. node = root,
            // 2. node != root & new_val < parent->elt (no work needed)
            // 3. node != root & new_val > parent->elt (max work: two sub cases)
            // 3a. node = leftmost or rightmost (sever 2 links)
            // 3b. node != leftmost (severe 3 links)
            
            // check for parent, roots dont have parents
            if (node->parent)
            {
                // if the val is less than my parent, then no work needed
                
                // if val is greater, then need to correct the heap
                if (this->compare(node->parent->elt, new_value))
                {
                    // store the parent before i sever link
                    Node *temp1 = node->parent;
                    
                    // check if I am leftmost
                    if (node->parent->child == node)
                    {
                        // give the old root a new child, which is leftmost's sibling
                        node->parent->child = node->sibling;
                    }
                    // check for prev, sibling, and parent, then sever 3 links
                    else
                    {
                        // start at leftmost
                        Node *temp2 = node->parent->child;
                        
                        // traverse until one before node
                        while (temp2->sibling != node)
                        {
                            temp2 = temp2->sibling;
                        }
                        
                        // extend the link past me
                        temp2->sibling = node->sibling;
                    }
                    
                    // sever sibling and parent links
                    node->parent = nullptr;
                    node->sibling = nullptr;
                    
                    root = meld(root, node);
                    
                }
            }
        }
        
    } // updateElt()
    
    
    // Description: Add a new element to the pairing heap. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val)
    {
        // use the new keyword here to create the new node
        Node *newNode = new Node{ val };
        
        // we have two cases: empty and size > 0
        
        
        if (empty()) // give it to root
            root = newNode;
        else        // meld it
            root = meld(newNode, root);
        
        numNodes++;
        
        return root;
        
    } // addNode()
    
    
private:
    
    // Links two non null heaps: root a and root b (b is always the curr root)
    // these roots must not have a parent or siblings.
    Node* meld(Node *a, Node *b)
    {
        // get extreme
        if (this->compare(b->elt, a->elt))
        {
            // give b a new parent and sibling
            b->parent = a;
            b->sibling = a->child;
            
            // give a a new child
            a->child = b;
            
            // return the larger one
            return a;
        }
        
        // give a a new parent and sibling
        a->parent = b;
        a->sibling = b->child;
        
        // give b a new child
        b->child = a;
        
        // return the larger one
        return b;
        
    } // meld()
    
    // root of heap and size
    Node *root;
    size_t numNodes;
    
};

#endif // PAIRINGPQ_H



















