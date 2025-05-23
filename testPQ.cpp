/*
 * Compile this test against your .h files to make sure they compile. Note how
 * the eecs281 priority queues can be constructed with the different types. We
 * suggest adding to this file or creating your own test cases to test your
 * priority queue implementations more thoroughly. If you do not call a
 * function from here, it is NOT compiled due to templates!  So for instance,
 * if you don't add code to call updatePriorities(), you could have compiler
 * errors that you don't even know about.
 *
 * Our makefile will build an executable named testPQ if you type 'make testPQ'
 * or 'make alltests' (without the quotes).
 *
 * Notice that testPairing() tests the range-based constructor but main() and
 * testPriorityQueue() do not.  Make sure to test the range-based constructor
 * for other PQ types, and also test the PairingPQ-specific member functions.
 *
 * 
 * 
 * NOTE THAT I AM RUNNING THIS PROGRAM BY SIMPLY USING THE PLAY BUTTON, WHICH COMPILES THE TESTS AND THEN I RUN THE FILE MANUALLY IN THE TERMINAL: ./testPQ
 * 
 * 
 * 
 */

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <climits> // For INT_MAX and INT_MIN
#include <cmath>
#include <algorithm>

#include "Eecs281PQ.h"
#include "BinaryPQ.h"
#include "UnorderedPQ.h"
#include "UnorderedFastPQ.h"
#include "PairingPQ.h"
#include "SortedPQ.h"

using namespace std;


/*

    Note that I can define the data type that will be compared from the perspective of the class (typename) and the container (datatype). I can also define how to compare, i.e., define what "priority" means, which is with a custom comparator (COMP_FUNCTOR). Below is two ways to create a custom comparator. The 1st uses a struct and the 2nd uses a lambda, which sets it to a variable name just for demo.

*/

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


    // Prioritize odd numbers over even numbers, then compare by value.
    struct OddFirstComp 
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const 
        {
            if ((a.data % 2) != (b.data % 2)) 
            {
                return (a.data % 2) < (b.data % 2); // Odd numbers come first
            }
            return a.data < b.data; // Then sort by value
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

    if (pqType == "Sorted")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        SortedPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.size() == 3);
        assert(pq.top().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.top().data == 5);
        pq.pop();
        assert(pq.top().data == 3);
        pq.pop();
        assert(pq.empty());

        SortedPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.top().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.top().dec_data == 5.2);


        // Odds before evens
        SortedPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.top().data == 3); 
        pq4.pop();
        assert(pq4.top().data == 1);
        pq4.pop();
        pq4.pop();
        pq4.pop();
        assert(pq4.empty());



        // Order by the last digit
        SortedPQ<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.top().data == 7);
        pqL.pop();
        assert(pqL.top().data == 25);
        pqL.pop();
        assert(pqL.top().data == 13);
        pqL.pop();
        assert(pqL.top().data == 12);
        pqL.pop();
        assert(pqL.empty());
    }
    else if (pqType == "Pairing")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        PairingPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.size() == 3);
        assert(pq.top().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.top().data == 5);
        pq.pop();
        assert(pq.top().data == 3);
        pq.pop();
        assert(pq.empty());



        PairingPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.top().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.top().dec_data == 5.2);


        // Odds before evens
        PairingPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        // cout << "Expected top is: 1" << endl;
        // cout << "Actual top is: " << pq4.top().data << endl;
        // pq4.pop();
        // cout << "When I pop that top, I get a new top of: " << pq4.top().data << endl;

        assert(pq4.top().data == 1 || pq4.top().data == 3); 
        if (pq4.top().data == 1)
        {
            pq4.pop();
            assert(pq4.top().data == 3);
        }
        else
        {
            pq4.pop();
            assert(pq4.top().data == 1);
        }
        pq4.pop();
        pq4.pop();
        pq4.pop();
        assert(pq4.empty());



        // Order by the last digit
        PairingPQ<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.top().data == 7);
        pqL.pop();
        assert(pqL.top().data == 25);
        pqL.pop();
        assert(pqL.top().data == 13);
        pqL.pop();
        assert(pqL.top().data == 12);
        pqL.pop();
        assert(pqL.empty());
    }
    else if (pqType == "Unordered")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        UnorderedPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.size() == 3);
        assert(pq.top().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.top().data == 5);
        pq.pop();
        assert(pq.top().data == 3);
        pq.pop();
        assert(pq.empty());

        UnorderedPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.top().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.top().dec_data == 5.2);


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

        assert(pq4.top().data == 3); 
        pq4.pop();
        assert(pq4.top().data == 1);
        pq4.pop();
        pq4.pop();
        assert(pq4.top().data == 2);
        pq4.pop();
        assert(pq4.empty());



        // Order by the last digit
        UnorderedPQ<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.top().data == 7);
        pqL.pop();
        assert(pqL.top().data == 25);
        pqL.pop();
        assert(pqL.top().data == 13);
        pqL.pop();
        assert(pqL.top().data == 12);
        pqL.pop();
        assert(pqL.empty());
    }
    else if (pqType == "UnorderedOPT")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        UnorderedFastPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.size() == 3);
        assert(pq.top().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.top().data == 5);
        pq.pop();
        assert(pq.top().data == 3);
        pq.pop();
        assert(pq.empty());

        UnorderedFastPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.top().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.top().dec_data == 5.2);


        // Odds before evens
        UnorderedFastPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.top().data == 3); 
        pq4.pop();
        assert(pq4.top().data == 1);
        pq4.pop();
        pq4.pop();
        assert(pq4.top().data == 2);
        pq4.pop();
        assert(pq4.empty());



        // Order by the last digit
        UnorderedFastPQ<HiddenData, decltype(customComp)> pqL(customComp);  
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.top().data == 7);
        pqL.pop();
        assert(pqL.top().data == 25);
        pqL.pop();
        assert(pqL.top().data == 13);
        pqL.pop();
        assert(pqL.top().data == 12);
        pqL.pop();
        assert(pqL.empty());
    }
    else if (pqType == "Binary")
    {
        // Create a simple sorted pq w/ the hidden type and comparator.
        BinaryPQ<HiddenData, HiddenDataMaxHeap> pq;

        // Populate the pq
        pq.push(HiddenData(5));
        pq.push(HiddenData(10));
        pq.push(HiddenData(3));

        // Verify size and top element.
        assert(pq.size() == 3);
        assert(pq.top().data == 10);

        // Pop and check remaining elements
        pq.pop();
        assert(pq.top().data == 5);
        pq.pop();
        assert(pq.top().data == 3);
        pq.pop();
        assert(pq.empty());

        BinaryPQ<HiddenData, HDAbsComparator> pq2;
        pq2.push(HiddenData(-10.5));
        pq2.push(HiddenData(5.2));
        pq2.push(HiddenData(-3.3));

        assert(pq2.top().dec_data == -10.5); // Largest absolute decimal
        pq2.pop();
        assert(pq2.top().dec_data == 5.2);


        // Odds before evens
        BinaryPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.top().data == 3); 
        pq4.pop();
        assert(pq4.top().data == 1);
        pq4.pop();
        pq4.pop();
        assert(pq4.top().data == 2);
        pq4.pop();
        assert(pq4.empty());



        // Order by the last digit
        BinaryPQ<HiddenData, decltype(customComp)> pqL(customComp);
        pqL.push(HiddenData(12));
        pqL.push(HiddenData(25));
        pqL.push(HiddenData(13));
        pqL.push(HiddenData(7));

        assert(pqL.top().data == 7);
        pqL.pop();
        assert(pqL.top().data == 25);
        pqL.pop();
        assert(pqL.top().data == 13);
        pqL.pop();
        assert(pqL.top().data == 12);
        pqL.pop();
        assert(pqL.empty());
    }


    cout << "\n\n********** END: Testing " << pqType << " with hidden data succeeded! **********\n" << endl;
   
} // testHiddenData()



// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePQHelper(Eecs281PQ<int *, IntPtrComp> *pq)
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
    pq->updatePriorities();
    assert(*pq->top() == 90);
    data[1] = 200;
    pq->updatePriorities();
    assert(*pq->top() == 200);
    data[0] = 3000;
    pq->updatePriorities();
    assert(*pq->top() == 3000);
    
} // testUpdatePQHelper()



// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePQ(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " priority queue updatePQ() **********\n" << endl;

    Eecs281PQ<int *, IntPtrComp> *pq = nullptr;

    if (pqType == "Unordered")
    {
        pq = new UnorderedPQ<int *, IntPtrComp>;
    } // if
    else if (pqType == "Sorted")
    {
        pq = new SortedPQ<int *, IntPtrComp>;
    } // else if
    else if (pqType == "Binary")
    {
        pq = new BinaryPQ<int *, IntPtrComp>;
    } // else if
    else if (pqType == "Pairing")
    {
        pq = new PairingPQ<int *, IntPtrComp>;
    } // else if
    // TODO: Add more types here inside 'else if' statements, like in main().
    
    if (!pq)
    {
        cout << "Invalid pq pointer; did you forget to create it?" << endl;
        return;
    } // if
    
    testUpdatePQHelper(pq);

    cout << "\n\n********** END: Testing " << pqType << " priority queue updatePQ() succeeded! **********\n" << endl;

    delete pq;
} // testUpdatePQ()



// Test the basic functionality of the priority queue pointer version.
void testPriorityQueuePtr(Eecs281PQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (pointer version) **********\n" << endl;
    
    // Very basic testing.
    cout << "Test 1: integer values...\n"
         << endl;
    pq->push(3);
    pq->push(4);
    assert(pq->size() == 2);
    assert(pq->top() == 4);
    
    pq->pop();
    assert(pq->size() == 1);
    assert(pq->top() == 3);
    assert(!pq->empty());
    
    pq->pop();
    assert(pq->size() == 0);
    assert(pq->empty());
    
    
    pq->push(18);
    pq->push(81);
    pq->push(42);
    assert(pq->top() == 81);
    pq->pop();
    assert(pq->top() == 42);
    pq->pop();
    assert(pq->top() == 18);
    pq->pop();


    // Start fresh w/ an empty pq.
    assert(pq->empty());

    cout << "Integer value tests passed!\n"
         << endl;


    // TODO: Add more testing here!

    // More focused unit testing.



    // Large range of values
    cout << "Testing with extrame values...\n" << endl;
    pq->push(INT_MIN);
    pq->push(INT_MAX);
    pq->push(0);
    pq->push(500);
    pq->push(-500);
    assert(pq->size() == 5);
    assert(pq->top() == INT_MAX);
    pq->pop();
    assert(pq->top() == 500);
    pq->pop();
    assert(pq->top() == 0);
    pq->pop();
    assert(pq->top() == -500);
    pq->pop();
    assert(pq->top() == INT_MIN);
    pq->pop();
    assert(pq->empty());
    cout << "Extreme value tests passed!\n" << endl;



    // Sorted input
    cout << "Testing with sorted values...\n" << endl;
    for (int i = 1; i <= 10; ++i) 
        pq->push(i);
    assert(pq->size() == 10);
    assert(pq->top() == 10);
    pq->pop();
    assert(pq->top() == 9);
    pq->pop();
    // Clean-up
    while (!pq->empty()) 
        pq->pop();
    cout << "Sorted value tests passed!\n" << endl;



    // Reverse-sorted input
    cout << "Testing with reversed sorted values...\n" << endl;
    for (int i = 10; i >= 1; --i) 
        pq->push(i);
    assert(pq->size() == 10);
    assert(pq->top() == 10);
    pq->pop();
    assert(pq->top() == 9);
    pq->pop();
    // Clean-up
    while (!pq->empty()) 
        pq->pop();
    cout << "Reversed sorted value tests passed!\n" << endl;



    // Mix of positive and negative numbers
    cout << "Testing with both +/- values...\n" << endl;
    pq->push(-10);
    pq->push(50);
    pq->push(0);
    pq->push(-5);
    pq->push(30);
    assert(pq->size() == 5);
    assert(pq->top() == 50);
    pq->pop();
    assert(pq->top() == 30);
    pq->pop();
    assert(pq->top() == 0);
    pq->pop();
    assert(pq->top() == -5);
    pq->pop();
    assert(pq->top() == -10);
    pq->pop();
    cout << "Both +/- value tests passed!\n" << endl;



    // Testing `updatePriorities()` after manual data modification
    /*
        I.e., simulate a scenario where updatePriorities() is called explicitly and verify heap integrity afterward.
    */
    cout << "Testing by manually modifying the values...\n" << endl;
    pq->push(100);
    pq->push(200);
    pq->push(50);
    pq->push(75);
    // Simulate external modification
    pq->updatePriorities();
    assert(pq->top() == 200);
    pq->pop();
    assert(pq->top() == 100);
    pq->pop();
    assert(pq->top() == 75);
    cout << "Manual value modification tests passed!\n" << endl;

    cout << "\n\n********** END: Testing " << pqType << " basic functionality (pointer version) succeeded! **********\n" << endl;

} // testPriorityQueuePtr()



