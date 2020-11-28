/*
 * File: NNsearch.cpp
 * --------------
 * Final Project
 * Jake Taylor
 */

#include <iostream>
#include "console.h"
#include "vector.h"
#include "testing/SimpleTest.h"
#include "gobjects.h"
#include "gwindow.h"
#include "ball.h"
#include "kdtree.h"
#include "pqheap.h"
using namespace std;

/*////////////////////////////////////////////////////////////
 * Global Constants
 * ///////////////////////////////////////////////////////////
 */
static const int ADJUST = 6;
static const int SCREEN_WIDTH = 1000; // #GotTwoMonitors?
static const int SCREEN_HEIGHT = 1000;
static const int BASE_Y = SCREEN_HEIGHT - SCREEN_HEIGHT * .4;
static const int BASE_LEFT_X = 170;
static const int BASE_RIGHT_X = SCREEN_WIDTH - 170;
static const double KLargeNumber = 99999999999999999;

/*////////////////////////////////////////////////////////////
 * Helper Functions common to both solutions
 * ///////////////////////////////////////////////////////////
 */
// Helper function that calculates squared distance between
// a query point and a node
double distSquared(Vector<int> candidate, Vector<int> query){
    double distance = 0;
    for (int i=0; i < d; i++){
        distance += pow((query[i] - candidate[i]),2);
    }
    return distance;
}

/*////////////////////////////////////////////////////////////
 * Code for finding Nearest Neighbor (Naive Solution)
 * ///////////////////////////////////////////////////////////
 */
/* Using the Priority Queue data structure as a tool to sort and find the NN! */
Vector<int> nearestNaive(Vector<Vector<int>> locations, Vector<int> candidate) {
    PQHeap pq;
    Vector<DataPoint> NN(locations.size());

    /* Add all the elements to the priority queue. */
    for (int i = 0; i < locations.size(); i++) {
        double distance = distSquared(locations[i], candidate);
        pq.enqueue({integerToString(i), distance});
    }

    for (int i = 0; i < NN.size(); i++) {
        NN[i] = pq.dequeue();
    }
    DataPoint result = NN[0];
    return locations[stringToInteger(result.name)];
}

/*////////////////////////////////////////////////////////////
 * Code for finding Nearest Neighbor (Fast Solution)
 * ///////////////////////////////////////////////////////////
 */
// helper function that determines the closer node to a query point
Node* closest(Vector<int> query, Node* node1, Node* node2){
    if (node1 == nullptr){
        return node2;
    }

    if (node2 == nullptr){
        return node1;
    }

    double d1 = distSquared(node1 -> location, query);
    double d2 = distSquared(node2 -> location, query);
    if(d1 < d2){
        return node1;
    } else {
        return node2;
    }
}

// Recursive function that finds the nearest neighbor between a
// K-D tree and a query point. Uses backtracking to eliminate options
// from the search area.
Node* nearestNeighborRec(Node* root, Vector<int> query, int depth, int& numVisited){
    if(root == nullptr){
        return nullptr;
    }

    // Keeping track of # of visited nodes
    numVisited += 1;
    //cout << "Visiting node number: " << numVisited << "..." << endl;

    // modulo operation cycling through dimensions
    int currDimension = depth % d;

    Node* nextBranch = nullptr;
    Node* otherBranch = nullptr;

    // checking to see which half-plane we recurse down
    if(query[currDimension] < (root -> location[currDimension])){
        nextBranch = root -> left;
        otherBranch = root -> right;
    } else {
        nextBranch = root -> right;
        otherBranch = root -> left;
    }

    // search down the half-plane containing our query
    Node* temp = nearestNeighborRec(nextBranch, query, depth + 1, numVisited);
    Node* best = closest(query, temp, root);

    // current radius around best point
    double radiusSquared = distSquared(best -> location, query);

    // 1-d distance calculation to the alternative half-plane while backtracking
    double alternativeDistance = query[currDimension] - root -> location[currDimension];

    // Check the other path while backtracking to the root node
    // note we are comparing squared distances to save computations!
    if(radiusSquared >= pow(alternativeDistance, 2)){
        temp = nearestNeighborRec(otherBranch, query, depth + 1, numVisited);
        best = closest(query, temp, best);
    }
    return(best);
}

// Function that computes the NN between a root and a query location
Node* nearestFast(Node* root, Vector<int> query){
    int numVisited = 0;
    return nearestNeighborRec(root, query, 0, numVisited);
}

/*////////////////////////////////////////////////////////////
 * Code for finding K Nearest Neighbors (Fast Solution)
 * ///////////////////////////////////////////////////////////
 */
