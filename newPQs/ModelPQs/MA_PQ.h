#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "SPsPQ.h"
#include <deque>
#include <utility>
#include <iostream>
#include <stdexcept>

using namespace std;

/// @brief A pairing heap implementation using raw pointers for memory efficiency.
/// @details Pairing heap is a self-adjusting heap that provides efficient amortized
///          time complexity for heap operations. It's particularly good for decrease-key
///          operations which are common in algorithms like Dijkstra's.
/// @param T The type of elements stored in the heap/queue
/// @param Compare The comparison function (default: std::less<T> for max heap)
template <typename T, typename Compare = std::less<T>>
class PairingPQ : public SPsPQ<T, Compare>
{
    using BaseClass = SPsPQ<T, Compare>;
    
public:
    
    /// @brief Node structure for the pairing heap
    /// @details Each node maintains pointers to its leftmost child, right sibling,
    ///          and parent for efficient heap operations.
    class Node
    {
    public:
        /// @brief Construct a node with given value
        /// @param val The value to store in the node
        explicit Node(const T &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
        {}
        
        /// @brief Get the element stored in this node
        /// @return Const reference to the element
        const T &getElt() const { return elt; }
        
        /// @brief Dereference operator for convenient element access
        /// @return Const reference to the element
        const T &operator*() const { return elt; }
        
        friend PairingPQ;
        
    private:
        T elt;          ///< The element stored in this node
        Node *child;    ///< Pointer to leftmost child
        Node *sibling;  ///< Pointer to right sibling
        Node *parent;   ///< Pointer to parent node
    };
    
    
    /// @brief Default constructor
    /// @param comp Comparison function for ordering elements
    /// @runtime O(1)
    explicit PairingPQ(const Compare &comp = Compare()) :
    BaseClass{ comp }, root{ nullptr }, numNodes{ 0 }
    {}
    
    
    /// @brief Range constructor - builds heap from iterator range
    /// @param start Beginning of range
    /// @param end End of range
    /// @param comp Comparison function
    /// @runtime O(n) where n is the number of elements
    /// @note Uses repeated insertion which is O(1) amortized per element
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
    }
    
    
    /// @brief Copy constructor - performs deep copy
    /// @param other The heap to copy from
    /// @runtime O(n) where n is the number of elements
    /// @details Uses level-order traversal to preserve heap structure
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
    
    
    /// @brief Copy assignment operator using copy-and-swap idiom
    /// @param rhs The heap to copy from
    /// @return Reference to this heap
    /// @runtime O(n) where n is the number of elements
    PairingPQ &operator=(const PairingPQ &rhs)
    {
        PairingPQ temp(rhs);
        std::swap(temp.numNodes, numNodes);
        std::swap(temp.root, root);
        return *this;
    }


    /// @brief Move constructor
    /// @param other The heap to move from
    /// @runtime O(1)
    PairingPQ(PairingPQ&& other) noexcept
        : BaseClass(std::move(other)), 
          root(other.root),
          numNodes(other.numNodes) 
    {
        other.root = nullptr;
        other.numNodes = 0;
    }


    /// @brief Destructor - deallocates all nodes
    /// @runtime O(n) where n is the number of elements
    /// @details Uses level-order traversal to delete all nodes
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


    /// @brief Update priority queue after internal modifications
    /// @runtime O(log n) amortized
    /// @details Re-melds all children of root after root removal
    /// @note This is primarily used internally after pop operations
    virtual void updatePQ()
    {
        if (!root || !root->child) return;
        
        std::deque<Node*> dq;
        dq.push_back(root->child);
        root->child = nullptr;
        
        while (!dq.empty())
        {
            Node *ptr = traversalHelper(dq);
            severNodeCompletely(ptr);
            root = meld(ptr, root);
        }
    }

    
    /// @brief Insert a new element into the heap
    /// @param val The value to insert
    /// @runtime O(1) amortized
    virtual void push(const T &val)
    {
        addNode(val);
    }
    
    
    /// @brief Remove the top element from the heap
    /// @runtime O(log n) amortized
    /// @throws std::runtime_error if heap is empty
    /// @details Removes root and performs multi-pass pairing of children
    virtual void pop()
    {
        if (isEmpty())
            throw std::runtime_error("Cannot pop from empty priority queue");

        Node *temp = root->child;
        root->child = nullptr;
        
        delete root;
        numNodes--;
        
        if (!temp)
        {
            root = nullptr;
        }
        else if (numNodes == 1)
        {
            temp->parent = nullptr;
            root = temp;
        }
        else
        {
            root = multiPassPairing(temp);
        }
    }
    
    
    /// @brief Get the top element without removing it
    /// @return Const reference to the top element
    /// @runtime O(1)
    /// @throws std::runtime_error if heap is empty
    virtual const T &getTop() const
    {
        if (isEmpty())
            throw std::runtime_error("Cannot get top of empty priority queue");
        return root->elt;
    }
    
    
    /// @brief Get the number of elements in the heap
    /// @return Number of elements
    /// @runtime O(1)
    virtual std::size_t getSize() const
    {
        return numNodes;
    }
    
    
    /// @brief Check if the heap is empty
    /// @return true if empty, false otherwise
    /// @runtime O(1)
    virtual bool isEmpty() const
    {
        return !root;
    }
    
    
    /// @brief Update the value of a node (increase priority only)
    /// @param node The node to update
    /// @param new_value The new value (must be higher priority than current)
    /// @runtime O(log n) amortized
    /// @throws std::invalid_argument if new value doesn't increase priority
    /// @details If the node becomes higher priority than its parent, it's
    ///          removed from its current position and melded with the root
    void updateElt(Node* node, const T &new_value)
    {
        if (!node)
            throw std::invalid_argument("Cannot update null node");
            
        if (!this->compareFunctor(node->elt, new_value))
            throw std::invalid_argument("New value must have higher priority than current value");
        
        node->elt = new_value;
        
        // If node has parent and now violates heap property
        if (node->parent && this->compareFunctor(node->parent->elt, new_value))
        {
            removeNodeFromTree(node);
            root = meld(root, node);
        }
    }
    
    
    /// @brief Add a new node to the heap
    /// @param val The value to add
    /// @return Pointer to the newly created node
    /// @runtime O(1) amortized
    /// @note The returned pointer remains valid until the node is removed
    Node* addNode(const T &val)
    {
        Node *newNode = new Node{ val };
        
        if (isEmpty())
            root = newNode;
        else
            root = meld(newNode, root);
        
        numNodes++;
        return newNode;
    }


