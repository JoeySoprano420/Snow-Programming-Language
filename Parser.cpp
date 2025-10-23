#include "Parser.h"
#include <iostream>

namespace Snow {

// ============================================================================
// PARSER IMPLEMENTATION
// ============================================================================

Parser::Parser(Lexer& lexer)
    : lexer_(lexer), had_error_(false) {
    Advance(); // Initialize first token
}

void Parser::Advance() {
    previous_token_ = current_token_;
    current_token_ = lexer_.NextToken();
    
    // Skip invalid tokens
    while (current_token_.type == TokenType::INVALID) {
        Error("Invalid token: " + current_token_.lexeme);
        current_token_ = lexer_.NextToken();
    }
}

bool Parser::Check(TokenType type) const {
    return current_token_.type == type;
}

bool Parser::Match(TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool Parser::Match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
      if (Check(type)) {
       Advance();
            return true;
        }
    }
    return false;
}

Token Parser::Consume(TokenType type, const std::string& message) {
    if (Check(type)) {
  Token token = current_token_;
  Advance();
        return token;
  }
    
    Error(message);
    throw std::runtime_error(message);
}

void Parser::Error(const std::string& message) {
    std::cerr << "Parse error at " << current_token_.location.ToString() 
   << ": " << message << std::endl;
    had_error_ = true;
}

void Parser::Synchronize() {
    Advance();
    
    while (!Check(TokenType::ENDOFFILE)) {
      if (previous_token_.type == TokenType::SEMICOLON) return;
        
    switch (current_token_.type) {
       case TokenType::KW_FN:
         case TokenType::KW_LET:
          case TokenType::KW_IF:
 case TokenType::KW_WHILE:
            case TokenType::KW_FOR:
    case TokenType::KW_RETURN:    // Changed from KW_RET
                return;
   default:
                break;
        }
        
        Advance();
 }
}

// ============================================================================
// PROGRAM PARSING
// ============================================================================

std::shared_ptr<AST::Program> Parser::ParseProgram() {
    std::vector<AST::StmtPtr> statements;
    
    while (!Check(TokenType::ENDOFFILE)) {
        try {
statements.push_back(ParseStatement());
        } catch (const std::exception& e) {
            Synchronize();
        }
    }
    
  return std::make_shared<AST::Program>(statements);
}

// ============================================================================
// STATEMENT PARSING
// ============================================================================

AST::StmtPtr Parser::ParseStatement() {
    if (Match(TokenType::KW_FN)) return ParseFunctionDecl();
 if (Match(TokenType::KW_LET)) return ParseVariableDecl();
    if (Match(TokenType::KW_IF)) return ParseIfStatement();
    if (Match(TokenType::KW_EVERY)) return ParseEveryStatement();
    // if (Match(TokenType::KW_PARALLEL)) return ParseParallelBlock(); // Not implemented
    if (Match(TokenType::KW_DERIVE)) return ParseDeriveStatement();
    if (Match(TokenType::KW_WAIT)) return ParseWaitStatement();
    if (Match(TokenType::KW_RETURN)) return ParseReturnStatement();  // Changed from KW_RET
    
    return ParseExpressionStatement();
}

