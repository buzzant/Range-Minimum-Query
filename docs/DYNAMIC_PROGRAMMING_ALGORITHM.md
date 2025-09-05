# Dynamic Programming Algorithm

## Overview
The Dynamic Programming approach precomputes and stores the minimum value for ALL possible ranges in a 2D table. This provides O(1) query time at the cost of O(n²) preprocessing time and space.

## Algorithm Description

### Core Concept
Build a 2D table `dp[i][j]` where each entry stores the minimum value in the range `[i, j]`. Once built, any RMQ can be answered by a simple table lookup.

### Complexity Analysis
- **Preprocessing Time**: O(n²) - Fill all n×n table entries
- **Preprocessing Space**: O(n²) - Store the entire DP table
- **Query Time**: O(1) - Direct table lookup
- **Query Space**: O(1) - No additional space for queries
- **Total Space**: O(n²) - DP table dominates space usage
- **Update Time**: O(n²) - Requires rebuilding the entire table

## How It Works

### Example Array
Consider the array: `[3, 1, 4, 1, 5]`

### Building the DP Table

```
Array: [3, 1, 4, 1, 5]
Index:  0  1  2  3  4
```

### ASCII Art Visualization

```
Dynamic Programming Table Construction
======================================

Original Array:
    [3]  [1]  [4]  [1]  [5]
     0    1    2    3    4

DP Table dp[i][j] = minimum in range [i,j]:
============================================

Step 1: Initialize diagonal (ranges of length 1)
─────────────────────────────────────────────────
    j→  0    1    2    3    4
i↓  ┌────┬────┬────┬────┬────┐
0   │ 3  │ ?  │ ?  │ ?  │ ?  │  dp[0][0] = arr[0] = 3
    ├────┼────┼────┼────┼────┤
1   │ X  │ 1  │ ?  │ ?  │ ?  │  dp[1][1] = arr[1] = 1
    ├────┼────┼────┼────┼────┤
2   │ X  │ X  │ 4  │ ?  │ ?  │  dp[2][2] = arr[2] = 4
    ├────┼────┼────┼────┼────┤
3   │ X  │ X  │ X  │ 1  │ ?  │  dp[3][3] = arr[3] = 1
    ├────┼────┼────┼────┼────┤
4   │ X  │ X  │ X  │ X  │ 5  │  dp[4][4] = arr[4] = 5
    └────┴────┴────┴────┴────┘

Step 2: Fill ranges of length 2
─────────────────────────────────
dp[0][1] = min(dp[0][0], arr[1]) = min(3, 1) = 1
dp[1][2] = min(dp[1][1], arr[2]) = min(1, 4) = 1
dp[2][3] = min(dp[2][2], arr[3]) = min(4, 1) = 1
dp[3][4] = min(dp[3][3], arr[4]) = min(1, 5) = 1

    j→  0    1    2    3    4
i↓  ┌────┬────┬────┬────┬────┐
0   │ 3  │ 1  │ ?  │ ?  │ ?  │
    ├────┼────┼────┼────┼────┤
1   │ X  │ 1  │ 1  │ ?  │ ?  │
    ├────┼────┼────┼────┼────┤
2   │ X  │ X  │ 4  │ 1  │ ?  │
    ├────┼────┼────┼────┼────┤
3   │ X  │ X  │ X  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
4   │ X  │ X  │ X  │ X  │ 5  │
    └────┴────┴────┴────┴────┘

Step 3: Fill ranges of length 3
─────────────────────────────────
dp[0][2] = min(dp[0][1], arr[2]) = min(1, 4) = 1
dp[1][3] = min(dp[1][2], arr[3]) = min(1, 1) = 1
dp[2][4] = min(dp[2][3], arr[4]) = min(1, 5) = 1

    j→  0    1    2    3    4
i↓  ┌────┬────┬────┬────┬────┐
0   │ 3  │ 1  │ 1  │ ?  │ ?  │
    ├────┼────┼────┼────┼────┤
1   │ X  │ 1  │ 1  │ 1  │ ?  │
    ├────┼────┼────┼────┼────┤
2   │ X  │ X  │ 4  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
3   │ X  │ X  │ X  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
4   │ X  │ X  │ X  │ X  │ 5  │
    └────┴────┴────┴────┴────┘

Step 4: Fill ranges of length 4
─────────────────────────────────
dp[0][3] = min(dp[0][2], arr[3]) = min(1, 1) = 1
dp[1][4] = min(dp[1][3], arr[4]) = min(1, 5) = 1

Step 5: Fill ranges of length 5
─────────────────────────────────
dp[0][4] = min(dp[0][3], arr[4]) = min(1, 5) = 1

Final DP Table:
===============
    j→  0    1    2    3    4
i↓  ┌────┬────┬────┬────┬────┐
0   │ 3  │ 1  │ 1  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
1   │ X  │ 1  │ 1  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
2   │ X  │ X  │ 4  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
3   │ X  │ X  │ X  │ 1  │ 1  │
    ├────┼────┼────┼────┼────┤
4   │ X  │ X  │ X  │ X  │ 5  │
    └────┴────┴────┴────┴────┘

X = Invalid (i > j)
```

