#ifndef RMQ_ALGORITHMS_RMQ_LCA_H
#define RMQ_ALGORITHMS_RMQ_LCA_H

#include "../core/rmq_base.h"
#include <vector>
#include <stack>

namespace rmq {

/**
 * @brief LCA-based implementation of Range Minimum Query
 * 
 * This implementation converts RMQ to LCA (Lowest Common Ancestor) problem
 * by building a Cartesian tree. The minimum element in range [L, R] 
 * corresponds to the LCA of nodes L and R in the Cartesian tree.
 * 
 * @complexity
 * - Preprocessing: O(n) time to build tree, O(n log n) for LCA preprocessing
 * - Query: O(log n) time using binary lifting, or O(1) with Euler tour
 * - Update: Not supported (requires tree rebuild)
 * - Total Space: O(n log n) for binary lifting
 * 
 * @note This demonstrates the theoretical equivalence between RMQ and LCA
 */
class RMQLCABased final : public RMQBase {
private:
    static constexpr const char* ALGORITHM_NAME = "LCA-based (Cartesian Tree)";
    static constexpr AlgorithmType ALGORITHM_TYPE = AlgorithmType::LCA_BASED;
    
    /**
     * @brief Node structure for Cartesian tree
     */
    struct CartesianNode {
        Value value;           ///< Value at this node
        Index array_index;     ///< Original array index
        int left_child;        ///< Index of left child (-1 if none)
        int right_child;       ///< Index of right child (-1 if none)
        int parent;            ///< Index of parent (-1 if root)
        int depth;             ///< Depth in the tree
        
        CartesianNode() 
            : value(0), array_index(0), 
              left_child(-1), right_child(-1), 
              parent(-1), depth(0) {}
    };
    
    /**
     * @brief Cartesian tree nodes
     */
    std::vector<CartesianNode> tree_nodes_;
    
    /**
     * @brief Root of the Cartesian tree
     */
    int root_index_;
    
    /**
     * @brief Binary lifting table for LCA
     * ancestors_[i][j] = 2^j-th ancestor of node i
     */
    std::vector<std::vector<int>> ancestors_;
    
    /**
     * @brief Maximum levels for binary lifting
     */
    int max_log_;
    
    /**
     * @brief Map from array index to tree node index
     */
    std::vector<int> array_to_tree_;
    
    /**
     * @brief Build Cartesian tree from array
     * 
     * A Cartesian tree has the properties:
     * 1. In-order traversal gives the original array
     * 2. Each node's value <= its descendants (min-heap property)
     */
    void buildCartesianTree();
    
    /**
     * @brief Build binary lifting table for LCA queries
     */
    void buildLCAStructure();
    
    /**
     * @brief Compute depth of each node using DFS
     */
    void computeDepths(int node, int depth);
    
    /**
     * @brief Find LCA of two nodes using binary lifting
     * @param u First node index in tree
     * @param v Second node index in tree
     * @return Index of LCA node in tree
     */
    int findLCA(int u, int v) const;
    
    /**
     * @brief Get k-th ancestor of a node
     * @param node Node index
     * @param k Number of levels to go up
     * @return Ancestor node index
     */
    int getKthAncestor(int node, int k) const;
    
    /**
     * @brief Clear tree structures
     */
    void clearTree();
    
protected:
    /**
     * @brief Build Cartesian tree and LCA structure
     */
    void performPreprocess() override;
    
    /**
     * @brief Query using LCA on Cartesian tree
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return The minimum value in range [left, right]
     */
    Value performQuery(Index left, Index right) const override;
    
    /**
     * @brief Find index of minimum element using LCA
     * @param left Left boundary (inclusive)
     * @param right Right boundary (inclusive)
     * @return Index of minimum element in range
     */
    Index findMinimumIndex(Index left, Index right) const override;
    
public:
    /**
     * @brief Default constructor
     */
    RMQLCABased();
    
    /**
     * @brief Constructor with configuration
     * @param config Algorithm configuration
     */
    explicit RMQLCABased(const AlgorithmConfig& config);
    
    /**
     * @brief Destructor
     */
    ~RMQLCABased() override;
    
    /**
     * @brief Get the algorithm name
     * @return Human-readable algorithm name
     */
    std::string getName() const override {
        return ALGORITHM_NAME;
    }
    
    /**
     * @brief Get the algorithm type
     * @return Algorithm type enum value
     */
    AlgorithmType getType() const override {
        return ALGORITHM_TYPE;
    }
    
    /**
     * @brief Get complexity information
     * @return Complexity details for LCA-based algorithm
     */
    ComplexityInfo getComplexity() const override;
    
    /**
     * @brief Check if the algorithm supports dynamic updates
     * @return false (tree structure requires rebuild for updates)
     */
    bool supportsUpdate() const override {
        return false;
    }
    
    /**
     * @brief Clear all preprocessed data
     */
    void clear() override;
    
    /**
     * @brief Get memory usage in bytes
     * @return Approximate memory usage
     */
    size_t getMemoryUsage() const;
    
    /**
     * @brief Get the size of the Cartesian tree
     * @return Number of nodes in the tree
     */
    size_t getTreeSize() const {
        return tree_nodes_.size();
    }
    
    /**
     * @brief Get the depth of the Cartesian tree
     * @return Maximum depth of the tree
     */
    int getTreeDepth() const;
    
    /**
     * @brief Verify tree structure (for debugging)
     * @return true if tree is valid, false otherwise
     */
    bool verifyTree() const;
    
    /**
     * @brief Get tree statistics
     * @return Tuple of (num_nodes, tree_depth, memory_bytes)
     */
    std::tuple<size_t, int, size_t> getTreeStats() const;
};

} // namespace rmq

#endif // RMQ_ALGORITHMS_RMQ_LCA_H