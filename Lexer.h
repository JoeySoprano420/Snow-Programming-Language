#pragma once

#include "../Common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>

namespace Snow {

// ============================================================================
// FORWARD DECLARATIONS FOR MEMORY OPTIMIZATION
// ============================================================================

struct LabeledTokenContainer;
class StringInterner;
class KeywordTrie;

// ============================================================================
// COMPREHENSIVE TOKEN TYPES
// ============================================================================

enum class TokenType {
    // ========================================================================
    // LITERALS
    // ========================================================================
    DODECAGRAM,          // Base-12 number: 10, 3b, a5
    DECIMAL_LITERAL,     // Explicit decimal: 10#47
    BINARY_LITERAL,      // Binary: 0b1010
    HEX_LITERAL,         // Hexadecimal: 0x2A
    OCTAL_LITERAL,       // Octal: 0o52
    FLOAT_LITERAL, // Floating point: 3.14, 1.5e10
    STRING,           // String literal: "text"
    CHAR,   // Character literal: 'a'
  RAW_STRING,    // Raw string: r"text\n"
    INTERPOLATED_STRING, // String interpolation: $"value: {x}"
    MULTILINE_STRING,    // Multiline: """text"""
BOOLEAN_TRUE,    // true
    BOOLEAN_FALSE,       // false
    NULL_LITERAL,        // null/nil
    
    // ========================================================================
    // IDENTIFIERS
    // ========================================================================
    IDENTIFIER,          // Regular identifier
    TYPE_IDENTIFIER,     // Type name (capitalized)
    MACRO_IDENTIFIER,    // CIAM macro: @macro_name
    LABEL_IDENTIFIER,    // Label: :label_name
    
// ========================================================================
    // KEYWORDS - DECLARATIONS
    // ========================================================================
    KW_FN,      // Fn - function declaration
    KW_LET,    // let - variable declaration
    KW_CONST,            // const - constant declaration
    KW_VAR,          // var - mutable variable
    KW_TYPE,           // type - type alias
    KW_STRUCT,   // struct - structure definition
    KW_ENUM,             // enum - enumeration
    KW_UNION,   // union - union type
    KW_TRAIT,   // trait - interface
    KW_IMPL,   // impl - implementation
    KW_CLASS,       // class - class definition
    KW_INTERFACE,        // interface - interface definition
 KW_NAMESPACE,        // namespace - namespace declaration
    KW_MODULE,      // module - module declaration
    KW_PACKAGE,          // package - package declaration
    
    // ========================================================================
    // KEYWORDS - CONTROL FLOW
    // ========================================================================
    KW_IF,       // if
    KW_ELSE,     // else
    KW_ELIF,      // elif/elsif
    KW_UNLESS,    // unless (inverted if)
    KW_WHEN,             // when (pattern matching)
    KW_MATCH,            // match (exhaustive pattern matching)
    KW_CASE,             // case
    KW_DEFAULT,          // default
    KW_SWITCH,    // switch
    KW_WHILE,            // while
    KW_FOR,    // for
    KW_FOREACH,    // foreach
    KW_LOOP,   // loop (infinite)
    KW_DO,               // do (do-while)
    KW_UNTIL,     // until
    KW_REPEAT,           // repeat
    KW_BREAK,         // break
    KW_CONTINUE,  // continue
    KW_RETURN,           // return/ret
    KW_YIELD,   // yield (generators)
    KW_GOTO,             // goto
    KW_FALLTHROUGH,      // fallthrough
    
  // ========================================================================
    // KEYWORDS - TEMPORAL (Snow-specific)
    // ========================================================================
    KW_EVERY, // every - temporal loop
    KW_AFTER,      // after - delayed execution
    KW_BEFORE,    // before - pre-condition
    KW_DURING,           // during - duration-based
    KW_WAIT,             // wait - sleep/delay
    KW_TIMEOUT,          // timeout - time limit
    KW_DERIVE,     // derive - derivative (d/dt)
    KW_INTEGRATE,        // integrate - integral
    KW_AT,   // at - specific time
    KW_SCHEDULE,// schedule - scheduled execution
    KW_DOZISECOND,       // dozisecond - 1/12 second unit
    KW_TEMPORAL,         // temporal - time-aware block
    
    // ========================================================================
    // KEYWORDS - DODECAGRAM (Snow-specific)
 // ========================================================================
    KW_DOZEN,            // dozen - 12
    KW_GROSS,     // gross - 144
    KW_GREAT_GROSS,      // great-gross - 1728
    KW_BASE12,           // base12 - explicit base-12
    KW_DUODECIMAL,       // duodecimal - another name
    
