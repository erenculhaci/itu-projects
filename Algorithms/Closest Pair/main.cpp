/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 2
@Author
* Student Name: EREN CULHACI
* Student ID:  150220763
*/

#include <iostream>   // For input/output
#include <cmath>      // For distance calculations
#include <vector>     // For vector operations
#include <algorithm>  // For sorting operations
#include <limits>     // For min(), max() functions
#include <fstream>    // To read/write files
#include <chrono>     // For time measurement

//(These libraries are sufficient for this assignment.)

using namespace std;

// Structure to represent a point in 2D space
struct Point {
    double x, y; // x and y coordinates of the point
};

// Helper function to calculate distance between two points
double distance(Point p1, Point p2) {
    double x_diff = p2.x - p1.x; // Calculate the difference between x-coordinates of two points
    double y_diff = p2.y - p1.y; // Calculate the difference between y-coordinates of two points
    return std::sqrt(x_diff * x_diff + y_diff * y_diff); // Return the Euclidean distance
}

// Helper function to compare points based on x-coordinate, can use this for sorting.
bool compareX(Point p1, Point p2) {
    return p1.x < p2.x; // Return true if the x-coordinate of the first point is less than the x-coordinate of the second point
}

// Helper function to compare points based on y-coordinate, can use this for sorting.
bool compareY(Point p1, Point p2) {
    return p1.y < p2.y; // Return true if the y-coordinate of the first point is less than the y-coordinate of the second point
}

// Function to find the closest pair of points recursively using brute force.
pair<Point, Point> bruteForceClosestPair(vector<Point>& points) {
    pair<Point, Point> closestPair; // Initialize a pair for the closest points
    double minDist = numeric_limits<double>::max(); // Initialize the minimum distance to the maximum value of double
    
    for (size_t i = 0; i < points.size(); ++i) { // Iterate over the points
        for (size_t j = i + 1; j < points.size(); ++j) { // Iterate over the points
            double dist = distance(points[i], points[j]); // Calculate the distance between the points
            if (dist < minDist) { // If the distance is less than the minimum distance
                minDist = dist; // Update the minimum distance
                closestPair = make_pair(points[i], points[j]); // Update the closest pair
            }
        }
    }
    
    return closestPair; // Return the closest pair of points
}

// Function to find the closest pair of points recursively using divide and conquer.
pair<Point, Point> closestPair(vector<Point>& points) {
    if (points.size() <= 3) { // If there are 3 or fewer points, use brute force
        return bruteForceClosestPair(points);
    }

    // Sort points by x-coordinate
    sort(points.begin(), points.end(), [](const Point& p1, const Point& p2) {
        return p1.x < p2.x;
    });

    // Divide the points into two halves
    size_t mid = points.size() / 2; // Find the middle index
    vector<Point> leftHalf(points.begin(), points.begin() + mid); // Create a vector for the left half
    vector<Point> rightHalf(points.begin() + mid, points.end()); // Create a vector for the right half

    // Recursively find the closest pair in each half
    pair<Point, Point> leftClosest = closestPair(leftHalf); // Find the closest pair in the left half
    pair<Point, Point> rightClosest = closestPair(rightHalf); // Find the closest pair in the right half

    // Find the minimum distance between the closest pairs in left and right halves
    double minDist = min(distance(leftClosest.first, leftClosest.second),
                         distance(rightClosest.first, rightClosest.second));

    // Merge the results from left and right halves
    pair<Point, Point> mergedClosest; // Initialize a pair for the merged closest points
    if (minDist == 0) {
        // If the minimum distance is 0, return one of the pairs with 0 distance
        if (distance(leftClosest.first, leftClosest.second) == 0)
            return leftClosest;
        else
            return rightClosest;
    }

    // Create a strip vector to store points within the minimum distance strip
    vector<Point> strip; // Initialize a vector for the strip
    for (const Point& p : points) { // Iterate over the points
        if (abs(p.x - points[mid].x) < minDist) // If the point is within the strip
            strip.push_back(p); // Add the point to the strip
    } 

    // Sort points in the strip by y-coordinate
    sort(strip.begin(), strip.end(), [](const Point& p1, const Point& p2) {
        return p1.y < p2.y; 
    });

    // Compare points in the strip to find closest pair within the strip
    for (size_t i = 0; i < strip.size(); ++i) { // Iterate over the strip
        for (size_t j = i + 1; j < strip.size() && strip[j].y - strip[i].y < minDist; ++j) { 
            double dist = distance(strip[i], strip[j]); // Calculate the distance between the points
            if (dist < minDist) { // If the distance is less than the minimum distance
                minDist = dist; // Update the minimum distance
                mergedClosest = make_pair(strip[i], strip[j]); // Update the merged closest pair
            }
        }
    }

    // Return the closest pair among left, right, and merged closest pairs
    if (minDist == distance(leftClosest.first, leftClosest.second)) // If the minimum distance is the distance between the left closest pair
        return leftClosest; // Return the left closest pair
    else if (minDist == distance(rightClosest.first, rightClosest.second)) // If the minimum distance is the distance between the right closest pair
        return rightClosest; // Return the right closest pair
    else // If the minimum distance is the distance between the merged closest pair
        return mergedClosest; // Return the merged closest pair
}



