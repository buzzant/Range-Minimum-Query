#ifndef RMQ_FACTORY_RMQ_FACTORY_H
#define RMQ_FACTORY_RMQ_FACTORY_H

#include "../core/rmq_base.h"
#include "../core/rmq_types.h"
#include <memory>
#include <string>

namespace rmq {

/**
 * @brief Factory class for creating RMQ algorithm instances
 * 
 * This factory implements the Factory Method pattern to create
 * appropriate RMQ algorithm instances based on various criteria.
 * It provides both manual selection and automatic optimization.
 */
class RMQFactory {
public:
    /**
     * @brief Optimization criteria for automatic algorithm selection
     */
    enum class OptimizationCriteria {
        QUERY_TIME,        ///< Optimize for fastest query time
        PREPROCESSING_TIME,///< Optimize for fastest preprocessing
        MEMORY_USAGE,      ///< Optimize for minimum memory usage
        BALANCED,          ///< Balance between all factors
        UPDATE_SUPPORT     ///< Require support for updates
    };
    
    /**
     * @brief Create an RMQ algorithm instance by type
     * @param type The algorithm type to create
     * @param config Optional configuration
     * @return Unique pointer to the created algorithm
     * @throws std::invalid_argument if type is unknown
     */
    static RMQAlgorithmPtr create(
        AlgorithmType type,
        const AlgorithmConfig& config = AlgorithmConfig()
    );
    
    /**
     * @brief Create an optimal RMQ algorithm based on problem characteristics
     * @param array_size Size of the input array
     * @param expected_queries Expected number of queries
     * @param criteria Optimization criteria
     * @param config Optional configuration
     * @return Unique pointer to the optimal algorithm
     */
    static RMQAlgorithmPtr createOptimal(
        size_t array_size,
        size_t expected_queries,
        OptimizationCriteria criteria = OptimizationCriteria::BALANCED,
        const AlgorithmConfig& config = AlgorithmConfig()
    );
    
    /**
     * @brief Get recommended algorithm type based on characteristics
     * @param array_size Size of the input array
     * @param expected_queries Expected number of queries
     * @param requires_updates Whether updates are required
     * @return Recommended algorithm type
     */
    static AlgorithmType recommendAlgorithm(
        size_t array_size,
        size_t expected_queries,
        bool requires_updates = false
    );
    
    /**
     * @brief Get all available algorithm types
     * @return Vector of all algorithm types
     */
    static std::vector<AlgorithmType> getAvailableAlgorithms();
    
    /**
     * @brief Get algorithm description
     * @param type Algorithm type
     * @return Human-readable description
     */
    static std::string getAlgorithmDescription(AlgorithmType type);
    
    /**
     * @brief Check if an algorithm supports a specific feature
     * @param type Algorithm type
     * @param feature Feature name (e.g., "update", "O(1) query")
     * @return true if feature is supported
     */
    static bool supportsFeature(AlgorithmType type, const std::string& feature);
    
    /**
     * @brief Create all algorithms for comparison
     * @param config Optional configuration for all algorithms
     * @return Vector of all algorithm instances
     */
    static std::vector<RMQAlgorithmPtr> createAll(
        const AlgorithmConfig& config = AlgorithmConfig()
    );
    
    /**
     * @brief Benchmark decision helper
     * 
     * This structure helps decide which algorithm to use based on benchmarks
     */
    struct BenchmarkRecommendation {
        AlgorithmType recommended_type;
        std::string reasoning;
        double expected_preprocessing_ms;
        double expected_query_ms;
        size_t expected_memory_bytes;
    };
    
    /**
     * @brief Get benchmark-based recommendation
     * @param array_size Size of the input array
     * @param expected_queries Expected number of queries
     * @return Benchmark-based recommendation
     */
    static BenchmarkRecommendation getBenchmarkRecommendation(
        size_t array_size,
        size_t expected_queries
    );
    
    /**
     * @brief Calculate expected time for preprocessing (milliseconds)
     */
    static double calculatePreprocessingTime(
        AlgorithmType type,
        size_t array_size
    );
    
    /**
     * @brief Calculate expected time per query (microseconds)
     */
    static double calculateQueryTime(
        AlgorithmType type,
        size_t array_size
    );
    
    /**
     * @brief Calculate expected memory usage (bytes)
     */
    static size_t calculateMemoryUsage(
        AlgorithmType type,
        size_t array_size
    );
    
private:
    
    /**
     * @brief Private constructor (static class)
     */
    RMQFactory() = delete;
};

} // namespace rmq

#endif // RMQ_FACTORY_RMQ_FACTORY_H