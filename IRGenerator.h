#pragma once

#include "../AST/AST.h"
#include "../IR/IR.h"
#include <unordered_map>
#include <string>

namespace Snow {

// ============================================================================
// IR GENERATOR
// ============================================================================

class IRGenerator {
public:
    IRGenerator();
    
 // Generate IR from AST
    IR::Module* Generate(const AST::Program& program);
    
private:
    IR::Module module_;
    IR::Function* current_function_;
    IR::BasicBlock* current_block_;
    
    // Symbol table (variable name -> register)
    std::unordered_map<std::string, int> symbol_table_;
    
    // Label counter
    int next_label_id_;
    
    // Helper methods
    std::string GenerateLabel(const std::string& prefix = "L");
    int GetOrCreateVariable(const std::string& name);
    
    // Statement generation
    void GenerateStatement(const AST::Statement& stmt);
    void GenerateFunctionDecl(const AST::FunctionDecl& func);
    void GenerateVariableDecl(const AST::VariableDecl& var);
    void GenerateIfStatement(const AST::IfStatement& if_stmt);
 void GenerateEveryStatement(const AST::EveryStatement& every);
    void GenerateDeriveStatement(const AST::DeriveStatement& derive);
    void GenerateWaitStatement(const AST::WaitStatement& wait);
    void GenerateReturnStatement(const AST::ReturnStatement& ret);
    void GenerateExpressionStatement(const AST::ExpressionStatement& expr_stmt);
    void GenerateBlock(const AST::BlockStatement& block);
    
    // Expression generation (returns register holding result)
    int GenerateExpression(const AST::Expression& expr);
  int GenerateBinaryOp(const AST::BinaryOpExpr& binop);
  int GenerateCall(const AST::CallExpr& call);
    int GenerateLiteral(const AST::LiteralExpr& literal);
    int GenerateIdentifier(const AST::IdentifierExpr& id);
    int GenerateDuration(const AST::DurationExpr& duration);
    int GenerateDerivative(const AST::DerivativeExpr& deriv);
};

} // namespace Snow
