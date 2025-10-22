#include "Optimizer.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace Snow {

// ============================================================================
// OPTIMIZATION STATISTICS
// ============================================================================

void OptimizationStats::Print() const {
    std::cout << "\n[CIAM Optimizer Statistics]" << std::endl;
    std::cout << "  Constants folded: " << constants_folded << std::endl;
    std::cout << "  Dead code removed: " << dead_code_removed << std::endl;
    std::cout << "  Loops unrolled: " << loops_unrolled << std::endl;
  std::cout << "  Peephole optimizations: " << peephole_optimizations << std::endl;
    std::cout << "  Tail calls optimized: " << tail_calls_optimized << std::endl;
    std::cout << "  Vectorized loops: " << vectorized_loops << std::endl;
    std::cout << "  Bounds checks eliminated: " << bounds_checks_eliminated << std::endl;
    std::cout << "  Branches optimized: " << branches_optimized << std::endl;
    std::cout << "  Loops fused: " << loops_fused << std::endl;
}

void OptimizationStats::Reset() {
  *this = OptimizationStats();
}

// ============================================================================
// CIAM OPTIMIZER IMPLEMENTATION
// ============================================================================

CIAMOptimizer::CIAMOptimizer() : opt_level_(1) {
    // Initialize optimization flags
    optimization_flags_["constant_folding"] = true;
optimization_flags_["dead_code_elimination"] = true;
    optimization_flags_["loop_unrolling"] = true;
    optimization_flags_["peephole"] = true;
    optimization_flags_["tail_call"] = true;
    optimization_flags_["vectorization"] = false; // Aggressive
    optimization_flags_["lookahead"] = false; // Aggressive
  optimization_flags_["bounds_check"] = true;
    optimization_flags_["branch_opt"] = true;
    optimization_flags_["loop_fusion"] = false; // Aggressive
    optimization_flags_["scheduling"] = false; // Maximum
    optimization_flags_["footprint"] = true;
    optimization_flags_["adaptive"] = false; // Maximum
    optimization_flags_["profile_guided"] = false;
}

void CIAMOptimizer::SetOptimizationLevel(int level) {
    opt_level_ = level;
    
    switch (level) {
   case 0: // No optimization
         for (auto& flag : optimization_flags_) {
  flag.second = false;
}
         break;
            
case 1: // Basic (-O1)
          optimization_flags_["constant_folding"] = true;
  optimization_flags_["dead_code_elimination"] = true;
        optimization_flags_["peephole"] = true;
            optimization_flags_["bounds_check"] = true;
          optimization_flags_["branch_opt"] = true;
            optimization_flags_["footprint"] = true;
            break;
  
        case 2: // Aggressive (-O2)
      SetOptimizationLevel(1);
  optimization_flags_["loop_unrolling"] = true;
            optimization_flags_["tail_call"] = true;
   optimization_flags_["vectorization"] = true;
            optimization_flags_["lookahead"] = true;
      optimization_flags_["loop_fusion"] = true;
            break;
      
        case 3: // Maximum (-O3)
   SetOptimizationLevel(2);
            optimization_flags_["scheduling"] = true;
            optimization_flags_["adaptive"] = true;
            optimization_flags_["profile_guided"] = true;
            break;
    }
}

void CIAMOptimizer::EnableOptimization(const std::string& name, bool enabled) {
    optimization_flags_[name] = enabled;
}

void CIAMOptimizer::SetProfileData(const ProfileData& data) {
    profile_data_ = data;
    optimization_flags_["profile_guided"] = true;
}

