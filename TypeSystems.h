#pragma once

#include "../Common/Types.h"
#include "../AST/AST.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Snow {
namespace TypeSystem {

// ============================================================================
// TYPE KINDS
// ============================================================================

enum class TypeKind {
    Void,
    Integer,
    Float,
    Boolean,
  String,
    Duration,
    Dodecagram,
    Function,
    Array,
    Struct,
    Union,
    Pointer,
    Reference,
    Generic,
 Auto  // Type inference
};

// ============================================================================
// BASE TYPE
// ============================================================================

class Type {
public:
    explicit Type(TypeKind kind) : kind_(kind) {}
    virtual ~Type() = default;
    
    TypeKind GetKind() const { return kind_; }
    virtual std::string ToString() const = 0;
    virtual bool IsCompatibleWith(const Type& other) const;
    virtual size_t GetSize() const = 0;
    virtual size_t GetAlignment() const = 0;

protected:
    TypeKind kind_;
};

using TypePtr = std::shared_ptr<Type>;

// ============================================================================
// PRIMITIVE TYPES
// ============================================================================

class IntegerType : public Type {
public:
    IntegerType(int bits, bool is_signed)
        : Type(TypeKind::Integer), bits_(bits), is_signed_(is_signed) {}
    
    int GetBits() const { return bits_; }
    bool IsSigned() const { return is_signed_; }
    
    std::string ToString() const override;
    size_t GetSize() const override { return bits_ / 8; }
    size_t GetAlignment() const override { return GetSize(); }

private:
    int bits_;
    bool is_signed_;
};

class FloatType : public Type {
public:
    explicit FloatType(int bits) : Type(TypeKind::Float), bits_(bits) {}
    
    int GetBits() const { return bits_; }
    std::string ToString() const override;
    size_t GetSize() const override { return bits_ / 8; }
    size_t GetAlignment() const override { return GetSize(); }

private:
    int bits_;
};

class BooleanType : public Type {
public:
    BooleanType() : Type(TypeKind::Boolean) {}
    std::string ToString() const override { return "bool"; }
size_t GetSize() const override { return 1; }
    size_t GetAlignment() const override { return 1; }
};

class StringType : public Type {
public:
    StringType() : Type(TypeKind::String) {}
    std::string ToString() const override { return "string"; }
    size_t GetSize() const override { return 8; } // Pointer size
    size_t GetAlignment() const override { return 8; }
};

class DurationTypeClass : public Type {
public:
    DurationTypeClass() : Type(TypeKind::Duration) {}
    std::string ToString() const override { return "duration"; }
size_t GetSize() const override { return 8; }
    size_t GetAlignment() const override { return 8; }
};

class DodecagramType : public Type {
public:
    DodecagramType() : Type(TypeKind::Dodecagram) {}
    std::string ToString() const override { return "dodecagram"; }
    size_t GetSize() const override { return 8; }
size_t GetAlignment() const override { return 8; }
};

// ============================================================================
// COMPOSITE TYPES
// ============================================================================

class FunctionType : public Type {
public:
    FunctionType(TypePtr return_type, std::vector<TypePtr> param_types)
 : Type(TypeKind::Function), return_type_(return_type), 
          param_types_(std::move(param_types)) {}
    
    TypePtr GetReturnType() const { return return_type_; }
    const std::vector<TypePtr>& GetParameterTypes() const { return param_types_; }
    
    std::string ToString() const override;
    size_t GetSize() const override { return 8; } // Function pointer
    size_t GetAlignment() const override { return 8; }

private:
    TypePtr return_type_;
    std::vector<TypePtr> param_types_;
};

class ArrayType : public Type {
public:
    ArrayType(TypePtr element_type, size_t size)
    : Type(TypeKind::Array), element_type_(element_type), size_(size) {}
    
    TypePtr GetElementType() const { return element_type_; }
    size_t GetArraySize() const { return size_; }
    
