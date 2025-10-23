#include "Lexer.h"
#include <cctype>
#include <algorithm>

namespace Snow {

// ============================================================================
// MEMORY POOL - Fast allocation for tokens
// ============================================================================

template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t block_size = 1024) 
      : block_size_(block_size), current_block_(0), current_index_(0) {
        AllocateBlock();
    }
    
    ~MemoryPool() {
      for (auto* block : blocks_) {
     delete[] block;
        }
    }
    
    T* Allocate() {
  if (current_index_ >= block_size_) {
       AllocateBlock();
}
    return &blocks_[current_block_][current_index_++];
    }
    
    void Reset() {
current_block_ = 0;
        current_index_ = 0;
    }
    
private:
    void AllocateBlock() {
blocks_.push_back(new T[block_size_]);
  current_block_ = blocks_.size() - 1;
        current_index_ = 0;
    }
    
    std::vector<T*> blocks_;
    size_t block_size_;
    size_t current_block_;
    size_t current_index_;
};

// ============================================================================
// LABELED CONTAINERS - Fast lookup and categorization
// ============================================================================

struct LabeledTokenContainer {
    // Fast lookup by category
    std::unordered_map<std::string, std::vector<Token*>> by_category_;
    
    // Fast lookup by type
    std::unordered_map<TokenType, std::vector<Token*>> by_type_;
    
    // Fast lookup by lexeme
    std::unordered_map<std::string, Token*> by_lexeme_;
 
    // All tokens in order
    std::vector<Token*> all_tokens_;
    
    // Memory pool for tokens
    MemoryPool<Token> token_pool_;
    
    // Add token with automatic categorization
    Token* Add(const Token& token, const std::string& category = "") {
   Token* new_token = token_pool_.Allocate();
        *new_token = token;
        
     all_tokens_.push_back(new_token);
        by_type_[token.type].push_back(new_token);
        
     if (!token.lexeme.empty()) {
     by_lexeme_[token.lexeme] = new_token;
        }
        
  if (!category.empty()) {
            by_category_[category].push_back(new_token);
        }
  
   // Auto-categorize
  if (token.IsKeyword()) {
      by_category_["keywords"].push_back(new_token);
      } else if (token.IsLiteral()) {
            by_category_["literals"].push_back(new_token);
        } else if (token.IsOperator()) {
 by_category_["operators"].push_back(new_token);
        }
        
    return new_token;
    }
    
    // Quick lookup methods
    const std::vector<Token*>* GetByCategory(const std::string& category) const {
  auto it = by_category_.find(category);
    return (it != by_category_.end()) ? &it->second : nullptr;
    }
    
    const std::vector<Token*>* GetByType(TokenType type) const {
      auto it = by_type_.find(type);
        return (it != by_type_.end()) ? &it->second : nullptr;
    }
    
    Token* GetByLexeme(const std::string& lexeme) const {
  auto it = by_lexeme_.find(lexeme);
        return (it != by_lexeme_.end()) ? it->second : nullptr;
    }
    
    void Clear() {
     by_category_.clear();
 by_type_.clear();
        by_lexeme_.clear();
all_tokens_.clear();
        token_pool_.Reset();
    }
};

// ============================================================================
// STRING INTERNING - Memory efficient string storage
// ============================================================================

class StringInterner {
public:
    const std::string& Intern(const std::string& str) {
        auto it = interned_strings_.find(str);
        if (it != interned_strings_.end()) {
return *it;
        }
        auto result = interned_strings_.insert(str);
        return *result.first;
    }
  
    size_t GetMemorySavings() const {
        size_t total_length = 0;
        for (const auto& str : interned_strings_) {
 total_length += str.length();
        }
        return total_length * (reference_count_ - interned_strings_.size());
    }
    
    void IncrementReference() { reference_count_++; }
    
private:
    std::unordered_set<std::string> interned_strings_;
    size_t reference_count_ = 0;
};

// ============================================================================
// FAST KEYWORD TRIE - O(1) keyword lookup
// ============================================================================

struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    TokenType token_type = TokenType::INVALID;
    bool is_end = false;
    
    ~TrieNode() {
        for (auto& pair : children) {
     delete pair.second;
        }
    }
};

class KeywordTrie {
public:
    KeywordTrie() : root_(new TrieNode()) {}
    
