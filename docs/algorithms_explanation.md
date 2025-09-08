# Range Minimum Query Algorithms Explanation

## Table of Contents
1. [Problem Definition](#problem-definition)
2. [Naive Approach](#1-naive-approach)
3. [Dynamic Programming](#2-dynamic-programming-approach)
4. [Sparse Table](#3-sparse-table-binary-lifting)
5. [Block Decomposition](#4-block-decomposition-square-root-decomposition)
6. [LCA-based RMQ](#5-lca-based-rmq)
7. [Comparison Table](#comparison-table)

## Problem Definition

Given an array `A[0...n-1]` of comparable elements, a Range Minimum Query (RMQ) asks for the position of the minimum element in a subarray `A[L...R]`.

```
RMQ(A, L, R) = argmin{A[k] : L ≤ k ≤ R}
```

## 1. Naive Approach

### Algorithm
Simply iterate through the range [L, R] and find the minimum element.

```cpp
int query(L, R) {
    min_val = A[L]
    for i from L+1 to R:
        min_val = min(min_val, A[i])
    return min_val
}
```

### Complexity
- **Preprocessing**: O(1) - No preprocessing needed
- **Query**: O(n) - Linear scan through the range
- **Space**: O(1) - No extra space
- **Update**: O(1) - Direct array update

### Use Case
- When queries are rare
- When array is frequently updated
- Small arrays

## 2. Dynamic Programming Approach

### Algorithm
Precompute answers for all possible ranges using dynamic programming.

```
dp[i][j] = minimum in range [i, j]
dp[i][i] = A[i]
dp[i][j] = min(dp[i][j-1], A[j]) for j > i
```

### Implementation Strategy
```cpp
// Build DP table
for length from 1 to n:
    for start from 0 to n-length:
        end = start + length - 1
        if length == 1:
            dp[start][end] = A[start]
        else:
            dp[start][end] = min(dp[start][end-1], A[end])
```

### Complexity
- **Preprocessing**: O(n²) - Fill entire DP table
- **Query**: O(1) - Direct lookup
- **Space**: O(n²) - Store all range results
- **Update**: O(n²) - Rebuild affected ranges

### Use Case
- Small arrays (n ≤ 1000)
- Many queries on static data
- When O(n²) space is acceptable

## 3. Sparse Table (Binary Lifting)

### Algorithm
Store minimum for ranges of length 2^k starting at each position.

```
st[i][j] = minimum in range [i, i + 2^j - 1]
```

For query [L, R]:
1. Find largest k where 2^k ≤ R - L + 1
2. Return min(st[L][k], st[R - 2^k + 1][k])

### Key Insight
Any range can be covered by two overlapping power-of-2 ranges.

### Implementation
```cpp
// Preprocessing
for i from 0 to n-1:
    st[i][0] = A[i]

for j from 1 to log(n):
    for i from 0 to n - 2^j + 1:
        st[i][j] = min(st[i][j-1], st[i + 2^(j-1)][j-1])

// Query
int k = log2(R - L + 1)
return min(st[L][k], st[R - (1<<k) + 1][k])
```

### Complexity
- **Preprocessing**: O(n log n) - Fill sparse table
- **Query**: O(1) - Two lookups and min
- **Space**: O(n log n) - Sparse table storage
- **Update**: O(n log n) - Rebuild table

### Use Case
- Static arrays with many queries
- When O(1) query is critical
- Competitive programming

## 4. Block Decomposition (Square Root Decomposition)

### Algorithm
1. Divide array into blocks of size √n
2. Precompute minimum for each complete block
3. For queries, combine:
   - Partial left block
   - Complete middle blocks
   - Partial right block

### Implementation Strategy
```cpp
block_size = sqrt(n)
num_blocks = (n + block_size - 1) / block_size

// Preprocessing
for i from 0 to num_blocks-1:
    block_min[i] = min of elements in block i

// Query [L, R]
left_block = L / block_size
right_block = R / block_size

if left_block == right_block:
    return linear_scan(L, R)
else:
    result = min(
        partial_left_block,
        complete_middle_blocks,
        partial_right_block
    )
```

### Complexity
- **Preprocessing**: O(n) - Scan array once
- **Query**: O(√n) - At most 2√n elements
- **Space**: O(√n) - Store block minimums
- **Update**: O(1) - Update element and block

### Use Case
- Balance between query and update
- Moderate number of queries
- Dynamic arrays

## 5. LCA-based RMQ

### Algorithm
Convert RMQ to Lowest Common Ancestor (LCA) problem on Cartesian tree.

#### Steps:
1. **Build Cartesian Tree**: A binary tree where:
   - In-order traversal gives original array
   - Each node's value ≤ its descendants (min-heap property)

2. **Convert to LCA**: RMQ(L, R) = LCA of nodes at positions L and R

3. **Solve LCA**: Use any LCA algorithm (binary lifting, Euler tour + RMQ)

### Cartesian Tree Construction
```cpp
stack<int> st
for i from 0 to n-1:
    last_popped = null
    while !st.empty() and A[st.top()] > A[i]:
        last_popped = st.pop()
    
    if !st.empty():
        right[st.top()] = i
    if last_popped != null:
        left[i] = last_popped
    
    st.push(i)
```

### Why It Works
- The minimum element in range [L, R] is the LCA of L and R in the Cartesian tree
- This shows RMQ ≡ LCA (equivalent problems)

### Complexity
- **Preprocessing**: O(n) - Build tree + LCA preprocessing
- **Query**: O(log n) or O(1) - Depends on LCA method
- **Space**: O(n) - Tree structure
- **Update**: O(n) - Rebuild tree

### Use Case
- Theoretical importance
- When you have efficient LCA implementation
- Demonstrating problem equivalence

## Comparison Table

| Algorithm | Preprocess | Query | Space | Update | Best For |
|-----------|------------|-------|--------|---------|-----------|
| Naive | O(1) | O(n) | O(1) | O(1) | Rare queries, frequent updates |
| DP | O(n²) | O(1) | O(n²) | O(n²) | Small arrays, many queries |
| Sparse Table | O(n log n) | O(1) | O(n log n) | O(n log n) | Static data, optimal query |
| Block Decomposition | O(n) | O(√n) | O(√n) | O(1) | Balanced operations |
| LCA-based | O(n) | O(log n)* | O(n) | O(n) | Theoretical interest |

*Can be O(1) with Euler tour + Sparse Table

## Choosing the Right Algorithm

### Decision Tree
```
Is the array static?
├─ Yes: Will there be many queries?
│   ├─ Yes: Use Sparse Table (O(1) query)
│   └─ No: Use Naive (simple)
└─ No: Need fast updates?
    ├─ Yes: Use Block Decomposition
    └─ No: How large is n?
        ├─ Small (n < 1000): Use DP
        └─ Large: Use Block Decomposition
```

## Implementation Tips

1. **Index Management**: Be careful with 0-based vs 1-based indexing
2. **Overflow**: For large arrays, use appropriate data types
3. **Edge Cases**: Handle L = R, empty ranges
4. **Optimization**: Use bit operations for power-of-2 calculations
5. **Testing**: Verify against naive approach for correctness