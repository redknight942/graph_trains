#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <algorithm> // Added for find_if
using namespace std;

// Custom hash function for std::pair
struct pair_hash {
    /**********************************************************************/
    // operator()- Custom hash function for std::pair
    // arguments- reference to a pair of values (T1, T2)
    // returns- size_t (hash value)
    /**********************************************************************/
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1); // Combine hashes
    }
};

class Graph {
private:
    unordered_map<int, string> nodeNames; // Map node indices to names
    vector<vector<int>> adjMatrix;       // Adjacency matrix
    unordered_set<pair<int, int>, pair_hash> edgeSet; // Edge set

public:
    /**********************************************************************/
    // parseCSV- Parse a CSV file into a vector of rows
    // arguments- reference to filename
    // returns- vector<vector<string>> (parsed rows of CSV)
    /**********************************************************************/
    vector<vector<string>> parseCSV(const string& filename) {
        vector<vector<string>> data;
        ifstream file(filename);
        string line, cell;

        if (!file.is_open()) {
            cerr << "Error: Could not open file." << endl;
            return data;
        }

        while (getline(file, line)) {
            vector<string> row;
            stringstream lineStream(line);
            while (getline(lineStream, cell, ',')) {
                row.push_back(cell);
            }
            data.push_back(row);
        }

        file.close();
        return data;
    }