    // ========================================================================
    // KEYWORDS - MODIFIERS
    // ========================================================================
    KW_PUB,     // pub/public
    KW_PRIV,          // priv/private
    KW_PROT,         // prot/protected
    KW_INTERNAL,      // internal
    KW_EXTERN,           // extern
    KW_STATIC,           // static
    KW_INLINE,  // inline
    KW_VIRTUAL,       // virtual
 KW_ABSTRACT,         // abstract
    KW_FINAL,            // final
    KW_OVERRIDE,      // override
    KW_CONST_KW,         // const (as modifier)
    KW_MUTABLE,  // mutable
    KW_VOLATILE,         // volatile
    KW_ASYNC,       // async
    KW_AWAIT,            // await
    KW_LAZY,             // lazy
    
    // ========================================================================
    // KEYWORDS - MEMORY & POINTERS
    // ========================================================================
    KW_NEW,          // new - allocation
  KW_DELETE,           // delete - deallocation
    KW_REF,        // ref - reference
KW_PTR,       // ptr - pointer
    KW_DEREF,        // deref - dereference
    KW_ADDR,             // addr - address-of
    KW_SIZEOF,           // sizeof
    KW_ALIGNOF,          // alignof
    KW_OFFSETOF,         // offsetof
    KW_UNSAFE,           // unsafe block
    
    // ========================================================================
    // KEYWORDS - PARALLELISM & CONCURRENCY
    // ========================================================================
    KW_PARALLEL,         // parallel - parallel execution
    KW_CONCURRENT,       // concurrent
    KW_ATOMIC,           // atomic
    KW_SYNC,             // sync/synchronized
    KW_LOCK,     // lock
    KW_UNLOCK,           // unlock
    KW_THREAD,  // thread
    KW_TASK,             // task
    KW_SPAWN,            // spawn - create thread
    KW_JOIN,          // join - wait for thread
    KW_CHANNEL,  // channel - message passing
    KW_SEND,   // send - channel send
    KW_RECV,  // recv - channel receive
    KW_SELECT, // select - channel selection
    
    // ========================================================================
    // KEYWORDS - EXCEPTION HANDLING
    // ========================================================================
    KW_TRY,      // try
KW_CATCH,            // catch
    KW_FINALLY,    // finally
    KW_THROW,            // throw
    KW_RAISE,   // raise
    KW_PANIC,         // panic
    KW_RECOVER,  // recover
    KW_ENSURE,       // ensure
    KW_RESCUE,           // rescue
    
    // ========================================================================
    // KEYWORDS - TESTING & DEBUGGING
    // ========================================================================
    KW_ASSERT,           // assert
    KW_DEBUG,      // debug
    KW_TEST,         // test
    KW_BENCHMARK,        // benchmark
    KW_PROFILE,    // profile
    KW_TRACE,         // trace
    
    // ========================================================================
    // KEYWORDS - METAPROGRAMMING
// ========================================================================
    KW_MACRO,     // macro
    KW_TEMPLATE,// template
    KW_GENERIC,          // generic
    KW_WHERE,// where (type constraints)
    KW_TYPEOF,           // typeof
    KW_NAMEOF,           // nameof
    KW_REFLECT,        // reflect
    KW_COMPILE_TIME,     // compile-time
    
    // ========================================================================
    // KEYWORDS - SPECIAL
    // ========================================================================
    KW_IMPORT,           // import
    KW_EXPORT,           // export
    KW_USE,   // use
    KW_AS,   // as (aliasing)
    KW_FROM,    // from
    KW_WITH,   // with
    KW_IN,      // in
    KW_IS,               // is (type check)
    KW_AND,    // and (logical)
    KW_OR,          // or (logical)
    KW_NOT,  // not (logical)
    KW_XOR,       // xor (logical)
    KW_END,      // end (block terminator)
    KW_THEN,      // then
    KW_OVER,          // over
KW_SAY,     // Say (output)
    KW_PRINT,   // print
    KW_INPUT,          // input
    
    // ========================================================================
    // ARITHMETIC OPERATORS
    // ========================================================================
  OP_PLUS,     // +
    OP_MINUS,     // -
    OP_MULTIPLY,         // *
    OP_DIVIDE,           // /
    OP_MODULO,           // %
    OP_POWER,            // ** or ^
    OP_FLOOR_DIV,  // //
    OP_INCREMENT,        // ++
 OP_DECREMENT,// --
    
