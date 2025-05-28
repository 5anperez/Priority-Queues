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

    // Default constructor
    explicit SortedPQ(const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp) {}





    // Range-based constructor
    template <typename Iterator>
    SortedPQ(Iterator begin, Iterator end, const Compare &comp = Compare())
        : SPsPQ<T, Compare>(comp), data(begin, end) 
    { 
        // Sort the data to establish invariant
        std::sort(data.begin(), data.end(), 
            [this](const T& a, const T& b) 
            { 
                return hasLowerPriority(a, b); 
            }
        );
    }


    // Copy constructor
    SortedPQ(const SortedPQ& other) 
        : SPsPQ<T, Compare>(other), data(other.data) {}


    // Move constructor
    SortedPQ(SortedPQ&& other) noexcept
        : SPsPQ<T, Compare>(std::move(other)), data(std::move(other.data)) {}


    // Copy assignment
    SortedPQ& operator=(const SortedPQ& other) {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(other);
            data = other.data;
        }
        return *this;
    }


    // Move assignment
    SortedPQ& operator=(SortedPQ&& other) noexcept {
        if (this != &other) {
            SPsPQ<T, Compare>::operator=(std::move(other));
            data = std::move(other.data);
        }
        return *this;
    }





    virtual ~SortedPQ() = default;


    void push(const T &value) override {
        // Find correct position and insert to maintain sorted order
        std::size_t pos = findInsertPosition(value);
        data.insert(data.begin() + pos, value);
    }



    void pop() override {
        if (isEmpty()) return;
        data.pop_back(); // Remove highest priority element (at back)
    }



    const T &getTop() const override {
        if (isEmpty())
            throw std::runtime_error("Priority queue is empty");
        return data.back(); // Highest priority is at back
    }



    std::size_t getSize() const override { 
        return data.size(); 
    }


    bool isEmpty() const override { 
        return data.empty(); 
    }


    void updatePQ() override 
    { 
        // Re-sort the entire container
        std::sort(data.begin(), data.end(), 
            [this](const T& a, const T& b) 
            { 
                return hasLowerPriority(a, b); 
            }
        );
    }
    
    // Additional method to verify sorted order (for testing)
    bool isSorted() const 
    {
        return std::is_sorted(data.begin(), data.end(),
            [this](const T& a, const T& b) 
            { 
                return hasLowerPriority(a, b); 
            }
        );
    }


private:
    
    std::vector<T> data;
    
    // Helper to check if compareFunctor says a < b (a has lower priority than b)
    bool hasLowerPriority(const T& a, const T& b) const {
        return this->compareFunctor(a, b);
    }

    
    // Find the position where value should be inserted to maintain sorted order
    std::size_t findInsertPosition(const T& value) const 
    {
        // Use binary search to find insertion point
        // We want to find the first element that has higher priority than value
        auto it = std::lower_bound(data.begin(), data.end(), value, 
            [this](const T& a, const T& b) 
            { 
                return hasLowerPriority(a, b); 
            }
        );
            
        return std::distance(data.begin(), it);
    }
};

#endif // SORTED_PQ_H


















































