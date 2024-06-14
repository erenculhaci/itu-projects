/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 1
 * Student Name: EREN CULHACI
 * Student ID: 150220763
*/

/* * * * * * * * * * * * * * * * * *
 * DO NOT MODIFY UNLESS REQUESTED  *
* * * * * * * * * * * * * * * * * */
#include <iostream>         
#include <vector>
#include <fstream>
#include <sstream>       
#include <stack>
#include <queue>
#include <chrono>       // For time measurement
#include <algorithm> // included for sorting 


using namespace std;
using namespace std::chrono;

/**
 * Perform Depth-First Search (DFS) traversal on a map.
 * 
 * @param map The map represented as a 2D vector of integers.
 * @param row The starting row index for the DFS traversal.
 * @param col The starting column index for the DFS traversal.
 * @param resource The resource value to search for during the traversal.
 * @return The size of the colony found during the DFS traversal.
 */
// Perform Depth-First Search (DFS) traversal on a map
int dfs(vector<vector<int>>& map, int row, int col, int resource, vector<vector<bool>>& visited) {
    int size = 0; // Colony size initialized to 0
    stack<pair<int, int>> s; // Stack to store the indices of the cells
    s.push(make_pair(row, col)); // Push the starting cell to the stack
    while (!s.empty()) { // While the stack is not empty
        pair<int, int> p = s.top(); // Get the top element from the stack
        s.pop(); // Remove the top element from the stack
        int r = p.first; // Get the row index
        int c = p.second; // Get the column index
        if (r < 0) // If the row index is negative
            r = map.size() - 1; // Wrap around to the bottom row
        else if (r >= (int)map.size()) // If the row index is greater than or equal to the number of rows
            r = 0; // Wrap around to the top row
        if (c < 0) // If the column index is negative
            c = map[0].size() - 1; // Wrap around to the rightmost column
        else if (c >= (int)map[0].size()) // If the column index is greater than or equal to the number of columns
            c = 0; // Wrap around to the leftmost column
        if (map[r][c] != resource || visited[r][c]) // If the cell does not contain the resource or it is already visited
            continue; // Skip the cell
        visited[r][c] = true; // Mark the cell as visited
        size++; // Increment the colony size
        s.push(make_pair(r - 1, c)); // Move north
        s.push(make_pair(r + 1, c)); // Move south
        s.push(make_pair(r, c - 1)); // Move west
        s.push(make_pair(r, c + 1)); // Move east
    }
    return size; // Return the size of the colony
}



/**
 * Perform Breadth-First Search (BFS) traversal on a map.
 * 
 * @param map The map represented as a 2D vector of integers.
 * @param row The starting row index for the BFS traversal.
 * @param col The starting column index for the BFS traversal.
 * @param resource The resource value to search for during the traversal.
 * @return The size of the colony found during the BFS traversal.
 */
int bfs(vector<vector<int>>& map, int row, int col, int resource, vector<vector<bool>>& visited) {
    int size = 0; // Colony size initialized to 0
    queue<pair<int, int>> q; // Queue to store the indices of the cells
    q.push(make_pair(row, col)); // Push the starting cell to the queue
    while (!q.empty()) { // While the queue is not empty
        pair<int, int> p = q.front(); // Get the front element from the queue
        q.pop(); // Remove the front element from the queue
        int r = p.first; // Get the row index
        int c = p.second; // Get the column index
        if (r < 0) // If the row index is negative
            r = map.size() - 1; // Wrap around to the bottom row
        else if (r >= (int)map.size()) // If the row index is greater than or equal to the number of rows
            r = 0; // Wrap around to the top row
        if (c < 0) // If the column index is negative
            c = map[0].size() - 1; // Wrap around to the rightmost column
        else if (c >= (int)map[0].size()) // If the column index is greater than or equal to the number of columns
            c = 0; // Wrap around to the leftmost column
        if (map[r][c] != resource || visited[r][c]) // If the cell does not contain the resource or it is already visited
            continue; // Skip the cell
        visited[r][c] = true; // Mark the cell as visited
        size++; // Increment the colony size
        q.push(make_pair(r - 1, c)); // Move north
        q.push(make_pair(r + 1, c)); // Move south
        q.push(make_pair(r, c - 1)); // Move west
        q.push(make_pair(r, c + 1)); // Move east
    }
    return size; //
}

