#pragma once

#include "../Common/Types.h"
#include <memory>
#include <vector>
#include <string>

namespace Snow {
namespace AST {

// Forward declarations
class Visitor;

// ============================================================================
// BASE AST NODE
// ============================================================================

enum class NodeType {
    // Statements
  Program,
    FunctionDecl,
    VariableDecl,
    IfStatement,
    WhileStatement,
    ForStatement,
    EveryStatement,
    ParallelBlock,
    DeriveStatement,
    WaitStatement,
    ReturnStatement,
    BreakStatement,
 ContinueStatement,
    ExpressionStatement,
 BlockStatement,
    NamespaceDecl,
    UseStatement,
    
// Expressions
    BinaryOp,
    UnaryOp,
    CallExpr,
    IdentifierExpr,
    LiteralExpr,
    DurationExpr,
    DerivativeExpr
};

class ASTNode {
public:
    explicit ASTNode(NodeType type, const SourceLocation& loc = SourceLocation())
 : node_type_(type), location_(loc) {}
  
    virtual ~ASTNode() = default;
    
    NodeType GetNodeType() const { return node_type_; }
    SourceLocation GetLocation() const { return location_; }
    
    virtual std::string ToString() const = 0;

protected:
    NodeType node_type_;
    SourceLocation location_;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

// ============================================================================
// EXPRESSIONS
// ============================================================================

class Expression : public ASTNode {
public:
    explicit Expression(NodeType type, const SourceLocation& loc = SourceLocation())
        : ASTNode(type, loc) {}
};

using ExprPtr = std::shared_ptr<Expression>;

// Literal Expression (numbers, strings)
class LiteralExpr : public Expression {
public:
    enum class LiteralType { Number, String, Boolean };
    
    LiteralExpr(const DodecagramNumber& value, const SourceLocation& loc)
        : Expression(NodeType::LiteralExpr, loc), 
    literal_type_(LiteralType::Number),
    number_value_(value) {}
    
    LiteralExpr(const std::string& value, const SourceLocation& loc)
        : Expression(NodeType::LiteralExpr, loc),
       literal_type_(LiteralType::String),
 string_value_(value) {}
    
 LiteralType GetLiteralType() const { return literal_type_; }
    DodecagramNumber GetNumberValue() const { return number_value_; }
    std::string GetStringValue() const { return string_value_; }
    
    std::string ToString() const override;

private:
    LiteralType literal_type_;
 DodecagramNumber number_value_;
    std::string string_value_;
};

// Identifier Expression
class IdentifierExpr : public Expression {
public:
    IdentifierExpr(const std::string& name, const SourceLocation& loc)
: Expression(NodeType::IdentifierExpr, loc), name_(name) {}
    
    std::string GetName() const { return name_; }
    std::string ToString() const override { return name_; }

private:
    std::string name_;
};

// Binary Operation
class BinaryOpExpr : public Expression {
public:
    enum class Operator {
   Add, Subtract, Multiply, Divide,
   Equal, NotEqual, LessThan, GreaterThan, LessEqual, GreaterEqual
    };
    
    BinaryOpExpr(Operator op, ExprPtr left, ExprPtr right, const SourceLocation& loc)
        : Expression(NodeType::BinaryOp, loc), op_(op), left_(left), right_(right) {}
    
    Operator GetOperator() const { return op_; }
    ExprPtr GetLeft() const { return left_; }
    ExprPtr GetRight() const { return right_; }
    
    std::string ToString() const override;

private:
    Operator op_;
    ExprPtr left_;
    ExprPtr right_;
};

// Call Expression
class CallExpr : public Expression {
public:
    CallExpr(const std::string& function_name, std::vector<ExprPtr> args, const SourceLocation& loc)
        : Expression(NodeType::CallExpr, loc), function_name_(function_name), arguments_(args) {}
    
std::string GetFunctionName() const { return function_name_; }
    const std::vector<ExprPtr>& GetArguments() const { return arguments_; }
    
    std::string ToString() const override;

private:
    std::string function_name_;
    std::vector<ExprPtr> arguments_;
};

// Duration Expression
class DurationExpr : public Expression {
public:
    DurationExpr(const Duration& duration, const SourceLocation& loc)
        : Expression(NodeType::DurationExpr, loc), duration_(duration) {}
    
  Duration GetDuration() const { return duration_; }
    std::string ToString() const override { return "Duration"; }

private:
    Duration duration_;
};

// Derivative Expression (d(expr))
class DerivativeExpr : public Expression {
public:
    DerivativeExpr(ExprPtr expr, const SourceLocation& loc)
   : Expression(NodeType::DerivativeExpr, loc), expr_(expr) {}
    
    ExprPtr GetExpression() const { return expr_; }
    std::string ToString() const override;

private:
    ExprPtr expr_;
};

// ============================================================================
// STATEMENTS
// ============================================================================

class Statement : public ASTNode {
public:
    explicit Statement(NodeType type, const SourceLocation& loc = SourceLocation())
   : ASTNode(type, loc) {}
};

using StmtPtr = std::shared_ptr<Statement>;

// Block Statement (collection of statements)
class BlockStatement : public Statement {
public:
    explicit BlockStatement(std::vector<StmtPtr> statements, const SourceLocation& loc = SourceLocation())
        : Statement(NodeType::BlockStatement, loc), statements_(statements) {}
    