    // ========================================================================
    // ASSIGNMENT OPERATORS
    // ========================================================================
    OP_ASSIGN,           // =
    OP_PLUS_ASSIGN, // +=
    OP_MINUS_ASSIGN, // -=
    OP_MULT_ASSIGN,      // *=
    OP_DIV_ASSIGN,       // /=
    OP_MOD_ASSIGN,       // %=
    OP_POWER_ASSIGN,   // **=
    OP_AND_ASSIGN,   // &=
    OP_OR_ASSIGN,     // |=
    OP_XOR_ASSIGN,       // ^=
    OP_SHL_ASSIGN, // <<=
 OP_SHR_ASSIGN,       // >>=
    
    // ========================================================================
    // COMPARISON OPERATORS
    // ========================================================================
    OP_EQ,   // ==
    OP_NEQ,    // !=
    OP_LT,        // <
    OP_GT,    // >
    OP_LTE,        // <=
    OP_GTE,          // >=
  OP_SPACESHIP,        // <=> (three-way comparison)
    
    // ========================================================================
    // LOGICAL OPERATORS
    // ========================================================================
    OP_LOGICAL_AND,  // &&
    OP_LOGICAL_OR,    // ||
    OP_LOGICAL_NOT,      // !
    OP_LOGICAL_XOR,      // ^^
    
    // ========================================================================
    // BITWISE OPERATORS
    // ========================================================================
    OP_BIT_AND,          // &
    OP_BIT_OR,           // |
 OP_BIT_XOR,          // ^
    OP_BIT_NOT,          // ~
    OP_SHL,        // <<
OP_SHR,    // >
    OP_ROTATE_LEFT,      // <<<
    OP_ROTATE_RIGHT,     // >>>
    
    // ========================================================================
    // SPECIAL OPERATORS
  // ========================================================================
 OP_ARROW,          // ->
    OP_FAT_ARROW,        // =>
    OP_DOUBLE_COLON,     // ::
 OP_TRIPLE_DOT,    // ... (variadic/spread)
    OP_DOUBLE_DOT,       // .. (range)
    OP_SAFE_NAV,    // ?. (safe navigation)
    OP_NULL_COALESCE,    // ?? (null coalescing)
    OP_ELVIS,         // ?: (elvis operator)
    OP_PIPELINE,    // |> (pipeline)
    OP_COMPOSE,          // >> (function composition)
    OP_DOLLAR,           // $ (various uses)
    OP_AT,    // @ (attribute/annotation)
    OP_HASH,             // # (directive/meta)
    OP_QUESTION,         // ? (optional/ternary)
    OP_EXCLAIM,// ! (unwrap/assert)
    OP_AMPERSAND,        // & (reference)
    OP_ASTERISK,         // * (dereference/pointer)
    
    // ========================================================================
    // DELIMITERS
    // ========================================================================
    LPAREN,        // (
    RPAREN,              // )
    LBRACKET,// [
    RBRACKET, // ]
    LBRACE,              // {
    RBRACE,  // }
    LANGLE,              // < (also used for comparison)
    RANGLE,              // > (also used for comparison)
    SEMICOLON,     // ;
 COLON,        // :
    COMMA,        // ,
    DOT,            // .
    DOUBLE_DOT_DEL,      // .. (range delimiter)
    TRIPLE_DOT_DEL,      // ... (spread)
  
    // ========================================================================
    // TIME UNITS (Snow-specific)
    // ========================================================================
    TIME_NANOSECOND,     // ns
    TIME_MICROSECOND,    // us
    TIME_MILLISECOND,    // ms
    TIME_DOZISECOND, // ds (1/12 second)
    TIME_SECOND,         // s
    TIME_MINUTE,         // m
    TIME_HOUR,           // h
    TIME_DAY,    // d
    TIME_WEEK,       // w
    TIME_MONTH,          // mo
    TIME_YEAR,           // y
    
    // ========================================================================
  // SPECIAL TOKENS
    // ========================================================================
 COMMENT_SINGLE,      // # comment
    COMMENT_MULTI,  // ## comment ##
    COMMENT_DOC,     // #! documentation comment
    DIRECTIVE,           // #directive (preprocessor-like)
    ANNOTATION,    // @annotation
    ATTRIBUTE,   // [attribute]
    PRAGMA,   // #pragma
    NEWLINE,           // \n
    INDENT,         // Indentation level
    DEDENT,            // Dedentation
    ENDOFFILE,         // EOF
  INVALID,  // Error token
    
