// #include "MA_PQ.h"
#include "MB_PQ.h"
#include "SPsPQ.h"
#include "UnorderedSPsPQ.h" 
#include "UnorderedFastSPsPQ.h"
#include "BinPQ.h"
#include "SortedPQ.h"
// #include "PairingPQ.h"

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

/**
 * ********** OR **********
 * 
 * USE A LAMBDA FUNCTION INSTEAD OF A STRUCT LIKE BELOW.
 */

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
/*
    I DEFINED A CUSTOM COMPARATOR FOR THE "HIDDENDATA" CUSTOM TYPE, WHICH WILL TEST AND CONFIRM THE PQ IMPLEMENTATION HANDLES CUSTOM COMPARATORS AND DATA TYPES. O/W, IT WILL REVEAL BUGS THAT DONT OCCUR WITH PRIMITIVE TYPES. 
*/
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
            // Odd numbers come first
            if ((a.data % 2) != (b.data % 2)) 
                return (a.data % 2) < (b.data % 2); 

            // Then sort by value
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


    SPsPQ<HiddenData, HiddenDataMaxHeap> *pq = nullptr;
    SPsPQ<HiddenData, HDAbsComparator> *pq2 = nullptr;
    SPsPQ<HiddenData, OddFirstComp> *pq4 = nullptr;
    SPsPQ<HiddenData, decltype(customComp)> *pqL = nullptr;

    if (pqType == "Unordered")
    {
        pq = new UnorderedPQ<HiddenData, HiddenDataMaxHeap>;
        pq2 = new UnorderedPQ<HiddenData, HDAbsComparator>;
        pq4 = new UnorderedPQ<HiddenData, OddFirstComp>;
        pqL = new UnorderedPQ<HiddenData, decltype(customComp)>;
    }
    else if (pqType == "UnorderedOPT")
    {
        pq = new UnorderedPQOptimized<HiddenData, HiddenDataMaxHeap>;
        pq2 = new UnorderedPQOptimized<HiddenData, HDAbsComparator>;
        pq4 = new UnorderedPQOptimized<HiddenData, OddFirstComp>;
        pqL = new UnorderedPQOptimized<HiddenData, decltype(customComp)>;
    }
    else if (pqType == "Binary")
    {
        pq = new BinPQ<HiddenData, HiddenDataMaxHeap>;
        pq2 = new BinPQ<HiddenData, HDAbsComparator>;
        pq4 = new BinPQ<HiddenData, OddFirstComp>;
        pqL = new BinPQ<HiddenData, decltype(customComp)>;
    }
    else if (pqType == "Sorted")
    {
        pq = new SortedPQ<HiddenData, HiddenDataMaxHeap>;
        pq2 = new SortedPQ<HiddenData, HDAbsComparator>;
        pq4 = new SortedPQ<HiddenData, OddFirstComp>;
        pqL = new SortedPQ<HiddenData, decltype(customComp)>;
    }
    else if (pqType == "Pairing")
    {
        pq = new PairingPQ<HiddenData, HiddenDataMaxHeap>;
        pq2 = new PairingPQ<HiddenData, HDAbsComparator>;
        pq4 = new PairingPQ<HiddenData, OddFirstComp>;
        pqL = new PairingPQ<HiddenData, decltype(customComp)>;
    }


    /// @b Populate: pq
    pq->push(HiddenData(5));
    pq->push(HiddenData(10));
    pq->push(HiddenData(3));

    // Verify size and top element.
    assert(pq->getSize() == 3);
    assert(pq->getTop().data == 10);

    // Pop and check remaining elements
    pq->pop();
    assert(pq->getTop().data == 5);
    pq->pop();
    assert(pq->getTop().data == 3);
    pq->pop();
    assert(pq->isEmpty());



    /// @b Populate: pq2
    pq2->push(HiddenData(-10.5));
    pq2->push(HiddenData(5.2));
    pq2->push(HiddenData(-3.3));

    assert(pq2->getTop().dec_data == -10.5); // Largest absolute decimal
    pq2->pop();
    assert(pq2->getTop().dec_data == 5.2);



    /// @b Populate: pq4
    // Odds before evens:
    /**
     * Input: [4, 1, 3, 2]
     * Ordering:
     * 1. Odd numbers first: [1, 3, 4, 2]
     * 2. Within odd numbers, larger first: [3, 1, 4, 2]
     * 3. Within even numbers, larger first: [3, 1, 4, 2]
     * Final order: [3, 1, 4, 2]
     */
    pq4->push(HiddenData(4));
    pq4->push(HiddenData(1));
    pq4->push(HiddenData(3));
    pq4->push(HiddenData(2));

    assert(pq4->getTop().data == 3); 
    pq4->pop();
    assert(pq4->getTop().data == 1);
    pq4->pop();
    pq4->pop();
    assert(pq4->getTop().data == 2);
    pq4->pop();
    assert(pq4->isEmpty());



    /// @b Populate: pqL
    // Order by the last digit
    pqL->push(HiddenData(12));
    pqL->push(HiddenData(25));
    pqL->push(HiddenData(13));
    pqL->push(HiddenData(7));

    assert(pqL->getTop().data == 7);
    pqL->pop();
    assert(pqL->getTop().data == 25);
    pqL->pop();
    assert(pqL->getTop().data == 13);
    pqL->pop();
    assert(pqL->getTop().data == 12);
    pqL->pop();
    assert(pqL->isEmpty());

    delete pq;
    delete pq2;
    delete pq4;
    delete pqL;

    cout << "\n\n********** END: Testing " << pqType << " with hidden data succeeded! **********\n" << endl;
   
} // testHiddenData()



/// @brief Helper function to test the updatePQ() method. It corrupts the data 
///        and tests that the top and pop values are correct.
/// @param pq 
void testUpdatePQHelper(SPsPQ<int *, IntPtrComp> *pq, const string &pqType)
{
    // Create a vector of integers, then push the ints as references to the pq 
    // such that we can manually modify the ints and test that the top and 
    // pop values are correct.
    vector<int> data;
    data.reserve(100);

    // Populate the vector with 100 values.
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
        pq->push(&data[i]); // mutable references to the pq.
    
    // Change the first value (which is pointed to by the pq), and check it.
    data[0] = 10;
    pq->updatePQ();
    assert(*pq->getTop() == 90);

    if (pqType == "Sorted")
    {
        // Check the 2nd element
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(7)) == 80);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(6)) == 10);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(5)) == 9);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(4)) == 5);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(3)) == 4);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(2)) == 3);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(1)) == 2);
        assert(*(static_cast<SortedPQ<int *, IntPtrComp>*>(pq)->at(0)) == 1);
    }

    data[1] = 200;
    pq->updatePQ();
    assert(*pq->getTop() == 200);
    data[0] = 3000;
    pq->updatePQ();
    assert(*pq->getTop() == 3000);
    
} // testUpdatePQHelper()



/// TODO: Add more code to this function to test if updatePQ()
/// is working properly.
/// @note This function calls the testUpdatePQHelper function.
void testUpdatePQ(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << "'s updatePQ() method by manually modifying values **********\n" << endl;

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
    else if (pqType == "Sorted")
    {
        pq = new SortedPQ<int *, IntPtrComp>;
    } // else if
    else if (pqType == "Pairing")
    {
        pq = new PairingPQ<int *, IntPtrComp>;
    } // else if
    
    if (!pq)
    {
        cout << "Invalid pq pointer; did you forget to create it?" << endl;
        return;
    } // if
    
    testUpdatePQHelper(pq, pqType);

    cout << "\n\n********** END: Testing " << pqType << "'s updatePQ() method by manually modifying values succeeded! **********\n" << endl;

    delete pq;
} // testUpdatePQ()



/// @brief Test the basic functionality of the priority 
///         queue pointer version.
/// @param pq The priority queue to test.
/// @param pqType The type of priority queue to test.
/// @note This function tests the basic functionality of 
///       the priority queue pointer version:
///       It tests the push, pop, and getTop functions.
///       It also tests the isEmpty function.
///       It also tests the updatePQ function.
void testPriorityQueuePtr(SPsPQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (pointer version) **********\n" << endl;

    // Very basic testing.
    cout << "Test 1: integer values on getSize(), getTop(), push(), pop(), and isEmpty()...\n"
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

    cout << "Test 1: integer value tests passed!\n"
         << endl;

    // TODO: Add more testing here!

    // More focused unit testing.

    // Large range of values
    cout << "Test 2: extrame values...\n"
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
    cout << "Test 2: extreme value tests passed!\n"
         << endl;

    // Sorted input
    cout << "Test 3: sorted values...\n"
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
    cout << "Test 3: sorted value tests passed!\n"
         << endl;

    // Reverse-sorted input
    cout << "Test 4: reversed sorted values...\n"
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
    cout << "Test 4: reversed sorted value tests passed!\n"
         << endl;

    // Mix of positive and negative numbers
    cout << "Test 5: both +/- values...\n"
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
    cout << "Test 5: both +/- value tests passed!\n"
         << endl;

    // Testing `updatePQ()` after manual data modification
    /*
        I.e., simulate a scenario where updatePQ() is called explicitly and verify heap integrity afterward.
    */
    cout << "Test 6: manually modifying the values...\n"
         << endl;
    pq->push(100);
    pq->push(200);
    pq->push(50);
    pq->push(75);
    // Simulate external modification twice.
    pq->updatePQ();
    assert(pq->getTop() == 200);
    pq->pop();
    assert(pq->getTop() == 100);
    pq->updatePQ();
    pq->pop();
    assert(pq->getTop() == 75);
    cout << "Test 6: manual value modification tests passed!\n"
         << endl;


    cout << "\n\n********** END: Testing " << pqType << " basic functionality (pointer version) succeeded! **********\n" << endl;
} // testPriorityQueuePtr()



