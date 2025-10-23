#pragma once

#include "../SSA/SSA.h"
#include "../AdvancedOptimization/AdvancedOptimizer.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <functional>

namespace Snow {
namespace BubbleRuntime {

// ============================================================================
// PARALLEL EXECUTION MODULE
// ============================================================================

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads = 0);
 ~ThreadPool();
  
    // Submit task for execution
    template<typename F>
    void Submit(F&& task) {
        {
   std::lock_guard<std::mutex> lock(queue_mutex_);
   tasks_.emplace(std::forward<F>(task));
  }
        condition_.notify_one();
    }
    
    // Wait for all tasks to complete
    void WaitAll();
  
    // Get number of threads
    size_t GetNumThreads() const { return threads_.size(); }

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
  std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    std::atomic<int> active_tasks_;
    
    void WorkerThread();
};

// ============================================================================
// CIAM INTROSPECTION
// ============================================================================

class CIAMIntrospection {
public:
    CIAMIntrospection();
    
    // Runtime type information
    struct TypeInfo {
   std::string name;
size_t size;
     size_t alignment;
  };
    
    // Register CIAM macro
    void RegisterMacro(const std::string& name, void* implementation);
    
    // Query macro at runtime
  void* QueryMacro(const std::string& name);
    
    // Get type information
    const TypeInfo* GetTypeInfo(const std::string& type_name);
    
    // Performance counters
    struct PerformanceCounters {
uint64_t instructions_executed;
        uint64_t cache_misses;
    uint64_t branch_mispredictions;
        uint64_t cycles;
        double ipc; // Instructions per cycle
    };
    
    const PerformanceCounters& GetPerformanceCounters() const { return perf_counters_; }
    void ResetPerformanceCounters();

private:
    std::unordered_map<std::string, void*> macro_registry_;
    std::unordered_map<std::string, TypeInfo> type_registry_;
    PerformanceCounters perf_counters_;
};

// ============================================================================
// ASTROLAKE HARDWARE SIMULATION
// ============================================================================

class AstroLakeSimulator {
public:
    AstroLakeSimulator();
    
    // Hardware configuration
    struct HardwareConfig {
        int num_sockets;
 int cores_per_socket;
      int e_cores; // Efficiency cores
        int p_cores; // Performance cores
        int l1_cache_kb;
    int l2_cache_kb;
      int l3_cache_kb;
        bool has_photonic_bus;
        bool has_lstm_predictor;
    };
 
    void SetConfig(const HardwareConfig& config);
    const HardwareConfig& GetConfig() const { return config_; }
    
    // Simulate execution
 void SimulateExecution(const SSA::SSAModule& module);
  
    // Get telemetry
    struct Telemetry {
  double execution_time_ms;
        double power_watts;
  double temperature_celsius;
        uint64_t total_instructions;
        uint64_t cache_hits;
        uint64_t cache_misses;
        uint64_t branch_predictions;
        uint64_t branch_mispredictions;
        double thermal_throttle_percent;
    };
  
    const Telemetry& GetTelemetry() const { return telemetry_; }
    
    // LSTM thermal predictor
    double PredictTemperature(double current_temp, double workload);
    
    // Photonic bus simulation
    void SimulatePhotonicBus(size_t data_size);

private:
    HardwareConfig config_;
    Telemetry telemetry_;
    
// Cache simulation
    struct CacheSimulator {
        size_t size;
        int associativity;
        int line_size;
        uint64_t hits;
      uint64_t misses;
 
        bool Access(uint64_t address);
    };
    
    CacheSimulator l1_cache_;
    CacheSimulator l2_cache_;
    CacheSimulator l3_cache_;
    
    // Branch predictor simulation
    struct BranchPredictor {
    int history_bits;
        std::vector<int> pattern_history_table;
     
   bool Predict(uint64_t pc);
        void Update(uint64_t pc, bool taken);
    };
    
    BranchPredictor branch_predictor_;
    
