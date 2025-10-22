#include "Types.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Snow {

// ============================================================================
// DODECAGRAM NUMBER IMPLEMENTATION
// ============================================================================

DodecagramNumber DodecagramNumber::FromDodecagram(const std::string& dode_str) {
    int64_t result = 0;
    int64_t base = 1;
    
    std::string str = dode_str;
 // Convert to lowercase for consistency
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    
    // Parse right to left
    for (int i = static_cast<int>(str.length()) - 1; i >= 0; --i) {
        char c = str[i];
 int digit;
        
        if (c >= '0' && c <= '9') {
  digit = c - '0';
        } else if (c == 'a') {
            digit = 10;
        } else if (c == 'b') {
            digit = 11;
        } else {
 throw std::runtime_error("Invalid dodecagram digit: " + std::string(1, c));
        }
 
        result += digit * base;
        base *= 12;
    }
    
    return DodecagramNumber(result);
}

DodecagramNumber DodecagramNumber::FromDecimal(const std::string& dec_str) {
    // Simple decimal parsing
    int64_t value = std::stoll(dec_str);
    return DodecagramNumber(value);
}

std::string DodecagramNumber::ToDodecagram() const {
    if (value_ == 0) return "0";
    
  std::string result;
    int64_t temp = value_;
    bool negative = temp < 0;
    if (negative) temp = -temp;
    
    while (temp > 0) {
        int digit = temp % 12;
        if (digit < 10) {
            result = char('0' + digit) + result;
   } else if (digit == 10) {
 result = 'a' + result;
        } else { // digit == 11
   result = 'b' + result;
     }
        temp /= 12;
    }
    
    if (negative) result = "-" + result;
    return result;
}

// ============================================================================
// DURATION IMPLEMENTATION
// ============================================================================

Duration::Duration(const DodecagramNumber& value, TimeUnit unit) {
    int64_t base_value = value.ToDecimal();
    
    switch (unit) {
        case TimeUnit::Nanoseconds:
       nanoseconds_ = base_value;
            break;
        case TimeUnit::Milliseconds:
            // 1 dozisecond = 1/12 second = 83,333,333 ns (approx)
          nanoseconds_ = base_value * 83333333LL;
   break;
     case TimeUnit::Seconds:
   // In base-12: 1s = 12 doziseconds
     nanoseconds_ = base_value * 1000000000LL;
  break;
        case TimeUnit::Minutes:
          nanoseconds_ = base_value * 60LL * 1000000000LL;
   break;
        case TimeUnit::Hours:
    nanoseconds_ = base_value * 3600LL * 1000000000LL;
            break;
    }
}

double Duration::ToUnit(TimeUnit unit) const {
    switch (unit) {
        case TimeUnit::Nanoseconds:
            return static_cast<double>(nanoseconds_);
        case TimeUnit::Milliseconds:
   return static_cast<double>(nanoseconds_) / 83333333.0;
   case TimeUnit::Seconds:
     return static_cast<double>(nanoseconds_) / 1000000000.0;
        case TimeUnit::Minutes:
          return static_cast<double>(nanoseconds_) / 60000000000.0;
        case TimeUnit::Hours:
            return static_cast<double>(nanoseconds_) / 3600000000000.0;
        default:
      return 0.0;
    }
}

// ============================================================================
// SOURCE LOCATION IMPLEMENTATION
// ============================================================================

std::string SourceLocation::ToString() const {
    std::stringstream ss;
    if (!filename.empty()) {
        ss << filename << ":";
  }
    ss << line << ":" << column;
    return ss.str();
}

} // namespace Snow
