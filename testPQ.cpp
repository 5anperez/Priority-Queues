// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

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
 * This is NOT a complete test of your priority queues, you have to add code
 * to do more testing!
 *
 * You do not have to submit this file, but it won't cause problems if you do.
 */

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Eecs281PQ.h"
#include "BinaryPQ.h"
#include "UnorderedPQ.h"
#include "PairingPQ.h"
#include "SortedPQ.h"

using namespace std;


// Used to test a priority queue containing pointers to integers.
struct IntPtrComp
{
    bool operator() (const int *a, const int *b) const
    {
        return *a < *b;
    }
};


// TODO: Make sure that you're using this-compare() properly, and everywhere
// that you should.  Complete this function by adding a functor that compares
// two HiddenData structures, create a PQ of the specified type, and call
// this function from main().
void testHiddenData(const string &pqType)
{
    struct HiddenData
    {
        int data;
    };
    
    struct HiddenDataComp
    {
        bool operator()(const HiddenData &a, const HiddenData &b) const
        {
            if (a.data < b.data)
                return true;
            return false;
        }
    };
    
    cout << "Testing " << pqType << " with hidden data" << endl;
    
    // TODO: Add code here to actually test with the HiddenData type.
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
    data[0] = 30000;
    pq->updatePriorities();
    assert(*pq->top() == 3000);
    
    
    
} // testUpdatePrioritiesHelper()


// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePriorities(const string &pqType)
{
    Eecs281PQ<int *, IntPtrComp> *pq = nullptr;
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
    
    testUpdatePrioritiesHelper(pq);
    delete pq;
} // testUpdatePriorities()


// Very basic testing.
void testPriorityQueue(Eecs281PQ<int> *pq, const string &pqType)
{
    cout << "Testing priority queue: " << pqType << endl;
    
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
    
    // TODO: Add more testing here!
    pq->push(18);
    pq->push(81);
    pq->push(42);
    assert(pq->top() == 81);
    pq->pop();
    assert(pq->top() == 42);
    pq->pop();
    assert(pq->top() == 18);
    
    
    
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
    PairingPQ<int> * pq3 = new PairingPQ<int>();
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
    vector<string> types{ "Unordered", "Sorted", "Binary", "Pairing", "Unordered.2" };
    unsigned int choice;
    
    cout << "PQ tester" << endl << endl;
    for (size_t i = 0; i < types.size(); ++i)
        cout << "  " << i << ") " << types[i] << endl;
    cout << endl;
    cout << "Select one: ";
    cin >> choice;
    
    if (choice == 0)
    {
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
        
        // send a subset = {6,...,45}
        auto start = tester.begin() + 5;
        auto end = tester.begin() + 11;
        
        pq = new UnorderedPQ<int>(start, end);
    } // else if
    else
    {
        cout << "Unknown container!" << endl << endl;
        exit(1);
    } // else
    
    testPriorityQueue(pq, types[choice]);
    testUpdatePriorities(types[choice]);
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