    // LSTM thermal model
    struct LSTMThermalModel {
        std::vector<double> weights;
        std::vector<double> state;
        
  double Forward(double temp, double workload);
    };
    
    LSTMThermalModel thermal_model_;
};

// ============================================================================
// PROFILING & FEEDBACK
// ============================================================================

class ProfileCollector {
public:
    ProfileCollector();
    
    // Start profiling
    void StartProfiling();
    
    // Stop profiling
    void StopProfiling();
    
    // Collect profile data
    AdvancedOptimization::ProfileData GetProfileData();
    
    // Export to file
    void ExportToFile(const std::string& filename);
    
    // Import from file
    void ImportFromFile(const std::string& filename);
    
    // Update counters
    void RecordBlockExecution(const std::string& block_name);
    void RecordBranchTaken(const std::string& branch_name, bool taken);
    void RecordLoopIteration(const std::string& loop_name, int iterations);
    void RecordFunctionCall(const std::string& function_name);
    void RecordCacheMiss(const std::string& location);

private:
    bool profiling_active_;
    AdvancedOptimization::ProfileData profile_data_;
    std::mutex data_mutex_;
    
    std::chrono::high_resolution_clock::time_point start_time_;
};

// ============================================================================
// ADAPTIVE REOPTIMIZATION
// ============================================================================

class AdaptiveReoptimizer {
public:
    AdaptiveReoptimizer();
    
    // Set thresholds
    void SetReoptimizationThreshold(double threshold);
    
    // Check if reoptimization is needed
    bool ShouldReoptimize(const AdvancedOptimization::ProfileData& profile);
    
    // Perform adaptive reoptimization
    void Reoptimize(SSA::SSAModule& module, const AdvancedOptimization::ProfileData& profile);
    
    // Learning database
    void UpdateLearningDatabase(const AdvancedOptimization::ProfileData& profile, double speedup);
    
    // Query learned patterns
    std::vector<std::string> GetOptimizationSuggestions(const SSA::SSAModule& module);

private:
    double reopt_threshold_;
  
    // Learning database
 struct OptimizationPattern {
        std::string code_pattern;
        std::string optimization;
        double average_speedup;
        int use_count;
    };
    
    std::vector<OptimizationPattern> learned_patterns_;
    
    // Pattern matching
    bool MatchesPattern(const SSA::SSAModule& module, const std::string& pattern);
};

// ============================================================================
// BUBBLE RUNTIME MANAGER
// ============================================================================

class BubbleRuntimeManager {
public:
    BubbleRuntimeManager();
    
    // Initialize runtime
    void Initialize(const AstroLakeSimulator::HardwareConfig& hw_config);
    
    // Execute module
    void Execute(const SSA::SSAModule& module);
    
    // Enable parallel execution
    void SetParallelExecution(bool enable);
    
    // Enable profiling
    void SetProfiling(bool enable);
    
    // Enable adaptive reoptimization
    void SetAdaptiveReoptimization(bool enable);
    
    // Get runtime statistics
    struct RuntimeStats {
        double total_execution_time_ms;
        int num_reoptimizations;
        double average_speedup;
        AstroLakeSimulator::Telemetry hardware_telemetry;
        CIAMIntrospection::PerformanceCounters perf_counters;
    };
    
    const RuntimeStats& GetStats() const { return stats_; }
    
    // Feedback loop
    void FeedbackLoop();

private:
    ThreadPool thread_pool_;
    CIAMIntrospection introspection_;
    AstroLakeSimulator hardware_sim_;
    ProfileCollector profiler_;
    AdaptiveReoptimizer reoptimizer_;
    
    bool parallel_enabled_;
    bool profiling_enabled_;
    bool adaptive_reopt_enabled_;
    
    RuntimeStats stats_;
    
    std::thread feedback_thread_;
    std::atomic<bool> feedback_running_;
    
  void ExecuteParallel(const SSA::SSAModule& module);
    void ExecuteSequential(const SSA::SSAModule& module);
};

} // namespace BubbleRuntime
} // namespace Snow