### Query Examples

```
Query: RMQ(1, 3)
Simply look up dp[1][3] = 1
Time: O(1)

Query: RMQ(0, 4)
Simply look up dp[0][4] = 1
Time: O(1)

Query: RMQ(2, 3)
Simply look up dp[2][3] = 1
Time: O(1)
```

## Implementation Details

### Pseudocode
```
function preprocess_DP(array):
    n = array.length
    create dp[n][n] table
    
    // Base case: ranges of length 1
    for i from 0 to n-1:
        dp[i][i] = array[i]
    
    // Fill for increasing lengths
    for length from 2 to n:
        for i from 0 to n-length:
            j = i + length - 1
            dp[i][j] = min(dp[i][j-1], array[j])
    
function query_DP(left, right):
    return dp[left][right]  // O(1) lookup
```

### C++ Implementation Highlights
```cpp
void RMQDynamicProgramming::performPreprocess() {
    Size n = data_.size();
    
    // Allocate DP table
    dp_table_.resize(n, std::vector<Value>(n));
    
    // Base case: single elements
    for (Index i = 0; i < n; ++i) {
        dp_table_[i][i] = data_[i];
    }
    
    // Fill for increasing lengths
    for (Size length = 2; length <= n; ++length) {
        for (Index i = 0; i <= n - length; ++i) {
            Index j = i + length - 1;
            dp_table_[i][j] = std::min(dp_table_[i][j - 1], data_[j]);
        }
    }
}

Value RMQDynamicProgramming::performQuery(Index left, Index right) const {
    return dp_table_[left][right];  // O(1) lookup
}
```

## Visual Representation of DP Recurrence

```
Computing dp[i][j]:
===================

Method 1: Extend from left
──────────────────────────
dp[i][j] = min(dp[i][j-1], arr[j])

    [i]────────[j-1] [j]
    └──dp[i][j-1]──┘
                     ↓
         Compare with arr[j]
                ↓
          dp[i][j]

Method 2: Could also extend from right
───────────────────────────────────────
dp[i][j] = min(arr[i], dp[i+1][j])

    [i] [i+1]────────[j]
         └──dp[i+1][j]─┘
     ↓
   arr[i] compared with dp[i+1][j]
                ↓
          dp[i][j]
```

## Memory Layout Visualization

```
Memory Usage for n=5:
=====================

Original Array (n elements):
[3][1][4][1][5] = 5 × 4 bytes = 20 bytes

DP Table (n×n elements):
┌─┬─┬─┬─┬─┐
│3│1│1│1│1│
├─┼─┼─┼─┼─┤
│ │1│1│1│1│
├─┼─┼─┼─┼─┤
│ │ │4│1│1│  = 5×5 × 4 bytes = 100 bytes
├─┼─┼─┼─┼─┤
│ │ │ │1│1│
├─┼─┼─┼─┼─┤
│ │ │ │ │5│
└─┴─┴─┴─┴─┘

Total: ~120 bytes (simplified)

For n=1000:
DP Table = 1,000,000 × 4 bytes = 4 MB
```

## Advantages
1. **Constant Query Time**: O(1) - fastest possible query time
2. **Simple Queries**: Just a table lookup, no computation
3. **Predictable Performance**: Every query takes exactly the same time
4. **Cache Friendly**: Sequential access during preprocessing
5. **Easy to Understand**: Straightforward DP approach

## Disadvantages
1. **Quadratic Space**: O(n²) space becomes prohibitive for large arrays
2. **Quadratic Preprocessing**: O(n²) time to build the table
3. **No Dynamic Updates**: Any update requires full table rebuild
4. **Memory Limitations**: Unsuitable for n > 10,000 (100M entries)
5. **Wasted Space**: Half the table is unused (i > j entries)

## When to Use
- Small arrays (n ≤ 1,000)
- Many queries (> n²) to amortize preprocessing cost
- When O(1) query time is critical
- Static data (no updates)
- When memory is not a constraint

## Example Use Cases
1. **Game Development**: Precomputed damage tables for small grids
2. **Small Matrices**: Statistics on fixed-size matrices
3. **Lookup Tables**: When all range queries are equally likely
4. **Embedded Systems**: With small, fixed-size arrays

## Space-Time Trade-off Analysis

| Array Size | DP Table Size | Memory Usage | Viable? |
|------------|---------------|--------------|---------|
| 100        | 10,000        | 40 KB        | ✓ Yes   |
| 1,000      | 1,000,000     | 4 MB         | ✓ Yes   |
| 10,000     | 100,000,000   | 400 MB       | ⚠ Maybe |
| 100,000    | 10^10         | 40 GB        | ✗ No    |

## Conclusion
The Dynamic Programming approach represents the ultimate space-time trade-off for RMQ: maximum space usage for minimum query time. It's perfect for small arrays with many queries, but becomes impractical for large datasets due to its O(n²) space requirement. This algorithm demonstrates the principle of precomputing all possible answers, a technique useful in many optimization problems.