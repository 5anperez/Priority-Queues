#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "SPsPQ.h"
#include <deque>
#include <utility>
#include <iostream>
#include <stdexcept>

using namespace std;

/**
 * @brief A pairing heap implementation of a priority queue.
 * 
 * The pairing heap is a heap-ordered multiway tree where operations are performed
 * efficiently through a "pairing" process. It provides excellent amortized performance
 * for most operations, particularly decrease-key operations.
 * 
 * Key properties:
 * - Heap property: parent >= all descendants (for max-heap)
 * - Multiway tree structure with leftmost-child, right-sibling representation
 * - Supports efficient decrease-key operations
 * 
 * @tparam T The type of elements stored in the heap
 * @tparam Compare The comparison function object type
 */
template <typename T, typename Compare = std::less<T>>
class PairingPQ : public SPsPQ<T, Compare>
{
    using BaseClass = SPsPQ<T, Compare>;
    
public:
    
    /**
     * @brief Node structure for the pairing heap.
     * 
     * Uses a leftmost-child, right-sibling representation:
     * - child: points to leftmost child
     * - sibling: points to next sibling (forms a linked list of children)
     * - parent: points to parent node
     */
    class Node
    {
    public:
        /**
         * @brief Construct a new Node with the given value.
         * @param val The value to store in this node
         * Runtime: O(1)
         */
        explicit Node(const T &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
        {}
        
        const T &getElt() const { return elt; }
        const T &operator*() const { return elt; }
        
        friend PairingPQ;

    private:
        T elt;          ///< The element stored in this node
        Node *child;    ///< Pointer to leftmost child
        Node *sibling;  ///< Pointer to next sibling
        Node *parent;   ///< Pointer to parent node
    };
    