/**
 * Finds the top-k largest colonies in a given map.
 *
 * @param map The map represented as a 2D vector of integers.
 * @param useDFS A boolean flag indicating whether to use Depth-First Search (DFS) or Breadth-First Search (BFS) algorithm.
 * @param k The number of top colonies to find.
 * @return A vector of pairs representing the size and resource type of the top-k largest colonies.
 */
vector<pair<int, int>> top_k_largest_colonies(vector<vector<int>>& map, bool useDFS, unsigned int k) {
    auto start = high_resolution_clock::now(); // Start measuring time

    int rows = map.size(); // Get the number of rows
    int cols = map.empty() ? 0 : map[0].size(); // Check if the map is empty else get the number of columns
    if (rows == 0 || cols == 0) { // If the map is empty
        return {}; // Return empty vector if the map is empty
    }
    vector<vector<bool>> visited(rows, vector<bool>(cols, false)); // Create a 2D vector to store the visited cells
    vector<pair<int, int>> colonies; // Pair of colony size and resource type

    for (int i = 0; i < rows; ++i) { // Iterate over the rows
        for (int j = 0; j < cols; ++j) { // Iterate over the columns
            if (!visited[i][j]) { // If the cell is not visited
                int resource = map[i][j]; // Get the resource type
                int size = useDFS ? dfs(map, i, j, resource, visited) : bfs(map, i, j, resource, visited); // if useDFS is true, use DFS else use BFS
                colonies.push_back({size, resource}); // Add the colony size and resource type to the colonies vector
            }
        }
    }

    sort(colonies.begin(), colonies.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        // Sort the colonies vector in descending order of size and ascending order of resource type
        if (a.first != b.first) // If sizes are different, sort by size
            return a.first > b.first; //
        else // If sizes are equal, sort by resource type
            return a.second < b.second; // Smallest resource type first
    });

    vector<pair<int, int>> result; // Vector to store the top-k largest colonies
    for (unsigned int i = 0; i < min(k, (unsigned int)colonies.size()); ++i) { // Iterate over the colonies vector
        result.push_back(colonies[i]); // Add the top-k largest colonies to the result vector
    }
    
   sort(result.begin(), result.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
    /*This is a sort function just to be sure that the result is sorted in the same way as the expected 
    example in the .yaml file.*/
    if (a.first != b.first) // If sizes are different, sort by size
        return a.first > b.first; 
    else // If sizes are equal, sort by resource type
        return a.second < b.second; // Smallest resource type first
});

    auto stop = high_resolution_clock::now(); // Stop measuring time
    auto duration = duration_cast<nanoseconds>(stop - start); // Calculate the duration
    cout << "Time taken: " << duration.count() << " nanoseconds" << endl;

    
    return result; // Return the top-k largest colonies
}

// Main function
int main(int argc, char* argv[]) {

    // Check the command line arguments
    if (argc != 4) {

        // It should be: <1 or 0> <k> <filename.txt>
        cerr << "Usage: " << argv[0] << " <1 or 0> <k> <filename.txt>" << endl;
        return 1;
    }

    // Get the arguments
    bool useDFS = (atoi(argv[1]) == 1);
    int k = atoi(argv[2]);

    // Read the map from the text file
    string filename = argv[3];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    vector<vector<int>> sector_map;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        vector<int> row;
        int num;
        while (iss >> num) {
            row.push_back(num);
        }
        sector_map.push_back(row);
    }
    file.close();   

    // Find the largest colony
    vector<pair<int, int>> result = top_k_largest_colonies(sector_map, useDFS, k);

    // Print the result
    cout << "Algorithm: " << (useDFS ? "DFS" : "BFS") << endl;
    cout << "Map: " << filename << endl;

    // If no colonies are found
    if (result.empty()) {
        cout << "No colonies found." << endl;
        return 0;
    }

    // Print the top-k colonies
    for (size_t i = 0; i < result.size(); ++i) {
        cout << "Colony " << i + 1 << ": Size = " << result[i].first << ", Resource Type = " << result[i].second << endl;
    }

    return 0;
} 