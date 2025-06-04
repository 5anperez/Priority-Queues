#ifndef PAIRING_PQ_H
#define PAIRING_PQ_H

#include "SPsPQ.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>

using namespace std;





/**
 * ✅ ❌
 * 
 * TODO: 
 * - CODE DUPE
 * - TAILOR THE CODE TO *MY* SPECIFICATIONS.
 * - THERE HAS TO BE EDGE TESTS I AM MISSING?
 * - 
 */






/// @implements PREV VERSION
/// @brief A pairing heap implementation of a priority queue.
/// @param T The type of elements stored in the heap/queue
/// @param Compare The comparison function (default: std::less<T> for max heap)
/// @note It uses raw pointers for memory efficiency.
/// @details The main difference between this and the PARENT version is that
///          prev pointers make the updateElt function more efficient by
///          allowing very quick sibling-sibling link severing when the node
///          is not the leftmost child since all links must be cut before melding.
///          Note loops required within updateElt()'s 3rd case!
///          HOWEVER, the prev pointers dont allow peeking at the parent's value,
///          therfore, severing and melding are ALWAYS required.
/// @runtime: This implementation provides O(1) insert, O(1) increase-priority,
///           O(1) find-extreme-priority, and O(log n) amortized delete-extreme-priority.
template <typename T, typename Compare = std::less<T>>
class PairingPQ : public SPsPQ<T, Compare>
{
    using BaseClass = SPsPQ<T, Compare>;

public:
    
    struct Node 
    {
        /// @note Prev pointers always point to the left,
        ///       which is different from the 'parent' pointer.
        T value;
        Node* child;    // Leftmost child
        Node* sibling;  // Right sibling
        Node* prev;     // Previous sibling 
        
        Node(const T& val) 
            : value(val), child(nullptr), sibling(nullptr), prev(nullptr) {}

        const T &getElt() const { return value; }
        const T &operator*() const { return value; }
        
    };


private:

    Node* root;           // Root of the heap
    std::size_t numNodes; // Number of nodes in the heap



public:

    // Default constructor
    explicit PairingPQ(const Compare &comp = Compare())
        : BaseClass(comp), root(nullptr), numNodes(0) {}
    // DEFAULT CTOR


    // Range-based constructor
    template <typename Iterator>
    PairingPQ(Iterator begin, Iterator end, const Compare &comp = Compare())
        : BaseClass(comp), root(nullptr), numNodes(0) {
        for (Iterator it = begin; it != end; ++it) {
            push(*it);
        }
    }
    // R-B CTOR


    // Copy constructor
    PairingPQ(const PairingPQ& other) 
        : BaseClass(other), root(nullptr), numNodes(0) {
        if (other.root) {
            root = copyTree(other.root);
            numNodes = other.numNodes;
        }
    }
    // COPY CTOR


    // Move constructor
    PairingPQ(PairingPQ&& other) noexcept
        : BaseClass(std::move(other)), 
          root(other.root),
          numNodes(other.numNodes) {
        other.root = nullptr;
        other.numNodes = 0;
    }
    // MOVE CTOR


    // Copy assignment
    PairingPQ& operator=(const PairingPQ& other) {
        if (this != &other) {
            BaseClass::operator=(other);
            clearTree(root);
            root = nullptr;
            numNodes = 0;
            if (other.root) {
                root = copyTree(other.root);
                numNodes = other.numNodes;
            }
        }
        return *this;
    }
    // COPY ASSIGNMENT


    // Move assignment
    PairingPQ& operator=(PairingPQ&& other) noexcept {
        if (this != &other) {
            BaseClass::operator=(std::move(other));
            clearTree(root);
            root = other.root;
            numNodes = other.numNodes;
            other.root = nullptr;
            other.numNodes = 0;
        }
        return *this;
    }
    // MOVE ASSIGNMENT


    // Destructor
    virtual ~PairingPQ() {
        clearTree(root);
    }


    /// @brief Push a new value into the heap.
    /// @param value: The value to push into the queue.
    /// @note Creates a new node and merges it with the root.
    void push(const T &value) override 
    {
        Node* newNode = new Node(value);
        root = merge(root, newNode);
        ++numNodes;
    } // push()


