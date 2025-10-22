#include "AST.h"
#include <sstream>

namespace Snow {
namespace AST {

// ============================================================================
// EXPRESSION TO_STRING IMPLEMENTATIONS
// ============================================================================

std::string LiteralExpr::ToString() const {
    if (literal_type_ == LiteralType::Number) {
        return number_value_.ToDodecagram();
    } else if (literal_type_ == LiteralType::String) {
  return "\"" + string_value_ + "\"";
    }
    return "Literal";
}

std::string BinaryOpExpr::ToString() const {
    std::string op_str;
    switch (op_) {
        case Operator::Add: op_str = "+"; break;
        case Operator::Subtract: op_str = "-"; break;
        case Operator::Multiply: op_str = "*"; break;
        case Operator::Divide: op_str = "/"; break;
        case Operator::Equal: op_str = "=="; break;
        case Operator::NotEqual: op_str = "!="; break;
        case Operator::LessThan: op_str = "<"; break;
        case Operator::GreaterThan: op_str = ">"; break;
        case Operator::LessEqual: op_str = "<="; break;
        case Operator::GreaterEqual: op_str = ">="; break;
    }
    return "(" + left_->ToString() + " " + op_str + " " + right_->ToString() + ")";
}

std::string CallExpr::ToString() const {
    std::stringstream ss;
    ss << function_name_ << "(";
    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << arguments_[i]->ToString();
    }
    ss << ")";
    return ss.str();
}

std::string DerivativeExpr::ToString() const {
    return "d(" + expr_->ToString() + ")";
}

// ============================================================================
// STATEMENT TO_STRING IMPLEMENTATIONS
// ============================================================================

std::string VariableDecl::ToString() const {
    return "let " + name_ + " = " + (initializer_ ? initializer_->ToString() : "null");
}

std::string FunctionDecl::ToString() const {
    std::stringstream ss;
    ss << "Fn " << name_ << "(";
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) ss << ", ";
     ss << parameters_[i];
    }
    ss << ")";
    return ss.str();
}

} // namespace AST
} // namespace Snow
