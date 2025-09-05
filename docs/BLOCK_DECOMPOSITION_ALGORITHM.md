# Block Decomposition Algorithm (Square Root Decomposition)

## Overview
Block Decomposition divides the array into blocks of size √n and precomputes the minimum for each block. It provides a balanced trade-off between preprocessing time, query time, and update capability, making it versatile for dynamic RMQ problems.

## Algorithm Description

### Core Concept
1. Divide the array into blocks of size √n
2. Precompute and store the minimum value for each complete block
3. For queries, combine results from:
   - Partial left block (if any)
   - Complete middle blocks
   - Partial right block (if any)

### Complexity Analysis
- **Preprocessing Time**: O(n) - Single pass to compute block minimums
- **Preprocessing Space**: O(√n) - Store one minimum per block
- **Query Time**: O(√n) - At most 2√n elements to check
- **Query Space**: O(1) - No additional space
- **Update Time**: O(1) - Update element and recompute one block
- **Total Space**: O(n + √n) - Original array plus block minimums

## How It Works

### Example Array
Consider the array: `[3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8]` (n = 12)

### ASCII Art Visualization

```
Block Decomposition Structure
==============================

Original Array (n = 12):
    [3]  [1]  [4] | [1]  [5]  [9] | [2]  [6]  [5] | [3]  [5]  [8]
     0    1    2  |  3    4    5  |  6    7    8  |  9   10   11

Block Size = √12 ≈ 3 (rounded)
Number of Blocks = ⌈12/3⌉ = 4

Block Division:
===============
    Block 0        Block 1        Block 2        Block 3
    [0, 2]         [3, 5]         [6, 8]         [9, 11]
    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
    │ 3  1  4 │    │ 1  5  9 │    │ 2  6  5 │    │ 3  5  8 │
    └─────────┘    └─────────┘    └─────────┘    └─────────┘
        ↓              ↓              ↓              ↓
     min = 1        min = 1        min = 2        min = 3
     
Block Minimum Array:
    [1]  [1]  [2]  [3]
     B0   B1   B2   B3

Preprocessing Visualization:
============================
For each block i:
    block_min[i] = minimum of all elements in block i
    
    Block 0: min(3, 1, 4) = 1
    Block 1: min(1, 5, 9) = 1
    Block 2: min(2, 6, 5) = 2
    Block 3: min(3, 5, 8) = 3
```

### Query Example: RMQ(2, 9)

```
Query Process for RMQ(2, 9):
=============================

Step 1: Identify blocks involved
    Left index 2 → Block 0
    Right index 9 → Block 3

Step 2: Decompose query into parts

Array View:
    Block 0        Block 1        Block 2        Block 3
    [3]  [1]  [4] | [1]  [5]  [9] | [2]  [6]  [5] | [3]  [5]  [8]
     0    1    2  |  3    4    5  |  6    7    8  |  9   10   11
              ↓─────────────────────────────────────↓
              Query Range: [2, 9]

Query Components:
─────────────────
1. Partial Block 0: [2]
   Elements: [4]
   Minimum: 4

2. Complete Block 1: [3, 5]
   Use precomputed: block_min[1] = 1

3. Complete Block 2: [6, 8]
   Use precomputed: block_min[2] = 2

4. Partial Block 3: [9]
   Elements: [3]
   Minimum: 3

Visual Breakdown:
         Partial    Complete    Complete    Partial
         Block 0    Block 1     Block 2     Block 3
         ┌───┐    ┌─────────┐  ┌─────────┐  ┌───┐
    ... [4] | [1]  [5]  [9] | [2]  [6]  [5] | [3] ...
         └───┘    └─────────┘  └─────────┘  └───┘
          ↓          ↓            ↓           ↓
         4           1            2           3
                     ↓
              Final Result = min(4, 1, 2, 3) = 1
```

### Different Query Scenarios

```
Scenario 1: Query within single block
======================================
RMQ(3, 5) - All in Block 1

    Block 1
    [1]  [5]  [9]
     3    4    5
     └─────────┘
    Linear scan: min(1, 5, 9) = 1

Scenario 2: Query spans multiple complete blocks
==================================================
RMQ(3, 8) - Complete Blocks 1 and 2

    Block 1        Block 2
    [1]  [5]  [9] | [2]  [6]  [5]
     └───────────────────────────┘
    Use: min(block_min[1], block_min[2]) = min(1, 2) = 1

Scenario 3: Query with partial blocks on both ends
====================================================
RMQ(1, 10) - Partial B0, Complete B1 & B2, Partial B3

    B0         B1           B2           B3
    x [1][4] | [1][5][9] | [2][6][5] | [3][5] x
       └──────────────────────────────────┘
    Partial: [1,4]→1  Complete: 1,2  Partial: [3,5]→3
    Result: min(1, 1, 2, 3) = 1
```

## Implementation Details

### Pseudocode
```
function preprocess_BlockDecomposition(array):
    n = array.length
    block_size = sqrt(n)
    num_blocks = ceil(n / block_size)
    
    for block from 0 to num_blocks-1:
        start = block * block_size
        end = min(start + block_size - 1, n - 1)
        block_min[block] = minimum in array[start...end]

function query_BlockDecomposition(left, right):
    left_block = left / block_size
    right_block = right / block_size
    
    if left_block == right_block:
        // Query within single block
        return minimum in array[left...right]
    
    result = INFINITY
    
    // Partial left block
    left_block_end = (left_block + 1) * block_size - 1
    result = min(result, minimum in array[left...left_block_end])
    
    // Complete middle blocks
    for block from left_block+1 to right_block-1:
        result = min(result, block_min[block])
    
    // Partial right block
    right_block_start = right_block * block_size
    result = min(result, minimum in array[right_block_start...right])
    
    return result

function update_BlockDecomposition(index, value):
    array[index] = value
    block = index / block_size
    recompute block_min[block]
```

