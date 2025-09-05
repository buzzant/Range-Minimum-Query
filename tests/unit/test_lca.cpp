#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <chrono>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <tuple>
#include "../../include/algorithms/rmq_lca.h"
#include "../../include/algorithms/rmq_naive.h"
#include "../../src/core/rmq_base.cpp"
#include "../../src/algorithms/rmq_lca.cpp"
#include "../../src/algorithms/rmq_naive.cpp"

using namespace rmq;

class TestRunner {
private:
    int tests_passed_;
    int tests_failed_;
    std::vector<std::string> failed_tests_;
    
public:
    TestRunner() : tests_passed_(0), tests_failed_(0) {}
    
    void runTest(const std::string& test_name, std::function<void()> test_func) {
        std::cout << "Running: " << std::left << std::setw(50) << test_name << " ";
        try {
            test_func();
            std::cout << "[PASSED]" << std::endl;
            tests_passed_++;
        } catch (const std::exception& e) {
            std::cout << "[FAILED] - " << e.what() << std::endl;
            tests_failed_++;
            failed_tests_.push_back(test_name);
        } catch (...) {
            std::cout << "[FAILED] - Unknown error" << std::endl;
            tests_failed_++;
            failed_tests_.push_back(test_name);
        }
    }
    
    void printSummary() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Test Summary:" << std::endl;
        std::cout << "Passed: " << tests_passed_ << std::endl;
        std::cout << "Failed: " << tests_failed_ << std::endl;
        
        if (!failed_tests_.empty()) {
            std::cout << "\nFailed tests:" << std::endl;
            for (const auto& test : failed_tests_) {
                std::cout << "  - " << test << std::endl;
            }
        }
        std::cout << std::string(70, '=') << std::endl;
    }
    
    bool allTestsPassed() const {
        return tests_failed_ == 0;
    }
};

class RMQLCATest {
private:
    std::unique_ptr<RMQLCABased> rmq_;
    
public:
    RMQLCATest() : rmq_(std::make_unique<RMQLCABased>()) {}
    