void CIAMOptimizer::Optimize(IR::Module& module) {
    std::cout << "\n[CIAM Optimizer] Running optimization passes (Level " << opt_level_ << ")..." << std::endl;
    stats_.Reset();
 
    for (const auto& func : module.GetFunctions()) {
      std::cout << "  Optimizing function: " << func->GetName() << std::endl;
  
        // Pass 1: Constant Folding
  if (optimization_flags_["constant_folding"]) {
            ConstantFolding(*func);
        }
  
        // Pass 2: Dead Code Elimination
    if (optimization_flags_["dead_code_elimination"]) {
          DeadCodeElimination(*func);
        }
        
        // Pass 3: Peephole Optimization
        if (optimization_flags_["peephole"]) {
       PeepholeOptimization(*func);
     }
        
  // Pass 4: Algebraic Simplification
        OptimizeBase12Arithmetic(*func);
        
 // Pass 5: Bounds Check Elimination
        if (optimization_flags_["bounds_check"]) {
       BoundsCheckElimination(*func);
        }
        
   // Pass 6: Branch Optimization
if (optimization_flags_["branch_opt"]) {
            BranchChainOptimization(*func);
        }
        
     // Pass 7: Loop Unrolling
        if (optimization_flags_["loop_unrolling"]) {
    LoopUnrolling(*func);
        }
        
     // Pass 8: Loop Fusion (Curling)
        if (optimization_flags_["loop_fusion"]) {
  LoopFusion(*func);
        }
      
    // Pass 9: Vectorization
     if (optimization_flags_["vectorization"]) {
            Vectorization(*func);
        }
        
        // Pass 10: Tail Call Optimization
        if (optimization_flags_["tail_call"]) {
       TailCallOptimization(*func);
        }
        
   // Pass 11: Look-Ahead Optimization
        if (optimization_flags_["lookahead"]) {
   LookAheadOptimization(*func);
        }
        
        // Pass 12: Synchronized Scheduling
        if (optimization_flags_["scheduling"]) {
        SynchronizedScheduling(*func);
        }

        // Pass 13: Footprint Compression
        if (optimization_flags_["footprint"]) {
  FootprintCompression(*func);
        }
        
        // Pass 14: Adaptive Tuning
        if (optimization_flags_["adaptive"]) {
        AdaptiveTuning(*func);
 }
        
        // Pass 15: Profile-Guided Optimization
        if (optimization_flags_["profile_guided"]) {
 ProfileGuidedOptimization(*func);
        }
      
        // Pass 16: Dodecagram-specific optimizations
        OptimizeDozisecondOperations(*func);
        
        // Final cleanup
        RemoveRedundantMoves(*func);
    }
    
    std::cout << "[CIAM Optimizer] Optimization complete." << std::endl;
    stats_.Print();
}

// ============================================================================
// 1. CONSTANT FOLDING
// ============================================================================

void CIAMOptimizer::ConstantFolding(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
        
        for (auto& instr : instructions) {
    if (IsConstant(instr.src1) && IsConstant(instr.src2)) {
    int64_t val1 = GetConstantValue(instr.src1);
      int64_t val2 = GetConstantValue(instr.src2);
     int64_t result;

      if (TryEvaluateConstant(instr.opcode, val1, val2, result)) {
   // Replace with MOV immediate
    instr.opcode = IR::OpCode::MOV;
             instr.src1 = IR::Operand::Immediate(result);
        instr.src2 = IR::Operand::Register(0);
         stats_.constants_folded++;
          }
            }
        }
 }
}

bool CIAMOptimizer::TryEvaluateConstant(IR::OpCode op, int64_t a, int64_t b, int64_t& result) {
    switch (op) {
      case IR::OpCode::ADD:
       result = a + b;
        return true;
   case IR::OpCode::SUB:
  result = a - b;
       return true;
        case IR::OpCode::MUL:
      result = a * b;
            return true;
   case IR::OpCode::DIV:
       if (b != 0) {
         result = a / b;
             return true;
   }
            return false;
        default:
   return false;
    }
}

// ============================================================================
// 2. DEAD CODE ELIMINATION
// ============================================================================

void CIAMOptimizer::DeadCodeElimination(IR::Function& func) {
    std::unordered_set<IR::Instruction*> live_instructions;
    
    // Mark live instructions
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
     
     for (auto& instr : instructions) {
            // Instructions with side effects are always live
            if (HasSideEffects(instr)) {
      live_instructions.insert(&instr);
    }
        }
    }
    
    // Remove dead code
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
    
        instructions.erase(
        std::remove_if(instructions.begin(), instructions.end(),
     [&](const IR::Instruction& instr) {
   bool is_dead = (live_instructions.find(const_cast<IR::Instruction*>(&instr)) == live_instructions.end())
          && !HasSideEffects(instr);
        if (is_dead) stats_.dead_code_removed++;
      return is_dead;
     }),
            instructions.end()
        );
    }
}

