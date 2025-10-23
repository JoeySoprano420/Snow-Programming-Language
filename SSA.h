#pragma once

#include "../Common/Types.h"
#include "../AST/AST.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Snow {
namespace SSA {

// ============================================================================
// SSA-BASED INTERMEDIATE REPRESENTATION
// ============================================================================

// SSA Value - represents a single-assignment value
class SSAValue {
public:
    enum class Kind {
        Register,
        Constant,
      Parameter,
  GlobalVariable
    };
 
    SSAValue(Kind kind, int id) : kind_(kind), id_(id), type_(nullptr) {}
    
    Kind GetKind() const { return kind_; }
    int GetID() const { return id_; }
    void SetType(std::shared_ptr<void> type) { type_ = type; }
    
    std::string GetName() const;

private:
    Kind kind_;
    int id_;
    std::shared_ptr<void> type_;
};

// SSA Instruction
class SSAInstruction {
public:
    enum class OpCode {
        // Arithmetic
        Add, Sub, Mul, Div, Mod,
     // Logical
        And, Or, Xor, Not,
    // Comparison
     Eq, Ne, Lt, Le, Gt, Ge,
     // Memory
        Load, Store, Alloca,
        // Control Flow
     Br, CondBr, Ret, Call,
 // SSA-specific
        Phi,
        // SIMD/Vector
    VectorLoad, VectorStore, VectorAdd, VectorMul,
        // Dodecagram-specific
        DodecConvert, DodecArithmetic,
   // Duration-specific
        DurationCreate, DurationCompare
    };
    
    SSAInstruction(OpCode op) : opcode_(op), result_(nullptr) {}
    
    OpCode GetOpCode() const { return opcode_; }
    SSAValue* GetResult() const { return result_; }
    void SetResult(SSAValue* val) { result_ = val; }
    
    void AddOperand(SSAValue* val) { operands_.push_back(val); }
    const std::vector<SSAValue*>& GetOperands() const { return operands_; }
    
    // Debug information
    void SetDebugInfo(const SourceLocation& loc) { debug_loc_ = loc; }
    const SourceLocation& GetDebugInfo() const { return debug_loc_; }
    
    // Vector metadata for SIMD operations
    void SetVectorWidth(int width) { vector_width_ = width; }
    int GetVectorWidth() const { return vector_width_; }

private:
    OpCode opcode_;
    SSAValue* result_;
    std::vector<SSAValue*> operands_;
    SourceLocation debug_loc_;
    int vector_width_ = 1; // 1 = scalar, >1 = vector
};

// SSA Basic Block
class SSABasicBlock {
public:
 explicit SSABasicBlock(const std::string& name) : name_(name) {}
    
    const std::string& GetName() const { return name_; }
    
    void AddInstruction(std::unique_ptr<SSAInstruction> instr) {
        instructions_.push_back(std::move(instr));
    }
    
 const std::vector<std::unique_ptr<SSAInstruction>>& GetInstructions() const {
        return instructions_;
 }
    
    void AddPredecessor(SSABasicBlock* pred) { predecessors_.push_back(pred); }
    void AddSuccessor(SSABasicBlock* succ) { successors_.push_back(succ); }
    
    const std::vector<SSABasicBlock*>& GetPredecessors() const { return predecessors_; }
    const std::vector<SSABasicBlock*>& GetSuccessors() const { return successors_; }

private:
    std::string name_;
  std::vector<std::unique_ptr<SSAInstruction>> instructions_;
    std::vector<SSABasicBlock*> predecessors_;
    std::vector<SSABasicBlock*> successors_;
};

// SSA Function
class SSAFunction {
public:
    explicit SSAFunction(const std::string& name) : name_(name) {}
    
    const std::string& GetName() const { return name_; }
    
    SSABasicBlock* CreateBasicBlock(const std::string& name) {
        auto block = std::make_unique<SSABasicBlock>(name);
        auto* ptr = block.get();
        blocks_.push_back(std::move(block));
        return ptr;
    }
    
    const std::vector<std::unique_ptr<SSABasicBlock>>& GetBlocks() const {
    return blocks_;
    }
    
    SSAValue* CreateValue(SSAValue::Kind kind) {
        int id = next_value_id_++;
        auto value = std::make_unique<SSAValue>(kind, id);
        auto* ptr = value.get();
      values_.push_back(std::move(value));
        return ptr;
    }

private:
    std::string name_;
    std::vector<std::unique_ptr<SSABasicBlock>> blocks_;
    std::vector<std::unique_ptr<SSAValue>> values_;
int next_value_id_ = 0;
};

// SSA Module
class SSAModule {
public:
    SSAModule() = default;
    
    SSAFunction* CreateFunction(const std::string& name) {
        auto func = std::make_unique<SSAFunction>(name);
        auto* ptr = func.get();
        functions_.push_back(std::move(func));
        return ptr;
    }
    
    const std::vector<std::unique_ptr<SSAFunction>>& GetFunctions() const {
        return functions_;
    }

private:
    std::vector<std::unique_ptr<SSAFunction>> functions_;
};

// ============================================================================
// SSA BUILDER - Converts AST to SSA form
// ============================================================================

class SSABuilder {
public:
    SSABuilder();
    
    // Build SSA IR from AST
    std::unique_ptr<SSAModule> BuildFromAST(const AST::Program& program);
    
    // Emit debug information
    void SetEmitDebugInfo(bool emit) { emit_debug_info_ = emit; }
    
private:
    bool emit_debug_info_;
    SSAModule* current_module_;
    SSAFunction* current_function_;
    SSABasicBlock* current_block_;
    
    std::unordered_map<std::string, SSAValue*> symbol_table_;
 
    void BuildFunction(const AST::FunctionDecl& func);
    void BuildStatement(const AST::Statement& stmt);
    SSAValue* BuildExpression(const AST::Expression& expr);
    
    // SSA-specific helpers
    void InsertPhiNodes();
  void RenameVariables();
};

} // namespace SSA
} // namespace Snow
