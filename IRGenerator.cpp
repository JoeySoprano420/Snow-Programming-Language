#include "IRGenerator.h"
#include <stdexcept>
#include <iostream>

namespace Snow {

// ============================================================================
// IR GENERATOR IMPLEMENTATION
// ============================================================================

IRGenerator::IRGenerator()
    : current_function_(nullptr), current_block_(nullptr), next_label_id_(0) {
}

IR::Module* IRGenerator::Generate(const AST::Program& program) {
    // Generate IR for each statement
    for (const auto& stmt : program.GetStatements()) {
    GenerateStatement(*stmt);
    }
    
    return &module_;
}

std::string IRGenerator::GenerateLabel(const std::string& prefix) {
    return prefix + std::to_string(next_label_id_++);
}

int IRGenerator::GetOrCreateVariable(const std::string& name) {
    auto it = symbol_table_.find(name);
  if (it != symbol_table_.end()) {
  return it->second;
    }
    
    int reg = current_function_->AllocateRegister();
    symbol_table_[name] = reg;
  return reg;
}

// ============================================================================
// STATEMENT GENERATION
// ============================================================================

void IRGenerator::GenerateStatement(const AST::Statement& stmt) {
    switch (stmt.GetNodeType()) {
        case AST::NodeType::FunctionDecl:
    GenerateFunctionDecl(static_cast<const AST::FunctionDecl&>(stmt));
   break;
    case AST::NodeType::VariableDecl:
    GenerateVariableDecl(static_cast<const AST::VariableDecl&>(stmt));
  break;
     case AST::NodeType::IfStatement:
   GenerateIfStatement(static_cast<const AST::IfStatement&>(stmt));
       break;
        case AST::NodeType::EveryStatement:
    GenerateEveryStatement(static_cast<const AST::EveryStatement&>(stmt));
    break;
  case AST::NodeType::DeriveStatement:
  GenerateDeriveStatement(static_cast<const AST::DeriveStatement&>(stmt));
 break;
        case AST::NodeType::WaitStatement:
       GenerateWaitStatement(static_cast<const AST::WaitStatement&>(stmt));
 break;
        case AST::NodeType::ReturnStatement:
       GenerateReturnStatement(static_cast<const AST::ReturnStatement&>(stmt));
    break;
  case AST::NodeType::ExpressionStatement:
            GenerateExpressionStatement(static_cast<const AST::ExpressionStatement&>(stmt));
         break;
        case AST::NodeType::BlockStatement:
            GenerateBlock(static_cast<const AST::BlockStatement&>(stmt));
     break;
     default:
std::cerr << "Warning: Unhandled statement type" << std::endl;
            break;
    }
}

void IRGenerator::GenerateFunctionDecl(const AST::FunctionDecl& func) {
    // Create new function
    current_function_ = module_.CreateFunction(func.GetName());
    symbol_table_.clear();
    
    // Add parameters to symbol table
    for (const auto& param : func.GetParameters()) {
        current_function_->AddParameter(param);
        GetOrCreateVariable(param);
    }
    
    // Create entry block
    current_block_ = current_function_->CreateBlock("entry");
    
    // Generate function body
    if (func.GetBody()) {
        GenerateBlock(*func.GetBody());
    }
    
    // Add implicit return if needed
    if (current_block_) {
        current_block_->AddInstruction(IR::Instruction(IR::OpCode::RET));
    }
}

void IRGenerator::GenerateVariableDecl(const AST::VariableDecl& var) {
    int var_reg = GetOrCreateVariable(var.GetName());
    
    if (var.GetInitializer()) {
 int init_reg = GenerateExpression(*var.GetInitializer());
        
        // Move initializer value to variable register
        current_block_->AddInstruction(
  IR::Instruction(IR::OpCode::MOV, 
      IR::Operand::Register(var_reg),
                IR::Operand::Register(init_reg))
     );
    }
}

void IRGenerator::GenerateIfStatement(const AST::IfStatement& if_stmt) {
 // Generate condition
    int cond_reg = GenerateExpression(*if_stmt.GetCondition());
    
    // Create blocks
    std::string then_label = GenerateLabel("then");
    std::string else_label = GenerateLabel("else");
    std::string end_label = GenerateLabel("endif");
  
    // Compare condition to zero (false)
 current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::CMP,
            IR::Operand::Register(cond_reg),
     IR::Operand::Immediate(0))
    );
    
    // Jump if equal to zero (false) to else block
    if (if_stmt.GetElseBranch()) {
        current_block_->AddInstruction(
   IR::Instruction(IR::OpCode::JE, IR::Operand::Label(else_label))
        );
 } else {
        current_block_->AddInstruction(
            IR::Instruction(IR::OpCode::JE, IR::Operand::Label(end_label))
  );
    }
    
 // Then block
    current_block_ = current_function_->CreateBlock(then_label);
    GenerateStatement(*if_stmt.GetThenBranch());
    current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::JMP, IR::Operand::Label(end_label))
    );
    
    // Else block (if exists)
    if (if_stmt.GetElseBranch()) {
   current_block_ = current_function_->CreateBlock(else_label);
        GenerateStatement(*if_stmt.GetElseBranch());
  }
    
    // End block
    current_block_ = current_function_->CreateBlock(end_label);
}

