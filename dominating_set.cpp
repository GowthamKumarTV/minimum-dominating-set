#include <iostream>
#include <fstream>

using namespace std;

const int MAX_N = 20;

struct Result {
    int size;
    int mask;
};

bool isSelected(int mask, int node) {
    return (mask & (1 << node)) != 0;
}

int countSelectedNodes(int mask, int n) {
    int count = 0;
    for (int node = 0; node < n; node++) {
        if (isSelected(mask, node)) {
            count++;
        }
    }
    return count;
}

bool readGraph(const char* filename, int graph[MAX_N][MAX_N], int& n) {
    ifstream input(filename);
    if (!input) {
        cerr << "Error: Cannot open input file.\n";
        return false;
    }

    int m;
    if (!(input >> n >> m) || n < 0 || n > MAX_N || m < 0) {
        cerr << "Error: Use 0 to 20 nodes and a non-negative edge count.\n";
        return false;
    }

    for (int edge = 0; edge < m; edge++) {
        int u, v;
        if (!(input >> u >> v) || u < 0 || u >= n || v < 0 || v >= n) {
            cerr << "Error: Each edge needs two node numbers from 0 to n-1.\n";
            return false;
        }
        graph[u][v] = 1;
        graph[v][u] = 1;
    }
    return true;
}

bool isDominatingSet(int mask, int graph[MAX_N][MAX_N], int n) {
    for (int node = 0; node < n; node++) {
        bool covered = isSelected(mask, node);
        for (int neighbour = 0; neighbour < n && !covered; neighbour++) {
            if (graph[node][neighbour] == 1 && isSelected(mask, neighbour)) {
                covered = true;
            }
        }
        if (!covered) {
            return false;
        }
    }
    return true;
}

Result bruteForce(int graph[MAX_N][MAX_N], int n) {
    Result best;
    best.size = n + 1;
    best.mask = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        if (isDominatingSet(mask, graph, n)) {
            int size = countSelectedNodes(mask, n);
            if (size < best.size) {
                best.size = size;
                best.mask = mask;
            }
        }
    }
    return best;
}

int lowerBound(int mask, int nextNode, int graph[MAX_N][MAX_N], int n) {
    bool covered[MAX_N] = {false};
    for (int selected = 0; selected < n; selected++) {
        if (isSelected(mask, selected)) {
            covered[selected] = true;
            for (int node = 0; node < n; node++) {
                if (graph[selected][node] == 1) {
                    covered[node] = true;
                }
            }
        }
    }

    int uncovered = 0;
    for (int node = 0; node < n; node++) {
        if (!covered[node]) {
            uncovered++;
        }
    }
    if (uncovered == 0) {
        return 0;
    }

    int maxCover = 0;
    for (int candidate = nextNode; candidate < n; candidate++) {
        int newCover = 0;
        for (int node = 0; node < n; node++) {
            if (!covered[node] && (node == candidate || graph[candidate][node] == 1)) {
                newCover++;
            }
        }
        if (newCover > maxCover) {
            maxCover = newCover;
        }
    }
    if (maxCover == 0) {
        return n + 1;
    }

    return (uncovered + maxCover - 1) / maxCover;
}

void search(int nextNode, int mask, int selectedCount,
            int graph[MAX_N][MAX_N], int n, Result& best) {
    if (selectedCount >= best.size) {
        return;
    }
    if (isDominatingSet(mask, graph, n)) {
        best.size = selectedCount;
        best.mask = mask;
        return;
    }
    if (nextNode == n) {
        return;
    }

    int extraNeeded = lowerBound(mask, nextNode, graph, n);
    if (selectedCount + extraNeeded >= best.size) {
        return;
    }

    search(nextNode + 1, mask | (1 << nextNode), selectedCount + 1, graph, n, best);
    search(nextNode + 1, mask, selectedCount, graph, n, best);
}

Result branchAndBound(int graph[MAX_N][MAX_N], int n) {
    Result best;
    best.size = n;
    best.mask = (1 << n) - 1;
    search(0, 0, 0, graph, n, best);
    return best;
}

void printSet(int mask, int n) {
    cout << '{';
    bool first = true;
    for (int node = 0; node < n; node++) {
        if (isSelected(mask, node)) {
            if (!first) {
                cout << ", ";
            }
            cout << node;
            first = false;
        }
    }
    cout << '}';
}

void printResult(Result answer, int n) {
    cout << "Minimum nodes needed: " << answer.size << '\n';
    cout << "Selected set: ";
    printSet(answer.mask, n);
    cout << '\n';
}

void explainCoverage(Result answer, int graph[MAX_N][MAX_N], int n) {
    cout << "\nHOW THE BRANCH AND BOUND SET COVERS THE GRAPH\n";
    for (int selected = 0; selected < n; selected++) {
        if (isSelected(answer.mask, selected)) {
            int coveredMask = 1 << selected;
            for (int node = 0; node < n; node++) {
                if (graph[selected][node] == 1) {
                    coveredMask = coveredMask | (1 << node);
                }
            }
            cout << "Node " << selected << " covers ";
            printSet(coveredMask, n);
            cout << " (itself and its neighbours).\n";
        }
    }
    if (n == 0) {
        cout << "The graph is empty, so no nodes need to be selected.\n";
    } else {
        cout << "All " << n << " nodes are covered.\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: dominating_set <input-file>\n";
        return 1;
    }

    int graph[MAX_N][MAX_N] = {0};
    int n;
    if (!readGraph(argv[1], graph, n)) {
        return 1;
    }

    cout << "MINIMUM DOMINATING SET\n";
    cout << "Input file: " << argv[1] << "\nNodes: " << n << '\n';
    cout << "Goal: select the fewest nodes that cover the whole graph.\n";
    cout << "A selected node covers itself and its direct neighbours.\n";

    Result brute = bruteForce(graph, n);
    Result bounded = branchAndBound(graph, n);

    cout << "\n1. BRUTE FORCE\n";
    cout << "Tries all " << (1 << n) << " possible subsets.\n";
    printResult(brute, n);
    cout << "\n2. BRANCH AND BOUND\n";
    cout << "Selects or skips nodes; stops branches that cannot improve the best answer.\n";
    printResult(bounded, n);

    bool validBrute = isDominatingSet(brute.mask, graph, n)
                   && countSelectedNodes(brute.mask, n) == brute.size;
    bool validBounded = isDominatingSet(bounded.mask, graph, n)
                     && countSelectedNodes(bounded.mask, n) == bounded.size;
    if (!validBrute || !validBounded || brute.size != bounded.size) {
        cerr << "\nCHECK FAILED: the answers disagree or a selected set is invalid.\n";
        return 1;
    }

    cout << "\nCHECK PASSED: both methods found minimum size " << brute.size << ".\n";
    explainCoverage(bounded, graph, n);
    return 0;
}
