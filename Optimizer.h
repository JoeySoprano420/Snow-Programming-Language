#pragma once

#include "../IR/IR.h"

namespace Snow {

// ============================================================================
// CIAM OPTIMIZER
// Contextual Inference Abstraction Macro System
// ============================================================================

class CIAMOptimizer {
public:
    CIAMOptimizer() = default;
    
    // Run all optimization passes
    void Optimize(IR::Module& module);
    
private:
    // Individual optimization passes
 void ConstantFolding(IR::Function& func);
    void DeadCodeElimination(IR::Function& func);
    void PeepholeOptimization(IR::Function& func);
    void LoopUnrolling(IR::Function& func);
    
    // Helper methods
    bool IsConstant(const IR::Operand& op);
 int64_t GetConstantValue(const IR::Operand& op);
};

} // namespace Snow
