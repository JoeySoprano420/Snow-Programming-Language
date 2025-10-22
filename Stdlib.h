#pragma once

#include "../Common/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Snow {
namespace StdLib {

// ============================================================================
// I/O FUNCTIONS
// ============================================================================

// Output
void Say(const std::string& message);
void Sayln(const std::string& message);
void Print(const std::string& format, ...);
void Printf(const std::string& format, ...);

// Input
std::string ReadLine();
std::string Read();
char ReadChar();
int64_t ReadInt();
double ReadFloat();

// File I/O
class File {
public:
    static File* Open(const std::string& path, const std::string& mode);
    void Close();
    std::string ReadAll();
    std::string ReadLine();
    void Write(const std::string& data);
    void WriteLine(const std::string& line);
    bool IsEOF() const;
    
private:
    void* handle_;
    std::string path_;
};

// ============================================================================
// STRING FUNCTIONS
// ============================================================================

namespace String {
    int Length(const std::string& str);
    std::string Substring(const std::string& str, int start, int length);
    std::string ToUpper(const std::string& str);
    std::string ToLower(const std::string& str);
    std::string Trim(const std::string& str);
    std::string Replace(const std::string& str, const std::string& from, const std::string& to);
    std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
    std::string Join(const std::vector<std::string>& parts, const std::string& separator);
    bool StartsWith(const std::string& str, const std::string& prefix);
    bool EndsWith(const std::string& str, const std::string& suffix);
  bool Contains(const std::string& str, const std::string& substr);
    int IndexOf(const std::string& str, const std::string& substr);
}

// ============================================================================
// MATH FUNCTIONS
// ============================================================================

namespace Math {
    // Constants
    extern const double PI;
    extern const double E;
    extern const double PHI; // Golden ratio
    
    // Basic
    int64_t Abs(int64_t x);
    double Abs(double x);
    int64_t Min(int64_t a, int64_t b);
    int64_t Max(int64_t a, int64_t b);
    double Min(double a, double b);
    double Max(double a, double b);
    int64_t Clamp(int64_t x, int64_t min, int64_t max);
    double Clamp(double x, double min, double max);
    
    // Power & Roots
    double Pow(double base, double exp);
    double Sqrt(double x);
    double Cbrt(double x);
    
    // Exponential & Logarithm
    double Exp(double x);
    double Log(double x);
    double Log10(double x);
    double Log2(double x);
    
    // Trigonometry
    double Sin(double x);
    double Cos(double x);
    double Tan(double x);
    double Asin(double x);
    double Acos(double x);
    double Atan(double x);
    double Atan2(double y, double x);
    
    // Hyperbolic
    double Sinh(double x);
    double Cosh(double x);
    double Tanh(double x);
    
    // Rounding
    double Floor(double x);
    double Ceil(double x);
    double Round(double x);
    double Trunc(double x);
    
// Random
    void SetSeed(int64_t seed);
    int64_t Random();
    int64_t Random(int64_t max);
    int64_t Random(int64_t min, int64_t max);
    double RandomFloat();
    double RandomFloat(double max);
 double RandomFloat(double min, double max);
}

// ============================================================================
// DODECAGRAM FUNCTIONS
// ============================================================================

namespace Dodec {
    std::string ToDodecagram(int64_t decimal);
    int64_t FromDodecagram(const std::string& dode);
    void ShowDod(int64_t value);
  
  // Dodecagram math
 int64_t DodecAdd(int64_t a, int64_t b);
    int64_t DodecSub(int64_t a, int64_t b);
  int64_t DodecMul(int64_t a, int64_t b);
    int64_t DodecDiv(int64_t a, int64_t b);
 int64_t DodecMod(int64_t a, int64_t b);
    
    // Constants
    extern const int64_t DOZEN;  // 12
    extern const int64_t GROSS;  // 144
    extern const int64_t GREAT_GROSS; // 1728
}

// ============================================================================
// TIME & DURATION FUNCTIONS
// ============================================================================

namespace Time {
    int64_t Now();  // Nanoseconds since epoch
    int64_t Ticks(); // High-resolution timer
    void Sleep(int64_t nanoseconds);
    void SleepMs(int64_t milliseconds);
    void SleepSeconds(int64_t seconds);
    
    // Dozisecond functions
    int64_t NowDoziseconds();
    void SleepDoziseconds(int64_t doziseconds);
    
