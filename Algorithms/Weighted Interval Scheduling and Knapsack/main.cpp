/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 3
 * Eren CULHACI
 * 150220763
*/
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip> // for std::get_time
#include <cmath> // for std::floor
#include <map> // for std::map


using namespace std;

//structs to store data
struct interval_and_priority {
    string floorName; // Floor name
    string roomNo; // Room number
    int start; // Start time
    int end; // End time
    int priority; // Priority
};

struct Item { // Items to buy
    string name; // Item name
    int price; // Item price
    double value; // Item value
};

// Function to read intervals and priorities from files
vector<interval_and_priority> readIntervalAndPriority(const string& filename1, const string& filename2) {
    ifstream file1(filename1); // Room time intervals file
    ifstream file2(filename2); // Priority file
    vector<interval_and_priority> intervalAndPriorities; // Vector to store intervals and priorities

    string line1, line2; // Strings to store lines read from files
    // Skip the first line of both files
    getline(file1, line1);
    getline(file2, line2);

    // Create a map to store priorities for each room interval
    map<pair<string, string>, int> roomPriorities;

    // Read priorities from priority file
    while (getline(file2, line2)) { // Read line by line
        stringstream ss(line2); // Create a stringstream to parse the line
        string floorName, roomNo; // Variables to store floor name and room number
        int priority; // Variable to store priority
        ss >> floorName >> roomNo >> priority; // Parse the line
        roomPriorities[{floorName, roomNo}] = priority; // Store the priority for the room interval
    }

    // Read intervals from room_time_intervals file and merge with priorities
    while (getline(file1, line1)) { // Read line by line
        stringstream ss(line1); // Create a stringstream to parse the line
        interval_and_priority data; // Create a struct to store interval data
        // Read interval data 
        ss >> data.floorName >> data.roomNo; // Parse floor name and room number

        // Parse start and end times as strings
        string start_str, end_str; // Strings to store start and end times
        ss >> start_str >> end_str; // Parse start and end times

        // Convert start and end times to integers
        int start_hour, start_minute, end_hour, end_minute; // Variables to store start and end times
        sscanf(start_str.c_str(), "%d:%d", &start_hour, &start_minute); // Parse start time
        sscanf(end_str.c_str(), "%d:%d", &end_hour, &end_minute); // Parse end time
        data.start = start_hour * 60 + start_minute; // Convert to minutes
        data.end = end_hour * 60 + end_minute; // Convert to minutes

        // Get priority for this interval's room
        data.priority = roomPriorities[{data.floorName, data.roomNo}];

        // Store the interval data
        intervalAndPriorities.push_back(data); 
    }

    return intervalAndPriorities; // Return the vector of intervals and priorities
}

// Function to read items from file
vector<Item> readItems(const string& filename) {
    ifstream file(filename);
    vector<Item> items;
    string line;
    // Skip the first line
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        Item item;
        ss >> item.name >> item.price >> item.value;
        items.push_back(item);
    }
    return items;
}

///////////////////// WEIGHTED INTERVAL SCHEDULING START ////////////////////////////
struct Schedule { // Struct to store schedules for each floor
    vector<interval_and_priority> floors; //schedules for each floor
};

// Function to create schedules for each floor
vector<Schedule> create_schedules(vector<interval_and_priority>& interval_and_priorities) {
    vector<Schedule> schedules; // Vector to store schedules
    map<string, vector<interval_and_priority>> floorIntervals; // Map to group intervals by floor

    // Group intervals by floor
    for (const auto& interval : interval_and_priorities) { // Iterate over intervals
        floorIntervals[interval.floorName].push_back(interval); // Group intervals by floor
    }

for (auto it = floorIntervals.begin(); it != floorIntervals.end(); ++it) { // Iterate over floors
    Schedule schedule; // Create a schedule for the floor
    schedule.floors = it->second; // Store intervals for the floor
    schedules.push_back(schedule); // Store the schedule
}
    return schedules; // Return the schedules
}


// function to find the best schedule for each floor
vector<Schedule> weightedIntervalScheduling(vector<Schedule>& schedules) {
    vector<Schedule> optimal_schedules; // Vector to store optimal schedules
    for (auto& schedule : schedules) { // Iterate over schedules
        vector<interval_and_priority>& intervals = schedule.floors; // Get intervals for the floor
        sort(intervals.begin(), intervals.end(), [](const interval_and_priority& a, const interval_and_priority& b) { 
            return a.end < b.end; // Sort intervals by end times
        });

        int n = intervals.size(); // Number of intervals
        vector<int> dp(n); // Vector to store maximum priority gain for each interval
        vector<int> prev(n, -1); // Vector to store the latest non-conflicting interval for each interval

        dp[0] = intervals[0].priority; // Initialize the first interval

        // Find the latest non-conflicting interval for each interval
        for (int i = 1; i < n; ++i) { // Iterate over intervals
            int prev_interval = -1; // Initialize the latest non-conflicting interval
            for (int j = i - 1; j >= 0; --j) { // Iterate over previous intervals
                if (intervals[j].end <= intervals[i].start) { // Check if intervals are non-conflicting
                    prev_interval = j; // Update the latest non-conflicting interval
                    break; // Break the loop
                } 
            }
            prev[i] = prev_interval; // Store the latest non-conflicting interval
        }

        // Calculate maximum priority gain for each interval
        for (int i = 1; i < n; ++i) { // Iterate over intervals
            int with_current = intervals[i].priority; // Initialize priority gain with the current interval
            if (prev[i] != -1) { // Check if there is a non-conflicting interval
                with_current += dp[prev[i]]; // Update priority gain with the non-conflicting interval
            } 
            dp[i] = max(dp[i - 1], with_current); // Update maximum priority gain
        }

        // Backtrack to find selected intervals
        vector<interval_and_priority> selected_intervals; // Vector to store selected intervals
        int i = n - 1; // Start from the last interval
        while (i >= 0) { // Iterate over intervals
            if (i == 0 || dp[i] != dp[i - 1]) { // Check if the interval is selected
                selected_intervals.push_back(intervals[i]); // Store the selected interval
                if (prev[i] != -1) { // Check if there is a non-conflicting interval
                    i = prev[i]; // Update the interval
                } else { // Break the loop if there is no non-conflicting interval
                    break; 
                }
            } else { // If the interval is not selected
                --i; // Move to the previous interval
            }
        }

        reverse(selected_intervals.begin(), selected_intervals.end()); // Reverse the selected intervals
        Schedule optimal_schedule; // Create a schedule for the optimal intervals
        optimal_schedule.floors = selected_intervals; // Store the selected intervals
        optimal_schedules.push_back(optimal_schedule); // Store the optimal schedule
    }

    return optimal_schedules; // Return the resultant optimal schedules
}

