#pragma once

#include "../IR/IR.h"
#include <string>
#include <vector>
#include <fstream>

namespace Snow {

// ============================================================================
// CODE GENERATOR (x86_64)
// ============================================================================

class CodeGenerator {
public:
    CodeGenerator();
    
    // Generate native code from IR
    bool Generate(const IR::Module& module, const std::string& output_path);
    
private:
    std::ofstream output_;
    
    // Code generation methods
    void GenerateFunction(const IR::Function& func);
    void GenerateInstruction(const IR::Instruction& instr);
    
    // Helper methods
    std::string GetRegisterName(int reg);
    void EmitPrologue();
    void EmitEpilogue();
    void EmitLabel(const std::string& label);
    
    // x86_64 instruction emission
    void EmitMov(const std::string& dest, const std::string& src);
    void EmitAdd(const std::string& dest, const std::string& src1, const std::string& src2);
    void EmitSub(const std::string& dest, const std::string& src1, const std::string& src2);
    void EmitMul(const std::string& dest, const std::string& src);
 void EmitDiv(const std::string& divisor);
    void EmitCmp(const std::string& op1, const std::string& op2);
    void EmitJmp(const std::string& label);
  void EmitJe(const std::string& label);
    void EmitJne(const std::string& label);
    void EmitJg(const std::string& label);
  void EmitJl(const std::string& label);
    void EmitCall(const std::string& function);
    void EmitRet();
};

} // namespace Snow
