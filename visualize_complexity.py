"""
RMQ Algorithm Complexity Visualization
Generates graphs showing time and space complexity of different RMQ algorithms
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import warnings
warnings.filterwarnings('ignore')

# Set style for better-looking plots
plt.style.use('seaborn-v0_8-darkgrid')

def load_data():
    """Load benchmark results from CSV files"""
    prep_df = pd.read_csv('benchmark_preprocessing.csv')
    query_df = pd.read_csv('benchmark_query.csv')
    memory_df = pd.read_csv('benchmark_memory.csv')
    return prep_df, query_df, memory_df

def plot_preprocessing_time(df):
    """Plot preprocessing time complexity"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Linear scale plot
    for algorithm in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algorithm]
        ax1.plot(algo_data['ArraySize'], algo_data['PreprocessingTime_ms'], 
                marker='o', linewidth=2, markersize=8, label=algorithm)
    
    ax1.set_xlabel('Array Size (n)', fontsize=12)
    ax1.set_ylabel('Preprocessing Time (ms)', fontsize=12)
    ax1.set_title('Preprocessing Time Complexity - Linear Scale', fontsize=14, fontweight='bold')
    ax1.legend(loc='upper left', fontsize=10)
    ax1.grid(True, alpha=0.3)
    
    # Log-log scale plot
    for algorithm in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algorithm]
        ax2.loglog(algo_data['ArraySize'], algo_data['PreprocessingTime_ms'], 
                  marker='o', linewidth=2, markersize=8, label=algorithm)
    
    # Add theoretical complexity lines
    n = np.logspace(1, 5, 100)
    ax2.loglog(n, 0.00001 * np.ones_like(n), 'k--', alpha=0.3, label='O(1) reference')
    ax2.loglog(n, 0.00001 * n, 'k-.', alpha=0.3, label='O(n) reference')
    ax2.loglog(n, 0.00001 * n * np.log2(n), 'k:', alpha=0.3, label='O(n log n) reference')
    ax2.loglog(n[n<=2000], 0.000001 * n[n<=2000]**2, 'k-', alpha=0.3, label='O(n²) reference')
    
    ax2.set_xlabel('Array Size (n) - Log Scale', fontsize=12)
    ax2.set_ylabel('Preprocessing Time (ms) - Log Scale', fontsize=12)
    ax2.set_title('Preprocessing Time Complexity - Log Scale', fontsize=14, fontweight='bold')
    ax2.legend(loc='upper left', fontsize=9)
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('preprocessing_complexity.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_query_time(df):
    """Plot query time complexity"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Linear scale plot
    for algorithm in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algorithm]
        ax1.plot(algo_data['ArraySize'], algo_data['QueryTime_us'], 
                marker='o', linewidth=2, markersize=8, label=algorithm)
    
    ax1.set_xlabel('Array Size (n)', fontsize=12)
    ax1.set_ylabel('Query Time (μs)', fontsize=12)
    ax1.set_title('Query Time Complexity - Linear Scale', fontsize=14, fontweight='bold')
    ax1.legend(loc='upper left', fontsize=10)
    ax1.grid(True, alpha=0.3)
    
    # Log-log scale plot
    for algorithm in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algorithm]
        ax2.loglog(algo_data['ArraySize'], algo_data['QueryTime_us'], 
                  marker='o', linewidth=2, markersize=8, label=algorithm)
    
    # Add theoretical complexity lines
    n = np.logspace(1, 5, 100)
    ax2.loglog(n, 0.01 * np.ones_like(n), 'k--', alpha=0.3, label='O(1) reference')
    ax2.loglog(n, 0.001 * np.log2(n), 'k-.', alpha=0.3, label='O(log n) reference')
    ax2.loglog(n, 0.01 * np.sqrt(n), 'k:', alpha=0.3, label='O(√n) reference')
    ax2.loglog(n, 0.001 * n, 'k-', alpha=0.3, label='O(n) reference')
    
    ax2.set_xlabel('Array Size (n) - Log Scale', fontsize=12)
    ax2.set_ylabel('Query Time (μs) - Log Scale', fontsize=12)
    ax2.set_title('Query Time Complexity - Log Scale', fontsize=14, fontweight='bold')
    ax2.legend(loc='upper left', fontsize=9)
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('query_complexity.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_memory_usage(df):
    """Plot memory usage (space complexity)"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Linear scale plot
    for algorithm in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algorithm]
        ax1.plot(algo_data['ArraySize'], algo_data['Memory_MB'], 
                marker='o', linewidth=2, markersize=8, label=algorithm)
    
    ax1.set_xlabel('Array Size (n)', fontsize=12)
    ax1.set_ylabel('Memory Usage (MB)', fontsize=12)
    ax1.set_title('Space Complexity - Linear Scale', fontsize=14, fontweight='bold')
    ax1.legend(loc='upper left', fontsize=10)
    ax1.grid(True, alpha=0.3)
    
    # Log-log scale plot
    for algorithm in df['Algorithm'].unique():
        algo_data = df[df['Algorithm'] == algorithm]
        ax2.loglog(algo_data['ArraySize'], algo_data['Memory_MB'], 
                  marker='o', linewidth=2, markersize=8, label=algorithm)
    
    # Add theoretical complexity lines
    n = np.logspace(1, 5, 100)
    ax2.loglog(n, 0.00001 * n, 'k--', alpha=0.3, label='O(n) reference')
    ax2.loglog(n, 0.00001 * n * np.log2(n), 'k-.', alpha=0.3, label='O(n log n) reference')
    ax2.loglog(n[n<=2000], 0.000001 * n[n<=2000]**2, 'k:', alpha=0.3, label='O(n²) reference')
    
    ax2.set_xlabel('Array Size (n) - Log Scale', fontsize=12)
    ax2.set_ylabel('Memory Usage (MB) - Log Scale', fontsize=12)
    ax2.set_title('Space Complexity - Log Scale', fontsize=14, fontweight='bold')
    ax2.legend(loc='upper left', fontsize=9)
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('memory_complexity.png', dpi=300, bbox_inches='tight')
    plt.show()

