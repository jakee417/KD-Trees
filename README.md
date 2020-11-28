# KD Trees
This is the code repository for the final project of Stanford's CS106b which implements a KD Tree and conducts a K Nearest Neighbor search.

## FinalReport.pdf
Final report that explains the motivation for the setup of the problem along with an explanation of the solutions with discussion.

## kdtree.h
Header file that creates the structure and newNode and insert functions.

## NNsearch.cpp
Implements a naive 1-NN search, recursive 1-NN search with KD trees, and a recursive K-NN search with KD trees. Has simple test cases along with graphical output to confirm validity of solution.

## datapoint.h, pqheap.h, & pqheap.cpp
Helper files for NNsearch.cpp implementing a Priority Queue and datapoint struc. This was created in a previous HW assignment for the course.

## ball.h & ball.cpp
Graphics helpers for testing and viewing NN search
