#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "SPsPQ.h"
#include <deque>
#include <utility>
#include <iostream>

using namespace std;



/**
 * TODO: 
 * - ✅ GET YOUR UPDATE ELT WORKING. E.G., SEE IF YOU CAN GET YOUR VERSION DEBUGGED (ADDNODE TOO).
 * - ✅ SEE IF MA VERSION CAN BE MODIFIED TO USE PARENT PTR INSTEAD OF PREV PTR.
 * - CHECK OUT THE TRICKS WITHIN THE WHITE PAPERS.
 * - THERE HAS TO BE EDGE TESTS I AM MISSING.
 * - WHY DID PROF SAY ❌DONT❌ USE RECURSION?
 * 
 * 
 * 
 * NEEDED EDGE CASES:
 * - create a mulit-level heap by alternating greater than and less than value pushes, but do some on lower levels with addNode, then corrupt it with updateElt, and finally updatePQ to check for root correctness.
 */



/// @implements PARENT VERSION
/// @brief A pairing heap implementation of a priority queue.
/// @param T The type of elements stored in the heap/queue
/// @param Compare The comparison function (default: std::less<T> for max heap)
/// @note It uses raw pointers for memory efficiency.
/// @details The main difference between this and the PREV version is that
///          prev pointers make the updateElt function more efficient by
///          allowing very quick sibling-sibling link severing when the node
///          is not the leftmost child since all links must be cut before melding.
///          Note the loop within updateElt()'s 3rd case, which is not needed w/ prev ptrs.
///          HOWEVER, the prev pointers dont allow peeking at the parent's value,
///          therfore, we can do less work in some cases.
/// @runtime: This implementation provides O(1) insert, O(log n) increase-priority,
///           O(1) find-extreme-priority, and O(log n) amortized delete-extreme-priority.
template <typename T, typename Compare = std::less<T>>
class PairingPQ : public SPsPQ<T, Compare>
{
    // Use an alias for brevity.
    using BaseClass = SPsPQ<T, Compare>;
    
public:
    
    /// @brief Nodes make up the tree structure within the pairing heap.
    class Node
    {

    public:
        