def plot_algorithm_comparison():
    """Create a comprehensive comparison plot"""
    prep_df, query_df, memory_df = load_data()
    
    fig, axes = plt.subplots(2, 3, figsize=(18, 10))
    
    # Define colors for each algorithm
    colors = {
        'Naive Linear Scan': '#FF6B6B',
        'Dynamic Programming': '#4ECDC4',
        'Sparse Table (Binary Lifting)': '#45B7D1',
        'Block Decomposition (Square Root)': '#96CEB4',
        'LCA-based (Cartesian Tree)': '#FECA57'
    }
    
    # 1. Preprocessing Time (Linear)
    ax = axes[0, 0]
    for algorithm in prep_df['Algorithm'].unique():
        algo_data = prep_df[prep_df['Algorithm'] == algorithm]
        color = colors.get(algorithm, 'gray')
        ax.plot(algo_data['ArraySize'], algo_data['PreprocessingTime_ms'], 
               marker='o', linewidth=2, markersize=6, label=algorithm, color=color)
    ax.set_xlabel('Array Size (n)')
    ax.set_ylabel('Time (ms)')
    ax.set_title('Preprocessing Time', fontweight='bold')
    ax.legend(fontsize=8)
    ax.grid(True, alpha=0.3)
    
    # 2. Query Time (Linear)
    ax = axes[0, 1]
    for algorithm in query_df['Algorithm'].unique():
        algo_data = query_df[query_df['Algorithm'] == algorithm]
        color = colors.get(algorithm, 'gray')
        ax.plot(algo_data['ArraySize'], algo_data['QueryTime_us'], 
               marker='o', linewidth=2, markersize=6, label=algorithm, color=color)
    ax.set_xlabel('Array Size (n)')
    ax.set_ylabel('Time (μs)')
    ax.set_title('Query Time', fontweight='bold')
    ax.legend(fontsize=8)
    ax.grid(True, alpha=0.3)
    
    # 3. Memory Usage (Linear)
    ax = axes[0, 2]
    for algorithm in memory_df['Algorithm'].unique():
        algo_data = memory_df[memory_df['Algorithm'] == algorithm]
        color = colors.get(algorithm, 'gray')
        ax.plot(algo_data['ArraySize'], algo_data['Memory_MB'], 
               marker='o', linewidth=2, markersize=6, label=algorithm, color=color)
    ax.set_xlabel('Array Size (n)')
    ax.set_ylabel('Memory (MB)')
    ax.set_title('Memory Usage', fontweight='bold')
    ax.legend(fontsize=8)
    ax.grid(True, alpha=0.3)
    
    # 4. Preprocessing Time (Log-Log)
    ax = axes[1, 0]
    for algorithm in prep_df['Algorithm'].unique():
        algo_data = prep_df[prep_df['Algorithm'] == algorithm]
        color = colors.get(algorithm, 'gray')
        ax.loglog(algo_data['ArraySize'], algo_data['PreprocessingTime_ms'], 
                 marker='o', linewidth=2, markersize=6, label=algorithm, color=color)
    ax.set_xlabel('Array Size (n) - Log Scale')
    ax.set_ylabel('Time (ms) - Log Scale')
    ax.set_title('Preprocessing (Log-Log Scale)', fontweight='bold')
    ax.grid(True, alpha=0.3)
    
    # 5. Query Time (Log-Log)
    ax = axes[1, 1]
    for algorithm in query_df['Algorithm'].unique():
        algo_data = query_df[query_df['Algorithm'] == algorithm]
        color = colors.get(algorithm, 'gray')
        ax.loglog(algo_data['ArraySize'], algo_data['QueryTime_us'], 
                 marker='o', linewidth=2, markersize=6, label=algorithm, color=color)
    ax.set_xlabel('Array Size (n) - Log Scale')
    ax.set_ylabel('Time (μs) - Log Scale')
    ax.set_title('Query Time (Log-Log Scale)', fontweight='bold')
    ax.grid(True, alpha=0.3)
    
    # 6. Memory Usage (Log-Log)
    ax = axes[1, 2]
    for algorithm in memory_df['Algorithm'].unique():
        algo_data = memory_df[memory_df['Algorithm'] == algorithm]
        color = colors.get(algorithm, 'gray')
        ax.loglog(algo_data['ArraySize'], algo_data['Memory_MB'], 
                 marker='o', linewidth=2, markersize=6, label=algorithm, color=color)
    ax.set_xlabel('Array Size (n) - Log Scale')
    ax.set_ylabel('Memory (MB) - Log Scale')
    ax.set_title('Memory Usage (Log-Log Scale)', fontweight='bold')
    ax.grid(True, alpha=0.3)
    
    plt.suptitle('RMQ Algorithm Complexity Analysis', fontsize=16, fontweight='bold', y=1.02)
    plt.tight_layout()
    plt.savefig('rmq_complexity_comparison.png', dpi=300, bbox_inches='tight')
    plt.show()