// Helper function that adds to the priority queue
void topK(PQHeap& nearest, Node* candidate, double distance, int k, int& numVisited){
    DataPoint entry;
    // handle nullptr case
    // convert distances to priorities using 1/x transform
    if(candidate == nullptr){return;}
    else{
        //cout << "Visiting node number: " << numVisited << "..." << endl;
        numVisited += 1;
        entry = {candidate->location.toString(), -distance};
        if(nearest.size() < k){
            nearest.enqueue(entry);
        } else if(nearest.peek().priority < entry.priority) {
                nearest.dequeue();
                nearest.enqueue(entry);
        }
    }
    //cout << nearest.printDebugInfo().toString() << endl;
    //cout << nearest.printDebugInfoPriority().toString() << endl;
}

// Helper function that recursively searches the tree and adds the nodes to the pq
void KnearestNeighborRec(Node* root, Vector<int> query, int depth, int& numVisited, PQHeap& nearest, int k){
    if(root == nullptr){return;}

    // Keeping track of # of visited nodes
    double radiusSquared = distSquared(root -> location, query);
    topK(nearest, root, radiusSquared, k, numVisited);

    // modulo operation cycling through dimensions
    int currDimension = depth % d;
    Node* nextBranch = nullptr;
    Node* otherBranch = nullptr;

    // checking to see which half-plane we recurse down
    if(query[currDimension] < (root -> location[currDimension])){
        nextBranch = root -> left;
        otherBranch = root -> right;
    } else {
        nextBranch = root -> right;
        otherBranch = root -> left;
    }

    // search down the half-plane containing our query
    KnearestNeighborRec(nextBranch, query, depth + 1, numVisited, nearest, k);

    // 1-d distance calculation to the alternative half-plane while backtracking
    double alternativePriority = -pow(query[currDimension] - root -> location[currDimension], 2);

    // Check the other path while backtracking to the root node
    // note we are comparing squared distances to save computations!
    if(nearest.size() < k || nearest.peek().priority < alternativePriority){
        // recurse down path and see if nodes make topK
        KnearestNeighborRec(otherBranch, query, depth + 1, numVisited, nearest, k);
    }
}

// Function that computes the KNN between a root and a query location
void KnearestFast(Node* root, Vector<int> query, PQHeap& nearest, int k){
    int numVisited = 0;
    KnearestNeighborRec(root, query, 0, numVisited, nearest, k);
}

/*////////////////////////////////////////////////////////////
 * Test Cases - Graphical Output
 * ///////////////////////////////////////////////////////////
 */
PROVIDED_TEST("7 node KD tree with 4 queries from example"){
    struct Node *root = nullptr;
    Vector<Vector<int>> locations = {{3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}};

    int n = locations.size();

    for (int i=0; i<n; i++)
       root = insert(root, locations[i]);

    Node* NN = nullptr;
    Vector<int> house = {15,16};
    NN = nearestFast(root, house);
    cout << "Nearest Neighbor found at x = " << NN ->location[0] <<
            " y = " << NN -> location[1] << endl;
    EXPECT_EQUAL(17, NN ->location[0]);
    EXPECT_EQUAL(15, NN ->location[1]);

    Vector<int> naiveNN = nearestNaive(locations, house);
    EXPECT_EQUAL(17, naiveNN[0]);
    EXPECT_EQUAL(15, naiveNN[1]);

    house = {0,8};
    NN = nearestFast(root, house);
    cout << "Nearest Neighbor found at x = " << NN ->location[0] <<
            " y = " << NN -> location[1] << endl;
    EXPECT_EQUAL(2, NN ->location[0]);
    EXPECT_EQUAL(7, NN ->location[1]);

    naiveNN = nearestNaive(locations, house);
    EXPECT_EQUAL(2, naiveNN[0]);
    EXPECT_EQUAL(7, naiveNN[1]);

    house = {15,5};
    NN = nearestFast(root, house);
    cout << "Nearest Neighbor found at x = " << NN ->location[0] <<
            " y = " << NN -> location[1] << endl;
    EXPECT_EQUAL(9, NN ->location[0]);
    EXPECT_EQUAL(1, NN ->location[1]);

    naiveNN = nearestNaive(locations, house);
    EXPECT_EQUAL(9, naiveNN[0]);
    EXPECT_EQUAL(1, naiveNN[1]);

    house = {0,20};
    NN = nearestFast(root, house);
    cout << "Nearest Neighbor found at x = " << NN ->location[0] <<
            " y = " << NN -> location[1] << endl;
    EXPECT_EQUAL(6, NN ->location[0]);
    EXPECT_EQUAL(12, NN ->location[1]);

    naiveNN = nearestNaive(locations, house);
    EXPECT_EQUAL(6, naiveNN[0]);
    EXPECT_EQUAL(12, naiveNN[1]);
}

