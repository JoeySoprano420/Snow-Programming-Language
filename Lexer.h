#pragma once

#include "../Common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace Snow {

// ============================================================================
// TOKEN TYPES
// ============================================================================

enum class TokenType {
    // Literals
    DODECAGRAM,      // Base-12 number
  STRING,          // String literal
    IDENTIFIER,      // Variable/function names
    
    // Keywords
    KW_FN,           // Fn
    KW_LET,          // let
    KW_IF,           // if
    KW_ELSE,         // else
    KW_WHILE,        // while
    KW_FOR,     // for
    KW_EVERY,      // every
    KW_PARALLEL,     // parallel
    KW_AND,          // and
    KW_DERIVE,       // derive
    KW_WAIT,  // wait
    KW_RET,          // ret
    KW_BREAK,     // break
    KW_CONTINUE,     // continue
  KW_NAMESPACE,    // namespace
    KW_USE,          // use
    KW_END,// end
    KW_SAY,          // Say/say
    KW_OVER,         // over
    
    // Operators
    OP_PLUS,         // +
    OP_MINUS,      // -
    OP_MULTIPLY,     // *
    OP_DIVIDE,       // /
    OP_ASSIGN,       // =
OP_EQ,     // ==
    OP_NEQ,      // !=
  OP_LT,           // <
    OP_GT,     // >
    OP_LTE,          // <=
    OP_GTE,   // >=
    
    // Delimiters
    LPAREN,          // (
    RPAREN,          // )
    LBRACKET,        // [
    RBRACKET,        // ]
    LBRACE,      // {
    RBRACE,          // }
    SEMICOLON,       // ;
    COLON,        // :
    COMMA,           // ,
    DOT,     // .
    
    // Special
    TIME_UNIT,       // ns, ms, s, m, h
    COMMENT,     // # or ## ... ##
 NEWLINE,         // \n
    ENDOFFILE,       // EOF
    INVALID      // Error token
};

// ============================================================================
// TOKEN
// ============================================================================

struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    
// For numeric literals
    DodecagramNumber numeric_value;
    
    // For time units
    TimeUnit time_unit;
    
    Token(TokenType t = TokenType::INVALID, 
          const std::string& lex = "",
          const SourceLocation& loc = SourceLocation())
        : type(t), lexeme(lex), location(loc) {}
    
    std::string ToString() const;
};

// ============================================================================
// LEXER
// ============================================================================

class Lexer {
public:
    explicit Lexer(const std::string& source, const std::string& filename = "");
    
    // Get next token
    Token NextToken();

    // Peek at next token without consuming
  Token PeekToken();
    
    // Check if at end
    bool IsAtEnd() const { return current_ >= source_.length(); }
    
    // Get current location
    SourceLocation GetLocation() const {
        return SourceLocation(filename_, line_, column_);
    }
    
    // Extend keyword dictionary
    void AddKeyword(const std::string& keyword, TokenType type);

private:
    std::string source_;
    std::string filename_;
    size_t current_;
 size_t line_;
    size_t column_;
    
    // Keyword dictionary
    std::unordered_map<std::string, TokenType> keywords_;
    
    // Helper functions
    char Peek() const;
    char PeekNext() const;
    char Advance();
  bool Match(char expected);
    void SkipWhitespace();
    void SkipComment();
 
    Token MakeToken(TokenType type);
    Token MakeToken(TokenType type, const std::string& lexeme);
    Token ErrorToken(const std::string& message);
    
    Token ScanString();
    Token ScanNumber();
Token ScanIdentifier();
    Token ScanTimeUnit(const std::string& base);
    
    void InitializeKeywords();
    std::string ToLower(const std::string& str) const;
};

} // namespace Snow
