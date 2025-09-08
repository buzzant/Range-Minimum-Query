#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>

// Include all implementations
#include "include/factory/rmq_factory.h"
#include "include/algorithms/rmq_naive.h"
#include "include/algorithms/rmq_dp.h"
#include "include/algorithms/rmq_sparse_table.h"
#include "include/algorithms/rmq_block.h"
#include "include/algorithms/rmq_lca.h"

// Include source files
#include "src/core/rmq_base.cpp"
#include "src/algorithms/rmq_naive.cpp"
#include "src/algorithms/rmq_dp.cpp"
#include "src/algorithms/rmq_sparse_table.cpp"
#include "src/algorithms/rmq_block.cpp"
#include "src/algorithms/rmq_lca.cpp"
#include "src/factory/rmq_factory.cpp"

using namespace rmq;
using namespace std::chrono;

/**
 * @brief Benchmark results for a single algorithm
 */
struct BenchmarkResult {
    std::string algorithm_name;
    size_t array_size;
    double preprocessing_ms;
    double query_us;  // microseconds per query
    double total_queries_ms;
    size_t memory_bytes;
    size_t num_queries;
};

/**
 * @brief Benchmark suite for RMQ algorithms
 */
class RMQBenchmark {
private:
    std::vector<size_t> test_sizes_;
    std::vector<BenchmarkResult> results_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> value_dist_;
    
    static constexpr size_t QUERIES_PER_SIZE = 10000;
    static constexpr size_t WARMUP_QUERIES = 100;
    
public:
    RMQBenchmark() : gen_(42), value_dist_(-10000, 10000) {  // Fixed seed for reproducibility
        // Test sizes: exponentially growing
        for (size_t size = 10; size <= 100000; size *= 2) {
            test_sizes_.push_back(size);
        }
        test_sizes_.push_back(50000);
        test_sizes_.push_back(100000);
        std::sort(test_sizes_.begin(), test_sizes_.end());
    }
    
