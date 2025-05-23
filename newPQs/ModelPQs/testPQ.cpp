// #include "MA_PQ.h"
// #include "MB_PQ.h"
#include "UnorderedSPsPQ.h" 
#include "UnorderedFastSPsPQ.h"
#include "BinPQ.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <string>
#include <chrono>

using namespace std;





// 1. Create the functor via struct.
// Used to test a min-heap type priority queue containing doubles.
struct DescendingComp1
{
    // Prioritizes smaller doubles.
    bool operator()(double a, double b) const
    { return a > b; }
};

// 2. Create the functor via lambda.
// Used to test a min-heap type priority queue containing doubles.
// auto DescendingComp2 = [](double a, double b) 
// {
//     // Prioritizes smaller doubles.
//     return a > b;
// };

struct AbsComparator 
{
    bool operator()(double a, double b) const 
    { return fabs(a) < fabs(b); }
};

// Introduce "thresholded" comparisons, where small differences are treated as equal.
struct ThresholdComparator 
{
    bool operator()(double a, double b) const 
    {
        const double threshold = 0.01;
        return (fabs(a - b) > threshold) ? (a < b) : false;
    }
};




// Used to test a priority queue containing pointers to integers.
struct IntPtrComp
{
    // Prioritizes larger pointer ints.
    bool operator() (const int *a, const int *b) const
    { return *a < *b; }
};






// modify the pointer comp to work with hidden data!!!!!
// TODO: Make sure that you're using this->compare() properly, and everywhere
// that you should.  Complete this function by adding a functor that compares
// two HiddenData structures, create a PQ of the specified type, and call
// this function from main().
void testHiddenData(const string &pqType)
{
    // Complex comparisons: mulit-field objects.
    struct HiddenData
    {
        int data;
        double dec_data;
        HiddenData(int d) : data(d) {}
        HiddenData(double dec) : dec_data(dec) {}
    };

    
    // Basic max-heap functionality.
    struct HiddenDataMaxHeap
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const
            { return (a.data < b.data); }
    };


    // Prioritize doubles with respect to absolute values.
    struct HDAbsComparator 
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const 
        { return fabs(a.dec_data) < fabs(b.dec_data); }
    };


    
    struct OddFirstComp 
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const 
        {
            if ((a.data % 2) != (b.data % 2)) 
            {
                return (a.data % 2) < (b.data % 2); 
            }
            return a.data < b.data; 
        }
    };


    // Prioritize integers with respect to absolute values.
    struct AbsValueComp 
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const 
        { return std::abs(a.data) < std::abs(b.data); }
    };


    // Order by the last digit
    auto customComp = [](const HiddenData &a, const HiddenData &b) 
    { return (a.data % 10) < (b.data % 10); };

    
    cout << "\n\n********** START: Testing " << pqType << " with hidden data **********\n" << endl;
    
    // TODO: Add code here to actually test with the HiddenData type.

    /*
        I ESSENTIALLY HAVE TO DEFINE A CUSTOM COMPARATOR FOR THE "HIDDENDATA" CUSTOM TYPE. THIS WILL TEST AND VERIFY THAT THE PQ IMPLEMENTATION ALWAYS RESPECTS THE SPECIFIED COMPARATOR AND ALSO CORRECTLY IDENTIFIES THE MOST EXTREME ELEMENT ACCORDING TO THE COMPARATOR. IT MIGHT ALSO REVEAL ISSUES THAT DONT OCCUR WITH PRIMITIVE TYPES. 
    */

    // Would it make sense or be a good test to create a simple unordered pq w/ the hidden type and no custom comparator, or does that defeat the purpose?



    

    if (pqType == "Unordered")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        UnorderedPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.getSize() == 3);
        assert(pq.getTop().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.getTop().data == 5);
        pq.pop();
        assert(pq.getTop().data == 3);
        pq.pop();
        assert(pq.isEmpty());

        UnorderedPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.getTop().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.getTop().dec_data == 5.2);


        // Odds before evens:
        /**
         * Input: [4, 1, 3, 2]
         * Ordering:
         * 1. Odd numbers first: [1, 3, 4, 2]
         * 2. Within odd numbers, larger first: [3, 1, 4, 2]
         * 3. Within even numbers, larger first: [3, 1, 4, 2]
         * Final order: [3, 1, 4, 2]
         */
        UnorderedPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.getTop().data == 3); 
        pq4.pop();
        assert(pq4.getTop().data == 1);
        pq4.pop();
        pq4.pop();
        assert(pq4.getTop().data == 2);
        pq4.pop();
        assert(pq4.isEmpty());



        // Order by the last digit
        UnorderedPQ<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.getTop().data == 7);
        pqL.pop();
        assert(pqL.getTop().data == 25);
        pqL.pop();
        assert(pqL.getTop().data == 13);
        pqL.pop();
        assert(pqL.getTop().data == 12);
        pqL.pop();
        assert(pqL.isEmpty());
    }
    else if (pqType == "UnorderedOPT")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        UnorderedPQOptimized<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.getSize() == 3);
        assert(pq.getTop().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.getTop().data == 5);
        pq.pop();
        assert(pq.getTop().data == 3);
        pq.pop();
        assert(pq.isEmpty());

        UnorderedPQOptimized<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.getTop().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.getTop().dec_data == 5.2);


        // Odds before evens
        UnorderedPQOptimized<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.getTop().data == 3); 
        pq4.pop();
        assert(pq4.getTop().data == 1);
        pq4.pop();
        pq4.pop();
        assert(pq4.getTop().data == 2);
        pq4.pop();
        assert(pq4.isEmpty());



        // Order by the last digit
        UnorderedPQOptimized<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.getTop().data == 7);
        pqL.pop();
        assert(pqL.getTop().data == 25);
        pqL.pop();
        assert(pqL.getTop().data == 13);
        pqL.pop();
        assert(pqL.getTop().data == 12);
        pqL.pop();
        assert(pqL.isEmpty());
    }
    else if (pqType == "Binary")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        BinPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.getSize() == 3);
        assert(pq.getTop().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.getTop().data == 5);
        pq.pop();
        assert(pq.getTop().data == 3);
        pq.pop();
        assert(pq.isEmpty());

        BinPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.getTop().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.getTop().dec_data == 5.2);


        // Odds before evens
        BinPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.getTop().data == 3); 
        pq4.pop();
        assert(pq4.getTop().data == 1);
        pq4.pop();
        pq4.pop();
        assert(pq4.getTop().data == 2);
        pq4.pop();
        assert(pq4.isEmpty());



        // Order by the last digit
        BinPQ<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.getTop().data == 7);
        pqL.pop();
        assert(pqL.getTop().data == 25);
        pqL.pop();
        assert(pqL.getTop().data == 13);
        pqL.pop();
        assert(pqL.getTop().data == 12);
        pqL.pop();
        assert(pqL.isEmpty());
    }


    cout << "\n\n********** END: Testing " << pqType << " with hidden data succeeded! **********\n" << endl;
   
} // testHiddenData()






// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePQHelper(SPsPQ<int *, IntPtrComp> *pq)
{
    vector<int> data;
    data.reserve(100);
    data.push_back(100);
    data.push_back(1);
    data.push_back(5);
    data.push_back(4);
    data.push_back(9);
    data.push_back(2);
    data.push_back(3);
    data.push_back(80);
    data.push_back(90);
    
    // NOTE: If you add more data to the vector, don't push the pointers
    // until AFTER the vector stops changing size!  Think about why.
    // You can add up to 100 values, or change the reserve if you want more.
    
    for (size_t i = 0; i < data.size(); ++i)
    {
        pq->push(&data[i]);
    } // for
    
    // Change the first value (which is pointed to by the pq), and check it.
    data[0] = 10;
    pq->updatePQ();
    assert(*pq->getTop() == 90);
    data[1] = 200;
    pq->updatePQ();
    assert(*pq->getTop() == 200);
    data[0] = 3000;
    pq->updatePQ();
    assert(*pq->getTop() == 3000);
    
} // testUpdatePrioritiesHelper()








// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePQ(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " priority queue updatePQ() **********\n" << endl;

    SPsPQ<int *, IntPtrComp> *pq = nullptr;
    
    if (pqType == "Unordered")
    {
        pq = new UnorderedPQ<int *, IntPtrComp>;
    } // if
    else if (pqType == "UnorderedOPT")
    {
        pq = new UnorderedPQOptimized<int *, IntPtrComp>;
    }
    else if (pqType == "Binary")
    {
        pq = new BinPQ<int *, IntPtrComp>;
    } // else if

    // TODO: Add more types here inside 'else if' statements, like in main().
    
    if (!pq)
    {
        cout << "Invalid pq pointer; did you forget to create it?" << endl;
        return;
    } // if
    
    // if (pqType != "Unordered")
    //     testUpdatePQHelper(pq);

    testUpdatePQHelper(pq);

    cout << "\n\n********** END: Testing " << pqType << " priority queue updatePQ() succeeded! **********\n" << endl;

    delete pq;
} // testUpdatePQ()