    std::string ToString() const override;
    size_t GetSize() const override { return element_type_->GetSize() * size_; }
    size_t GetAlignment() const override { return element_type_->GetAlignment(); }

private:
    TypePtr element_type_;
    size_t size_;
};

class StructType : public Type {
public:
    struct Field {
        std::string name;
   TypePtr type;
   size_t offset;
    };
    
    explicit StructType(const std::string& name) 
        : Type(TypeKind::Struct), name_(name) {}
    
    void AddField(const std::string& name, TypePtr type);
    const std::vector<Field>& GetFields() const { return fields_; }
    TypePtr GetFieldType(const std::string& name) const;
    
    std::string ToString() const override { return "struct " + name_; }
    size_t GetSize() const override;
    size_t GetAlignment() const override;

private:
    std::string name_;
    std::vector<Field> fields_;
};

class PointerType : public Type {
public:
    explicit PointerType(TypePtr pointee) 
        : Type(TypeKind::Pointer), pointee_(pointee) {}
    
    TypePtr GetPointeeType() const { return pointee_; }
    std::string ToString() const override { return pointee_->ToString() + "*"; }
    size_t GetSize() const override { return 8; }
    size_t GetAlignment() const override { return 8; }

private:
    TypePtr pointee_;
};

// ============================================================================
// TYPE INFERENCE
// ============================================================================

class TypeInference {
public:
    TypeInference() = default;
    
    // Infer type from expression
    TypePtr InferType(const AST::Expression& expr);
    
    // Unify two types (find common type)
    TypePtr Unify(TypePtr t1, TypePtr t2);
    
    // Check if conversion is possible
    bool CanConvert(TypePtr from, TypePtr to);
 
    // Get implicit conversion cost
    int ConversionCost(TypePtr from, TypePtr to);

private:
    std::unordered_map<std::string, TypePtr> type_cache_;
};

// ============================================================================
// TYPE CHECKER
// ============================================================================

class TypeChecker {
public:
 TypeChecker();
    
    // Type check entire program
    bool Check(AST::Program& program);
    
    // Type check statement
    bool CheckStatement(AST::Statement& stmt);
    
    // Type check expression and return its type
    TypePtr CheckExpression(AST::Expression& expr);
    
    // Report type error
    void ReportError(const std::string& message, const SourceLocation& loc);
    
 // Get errors
    const std::vector<std::string>& GetErrors() const { return errors_; }
    bool HasErrors() const { return !errors_.empty(); }

private:
    TypeInference inference_;
std::unordered_map<std::string, TypePtr> symbol_types_;
    std::vector<std::string> errors_;
    
    // Built-in types
    TypePtr void_type_;
    TypePtr int_type_;
    TypePtr float_type_;
    TypePtr bool_type_;
    TypePtr string_type_;
    TypePtr duration_type_;
    TypePtr dodecagram_type_;
    
    void InitializeBuiltInTypes();
};

// ============================================================================
// TYPE REGISTRY
// ============================================================================

class TypeRegistry {
public:
    static TypeRegistry& Instance();
    
    // Register type
    void RegisterType(const std::string& name, TypePtr type);
    
    // Get type by name
    TypePtr GetType(const std::string& name) const;
    
    // Built-in types
    TypePtr GetIntType(int bits = 64, bool is_signed = true);
    TypePtr GetFloatType(int bits = 64);
    TypePtr GetBoolType();
    TypePtr GetStringType();
    TypePtr GetDurationType();
    TypePtr GetDodecagramType();
    TypePtr GetVoidType();
    
    // Create composite types
    TypePtr CreateArrayType(TypePtr element, size_t size);
    TypePtr CreatePointerType(TypePtr pointee);
  TypePtr CreateFunctionType(TypePtr return_type, std::vector<TypePtr> params);

private:
    TypeRegistry();
    std::unordered_map<std::string, TypePtr> types_;
};

} // namespace TypeSystem
} // namespace Snow