void testSortedPQ(Eecs281PQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " sorted functionality **********\n" << endl;


    /// NOTE: WHAT SHOULD I DO WITH THE PTR??? MAYBE TRY SOME COPYING AND ASSIGNMENT TESTS???
    
    
    // Test the default constructor
    cout << "Testing the default constructor's basic functionality...\n" << endl;
    SortedPQ<int> pq2;
    assert(pq2.empty());
    assert(pq2.size() == 0);
    pq2.push(10);
    assert(!pq2.empty());
    assert(pq2.top() == 10);
    pq2.pop();
    assert(pq2.empty());
    cout << "Default constructor tests passed!\n" << endl;



    // Test Range Constructor
    cout << "Testing the range-based integer constructor...\n" << endl;
    cout << "1. With an empty pq...\n" << endl;
    std::vector<int> emptyVec;
    SortedPQ<int> pqEmpty(emptyVec.begin(), emptyVec.end());
    assert(pqEmpty.empty());
    std::cout << "Empty range test passed!\n" << std::endl;

    cout << "2. With a pre-sorted pq...\n" << endl;
    std::vector<int> sortedVec = {1, 2, 3, 4, 5};
    SortedPQ<int> pqSorted(sortedVec.begin(), sortedVec.end());
    assert(pqSorted.size() == 5);
    assert(pqSorted.top() == 5);
    std::cout << "Pre-sorted range test passed!\n" << std::endl;

    cout << "3. With a Reverse-sorted pq...\n" << endl;
    std::vector<int> reverseSortedVec = {7, 5, 4, 3, 2, 1};
    SortedPQ<int> pqReverse(reverseSortedVec.begin(), reverseSortedVec.end());
    assert(pqReverse.size() == 6);
    assert(pqReverse.top() == 7);
    std::cout << "Reverse-sorted range test passed!\n" << std::endl;

    cout << "4. With duplicates...\n" << endl;
    std::vector<int> duplicateVec = {3, 3, 3, 3};
    SortedPQ<int> pqDuplicates(duplicateVec.begin(), duplicateVec.end());
    assert(pqDuplicates.size() == 4);
    assert(pqDuplicates.top() == 3);
    std::cout << "Range with duplicates test passed!\n" << std::endl;




    // Test Custom Comparator
    cout << "Testing with a min-heap type custom comparator...\n" << endl;
    cout << "1. Basic min extraction w/ push()...\n" << endl;
    auto greaterComparator = [](int a, int b) { return a > b; };
    SortedPQ<int, decltype(greaterComparator)> pq3(greaterComparator);
    pq3.push(1);
    pq3.push(5);
    pq3.push(3);
    assert(pq3.top() == 1); // Smallest element should now be at the "top"
    pq3.pop();
    assert(pq3.top() == 3);
    std::cout << "Basic min extraction test passed!\n" << std::endl;



    // Test Random Inputs With Comparator

    /*
        *** A Note on the following syntax: lambda and decltype ***

        The comparator uses a lambda function, which is a mini-function that you don't need to name and can define exactly where you need to use it. You can create them on-the-fly to perform a specific task.

        lambda syntax:
        - [] is the capture list. It tells the lambda what variables from the outside it can use. In this case, it's empty because we don't need to use any external variables.
        - (int a, int b) is like the parameter list in a regular function. It defines what the lambda takes as an input.
        - { ... } contains the code of the lambda. Here, it checks if an item is on sale and prints its name if it is.


        It also uses the decltype keyword, which is different from auto. 
        
        The auto keyword is used for type inference primarily based on the value assigned to the variable at the time of its initialization. When you use auto, you're telling the compiler to deduce the type of the variable from the expression used to initialize it. It simplifies the declaration of variables, especially when dealing with complex data types or iterator types in container classes. It is also useful in lambda expressions and range-based for loops to keep the code clean and manageable. 

        e.g.: std::vector<int> numbers = {1, 2, 3, 4, 5};
        auto it = numbers.begin(); // `auto` deduces that `it` is a std::vector<int>::iterator


        The decltype keyword is used to query the type of an expression but does not itself evaluate the expression. It is strictly used to extract the type from the expression it is given. decltype is particularly useful in templates and metaprogramming when you need to determine the type resulting from operations without actually performing those operations. It is also used in scenarios where the behavior depends on the type of the result of expressions.

        e.g.: int x = 5;
        double y = 10.0;
        decltype(x * y) z; // `decltype` deduces that `z` is a double because `x * y` is a double

    
    */



    cout << "2. Random inputs w/ the range-based ctor & min-heapPQ...\n" << endl;
    std::vector<int> randomVec = {50, 20, 40, 10, 30};
    auto customComparator = [](int a, int b) { return a > b; }; // Min-heap
    SortedPQ<int, decltype(customComparator)> pq6(randomVec.begin(), randomVec.end(), customComparator);
    assert(pq6.top() == 10); // Smallest element
    pq6.pop();
    assert(pq6.top() == 20);
    std::cout << "Random inputs w/ the range-based ctor & min-heapPQ test passed!" << std::endl;    
    




    cout << "Testing priority queue with non-integer values: " << pqType << "\n" << endl;

    Eecs281PQ<double> *pqq = new SortedPQ<double>;

    // Very basic testing.
    cout << "Testing basic functionality...\n" << endl;
    pqq->push(3.5);
    pqq->push(2.75);
    pqq->push(4.25);
    pqq->push(4.26);
    assert(pqq->size() == 4);
    assert(pqq->top() == 4.26);
    
    pqq->pop();
    assert(pqq->size() == 3);
    assert(pqq->top() == 4.25);
    
    pqq->pop();
    assert(pqq->size() == 2);
    assert(pqq->top() == 3.5);
    
    pqq->pop();
    pqq->pop();
    assert(pqq->empty());
    cout << "Testing basic functionality passed!\n" << endl;

    
    cout << "\ntestNonIntegerValues() succeeded!\n" << endl;




    // Test Update Priorities Empty
    cout << "Testing updatePriorities...\n" << endl;
    SortedPQ<int> pq4;
    pq4.updatePriorities();
    assert(pq4.empty());
    std::cout << "updatePriorities on empty queue test passed!\n" << std::endl;

    pq4.push(1);
    pq4.push(2);
    pq4.push(3);
    pq4.updatePriorities();
    assert(pq4.top() == 3);
    pq4.pop();
    assert(pq4.top() == 2);

    /**
     * In this case, since pq is a pointer to Eecs281PQ<int> 
     * and pq4 is a SortedPQ<int>, you need to use the 
     * address-of operator to get the address of pq4.
     */
    pq = &pq4;
    pq->push(0);
    pq->push(-1);   // pq = [-1, 0, 1, 2]
    assert(pq->top() == 2);
    
    pq->pop();
    pq->pop();
    pq->pop();
    assert(pq->top() == -1);
    
    
    
    

    
    cout << "\n\n********** END: Testing " << pqType << " sorted functionality succeeded! **********\n" << endl;
    
} // testSortedPQ()