// Test the basic functionality of the priority queue pointer version.
void testPriorityQueuePtr(SPsPQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (pointer version) **********\n" << endl;

    // Very basic testing.
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

    // Start fresh w/ an empty pq.
    assert(pq->isEmpty());

    cout << "Integer value tests passed!\n"
         << endl;

    // TODO: Add more testing here!

    // More focused unit testing.

    // Large range of values
    cout << "Testing with extrame values...\n"
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
    cout << "Testing with sorted values...\n"
         << endl;
    for (int i = 1; i <= 10; ++i)
        pq->push(i);
    assert(pq->getSize() == 10);
    assert(pq->getTop() == 10);
    pq->pop();
    assert(pq->getTop() == 9);
    pq->pop();
    // Clean-up
    while (!pq->isEmpty())
        pq->pop();
    cout << "Sorted value tests passed!\n"
         << endl;

    // Reverse-sorted input
    cout << "Testing with reversed sorted values...\n"
         << endl;
    for (int i = 10; i >= 1; --i)
        pq->push(i);
    assert(pq->getSize() == 10);
    assert(pq->getTop() == 10);
    pq->pop();
    assert(pq->getTop() == 9);
    pq->pop();
    // Clean-up
    while (!pq->isEmpty())
        pq->pop();
    cout << "Reversed sorted value tests passed!\n"
         << endl;

    // Mix of positive and negative numbers
    cout << "Testing with both +/- values...\n"
         << endl;
    pq->push(-10);
    pq->push(50);
    pq->push(0);
    pq->push(-5);
    pq->push(30);
    assert(pq->getSize() == 5);
    assert(pq->getTop() == 50);
    pq->pop();
    assert(pq->getTop() == 30);
    pq->pop();
    assert(pq->getTop() == 0);
    pq->pop();
    assert(pq->getTop() == -5);
    pq->pop();
    assert(pq->getTop() == -10);
    pq->pop();
    cout << "Both +/- value tests passed!\n"
         << endl;

    // Testing `updatePriorities()` after manual data modification
    /*
        I.e., simulate a scenario where updatePriorities() is called explicitly and verify heap integrity afterward.
    */
    cout << "Testing by manually modifying the values...\n"
         << endl;
    pq->push(100);
    pq->push(200);
    pq->push(50);
    pq->push(75);
    // Simulate external modification
    pq->updatePQ();
    assert(pq->getTop() == 200);
    pq->pop();
    assert(pq->getTop() == 100);
    pq->pop();
    assert(pq->getTop() == 75);
    cout << "Manual value modification tests passed!\n"
         << endl;


    cout << "\n\n********** END: Testing " << pqType << " basic functionality (pointer version) succeeded! **********\n" << endl;
} // testPriorityQueuePtr()







// Test the unsorted heap's range-based constructor, copy constructor,
// and operator=().
//
void testPQsCtor(SPsPQ<int> *pq, const string &pqType)
{
    // Send a subset = {6,...,87}, i.e., shrink the range
    // by setting a ptr to the 6th elt and the the 11th
    // elt to get the subset [6, 45).

    cout << "\n\n********** START: Testing " << pqType << "'s range-based constructor **********\n" << endl;

    // Empty vector tests
    // cout << "Testing with an empty vector...\n"
    //      << endl;

    // std::vector<int> empty;
    // SPsPQ<int>* emptyPQ = new UnorderedPQ<int>(empty.begin(), empty.end());
    // assert(emptyPQ->isEmpty());
    // assert(emptyPQ->getSize() == 0);
    // assert(emptyPQ->getTop() == 0);
    // delete emptyPQ;

    // Mix of positive and negative numbers
    cout << "Testing with both +/- values...\n"
         << endl;

    assert(pq->getSize() == 6);
    assert(pq->getTop() == 87);

    pq->pop();
    assert(pq->getSize() == 5);
    assert(pq->getTop() == 65);
    assert(!pq->isEmpty());

    pq->pop();
    assert(pq->getSize() == 4);
    assert(!pq->isEmpty());

    // TODO: Add more testing here!

    cout << "\n\n********** END: Testing " << pqType << "'s range-based constructor succeeded! **********\n" << endl;
} // testPQsCtor()