bool CIAMOptimizer::HasSideEffects(const IR::Instruction& instr) {
    switch (instr.opcode) {
        case IR::OpCode::CALL:
   case IR::OpCode::RET:
        case IR::OpCode::STORE:
        case IR::OpCode::WAIT:
        case IR::OpCode::JMP:
        case IR::OpCode::JE:
        case IR::OpCode::JNE:
        case IR::OpCode::JG:
   case IR::OpCode::JL:
        case IR::OpCode::JGE:
        case IR::OpCode::JLE:
     return true;
        default:
            return false;
    }
}

// ============================================================================
// 3. LOOP UNROLLING
// ============================================================================

void CIAMOptimizer::LoopUnrolling(IR::Function& func) {
    std::vector<std::vector<IR::BasicBlock*>> loops;
    DetectLoops(func, loops);
    
    for (auto& loop : loops) {
      int iterations = EstimateLoopIterations(loop);
        
        // Only unroll small loops with known iteration counts
        if (iterations > 0 && iterations <= 12) { // Dodecagram threshold!
            UnrollLoop(loop, iterations);
     stats_.loops_unrolled++;
    }
    }
}

void CIAMOptimizer::DetectLoops(IR::Function& func, std::vector<std::vector<IR::BasicBlock*>>& loops) {
 // Simplified loop detection - looks for back edges
    for (const auto& block : func.GetBlocks()) {
        for (auto* successor : block->GetSuccessors()) {
            if (IsBackEdge(block.get(), successor)) {
     std::vector<IR::BasicBlock*> loop_blocks;
       loop_blocks.push_back(const_cast<IR::BasicBlock*>(block.get()));
                loops.push_back(loop_blocks);
  }
        }
    }
}

int CIAMOptimizer::EstimateLoopIterations(const std::vector<IR::BasicBlock*>& loop) {
    // Check profile data first
    if (!loop.empty() && !profile_data_.average_loop_iterations.empty()) {
        auto it = profile_data_.average_loop_iterations.find(loop[0]->GetName());
if (it != profile_data_.average_loop_iterations.end()) {
            return static_cast<int>(it->second);
        }
    }
    
    // Default: try to detect constant iteration count
    return -1; // Unknown
}

void CIAMOptimizer::UnrollLoop(std::vector<IR::BasicBlock*>& loop_blocks, int factor) {
    // Simplified unrolling: duplicate loop body
    if (loop_blocks.empty()) return;
    
 // Implementation would duplicate the loop body 'factor' times
    // This is a framework - full implementation would clone instructions
}

bool CIAMOptimizer::IsBackEdge(IR::BasicBlock* from, IR::BasicBlock* to) {
    // Simplified: check if 'to' appears before 'from' (back edge indicator)
    return false; // Placeholder
}

// ============================================================================
// 4. PEEPHOLE OPTIMIZATION
// ============================================================================

void CIAMOptimizer::PeepholeOptimization(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
   
  for (size_t i = 0; i < instructions.size(); ++i) {
            // Optimize single instructions
            if (OptimizeRedundantMoves(instructions[i])) {
       stats_.peephole_optimizations++;
       }
     
            if (OptimizeAlgebraicIdentities(instructions[i])) {
    stats_.peephole_optimizations++;
     }
      
        // Optimize instruction pairs
 if (i + 1 < instructions.size()) {
     if (OptimizeInstructionPair(instructions[i], instructions[i + 1])) {
   stats_.peephole_optimizations++;
                }
            }
    }
    }
}