// Test the unsorted heap's range-based constructor, copy constructor,
// and operator=().
//
void testPQsCtor(Eecs281PQ<int> *pq, const string &pqType)
{
    // Send a subset = {6,...,87}, i.e., shrink the range
    // by setting a ptr to the 6th elt and the the 11th
    // elt to get the subset [6, 45).

    cout << "\n\n********** START: Testing " << pqType << "'s range-based constructor **********\n" << endl;
    
    assert(pq->size() == 6);
    assert(pq->top() == 87);
    
    pq->pop();
    assert(pq->size() == 5);
    assert(pq->top() == 65);
    assert(!pq->empty());
    
    pq->pop();
    assert(pq->size() == 4);
    assert(!pq->empty());
    
    // TODO: Add more testing here!
    
    cout << "\n\n********** END: Testing " << pqType << "'s range-based constructor succeeded! **********\n" << endl;
} // testPQsCtor()



// Test the pairing heap's range-based constructor, copy constructor,
// and operator=().
//
void testPairing(vector<int> & vec)
{
    cout << "Testing Pairing Heap separately" << endl;
    Eecs281PQ<int> * pq1 = new PairingPQ<int>(vec.begin(), vec.end());
    Eecs281PQ<int> * pq2 = new PairingPQ<int>(*((PairingPQ<int> *)pq1));
    
//    PairingPQ pq1 {vec.cbegin(), vec.cend()};
//    PairingPQ pq2 {pq1};
    
    // This line is different just to show two different ways to declare a
    // pairing heap: as an Eecs281PQ and as a PairingPQ. Yay for inheritance!
    PairingPQ<int> *pq3 = new PairingPQ<int>();
    *pq3 = *((PairingPQ<int> *)pq2);
    
    pq1->push(3);
    pq2->pop();
    assert(pq1->size() == 3); // 0,1,3
    assert(!pq1->empty());
    assert(pq1->top() == 3);
    pq2->push(pq3->top());
    assert(pq2->top() == pq3->top());
    
    cout << "Basic tests done!" << endl;
    
    // TODO: Add more code to test addNode(), updateElt(), etc.
    pq1->push(45);
    pq1->push(7);
    pq1->push(1);
    pq1->push(15);
    pq1->push(12);
    assert(pq1->top() == 45);
    pq1->pop();
    assert(pq1->top() == 15);
    pq1->pop();
    assert(pq1->top() == 12);
    pq1->push(45);
    assert(pq1->top() == 45);
    pq1->push(45);
    pq1->push(45);
    pq1->push(30);
    assert(pq1->top() == 45);
    pq1->push(33);
    pq1->push(20);
    pq1->pop();
    pq1->pop();
    pq1->pop();
    assert(pq1->top() == 33);
    
    
    cout << "Calling destructors" << endl;
    delete pq1;
    delete pq2;
    delete pq3;
    
    cout << "testPairing() succeeded" << endl;
} // testPairing()



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
        assert(pq1.empty() && "Empty queue should report empty");
        try {
            pq1.top();
            std::cout << "Test 1 failed: Expected exception on empty getTop\n";
        } catch (const std::runtime_error&) {
            std::cout << "Test 1 passed: Empty queue throws on getTop\n";
        }

        // Test 2: Single element
        cout << "Test 2: single element..."
         << endl;
        pq1.push(42);
        assert(pq1.size() == 1 && "Size should be 1");
        assert(pq1.top() == 42 && "Top should be 42");
        pq1.pop();
        assert(pq1.empty() && "Queue should be empty after pop");
        cout << "Test 2 passed!\n"
         << endl;

        // Test 3: Range-based constructor with duplicates
        cout << "Test 3: range-based constructor with duplicates..."
         << endl;
        int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
        UnorderedPQ<int> pq2(arr, arr + 9);
        assert(pq2.size() == 9 && "Size should match range");
        assert(pq2.top() == 9 && "Top should be max (9)");
        pq2.pop();
        assert(pq2.top() == 6 && "Next top should be 6 after pop");
        cout << "Test 3 passed!\n"
         << endl;

        // Test 4: All equal elements
        cout << "Test 4: all equal elements..."
         << endl;
        UnorderedPQ<int> pq3;
        for (int i = 0; i < 5; ++i) pq3.push(7);
        assert(pq3.size() == 5 && "Size should be 5");
        assert(pq3.top() == 7 && "Top should be 7");
        pq3.pop();
        assert(pq3.size() == 4 && "Size should decrease");
        assert(pq3.top() == 7 && "Top should still be 7");
        cout << "Test 4 passed!\n"
         << endl;

        // Test 5: Push after pop to zero
        cout << "Test 5: push after pop to zero..."
         << endl;
        UnorderedPQ<int> pq4;
        pq4.push(1);
        pq4.pop();
        assert(pq4.empty() && "Should be empty after pop");
        pq4.push(2);
        assert(pq4.top() == 2 && "New push should work");
        cout << "Test 5 passed!\n"
         << endl;

        // Test 6: Large number of elements
        cout << "Test 6: large number of elements..."
         << endl;
        UnorderedPQ<int> pq5;
        for (int i = 0; i < 1000; ++i) pq5.push(i);
        assert(pq5.size() == 1000 && "Size should be 1000");
        assert(pq5.top() == 999 && "Top should be max (999)");
        cout << "Test 6 passed!\n"
         << endl;

    }
    else if (pqType == "UnorderedOPT")
    {
        // Test 1: Empty queue
        cout << "Test 1: empty queue..."
         << endl;
        UnorderedFastPQ<int> pq1;
        assert(pq1.empty() && "Empty queue should report empty");
        try {
            pq1.top();
            std::cout << "Test 1 failed: Expected exception on empty getTop\n";
        } catch (const std::runtime_error&) {
            std::cout << "Test 1 passed: Empty queue throws on getTop\n";
        }

        // Test 2: Single element
        cout << "Test 2: single element..."
         << endl;
        pq1.push(42);
        assert(pq1.size() == 1 && "Size should be 1");
        assert(pq1.top() == 42 && "Top should be 42");
        pq1.pop();
        assert(pq1.empty() && "Queue should be empty after pop");
        cout << "Test 2 passed!\n"
         << endl;

        // Test 3: Range-based constructor with duplicates
        cout << "Test 3: range-based constructor with duplicates..."
         << endl;
        int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
        UnorderedFastPQ<int> pq2(arr, arr + 9);
        assert(pq2.size() == 9 && "Size should match range");
        assert(pq2.top() == 9 && "Top should be max (9)");
        pq2.pop();
        assert(pq2.top() == 6 && "Next top should be 6 after pop");
        cout << "Test 3 passed!\n"
         << endl;

        // Test 4: All equal elements
        cout << "Test 4: all equal elements..."
         << endl;
        UnorderedFastPQ<int> pq3;
        for (int i = 0; i < 5; ++i) pq3.push(7);
        assert(pq3.size() == 5 && "Size should be 5");
        assert(pq3.top() == 7 && "Top should be 7");
        pq3.pop();
        assert(pq3.size() == 4 && "Size should decrease");
        assert(pq3.top() == 7 && "Top should still be 7");
        cout << "Test 4 passed!\n"
         << endl;

        // Test 5: Push after pop to zero
        cout << "Test 5: push after pop to zero..."
         << endl;
        UnorderedFastPQ<int> pq4;
        pq4.push(1);
        pq4.pop();
        assert(pq4.empty() && "Should be empty after pop");
        pq4.push(2);
        assert(pq4.top() == 2 && "New push should work");
        cout << "Test 5 passed!\n"
         << endl;

        // Test 6: Large number of elements
        cout << "Test 6: large number of elements..."
         << endl;
        UnorderedFastPQ<int> pq5;
        for (int i = 0; i < 1000; ++i) pq5.push(i);
        assert(pq5.size() == 1000 && "Size should be 1000");
        assert(pq5.top() == 999 && "Top should be max (999)");
        cout << "Test 6 passed!\n"
         << endl;
    }
    else if (pqType == "Binary")
    {
        // Test 1: Empty queue
        cout << "Test 1: empty queue..."
         << endl;
        BinaryPQ<int> pq1;
        assert(pq1.empty() && "Empty queue should report empty");
        try {
            pq1.top();
            std::cout << "Test 1 failed: Expected exception on empty getTop\n";
        } catch (const std::runtime_error&) {
            std::cout << "Test 1 passed: Empty queue throws on getTop\n";
        }

        // Test 2: Single element
        cout << "Test 2: single element..."
         << endl;
        pq1.push(42);
        assert(pq1.size() == 1 && "Size should be 1");
        assert(pq1.top() == 42 && "Top should be 42");
        pq1.pop();
        assert(pq1.empty() && "Queue should be empty after pop");
        cout << "Test 2 passed!\n"
         << endl;

        // Test 3: Range-based constructor with duplicates
        cout << "Test 3: range-based constructor with duplicates..."
         << endl;
        int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
        BinaryPQ<int> pq2(arr, arr + 9);
        assert(pq2.size() == 9 && "Size should match range");
        assert(pq2.top() == 9 && "Top should be max (9)");
        pq2.pop();
        assert(pq2.top() == 6 && "Next top should be 6 after pop");
        cout << "Test 3 passed!\n"
         << endl;

        // Test 4: All equal elements
        cout << "Test 4: all equal elements..."
         << endl;
        BinaryPQ<int> pq3;
        for (int i = 0; i < 5; ++i) pq3.push(7);
        assert(pq3.size() == 5 && "Size should be 5");
        assert(pq3.top() == 7 && "Top should be 7");
        pq3.pop();
        assert(pq3.size() == 4 && "Size should decrease");
        assert(pq3.top() == 7 && "Top should still be 7");
        cout << "Test 4 passed!\n"
         << endl;

        // Test 5: Push after pop to zero
        cout << "Test 5: push after pop to zero..."
         << endl;
        BinaryPQ<int> pq4;
        pq4.push(1);
        pq4.pop();
        assert(pq4.empty() && "Should be empty after pop");
        pq4.push(2);
        assert(pq4.top() == 2 && "New push should work");
        cout << "Test 5 passed!\n"
         << endl;

        // Test 6: Large number of elements
        cout << "Test 6: large number of elements..."
         << endl;
        BinaryPQ<int> pq5;
        for (int i = 0; i < 1000; ++i) pq5.push(i);
        assert(pq5.size() == 1000 && "Size should be 1000");
        assert(pq5.top() == 999 && "Top should be max (999)");
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
            assert(pq.top() == 5 && "Top should be 5");
            pq.pop();    // Remove one 5
            assert(pq.top() == 5 && "Top should still be 5");
            pq.push(5);  // Add another 5
            assert(pq.top() == 5 && "Top should remain 5");
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
            assert(pq.empty() && "Queue should be empty");
            pq.push(5);  // New max after empty
            assert(pq.top() == 5 && "Top should be 5");
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
            assert(pq.top() == 8 && "Top should be 8");
            pq.push(5);  // Lower than current top
            assert(pq.top() == 8 && "Top should still be 8");
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
            assert(pq.top() == 7 && "Top should be 7");
            pq.pop();    // Remove another 7
            assert(pq.top() == 7 && "Top should still be 7");
            pq.pop();    // Remove last 7
            assert(pq.empty() && "Queue should be empty");
            std::cout << "Test 4 passed\n";
        }
        // Potential Bug: Optimized might fail to find the next 7 if topIdx isn’t updated
        // after each pop, or it might not handle empty state correctly.

        // Test 5: Range init with all equal, pop all
        {
            std::cout << "Test 5: Range init with all equal, pop all\n";
            int arr[] = {4, 4, 4, 4};
            UnorderedPQ<int> pq(arr, arr + 4);
            assert(pq.top() == 4 && "Top should be 4");
            pq.pop();
            assert(pq.top() == 4 && "Top should be 4");
            pq.pop();
            pq.pop();
            pq.pop();
            assert(pq.empty() && "Queue should be empty");
            std::cout << "Test 5 passed\n";
        }

    }
    else if (pqType == "UnorderedOPT")
    {
        std::cout << "Running edge tests on UnorderedPQOptimized to reveal version-specific bugs...\n";

        // Test 1: Push-pop-push with equal elements
        {
            std::cout << "Test 1: Push-pop-push with equal elements\n";
            UnorderedFastPQ<int> pq;  // Replace with UnorderedPQOptimized for comparison
            pq.push(5);
            pq.push(5);  // Two equal max elements
            assert(pq.top() == 5 && "Top should be 5");
            pq.pop();    // Remove one 5
            assert(pq.top() == 5 && "Top should still be 5");
            pq.push(5);  // Add another 5
            assert(pq.top() == 5 && "Top should remain 5");
            std::cout << "Test 1 passed\n";
        }
        // Potential Bug: In Optimized, if pop invalidates topIdx but doesn’t rescan correctly,
        // it might miss the remaining 5 or pick the wrong index after push.

        // Test 2: Pop all elements then push new max
        {
            std::cout << "Test 2: Pop all elements then push new max\n";
            UnorderedFastPQ<int> pq;
            pq.push(3);
            pq.push(4);
            pq.pop();  // Remove 4
            pq.pop();  // Remove 3
            assert(pq.empty() && "Queue should be empty");
            pq.push(5);  // New max after empty
            assert(pq.top() == 5 && "Top should be 5");
            std::cout << "Test 2 passed\n";
        }
        // Potential Bug: Optimized might not reset topIdx properly after emptying,
        // causing getTop to reference an invalid index or old data.

        // Test 3: Push lower value after pop, check top
        {
            std::cout << "Test 3: Push lower value after pop\n";
            UnorderedFastPQ<int> pq;
            pq.push(10);
            pq.push(8);
            pq.pop();  // Remove 10
            assert(pq.top() == 8 && "Top should be 8");
            pq.push(5);  // Lower than current top
            assert(pq.top() == 8 && "Top should still be 8");
            std::cout << "Test 3 passed\n";
        }
        // Potential Bug: Optimized might incorrectly update topIdx to 5 during push,
        // assuming new elements always challenge the top.

        // Test 4: Repeated pop with duplicates
        {
            std::cout << "Test 4: Repeated pop with duplicates\n";
            UnorderedFastPQ<int> pq;
            pq.push(7);
            pq.push(7);
            pq.push(7);  // Three equal max elements
            pq.pop();    // Remove one 7
            assert(pq.top() == 7 && "Top should be 7");
            pq.pop();    // Remove another 7
            assert(pq.top() == 7 && "Top should still be 7");
            pq.pop();    // Remove last 7
            assert(pq.empty() && "Queue should be empty");
            std::cout << "Test 4 passed\n";
        }
        // Potential Bug: Optimized might fail to find the next 7 if topIdx isn’t updated
        // after each pop, or it might not handle empty state correctly.

        // Test 5: Range init with all equal, pop all
        {
            std::cout << "Test 5: Range init with all equal, pop all\n";
            int arr[] = {4, 4, 4, 4};
            UnorderedFastPQ<int> pq(arr, arr + 4);
            assert(pq.top() == 4 && "Top should be 4");
            pq.pop();
            assert(pq.top() == 4 && "Top should be 4");
            pq.pop();
            pq.pop();
            pq.pop();
            assert(pq.empty() && "Queue should be empty");
            std::cout << "Test 5 passed\n";
        }

    }
    else if (pqType == "Binary")
    {
        // Test 1: Push-pop-push with equal elements
        {
            cout << "Test 1: Push-pop-push with equal elements\n";
            BinaryPQ<int> pq;  // Replace with UnorderedPQOptimized for comparison
            pq.push(5);
            pq.push(5);  // Two equal max elements
            assert(pq.top() == 5 && "Top should be 5");
            pq.pop();    // Remove one 5
            assert(pq.top() == 5 && "Top should still be 5");
            pq.push(5);  // Add another 5
            assert(pq.top() == 5 && "Top should remain 5");
            cout << "Test 1 passed!\n"
             << endl;
        }
        // Potential Bug: In Optimized, if pop invalidates topIdx but doesn’t rescan correctly,
        // it might miss the remaining 5 or pick the wrong index after push.

        // Test 2: Pop all elements then push new max
        {
            std::cout << "Test 2: Pop all elements then push new max\n";
            BinaryPQ<int> pq;
            pq.push(3);
            pq.push(4);
            pq.pop();  // Remove 4
            pq.pop();  // Remove 3
            assert(pq.empty() && "Queue should be empty");
            pq.push(5);  // New max after empty
            assert(pq.top() == 5 && "Top should be 5");
            std::cout << "Test 2 passed\n";
        }
        // Potential Bug: Optimized might not reset topIdx properly after emptying,
        // causing getTop to reference an invalid index or old data.

        // Test 3: Push lower value after pop, check top
        {
            std::cout << "Test 3: Push lower value after pop\n";
            BinaryPQ<int> pq;
            pq.push(10);
            pq.push(8);
            pq.pop();  // Remove 10
            assert(pq.top() == 8 && "Top should be 8");
            pq.push(5);  // Lower than current top
            assert(pq.top() == 8 && "Top should still be 8");
            std::cout << "Test 3 passed\n";
        }
        // Potential Bug: Optimized might incorrectly update topIdx to 5 during push,
        // assuming new elements always challenge the top.

        // Test 4: Repeated pop with duplicates
        {
            std::cout << "Test 4: Repeated pop with duplicates\n";
            BinaryPQ<int> pq;
            pq.push(7);
            pq.push(7);
            pq.push(7);  // Three equal max elements
            pq.pop();    // Remove one 7
            assert(pq.top() == 7 && "Top should be 7");
            pq.pop();    // Remove another 7
            assert(pq.top() == 7 && "Top should still be 7");
            pq.pop();    // Remove last 7
            assert(pq.empty() && "Queue should be empty");
            std::cout << "Test 4 passed\n";
        }
        // Potential Bug: Optimized might fail to find the next 7 if topIdx isn’t updated
        // after each pop, or it might not handle empty state correctly.

        // Test 5: Range init with all equal, pop all
        {
            std::cout << "Test 5: Range init with all equal, pop all\n";
            int arr[] = {4, 4, 4, 4};
            BinaryPQ<int> pq(arr, arr + 4);
            assert(pq.top() == 4 && "Top should be 4");
            pq.pop();
            assert(pq.top() == 4 && "Top should be 4");
            pq.pop();
            pq.pop();
            pq.pop();
            assert(pq.empty() && "Queue should be empty");
            std::cout << "Test 5 passed\n";
        }

    }

    cout << "\n\n********** END: Testing " << pqType << " edge tests to reveal version-specific bugs (regular version) succeeded! **********\n" << endl;

} // runEdgeTests()



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
        assert(pq.top() == std::make_pair(10, 1) && "First element should be top");
        
        pq.push({10, 2});  // Value 10, position 2
        // BUG: Should select position 2 as top since it's "greater" with equal values
        // But the implementation only updates topIdx when strictly greater
        assert(pq.top() == std::make_pair(10, 2) && "Second element should be top");
        
        std::cout << "Test " << (pq.top().second == 2 ? "passed" : "FAILED") << "\n";
    }
    else if (pqType == "UnorderedOPT")
    {
        

        // Create PQ with position-aware comparator
        UnorderedFastPQ<std::pair<int, int>, PositionAwareCompare> pq;
        
        // Push elements with same value but different positions
        pq.push({10, 1});  // Value 10, position 1
        assert(pq.top() == std::make_pair(10, 1) && "First element should be top");
        
        pq.push({10, 2});  // Value 10, position 2
        // BUG: Should select position 2 as top since it's "greater" with equal values
        // But the implementation only updates topIdx when strictly greater
        assert(pq.top() == std::make_pair(10, 2) && "Second element should be top");
        
        std::cout << "Test " << (pq.top().second == 2 ? "passed" : "FAILED") << "\n";
    }
    else if (pqType == "Binary")
    {
        

        // Create PQ with position-aware comparator
        BinaryPQ<std::pair<int, int>, PositionAwareCompare> pq;
        
        // Push elements with same value but different positions
        pq.push({10, 1});  // Value 10, position 1
        assert(pq.top() == std::make_pair(10, 1) && "First element should be top");
        
        pq.push({10, 2});  // Value 10, position 2
        // BUG: Should select position 2 as top since it's "greater" with equal values
        // But the implementation only updates topIdx when strictly greater
        assert(pq.top() == std::make_pair(10, 2) && "Second element should be top");
        
        std::cout << "Test " << (pq.top().second == 2 ? "passed" : "FAILED") << "\n";
    }

    cout << "\n\n********** END: Testing " << pqType << "'s equal elements corner case succeeded! **********\n" << endl;
} // specialTests()



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
        assert(pq.top() == 10);  // This might fail if the bug exists
    }
    else if (pqType == "UnorderedOPT")
    {
        UnorderedFastPQ<int> pq;
        pq.push(10);
        pq.push(20);
        pq.pop();  // Remove 20
        pq.push(5);  // Push smaller value
        assert(pq.top() == 10);  // This might fail if the bug exists

        UnorderedFastPQ<int> pq2;
        pq2.push(10);
        pq2.push(5);
        pq2.pop();  // Remove 10
        pq2.push(4);  // Push smaller value
        assert(pq2.top() == 5);  // This might fail if the bug exists
    }
    else if (pqType == "Binary")
    {
        BinaryPQ<int> pq;
        pq.push(10);
        pq.push(20);
        pq.pop();  // Remove 20
        pq.push(5);  // Push smaller value
        assert(pq.top() == 10);  // This might fail if the bug exists

        BinaryPQ<int> pq2;
        pq2.push(10);
        pq2.push(5);
        pq2.pop();  // Remove 10
        pq2.push(4);  // Push smaller value
        assert(pq2.top() == 5);  // This might fail if the bug exists
    }

    cout << "\n\n********** END: Testing " << pqType << " special tests to reveal version-specific bugs succeeded! **********\n" << endl;
} // specialTests2()



