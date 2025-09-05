# Naive Linear Scan Algorithm

## Overview
The Naive algorithm is the simplest approach to solving Range Minimum Query (RMQ) problems. It performs a linear scan through the specified range for each query without any preprocessing.

## Algorithm Description

### Core Concept
For each query `RMQ(L, R)`, the algorithm iterates through all elements from index `L` to `R` and keeps track of the minimum value encountered.

### Complexity Analysis
- **Preprocessing Time**: O(1) - No preprocessing required
- **Preprocessing Space**: O(1) - No additional space needed
- **Query Time**: O(n) - Linear scan through the range
- **Query Space**: O(1) - Only stores the minimum value
- **Total Space**: O(n) - Just stores the original array
- **Update Time**: O(1) - Direct array update

## How It Works

### Example Array
Consider the array: `[3, 1, 4, 1, 5, 9, 2, 6]`

### Query Example: RMQ(2, 6)
Finding the minimum element between indices 2 and 6 (inclusive):

```
Array:  [3, 1, 4, 1, 5, 9, 2, 6]
Index:   0  1  2  3  4  5  6  7
         
Query Range: [2, 6]
         └─────────────────┘
```

### ASCII Art Visualization

```
Step-by-step Linear Scan for RMQ(2, 6):
=========================================

Initial Array:
    [3]  [1]  [4]  [1]  [5]  [9]  [2]  [6]
     0    1    2    3    4    5    6    7

Query Range [2, 6]:
              ↓────────────────────↓
    [3]  [1] │[4]  [1]  [5]  [9]  [2]│ [6]
              └──────────────────────┘

Linear Scan Process:
--------------------
Step 1: Check index 2
    min = 4
         ↓
    ... │[4]  [1]  [5]  [9]  [2]│ ...
         ═══

Step 2: Check index 3
    min = min(4, 1) = 1
              ↓
    ... │[4]  [1]  [5]  [9]  [2]│ ...
         ═══  ═══

Step 3: Check index 4
    min = min(1, 5) = 1
                   ↓
    ... │[4]  [1]  [5]  [9]  [2]│ ...
         ═══  ═══  ═══

Step 4: Check index 5
    min = min(1, 9) = 1
                        ↓
    ... │[4]  [1]  [5]  [9]  [2]│ ...
         ═══  ═══  ═══  ═══

Step 5: Check index 6
    min = min(1, 2) = 1
                             ↓
    ... │[4]  [1]  [5]  [9]  [2]│ ...
         ═══  ═══  ═══  ═══  ═══

Final Result: minimum = 1 (at index 3)
```

## Implementation Details

### Pseudocode
```
function RMQ_Naive(array, left, right):
    min_value = array[left]
    
    for i from left+1 to right:
        if array[i] < min_value:
            min_value = array[i]
    
    return min_value
```

### C++ Implementation Highlights
```cpp
Value RMQNaive::performQuery(Index left, Index right) const {
    Value min_value = data_[left];
    
    for (Index i = left + 1; i <= right; ++i) {
        if (data_[i] < min_value) {
            min_value = data_[i];
        }
    }
    
    return min_value;
}
```

## Advantages
1. **Simplicity**: Extremely easy to implement and understand
2. **No Preprocessing**: Can answer queries immediately
3. **Memory Efficient**: No extra space beyond the input array
4. **Dynamic Updates**: O(1) updates since no preprocessing structure to maintain
5. **Good for Few Queries**: Optimal when queries are rare

## Disadvantages
1. **Slow Queries**: O(n) time per query is inefficient for many queries
2. **Not Scalable**: Performance degrades linearly with range size
3. **Repeated Work**: Doesn't reuse computations between overlapping queries

## When to Use
- When you have very few queries (< √n queries)
- When updates are frequent and queries are rare
- When simplicity is more important than performance
- For small arrays where O(n) is acceptable
- As a baseline for testing other algorithms

## Example Use Cases
1. **Real-time Systems**: Where updates happen frequently but queries are occasional
2. **Small Datasets**: When n < 100 and simplicity matters
3. **Debugging**: As a reference implementation to verify correctness
4. **One-time Queries**: When you only need to query once or twice

## Visualization of Multiple Queries

```
Array: [3, 1, 4, 1, 5, 9, 2, 6]

Query 1: RMQ(0, 3)
┌───────────┐
[3] [1] [4] [1] [5] [9] [2] [6]
 ↓   ↓   ↓   ↓
Scan: 3→1→4→1
Result: 1

Query 2: RMQ(4, 7)
                ┌───────────────┐
[3] [1] [4] [1] [5] [9] [2] [6]
                 ↓   ↓   ↓   ↓
                Scan: 5→9→2→6
                Result: 2

Query 3: RMQ(0, 7) - Full array
┌───────────────────────────────┐
[3] [1] [4] [1] [5] [9] [2] [6]
 ↓   ↓   ↓   ↓   ↓   ↓   ↓   ↓
Scan all elements
Result: 1
```

## Performance Comparison

| Array Size | Query Range | Time Taken |
|------------|-------------|------------|
| 100        | 50          | ~50 operations |
| 1,000      | 500         | ~500 operations |
| 10,000     | 5,000       | ~5,000 operations |
| 100,000    | 50,000      | ~50,000 operations |

## Conclusion
The Naive algorithm serves as the foundation for understanding RMQ problems. While not efficient for multiple queries, its simplicity and immediate query capability make it valuable in specific scenarios. It's particularly useful as a reference implementation and for systems with infrequent queries or frequent updates.