/// @brief  Testing the sorted priority queue.
/// @param pq 
/// @param pqType 
void testSortedPQ(SPsPQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " PQ's sorted-specific functionality **********\n" << endl;


    /// NOTE: WHAT SHOULD I DO WITH THE PTR??? MAYBE TRY SOME COPYING AND ASSIGNMENT TESTS??? 

    /// A: MAKE SOME TESTS FOR THE RANGE-BASED CONSTRUCTOR AND PQ2! LOOK AT testPairing() TO SEE HOW TO DO IT! 
    
    
    // Test the default constructor
    cout << "Test 1: the default constructor's basic functionality...\n" << endl;
    SortedPQ<int> pq2;
    assert(pq2.isEmpty());
    assert(pq2.getSize() == 0);
    pq2.push(10);
    assert(!pq2.isEmpty());
    assert(pq2.getTop() == 10);
    pq2.pop();
    assert(pq2.isEmpty());
    cout << "Test 1: default constructor tests passed!\n" << endl;



    // Test Range Constructor
    cout << "Test 2: the range-based integer constructor...\n" << endl;
    cout << "    2.1. With an empty pq...\n" << endl;
    std::vector<int> emptyVec;
    SortedPQ<int> pqEmpty(emptyVec.begin(), emptyVec.end());
    assert(pqEmpty.isEmpty());
    cout << "        Empty range test passed!\n" << endl;


    cout << "    2.2. With a pre-sorted pq...\n" << endl;
    std::vector<int> sortedVec = {1, 2, 3, 4, 5};
    SortedPQ<int> pqSorted(sortedVec.begin(), sortedVec.end());
    assert(pqSorted.getSize() == 5);
    assert(pqSorted.getTop() == 5);
    cout << "        Pre-sorted range test passed!\n" << endl;


    cout << "    2.3. With a Reverse-sorted pq...\n" << endl;
    std::vector<int> reverseSortedVec = {7, 5, 4, 3, 2, 1};
    SortedPQ<int> pqReverse(reverseSortedVec.begin(), reverseSortedVec.end());
    assert(pqReverse.getSize() == 6);
    assert(pqReverse.getTop() == 7);
    cout << "        Reverse-sorted range test passed!\n" << endl;


    cout << "    2.4. With duplicates...\n" << endl;
    std::vector<int> duplicateVec = {3, 3, 3, 3};
    SortedPQ<int> pqDuplicates(duplicateVec.begin(), duplicateVec.end());
    assert(pqDuplicates.getSize() == 4);
    assert(pqDuplicates.getTop() == 3);
    cout << "        Range with duplicates test passed!\n" << endl;




    // Test Custom Comparator
    cout << "Test 3: testing with a min-heap type custom comparator...\n" << endl;
    cout << "    3.1. Basic min extraction w/ push()...\n" << endl;
    auto greaterComparator = [](int a, int b) { return a > b; };
    SortedPQ<int, decltype(greaterComparator)> pq3(greaterComparator);
    pq3.push(1);
    pq3.push(5);
    pq3.push(3);
    assert(pq3.getTop() == 1); // Smallest element should now be at the "top"
    pq3.pop();
    assert(pq3.getTop() == 3);
    cout << "        Basic min extraction test passed!\n" << endl;



    
    /// NOTE: the following syntax for lambda and decltype

    /*
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



    // Test Random Inputs With Comparator
    cout << "    3.2. Random inputs w/ the range-based ctor & min-heapPQ...\n" << endl;
    std::vector<int> randomVec = {50, 20, 40, 10, 30};
    auto customComparator = [](int a, int b) { return a > b; }; // Min-heap
    SortedPQ<int, decltype(customComparator)> pq6(randomVec.begin(), randomVec.end(), customComparator);
    assert(pq6.getTop() == 10); // Smallest element
    pq6.pop();
    assert(pq6.getTop() == 20);
    cout << "        Random inputs w/ the range-based ctor & min-heapPQ test passed!" << endl;    
    




    cout << "Test 4: testing with non-integer values...\n" << endl;

    SPsPQ<double> *pqq = new SortedPQ<double>;

    // Very basic testing.
    cout << "    4.1. Testing basic functionality...\n" << endl;
    pqq->push(3.5);
    pqq->push(2.75);
    pqq->push(4.25);
    pqq->push(4.26);
    assert(pqq->getSize() == 4);
    assert(pqq->getTop() == 4.26);
    
    pqq->pop();
    assert(pqq->getSize() == 3);
    assert(pqq->getTop() == 4.25);
    
    pqq->pop();
    assert(pqq->getSize() == 2);
    assert(pqq->getTop() == 3.5);
    
    pqq->pop();
    pqq->pop();
    assert(pqq->isEmpty());
    cout << "    4.1. Testing basic functionality passed!\n" << endl;

    
    cout << "\nTest 4: testing with non-integer values succeeded!\n" << endl;




    // Test Update Priorities Empty
    cout << "Test 5: testing updatePQ() on an empty queue...\n" << endl;
    SortedPQ<int> pq4;
    assert(pq4.isEmpty() && "Empty queue should report empty");
    try {
        pq4.updatePQ();
        cout << "Test 5 failed: Expected exception on empty updatePQ\n";
    } catch (const std::runtime_error&) {
        cout << "Test 5 passed: Empty queue throws on updatePQ\n";
    }

    pq4.push(1);
    pq4.push(2);
    pq4.push(3);
    pq4.updatePQ();
    assert(pq4.getTop() == 3);
    pq4.pop();
    assert(pq4.getTop() == 2);


    
    if (pq->isEmpty())
    {
        // Test Update Priorities Empty
        cout << "Test 6: assignment operator...\n" << endl;
        /**
         * In this case, since pq is a pointer to SPsPQ<int> 
         * and pq4 is a SortedPQ<int>, you need to use the 
         * address-of operator to get the address of pq4.
         */
        pq = &pq4;
        pq->push(0);
        pq->push(-1);   // pq = [-1, 0, 1, 2]
        assert(pq->getTop() == 2);
        
        pq->pop();
        pq->pop();
        pq->pop();
        assert(pq->getTop() == -1);
        pq->pop();
        assert(pq->isEmpty());
        
        cout << "\nTest 6: assignment operator succeeded!\n" << endl;
    }
    else
    {
        // Test Update Priorities Empty
        cout << "Test 6: ranged-based pointer copy constructor...\n" << endl;
        
        // Send a subset = {6,...,87}, i.e., shrink the range
        // by setting a ptr to the 6th elt and the the 11th
        // elt to get the subset [6, 45).
        SPsPQ<int> *pq2ptr = new SortedPQ<int>(*((SortedPQ<int> *)pq));
        SortedPQ<int> *pq3 = new SortedPQ<int>();
        *pq3 = *((SortedPQ<int> *)pq2ptr);

        assert(pq3->getSize() == 6);
        assert(pq3->getTop() == 87);
        
        pq3->pop(); // 87
        pq3->pop(); // 65
        pq3->pop(); // 43
        assert(pq3->getSize() == 3);
        assert(pq3->getTop() == 42);
        assert(!pq3->isEmpty());
        
        pq3->pop();
        assert(pq3->getTop() == 13);
        pq3->pop();
        assert(pq3->getTop() == 6);
        assert(!pq3->isEmpty());

        pq3->pop();
        assert(pq3->isEmpty());

        // Empty out the original ranged based pq
        pq = pq3;
        assert(pq->getSize() == 0);
        assert(pq->isEmpty());

        delete pq2ptr;
        delete pq3;
        
        cout << "\nTest 6: ranged-based pointer copy constructor succeeded!\n" << endl;

    }
    

    // Test 7: Multiple elements - verify sorted order
    cout << "Test 7: multiple elements in sorted order..."
         << endl;
    // pq2 is empty by the time we get here. It was emptied on the 1st test.
    // SortedPQ<int> pq2;
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
    
    cout << "Test 7: multiple elements in sorted order succeeded!\n"
         << endl;
    


    // Test 8: Pop sequence verification
    cout << "Test 8: pop sequence verification..."
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
    cout << "Test 8: pop sequence verification succeeded!\n"
         << endl;
    


    // Test 9: Range-based constructor with duplicates
    cout << "Test 9: range-based constructor with duplicates..."
         << endl;
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    pq = new SortedPQ<int>(arr, arr + 9);
    assert(pq->getSize() == 9 && "Size should match range");
    assert(pq->getTop() == 9 && "Top should be max (9)");
    
    // Verify sorted order: [1, 1, 2, 3, 4, 5, 5, 6, 9]
    // Cast to SortedPQ* to resolve slicing, e.g., to access at()
    assert(static_cast<SortedPQ<int>*>(pq)->at(0) == 1);
    assert(static_cast<SortedPQ<int>*>(pq)->at(1) == 1);
    assert(static_cast<SortedPQ<int>*>(pq)->at(2) == 2);
    assert(static_cast<SortedPQ<int>*>(pq)->at(8) == 9);
    
    cout << "Test 9: range-based constructor with duplicates succeeded!\n"
         << endl;
    


    // Test 10: All equal elements
    cout << "Test 10: all equal elements..."
         << endl;
    SortedPQ<int> pq10;
    for (int i = 0; i < 5; ++i) pq10.push(7);
    assert(pq10.getSize() == 5 && "Size should be 5");
    assert(pq10.getTop() == 7 && "Top should be 7");
    
    // All elements should be 7
    for (std::size_t i = 0; i < 5; ++i) {
        assert(pq10.at(i) == 7);
    }
    
    pq10.pop();
    assert(pq10.getSize() == 4 && "Size should decrease");
    assert(pq10.getTop() == 7 && "Top should still be 7");
    cout << "Test 10 passed!\n"
         << endl;




    // Test 11: Push after pop to zero
    cout << "Test 11: push after pop to zero..."
         << endl;
    SortedPQ<int> pq11;
    pq11.push(1);
    pq11.pop();
    assert(pq11.isEmpty() && "Should be empty after pop");
    pq11.push(2);
    assert(pq11.getTop() == 2 && "New push should work");
    cout << "Test 11 passed!\n"
         << endl;




    // Test 12: Sorted insertion order maintenance
    cout << "Test 12: sorted insertion order maintenance..."
         << endl;
    SortedPQ<int> pq12;
    vector<int> test_vals = {50, 25, 75, 10, 60, 30, 80};
    
    for (int val : test_vals) {
        pq12.push(val);
    }
    
    // Values should be sorted: [10, 25, 30, 50, 60, 75, 80]
    vector<int> expected = {10, 25, 30, 50, 60, 75, 80};
    for (std::size_t i = 0; i < expected.size(); ++i) {
        assert(pq12.at(i) == expected[i]);
    }
    
    cout << "Test 12 passed!\n"
         << endl;




    // Test 13: Large number of elements
    cout << "Test 13: large number of elements..."
         << endl;
    SortedPQ<int> pq13;
    for (int i = 999; i >= 0; --i) pq13.push(i); // Insert in reverse order
    assert(pq13.getSize() == 1000 && "Size should be 1000");
    assert(pq13.getTop() == 999 && "Top should be max (999)");
    
    // First few elements should be 0, 1, 2...
    assert(pq13.at(0) == 0);
    assert(pq13.at(1) == 1);
    assert(pq13.at(2) == 2);
    
    cout << "Test 13 passed!\n"
         << endl;
    



    
    /// TODO: Add more assign, move, and copy testing here!
    
    cout << "\n\n********** END: Testing " << pqType << " PQ's sorted functionality succeeded! **********\n" << endl;
    
} // testSortedPQ()



