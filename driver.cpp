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
    // addNode- Add a new node to the graph
    // arguments- reference to the node name
    // returns- void
    /**********************************************************************/
    void addNode(const string& nodeName) {
        int newNodeIndex = adjMatrix.size();
        nodeNames[newNodeIndex] = nodeName;

        // Expand adjacency matrix
        for (auto& row : adjMatrix) {
            row.push_back(0);
        }
        adjMatrix.emplace_back(newNodeIndex + 1, 0);

        cout << "Node '" << nodeName << "' added to the graph." << endl;
        writeActionToCSV("output_viz_add_node.csv", "ADD_NODE", {0}, newNodeIndex);
;
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
        vector<int> depth(adjMatrix.size(), -1);
        queue<int> q;
        int l = 0;

        visited[startNode] = true;
        depth[startNode] = 0;
        q.push(startNode);

        cout << "BFS Traversal: \n";

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            cout << nodeNames[node] << " (Depth: " << depth[node] << ")\n";            
            l++;
            cout << "\nNode counter: " << l << endl; 
            for (size_t i = 0; i < adjMatrix[node].size(); ++i) {
                if (adjMatrix[node][i] == 1 && !visited[i]) {
                    visited[i] = true;
                    depth[i] = depth[node] + 1;
                    q.push(i);
                    
                }
            }
        }

        writeActionToCSV("output_viz_bfs.csv", "BFS", depth, l);
        return l;
    }


    /**********************************************************************/
    // dfsTraversal- Perform Depth-First Search (DFS) on the graph
    // arguments- starting node index
    // returns- int (number of traversed nodes)
    /**********************************************************************/
    int dfsTraversal(int startNode) {
    vector<bool> visited(adjMatrix.size(), false);
    vector<int> depth(adjMatrix.size(), -1);
    stack<pair<int, int>> s;
    int n = 0;

    s.push({startNode, 0});

    cout << "DFS Traversal: \n";

    while (!s.empty()) {
        auto [node, currentDepth] = s.top();
        s.pop();

        if (!visited[node]) {
            visited[node] = true;
            depth[node] = currentDepth;
            cout << nodeNames[node] << " (Depth: " << depth[node] << ")\n";
            n++;
        }

        for (size_t i = 0; i < adjMatrix[node].size(); ++i) {
            if (adjMatrix[node][i] == 1 && !visited[i]) {
                s.push({i, depth[node] + 1});
            }
        }
    }

    writeActionToCSV("output_viz_dfs.csv", "DFS", depth, n);
    return n;
    }


    /**********************************************************************/
    // writeActionToCSV- Template for writing actions to CSV
    // arguments- reference to filename, action name, details vector
    // returns- void
    /**********************************************************************/
    void writeActionToCSV(const string& filename, const string& action, const vector<int>& details, int counter) {
        ofstream outFile;
        outFile.open(filename, std::ios_base::app);
        if (!outFile.is_open()) {
            cerr << "Error: Could not write to file." << endl;
            return;
        }

        cout << "Writing action to CSV\n";
        //outFile << "Action,Node,Depth,Counter\n";
        outFile << action << "," << nodeNames[0] << "," << details[0] << "," << counter << "\n";
        /*
        for (size_t i = 0; i < details.size(); ++i) {
            if (details[i] != -1) {
                outFile << action << "," << nodeNames[i] << "," << details[i] << "," << counter << "\n";
            }
        }*/

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
        cout << "Adjacency Matrix Size: " << graph.getAdjMatrixSize() << endl;
    } else if (choice == 2) {
        graph.loadFromEdgeSetCSV("edge_set.csv");
        graph.writeEdgeSetToCSV("output_edge_set.csv");
    } else {
        cerr << "Invalid choice." << endl;
        return 1;
    }

    cout << "Graph loaded successfully.\n";
    graph.printGraph();

    while (true) {
        cout << "Choose an option (1: BFS, 2: DFS, 3: Add Node, 4: Exit): ";
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
            if (choice == 1) {
                counter = graph.bfsTraversal(startNode);
            } else {
                counter = graph.dfsTraversal(startNode);
            }

            cout << "Traversal completed. Nodes visited: " << counter << endl;
        } else if (choice == 3) {
            string nodeName;
            cout << "Enter the name of the new node: ";
            cin >> nodeName;
            graph.addNode(nodeName);
        } else if (choice == 4) {
            break;
        } else {
            cerr << "Invalid choice." << endl;
        }
    }

    return 0;
}