#include "Runtime.h"
#include "../Common/Types.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

namespace Snow {
namespace Runtime {

// ============================================================================
// OUTPUT FUNCTIONS
// ============================================================================

void Say(const std::string& message) {
    std::cout << message << std::endl;
}

void Say(int64_t value) {
    std::cout << value << std::endl;
}

void Say(const std::string& message, int64_t value) {
    std::cout << message << value << std::endl;
}

// ============================================================================
// TIME FUNCTIONS
// ============================================================================

void Wait(int64_t nanoseconds) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(nanoseconds));
}

int64_t GetCurrentTime() {
 auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

// ============================================================================
// DODECAGRAM CONVERSION
// ============================================================================

std::string DecToDod(int64_t decimal) {
    DodecagramNumber num(decimal);
    return num.ToDodecagram();
}

int64_t DodToDec(const std::string& dodecagram) {
    DodecagramNumber num = DodecagramNumber::FromDodecagram(dodecagram);
    return num.ToDecimal();
}

void ShowDod(int64_t value) {
    std::cout << DecToDod(value) << std::endl;
}

// ============================================================================
// MEMORY MANAGEMENT
// ============================================================================

void* Allocate(size_t size) {
    return std::malloc(size);
}

void Deallocate(void* ptr) {
    std::free(ptr);
}

// ============================================================================
// MATH FUNCTIONS
// ============================================================================

int64_t Abs(int64_t value) {
 return value < 0 ? -value : value;
}

int64_t Min(int64_t a, int64_t b) {
    return a < b ? a : b;
}

int64_t Max(int64_t a, int64_t b) {
    return a > b ? a : b;
}

} // namespace Runtime
} // namespace Snow
