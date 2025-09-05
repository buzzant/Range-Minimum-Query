#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <chrono>
#include <iomanip>
#include <functional>
#include <algorithm>
#include "../../include/algorithms/rmq_naive.h"
#include "../../src/core/rmq_base.cpp"
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

// Test fixture for RMQ tests
class RMQNaiveTest {
private:
    std::unique_ptr<RMQNaive> rmq_;
    
public:
    RMQNaiveTest() : rmq_(std::make_unique<RMQNaive>()) {}
    
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
        
        QueryResult result = rmq_->queryDetailed(0, 0);
        assert(result.minimum_value == 42);
        assert(result.minimum_index == 0);
    }
    
    void testIdenticalElements() {
        std::vector<Value> data = {5, 5, 5, 5, 5};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 4) == 5);
        assert(rmq_->query(1, 3) == 5);
        assert(rmq_->query(2, 2) == 5);
    }
    
    void testIncreasingSequence() {
        std::vector<Value> data = {1, 2, 3, 4, 5, 6, 7, 8};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 7) == 1);  // First element
        assert(rmq_->query(3, 7) == 4);  // Starting from index 3
        assert(rmq_->query(5, 6) == 6);  // min(6, 7) = 6
    }
    
    void testDecreasingSequence() {
        std::vector<Value> data = {8, 7, 6, 5, 4, 3, 2, 1};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 7) == 1);  // Last element
        assert(rmq_->query(0, 3) == 5);  // min(8, 7, 6, 5) = 5
        assert(rmq_->query(6, 7) == 1);  // min(2, 1) = 1
    }
    
    void testNegativeNumbers() {
        std::vector<Value> data = {3, -1, 4, -5, 2, -3, 6};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 6) == -5);   // Global minimum
        assert(rmq_->query(0, 2) == -1);   // min(3, -1, 4) = -1
        assert(rmq_->query(3, 5) == -5);   // min(-5, 2, -3) = -5
    }
    
    void testQueryDetailed() {
        std::vector<Value> data = {7, 2, 5, 2, 9, 1, 3};
        rmq_->preprocess(data);
        
        QueryResult result = rmq_->queryDetailed(1, 4);
        assert(result.minimum_value == 2);
        assert(result.minimum_index == 1 || result.minimum_index == 3);  // First occurrence
        
        result = rmq_->queryDetailed(0, 6);
        assert(result.minimum_value == 1);
        assert(result.minimum_index == 5);
    }
    
    void testEmptyDataException() {
        std::vector<Value> empty_data;
        bool exception_thrown = false;
        
        try {
            rmq_->preprocess(empty_data);
        } catch (const InvalidDataException&) {
            exception_thrown = true;
        }
        
        assert(exception_thrown);
    }
    
    void testNotPreprocessedException() {
        RMQNaive fresh_rmq;
        bool exception_thrown = false;
        
        try {
            fresh_rmq.query(0, 5);
        } catch (const NotPreprocessedException&) {
            exception_thrown = true;
        }
        
        assert(exception_thrown);
    }
    
    void testInvalidQueryRange() {
        std::vector<Value> data = {1, 2, 3, 4, 5};
        rmq_->preprocess(data);
        
        bool exception_thrown = false;
        try {
            rmq_->query(3, 2);  // left > right
        } catch (const InvalidQueryException&) {
            exception_thrown = true;
        }
        assert(exception_thrown);
    }
    
    void testOutOfBoundsQuery() {
        std::vector<Value> data = {1, 2, 3, 4, 5};
        rmq_->preprocess(data);
        
        bool exception_thrown = false;
        try {
            rmq_->query(2, 10);  // right > size
        } catch (const BoundsException&) {
            exception_thrown = true;
        }
        assert(exception_thrown);
    }
    
    void testUpdate() {
        std::vector<Value> data = {3, 1, 4, 1, 5};
        rmq_->preprocess(data);
        
        assert(rmq_->query(0, 4) == 1);
        
        rmq_->update(1, 10);  // Change 1 to 10
        assert(rmq_->query(0, 4) == 1);  // Still 1 (at index 3)
        assert(rmq_->query(0, 2) == 3);  // min(3, 10, 4) = 3
        
        rmq_->update(3, 0);   // Change second 1 to 0
        assert(rmq_->query(0, 4) == 0);  // New minimum
    }
    
    void testBatchUpdate() {
        std::vector<Value> data = {5, 4, 3, 2, 1};
        rmq_->preprocess(data);
        
        std::vector<std::pair<Index, Value>> updates = {
            {0, 10}, {2, 15}, {4, 20}
        };
        
        rmq_->batchUpdate(updates);
        
        assert(rmq_->query(0, 4) == 2);  // min(10, 4, 15, 2, 20) = 2
        assert(rmq_->query(0, 0) == 10);
        assert(rmq_->query(4, 4) == 20);
    }
    
    void testComplexityInfo() {
        ComplexityInfo info = rmq_->getComplexity();
        
        assert(info.preprocessing_time == "O(1)");
        assert(info.preprocessing_space == "O(1)");
        assert(info.query_time == "O(n)");
        assert(info.query_space == "O(1)");
        assert(info.total_space == "O(n)");
    }
    
    void testMemoryUsage() {
        std::vector<Value> data(1000, 42);
        rmq_->preprocess(data);
        
        size_t memory = rmq_->getMemoryUsage();
        assert(memory > 1000 * sizeof(Value));  // At least the data size
        assert(memory < 10000 * sizeof(Value)); // But not too much
    }
    
    void testLargeDataset() {
        const size_t size = 10000;
        std::vector<Value> data(size);
        
        // Generate random data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-1000, 1000);
        
        for (size_t i = 0; i < size; ++i) {
            data[i] = dis(gen);
        }
        
        rmq_->preprocess(data);
        
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
    
    void testConfiguration() {
        AlgorithmConfig config;
        config.withCaching(true)
              .withStatistics(true);
        
        RMQNaive configured_rmq(config);
        
        std::vector<Value> data = {1, 2, 3};
        configured_rmq.preprocess(data);
        
        assert(configured_rmq.getConfig().enable_caching == true);
        assert(configured_rmq.getConfig().track_statistics == true);
    }
    
    void runAllTests(TestRunner& runner) {
        runner.runTest("Basic Functionality", [this]() { testBasicFunctionality(); });
        runner.runTest("Single Element", [this]() { testSingleElement(); });
        runner.runTest("Identical Elements", [this]() { testIdenticalElements(); });
        runner.runTest("Increasing Sequence", [this]() { testIncreasingSequence(); });
        runner.runTest("Decreasing Sequence", [this]() { testDecreasingSequence(); });
        runner.runTest("Negative Numbers", [this]() { testNegativeNumbers(); });
        runner.runTest("Query Detailed", [this]() { testQueryDetailed(); });
        runner.runTest("Empty Data Exception", [this]() { testEmptyDataException(); });
        runner.runTest("Not Preprocessed Exception", [this]() { testNotPreprocessedException(); });
        runner.runTest("Invalid Query Range", [this]() { testInvalidQueryRange(); });
        runner.runTest("Out of Bounds Query", [this]() { testOutOfBoundsQuery(); });
        runner.runTest("Update Single Element", [this]() { testUpdate(); });
        runner.runTest("Batch Update", [this]() { testBatchUpdate(); });
        runner.runTest("Complexity Info", [this]() { testComplexityInfo(); });
        runner.runTest("Memory Usage", [this]() { testMemoryUsage(); });
        runner.runTest("Large Dataset", [this]() { testLargeDataset(); });
        runner.runTest("Configuration", [this]() { testConfiguration(); });
    }
};

int main() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "RMQ Naive Implementation Unit Tests" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;
    
    TestRunner runner;
    RMQNaiveTest test;
    test.runAllTests(runner);
    
    runner.printSummary();
    
    return runner.allTestsPassed() ? 0 : 1;
}