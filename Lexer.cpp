#include "Lexer.h"
#include <cctype>
#include <algorithm>

namespace Snow {

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
// LEXER IMPLEMENTATION
// ============================================================================

Lexer::Lexer(const std::string& source, const std::string& filename, const LexerConfig& config)
    : source_(source), filename_(filename), current_(0), line_(1), column_(1),
      start_(0), config_(config) {
    InitializeKeywords();
 InitializeReservedWords();
  indent_stack_.push_back(0);
stats_ = Statistics();
}

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
    
    // More keywords (abbreviated list - see full implementation)
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
}

void Lexer::RemoveKeyword(const std::string& keyword) {
  keywords_.erase(ToLower(keyword));
}

bool Lexer::IsKeyword(const std::string& word) const {
    return keywords_.find(ToLower(word)) != keywords_.end();
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
    Token token(type, lexeme, GetLocation());
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
    size_t start = current_;
    std::string numstr;
    
    // Check for base prefix (10# for decimal, 12# for explicit dodecagram)
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
        // Not a prefix, backtrack
  current_ = saved_pos;
     }
}
    }
    
    // Scan digits (0-9, a, b)
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
    
    // Parse the numeric value
    if (explicit_decimal) {
        token.numeric_value = DodecagramNumber::FromDecimal(numstr);
    } else {
 // Default is dodecagram
        token.numeric_value = DodecagramNumber::FromDodecagram(numstr);
    }
    
  return token;
}

Token Lexer::ScanTimeUnit(const std::string& base) {
    std::string unit;
    
 // Scan unit letters
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
    
    std::string lower = ToLower(identifier);
    auto it = keywords_.find(lower);
 if (it != keywords_.end()) {
        stats_.keywords_count++;
        return MakeToken(it->second, identifier);
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
    
    // Numbers (simplified - full version in expanded implementation)
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
   tokens.push_back(tok);
    }
    tokens.push_back(tok); // Include EOF
    return tokens;
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
