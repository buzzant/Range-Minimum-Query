#include "../../include/algorithms/rmq_dp.h"
#include <algorithm>
#include <sstream>

namespace rmq {

RMQDynamicProgramming::RMQDynamicProgramming(const AlgorithmConfig& config) 
    : RMQBase(config) {
}

RMQDynamicProgramming::~RMQDynamicProgramming() {
    clearTables();
}

void RMQDynamicProgramming::validateSizeForDP() const {
    if (data_.size() > constants::MAX_ARRAY_SIZE) {
        std::ostringstream oss;
        oss << "Array size " << data_.size() 
            << " exceeds maximum allowed size " << constants::MAX_ARRAY_SIZE;
        throw InvalidDataException(oss.str());
    }
    
    // Check if size would cause memory issues
    size_t required_memory = data_.size() * data_.size() * (sizeof(Value) + sizeof(Index));
    const size_t MAX_MEMORY = 1024 * 1024 * 512; // 512 MB limit
    
    if (required_memory > MAX_MEMORY) {
        std::ostringstream oss;
        oss << "Array size " << data_.size() 
            << " would require " << (required_memory / (1024 * 1024)) 
            << " MB of memory, exceeding the limit";
        throw AllocationException(oss.str());
    }
    
    // Warning for large sizes (but still allow)
    if (!isRecommendedSize(data_.size()) && config_.track_statistics) {
        // In production, this would log a warning
        // For now, we just note it internally
    }
}

void RMQDynamicProgramming::clearTables() {
    dp_table_.clear();
    dp_table_.shrink_to_fit();
    min_index_table_.clear();
    min_index_table_.shrink_to_fit();
}

void RMQDynamicProgramming::performPreprocess() {
    Size n = data_.size();
    validateSizeForDP();
    
    // Clear any existing tables
    clearTables();
    
    // Allocate DP tables
    try {
        dp_table_.resize(n, std::vector<Value>(n));
        min_index_table_.resize(n, std::vector<Index>(n));
    } catch (const std::bad_alloc&) {
        clearTables();
        throw AllocationException(n * n * (sizeof(Value) + sizeof(Index)));
    }
    
    // Fill DP table using dynamic programming
    // Base case: single elements
    for (Index i = 0; i < n; ++i) {
        dp_table_[i][i] = data_[i];
        min_index_table_[i][i] = i;
    }
    
    // Fill for increasing lengths
    for (Size length = 2; length <= n; ++length) {
        for (Index i = 0; i <= n - length; ++i) {
            Index j = i + length - 1;
            
            // Compare the minimum of [i, j-1] with element at j
            if (dp_table_[i][j - 1] <= data_[j]) {
                dp_table_[i][j] = dp_table_[i][j - 1];
                min_index_table_[i][j] = min_index_table_[i][j - 1];
            } else {
                dp_table_[i][j] = data_[j];
                min_index_table_[i][j] = j;
            }
        }
    }
}

Value RMQDynamicProgramming::performQuery(Index left, Index right) const {
    // Direct O(1) lookup from precomputed table
    return dp_table_[left][right];
}

Index RMQDynamicProgramming::findMinimumIndex(Index left, Index right) const {
    // Direct O(1) lookup from precomputed index table
    return min_index_table_[left][right];
}

ComplexityInfo RMQDynamicProgramming::getComplexity() const {
    return ComplexityInfo(
        "O(n²)",     // preprocessing_time
        "O(n²)",     // preprocessing_space
        "O(1)",      // query_time
        "O(1)",      // query_space
        "O(n²)"      // total_space
    );
}

void RMQDynamicProgramming::clear() {
    RMQBase::clear();
    clearTables();
}

size_t RMQDynamicProgramming::getMemoryUsage() const {
    size_t base_memory = sizeof(RMQDynamicProgramming);
    
    // Data vector memory
    if (!data_.empty()) {
        base_memory += data_.capacity() * sizeof(Value);
    }
    
    // DP table memory
    if (!dp_table_.empty()) {
        size_t table_memory = 0;
        for (const auto& row : dp_table_) {
            table_memory += row.capacity() * sizeof(Value);
        }
        base_memory += table_memory;
        base_memory += dp_table_.capacity() * sizeof(std::vector<Value>);
    }
    
    // Index table memory
    if (!min_index_table_.empty()) {
        size_t index_memory = 0;
        for (const auto& row : min_index_table_) {
            index_memory += row.capacity() * sizeof(Index);
        }
        base_memory += index_memory;
        base_memory += min_index_table_.capacity() * sizeof(std::vector<Index>);
    }
    
    return base_memory;
}

size_t RMQDynamicProgramming::getTableSize() const {
    if (dp_table_.empty()) {
        return 0;
    }
    
    Size n = dp_table_.size();
    return n * n;
}

std::pair<size_t, size_t> RMQDynamicProgramming::getPreprocessingStats() const {
    size_t entries = getTableSize();
    size_t memory = getMemoryUsage();
    return {entries, memory};
}

} // namespace rmq