#ifndef RMQ_CORE_RMQ_TYPES_H
#define RMQ_CORE_RMQ_TYPES_H

#include <cstddef>
#include <string>
#include <chrono>
#include <limits>

namespace rmq {

/**
 * @brief Type alias for array indices
 */
using Index = std::size_t;

/**
 * @brief Type alias for array values
 */
using Value = int;

/**
 * @brief Type alias for array size
 */
using Size = std::size_t;

/**
 * @brief Time duration type for benchmarking
 */
using Duration = std::chrono::duration<double, std::milli>;

/**
 * @brief Constants for RMQ algorithms
 */
namespace constants {
    
    /**
     * @brief Maximum supported array size
     */
    constexpr Size MAX_ARRAY_SIZE = 1000000;
    
    /**
     * @brief Minimum supported array size
     */
    constexpr Size MIN_ARRAY_SIZE = 1;
    
    /**
     * @brief Invalid index marker
     */
    constexpr Index INVALID_INDEX = std::numeric_limits<Index>::max();
    
    /**
     * @brief Default block size for block decomposition (will be computed as sqrt(n))
     */
    constexpr Size DEFAULT_BLOCK_SIZE = 0;
    
    /**
     * @brief Maximum recursion depth for LCA
     */
    constexpr Size MAX_RECURSION_DEPTH = 1000;
    
} // namespace constants

/**
 * @brief Enumeration of available RMQ algorithms
 */
enum class AlgorithmType {
    NAIVE,              ///< O(n) query, O(1) preprocessing
    DYNAMIC_PROGRAMMING,///< O(1) query, O(n²) preprocessing
    SPARSE_TABLE,       ///< O(1) query, O(n log n) preprocessing
    BLOCK_DECOMPOSITION,///< O(√n) query, O(n) preprocessing
    LCA_BASED          ///< O(log n) query, O(n) preprocessing
};

/**
 * @brief Complexity information for an algorithm
 */
struct ComplexityInfo {
    std::string preprocessing_time;  ///< Time complexity of preprocessing
    std::string preprocessing_space; ///< Space complexity of preprocessing
    std::string query_time;         ///< Time complexity of query
    std::string query_space;        ///< Space complexity of query
    std::string total_space;        ///< Total space complexity
    
    /**
     * @brief Constructor with all complexity parameters
     */
    ComplexityInfo(
        const std::string& prep_time,
        const std::string& prep_space,
        const std::string& q_time,
        const std::string& q_space,
        const std::string& t_space
    ) : preprocessing_time(prep_time),
        preprocessing_space(prep_space),
        query_time(q_time),
        query_space(q_space),
        total_space(t_space) {}
    
    /**
     * @brief Default constructor
     */
    ComplexityInfo() = default;
};

/**
 * @brief Query request structure
 */
struct Query {
    Index left;   ///< Left boundary (inclusive)
    Index right;  ///< Right boundary (inclusive)
    
    /**
     * @brief Constructor with bounds
     */
    Query(Index l, Index r) : left(l), right(r) {}
    
    /**
     * @brief Check if query is valid
     */
    bool isValid() const noexcept {
        return left <= right;
    }
    
    /**
     * @brief Get query range length
     */
    Size length() const noexcept {
        return right - left + 1;
    }
    
    /**
     * @brief Check if query is in bounds
     */
    bool isInBounds(Size array_size) const noexcept {
        return right < array_size;
    }
};

/**
 * @brief Result of a query operation
 */
struct QueryResult {
    Value minimum_value;  ///< The minimum value in the range
    Index minimum_index;  ///< The index of the minimum value
    Duration query_time;  ///< Time taken for the query
    
    /**
     * @brief Constructor
     */
    QueryResult(Value val, Index idx, Duration time) 
        : minimum_value(val), minimum_index(idx), query_time(time) {}
    
    /**
     * @brief Default constructor
     */
    QueryResult() : minimum_value(0), minimum_index(0), query_time(0) {}
};

/**
 * @brief Configuration for algorithm behavior
 */
struct AlgorithmConfig {
    bool enable_caching = false;        ///< Enable query result caching
    bool enable_parallel = false;       ///< Enable parallel preprocessing
    bool track_statistics = false;      ///< Track detailed statistics
    Size block_size = constants::DEFAULT_BLOCK_SIZE; ///< Block size for block decomposition
    
    /**
     * @brief Default constructor with default values
     */
    AlgorithmConfig() = default;
    
    /**
     * @brief Builder pattern method for caching
     */
    AlgorithmConfig& withCaching(bool enable) {
        enable_caching = enable;
        return *this;
    }
    
    /**
     * @brief Builder pattern method for parallel processing
     */
    AlgorithmConfig& withParallel(bool enable) {
        enable_parallel = enable;
        return *this;
    }
    
    /**
     * @brief Builder pattern method for statistics tracking
     */
    AlgorithmConfig& withStatistics(bool enable) {
        track_statistics = enable;
        return *this;
    }
    
    /**
     * @brief Builder pattern method for block size
     */
    AlgorithmConfig& withBlockSize(Size size) {
        block_size = size;
        return *this;
    }
};

/**
 * @brief Convert AlgorithmType to string
 */
inline std::string algorithmTypeToString(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::NAIVE:
            return "Naive";
        case AlgorithmType::DYNAMIC_PROGRAMMING:
            return "Dynamic Programming";
        case AlgorithmType::SPARSE_TABLE:
            return "Sparse Table";
        case AlgorithmType::BLOCK_DECOMPOSITION:
            return "Block Decomposition";
        case AlgorithmType::LCA_BASED:
            return "LCA-based";
        default:
            return "Unknown";
    }
}

} // namespace rmq

#endif // RMQ_CORE_RMQ_TYPES_H