    /**
     * @brief Default constructor.
     * @param comp Comparison function object
     * Runtime: O(1)
     */
    explicit PairingPQ(const Compare &comp = Compare()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {}
    
    /**
     * @brief Range constructor - builds heap from iterator range.
     * @param start Beginning of range
     * @param end End of range  
     * @param comp Comparison function object
     * Runtime: O(n log n) where n = distance(start, end)
     */
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end,
              const Compare &comp = Compare()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {
        while (start != end)
        {
            push(*start);
            ++start;
        }
    }
    
    /**
     * @brief Copy constructor - creates deep copy of other heap.
     * Uses level-order traversal to copy all nodes.
     * @param other The heap to copy
     * Runtime: O(n) where n is number of nodes
     */
    PairingPQ(const PairingPQ &other) :
    BaseClass{ other.compareFunctor }, root{ nullptr }, numNodes{ 0 }
    {
        if (other.root)
        {
            std::deque<Node*> dq;
            dq.push_back(other.root);
            
            while (!dq.empty())
            {
                Node *ptr = traversalHelper(dq);
                push(ptr->elt);
            }
        }
    }
    
    /**
     * @brief Copy assignment operator using copy-and-swap idiom.
     * @param rhs The heap to copy
     * @return Reference to this heap
     * Runtime: O(n) where n is number of nodes in rhs
     */
    PairingPQ &operator=(const PairingPQ &rhs)
    {
        PairingPQ temp(rhs);  // Copy construct
        std::swap(temp.numNodes, numNodes);  // Swap contents
        std::swap(temp.root, root);
        return *this;  // temp destructor cleans up old data
    }

    /**
     * @brief Move constructor - transfers ownership of resources.
     * @param other The heap to move from
     * Runtime: O(1)
     */
    PairingPQ(PairingPQ&& other) noexcept
        : BaseClass(std::move(other)), 
          root(other.root),
          numNodes(other.numNodes) 
    {
        other.root = nullptr;
        other.numNodes = 0;
    }

    /**
     * @brief Destructor - deallocates all nodes using level-order traversal.
     * Runtime: O(n) where n is number of nodes
     */
    ~PairingPQ()
    {
        if (!root) return;
        
        std::deque<Node*> dq;
        dq.push_back(root);
        
        while (!dq.empty())
        {
            Node *ptr = traversalHelper(dq);
            delete ptr;
        }
        
        numNodes = 0;
    }

    /**
     * @brief Rebuilds heap after arbitrary modifications to node values.
     * 
     * This operation is used when multiple nodes have been modified and
     * heap property may be violated. It performs a series of meld operations
     * to restore the heap property.
     * 
     * Caveat: This is an expensive operation and should be used sparingly.
     * Runtime: O(n log n) amortized, where n is number of nodes
     */
    virtual void updatePQ()
    {
        if (!root || !root->child) return;
        
        std::deque<Node*> dq;
        dq.push_back(root->child);
        root->child = nullptr;
        
        while (!dq.empty())
        {
            Node *ptr = traversalHelper(dq);
            severFromParent(ptr);  // Clean up relationships
            root = meld(ptr, root);
        }
    }
    
    /**
     * @brief Insert a new element into the heap.
     * @param val The value to insert
     * Runtime: O(1) amortized
     */
    virtual void push(const T &val)
    {
        addNode(val);
    }
    
    /**
     * @brief Remove and return the maximum element (root).
     * 
     * Algorithm:
     * 1. Save root's children
     * 2. Delete root
     * 3. Perform multi-pass pairing on children to rebuild heap
     * 
     * The multi-pass algorithm pairs adjacent children in the first pass,
     * then continues pairing until only one tree remains.
     * 
     * @throws std::runtime_error if heap is empty
     * Runtime: O(log n) amortized, O(n) worst case
     */
    virtual void pop()
    {
        if (isEmpty())
            throw std::runtime_error("Cannot pop from empty priority queue");

        Node *firstChild = root->child;
        
        // Delete root and update count
        delete root;
        numNodes--;
        
        if (!firstChild)  // Heap becomes empty
        {
            root = nullptr;
            return;
        }
        
        if (numNodes == 1)  // Only one node left
        {
            firstChild->parent = nullptr;
            root = firstChild;
            return;
        }
        
        // Multi-pass pairing algorithm
        root = multiPassPairing(firstChild);
    }
    
    /**
     * @brief Get the maximum element without removing it.
     * @return Reference to the maximum element
     * @throws std::runtime_error if heap is empty
     * Runtime: O(1)
     */
    virtual const T &getTop() const
    {
        if (isEmpty())
            throw std::runtime_error("Cannot access top of empty priority queue");
        return root->elt;
    }
    
    /**
     * @brief Get the number of elements in the heap.
     * @return Number of elements
     * Runtime: O(1)
     */
    virtual std::size_t getSize() const
    {
        return numNodes;
    }
    
    /**
     * @brief Check if the heap is empty.
     * @return true if empty, false otherwise
     * Runtime: O(1)
     */
    virtual bool isEmpty() const
    {
        return !root;
    }
    
    /**
     * @brief Update the value of a specific node (decrease-key operation).
     * 
     * This is one of the key advantages of pairing heaps - efficient decrease-key.
     * If the new value violates heap property with parent, the node is cut from
     * its current position and melded back with the root.
     * 
     * @param node Pointer to the node to update (must be valid)
     * @param new_value The new value (must be >= current value for max-heap)
     * @throws std::runtime_error if node is invalid
     * @throws std::invalid_argument if new_value violates heap ordering
     * Runtime: O(log n) amortized
     */
    void updateElt(Node* node, const T &new_value)
    {
        if (!node)
            throw std::invalid_argument("Cannot update null node pointer");
        
        if (!this->compareFunctor(node->elt, new_value))
            throw std::invalid_argument("New value must have higher priority than current value");
        
        node->elt = new_value;
        
        // If node has parent and violates heap property, cut and meld
        if (node->parent && this->compareFunctor(node->parent->elt, new_value))
        {
            Node *parent = node->parent;
            severFromSiblings(node, parent);  // Remove from sibling list
            severFromParent(node);            // Clear parent/sibling pointers
            root = meld(root, node);          // Meld back into heap
        }
    }
    
    /**
     * @brief Add a new node to the heap and return pointer to it.
     * 
     * This method provides access to the actual node for later updates.
     * The node is created and melded with the existing heap.
     * 
     * @param val The value to insert
     * @return Pointer to the newly created node
     * Runtime: O(1) amortized
     */
    Node* addNode(const T &val)
    {
        Node *newNode = new Node{ val };
        
        if (isEmpty())
            root = newNode;
        else        
            root = meld(newNode, root);
        
        numNodes++;
        return newNode;  // Return the newly created node
    }

    /**
     * @brief Print the heap structure for debugging.
     * Uses recursive traversal to display the tree structure with indentation.
     * Runtime: O(n) where n is number of nodes
     */
    void print() const 
    {
        std::cout << "PairingPQ structure (size=" << numNodes << "):\n";
        if (isEmpty()) {
            std::cout << "[empty]\n";
            return;
        }
        
        printTree(root, 0);
        std::cout << std::endl;
    }
    
private:
    
    /**
     * @brief Helper for level-order traversal during copy/destruction.
     * 
     * Gets next node from deque and adds its children to the deque.
     * This maintains level-order traversal order.
     * 
     * @param dq Deque containing nodes to visit
     * @return Next node to process
     * Runtime: O(1) amortized
     */
    Node *traversalHelper(std::deque<Node*> &dq)
    {
        Node *ptr = dq.front();
        dq.pop_front();
            
        // Add siblings and children to continue traversal
        if (ptr->sibling)
            dq.push_back(ptr->sibling);
        if (ptr->child)
            dq.push_back(ptr->child);

        return ptr;
    }
    
    /**
     * @brief Meld two heap trees with roots a and b.
     * 
     * The tree with larger root becomes the new root, and the other tree
     * becomes its leftmost child. This maintains the heap property.
     * 
     * Precondition: Both nodes must have no parent or sibling pointers set.
     * 
     * @param a Root of first tree
     * @param b Root of second tree  
     * @return Root of the melded tree
     * Runtime: O(1)
     */
    Node* meld(Node *a, Node *b)
    {
        // Determine which node should be the new root
        if (this->compareFunctor(b->elt, a->elt))
        {
            // a becomes root, b becomes its leftmost child
            b->parent = a;
            b->sibling = a->child;
            a->child = b;
            return a;
        }
        else
        {
            // b becomes root, a becomes its leftmost child  
            a->parent = b;
            a->sibling = b->child;
            b->child = a;
            return b;
        }
    }

    /**
     * @brief Remove a node from its sibling list.
     * 
     * Updates the sibling pointers to bypass the given node.
     * The node must be a child of the given parent.
     * 
     * @param node Node to remove from sibling list
     * @param parent Parent of the node
     * Runtime: O(k) where k is number of siblings
     */
    void severFromSiblings(Node* node, Node* parent)
    {
        if (parent->child == node)
        {
            // Node is leftmost child - update parent's child pointer
            parent->child = node->sibling;
        }
        else
        {
            // Find previous sibling and update its sibling pointer
            Node *prev = parent->child;
            while (prev && prev->sibling != node)
            {
                prev = prev->sibling;
            }
            if (prev)
            {
                prev->sibling = node->sibling;
            }
        }
    }
    
    /**
     * @brief Clear a node's parent and sibling pointers.
     * 
     * This prepares a node for melding by ensuring it has no existing
     * relationships that could create cycles or inconsistencies.
     * 
     * @param node Node to sever from its parent
     * Runtime: O(1)
     */
    void severFromParent(Node* node)
    {
        node->parent = nullptr;
        node->sibling = nullptr;
    }
    
    /**
     * @brief Perform multi-pass pairing on a list of sibling nodes.
     * 
     * Algorithm:
     * 1. Collect all siblings into a deque
     * 2. While more than one tree exists:
     *    - Pair adjacent trees from front of deque
     *    - Add result to back of deque
     * 3. Return the final remaining tree
     * 
     * @param firstSibling First node in the sibling list
     * @return Root of the rebuilt heap
     * Runtime: O(log n) amortized
     */
    Node* multiPassPairing(Node* firstSibling)
    {
        std::deque<Node*> dq;
        
        // Collect all siblings and clean up their relationships
        Node* current = firstSibling;
        while (current)
        {
            Node* next = current->sibling;
            severFromParent(current);  // Clean up relationships
            dq.push_back(current);
            current = next;
        }
        
        // Perform multi-pass pairing
        while (dq.size() > 1)
        {
            Node* first = dq.front(); dq.pop_front();
            Node* second = dq.front(); dq.pop_front();
            
            // Meld the two trees and add result to back
            Node* melded = meld(first, second);
            dq.push_back(melded);
        }
        
        return dq.front();
    }

    /**
     * @brief Recursively print the tree structure with indentation.
     * @param node Current node to print
     * @param depth Current depth for indentation
     * Runtime: O(n) where n is number of nodes in subtree
     */
    void printTree(Node* node, int depth) const 
    {
        if (node == nullptr) return;
        
        // Print current node with indentation
        for (int i = 0; i < depth; ++i) {
            std::cout << "  ";
        }
        std::cout << node->elt << "\n";
        
        // Print all children (traverse sibling list)
        Node* child = node->child;
        while (child != nullptr) {
            printTree(child, depth + 1);
            child = child->sibling;
        }
    }

    Node *root;        ///< Root of the pairing heap
    size_t numNodes;   ///< Number of nodes in the heap
};

#endif // PAIRINGPQ_H

























