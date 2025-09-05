#ifndef RMQ_ALGORITHMS_RMQ_BLOCK_H
#define RMQ_ALGORITHMS_RMQ_BLOCK_H

#include "../core/rmq_base.h"
#include <vector>
#include <cmath>

namespace rmq {

/**
 * @brief Block Decomposition (Square Root Decomposition) implementation of RMQ
 * 
 * This implementation divides the array into blocks of size √n and precomputes
 * the minimum for each complete block. Queries are answered by combining
 * partial blocks and complete blocks.
 * 
 * @complexity
 * - Preprocessing: O(n) time, O(√n) space
 * - Query: O(√n) time, O(1) space
 * - Update: O(1) time for single update, O(√n) for block rebuild
 * - Total Space: O(n + √n)
 * 
 * @note This provides a good balance between query time and update time
 */
class RMQBlockDecomposition final : public RMQBase {
private:
    static constexpr const char* ALGORITHM_NAME = "Block Decomposition (Square Root)";
    static constexpr AlgorithmType ALGORITHM_TYPE = AlgorithmType::BLOCK_DECOMPOSITION;
    
    /**
     * @brief Size of each block (√n by default)
     */
    size_t block_size_;
    
    /**
     * @brief Number of blocks
     */
    size_t num_blocks_;
    
    /**
     * @brief Minimum value for each complete block
     */
    std::vector<Value> block_min_;
    
    /**
     * @brief Index of minimum element in each block
     */
    std::vector<Index> block_min_index_;
    
    /**
     * @brief Calculate optimal block size
     */
    size_t calculateBlockSize(size_t n) const;
    
    /**
     * @brief Get block number for an index
     */
    size_t getBlockNumber(Index idx) const {
        return idx / block_size_;
    }
    
    /**
     * @brief Get starting index of a block
     */
    Index getBlockStart(size_t block) const {
        return block * block_size_;
    }
    
    /**
     * @brief Get ending index of a block (inclusive)
     */
    Index getBlockEnd(size_t block) const {
        Index end = (block + 1) * block_size_ - 1;
        return std::min(end, static_cast<Index>(data_.size() - 1));
    }
    
    /**
     * @brief Compute minimum for a single block
     */
    void computeBlockMinimum(size_t block);
    
    /**
     * @brief Query minimum in a partial block range
     */
    Value queryPartialBlock(Index left, Index right) const;
    
    /**
     * @brief Find minimum index in a partial block range
     */
    Index findMinIndexPartialBlock(Index left, Index right) const;
    
    /**
     * @brief Clear block data structures
     */
    void clearBlocks();
    
protected:
    /**
     * @brief Build block decomposition structure
     * 
     * Algorithm:
     * 1. Determine block size (√n or custom)
     * 2. Compute minimum for each complete block
     */
    void performPreprocess() override;
    
    /**
     * @brief Query using block decomposition
     * 
     * Algorithm:
     * 1. Handle partial left block
     * 2. Handle complete middle blocks
     * 3. Handle partial right block
     * 4. Return minimum of all parts
     * 
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in range [left, right]
     */
    Value performQuery(Index left, Index right) const override;
    
    /**
     * @brief Find index of minimum element
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Index of minimum element in range
     */
    Index findMinimumIndex(Index left, Index right) const override;
    
public:
    /**
     * @brief Default constructor
     */
    RMQBlockDecomposition();
    
    /**
     * @brief Constructor with configuration
     * @param config Algorithm configuration (can specify custom block size)
     */
    explicit RMQBlockDecomposition(const AlgorithmConfig& config);
    
    /**
     * @brief Destructor
     */
    ~RMQBlockDecomposition() override;
    
    /**
     * @brief Get the algorithm name
     * @return Human-readable algorithm name
     */
    std::string getName() const override {
        return ALGORITHM_NAME;
    }
    
    /**
     * @brief Get the algorithm type
     * @return Algorithm type enum value
     */
    AlgorithmType getType() const override {
        return ALGORITHM_TYPE;
    }
    
    /**
     * @brief Get complexity information
     * @return Complexity details for block decomposition
     */
    ComplexityInfo getComplexity() const override;
    
    /**
     * @brief Check if the algorithm supports dynamic updates
     * @return true (block decomposition supports efficient updates)
     */
    bool supportsUpdate() const override {
        return true;
    }
    
    /**
     * @brief Update a single element
     * @param index Index to update
     * @param value New value
     * @throws BoundsException if index is out of bounds
     * @throws NotPreprocessedException if not preprocessed
     */
    void update(Index index, Value value);
    
    /**
     * @brief Batch update multiple elements
     * @param updates Vector of pairs (index, new_value)
     * @throws BoundsException if any index is out of bounds
     * @throws NotPreprocessedException if not preprocessed
     */
    void batchUpdate(const std::vector<std::pair<Index, Value>>& updates);
    
    /**
     * @brief Clear all preprocessed data
     */
    void clear() override;
    
    /**
     * @brief Get memory usage in bytes
     * @return Approximate memory usage
     */
    size_t getMemoryUsage() const;
    
    /**
     * @brief Get current block size
     * @return Size of blocks
     */
    size_t getBlockSize() const {
        return block_size_;
    }
    
    /**
     * @brief Get number of blocks
     * @return Total number of blocks
     */
    size_t getNumBlocks() const {
        return num_blocks_;
    }
    
    /**
     * @brief Get block statistics
     * @return Tuple of (block_size, num_blocks, memory_bytes)
     */
    std::tuple<size_t, size_t, size_t> getBlockStats() const;
    
    /**
     * @brief Rebuild all block minimums (useful after many updates)
     */
    void rebuildBlocks();
};

} // namespace rmq

#endif // RMQ_ALGORITHMS_RMQ_BLOCK_H