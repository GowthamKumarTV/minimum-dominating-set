# Minimum Dominating Set

A simple C++11 program that solves the same graph using brute force and
Branch and Bound. Graphs can contain 0 to 20 nodes. No external libraries.

## Compile and run

From PowerShell in `C:\DAA package`:

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -pedantic dominating_set.cpp -o dominating_set.exe
.\dominating_set.exe .\path6.txt
```

There are no extra command modes or search statistics. The program displays
both answers, checks agreement, and explains the coverage of the B&B set.

## Input

First line: `n m`, the number of nodes and edges. The next `m` lines each
contain an undirected edge `u v`. Nodes are numbered from 0 to `n-1`.
The graph is stored in a 2D array. Both `graph[u][v]` and `graph[v][u]` are set
to 1. Missing files, invalid sizes, missing edges, and invalid labels cause an
error. Repeated edges and self-loops do not change domination.

## What the answer means

A selected node covers itself and its direct neighbours. A dominating set
covers every node. We find a dominating set with the smallest size.

| Graph | Minimum size | One optimal set | Hand check |
|---|---:|---|---|
| Six-node path (`path6.txt`) | 2 | {1, 4} | 1 covers 0,1,2; 4 covers 3,4,5 |
| Six nodes with only edge (0,1) | 5 | {0, 2, 3, 4, 5} | Four isolated nodes must be selected, plus one edge endpoint |

Different optimal sets are allowed, so the program compares sizes rather
than requiring identical sets.

## Code, in execution order

1. `main` starts the graph array at zero and reads the file with `readGraph`.
2. `bruteForce` tries every possible subset and keeps the smallest valid one.
3. `branchAndBound` independently starts with all nodes selected as its best
   known answer, then calls `search` to improve it.
4. `main` prints the two results and checks that both sets cover the graph,
   have their reported sizes, and agree on the minimum size.
5. `explainCoverage` prints which nodes each selected B&B node covers.

`Result` contains only `size` and `mask`. A mask is an integer with one binary
digit per node: 1 means selected. For example, binary `010010` selects 1 and 4.

- `1 << node`: create the bit for this node.
- `mask & (1 << node)`: check its bit.
- `mask | (1 << node)`: select it.
- `1 << n`: the number of subsets, 2 to the power n.
- `isSelected`: perform the bit check.
- `countSelectedNodes`: count the selected bits.
- `isDominatingSet`: check each node is selected or has a selected neighbour.
- `Result& best`: recursive calls update the same best answer.
- `int& n`: let the input function fill the caller's node count.
- `printSet` and `printResult`: turn the answer into readable output.

## Branch and Bound, simply

Each call to `search` decides whether to select or skip `nextNode`.
It tracks the selected nodes and their count. It stops if:

- The count already reaches the best known size.
- Everything is covered: save this smaller answer.
- There are no undecided nodes left.
- The selected count plus the fewest extra nodes we could need cannot improve
  the best answer.

The one remaining bound counts uncovered nodes and finds the largest number
that any one undecided candidate could cover. Divide, rounding up.
For example, seven uncovered nodes with at most three covered per choice need
at least three more choices. In code, rounding up is `(uncovered + maxCover - 1)
/ maxCover`. If nothing remains uncovered, return zero. If no candidate covers
anything new, return `n + 1` to signal an impossible completion.

Coverage can overlap, so this bound may be smaller than the actual requirement.
It never exceeds it. A branch stopped by this test cannot contain a better
answer. Equality can be discarded because we only need one optimal set.
The mask is copied into each recursive call; the best answer is shared.

B&B never uses the brute-force answer as a target. The algorithms solve the
problem independently, then their minimum sizes are compared.

## What to say to the professor

"My program finds the fewest nodes that cover a graph. It solves the graph
using brute force and Branch and Bound, checks that the answers agree, and
shows which nodes are covered by each selected node."

If asked about B&B: "I either select or skip each node. I stop a branch when
it cannot improve the best valid answer I already have."

If asked about the bound: "If seven nodes remain uncovered and one choice
covers at most three, I need at least three more choices."

If asked why the answer is exact: "Every subset is represented by select/skip
choices. I only stop branches that cannot give a better answer."

## Complexity and limits

Both methods have `O(2^n * n^2)` worst-case time with these matrix-based checks.
B&B can avoid branches, but worst-case time is still exponential. Space is
`O(n^2)` for the matrix plus `O(n)` recursion depth and helper storage.
The limit is 20 nodes. Do not increase it without changing the mask type and
considering runtime. The normal program always runs brute force too, so its
total runtime includes that cost. An empty graph needs zero selected nodes.