int binTests() 
{
    cout << "\n\n********** START: Testing BinaryPQ **********\n" << endl;

    // Test 1: Basic operations with default constructor
    BinaryPQ<int> pq1;
    pq1.push(10);
    pq1.push(20);
    pq1.push(5);
    std::cout << "Test 1 - Top after pushes (5,10,20): " << pq1.top() << std::endl; // Should be 20
    assert(pq1.top() == 20);
    pq1.pop();
    std::cout << "Test 1 - Top after pop: " << pq1.top() << std::endl; // Should be 10
    assert(pq1.top() == 10);


    // Test 2: Range-based constructor
    std::vector<int> vec = {5, 2, 8, 1, 9, 3};
    BinaryPQ<int> pq2(vec.begin(), vec.end());
    std::cout << "Test 2 - Top from range (5,2,8,1,9,3): " << pq2.top() << std::endl; // Should be 9
    assert(pq2.top() == 9);


    // Test 3: bottomUp (priority increase simulation)
    BinaryPQ<int> pq3;
    pq3.push(10);
    pq3.push(20);
    pq3.push(5);
    // Simulate priority increase at index 2 (value 5 to something larger, say 25)
    pq3.push(25); // Adds at index 3, but for simulation, assume we modify index 2
    // In a real scenario, you'd modify data directly if priorities change, but for test, push and adjust
    std::cout << "Test 3 - Top before bottomUp: " << pq3.top() << std::endl; // Should be 25 or 20
    assert(pq3.top() == 25 || pq3.top() == 20);
    // pq3.updatePriorities(2); // Should have no effect unless data[2] changed; for demo, assume it’s updated
    // Note: For a real test, modify data[2] directly if priority changes


    // Test 4: topDown (priority decrease simulation)
    BinaryPQ<int> pq4;
    pq4.push(30);
    pq4.push(20);
    pq4.push(10);
    // Simulate priority decrease at root (index 0, value 30 to 5)
    // In real usage, you'd modify data[0] = 5, then call topDown(0)
    std::cout << "Test 4 - Top before topDown: " << pq4.top() << std::endl; // Should be 30
    assert(pq4.top() == 30);
    // Assume data[0] = 5; then call pq4.topDown(0); top should become 20
    // For simplicity, just pop and push to simulate
    pq4.pop();
    pq4.push(5);
    std::cout << "Test 4 - Top after simulated decrease: " << pq4.top() << std::endl; // Should be 20
    assert(pq4.top() == 20);


    // Test 5: updatePQ (rebuild heap)
    BinaryPQ<int> pq5;
    pq5.push(10);
    pq5.push(5);
    pq5.push(20);
    pq5.updatePriorities(); // Rebuild heap, should still have 20 as top
    std::cout << "Test 5 - Top after updatePQ: " << pq5.top() << std::endl; // Should be 20
    assert(pq5.top() == 20);

    cout << "\n\n********** END: Testing BinaryPQ **********\n" << endl;

    return 0;
} // binTests()



