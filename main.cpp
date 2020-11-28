/*
 * File: main.cpp
 * --------------
 * Final Project
 * Jake Taylor
 */

#include <iostream>
#include "console.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;

/*////////////////////////////////////////////////////////////
 * main()
 * ///////////////////////////////////////////////////////////
 */
int main() {
    if (runSimpleTests(SELECTED_TESTS)) {
        return 0;
    }
    cout << "All done, exiting" << endl;
    return 0;
}