    ~KeywordTrie() {
delete root_;
    }
    
    void Insert(const std::string& keyword, TokenType type) {
        TrieNode* node = root_;
   for (char c : keyword) {
            char lower_c = std::tolower(c);
          if (node->children.find(lower_c) == node->children.end()) {
       node->children[lower_c] = new TrieNode();
            }
          node = node->children[lower_c];
        }
        node->is_end = true;
     node->token_type = type;
    }
    
    bool Search(const std::string& word, TokenType& out_type) const {
        TrieNode* node = root_;
        for (char c : word) {
            char lower_c = std::tolower(c);
   auto it = node->children.find(lower_c);
            if (it == node->children.end()) {
           return false;
          }
            node = it->second;
        }
        if (node->is_end) {
        out_type = node->token_type;
       return true;
}
        return false;
    }
    
private:
    TrieNode* root_;
};

// ============================================================================
// TOKEN IMPLEMENTATION
// ============================================================================

std::string Token::ToString() const {
    return "Token(" + std::to_string(static_cast<int>(type)) + ", '" + lexeme + "')";
}

bool Token::IsLiteral() const {
    return type >= TokenType::DODECAGRAM && type <= TokenType::NULL_LITERAL;
}

bool Token::IsKeyword() const {
    return type >= TokenType::KW_FN && type <= TokenType::KW_INPUT;
}

bool Token::IsOperator() const {
    return type >= TokenType::OP_PLUS && type <= TokenType::OP_ASTERISK;
}

bool Token::IsDelimiter() const {
    return type >= TokenType::LPAREN && type <= TokenType::TRIPLE_DOT_DEL;
}

bool Token::IsTimeUnit() const {
    return type >= TokenType::TIME_NANOSECOND && type <= TokenType::TIME_YEAR;
}

// ============================================================================
// LEXER IMPLEMENTATION WITH LABELED CONTAINERS
// ============================================================================

Lexer::Lexer(const std::string& source, const std::string& filename, const LexerConfig& config)
    : source_(source), filename_(filename), current_(0), line_(1), column_(1),
      start_(0), config_(config) {
    InitializeKeywords();
    InitializeReservedWords();
    indent_stack_.push_back(0);
    stats_ = Statistics();
    
 // Initialize labeled containers
    token_container_ = std::make_unique<LabeledTokenContainer>();
    string_interner_ = std::make_unique<StringInterner>();
    keyword_trie_ = std::make_unique<KeywordTrie>();
    
    // Build keyword trie for fast lookup
    for (const auto& kv : keywords_) {
        keyword_trie_->Insert(kv.first, kv.second);
  }
}

// Destructor must be defined after complete types are available
Lexer::~Lexer() = default;

void Lexer::InitializeKeywords() {
    // Core language keywords
    keywords_["fn"] = TokenType::KW_FN;
    keywords_["let"] = TokenType::KW_LET;
    keywords_["const"] = TokenType::KW_CONST;
    keywords_["if"] = TokenType::KW_IF;
    keywords_["else"] = TokenType::KW_ELSE;
    keywords_["while"] = TokenType::KW_WHILE;
    keywords_["for"] = TokenType::KW_FOR;
    keywords_["every"] = TokenType::KW_EVERY;
    keywords_["parallel"] = TokenType::KW_PARALLEL;
    keywords_["and"] = TokenType::KW_AND;
    keywords_["derive"] = TokenType::KW_DERIVE;
    keywords_["wait"] = TokenType::KW_WAIT;
    keywords_["return"] = TokenType::KW_RETURN;
    keywords_["ret"] = TokenType::KW_RETURN;  // Alias
    keywords_["break"] = TokenType::KW_BREAK;
    keywords_["continue"] = TokenType::KW_CONTINUE;
    keywords_["namespace"] = TokenType::KW_NAMESPACE;
    keywords_["use"] = TokenType::KW_USE;
    keywords_["end"] = TokenType::KW_END;
    keywords_["say"] = TokenType::KW_SAY;
    keywords_["over"] = TokenType::KW_OVER;
    
    // Temporal keywords
    keywords_["after"] = TokenType::KW_AFTER;
    keywords_["before"] = TokenType::KW_BEFORE;
    keywords_["during"] = TokenType::KW_DURING;
    keywords_["timeout"] = TokenType::KW_TIMEOUT;
    keywords_["dozisecond"] = TokenType::KW_DOZISECOND;
    keywords_["temporal"] = TokenType::KW_TEMPORAL;
    
    // Dodecagram keywords
    keywords_["dozen"] = TokenType::KW_DOZEN;
    keywords_["gross"] = TokenType::KW_GROSS;
    keywords_["base12"] = TokenType::KW_BASE12;
    
    // More keywords
  keywords_["struct"] = TokenType::KW_STRUCT;
  keywords_["enum"] = TokenType::KW_ENUM;
    keywords_["match"] = TokenType::KW_MATCH;
    keywords_["async"] = TokenType::KW_ASYNC;
    keywords_["await"] = TokenType::KW_AWAIT;
    keywords_["thread"] = TokenType::KW_THREAD;
    keywords_["lock"] = TokenType::KW_LOCK;
    keywords_["try"] = TokenType::KW_TRY;
    keywords_["catch"] = TokenType::KW_CATCH;
  keywords_["assert"] = TokenType::KW_ASSERT;
  
  // Literals
    keywords_["true"] = TokenType::BOOLEAN_TRUE;
    keywords_["false"] = TokenType::BOOLEAN_FALSE;
    keywords_["null"] = TokenType::NULL_LITERAL;
    keywords_["nil"] = TokenType::NULL_LITERAL;
}