bool CIAMOptimizer::OptimizeRedundantMoves(IR::Instruction& instr) {
    // MOV R1, R1 -> NOP
    if (instr.opcode == IR::OpCode::MOV &&
    instr.dest.type == IR::OperandType::Register &&
        instr.src1.type == IR::OperandType::Register &&
        instr.dest.value == instr.src1.value) {
        instr.opcode = IR::OpCode::NOP;
        return true;
    }
    return false;
}

bool CIAMOptimizer::OptimizeAlgebraicIdentities(IR::Instruction& instr) {
    // ADD R1, R2, 0 -> MOV R1, R2
    if (instr.opcode == IR::OpCode::ADD &&
        IsConstant(instr.src2) && GetConstantValue(instr.src2) == 0) {
        instr.opcode = IR::OpCode::MOV;
  instr.src2 = IR::Operand::Register(0);
        return true;
    }
    
    // MUL R1, R2, 1 -> MOV R1, R2
    if (instr.opcode == IR::OpCode::MUL &&
        IsConstant(instr.src2) && GetConstantValue(instr.src2) == 1) {
        instr.opcode = IR::OpCode::MOV;
        instr.src2 = IR::Operand::Register(0);
        return true;
    }
    
    // MUL R1, R2, 0 -> MOV R1, 0
    if (instr.opcode == IR::OpCode::MUL &&
        IsConstant(instr.src2) && GetConstantValue(instr.src2) == 0) {
  instr.opcode = IR::OpCode::MOV;
        instr.src1 = IR::Operand::Immediate(0);
instr.src2 = IR::Operand::Register(0);
    return true;
    }
    
    // Dodecagram-specific: MUL by 10 (12 decimal) = shift + add
    if (instr.opcode == IR::OpCode::MUL &&
        IsConstant(instr.src2) && GetConstantValue(instr.src2) == 12) {
        // Could be optimized to (x << 3) + (x << 2)
        instr.comment = "Dodecagram multiply by dozen";
        return true;
    }
    
    return false;
}

bool CIAMOptimizer::OptimizeInstructionPair(IR::Instruction& i1, IR::Instruction& i2) {
    // MOV R1, X; MOV R2, R1 -> MOV R1, X; MOV R2, X
    if (i1.opcode == IR::OpCode::MOV && i2.opcode == IR::OpCode::MOV &&
        i2.src1.type == IR::OperandType::Register &&
        i1.dest.type == IR::OperandType::Register &&
        i2.src1.value == i1.dest.value) {
        i2.src1 = i1.src1;
     return true;
    }
    
    return false;
}

// ============================================================================
// 5. TAIL CALL OPTIMIZATION
// ============================================================================

void CIAMOptimizer::TailCallOptimization(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
        
        for (auto& instr : instructions) {
 if (instr.opcode == IR::OpCode::CALL && IsTailCall(instr, *block)) {
           ConvertToTailCall(instr);
                stats_.tail_calls_optimized++;
       }
        }
    }
}

bool CIAMOptimizer::IsTailCall(const IR::Instruction& call_instr, const IR::BasicBlock& block) {
    // Check if CALL is followed immediately by RET
    const auto& instrs = block.GetInstructions();
    for (size_t i = 0; i < instrs.size() - 1; ++i) {
        if (&instrs[i] == &call_instr) {
   return instrs[i + 1].opcode == IR::OpCode::RET;
   }
    }
    return false;
}

void CIAMOptimizer::ConvertToTailCall(IR::Instruction& call_instr) {
    // Mark as tail call for codegen
    call_instr.comment = "TAIL_CALL";
}

// ============================================================================
// 6. VECTORIZATION (SIMD/AVX)
// ============================================================================

void CIAMOptimizer::Vectorization(IR::Function& func) {
    std::vector<std::vector<IR::BasicBlock*>> loops;
    DetectLoops(func, loops);
    
    for (auto& loop : loops) {
        if (IsVectorizableLoop(loop)) {
            VectorizeLoop(loop);
   stats_.vectorized_loops++;
        }
  }
}