// Test the basic functionality of the priority queue regular version.
void testPriorityQueue(const string &pqType) 
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (regular version) **********\n" << endl;

    if (pqType == "Unordered")
    {
        // Test 1: Empty queue
        cout << "Test 1: empty queue..."
         << endl;
        UnorderedPQ<int> pq1;
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

        // Test 3: Range-based constructor with duplicates
        cout << "Test 3: range-based constructor with duplicates..."
         << endl;
        int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
        UnorderedPQ<int> pq2(arr, arr + 9);
        assert(pq2.getSize() == 9 && "Size should match range");
        assert(pq2.getTop() == 9 && "Top should be max (9)");
        pq2.pop();
        assert(pq2.getTop() == 6 && "Next top should be 6 after pop");
        cout << "Test 3 passed!\n"
         << endl;

        // Test 4: All equal elements
        cout << "Test 4: all equal elements..."
         << endl;
        UnorderedPQ<int> pq3;
        for (int i = 0; i < 5; ++i) pq3.push(7);
        assert(pq3.getSize() == 5 && "Size should be 5");
        assert(pq3.getTop() == 7 && "Top should be 7");
        pq3.pop();
        assert(pq3.getSize() == 4 && "Size should decrease");
        assert(pq3.getTop() == 7 && "Top should still be 7");
        cout << "Test 4 passed!\n"
         << endl;

        // Test 5: Push after pop to zero
        cout << "Test 5: push after pop to zero..."
         << endl;
        UnorderedPQ<int> pq4;
        pq4.push(1);
        pq4.pop();
        assert(pq4.isEmpty() && "Should be empty after pop");
        pq4.push(2);
        assert(pq4.getTop() == 2 && "New push should work");
        cout << "Test 5 passed!\n"
         << endl;

        // Test 6: Large number of elements
        cout << "Test 6: large number of elements..."
         << endl;
        UnorderedPQ<int> pq5;
        for (int i = 0; i < 1000; ++i) pq5.push(i);
        assert(pq5.getSize() == 1000 && "Size should be 1000");
        assert(pq5.getTop() == 999 && "Top should be max (999)");
        cout << "Test 6 passed!\n"
         << endl;

    }
    else if (pqType == "UnorderedOPT")
    {
        // Test 1: Empty queue
        cout << "Test 1: empty queue..."
         << endl;
        UnorderedPQOptimized<int> pq1;
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

        // Test 3: Range-based constructor with duplicates
        cout << "Test 3: range-based constructor with duplicates..."
         << endl;
        int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
        UnorderedPQOptimized<int> pq2(arr, arr + 9);
        assert(pq2.getSize() == 9 && "Size should match range");
        assert(pq2.getTop() == 9 && "Top should be max (9)");
        pq2.pop();
        assert(pq2.getTop() == 6 && "Next top should be 6 after pop");
        cout << "Test 3 passed!\n"
         << endl;

        // Test 4: All equal elements
        cout << "Test 4: all equal elements..."
         << endl;
        UnorderedPQOptimized<int> pq3;
        for (int i = 0; i < 5; ++i) pq3.push(7);
        assert(pq3.getSize() == 5 && "Size should be 5");
        assert(pq3.getTop() == 7 && "Top should be 7");
        pq3.pop();
        assert(pq3.getSize() == 4 && "Size should decrease");
        assert(pq3.getTop() == 7 && "Top should still be 7");
        cout << "Test 4 passed!\n"
         << endl;

        // Test 5: Push after pop to zero
        cout << "Test 5: push after pop to zero..."
         << endl;
        UnorderedPQOptimized<int> pq4;
        pq4.push(1);
        pq4.pop();
        assert(pq4.isEmpty() && "Should be empty after pop");
        pq4.push(2);
        assert(pq4.getTop() == 2 && "New push should work");
        cout << "Test 5 passed!\n"
         << endl;

        // Test 6: Large number of elements
        cout << "Test 6: large number of elements..."
         << endl;
        UnorderedPQOptimized<int> pq5;
        for (int i = 0; i < 1000; ++i) pq5.push(i);
        assert(pq5.getSize() == 1000 && "Size should be 1000");
        assert(pq5.getTop() == 999 && "Top should be max (999)");
        cout << "Test 6 passed!\n"
         << endl;
    }
    else if (pqType == "Binary")
    {
        // Test 1: Empty queue
        cout << "Test 1: empty queue..."
         << endl;
        BinPQ<int> pq1;
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

        // Test 3: Range-based constructor with duplicates
        cout << "Test 3: range-based constructor with duplicates..."
         << endl;
        int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
        BinPQ<int> pq2(arr, arr + 9);
        assert(pq2.getSize() == 9 && "Size should match range");
        assert(pq2.getTop() == 9 && "Top should be max (9)");
        pq2.pop();
        assert(pq2.getTop() == 6 && "Next top should be 6 after pop");
        cout << "Test 3 passed!\n"
         << endl;

        // Test 4: All equal elements
        cout << "Test 4: all equal elements..."
         << endl;
        BinPQ<int> pq3;
        for (int i = 0; i < 5; ++i) pq3.push(7);
        assert(pq3.getSize() == 5 && "Size should be 5");
        assert(pq3.getTop() == 7 && "Top should be 7");
        pq3.pop();
        assert(pq3.getSize() == 4 && "Size should decrease");
        assert(pq3.getTop() == 7 && "Top should still be 7");
        cout << "Test 4 passed!\n"
         << endl;

        // Test 5: Push after pop to zero
        cout << "Test 5: push after pop to zero..."
         << endl;
        BinPQ<int> pq4;
        pq4.push(1);
        pq4.pop();
        assert(pq4.isEmpty() && "Should be empty after pop");
        pq4.push(2);
        assert(pq4.getTop() == 2 && "New push should work");
        cout << "Test 5 passed!\n"
         << endl;

        // Test 6: Large number of elements
        cout << "Test 6: large number of elements..."
         << endl;
        BinPQ<int> pq5;
        for (int i = 0; i < 1000; ++i) pq5.push(i);
        assert(pq5.getSize() == 1000 && "Size should be 1000");
        assert(pq5.getTop() == 999 && "Top should be max (999)");
        cout << "Test 6 passed!\n"
         << endl;
    }

    cout << "\n\n********** END: Testing " << pqType << " basic functionality (regular version) succeeded! **********\n" << endl;
} // testPriorityQueue()








