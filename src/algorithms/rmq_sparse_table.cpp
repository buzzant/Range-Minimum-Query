#include "../../include/algorithms/rmq_sparse_table.h"
#include <algorithm>
#include <tuple>

namespace rmq {

RMQSparseTable::RMQSparseTable() : RMQBase(), max_level_(0) {
}

RMQSparseTable::RMQSparseTable(const AlgorithmConfig& config) 
    : RMQBase(config), max_level_(0) {
}

RMQSparseTable::~RMQSparseTable() {
    clearTables();
}

int RMQSparseTable::computeLog2(size_t n) {
    if (n == 0) return 0;
    
    int log = 0;
    while ((size_t(1) << (log + 1)) <= n) {
        log++;
    }
    return log;
}

void RMQSparseTable::precomputeLogTable(size_t n) {
    log_table_.resize(n + 1);
    log_table_[0] = 0;
    log_table_[1] = 0;
    
    for (size_t i = 2; i <= n; ++i) {
        log_table_[i] = log_table_[i / 2] + 1;
    }
}

void RMQSparseTable::clearTables() {
    sparse_table_.clear();
    sparse_table_.shrink_to_fit();
    index_table_.clear();
    index_table_.shrink_to_fit();
    log_table_.clear();
    log_table_.shrink_to_fit();
    max_level_ = 0;
}

void RMQSparseTable::performPreprocess() {
    Size n = data_.size();
    if (n == 0) return;
    
    // Clear any existing tables
    clearTables();
    
    // Compute maximum level needed
    max_level_ = computeLog2(n) + 1;
    
    // Precompute logarithms for O(1) query
    precomputeLogTable(n);
    
    // Allocate sparse tables
    try {
        sparse_table_.resize(n);
        index_table_.resize(n);
        
        for (Index i = 0; i < n; ++i) {
            sparse_table_[i].resize(max_level_);
            index_table_[i].resize(max_level_);
        }
    } catch (const std::bad_alloc&) {
        clearTables();
        throw AllocationException("Failed to allocate sparse table");
    }
    
    // Initialize base case (ranges of length 1)
    for (Index i = 0; i < n; ++i) {
        sparse_table_[i][0] = data_[i];
        index_table_[i][0] = i;
    }
    
    // Build sparse table using binary lifting
    for (size_t j = 1; j < max_level_; ++j) {
        // Range length is 2^j
        size_t range_len = size_t(1) << j;
        
        // For all starting positions where range fits
        for (Index i = 0; i + range_len <= n; ++i) {
            // Combine two halves of length 2^(j-1)
            size_t half_len = size_t(1) << (j - 1);
            Index mid = i + half_len;
            
            if (sparse_table_[i][j - 1] <= sparse_table_[mid][j - 1]) {
                sparse_table_[i][j] = sparse_table_[i][j - 1];
                index_table_[i][j] = index_table_[i][j - 1];
            } else {
                sparse_table_[i][j] = sparse_table_[mid][j - 1];
                index_table_[i][j] = index_table_[mid][j - 1];
            }
        }
    }
}

Value RMQSparseTable::performQuery(Index left, Index right) const {
    // Compute range length
    size_t length = right - left + 1;
    
    // Find largest power of 2 that fits in the range
    int k = log_table_[length];
    
    // Cover the range with two overlapping power-of-2 ranges
    size_t power = size_t(1) << k;
    
    // Return minimum of the two overlapping ranges
    return std::min(
        sparse_table_[left][k],
        sparse_table_[right - power + 1][k]
    );
}

Index RMQSparseTable::findMinimumIndex(Index left, Index right) const {
    // Compute range length
    size_t length = right - left + 1;
    
    // Find largest power of 2 that fits in the range
    int k = log_table_[length];
    
    // Cover the range with two overlapping power-of-2 ranges
    size_t power = size_t(1) << k;
    
    // Return index corresponding to minimum value
    if (sparse_table_[left][k] <= sparse_table_[right - power + 1][k]) {
        return index_table_[left][k];
    } else {
        return index_table_[right - power + 1][k];
    }
}

ComplexityInfo RMQSparseTable::getComplexity() const {
    return ComplexityInfo(
        "O(n log n)",  // preprocessing_time
        "O(n log n)",  // preprocessing_space
        "O(1)",        // query_time
        "O(1)",        // query_space
        "O(n log n)"   // total_space
    );
}

void RMQSparseTable::clear() {
    RMQBase::clear();
    clearTables();
}

size_t RMQSparseTable::getMemoryUsage() const {
    size_t base_memory = sizeof(RMQSparseTable);
    
    // Data vector memory
    if (!data_.empty()) {
        base_memory += data_.capacity() * sizeof(Value);
    }
    
    // Sparse table memory
    if (!sparse_table_.empty()) {
        for (const auto& row : sparse_table_) {
            base_memory += row.capacity() * sizeof(Value);
        }
        base_memory += sparse_table_.capacity() * sizeof(std::vector<Value>);
    }
    
    // Index table memory
    if (!index_table_.empty()) {
        for (const auto& row : index_table_) {
            base_memory += row.capacity() * sizeof(Index);
        }
        base_memory += index_table_.capacity() * sizeof(std::vector<Index>);
    }
    
    // Log table memory
    if (!log_table_.empty()) {
        base_memory += log_table_.capacity() * sizeof(int);
    }
    
    return base_memory;
}

size_t RMQSparseTable::getTableEntries() const {
    if (sparse_table_.empty()) {
        return 0;
    }
    
    size_t total = 0;
    for (const auto& row : sparse_table_) {
        total += row.size();
    }
    return total;
}

bool RMQSparseTable::verifyTable() const {
    if (!preprocessed_ || sparse_table_.empty()) {
        return false;
    }
    
    Size n = data_.size();
    
    // Verify base case
    for (Index i = 0; i < n; ++i) {
        if (sparse_table_[i][0] != data_[i]) {
            return false;
        }
    }
    
    // Verify each level
    for (size_t j = 1; j < max_level_; ++j) {
        size_t range_len = size_t(1) << j;
        
        for (Index i = 0; i + range_len <= n; ++i) {
            // Compute expected value from previous level
            size_t half_len = size_t(1) << (j - 1);
            Index mid = i + half_len;
            
            Value expected = std::min(
                sparse_table_[i][j - 1],
                sparse_table_[mid][j - 1]
            );
            
            if (sparse_table_[i][j] != expected) {
                return false;
            }
        }
    }
    
    return true;
}

std::tuple<size_t, size_t, size_t> RMQSparseTable::getTableStats() const {
    size_t levels = max_level_;
    size_t entries = getTableEntries();
    size_t memory = getMemoryUsage();
    
    return std::make_tuple(levels, entries, memory);
}

} // namespace rmq