    void testBasicFunctionality() {
        std::vector<Value> data = {3, 1, 4, 1, 5, 9, 2, 6};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 2) == 1);  // min(3, 1, 4) = 1
        assert(rmq_->query(2, 4) == 1);  // min(4, 1, 5) = 1
        assert(rmq_->query(4, 7) == 2);  // min(5, 9, 2, 6) = 2
        assert(rmq_->query(0, 7) == 1);  // min of all = 1
    }
    
    void testSingleElement() {
        std::vector<Value> data = {42};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 0) == 42);
        assert(rmq_->getTreeSize() == 1);
        
        QueryResult result = rmq_->queryDetailed(0, 0);
        assert(result.minimum_value == 42);
        assert(result.minimum_index == 0);
    }
    
    void testCartesianTreeStructure() {
        // Test with a specific sequence to verify tree structure
        std::vector<Value> data = {3, 1, 4, 1, 5};
        rmq_->preprocess(data);
        
        // The Cartesian tree should have min element (1 at index 1) near root
        assert(rmq_->verifyTree() == true);
        assert(rmq_->getTreeSize() == 5);
        
        // Test various queries
        assert(rmq_->query(0, 4) == 1);  // Global minimum
        assert(rmq_->query(2, 4) == 1);  // Minimum at index 3
        assert(rmq_->query(2, 2) == 4);  // Single element
    }
    
    void testIncreasingSequence() {
        std::vector<Value> data = {1, 2, 3, 4, 5, 6, 7, 8};
        rmq_->preprocess(data);
        
        // For increasing sequence, tree should be left-skewed
        assert(rmq_->verifyTree() == true);
        
        assert(rmq_->query(0, 7) == 1);  // First element
        assert(rmq_->query(3, 7) == 4);  // Starting from index 3
        assert(rmq_->query(5, 6) == 6);  // min(6, 7) = 6
    }
    
    void testDecreasingSequence() {
        std::vector<Value> data = {8, 7, 6, 5, 4, 3, 2, 1};
        rmq_->preprocess(data);
        
        // For decreasing sequence, tree should be right-skewed
        assert(rmq_->verifyTree() == true);
        
        assert(rmq_->query(0, 7) == 1);  // Last element
        assert(rmq_->query(0, 3) == 5);  // min(8, 7, 6, 5) = 5
        assert(rmq_->query(6, 7) == 1);  // min(2, 1) = 1
    }
    
    void testMinimumIndexTracking() {
        std::vector<Value> data = {7, 2, 5, 2, 9, 1, 3};
        rmq_->preprocess(data);
        
        QueryResult result = rmq_->queryDetailed(0, 3);
        assert(result.minimum_value == 2);
        assert(result.minimum_index == 1);  // First occurrence
        
        result = rmq_->queryDetailed(2, 6);
        assert(result.minimum_value == 1);
        assert(result.minimum_index == 5);
    }
    
    void testTreeDepth() {
        // Test with various array sizes
        for (int size = 10; size <= 100; size += 10) {
            std::vector<Value> data(size);
            for (int i = 0; i < size; ++i) {
                data[i] = i;  // Increasing sequence
            }
            
            rmq_->preprocess(data);
            
            int depth = rmq_->getTreeDepth();
            // For increasing sequence, depth should be n-1
            assert(depth == size - 1);
        }
    }
    
    void testCompareWithNaive() {
        const size_t size = 100;
        std::vector<Value> data(size);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-1000, 1000);
        
        for (size_t i = 0; i < size; ++i) {
            data[i] = dis(gen);
        }
        
        rmq_->preprocess(data);
        
        RMQNaive naive;
        naive.preprocess(data);
        
        // Test random queries
        for (int i = 0; i < 100; ++i) {
            size_t left = dis(gen) % size;
            size_t right = left + (dis(gen) % (size - left));
            
            Value lca_result = rmq_->query(left, right);
            Value naive_result = naive.query(left, right);
            
            assert(lca_result == naive_result);
        }
    }
    
    void testLargeDataset() {
        const size_t size = 1000;
        std::vector<Value> data(size);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-10000, 10000);
        
        for (size_t i = 0; i < size; ++i) {
            data[i] = dis(gen);
        }
        
        rmq_->preprocess(data);
        
        assert(rmq_->verifyTree() == true);
        assert(rmq_->getTreeSize() == size);
        
        // Test random queries
        for (int i = 0; i < 100; ++i) {
            size_t left = dis(gen) % size;
            size_t right = left + (dis(gen) % (size - left));
            
            Value result = rmq_->query(left, right);
            
            // Verify with brute force
            Value expected = data[left];
            for (size_t j = left + 1; j <= right; ++j) {
                expected = std::min(expected, data[j]);
            }
            
            assert(result == expected);
        }
    }
    
    void testComplexityInfo() {
        ComplexityInfo info = rmq_->getComplexity();
        
        assert(info.preprocessing_time == "O(n log n)");
        assert(info.preprocessing_space == "O(n log n)");
        assert(info.query_time == "O(log n)");
        assert(info.query_space == "O(1)");
        assert(info.total_space == "O(n log n)");
    }
    
    void testNoUpdateSupport() {
        assert(rmq_->supportsUpdate() == false);
    }
    
    void testTreeStats() {
        std::vector<Value> data = {5, 2, 8, 1, 9, 3, 7, 4};
        rmq_->preprocess(data);
        
        auto [num_nodes, tree_depth, memory] = rmq_->getTreeStats();
        
        assert(num_nodes == 8);
        assert(tree_depth >= 0);  // Depth depends on structure
        assert(memory > 0);
    }
    
    void testMemoryUsage() {
        std::vector<Value> data(100, 42);
        rmq_->preprocess(data);
        
        size_t memory = rmq_->getMemoryUsage();
        
        // Memory should include tree nodes + ancestors table
        assert(memory > 100 * sizeof(Value));  // At least the data
        assert(memory < 100000);  // But reasonable
    }
    
    void testClearFunction() {
        std::vector<Value> data = {1, 2, 3, 4, 5};
        rmq_->preprocess(data);
        
        assert(rmq_->isPreprocessed() == true);
        assert(rmq_->getTreeSize() > 0);
        
        rmq_->clear();
        
        assert(rmq_->isPreprocessed() == false);
        assert(rmq_->getTreeSize() == 0);
    }
    
    void testSpecialPatterns() {
        // Test with all same values
        std::vector<Value> data(50, 5);
        rmq_->preprocess(data);
        assert(rmq_->query(10, 40) == 5);
        
        // Test with alternating high-low pattern
        data.clear();
        for (int i = 0; i < 50; ++i) {
            data.push_back(i % 2 == 0 ? 100 : 1);
        }
        rmq_->preprocess(data);
        assert(rmq_->query(0, 49) == 1);
        assert(rmq_->query(0, 2) == 1);  // 100, 1, 100 -> 1
        
        // Test with V-shaped pattern (decreasing then increasing)
        data = {10, 8, 6, 4, 2, 1, 3, 5, 7, 9};
        rmq_->preprocess(data);
        assert(rmq_->query(0, 9) == 1);  // Global minimum at index 5
        assert(rmq_->query(0, 4) == 2);  // Left half minimum
        assert(rmq_->query(6, 9) == 3);  // Right half minimum
    }
    
    void testQueryPerformance() {
        // Test that queries are O(log n)
        const size_t size = 10000;
        std::vector<Value> data(size, 1);
        rmq_->preprocess(data);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 10000; ++i) {
            rmq_->query(0, size - 1);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Each query should be reasonably fast (log n complexity)
        double avg_query_time = duration.count() / 10000.0;
        assert(avg_query_time < 100);  // Less than 100 microseconds per query
    }
    
    void runAllTests(TestRunner& runner) {
        runner.runTest("Basic Functionality", [this]() { testBasicFunctionality(); });
        runner.runTest("Single Element", [this]() { testSingleElement(); });
        runner.runTest("Cartesian Tree Structure", [this]() { testCartesianTreeStructure(); });
        runner.runTest("Increasing Sequence", [this]() { testIncreasingSequence(); });
        runner.runTest("Decreasing Sequence", [this]() { testDecreasingSequence(); });
        runner.runTest("Minimum Index Tracking", [this]() { testMinimumIndexTracking(); });
        runner.runTest("Tree Depth", [this]() { testTreeDepth(); });
        runner.runTest("Compare With Naive", [this]() { testCompareWithNaive(); });
        runner.runTest("Large Dataset", [this]() { testLargeDataset(); });
        runner.runTest("Complexity Info", [this]() { testComplexityInfo(); });
        runner.runTest("No Update Support", [this]() { testNoUpdateSupport(); });
        runner.runTest("Tree Stats", [this]() { testTreeStats(); });
        runner.runTest("Memory Usage", [this]() { testMemoryUsage(); });
        runner.runTest("Clear Function", [this]() { testClearFunction(); });
        runner.runTest("Special Patterns", [this]() { testSpecialPatterns(); });
        runner.runTest("Query Performance", [this]() { testQueryPerformance(); });
    }
};

int main() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "RMQ LCA-based Implementation Unit Tests" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;
    
    TestRunner runner;
    RMQLCATest test;
    test.runAllTests(runner);
    
    runner.printSummary();
    
    return runner.allTestsPassed() ? 0 : 1;
}