    /// @brief Add a new element to the pairing heap.
    /// @param value: The value to add to the heap.
    /// @return Pointer to the newly created node.
    /// @note This allows external tracking of nodes for updateValue operations.
    Node* addNode(const T& value) 
    {
        Node* newNode = new Node(value);
        root = merge(root, newNode);
        ++numNodes;
        return newNode;
    } // addNode()


    /// @brief Update the priority of a node in the heap.
    /// @param node: Pointer to the node to update.
    /// @param newValue: The new value (must have higher priority than current).
    /// @note For max-heap: newValue must be >= node->value
    ///       For min-heap: newValue must be <= node->value
    void updateElt(Node* node, const T& newValue) 
    {
        if (!node) {
            throw std::invalid_argument("Cannot update null node");
        }
        
        // Verify the new value has higher priority
        if (hasLowerPriority(newValue, node->value)) {
            throw std::invalid_argument("New value must have higher or equal priority");
        }
        
        // Update the value
        node->value = newValue;
        
        // If node is root, nothing more to do
        if (node == root) return;
        
        // Detach node from its current position
        detachNode(node);
        
        // Merge the detached subtree back with root
        root = merge(root, node);
    } // updateElt()


    /// @brief Remove the top element (minimum/maximum).
    /// @note Performs a two-pass pairing of the root's children.
    void pop() override {
        if (isEmpty()) return;
        
        Node* oldRoot = root;
        root = twoPassPairing(root->child);
        delete oldRoot;
        --numNodes;
    } // pop()


    /// @brief Get the top element (highest priority).
    /// @return The top element of the queue.
    const T &getTop() const override {
        if (isEmpty())
            throw std::runtime_error("Priority queue is empty");
        return root->value;
    } // getTop()


    /// @brief Get the current size.
    /// @return The current size of the queue.
    std::size_t getSize() const override {
        return numNodes;
    } // getSize()


    /// @brief Check if the queue is empty.
    /// @return True if the queue is empty, false otherwise.
    bool isEmpty() const override {
        return root == nullptr;
    } // isEmpty()


    /// @brief Rebuild the entire heap from scratch.
    /// @note This collects all reachable values and rebuilds the heap.
    void updatePQ() override {
        if (isEmpty()) return;
        
        // Collect all values from the heap
        std::vector<T> values;
        collectValues(root, values);
        
        // Clear and rebuild
        clearTree(root);
        root = nullptr;
        numNodes = 0;
        
        for (const T& value : values) {
            push(value);
        }
    } // updatePQ()


    /// @brief Print the heap structure.
    /// @note Prints the heap in a tree-like format.
    void print() const {
        std::cout << "PairingPQ [size=" << getSize() << "]: ";
        if (isEmpty()) {
            std::cout << "(empty)" << std::endl;
        } else {
            std::cout << "(top=" << getTop() << ")" << std::endl;
            printTree(root, "", true);
        }
    } // print()


private:

    /// @brief Detach a node from its current position in the heap.
    /// @param node: The node to detach.
    /// @note Maintains all sibling and parent links correctly.
    void detachNode(Node* node) 
    {
        // If node has a previous sibling, update its sibling pointer
        if (node->prev && node->prev->sibling == node) {
            node->prev->sibling = node->sibling;
        }
        // If node is the leftmost child of its parent, update parent's child pointer
        else if (node->prev && node->prev->child == node) {
            node->prev->child = node->sibling;
        }
        
        // If node has a next sibling, update its prev pointer
        if (node->sibling) {
            node->sibling->prev = node->prev;
        }
        
        // Clear node's sibling and prev pointers
        node->sibling = nullptr;
        node->prev = nullptr;
    } // detachNode()


