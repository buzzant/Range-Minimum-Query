#ifndef RMQ_ALGORITHMS_RMQ_DP_H
#define RMQ_ALGORITHMS_RMQ_DP_H

#include "../core/rmq_base.h"
#include <vector>

namespace rmq {

/**
 * @brief Dynamic Programming implementation of Range Minimum Query
 * 
 * This implementation precomputes answers for all possible ranges
 * using dynamic programming. It provides O(1) query time at the cost
 * of O(n²) preprocessing time and space.
 * 
 * @complexity
 * - Preprocessing: O(n²) time, O(n²) space
 * - Query: O(1) time, O(1) space
 * - Update: O(n²) time (requires rebuilding)
 * - Total Space: O(n²) for the DP table
 * 
 * @note This approach is only suitable for small arrays (n ≤ 1000)
 * due to quadratic space requirements.
 */
class RMQDynamicProgramming final : public RMQBase {
private:
    static constexpr const char* ALGORITHM_NAME = "Dynamic Programming";
    static constexpr AlgorithmType ALGORITHM_TYPE = AlgorithmType::DYNAMIC_PROGRAMMING;
    static constexpr Size RECOMMENDED_MAX_SIZE = 1000;
    
    /**
     * @brief 2D DP table where dp_table_[i][j] stores minimum in range [i, j]
     */
    std::vector<std::vector<Value>> dp_table_;
    
    /**
     * @brief 2D table storing index of minimum element in range [i, j]
     */
    std::vector<std::vector<Index>> min_index_table_;
    
    /**
     * @brief Check if array size is suitable for DP approach
     */
    void validateSizeForDP() const;
    
    /**
     * @brief Clear DP tables and free memory
     */
    void clearTables();
    
protected:
    /**
     * @brief Build the DP table for all ranges
     * 
     * Uses dynamic programming to compute minimum for all possible ranges:
     * - dp[i][i] = A[i] (base case)
     * - dp[i][j] = min(dp[i][j-1], A[j]) for j > i
     */
    void performPreprocess() override;
    
    /**
     * @brief Query using precomputed DP table
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in range [left, right]
     */
    Value performQuery(Index left, Index right) const override;
    
    /**
     * @brief Get index of minimum element from precomputed table
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Index of minimum element in range
     */
    Index findMinimumIndex(Index left, Index right) const override;
    
public:
    /**
     * @brief Default constructor
     */
    RMQDynamicProgramming() = default;
    
    /**
     * @brief Constructor with configuration
     * @param config Algorithm configuration
     */
    explicit RMQDynamicProgramming(const AlgorithmConfig& config);
    
    /**
     * @brief Destructor - ensures proper cleanup of DP tables
     */
    ~RMQDynamicProgramming() override;
    
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
     * @return Complexity details for the DP algorithm
     */
    ComplexityInfo getComplexity() const override;
    
    /**
     * @brief Check if the algorithm supports dynamic updates
     * @return false (DP requires full rebuild for updates)
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
     * @return Approximate memory usage including DP tables
     */
    size_t getMemoryUsage() const;
    
    /**
     * @brief Get the size of the DP table
     * @return Number of entries in the DP table
     */
    size_t getTableSize() const;
    
    /**
     * @brief Check if a given array size is recommended for DP
     * @param size Size to check
     * @return true if size is within recommended limits
     */
    static bool isRecommendedSize(Size size) {
        return size <= RECOMMENDED_MAX_SIZE;
    }
    
    /**
     * @brief Get statistics about the preprocessing
     * @return Pair of (table_entries, memory_bytes)
     */
    std::pair<size_t, size_t> getPreprocessingStats() const;
};

} // namespace rmq

#endif // RMQ_ALGORITHMS_RMQ_DP_H