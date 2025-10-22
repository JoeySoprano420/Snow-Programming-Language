#include "IR.h"
#include <iostream>
#include <sstream>

namespace Snow {
namespace IR {

// ============================================================================
// OPERAND IMPLEMENTATION
// ============================================================================

std::string Operand::ToString() const {
    switch (type) {
        case OperandType::Register:
   return "R" + std::to_string(value);
        case OperandType::Immediate:
     return std::to_string(value);
        case OperandType::Memory:
      return "[" + std::to_string(value) + "]";
        case OperandType::Label:
     return label;
    default:
      return "?";
    }
}

// ============================================================================
// INSTRUCTION IMPLEMENTATION
// ============================================================================

std::string Instruction::ToString() const {
  std::stringstream ss;
    
  // Opcode name
    switch (opcode) {
   case OpCode::MOV: ss << "MOV"; break;
        case OpCode::LOAD: ss << "LOAD"; break;
        case OpCode::STORE: ss << "STORE"; break;
        case OpCode::ADD: ss << "ADD"; break;
        case OpCode::SUB: ss << "SUB"; break;
        case OpCode::MUL: ss << "MUL"; break;
        case OpCode::DIV: ss << "DIV"; break;
        case OpCode::CMP: ss << "CMP"; break;
    case OpCode::JMP: ss << "JMP"; break;
        case OpCode::JE: ss << "JE"; break;
case OpCode::JNE: ss << "JNE"; break;
 case OpCode::JG: ss << "JG"; break;
  case OpCode::JL: ss << "JL"; break;
        case OpCode::JGE: ss << "JGE"; break;
        case OpCode::JLE: ss << "JLE"; break;
        case OpCode::CALL: ss << "CALL"; break;
case OpCode::RET: ss << "RET"; break;
 case OpCode::WAIT: ss << "WAIT"; break;
        case OpCode::DODECAP: ss << "DODECAP"; break;
        case OpCode::SAMPLE: ss << "SAMPLE"; break;
      case OpCode::DELTA: ss << "DELTA"; break;
 case OpCode::LABEL: ss << "LABEL"; break;
        case OpCode::NOP: ss << "NOP"; break;
        default: ss << "???"; break;
    }
    
    // Operands
    if (dest.type != OperandType::Register || dest.value != 0) {
   ss << " " << dest.ToString();
    }
 if (src1.type != OperandType::Register || src1.value != 0) {
        ss << ", " << src1.ToString();
    }
    if (src2.type != OperandType::Register || src2.value != 0) {
        ss << ", " << src2.ToString();
}
    
// Comment
    if (!comment.empty()) {
        ss << " ; " << comment;
    }
    
    return ss.str();
}

// ============================================================================
// FUNCTION IMPLEMENTATION
// ============================================================================

BasicBlock* Function::CreateBlock(const std::string& name) {
    auto block = std::make_unique<BasicBlock>(name);
    BasicBlock* ptr = block.get();
    blocks_.push_back(std::move(block));
    
    if (!entry_block_) {
        entry_block_ = ptr;
    }
    
  return ptr;
}

// ============================================================================
// MODULE IMPLEMENTATION
// ============================================================================

Function* Module::CreateFunction(const std::string& name) {
    auto func = std::make_unique<Function>(name);
    Function* ptr = func.get();
    functions_.push_back(std::move(func));
    return ptr;
}

void Module::Print() const {
    for (const auto& func : functions_) {
        std::cout << "\n[FUNCTION " << func->GetName() << "]" << std::endl;
        
for (const auto& block : func->GetBlocks()) {
 std::cout << block->GetName() << ":" << std::endl;
            for (const auto& instr : block->GetInstructions()) {
     std::cout << "  " << instr.ToString() << std::endl;
            }
        }
    }
}

} // namespace IR
} // namespace Snow
