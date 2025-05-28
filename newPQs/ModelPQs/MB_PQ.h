#ifndef SORTED_PQ_H
#define SORTED_PQ_H

#include "SPsPQ.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

template <typename T, typename Compare = std::less<T>>
class SortedPQ : public SPsPQ<T, Compare>
{

public:

    
    explicit SortedPQ(const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp) {}


    
    template <typename Iterator>
    SortedPQ(Iterator begin, Iterator end, const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end) {
        updatePQ();
    }


    
    SortedPQ(const SortedPQ& other) 
        : SPsPQ<T, Compare>(other), data(other.data) {}

    
    SortedPQ(SortedPQ&& other) noexcept
        : SPsPQ<T, Compare>(std::move(other)), data(std::move(other.data)) {}

    
    SortedPQ& operator=(const SortedPQ& other) {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(other);
            data = other.data;
        }
        return *this;
    }

    
    SortedPQ& operator=(SortedPQ&& other) noexcept {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(std::move(other));
            data = std::move(other.data);
        }
        return *this;
    }



    virtual ~SortedPQ() = default;



    void push(const T &value) override 
    {
        auto pos = lower_bound(
            data.begin(), 
            data.end(), 
            value, 
            this->compareFunctor
        );

        data.insert(pos, value);
    }



    void pop() override {
        if (!isEmpty()) {
            data.pop_back(); 
        }
    }



    const T &getTop() const override {
        if (isEmpty())
            throw runtime_error("Priority queue is empty");
        return data.back(); 
    }



    size_t getSize() const override { 
        return data.size(); 
    } 


    bool isEmpty() const override { 
        return data.empty(); 
    }


    void updatePQ() override {
        if (getSize() <= 1) return;
        std::sort(data.begin(), data.end(), this->compareFunctor);
    }


    
    const T& at(std::size_t index) const {
        if (index >= getSize())
            throw std::out_of_range("Index out of range");
        return data[index];
    }

private:

    std::vector<T> data;
};

#endif 


























