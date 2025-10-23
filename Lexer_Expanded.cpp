#include "Lexer.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Snow {

// ============================================================================
// TOKEN IMPLEMENTATION
// ============================================================================

std::string Token::ToString() const {
    std::stringstream ss;
ss << "Token(" << static_cast<int>(type) << ", '" << lexeme << "' at " 
       << location.ToString() << ")";
    return ss.str();
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
    indent_stack_.push_back(0); // Base indentation
    
    stats_ = Statistics();
}

void Lexer::InitializeKeywords() {
    // Declarations
    keywords_["fn"] = TokenType::KW_FN;
    keywords_["let"] = TokenType::KW_LET;
    keywords_["const"] = TokenType::KW_CONST;
    keywords_["var"] = TokenType::KW_VAR;
    keywords_["type"] = TokenType::KW_TYPE;
    keywords_["struct"] = TokenType::KW_STRUCT;
    keywords_["enum"] = TokenType::KW_ENUM;
    keywords_["union"] = TokenType::KW_UNION;
    keywords_["trait"] = TokenType::KW_TRAIT;
    keywords_["impl"] = TokenType::KW_IMPL;
    keywords_["class"] = TokenType::KW_CLASS;
    keywords_["interface"] = TokenType::KW_INTERFACE;
    keywords_["namespace"] = TokenType::KW_NAMESPACE;
    keywords_["module"] = TokenType::KW_MODULE;
    keywords_["package"] = TokenType::KW_PACKAGE;
    
  // Control flow
    keywords_["if"] = TokenType::KW_IF;
    keywords_["else"] = TokenType::KW_ELSE;
    keywords_["elif"] = TokenType::KW_ELIF;
    keywords_["elsif"] = TokenType::KW_ELIF;
    keywords_["unless"] = TokenType::KW_UNLESS;
    keywords_["when"] = TokenType::KW_WHEN;
    keywords_["match"] = TokenType::KW_MATCH;
    keywords_["case"] = TokenType::KW_CASE;
    keywords_["default"] = TokenType::KW_DEFAULT;
    keywords_["switch"] = TokenType::KW_SWITCH;
    keywords_["while"] = TokenType::KW_WHILE;
    keywords_["for"] = TokenType::KW_FOR;
    keywords_["foreach"] = TokenType::KW_FOREACH;
 keywords_["loop"] = TokenType::KW_LOOP;
  keywords_["do"] = TokenType::KW_DO;
    keywords_["until"] = TokenType::KW_UNTIL;
    keywords_["repeat"] = TokenType::KW_REPEAT;
    keywords_["break"] = TokenType::KW_BREAK;
    keywords_["continue"] = TokenType::KW_CONTINUE;
    keywords_["return"] = TokenType::KW_RETURN;
    keywords_["ret"] = TokenType::KW_RETURN;
    keywords_["yield"] = TokenType::KW_YIELD;
    keywords_["goto"] = TokenType::KW_GOTO;
    keywords_["fallthrough"] = TokenType::KW_FALLTHROUGH;
    
    // Temporal
    keywords_["every"] = TokenType::KW_EVERY;
    keywords_["after"] = TokenType::KW_AFTER;
  keywords_["before"] = TokenType::KW_BEFORE;
    keywords_["during"] = TokenType::KW_DURING;
    keywords_["wait"] = TokenType::KW_WAIT;
    keywords_["timeout"] = TokenType::KW_TIMEOUT;
    keywords_["derive"] = TokenType::KW_DERIVE;
    keywords_["integrate"] = TokenType::KW_INTEGRATE;
    keywords_["at"] = TokenType::KW_AT;
    keywords_["schedule"] = TokenType::KW_SCHEDULE;
    keywords_["dozisecond"] = TokenType::KW_DOZISECOND;
    keywords_["temporal"] = TokenType::KW_TEMPORAL;
    
    // Dodecagram
    keywords_["dozen"] = TokenType::KW_DOZEN;
    keywords_["gross"] = TokenType::KW_GROSS;
    keywords_["base12"] = TokenType::KW_BASE12;
    keywords_["duodecimal"] = TokenType::KW_DUODECIMAL;
    
    // Modifiers
    keywords_["pub"] = TokenType::KW_PUB;
    keywords_["public"] = TokenType::KW_PUB;
    keywords_["priv"] = TokenType::KW_PRIV;
    keywords_["private"] = TokenType::KW_PRIV;
    keywords_["prot"] = TokenType::KW_PROT;
    keywords_["protected"] = TokenType::KW_PROT;
    keywords_["internal"] = TokenType::KW_INTERNAL;
    keywords_["extern"] = TokenType::KW_EXTERN;
    keywords_["static"] = TokenType::KW_STATIC;
    keywords_["inline"] = TokenType::KW_INLINE;
    keywords_["virtual"] = TokenType::KW_VIRTUAL;
    keywords_["abstract"] = TokenType::KW_ABSTRACT;
    keywords_["final"] = TokenType::KW_FINAL;
    keywords_["override"] = TokenType::KW_OVERRIDE;
    keywords_["mutable"] = TokenType::KW_MUTABLE;
    keywords_["volatile"] = TokenType::KW_VOLATILE;
    keywords_["async"] = TokenType::KW_ASYNC;
    keywords_["await"] = TokenType::KW_AWAIT;
    keywords_["lazy"] = TokenType::KW_LAZY;
    
    // Memory
    keywords_["new"] = TokenType::KW_NEW;
    keywords_["delete"] = TokenType::KW_DELETE;
    keywords_["ref"] = TokenType::KW_REF;
    keywords_["ptr"] = TokenType::KW_PTR;
    keywords_["deref"] = TokenType::KW_DEREF;
    keywords_["addr"] = TokenType::KW_ADDR;
    keywords_["sizeof"] = TokenType::KW_SIZEOF;
    keywords_["alignof"] = TokenType::KW_ALIGNOF;
 keywords_["offsetof"] = TokenType::KW_OFFSETOF;
    keywords_["unsafe"] = TokenType::KW_UNSAFE;
    
    // Parallelism
    keywords_["parallel"] = TokenType::KW_PARALLEL;
    keywords_["concurrent"] = TokenType::KW_CONCURRENT;
    keywords_["atomic"] = TokenType::KW_ATOMIC;
    keywords_["sync"] = TokenType::KW_SYNC;
    keywords_["synchronized"] = TokenType::KW_SYNC;
    keywords_["lock"] = TokenType::KW_LOCK;
    keywords_["unlock"] = TokenType::KW_UNLOCK;
    keywords_["thread"] = TokenType::KW_THREAD;
    keywords_["task"] = TokenType::KW_TASK;
    keywords_["spawn"] = TokenType::KW_SPAWN;
    keywords_["join"] = TokenType::KW_JOIN;
    keywords_["channel"] = TokenType::KW_CHANNEL;
    keywords_["send"] = TokenType::KW_SEND;
    keywords_["recv"] = TokenType::KW_RECV;
    keywords_["select"] = TokenType::KW_SELECT;
    
    // Exceptions
    keywords_["try"] = TokenType::KW_TRY;
    keywords_["catch"] = TokenType::KW_CATCH;
    keywords_["finally"] = TokenType::KW_FINALLY;
    keywords_["throw"] = TokenType::KW_THROW;
  keywords_["raise"] = TokenType::KW_RAISE;
    keywords_["panic"] = TokenType::KW_PANIC;
    keywords_["recover"] = TokenType::KW_RECOVER;
    keywords_["ensure"] = TokenType::KW_ENSURE;
    keywords_["rescue"] = TokenType::KW_RESCUE;
    
    // Testing
    keywords_["assert"] = TokenType::KW_ASSERT;
    keywords_["debug"] = TokenType::KW_DEBUG;
    keywords_["test"] = TokenType::KW_TEST;
    keywords_["benchmark"] = TokenType::KW_BENCHMARK;
  keywords_["profile"] = TokenType::KW_PROFILE;
    keywords_["trace"] = TokenType::KW_TRACE;
    
  // Metaprogramming
    keywords_["macro"] = TokenType::KW_MACRO;
    keywords_["template"] = TokenType::KW_TEMPLATE;
    keywords_["generic"] = TokenType::KW_GENERIC;
    keywords_["where"] = TokenType::KW_WHERE;
    keywords_["typeof"] = TokenType::KW_TYPEOF;
    keywords_["nameof"] = TokenType::KW_NAMEOF;
    keywords_["reflect"] = TokenType::KW_REFLECT;
    
    // Special
    keywords_["import"] = TokenType::KW_IMPORT;
    keywords_["export"] = TokenType::KW_EXPORT;
    keywords_["use"] = TokenType::KW_USE;
    keywords_["as"] = TokenType::KW_AS;
    keywords_["from"] = TokenType::KW_FROM;
    keywords_["with"] = TokenType::KW_WITH;
    keywords_["in"] = TokenType::KW_IN;
    keywords_["is"] = TokenType::KW_IS;
    keywords_["and"] = TokenType::KW_AND;
    keywords_["or"] = TokenType::KW_OR;
    keywords_["not"] = TokenType::KW_NOT;
    keywords_["xor"] = TokenType::KW_XOR;
    keywords_["end"] = TokenType::KW_END;
    keywords_["then"] = TokenType::KW_THEN;
    keywords_["over"] = TokenType::KW_OVER;
    keywords_["say"] = TokenType::KW_SAY;
    keywords_["print"] = TokenType::KW_PRINT;
    keywords_["input"] = TokenType::KW_INPUT;
    
    // Literals
    keywords_["true"] = TokenType::BOOLEAN_TRUE;
    keywords_["false"] = TokenType::BOOLEAN_FALSE;
    keywords_["null"] = TokenType::NULL_LITERAL;
    keywords_["nil"] = TokenType::NULL_LITERAL;
}

void Lexer::InitializeReservedWords() {
    // Add all keywords to reserved words
    for (const auto& kv : keywords_) {
        reserved_words_.insert(kv.first);
    }
}

// ... (continuing in next part due to length)
