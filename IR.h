#pragma once

#include "../AST/AST.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace Snow {
namespace IR {

// ============================================================================
// IR INSTRUCTION TYPES
// ============================================================================

enum class OpCode {
    // Data movement
  MOV,        // Move value
    LOAD,       // Load from memory
    STORE,      // Store to memory
    
    // Arithmetic
 ADD,        // Addition
    SUB,        // Subtraction
    MUL,        // Multiplication
    DIV,        // Division
    
    // Comparison
    CMP,        // Compare
    
    // Control flow
    JMP,        // Unconditional jump
    JE, // Jump if equal
JNE,        // Jump if not equal
    JG,       // Jump if greater
    JL,         // Jump if less
    JGE,        // Jump if greater or equal
    JLE,      // Jump if less or equal
    
    // Function calls
    CALL,   // Call function
 RET,        // Return
    
    // Temporal operations
    WAIT,       // Wait for duration
    DODECAP,    // Dodecagram capture
    SAMPLE,     // Sample value
    DELTA,      // Compute delta
    
    // Special
  LABEL,      // Label marker
 NOP   // No operation
};

// ============================================================================
// IR OPERAND
// ============================================================================

enum class OperandType {
    Register,
    Immediate,
    Memory,
    Label
};

struct Operand {
    OperandType type;
    int64_t value;
    std::string label;
  
    static Operand Register(int reg) {
        Operand op;
    op.type = OperandType::Register;
        op.value = reg;
  return op;
    }
    
    static Operand Immediate(int64_t val) {
        Operand op;
        op.type = OperandType::Immediate;
        op.value = val;
        return op;
    }
  
    static Operand Label(const std::string& lbl) {
 Operand op;
        op.type = OperandType::Label;
     op.label = lbl;
        return op;
    }
    
    std::string ToString() const;
};

// ============================================================================
// IR INSTRUCTION
// ============================================================================

struct Instruction {
    OpCode opcode;
 Operand dest;
    Operand src1;
    Operand src2;
    std::string comment;
    
    Instruction(OpCode op) : opcode(op) {}
    Instruction(OpCode op, Operand d) : opcode(op), dest(d) {}
    Instruction(OpCode op, Operand d, Operand s1) : opcode(op), dest(d), src1(s1) {}
    Instruction(OpCode op, Operand d, Operand s1, Operand s2) 
 : opcode(op), dest(d), src1(s1), src2(s2) {}
    
    std::string ToString() const;
};

// ============================================================================
// IR BASIC BLOCK
// ============================================================================

class BasicBlock {
public:
    explicit BasicBlock(const std::string& name) : name_(name) {}
    
    void AddInstruction(const Instruction& instr) { instructions_.push_back(instr); }
    const std::vector<Instruction>& GetInstructions() const { return instructions_; }
    std::string GetName() const { return name_; }
    
    void AddSuccessor(BasicBlock* block) { successors_.push_back(block); }
    const std::vector<BasicBlock*>& GetSuccessors() const { return successors_; }

private:
    std::string name_;
    std::vector<Instruction> instructions_;
    std::vector<BasicBlock*> successors_;
};

// ============================================================================
// IR FUNCTION
// ============================================================================

class Function {
public:
    explicit Function(const std::string& name) : name_(name), next_register_(0) {}
    
    std::string GetName() const { return name_; }
    
 BasicBlock* CreateBlock(const std::string& name);
    BasicBlock* GetEntryBlock() { return entry_block_; }
void SetEntryBlock(BasicBlock* block) { entry_block_ = block; }
    
    const std::vector<std::unique_ptr<BasicBlock>>& GetBlocks() const { return blocks_; }
    
    int AllocateRegister() { return next_register_++; }
    
    void AddParameter(const std::string& name) { parameters_.push_back(name); }
    const std::vector<std::string>& GetParameters() const { return parameters_; }

private:
    std::string name_;
    std::vector<std::unique_ptr<BasicBlock>> blocks_;
    BasicBlock* entry_block_ = nullptr;
  std::vector<std::string> parameters_;
    int next_register_;
};

// ============================================================================
// IR MODULE
// ============================================================================

class Module {
public:
    Module() = default;

  Function* CreateFunction(const std::string& name);
    const std::vector<std::unique_ptr<Function>>& GetFunctions() const { return functions_; }
    
    void Print() const;

private:
    std::vector<std::unique_ptr<Function>> functions_;
};

} // namespace IR
} // namespace Snow