AST::StmtPtr Parser::ParseFunctionDecl() {
    // Fn = [name param1 param2 ...];
    // or
 // Fn name(param1, param2) body
    
    SourceLocation loc = previous_token_.location;
    
    // Check for assignment style: Fn = [...]
    if (Check(TokenType::OP_ASSIGN)) {
        Advance(); // consume =
        Consume(TokenType::LBRACKET, "Expected '[' after 'Fn ='");
      
        Token name_token = Consume(TokenType::IDENTIFIER, "Expected function name");
        std::string name = name_token.lexeme;
        
     std::vector<std::string> params;
        while (!Check(TokenType::RBRACKET)) {
    Token param = Consume(TokenType::IDENTIFIER, "Expected parameter name");
        params.push_back(param.lexeme);
     }
        
    Consume(TokenType::RBRACKET, "Expected ']'");
        Consume(TokenType::SEMICOLON, "Expected ';' after function declaration");
    
        // Empty body for now
        auto body = std::make_shared<AST::BlockStatement>(std::vector<AST::StmtPtr>(), loc);
        return std::make_shared<AST::FunctionDecl>(name, params, body, loc);
    }
    
    // Traditional style: Fn name(params) body
    Token name_token = Consume(TokenType::IDENTIFIER, "Expected function name");
    std::string name = name_token.lexeme;
    
std::vector<std::string> params;
    if (Match(TokenType::LPAREN)) {
if (!Check(TokenType::RPAREN)) {
 do {
    Token param = Consume(TokenType::IDENTIFIER, "Expected parameter name");
      params.push_back(param.lexeme);
       } while (Match(TokenType::COMMA));
        }
        Consume(TokenType::RPAREN, "Expected ')' after parameters");
    }
    
    auto body = ParseBlock();
    
    return std::make_shared<AST::FunctionDecl>(name, params, body, loc);
}

AST::StmtPtr Parser::ParseVariableDecl() {
  SourceLocation loc = previous_token_.location;
    Token name_token = Consume(TokenType::IDENTIFIER, "Expected variable name");
    
    AST::ExprPtr initializer = nullptr;
    if (Match(TokenType::OP_ASSIGN)) {
        initializer = ParseExpression();
    }
    
    Consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_shared<AST::VariableDecl>(name_token.lexeme, initializer, loc);
}

AST::StmtPtr Parser::ParseIfStatement() {
    SourceLocation loc = previous_token_.location;
    
    auto condition = ParseExpression();
    Consume(TokenType::COLON, "Expected ':' after if condition");
    
    auto then_branch = ParseBlock();
    
    AST::StmtPtr else_branch = nullptr;
    if (Match(TokenType::KW_ELSE)) {
        Consume(TokenType::COLON, "Expected ':' after 'else'");
        else_branch = ParseBlock();
    }
    
  return std::make_shared<AST::IfStatement>(condition, then_branch, else_branch, loc);
}

AST::StmtPtr Parser::ParseEveryStatement() {
    SourceLocation loc = previous_token_.location;
    
    Duration interval = ParseDuration();
    Consume(TokenType::COLON, "Expected ':' after duration");
    
    auto body = ParseBlock();
  Consume(TokenType::KW_END, "Expected 'end' after every block");
    Consume(TokenType::SEMICOLON, "Expected ';' after 'end'");
    
    return std::make_shared<AST::EveryStatement>(interval, body, loc);
}

AST::StmtPtr Parser::ParseDeriveStatement() {
    SourceLocation loc = previous_token_.location;
    
 Token var_name = Consume(TokenType::IDENTIFIER, "Expected variable name");
    
    AST::ExprPtr expr = nullptr;
    Duration duration(DodecagramNumber(0), TimeUnit::Milliseconds);
    std::shared_ptr<AST::BlockStatement> body = nullptr;
    
    if (Match(TokenType::OP_ASSIGN)) {
        // Simple derive: derive var = expr;
        expr = ParseExpression();
     Consume(TokenType::SEMICOLON, "Expected ';' after derive statement");
    } else if (Match(TokenType::KW_OVER)) {
        // Temporal derive: derive var over duration: ... end;
        duration = ParseDuration();
        Consume(TokenType::COLON, "Expected ':' after duration");
      body = ParseBlock();
        Consume(TokenType::KW_END, "Expected 'end' after derive block");
        Consume(TokenType::SEMICOLON, "Expected ';' after 'end'");
    }
    
    return std::make_shared<AST::DeriveStatement>(var_name.lexeme, expr, duration, body, loc);
}

