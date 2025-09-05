#include "../../include/algorithms/rmq_naive.h"
#include <algorithm>
#include <limits>

namespace rmq {

RMQNaive::RMQNaive(const AlgorithmConfig& config) : RMQBase(config) {
}

void RMQNaive::performPreprocess() {
    // No preprocessing required for naive approach
    // The data is already stored in the base class
}

Value RMQNaive::performQuery(Index left, Index right) const {
    // Simple linear scan to find minimum
    Value min_value = data_[left];
    
    for (Index i = left + 1; i <= right; ++i) {
        if (data_[i] < min_value) {
            min_value = data_[i];
        }
    }
    
    return min_value;
}

Index RMQNaive::findMinimumIndex(Index left, Index right) const {
    Value min_value = data_[left];
    Index min_index = left;
    
    for (Index i = left + 1; i <= right; ++i) {
        if (data_[i] < min_value) {
            min_value = data_[i];
            min_index = i;
        }
    }
    
    return min_index;
}

ComplexityInfo RMQNaive::getComplexity() const {
    return ComplexityInfo(
        "O(1)",      // preprocessing_time
        "O(1)",      // preprocessing_space
        "O(n)",      // query_time
        "O(1)",      // query_space
        "O(n)"       // total_space
    );
}

void RMQNaive::update(Index index, Value value) {
    ensurePreprocessed();
    
    if (index >= data_.size()) {
        throw BoundsException(index, data_.size());
    }
    
    data_[index] = value;
}

void RMQNaive::batchUpdate(const std::vector<std::pair<Index, Value>>& updates) {
    ensurePreprocessed();
    
    // Validate all indices first
    for (const auto& [index, value] : updates) {
        if (index >= data_.size()) {
            throw BoundsException(index, data_.size());
        }
    }
    
    // Apply all updates
    for (const auto& [index, value] : updates) {
        data_[index] = value;
    }
}

size_t RMQNaive::getMemoryUsage() const {
    // Base memory: vector overhead + data
    size_t base_memory = sizeof(RMQNaive);
    
    // Data vector memory
    if (!data_.empty()) {
        base_memory += data_.capacity() * sizeof(Value);
    }
    
    // Vector overhead (approximate)
    base_memory += sizeof(std::vector<Value>);
    
    return base_memory;
}

} // namespace rmq