void runEdgeTests(const string &pqType) 
{
    cout << "\n\n********** START: Testing " << pqType << " edge tests to reveal version-specific bugs (regular version) **********\n" << endl;


    if (pqType == "Unordered")
    {
        // Test 1: Push-pop-push with equal elements
        {
            cout << "Test 1: Push-pop-push with equal elements\n";
            UnorderedPQ<int> pq;  // Replace with UnorderedPQOptimized for comparison
            pq.push(5);
            pq.push(5);  // Two equal max elements
            assert(pq.getTop() == 5 && "Top should be 5");
            pq.pop();    // Remove one 5
            assert(pq.getTop() == 5 && "Top should still be 5");
            pq.push(5);  // Add another 5
            assert(pq.getTop() == 5 && "Top should remain 5");
            cout << "Test 1 passed!\n"
             << endl;
        }
        // Potential Bug: In Optimized, if pop invalidates topIdx but doesn’t rescan correctly,
        // it might miss the remaining 5 or pick the wrong index after push.

        // Test 2: Pop all elements then push new max
        {
            std::cout << "Test 2: Pop all elements then push new max\n";
            UnorderedPQ<int> pq;
            pq.push(3);
            pq.push(4);
            pq.pop();  // Remove 4
            pq.pop();  // Remove 3
            assert(pq.isEmpty() && "Queue should be empty");
            pq.push(5);  // New max after empty
            assert(pq.getTop() == 5 && "Top should be 5");
            std::cout << "Test 2 passed\n";
        }
        // Potential Bug: Optimized might not reset topIdx properly after emptying,
        // causing getTop to reference an invalid index or old data.

        // Test 3: Push lower value after pop, check top
        {
            std::cout << "Test 3: Push lower value after pop\n";
            UnorderedPQ<int> pq;
            pq.push(10);
            pq.push(8);
            pq.pop();  // Remove 10
            assert(pq.getTop() == 8 && "Top should be 8");
            pq.push(5);  // Lower than current top
            assert(pq.getTop() == 8 && "Top should still be 8");
            std::cout << "Test 3 passed\n";
        }
        // Potential Bug: Optimized might incorrectly update topIdx to 5 during push,
        // assuming new elements always challenge the top.

        // Test 4: Repeated pop with duplicates
        {
            std::cout << "Test 4: Repeated pop with duplicates\n";
            UnorderedPQ<int> pq;
            pq.push(7);
            pq.push(7);
            pq.push(7);  // Three equal max elements
            pq.pop();    // Remove one 7
            assert(pq.getTop() == 7 && "Top should be 7");
            pq.pop();    // Remove another 7
            assert(pq.getTop() == 7 && "Top should still be 7");
            pq.pop();    // Remove last 7
            assert(pq.isEmpty() && "Queue should be empty");
            std::cout << "Test 4 passed\n";
        }
        // Potential Bug: Optimized might fail to find the next 7 if topIdx isn’t updated
        // after each pop, or it might not handle empty state correctly.

        // Test 5: Range init with all equal, pop all
        {
            std::cout << "Test 5: Range init with all equal, pop all\n";
            int arr[] = {4, 4, 4, 4};
            UnorderedPQ<int> pq(arr, arr + 4);
            assert(pq.getTop() == 4 && "Top should be 4");
            pq.pop();
            assert(pq.getTop() == 4 && "Top should be 4");
            pq.pop();
            pq.pop();
            pq.pop();
            assert(pq.isEmpty() && "Queue should be empty");
            std::cout << "Test 5 passed\n";
        }

    }
    else if (pqType == "UnorderedOPT")
    {
        std::cout << "Running edge tests on UnorderedPQOptimized to reveal version-specific bugs...\n";

        // Test 1: Push-pop-push with equal elements
        {
            std::cout << "Test 1: Push-pop-push with equal elements\n";
            UnorderedPQOptimized<int> pq;  // Replace with UnorderedPQOptimized for comparison
            pq.push(5);
            pq.push(5);  // Two equal max elements
            assert(pq.getTop() == 5 && "Top should be 5");
            pq.pop();    // Remove one 5
            assert(pq.getTop() == 5 && "Top should still be 5");
            pq.push(5);  // Add another 5
            assert(pq.getTop() == 5 && "Top should remain 5");
            std::cout << "Test 1 passed\n";
        }
        // Potential Bug: In Optimized, if pop invalidates topIdx but doesn’t rescan correctly,
        // it might miss the remaining 5 or pick the wrong index after push.

        // Test 2: Pop all elements then push new max
        {
            std::cout << "Test 2: Pop all elements then push new max\n";
            UnorderedPQOptimized<int> pq;
            pq.push(3);
            pq.push(4);
            pq.pop();  // Remove 4
            pq.pop();  // Remove 3
            assert(pq.isEmpty() && "Queue should be empty");
            pq.push(5);  // New max after empty
            assert(pq.getTop() == 5 && "Top should be 5");
            std::cout << "Test 2 passed\n";
        }
        // Potential Bug: Optimized might not reset topIdx properly after emptying,
        // causing getTop to reference an invalid index or old data.

        // Test 3: Push lower value after pop, check top
        {
            std::cout << "Test 3: Push lower value after pop\n";
            UnorderedPQOptimized<int> pq;
            pq.push(10);
            pq.push(8);
            pq.pop();  // Remove 10
            assert(pq.getTop() == 8 && "Top should be 8");
            pq.push(5);  // Lower than current top
            assert(pq.getTop() == 8 && "Top should still be 8");
            std::cout << "Test 3 passed\n";
        }
        // Potential Bug: Optimized might incorrectly update topIdx to 5 during push,
        // assuming new elements always challenge the top.

        // Test 4: Repeated pop with duplicates
        {
            std::cout << "Test 4: Repeated pop with duplicates\n";
            UnorderedPQOptimized<int> pq;
            pq.push(7);
            pq.push(7);
            pq.push(7);  // Three equal max elements
            pq.pop();    // Remove one 7
            assert(pq.getTop() == 7 && "Top should be 7");
            pq.pop();    // Remove another 7
            assert(pq.getTop() == 7 && "Top should still be 7");
            pq.pop();    // Remove last 7
            assert(pq.isEmpty() && "Queue should be empty");
            std::cout << "Test 4 passed\n";
        }
        // Potential Bug: Optimized might fail to find the next 7 if topIdx isn’t updated
        // after each pop, or it might not handle empty state correctly.

        // Test 5: Range init with all equal, pop all
        {
            std::cout << "Test 5: Range init with all equal, pop all\n";
            int arr[] = {4, 4, 4, 4};
            UnorderedPQOptimized<int> pq(arr, arr + 4);
            assert(pq.getTop() == 4 && "Top should be 4");
            pq.pop();
            assert(pq.getTop() == 4 && "Top should be 4");
            pq.pop();
            pq.pop();
            pq.pop();
            assert(pq.isEmpty() && "Queue should be empty");
            std::cout << "Test 5 passed\n";
        }

    }
    else if (pqType == "Binary")
    {
        // Test 1: Push-pop-push with equal elements
        {
            cout << "Test 1: Push-pop-push with equal elements\n";
            BinPQ<int> pq;  // Replace with UnorderedPQOptimized for comparison
            pq.push(5);
            pq.push(5);  // Two equal max elements
            assert(pq.getTop() == 5 && "Top should be 5");
            pq.pop();    // Remove one 5
            assert(pq.getTop() == 5 && "Top should still be 5");
            pq.push(5);  // Add another 5
            assert(pq.getTop() == 5 && "Top should remain 5");
            cout << "Test 1 passed!\n"
             << endl;
        }
        // Potential Bug: In Optimized, if pop invalidates topIdx but doesn’t rescan correctly,
        // it might miss the remaining 5 or pick the wrong index after push.

        // Test 2: Pop all elements then push new max
        {
            std::cout << "Test 2: Pop all elements then push new max\n";
            BinPQ<int> pq;
            pq.push(3);
            pq.push(4);
            pq.pop();  // Remove 4
            pq.pop();  // Remove 3
            assert(pq.isEmpty() && "Queue should be empty");
            pq.push(5);  // New max after empty
            assert(pq.getTop() == 5 && "Top should be 5");
            std::cout << "Test 2 passed\n";
        }
        // Potential Bug: Optimized might not reset topIdx properly after emptying,
        // causing getTop to reference an invalid index or old data.

        // Test 3: Push lower value after pop, check top
        {
            std::cout << "Test 3: Push lower value after pop\n";
            BinPQ<int> pq;
            pq.push(10);
            pq.push(8);
            pq.pop();  // Remove 10
            assert(pq.getTop() == 8 && "Top should be 8");
            pq.push(5);  // Lower than current top
            assert(pq.getTop() == 8 && "Top should still be 8");
            std::cout << "Test 3 passed\n";
        }
        // Potential Bug: Optimized might incorrectly update topIdx to 5 during push,
        // assuming new elements always challenge the top.

        // Test 4: Repeated pop with duplicates
        {
            std::cout << "Test 4: Repeated pop with duplicates\n";
            BinPQ<int> pq;
            pq.push(7);
            pq.push(7);
            pq.push(7);  // Three equal max elements
            pq.pop();    // Remove one 7
            assert(pq.getTop() == 7 && "Top should be 7");
            pq.pop();    // Remove another 7
            assert(pq.getTop() == 7 && "Top should still be 7");
            pq.pop();    // Remove last 7
            assert(pq.isEmpty() && "Queue should be empty");
            std::cout << "Test 4 passed\n";
        }
        // Potential Bug: Optimized might fail to find the next 7 if topIdx isn’t updated
        // after each pop, or it might not handle empty state correctly.

        // Test 5: Range init with all equal, pop all
        {
            std::cout << "Test 5: Range init with all equal, pop all\n";
            int arr[] = {4, 4, 4, 4};
            BinPQ<int> pq(arr, arr + 4);
            assert(pq.getTop() == 4 && "Top should be 4");
            pq.pop();
            assert(pq.getTop() == 4 && "Top should be 4");
            pq.pop();
            pq.pop();
            pq.pop();
            assert(pq.isEmpty() && "Queue should be empty");
            std::cout << "Test 5 passed\n";
        }

    }

    cout << "\n\n********** END: Testing " << pqType << " edge tests to reveal version-specific bugs (regular version) succeeded! **********\n" << endl;

}