void IRGenerator::GenerateEveryStatement(const AST::EveryStatement& every) {
    // Create loop blocks
 std::string loop_start = GenerateLabel("every_start");
    std::string loop_body = GenerateLabel("every_body");
    std::string loop_end = GenerateLabel("every_end");
    
    // Store interval in nanoseconds
    int interval_reg = current_function_->AllocateRegister();
    current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::MOV,
    IR::Operand::Register(interval_reg),
IR::Operand::Immediate(every.GetInterval().GetNanoseconds()))
    );
    
    // Loop start
    current_block_ = current_function_->CreateBlock(loop_start);
    
    // Wait for interval
    current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::WAIT, IR::Operand::Register(interval_reg))
    );
 
    // Generate body
    GenerateBlock(*every.GetBody());
    
  // Loop back
  current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::JMP, IR::Operand::Label(loop_start))
);
    
    // End (unreachable for now - will handle break later)
    current_block_ = current_function_->CreateBlock(loop_end);
}

void IRGenerator::GenerateDeriveStatement(const AST::DeriveStatement& derive) {
    // CIAM expansion for derivatives
    // This is a placeholder - full implementation would involve
    // temporal sampling and delta calculations
  
 if (derive.GetExpression()) {
        // Simple derive: derive var = d(expr)
 int var_reg = GetOrCreateVariable(derive.GetVariableName());
        
// For now, just generate the expression and store it
  int expr_reg = GenerateExpression(*derive.GetExpression());
        
        current_block_->AddInstruction(
    IR::Instruction(IR::OpCode::DODECAP, 
   IR::Operand::Register(var_reg),
         IR::Operand::Register(expr_reg))
        );
    } else if (derive.GetBody()) {
   // Temporal derive with body
        GenerateBlock(*derive.GetBody());
    }
}

void IRGenerator::GenerateWaitStatement(const AST::WaitStatement& wait) {
    int duration_ns = wait.GetDuration().GetNanoseconds();
    int duration_reg = current_function_->AllocateRegister();
    
    current_block_->AddInstruction(
IR::Instruction(IR::OpCode::MOV,
            IR::Operand::Register(duration_reg),
     IR::Operand::Immediate(duration_ns))
    );
    
    current_block_->AddInstruction(
    IR::Instruction(IR::OpCode::WAIT, IR::Operand::Register(duration_reg))
    );
}

void IRGenerator::GenerateReturnStatement(const AST::ReturnStatement& ret) {
    if (ret.GetValue()) {
     int ret_reg = GenerateExpression(*ret.GetValue());
        
        // Move return value to R0 (conventional return register)
        current_block_->AddInstruction(
     IR::Instruction(IR::OpCode::MOV,
        IR::Operand::Register(0),
         IR::Operand::Register(ret_reg))
 );
    }
    
    current_block_->AddInstruction(IR::Instruction(IR::OpCode::RET));
}

void IRGenerator::GenerateExpressionStatement(const AST::ExpressionStatement& expr_stmt) {
    if (expr_stmt.GetExpression()) {
        GenerateExpression(*expr_stmt.GetExpression());
    }
}

void IRGenerator::GenerateBlock(const AST::BlockStatement& block) {
    for (const auto& stmt : block.GetStatements()) {
  GenerateStatement(*stmt);
    }
}

// ============================================================================
// EXPRESSION GENERATION
// ============================================================================

