#pragma once

#include "../SSA/SSA.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace Snow {
namespace AdvancedOptimization {

// ============================================================================
// PROFILE-GUIDED OPTIMIZATION DATA
// ============================================================================

struct ProfileData {
    // Basic block execution counts
    std::unordered_map<std::string, uint64_t> block_counts;
  
  // Branch prediction data
    std::unordered_map<std::string, uint64_t> branch_taken;
    std::unordered_map<std::string, uint64_t> branch_not_taken;
    
    // Loop iteration counts
    std::unordered_map<std::string, std::vector<uint64_t>> loop_iterations;
    
 // Function call frequencies
    std::unordered_map<std::string, uint64_t> call_counts;
    
// Cache miss rates
    std::unordered_map<std::string, double> cache_miss_rate;
  
    // Thermal data (AstroLake specific)
    std::unordered_map<std::string, double> thermal_profile;
};

// ============================================================================
// INTERPROCEDURAL ANALYSIS
// ============================================================================

class InterproceduralAnalysis {
public:
    InterproceduralAnalysis() = default;
    
    // Alias analysis
 void PerformAliasAnalysis(SSA::SSAModule& module);
    bool MayAlias(SSA::SSAValue* a, SSA::SSAValue* b);
    
    // Escape analysis
    void PerformEscapeAnalysis(SSA::SSAModule& module);
    bool DoesEscape(SSA::SSAValue* value);
    
    // Global Value Numbering
    void PerformGVN(SSA::SSAModule& module);
    
    // Call graph analysis
    void BuildCallGraph(SSA::SSAModule& module);
    std::vector<SSA::SSAFunction*> GetCallees(SSA::SSAFunction* func);
 
    // Devirtualization
    void PerformDevirtualization(SSA::SSAModule& module);

private:
    std::unordered_map<SSA::SSAValue*, std::unordered_set<SSA::SSAValue*>> alias_sets_;
    std::unordered_set<SSA::SSAValue*> escaped_values_;
    std::unordered_map<SSA::SSAFunction*, std::vector<SSA::SSAFunction*>> call_graph_;
};

// ============================================================================
// ADVANCED OPTIMIZER
// ============================================================================

class AdvancedOptimizer {
public:
    AdvancedOptimizer();
    
    // Set optimization level
    void SetOptimizationLevel(int level); // 0-3
    
    // Set profile data
    void SetProfileData(const ProfileData& data);
    
    // Enable specific optimizations
    void EnableLinkTimeOptimization(bool enable);
    void EnableAutoFDO(bool enable);
    void EnableSpeculativeOptimization(bool enable);
    
    // Main optimization entry point
    void Optimize(SSA::SSAModule& module);
    
    // Get statistics
    struct OptimizationStats {
        int instructions_eliminated;
        int loops_vectorized;
        int functions_inlined;
        int tail_calls_eliminated;
double speedup_estimate;
    };
    
    const OptimizationStats& GetStats() const { return stats_; }

private:
    int opt_level_;
    ProfileData profile_data_;
    bool enable_lto_;
    bool enable_autofdo_;
    bool enable_speculative_;
    OptimizationStats stats_;
  InterproceduralAnalysis ipa_;
    
    // ========================================================================
    // TIER 1: BASIC OPTIMIZATIONS (-O1)
    // ========================================================================
    
    void Tier1_ConstantFolding(SSA::SSAModule& module);
    void Tier1_DeadCodeElimination(SSA::SSAModule& module);
    void Tier1_PeepholeOptimization(SSA::SSAModule& module);
    void Tier1_BoundsCheckElimination(SSA::SSAModule& module);
    void Tier1_BranchOptimization(SSA::SSAModule& module);
    void Tier1_FootprintCompression(SSA::SSAModule& module);
    
    // ========================================================================
    // TIER 2: AGGRESSIVE OPTIMIZATIONS (-O2)
    // ========================================================================
    
    void Tier2_LoopUnrolling(SSA::SSAModule& module);
    void Tier2_LoopFusion(SSA::SSAModule& module);
    void Tier2_Vectorization(SSA::SSAModule& module);
    void Tier2_LookaheadReordering(SSA::SSAModule& module);
    void Tier2_TailCallElimination(SSA::SSAModule& module);
    
 // ========================================================================
 // TIER 3: MAXIMUM OPTIMIZATIONS (-O3)
    // ========================================================================
    
    void Tier3_ProfileGuidedOptimization(SSA::SSAModule& module);
    void Tier3_LinkTimeOptimization(SSA::SSAModule& module);
    void Tier3_AutoFDO(SSA::SSAModule& module);
    void Tier3_AdaptiveTuning(SSA::SSAModule& module);
 void Tier3_Base12ArithmeticFusion(SSA::SSAModule& module);
    void Tier3_DozisecondTemporalSync(SSA::SSAModule& module);
    void Tier3_SpeculativeScheduling(SSA::SSAModule& module);
    
