# Minimum Dominating Set using Branch and Bound

## Aim

Find the smallest set of nodes such that every node is selected or directly
connected to a selected node. Implement brute force and Branch and Bound,
compare their minimum sizes, and display how the chosen nodes cover the graph.

## Implementation

The program uses C++11 and the standard library. A 2D adjacency matrix stores
an undirected graph with up to 20 nodes. An integer bitmask represents a
selection: bit i is set when node i is selected. The input file contains `n m`
followed by `m` edges with zero-based node labels.

Brute force tries all `2^n` subsets. A subset is valid when every node is either
selected or has a selected neighbour. The smallest valid subset is saved.

Branch and Bound independently begins with all nodes selected as a valid
starting answer. It recursively selects or skips the next undecided node.
When a smaller valid selection is found, it replaces the best answer.

There is one lower bound. Let U be the number of uncovered nodes and M the
maximum number of them that one remaining candidate can cover. At least
`ceil(U / M)` additional choices are necessary. If no candidate covers any
uncovered node, the branch cannot succeed. If the selected count plus this
bound reaches the best known size, further exploration cannot improve the
answer and is stopped. The bound may underestimate the requirement because
coverage overlaps, but it never overestimates it.

The program contains no greedy initialization, stronger bound, experiment
mode, or search statistics. Its output concentrates on the graph's minimum
size, the nodes selected by each method, an agreement check, and the coverage
of each node in the B&B solution.

## Correctness

Brute force checks every subset. The full B&B select/skip tree also represents
every subset. B&B stops only when a branch already cannot improve the current
valid answer, has completed coverage, has exhausted its choices, or cannot
improve even with the lower bound. Because the bound never exceeds the extra
selections actually needed, no better solution is discarded. The saved answer
is therefore minimum. The two solvers do not share a target minimum.

## Hand-checkable results

| Input | Minimum size | One optimal set |
|---|---:|---|
| Six-node path | 2 | {1, 4} |
| Seven-node example | 2 | {1, 2} |
| Six nodes with only edge (0,1) | 5 | {0, 2, 3, 4, 5} |

For the path, node 1 covers {0,1,2} and node 4 covers {3,4,5}. One selected
node covers at most three nodes, proving that two is the minimum.
For the one-edge example, every isolated node must be selected, along with
one endpoint of the edge, so the answer is five.

## Validation

The two solvers must return the same minimum size. Each set is also checked
for domination and for consistency between its bitmask and reported size.
The simplified implementation was checked against brute force on all 1,100
labelled simple graphs with 0 through 5 nodes and 500 random graphs with 0
through 12 nodes. Tests are run separately from the submission program to
keep the source focused on the two algorithms and their output.

## Complexity and limitations

Both methods take `O(2^n * n^2)` worst-case time with the matrix checks. B&B
can avoid parts of the search but still has exponential worst-case time.
Space is `O(n^2)` for the graph plus linear recursion depth and helper storage.
The program supports up to 20 nodes and prints one optimal answer per method.
It handles empty graphs and disconnected graphs. The normal command includes
the brute-force run, so its total runtime includes exhaustive enumeration.
