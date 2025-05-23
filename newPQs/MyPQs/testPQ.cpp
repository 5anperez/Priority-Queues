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

#include "SPsPQ.h"

using namespace std;


// void testHiddenData()



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
        //pq = new UnorderedPQ<int>;
    } // if
    else
    {
        cout << "Unknown container!" << endl << endl;
        exit(1);
    } // else

    
    // Clean up!
    delete pq;
    
    return 0;
} // main()