// Function to remove a pair of points from the point vector. Returns the newly formed vector.
vector<Point> removePairFromVector(vector<Point>& point_vector, pair<Point,Point> point_pair) {
    // Iterate over the vector and remove points that match either of the points in the pair
    point_vector.erase(remove_if(point_vector.begin(), point_vector.end(), [&](const Point& p){
        return (p.x == point_pair.first.x && p.y == point_pair.first.y) || 
               (p.x == point_pair.second.x && p.y == point_pair.second.y);
    }), point_vector.end());

    return point_vector; // Return the modified vector
}

// This function should wrap up the entire algorithm. It should:
//    1- Find the closest pair
//    2- Add the pair to the "pairs" vector
//    3- Remove the paired result from the map
// at each iteration.
// Then it should set "unconnected" variable as the final, unconnected city (if it exists).
void findClosestPairOrder(vector<Point> points) {
    vector<pair<Point,Point>> pairs; //add your pairs to this vector
    Point unconnected = {-1,-1}; //set this as the unconnected point if it exists, leave as is otherwise.

    while(points.size() > 1) { // While there are more than one points left
        pair<Point, Point> closestpair = closestPair(points); // Find the closest pair of points
        pairs.push_back(closestpair); // Add the closest pair to the pairs vector
        points = removePairFromVector(points, closestpair); // Remove the closest pair from the points vector
    }
    if (points.size() == 1) { // If there is one point left
        unconnected = points[0]; // Set the unconnected point as the remaining point
    }

    //this sorts the points in each pair according to the point with the smaller y-coordinate, if equal smaller x coordinate.
    for (auto& pair : pairs) { // Iterate over the pairs
        if (compareY(pair.second, pair.first)) { // If the second point has a smaller y-coordinate than the first point
            swap(pair.first, pair.second); // Swap the points
        } else if (pair.first.y == pair.second.y && compareX(pair.second, pair.first)) { // If the points have the same y-coordinate and the second point has a smaller x-coordinate
            swap(pair.first, pair.second); // Swap the points
        }
    }

    //This part prints the output, don't modify.    
    for(size_t i = 0; i < pairs.size(); i++){
        cout << "Pair " << i+1 << ": " << pairs[i].first.x << ", " << pairs[i].first.y << " - " << pairs[i].second.x << ", " << pairs[i].second.y << endl;
    }
    if (unconnected.x != -1){
        cout << "Unconnected " << unconnected.x << ", " << unconnected.y;
    }
}

//Read the coordinates from the file and convert them to a vector. Return the vector as a "vector<Point>"
vector<Point> readCoordinatesFromFile(const string& filename) {
    vector<Point> points;
    ifstream file(filename);    // Open the file
    if (file.is_open()) {       // If the file is open
        string line;            // Initialize a string to read lines
        while (getline(file, line)) { // Read lines from the file
            Point point;        // Initialize a point
            sscanf(line.c_str(), "%lf %lf", &point.x, &point.y); // Read the coordinates from the line
            points.push_back(point); // Add the point to the vector
        }
        file.close();           // Close the file
    }
    return points; // Return the vector of points
}

// Main function. Call your other functions here.
int main(int argc, char* argv[]) {
    vector<Point> points = readCoordinatesFromFile(argv[1]); // Read the coordinates from the file
    findClosestPairOrder(points); // Find the closest pair of points and print the results
    return 0;
}
    
