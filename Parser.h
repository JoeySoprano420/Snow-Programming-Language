#pragma once

#include "../Lexer/Lexer.h"
#include "../AST/AST.h"
#include <memory>
#include <vector>
#include <stdexcept>

namespace Snow {

// ============================================================================
// PARSER
// ============================================================================

class Parser {
public:
    explicit Parser(Lexer& lexer);
    
    // Parse entire program
    std::shared_ptr<AST::Program> ParseProgram();
    
private:
    Lexer& lexer_;
    Token current_token_;
    Token previous_token_;
    bool had_error_;
    
    // Token management
    void Advance();
    bool Check(TokenType type) const;
    bool Match(TokenType type);
    bool Match(const std::vector<TokenType>& types);
  Token Consume(TokenType type, const std::string& message);
    
 // Error handling
    void Error(const std::string& message);
    void Synchronize();
    
    // Parsing methods
    AST::StmtPtr ParseStatement();
    AST::StmtPtr ParseFunctionDecl();
    AST::StmtPtr ParseVariableDecl();
    AST::StmtPtr ParseIfStatement();
    AST::StmtPtr ParseEveryStatement();
    AST::StmtPtr ParseDeriveStatement();
  AST::StmtPtr ParseWaitStatement();
    AST::StmtPtr ParseReturnStatement();
    AST::StmtPtr ParseExpressionStatement();
    std::shared_ptr<AST::BlockStatement> ParseBlock();
    
    // Expression parsing (precedence climbing)
    AST::ExprPtr ParseExpression();
    AST::ExprPtr ParseAssignment();
    AST::ExprPtr ParseLogicalOr();
    AST::ExprPtr ParseLogicalAnd();
    AST::ExprPtr ParseEquality();
    AST::ExprPtr ParseComparison();
    AST::ExprPtr ParseTerm();
    AST::ExprPtr ParseFactor();
    AST::ExprPtr ParseUnary();
    AST::ExprPtr ParseCall();
    AST::ExprPtr ParsePrimary();
    
    // Helper methods
    Duration ParseDuration();
    std::vector<AST::ExprPtr> ParseArgumentList();
};

} // namespace Snow
