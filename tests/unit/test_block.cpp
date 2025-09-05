#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <chrono>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <tuple>
#include <cmath>
#include "../../include/algorithms/rmq_block.h"
#include "../../include/algorithms/rmq_naive.h"
#include "../../src/core/rmq_base.cpp"
#include "../../src/algorithms/rmq_block.cpp"
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

class RMQBlockTest {
private:
    std::unique_ptr<RMQBlockDecomposition> rmq_;
    
public:
    RMQBlockTest() : rmq_(std::make_unique<RMQBlockDecomposition>()) {}
    
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
        assert(rmq_->getNumBlocks() == 1);
        
        QueryResult result = rmq_->queryDetailed(0, 0);
        assert(result.minimum_value == 42);
        assert(result.minimum_index == 0);
    }
    
    void testBlockStructure() {
        // Test with perfect square size
        std::vector<Value> data(100);
        for (int i = 0; i < 100; ++i) {
            data[i] = 100 - i;  // Decreasing sequence
        }
        
        rmq_->preprocess(data);
        
        // Block size should be approximately sqrt(100) = 10
        size_t block_size = rmq_->getBlockSize();
        assert(block_size >= 10 && block_size <= 11);
        
        // Number of blocks
        size_t num_blocks = rmq_->getNumBlocks();
        assert(num_blocks >= 9 && num_blocks <= 11);
        
        // Test queries
        assert(rmq_->query(0, 99) == 1);  // Minimum is at end
        assert(rmq_->query(0, 0) == 100);  // First element
    }
    
    void testCustomBlockSize() {
        AlgorithmConfig config;
        config.withBlockSize(5);  // Custom block size of 5
        
        RMQBlockDecomposition custom_rmq(config);
        std::vector<Value> data = {9, 3, 7, 1, 8, 2, 5, 4, 6, 10};
        custom_rmq.preprocess(data);
        
        assert(custom_rmq.getBlockSize() == 5);
        assert(custom_rmq.getNumBlocks() == 2);  // 10 elements / 5 = 2 blocks
        
        // Test queries
        assert(custom_rmq.query(0, 9) == 1);
        assert(custom_rmq.query(5, 9) == 2);
    }
    
    void testSingleUpdate() {
        std::vector<Value> data = {5, 2, 8, 1, 9, 3};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 5) == 1);  // Minimum is 1 at index 3
        
        // Update minimum to a larger value
        rmq_->update(3, 10);
        assert(rmq_->query(0, 5) == 2);  // New minimum is 2 at index 1
        
        // Update to create new minimum
        rmq_->update(4, -1);
        assert(rmq_->query(0, 5) == -1);  // New minimum is -1 at index 4
    }
    
    void testBatchUpdate() {
        std::vector<Value> data = {5, 4, 3, 2, 1};
        rmq_->preprocess(data);
        
        std::vector<std::pair<Index, Value>> updates = {
            {0, 10}, {2, 15}, {4, 20}
        };
        
        rmq_->batchUpdate(updates);
        
        assert(rmq_->query(0, 4) == 2);   // min(10, 4, 15, 2, 20) = 2
        assert(rmq_->query(0, 0) == 10);
        assert(rmq_->query(4, 4) == 20);
    }
    
    void testRebuildBlocks() {
        std::vector<Value> data = {1, 2, 3, 4, 5, 6, 7, 8};
        rmq_->preprocess(data);
        
        // Make many direct changes to data (simulating external modifications)
        // Note: In production, this would be through proper update methods
        rmq_->update(0, 10);
        rmq_->update(2, 20);
        rmq_->update(4, 30);
        
        // Rebuild blocks to ensure consistency
        rmq_->rebuildBlocks();
        
        assert(rmq_->query(0, 7) == 2);  // Minimum should be 2 at index 1
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
            
            Value block_result = rmq_->query(left, right);
            Value naive_result = naive.query(left, right);
            
            assert(block_result == naive_result);
        }
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
    
    void testLargeDataset() {
        const size_t size = 10000;
        std::vector<Value> data(size);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-10000, 10000);
        
        for (size_t i = 0; i < size; ++i) {
            data[i] = dis(gen);
        }
        
        rmq_->preprocess(data);
        
        // Block size should be approximately sqrt(10000) = 100
        size_t block_size = rmq_->getBlockSize();
        assert(block_size >= 99 && block_size <= 101);
        
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
        
        assert(info.preprocessing_time == "O(n)");
        assert(info.preprocessing_space == "O(√n)");
        assert(info.query_time == "O(√n)");
        assert(info.query_space == "O(1)");
        assert(info.total_space == "O(n + √n)");
    }
    
    void testUpdateSupport() {
        assert(rmq_->supportsUpdate() == true);
    }
    
    void testMemoryUsage() {
        std::vector<Value> data(1000, 42);
        rmq_->preprocess(data);
        
        size_t memory = rmq_->getMemoryUsage();
        
        // Memory should include data + block arrays (sqrt(n) blocks)
        // Plus overhead for vector metadata and object size
        assert(memory > 1000 * sizeof(Value));  // At least the data
        assert(memory < 100000);  // But reasonable overall
    }
    
    void testBlockStats() {
        std::vector<Value> data(100, 1);
        rmq_->preprocess(data);
        
        auto [block_size, num_blocks, memory] = rmq_->getBlockStats();
        
        assert(block_size > 0);
        assert(num_blocks > 0);
        assert(memory > 0);
        assert(block_size * num_blocks >= 100);  // Should cover all elements
    }
    
    void testClearFunction() {
        std::vector<Value> data = {1, 2, 3, 4, 5};
        rmq_->preprocess(data);
        
        assert(rmq_->isPreprocessed() == true);
        assert(rmq_->getNumBlocks() > 0);
        
        rmq_->clear();
        
        assert(rmq_->isPreprocessed() == false);
        assert(rmq_->getNumBlocks() == 0);
        assert(rmq_->getBlockSize() == 0);
    }
    
    void testUpdatePerformance() {
        // Test that updates are indeed O(1) + O(√n) for block rebuild
        const size_t size = 10000;
        std::vector<Value> data(size, 1);
        rmq_->preprocess(data);
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000; ++i) {
            rmq_->update(i % size, i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // 1000 updates should be fast
        assert(duration.count() < 100);  // Less than 100ms for 1000 updates
    }
    
    void runAllTests(TestRunner& runner) {
        runner.runTest("Basic Functionality", [this]() { testBasicFunctionality(); });
        runner.runTest("Single Element", [this]() { testSingleElement(); });
        runner.runTest("Block Structure", [this]() { testBlockStructure(); });
        runner.runTest("Custom Block Size", [this]() { testCustomBlockSize(); });
        runner.runTest("Single Update", [this]() { testSingleUpdate(); });
        runner.runTest("Batch Update", [this]() { testBatchUpdate(); });
        runner.runTest("Rebuild Blocks", [this]() { testRebuildBlocks(); });
        runner.runTest("Compare With Naive", [this]() { testCompareWithNaive(); });
        runner.runTest("Minimum Index Tracking", [this]() { testMinimumIndexTracking(); });
        runner.runTest("Large Dataset", [this]() { testLargeDataset(); });
        runner.runTest("Complexity Info", [this]() { testComplexityInfo(); });
        runner.runTest("Update Support", [this]() { testUpdateSupport(); });
        runner.runTest("Memory Usage", [this]() { testMemoryUsage(); });
        runner.runTest("Block Stats", [this]() { testBlockStats(); });
        runner.runTest("Clear Function", [this]() { testClearFunction(); });
        runner.runTest("Update Performance", [this]() { testUpdatePerformance(); });
    }
};

int main() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "RMQ Block Decomposition Implementation Unit Tests" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;
    
    TestRunner runner;
    RMQBlockTest test;
    test.runAllTests(runner);
    
    runner.printSummary();
    
    return runner.allTestsPassed() ? 0 : 1;
}