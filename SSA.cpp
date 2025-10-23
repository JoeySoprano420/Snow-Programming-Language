#include "SSA.h"
#include <sstream>

namespace Snow {
namespace SSA {

// ============================================================================
// SSA VALUE
// ============================================================================

std::string SSAValue::GetName() const {
    std::stringstream ss;
    switch (kind_) {
        case Kind::Register: ss << "%r" << id_; break;
        case Kind::Constant: ss << "%c" << id_; break;
        case Kind::Parameter: ss << "%p" << id_; break;
        case Kind::GlobalVariable: ss << "@g" << id_; break;
    }
    return ss.str();
}

// ============================================================================
// SSA BUILDER
// ============================================================================

SSABuilder::SSABuilder() 
: emit_debug_info_(true), 
      current_module_(nullptr),
      current_function_(nullptr),
      current_block_(nullptr) {}

std::unique_ptr<SSAModule> SSABuilder::BuildFromAST(const AST::Program& program) {
    auto module = std::make_unique<SSAModule>();
    current_module_ = module.get();
    
    // Process all function declarations
    for (const auto& stmt : program.GetStatements()) {
        if (stmt->GetNodeType() == AST::NodeType::FunctionDecl) {
            BuildFunction(static_cast<const AST::FunctionDecl&>(*stmt));
    }
    }
    
    return module;
}

void SSABuilder::BuildFunction(const AST::FunctionDecl& func) {
    current_function_ = current_module_->CreateFunction(func.GetName());
    
    // Create entry block
    current_block_ = current_function_->CreateBasicBlock("entry");
    
    // Build function body
    if (func.GetBody()) {
        for (const auto& stmt : func.GetBody()->GetStatements()) {
         BuildStatement(*stmt);
    }
    }
    
    // Insert Phi nodes and rename variables to SSA form
    InsertPhiNodes();
    RenameVariables();
}

void SSABuilder::BuildStatement(const AST::Statement& stmt) {
    switch (stmt.GetNodeType()) {
        case AST::NodeType::VariableDecl: {
            auto& var_decl = static_cast<const AST::VariableDecl&>(stmt);
  if (var_decl.GetInitializer()) {
  auto* value = BuildExpression(*var_decl.GetInitializer());
 symbol_table_[var_decl.GetName()] = value;
  }
   break;
 }
        
     case AST::NodeType::ReturnStatement: {
        auto& ret = static_cast<const AST::ReturnStatement&>(stmt);
     auto instr = std::make_unique<SSAInstruction>(SSAInstruction::OpCode::Ret);
            if (ret.GetValue()) {
      auto* value = BuildExpression(*ret.GetValue());
              instr->AddOperand(value);
   }
   current_block_->AddInstruction(std::move(instr));
    break;
 }
        
        default:
       break;
    }
}

SSAValue* SSABuilder::BuildExpression(const AST::Expression& expr) {
    switch (expr.GetNodeType()) {
        case AST::NodeType::LiteralExpr: {
        auto& lit = static_cast<const AST::LiteralExpr&>(expr);
       auto* value = current_function_->CreateValue(SSAValue::Kind::Constant);
  return value;
 }
  
   case AST::NodeType::BinaryOp: {
      auto& binop = static_cast<const AST::BinaryOpExpr&>(expr);
   auto* left = BuildExpression(*binop.GetLeft());
 auto* right = BuildExpression(*binop.GetRight());
  
       SSAInstruction::OpCode op_code;
 switch (binop.GetOperator()) {
            case AST::BinaryOpExpr::Operator::Add: op_code = SSAInstruction::OpCode::Add; break;
       case AST::BinaryOpExpr::Operator::Subtract: op_code = SSAInstruction::OpCode::Sub; break;
      case AST::BinaryOpExpr::Operator::Multiply: op_code = SSAInstruction::OpCode::Mul; break;
       case AST::BinaryOpExpr::Operator::Divide: op_code = SSAInstruction::OpCode::Div; break;
        default: op_code = SSAInstruction::OpCode::Add; break;
      }
            
 auto instr = std::make_unique<SSAInstruction>(op_code);
       instr->AddOperand(left);
    instr->AddOperand(right);
          
   auto* result = current_function_->CreateValue(SSAValue::Kind::Register);
       instr->SetResult(result);
        
            current_block_->AddInstruction(std::move(instr));
  return result;
        }
   
  default:
    return nullptr;
 }
}

void SSABuilder::InsertPhiNodes() {
    // Implement dominance frontier algorithm for Phi node placement
    // This is a simplified version
}

void SSABuilder::RenameVariables() {
    // Implement variable renaming to achieve SSA form
    // This is a simplified version
}

} // namespace SSA
} // namespace Snow
