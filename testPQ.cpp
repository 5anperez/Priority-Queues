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

// 2. Creat the functor via lambda.
// Used to test a min-heap type priority queue containing doubles.
auto DescendingComp2 = [](double a, double b) 
{
    // Prioritizes smaller doubles.
    return a > b;
};

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


    // Introduce "thresholded" comparisons, where small differences are treated as equal.
    struct HDThresholdComparator 
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const 
        {
            const double threshold = 0.01;
            double result = a.dec_data - b.dec_data;
            return (fabs(result) > threshold) ? (result) : false;
        }
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

    
    cout << "Testing " << pqType << " with hidden data" << endl;
    
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



        SortedPQ<HiddenData, HDThresholdComparator> pq3;
        pq3.push(HiddenData(1.001));
        pq3.push(HiddenData(1.002));
        pq3.push(HiddenData(1.020));

        assert(pq3.top().dec_data == 1.020); 
        pq3.pop();
        assert(pq3.top().dec_data == 1.001);
        pq3.pop();
        pq3.pop();
        assert(pq3.empty());


        // Odds before evens
        SortedPQ<HiddenData, OddFirstComp> pq4;
        pq4.push(HiddenData(4));
        pq4.push(HiddenData(1));
        pq4.push(HiddenData(3));
        pq4.push(HiddenData(2));

        assert(pq4.top().data == 1); 
        pq4.pop();
        assert(pq4.top().data == 3);
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
        cout << "Testing " << pqType << " with custom functors...\n" << endl;
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



        PairingPQ<HiddenData, HDThresholdComparator> pq3;
        pq3.push(HiddenData(1.001));
        pq3.push(HiddenData(1.002));
        pq3.push(HiddenData(1.020));

        assert(pq3.top().dec_data == 1.020); 
        pq3.pop();
        assert(pq3.top().dec_data == 1.001);
        pq3.pop();
        pq3.pop();
        assert(pq3.empty());


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
    

    
   
    cout << "testHiddenData() succeeded!" << endl;
   
} // testHiddenData()














// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePrioritiesHelper(Eecs281PQ<int *, IntPtrComp> *pq)
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
    
    cout << "updatePriorities() succeeded!" << endl;
    
} // testUpdatePrioritiesHelper()








// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePriorities(const string &pqType)
{
    Eecs281PQ<int *, IntPtrComp> *pq = nullptr;

    if (pqType != "Unordered")
        cout << "Testing updatePriorities() on " << pqType << endl;
    
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
    
    if (pqType != "Unordered")
        testUpdatePrioritiesHelper(pq);

    delete pq;
} // testUpdatePriorities()









void testPriorityQueue(Eecs281PQ<int> *pq, const string &pqType)
{
    cout << "\nTesting priority queue with integer values:: " << pqType << "\n" << endl;
    
    // Very basic testing.
    cout << "Testing basic functionality...\n" << endl;
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

    cout << "Testing basic functionality passed!\n" << endl;


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
    cout << "Testing with extrame values passed!\n" << endl;



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
    cout << "Testing with sorted values passed!\n" << endl;



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
    cout << "Testing with reversed sorted values passed!\n" << endl;



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
    cout << "Testing with both +/- values passed!\n" << endl;



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
    cout << "Testing by manually modifying the values passed!\n" << endl;



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
    cout << "Testing the default constructor passed!\n" << endl;



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
    

    cout << "\n***************************" << endl;
    cout << "ALL INTEGER TESTS PASSED!!!" << endl;
    cout << "***************************\n" << endl;



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
    
    
    cout << "testPriorityQueue() succeeded!" << endl;
} // testPriorityQueue()




// Test the unsorted heap's range-based constructor, copy constructor,
// and operator=().
//
void testUnsortedPQ(Eecs281PQ<int> *pq, const string &pqType)
{
    cout << "Testing unsorted queue: " << pqType << endl;
    
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
    
    cout << "testUnsortedPQ() succeeded!" << endl;
} // testPriorityQueue()




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









int main()
{
    // Basic pointer, allocate a new PQ later based on user choice.
    Eecs281PQ<int> *pq;
    vector<string> types{ 
        "Unordered", 
        "Sorted", 
        "Binary", 
        "Pairing", 
        "Unordered.2" 
    };
    unsigned int choice;
    
    cout << "PQ tester" << endl << endl;
    for (size_t i = 0; i < types.size(); ++i)
        cout << "  " << i << ") " << types[i] << endl;
    cout << endl;
    cout << "Select one: ";
    cin >> choice;
    
    if (choice == 0)
    {
        // Test using the default comparator, 
        // which organizes in ascending order.
        pq = new UnorderedPQ<int>;
    } // if
    else if (choice == 1)
    {
        pq = new SortedPQ<int>;
    } // else if
    else if (choice == 2)
    {
        pq = new BinaryPQ<int>;
    } // else if
    else if (choice == 3)
    {
        pq = new PairingPQ<int>;
    } // else if
    else if (choice == 4)
    {
        vector<int> tester = {1,2,3,8,7,6,13,42,65,43,87,45,56,35,29};
        
        // Send a subset = {6,...,87}, i.e., shrink the range
        // by setting a ptr to the 6th elt and the the 11th 
        // elt to get the subset [6, 45).
        auto start = tester.begin() + 5;
        auto end = tester.begin() + 11;
        
        pq = new UnorderedPQ<int>(start, end);
    } // else if
    else
    {
        cout << "Unknown container!" << endl << endl;
        exit(1);
    } // else
    
    // Send the pq.
    testPriorityQueue(pq, types[choice]);
    
    // Start with a fresh pq once inside here.
    testUpdatePriorities(types[choice]);

    // Test comparator robustness.
    testHiddenData(types[choice]);
    
    if (choice == 4)
    {
        testUnsortedPQ(pq, types[choice]);
    }
    
    if (choice == 3)
    {
        vector<int> vec;
        vec.push_back(0);
        vec.push_back(1);
        testPairing(vec);
    } // if
    
    // Clean up!
    delete pq;
    
    return 0;
} // main()














