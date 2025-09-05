#include "../../include/factory/rmq_factory.h"
#include "../../include/algorithms/rmq_naive.h"
#include "../../include/algorithms/rmq_dp.h"
#include "../../include/algorithms/rmq_sparse_table.h"
#include "../../include/algorithms/rmq_block.h"
#include "../../include/algorithms/rmq_lca.h"
#include <stdexcept>
#include <cmath>
#include <sstream>

namespace rmq {

RMQAlgorithmPtr RMQFactory::create(
    AlgorithmType type,
    const AlgorithmConfig& config) {
    
    switch (type) {
        case AlgorithmType::NAIVE:
            return std::make_unique<RMQNaive>(config);
            
        case AlgorithmType::DYNAMIC_PROGRAMMING:
            return std::make_unique<RMQDynamicProgramming>(config);
            
        case AlgorithmType::SPARSE_TABLE:
            return std::make_unique<RMQSparseTable>(config);
            
        case AlgorithmType::BLOCK_DECOMPOSITION:
            return std::make_unique<RMQBlockDecomposition>(config);
            
        case AlgorithmType::LCA_BASED:
            return std::make_unique<RMQLCABased>(config);
            
        default:
            throw std::invalid_argument("Unknown algorithm type");
    }
}

RMQAlgorithmPtr RMQFactory::createOptimal(
    size_t array_size,
    size_t expected_queries,
    OptimizationCriteria criteria,
    const AlgorithmConfig& config) {
    
    AlgorithmType recommended = AlgorithmType::NAIVE;
    
    switch (criteria) {
        case OptimizationCriteria::QUERY_TIME:
            // Optimize for O(1) query time
            if (array_size <= 1000) {
                recommended = AlgorithmType::DYNAMIC_PROGRAMMING;
            } else {
                recommended = AlgorithmType::SPARSE_TABLE;
            }
            break;
            
        case OptimizationCriteria::PREPROCESSING_TIME:
            // Optimize for O(1) or O(n) preprocessing
            recommended = AlgorithmType::NAIVE;
            break;
            
        case OptimizationCriteria::MEMORY_USAGE:
            // Optimize for minimum memory
            if (expected_queries < array_size / 10) {
                recommended = AlgorithmType::NAIVE;
            } else {
                recommended = AlgorithmType::BLOCK_DECOMPOSITION;
            }
            break;
            
        case OptimizationCriteria::UPDATE_SUPPORT:
            // Require update support
            if (expected_queries < array_size) {
                recommended = AlgorithmType::NAIVE;
            } else {
                recommended = AlgorithmType::BLOCK_DECOMPOSITION;
            }
            break;
            
        case OptimizationCriteria::BALANCED:
        default:
            // Balance all factors
            recommended = recommendAlgorithm(array_size, expected_queries, false);
            break;
    }
    
    return create(recommended, config);
}

AlgorithmType RMQFactory::recommendAlgorithm(
    size_t array_size,
    size_t expected_queries,
    bool requires_updates) {
    
    // If updates are required, limit choices
    if (requires_updates) {
        if (expected_queries < array_size / 10) {
            return AlgorithmType::NAIVE;
        } else {
            return AlgorithmType::BLOCK_DECOMPOSITION;
        }
    }
    
    // For very small arrays, DP is often best
    if (array_size <= 100) {
        return AlgorithmType::DYNAMIC_PROGRAMMING;
    }
    
    // For small arrays with many queries
    if (array_size <= 1000 && expected_queries > array_size * 10) {
        return AlgorithmType::DYNAMIC_PROGRAMMING;
    }
    
    // For few queries, naive is sufficient
    if (expected_queries < std::sqrt(array_size)) {
        return AlgorithmType::NAIVE;
    }
    
    // For many queries on static data
    if (expected_queries > array_size * std::log2(array_size)) {
        return AlgorithmType::SPARSE_TABLE;
    }
    
    // Default to block decomposition for balanced performance
    return AlgorithmType::BLOCK_DECOMPOSITION;
}

std::vector<AlgorithmType> RMQFactory::getAvailableAlgorithms() {
    return {
        AlgorithmType::NAIVE,
        AlgorithmType::DYNAMIC_PROGRAMMING,
        AlgorithmType::SPARSE_TABLE,
        AlgorithmType::BLOCK_DECOMPOSITION,
        AlgorithmType::LCA_BASED
    };
}

std::string RMQFactory::getAlgorithmDescription(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::NAIVE:
            return "Naive Linear Scan - O(n) query, O(1) preprocessing, supports updates";
            
        case AlgorithmType::DYNAMIC_PROGRAMMING:
            return "Dynamic Programming - O(1) query, O(n²) preprocessing and space";
            
        case AlgorithmType::SPARSE_TABLE:
            return "Sparse Table - O(1) query, O(n log n) preprocessing and space";
            
        case AlgorithmType::BLOCK_DECOMPOSITION:
            return "Block Decomposition - O(√n) query, O(n) preprocessing, supports updates";
            
        case AlgorithmType::LCA_BASED:
            return "LCA-based - O(log n) query, O(n) preprocessing";
            
        default:
            return "Unknown algorithm";
    }
}