    const std::vector<StmtPtr>& GetStatements() const { return statements_; }
    void AddStatement(StmtPtr stmt) { statements_.push_back(stmt); }
    
    std::string ToString() const override { return "Block"; }

private:
 std::vector<StmtPtr> statements_;
};

// Variable Declaration
class VariableDecl : public Statement {
public:
    VariableDecl(const std::string& name, ExprPtr initializer, const SourceLocation& loc)
        : Statement(NodeType::VariableDecl, loc), name_(name), initializer_(initializer) {}
    
    std::string GetName() const { return name_; }
    ExprPtr GetInitializer() const { return initializer_; }
    
    std::string ToString() const override;

private:
    std::string name_;
    ExprPtr initializer_;
};

// Function Declaration
class FunctionDecl : public Statement {
public:
    FunctionDecl(const std::string& name, std::vector<std::string> params, 
        std::shared_ptr<BlockStatement> body, const SourceLocation& loc)
        : Statement(NodeType::FunctionDecl, loc), name_(name), parameters_(params), body_(body) {}
    
    std::string GetName() const { return name_; }
    const std::vector<std::string>& GetParameters() const { return parameters_; }
  std::shared_ptr<BlockStatement> GetBody() const { return body_; }
 
    std::string ToString() const override;

private:
    std::string name_;
    std::vector<std::string> parameters_;
    std::shared_ptr<BlockStatement> body_;
};

// If Statement
class IfStatement : public Statement {
public:
    IfStatement(ExprPtr condition, StmtPtr then_branch, StmtPtr else_branch, const SourceLocation& loc)
 : Statement(NodeType::IfStatement, loc), 
   condition_(condition), then_branch_(then_branch), else_branch_(else_branch) {}
    
    ExprPtr GetCondition() const { return condition_; }
    StmtPtr GetThenBranch() const { return then_branch_; }
    StmtPtr GetElseBranch() const { return else_branch_; }
    
    std::string ToString() const override { return "If"; }

private:
  ExprPtr condition_;
    StmtPtr then_branch_;
    StmtPtr else_branch_;
};

// Every Statement (temporal loop)
class EveryStatement : public Statement {
public:
    EveryStatement(const Duration& interval, std::shared_ptr<BlockStatement> body, const SourceLocation& loc)
        : Statement(NodeType::EveryStatement, loc), interval_(interval), body_(body) {}
    
  Duration GetInterval() const { return interval_; }
    std::shared_ptr<BlockStatement> GetBody() const { return body_; }
  
    std::string ToString() const override { return "Every"; }

private:
    Duration interval_;
    std::shared_ptr<BlockStatement> body_;
};

// Derive Statement
class DeriveStatement : public Statement {
public:
    DeriveStatement(const std::string& var_name, ExprPtr expr, 
            const Duration& duration, std::shared_ptr<BlockStatement> body,
         const SourceLocation& loc)
        : Statement(NodeType::DeriveStatement, loc),
 variable_name_(var_name), expression_(expr), 
      duration_(duration), body_(body) {}
    
    std::string GetVariableName() const { return variable_name_; }
    ExprPtr GetExpression() const { return expression_; }
    Duration GetDuration() const { return duration_; }
    std::shared_ptr<BlockStatement> GetBody() const { return body_; }
    
    std::string ToString() const override { return "Derive"; }

private:
    std::string variable_name_;
    ExprPtr expression_;
    Duration duration_;
 std::shared_ptr<BlockStatement> body_;
};

// Wait Statement
class WaitStatement : public Statement {
public:
    WaitStatement(const Duration& duration, const SourceLocation& loc)
        : Statement(NodeType::WaitStatement, loc), duration_(duration) {}
    
    Duration GetDuration() const { return duration_; }
    std::string ToString() const override { return "Wait"; }

private:
    Duration duration_;
};

// Return Statement
class ReturnStatement : public Statement {
public:
    ReturnStatement(ExprPtr value, const SourceLocation& loc)
        : Statement(NodeType::ReturnStatement, loc), value_(value) {}
    
    ExprPtr GetValue() const { return value_; }
    std::string ToString() const override { return "Return"; }

private:
    ExprPtr value_;
};

// Expression Statement
class ExpressionStatement : public Statement {
public:
    ExpressionStatement(ExprPtr expr, const SourceLocation& loc)
        : Statement(NodeType::ExpressionStatement, loc), expression_(expr) {}
    
    ExprPtr GetExpression() const { return expression_; }
    std::string ToString() const override { return "ExprStmt"; }

private:
    ExprPtr expression_;
};

// Program (root node)
class Program : public ASTNode {
public:
    explicit Program(std::vector<StmtPtr> statements)
    : ASTNode(NodeType::Program), statements_(statements) {}
    
    const std::vector<StmtPtr>& GetStatements() const { return statements_; }
    void AddStatement(StmtPtr stmt) { statements_.push_back(stmt); }
    
    std::string ToString() const override { return "Program"; }

private:
    std::vector<StmtPtr> statements_;
};

} // namespace AST
} // namespace Snow