    /// @brief Merge two heaps rooted at the given nodes.
    /// @param a: Root of first heap.
    /// @param b: Root of second heap.
    /// @return Root of the merged heap.
    /// @note The second if inside executes when a has lower priority than b,
    ///       meaning b should become the new root. This happens when we're
    ///       maintaining a max-heap and b's value > a's value.
    Node* merge(Node* a, Node* b) // root and newNode
    {
        if (!a) return b;
        if (!b) return a;
        
        // This 2nd if executes when a has lower priority than b
        // Example: In a max-heap (using std::less), if a->value < b->value,
        // then b should be the parent, so we swap them
        if (hasLowerPriority(a->value, b->value)) {
            std::swap(a, b);
        }
        
        // Make b the leftmost child of a
        b->sibling = a->child;
        if (a->child) {
            a->child->prev = b;
        }
        b->prev = a;
        a->child = b;
        
        return a;
    } // merge()


    /// @brief Perform two-pass pairing on a list of siblings.
    /// @param firstChild: First child in the sibling list.
    /// @return Root of the resulting heap.
    Node* twoPassPairing(Node* firstChild) {
        if (!firstChild) return nullptr;
        
        // Handle single child case
        if (!firstChild->sibling) {
            firstChild->prev = nullptr;
            return firstChild;
        }
        
        std::vector<Node*> pairs;
        
        // First pass: pair up siblings left to right
        Node* curr = firstChild;
        while (curr) {
            Node* next = curr->sibling;
            if (next) {
                Node* nextNext = next->sibling;
                
                // Clear sibling pointers before merging
                curr->sibling = nullptr;
                curr->prev = nullptr;
                next->sibling = nullptr;
                next->prev = nullptr;
                
                pairs.push_back(merge(curr, next));
                curr = nextNext;
            } else {
                // Odd node out
                curr->sibling = nullptr;
                curr->prev = nullptr;
                pairs.push_back(curr);
                curr = nullptr;
            }
        }
        
        // Second pass: merge pairs right to left
        Node* result = pairs.back();
        for (std::size_t i = pairs.size() - 1; i-- > 0;) {
            result = merge(result, pairs[i]);
        }
        
        return result;
    } // twoPassPairing()


    /// @brief Helper to check if a has lower priority than b.
    bool hasLowerPriority(const T& a, const T& b) const {
        return this->compareFunctor(a, b);
    } // hasLowerPriority()


    /// @brief Recursively delete all nodes in the tree.
    void clearTree(Node* node) {
        if (!node) return;
        
        Node* child = node->child;
        while (child) {
            Node* nextChild = child->sibling;
            clearTree(child);
            child = nextChild;
        }
        
        delete node;
    } // clearTree()


    /// @brief Deep copy a tree structure.
    Node* copyTree(Node* node) 
    {
        if (!node) return nullptr;
        
        Node* newNode = new Node(node->value);
        
        // Copy all children
        Node* child = node->child;
        Node* lastCopied = nullptr;
        
        while (child) 
        {
            Node* copiedChild = copyTree(child);
            copiedChild->prev = newNode;
            
            if (!newNode->child) {
                newNode->child = copiedChild;
            } else {
                lastCopied->sibling = copiedChild;
                copiedChild->prev = lastCopied;
            }
            
            lastCopied = copiedChild;
            child = child->sibling;
        }
        
        return newNode;
    } // copyTree()


    /// @brief Collect all values from the heap into a vector.
    void collectValues(Node* node, std::vector<T>& values) const {
        if (!node) return;
        
        values.push_back(node->value);
        
        // Collect from all children
        Node* child = node->child;
        while (child) {
            collectValues(child, values);
            child = child->sibling;
        }
    } // collectValues()


    /// @brief Print the tree structure recursively.
    void printTree(Node* node, const std::string& prefix, bool isLast) const {
        if (!node) return;
        
        std::cout << prefix;
        std::cout << (isLast ? "└── " : "├── ");
        std::cout << node->value << std::endl;
        
        // Collect all children for printing
        std::vector<Node*> children;
        Node* child = node->child;
        while (child) {
            children.push_back(child);
            child = child->sibling;
        }
        
        for (std::size_t i = 0; i < children.size(); ++i) {
            printTree(children[i], 
                     prefix + (isLast ? "    " : "│   "), 
                     i == children.size() - 1);
        }
    } // printTree()

}; // class PairingPQ







#endif // PAIRING_PQ_H