    // ========================================================================
    // PREPROCESSOR/DIRECTIVES
    // ========================================================================
    DIR_INCLUDE,         // #include
  DIR_DEFINE,// #define
    DIR_IFDEF,      // #ifdef
    DIR_IFNDEF,          // #ifndef
    DIR_ENDIF, // #endif
    DIR_IF,    // #if
    DIR_ELSE,   // #else
    DIR_ELIF, // #elif
    DIR_PRAGMA,   // #pragma
    DIR_ERROR,  // #error
    DIR_WARNING,         // #warning
    DIR_LINE,          // #line
};

// ============================================================================
// TOKEN
// ============================================================================

struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    
    // Extended token data
    DodecagramNumber numeric_value;
    double float_value;
    TimeUnit time_unit;
    int64_t integer_value;
    bool boolean_value;
    char char_value;
    
    // Token metadata
    int indent_level;           // For indentation-based syntax
  bool is_interpolated;     // For string interpolation
    std::string raw_text;   // Original text before processing
    
    Token(TokenType t = TokenType::INVALID, 
    const std::string& lex = "",
          const SourceLocation& loc = SourceLocation())
        : type(t), lexeme(lex), location(loc), 
          float_value(0.0), integer_value(0), 
        boolean_value(false), char_value('\0'),
          indent_level(0), is_interpolated(false) {}
    
    std::string ToString() const;
    bool IsLiteral() const;
    bool IsKeyword() const;
    bool IsOperator() const;
    bool IsDelimiter() const;
    bool IsTimeUnit() const;
};

// ============================================================================
// LEXER ERROR
// ============================================================================

struct LexerError {
    std::string message;
    SourceLocation location;
    
    LexerError(const std::string& msg, const SourceLocation& loc)
     : message(msg), location(loc) {}
    
    std::string ToString() const {
        return location.ToString() + ": Lexer error: " + message;
    }
};

// ============================================================================
// LEXER CONFIGURATION
// ============================================================================

struct LexerConfig {
    bool case_insensitive_keywords = true;
    bool allow_unicode_identifiers = true;
    bool allow_emoji_identifiers = false;
    bool track_whitespace = false;
    bool track_comments = false;
    bool enable_trigraphs = false;
    bool enable_digraphs = false;
    bool enable_raw_strings = true;
    bool enable_interpolated_strings = true;
    bool enable_multiline_strings = true;
    bool enable_implicit_semicolons = true;
    bool enable_indentation_syntax = false;
    int tab_width = 4;
};

// ============================================================================
// COMPREHENSIVE LEXER
// ============================================================================

class Lexer {
public:
    explicit Lexer(const std::string& source, 
         const std::string& filename = "",
         const LexerConfig& config = LexerConfig());
    
    // Destructor needs to be defined in .cpp where complete types are available
    ~Lexer();

    // ========================================================================
    // CORE TOKENIZATION
    // ========================================================================
    
    // Get next token
    Token NextToken();
    
    // Peek at next token without consuming
    Token PeekToken();
    
    // Peek ahead multiple tokens
    Token PeekAhead(int count);
    
    // Get all tokens
  std::vector<Token> TokenizeAll();
    
    // Check if at end
    bool IsAtEnd() const { return current_ >= source_.length(); }
    
    // Get current location
  SourceLocation GetLocation() const {
        return SourceLocation(filename_, line_, column_);
    }
    
    // ========================================================================
    // KEYWORD MANAGEMENT
    // ========================================================================
    
    // Extend keyword dictionary
    void AddKeyword(const std::string& keyword, TokenType type);
    void RemoveKeyword(const std::string& keyword);
    bool IsKeyword(const std::string& word) const;
    
    // Register CIAM macro
    void RegisterMacro(const std::string& name, TokenType type);
    
  // ========================================================================
    // ERROR HANDLING
    // ========================================================================
    
    const std::vector<LexerError>& GetErrors() const { return errors_; }
  bool HasErrors() const { return !errors_.empty(); }
    void ClearErrors() { errors_.clear(); }
    
    // ========================================================================
    // CONFIGURATION
    // ========================================================================
    
    void SetConfig(const LexerConfig& config) { config_ = config; }
 const LexerConfig& GetConfig() const { return config_; }
    
    // ========================================================================
    // STATISTICS
// ========================================================================
    
    struct Statistics {
    int total_tokens;
        int total_lines;
        int total_characters;
        int keywords_count;
        int identifiers_count;
    int literals_count;
      int operators_count;
     int comments_count;
        int errors_count;
    };
    
    Statistics GetStatistics() const { return stats_; }

    // ========================================================================
  // LABELED CONTAINER ACCESS (NEW!)
  // ========================================================================
    
    // Quick access to categorized tokens
    const std::vector<Token*>* GetTokensByCategory(const std::string& category) const;
    const std::vector<Token*>* GetTokensByType(TokenType type) const;
    Token* GetTokenByLexeme(const std::string& lexeme) const;
    
