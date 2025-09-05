#ifndef RMQ_CORE_RMQ_BASE_H
#define RMQ_CORE_RMQ_BASE_H

#include <vector>
#include <memory>
#include <chrono>
#include "rmq_types.h"
#include "rmq_exception.h"

namespace rmq {

/**
 * @brief Abstract interface for all RMQ algorithms
 * 
 * This interface defines the contract that all RMQ implementations must follow.
 * It uses the Interface Segregation Principle to keep the interface minimal and focused.
 */
class IRMQAlgorithm {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IRMQAlgorithm() = default;
    
    /**
     * @brief Preprocess the input data for efficient queries
     * @param data The input array to preprocess
     * @throws InvalidDataException if data is empty or invalid
     * @throws AllocationException if memory allocation fails
     */
    virtual void preprocess(const std::vector<Value>& data) = 0;
    
    /**
     * @brief Query the minimum value in a range
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in the range [left, right]
     * @throws NotPreprocessedException if preprocess() hasn't been called
     * @throws BoundsException if indices are out of bounds
     * @throws InvalidQueryException if left > right
     */
    virtual Value query(Index left, Index right) const = 0;
    
    /**
     * @brief Query with detailed result information
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Detailed query result including value, index, and timing
     */
    virtual QueryResult queryDetailed(Index left, Index right) const = 0;
    
    /**
     * @brief Get the name of the algorithm
     * @return Human-readable algorithm name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the algorithm type enum
     * @return The algorithm type
     */
    virtual AlgorithmType getType() const = 0;
    
    /**
     * @brief Get complexity information
     * @return Complexity details for the algorithm
     */
    virtual ComplexityInfo getComplexity() const = 0;
    
    /**
     * @brief Check if the algorithm supports dynamic updates
     * @return true if updates are supported, false otherwise
     */
    virtual bool supportsUpdate() const = 0;
    
    /**
     * @brief Check if the algorithm has been preprocessed
     * @return true if preprocessed, false otherwise
     */
    virtual bool isPreprocessed() const = 0;
    
    /**
     * @brief Get the size of the preprocessed array
     * @return Size of the array, 0 if not preprocessed
     */
    virtual Size size() const = 0;
};

/**
 * @brief Base implementation class with common functionality
 * 
 * This class uses the Template Method pattern to provide common functionality
 * while allowing derived classes to implement specific algorithm details.
 */
class RMQBase : public IRMQAlgorithm {
protected:
    std::vector<Value> data_;           ///< The input data
    bool preprocessed_;                  ///< Whether preprocessing is complete
    mutable Duration last_query_time_;  ///< Time taken for the last query
    AlgorithmConfig config_;             ///< Algorithm configuration
    
    /**
     * @brief Validate query indices
     * @param left Left boundary
     * @param right Right boundary
     * @throws BoundsException if indices are out of bounds
     * @throws InvalidQueryException if left > right
     */
    void validateQuery(Index left, Index right) const;
    
    /**
     * @brief Validate input data
     * @param data The input data to validate
     * @throws InvalidDataException if data is invalid
     */
    void validateData(const std::vector<Value>& data) const;
    
    /**
     * @brief Check if preprocessing has been done
     * @throws NotPreprocessedException if not preprocessed
     */
    void ensurePreprocessed() const;
    
    /**
     * @brief Perform the actual preprocessing (to be implemented by derived classes)
     */
    virtual void performPreprocess() = 0;
    
    /**
     * @brief Perform the actual query (to be implemented by derived classes)
     * @param left Left boundary
     * @param right Right boundary
     * @return The minimum value in the range
     */
    virtual Value performQuery(Index left, Index right) const = 0;
    
    /**
     * @brief Find the index of the minimum value (optional override)
     * @param left Left boundary
     * @param right Right boundary
     * @return Index of the minimum value
     */
    virtual Index findMinimumIndex(Index left, Index right) const;
    
public:
    /**
     * @brief Default constructor
     */
    RMQBase();
    
    /**
     * @brief Constructor with configuration
     * @param config Algorithm configuration
     */
    explicit RMQBase(const AlgorithmConfig& config);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~RMQBase() = default;
    
    /**
     * @brief Preprocess the input data (Template Method)
     * @param data The input array to preprocess
     */
    void preprocess(const std::vector<Value>& data) override final;
    
    /**
     * @brief Query the minimum value in a range (Template Method)
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in the range
     */
    Value query(Index left, Index right) const override final;
    
    /**
     * @brief Query with detailed result information
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Detailed query result
     */
    QueryResult queryDetailed(Index left, Index right) const override final;
    
    /**
     * @brief Check if the algorithm has been preprocessed
     * @return true if preprocessed, false otherwise
     */
    bool isPreprocessed() const override final {
        return preprocessed_;
    }
    
    /**
     * @brief Get the size of the preprocessed array
     * @return Size of the array, 0 if not preprocessed
     */
    Size size() const override final {
        return data_.size();
    }
    
    /**
     * @brief Get the configuration
     * @return Current algorithm configuration
     */
    const AlgorithmConfig& getConfig() const {
        return config_;
    }
    
    /**
     * @brief Set the configuration
     * @param config New algorithm configuration
     */
    void setConfig(const AlgorithmConfig& config) {
        config_ = config;
    }
    
    /**
     * @brief Get the last query time
     * @return Duration of the last query
     */
    Duration getLastQueryTime() const {
        return last_query_time_;
    }
    
    /**
     * @brief Clear preprocessed data and reset state
     */
    virtual void clear();
};

/**
 * @brief Smart pointer type for RMQ algorithms
 */
using RMQAlgorithmPtr = std::unique_ptr<IRMQAlgorithm>;

/**
 * @brief Shared pointer type for RMQ algorithms
 */
using RMQAlgorithmSharedPtr = std::shared_ptr<IRMQAlgorithm>;

} // namespace rmq

#endif // RMQ_CORE_RMQ_BASE_H