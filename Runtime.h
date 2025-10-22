#pragma once

#include <string>
#include <vector>

namespace Snow {
namespace Runtime {

// ============================================================================
// RUNTIME LIBRARY FUNCTIONS
// ============================================================================

// Output functions
void Say(const std::string& message);
void Say(int64_t value);
void Say(const std::string& message, int64_t value);

// Time functions
void Wait(int64_t nanoseconds);
int64_t GetCurrentTime();

// Dodecagram conversion functions
std::string DecToDod(int64_t decimal);
int64_t DodToDec(const std::string& dodecagram);
void ShowDod(int64_t value);

// Memory allocation
void* Allocate(size_t size);
void Deallocate(void* ptr);

// Math functions
int64_t Abs(int64_t value);
int64_t Min(int64_t a, int64_t b);
int64_t Max(int64_t a, int64_t b);

} // namespace Runtime
} // namespace Snow