PROVIDED_TEST("Test KNN on example from handout"){
    struct Node *root = nullptr;
    Vector<Vector<int>> locations = {{3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}};

    int n = locations.size();
    for (int i=0; i<n; i++)
       root = insert(root, locations[i]);

    Node* NN = nullptr;
    Vector<int> house = {15,16};
    PQHeap nearest;
    int k = 3;
    KnearestFast(root, house, nearest, k);

    Vector<string> result(k);
    DataPoint temp;
    if(nearest.size() == 0){
        result = {};
    } else {
        // add going backwards for O(1) insert
        for(int i = result.size() - 1; i >= 0; i--){
            temp = nearest.dequeue();
            result[i] = temp.name;
        }
    }
    cout << result.toString() << endl;
}

PROVIDED_TEST("Test with graphics, validate with naive solution"){
    GWindow w(SCREEN_WIDTH, SCREEN_HEIGHT);
    GPoint bottomLeft(BASE_LEFT_X, BASE_Y);
    GPoint bottomRight(BASE_RIGHT_X, BASE_Y);
    w.setColor("black");
    w.setFillColor("black");
    w.setFont("*-Bold-10");

    struct Node *root = nullptr;

    // Vector of restaurant locations
    Vector<Vector<int>> locations;

    // Initialize restaurants
    for (int i = 0; i < 2000; i++) {
        int _x = randomInteger(0, w.getWidth());
        int _y = randomInteger(0, w.getHeight());
        locations.add({_x,_y});
    }

    // Create KD tree and add to graphic
    int n = locations.size();
    for (int i = 0; i < n; i++){
        root = insert(root, locations[i]);
        Vector<int> pos = locations[i];
        Ball ball(&w, i, pos[0], pos[1]);
        ball.draw("Green", true);
    }

    // Initialize houses and perform both NN searches
    for (int i = 0; i < 100; i++) {
        Node* NN = nullptr;
        int _x = randomInteger(0, w.getWidth());
        int _y = randomInteger(0, w.getHeight());
        Vector<int> house = {_x, _y};
        Vector<int> pos = house;
        Ball ball(&w, i, house[0], house[1]);
        ball.draw("Red", false);

        // Conduct NN search over all houses using the KD tree
        NN = nearestFast(root, house);
        int xNN = NN -> location[0];
        int yNN = NN -> location[1];
//        cout << "Nearest Neighbor found at x = " << xNN <<
//                " y = " << yNN << endl;

        // Draw results
        GPoint source((double) _x + ADJUST, (double) _y + ADJUST);
        GPoint dest((double) xNN + ADJUST, (double) yNN + ADJUST);
        w.drawLine(source, dest);

        // Test against naive solution for correctness
        Vector <int> naiveNN = nearestNaive(locations, house);
        EXPECT_EQUAL(naiveNN[0], xNN);
        EXPECT_EQUAL(naiveNN[1], yNN);
    }
}

PROVIDED_TEST("Test KNN with graphics"){
    GWindow w(SCREEN_WIDTH, SCREEN_HEIGHT);
    GPoint bottomLeft(BASE_LEFT_X, BASE_Y);
    GPoint bottomRight(BASE_RIGHT_X, BASE_Y);
    w.setColor("black");
    w.setFillColor("black");
    w.setFont("*-Bold-10");

    struct Node *root = nullptr;

    // Vector of restaurant locations
    Vector<Vector<int>> locations;

    // Initialize restaurants
    for (int i = 0; i < 10000; i++) {
        int _x = randomInteger(0, w.getWidth());
        int _y = randomInteger(0, w.getHeight());
        locations.add({_x,_y});
    }

    // Create KD tree and add to graphic
    int n = locations.size();
    for (int i = 0; i < n; i++){
        root = insert(root, locations[i]);
        Vector<int> pos = locations[i];
        Ball ball(&w, i, pos[0], pos[1]);
        ball.draw("Green", true);
    }

    // Initialize houses and perform KNN searches
    for (int i = 0; i < 30; i++) {
        int _x = randomInteger(0, w.getWidth());
        int _y = randomInteger(0, w.getHeight());
        Vector<int> house = {_x, _y};
        Vector<int> pos = house;

        // Conduct NN search over all houses using the KD tree
        PQHeap nearest;
        int k = randomInteger(100, 200);
        KnearestFast(root, house, nearest, k);
        Vector<string> result(3);
        if(nearest.size() == 0){
            result = {};
        } else {
            Vector<string> result(k);
            while(!nearest.isEmpty()){
                DataPoint temp = nearest.dequeue();
                string pos = temp.name;
                result.add(pos);
                Vector<string> parsed = stringSplit(pos, ",");
                int xNN = stringToInteger(parsed[0].substr(1,parsed[0].size()-1));
                int yNN = stringToInteger(parsed[1].substr(0,parsed[1].size()-1));
                // Draw results
                GPoint source((double) _x + ADJUST, (double) _y + ADJUST);
                GPoint dest((double) xNN + ADJUST, (double) yNN + ADJUST);
                w.drawLine(source, dest);
            }
            //cout << result.toString() << endl;
            nearest.clear();
            Ball ball(&w, i, house[0], house[1]);
            ball.draw("Blue", false);
        }
    }
}

