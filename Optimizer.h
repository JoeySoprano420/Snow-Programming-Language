#pragma once

#include "../IR/IR.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Snow {

// ============================================================================
// OPTIMIZATION STATISTICS
// ============================================================================

struct OptimizationStats {
    int constants_folded = 0;
    int dead_code_removed = 0;
    int loops_unrolled = 0;
    int peephole_optimizations = 0;
    int tail_calls_optimized = 0;
    int vectorized_loops = 0;
    int bounds_checks_eliminated = 0;
    int branches_optimized = 0;
    int loops_fused = 0;
    
    void Print() const;
    void Reset();
};

// ============================================================================
// PROFILE DATA
// ============================================================================

struct ProfileData {
    std::unordered_map<std::string, int> block_execution_count;
    std::unordered_map<std::string, int> branch_taken_count;
 std::unordered_map<std::string, double> average_loop_iterations;
};

// ============================================================================
// CIAM OPTIMIZER
// Contextual Inference Abstraction Macro System
// ============================================================================

class CIAMOptimizer {
public:
    CIAMOptimizer();
    
    // Main optimization entry point
    void Optimize(IR::Module& module);
    
    // Enable/disable specific optimizations
    void EnableOptimization(const std::string& name, bool enabled);
    void SetOptimizationLevel(int level); // 0=none, 1=basic, 2=aggressive, 3=maximum
  
    // Profile-guided optimization
    void SetProfileData(const ProfileData& data);
    
    // Get statistics
    const OptimizationStats& GetStats() const { return stats_; }
    
private:
    OptimizationStats stats_;
    ProfileData profile_data_;
    int opt_level_;
  
    std::unordered_map<std::string, bool> optimization_flags_;
    
    // ========================================================================
    // OPTIMIZATION PASSES
    // ========================================================================
    
    // 1. Constant Folding
    void ConstantFolding(IR::Function& func);
    void FoldConstantExpression(IR::Instruction& instr);
    
    // 2. Dead Code Elimination
    void DeadCodeElimination(IR::Function& func);
    void MarkLiveInstructions(IR::Function& func, std::unordered_set<IR::Instruction*>& live);
    void RemoveDeadCode(IR::Function& func, const std::unordered_set<IR::Instruction*>& live);
  
    // 3. Loop Unrolling
  void LoopUnrolling(IR::Function& func);
    bool DetectLoop(IR::BasicBlock* block, std::vector<IR::BasicBlock*>& loop_blocks);
    void UnrollLoop(std::vector<IR::BasicBlock*>& loop_blocks, int factor);
 
    // 4. Peephole Optimization
    void PeepholeOptimization(IR::Function& func);
    bool OptimizeInstructionPair(IR::Instruction& i1, IR::Instruction& i2);
    bool OptimizeRedundantMoves(IR::Instruction& instr);
    bool OptimizeAlgebraicIdentities(IR::Instruction& instr);
    
    // 5. Tail Call Optimization
    void TailCallOptimization(IR::Function& func);
    bool IsTailCall(const IR::Instruction& call_instr, const IR::BasicBlock& block);
    void ConvertToTailCall(IR::Instruction& call_instr);
    
    // 6. Vectorization (SIMD/AVX)
    void Vectorization(IR::Function& func);
    bool IsVectorizableLoop(const std::vector<IR::BasicBlock*>& loop);
    void VectorizeLoop(std::vector<IR::BasicBlock*>& loop);
    
    // 7. Look-Ahead Optimization
    void LookAheadOptimization(IR::Function& func);
    void ReorderInstructionsForLatency(IR::BasicBlock& block);
    
    // 8. Bounds Check Elimination
    void BoundsCheckElimination(IR::Function& func);
    void AnalyzeArrayAccess(IR::Function& func);
    bool CanEliminateBoundsCheck(const IR::Instruction& check);
    
    // 9. Branch Chain Optimization
    void BranchChainOptimization(IR::Function& func);
    void SimplifyBranchChains(IR::BasicBlock& block);
    void EliminateRedundantBranches(IR::BasicBlock& block);
    
    // 10. Curling (Loop Fusion)
    void LoopFusion(IR::Function& func);
    bool CanFuseLoops(const std::vector<IR::BasicBlock*>& loop1,
      const std::vector<IR::BasicBlock*>& loop2);
 void FuseLoops(std::vector<IR::BasicBlock*>& loop1,
               std::vector<IR::BasicBlock*>& loop2);
    
    // 11. Synchronized Scheduling
    void SynchronizedScheduling(IR::Function& func);
 void ScheduleForPipeline(IR::BasicBlock& block);
    
    // 12. Footprint Compression
    void FootprintCompression(IR::Function& func);
    void CompressRegisterUsage(IR::Function& func);
 void MinimizeStackFrame(IR::Function& func);
    
  // 13. Adaptive Tuning
    void AdaptiveTuning(IR::Function& func);
    void TuneForCacheLocality(IR::Function& func);
    void OptimizeForBranchPrediction(IR::Function& func);
    
    // 14. Profile-Guided Optimization
    void ProfileGuidedOptimization(IR::Function& func);
    void ReorderBlocksForHotPath(IR::Function& func);
    void InlineHotFunctions(IR::Module& module);
    
    // ========================================================================
    // HELPER METHODS
    // ========================================================================
    
    // Constant analysis
    bool IsConstant(const IR::Operand& op) const;
    int64_t GetConstantValue(const IR::Operand& op) const;
    bool TryEvaluateConstant(IR::OpCode op, int64_t a, int64_t b, int64_t& result);

    // Control flow analysis
    void BuildControlFlowGraph(IR::Function& func);
    void FindDominators(IR::Function& func);
    bool IsBackEdge(IR::BasicBlock* from, IR::BasicBlock* to);
  
    // Data flow analysis
    void ComputeDefUseChains(IR::Function& func);
    void ComputeLiveRanges(IR::Function& func);
    bool HasSideEffects(const IR::Instruction& instr);
    
    // Loop analysis
    void DetectLoops(IR::Function& func, std::vector<std::vector<IR::BasicBlock*>>& loops);
  int EstimateLoopIterations(const std::vector<IR::BasicBlock*>& loop);
    bool IsInnerLoop(const std::vector<IR::BasicBlock*>& loop);
    
    // Register allocation
    void RegisterCoalescing(IR::Function& func);
    void RemoveRedundantMoves(IR::Function& func);
    
    // Dodecagram-specific optimizations
    void OptimizeBase12Arithmetic(IR::Function& func);
    void OptimizeDozisecondOperations(IR::Function& func);
    
    // Statistics tracking
    void IncrementStat(const std::string& stat_name);
};

} // namespace Snow