int IRGenerator::GenerateExpression(const AST::Expression& expr) {
    switch (expr.GetNodeType()) {
        case AST::NodeType::BinaryOp:
        return GenerateBinaryOp(static_cast<const AST::BinaryOpExpr&>(expr));
        case AST::NodeType::CallExpr:
return GenerateCall(static_cast<const AST::CallExpr&>(expr));
      case AST::NodeType::LiteralExpr:
          return GenerateLiteral(static_cast<const AST::LiteralExpr&>(expr));
        case AST::NodeType::IdentifierExpr:
   return GenerateIdentifier(static_cast<const AST::IdentifierExpr&>(expr));
        case AST::NodeType::DurationExpr:
   return GenerateDuration(static_cast<const AST::DurationExpr&>(expr));
        case AST::NodeType::DerivativeExpr:
       return GenerateDerivative(static_cast<const AST::DerivativeExpr&>(expr));
        default:
   std::cerr << "Warning: Unhandled expression type" << std::endl;
 return current_function_->AllocateRegister();
    }
}

int IRGenerator::GenerateBinaryOp(const AST::BinaryOpExpr& binop) {
    int left_reg = GenerateExpression(*binop.GetLeft());
    int right_reg = GenerateExpression(*binop.GetRight());
    int result_reg = current_function_->AllocateRegister();
    
    IR::OpCode opcode;
    switch (binop.GetOperator()) {
        case AST::BinaryOpExpr::Operator::Add:
   opcode = IR::OpCode::ADD;
            break;
        case AST::BinaryOpExpr::Operator::Subtract:
   opcode = IR::OpCode::SUB;
  break;
        case AST::BinaryOpExpr::Operator::Multiply:
   opcode = IR::OpCode::MUL;
      break;
   case AST::BinaryOpExpr::Operator::Divide:
     opcode = IR::OpCode::DIV;
       break;
  default:
      // Comparison operators use CMP
            current_block_->AddInstruction(
            IR::Instruction(IR::OpCode::CMP,
   IR::Operand::Register(left_reg),
   IR::Operand::Register(right_reg))
            );
    return result_reg;
 }
    
    current_block_->AddInstruction(
        IR::Instruction(opcode,
 IR::Operand::Register(result_reg),
          IR::Operand::Register(left_reg),
            IR::Operand::Register(right_reg))
    );
    
    return result_reg;
}

int IRGenerator::GenerateCall(const AST::CallExpr& call) {
    // Generate arguments
    std::vector<int> arg_regs;
    for (const auto& arg : call.GetArguments()) {
   arg_regs.push_back(GenerateExpression(*arg));
    }
    
// For now, simple call handling
    int result_reg = current_function_->AllocateRegister();
    
    current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::CALL, IR::Operand::Label(call.GetFunctionName()))
  );
    
    // Result is in R0 by convention
  current_block_->AddInstruction(
 IR::Instruction(IR::OpCode::MOV,
            IR::Operand::Register(result_reg),
        IR::Operand::Register(0))
    );
    
    return result_reg;
}

int IRGenerator::GenerateLiteral(const AST::LiteralExpr& literal) {
    int reg = current_function_->AllocateRegister();
    
    if (literal.GetLiteralType() == AST::LiteralExpr::LiteralType::Number) {
      current_block_->AddInstruction(
  IR::Instruction(IR::OpCode::MOV,
 IR::Operand::Register(reg),
   IR::Operand::Immediate(literal.GetNumberValue().ToDecimal()))
     );
    }
 
    return reg;
}

int IRGenerator::GenerateIdentifier(const AST::IdentifierExpr& id) {
  return GetOrCreateVariable(id.GetName());
}

int IRGenerator::GenerateDuration(const AST::DurationExpr& duration) {
    int reg = current_function_->AllocateRegister();
    
    current_block_->AddInstruction(
        IR::Instruction(IR::OpCode::MOV,
   IR::Operand::Register(reg),
   IR::Operand::Immediate(duration.GetDuration().GetNanoseconds()))
    );
    
    return reg;
}

int IRGenerator::GenerateDerivative(const AST::DerivativeExpr& deriv) {
    // Generate the inner expression
 int expr_reg = GenerateExpression(*deriv.GetExpression());
    int result_reg = current_function_->AllocateRegister();
    
    // DODECAP operation for derivative
    current_block_->AddInstruction(
   IR::Instruction(IR::OpCode::DODECAP,
   IR::Operand::Register(result_reg),
   IR::Operand::Register(expr_reg))
    );
    
    return result_reg;
}

} // namespace Snow
