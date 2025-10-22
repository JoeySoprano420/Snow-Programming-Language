#include "TypeSystem.h"
#include <sstream>
#include <algorithm>

namespace Snow {
namespace TypeSystem {

// ============================================================================
// TYPE BASE CLASS
// ============================================================================

bool Type::IsCompatibleWith(const Type& other) const {
    return kind_ == other.kind_;
}

// ============================================================================
// INTEGER TYPE
// ============================================================================

std::string IntegerType::ToString() const {
    std::stringstream ss;
    if (!is_signed_) ss << "u";
    ss << "int" << bits_;
    return ss.str();
}

// ============================================================================
// FLOAT TYPE
// ============================================================================

std::string FloatType::ToString() const {
    return bits_ == 32 ? "float" : "double";
}

// ============================================================================
// FUNCTION TYPE
// ============================================================================

std::string FunctionType::ToString() const {
    std::stringstream ss;
    ss << "fn(";
    for (size_t i = 0; i < param_types_.size(); ++i) {
        if (i > 0) ss << ", ";
   ss << param_types_[i]->ToString();
    }
    ss << ") -> " << return_type_->ToString();
    return ss.str();
}

// ============================================================================
// ARRAY TYPE
// ============================================================================

std::string ArrayType::ToString() const {
    return "[" + std::to_string(size_) + "]" + element_type_->ToString();
}

// ============================================================================
// STRUCT TYPE
// ============================================================================

void StructType::AddField(const std::string& name, TypePtr type) {
 size_t offset = 0;
    if (!fields_.empty()) {
        const auto& last = fields_.back();
     offset = last.offset + last.type->GetSize();
        // Align offset
        size_t align = type->GetAlignment();
        offset = (offset + align - 1) & ~(align - 1);
    }
fields_.push_back({name, type, offset});
}

TypePtr StructType::GetFieldType(const std::string& name) const {
    for (const auto& field : fields_) {
        if (field.name == name) return field.type;
    }
    return nullptr;
}

size_t StructType::GetSize() const {
    if (fields_.empty()) return 0;
    const auto& last = fields_.back();
    return last.offset + last.type->GetSize();
}

size_t StructType::GetAlignment() const {
    size_t max_align = 1;
    for (const auto& field : fields_) {
        max_align = std::max(max_align, field.type->GetAlignment());
    }
    return max_align;
}

// ============================================================================
// TYPE INFERENCE
// ============================================================================

TypePtr TypeInference::InferType(const AST::Expression& expr) {
    auto& registry = TypeRegistry::Instance();
    
    switch (expr.GetNodeType()) {
      case AST::NodeType::LiteralExpr: {
 auto& lit = static_cast<const AST::LiteralExpr&>(expr);
            if (lit.GetLiteralType() == AST::LiteralExpr::LiteralType::Number) {
        return registry.GetDodecagramType();
            } else if (lit.GetLiteralType() == AST::LiteralExpr::LiteralType::String) {
         return registry.GetStringType();
            } else {
       return registry.GetBoolType();
            }
        }
        
        case AST::NodeType::BinaryOp: {
            auto& binop = static_cast<const AST::BinaryOpExpr&>(expr);
            auto left_type = InferType(*binop.GetLeft());
        auto right_type = InferType(*binop.GetRight());
            return Unify(left_type, right_type);
      }
 
        case AST::NodeType::DurationExpr:
     return registry.GetDurationType();
            
        default:
     return registry.GetVoidType();
    }
}

TypePtr TypeInference::Unify(TypePtr t1, TypePtr t2) {
    if (!t1 || !t2) return nullptr;
    
    // Same type
    if (t1->GetKind() == t2->GetKind()) {
        return t1;
    }
    
    // Numeric promotion
    if (t1->GetKind() == TypeKind::Integer && t2->GetKind() == TypeKind::Integer) {
        auto i1 = std::static_pointer_cast<IntegerType>(t1);
        auto i2 = std::static_pointer_cast<IntegerType>(t2);
   // Promote to larger type
        return i1->GetBits() > i2->GetBits() ? t1 : t2;
    }
    
    return nullptr;
}

bool TypeInference::CanConvert(TypePtr from, TypePtr to) {
    if (!from || !to) return false;
    if (from->GetKind() == to->GetKind()) return true;
    
    // Allow numeric conversions
 if ((from->GetKind() == TypeKind::Integer || from->GetKind() == TypeKind::Dodecagram) &&
        (to->GetKind() == TypeKind::Integer || to->GetKind() == TypeKind::Dodecagram)) {
        return true;
    }
    
    return false;
}

int TypeInference::ConversionCost(TypePtr from, TypePtr to) {
    if (!from || !to) return 1000;
    if (from->GetKind() == to->GetKind()) return 0;
    
    // Similar types have low cost
    if ((from->GetKind() == TypeKind::Integer && to->GetKind() == TypeKind::Dodecagram) ||
        (from->GetKind() == TypeKind::Dodecagram && to->GetKind() == TypeKind::Integer)) {
    return 1;
    }
    
    return 10; // High cost for other conversions
}

// ============================================================================
// TYPE CHECKER
// ============================================================================

TypeChecker::TypeChecker() {
    InitializeBuiltInTypes();
}

void TypeChecker::InitializeBuiltInTypes() {
    auto& registry = TypeRegistry::Instance();
    void_type_ = registry.GetVoidType();
  int_type_ = registry.GetIntType();
  float_type_ = registry.GetFloatType();
    bool_type_ = registry.GetBoolType();
    string_type_ = registry.GetStringType();
duration_type_ = registry.GetDurationType();
    dodecagram_type_ = registry.GetDodecagramType();
}

bool TypeChecker::Check(AST::Program& program) {
    errors_.clear();
    
    for (const auto& stmt : program.GetStatements()) {
  if (!CheckStatement(*stmt)) {
return false;
        }
    }

    return !HasErrors();
}

bool TypeChecker::CheckStatement(AST::Statement& stmt) {
    switch (stmt.GetNodeType()) {
        case AST::NodeType::VariableDecl: {
        auto& var_decl = static_cast<AST::VariableDecl&>(stmt);
   if (var_decl.GetInitializer()) {
   auto type = CheckExpression(*var_decl.GetInitializer());
          symbol_types_[var_decl.GetName()] = type;
     }
         return true;
        }
        
        case AST::NodeType::FunctionDecl: {
            auto& func_decl = static_cast<AST::FunctionDecl&>(stmt);
     // Check function body
 if (func_decl.GetBody()) {
     for (const auto& body_stmt : func_decl.GetBody()->GetStatements()) {
   CheckStatement(*body_stmt);
          }
   }
     return true;
        }
      
        default:
      return true;
    }
}

TypePtr TypeChecker::CheckExpression(AST::Expression& expr) {
return inference_.InferType(expr);
}

void TypeChecker::ReportError(const std::string& message, const SourceLocation& loc) {
    errors_.push_back(loc.ToString() + ": " + message);
}

// ============================================================================
// TYPE REGISTRY
// ============================================================================

TypeRegistry& TypeRegistry::Instance() {
    static TypeRegistry instance;
    return instance;
}

TypeRegistry::TypeRegistry() {
    // Register built-in types
    types_["void"] = std::make_shared<IntegerType>(0, false);
    types_["int"] = std::make_shared<IntegerType>(64, true);
    types_["int8"] = std::make_shared<IntegerType>(8, true);
    types_["int16"] = std::make_shared<IntegerType>(16, true);
    types_["int32"] = std::make_shared<IntegerType>(32, true);
    types_["int64"] = std::make_shared<IntegerType>(64, true);
    types_["uint8"] = std::make_shared<IntegerType>(8, false);
    types_["uint16"] = std::make_shared<IntegerType>(16, false);
    types_["uint32"] = std::make_shared<IntegerType>(32, false);
    types_["uint64"] = std::make_shared<IntegerType>(64, false);
    types_["float"] = std::make_shared<FloatType>(32);
    types_["double"] = std::make_shared<FloatType>(64);
    types_["bool"] = std::make_shared<BooleanType>();
    types_["string"] = std::make_shared<StringType>();
  types_["duration"] = std::make_shared<DurationTypeClass>();
    types_["dodecagram"] = std::make_shared<DodecagramType>();
}

void TypeRegistry::RegisterType(const std::string& name, TypePtr type) {
 types_[name] = type;
}

TypePtr TypeRegistry::GetType(const std::string& name) const {
  auto it = types_.find(name);
    return it != types_.end() ? it->second : nullptr;
}

TypePtr TypeRegistry::GetIntType(int bits, bool is_signed) {
    return std::make_shared<IntegerType>(bits, is_signed);
}

TypePtr TypeRegistry::GetFloatType(int bits) {
    return std::make_shared<FloatType>(bits);
}

TypePtr TypeRegistry::GetBoolType() {
    return GetType("bool");
}

TypePtr TypeRegistry::GetStringType() {
    return GetType("string");
}

TypePtr TypeRegistry::GetDurationType() {
    return GetType("duration");
}

TypePtr TypeRegistry::GetDodecagramType() {
    return GetType("dodecagram");
}

TypePtr TypeRegistry::GetVoidType() {
    return GetType("void");
}

TypePtr TypeRegistry::CreateArrayType(TypePtr element, size_t size) {
    return std::make_shared<ArrayType>(element, size);
}

TypePtr TypeRegistry::CreatePointerType(TypePtr pointee) {
    return std::make_shared<PointerType>(pointee);
}

TypePtr TypeRegistry::CreateFunctionType(TypePtr return_type, std::vector<TypePtr> params) {
    return std::make_shared<FunctionType>(return_type, std::move(params));
}

} // namespace TypeSystem
} // namespace Snow
