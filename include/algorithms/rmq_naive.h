#ifndef RMQ_ALGORITHMS_RMQ_NAIVE_H
#define RMQ_ALGORITHMS_RMQ_NAIVE_H

#include "../core/rmq_base.h"

namespace rmq {

/**
 * @brief Naive implementation of Range Minimum Query
 * 
 * This implementation uses a simple linear scan for each query.
 * No preprocessing is required, making it suitable for scenarios
 * with frequent updates and rare queries.
 * 
 * @complexity
 * - Preprocessing: O(1) time, O(1) space
 * - Query: O(n) time, O(1) space
 * - Update: O(1) time
 * - Total Space: O(n) for storing the array
 */
class RMQNaive final : public RMQBase {
private:
    static constexpr const char* ALGORITHM_NAME = "Naive Linear Scan";
    static constexpr AlgorithmType ALGORITHM_TYPE = AlgorithmType::NAIVE;
    
protected:
    /**
     * @brief Perform preprocessing (no-op for naive approach)
     * 
     * The naive approach doesn't require any preprocessing
     * as it performs a linear scan for each query.
     */
    void performPreprocess() override;
    
    /**
     * @brief Perform a range minimum query using linear scan
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in the range [left, right]
     */
    Value performQuery(Index left, Index right) const override;
    
    /**
     * @brief Find the index of the minimum value in range
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Index of the first occurrence of minimum value
     */
    Index findMinimumIndex(Index left, Index right) const override;
    
public:
    /**
     * @brief Default constructor
     */
    RMQNaive() = default;
    
    /**
     * @brief Constructor with configuration
     * @param config Algorithm configuration
     */
    explicit RMQNaive(const AlgorithmConfig& config);
    
    /**
     * @brief Destructor
     */
    ~RMQNaive() override = default;
    
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
     * @return Complexity details for the naive algorithm
     */
    ComplexityInfo getComplexity() const override;
    
    /**
     * @brief Check if the algorithm supports dynamic updates
     * @return true (naive approach naturally supports updates)
     */
    bool supportsUpdate() const override {
        return true;
    }
    
    /**
     * @brief Update a single element (optional operation)
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
     * @brief Get memory usage in bytes
     * @return Approximate memory usage
     */
    size_t getMemoryUsage() const;
};

} // namespace rmq

#endif // RMQ_ALGORITHMS_RMQ_NAIVE_H