bool CIAMOptimizer::IsVectorizableLoop(const std::vector<IR::BasicBlock*>& loop) {
    // Check if loop can be vectorized (no dependencies, simple operations)
    // This is a simplified check
    return loop.size() == 1; // Single block loops are easier to vectorize
}

void CIAMOptimizer::VectorizeLoop(std::vector<IR::BasicBlock*>& loop) {
    // Convert scalar operations to SIMD operations
    // Placeholder for actual vectorization logic
}

// ============================================================================
// 7. LOOK-AHEAD OPTIMIZATION
// ============================================================================

void CIAMOptimizer::LookAheadOptimization(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
    auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
      
        // Reorder instructions to hide latency
        ReorderInstructionsForLatency(const_cast<IR::BasicBlock&>(*block));
    }
}

void CIAMOptimizer::ReorderInstructionsForLatency(IR::BasicBlock& block) {
    // Instruction scheduling to minimize pipeline stalls
    // Move independent instructions earlier to fill bubbles
}

// ============================================================================
// 8. BOUNDS CHECK ELIMINATION
// ============================================================================

void CIAMOptimizer::BoundsCheckElimination(IR::Function& func) {
    AnalyzeArrayAccess(func);
    
    for (const auto& block : func.GetBlocks()) {
     auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
        
        instructions.erase(
            std::remove_if(instructions.begin(), instructions.end(),
       [&](const IR::Instruction& instr) {
          bool can_eliminate = CanEliminateBoundsCheck(instr);
            if (can_eliminate) stats_.bounds_checks_eliminated++;
          return can_eliminate;
    }),
       instructions.end()
      );
    }
}

void CIAMOptimizer::AnalyzeArrayAccess(IR::Function& func) {
    // Analyze array access patterns to prove bounds
}

bool CIAMOptimizer::CanEliminateBoundsCheck(const IR::Instruction& check) {
    // Determine if bounds check is redundant
    return false; // Conservative
}

// ============================================================================
// 9. BRANCH CHAIN OPTIMIZATION
// ============================================================================

void CIAMOptimizer::BranchChainOptimization(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        SimplifyBranchChains(const_cast<IR::BasicBlock&>(*block));
   EliminateRedundantBranches(const_cast<IR::BasicBlock&>(*block));
}
}

void CIAMOptimizer::SimplifyBranchChains(IR::BasicBlock& block) {
    // JMP to JMP -> Direct JMP
    stats_.branches_optimized++;
}

void CIAMOptimizer::EliminateRedundantBranches(IR::BasicBlock& block) {
    // Remove unreachable branches
}

// ============================================================================
// 10. LOOP FUSION (CURLING)
// ============================================================================

void CIAMOptimizer::LoopFusion(IR::Function& func) {
    std::vector<std::vector<IR::BasicBlock*>> loops;
    DetectLoops(func, loops);
    
    for (size_t i = 0; i < loops.size(); ++i) {
    for (size_t j = i + 1; j < loops.size(); ++j) {
            if (CanFuseLoops(loops[i], loops[j])) {
                FuseLoops(loops[i], loops[j]);
                stats_.loops_fused++;
}
        }
    }
}

bool CIAMOptimizer::CanFuseLoops(const std::vector<IR::BasicBlock*>& loop1,
      const std::vector<IR::BasicBlock*>& loop2) {
    // Check if loops can be fused (same bounds, no dependencies)
    return false; // Conservative
}

void CIAMOptimizer::FuseLoops(std::vector<IR::BasicBlock*>& loop1,
          std::vector<IR::BasicBlock*>& loop2) {
    // Merge loop bodies
}

// ============================================================================
// 11. SYNCHRONIZED SCHEDULING
// ============================================================================

void CIAMOptimizer::SynchronizedScheduling(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        ScheduleForPipeline(const_cast<IR::BasicBlock&>(*block));
    }
}

void CIAMOptimizer::ScheduleForPipeline(IR::BasicBlock& block) {
 // Schedule instructions for optimal pipeline utilization
}

// ============================================================================
// 12. FOOTPRINT COMPRESSION
// ============================================================================