        /// @brief Node constructor.
        /// @param val: The value to store in the node.
        /// @runtime: O(1)
        explicit Node(const T &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
        {}
        

        /// @brief Allows access to the element at that Node's position.
        /// @note There are two versions, getElt() and a dereference operator, which
        ///       are essentially equivalent.
        /// @runtime: O(1).
        const T &getElt() const { return elt; }
        const T &operator*() const { return elt; }
        

        /// @brief To access private data members of this Node class 
        ///        from within the PairingPQ class.
        friend PairingPQ;

    private:

        /// @note Parent pointers always point up the tree,
        ///       which is different from the 'prev' pointer.
        T elt;
        Node *child;
        Node *sibling;
        Node *parent;
        
    }; // class Node
    

    
    /// @brief Construct an empty pairing heap with an optional comparison functor.
    /// @runtime: O(1)
    explicit PairingPQ(const Compare &comp = Compare()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {} // DEFAULT CTOR
    
    

    /// @brief Construct a pairing heap out of an iterator range with an optional
    ///        comparison functor.
    /// @runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end,
              const Compare &comp = Compare()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {
        while (start != end)
        {
            push(*start);
            start++;
        }
    } // R-B CTOR
    
    

    /// @brief Copy constructor - performs deep copy
    /// @param other The heap to copy from
    /// @runtime: O(n)
    PairingPQ(const PairingPQ &other) :
    BaseClass{ other.compareFunctor }, root{ nullptr }, numNodes{ 0 }
    {
        if (other.root)
        {
            deque<Node*> dq;
            dq.push_back(other.root);
            
            while (!dq.empty())
            {
                // get the next element
                Node *ptr = traversalHelper(dq);
                
                // I now have the current node's child and sibling in the deck
                // waiting to get pushed. I push the current node first.
                push(ptr->elt);
            }
        }
    } // COPY CTOR
    


    /// @brief Copy assignment operator - uses copy-swap method 
    /// @param rhs The heap to copy from
    /// @runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs)
    {
        // Invoke copy ctor 
        PairingPQ temp(rhs);
        
        // Begin swapping the temp's members with the current 
        // object's (this = lhs) members to get lhs = rhs.
        swap(temp.numNodes, numNodes);
        swap(temp.root, root);
        
        // Temp is destroyed once it goes out-of-scope.
        return *this;
    } // operator=()



    /// @brief Move constructor.
    /// @runtime: O(1)
    PairingPQ(PairingPQ&& other) noexcept
        : BaseClass(std::move(other)), 
          root(other.root),
          numNodes(other.numNodes) 
    {
        other.root = nullptr;
        other.numNodes = 0;
    } // MOVE CTOR



    /// @brief Destructor.
    /// @runtime: O(n)
    ~PairingPQ()
    {
        if (isEmpty())
            return;
        
        deque<Node*> dq;
        dq.push_back(root);
        while (!dq.empty())
        {
            Node *ptr = traversalHelper(dq);
            delete ptr;
        }
        
        // Reset the heap size.
        numNodes = 0;
    } // ~PairingPQ()



    /// @brief Assumes that all elements inside the pairing heap are out of order and
    ///        'rebuilds' the pairing heap by fixing the pairing heap invariant.
    /// @warning You CANNOT delete 'old' nodes and create new ones!
    /// @runtime: O(n)
    virtual void updatePQ()
    {
        if (!root || !root->child) return;

        // Create an aux deque.
        deque<Node*> dq;
        dq.push_back(root->child);
        root->child = nullptr;
        
        // Start from the root and work down the tree.
        while (!dq.empty())
        {
            Node *ptr = traversalHelper(dq);
            parentSiblingSever(ptr);
            root = meld(ptr, root);
        } // while()
    } // updatePQ()


    
    /// @brief Add a new element to the pairing heap.
    /// @note  Push functionality is implemented entirely in the addNode()
    ///        function, which this function calls.
    /// @runtime: O(1)
    virtual void push(const T &val) {
        addNode(val);
    } // push()
    
    

    /// @brief Remove the most extreme (defined by 'compare') element from
    ///        the pairing heap via multi-pass pop. 
    /// @note  
    /// @runtime: Amortized O(log(n))
    virtual void pop()
    {
        // Base case: empty heap.
        if (isEmpty())
            throw runtime_error("Cannot pop from empty priority queue");

        // Isolate, then delete the highest priority node (root).
        Node *temp = root->child;
        delete root;
        numNodes--; // update the heap size
        
        // Three cases: one node, two nodes, or many.
        if (!temp)
        {
            root = nullptr;
        }
        else if (getSize() == 1)
        {
            temp->parent = nullptr;
            root = temp;
        }
        else // Heap restored!
            root = multiPassPairing(temp);
    } // pop()
    

    
    /// @brief Return the most extreme (defined by 'compare') element of
    ///        the heap.  This should be a reference for speed.  It MUST be
    ///        const because we cannot allow it to be modified, as that
    ///        might make it no longer be the most extreme element.
    /// @runtime: O(1)
    virtual const T &getTop() const
    {
        if (isEmpty())
            throw runtime_error("Cannot access top of empty priority queue");
        return root->elt;
    } // getTop()
    
    
    /// @brief Get the number of elements in the pairing heap.
    /// @runtime: O(1)
    virtual size_t getSize() const {
        return numNodes;
    } // getSize()
    

    /// @brief Return true if the pairing heap is empty.
    /// @runtime: O(1)
    virtual bool isEmpty() const {
        return !root;
    } // isEmpty()
    
    
    /// @brief Updates the priority of an element already in the pairing heap by
    ///        replacing the element refered to by the Node with newPriority.
    ///        Must maintain pairing heap invariants. Uses a linked-list type
    ///        traversal to find the node's previous sibling link, which is cut.
    ///
    /// @pre @cond The new priority, given by 'newPriority' must be more extreme
    ///              (as defined by comp) than the old priority.
    ///
    /// @note  Three main cases:
    ///        1. node = root (min work needed),
    ///        2. node != root & newPriority < parent->elt (min work needed),
    ///        3. node != root & parent->elt < newPriority (max work: two sub cases),
    ///        3a. node = leftmost (less work: no traversal required),
    ///        3b. node != leftmost (more work: traversal is required).
    ///
    /// @runtime: O(log(n))
    void updateElt(Node* node, const T &newPriority)
    {
        if (!node)
            throw invalid_argument("Cannot update null node pointer");

        // Check for heap precondition.    
        if (!this->compareFunctor(node->elt, newPriority))
            throw invalid_argument("New value must have higher priority than current value");

        // Update the priority
        node->elt = newPriority;

        /// NOTE: IF I ABSTRACT THIS if-else LOGIC, I CAN CALL IT findSiblingAndUpdate() OR findSiblingAndExtract()
        
        // Check if node has a parent and if the new value is greater than the parent's value
        if (node->parent && // roots dont have parents 
            this->compareFunctor(node->parent->elt, newPriority)) // 3rd case
        {
            // Store parent before severing links
            Node *p = node->parent;
            
            // Get the leftmost child since I have to sever the 
            // sibling link pointing to me, and I dont store the prev ptr.
            // In either case, I have to extend the link past node, to 
            // its sibling.
            if (p->child != node) // if not leftmost
            {
                // Find my previous sibling (traverse until one before node)
                Node *prev = p->child;
                while (prev->sibling != node)
                    prev = prev->sibling;
                
                prev->sibling = node->sibling;
            }
            else // Node is the leftmost child
                p->child = node->sibling;
            
            // Isolate node and its children from the heap.
            parentSiblingSever(node);
            
            // Meld node subheap back into the rootheap.
            root = meld(root, node);
        }
    } // updateElt()
    
    

    /// @brief    Add a new element to the pairing heap.
    /// @return   A Node* corresponding to the newly added element.
    /// @runtime: O(1)
    /// @warning  Whenever you create a node, and thus return a Node *, you must be sure to
    ///           never move or copy/delete that node in the future, until it is eliminated
    ///           by the user calling pop().  Remember this when you implement updateElt() and
    ///           updatePQ(). WHY: IS THIS TRUE??????
    Node* addNode(const T &val)
    {
        Node *newNode = new Node{ val };

        if (isEmpty()) // give it to root
            root = newNode;
        else        // meld it
            root = meld(newNode, root);
        
        numNodes++;
        
        return newNode;
    } // addNode()



    /// @brief Print the heap structure.
    /// @note Prints the heap in a tree-like format.
    /// @runtime: O(n)
    void print() const 
    {
        cout << "PairingPQ [size=" << getSize() << "]: ";
        if (isEmpty()) {
            cout << "(empty)" << endl;
        } else {
            cout << "(top=" << getTop() << ")" << endl;
            printTree(root, "", true);
        }
    } // print()    
    
    
private:



    /// @brief Sever the parent and sibling links of a node.
    /// @runtime: O(1)
    void parentSiblingSever(Node *node)
    {
        if (!node) return;
        node->parent = nullptr;
        node->sibling = nullptr;
    } // parentSiblingSever()



    /// NOTE: IS THIS FUNCTION NAME APPROPRIATE? traverseAndExtract()
    /// @brief Processes one node and adds children to the dq queue.
    /// @param dq: A deque of pointers to nodes.
    /// @return A pointer to the next node to process.
    /// @runtime: O(1)
    Node *traversalHelper(deque<Node*> &dq)
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



    /// @brief Perform multi-pass pairing on a list of siblings
    /// @param child The first node in the sibling list
    /// @return The root of the resulting paired heap
    /// @details Implements the multi-pass pairing strategy for optimal performance.
    ///          Nodes must be preprocessed by severing their parent and sibling links
    ///          before melding.
    /// @runtime: O(log n) amortized
    Node* multiPassPairing(Node *child)
    {
        deque<Node*> dq;
        
        // Collect all children 
        while (child)
        {
            dq.push_back(child);
            child = child->sibling;
        }
        
        // Pair adjacent siblings, pop and meld them, then 
        // push the result to the back until one remains.
        while (dq.size() != 1)
        {
            Node *n1 = dq.front();
            parentSiblingSever(n1);
            dq.pop_front();

            Node *n2 = dq.front();
            parentSiblingSever(n2);
            dq.pop_front();
            
            dq.push_back(meld(n1, n2));
        }
        
        return dq.front();
    } // multiPassPairing()


    
    /// @brief Links two non null heaps: root a and root b 
    ///        MUST NOT have a parent or siblings.
    /// @runtime: O(1)
    Node* meld(Node *a, Node *b)
    {
        // Get the extreme value.
        if (this->compareFunctor(b->elt, a->elt))
            swap(a, b);
        
        // Make the lower priority node a child 
        // of the higher priority node.
        a->parent = b;
        a->sibling = b->child;
        b->child = a; // highest priority node
        return b;        
    } // meld()



    /// @brief Print the tree structure recursively, for debugging.
    /// @runtime: O(n)
    void printTree(Node* node, const string& prefix, bool isLast) const 
    {
        if (!node) return;
        
        cout << prefix;
        cout << (isLast ? "└── " : "├── ");
        cout << node->elt << endl;
        
        // Collect all children for printing
        vector<Node*> children;
        Node* child = node->child;
        while (child) 
        {
            children.push_back(child);
            child = child->sibling;
        }
        
        for (size_t i = 0; i < children.size(); ++i) 
        {
            printTree(
                children[i], 
                prefix + (isLast ? "    " : "│   "), 
                i == children.size() - 1
            );
        }
    } // printTree()

    
    // Root of heap and size.
    Node *root;
    size_t numNodes;
};





#endif // PAIRINGPQ_H



















