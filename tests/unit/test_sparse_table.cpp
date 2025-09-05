#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <chrono>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <tuple>
#include "../../include/algorithms/rmq_sparse_table.h"
#include "../../include/algorithms/rmq_naive.h"
#include "../../src/core/rmq_base.cpp"
#include "../../src/algorithms/rmq_sparse_table.cpp"
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

class RMQSparseTableTest {
private:
    std::unique_ptr<RMQSparseTable> rmq_;
    
public:
    RMQSparseTableTest() : rmq_(std::make_unique<RMQSparseTable>()) {}
    
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
        assert(rmq_->getLevels() == 1);  // Only one level needed
        
        QueryResult result = rmq_->queryDetailed(0, 0);
        assert(result.minimum_value == 42);
        assert(result.minimum_index == 0);
    }
    
    void testPowerOfTwoSizes() {
        // Test with various power-of-2 sizes
        for (int power = 1; power <= 10; ++power) {
            size_t size = 1 << power;  // 2, 4, 8, 16, ..., 1024
            std::vector<Value> data(size);
            for (size_t i = 0; i < size; ++i) {
                data[i] = static_cast<Value>(size - i);  // Decreasing sequence
            }
            
            rmq_->preprocess(data);
            
            // Minimum should always be 1 (last element)
            assert(rmq_->query(0, size - 1) == 1);
            
            // Expected levels: log2(size) + 1
            assert(rmq_->getLevels() == static_cast<size_t>(power + 1));
        }
    }
    
    void testTableVerification() {
        std::vector<Value> data = {5, 2, 8, 1, 9, 3, 7, 4};
        rmq_->preprocess(data);
        
        assert(rmq_->verifyTable() == true);
        
        // Test that table stats are reasonable
        auto [levels, entries, memory] = rmq_->getTableStats();
        assert(levels == 4);  // log2(8) + 1
        assert(entries > 0 && entries <= 8 * 4);  // At most n * log(n) entries
        assert(memory > 0);
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
    
    void testOverlappingRanges() {
        // Test the overlapping power-of-2 range technique
        std::vector<Value> data = {9, 3, 7, 1, 8, 2, 5, 4, 6};
        rmq_->preprocess(data);
        
        // Test various range sizes
        assert(rmq_->query(0, 4) == 1);  // Range of 5
        assert(rmq_->query(2, 5) == 1);  // Range of 4
        assert(rmq_->query(1, 7) == 1);  // Range of 7
        assert(rmq_->query(5, 8) == 2);  // Range of 4
    }
    
    void testCompareWithNaive() {
        // Compare results with naive implementation
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
            
            Value sparse_result = rmq_->query(left, right);
            Value naive_result = naive.query(left, right);
            
            assert(sparse_result == naive_result);
        }
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
        
        // Verify levels: log2(10000) + 1 ≈ 14
        assert(rmq_->getLevels() <= 15);
        
        // Test random queries
        for (int i = 0; i < 1000; ++i) {
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
    
    void testO1QueryTime() {
        // Verify O(1) query time
        const size_t size = 10000;
        std::vector<Value> data(size, 1);
        rmq_->preprocess(data);
        
        // Time many queries
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; ++i) {
            rmq_->query(0, size - 1);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Each query should be very fast
        double avg_query_time = duration.count() / 100000.0;
        assert(avg_query_time < 10);  // Less than 10 microseconds per query
    }
    
    void testMemoryUsage() {
        std::vector<Value> data(1000, 42);
        rmq_->preprocess(data);
        
        size_t memory = rmq_->getMemoryUsage();
        size_t entries = rmq_->getTableEntries();
        
        // Memory should be roughly n * log(n) * sizeof(Value)
        assert(memory > 1000 * sizeof(Value));  // At least n values
        assert(memory < 1000 * 20 * sizeof(Value) * 4);  // But reasonable (accounting for both value and index tables)
        
        // Entries should be approximately n * log(n)
        assert(entries > 1000);
        assert(entries < 1000 * 20);
    }
    
    void testComplexityInfo() {
        ComplexityInfo info = rmq_->getComplexity();
        
        assert(info.preprocessing_time == "O(n log n)");
        assert(info.preprocessing_space == "O(n log n)");
        assert(info.query_time == "O(1)");
        assert(info.query_space == "O(1)");
        assert(info.total_space == "O(n log n)");
    }
    
    void testNoUpdateSupport() {
        assert(rmq_->supportsUpdate() == false);
    }
    
    void testClearFunction() {
        std::vector<Value> data = {1, 2, 3, 4, 5};
        rmq_->preprocess(data);
        
        assert(rmq_->isPreprocessed() == true);
        assert(rmq_->getTableEntries() > 0);
        
        rmq_->clear();
        
        assert(rmq_->isPreprocessed() == false);
        assert(rmq_->getTableEntries() == 0);
        assert(rmq_->getLevels() == 0);
    }
    
    void testEdgeCases() {
        // Test with all same values
        std::vector<Value> data(100, 5);
        rmq_->preprocess(data);
        assert(rmq_->query(10, 90) == 5);
        
        // Test with strictly increasing
        data.clear();
        for (int i = 0; i < 100; ++i) {
            data.push_back(i);
        }
        rmq_->preprocess(data);
        assert(rmq_->query(50, 99) == 50);
        
        // Test with strictly decreasing
        data.clear();
        for (int i = 100; i > 0; --i) {
            data.push_back(i);
        }
        rmq_->preprocess(data);
        assert(rmq_->query(0, 50) == 50);  // Value at index 50 is 50
    }
    
    void runAllTests(TestRunner& runner) {
        runner.runTest("Basic Functionality", [this]() { testBasicFunctionality(); });
        runner.runTest("Single Element", [this]() { testSingleElement(); });
        runner.runTest("Power of Two Sizes", [this]() { testPowerOfTwoSizes(); });
        runner.runTest("Table Verification", [this]() { testTableVerification(); });
        runner.runTest("Minimum Index Tracking", [this]() { testMinimumIndexTracking(); });
        runner.runTest("Overlapping Ranges", [this]() { testOverlappingRanges(); });
        runner.runTest("Compare With Naive", [this]() { testCompareWithNaive(); });
        runner.runTest("Large Dataset", [this]() { testLargeDataset(); });
        runner.runTest("O(1) Query Time", [this]() { testO1QueryTime(); });
        runner.runTest("Memory Usage", [this]() { testMemoryUsage(); });
        runner.runTest("Complexity Info", [this]() { testComplexityInfo(); });
        runner.runTest("No Update Support", [this]() { testNoUpdateSupport(); });
        runner.runTest("Clear Function", [this]() { testClearFunction(); });
        runner.runTest("Edge Cases", [this]() { testEdgeCases(); });
    }
};

int main() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "RMQ Sparse Table Implementation Unit Tests" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;
    
    TestRunner runner;
    RMQSparseTableTest test;
    test.runAllTests(runner);
    
    runner.printSummary();
    
    return runner.allTestsPassed() ? 0 : 1;
}