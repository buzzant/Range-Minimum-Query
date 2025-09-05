#include "../../include/algorithms/rmq_lca.h"
#include <algorithm>
#include <stack>
#include <cmath>
#include <limits>

namespace rmq {

RMQLCABased::RMQLCABased() 
    : RMQBase(), root_index_(-1), max_log_(0) {
}

RMQLCABased::RMQLCABased(const AlgorithmConfig& config)
    : RMQBase(config), root_index_(-1), max_log_(0) {
}

RMQLCABased::~RMQLCABased() {
    clearTree();
}

void RMQLCABased::clearTree() {
    tree_nodes_.clear();
    tree_nodes_.shrink_to_fit();
    ancestors_.clear();
    ancestors_.shrink_to_fit();
    array_to_tree_.clear();
    array_to_tree_.shrink_to_fit();
    root_index_ = -1;
    max_log_ = 0;
}

void RMQLCABased::buildCartesianTree() {
    Size n = data_.size();
    if (n == 0) return;
    
    // Initialize tree nodes
    tree_nodes_.resize(n);
    array_to_tree_.resize(n);
    
    for (Index i = 0; i < n; ++i) {
        tree_nodes_[i].value = data_[i];
        tree_nodes_[i].array_index = i;
        tree_nodes_[i].parent = -1;
        tree_nodes_[i].left_child = -1;
        tree_nodes_[i].right_child = -1;
        array_to_tree_[i] = static_cast<int>(i);
    }
    
    // Build Cartesian tree using stack-based algorithm
    // This maintains the invariant that the stack contains
    // the rightmost path from root to the current position
    std::stack<int> rightmost_path;
    
    for (int i = 0; i < static_cast<int>(n); ++i) {
        int last_popped = -1;
        
        // Pop elements from stack that are greater than current
        while (!rightmost_path.empty() && 
               tree_nodes_[rightmost_path.top()].value > tree_nodes_[i].value) {
            last_popped = rightmost_path.top();
            rightmost_path.pop();
        }
        
        // Set relationships
        if (!rightmost_path.empty()) {
            // Current node becomes right child of stack top
            tree_nodes_[rightmost_path.top()].right_child = i;
            tree_nodes_[i].parent = rightmost_path.top();
        }
        
        if (last_popped != -1) {
            // Last popped becomes left child of current
            tree_nodes_[i].left_child = last_popped;
            tree_nodes_[last_popped].parent = i;
        }
        
        rightmost_path.push(i);
    }
    
    // Find root (node with no parent)
    root_index_ = -1;
    for (int i = 0; i < static_cast<int>(n); ++i) {
        if (tree_nodes_[i].parent == -1) {
            root_index_ = i;
            break;
        }
    }
    
    // Compute depths
    if (root_index_ != -1) {
        computeDepths(root_index_, 0);
    }
}

void RMQLCABased::computeDepths(int node, int depth) {
    if (node == -1) return;
    
    tree_nodes_[node].depth = depth;
    
    if (tree_nodes_[node].left_child != -1) {
        computeDepths(tree_nodes_[node].left_child, depth + 1);
    }
    
    if (tree_nodes_[node].right_child != -1) {
        computeDepths(tree_nodes_[node].right_child, depth + 1);
    }
}

void RMQLCABased::buildLCAStructure() {
    Size n = tree_nodes_.size();
    if (n == 0 || root_index_ == -1) return;
    
    // Calculate maximum log needed for binary lifting
    max_log_ = 0;
    while ((1 << max_log_) < static_cast<int>(n)) {
        max_log_++;
    }
    max_log_++;
    
    // Initialize ancestors table
    ancestors_.assign(n, std::vector<int>(max_log_, -1));
    
    // Set immediate parents
    for (size_t i = 0; i < n; ++i) {
        ancestors_[i][0] = tree_nodes_[i].parent;
    }
    
    // Fill binary lifting table
    for (int j = 1; j < max_log_; ++j) {
        for (size_t i = 0; i < n; ++i) {
            if (ancestors_[i][j - 1] != -1) {
                ancestors_[i][j] = ancestors_[ancestors_[i][j - 1]][j - 1];
            }
        }
    }
}

int RMQLCABased::getKthAncestor(int node, int k) const {
    if (node == -1 || k < 0) return -1;
    
    for (int i = 0; i < max_log_ && node != -1; ++i) {
        if (k & (1 << i)) {
            node = ancestors_[node][i];
        }
    }
    
    return node;
}

int RMQLCABased::findLCA(int u, int v) const {
    if (u == -1 || v == -1) return -1;
    
    // Ensure u is at the same or deeper level than v
    if (tree_nodes_[u].depth < tree_nodes_[v].depth) {
        std::swap(u, v);
    }
    
    // Bring u up to the same level as v
    int depth_diff = tree_nodes_[u].depth - tree_nodes_[v].depth;
    u = getKthAncestor(u, depth_diff);
    
    if (u == v) {
        return u;
    }
    
    // Binary search for LCA
    for (int i = max_log_ - 1; i >= 0; --i) {
        if (ancestors_[u][i] != ancestors_[v][i]) {
            u = ancestors_[u][i];
            v = ancestors_[v][i];
        }
    }
    
    // Parent of u (or v) is the LCA
    return ancestors_[u][0];
}

void RMQLCABased::performPreprocess() {
    Size n = data_.size();
    if (n == 0) return;
    
    // Clear any existing tree
    clearTree();
    
    try {
        // Build Cartesian tree
        buildCartesianTree();
        
        // Build LCA structure
        buildLCAStructure();
        
    } catch (const std::bad_alloc&) {
        clearTree();
        throw AllocationException("Failed to allocate memory for Cartesian tree");
    }
}

Value RMQLCABased::performQuery(Index left, Index right) const {
    // Convert array indices to tree node indices
    int tree_left = array_to_tree_[left];
    int tree_right = array_to_tree_[right];
    
    // Find LCA of the two nodes
    int lca = findLCA(tree_left, tree_right);
    
    if (lca == -1) {
        throw AlgorithmException(getName(), "LCA query failed");
    }
    
    // Return value at LCA node
    return tree_nodes_[lca].value;
}

Index RMQLCABased::findMinimumIndex(Index left, Index right) const {
    // Convert array indices to tree node indices
    int tree_left = array_to_tree_[left];
    int tree_right = array_to_tree_[right];
    
    // Find LCA of the two nodes
    int lca = findLCA(tree_left, tree_right);
    
    if (lca == -1) {
        throw AlgorithmException(getName(), "LCA query failed");
    }
    
    // Return original array index of LCA node
    return tree_nodes_[lca].array_index;
}

ComplexityInfo RMQLCABased::getComplexity() const {
    return ComplexityInfo(
        "O(n log n)",  // preprocessing_time (tree + LCA structure)
        "O(n log n)",  // preprocessing_space
        "O(log n)",    // query_time (binary lifting)
        "O(1)",        // query_space
        "O(n log n)"   // total_space
    );
}

void RMQLCABased::clear() {
    RMQBase::clear();
    clearTree();
}

size_t RMQLCABased::getMemoryUsage() const {
    size_t base_memory = sizeof(RMQLCABased);
    
    // Data vector memory
    if (!data_.empty()) {
        base_memory += data_.capacity() * sizeof(Value);
    }
    
    // Tree nodes memory
    if (!tree_nodes_.empty()) {
        base_memory += tree_nodes_.capacity() * sizeof(CartesianNode);
    }
    
    // Ancestors table memory
    if (!ancestors_.empty()) {
        for (const auto& row : ancestors_) {
            base_memory += row.capacity() * sizeof(int);
        }
        base_memory += ancestors_.capacity() * sizeof(std::vector<int>);
    }
    
    // Array to tree mapping
    if (!array_to_tree_.empty()) {
        base_memory += array_to_tree_.capacity() * sizeof(int);
    }
    
    return base_memory;
}

int RMQLCABased::getTreeDepth() const {
    if (tree_nodes_.empty()) return 0;
    
    int max_depth = 0;
    for (const auto& node : tree_nodes_) {
        max_depth = std::max(max_depth, node.depth);
    }
    
    return max_depth;
}

bool RMQLCABased::verifyTree() const {
    if (tree_nodes_.empty() || root_index_ == -1) {
        return false;
    }
    
    Size n = tree_nodes_.size();
    
    // Verify that we have exactly one root
    int root_count = 0;
    for (size_t i = 0; i < n; ++i) {
        if (tree_nodes_[i].parent == -1) {
            root_count++;
        }
    }
    
    if (root_count != 1) {
        return false;
    }
    
    // Verify parent-child relationships are consistent
    for (size_t i = 0; i < n; ++i) {
        int left = tree_nodes_[i].left_child;
        int right = tree_nodes_[i].right_child;
        
        if (left != -1) {
            if (left < 0 || left >= static_cast<int>(n)) {
                return false;
            }
            if (tree_nodes_[left].parent != static_cast<int>(i)) {
                return false;
            }
            // Min-heap property
            if (tree_nodes_[left].value < tree_nodes_[i].value) {
                return false;
            }
        }
        
        if (right != -1) {
            if (right < 0 || right >= static_cast<int>(n)) {
                return false;
            }
            if (tree_nodes_[right].parent != static_cast<int>(i)) {
                return false;
            }
            // Min-heap property
            if (tree_nodes_[right].value < tree_nodes_[i].value) {
                return false;
            }
        }
    }
    
    return true;
}

std::tuple<size_t, int, size_t> RMQLCABased::getTreeStats() const {
    size_t num_nodes = tree_nodes_.size();
    int tree_depth = getTreeDepth();
    size_t memory = getMemoryUsage();
    
    return std::make_tuple(num_nodes, tree_depth, memory);
}

} // namespace rmq