void Lexer::InitializeReservedWords() {
    for (const auto& kv : keywords_) {
        reserved_words_.insert(kv.first);
    }
}

void Lexer::AddKeyword(const std::string& keyword, TokenType type) {
    keywords_[ToLower(keyword)] = type;
    keyword_trie_->Insert(ToLower(keyword), type);
}

void Lexer::RemoveKeyword(const std::string& keyword) {
    keywords_.erase(ToLower(keyword));
}

bool Lexer::IsKeyword(const std::string& word) const {
    TokenType dummy;
    return keyword_trie_->Search(word, dummy);
}

void Lexer::RegisterMacro(const std::string& name, TokenType type) {
    macros_[name] = type;
}

std::string Lexer::ToLower(const std::string& str) const {
    std::string result = str;
    if (config_.case_insensitive_keywords) {
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    }
  return result;
}

char Lexer::Peek() const {
    if (IsAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::PeekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

char Lexer::PeekAt(int offset) const {
    if (current_ + offset >= source_.length()) return '\0';
    return source_[current_ + offset];
}

char Lexer::Advance() {
    if (IsAtEnd()) return '\0';
 char c = source_[current_++];
    column_++;
    if (c == '\n') {
  line_++;
        column_ = 1;
    }
    return c;
}

bool Lexer::Match(char expected) {
    if (IsAtEnd() || Peek() != expected) return false;
    Advance();
    return true;
}

bool Lexer::MatchAny(const std::string& chars) {
 for (char c : chars) {
        if (Match(c)) return true;
    }
    return false;
}

void Lexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char c = Peek();
if (c == ' ' || c == '\t' || c == '\r') {
 Advance();
        } else if (c == '\n') {
    if (config_.enable_implicit_semicolons) {
     // Could insert semicolon here
      }
       Advance();
        } else {
            break;
     }
    }
}

void Lexer::SkipComment() {
    if (Peek() == '#') {
if (PeekNext() == '#') {
          // Multi-line comment
Advance(); // First #
   Advance(); // Second #
        while (!IsAtEnd()) {
       if (Peek() == '#' && PeekNext() == '#') {
     Advance();
    Advance();
       break;
  }
                Advance();
   }
        } else {
            // Single-line comment
      while (!IsAtEnd() && Peek() != '\n') {
Advance();
         }
        }
    }
}

Token Lexer::MakeToken(TokenType type) {
    return Token(type, "", GetLocation());
}

Token Lexer::MakeToken(TokenType type, const std::string& lexeme) {
  // Use string interning for memory efficiency
    const std::string& interned = string_interner_->Intern(lexeme);
    string_interner_->IncrementReference();
    
    Token token(type, interned, GetLocation());
    return token;
}

Token Lexer::ErrorToken(const std::string& message) {
    return MakeToken(TokenType::INVALID, message);
}