    // Date/Time
    struct DateTime {
  int year;
        int month;
    int day;
        int hour;
   int minute;
        int second;
        int nanosecond;
    };
    
    DateTime GetLocalTime();
    DateTime GetUTCTime();
    std::string FormatTime(const DateTime& dt, const std::string& format);
}

// ============================================================================
// COLLECTION FUNCTIONS
// ============================================================================

namespace Collections {
    // Array operations
    template<typename T>
    int Length(const std::vector<T>& arr) {
   return static_cast<int>(arr.size());
    }
    
    template<typename T>
    void Push(std::vector<T>& arr, const T& value) {
 arr.push_back(value);
    }
    
    template<typename T>
    T Pop(std::vector<T>& arr) {
        T value = arr.back();
      arr.pop_back();
      return value;
    }
    
    template<typename T>
    void Insert(std::vector<T>& arr, int index, const T& value) {
        arr.insert(arr.begin() + index, value);
    }
    
    template<typename T>
    void Remove(std::vector<T>& arr, int index) {
        arr.erase(arr.begin() + index);
    }
    
    template<typename T>
    void Clear(std::vector<T>& arr) {
        arr.clear();
    }
    
    template<typename T>
    bool Contains(const std::vector<T>& arr, const T& value) {
        return std::find(arr.begin(), arr.end(), value) != arr.end();
}
    
  template<typename T>
    int IndexOf(const std::vector<T>& arr, const T& value) {
        auto it = std::find(arr.begin(), arr.end(), value);
        return it != arr.end() ? std::distance(arr.begin(), it) : -1;
    }
    
    template<typename T>
    void Reverse(std::vector<T>& arr) {
   std::reverse(arr.begin(), arr.end());
    }
    
    template<typename T>
    void Sort(std::vector<T>& arr) {
        std::sort(arr.begin(), arr.end());
    }
}

// ============================================================================
// SYSTEM FUNCTIONS
// ============================================================================

namespace System {
    // Process
    int Exit(int code);
    std::string GetEnv(const std::string& name);
    void SetEnv(const std::string& name, const std::string& value);
  std::vector<std::string> GetArgs();
    
    // System info
    std::string GetOS();
    std::string GetArch();
    int GetCPUCount();
    int64_t GetTotalMemory();
    int64_t GetFreeMemory();
    
    // Execution
    int Execute(const std::string& command);
    std::string ExecuteAndCapture(const std::string& command);
}

// ============================================================================
// CONCURRENCY FUNCTIONS
// ============================================================================

namespace Concurrency {
    // Thread ID type
    using ThreadID = void*;
  using ThreadFunc = std::function<void()>;
    
    // Thread operations
    ThreadID CreateThread(ThreadFunc func);
    void JoinThread(ThreadID thread);
    void DetachThread(ThreadID thread);
    ThreadID GetCurrentThread();
    void YieldThread();
    
    // Mutex
    class Mutex {
    public:
        Mutex();
      ~Mutex();
      void Lock();
 void Unlock();
        bool TryLock();
    private:
        void* handle_;
  };
    
    // Atomic operations
 int64_t AtomicIncrement(int64_t* ptr);
    int64_t AtomicDecrement(int64_t* ptr);
    int64_t AtomicAdd(int64_t* ptr, int64_t value);
    int64_t AtomicExchange(int64_t* ptr, int64_t value);
    bool AtomicCompareExchange(int64_t* ptr, int64_t expected, int64_t desired);
}

// ============================================================================
// MEMORY FUNCTIONS
// ============================================================================

namespace Memory {
    void* Allocate(size_t size);
    void Deallocate(void* ptr);
  void* Reallocate(void* ptr, size_t new_size);
    void Copy(void* dest, const void* src, size_t size);
    void Move(void* dest, const void* src, size_t size);
    void Set(void* ptr, int value, size_t size);
    void Zero(void* ptr, size_t size);
    int Compare(const void* a, const void* b, size_t size);
}

// ============================================================================
// NETWORKING (FUTURE)
// ============================================================================

namespace Net {
    // Socket operations
    // HTTP client
    // WebSocket
    // (To be implemented)
}

// ============================================================================
// LIBRARY INITIALIZATION
// ============================================================================

void Initialize();
void Shutdown();

} // namespace StdLib
} // namespace Snow