AST::StmtPtr Parser::ParseWaitStatement() {
    SourceLocation loc = previous_token_.location;
    
    Duration duration = ParseDuration();
    Consume(TokenType::SEMICOLON, "Expected ';' after wait statement");
    
    return std::make_shared<AST::WaitStatement>(duration, loc);
}

AST::StmtPtr Parser::ParseReturnStatement() {
    SourceLocation loc = previous_token_.location;
  
    AST::ExprPtr value = nullptr;
    if (!Check(TokenType::SEMICOLON)) {
        value = ParseExpression();
    }
    
    Consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    
    return std::make_shared<AST::ReturnStatement>(value, loc);
}

AST::StmtPtr Parser::ParseExpressionStatement() {
    SourceLocation loc = current_token_.location;
    auto expr = ParseExpression();
    Consume(TokenType::SEMICOLON, "Expected ';' after expression");
    
    return std::make_shared<AST::ExpressionStatement>(expr, loc);
}

std::shared_ptr<AST::BlockStatement> Parser::ParseBlock() {
    SourceLocation loc = current_token_.location;
    std::vector<AST::StmtPtr> statements;
    
    // Blocks can be delimited by 'end' keyword or just be a collection of statements
    while (!Check(TokenType::KW_END) && !Check(TokenType::ENDOFFILE) && 
   !Check(TokenType::KW_ELSE)) {
        statements.push_back(ParseStatement());
    }
    
    return std::make_shared<AST::BlockStatement>(statements, loc);
}

// ============================================================================
// EXPRESSION PARSING
// ============================================================================

AST::ExprPtr Parser::ParseExpression() {
    return ParseAssignment();
}

AST::ExprPtr Parser::ParseAssignment() {
    return ParseLogicalOr();
}

AST::ExprPtr Parser::ParseLogicalOr() {
    return ParseLogicalAnd();
}

AST::ExprPtr Parser::ParseLogicalAnd() {
    return ParseEquality();
}

AST::ExprPtr Parser::ParseEquality() {
    auto expr = ParseComparison();
    
    while (Match({TokenType::OP_EQ, TokenType::OP_NEQ})) {
        TokenType op_type = previous_token_.type;
        auto right = ParseComparison();
        
     AST::BinaryOpExpr::Operator op = (op_type == TokenType::OP_EQ) 
    ? AST::BinaryOpExpr::Operator::Equal 
         : AST::BinaryOpExpr::Operator::NotEqual;
   
  expr = std::make_shared<AST::BinaryOpExpr>(op, expr, right, previous_token_.location);
    }
    
    return expr;
}

AST::ExprPtr Parser::ParseComparison() {
    auto expr = ParseTerm();
    
    while (Match({TokenType::OP_LT, TokenType::OP_GT, TokenType::OP_LTE, TokenType::OP_GTE})) {
        TokenType op_type = previous_token_.type;
        auto right = ParseTerm();
        
        AST::BinaryOpExpr::Operator op;
        switch (op_type) {
   case TokenType::OP_LT: op = AST::BinaryOpExpr::Operator::LessThan; break;
            case TokenType::OP_GT: op = AST::BinaryOpExpr::Operator::GreaterThan; break;
            case TokenType::OP_LTE: op = AST::BinaryOpExpr::Operator::LessEqual; break;
     case TokenType::OP_GTE: op = AST::BinaryOpExpr::Operator::GreaterEqual; break;
          default: throw std::runtime_error("Invalid comparison operator");
 }
        
        expr = std::make_shared<AST::BinaryOpExpr>(op, expr, right, previous_token_.location);
    }
    
    return expr;
}