void testSortedPQBasic()
{
    cout << "\n\n********** START: Testing SortedPQ basic functionality **********\n"
         << endl;

    // Test 1: Empty queue operations
    {
        cout << "Test 1: Empty queue operations..." << endl;
        SortedPQ<int> pq;
        assert(pq.isEmpty() && "Empty queue should report empty");
        assert(pq.getSize() == 0 && "Empty queue size should be 0");

        try
        {
            pq.getTop();
            cout << "Test 1 failed: Expected exception on empty getTop" << endl;
            assert(false);
        }
        catch (const std::runtime_error &)
        {
            cout << "Test 1 passed: Empty queue throws on getTop" << endl;
        }

        pq.pop(); // Should not crash on empty queue
        assert(pq.isEmpty() && "Queue should still be empty after pop on empty");
    }



    // Test 2: Single element operations
    {
        cout << "\nTest 2: Single element operations..." << endl;
        SortedPQ<int> pq;
        pq.push(42);
        assert(pq.getSize() == 1 && "Size should be 1");
        assert(pq.getTop() == 42 && "Top should be 42");
        assert(!pq.isEmpty() && "Queue should not be empty");

        pq.pop();
        assert(pq.isEmpty() && "Queue should be empty after pop");

        // Push after emptying
        pq.push(100);
        assert(pq.getTop() == 100 && "Top should be 100");
        cout << "Test 2 passed!" << endl;
    }



    // Test 3: Multiple elements in ascending order
    {
        cout << "\nTest 3: Multiple elements - ascending order push..." << endl;
        SortedPQ<int> pq;
        for (int i = 1; i <= 5; ++i)
        {
            pq.push(i);
            assert(pq.getTop() == i && "Top should be current max");
        }
        assert(pq.getSize() == 5 && "Size should be 5");

        // Pop all and verify order
        for (int i = 5; i >= 1; --i)
        {
            assert(pq.getTop() == i && "Top should match expected");
            pq.pop();
        }
        assert(pq.isEmpty() && "Queue should be empty");
        cout << "Test 3 passed!" << endl;
    }



    // Test 4: Multiple elements in descending order
    {
        cout << "\nTest 4: Multiple elements - descending order push..." << endl;
        SortedPQ<int> pq;
        for (int i = 5; i >= 1; --i)
        {
            pq.push(i);
            assert(pq.getTop() == 5 && "Top should always be 5");
        }
        assert(pq.getSize() == 5 && "Size should be 5");
        cout << "Test 4 passed!" << endl;
    }



    // Test 5: Random order insertions
    {
        cout << "\nTest 5: Random order insertions..." << endl;
        SortedPQ<int> pq;
        vector<int> values = {3, 1, 4, 1, 5, 9, 2, 6, 5};

        for (int val : values)
        {
            pq.push(val);
        }
        assert(pq.getSize() == 9 && "Size should be 9");
        assert(pq.getTop() == 9 && "Top should be 9");

        // Pop and verify descending order
        int prev = pq.getTop();
        pq.pop();
        int count = 1;

        while (!pq.isEmpty())
        {
            assert(pq.getTop() <= prev && "Elements should come out in descending order");
            prev = pq.getTop();
            pq.pop();
            count++;
        }
        assert(count == 9 && "Should have popped 9 elements");
        cout << "Test 5 passed!" << endl;
    }



    // Test 6: Duplicate elements
    {
        cout << "\nTest 6: Duplicate elements..." << endl;
        SortedPQ<int> pq;
        for (int i = 0; i < 5; ++i)
        {
            pq.push(7);
        }
        assert(pq.getSize() == 5 && "Size should be 5");
        assert(pq.getTop() == 7 && "Top should be 7");

        pq.pop();
        assert(pq.getSize() == 4 && "Size should decrease");
        assert(pq.getTop() == 7 && "Top should still be 7");

        while (!pq.isEmpty())
        {
            assert(pq.getTop() == 7 && "All elements should be 7");
            pq.pop();
        }
        cout << "Test 6 passed!" << endl;
    }



    // Test 7: Range-based constructor
    {
        cout << "\nTest 7: Range-based constructor..." << endl;
        vector<int> vec = {5, 2, 8, 1, 9, 3, 7, 4, 6};
        SortedPQ<int> pq(vec.begin(), vec.end());

        assert(pq.getSize() == 9 && "Size should match input");
        assert(pq.getTop() == 9 && "Top should be max element");
        assert(pq.isSorted() && "Internal data should be sorted");

        // Verify all elements come out in descending order
        for (int i = 9; i >= 1; --i)
        {
            assert(pq.getTop() == i && "Elements should be in order");
            pq.pop();
        }
        cout << "Test 7 passed!" << endl;
    }



    // Test 8: updatePQ functionality
    {
        cout << "\nTest 8: updatePQ functionality..." << endl;
        vector<int> vec = {3, 1, 4, 1, 5};
        SortedPQ<int> pq(vec.begin(), vec.end());

        assert(pq.getTop() == 5 && "Top should be 5");

        // Call updatePQ (should maintain sorted order)
        pq.updatePQ();
        assert(pq.getTop() == 5 && "Top should still be 5 after updatePQ");
        assert(pq.isSorted() && "Should remain sorted after updatePQ");
        cout << "Test 8 passed!" << endl;
    }

    // Test 9: Min-heap behavior with greater comparator
    {
        cout << "\nTest 9: Min-heap with greater comparator..." << endl;
        SortedPQ<int, std::greater<int>> minPq;

        minPq.push(5);
        minPq.push(1);
        minPq.push(10);
        minPq.push(3);

        assert(minPq.getTop() == 1 && "Top should be minimum (1)");
        minPq.pop();
        assert(minPq.getTop() == 3 && "Next top should be 3");
        minPq.pop();
        assert(minPq.getTop() == 5 && "Next top should be 5");
        cout << "Test 9 passed!" << endl;
    }



    // Test 10: Copy and move operations
    {
        cout << "\nTest 10: Copy and move operations..." << endl;
        SortedPQ<int> original;
        original.push(10);
        original.push(20);
        original.push(15);

        // Test copy constructor
        SortedPQ<int> copy(original);
        assert(copy.getSize() == original.getSize() && "Sizes should match");
        assert(copy.getTop() == original.getTop() && "Tops should match");

        // Modify copy shouldn't affect original
        copy.pop();
        assert(copy.getSize() == original.getSize() - 1 && "Only copy size should change");
        assert(original.getTop() == 20 && "Original should be unchanged");

        // Test move constructor
        SortedPQ<int> moved(std::move(copy));
        assert(moved.getSize() == 2 && "Moved should have 2 elements");
        assert(moved.getTop() == 15 && "Moved top should be 15");

        // Test assignment
        SortedPQ<int> assigned;
        assigned = original;
        assert(assigned.getTop() == original.getTop() && "Assignment should copy correctly");
        cout << "Test 10 passed!" << endl;
    }

    cout << "\n\n********** END: Testing SortedPQ basic functionality succeeded! **********\n"
         << endl;
}



