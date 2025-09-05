#include "../../include/algorithms/rmq_block.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace rmq {

RMQBlockDecomposition::RMQBlockDecomposition() 
    : RMQBase(), block_size_(0), num_blocks_(0) {
}

RMQBlockDecomposition::RMQBlockDecomposition(const AlgorithmConfig& config)
    : RMQBase(config), block_size_(0), num_blocks_(0) {
}

RMQBlockDecomposition::~RMQBlockDecomposition() {
    clearBlocks();
}

size_t RMQBlockDecomposition::calculateBlockSize(size_t n) const {
    // Use custom block size if specified, otherwise use sqrt(n)
    if (config_.block_size != constants::DEFAULT_BLOCK_SIZE) {
        return std::min(config_.block_size, n);
    }
    
    // Default to sqrt(n) for optimal complexity
    return static_cast<size_t>(std::sqrt(n)) + 1;
}

void RMQBlockDecomposition::clearBlocks() {
    block_min_.clear();
    block_min_.shrink_to_fit();
    block_min_index_.clear();
    block_min_index_.shrink_to_fit();
    block_size_ = 0;
    num_blocks_ = 0;
}

void RMQBlockDecomposition::computeBlockMinimum(size_t block) {
    Index start = getBlockStart(block);
    Index end = getBlockEnd(block);
    
    Value min_val = data_[start];
    Index min_idx = start;
    
    for (Index i = start + 1; i <= end; ++i) {
        if (data_[i] < min_val) {
            min_val = data_[i];
            min_idx = i;
        }
    }
    
    block_min_[block] = min_val;
    block_min_index_[block] = min_idx;
}

void RMQBlockDecomposition::performPreprocess() {
    Size n = data_.size();
    if (n == 0) return;
    
    // Clear any existing block data
    clearBlocks();
    
    // Calculate block size and number of blocks
    block_size_ = calculateBlockSize(n);
    num_blocks_ = (n + block_size_ - 1) / block_size_;
    
    // Allocate block arrays
    try {
        block_min_.resize(num_blocks_);
        block_min_index_.resize(num_blocks_);
    } catch (const std::bad_alloc&) {
        clearBlocks();
        throw AllocationException("Failed to allocate block arrays");
    }
    
    // Compute minimum for each block
    for (size_t block = 0; block < num_blocks_; ++block) {
        computeBlockMinimum(block);
    }
}

Value RMQBlockDecomposition::queryPartialBlock(Index left, Index right) const {
    Value min_val = data_[left];
    
    for (Index i = left + 1; i <= right; ++i) {
        if (data_[i] < min_val) {
            min_val = data_[i];
        }
    }
    
    return min_val;
}

Index RMQBlockDecomposition::findMinIndexPartialBlock(Index left, Index right) const {
    Value min_val = data_[left];
    Index min_idx = left;
    
    for (Index i = left + 1; i <= right; ++i) {
        if (data_[i] < min_val) {
            min_val = data_[i];
            min_idx = i;
        }
    }
    
    return min_idx;
}

Value RMQBlockDecomposition::performQuery(Index left, Index right) const {
    size_t left_block = getBlockNumber(left);
    size_t right_block = getBlockNumber(right);
    
    Value result = std::numeric_limits<Value>::max();
    
    if (left_block == right_block) {
        // Query is within a single block
        result = queryPartialBlock(left, right);
    } else {
        // Handle partial left block
        Index left_block_end = getBlockEnd(left_block);
        result = std::min(result, queryPartialBlock(left, left_block_end));
        
        // Handle complete middle blocks
        for (size_t block = left_block + 1; block < right_block; ++block) {
            result = std::min(result, block_min_[block]);
        }
        
        // Handle partial right block
        Index right_block_start = getBlockStart(right_block);
        result = std::min(result, queryPartialBlock(right_block_start, right));
    }
    
    return result;
}

Index RMQBlockDecomposition::findMinimumIndex(Index left, Index right) const {
    size_t left_block = getBlockNumber(left);
    size_t right_block = getBlockNumber(right);
    
    Value min_val = std::numeric_limits<Value>::max();
    Index min_idx = left;
    
    if (left_block == right_block) {
        // Query is within a single block
        return findMinIndexPartialBlock(left, right);
    }
    
    // Handle partial left block
    Index left_block_end = getBlockEnd(left_block);
    Index partial_idx = findMinIndexPartialBlock(left, left_block_end);
    if (data_[partial_idx] < min_val) {
        min_val = data_[partial_idx];
        min_idx = partial_idx;
    }
    
    // Handle complete middle blocks
    for (size_t block = left_block + 1; block < right_block; ++block) {
        if (block_min_[block] < min_val) {
            min_val = block_min_[block];
            min_idx = block_min_index_[block];
        }
    }
    
    // Handle partial right block
    Index right_block_start = getBlockStart(right_block);
    partial_idx = findMinIndexPartialBlock(right_block_start, right);
    if (data_[partial_idx] < min_val) {
        min_idx = partial_idx;
    }
    
    return min_idx;
}

ComplexityInfo RMQBlockDecomposition::getComplexity() const {
    return ComplexityInfo(
        "O(n)",      // preprocessing_time
        "O(√n)",     // preprocessing_space
        "O(√n)",     // query_time
        "O(1)",      // query_space
        "O(n + √n)"  // total_space
    );
}

void RMQBlockDecomposition::update(Index index, Value value) {
    ensurePreprocessed();
    
    if (index >= data_.size()) {
        throw BoundsException(index, data_.size());
    }
    
    // Update the value in the array
    data_[index] = value;
    
    // Recompute minimum for the affected block
    size_t block = getBlockNumber(index);
    computeBlockMinimum(block);
}

void RMQBlockDecomposition::batchUpdate(const std::vector<std::pair<Index, Value>>& updates) {
    ensurePreprocessed();
    
    // Validate all indices first
    for (const auto& [index, value] : updates) {
        if (index >= data_.size()) {
            throw BoundsException(index, data_.size());
        }
    }
    
    // Track which blocks need recomputation
    std::vector<bool> blocks_to_update(num_blocks_, false);
    
    // Apply all updates
    for (const auto& [index, value] : updates) {
        data_[index] = value;
        blocks_to_update[getBlockNumber(index)] = true;
    }
    
    // Recompute affected blocks
    for (size_t block = 0; block < num_blocks_; ++block) {
        if (blocks_to_update[block]) {
            computeBlockMinimum(block);
        }
    }
}

void RMQBlockDecomposition::clear() {
    RMQBase::clear();
    clearBlocks();
}

size_t RMQBlockDecomposition::getMemoryUsage() const {
    size_t base_memory = sizeof(RMQBlockDecomposition);
    
    // Data vector memory
    if (!data_.empty()) {
        base_memory += data_.capacity() * sizeof(Value);
    }
    
    // Block arrays memory
    if (!block_min_.empty()) {
        base_memory += block_min_.capacity() * sizeof(Value);
        base_memory += block_min_index_.capacity() * sizeof(Index);
    }
    
    return base_memory;
}

std::tuple<size_t, size_t, size_t> RMQBlockDecomposition::getBlockStats() const {
    return std::make_tuple(block_size_, num_blocks_, getMemoryUsage());
}

void RMQBlockDecomposition::rebuildBlocks() {
    if (!preprocessed_) {
        throw NotPreprocessedException(getName());
    }
    
    // Recompute all block minimums
    for (size_t block = 0; block < num_blocks_; ++block) {
        computeBlockMinimum(block);
    }
}

} // namespace rmq