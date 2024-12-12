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
#include <algorithm> 
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
    int traversalCounter = 0;            // Counter for traversals

public:
    /**********************************************************************/
    // getAdjMatrixSize- Get the size of the adjacency matrix
    // arguments- none
    // returns- size_t (number of nodes in the adjacency matrix)
    /**********************************************************************/
    size_t getAdjMatrixSize() const {
        return adjMatrix.size();
    }

    /**********************************************************************/
    // resetTraversalCounter- Reset the traversal counter to 0
    // arguments- none
    // returns- void
    /**********************************************************************/
    void resetTraversalCounter() {
        traversalCounter = 0;
        cout << "Traversal counter has been reset to 0." << endl;
    }

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

    // Initialize adjacency matrix after all nodes are added
    adjMatrix.resize(index, vector<int>(index, 0));

    // Populate adjacency matrix based on edge set
    for (const auto& edge : edgeSet) {
        adjMatrix[edge.first][edge.second] = 1;
        adjMatrix[edge.second][edge.first] = 1; // Assuming undirected graph
    }

    file.close();
    cout << "Adjacency matrix size: " << adjMatrix.size() << "x" << adjMatrix[0].size() << endl;
    }   


    /**********************************************************************/
    // bfsTraversal- Perform Breadth-First Search (BFS) on the graph
    // arguments- starting node index
    // returns- int (number of traversed nodes)
    /**********************************************************************/
    int bfsTraversal(int startNode) {
    vector<bool> visited(adjMatrix.size(), false);
    queue<int> q;
    traversalCounter = 0;

    visited[startNode] = true;
    q.push(startNode);

    cout << "BFS Traversal: ";

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        cout << nodeNames[node] << " ";
        traversalCounter++;

        for (size_t i = 0; i < adjMatrix[node].size(); ++i) {
            if (adjMatrix[node][i] == 1 && !visited[i]) {
                visited[i] = true;
                q.push(i);
            }
        }
    }

    cout << endl;
    return traversalCounter;
}


    /**********************************************************************/
    // dfsTraversal- Perform Depth-First Search (DFS) on the graph
    // arguments- starting node index
    // returns- int (number of traversed nodes)
    /**********************************************************************/
    int dfsTraversal(int startNode) {
    vector<bool> visited(adjMatrix.size(), false);
    stack<int> s;
    traversalCounter = 0;

    s.push(startNode);

    cout << "DFS Traversal: ";

    while (!s.empty()) {
        int node = s.top();
        s.pop();

        if (!visited[node]) {
            visited[node] = true;
            cout << nodeNames[node] << " ";
            traversalCounter++;
        }

        for (size_t i = 0; i < adjMatrix[node].size(); ++i) {
            if (adjMatrix[node][i] == 1 && !visited[i]) {
                s.push(i);
            }
        }
    }

    cout << endl;
    return traversalCounter;
    }


    /**********************************************************************/
    // writeIterationToCSV- Write counter from traversal to CSV
    // arguments- reference to filename, counter value, traversal name
    // returns- void
    /**********************************************************************/
    void writeIterationToCSV(const string& filename, int counter, string name) {
        ofstream outFile;
        outFile.open(filename, std::ios_base::app);
        if (!outFile.is_open()) {
            cerr << "Error: Could not write to file." << endl;
            return;
        }

        cout << "Writing iterations to CSV" << endl;

        outFile << name << "," << counter << endl;
        outFile.close();
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
        cout << "Adjacency Matrix Size: " << graph.getAdjMatrixSize() << endl;
    } else {
        cerr << "Invalid choice." << endl;
        return 1;
    }

    cout << "Graph loaded successfully.\n";
    graph.printGraph();

    while (true) {
        cout << "Choose traversal method (1: BFS, 2: DFS, 3: Reset Counter, 4: Exit): ";
        cin >> choice;

        if (choice == 1 || choice == 2) {
            int startNode;
            cout << "Enter the starting node index (0-based): ";
            cin >> startNode;

            if (startNode < 0 || startNode >= graph.getAdjMatrixSize()) {
                cerr << "Invalid start node. Please ensure it is within the range 0 to " << graph.getAdjMatrixSize() - 1 << "." << endl;
                continue;
            }

            int counter = 0;
            string traversal_name;

            if (choice == 1) {
                counter = graph.bfsTraversal(startNode);
                traversal_name = "BFS";
            } else {
                counter = graph.dfsTraversal(startNode);
                traversal_name = "DFS";
            }

            cout << "Counter: " << counter << endl;
            graph.writeIterationToCSV("output_viz.csv", counter, traversal_name);
        } else if (choice == 3) {
            graph.resetTraversalCounter();
        } else if (choice == 4) {
            break;
        } else {
            cerr << "Invalid choice." << endl;
        }
    }

    return 0;
}
