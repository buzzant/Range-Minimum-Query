# Sparse Table Algorithm (Binary Lifting)

## Overview
The Sparse Table algorithm uses the concept of binary lifting to precompute minimum values for ranges of power-of-2 lengths. It achieves O(1) query time with O(n log n) preprocessing time and space, making it one of the most efficient static RMQ solutions.

## Algorithm Description

### Core Concept
1. Precompute minimum for all ranges of length 2^k (powers of 2)
2. Any range can be covered by at most two overlapping power-of-2 ranges
3. Since minimum is an idempotent operation, overlapping doesn't affect the result

### Complexity Analysis
- **Preprocessing Time**: O(n log n) - Build log n levels
- **Preprocessing Space**: O(n log n) - Store sparse table
- **Query Time**: O(1) - Two lookups and one comparison
- **Query Space**: O(1) - No additional space
- **Total Space**: O(n log n) - Sparse table storage
- **Update Time**: Not supported (requires full rebuild)

## How It Works

### Example Array
Consider the array: `[3, 1, 4, 1, 5, 9, 2, 6]`

### ASCII Art Visualization

```
Sparse Table Construction
=========================

Original Array:
    [3]  [1]  [4]  [1]  [5]  [9]  [2]  [6]
     0    1    2    3    4    5    6    7

Building Sparse Table st[i][j] = min in range [i, i+2^j-1]:
============================================================

Level 0: Ranges of length 2^0 = 1
──────────────────────────────────
st[0][0] = 3  (range [0,0])
st[1][0] = 1  (range [1,1])
st[2][0] = 4  (range [2,2])
st[3][0] = 1  (range [3,3])
st[4][0] = 5  (range [4,4])
st[5][0] = 9  (range [5,5])
st[6][0] = 2  (range [6,6])
st[7][0] = 6  (range [7,7])

Visual:
       [3]  [1]  [4]  [1]  [5]  [9]  [2]  [6]
        ↑    ↑    ↑    ↑    ↑    ↑    ↑    ↑
     Each element by itself

Level 1: Ranges of length 2^1 = 2
──────────────────────────────────
st[0][1] = min(st[0][0], st[1][0]) = min(3, 1) = 1
st[1][1] = min(st[1][0], st[2][0]) = min(1, 4) = 1
st[2][1] = min(st[2][0], st[3][0]) = min(4, 1) = 1
st[3][1] = min(st[3][0], st[4][0]) = min(1, 5) = 1
st[4][1] = min(st[4][0], st[5][0]) = min(5, 9) = 5
st[5][1] = min(st[5][0], st[6][0]) = min(9, 2) = 2
st[6][1] = min(st[6][0], st[7][0]) = min(2, 6) = 2

Visual:
       [3]  [1]  [4]  [1]  [5]  [9]  [2]  [6]
        └───┘    └───┘    └───┘    └───┘
          1        1        1        5      2        2
        [0,1]    [1,2]    [2,3]    [3,4]  [4,5]    [5,6]  [6,7]

Level 2: Ranges of length 2^2 = 4
──────────────────────────────────
st[0][2] = min(st[0][1], st[2][1]) = min(1, 1) = 1
st[1][2] = min(st[1][1], st[3][1]) = min(1, 1) = 1
st[2][2] = min(st[2][1], st[4][1]) = min(1, 5) = 1
st[3][2] = min(st[3][1], st[5][1]) = min(1, 2) = 1
st[4][2] = min(st[4][1], st[6][1]) = min(5, 2) = 2

Visual:
       [3]  [1]  [4]  [1]  [5]  [9]  [2]  [6]
        └───────────┘    └───────────┘
               1                1              1              1
            [0,3]            [1,4]          [2,5]          [3,6]  [4,7]
                                                                     2

Level 3: Ranges of length 2^3 = 8
──────────────────────────────────
st[0][3] = min(st[0][2], st[4][2]) = min(1, 2) = 1

Visual:
       [3]  [1]  [4]  [1]  [5]  [9]  [2]  [6]
        └───────────────────────────────┘
                        1
                     [0,7]

Complete Sparse Table:
======================
        j=0  j=1  j=2  j=3
    i=0  3    1    1    1
    i=1  1    1    1    -
    i=2  4    1    1    -
    i=3  1    1    1    -
    i=4  5    5    2    -
    i=5  9    2    -    -
    i=6  2    2    -    -
    i=7  6    -    -    -
```

### Query Example: RMQ(2, 6)

```
Query Process for RMQ(2, 6):
=============================

Step 1: Calculate range length
    length = 6 - 2 + 1 = 5

Step 2: Find largest power of 2 ≤ length
    k = floor(log2(5)) = 2
    2^k = 2^2 = 4

Step 3: Cover range with two overlapping ranges of length 4
    Range 1: [2, 2+4-1] = [2, 5]
    Range 2: [6-4+1, 6] = [3, 6]

Visualization:
    Index:  0   1   2   3   4   5   6   7
    Array: [3] [1] [4] [1] [5] [9] [2] [6]
                    ↓───────────────↓
    Query Range:  [2] [3] [4] [5] [6]
    
    Range 1:      [2] [3] [4] [5]      st[2][2] = 1
                   └───────────┘
    
    Range 2:          [3] [4] [5] [6]  st[3][2] = 1
                      └───────────┘
    
    Overlap:          [3] [4] [5]      (OK for min operation!)

Step 4: Return minimum
    result = min(st[2][2], st[3][2]) = min(1, 1) = 1
```

## Implementation Details