/// @brief Test function for SortedPQ::updatePQ() method.
/// @note This function tests the updatePQ method by:
///       1. Creating a sorted priority queue (default is max-heap behavior)
///       2. Pushing valid elements
///       3. Corrupting the internal data
///       4. Calling updatePQ to fix the corruption
///       5. Verifying correct operation after repair
void testSortedPQUpdateCornerCase() 
{
    std::cout << "\n=== Testing SortedPQ updatePQ() Corner Case ===" << std::endl;
    
    // Create a max-heap (using default std::less - highest value has highest priority)
    SortedPQ<int> pq;
    
    // Step 1: Push some values to create a valid sorted PQ
    std::cout << "Step 1: Pushing values 10, 30, 20, 50, 40..." << std::endl;
    pq.push(10);
    pq.push(30);
    pq.push(20);
    pq.push(50);
    pq.push(40);
    
    std::cout << "After pushing: ";
    pq.print();
    std::cout << "Top element: " << pq.getTop() << " (should be 50)" << std::endl;
    
    // Step 2: Corrupt the internal data
    std::cout << "\nStep 2: Corrupting internal data..." << std::endl;
    std::vector<int>& data = pq.getInternalData();
    
    // Current correct order: [10, 20, 30, 40, 50]
    // Corrupt by reversing the order
    std::reverse(data.begin(), data.end());
    // Now corrupted to: [50, 40, 30, 20, 10]
    
    std::cout << "After corruption: ";
    pq.print();
    std::cout << "Top element: " << pq.getTop() << " (incorrectly shows " << pq.getTop() << ")" << std::endl;
    
    // Step 3: Call updatePQ to fix the corruption
    std::cout << "\nStep 3: Calling updatePQ() to fix corruption..." << std::endl;
    pq.updatePQ();
    
    std::cout << "After updatePQ: ";
    pq.print();
    
    // Step 4: Verify correct operation
    std::cout << "\nStep 4: Verifying correct operation..." << std::endl;
    std::cout << "Top element: " << pq.getTop() << " (should be 50)" << std::endl;
    
    // Pop all elements and verify they come out in correct order
    std::cout << "Popping all elements: ";
    std::vector<int> poppedOrder;
    while (!pq.isEmpty()) {
        poppedOrder.push_back(pq.getTop());
        pq.pop();
    }
    
    // Display popped order
    for (size_t i = 0; i < poppedOrder.size(); ++i) {
        std::cout << poppedOrder[i];
        if (i < poppedOrder.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    
    // Verify the order was correct (should be 50, 40, 30, 20, 10 for max-heap)
    bool testPassed = true;
    std::vector<int> expectedOrder = {50, 40, 30, 20, 10};
    for (size_t i = 0; i < expectedOrder.size(); ++i) {
        if (poppedOrder[i] != expectedOrder[i]) {
            testPassed = false;
            break;
        }
    }
    
    std::cout << "\nTest Result: " << (testPassed ? "PASSED" : "FAILED") << std::endl;
    
    // Additional test: updatePQ on empty queue
    std::cout << "\nAdditional test: updatePQ on empty queue..." << std::endl;
    SortedPQ<int> emptyPQ;
    emptyPQ.updatePQ(); // Should not crash
    std::cout << "Empty PQ after updatePQ: ";
    emptyPQ.print();
    
    std::cout << "\n=== Test Complete ===" << std::endl;
} // testSortedPQUpdateCornerCase()



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
} // runSortedPQEdgeTests()



/// @brief Test the unsorted heap's range-based constructor, 
///         copy constructor, and operator=().
/// @param pq The priority queue to test.
/// @param pqType The type of priority queue to test.
/// @note This function tests a subset = {6,...,87}, i.e., shrink the  
///       range by setting a ptr to the 6th element and the the 11th
///       element yields the subset [6, 45), which was done in main().
void testPQsCtor(SPsPQ<int> *pq, const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << "'s range-based constructor **********\n" << endl;

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



/// @b Test the basic functionality of the priority queue regular version.
/// @param pqType The type of priority queue to test.
/// @note This function tests the basic functionality of the priority queue regular version.
///       It tests the push, pop, and getTop functions.
///       It also tests the isEmpty function.
///       It also tests the updatePQ function.
///       It also tests the range-based constructor.
void testPriorityQueue(const string &pqType) 
{
    cout << "\n\n********** START: Testing " << pqType << " basic functionality (regular version) **********\n" << endl;

    SPsPQ<int> *pq1 = nullptr;
    SPsPQ<int> *pq2 = nullptr;
    SPsPQ<int> *pq3 = nullptr;
    SPsPQ<int> *pq4 = nullptr;
    SPsPQ<int> *pq5 = nullptr;

    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    if (pqType == "Unordered")
    {
        // Test 1: Empty queue
        pq1 = new UnorderedPQ<int>;

        // Test 3: Range-based constructor with duplicates
        pq2 = new UnorderedPQ<int>(arr, arr + 9);

        // Test 4: All equal elements
        pq3 = new UnorderedPQ<int>;

        // Test 5: Push after pop to zero
        pq4 = new UnorderedPQ<int>;

        // Test 6: Large number of elements
        pq5 = new UnorderedPQ<int>;
    }
    else if (pqType == "UnorderedOPT")
    {
        // Test 1: Empty queue
        pq1 = new UnorderedPQOptimized<int>;

        // Test 3: Range-based constructor with duplicates
        pq2 = new UnorderedPQOptimized<int>(arr, arr + 9);

        // Test 4: All equal elements
        pq3 = new UnorderedPQOptimized<int>;

        // Test 5: Push after pop to zero
        pq4 = new UnorderedPQOptimized<int>;

        // Test 6: Large number of elements
        pq5 = new UnorderedPQOptimized<int>;
    }
    else if (pqType == "Binary")
    {
        // Test 1: Empty queue
        pq1 = new BinPQ<int>;

        // Test 3: Range-based constructor with duplicates
        pq2 = new BinPQ<int>(arr, arr + 9);

        // Test 4: All equal elements
        pq3 = new BinPQ<int>;

        // Test 5: Push after pop to zero
        pq4 = new BinPQ<int>;

        // Test 6: Large number of elements
        pq5 = new BinPQ<int>;
    }
    else if (pqType == "Sorted")
    {
        // Test 1: Empty queue
        pq1 = new SortedPQ<int>;

        // Test 3: Range-based constructor with duplicates
        pq2 = new SortedPQ<int>(arr, arr + 9);

        // Test 4: All equal elements
        pq3 = new SortedPQ<int>;

        // Test 5: Push after pop to zero
        pq4 = new SortedPQ<int>;

        // Test 6: Large number of elements
        pq5 = new SortedPQ<int>;
    }
    else if (pqType == "Pairing")
    {
        // Test 1: Empty queue
        pq1 = new PairingPQ<int>;

        // Test 3: Range-based constructor with duplicates
        pq2 = new PairingPQ<int>(arr, arr + 9);

        // Test 4: All equal elements
        pq3 = new PairingPQ<int>;

        // Test 5: Push after pop to zero
        pq4 = new PairingPQ<int>;

        // Test 6: Large number of elements
        pq5 = new PairingPQ<int>;
    }


    // Test 1: Empty queue
    cout << "Test 1: empty queue..."
        << endl;
    assert(pq1->isEmpty() && "Empty queue should report empty");
    try {
        pq1->getTop();
        cout << "Test 1 failed: Expected exception on empty getTop\n";
    } catch (const runtime_error&) {
        cout << "Test 1 passed: Empty queue throws on getTop\n";
    }

    try {
        pq1->pop(); // Should not crash on empty queue
        cout << "Test 1 failed: Expected exception on empty pop\n";
    } catch (const runtime_error&) {
        cout << "Test 1 passed: Empty queue throws on pop\n";
    }
    
    assert(pq1->isEmpty() && "Queue should still be empty after pop on empty");


    // Test 2: Single element
    cout << "Test 2: single element..."
        << endl;
    pq1->push(42);
    assert(pq1->getSize() == 1 && "Size should be 1");
    assert(pq1->getTop() == 42 && "Top should be 42");
    pq1->pop();
    assert(pq1->isEmpty() && "Queue should be empty after pop");

    // Push after emptying
    pq1->push(100);
    assert(pq1->getTop() == 100 && "Top should be 100");
    cout << "Test 2 passed!\n"
        << endl;


    // Test 3: Range-based constructor with duplicates
    cout << "Test 3: range-based constructor with duplicates..."
        << endl;
    assert(pq2->getSize() == 9 && "Size should match range");
    assert(pq2->getTop() == 9 && "Top should be max (9)");
    pq2->pop();
    assert(pq2->getTop() == 6 && "Next top should be 6 after pop");
    cout << "Test 3 passed!\n"
        << endl;


    // Test 4: All equal elements
    cout << "Test 4: all equal elements..."
        << endl;
    for (int i = 0; i < 5; ++i) pq3->push(7);
    assert(pq3->getSize() == 5 && "Size should be 5");
    assert(pq3->getTop() == 7 && "Top should be 7");
    pq3->pop();
    assert(pq3->getSize() == 4 && "Size should decrease");
    assert(pq3->getTop() == 7 && "Top should still be 7");
    while (!pq3->isEmpty()) {
        assert(pq3->getTop() == 7 && "All elements should be 7");
        pq3->pop();
    }
    cout << "Test 4 passed!\n"
        << endl;


    // Test 5: Push after pop to zero
    cout << "Test 5: push after pop to zero..."
        << endl;
    pq4->push(1);
    pq4->pop();
    assert(pq4->isEmpty() && "Should be empty after pop");
    pq4->push(2);
    assert(pq4->getTop() == 2 && "New push should work");
    cout << "Test 5 passed!\n"
        << endl;


    // Test 6: Large number of elements
    cout << "Test 6: large number of elements..."
        << endl;
    for (int i = 0; i < 1000; ++i) pq5->push(i);
    assert(pq5->getSize() == 1000 && "Size should be 1000");
    assert(pq5->getTop() == 999 && "Top should be max (999)");
    cout << "Test 6 passed!\n"
        << endl;


    // Test 7: Multiple elements in ascending order
    cout << "Test 7: Multiple elements - ascending order push..."
        << endl;
    pq4->pop(); // Empty queue 4
    assert(pq4->isEmpty() && "Queue should be empty");
    for (int i = 1; i <= 5; ++i) {
        pq4->push(i);
        assert(pq4->getTop() == i && "Top should be current max");
    }
    assert(pq4->getSize() == 5 && "Size should be 5");
    
    // Pop all and verify order
    for (int i = 5; i >= 1; --i) {
        assert(pq4->getTop() == i && "Top should match expected");
        pq4->pop();
    }
    assert(pq4->isEmpty() && "Queue should be empty");
    cout << "Test 7 passed!\n"
        << endl;


    // Test 8: Multiple elements in descending order
    cout << "Test 8: Multiple elements - descending order push..."
        << endl;
    for (int i = 5; i >= 1; --i) {
        pq4->push(i);
        assert(pq4->getTop() == 5 && "Top should always be 5");
    }
    assert(pq4->getSize() == 5 && "Size should be 5");
    cout << "Test 8 passed!\n"
        << endl;


    // Test 9: Random order insertions
    cout << "Test 9: Random order insertions..."
        << endl;
    
    // Empty queue 4
    for (int i = 0; i < 5; i++) 
        pq4->pop();
    
    assert(pq4->isEmpty() && "Queue should be empty");
    // Populate the queue
    vector<int> values = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    for (int val : values) 
        pq4->push(val);

    assert(pq4->getSize() == 9 && "Size should be 9");
    assert(pq4->getTop() == 9 && "Top should be 9");
    
    // Pop and verify descending order
    int prev = pq4->getTop();
    pq4->pop();
    int count = 1;
    
    while (!pq4->isEmpty()) {
        assert(pq4->getTop() <= prev && "Elements should come out in descending order");
        prev = pq4->getTop();
        pq4->pop();
        count++;
    }
    assert(count == 9 && "Should have popped 9 elements");
    cout << "Test 9 passed!\n"
        << endl;


    delete pq1;
    delete pq2;
    delete pq3;
    delete pq4;
    delete pq5;

    cout << "\n\n********** END: Testing " << pqType << " basic functionality (regular version) succeeded! **********\n" << endl;
} // testPriorityQueue()



void runEdgeTests(const string &pqType) 
{
    cout << "\n\n********** START: Testing " << pqType << " edge tests to reveal version-specific bugs (regular version) **********\n" << endl;

    SPsPQ<int> *pq1 = nullptr;
    SPsPQ<int> *pq2 = nullptr;
    SPsPQ<int> *pq3 = nullptr;
    SPsPQ<int> *pq4 = nullptr;
    SPsPQ<int> *pq5 = nullptr;

    int arr[] = {4, 4, 4, 4};

    if (pqType == "Unordered")
    {
        // Test 1: Push-pop-push with equal elements
        pq1 = new UnorderedPQ<int>();

        // Test 2: Pop all elements then push new max
        pq2 = new UnorderedPQ<int>();

        // Test 3: Push lower value after pop, check top
        pq3 = new UnorderedPQ<int>();
        
        // Test 4: Repeated pop with duplicates
        pq4 = new UnorderedPQ<int>();

        // Test 5: Range init with all equal, pop all
        pq5 = new UnorderedPQ<int>(arr, arr + 4);
    }
    else if (pqType == "UnorderedOPT")
    {
        // Test 1: Push-pop-push with equal elements
        pq1 = new UnorderedPQOptimized<int>();

        // Test 2: Pop all elements then push new max
        pq2 = new UnorderedPQOptimized<int>();

        // Test 3: Push lower value after pop, check top
        pq3 = new UnorderedPQOptimized<int>();
        
        // Test 4: Repeated pop with duplicates
        pq4 = new UnorderedPQOptimized<int>();

        // Test 5: Range init with all equal, pop all
        pq5 = new UnorderedPQOptimized<int>(arr, arr + 4);
    }
    else if (pqType == "Binary")
    {
        // Test 1: Push-pop-push with equal elements
        pq1 = new BinPQ<int>();

        // Test 2: Pop all elements then push new max
        pq2 = new BinPQ<int>();

        // Test 3: Push lower value after pop, check top
        pq3 = new BinPQ<int>();
        
        // Test 4: Repeated pop with duplicates
        pq4 = new BinPQ<int>();

        // Test 5: Range init with all equal, pop all
        pq5 = new BinPQ<int>(arr, arr + 4);
    }
    else if (pqType == "Sorted")
    {
        // Test 1: Push-pop-push with equal elements
        pq1 = new SortedPQ<int>();

        // Test 2: Pop all elements then push new max
        pq2 = new SortedPQ<int>();

        // Test 3: Push lower value after pop, check top
        pq3 = new SortedPQ<int>();
        
        // Test 4: Repeated pop with duplicates
        pq4 = new SortedPQ<int>();

        // Test 5: Range init with all equal, pop all
        pq5 = new SortedPQ<int>(arr, arr + 4);
    }
    else if (pqType == "Pairing")
    {
        // Test 1: Push-pop-push with equal elements
        pq1 = new PairingPQ<int>();

        // Test 2: Pop all elements then push new max
        pq2 = new PairingPQ<int>();

        // Test 3: Push lower value after pop, check top
        pq3 = new PairingPQ<int>();

        // Test 4: Repeated pop with duplicates
        pq4 = new PairingPQ<int>();

        // Test 5: Range init with all equal, pop all
        pq5 = new PairingPQ<int>(arr, arr + 4);
    }
    else
    {
        delete pq1;
        delete pq2;
        delete pq3;
        delete pq4;
        delete pq5;
        return;
    }


    // Test 1: Push-pop-push with equal elements
    {
        cout << "Test 1: Push-pop-push with equal elements\n";
        pq1->push(5);
        pq1->push(5);  // Two equal max elements
        assert(pq1->getTop() == 5 && "Top should be 5");
        pq1->pop();    // Remove one 5
        assert(pq1->getTop() == 5 && "Top should still be 5");
        pq1->push(5);  // Add another 5
        assert(pq1->getTop() == 5 && "Top should remain 5");
        cout << "Test 1 passed!\n"
            << endl;
    }

    // Test 2: Pop all elements then push new max
    {
        cout << "Test 2: Pop all elements then push new max\n";
        pq2->push(3);
        pq2->push(4);
        pq2->pop();  // Remove 4
        pq2->pop();  // Remove 3
        assert(pq2->isEmpty() && "Queue should be empty");
        pq2->push(5);  // New max after empty
        assert(pq2->getTop() == 5 && "Top should be 5");
        cout << "Test 2 passed\n";
    }

    // Test 3: Push lower value after pop, check top
    {
        cout << "Test 3: Push lower value after pop\n";
        pq3->push(10);
        pq3->push(8);
        pq3->pop();  // Remove 10
        assert(pq3->getTop() == 8 && "Top should be 8");
        pq3->push(5);  // Lower than current top
        assert(pq3->getTop() == 8 && "Top should still be 8");
        cout << "Test 3 passed\n";
    }

    // Test 4: Repeated pop with duplicates
    {
        cout << "Test 4: Repeated pop with duplicates\n";
        pq4->push(7);
        pq4->push(7);
        pq4->push(7);  // Three equal max elements
        pq4->pop();    // Remove one 7
        assert(pq4->getTop() == 7 && "Top should be 7");
        pq4->pop();    // Remove another 7
        assert(pq4->getTop() == 7 && "Top should still be 7");
        pq4->pop();    // Remove last 7
        assert(pq4->isEmpty() && "Queue should be empty");
        cout << "Test 4 passed\n";
    }

    // Test 5: Range init with all equal, pop all
    {
        cout << "Test 5: Range init with all equal, pop all\n";
        assert(pq5->getTop() == 4 && "Top should be 4");
        pq5->pop();
        assert(pq5->getTop() == 4 && "Top should be 4");
        pq5->pop();
        pq5->pop();
        pq5->pop();
        assert(pq5->isEmpty() && "Queue should be empty");
        cout << "Test 5 passed\n";
    }

    delete pq1;
    delete pq2;
    delete pq3;
    delete pq4;
    delete pq5;

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

    SPsPQ<pair<int, int>, PositionAwareCompare> *pq1 = nullptr;


    if (pqType == "Unordered")
    {
        // Create PQ with position-aware comparator
        pq1 = new UnorderedPQ<pair<int, int>, PositionAwareCompare>();
    }
    else if (pqType == "UnorderedOPT")
    {
        // Create PQ with position-aware comparator
        pq1 = new UnorderedPQOptimized<pair<int, int>, PositionAwareCompare>();
    }
    else if (pqType == "Binary")
    {
        // Create PQ with position-aware comparator
        pq1 = new BinPQ<pair<int, int>, PositionAwareCompare>();
    }
    else if (pqType == "Sorted")
    {
        // Create PQ with position-aware comparator
        pq1 = new SortedPQ<pair<int, int>, PositionAwareCompare>();
    }
    else if (pqType == "Pairing")
    {
        pq1 = new PairingPQ<pair<int, int>, PositionAwareCompare>();
    }
    else
    {
        delete pq1;
        return;
    }

    // Push elements with same value but different positions
    pq1->push({10, 1});  // Value 10, position 1
    assert(pq1->getTop() == std::make_pair(10, 1) && "First element should be top");
    
    pq1->push({10, 2});  // Value 10, position 2
    // BUG: Should select position 2 as top since it's "greater" with equal values
    // But the implementation only updates topIdx when strictly greater
    assert(pq1->getTop() == std::make_pair(10, 2) && "Second element should be top");
    
    cout << "Test " << (pq1->getTop().second == 2 ? "passed" : "FAILED") << "\n";

    delete pq1;

    cout << "\n\n********** END: Testing " << pqType << "'s equal elements corner case succeeded! **********\n" << endl;
} // specialTests()



void specialTests2(const string &pqType)
{
    cout << "\n\n********** START: Testing " << pqType << " special tests to reveal version-specific bugs **********\n" << endl;

    SPsPQ<int> *pq1 = nullptr;
    SPsPQ<int> *pq2 = nullptr;

    if (pqType == "Unordered")
    {
        pq1 = new UnorderedPQ<int>();
        pq2 = new UnorderedPQ<int>();
    }
    else if (pqType == "UnorderedOPT")
    {
        pq1 = new UnorderedPQOptimized<int>();
        pq2 = new UnorderedPQOptimized<int>();
    }
    else if (pqType == "Binary")
    {
        pq1 = new BinPQ<int>();
        pq2 = new BinPQ<int>();
    }
    else if (pqType == "Sorted")
    {
        pq1 = new SortedPQ<int>();
        pq2 = new SortedPQ<int>();
    }
    else if (pqType == "Pairing")
    {
        pq1 = new PairingPQ<int>();
        pq2 = new PairingPQ<int>();
    }
    else
    {
        delete pq1;
        delete pq2;
        return;
    }

    pq1->push(10);
    pq1->push(20);
    pq1->pop();  // Remove 20
    pq1->push(5);  // Push smaller value
    assert(pq1->getTop() == 10);  // This might fail if the bug exists

    pq2->push(10);
    pq2->push(5);
    pq2->pop();  // Remove 10
    pq2->push(4);  // Push smaller value
    assert(pq2->getTop() == 5);  // This might fail if the bug exists

    delete pq1;
    delete pq2;

    cout << "\n\n********** END: Testing " << pqType << " special tests to reveal version-specific bugs succeeded! **********\n" << endl;
} // specialTests2()



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
    assert(pq3.getTop() == 25);
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
} // binTests()



void additionalBinPQEdgeTests() 
{
    cout << "\n\n********** START: Additional BinPQ Edge Tests **********\n" << endl;

    // Test 1: Heap property verification after multiple operations
    {
        cout << "Test 1: Verify heap property maintained after complex operations, and updatePQ heap property maintenance\n";
        BinPQ<int> pq;
        vector<int> values = {15, 10, 20, 8, 2, 12, 25};
        
        for (int val : values) {
            pq.push(val);
        }
        
        // Extract all elements and verify they come out in sorted order
        vector<int> extracted;
        while (!pq.isEmpty()) {
            extracted.push_back(pq.getTop());
            pq.pop();
        }
        
        // Should be in descending order
        assert(std::is_sorted(extracted.rbegin(), extracted.rend()));


        // Test updatePQ maintains heap property
        std::vector<int> data = {1, 5, 3, 9, 2, 8, 4, 7, 6};
        BinPQ<int> pq2(data.begin(), data.end());
        
        pq2.updatePQ(); // Force rebuild
        
        // Verify heap property maintained
        std::vector<int> result;
        while (!pq.isEmpty()) {
            result.push_back(pq2.getTop());
            pq2.pop();
        }
        
        for (size_t i = 1; i < result.size(); ++i) {
            assert(result[i-1] >= result[i]);
        }
        cout << "Test 1 passed!\n" << endl;
    }



    // Test 2: Stress test with ascending sequence
    {
        cout << "Test 2: Ascending sequence stress test\n";
        BinPQ<int> pq;
        const int N = 1000;
        
        for (int i = 0; i < N; ++i) {
            pq.push(i);
        }
        
        assert(pq.getTop() == N - 1);
        
        // Pop half the elements
        for (int i = 0; i < N/2; ++i) {
            pq.pop();
        }
        
        assert(pq.getTop() == N/2 - 1);
        cout << "Test 2 passed!\n" << endl;
    }



    // Test 3: Descending sequence stress test
    {
        cout << "Test 3: Descending sequence stress test\n";
        BinPQ<int> pq;
        const int N = 1000;
        
        for (int i = N; i > 0; --i) {
            pq.push(i);
        }
        
        assert(pq.getTop() == N);
        cout << "Test 3 passed!\n" << endl;
    }



    // Test 4: updatePQ edge cases
    {
        cout << "Test 4: updatePQ with various heap states\n";
        
        // Empty heap
        BinPQ<int> pq1;
        pq1.updatePQ(); // Should not crash
        
        // Single element
        BinPQ<int> pq2;
        pq2.push(42);
        pq2.updatePQ();
        assert(pq2.getTop() == 42);
        
        // Already valid heap
        vector<int> valid_heap = {100, 50, 75, 25, 40, 60, 70};
        BinPQ<int> pq3(valid_heap.begin(), valid_heap.end());
        pq3.updatePQ();
        assert(pq3.getTop() == 100);
        
        cout << "Test 4 passed!\n" << endl;
    }



    // Test 5: Alternating push/pop pattern
    {
        cout << "Test 5: Alternating push/pop pattern\n";
        BinPQ<int> pq;
        
        for (int i = 0; i < 10; ++i) {
            pq.push(i * 10);
            pq.push(i * 10 + 5);
            assert(!pq.isEmpty());
            pq.pop();
        }
        
        // Should have 10 elements left
        assert(pq.getSize() == 10);
        
        // Verify heap property
        vector<int> remaining;
        while (!pq.isEmpty()) {
            remaining.push_back(pq.getTop());
            pq.pop();
        }
        assert(std::is_sorted(remaining.rbegin(), remaining.rend()));
        
        cout << "Test 5 passed!\n" << endl;
    }



    // Test 6: Copy and move semantics
    {
        cout << "Test 6: Copy and move semantics\n";
        
        // Test copy constructor
        BinPQ<int> original;
        original.push(10);
        original.push(20);
        original.push(5);
        
        BinPQ<int> copy(original);
        assert(copy.getTop() == original.getTop());
        assert(copy.getSize() == original.getSize());
        
        // Modify copy shouldn't affect original
        copy.pop();
        assert(copy.getSize() == original.getSize() - 1);
        
        // Test move constructor
        BinPQ<int> moved(std::move(copy));
        assert(moved.getSize() == 2);
        
        // Test assignment operators
        BinPQ<int> assigned;
        assigned = original;
        assert(assigned.getTop() == original.getTop());
        
        cout << "Test 6 passed!\n" << endl;
    }

    // Test 7: Large random dataset
    {
        cout << "Test 7: Large random dataset\n";
        BinPQ<int> pq;
        const int N = 10000;
        vector<int> values;
        
        // Generate random values
        for (int i = 0; i < N; ++i) {
            int val = rand() % 1000;
            values.push_back(val);
            pq.push(val);
        }
        
        // Sort for verification
        sort(values.rbegin(), values.rend());
        
        // Verify heap returns elements in correct order
        for (int i = 0; i < N; ++i) {
            assert(static_cast<long long>(pq.getTop()) == static_cast<long long>(values[static_cast<size_t>(i)]));
            pq.pop();
        }



        // Test with maximum/minimum values
        BinPQ<int> pq2;
        pq2.push(INT_MAX);
        pq2.push(INT_MIN);
        pq2.push(0);
        
        assert(pq2.getTop() == INT_MAX);
        pq2.pop();
        assert(pq2.getTop() == 0);
        pq2.pop();
        assert(pq2.getTop() == INT_MIN);
        
        cout << "Test 7 passed!\n" << endl;
    }

    // Test 8: Min-heap functionality
    {
        cout << "Test 8: Min-heap with greater comparator\n";
        BinPQ<int, std::greater<int>> minHeap;
        
        minHeap.push(10);
        minHeap.push(5);
        minHeap.push(15);
        minHeap.push(3);
        
        assert(minHeap.getTop() == 3);
        minHeap.pop();
        assert(minHeap.getTop() == 5);
        
        cout << "Test 8 passed!\n" << endl;
    }

    // Test 9: Custom comparator with complex logic
    {
        cout << "Test 9: Complex comparator test\n";
        
        // Comparator that prioritizes even numbers over odd
        auto evenFirst = [](int a, int b) {
            if (a % 2 != b % 2) {
                return (a % 2) > (b % 2); // odd > even in comparison
            }
            return a < b;
        };
        
        BinPQ<int, decltype(evenFirst)> pq(evenFirst);
        pq.push(1);
        pq.push(2);
        pq.push(3);
        pq.push(4);
        
        assert(pq.getTop() == 4); // Largest even
        pq.pop();
        assert(pq.getTop() == 2); // Next even
        pq.pop();
        assert(pq.getTop() == 3); // Largest odd
        
        cout << "Test 9 passed!\n" << endl;
    }

    cout << "\n\n********** END: Additional BinPQ Edge Tests **********\n" << endl;
} // additionalBinPQEdgeTests()



// Test the pairing heap's range-based constructor, copy constructor,
// and operator=().
//
void testPairing(vector<int> & vec, bool turnOne = true)
{
    cout << "\n\n********** START: Testing PairingPQ TURNS {1 || 2} **********\n" << endl;

    if (turnOne)
    {
        cout << "\nTest TURN {1.1: Range Ctor, Copy Ctor, and Assignment Op - Basic tests}\n";
        SPsPQ<int> * pq1 = new PairingPQ<int>(vec.begin(), vec.end());
        SPsPQ<int> * pq2 = new PairingPQ<int>(*((PairingPQ<int> *)pq1));
        
        // This line is different just to show the different ways to declare a
        // pairing heap: as an SPsPQ above, and as a PairingPQ below. Yay for inheritance!
        
        PairingPQ<int> *pq3 = new PairingPQ<int>();
        *pq3 = *((PairingPQ<int> *)pq2);
        
        pq1->push(3);
        pq2->pop();
        assert(pq1->getSize() == 3); // 0,1,3
        assert(!pq1->isEmpty());
        assert(pq1->getTop() == 3);
        pq2->push(pq3->getTop());
        assert(pq2->getTop() == pq3->getTop());
        
        pq1->push(45);
        pq1->push(7);
        pq1->push(1);
        pq1->push(15);
        pq1->push(12);
        assert(pq1->getTop() == 45);
        pq1->pop();
        assert(pq1->getTop() == 15);
        pq1->pop();
        assert(pq1->getTop() == 12);
        pq1->push(45);
        assert(pq1->getTop() == 45);
        pq1->push(45);
        pq1->push(45);
        pq1->push(30);
        assert(pq1->getTop() == 45);
        pq1->push(33);
        pq1->push(20);
        pq1->pop();
        pq1->pop();
        pq1->pop();
        assert(pq1->getTop() == 33);
    
        cout << "Calling destructors" << endl;
        delete pq1;
        delete pq2;
        delete pq3;
        cout << "\nTest TURN {1.1: Basic tests} passed!\n";



        cout << "\nTest TURN {1.2: Testing addNode() and updateElt() - Basic & Edge tests}\n";

        // Test 1: Basic addNode functionality
        {
            cout << "Test 1: Basic addNode functionality..." << endl;
            PairingPQ<int> pq;
            
            // Add to empty heap
            PairingPQ<int>::Node* node1 = pq.addNode(10);
            assert(pq.getSize() == 1);
            assert(pq.getTop() == 10);
            assert(node1->getElt() == 10);
            assert(node1->operator*() == 10);  // Test dereference operator
            
            // Add to non-empty heap
            PairingPQ<int>::Node* node2 = pq.addNode(20);
            assert(pq.getSize() == 2);
            assert(pq.getTop() == 20);  // pq = {10, 20}
            pq.updateElt(node2, 35);    // pq = {10, 35}
            assert(pq.getTop() == 35);  
            assert(node2->getElt() == 35);
            assert(node2->operator*() == 35);  // Test dereference operator
            
            // Add smaller value
            PairingPQ<int>::Node* node3 = pq.addNode(5); // pq = {10, 35, 5}
            assert(pq.getSize() == 3);
            assert(pq.getTop() == 35);  // Top should still be 35
            assert(node3->getElt() == 5);

            // Update a non-root node
            pq.updateElt(node3, 100); // pq = {10, 35, 100}
            pq.updateElt(node1, 111); // pq = {111, 35, 100} 
            assert(pq.getTop() == 111);
            
            cout << "Test 1 passed!" << endl;
        }

        // Test 2: Node pointer validity after operations
        {
            cout << "Test 2: Node pointer validity..." << endl;
            PairingPQ<int> pq;
            
            PairingPQ<int>::Node* node1 = pq.addNode(15);
            PairingPQ<int>::Node* node2 = pq.addNode(25);
            PairingPQ<int>::Node* node3 = pq.addNode(10);
            
            // Pointers should remain valid
            assert(node1->getElt() == 15);
            assert(node2->getElt() == 25);
            assert(node3->getElt() == 10);
            
            // After other operations
            pq.push(30);  // Regular push
            assert(node1->getElt() == 15);  // Still valid
            assert(node2->getElt() == 25);  // Still valid
            assert(node3->getElt() == 10);  // Still valid
            
            cout << "Test 2 passed!" << endl;
        }

        // Test 3: updateElt() - Valid updates (more extreme values)
        {
            cout << "Test 3: updateElt() with valid updates..." << endl;
            PairingPQ<int> pq;
            
            PairingPQ<int>::Node* node1 = pq.addNode(10);
            PairingPQ<int>::Node* node2 = pq.addNode(20);
            PairingPQ<int>::Node* node3 = pq.addNode(15);
            
            assert(pq.getTop() == 20); // pq = {10, 20, 15}
            
            // Update to higher priority (valid - more extreme)
            pq.updateElt(node1, 25);  // 10 → 25, pq = {25, 20, 15}
            assert(node1->getElt() == 25);
            assert(pq.getTop() == 25);  // Should become new top
            
            // Update another node
            pq.updateElt(node3, 30);  // 15 → 30, pq = {25, 20, 30}
            assert(node3->getElt() == 30);
            assert(pq.getTop() == 30);  // Should become new top

            pq.updateElt(node2, 100); // pq = {100, 20, 30}
            assert(node2->getElt() == 100);
            assert(pq.getTop() == 100);
            
            cout << "Test 3 passed!" << endl;
        }


        // Test 4: updateElt() - Root node updates
        {
            cout << "Test 4: updateElt() on root node..." << endl;
            PairingPQ<int> pq;
            
            PairingPQ<int>::Node* root = pq.addNode(20);
            pq.addNode(10);
            pq.addNode(15);
            
            assert(pq.getTop() == 20);
            
            // Update root to even higher value
            pq.updateElt(root, 25);
            assert(root->getElt() == 25);
            assert(pq.getTop() == 25);
            
            cout << "Test 4 passed!" << endl;
        }


        // Test 5: updateElt() - Non-root updates that don't change top
        {
            cout << "Test 5: updateElt() on non-root without top change..." << endl;
            PairingPQ<int> pq;
            
            pq.addNode(30);  // Will be top
            PairingPQ<int>::Node* node2 = pq.addNode(10);
            PairingPQ<int>::Node* node3 = pq.addNode(5);
            
            assert(pq.getTop() == 30);
            
            // Update smaller value, but still less than top
            pq.updateElt(node2, 15);  // 10 → 15 (still < 30)
            assert(node2->getElt() == 15);
            assert(pq.getTop() == 30);  // Top unchanged
            
            pq.updateElt(node3, 12);   // 5 → 12 (still < 30)
            assert(node3->getElt() == 12);
            assert(pq.getTop() == 30);  // Top unchanged
            
            cout << "Test 5 passed!" << endl;
        }



        // Test 6: updateElt() - Single element heap
        {
            cout << "Test 6: updateElt() on single element heap..." << endl;
            PairingPQ<int> pq;
            
            PairingPQ<int>::Node* onlyNode = pq.addNode(10);
            assert(pq.getSize() == 1);
            assert(pq.getTop() == 10);
            
            pq.updateElt(onlyNode, 20);
            assert(onlyNode->getElt() == 20);
            assert(pq.getTop() == 20);
            assert(pq.getSize() == 1);
            
            cout << "Test 6 passed!" << endl;
        }
        
        // Test 7: Complex scenario - multiple updates and operations
        {
            cout << "Test 7: Complex mixed operations..." << endl;
            PairingPQ<int> pq;
            
            vector<PairingPQ<int>::Node*> nodes;
            
            // Add several nodes
            nodes.push_back(pq.addNode(10));
            nodes.push_back(pq.addNode(5));
            nodes.push_back(pq.addNode(20));
            nodes.push_back(pq.addNode(15));
            nodes.push_back(pq.addNode(8));
            
            assert(pq.getTop() == 20);
            assert(pq.getSize() == 5);
            
            // Update multiple elements
            pq.updateElt(nodes[1], 25);  // 5 → 25 (should become top)
            assert(pq.getTop() == 25);
            
            pq.updateElt(nodes[4], 30);  // 8 → 30 (should become new top)
            assert(pq.getTop() == 30);
            
            // Mix with regular operations
            pq.push(12);  // Regular push
            assert(pq.getTop() == 30);  // Should still be 30
            
            pq.updateElt(nodes[0], 35);  // 10 → 35 (should become new top)
            assert(pq.getTop() == 35);
            
            // Verify all nodes still have correct values
            assert(nodes[0]->getElt() == 35);
            assert(nodes[1]->getElt() == 25);
            assert(nodes[2]->getElt() == 20);
            assert(nodes[3]->getElt() == 15);
            assert(nodes[4]->getElt() == 30);
            
            cout << "Test 7 passed!" << endl;
        }
        
        // Test 8: updateElt() with min-heap comparator
        {
            cout << "Test 8: updateElt() with min-heap..." << endl;
            auto minComp = [](int a, int b) { return a > b; };
            PairingPQ<int, decltype(minComp)> minPQ(minComp);
            
            auto node1 = minPQ.addNode(20);
            auto node2 = minPQ.addNode(10);
            auto node3 = minPQ.addNode(30);
            
            assert(minPQ.getTop() == 10);  // Min-heap, so smallest is top
            
            // In min-heap, "more extreme" means smaller
            minPQ.updateElt(node1, 5);   // 20 → 5 (more extreme for min-heap)
            assert(minPQ.getTop() == 5); // pq = {5, 10, 30}
            
            minPQ.updateElt(node3, 8);   // 30 → 8 (more extreme for min-heap)
            assert(minPQ.getTop() == 5); // Should still be 5, pq = {5, 10, 8}

            minPQ.updateElt(node2, -1); // pq = {5, -1, 8}
            assert(minPQ.getTop() == -1);
            
            cout << "Test 8 passed!" << endl;
        }
        
        // Test 9: Stress test - many nodes and updates
        {
            cout << "Test 9: Stress test with many updates..." << endl;
            PairingPQ<int> pq;
            vector<PairingPQ<int>::Node*> nodes;
            
            // Add many nodes
            for (int i = 0; i < 100; ++i) {
                nodes.push_back(pq.addNode(i));
            }
            
            assert(pq.getSize() == 100);
            assert(pq.getTop() == 99);
            
            // Update every 10th node to a higher value
            for (size_t i = 0; i < nodes.size(); i += 10) {
                int newVal = 100 + static_cast<int>(i);
                pq.updateElt(nodes[i], newVal);
                assert(nodes[i]->getElt() == newVal);
            }
            
            // Top should be the highest updated value
            assert(pq.getTop() == 190);  // 100 + 90
            
            cout << "Test 9 passed!" << endl;
        }
        
        // Test 10: Edge case - updateElt() precondition testing
        {
            cout << "Test 10: Testing updateElt() precondition behavior..." << endl;
            PairingPQ<int> pq;
            
            auto node = pq.addNode(20);
            
            // This should work (20 → 25 is more extreme)
            pq.updateElt(node, 25);
            assert(node->getElt() == 25);
            
            // Test what happens with invalid update (less extreme)
            // Note: Your implementation checks the precondition, so this should do nothing

            try {
                pq.updateElt(node, 15);  // 25 → 15 (less extreme, should be ignored)
                cout << "❌ Test 10 failed! (should have thrown invalid_argument)" << endl;
            } catch(const std::invalid_argument&) {
                assert(node->getElt() == 25);  // Should remain unchanged
                cout << "Test 10 passed! (threw invalid_argument as expected)" << endl;
            }
        }

        cout << "\nTest TURN {1.2: Basic & Edge tests} passed!\n";
        
        cout << "\n\n********** END: TURN {1} Complete ✅ **********\n" << endl;
    }
    else
    {
        cout << "\nTest TURN {2: Range Ctor, Copy Ctor, and Assignment Op - Edge tests}\n";

        cout << "Testing empty vector with range constructor..." << endl;
        PairingPQ<int> pq1 {vec.begin(), vec.end()};
        PairingPQ pq2 {pq1};

        pq2.push(3); // make it non empty

        PairingPQ<int> pq3;
        assert(pq3.isEmpty());
        assert(pq2.getSize() == 1);
        assert(pq2.getTop() == 3);

        pq2 = pq3;
        assert(pq2.getSize() == 0);
        assert(pq2.isEmpty());
        cout << "Testing empty vector passed!\n" << endl;


        // Test deep copy - changes to original shouldn't affect copy
        cout << "Testing deep copy independence..." << endl;
        PairingPQ<int> original;
        original.push(1);
        original.push(5);
        original.push(3);

        // Copy and verify independence
        PairingPQ<int> copy(original);
        assert(copy.getTop() == original.getTop());

        // Modify original
        original.push(10);
        assert(original.getTop() == 10);
        assert(copy.getTop() == 5);  // Copy should be unchanged!

        // Modify copy
        copy.push(15);
        assert(copy.getTop() == 15);
        assert(original.getTop() == 10);  // Original should be unchanged!

        // Pop from original
        original.pop();  // Remove 10
        assert(original.getTop() == 5);
        assert(copy.getTop() == 15);  // Copy still unaffected
        cout << "Testing deep copy independence passed!\n" << endl;


        // Test chained assignments: a = b = c
        cout << "Testing chained assignment..." << endl;
        PairingPQ<int> chain1, chain2, chain3;
        chain3.push(99);
        chain3.push(88);

        chain1 = chain2 = chain3;  // Chained assignment
        assert(chain1.getTop() == 99);
        assert(chain2.getTop() == 99);
        assert(chain3.getTop() == 99);

        // Verify they're independent
        chain1.pop();
        assert(chain1.getTop() == 88);
        assert(chain2.getTop() == 99);  // Unchanged
        assert(chain3.getTop() == 99);  // Unchanged
        cout << "Testing chained assignment passed!\n" << endl;



        // Test copying large heap
        cout << "Testing large heap copy..." << endl;
        PairingPQ<int> largePQ;
        for (int i = 0; i < 1000; ++i) {
            largePQ.push(i);
        }

        PairingPQ<int> largeCopy(largePQ);
        assert(largeCopy.getSize() == 1000);
        assert(largeCopy.getTop() == 999);

        // Verify structure by popping all elements
        for (int i = 999; i >= 0; --i) {
            assert(largeCopy.getTop() == i);
            largeCopy.pop();
        }
        assert(largeCopy.isEmpty());
        cout << "Testing large heap copy passed!\n" << endl;


        // Test your specific traversal logic edge case
        cout << "Testing copy constructor traversal edge case..." << endl;
        // Create a heap with specific structure that might break traversal
        PairingPQ<int> specialPQ;
        // Add elements in an order that creates complex tree structure
        vector<int> specialOrder = {50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 35};
        for (int val : specialOrder) {
            specialPQ.push(val);
        }

        PairingPQ<int> specialCopy(specialPQ);
        assert(specialCopy.getSize() == specialOrder.size());
        assert(specialCopy.getTop() == 80);

        // Verify all elements are present by extracting them
        vector<int> extracted;
        while (!specialCopy.isEmpty()) {
            extracted.push_back(specialCopy.getTop());
            specialCopy.pop();
        }
        assert(extracted.size() == specialOrder.size());
        // Should be in descending order due to max-heap property
        assert(is_sorted(extracted.rbegin(), extracted.rend()));
        cout << "Testing copy constructor traversal edge case passed!\n" << endl;
        

        // Test copying with custom comparator
        cout << "Testing custom comparator copy..." << endl;
        auto minComp = [](int a, int b) { return a > b; };  // Min-heap
        PairingPQ<int, decltype(minComp)> minPQ(minComp);
        minPQ.push(5);
        minPQ.push(1);
        minPQ.push(8);
        assert(minPQ.getTop() == 1);  // Minimum element

        PairingPQ<int, decltype(minComp)> minCopy(minPQ);
        assert(minCopy.getTop() == 1);
        minCopy.push(0);
        assert(minCopy.getTop() == 0);  // Still min-heap behavior
        cout << "Testing custom comparator copy passed!\n" << endl;

        cout << "\n\n********** END: TURN {2} Complete ✅ **********\n" << endl;
    }
    
    cout << "\n\n********** END: Testing PairingPQ TURNS Complete ✅ **********\n" << endl;
} // testPairing()



/// @brief Self-contained test function for PairingPQ.
/// @note Tests basic operations and the updatePQ method with corruption scenarios.
void testPairingPQ() 
{
    cout << "\n\n********** START: Testing PairingPQ Invariants: Basic & Corner Cases **********\n" << endl;
    
    // Test 1: Basic operations
    {
        cout << "\nTest 1: Basic top/pop operations\n";
        PairingPQ<int> pq;
        
        // Push some values
        vector<int> values = {5, 1, 8, 3, 9, 2, 7};
        for (int val : values) {
            pq.push(val);
        }
        
        cout << "After pushing: ";
        pq.print();
        cout << "Size: " << pq.getSize() << ", Top: " << pq.getTop() << "\n";
        
        // Pop all elements and verify order
        vector<int> popped;
        while (!pq.isEmpty()) {
            popped.push_back(pq.getTop());
            cout << "Popping: " << pq.getTop() << "\n";
            pq.pop();
        }
        
        cout << "Popped sequence (should be in descending order): ";
        for (size_t i = 0; i < popped.size(); ++i) {
            cout << popped[i];
            if (i < popped.size() - 1) cout << ", ";
        }
        cout << "\n";
        
        // Check if properly sorted in descending order
        bool sorted = true;
        for (size_t i = 1; i < popped.size(); ++i) {
            if (popped[i] > popped[i-1]) {
                sorted = false;
                break;
            }
        }

        assert(sorted);
        cout << "Test 1 Result: " << (sorted ? "✅ PASSED" : "❌ FAILED") << "\n";
    }
    
    // Test 2: Copy and move operations
    {
        cout << "\nTest 2: Copy and move operations\n";
        PairingPQ<int> pq1;
        for (int i = 1; i <= 5; ++i) {
            pq1.push(i);
        }
        
        // Test copy constructor
        PairingPQ<int> pq2(pq1);
        cout << "Original top: " << pq1.getTop() << ", Copy top: " << pq2.getTop() << "\n";
        
        // Test move constructor
        PairingPQ<int> pq3(std::move(pq1));
        cout << "After move, pq3 top: " << pq3.getTop() << ", pq1 empty (should be true): " << pq1.isEmpty() << "\n";
        
        bool testPassed = (pq2.getTop() == 5) && (pq3.getTop() == 5) && pq1.isEmpty();
        assert(testPassed);
        cout << "Test 2 Result: " << (testPassed ? "✅ PASSED" : "❌ FAILED") << "\n";
    }
    
    // Test 3: Range constructor
    {
        cout << "\nTest 3: Range constructor\n";
        vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6};
        PairingPQ<int> pq(vec.begin(), vec.end());
        
        cout << "Created from vector, top: " << pq.getTop() << ", size: " << pq.getSize() << "\n";
        
        bool testPassed = (pq.getTop() == 9) && (pq.getSize() == vec.size());
        assert(testPassed);
        cout << "Test 3 Result: " << (testPassed ? "✅ PASSED" : "❌ FAILED") << "\n";
    }

    // Test 4: pop()
    {
        PairingPQ<int> pq;
        auto node1 = pq.addNode(10);  // root
        auto node2 = pq.addNode(20);  // becomes child of root

        // At this point: root(10) -> child(20) -> sibling(30)
        // numNodes = 3

        assert(node1->getElt() == 10);
        assert(node2->operator*() == 20);

        pq.pop();  // Removes root(20), numNodes becomes 2
        assert(pq.getSize() == 1);
        assert(pq.getTop() == 10);
        cout << "Test 4 Result: ✅ PASSED\n";
    }


    // Test 4: updateElt()
    // {
    //     cout << "\nTest 4: updateElt() corner cases\n";
    //     PairingPQ<int> pq;
        
    //     // Push some values
    //     vector<int> values = {5, 1, 8, 3, 9, 2, 7};
    //     for (int val : values) {
    //         pq.push(val);
    //     }

    //     auto n1 = pq.addNode(5); // then this one
    //     auto n2 = pq.addNode(1);
    //     auto n3 = pq.addNode(8);
    //     auto n4 = pq.addNode(3); // this one first
    //     auto n5 = pq.addNode(9);
    //     auto n6 = pq.addNode(2);
    //     auto n7 = pq.addNode(7);

    //     pq.updateElt(n3, 4);

    //     /**
    //      * need one where it replaces root, and then one that doesnt. then one update to a left most node, middle, and rightmost. then multilevel. 
    //      */



    //     pq.updateElt(n1, 10);
    // }
    
    cout << "\n\n********** END: Testing PairingPQ Invariants: Basic & Corner Cases ✅ **********\n" << endl;
} // testPairingPQ()