AST::ExprPtr Parser::ParseTerm() {
    auto expr = ParseFactor();
    
    while (Match({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        TokenType op_type = previous_token_.type;
        auto right = ParseFactor();
        
        AST::BinaryOpExpr::Operator op = (op_type == TokenType::OP_PLUS)
        ? AST::BinaryOpExpr::Operator::Add
       : AST::BinaryOpExpr::Operator::Subtract;
        
    expr = std::make_shared<AST::BinaryOpExpr>(op, expr, right, previous_token_.location);
  }
    
    return expr;
}

AST::ExprPtr Parser::ParseFactor() {
    auto expr = ParseUnary();
    
    while (Match({TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE})) {
        TokenType op_type = previous_token_.type;
        auto right = ParseUnary();
        
        AST::BinaryOpExpr::Operator op = (op_type == TokenType::OP_MULTIPLY)
    ? AST::BinaryOpExpr::Operator::Multiply
: AST::BinaryOpExpr::Operator::Divide;
        
    expr = std::make_shared<AST::BinaryOpExpr>(op, expr, right, previous_token_.location);
    }
    
    return expr;
}

AST::ExprPtr Parser::ParseUnary() {
    if (Match(TokenType::OP_MINUS)) {
auto expr = ParseUnary();
        auto zero = std::make_shared<AST::LiteralExpr>(DodecagramNumber(0), previous_token_.location);
        return std::make_shared<AST::BinaryOpExpr>(
          AST::BinaryOpExpr::Operator::Subtract, zero, expr, previous_token_.location);
    }
    
  return ParseCall();
}

AST::ExprPtr Parser::ParseCall() {
    auto expr = ParsePrimary();
    
    if (Match(TokenType::LPAREN)) {
        // Function call
        if (auto id_expr = std::dynamic_pointer_cast<AST::IdentifierExpr>(expr)) {
    auto args = ParseArgumentList();
            Consume(TokenType::RPAREN, "Expected ')' after arguments");
    return std::make_shared<AST::CallExpr>(id_expr->GetName(), args, previous_token_.location);
        }
    }
    
    return expr;
}

AST::ExprPtr Parser::ParsePrimary() {
    // Literal number
    if (Match(TokenType::DODECAGRAM)) {
        return std::make_shared<AST::LiteralExpr>(previous_token_.numeric_value, previous_token_.location);
    }
    
    // String literal
    if (Match(TokenType::STRING)) {
        return std::make_shared<AST::LiteralExpr>(previous_token_.lexeme, previous_token_.location);
    }
    
    // Duration literal (handled by lexer): 100ms, 5s, 30m, 2h
    // These come in as time token types already

    // Identifier
    if (Match(TokenType::IDENTIFIER)) {
        std::string name = previous_token_.lexeme;
        
  // Check for derivative: d(expr)
        if (name == "d" && Match(TokenType::LPAREN)) {
         auto expr = ParseExpression();
            Consume(TokenType::RPAREN, "Expected ')' after derivative expression");
   return std::make_shared<AST::DerivativeExpr>(expr, previous_token_.location);
        }

        return std::make_shared<AST::IdentifierExpr>(name, previous_token_.location);
    }
    
    // Grouped expression
    if (Match(TokenType::LPAREN)) {
        auto expr = ParseExpression();
  Consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    Error("Expected expression");
    throw std::runtime_error("Expected expression");
}

Duration Parser::ParseDuration() {
    // Check for any time unit token
    if (Check(TokenType::TIME_NANOSECOND) || Check(TokenType::TIME_MICROSECOND) ||
     Check(TokenType::TIME_MILLISECOND) || Check(TokenType::TIME_SECOND) ||
     Check(TokenType::TIME_MINUTE) || Check(TokenType::TIME_HOUR)) {
        Token token = current_token_;
        Advance();
        return Duration(token.numeric_value, token.time_unit);
    }
    
 Error("Expected duration");
    throw std::runtime_error("Expected duration");
}

std::vector<AST::ExprPtr> Parser::ParseArgumentList() {
    std::vector<AST::ExprPtr> args;
    
    if (!Check(TokenType::RPAREN)) {
        do {
    args.push_back(ParseExpression());
        } while (Match(TokenType::COMMA));
    }
    
    return args;
}

} // namespace Snow
