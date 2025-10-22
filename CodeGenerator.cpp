#include "CodeGenerator.h"
#include <iostream>
#include <unordered_map>

namespace Snow {

// ============================================================================
// CODE GENERATOR IMPLEMENTATION
// ============================================================================

CodeGenerator::CodeGenerator() {
}

bool CodeGenerator::Generate(const IR::Module& module, const std::string& output_path) {
    output_.open(output_path);
  if (!output_.is_open()) {
 std::cerr << "Error: Could not open output file: " << output_path << std::endl;
        return false;
    }
  
    std::cout << "\n[CodeGen] Generating x86_64 assembly..." << std::endl;
    
    // Emit assembly header
    output_ << "; Snow Programming Language - Generated Assembly\n";
    output_ << "; Target: x86_64 Windows PE\n\n";
    
    output_ << "section .text\n";
    output_ << "global main\n\n";
    
    // Generate code for each function
    for (const auto& func : module.GetFunctions()) {
        GenerateFunction(*func);
    }
    
    output_.close();
    std::cout << "[CodeGen] Code generation complete: " << output_path << std::endl;
    
    return true;
}

std::string CodeGenerator::GetRegisterName(int reg) {
    // Map virtual registers to x86_64 registers
    // Using a simple mapping for now
    static const char* regs[] = {
        "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9",
     "r10", "r11", "r12", "r13", "r14", "r15"
    };
    
    if (reg >= 0 && reg < 14) {
    return regs[reg];
    }
    
    // For overflow, use stack
  return "[rbp-" + std::to_string((reg - 14 + 1) * 8) + "]";
}

void CodeGenerator::GenerateFunction(const IR::Function& func) {
    output_ << "; Function: " << func.GetName() << "\n";
    output_ << func.GetName() << ":\n";
    
    EmitPrologue();
    
    // Generate code for each basic block
  for (const auto& block : func.GetBlocks()) {
        if (block->GetName() != "entry") {
 EmitLabel(block->GetName());
    }
        
        for (const auto& instr : block->GetInstructions()) {
GenerateInstruction(instr);
        }
    }
    
    EmitEpilogue();
    output_ << "\n";
}

void CodeGenerator::GenerateInstruction(const IR::Instruction& instr) {
    output_ << "    ";
  
    switch (instr.opcode) {
        case IR::OpCode::MOV: {
  std::string dest = (instr.dest.type == IR::OperandType::Register) 
   ? GetRegisterName(instr.dest.value) : "";
   std::string src;
            
    if (instr.src1.type == IR::OperandType::Immediate) {
src = std::to_string(instr.src1.value);
     } else if (instr.src1.type == IR::OperandType::Register) {
  src = GetRegisterName(instr.src1.value);
            }
     
          EmitMov(dest, src);
   break;
        }
        
      case IR::OpCode::ADD: {
      std::string dest = GetRegisterName(instr.dest.value);
std::string src1 = GetRegisterName(instr.src1.value);
 std::string src2 = (instr.src2.type == IR::OperandType::Immediate)
? std::to_string(instr.src2.value)
    : GetRegisterName(instr.src2.value);
         
  // x86: add dest, src (dest = dest + src)
          // We need: dest = src1 + src2
    if (dest != src1) {
        EmitMov(dest, src1);
 }
    output_ << "add " << dest << ", " << src2 << "\n";
  break;
        }
        
 case IR::OpCode::SUB: {
   std::string dest = GetRegisterName(instr.dest.value);
            std::string src1 = GetRegisterName(instr.src1.value);
 std::string src2 = (instr.src2.type == IR::OperandType::Immediate)
 ? std::to_string(instr.src2.value)
: GetRegisterName(instr.src2.value);
            
       if (dest != src1) {
                EmitMov(dest, src1);
  }
  output_ << "sub " << dest << ", " << src2 << "\n";
        break;
        }
        
        case IR::OpCode::MUL: {
            std::string dest = GetRegisterName(instr.dest.value);
            std::string src1 = GetRegisterName(instr.src1.value);
 std::string src2 = (instr.src2.type == IR::OperandType::Immediate)
 ? std::to_string(instr.src2.value)
            : GetRegisterName(instr.src2.value);
   
            if (dest != src1) {
    EmitMov(dest, src1);
  }
output_ << "imul " << dest << ", " << src2 << "\n";
          break;
      }
     
   case IR::OpCode::DIV: {
   // x86 division uses rax and rdx
 output_ << "xor rdx, rdx\n";
 output_ << "    ";
            std::string divisor = GetRegisterName(instr.src1.value);
   output_ << "idiv " << divisor << "\n";
break;
        }
      
     case IR::OpCode::CMP: {
   std::string op1 = GetRegisterName(instr.dest.value);
 std::string op2 = (instr.src1.type == IR::OperandType::Immediate)
           ? std::to_string(instr.src1.value)
         : GetRegisterName(instr.src1.value);
     EmitCmp(op1, op2);
  break;
        }
     
        case IR::OpCode::JMP:
EmitJmp(instr.dest.label);
      break;
            
   case IR::OpCode::JE:
 EmitJe(instr.dest.label);
  break;
     
        case IR::OpCode::JNE:
   EmitJne(instr.dest.label);
       break;
    
   case IR::OpCode::JG:
        EmitJg(instr.dest.label);
            break;
 
     case IR::OpCode::JL:
  EmitJl(instr.dest.label);
   break;
    
        case IR::OpCode::CALL:
            EmitCall(instr.dest.label);
            break;
         
   case IR::OpCode::RET:
   EmitRet();
 break;
            
        case IR::OpCode::WAIT:
            // Call runtime wait function
      output_ << "mov rdi, " << GetRegisterName(instr.dest.value) << "\n";
        output_ << "    call _snow_wait\n";
            break;
            
        case IR::OpCode::DODECAP:
   case IR::OpCode::SAMPLE:
        case IR::OpCode::DELTA:
     // Placeholder for CIAM operations
    output_ << "; " << instr.ToString() << "\n";
      break;
        
        case IR::OpCode::NOP:
   output_ << "nop\n";
   break;
       
        default:
       output_ << "; Unknown opcode\n";
  break;
    }
}

void CodeGenerator::EmitPrologue() {
    output_ << " push rbp\n";
 output_ << "  mov rbp, rsp\n";
    output_ << "    sub rsp, 128  ; Local variable space\n";
}

void CodeGenerator::EmitEpilogue() {
    output_ << "    mov rsp, rbp\n";
    output_ << "    pop rbp\n";
    output_ << "    ret\n";
}

void CodeGenerator::EmitLabel(const std::string& label) {
    output_ << label << ":\n";
}

void CodeGenerator::EmitMov(const std::string& dest, const std::string& src) {
    output_ << "mov " << dest << ", " << src << "\n";
}

void CodeGenerator::EmitAdd(const std::string& dest, const std::string& src1, const std::string& src2) {
    if (dest != src1) {
        EmitMov(dest, src1);
    }
    output_ << "    add " << dest << ", " << src2 << "\n";
}

void CodeGenerator::EmitSub(const std::string& dest, const std::string& src1, const std::string& src2) {
 if (dest != src1) {
EmitMov(dest, src1);
    }
    output_ << "    sub " << dest << ", " << src2 << "\n";
}

void CodeGenerator::EmitMul(const std::string& dest, const std::string& src) {
    output_ << "imul " << dest << ", " << src << "\n";
}

void CodeGenerator::EmitDiv(const std::string& divisor) {
    output_ << "xor rdx, rdx\n";
 output_ << "    idiv " << divisor << "\n";
}

void CodeGenerator::EmitCmp(const std::string& op1, const std::string& op2) {
    output_ << "cmp " << op1 << ", " << op2 << "\n";
}

void CodeGenerator::EmitJmp(const std::string& label) {
    output_ << "jmp " << label << "\n";
}

void CodeGenerator::EmitJe(const std::string& label) {
 output_ << "je " << label << "\n";
}

void CodeGenerator::EmitJne(const std::string& label) {
 output_ << "jne " << label << "\n";
}

void CodeGenerator::EmitJg(const std::string& label) {
 output_ << "jg " << label << "\n";
}

void CodeGenerator::EmitJl(const std::string& label) {
    output_ << "jl " << label << "\n";
}

void CodeGenerator::EmitCall(const std::string& function) {
    output_ << "call " << function << "\n";
}

void CodeGenerator::EmitRet() {
    // Handled by epilogue
}

} // namespace Snow