bool RMQFactory::supportsFeature(AlgorithmType type, const std::string& feature) {
    if (feature == "update") {
        return type == AlgorithmType::NAIVE || 
               type == AlgorithmType::BLOCK_DECOMPOSITION;
    }
    
    if (feature == "O(1) query") {
        return type == AlgorithmType::DYNAMIC_PROGRAMMING || 
               type == AlgorithmType::SPARSE_TABLE;
    }
    
    if (feature == "O(n) space") {
        return type == AlgorithmType::NAIVE || 
               type == AlgorithmType::BLOCK_DECOMPOSITION;
    }
    
    if (feature == "O(1) preprocessing") {
        return type == AlgorithmType::NAIVE;
    }
    
    return false;
}

std::vector<RMQAlgorithmPtr> RMQFactory::createAll(const AlgorithmConfig& config) {
    std::vector<RMQAlgorithmPtr> algorithms;
    
    for (AlgorithmType type : getAvailableAlgorithms()) {
        try {
            algorithms.push_back(create(type, config));
        } catch (const std::exception&) {
            // Skip algorithms that can't be created
        }
    }
    
    return algorithms;
}

RMQFactory::BenchmarkRecommendation RMQFactory::getBenchmarkRecommendation(
    size_t array_size,
    size_t expected_queries) {
    
    BenchmarkRecommendation rec;
    
    // Calculate total expected time for each algorithm
    double naive_total = calculatePreprocessingTime(AlgorithmType::NAIVE, array_size) +
                        expected_queries * calculateQueryTime(AlgorithmType::NAIVE, array_size) / 1000.0;
    
    double dp_total = std::numeric_limits<double>::max();
    if (array_size <= 1000) {
        dp_total = calculatePreprocessingTime(AlgorithmType::DYNAMIC_PROGRAMMING, array_size) +
                  expected_queries * calculateQueryTime(AlgorithmType::DYNAMIC_PROGRAMMING, array_size) / 1000.0;
    }
    
    double sparse_total = calculatePreprocessingTime(AlgorithmType::SPARSE_TABLE, array_size) +
                         expected_queries * calculateQueryTime(AlgorithmType::SPARSE_TABLE, array_size) / 1000.0;
    
    double block_total = calculatePreprocessingTime(AlgorithmType::BLOCK_DECOMPOSITION, array_size) +
                        expected_queries * calculateQueryTime(AlgorithmType::BLOCK_DECOMPOSITION, array_size) / 1000.0;
    
    // Find the best algorithm
    double min_total = naive_total;
    rec.recommended_type = AlgorithmType::NAIVE;
    rec.reasoning = "Minimal preprocessing overhead";
    
    if (dp_total < min_total) {
        min_total = dp_total;
        rec.recommended_type = AlgorithmType::DYNAMIC_PROGRAMMING;
        rec.reasoning = "O(1) query time with acceptable space for small array";
    }
    
    if (sparse_total < min_total) {
        min_total = sparse_total;
        rec.recommended_type = AlgorithmType::SPARSE_TABLE;
        rec.reasoning = "O(1) query time optimal for many queries";
    }
    
    if (block_total < min_total) {
        min_total = block_total;
        rec.recommended_type = AlgorithmType::BLOCK_DECOMPOSITION;
        rec.reasoning = "Best balance between query time and space";
    }
    
    // Fill in expected metrics
    rec.expected_preprocessing_ms = calculatePreprocessingTime(rec.recommended_type, array_size);
    rec.expected_query_ms = calculateQueryTime(rec.recommended_type, array_size) / 1000.0;
    rec.expected_memory_bytes = calculateMemoryUsage(rec.recommended_type, array_size);
    
    return rec;
}