    // ========================================================================
    // INTERPROCEDURAL OPTIMIZATIONS
    // ========================================================================
    
    void IPO_AliasAnalysis(SSA::SSAModule& module);
    void IPO_EscapeAnalysis(SSA::SSAModule& module);
  void IPO_GlobalValueNumbering(SSA::SSAModule& module);
    void IPO_CFGPruning(SSA::SSAModule& module);
    void IPO_SSARebuild(SSA::SSAModule& module);
    void IPO_Devirtualization(SSA::SSAModule& module);
    void IPO_CacheColoring(SSA::SSAModule& module);
    void IPO_BranchPredictionModel(SSA::SSAModule& module);
    
    // ========================================================================
    // ADVANCED TECHNIQUES
    // ========================================================================
    
    // Superblock formation
    void FormSuperblocks(SSA::SSAModule& module);
  
    // Trace scheduling
    void PerformTraceScheduling(SSA::SSAModule& module);
    
    // Software pipelining
    void SoftwarePipelining(SSA::SSAModule& module);
    
    // Polyhedral optimization
    void PolyhedralOptimization(SSA::SSAModule& module);
    
    // Auto-parallelization
    void AutoParallelization(SSA::SSAModule& module);
    
    // Memory prefetching
    void InsertPrefetchInstructions(SSA::SSAModule& module);
    
    // ========================================================================
    // HELPER METHODS
    // ========================================================================
    
    // Loop detection and analysis
    struct Loop {
        SSA::SSABasicBlock* header;
 std::vector<SSA::SSABasicBlock*> blocks;
        std::vector<Loop*> nested_loops;
   int trip_count;
        bool is_vectorizable;
    };
    
    std::vector<Loop> DetectLoops(SSA::SSAFunction& func);
    bool IsVectorizable(const Loop& loop);
    int EstimateTripCount(const Loop& loop);
    
    // Dominance analysis
    void ComputeDominators(SSA::SSAFunction& func);
    bool Dominates(SSA::SSABasicBlock* a, SSA::SSABasicBlock* b);
    
    // Data dependency analysis
    bool HasDependency(SSA::SSAInstruction* a, SSA::SSAInstruction* b);
    
    // Cost model
    double EstimateSpeedup(const SSA::SSAModule& before, const SSA::SSAModule& after);
};

// ============================================================================
// VECTORIZATION ENGINE
// ============================================================================

class VectorizationEngine {
public:
    VectorizationEngine();
  
    // Target SIMD capabilities
    enum class SIMDTarget {
        SSE2,
        SSE4_2,
   AVX,
        AVX2,
AVX512
    };
    
    void SetTarget(SIMDTarget target);
    
    // Vectorize loops
    bool VectorizeLoop(SSA::SSAFunction& func, const AdvancedOptimizer::Loop& loop);
    
    // Vectorize straight-line code (SLP)
    void PerformSLPVectorization(SSA::SSAFunction& func);

private:
    SIMDTarget target_;
    int vector_width_;
    
    bool CanVectorize(const AdvancedOptimizer::Loop& loop);
    void GenerateVectorCode(SSA::SSAFunction& func, const AdvancedOptimizer::Loop& loop);
};

// ============================================================================
// REGISTER ALLOCATOR
// ============================================================================

class RegisterAllocator {
public:
    RegisterAllocator();
    
    // Perform register allocation
    void Allocate(SSA::SSAFunction& func);
    
    // Graph coloring
    void GraphColoringAllocation(SSA::SSAFunction& func);
    
    // Linear scan
    void LinearScanAllocation(SSA::SSAFunction& func);
    
    // Get allocation map
    std::unordered_map<SSA::SSAValue*, int> GetAllocation() const { return allocation_; }

private:
    std::unordered_map<SSA::SSAValue*, int> allocation_;
    
 void ComputeLiveRanges(SSA::SSAFunction& func);
    void BuildInterferenceGraph(SSA::SSAFunction& func);
};

// ============================================================================
// INSTRUCTION SCHEDULER
// ============================================================================

class InstructionScheduler {
public:
    InstructionScheduler();
    
    // Schedule instructions for pipelining
    void Schedule(SSA::SSABasicBlock& block);
    
    // List scheduling
    void ListScheduling(SSA::SSABasicBlock& block);
    
    // Software pipelining
    void SoftwarePipelining(const AdvancedOptimizer::Loop& loop);

private:
    struct InstructionNode {
        SSA::SSAInstruction* instr;
        std::vector<InstructionNode*> predecessors;
        std::vector<InstructionNode*> successors;
        int earliest_start;
        int latest_start;
    };
    
    std::vector<InstructionNode> BuildDAG(SSA::SSABasicBlock& block);
    int GetLatency(SSA::SSAInstruction* instr);
};

} // namespace AdvancedOptimization
} // namespace Snow