/// @brief Test function for PairingPQ with updateElt functionality
void testPairingPQUpdateElt() 
{
    cout << "\n\n********** START: Testing PairingPQ with Specific updateElt() Corner Cases **********\n" << endl;
    
    // Create a max-heap
    PairingPQ<int> pq;
    
    // Add nodes and keep track of them
    std::cout << "Adding nodes with values: 10, 20, 30, 40, 50" << std::endl;
    auto node10 = pq.addNode(10);
    auto node20 = pq.addNode(20);
    auto node30 = pq.addNode(30);
    auto node40 = pq.addNode(40);
    auto node50 = pq.addNode(50);
    
    pq.print();
    
    // Update a node to have higher priority
    std::cout << "\nUpdating node with value 20 to 60" << std::endl;
    pq.updateElt(node20, 60);
    pq.print();
    pq.updateElt(node50, 70);
    pq.print();
    assert(node10->operator*() == 10);
    pq.updateElt(node10, 65);
    pq.print();
    pq.updateElt(node30, 61);
    assert(node30->getElt() == 61);
    pq.print();
    pq.updateElt(node40, 59);
    pq.print();
    
    std::cout << "\nPopping all elements (should be in descending order):" << std::endl;
    while (!pq.isEmpty()) {
        std::cout << pq.getTop() << " ";
        pq.pop();
    }
    std::cout << std::endl;
    
    cout << "\n\n********** END: Testing PairingPQ with Specific updateElt() Corner Cases Complete ✅ **********\n" << endl;
} // testPairingPQUpdateElt()



