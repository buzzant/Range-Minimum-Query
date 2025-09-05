#include "../../include/core/rmq_base.h"
#include <algorithm>

namespace rmq {

RMQBase::RMQBase() 
    : preprocessed_(false), 
      last_query_time_(0),
      config_() {
}

RMQBase::RMQBase(const AlgorithmConfig& config) 
    : preprocessed_(false), 
      last_query_time_(0),
      config_(config) {
}

void RMQBase::validateQuery(Index left, Index right) const {
    if (left > right) {
        throw InvalidQueryException(left, right);
    }
    
    if (data_.empty()) {
        throw InvalidDataException("Cannot query empty data");
    }
    
    if (right >= data_.size()) {
        throw BoundsException(left, right, data_.size());
    }
}

void RMQBase::validateData(const std::vector<Value>& data) const {
    if (data.empty()) {
        throw InvalidDataException();
    }
    
    if (data.size() > constants::MAX_ARRAY_SIZE) {
        throw InvalidDataException(data.size());
    }
}

void RMQBase::ensurePreprocessed() const {
    if (!preprocessed_) {
        throw NotPreprocessedException(getName());
    }
}

void RMQBase::preprocess(const std::vector<Value>& data) {
    validateData(data);
    
    data_ = data;
    preprocessed_ = false;
    
    try {
        performPreprocess();
        preprocessed_ = true;
    } catch (const std::bad_alloc& e) {
        clear();
        throw AllocationException("Failed to allocate memory during preprocessing");
    } catch (const std::exception& e) {
        clear();
        throw AlgorithmException(getName(), std::string("Preprocessing failed: ") + e.what());
    } catch (...) {
        clear();
        throw AlgorithmException(getName(), "Unknown error during preprocessing");
    }
}

Value RMQBase::query(Index left, Index right) const {
    ensurePreprocessed();
    validateQuery(left, right);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    Value result = performQuery(left, right);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    last_query_time_ = std::chrono::duration_cast<Duration>(end_time - start_time);
    
    return result;
}

QueryResult RMQBase::queryDetailed(Index left, Index right) const {
    ensurePreprocessed();
    validateQuery(left, right);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    Value min_value = performQuery(left, right);
    Index min_index = findMinimumIndex(left, right);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    Duration query_time = std::chrono::duration_cast<Duration>(end_time - start_time);
    
    last_query_time_ = query_time;
    
    return QueryResult(min_value, min_index, query_time);
}

Index RMQBase::findMinimumIndex(Index left, Index right) const {
    Value min_value = performQuery(left, right);
    
    for (Index i = left; i <= right; ++i) {
        if (data_[i] == min_value) {
            return i;
        }
    }
    
    return left;
}

void RMQBase::clear() {
    data_.clear();
    preprocessed_ = false;
    last_query_time_ = Duration(0);
}

} // namespace rmq