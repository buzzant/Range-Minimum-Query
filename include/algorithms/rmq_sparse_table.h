#ifndef RMQ_ALGORITHMS_RMQ_SPARSE_TABLE_H
#define RMQ_ALGORITHMS_RMQ_SPARSE_TABLE_H

#include "../core/rmq_base.h"
#include <vector>
#include <cmath>

namespace rmq {

/**
 * @brief Sparse Table implementation of Range Minimum Query
 * 
 * This implementation uses binary lifting to precompute answers for
 * ranges of power-of-2 lengths. It provides O(1) query time with
 * O(n log n) preprocessing time and space.
 * 
 * @complexity
 * - Preprocessing: O(n log n) time, O(n log n) space
 * - Query: O(1) time, O(1) space
 * - Update: Not supported (requires full rebuild)
 * - Total Space: O(n log n) for the sparse table
 * 
 * @note This is optimal for static arrays with many queries
 */
class RMQSparseTable final : public RMQBase {
private:
    static constexpr const char* ALGORITHM_NAME = "Sparse Table (Binary Lifting)";
    static constexpr AlgorithmType ALGORITHM_TYPE = AlgorithmType::SPARSE_TABLE;
    
    /**
     * @brief Sparse table where sparse_table_[i][j] = min in range [i, i + 2^j - 1]
     */
    std::vector<std::vector<Value>> sparse_table_;
    
    /**
     * @brief Table storing index of minimum element for each range
     */
    std::vector<std::vector<Index>> index_table_;
    
    /**
     * @brief Precomputed logarithms for O(1) query
     */
    std::vector<int> log_table_;
    
    /**
     * @brief Number of levels in the sparse table (log2(n) + 1)
     */
    size_t max_level_;
    
    /**
     * @brief Compute floor(log2(n)) efficiently
     */
    static int computeLog2(size_t n);
    
    /**
     * @brief Precompute logarithms for all values up to n
     */
    void precomputeLogTable(size_t n);
    
    /**
     * @brief Clear sparse tables and free memory
     */
    void clearTables();
    
protected:
    /**
     * @brief Build the sparse table using binary lifting
     * 
     * Algorithm:
     * 1. Base case: sparse_table[i][0] = A[i]
     * 2. For each power j: sparse_table[i][j] = min(sparse_table[i][j-1], 
     *                                               sparse_table[i + 2^(j-1)][j-1])
     */
    void performPreprocess() override;
    
    /**
     * @brief Query using sparse table with overlap technique
     * 
     * For range [L, R]:
     * 1. Find largest k where 2^k <= R - L + 1
     * 2. Return min(sparse_table[L][k], sparse_table[R - 2^k + 1][k])
     * 
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in range [left, right]
     */
    Value performQuery(Index left, Index right) const override;
    
    /**
     * @brief Get index of minimum element using index table
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Index of minimum element in range
     */
    Index findMinimumIndex(Index left, Index right) const override;
    
public:
    /**
     * @brief Default constructor
     */
    RMQSparseTable();
    
    /**
     * @brief Constructor with configuration
     * @param config Algorithm configuration
     */
    explicit RMQSparseTable(const AlgorithmConfig& config);
    
    /**
     * @brief Destructor - ensures proper cleanup of tables
     */
    ~RMQSparseTable() override;
    
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
     * @return Complexity details for the sparse table algorithm
     */
    ComplexityInfo getComplexity() const override;
    
    /**
     * @brief Check if the algorithm supports dynamic updates
     * @return false (sparse table requires full rebuild for updates)
     */
    bool supportsUpdate() const override {
        return false;
    }
    
    /**
     * @brief Clear all preprocessed data
     */
    void clear() override;
    
    /**
     * @brief Get memory usage in bytes
     * @return Approximate memory usage including sparse tables
     */
    size_t getMemoryUsage() const;
    
    /**
     * @brief Get the number of levels in the sparse table
     * @return Number of levels (log2(n) + 1)
     */
    size_t getLevels() const {
        return max_level_;
    }
    
    /**
     * @brief Get total number of entries in sparse table
     * @return Total entries across all levels
     */
    size_t getTableEntries() const;
    
    /**
     * @brief Verify sparse table correctness (for debugging)
     * @return true if table is valid, false otherwise
     */
    bool verifyTable() const;
    
    /**
     * @brief Get statistics about the sparse table
     * @return Tuple of (levels, total_entries, memory_bytes)
     */
    std::tuple<size_t, size_t, size_t> getTableStats() const;
};

} // namespace rmq

#endif // RMQ_ALGORITHMS_RMQ_SPARSE_TABLE_H