void testExceptions() 
{
    cout << "\n\n********** START: Testing Exception Handling (MA) **********\n" << endl;
    
    // Test 1: pop() on empty heap
    {
        cout << "Test 1: pop() on empty heap" << endl;
        PairingPQ<int> pq;
        try {
            pq.pop();
            cout << "Test 1 failed: Expected exception on empty pop\n";
        } catch (const std::runtime_error& e) {
            cout << "Test 1 passed: Empty queue throws on pop\n";
        }
    }
    
    // Test 2: getTop() on empty heap
    {
        cout << "Test 2: getTop() on empty heap" << endl;
        PairingPQ<int> pq;
        try {
            pq.getTop();
            cout << "Test 2 failed: Expected exception on empty getTop\n";
        } catch (const std::runtime_error& e) {
            cout << "Test 2 passed: Empty queue throws on getTop\n";
        }
    }
    
    // Test 3: updateElt() with null node
    {
        cout << "Test 3: updateElt() with null node" << endl;
        PairingPQ<int> pq;
        try {
            pq.updateElt(nullptr, 42);
            cout << "Test 3 failed: Expected exception on null updateElt\n";
        } catch (const std::invalid_argument& e) {
            cout << "Test 3 passed: Null node throws on updateElt\n";
        }
    }
    
    // Test 4: updateElt() with invalid new value
    {
        cout << "Test 4: updateElt() with invalid new value" << endl;
        PairingPQ<int> pq;
        auto node = pq.addNode(50);
        try {
            pq.updateElt(node, 30); // Trying to decrease priority
            cout << "Test 4 failed: Expected exception on invalid updateElt\n";
        } catch (const std::invalid_argument& e) {
            cout << "Test 4 passed: Invalid updateElt throws\n";
        }
    }
    
    // Test 5: Verify heap still works after exception
    {
        cout << "Test 5: Heap functionality after exceptions" << endl;
        PairingPQ<int> pq;
        
        // Try invalid operations
        try { pq.pop(); } catch (...) {}
        try { pq.getTop(); } catch (...) {}
        
        // Heap should still work normally
        pq.push(10);
        pq.push(20);
        assert(pq.getSize() == 2);
        assert(pq.getTop() == 20);
        pq.pop();
        assert(pq.getTop() == 10);
        cout << "Test 5 passed: Heap functionality still works after exceptions\n";
    }
    
    cout << "\n\n********** END: Testing Exception Handling (MA) Complete ✅ **********\n" << endl;
} // testExceptions()