void Lexer::AddError(const std::string& message) {
    errors_.push_back(LexerError(message, GetLocation()));
    stats_.errors_count++;
}

Token Lexer::ScanString() {
    start_ = current_;
    Advance(); // Opening "
    
    std::string value;
    while (!IsAtEnd() && Peek() != '"') {
 if (Peek() == '\n') {
         AddError("Unterminated string");
            return ErrorToken("Unterminated string");
        }
        if (Peek() == '\\') {
            Advance();
            if (!IsAtEnd()) {
     char escaped = Advance();
 switch (escaped) {
        case 'n': value += '\n'; break;
          case 't': value += '\t'; break;
    case 'r': value += '\r'; break;
   case '\\': value += '\\'; break;
               case '"': value += '"'; break;
     default: value += escaped; break;
      }
   }
        } else {
         value += Advance();
        }
 }
    
    if (IsAtEnd()) {
        AddError("Unterminated string");
     return ErrorToken("Unterminated string");
    }
    
    Advance(); // Closing "
    stats_.literals_count++;
    return MakeToken(TokenType::STRING, value);
}

Token Lexer::ScanNumber() {
    std::string numstr;
    
    // Check for base prefix
    bool explicit_decimal = false;
    bool explicit_dodecagram = false;
    
    if (Peek() == '1') {
        if (PeekNext() == '0' || PeekNext() == '2') {
            char prefix = PeekNext();
  size_t saved_pos = current_;
         Advance();
            Advance();
      if (Peek() == '#') {
         Advance();
    if (prefix == '0') explicit_decimal = true;
        else explicit_dodecagram = true;
      } else {
           current_ = saved_pos;
   }
        }
    }
    
    // Scan digits
    while (!IsAtEnd()) {
        char c = Peek();
        if ((c >= '0' && c <= '9') || c == 'a' || c == 'A' || c == 'b' || c == 'B') {
         numstr += c;
         Advance();
        } else {
            break;
    }
    }
 
    // Check for time unit suffix
    if (!IsAtEnd() && (Peek() == 'n' || Peek() == 'm' || Peek() == 's' || Peek() == 'h')) {
        return ScanTimeUnit(numstr);
    }
    
    Token token = MakeToken(TokenType::DODECAGRAM, numstr);
    
    if (explicit_decimal) {
        token.numeric_value = DodecagramNumber::FromDecimal(numstr);
    } else {
        token.numeric_value = DodecagramNumber::FromDodecagram(numstr);
    }
    
    stats_.literals_count++;
    return token;
}

Token Lexer::ScanTimeUnit(const std::string& base) {
    std::string unit;
    
    while (!IsAtEnd() && std::isalpha(Peek())) {
     unit += Advance();
    }
    
    std::string lower_unit = ToLower(unit);
Token token = MakeToken(TokenType::TIME_MILLISECOND, base + lower_unit);
    token.numeric_value = DodecagramNumber::FromDodecagram(base);
    
    if (lower_unit == "ns") {
  token.type = TokenType::TIME_NANOSECOND;
     token.time_unit = TimeUnit::Nanoseconds;
    } else if (lower_unit == "ms") {
        token.type = TokenType::TIME_MILLISECOND;
        token.time_unit = TimeUnit::Milliseconds;
    } else if (lower_unit == "s") {
      token.type = TokenType::TIME_SECOND;
        token.time_unit = TimeUnit::Seconds;
    } else if (lower_unit == "m") {
        token.type = TokenType::TIME_MINUTE;
        token.time_unit = TimeUnit::Minutes;
  } else if (lower_unit == "h") {
        token.type = TokenType::TIME_HOUR;
        token.time_unit = TimeUnit::Hours;
    } else {
        AddError("Invalid time unit: " + unit);
        return ErrorToken("Invalid time unit");
    }

    return token;
}

Token Lexer::ScanIdentifier() {
    std::string identifier;
    while (!IsAtEnd() && (std::isalnum(Peek()) || Peek() == '_')) {
        identifier += Advance();
    }
    
  // Fast keyword lookup using trie
    TokenType keyword_type;
    if (keyword_trie_->Search(identifier, keyword_type)) {
        stats_.keywords_count++;
 return MakeToken(keyword_type, identifier);
    }
    
    stats_.identifiers_count++;
    return MakeToken(TokenType::IDENTIFIER, identifier);
}