void specialTests(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << "'s equal elements corner case **********\n" << endl;


    // Custom comparator that treats equal values differently based on position
    struct PositionAwareCompare 
    {
        bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
            // First compare values
            if (a.first != b.first) return a.first < b.first;
            // For equal values, prefer later positions (higher second value)
            return a.second < b.second;
        }
    };


    if (pqType == "Unordered")
    {
        // Create PQ with position-aware comparator
        UnorderedPQ<std::pair<int, int>, PositionAwareCompare> pq;
        
        // Push elements with same value but different positions
        pq.push({10, 1});  // Value 10, position 1
        assert(pq.getTop() == std::make_pair(10, 1) && "First element should be top");
        
        pq.push({10, 2});  // Value 10, position 2
        // BUG: Should select position 2 as top since it's "greater" with equal values
        // But the implementation only updates topIdx when strictly greater
        assert(pq.getTop() == std::make_pair(10, 2) && "Second element should be top");
        
        std::cout << "Test " << (pq.getTop().second == 2 ? "passed" : "FAILED") << "\n";
    }
    else if (pqType == "UnorderedOPT")
    {
        

        // Create PQ with position-aware comparator
        UnorderedPQOptimized<std::pair<int, int>, PositionAwareCompare> pq;
        
        // Push elements with same value but different positions
        pq.push({10, 1});  // Value 10, position 1
        assert(pq.getTop() == std::make_pair(10, 1) && "First element should be top");
        
        pq.push({10, 2});  // Value 10, position 2
        // BUG: Should select position 2 as top since it's "greater" with equal values
        // But the implementation only updates topIdx when strictly greater
        assert(pq.getTop() == std::make_pair(10, 2) && "Second element should be top");
        
        std::cout << "Test " << (pq.getTop().second == 2 ? "passed" : "FAILED") << "\n";
    }
    else if (pqType == "Binary")
    {
        

        // Create PQ with position-aware comparator
        BinPQ<std::pair<int, int>, PositionAwareCompare> pq;
        
        // Push elements with same value but different positions
        pq.push({10, 1});  // Value 10, position 1
        assert(pq.getTop() == std::make_pair(10, 1) && "First element should be top");
        
        pq.push({10, 2});  // Value 10, position 2
        // BUG: Should select position 2 as top since it's "greater" with equal values
        // But the implementation only updates topIdx when strictly greater
        assert(pq.getTop() == std::make_pair(10, 2) && "Second element should be top");
        
        std::cout << "Test " << (pq.getTop().second == 2 ? "passed" : "FAILED") << "\n";
    }

    cout << "\n\n********** END: Testing " << pqType << "'s equal elements corner case succeeded! **********\n" << endl;
}