int main()
{
    // Basic pointer, allocate a new PQ later based on user choice.
    SPsPQ<int> *pq1;
    SPsPQ<int> *pq2;

    vector<string> types
    {
        "Unordered",    // 0
        "UnorderedOPT", // 1
        "Sorted",       // 2
        "Binary",       // 3
        "Pairing",      // 4
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
    else if (choice == 2)
    {
        pq1 = new SortedPQ<int>;
        pq2 = new SortedPQ<int>(start, end);

        // testSortedPQUpdateCornerCase(); // MA
        // testSortedPQUpdateMethod(); // MB
        testSortedPQUpdateCornerCase();

        testSortedPQ(pq1, types[choice]);

        // TODO: Add more tests to this function for the range-based constructor and pq2!
        testSortedPQ(pq2, types[choice]);

        runSortedPQEdgeTests(types[choice]);
    } // else if
    else if (choice == 3)
    {
        binTests();
        additionalBinPQEdgeTests();
        pq1 = new BinPQ<int>;
        pq2 = new BinPQ<int>(start, end);
    }
    else if (choice == 4)
    {
        // MA: Test the exception handling
        testExceptions();

        // Test the pairing heap
        vector<int> turn1;
        turn1.push_back(0);
        turn1.push_back(1);
        testPairing(turn1);

        vector<int> turn2; // empty vector
        testPairing(turn2, false);

        testPairingPQ();
        testPairingPQUpdateElt();

        pq1 = new PairingPQ<int>;
        pq2 = new PairingPQ<int>(start, end);
    }
    else
    {
        cout << "Unknown container!" << endl
             << endl;
        exit(1);
    } // else

    // Send the pq type
    testPriorityQueue(types[choice]);
    
    // Send the pq ptr
    testPriorityQueuePtr(pq1, types[choice]);
    
    // Send the range-based pq ptr
    testPQsCtor(pq2, types[choice]);

    // Test the updatePQ function
    testUpdatePQ(types[choice]);

    // Test the hidden data function
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












































