    /// @brief Print the heap structure for debugging
    /// @runtime O(n) where n is the number of elements
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
    
    /// @brief Helper for tree traversal - processes one node and adds children to queue
    /// @param dq Deque containing nodes to process
    /// @return The node that was processed
    /// @runtime O(1)
    Node *traversalHelper(std::deque<Node*> &dq)
    {
        Node *ptr = dq.front();
        dq.pop_front();
            
        if (ptr->sibling)
            dq.push_back(ptr->sibling);
        if (ptr->child)
            dq.push_back(ptr->child);

        return ptr;
    }


    /// @brief Completely sever a node from its parent and siblings
    /// @param node The node to sever
    /// @runtime O(1)
    /// @details Sets parent, child, and sibling pointers to nullptr
    void severNodeCompletely(Node *node)
    {
        if (!node) return;
        node->parent = nullptr;
        node->sibling = nullptr;
        // Note: We don't clear child pointer as it may be needed
    }


    /// @brief Remove a node from its current position in the tree
    /// @param node The node to remove
    /// @runtime O(1) for leftmost child, O(k) for kth child
    /// @details Maintains all sibling relationships after removal
    void removeNodeFromTree(Node *node)
    {
        if (!node || !node->parent) return;
        
        Node *p = node->parent;
        
        if (p->child == node)
        {
            // Node is the leftmost child
            p->child = node->sibling;
            if (node->sibling)
                node->sibling->parent = p;
        }
        else
        {
            // Find previous sibling
            Node *prev = p->child;
            while (prev && prev->sibling != node)
            {
                prev = prev->sibling;
            }
            
            if (prev)
            {
                prev->sibling = node->sibling;
                if (node->sibling)
                    node->sibling->parent = p;
            }
        }
        
        // Clear node connections
        node->parent = nullptr;
        node->sibling = nullptr;
    }


    /// @brief Perform multi-pass pairing on a list of siblings
    /// @param firstChild The first node in the sibling list
    /// @return The root of the resulting paired heap
    /// @runtime O(log n) amortized
    /// @details Implements the two-pass pairing strategy for optimal performance
    Node* multiPassPairing(Node *firstChild)
    {
        if (!firstChild) return nullptr;
        
        std::deque<Node*> dq;
        
        // Collect all siblings
        while (firstChild)
        {
            dq.push_back(firstChild);
            firstChild = firstChild->sibling;
        }
        
        // Pair adjacent siblings until one remains
        while (dq.size() > 1)
        {
            Node *first = dq.front();
            dq.pop_front();
            Node *second = dq.front();
            dq.pop_front();
            
            severNodeCompletely(first);
            severNodeCompletely(second);
            
            dq.push_back(meld(first, second));
        }
        
        return dq.front();
    }

    
    /// @brief Meld two heap roots together
    /// @param a First heap root (must not have parent or siblings)
    /// @param b Second heap root (must not have parent or siblings)
    /// @return The new root after melding
    /// @runtime O(1)
    /// @details Makes the heap with lower priority root a child of the other
    Node* meld(Node *a, Node *b)
    {
        if (!a) return b;
        if (!b) return a;
        
        // Make the smaller root a child of the larger root
        if (this->compareFunctor(b->elt, a->elt))
        {
            makeChild(a, b);
            return a;
        }
        else
        {
            makeChild(b, a);
            return b;
        }
    }


    /// @brief Make one node a child of another
    /// @param parent The parent node
    /// @param child The node to become a child
    /// @runtime O(1)
    /// @details Handles all pointer updates for parent-child relationship
    void makeChild(Node *parent, Node *child)
    {
        child->parent = parent;
        child->sibling = parent->child;
        parent->child = child;
    }


    /// @brief Recursively print the tree structure
    /// @param node Current node to print
    /// @param depth Current depth in the tree
    /// @runtime O(n) where n is number of nodes in subtree
    void printTree(Node* node, int depth) const 
    {
        if (!node) return;
        
        for (int i = 0; i < depth; ++i) {
            std::cout << "  ";
        }
        std::cout << node->elt << "\n";
        
        Node* child = node->child;
        while (child)
        {
            printTree(child, depth + 1);
            child = child->sibling;
        }
    }

    
    Node *root;      ///< Root of the pairing heap
    size_t numNodes; ///< Number of nodes in the heap
};

#endif // PAIRINGPQ_H