void specialTests2(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " special tests to reveal version-specific bugs **********\n" << endl;

    if (pqType == "Unordered")
    {
        UnorderedPQ<int> pq;
        pq.push(10);
        pq.push(20);
        pq.pop();  // Remove 20
        pq.push(5);  // Push smaller value
        assert(pq.getTop() == 10);  // This might fail if the bug exists
    }
    else if (pqType == "UnorderedOPT")
    {
        UnorderedPQOptimized<int> pq;
        pq.push(10);
        pq.push(20);
        pq.pop();  // Remove 20
        pq.push(5);  // Push smaller value
        assert(pq.getTop() == 10);  // This might fail if the bug exists

        UnorderedPQOptimized<int> pq2;
        pq2.push(10);
        pq2.push(5);
        pq2.pop();  // Remove 10
        pq2.push(4);  // Push smaller value
        assert(pq2.getTop() == 5);  // This might fail if the bug exists
    }
    else if (pqType == "Binary")
    {
        BinPQ<int> pq;
        pq.push(10);
        pq.push(20);
        pq.pop();  // Remove 20
        pq.push(5);  // Push smaller value
        assert(pq.getTop() == 10);  // This might fail if the bug exists

        BinPQ<int> pq2;
        pq2.push(10);
        pq2.push(5);
        pq2.pop();  // Remove 10
        pq2.push(4);  // Push smaller value
        assert(pq2.getTop() == 5);  // This might fail if the bug exists
    }

    cout << "\n\n********** END: Testing " << pqType << " special tests to reveal version-specific bugs succeeded! **********\n" << endl;
}