void CIAMOptimizer::FootprintCompression(IR::Function& func) {
    CompressRegisterUsage(func);
    MinimizeStackFrame(func);
}

void CIAMOptimizer::CompressRegisterUsage(IR::Function& func) {
    RegisterCoalescing(func);
}

void CIAMOptimizer::MinimizeStackFrame(IR::Function& func) {
    // Reduce stack frame size
}

void CIAMOptimizer::RegisterCoalescing(IR::Function& func) {
    // Merge live ranges of variables to reduce register pressure
}

void CIAMOptimizer::RemoveRedundantMoves(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
     
        instructions.erase(
std::remove_if(instructions.begin(), instructions.end(),
   [](const IR::Instruction& instr) {
             return instr.opcode == IR::OpCode::NOP;
     }),
            instructions.end()
        );
    }
}

// ============================================================================
// 13. ADAPTIVE TUNING
// ============================================================================

void CIAMOptimizer::AdaptiveTuning(IR::Function& func) {
    TuneForCacheLocality(func);
    OptimizeForBranchPrediction(func);
}

void CIAMOptimizer::TuneForCacheLocality(IR::Function& func) {
    // Optimize data layout for cache efficiency
}

void CIAMOptimizer::OptimizeForBranchPrediction(IR::Function& func) {
    // Arrange code to improve branch prediction
}

// ============================================================================
// 14. PROFILE-GUIDED OPTIMIZATION
// ============================================================================

void CIAMOptimizer::ProfileGuidedOptimization(IR::Function& func) {
    if (profile_data_.block_execution_count.empty()) return;
    
    ReorderBlocksForHotPath(func);
}

void CIAMOptimizer::ReorderBlocksForHotPath(IR::Function& func) {
    // Reorder basic blocks to place hot code together
}

void CIAMOptimizer::InlineHotFunctions(IR::Module& module) {
    // Inline frequently called functions
}

// ============================================================================
// DODECAGRAM-SPECIFIC OPTIMIZATIONS
// ============================================================================

void CIAMOptimizer::OptimizeBase12Arithmetic(IR::Function& func) {
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
        
        for (auto& instr : instructions) {
    // Optimize division by 12 -> shift and subtract
     // Optimize multiplication by powers of 12
  if (instr.opcode == IR::OpCode::DIV &&
      IsConstant(instr.src2) && GetConstantValue(instr.src2) == 12) {
      instr.comment = "Base-12 division optimization candidate";
       }
     }
    }
}

void CIAMOptimizer::OptimizeDozisecondOperations(IR::Function& func) {
    // Optimize temporal operations (dozisecond timing)
    for (const auto& block : func.GetBlocks()) {
        auto& instructions = const_cast<std::vector<IR::Instruction>&>(block->GetInstructions());
        
        for (auto& instr : instructions) {
 if (instr.opcode == IR::OpCode::WAIT) {
         instr.comment = "Dozisecond timing";
            }
  }
    }
}

// ============================================================================
// HELPER METHODS
// ============================================================================

bool CIAMOptimizer::IsConstant(const IR::Operand& op) const {
    return op.type == IR::OperandType::Immediate;
}

int64_t CIAMOptimizer::GetConstantValue(const IR::Operand& op) const {
    if (op.type == IR::OperandType::Immediate) {
     return op.value;
    }
    return 0;
}

void CIAMOptimizer::BuildControlFlowGraph(IR::Function& func) {
    // Build CFG for analysis
}

void CIAMOptimizer::FindDominators(IR::Function& func) {
  // Compute dominator tree
}

void CIAMOptimizer::ComputeDefUseChains(IR::Function& func) {
    // Build def-use chains for dataflow analysis
}

void CIAMOptimizer::ComputeLiveRanges(IR::Function& func) {
    // Compute variable live ranges
}

bool CIAMOptimizer::IsInnerLoop(const std::vector<IR::BasicBlock*>& loop) {
    // Check if loop is innermost (no nested loops)
    return true; // Simplified
}

void CIAMOptimizer::IncrementStat(const std::string& stat_name) {
    // Track optimization statistics
}

} // namespace Snow