int main()
{
    // Basic pointer, allocate a new PQ later based on user choice.
    Eecs281PQ<int> *pq1;
    Eecs281PQ<int> *pq2;

    vector<string> types{ 
        "Unordered", 
        "UnorderedFast",
        "Sorted", 
        "Binary", 
        "Pairing", 
    };
    unsigned int choice;
    
    cout << "PQ tester" << endl << endl;
    for (size_t i = 0; i < types.size(); ++i)
        cout << "  " << i << ") " << types[i] << endl;
    cout << endl;
    cout << "Select one: ";
    cin >> choice;

    vector<int> tester = {1,2,3,8,7,6,13,42,65,43,87,45,56,35,29};
        
    // Send a subset = {6,...,87}, i.e., shrink the range
    // by setting a ptr to the 6th elt and the the 11th 
    // elt to get the subset [6, 45).
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
        pq1 = new UnorderedFastPQ<int>;
        pq2 = new UnorderedFastPQ<int>(start, end);
    } // else if
    else if (choice == 2)
    {
        pq1 = new SortedPQ<int>;
        pq2 = new SortedPQ<int>(start, end);
    } // else if
    else if (choice == 3)
    {
        binTests();
        pq1 = new BinaryPQ<int>;
        pq2 = new BinaryPQ<int>(start, end);
    } // else if
    else if (choice == 4)
    {
        vector<int> vec;
        vec.push_back(0);
        vec.push_back(1);
        testPairing(vec);

        pq1 = new PairingPQ<int>;
        pq2 = new PairingPQ<int>(start, end);
    } // else if
    else
    {
        cout << "Unknown container!" << endl << endl;
        exit(1);
    } // else
    
    // Send the pq.
    testPriorityQueue(types[choice]);
    testPriorityQueuePtr(pq1, types[choice]);
    testPQsCtor(pq2, types[choice]);
    
    // Start with a fresh pq once inside here.
    testUpdatePQ(types[choice]);

    // Test comparator robustness.
    testHiddenData(types[choice]);

    runEdgeTests(types[choice]);

    specialTests(types[choice]);

    specialTests2(types[choice]);
    
    // Clean up!
    delete pq1;
    delete pq2;

    cout << "\n\n********** ALL TESTS PASSED! **********\n" << endl;
    
    return 0;
} // main()














