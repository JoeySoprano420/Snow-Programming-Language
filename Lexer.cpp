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

// ============================================================================
// LEXER IMPLEMENTATION
// ============================================================================

Lexer::Lexer(const std::string& source, const std::string& filename)
    : source_(source), filename_(filename), current_(0), line_(1), column_(1) {
    InitializeKeywords();
}

void Lexer::InitializeKeywords() {
    // Case-insensitive keyword mapping
    keywords_["fn"] = TokenType::KW_FN;
    keywords_["let"] = TokenType::KW_LET;
    keywords_["if"] = TokenType::KW_IF;
keywords_["else"] = TokenType::KW_ELSE;
    keywords_["while"] = TokenType::KW_WHILE;
    keywords_["for"] = TokenType::KW_FOR;
    keywords_["every"] = TokenType::KW_EVERY;
    keywords_["parallel"] = TokenType::KW_PARALLEL;
    keywords_["and"] = TokenType::KW_AND;
  keywords_["derive"] = TokenType::KW_DERIVE;
    keywords_["wait"] = TokenType::KW_WAIT;
    keywords_["ret"] = TokenType::KW_RET;
    keywords_["break"] = TokenType::KW_BREAK;
    keywords_["continue"] = TokenType::KW_CONTINUE;
    keywords_["namespace"] = TokenType::KW_NAMESPACE;
    keywords_["use"] = TokenType::KW_USE;
  keywords_["end"] = TokenType::KW_END;
 keywords_["say"] = TokenType::KW_SAY;
    keywords_["over"] = TokenType::KW_OVER;
}

void Lexer::AddKeyword(const std::string& keyword, TokenType type) {
    keywords_[ToLower(keyword)] = type;
}

std::string Lexer::ToLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
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

void Lexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char c = Peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
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

Token Lexer::ScanString() {
    size_t start = current_;
    Advance(); // Opening "
    
    std::string value;
    while (!IsAtEnd() && Peek() != '"') {
   if (Peek() == '\n') {
     return ErrorToken("Unterminated string");
     }
value += Advance();
    }
    
    if (IsAtEnd()) {
        return ErrorToken("Unterminated string");
    }
    
    Advance(); // Closing "
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
    Token token = MakeToken(TokenType::TIME_UNIT, base + lower_unit);
    token.numeric_value = DodecagramNumber::FromDodecagram(base);
    
    if (lower_unit == "ns") {
        token.time_unit = TimeUnit::Nanoseconds;
    } else if (lower_unit == "ms") {
        token.time_unit = TimeUnit::Milliseconds;
    } else if (lower_unit == "s") {
  token.time_unit = TimeUnit::Seconds;
    } else if (lower_unit == "m") {
        token.time_unit = TimeUnit::Minutes;
    } else if (lower_unit == "h") {
        token.time_unit = TimeUnit::Hours;
    } else {
    return ErrorToken("Invalid time unit: " + unit);
    }
    
    return token;
}

Token Lexer::ScanIdentifier() {
    std::string identifier;
    
    while (!IsAtEnd() && (std::isalnum(Peek()) || Peek() == '_')) {
        identifier += Advance();
    }
    
    // Check if it's a keyword (case-insensitive)
    std::string lower = ToLower(identifier);
    auto it = keywords_.find(lower);
    if (it != keywords_.end()) {
        return MakeToken(it->second, identifier);
    }
    
    return MakeToken(TokenType::IDENTIFIER, identifier);
}

Token Lexer::NextToken() {
    SkipWhitespace();
    
    // Skip comments
    while (Peek() == '#') {
        SkipComment();
        SkipWhitespace();
    }
    
    if (IsAtEnd()) {
        return MakeToken(TokenType::ENDOFFILE);
}
    
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
         return ErrorToken("Unexpected character '!'");

   case '<':
       if (Match('=')) return MakeToken(TokenType::OP_LTE, "<=");
            return MakeToken(TokenType::OP_LT, "<");
      case '>':
            if (Match('=')) return MakeToken(TokenType::OP_GTE, ">=");
  return MakeToken(TokenType::OP_GT, ">");
        default:
        return ErrorToken("Unexpected character: " + std::string(1, c));
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

} // namespace Snow