    /**
     * @brief Generate random test data
     */
    std::vector<Value> generateData(size_t size) {
        std::vector<Value> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = value_dist_(gen_);
        }
        return data;
    }
    
    /**
     * @brief Generate random query pairs
     */
    std::vector<std::pair<Index, Index>> generateQueries(size_t array_size, size_t num_queries) {
        std::vector<std::pair<Index, Index>> queries;
        std::uniform_int_distribution<size_t> index_dist(0, array_size - 1);
        
        for (size_t i = 0; i < num_queries; ++i) {
            size_t left = index_dist(gen_);
            size_t right = index_dist(gen_);
            if (left > right) std::swap(left, right);
            queries.push_back({left, right});
        }
        
        return queries;
    }
    
    /**
     * @brief Benchmark a single algorithm
     */
    BenchmarkResult benchmarkAlgorithm(
        AlgorithmType type,
        const std::vector<Value>& data,
        const std::vector<std::pair<Index, Index>>& queries) {
        
        BenchmarkResult result;
        result.array_size = data.size();
        result.num_queries = queries.size();
        
        try {
            // Create algorithm instance
            auto algorithm = RMQFactory::create(type);
            result.algorithm_name = algorithm->getName();
            
            // Skip DP for large arrays
            if (type == AlgorithmType::DYNAMIC_PROGRAMMING && data.size() > 2000) {
                result.preprocessing_ms = -1;  // Indicate skipped
                result.query_us = -1;
                result.total_queries_ms = -1;
                result.memory_bytes = 0;
                return result;
            }
            
            // Measure preprocessing time
            auto start = high_resolution_clock::now();
            algorithm->preprocess(data);
            auto end = high_resolution_clock::now();
            
            result.preprocessing_ms = duration_cast<duration<double, std::milli>>(end - start).count();
            
            // Warmup queries (to stabilize cache)
            for (size_t i = 0; i < std::min(WARMUP_QUERIES, queries.size()); ++i) {
                algorithm->query(queries[i].first, queries[i].second);
            }
            
            // Measure query time
            start = high_resolution_clock::now();
            for (const auto& [left, right] : queries) {
                volatile Value v = algorithm->query(left, right);  // volatile to prevent optimization
                (void)v;
            }
            end = high_resolution_clock::now();
            
            result.total_queries_ms = duration_cast<duration<double, std::milli>>(end - start).count();
            result.query_us = (result.total_queries_ms * 1000.0) / queries.size();
            
            // Estimate memory usage
            result.memory_bytes = RMQFactory::calculateMemoryUsage(type, data.size());
            
        } catch (const std::exception& e) {
            std::cerr << "Error benchmarking " << algorithmTypeToString(type) 
                     << " with size " << data.size() << ": " << e.what() << std::endl;
            result.preprocessing_ms = -1;
            result.query_us = -1;
            result.total_queries_ms = -1;
            result.memory_bytes = 0;
        }
        
        return result;
    }
    
    /**
     * @brief Run benchmarks for all algorithms and sizes
     */
    void runBenchmarks() {
        std::cout << "Running RMQ Algorithm Complexity Benchmarks..." << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;
        
        for (size_t size : test_sizes_) {
            std::cout << "Testing with array size: " << size << std::endl;
            
            // Generate test data
            auto data = generateData(size);
            
            // Adjust number of queries based on array size
            size_t num_queries = std::min(QUERIES_PER_SIZE, size * 10);
            auto queries = generateQueries(size, num_queries);
            
            // Test each algorithm
            for (AlgorithmType type : RMQFactory::getAvailableAlgorithms()) {
                std::cout << "  - Benchmarking " << algorithmTypeToString(type) << "... ";
                
                auto result = benchmarkAlgorithm(type, data, queries);
                
                if (result.preprocessing_ms >= 0) {
                    results_.push_back(result);
                    std::cout << "Done (prep: " << std::fixed << std::setprecision(3) 
                             << result.preprocessing_ms << "ms, "
                             << "query: " << result.query_us << "μs)" << std::endl;
                } else {
                    std::cout << "Skipped (too large or error)" << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }
    
    /**
     * @brief Write results to CSV files for graphing
     */
    void writeResults() {
        // Write preprocessing time results
        std::ofstream prep_file("benchmark_preprocessing.csv");
        prep_file << "Algorithm,ArraySize,PreprocessingTime_ms,TheoreticalComplexity" << std::endl;
        
        // Write query time results
        std::ofstream query_file("benchmark_query.csv");
        query_file << "Algorithm,ArraySize,QueryTime_us,TheoreticalComplexity" << std::endl;
        
        // Write memory usage results
        std::ofstream memory_file("benchmark_memory.csv");
        memory_file << "Algorithm,ArraySize,Memory_MB,TheoreticalComplexity" << std::endl;
        
        for (const auto& result : results_) {
            if (result.preprocessing_ms >= 0) {
                // Preprocessing time
                prep_file << result.algorithm_name << ","
                         << result.array_size << ","
                         << result.preprocessing_ms << ","
                         << getPreprocessingComplexity(result.algorithm_name) << std::endl;
                
                // Query time
                query_file << result.algorithm_name << ","
                          << result.array_size << ","
                          << result.query_us << ","
                          << getQueryComplexity(result.algorithm_name) << std::endl;
                
                // Memory usage
                double memory_mb = result.memory_bytes / (1024.0 * 1024.0);
                memory_file << result.algorithm_name << ","
                           << result.array_size << ","
                           << memory_mb << ","
                           << getSpaceComplexity(result.algorithm_name) << std::endl;
            }
        }
        
        prep_file.close();
        query_file.close();
        memory_file.close();
        
        std::cout << "Results written to CSV files:" << std::endl;
        std::cout << "  - benchmark_preprocessing.csv" << std::endl;
        std::cout << "  - benchmark_query.csv" << std::endl;
        std::cout << "  - benchmark_memory.csv" << std::endl;
    }
    
    /**
     * @brief Print summary table
     */
    void printSummary() {
        std::cout << "\nSummary Table:" << std::endl;
        std::cout << std::string(120, '=') << std::endl;
        std::cout << std::left << std::setw(25) << "Algorithm" 
                  << std::setw(12) << "Size"
                  << std::setw(15) << "Prep (ms)"
                  << std::setw(15) << "Query (μs)"
                  << std::setw(15) << "Memory (MB)"
                  << std::setw(20) << "Prep Complex"
                  << std::setw(20) << "Query Complex" << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        
        for (const auto& result : results_) {
            if (result.preprocessing_ms >= 0) {
                std::cout << std::left << std::setw(25) << result.algorithm_name
                         << std::setw(12) << result.array_size
                         << std::setw(15) << std::fixed << std::setprecision(3) << result.preprocessing_ms
                         << std::setw(15) << std::fixed << std::setprecision(3) << result.query_us
                         << std::setw(15) << std::fixed << std::setprecision(2) 
                         << (result.memory_bytes / (1024.0 * 1024.0))
                         << std::setw(20) << getPreprocessingComplexity(result.algorithm_name)
                         << std::setw(20) << getQueryComplexity(result.algorithm_name) << std::endl;
            }
        }
        std::cout << std::string(120, '=') << std::endl;
    }
    
private:
    std::string getPreprocessingComplexity(const std::string& algorithm) {
        if (algorithm.find("Naive") != std::string::npos) return "O(1)";
        if (algorithm.find("Dynamic Programming") != std::string::npos) return "O(n²)";
        if (algorithm.find("Sparse Table") != std::string::npos) return "O(n log n)";
        if (algorithm.find("Block") != std::string::npos) return "O(n)";
        if (algorithm.find("LCA") != std::string::npos) return "O(n log n)";
        return "Unknown";
    }
    
    std::string getQueryComplexity(const std::string& algorithm) {
        if (algorithm.find("Naive") != std::string::npos) return "O(n)";
        if (algorithm.find("Dynamic Programming") != std::string::npos) return "O(1)";
        if (algorithm.find("Sparse Table") != std::string::npos) return "O(1)";
        if (algorithm.find("Block") != std::string::npos) return "O(√n)";
        if (algorithm.find("LCA") != std::string::npos) return "O(log n)";
        return "Unknown";
    }
    
    std::string getSpaceComplexity(const std::string& algorithm) {
        if (algorithm.find("Naive") != std::string::npos) return "O(n)";
        if (algorithm.find("Dynamic Programming") != std::string::npos) return "O(n²)";
        if (algorithm.find("Sparse Table") != std::string::npos) return "O(n log n)";
        if (algorithm.find("Block") != std::string::npos) return "O(n + √n)";
        if (algorithm.find("LCA") != std::string::npos) return "O(n log n)";
        return "Unknown";
    }
};

int main() {
    RMQBenchmark benchmark;
    
    // Run benchmarks
    benchmark.runBenchmarks();
    
    // Write results to CSV
    benchmark.writeResults();
    
    // Print summary
    benchmark.printSummary();
    
    std::cout << "\nBenchmark complete! CSV files generated for graphing." << std::endl;
    std::cout << "Use the Python visualization script to generate graphs." << std::endl;
    
    return 0;
}