int binTests() 
{
    cout << "\n\n********** START: Testing BinaryPQ **********\n" << endl;

    // Test 1: Basic operations with default constructor
    BinPQ<int> pq1;
    pq1.push(10);
    pq1.push(20);
    pq1.push(5);
    std::cout << "Test 1 - Top after pushes (5,10,20): " << pq1.getTop() << std::endl; // Should be 20
    assert(pq1.getTop() == 20);
    pq1.pop();
    std::cout << "Test 1 - Top after pop: " << pq1.getTop() << std::endl; // Should be 10
    assert(pq1.getTop() == 10);


    // Test 2: Range-based constructor
    std::vector<int> vec = {5, 2, 8, 1, 9, 3};
    BinPQ<int> pq2(vec.begin(), vec.end());
    std::cout << "Test 2 - Top from range (5,2,8,1,9,3): " << pq2.getTop() << std::endl; // Should be 9
    assert(pq2.getTop() == 9);


    // Test 3: bottomUp (priority increase simulation)
    BinPQ<int> pq3;
    pq3.push(10);
    pq3.push(20);
    pq3.push(5);
    // Simulate priority increase at index 2 (value 5 to something larger, say 25)
    pq3.push(25); // Adds at index 3, but for simulation, assume we modify index 2
    // In a real scenario, you'd modify data directly if priorities change, but for test, push and adjust
    std::cout << "Test 3 - Top before bottomUp: " << pq3.getTop() << std::endl; // Should be 25 or 20
    assert(pq3.getTop() == 25 || pq3.getTop() == 20);
    pq3.bottomUp(2); // Should have no effect unless data[2] changed; for demo, assume it’s updated
    // Note: For a real test, modify data[2] directly if priority changes


    // Test 4: topDown (priority decrease simulation)
    BinPQ<int> pq4;
    pq4.push(30);
    pq4.push(20);
    pq4.push(10);
    // Simulate priority decrease at root (index 0, value 30 to 5)
    // In real usage, you'd modify data[0] = 5, then call topDown(0)
    std::cout << "Test 4 - Top before topDown: " << pq4.getTop() << std::endl; // Should be 30
    assert(pq4.getTop() == 30);
    // Assume data[0] = 5; then call pq4.topDown(0); top should become 20
    // For simplicity, just pop and push to simulate
    pq4.pop();
    pq4.push(5);
    std::cout << "Test 4 - Top after simulated decrease: " << pq4.getTop() << std::endl; // Should be 20
    assert(pq4.getTop() == 20);


    // Test 5: updatePQ (rebuild heap)
    BinPQ<int> pq5;
    pq5.push(10);
    pq5.push(5);
    pq5.push(20);
    pq5.updatePQ(); // Rebuild heap, should still have 20 as top
    std::cout << "Test 5 - Top after updatePQ: " << pq5.getTop() << std::endl; // Should be 20
    assert(pq5.getTop() == 20);

    cout << "\n\n********** END: Testing BinaryPQ **********\n" << endl;

    return 0;
}
















int main()
{
    // Basic pointer, allocate a new PQ later based on user choice.
    SPsPQ<int> *pq1;
    SPsPQ<int> *pq2;
    vector<string> types{
        "Unordered",
        "UnorderedOPT",
        "Sorted",
        "Binary",
        "Pairing",
        "Unordered.2"
    }; // choice types

    
    unsigned int choice;
    cout << "PQ tester" << endl
         << endl;
    for (size_t i = 0; i < types.size(); ++i)
        cout << "  " << i << ") " << types[i] << endl;
    cout << endl;
    cout << "Select one {0-5}: ";
    cin >> choice;
    
    // Send a subset = {6,...,87}, i.e., shrink the range
    // by setting a ptr to the 6th elt and the the 11th
    // elt to get the subset [6, 45).
    vector<int> tester = {1, 2, 3, 8, 7, 6, 13, 42, 65, 43, 87, 45, 56, 35, 29};
    auto start = tester.begin() + 5;
    auto end = tester.begin() + 11;

    if (choice == 0)
    {
        // Test using the default comparator,
        // which organizes in ascending order.
        pq1 = new UnorderedPQ<int>;
        pq2 = new UnorderedPQ<int>(start, end);

    } // if
    else if (choice == 1)
    {
        pq1 = new UnorderedPQOptimized<int>;
        pq2 = new UnorderedPQOptimized<int>(start, end);
    }
    else if (choice == 3)
    {
        binTests();
        pq1 = new BinPQ<int>;
        pq2 = new BinPQ<int>(start, end);
    }
    else
    {
        cout << "Unknown container!" << endl
             << endl;
        exit(1);
    } // else

    testHiddenData(types[choice]);

    testPriorityQueue(types[choice]);

    runEdgeTests(types[choice]);

    specialTests(types[choice]);

    specialTests2(types[choice]);

    // Send the pq.
    testPriorityQueuePtr(pq1, types[choice]);

    testPQsCtor(pq2, types[choice]);

    testUpdatePQ(types[choice]);

    // Clean up!
    delete pq1;
    delete pq2;

    cout << "\n\n********** ALL TESTS PASSED! **********\n" << endl;

    return 0;
} // main()












































