### C++ Implementation Highlights
```cpp
void RMQBlockDecomposition::performPreprocess() {
    Size n = data_.size();
    block_size_ = static_cast<size_t>(std::sqrt(n)) + 1;
    num_blocks_ = (n + block_size_ - 1) / block_size_;
    
    block_min_.resize(num_blocks_);
    
    for (size_t block = 0; block < num_blocks_; ++block) {
        Index start = block * block_size_;
        Index end = std::min(start + block_size_ - 1, n - 1);
        
        Value min_val = data_[start];
        for (Index i = start + 1; i <= end; ++i) {
            min_val = std::min(min_val, data_[i]);
        }
        block_min_[block] = min_val;
    }
}

void RMQBlockDecomposition::update(Index index, Value value) {
    data_[index] = value;
    
    // Recompute only the affected block
    size_t block = index / block_size_;
    Index start = block * block_size_;
    Index end = std::min(start + block_size_ - 1, data_.size() - 1);
    
    Value min_val = data_[start];
    for (Index i = start + 1; i <= end; ++i) {
        min_val = std::min(min_val, data_[i]);
    }
    block_min_[block] = min_val;
}
```

## Update Operation Visualization

```
Update Example: Set array[4] = 0
=================================

Before Update:
    Block 0        Block 1        Block 2        Block 3
    [3]  [1]  [4] | [1]  [5]  [9] | [2]  [6]  [5] | [3]  [5]  [8]
                          ↑
                    array[4] = 5
    
    Block mins: [1]  [1]  [2]  [3]

Update Process:
1. Change array[4] from 5 to 0
2. Identify affected block: Block 1 (4 / 3 = 1)
3. Recompute Block 1 minimum only

After Update:
    Block 0        Block 1        Block 2        Block 3
    [3]  [1]  [4] | [1]  [0]  [9] | [2]  [6]  [5] | [3]  [5]  [8]
                          ↑
                    array[4] = 0
    
    Block 1 new min = min(1, 0, 9) = 0
    Block mins: [1]  [0]  [2]  [3]
                     ↑
                  Updated!

Time Complexity: O(√n) to scan one block
```

## Memory Layout

```
Memory Structure for n=12, block_size=3:
=========================================

Original Array (12 elements):
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ 3 │ 1 │ 4 │ 1 │ 5 │ 9 │ 2 │ 6 │ 5 │ 3 │ 5 │ 8 │
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
  0   1   2   3   4   5   6   7   8   9  10  11

Block Minimum Array (4 elements):
┌───┬───┬───┬───┐
│ 1 │ 1 │ 2 │ 3 │
└───┴───┴───┴───┘
 B0  B1  B2  B3

Total Space: n + √n = 12 + 4 = 16 elements
```

## Optimization: Custom Block Sizes

```
Block Size Trade-offs:
======================

Smaller Blocks (size < √n):
- More blocks → More space (O(n/block_size))
- Faster queries within blocks
- More block minimums to check
- Better for range queries

Larger Blocks (size > √n):
- Fewer blocks → Less space
- Slower queries within blocks
- Fewer block minimums to check
- Better for updates

Optimal Choices:
- Standard: block_size = √n
- Update-heavy: block_size = n^(2/3)
- Query-heavy: block_size = n^(1/3)
```

## Advantages
1. **Balanced Performance**: O(√n) for both query and preprocessing
2. **Supports Updates**: O(1) element update, O(√n) block recomputation
3. **Space Efficient**: Only O(√n) extra space
4. **Simple Implementation**: Easier than segment trees or sparse tables
5. **Flexible**: Can adjust block size for different trade-offs

## Disadvantages
1. **Not Optimal**: O(√n) query is slower than O(1) or O(log n)
2. **Update Overhead**: Still need to scan entire block after update
3. **Block Size Tuning**: Performance depends on choosing good block size
4. **Cache Misses**: Jumping between blocks can cause cache misses

## When to Use
- Dynamic arrays with both queries and updates
- When O(√n) performance is acceptable
- Medium-sized arrays (10^3 to 10^6 elements)
- When simplicity is preferred over optimal complexity
- As a stepping stone to more complex data structures

## Example Use Cases
1. **Dynamic Statistics**: Running minimum in sliding windows
2. **Game Development**: Dynamic terrain or collision detection
3. **Real-time Systems**: Sensor data with occasional updates
4. **Database Indexes**: Simple range query optimization
5. **Competitive Programming**: When easier to implement than segment tree

## Comparison with Other Methods

| Method | Preprocess | Query | Update | Space | Complexity |
|--------|------------|-------|--------|-------|------------|
| Naive | O(1) | O(n) | O(1) | O(n) | Simple |
| **Block** | **O(n)** | **O(√n)** | **O(√n)** | **O(n+√n)** | **Medium** |
| Sparse Table | O(n log n) | O(1) | N/A | O(n log n) | Complex |
| Segment Tree | O(n) | O(log n) | O(log n) | O(n) | Complex |

## Conclusion
Block Decomposition provides an elegant balance between simplicity and efficiency. While not optimal in any single metric, its versatility makes it valuable for problems requiring both queries and updates. The √n decomposition principle it demonstrates appears in many advanced algorithms, making it an important technique to master.