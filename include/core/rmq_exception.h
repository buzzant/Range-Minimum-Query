#ifndef RMQ_CORE_RMQ_EXCEPTION_H
#define RMQ_CORE_RMQ_EXCEPTION_H

#include <exception>
#include <stdexcept>
#include <string>
#include <sstream>
#include "rmq_types.h"

namespace rmq {

/**
 * @brief Base exception class for all RMQ-related errors
 */
class RMQException : public std::runtime_error {
public:
    /**
     * @brief Constructor with error message
     * @param message The error message
     */
    explicit RMQException(const std::string& message) 
        : std::runtime_error("RMQ Error: " + message) {}
    
    /**
     * @brief Constructor with error message and cause
     * @param message The error message
     * @param cause The underlying cause
     */
    RMQException(const std::string& message, const std::exception& cause)
        : std::runtime_error("RMQ Error: " + message + " (Cause: " + cause.what() + ")") {}
};

/**
 * @brief Exception thrown when array bounds are violated
 */
class BoundsException : public RMQException {
public:
    /**
     * @brief Constructor for index out of bounds
     * @param index The invalid index
     * @param size The array size
     */
    BoundsException(Index index, Size size)
        : RMQException(createMessage(index, size)) {}
    
    /**
     * @brief Constructor for range out of bounds
     * @param left Left boundary
     * @param right Right boundary
     * @param size Array size
     */
    BoundsException(Index left, Index right, Size size)
        : RMQException(createRangeMessage(left, right, size)) {}

private:
    static std::string createMessage(Index index, Size size) {
        std::ostringstream oss;
        oss << "Index " << index << " is out of bounds for array of size " << size;
        return oss.str();
    }
    
    static std::string createRangeMessage(Index left, Index right, Size size) {
        std::ostringstream oss;
        oss << "Range [" << left << ", " << right << "] is out of bounds for array of size " << size;
        return oss.str();
    }
};

/**
 * @brief Exception thrown when preprocessing hasn't been done
 */
class NotPreprocessedException : public RMQException {
public:
    /**
     * @brief Default constructor
     */
    NotPreprocessedException()
        : RMQException("Algorithm has not been preprocessed. Call preprocess() first.") {}
    
    /**
     * @brief Constructor with algorithm name
     * @param algorithm_name Name of the algorithm
     */
    explicit NotPreprocessedException(const std::string& algorithm_name)
        : RMQException(algorithm_name + " has not been preprocessed. Call preprocess() first.") {}
};

/**
 * @brief Exception thrown for invalid queries
 */
class InvalidQueryException : public RMQException {
public:
    /**
     * @brief Constructor for invalid range
     * @param left Left boundary
     * @param right Right boundary
     */
    InvalidQueryException(Index left, Index right)
        : RMQException(createMessage(left, right)) {}
    
    /**
     * @brief Constructor with custom message
     * @param message Custom error message
     */
    explicit InvalidQueryException(const std::string& message)
        : RMQException("Invalid query: " + message) {}

private:
    static std::string createMessage(Index left, Index right) {
        std::ostringstream oss;
        oss << "Invalid query range [" << left << ", " << right << "]. ";
        oss << "Left boundary must be less than or equal to right boundary.";
        return oss.str();
    }
};

/**
 * @brief Exception thrown for invalid input data
 */
class InvalidDataException : public RMQException {
public:
    /**
     * @brief Constructor for empty data
     */
    InvalidDataException()
        : RMQException("Input data is empty or invalid") {}
    
    /**
     * @brief Constructor with size information
     * @param size The invalid size
     */
    explicit InvalidDataException(Size size)
        : RMQException(createMessage(size)) {}
    
    /**
     * @brief Constructor with custom message
     * @param message Custom error message
     */
    explicit InvalidDataException(const std::string& message)
        : RMQException("Invalid data: " + message) {}

private:
    static std::string createMessage(Size size) {
        std::ostringstream oss;
        if (size == 0) {
            oss << "Input data is empty";
        } else if (size > constants::MAX_ARRAY_SIZE) {
            oss << "Input data size " << size << " exceeds maximum allowed size " 
                << constants::MAX_ARRAY_SIZE;
        } else {
            oss << "Invalid data size: " << size;
        }
        return oss.str();
    }
};

/**
 * @brief Exception thrown for configuration errors
 */
class ConfigurationException : public RMQException {
public:
    /**
     * @brief Constructor with configuration parameter name
     * @param param_name Name of the invalid parameter
     * @param message Error message
     */
    ConfigurationException(const std::string& param_name, const std::string& message)
        : RMQException("Configuration error for '" + param_name + "': " + message) {}
    
    /**
     * @brief Constructor with general message
     * @param message Error message
     */
    explicit ConfigurationException(const std::string& message)
        : RMQException("Configuration error: " + message) {}
};

/**
 * @brief Exception thrown for memory allocation failures
 */
class AllocationException : public RMQException {
public:
    /**
     * @brief Constructor with size that failed to allocate
     * @param size Size in bytes that failed to allocate
     */
    explicit AllocationException(Size size)
        : RMQException(createMessage(size)) {}
    
    /**
     * @brief Constructor with custom message
     * @param message Custom error message
     */
    explicit AllocationException(const std::string& message)
        : RMQException("Memory allocation failed: " + message) {}

private:
    static std::string createMessage(Size size) {
        std::ostringstream oss;
        oss << "Failed to allocate " << size << " bytes of memory";
        return oss.str();
    }
};

/**
 * @brief Exception thrown when an operation is not supported
 */
class NotSupportedException : public RMQException {
public:
    /**
     * @brief Constructor with operation name
     * @param operation Name of the unsupported operation
     */
    explicit NotSupportedException(const std::string& operation)
        : RMQException("Operation '" + operation + "' is not supported by this algorithm") {}
    
    /**
     * @brief Constructor with operation and algorithm names
     * @param operation Name of the unsupported operation
     * @param algorithm Name of the algorithm
     */
    NotSupportedException(const std::string& operation, const std::string& algorithm)
        : RMQException("Operation '" + operation + "' is not supported by " + algorithm) {}
};

/**
 * @brief Exception thrown for internal algorithm errors
 */
class AlgorithmException : public RMQException {
public:
    /**
     * @brief Constructor with algorithm name and error message
     * @param algorithm Name of the algorithm
     * @param message Error message
     */
    AlgorithmException(const std::string& algorithm, const std::string& message)
        : RMQException("Algorithm error in " + algorithm + ": " + message) {}
    
    /**
     * @brief Constructor with error message only
     * @param message Error message
     */
    explicit AlgorithmException(const std::string& message)
        : RMQException("Algorithm error: " + message) {}
};

} // namespace rmq

#endif // RMQ_CORE_RMQ_EXCEPTION_H