void testSortedPQEdgeCases()
{
    cout << "\n\n********** START: Testing SortedPQ edge cases **********\n"
         << endl;


    // Test 1: Large dataset performance
    {
        cout << "Test 1: Large dataset (10000 elements)..." << endl;
        SortedPQ<int> pq;
        const int N = 10000;

        // Push in random order
        vector<int> values;
        for (int i = 0; i < N; ++i)
        {
            int val = rand() % 1000;
            values.push_back(val);
            pq.push(val);
        }


        // Verify sorted property maintained
        assert(pq.isSorted() && "Should maintain sorted order");


        // Pop all and verify descending order
        int prev = INT_MAX;
        while (!pq.isEmpty())
        {
            int curr = pq.getTop();
            assert(curr <= prev && "Should maintain descending order");
            prev = curr;
            pq.pop();
        }
        cout << "Test 1 passed!" << endl;
    }



    // Test 2: Alternating push/pop pattern
    {
        cout << "\nTest 2: Alternating push/pop pattern..." << endl;
        SortedPQ<int> pq;

        for (int i = 0; i < 20; ++i)
        {
            pq.push(i);
            if (i % 2 == 1)
            {
                pq.pop();
            }
        }

        assert(pq.getSize() == 10 && "Should have 10 elements");
        assert(pq.getTop() == 18 && "Top should be 18");
        cout << "Test 2 passed!" << endl;
    }



    // Test 3: Custom comparator with complex logic
    {
        cout << "\nTest 3: Custom comparator (even numbers prioritized)..." << endl;

        auto evenFirst = [](int a, int b)
        {
            if (a % 2 != b % 2)
            {
                return (a % 2) > (b % 2); // odd > even in comparison
            }
            return a < b;
        };

        SortedPQ<int, decltype(evenFirst)> pq(evenFirst);
        vector<int> values = {1, 2, 3, 4, 5, 6};

        for (int val : values)
        {
            pq.push(val);
        }

        // Should get evens in descending order, then odds
        assert(pq.getTop() == 6 && "Top should be 6 (largest even)");
        pq.pop();
        assert(pq.getTop() == 4 && "Next should be 4");
        pq.pop();
        assert(pq.getTop() == 2 && "Next should be 2");
        pq.pop();
        assert(pq.getTop() == 5 && "Next should be 5 (largest odd)");

        cout << "Test 3 passed!" << endl;
    }

    cout << "\n\n********** END: Testing SortedPQ edge cases succeeded! **********\n"
         << endl;
}