// Demonstrating the efficiency difference
void compareInsertionEfficiency() {
    cout << "Comparing lower_bound vs upper_bound insertion efficiency:\n";
    
    // Test case: inserting many duplicates
    SortedPQ<int> pq_lower, pq_upper;
    
    // Pre-populate with some duplicates
    for (int i = 0; i < 1000; ++i) {
        pq_lower.push(50);  // Creates [50, 50, 50, ...]
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Using lower_bound (current implementation)
    for (int i = 0; i < 100; ++i) {
        pq_lower.push(50);  // Each insertion shifts 1000+ elements
    }
    
    auto mid = std::chrono::high_resolution_clock::now();
    
    // Reset for upper_bound test
    decltype(pq_upper) pq_upper_test;
    for (int i = 0; i < 1000; ++i) {
        pq_upper_test.push(50);
    }
    
    // Using upper_bound (more efficient)
    for (int i = 0; i < 100; ++i) {
        pq_upper_test.push(50);  // Each insertion shifts 0 elements (appends)
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    // upper_bound will be significantly faster for duplicate-heavy scenarios
}















void testSortedPQ(const string &pqType) 
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (regular version) **********\n" << endl;

    // Test 1: Empty queue
    cout << "Test 1: empty queue..."
         << endl;
    SortedPQ<int> pq1;
    assert(pq1.isEmpty() && "Empty queue should report empty");
    try {
        pq1.getTop();
        std::cout << "Test 1 failed: Expected exception on empty getTop\n";
    } catch (const std::runtime_error&) {
        std::cout << "Test 1 passed: Empty queue throws on getTop\n";
    }



    // Test 2: Single element
    cout << "Test 2: single element..."
         << endl;
    pq1.push(42);
    assert(pq1.getSize() == 1 && "Size should be 1");
    assert(pq1.getTop() == 42 && "Top should be 42");
    pq1.pop();
    assert(pq1.isEmpty() && "Queue should be empty after pop");
    cout << "Test 2 passed!\n"
         << endl;



    // Test 3: Multiple elements - verify sorted order
    cout << "Test 3: multiple elements in sorted order..."
         << endl;
    SortedPQ<int> pq2;
    pq2.push(30);
    pq2.push(10);
    pq2.push(20);
    pq2.push(5);
    pq2.push(25);
    
    assert(pq2.getSize() == 5 && "Size should be 5");
    assert(pq2.getTop() == 30 && "Top should be max (30)");
    
    // Verify internal sorted order: [5, 10, 20, 25, 30]
    assert(pq2.at(0) == 5 && "First element should be 5");
    assert(pq2.at(1) == 10 && "Second element should be 10");
    assert(pq2.at(2) == 20 && "Third element should be 20");
    assert(pq2.at(3) == 25 && "Fourth element should be 25");
    assert(pq2.at(4) == 30 && "Fifth element should be 30");
    
    cout << "Test 3 passed!\n"
         << endl;



    // Test 4: Pop sequence verification
    cout << "Test 4: pop sequence verification..."
         << endl;
    assert(pq2.getTop() == 30);
    pq2.pop();
    assert(pq2.getTop() == 25);
    pq2.pop();
    assert(pq2.getTop() == 20);
    pq2.pop();
    assert(pq2.getTop() == 10);
    pq2.pop();
    assert(pq2.getTop() == 5);
    pq2.pop();
    assert(pq2.isEmpty());
    cout << "Test 4 passed!\n"
         << endl;

    // Test 5: Range-based constructor with duplicates
    cout << "Test 5: range-based constructor with duplicates..."
         << endl;
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    SortedPQ<int> pq3(arr, arr + 9);
    assert(pq3.getSize() == 9 && "Size should match range");
    assert(pq3.getTop() == 9 && "Top should be max (9)");
    
    // Verify sorted order: [1, 1, 2, 3, 4, 5, 5, 6, 9]
    assert(pq3.at(0) == 1);
    assert(pq3.at(1) == 1);
    assert(pq3.at(2) == 2);
    assert(pq3.at(8) == 9);
    
    cout << "Test 5 passed!\n"
         << endl;



    // Test 6: All equal elements
    cout << "Test 6: all equal elements..."
         << endl;
    SortedPQ<int> pq4;
    for (int i = 0; i < 5; ++i) pq4.push(7);
    assert(pq4.getSize() == 5 && "Size should be 5");
    assert(pq4.getTop() == 7 && "Top should be 7");
    
    // All elements should be 7
    for (std::size_t i = 0; i < 5; ++i) {
        assert(pq4.at(i) == 7);
    }
    
    pq4.pop();
    assert(pq4.getSize() == 4 && "Size should decrease");
    assert(pq4.getTop() == 7 && "Top should still be 7");
    cout << "Test 6 passed!\n"
         << endl;

    // Test 7: Push after pop to zero
    cout << "Test 7: push after pop to zero..."
         << endl;
    SortedPQ<int> pq5;
    pq5.push(1);
    pq5.pop();
    assert(pq5.isEmpty() && "Should be empty after pop");
    pq5.push(2);
    assert(pq5.getTop() == 2 && "New push should work");
    cout << "Test 7 passed!\n"
         << endl;

    // Test 8: Sorted insertion order maintenance
    cout << "Test 8: sorted insertion order maintenance..."
         << endl;
    SortedPQ<int> pq6;
    vector<int> test_vals = {50, 25, 75, 10, 60, 30, 80};
    
    for (int val : test_vals) {
        pq6.push(val);
    }
    
    // Values should be sorted: [10, 25, 30, 50, 60, 75, 80]
    vector<int> expected = {10, 25, 30, 50, 60, 75, 80};
    for (std::size_t i = 0; i < expected.size(); ++i) {
        assert(pq6.at(i) == expected[i]);
    }
    
    cout << "Test 8 passed!\n"
         << endl;

    // Test 9: Large number of elements
    cout << "Test 9: large number of elements..."
         << endl;
    SortedPQ<int> pq7;
    for (int i = 999; i >= 0; --i) pq7.push(i); // Insert in reverse order
    assert(pq7.getSize() == 1000 && "Size should be 1000");
    assert(pq7.getTop() == 999 && "Top should be max (999)");
    
    // First few elements should be 0, 1, 2...
    assert(pq7.at(0) == 0);
    assert(pq7.at(1) == 1);
    assert(pq7.at(2) == 2);
    
    cout << "Test 9 passed!\n"
         << endl;

    cout << "\n\n********** END: Testing " << pqType << " basic functionality (regular version) succeeded! **********\n" << endl;
}



void testSortedPQPtr(SPsPQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (pointer version) **********\n" << endl;

    // Very basic testing
    cout << "Test 1: integer values...\n"
         << endl;
    pq->push(3);
    pq->push(4);
    assert(pq->getSize() == 2);
    assert(pq->getTop() == 4);

    pq->pop();
    assert(pq->getSize() == 1);
    assert(pq->getTop() == 3);
    assert(!pq->isEmpty());

    pq->pop();
    assert(pq->getSize() == 0);
    assert(pq->isEmpty());

    pq->push(18);
    pq->push(81);
    pq->push(42);
    assert(pq->getTop() == 81);
    pq->pop();
    assert(pq->getTop() == 42);
    pq->pop();
    assert(pq->getTop() == 18);
    pq->pop();

    assert(pq->isEmpty());
    cout << "Integer value tests passed!\n"
         << endl;

    // Test with extreme values
    cout << "Testing with extreme values...\n"
         << endl;
    pq->push(INT_MIN);
    pq->push(INT_MAX);
    pq->push(0);
    pq->push(500);
    pq->push(-500);
    assert(pq->getSize() == 5);
    assert(pq->getTop() == INT_MAX);
    pq->pop();
    assert(pq->getTop() == 500);
    pq->pop();
    assert(pq->getTop() == 0);
    pq->pop();
    assert(pq->getTop() == -500);
    pq->pop();
    assert(pq->getTop() == INT_MIN);
    pq->pop();
    assert(pq->isEmpty());
    cout << "Extreme value tests passed!\n"
         << endl;

    // Sorted input
    cout << "Testing with already sorted values...\n"
         << endl;
    for (int i = 1; i <= 10; ++i)
        pq->push(i);
    assert(pq->getSize() == 10);
    assert(pq->getTop() == 10);
    pq->pop();
    assert(pq->getTop() == 9);
    pq->pop();
    while (!pq->isEmpty())
        pq->pop();
    cout << "Sorted value tests passed!\n"
         << endl;

    // Reverse-sorted input
    cout << "Testing with reverse sorted values...\n"
         << endl;
    for (int i = 10; i >= 1; --i)
        pq->push(i);
    assert(pq->getSize() == 10);
    assert(pq->getTop() == 10);
    pq->pop();
    assert(pq->getTop() == 9);
    pq->pop();
    while (!pq->isEmpty())
        pq->pop();
    cout << "Reversed sorted value tests passed!\n"
         << endl;

    cout << "\n\n********** END: Testing " << pqType << " basic functionality (pointer version) succeeded! **********\n" << endl;
}




void testSortedPQHiddenData(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " with hidden data **********\n" << endl;
    
    struct HiddenData {
        int data;
        double dec_data;
        HiddenData(int d) : data(d), dec_data(0.0) {}
        HiddenData(double dec) : data(0), dec_data(dec) {}
    };
    
    struct HiddenDataMaxHeap {
        bool operator()(const HiddenData &a, const HiddenData &b) const
            { return (a.data < b.data); }
    };

    SortedPQ<HiddenData, HiddenDataMaxHeap> pq;

    pq.push(HiddenData(5));
    pq.push(HiddenData(10));
    pq.push(HiddenData(3));

    assert(pq.getSize() == 3);
    assert(pq.getTop().data == 10);

    // Verify sorted order: [3, 5, 10]
    assert(pq.at(0).data == 3);
    assert(pq.at(1).data == 5);
    assert(pq.at(2).data == 10);

    pq.pop();
    assert(pq.getTop().data == 5);
    pq.pop();
    assert(pq.getTop().data == 3);
    pq.pop();
    assert(pq.isEmpty());

    cout << "\n\n********** END: Testing " << pqType << " with hidden data succeeded! **********\n" << endl;
}




void runSortedPQEdgeTests(const string &pqType) 
{
    cout << "\n\n********** START: Testing " << pqType << " edge tests **********\n" << endl;

    // Test 1: updatePQ functionality
    {
        cout << "Test 1: updatePQ functionality\n";
        vector<int> unsorted = {5, 1, 9, 3, 7, 2, 8};
        SortedPQ<int> pq(unsorted.begin(), unsorted.end());
        
        // Should be sorted after construction: [1, 2, 3, 5, 7, 8, 9]
        vector<int> expected = {1, 2, 3, 5, 7, 8, 9};
        for (std::size_t i = 0; i < expected.size(); ++i) {
            assert(pq.at(i) == expected[i]);
        }
        
        assert(pq.getTop() == 9);
        cout << "Test 1 passed!\n"
             << endl;
    }

    // Test 2: Min-heap with custom comparator
    {
        cout << "Test 2: Min-heap with greater comparator\n";
        SortedPQ<int, std::greater<int>> minPQ;
        
        minPQ.push(10);
        minPQ.push(5);
        minPQ.push(15);
        minPQ.push(3);
        
        // For min-heap, should be sorted in descending order: [15, 10, 5, 3]
        // Top (highest priority) should be smallest value (3)
        assert(minPQ.getTop() == 3);
        assert(minPQ.at(0) == 15); // Largest at front
        assert(minPQ.at(3) == 3);  // Smallest at back
        
        cout << "Test 2 passed!\n"
             << endl;
    }

    // Test 3: Insertion maintains order
    {
        cout << "Test 3: Multiple insertions maintain sorted order\n";
        SortedPQ<int> pq;
        vector<int> insertOrder = {50, 25, 75, 12, 60, 30, 85, 5};
        
        for (int val : insertOrder) {
            pq.push(val);
            
            // Verify still sorted after each insertion
            for (std::size_t i = 1; i < pq.getSize(); ++i) {
                assert(pq.at(i-1) <= pq.at(i));
            }
        }
        
        assert(pq.getTop() == 85);
        cout << "Test 3 passed!\n"
             << endl;
    }

    // Test 4: Copy and move semantics
    {
        cout << "Test 4: Copy and move semantics\n";
        
        SortedPQ<int> original;
        original.push(10);
        original.push(20);
        original.push(5);
        
        // Test copy constructor
        SortedPQ<int> copy(original);
        assert(copy.getTop() == original.getTop());
        assert(copy.getSize() == original.getSize());
        
        // Verify both have same sorted order
        for (std::size_t i = 0; i < copy.getSize(); ++i) {
            assert(copy.at(i) == original.at(i));
        }
        
        // Test move constructor
        SortedPQ<int> moved(std::move(copy));
        assert(moved.getSize() == 3);
        assert(moved.getTop() == 20);
        
        cout << "Test 4 passed!\n"
             << endl;
    }

    cout << "\n\n********** END: Testing " << pqType << " edge tests succeeded! **********\n" << endl;
}




















































