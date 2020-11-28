/*
 * File: pqheap.cpp
 * --------------
 * Final Project - adapted from previous assignment
 * (credit goes towards CS106 instructors)
 * Jake Taylor
 */

#include "pqheap.h"
#include "error.h"
#include "random.h"
#include "console.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/MemoryUtils.h"
#include "testing/SimpleTest.h"
#include <iostream>
using namespace std;

/*
 * PQ Heap constructor that intializes with an empty
 * object that has a set capacity.
*/

PQHeap::PQHeap() {
    _numAllocated = INITIAL_CAPACITY;
    _elements = new DataPoint[_numAllocated];
    _numFilled = 0;
}

/*
 * Destructor that unassigns memory when we are done with an
 * instance of PQHeap
 */
PQHeap::~PQHeap() {
    delete[] _elements;
}

/*
 * Helper function that moves the memory from an old PQ to
 * a new PQ that has double the capacity.
 */
void PQHeap::move(){
    DataPoint *_newElements = new DataPoint[_numAllocated * 2];
    for(int i = 0; i < _numFilled; i++){
        _newElements[i] = _elements[i];
    }
    delete [] _elements;
    _elements = _newElements;
    _numAllocated *= 2;
}

/*
 * Helper function that swaps the i and j th elements
 * inside a PQHeap.
 */
void PQHeap::swap(int i, int j){
    DataPoint temp = _elements[j];
    _elements[j] = _elements[i];
    _elements[i] = temp;
}

/*
 * Helper function that iteratively bubbles up an item at a given index
 */
void PQHeap::bubble(int currIndex){
    while(inbounds(getParentIndex(currIndex)) &&
          _elements[currIndex].priority < _elements[getParentIndex(currIndex)].priority){
        swap(currIndex, getParentIndex(currIndex));
        currIndex = getParentIndex(currIndex);
    }
}

/*
 * Fuction that takes as input a single DataPoint and then enqueues
 * that element into a PQHeap. Updates the queue accordingly and
 * keeps track of the total number of elems in the queue.
 */
void PQHeap::enqueue(DataPoint elem) {
    if(_numFilled + 1 == _numAllocated){
        // lengthen array
        move();
    }
    _elements[_numFilled] = elem;
    bubble(_numFilled);
    _numFilled++;
}

/*
 * Function that returns the top element in the queue. Does not
 * alter the elements inside the queue.
 */
DataPoint PQHeap::peek() const {
    if (isEmpty()) {
        error("Cannot peek empty pqueue");
    }
    return _elements[0];
}

/*
 * Helper function that iteratively bubbles the root element down a
 * PQHeap into a correct position.
 */
void PQHeap::bubbleDown(){
    int currIndex = 0;
    while(inbounds(getLeftChildIndex(currIndex))){
        int leftPriority = _elements[getLeftChildIndex(currIndex)].priority;
        int rightPriority = _elements[getRightChildIndex(currIndex)].priority;
        int curPriority = _elements[currIndex].priority;
        // check no right branch case
        if(!inbounds(getRightChildIndex(currIndex)) &&
                leftPriority < curPriority){
            swap(currIndex, getLeftChildIndex(currIndex));
            currIndex = getLeftChildIndex(currIndex);
        // find smallest and swap/iterate on that branch
        } else if(leftPriority <= rightPriority &&
                  leftPriority < curPriority){
            swap(currIndex, getLeftChildIndex(currIndex));
            currIndex = getLeftChildIndex(currIndex);
        } else if(leftPriority > rightPriority &&
                  rightPriority < curPriority){
            swap(currIndex, getRightChildIndex(currIndex));
            currIndex = getRightChildIndex(currIndex);
        } else {
            break;
        }
    }
}

/*
 * Function that returns the top element and also removes it from the
 * PQHeap. The function also updates the internal structure of the PQHeap
 *and keeps track of how many elements remain after removing the top elem.
 */
DataPoint PQHeap::dequeue() {
    if (isEmpty()) {
        error("Cannot dequeue empty pqueue");
    }
    DataPoint top = _elements[0];
    _numFilled--;
    _elements[0] = _elements[_numFilled];
    bubbleDown();
    return top;
}

/*
 * Function that returns if the PQHeap is empty.
 */
bool PQHeap::isEmpty() const {
    return size() == 0;
}

/*
 * Function that returns the number of elements in the PQHeap.
 */
int PQHeap::size() const {
    return _numFilled;
}

/*
 * Function that resets a PQHeap to being empty
 */
void PQHeap::clear() {
    _numFilled = 0;
}

/*
 * Helper function returns a boolean if an index is within a PQHeap.
 * Used inside other function while adding and checking elements inside a
 * PQHeap.
 */
bool PQHeap::inbounds(int curIndex){
    return curIndex >= 0 && curIndex < _numFilled;
}

/*
 * Function that prints debug info. (I used the debugger instead of
 * relying on this function)
 */
Vector<string> PQHeap::printDebugInfo() {
    Vector<string> results(_numFilled);
    for (int i = 0; i < _numFilled; i++) {
        results[i] = _elements[i].name;
    }
    return results;
}

Vector<double> PQHeap::printDebugInfoPriority() {
    Vector<double> results(_numFilled);
    for (int i = 0; i < _numFilled; i++) {
        results[i] = _elements[i].priority;
    }
    return results;
}

/*
 * We strongly recommend implementing this helper function, which
 * should calculate the index of the parent of the element with the
 * provided index.
 */
int PQHeap::getParentIndex(int curIndex) {
    return (curIndex - 1)/ 2;
}

/*
 * We strongly recommend implementing this helper function, which
 * should calculate the index of the left child of the element with the
 * provided index.
 */
int PQHeap::getLeftChildIndex(int curIndex) {
    return (2 * curIndex) + 1;
}

/*
 * We strongly recommend implementing this helper function, which
 * should calculate the index of the right child of the element with the
 * provided index.
 */
int PQHeap::getRightChildIndex(int curIndex) {
    return (2 * curIndex) + 2;
}