### Pseudocode
```
function preprocess_SparseTable(array):
    n = array.length
    K = floor(log2(n)) + 1
    
    // Create sparse table
    st[n][K]
    
    // Initialize for ranges of length 1
    for i from 0 to n-1:
        st[i][0] = array[i]
    
    // Build for increasing powers of 2
    for j from 1 to K-1:
        for i from 0 to n-(1<<j):
            st[i][j] = min(st[i][j-1], st[i+(1<<(j-1))][j-1])

function query_SparseTable(L, R):
    length = R - L + 1
    k = floor(log2(length))
    
    // Cover with two overlapping ranges
    return min(st[L][k], st[R-(1<<k)+1][k])
```

### C++ Implementation Highlights
```cpp
void RMQSparseTable::performPreprocess() {
    Size n = data_.size();
    max_level_ = computeLog2(n) + 1;
    
    // Initialize sparse table
    sparse_table_.resize(n);
    for (Index i = 0; i < n; ++i) {
        sparse_table_[i].resize(max_level_);
        sparse_table_[i][0] = data_[i];
    }
    
    // Build using binary lifting
    for (size_t j = 1; j < max_level_; ++j) {
        size_t range_len = size_t(1) << j;
        for (Index i = 0; i + range_len <= n; ++i) {
            size_t half_len = size_t(1) << (j - 1);
            sparse_table_[i][j] = std::min(
                sparse_table_[i][j - 1],
                sparse_table_[i + half_len][j - 1]
            );
        }
    }
}

Value RMQSparseTable::performQuery(Index left, Index right) const {
    size_t length = right - left + 1;
    int k = log_table_[length];
    size_t power = size_t(1) << k;
    
    return std::min(
        sparse_table_[left][k],
        sparse_table_[right - power + 1][k]
    );
}
```

## Power-of-2 Range Coverage Visualization

```
Different Query Lengths:
========================

Length = 1 (2^0):
    [x]           Direct lookup

Length = 2 (2^1):
    [x][x]        Direct lookup

Length = 3:
    [x][x][x]     Use two ranges of length 2
     └─┘           Range 1: [0,1]
       └─┘         Range 2: [1,2]
     
Length = 5:
    [x][x][x][x][x]   Use two ranges of length 4
     └───────┘        Range 1: [0,3]
         └───────┘    Range 2: [1,4]

Length = 7:
    [x][x][x][x][x][x][x]   Use two ranges of length 4
     └───────┘              Range 1: [0,3]
             └───────┘      Range 2: [3,6]
```

## Memory Layout

```
Sparse Table Memory Structure:
==============================

For array of size n=8:
Level 0: 8 entries  (ranges of length 1)
Level 1: 7 entries  (ranges of length 2)
Level 2: 5 entries  (ranges of length 4)
Level 3: 1 entry    (ranges of length 8)

Total: 8 + 7 + 5 + 1 = 21 entries
General formula: ~n × log(n) entries

Memory visualization:
┌───┬───┬───┬───┬───┬───┬───┬───┐ Level 0
│ 3 │ 1 │ 4 │ 1 │ 5 │ 9 │ 2 │ 6 │ (8 entries)
└───┴───┴───┴───┴───┴───┴───┴───┘
┌───┬───┬───┬───┬───┬───┬───┐     Level 1
│ 1 │ 1 │ 1 │ 1 │ 5 │ 2 │ 2 │     (7 entries)
└───┴───┴───┴───┴───┴───┴───┘
┌───┬───┬───┬───┬───┐             Level 2
│ 1 │ 1 │ 1 │ 1 │ 2 │             (5 entries)
└───┴───┴───┴───┴───┘
┌───┐                             Level 3
│ 1 │                             (1 entry)
└───┘
```

## Advantages
1. **Constant Query Time**: O(1) - Optimal for static arrays
2. **Efficient Space**: O(n log n) - Much better than O(n²) DP
3. **Simple Queries**: Just two lookups and a comparison
4. **Cache Efficient**: Good locality during preprocessing
5. **Versatile**: Works for any idempotent operation (min, max, GCD)

## Disadvantages
1. **No Updates**: Static structure, requires full rebuild
2. **Preprocessing Time**: O(n log n) can be significant
3. **Space Overhead**: Still uses more space than linear methods
4. **Complex Implementation**: More complex than naive or DP

## When to Use
- Static arrays with many queries
- When O(1) query time is required
- Arrays of moderate size (up to 10^6 elements)
- When preprocessing time is acceptable
- Problems involving idempotent operations

## Example Use Cases
1. **Competitive Programming**: Classic RMQ problems
2. **Static Data Analysis**: Historical data queries
3. **Game Development**: Precomputed terrain heights
4. **Bioinformatics**: DNA sequence analysis
5. **Financial Analysis**: Historical price minimums

## Comparison with Other Methods

| Method | Preprocess | Query | Space | Updates |
|--------|------------|-------|-------|---------|
| Naive | O(1) | O(n) | O(n) | O(1) |
| DP | O(n²) | O(1) | O(n²) | O(n²) |
| **Sparse Table** | **O(n log n)** | **O(1)** | **O(n log n)** | **N/A** |
| Segment Tree | O(n) | O(log n) | O(n) | O(log n) |

## Conclusion
The Sparse Table algorithm represents an optimal solution for static RMQ problems, achieving constant query time with reasonable space usage. Its use of binary lifting makes it elegant and efficient, demonstrating how power-of-2 decomposition can solve range query problems. While it doesn't support updates, for static data with frequent queries, it's often the best choice.