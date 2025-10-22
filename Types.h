#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

namespace Snow {

// ============================================================================
// DODECAGRAM NUMBER SYSTEM (Base-12)
// ============================================================================

class DodecagramNumber {
public:
    DodecagramNumber() : value_(0) {}
    explicit DodecagramNumber(int64_t decimal_value) : value_(decimal_value) {}
    
    // Parse from base-12 string (e.g., "3b" = 47 decimal)
    static DodecagramNumber FromDodecagram(const std::string& dode_str);
    
    // Parse from decimal string with prefix (e.g., "10#47")
    static DodecagramNumber FromDecimal(const std::string& dec_str);
    
    // Convert to dodecagram string representation
    std::string ToDodecagram() const;
    
    // Get decimal value
    int64_t ToDecimal() const { return value_; }
    
    // Arithmetic operators
    DodecagramNumber operator+(const DodecagramNumber& other) const {
    return DodecagramNumber(value_ + other.value_);
    }
    
 DodecagramNumber operator-(const DodecagramNumber& other) const {
   return DodecagramNumber(value_ - other.value_);
    }
    
    DodecagramNumber operator*(const DodecagramNumber& other) const {
        return DodecagramNumber(value_ * other.value_);
    }
    
    DodecagramNumber operator/(const DodecagramNumber& other) const {
        if (other.value_ == 0) throw std::runtime_error("Division by zero");
        return DodecagramNumber(value_ / other.value_);
 }
    
    // Comparison operators
    bool operator==(const DodecagramNumber& other) const { return value_ == other.value_; }
    bool operator!=(const DodecagramNumber& other) const { return value_ != other.value_; }
    bool operator<(const DodecagramNumber& other) const { return value_ < other.value_; }
    bool operator>(const DodecagramNumber& other) const { return value_ > other.value_; }
    bool operator<=(const DodecagramNumber& other) const { return value_ <= other.value_; }
    bool operator>=(const DodecagramNumber& other) const { return value_ >= other.value_; }

private:
    int64_t value_; // Internal storage in decimal
};

// ============================================================================
// DURATION SYSTEM
// ============================================================================

enum class TimeUnit {
    Nanoseconds,   // ns
    Milliseconds,  // ms (doziseconds)
    Seconds,       // s
    Minutes,       // m
    Hours          // h
};

class Duration {
public:
    Duration() : nanoseconds_(0) {}
    Duration(const DodecagramNumber& value, TimeUnit unit);
    
// Get duration in nanoseconds
    int64_t GetNanoseconds() const { return nanoseconds_; }
    
    // Convert to specific unit
    double ToUnit(TimeUnit unit) const;
    
    // Arithmetic
    Duration operator+(const Duration& other) const {
        Duration result;
        result.nanoseconds_ = nanoseconds_ + other.nanoseconds_;
        return result;
    }
  
 Duration operator-(const Duration& other) const {
        Duration result;
        result.nanoseconds_ = nanoseconds_ - other.nanoseconds_;
        return result;
    }
    
    Duration operator/(int64_t divisor) const {
        Duration result;
 result.nanoseconds_ = nanoseconds_ / divisor;
        return result;
    }
    
    // Comparison
    bool operator==(const Duration& other) const { return nanoseconds_ == other.nanoseconds_; }
    bool operator<(const Duration& other) const { return nanoseconds_ < other.nanoseconds_; }
    bool operator>(const Duration& other) const { return nanoseconds_ > other.nanoseconds_; }

private:
    int64_t nanoseconds_; // Internal storage
};

// ============================================================================
// SOURCE LOCATION
// ============================================================================

struct SourceLocation {
 std::string filename;
    int line;
    int column;
    
    SourceLocation(const std::string& file = "", int l = 0, int c = 0)
      : filename(file), line(l), column(c) {}
    
    std::string ToString() const;
};

} // namespace Snow