def print_complexity_verification():
    """Verify that algorithms follow theoretical complexity"""
    prep_df, query_df, memory_df = load_data()
    
    print("\n" + "="*80)
    print("COMPLEXITY VERIFICATION ANALYSIS")
    print("="*80)
    
    # Analyze each algorithm
    algorithms = prep_df['Algorithm'].unique()
    
    for algo in algorithms:
        print(f"\n{algo}:")
        print("-" * len(algo))
        
        # Get data for this algorithm
        prep_data = prep_df[prep_df['Algorithm'] == algo]
        query_data = query_df[query_df['Algorithm'] == algo]
        
        if len(prep_data) > 2:
            # Calculate growth rates
            sizes = prep_data['ArraySize'].values
            prep_times = prep_data['PreprocessingTime_ms'].values
            query_times = query_data['QueryTime_us'].values
            
            # Fit to different complexity models
            print("\nGrowth Rate Analysis:")
            
            # For preprocessing
            if prep_times[-1] > 0.001:  # Skip if essentially O(1)
                ratio = prep_times[-1] / prep_times[-2] if prep_times[-2] > 0 else 0
                size_ratio = sizes[-1] / sizes[-2]
                
                if abs(ratio - 1) < 0.5:
                    print(f"  Preprocessing: ~O(1) (ratio: {ratio:.2f})")
                elif abs(ratio - size_ratio) < size_ratio * 0.5:
                    print(f"  Preprocessing: ~O(n) (ratio: {ratio:.2f}, expected: {size_ratio:.2f})")
                elif abs(ratio - size_ratio**2) < size_ratio**2 * 0.5:
                    print(f"  Preprocessing: ~O(n²) (ratio: {ratio:.2f}, expected: {size_ratio**2:.2f})")
                elif abs(ratio - size_ratio * np.log2(size_ratio)) < size_ratio * np.log2(size_ratio) * 0.5:
                    print(f"  Preprocessing: ~O(n log n) (ratio: {ratio:.2f})")
                else:
                    print(f"  Preprocessing: Growth ratio: {ratio:.2f}")
            else:
                print(f"  Preprocessing: O(1) (constant time)")
            
            # For queries
            ratio = query_times[-1] / query_times[-2] if query_times[-2] > 0 else 0
            
            if abs(ratio - 1) < 0.5:
                print(f"  Query: ~O(1) (ratio: {ratio:.2f})")
            elif abs(ratio - np.sqrt(size_ratio)) < np.sqrt(size_ratio) * 0.5:
                print(f"  Query: ~O(√n) (ratio: {ratio:.2f}, expected: {np.sqrt(size_ratio):.2f})")
            elif abs(ratio - np.log2(size_ratio)) < np.log2(size_ratio) * 0.5:
                print(f"  Query: ~O(log n) (ratio: {ratio:.2f}, expected: {np.log2(size_ratio):.2f})")
            elif abs(ratio - size_ratio) < size_ratio * 0.5:
                print(f"  Query: ~O(n) (ratio: {ratio:.2f}, expected: {size_ratio:.2f})")
            else:
                print(f"  Query: Growth ratio: {ratio:.2f}")

def main():
    """Main function to generate all visualizations"""
    
    # Check if data files exist
    if not all(os.path.exists(f) for f in ['benchmark_preprocessing.csv', 
                                            'benchmark_query.csv', 
                                            'benchmark_memory.csv']):
        print("Error: Benchmark CSV files not found!")
        print("Please run the C++ benchmark program first: ./benchmark_complexity")
        return
    
    # Load data
    prep_df, query_df, memory_df = load_data()
    
    print("Generating complexity visualization graphs...")
    
    # Generate individual plots
    print("1. Generating preprocessing time complexity graph...")
    plot_preprocessing_time(prep_df)
    
    print("2. Generating query time complexity graph...")
    plot_query_time(query_df)
    
    print("3. Generating memory usage complexity graph...")
    plot_memory_usage(memory_df)
    
    print("4. Generating comprehensive comparison graph...")
    plot_algorithm_comparison()
    
    print("5. Performing complexity verification analysis...")
    print_complexity_verification()
    
    print("\n✅ All graphs generated successfully!")
    print("\nGenerated files:")
    print("  - preprocessing_complexity.png")
    print("  - query_complexity.png")
    print("  - memory_complexity.png")
    print("  - rmq_complexity_comparison.png")

if __name__ == "__main__":
    main()