    // Memory efficiency metrics
    size_t GetMemorySavings() const;

private:
  // ========================================================================
    // INTERNAL STATE
    // ========================================================================
    
    std::string source_;
    std::string filename_;
  size_t current_;
    size_t line_;
    size_t column_;
    size_t start_; // Start of current token
  
    LexerConfig config_;
 Statistics stats_;
    
    // Token buffer for peeking
    std::vector<Token> token_buffer_;
  
 // Error collection
    std::vector<LexerError> errors_;
    
    // Keyword dictionaries
    std::unordered_map<std::string, TokenType> keywords_;
    std::unordered_map<std::string, TokenType> macros_;
    std::unordered_set<std::string> reserved_words_;
    
    // Indentation tracking
    std::vector<int> indent_stack_;
    
    // ========================================================================
    // LABELED CONTAINERS & MEMORY OPTIMIZATION (NEW!)
    // ========================================================================
    
    std::unique_ptr<LabeledTokenContainer> token_container_;
    std::unique_ptr<StringInterner> string_interner_;
    std::unique_ptr<KeywordTrie> keyword_trie_;
    
    // ========================================================================
    // CHARACTER OPERATIONS
  // ========================================================================
    
    char Peek() const;
    char PeekNext() const;
    char PeekAt(int offset) const;
    char Advance();
    bool Match(char expected);
    bool MatchAny(const std::string& chars);
    void SkipWhitespace();
    void SkipComment();
    
// ========================================================================
    // TOKEN CREATION
    // ========================================================================
    
  Token MakeToken(TokenType type);
    Token MakeToken(TokenType type, const std::string& lexeme);
    Token ErrorToken(const std::string& message);
    void AddError(const std::string& message);
    
    // ========================================================================
    // SCANNING METHODS
    // ========================================================================
    
    // Literals
    Token ScanString();
    Token ScanRawString();
    Token ScanInterpolatedString();
    Token ScanMultilineString();
    Token ScanCharacter();
    Token ScanNumber();
    Token ScanDodecagram();
    Token ScanBinary();
    Token ScanHexadecimal();
    Token ScanOctal();
    Token ScanFloat();
    
    // Identifiers & Keywords
    Token ScanIdentifier();
    Token ScanTypeIdentifier();
Token ScanMacroIdentifier();
    Token ScanLabel();
    
    // Special
    Token ScanTimeUnit(const std::string& base);
    Token ScanDirective();
    Token ScanAnnotation();
    Token ScanAttribute();
    
    // Operators
    Token ScanOperator();
    TokenType MatchCompoundOperator();
    
    // ========================================================================
    // HELPER METHODS
    // ========================================================================
    
    void InitializeKeywords();
    void InitializeReservedWords();
    std::string ToLower(const std::string& str) const;
    std::string CurrentLexeme() const;
    
    bool IsDigit(char c) const;
    bool IsDodecagramDigit(char c) const;
    bool IsHexDigit(char c) const;
    bool IsOctalDigit(char c) const;
    bool IsBinaryDigit(char c) const;
    bool IsAlpha(char c) const;
    bool IsAlphaNumeric(char c) const;
    bool IsWhitespace(char c) const;
    bool IsNewline(char c) const;
    bool IsIdentifierStart(char c) const;
    bool IsIdentifierContinue(char c) const;
    bool IsUnicodeIdentifier(char c) const;
    
    // Number parsing helpers
    int64_t ParseInteger(const std::string& str, int base);
    double ParseFloat(const std::string& str);
    DodecagramNumber ParseDodecagram(const std::string& str);
    
    // String processing
    std::string ProcessEscapeSequences(const std::string& str);
    std::string ProcessStringInterpolation(const std::string& str);
    
    // Indentation handling
    void HandleIndentation();
    Token EmitIndent();
Token EmitDedent();
};

// ============================================================================
// TOKEN STREAM (for buffered access)
// ============================================================================

class TokenStream {
public:
    explicit TokenStream(Lexer& lexer);
    
    Token Next();
    Token Peek();
    Token PeekAhead(int count);
    bool Match(TokenType type);
    bool MatchAny(const std::vector<TokenType>& types);
    void Expect(TokenType type, const std::string& message);
    
    bool IsAtEnd() const { return current_token_.type == TokenType::ENDOFFILE; }
    const Token& Current() const { return current_token_; }
    
private:
Lexer& lexer_;
    Token current_token_;
    std::vector<Token> buffer_;
};

} // namespace Snow
