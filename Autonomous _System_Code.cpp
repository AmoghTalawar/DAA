#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cmath>
#include <algorithm> 

using namespace std;

// Define a structure for representing hospital graph (adjacency list)
class HospitalGraph {
public:
    vector<vector<pair<int, int>>> graph;

    HospitalGraph(int n) {
        graph.resize(n);
    }

    void addEdge(int u, int v, int weight) {
        graph[u].push_back({v, weight});
        graph[v].push_back({u, weight});
    }
};

// A* Algorithm for shortest path
vector<int> aStar(HospitalGraph& hospital, int start, int goal) {
    int n = hospital.graph.size();
    vector<int> gScore(n, INT_MAX), fScore(n, INT_MAX), cameFrom(n, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> openSet;

    // Manhattan distance heuristic
    auto heuristic = [](int a, int b) -> int {
        return abs(a - b); // Example heuristic (can be adjusted)
    };

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push({fScore[start], start});

    while (!openSet.empty()) {
        int current = openSet.top().second;
        openSet.pop();

        if (current == goal) {
            vector<int> path;
            for (int at = goal; at != -1; at = cameFrom[at]) {
                path.push_back(at);
            }
            reverse(path.begin(), path.end());
            return path; // Reconstruct the path
        }

        for (auto& neighbor : hospital.graph[current]) {
            int neighborIndex = neighbor.first;
            int weight = neighbor.second;
            int tentative_gScore = gScore[current] + weight;

            if (tentative_gScore < gScore[neighborIndex]) {
                cameFrom[neighborIndex] = current;
                gScore[neighborIndex] = tentative_gScore;
                fScore[neighborIndex] = gScore[neighborIndex] + heuristic(neighborIndex, goal);
                openSet.push({fScore[neighborIndex], neighborIndex});
            }
        }
    }

    return {}; // No path found
}

// Fenwick Tree (Binary Indexed Tree) for priority management
class FenwickTree {
private:
    vector<int> tree;
    int n;
public:
    FenwickTree(int size) {
        n = size;
        tree.resize(n + 1, 0);
    }

    void update(int index, int delta) {
        while (index <= n) {
            tree[index] += delta;
            index += index & -index;
        }
    }

    int query(int index) {
        int sum = 0;
        while (index > 0) {
            sum += tree[index];
            index -= index & -index;
        }
        return sum;
    }
};

// Task Manager for medical dispatch tasks with priority
class TaskManager {
private:
    FenwickTree fenwickTree;
    priority_queue<pair<int, int>> taskQueue;
public:
    TaskManager(int numTasks) : fenwickTree(numTasks) {}

    void addTask(int taskId, int priority) {
        taskQueue.push({priority, taskId});
        fenwickTree.update(taskId, priority);
    }

    int dispatchNextTask() {
        if (!taskQueue.empty()) {
            int taskId = taskQueue.top().second;
            taskQueue.pop();
            fenwickTree.update(taskId, -fenwickTree.query(taskId)); // Correctly remove task
            return taskId;
        }
        return -1;
    }
};

// Main function
int main() {
    HospitalGraph hospital(5);
    hospital.addEdge(0, 1, 1);
    hospital.addEdge(1, 2, 1);
    hospital.addEdge(0, 3, 2);
    hospital.addEdge(3, 4, 1);
    hospital.addEdge(2, 4, 3);

    TaskManager taskManager(10);
    taskManager.addTask(1, 5);
    taskManager.addTask(2, 1);
    taskManager.addTask(3, 3);

    int nextTask = taskManager.dispatchNextTask();
    cout << "Dispatching Task ID: " << nextTask << "\n";

    int start = 0;
    int goal = 4;
    vector<int> path = aStar(hospital, start, goal);

    cout << "Optimal path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i != path.size() - 1) {
            cout << " -> "; // Add arrow between nodes
        }
    }
    cout << "\n";

    return 0;
}
