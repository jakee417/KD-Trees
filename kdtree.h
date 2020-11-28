#pragma once
/*
 * Helper file that builds the KD tree
 */
#include "vector.h"
#include <string>
using namespace std;

const int d = 2; // set dimension to 2 (this can be generalized)

/*////////////////////////////////////////////////////////////
 * Structure for K-D Tree
 * ///////////////////////////////////////////////////////////
 */
/**
 * Create a struct that holds a location, and two nodes
 */
struct Node {
    Vector<int> location = {0,0}; // this would need to be changed for different k
    Node* left;
    Node* right;
};

/*////////////////////////////////////////////////////////////
 * Functions for building a K-D Tree
 * ///////////////////////////////////////////////////////////
 */
/**
 * A method to create a node of a K-D tree. Runs in O(1) time and
 * returns the new node as a struct.
 */
struct Node* newNode(Vector<int> newLocation){
    struct Node* temp = new Node;

    for (int i=0; i<d; i++){
        temp->location[i] = newLocation[i];

    }

    temp->left = temp->right = nullptr;

    return temp;
}

/**
 * Helper function for insert()
 */
Node* insertRec(Node* root, Vector<int> point, int depth){
    if (root == nullptr)
       return newNode(point);

    int currDimension = depth % d;

    // Compare the new point with root on current dimension
    if (point[currDimension] < (root->location[currDimension])){
        root->left  = insertRec(root->left, point, depth + 1);
    } else{
        root->right = insertRec(root->right, point, depth + 1);
    }
    return root;
}

/**
 * Recursively inserts a new node into a KD tree. Runs in O(log(n))
 * where n is the number of elements already in the KD tree.
 * Returns a pointer to the modifed tree.
 */
Node* insert(Node* root, Vector<int> point){
    return insertRec(root, point, 0);
}
