## Introduction

Before your C++ code is compiled, it goes through a **preprocessor** - a text manipulation program that processes special directives and prepares your code for compilation.

**Preprocessor directives** are instructions that begin with `#` and tell the preprocessor what to do with your source code **before** compilation.

**Real-World Analogies**:

- **Mail merge** - Replace placeholders with actual content before printing
- **Recipe substitutions** - "Replace butter with margarine" before cooking
- **Template expansion** - Fill in blanks before creating final document
- **Assembly instructions** - "Insert Part A before welding"

---

## Why Preprocessor Directives Matter

### The Compilation Pipeline

```
Source Code (.cpp)
    ↓
Preprocessor (handles #directives)
    ↓
Expanded Source Code
    ↓
Compiler (creates object code)
    ↓
Linker (creates executable)
    ↓
Executable Program
```

**Preprocessor directives execute BEFORE compilation!**

### What Preprocessor Does

1. **Includes files** - `#include`
2. **Defines macros** - `#define`
3. **Conditional compilation** - `#if`, `#ifdef`, `#ifndef`
4. **Prevents multiple inclusion** - Include guards
5. **Generates errors/warnings** - `#error`, `#warning`
6. **Platform-specific code** - Different code for different systems

---

## Table of Contents

1. #include Directive
2. #define Directive (Macros)
3. Conditional Compilation (#if, #ifdef, #ifndef)
4. Include Guards
5. #pragma Directive
6. Predefined Macros
7. #error and #warning
8. Advanced Macro Techniques
9. Best Practices

---

# 1. #include Directive

## What is #include?

**#include** copies the entire contents of a file into your source code.

### Syntax

```cpp
#include <header>      // System headers (angle brackets)
#include "header"      // User headers (quotes)
```

---

## System Headers (Angle Brackets)

```cpp
#include <iostream>    // Standard library
#include <vector>
#include <string>
#include <algorithm>
```

**Searches in:**

- System include directories
- Standard library locations
- Compiler's default paths

---

## User Headers (Quotes)

```cpp
#include "MyClass.h"      // Your headers
#include "utilities.h"
#include "../config.h"    // Relative path
```

**Searches in:**

1. Current directory first
2. Then system directories

---

## How #include Works

```cpp
// Before preprocessing
// main.cpp
#include "helper.h"

int main() {
    helpFunction();
}

// helper.h
void helpFunction() {
    // implementation
}

// After preprocessing (what compiler sees)
// main.cpp
void helpFunction() {
    // implementation
}

int main() {
    helpFunction();
}
```

**The preprocessor literally copies and pastes the file contents!**

---

## Include Paths

### Specifying Custom Include Directories

```bash
# Compile with custom include path
g++ -I/path/to/headers main.cpp -o program

# Multiple include paths
g++ -I./include -I./external/include main.cpp -o program
```

---

# 2. #define Directive (Macros)

## Object-like Macros (Constants)

### Basic Definition

```cpp
#define PI 3.14159
#define MAX_SIZE 100
#define COMPANY_NAME "Acme Corp"

int main() {
    double area = PI * radius * radius;
    int array[MAX_SIZE];
    cout << COMPANY_NAME << endl;
    
    return 0;
}
```

**After preprocessing:**

```cpp
int main() {
    double area = 3.14159 * radius * radius;
    int array[100];
    cout << "Acme Corp" << endl;
    
    return 0;
}
```

---

### Modern Alternative: const

```cpp
// ❌ Old way (preprocessor)
#define PI 3.14159

// ✅ Modern way (typed constant)
const double PI = 3.14159;
constexpr double PI = 3.14159;  // C++11, compile-time constant
```

**Advantages of const/constexpr:**

- Type-safe
- Scoped
- Debugger-friendly
- Better error messages

---

## Function-like Macros

### Basic Function Macros

```cpp
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))

int main() {
    int result = SQUARE(5);        // Expands to ((5) * (5))
    int larger = MAX(10, 20);      // Expands to ((10) > (20) ? (10) : (20))
    int absolute = ABS(-15);       // Expands to ((-15) < 0 ? -(-15) : (-15))
    
    return 0;
}
```

---

### The Parentheses Problem

```cpp
// ❌ Bad macro (missing parentheses)
#define SQUARE(x) x * x

int result = SQUARE(3 + 2);
// Expands to: 3 + 2 * 3 + 2 = 3 + 6 + 2 = 11 (WRONG!)

// ✅ Good macro (with parentheses)
#define SQUARE(x) ((x) * (x))

int result = SQUARE(3 + 2);
// Expands to: ((3 + 2) * (3 + 2)) = 5 * 5 = 25 (CORRECT!)
```

**Always use parentheses around macro parameters and the entire expression!**

---

### Side Effects Problem

```cpp
#define SQUARE(x) ((x) * (x))

int value = 5;
int result = SQUARE(value++);
// Expands to: ((value++) * (value++))
// value is incremented TWICE! Undefined behavior!
```

---

### Modern Alternative: Inline Functions

```cpp
// ❌ Macro (no type checking, side effects)
#define SQUARE(x) ((x) * (x))

// ✅ Inline function (type-safe, no side effects)
inline int square(int x) {
    return x * x;
}

// ✅ C++11: constexpr for compile-time evaluation
constexpr int square(int x) {
    return x * x;
}

// ✅ C++14: auto for generic code
template<typename T>
constexpr auto square(T x) {
    return x * x;
}
```

---

## Stringification (#)

### Converting Macro Arguments to Strings

```cpp
#define STRINGIFY(x) #x
#define PRINT_VAR(var) cout << #var << " = " << var << endl

int main() {
    string text = STRINGIFY(Hello World);
    // Expands to: "Hello World"
    
    int age = 25;
    PRINT_VAR(age);
    // Expands to: cout << "age" << " = " << age << endl
    // Output: age = 25
    
    return 0;
}
```

---

## Token Pasting (##)

### Concatenating Tokens

```cpp
#define CONCAT(a, b) a##b

int main() {
    int CONCAT(var, 123) = 42;
    // Expands to: int var123 = 42;
    
    cout << CONCAT(var, 123) << endl;
    // Expands to: cout << var123 << endl
    // Output: 42
    
    return 0;
}
```

---

## Variadic Macros

### Macros with Variable Arguments

```cpp
// __VA_ARGS__ represents all arguments
#define LOG(format, ...) printf(format, __VA_ARGS__)

int main() {
    LOG("Value: %d\n", 42);
    LOG("Name: %s, Age: %d\n", "Alice", 25);
    
    return 0;
}
```

---

## Undefining Macros

```cpp
#define TEMP_VALUE 100

int x = TEMP_VALUE;  // x = 100

#undef TEMP_VALUE    // Remove definition

// #define TEMP_VALUE 200  // Can redefine now
```

---

# 3. Conditional Compilation

## #if, #elif, #else, #endif

### Basic Conditional Compilation

```cpp
#define DEBUG_MODE 1

#if DEBUG_MODE
    cout << "Debug mode enabled" << endl;
    cout << "Variable x = " << x << endl;
#else
    cout << "Release mode" << endl;
#endif
```

**After preprocessing (if DEBUG_MODE is 1):**

```cpp
    cout << "Debug mode enabled" << endl;
    cout << "Variable x = " << x << endl;
```

---

### Multi-level Conditionals

```cpp
#define OPTIMIZATION_LEVEL 2

#if OPTIMIZATION_LEVEL == 0
    cout << "No optimization" << endl;
#elif OPTIMIZATION_LEVEL == 1
    cout << "Basic optimization" << endl;
#elif OPTIMIZATION_LEVEL == 2
    cout << "Full optimization" << endl;
#else
    cout << "Unknown optimization level" << endl;
#endif
```

---

## #ifdef and #ifndef

### Check if Macro is Defined

```cpp
#define FEATURE_ENABLED

#ifdef FEATURE_ENABLED
    cout << "Feature is enabled" << endl;
#endif

#ifndef FEATURE_DISABLED
    cout << "Feature is NOT disabled" << endl;
#endif
```

---

## Platform-Specific Code

```cpp
#ifdef _WIN32
    #include <windows.h>
    void clearScreen() {
        system("cls");
    }
#elif defined(__linux__)
    #include <unistd.h>
    void clearScreen() {
        system("clear");
    }
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    void clearScreen() {
        system("clear");
    }
#else
    void clearScreen() {
        cout << "Clear screen not supported" << endl;
    }
#endif
```

---

## Debug vs Release Builds

```cpp
#ifdef DEBUG
    #define LOG(msg) cout << "[DEBUG] " << msg << endl
    #define ASSERT(condition) \
        if (!(condition)) { \
            cerr << "Assertion failed: " #condition << endl; \
            exit(1); \
        }
#else
    #define LOG(msg)
    #define ASSERT(condition)
#endif

int main() {
    LOG("Starting program");  // Only in debug builds
    
    int value = 10;
    ASSERT(value > 0);  // Only checked in debug builds
    
    return 0;
}
```

**Compile:**

```bash
# Debug build
g++ -DDEBUG main.cpp -o program_debug

# Release build
g++ main.cpp -o program_release
```

---

# 4. Include Guards

## The Multiple Inclusion Problem

```cpp
// Shape.h
class Shape {
    // ...
};

// Circle.h
#include "Shape.h"
class Circle : public Shape {
    // ...
};

// main.cpp
#include "Shape.h"   // Shape defined
#include "Circle.h"  // Shape defined again! ERROR!
```

---

## Traditional Include Guards

```cpp
// MyClass.h
#ifndef MYCLASS_H
#define MYCLASS_H

class MyClass {
    // Class definition
};

#endif // MYCLASS_H
```

**How it works:**

1. First inclusion: `MYCLASS_H` not defined → include content → define `MYCLASS_H`
2. Second inclusion: `MYCLASS_H` already defined → skip content

---

## #pragma once (Modern)

```cpp
// MyClass.h
#pragma once

class MyClass {
    // Class definition
};
```

**Advantages:**

- ✅ Simpler syntax
- ✅ Less error-prone
- ✅ Slightly faster compilation
- ✅ Supported by all modern compilers

**Disadvantages:**

- ❌ Not part of C++ standard (but universally supported)
- ❌ Can have issues with symlinks (rare)

**Recommendation:** Use `#pragma once` for modern projects.

---

# 5. #pragma Directive

## What is #pragma?

**#pragma** provides compiler-specific instructions.

---

## Common #pragma Directives

### #pragma once (Include Guard)

```cpp
#pragma once

class MyClass {
    // ...
};
```

---

### #pragma message (Compile-time Message)

```cpp
#pragma message("Compiling version 2.0")

#ifdef DEBUG
    #pragma message("Building in DEBUG mode")
#else
    #pragma message("Building in RELEASE mode")
#endif
```

---

### #pragma warning (Control Warnings - MSVC)

```cpp
// Disable specific warning
#pragma warning(disable: 4996)  // Disable deprecated function warning

// Push/pop warning state
#pragma warning(push)
#pragma warning(disable: 4244)  // Disable conversion warning
    // Code that generates warnings
#pragma warning(pop)
```

---

### #pragma GCC diagnostic (GCC/Clang)

```cpp
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    int unusedVar;  // Won't generate warning
#pragma GCC diagnostic pop
```

---

### #pragma pack (Structure Packing)

```cpp
// Default packing
struct Default {
    char c;    // 1 byte + 3 bytes padding
    int i;     // 4 bytes
    char d;    // 1 byte + 3 bytes padding
};  // Total: 12 bytes

// Packed structure
#pragma pack(push, 1)
struct Packed {
    char c;    // 1 byte
    int i;     // 4 bytes
    char d;    // 1 byte
};  // Total: 6 bytes
#pragma pack(pop)
```

---

# 6. Predefined Macros

## Standard Predefined Macros

```cpp
#include <iostream>
using namespace std;

int main() {
    cout << "File: " << __FILE__ << endl;
    cout << "Line: " << __LINE__ << endl;
    cout << "Date: " << __DATE__ << endl;
    cout << "Time: " << __TIME__ << endl;
    cout << "Function: " << __func__ << endl;  // C++11
    
    return 0;
}
```

**Output:**

```
File: main.cpp
Line: 6
Date: Feb 14 2026
Time: 10:30:45
Function: main
```

---

## Compiler-Specific Macros

```cpp
#ifdef _MSC_VER
    cout << "Microsoft Visual C++ compiler" << endl;
    cout << "Version: " << _MSC_VER << endl;
#endif

#ifdef __GNUC__
    cout << "GCC compiler" << endl;
    cout << "Version: " << __GNUC__ << "." << __GNUC_MINOR__ << endl;
#endif

#ifdef __clang__
    cout << "Clang compiler" << endl;
#endif
```

---

## C++ Standard Version Macros

```cpp
#if __cplusplus >= 202002L
    cout << "C++20 or later" << endl;
#elif __cplusplus >= 201703L
    cout << "C++17" << endl;
#elif __cplusplus >= 201402L
    cout << "C++14" << endl;
#elif __cplusplus >= 201103L
    cout << "C++11" << endl;
#else
    cout << "Pre-C++11" << endl;
#endif
```

---

## Platform Detection Macros

```cpp
#ifdef _WIN32
    cout << "Windows (32 or 64 bit)" << endl;
#endif

#ifdef _WIN64
    cout << "Windows 64-bit" << endl;
#endif

#ifdef __linux__
    cout << "Linux" << endl;
#endif

#ifdef __APPLE__
    cout << "macOS or iOS" << endl;
#endif

#ifdef __unix__
    cout << "Unix-like system" << endl;
#endif
```

---

# 7. #error and #warning

## #error - Stop Compilation

```cpp
#ifndef CONFIG_FILE_INCLUDED
    #error "config.h must be included before this file"
#endif

#if __cplusplus < 201103L
    #error "This code requires C++11 or later"
#endif

#ifndef REQUIRED_FEATURE
    #error "REQUIRED_FEATURE must be defined"
#endif
```

**When compiler encounters #error:**

- Stops compilation
- Displays error message
- Prevents building incorrect configuration

---

## #warning - Compilation Warning

```cpp
#ifdef OLD_API
    #warning "OLD_API is deprecated and will be removed in version 3.0"
#endif

#ifndef OPTIMIZATION_ENABLED
    #warning "Compiling without optimization - performance will be reduced"
#endif
```

**Note:** `#warning` is not standard C++ but widely supported.

---

# 8. Advanced Macro Techniques

## Multi-line Macros

```cpp
#define SWAP(a, b) \
    do { \
        auto temp = a; \
        a = b; \
        b = temp; \
    } while(0)

int main() {
    int x = 5, y = 10;
    SWAP(x, y);
    cout << "x = " << x << ", y = " << y << endl;
    // Output: x = 10, y = 5
    
    return 0;
}
```

**The `do { } while(0)` trick ensures the macro behaves like a single statement.**

---

## Debugging Macros

```cpp
#ifdef DEBUG
    #define DEBUG_PRINT(x) cout << "[DEBUG] " << #x << " = " << x << endl
    #define DEBUG_MSG(msg) cout << "[DEBUG] " << msg << endl
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_MSG(msg)
#endif

int main() {
    int value = 42;
    DEBUG_PRINT(value);  // Only in debug builds
    DEBUG_MSG("Starting calculation");
    
    return 0;
}
```

---

## Assert Macro

```cpp
#ifdef DEBUG
    #define ASSERT(condition, message) \
        if (!(condition)) { \
            cerr << "Assertion failed: " << #condition << endl; \
            cerr << "Message: " << message << endl; \
            cerr << "File: " << __FILE__ << ", Line: " << __LINE__ << endl; \
            abort(); \
        }
#else
    #define ASSERT(condition, message)
#endif

int main() {
    int age = -5;
    ASSERT(age >= 0, "Age cannot be negative");
    
    return 0;
}
```

---

## Macro for Class Boilerplate

```cpp
#define DISABLE_COPY(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

class NonCopyable {
    DISABLE_COPY(NonCopyable)
    
public:
    NonCopyable() = default;
};

int main() {
    NonCopyable obj1;
    // NonCopyable obj2 = obj1;  // Error! Copy disabled
    
    return 0;
}
```

---

# 9. Best Practices

## ✅ DO:

### 1. Use #pragma once for Include Guards

```cpp
// ✅ Modern and simple
#pragma once

class MyClass {
    // ...
};
```

---

### 2. Use const/constexpr Instead of #define for Constants

```cpp
// ❌ Avoid
#define MAX_SIZE 100

// ✅ Prefer
constexpr int MAX_SIZE = 100;
```

---

### 3. Use Inline Functions Instead of Function Macros

```cpp
// ❌ Avoid
#define SQUARE(x) ((x) * (x))

// ✅ Prefer
inline int square(int x) {
    return x * x;
}

// ✅ Or template
template<typename T>
constexpr T square(T x) {
    return x * x;
}
```

---

### 4. Use Conditional Compilation for Platform Code

```cpp
#ifdef _WIN32
    // Windows-specific code
#elif defined(__linux__)
    // Linux-specific code
#elif defined(__APPLE__)
    // macOS-specific code
#endif
```

---

### 5. Use UPPERCASE for Macro Names

```cpp
#define MAX_BUFFER_SIZE 1024
#define LOG_ERROR(msg) cerr << "ERROR: " << msg << endl
```

---

## ❌ DON'T:

### 1. Don't Use Macros When Better Alternatives Exist

```cpp
// ❌ Bad
#define PI 3.14159

// ✅ Good
constexpr double PI = 3.14159;
```

---

### 2. Don't Create Complex Multi-statement Macros

```cpp
// ❌ Bad - hard to debug, no type safety
#define COMPLEX_MACRO(x, y) \
    x += y; \
    y *= 2; \
    if (x > y) x = y;

// ✅ Good - use a function
inline void complexOperation(int& x, int& y) {
    x += y;
    y *= 2;
    if (x > y) x = y;
}
```

---

### 3. Don't Forget Parentheses in Macros

```cpp
// ❌ Bad
#define SQUARE(x) x * x

// ✅ Good
#define SQUARE(x) ((x) * (x))
```

---

### 4. Don't Use Macros with Side Effects

```cpp
// ❌ Dangerous
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int x = 5;
int result = MAX(x++, 10);  // x++ evaluated twice!

// ✅ Safe
template<typename T>
const T& max(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

---

## Complete Example: Configuration System

```cpp
// config.h
#pragma once

// Version information
#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_PATCH 0

// Feature flags
#define FEATURE_LOGGING 1
#define FEATURE_NETWORKING 1
#define FEATURE_DATABASE 0

// Platform detection
#ifdef _WIN32
    #define PLATFORM "Windows"
    #define PATH_SEPARATOR '\\'
#elif defined(__linux__)
    #define PLATFORM "Linux"
    #define PATH_SEPARATOR '/'
#elif defined(__APPLE__)
    #define PLATFORM "macOS"
    #define PATH_SEPARATOR '/'
#else
    #define PLATFORM "Unknown"
    #define PATH_SEPARATOR '/'
#endif

// Debug macros
#ifdef DEBUG
    #define LOG(msg) cout << "[LOG] " << msg << endl
    #define ASSERT(cond) \
        if (!(cond)) { \
            cerr << "Assertion failed: " #cond << endl; \
            cerr << __FILE__ << ":" << __LINE__ << endl; \
            abort(); \
        }
#else
    #define LOG(msg)
    #define ASSERT(cond)
#endif

// Utility macros
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define VERSION_STRING \
    TO_STRING(VERSION_MAJOR) "." \
    TO_STRING(VERSION_MINOR) "." \
    TO_STRING(VERSION_PATCH)
```

```cpp
// main.cpp
#include <iostream>
#include "config.h"

using namespace std;

int main() {
    cout << "Application Version: " << VERSION_STRING << endl;
    cout << "Platform: " << PLATFORM << endl;
    cout << "Path Separator: " << PATH_SEPARATOR << endl;
    
    LOG("Application started");
    
    #if FEATURE_LOGGING
        cout << "Logging feature enabled" << endl;
    #endif
    
    #if FEATURE_NETWORKING
        cout << "Networking feature enabled" << endl;
    #endif
    
    #if FEATURE_DATABASE
        cout << "Database feature enabled" << endl;
    #else
        cout << "Database feature disabled" << endl;
    #endif
    
    int value = 10;
    ASSERT(value > 0);
    
    return 0;
}
```

---

## Summary

### Key Concepts

1. **#include** - File inclusion
2. **#define** - Macro definition
3. **#if/#ifdef/#ifndef** - Conditional compilation
4. **#pragma** - Compiler directives
5. **Predefined macros** - Compiler/platform info
6. **#error/#warning** - Compile-time messages

### When to Use Preprocessor

✅ **Use for:**

- Include guards (#pragma once)
- Platform-specific code
- Debug vs release builds
- Feature toggles
- Header organization

❌ **Avoid for:**

- Constants (use const/constexpr)
- Functions (use inline/template)
- Type definitions (use typedef/using)
- Complex logic

### Modern C++ Alternatives

|Old (Preprocessor)|Modern (C++)|
|---|---|
|`#define MAX 100`|`constexpr int MAX = 100;`|
|`#define SQUARE(x)`|`template<typename T> T square(T x)`|
|`#define DEBUG_LOG`|`if constexpr` with logging|
|Macro constants|`enum class` or `constexpr`|

---

## Conclusion

**Preprocessor directives** are powerful but should be used judiciously:

- **Essential uses** - Include guards, platform code, feature flags
- **Legacy uses** - Constants and function macros (use modern alternatives)
- **Power** - Text manipulation before compilation
- **Danger** - No type safety, hard to debug

Master preprocessor directives to:

- ✅ Organize large projects with include guards
- ✅ Write platform-independent code
- ✅ Create flexible build configurations
- ✅ Understand legacy codebases
- ✅ Use conditional compilation effectively

**But remember:** Modern C++ provides better alternatives for most preprocessor uses!

**Happy Coding!** 🚀

---

## Remember

_"With great power comes great responsibility"_

**Preprocessor is powerful but:**

- Hard to debug (no type checking)
- Can create obscure errors
- Modern C++ has better alternatives

**Use it wisely:**

- ✅ Include guards
- ✅ Platform detection
- ✅ Conditional compilation
- ❌ Not for constants (use constexpr)
- ❌ Not for functions (use inline/template)

**You now understand what happens before compilation!** 🎓