Token Lexer::NextToken() {
    SkipWhitespace();
    
    while (Peek() == '#') {
        SkipComment();
        SkipWhitespace();
    }
    
    if (IsAtEnd()) {
        return MakeToken(TokenType::ENDOFFILE);
    }
    
    start_ = current_;
 char c = Peek();
    
    // String literals
    if (c == '"') {
        return ScanString();
    }
    
  // Numbers
    if (std::isdigit(c) || c == 'a' || c == 'A' || c == 'b' || c == 'B') {
   return ScanNumber();
    }
  
    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
 return ScanIdentifier();
    }
    
    // Single-character tokens
    Advance();
    switch (c) {
      case '(': return MakeToken(TokenType::LPAREN, "(");
        case ')': return MakeToken(TokenType::RPAREN, ")");
        case '[': return MakeToken(TokenType::LBRACKET, "[");
        case ']': return MakeToken(TokenType::RBRACKET, "]");
 case '{': return MakeToken(TokenType::LBRACE, "{");
      case '}': return MakeToken(TokenType::RBRACE, "}");
        case ';': return MakeToken(TokenType::SEMICOLON, ";");
        case ':': return MakeToken(TokenType::COLON, ":");
        case ',': return MakeToken(TokenType::COMMA, ",");
        case '.': return MakeToken(TokenType::DOT, ".");
   case '+': return MakeToken(TokenType::OP_PLUS, "+");
        case '-': return MakeToken(TokenType::OP_MINUS, "-");
      case '*': return MakeToken(TokenType::OP_MULTIPLY, "*");
  case '/': return MakeToken(TokenType::OP_DIVIDE, "/");
 case '=':
        if (Match('=')) return MakeToken(TokenType::OP_EQ, "==");
     return MakeToken(TokenType::OP_ASSIGN, "=");
        case '!':
            if (Match('=')) return MakeToken(TokenType::OP_NEQ, "!=");
   return MakeToken(TokenType::OP_EXCLAIM, "!");
        case '<':
    if (Match('=')) return MakeToken(TokenType::OP_LTE, "<=");
            return MakeToken(TokenType::OP_LT, "<");
 case '>':
  if (Match('=')) return MakeToken(TokenType::OP_GTE, ">=");
       return MakeToken(TokenType::OP_GT, ">");
      default:
   AddError("Unexpected character: " + std::string(1, c));
  return ErrorToken("Unexpected character");
    }
}

Token Lexer::PeekToken() {
    size_t saved_current = current_;
    size_t saved_line = line_;
    size_t saved_column = column_;
  
  Token token = NextToken();
    
    current_ = saved_current;
    line_ = saved_line;
    column_ = saved_column;
  
    return token;
}

Token Lexer::PeekAhead(int count) {
    std::vector<size_t> saved_state = {current_, line_, column_};
  Token token;
    for (int i = 0; i <= count; i++) {
   token = NextToken();
    }
    current_ = saved_state[0];
    line_ = saved_state[1];
    column_ = saved_state[2];
    return token;
}

std::vector<Token> Lexer::TokenizeAll() {
 std::vector<Token> tokens;
    Token tok;
    while ((tok = NextToken()).type != TokenType::ENDOFFILE) {
        // Add to labeled container for fast lookup
        token_container_->Add(tok);
        tokens.push_back(tok);
    }
    tokens.push_back(tok); // Include EOF
    
    stats_.total_tokens = static_cast<int>(tokens.size());
    stats_.total_lines = static_cast<int>(line_);
    stats_.total_characters = static_cast<int>(source_.length());
    
    return tokens;
}

// ============================================================================
// LABELED CONTAINER ACCESS METHODS
// ============================================================================

const std::vector<Token*>* Lexer::GetTokensByCategory(const std::string& category) const {
    return token_container_->GetByCategory(category);
}

const std::vector<Token*>* Lexer::GetTokensByType(TokenType type) const {
  return token_container_->GetByType(type);
}

Token* Lexer::GetTokenByLexeme(const std::string& lexeme) const {
    return token_container_->GetByLexeme(lexeme);
}

size_t Lexer::GetMemorySavings() const {
    return string_interner_->GetMemorySavings();
}