///////////////////// WEIGHTED INTERVAL SCHEDULING END ////////////////////////////

///////////////////// KNAPSACK PROBLEM START ////////////////////////////
// Function to select the most valuable items that can be purchased with a certain budget
vector<Item> knapsack(const vector<Item>& items, int budget) { // parameters are items vector and budget value
    vector<vector<double>> dp(items.size() + 1, vector<double>(budget + 1, 0)); // DP table to store maximum value for each item and budget
    vector<vector<bool>> selected(items.size() + 1, vector<bool>(budget + 1, false)); // Table to store selected items

    for (size_t i = 1; i <= items.size(); ++i) { // Iterate over items
        for (int j = 0; j <= budget; ++j) { // Iterate over budgets
            if (items[i - 1].price <= j) { // Check if the item can be purchased
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - items[i - 1].price] + items[i - 1].value); // Update maximum value with and without the item
                if (dp[i][j] != dp[i - 1][j]) { // Check if the item is selected
                    selected[i][j] = true; // Mark the item as selected
                }
            } else { // If the item cannot be purchased
                dp[i][j] = dp[i - 1][j]; // Update maximum value without the item 
            }
        }
    }

    vector<Item> selectedItems; // Vector to store selected items
    int j = budget; // Initialize the budget
    for (int i = items.size(); i > 0 && j > 0; --i) { // Backtrack to find selected items
        if (selected[i][j]) { // Check if the item is selected
            selectedItems.push_back(items[i - 1]); // Store the selected item
            j -= items[i - 1].price; // Update the budget
        }
    }

    return selectedItems; // Return the selected items
}
///////////////////// KNAPSACK PROBLEM END //////////////////////////////

int main(int argc, char *argv[]) {
    // Check if correct number of arguments is provided
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <case_number>" << endl;
        return 1;
    }

    string case_no = argv[1]; // Get the case number
    string case_name = "case_" + case_no; // Create the case name
    string path = "./inputs/" + case_name; // Create the path to the case folder

    int total_budget = 200000; // Total budget

    // File paths for input files
    string room_intervals_file = path + "/room_time_intervals.txt"; // Room time intervals file
    string priority_file = path + "/priority.txt"; // Priority file
    string items_file = path + "/items.txt"; // Items file

    // Read input files
    vector<interval_and_priority> interval_and_priorities = readIntervalAndPriority(room_intervals_file, priority_file);
    vector<Item> items = readItems(items_file);

    // Weighted interval scheduling
    vector<Schedule> schedules = create_schedules(interval_and_priorities);
    vector<Schedule> optimalSchedules = weightedIntervalScheduling(schedules);

    // Output the schedules
    cout << "Best Schedule for Each Floor" << endl << endl;
    for (const auto& schedule : optimalSchedules) { // Iterate over schedules
        cout << schedule.floors[0].floorName << " --> Priority Gain: "; // Output floor name
        int totalPriority = 0; // Initialize total priority with 0
        for (const auto& interval : schedule.floors) { // Iterate over intervals
            totalPriority += interval.priority; // Update total priority for the floor
        }
        cout << totalPriority << endl; // Output total priority gain
        for (const auto& interval : schedule.floors) { // Iterate over intervals
            cout << interval.floorName << "\t" << interval.roomNo << "\t" // Output floor name and room number
                << setw(2) << setfill('0') << interval.start / 60 << ":" // Output start time by converting the integer value into hour
                << setw(2) << setfill('0') << interval.start % 60 << "\t" // Output start time by converting the integer value into minute
                << setw(2) << setfill('0') << interval.end / 60 << ":" // Output end time by converting the integer value into hour
                << setw(2) << setfill('0') << interval.end % 60 << endl; // Output end time by converting the integer value into minute
        }
        cout << endl; // Output an empty line
    }

    // Knapsack problem
    vector<Item> selectedItems = knapsack(items, total_budget); // Solve the knapsack problem by calling the knapsack function
    // Output the selected items
    double totalValue = 0; // Initialize total value with 0
    cout << "Best Use of Budget" << endl; // Output the title
    for (const auto& item : selectedItems) { // Iterate over selected items
        totalValue += item.value; // Update total value
    }
    totalValue = std::floor(totalValue * 10 + 0.5) / 10; // Round to the nearest tenth
    cout << "Total Value --> " << std::fixed << std::setprecision(1) << totalValue << endl << endl; // Output total value with one decimal point
    for (const auto& item : selectedItems) { // Iterate over selected items
        cout << item.name << endl; // Output item name
    }
    return 0; // End the program
}
