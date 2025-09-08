# RMQ Implementation Plan

## Project Overview
Implementation of Range Minimum Query (RMQ) algorithms in C++ with multiple optimization techniques, following clean Object-Oriented Programming principles and design patterns.

## Core Design Principles

### Clean Code Architecture
- **SOLID Principles**: Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion
- **DRY (Don't Repeat Yourself)**: Shared functionality in base classes and utility functions
- **KISS (Keep It Simple, Stupid)**: Clear, understandable implementations over clever tricks
- **YAGNI (You Aren't Gonna Need It)**: No premature optimization or unnecessary features

### OOP Design Patterns
1. **Template Method Pattern**: Base class defines algorithm structure, derived classes implement specifics
2. **Strategy Pattern**: Each RMQ algorithm as a strategy, interchangeable at runtime
3. **Factory Pattern**: Factory class for creating appropriate RMQ instances
4. **Builder Pattern**: For complex algorithm configurations

### Code Organization
- **Separation of Concerns**: Interface, implementation, and testing clearly separated
- **Abstraction Layers**: Clean interfaces hiding implementation complexity
- **Encapsulation**: Private implementation details, public well-defined APIs
- **Inheritance Hierarchy**: Proper use of virtual functions and abstract base classes

## Algorithms to Implement

### 1. Naive Approach
- Simple linear scan for each query
- Baseline for performance comparison

### 2. Dynamic Programming Approach
- 2D DP table for all possible ranges
- Precompute all [i,j] pairs

### 3. Sparse Table (Binary Lifting)
- Logarithmic decomposition of ranges
- Optimal for static arrays with many queries

### 4. Block Decomposition (Square Root Decomposition)
- Divide array into √n blocks
- Balance between preprocessing and query time

### 5. LCA-based RMQ
- Convert RMQ to Lowest Common Ancestor problem
- Demonstrates RMQ-LCA equivalence

## File Structure

```
RMQ/
├── docs/
│   ├── implementation_plan.md      # This file
│   ├── algorithms_explanation.md   # Detailed algorithm explanations
│   └── CLAUDE.md                  # Clean code guidelines for AI assistance
├── include/
│   ├── core/
│   │   ├── rmq_base.h            # Abstract base class and interfaces
│   │   ├── rmq_exception.h       # Custom exception classes
│   │   └── rmq_types.h           # Type definitions and constants
│   ├── algorithms/
│   │   ├── rmq_naive.h           # Naive implementation
│   │   ├── rmq_dp.h              # Dynamic Programming implementation
│   │   ├── rmq_sparse_table.h    # Sparse Table implementation
│   │   ├── rmq_block.h           # Block Decomposition implementation
│   │   └── rmq_lca.h             # LCA-based implementation
│   ├── factory/
│   │   └── rmq_factory.h         # Factory for creating RMQ instances
│   └── utils/
│       ├── benchmark.h           # Benchmarking utilities
│       └── validator.h           # Input validation utilities
├── src/
│   ├── algorithms/
│   │   ├── rmq_naive.cpp         # Naive implementation
│   │   ├── rmq_dp.cpp            # DP implementation
│   │   ├── rmq_sparse_table.cpp  # Sparse Table implementation
│   │   ├── rmq_block.cpp         # Block implementation
│   │   └── rmq_lca.cpp           # LCA implementation
│   └── factory/
│       └── rmq_factory.cpp       # Factory implementation
├── tests/
│   ├── unit/
│   │   ├── test_naive.cpp        # Unit tests for naive
│   │   ├── test_dp.cpp           # Unit tests for DP
│   │   ├── test_sparse_table.cpp # Unit tests for sparse table
│   │   ├── test_block.cpp        # Unit tests for block
│   │   └── test_lca.cpp          # Unit tests for LCA
│   ├── integration/
│   │   └── test_all.cpp          # Integration tests
│   └── performance/
│       └── benchmark.cpp         # Performance benchmarks
├── examples/
│   └── demo.cpp                  # Usage examples
├── CMakeLists.txt                # CMake build configuration
└── Makefile                      # Alternative Make build
```

## Implementation Steps

### Phase 1: Setup
1. Create base RMQ interface class
2. Define common query interface
3. Set up testing framework

### Phase 2: Basic Implementations
1. Implement Naive approach
2. Implement DP approach
3. Add basic test cases

### Phase 3: Advanced Implementations
1. Implement Sparse Table
2. Implement Block Decomposition
3. Implement LCA-based approach

### Phase 4: Testing & Benchmarking
1. Create comprehensive test suite
2. Add performance benchmarks
3. Compare time/space complexities

## Interface Design

### Base Class Hierarchy

```cpp
namespace rmq {

// Abstract base class following Interface Segregation Principle
class IRMQAlgorithm {
public:
    virtual ~IRMQAlgorithm() = default;
    
    // Core operations
    virtual void preprocess(const std::vector<int>& data) = 0;
    virtual int query(size_t left, size_t right) const = 0;
    
    // Metadata operations
    virtual std::string getName() const = 0;
    virtual ComplexityInfo getComplexity() const = 0;
    virtual bool supportsUpdate() const = 0;
};

// Base implementation with common functionality
class RMQBase : public IRMQAlgorithm {
protected:
    std::vector<int> data_;
    bool preprocessed_;
    
    // Template Method Pattern - derived classes override these
    virtual void validateInput(size_t left, size_t right) const;
    virtual void performPreprocess() = 0;
    virtual int performQuery(size_t left, size_t right) const = 0;
    
public:
    RMQBase();
    
    // Final methods enforcing invariants
    void preprocess(const std::vector<int>& data) override final;
    int query(size_t left, size_t right) const override final;
    
    // Helper methods
    size_t size() const { return data_.size(); }
    bool isPreprocessed() const { return preprocessed_; }
};

} // namespace rmq
```

### Factory Pattern

```cpp
namespace rmq {

enum class AlgorithmType {
    NAIVE,
    DYNAMIC_PROGRAMMING,
    SPARSE_TABLE,
    BLOCK_DECOMPOSITION,
    LCA_BASED
};

class RMQFactory {
public:
    static std::unique_ptr<IRMQAlgorithm> create(AlgorithmType type);
    static std::unique_ptr<IRMQAlgorithm> createOptimal(size_t arraySize, 
                                                        size_t expectedQueries,
                                                        bool requiresUpdate = false);
};

} // namespace rmq
```

## Testing Strategy

### Correctness Tests
- Small arrays (n < 10)
- Edge cases (single element, entire array)
- Random queries validation

### Performance Tests
- Arrays of size: 100, 1000, 10000, 100000
- Query patterns: random, sequential, overlapping
- Measure preprocessing and query times

## Coding Standards

### Naming Conventions
- **Classes**: PascalCase (e.g., `RMQSparseTable`)
- **Methods**: camelCase (e.g., `performQuery`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_ARRAY_SIZE`)
- **Private Members**: trailing underscore (e.g., `data_`)
- **Namespaces**: lowercase (e.g., `rmq`)

### Documentation
- Doxygen-style comments for all public APIs
- Complexity annotations for all algorithms
- Usage examples in header files
- Implementation notes for complex logic

### Error Handling
- Custom exception hierarchy
- RAII for resource management
- Strong exception safety guarantee
- Descriptive error messages

### Modern C++ Features
- Smart pointers for memory management
- Move semantics for efficiency
- `constexpr` for compile-time computation
- `noexcept` specifications
- Range-based for loops
- Auto type deduction where appropriate

## Success Criteria
- All implementations pass correctness tests
- Performance matches theoretical complexity
- Clean, well-documented OOP code
- Proper separation of concerns
- No code duplication
- Comprehensive test coverage (>90%)
- Memory leak free (verified with valgrind)
- Comparative analysis of approaches