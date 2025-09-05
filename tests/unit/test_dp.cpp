#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <chrono>
#include <iomanip>
#include <functional>
#include <algorithm>
#include "../../include/algorithms/rmq_dp.h"
#include "../../src/core/rmq_base.cpp"
#include "../../src/algorithms/rmq_dp.cpp"

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

class RMQDPTest {
private:
    std::unique_ptr<RMQDynamicProgramming> rmq_;
    
public:
    RMQDPTest() : rmq_(std::make_unique<RMQDynamicProgramming>()) {}
    
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
    
    void testAllPossibleRanges() {
        std::vector<Value> data = {5, 2, 8, 1, 9, 3};
        rmq_->preprocess(data);
        
        // Test all possible ranges and verify with brute force
        for (Index i = 0; i < data.size(); ++i) {
            for (Index j = i; j < data.size(); ++j) {
                Value dp_result = rmq_->query(i, j);
                
                // Verify with brute force
                Value expected = data[i];
                for (Index k = i + 1; k <= j; ++k) {
                    expected = std::min(expected, data[k]);
                }
                
                assert(dp_result == expected);
            }
        }
    }
    
    void testTableSize() {
        std::vector<Value> data(10, 1);
        rmq_->preprocess(data);
        
        assert(rmq_->getTableSize() == 100);  // 10x10 table
        
        auto [entries, memory] = rmq_->getPreprocessingStats();
        assert(entries == 100);
        assert(memory > 0);
    }
    
    void testMemoryUsage() {
        std::vector<Value> data(100, 42);
        rmq_->preprocess(data);
        
        size_t memory = rmq_->getMemoryUsage();
        // Should include base + data + dp_table + index_table
        assert(memory > 100 * 100 * sizeof(Value));  // At least the DP table
        assert(memory < 100 * 100 * sizeof(Value) * 10); // But reasonable
    }
    
    void testMinimumIndexTracking() {
        std::vector<Value> data = {7, 2, 5, 2, 9, 1, 3};
        rmq_->preprocess(data);
        
        // Test that we get the correct index
        QueryResult result = rmq_->queryDetailed(0, 3);
        assert(result.minimum_value == 2);
        assert(result.minimum_index == 1);  // First occurrence of 2
        
        result = rmq_->queryDetailed(2, 6);
        assert(result.minimum_value == 1);
        assert(result.minimum_index == 5);
    }
    
    void testRecommendedSizeCheck() {
        assert(RMQDynamicProgramming::isRecommendedSize(100) == true);
        assert(RMQDynamicProgramming::isRecommendedSize(1000) == true);
        assert(RMQDynamicProgramming::isRecommendedSize(1001) == false);
        assert(RMQDynamicProgramming::isRecommendedSize(10000) == false);
    }
    
    void testLargeButAllowedSize() {
        // Test with size larger than recommended but still allowed
        const size_t size = 500;
        std::vector<Value> data(size);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-100, 100);
        
        for (size_t i = 0; i < size; ++i) {
            data[i] = dis(gen);
        }
        
        rmq_->preprocess(data);
        
        // Test some random queries
        for (int i = 0; i < 20; ++i) {
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
    
    void testClearFunction() {
        std::vector<Value> data = {1, 2, 3, 4, 5};
        rmq_->preprocess(data);
        
        assert(rmq_->isPreprocessed() == true);
        assert(rmq_->getTableSize() > 0);
        
        rmq_->clear();
        
        assert(rmq_->isPreprocessed() == false);
        assert(rmq_->getTableSize() == 0);
        assert(rmq_->size() == 0);
    }
    
    void testComplexityInfo() {
        ComplexityInfo info = rmq_->getComplexity();
        
        assert(info.preprocessing_time == "O(n²)");
        assert(info.preprocessing_space == "O(n²)");
        assert(info.query_time == "O(1)");
        assert(info.query_space == "O(1)");
        assert(info.total_space == "O(n²)");
    }
    
    void testNoUpdateSupport() {
        assert(rmq_->supportsUpdate() == false);
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
        RMQDynamicProgramming fresh_rmq;
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
    
    void testO1QueryTime() {
        // Test that query time is indeed O(1) by timing
        const size_t size = 500;
        std::vector<Value> data(size, 1);
        rmq_->preprocess(data);
        
        // Time many queries
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 10000; ++i) {
            rmq_->query(0, size - 1);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Each query should be very fast (< 1 microsecond on average)
        double avg_query_time = duration.count() / 10000.0;
        assert(avg_query_time < 10);  // Less than 10 microseconds per query
    }
    
    void testConfiguration() {
        AlgorithmConfig config;
        config.withStatistics(true);
        
        RMQDynamicProgramming configured_rmq(config);
        
        std::vector<Value> data = {1, 2, 3};
        configured_rmq.preprocess(data);
        
        assert(configured_rmq.getConfig().track_statistics == true);
    }
    
    void testDestructorCleansUp() {
        // Test that destructor properly cleans up
        {
            RMQDynamicProgramming temp_rmq;
            std::vector<Value> data = {1, 2, 3, 4, 5};
            temp_rmq.preprocess(data);
            // Destructor will be called when temp_rmq goes out of scope
        }
        // If we get here without crash, destructor worked
        assert(true);
    }
    
    void runAllTests(TestRunner& runner) {
        runner.runTest("Basic Functionality", [this]() { testBasicFunctionality(); });
        runner.runTest("Single Element", [this]() { testSingleElement(); });
        runner.runTest("All Possible Ranges", [this]() { testAllPossibleRanges(); });
        runner.runTest("Table Size", [this]() { testTableSize(); });
        runner.runTest("Memory Usage", [this]() { testMemoryUsage(); });
        runner.runTest("Minimum Index Tracking", [this]() { testMinimumIndexTracking(); });
        runner.runTest("Recommended Size Check", [this]() { testRecommendedSizeCheck(); });
        runner.runTest("Large But Allowed Size", [this]() { testLargeButAllowedSize(); });
        runner.runTest("Clear Function", [this]() { testClearFunction(); });
        runner.runTest("Complexity Info", [this]() { testComplexityInfo(); });
        runner.runTest("No Update Support", [this]() { testNoUpdateSupport(); });
        runner.runTest("Empty Data Exception", [this]() { testEmptyDataException(); });
        runner.runTest("Not Preprocessed Exception", [this]() { testNotPreprocessedException(); });
        runner.runTest("Invalid Query Range", [this]() { testInvalidQueryRange(); });
        runner.runTest("O(1) Query Time", [this]() { testO1QueryTime(); });
        runner.runTest("Configuration", [this]() { testConfiguration(); });
        runner.runTest("Destructor Cleans Up", [this]() { testDestructorCleansUp(); });
    }
};

int main() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "RMQ Dynamic Programming Implementation Unit Tests" << std::endl;
    std::cout << std::string(70, '=') << std::endl << std::endl;
    
    TestRunner runner;
    RMQDPTest test;
    test.runAllTests(runner);
    
    runner.printSummary();
    
    return runner.allTestsPassed() ? 0 : 1;
}