// Stub implementations for other methods
Token Lexer::ScanRawString() { return ErrorToken("Not implemented"); }
Token Lexer::ScanInterpolatedString() { return ErrorToken("Not implemented"); }
Token Lexer::ScanMultilineString() { return ErrorToken("Not implemented"); }
Token Lexer::ScanCharacter() { return ErrorToken("Not implemented"); }
Token Lexer::ScanDodecagram() { return ScanNumber(); }
Token Lexer::ScanBinary() { return ErrorToken("Not implemented"); }
Token Lexer::ScanHexadecimal() { return ErrorToken("Not implemented"); }
Token Lexer::ScanOctal() { return ErrorToken("Not implemented"); }
Token Lexer::ScanFloat() { return ErrorToken("Not implemented"); }
Token Lexer::ScanTypeIdentifier() { return ScanIdentifier(); }
Token Lexer::ScanMacroIdentifier() { return ErrorToken("Not implemented"); }
Token Lexer::ScanLabel() { return ErrorToken("Not implemented"); }
Token Lexer::ScanDirective() { return ErrorToken("Not implemented"); }
Token Lexer::ScanAnnotation() { return ErrorToken("Not implemented"); }
Token Lexer::ScanAttribute() { return ErrorToken("Not implemented"); }
Token Lexer::ScanOperator() { return ErrorToken("Not implemented"); }
TokenType Lexer::MatchCompoundOperator() { return TokenType::INVALID; }

std::string Lexer::CurrentLexeme() const {
    return source_.substr(start_, current_ - start_);
}

bool Lexer::IsDigit(char c) const { return std::isdigit(c); }
bool Lexer::IsDodecagramDigit(char c) const {
    return (c >= '0' && c <= '9') || c == 'a' || c == 'A' || c == 'b' || c == 'B';
}
bool Lexer::IsHexDigit(char c) const { return std::isxdigit(c); }
bool Lexer::IsOctalDigit(char c) const { return c >= '0' && c <= '7'; }
bool Lexer::IsBinaryDigit(char c) const { return c == '0' || c == '1'; }
bool Lexer::IsAlpha(char c) const { return std::isalpha(c); }
bool Lexer::IsAlphaNumeric(char c) const { return std::isalnum(c); }
bool Lexer::IsWhitespace(char c) const { return std::isspace(c); }
bool Lexer::IsNewline(char c) const { return c == '\n'; }
bool Lexer::IsIdentifierStart(char c) const { return std::isalpha(c) || c == '_'; }
bool Lexer::IsIdentifierContinue(char c) const { return std::isalnum(c) || c == '_'; }
bool Lexer::IsUnicodeIdentifier(char c) const { return c > 127; }

int64_t Lexer::ParseInteger(const std::string& str, int base) {
    return std::stoll(str, nullptr, base);
}

double Lexer::ParseFloat(const std::string& str) {
    return std::stod(str);
}

DodecagramNumber Lexer::ParseDodecagram(const std::string& str) {
    return DodecagramNumber::FromDodecagram(str);
}

std::string Lexer::ProcessEscapeSequences(const std::string& str) {
    return str; // Simplified
}

std::string Lexer::ProcessStringInterpolation(const std::string& str) {
    return str; // Simplified
}

void Lexer::HandleIndentation() {
    // Stub
}

Token Lexer::EmitIndent() {
    return MakeToken(TokenType::INDENT);
}

Token Lexer::EmitDedent() {
    return MakeToken(TokenType::DEDENT);
}

// TokenStream implementation
TokenStream::TokenStream(Lexer& lexer) : lexer_(lexer) {
    current_token_ = lexer_.NextToken();
}

Token TokenStream::Next() {
 Token tok = current_token_;
    current_token_ = lexer_.NextToken();
    return tok;
}

Token TokenStream::Peek() {
    return current_token_;
}

Token TokenStream::PeekAhead(int count) {
    return lexer_.PeekAhead(count);
}

bool TokenStream::Match(TokenType type) {
  if (current_token_.type == type) {
        Next();
        return true;
    }
    return false;
}

bool TokenStream::MatchAny(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (Match(type)) return true;
    }
    return false;
}

void TokenStream::Expect(TokenType type, const std::string& message) {
    if (!Match(type)) {
throw std::runtime_error(message);
    }
}

} // namespace Snow