    /**********************************************************************/
    // loadFromAdjMatrixCSV- Load graph from adjacency matrix CSV
    // arguments- reference to filename
    // returns- void
    /**********************************************************************/
    void loadFromAdjMatrixCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open adjacency matrix file." << endl;
            return;
        }

        string line;
        getline(file, line); // Read header row
        stringstream headerStream(line);
        string nodeName;

        // Parse header row to populate nodeNames
        int index = 0;
        getline(headerStream, nodeName, ','); // Skip the first empty cell
        while (getline(headerStream, nodeName, ',')) {
            nodeNames[index++] = nodeName;
        }

        // Initialize adjacency matrix
        adjMatrix.resize(index, vector<int>(index, 0));

        // Read and populate the adjacency matrix
        int row = 0;
        while (getline(file, line)) {
            stringstream rowStream(line);
            string cell;

            // Read row header
            getline(rowStream, cell, ',');

            // Read adjacency values
            int col = 0;
            while (getline(rowStream, cell, ',')) {
                adjMatrix[row][col++] = stoi(cell);
            }
            row++;
        }

        file.close();
    }

    /**********************************************************************/
    // loadFromEdgeSetCSV- Load graph from edge set CSV
    // arguments- reference to filename
    // returns- void
    /**********************************************************************/
    void loadFromEdgeSetCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open edge set file." << endl;
            return;
        }

        string line;
        getline(file, line); // Skip header row

        int index = 0;
        while (getline(file, line)) {
            stringstream lineStream(line);
            string origin, destination;

            // Read origin and destination
            getline(lineStream, origin, ',');
            getline(lineStream, destination, ',');

            // Map node names to indices if not already mapped
            auto originIt = find_if(nodeNames.begin(), nodeNames.end(),
                                    [&](const auto& pair) { return pair.second == origin; });
            if (originIt == nodeNames.end()) {
                nodeNames[index++] = origin;
            }

            auto destIt = find_if(nodeNames.begin(), nodeNames.end(),
                                  [&](const auto& pair) { return pair.second == destination; });
            if (destIt == nodeNames.end()) {
                nodeNames[index++] = destination;
            }

            // Add edge to edge set
            int originIdx = find_if(nodeNames.begin(), nodeNames.end(), [&](const auto& pair) { return pair.second == origin; })->first;
            int destIdx = find_if(nodeNames.begin(), nodeNames.end(), [&](const auto& pair) { return pair.second == destination; })->first;
            edgeSet.emplace(originIdx, destIdx);
        }

        file.close();
    }

    /**********************************************************************/
    // bfsTraversal- Perform Breadth-First Search (BFS) on the graph
    // arguments- starting node index
    // returns- void
    /**********************************************************************/
    void bfsTraversal(int startNode) {
        vector<bool> visited(adjMatrix.size(), false);
        queue<int> q;

        visited[startNode] = true;
        q.push(startNode);

        cout << "BFS Traversal: ";

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            cout << nodeNames[node] << " ";

            for (size_t i = 0; i < adjMatrix[node].size(); ++i) {
                if (adjMatrix[node][i] == 1 && !visited[i]) {
                    visited[i] = true;
                    q.push(i);
                }
            }
        }

        cout << endl;
    }

    /**********************************************************************/
    // dfsTraversal- Perform Depth-First Search (DFS) on the graph
    // arguments- starting node index
    // returns- void
    /**********************************************************************/
    void dfsTraversal(int startNode) {
        vector<bool> visited(adjMatrix.size(), false);
        stack<int> s;

        s.push(startNode);

        cout << "DFS Traversal: ";

        while (!s.empty()) {
            int node = s.top();
            s.pop();

            if (!visited[node]) {
                visited[node] = true;
                cout << nodeNames[node] << " ";
            }

            for (size_t i = 0; i < adjMatrix[node].size(); ++i) {
                if (adjMatrix[node][i] == 1 && !visited[i]) {
                    s.push(i);
                }
            }
        }

        cout << endl;
    }

    /**********************************************************************/
    // printGraph- Print adjacency matrix and edge set
    // arguments- none
    // returns- void
    /**********************************************************************/
    void printGraph() {
        cout << "Adjacency Matrix:" << endl;
        for (size_t i = 0; i < adjMatrix.size(); ++i) {
            for (size_t j = 0; j < adjMatrix[i].size(); ++j) {
                cout << adjMatrix[i][j] << " ";
            }
            cout << endl;
        }

        cout << "\nEdge Set:" << endl;
        for (const auto& edge : edgeSet) {
            cout << "(" << nodeNames[edge.first] << ", " << nodeNames[edge.second] << ")" << endl;
        }
    }

    /**********************************************************************/
    // writeAdjMatrixToCSV- Write adjacency matrix to CSV
    // arguments- reference to filename
    // returns- void
    /**********************************************************************/
    void writeAdjMatrixToCSV(const string& filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Could not write to file." << endl;
            return;
        }

        // Write header row
        outFile << ",";
        for (const auto& [index, name] : nodeNames) {
            outFile << name << ",";
        }
        outFile << "\n";

        // Write adjacency matrix rows
        for (size_t i = 0; i < adjMatrix.size(); ++i) {
            outFile << nodeNames[i] << ",";
            for (int val : adjMatrix[i]) {
                outFile << val << ",";
            }
            outFile << "\n";
        }

        outFile.close();
    }

    /**********************************************************************/
    // writeEdgeSetToCSV- Write edge set to CSV
    // arguments- reference to filename
    // returns- void
    /**********************************************************************/
    void writeEdgeSetToCSV(const string& filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Could not write to file." << endl;
            return;
        }

        outFile << "Origin,Destination\n";
        for (const auto& edge : edgeSet) {
            outFile << nodeNames[edge.first] << "," << nodeNames[edge.second] << "\n";
        }

        outFile.close();
    }
};

int main() {
    Graph graph;

    cout << "Load graph from adjacency matrix or edge set? (1: Adjacency Matrix, 2: Edge Set): ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        graph.loadFromAdjMatrixCSV("adjacency_matrix.csv");
        graph.writeAdjMatrixToCSV("output_adjacency_matrix.csv");
    } else if (choice == 2) {
        graph.loadFromEdgeSetCSV("edge_set.csv");
        graph.writeEdgeSetToCSV("output_edge_set.csv");
    } else {
        cerr << "Invalid choice." << endl;
        return 1;
    }

    cout << "Choose traversal method (1: BFS, 2: DFS): ";
    cin >> choice;

    int startNode = 0;
    cout << "Enter the starting node index (0-based): ";
    cin >> startNode;

    if (choice == 1) {
        graph.bfsTraversal(startNode);
    } else if (choice == 2) {
        graph.dfsTraversal(startNode);
    } else {
        cerr << "Invalid choice." << endl;
    }

    graph.printGraph();

    return 0;
}