double RMQFactory::calculatePreprocessingTime(AlgorithmType type, size_t array_size) {
    // Rough estimates in milliseconds based on complexity
    const double CONSTANT_FACTOR = 0.000001;  // 1 microsecond per operation
    
    switch (type) {
        case AlgorithmType::NAIVE:
            return CONSTANT_FACTOR;  // O(1)
            
        case AlgorithmType::DYNAMIC_PROGRAMMING:
            return CONSTANT_FACTOR * array_size * array_size;  // O(n²)
            
        case AlgorithmType::SPARSE_TABLE:
            return CONSTANT_FACTOR * array_size * std::log2(array_size);  // O(n log n)
            
        case AlgorithmType::BLOCK_DECOMPOSITION:
            return CONSTANT_FACTOR * array_size;  // O(n)
            
        default:
            return 0;
    }
}

double RMQFactory::calculateQueryTime(AlgorithmType type, size_t array_size) {
    // Rough estimates in microseconds based on complexity
    const double CONSTANT_FACTOR = 0.001;  // 1 nanosecond per operation
    
    switch (type) {
        case AlgorithmType::NAIVE:
            return CONSTANT_FACTOR * array_size;  // O(n)
            
        case AlgorithmType::DYNAMIC_PROGRAMMING:
            return CONSTANT_FACTOR;  // O(1)
            
        case AlgorithmType::SPARSE_TABLE:
            return CONSTANT_FACTOR;  // O(1)
            
        case AlgorithmType::BLOCK_DECOMPOSITION:
            return CONSTANT_FACTOR * std::sqrt(array_size);  // O(√n)
            
        case AlgorithmType::LCA_BASED:
            return CONSTANT_FACTOR * std::log2(array_size);  // O(log n)
            
        default:
            return 0;
    }
}

size_t RMQFactory::calculateMemoryUsage(AlgorithmType type, size_t array_size) {
    // Estimates in bytes
    const size_t element_size = sizeof(Value);
    
    switch (type) {
        case AlgorithmType::NAIVE:
            return array_size * element_size;  // O(n)
            
        case AlgorithmType::DYNAMIC_PROGRAMMING:
            return array_size * array_size * element_size * 2;  // O(n²)
            
        case AlgorithmType::SPARSE_TABLE:
            return array_size * static_cast<size_t>(std::log2(array_size) + 1) * element_size * 2;  // O(n log n)
            
        case AlgorithmType::BLOCK_DECOMPOSITION:
            return array_size * element_size + 
                   static_cast<size_t>(std::sqrt(array_size)) * element_size * 2;  // O(n + √n)
            
        case AlgorithmType::LCA_BASED:
            return array_size * static_cast<size_t>(std::log2(array_size) + 1) * element_size * 2;  // O(n log n)
            
        default:
            return 0;
    }
}

} // namespace rmq