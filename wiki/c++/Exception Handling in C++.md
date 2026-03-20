## Introduction

Imagine your program is running smoothly when suddenly: file not found, division by zero, out of memory, or invalid user input. Without proper error handling, your program crashes. With **exception handling**, you can gracefully manage errors and keep your program running.

**Exception Handling** is a mechanism to handle runtime errors in a controlled, organized way.

**Real-World Analogies**:

- **Circuit Breaker**: When electrical overload occurs, circuit breaker trips instead of causing fire
- **Airbag in Car**: When crash detected, airbag deploys to minimize damage
- **Emergency Exit**: When fire alarm sounds, follow emergency exit plan
- **Insurance**: When unexpected event occurs, insurance handles the financial impact

---

## Why Exception Handling Matters

### Without Exception Handling

```cpp
int divide(int a, int b) {
    return a / b;  // What if b is 0? Crash!
}

int main() {
    int result = divide(10, 0);  // Program crashes
    cout << result << endl;       // Never reached
    return 0;
}
```

**Problems**:

- Program crashes unexpectedly
- No opportunity to clean up resources
- No user-friendly error messages
- Difficult to debug

### With Exception Handling

```cpp
int divide(int a, int b) {
    if (b == 0) {
        throw runtime_error("Division by zero!");
    }
    return a / b;
}

int main() {
    try {
        int result = divide(10, 0);
        cout << result << endl;
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        // Program continues gracefully
    }
    
    cout << "Program continues..." << endl;
    return 0;
}
```

**Benefits**:

- Program doesn't crash
- Resources cleaned up properly
- User-friendly error messages
- Controlled error handling

---

## Exception Handling Basics

### Three Keywords

1. **try** - Code that might throw exceptions
2. **throw** - Throw an exception when error occurs
3. **catch** - Handle the exception

### Basic Syntax

```cpp
try {
    // Code that might throw an exception
    // Risky operations here
}
catch (ExceptionType e) {
    // Handle the exception
    // Error handling code here
}
```

### Simple Example

```cpp
#include <iostream>
#include <stdexcept>
using namespace std;

int main() {
    try {
        int age;
        cout << "Enter your age: ";
        cin >> age;
        
        if (age < 0) {
            throw invalid_argument("Age cannot be negative!");
        }
        
        if (age < 18) {
            throw runtime_error("You must be 18 or older!");
        }
        
        cout << "Access granted!" << endl;
    }
    catch (const invalid_argument& e) {
        cout << "Invalid input: " << e.what() << endl;
    }
    catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    return 0;
}
```

---

## Throwing Exceptions

### Throwing Built-in Types

```cpp
// Throw integers
throw 404;

// Throw strings
throw "Error occurred!";

// Throw doubles
throw 3.14;

// Throw characters
throw 'E';
```

### Throwing Standard Exceptions

C++ provides standard exception classes in `<stdexcept>`:

```cpp
#include <stdexcept>

// Logic errors
throw logic_error("Logic error occurred");
throw invalid_argument("Invalid argument");
throw domain_error("Domain error");
throw length_error("Length error");
throw out_of_range("Out of range");

// Runtime errors
throw runtime_error("Runtime error occurred");
throw range_error("Range error");
throw overflow_error("Overflow error");
throw underflow_error("Underflow error");
```

### Example with Multiple Exception Types

```cpp
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

void processArray(vector<int>& arr, int index, int value) {
    if (index < 0) {
        throw invalid_argument("Index cannot be negative");
    }
    
    if (index >= arr.size()) {
        throw out_of_range("Index out of bounds");
    }
    
    if (value < 0) {
        throw domain_error("Value cannot be negative");
    }
    
    arr[index] = value;
    cout << "Value set successfully!" << endl;
}

int main() {
    vector<int> numbers = {1, 2, 3, 4, 5};
    
    try {
        processArray(numbers, 2, 100);   // OK
        processArray(numbers, -1, 50);   // Throws invalid_argument
    }
    catch (const invalid_argument& e) {
        cout << "Invalid argument: " << e.what() << endl;
    }
    catch (const out_of_range& e) {
        cout << "Out of range: " << e.what() << endl;
    }
    catch (const domain_error& e) {
        cout << "Domain error: " << e.what() << endl;
    }
    
    return 0;
}
```

---

## Catching Exceptions

### Single Catch Block

```cpp
try {
    // Code that might throw
    throw runtime_error("Something went wrong");
}
catch (const runtime_error& e) {
    cout << "Error: " << e.what() << endl;
}
```

### Multiple Catch Blocks

**Order matters!** Catch blocks are checked from top to bottom.

```cpp
try {
    // Code that might throw different exceptions
}
catch (const out_of_range& e) {
    cout << "Out of range: " << e.what() << endl;
}
catch (const invalid_argument& e) {
    cout << "Invalid argument: " << e.what() << endl;
}
catch (const runtime_error& e) {
    cout << "Runtime error: " << e.what() << endl;
}
catch (const exception& e) {
    cout << "General exception: " << e.what() << endl;
}
```

### Catch All Exceptions

```cpp
try {
    // Code that might throw
}
catch (...) {  // Catches ANY exception
    cout << "An unknown exception occurred" << endl;
}
```

### Catch by Reference (Best Practice)

```cpp
// Good - catch by const reference
catch (const exception& e) {
    cout << e.what() << endl;
}

// Avoid - catch by value (makes copy)
catch (exception e) {
    cout << e.what() << endl;
}

// Avoid - catch by pointer
catch (exception* e) {
    cout << e->what() << endl;
}
```

---

## Standard Exception Hierarchy

```
exception (base class)
│
├── logic_error
│   ├── invalid_argument
│   ├── domain_error
│   ├── length_error
│   ├── out_of_range
│   └── future_error (C++11)
│
├── runtime_error
│   ├── range_error
│   ├── overflow_error
│   └── underflow_error
│
├── bad_alloc (memory allocation failure)
├── bad_cast (dynamic_cast failure)
└── bad_exception
```

### Using the Hierarchy

```cpp
try {
    throw out_of_range("Index out of range");
}
catch (const out_of_range& e) {
    // Catches out_of_range specifically
}
catch (const logic_error& e) {
    // Catches any logic_error (including out_of_range)
}
catch (const exception& e) {
    // Catches any standard exception
}
```

---

## Custom Exceptions

### Creating Custom Exception Class

```cpp
#include <exception>
#include <string>

class MyException : public exception {
private:
    string message;

public:
    MyException(const string& msg) : message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
};

int main() {
    try {
        throw MyException("Custom error occurred!");
    }
    catch (const MyException& e) {
        cout << "Caught: " << e.what() << endl;
    }
    
    return 0;
}
```

### More Complex Custom Exception

```cpp
class FileException : public runtime_error {
private:
    string filename;
    int errorCode;

public:
    FileException(const string& msg, const string& file, int code)
        : runtime_error(msg), filename(file), errorCode(code) {}
    
    string getFilename() const { return filename; }
    int getErrorCode() const { return errorCode; }
    
    string getFullMessage() const {
        return string(what()) + " (File: " + filename + 
               ", Code: " + to_string(errorCode) + ")";
    }
};

// Usage
try {
    throw FileException("Cannot open file", "data.txt", 404);
}
catch (const FileException& e) {
    cout << e.getFullMessage() << endl;
}
```

---

## Exception Handling with Functions

### Function Throwing Exception

```cpp
double calculateSquareRoot(double value) {
    if (value < 0) {
        throw invalid_argument("Cannot calculate square root of negative number");
    }
    return sqrt(value);
}

int main() {
    try {
        double result = calculateSquareRoot(-4);
        cout << result << endl;
    }
    catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    return 0;
}
```

### Exception Specification (Deprecated in C++11)

```cpp
// Old way (deprecated)
void func() throw(int, runtime_error) {
    // Can only throw int or runtime_error
}

// Modern way - noexcept
void safeFunc() noexcept {
    // Promises not to throw exceptions
}

void mayThrow() noexcept(false) {
    // May throw exceptions
}
```

### noexcept Keyword (C++11)

```cpp
// Function that doesn't throw
void noThrowFunc() noexcept {
    // Implementation
}

// Conditional noexcept
template<typename T>
void func(T value) noexcept(noexcept(T(value))) {
    T copy(value);
}
```

---

## Re-throwing Exceptions

### Re-throw Current Exception

```cpp
try {
    throw runtime_error("Original error");
}
catch (const exception& e) {
    cout << "Caught: " << e.what() << endl;
    throw;  // Re-throw the same exception
}
```

### Wrap and Re-throw

```cpp
try {
    throw runtime_error("Low-level error");
}
catch (const runtime_error& e) {
    // Wrap in higher-level exception
    throw logic_error(string("High-level error: ") + e.what());
}
```

### Example: Nested Try-Catch

```cpp
#include <iostream>
#include <stdexcept>
using namespace std;

void innerFunction() {
    throw runtime_error("Error in inner function");
}

void middleFunction() {
    try {
        innerFunction();
    }
    catch (const runtime_error& e) {
        cout << "Middle caught: " << e.what() << endl;
        throw;  // Re-throw to outer
    }
}

void outerFunction() {
    try {
        middleFunction();
    }
    catch (const runtime_error& e) {
        cout << "Outer caught: " << e.what() << endl;
    }
}

int main() {
    outerFunction();
    return 0;
}
```

**Output**:

```
Middle caught: Error in inner function
Outer caught: Error in inner function
```

---

## RAII and Exception Safety

### What is RAII?

**RAII** (Resource Acquisition Is Initialization) - Resources are tied to object lifetime.

**Principle**:

- Acquire resource in constructor
- Release resource in destructor
- Destructor is called even if exception occurs

### Example: File Handling with RAII

```cpp
#include <fstream>

// Bad - Resource leak if exception occurs
void badFileHandling() {
    ofstream* file = new ofstream("data.txt");
    
    // If exception thrown here, file never closed/deleted!
    throw runtime_error("Error!");
    
    file->close();
    delete file;  // Never reached
}

// Good - RAII ensures cleanup
void goodFileHandling() {
    ofstream file("data.txt");  // RAII object
    
    // If exception thrown, destructor still called
    throw runtime_error("Error!");
    
}  // Destructor automatically closes file
```

### Custom RAII Class

```cpp
class FileHandler {
private:
    ofstream file;

public:
    FileHandler(const string& filename) {
        file.open(filename);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file: " + filename);
        }
        cout << "File opened" << endl;
    }
    
    ~FileHandler() {
        if (file.is_open()) {
            file.close();
            cout << "File closed" << endl;
        }
    }
    
    void write(const string& data) {
        file << data;
    }
};

int main() {
    try {
        FileHandler fh("data.txt");
        fh.write("Hello");
        throw runtime_error("Error occurred!");
    }
    catch (const exception& e) {
        cout << "Caught: " << e.what() << endl;
    }
    // Destructor called here - file closed automatically
    
    return 0;
}
```

**Output**:

```
File opened
Caught: Error occurred!
File closed
```

---

## Exception Safety Guarantees

### Three Levels of Exception Safety

1. **Basic Guarantee**: No resource leaks, object in valid state
2. **Strong Guarantee**: Operation succeeds or has no effect (rollback)
3. **No-throw Guarantee**: Never throws exceptions (noexcept)

### Basic Guarantee Example

```cpp
class Container {
private:
    int* data;
    int size;

public:
    void resize(int newSize) {
        int* newData = new int[newSize];  // May throw bad_alloc
        
        // Copy old data
        for (int i = 0; i < min(size, newSize); i++) {
            newData[i] = data[i];
        }
        
        delete[] data;
        data = newData;
        size = newSize;
    }
};
```

### Strong Guarantee Example

```cpp
class Container {
private:
    int* data;
    int size;

public:
    void resize(int newSize) {
        int* newData = new int[newSize];  // May throw
        
        try {
            // Copy old data
            for (int i = 0; i < min(size, newSize); i++) {
                newData[i] = data[i];
            }
            
            // Only commit if successful
            delete[] data;
            data = newData;
            size = newSize;
        }
        catch (...) {
            delete[] newData;  // Clean up on failure
            throw;             // Re-throw
        }
    }
};
```

---

## Practical Examples and Projects

### Project 1: Safe Calculator

```cpp
#include <iostream>
#include <stdexcept>
#include <limits>
using namespace std;

class Calculator {
public:
    double add(double a, double b) {
        double result = a + b;
        if (!isfinite(result)) {
            throw overflow_error("Addition overflow");
        }
        return result;
    }
    
    double subtract(double a, double b) {
        return add(a, -b);
    }
    
    double multiply(double a, double b) {
        if (a == 0 || b == 0) return 0;
        
        double result = a * b;
        if (!isfinite(result)) {
            throw overflow_error("Multiplication overflow");
        }
        return result;
    }
    
    double divide(double a, double b) {
        if (b == 0) {
            throw invalid_argument("Division by zero");
        }
        
        double result = a / b;
        if (!isfinite(result)) {
            throw overflow_error("Division overflow");
        }
        return result;
    }
    
    double squareRoot(double value) {
        if (value < 0) {
            throw domain_error("Cannot calculate square root of negative number");
        }
        return sqrt(value);
    }
    
    double power(double base, double exponent) {
        double result = pow(base, exponent);
        if (!isfinite(result)) {
            throw overflow_error("Power operation overflow");
        }
        return result;
    }
};

int main() {
    Calculator calc;
    
    try {
        cout << "10 / 2 = " << calc.divide(10, 2) << endl;
        cout << "10 / 0 = " << calc.divide(10, 0) << endl;  // Throws
    }
    catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    try {
        cout << "sqrt(16) = " << calc.squareRoot(16) << endl;
        cout << "sqrt(-4) = " << calc.squareRoot(-4) << endl;  // Throws
    }
    catch (const domain_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    try {
        cout << "2^10 = " << calc.power(2, 10) << endl;
        cout << "999^999 = " << calc.power(999, 999) << endl;  // Throws
    }
    catch (const overflow_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    return 0;
}
```

### Project 2: Robust File Manager

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
using namespace std;

class FileManager {
public:
    static string readFile(const string& filename) {
        ifstream file(filename);
        
        if (!file.is_open()) {
            throw runtime_error("Cannot open file: " + filename);
        }
        
        string content;
        string line;
        
        while (getline(file, line)) {
            content += line + "\n";
        }
        
        if (file.bad()) {
            throw runtime_error("Error reading file: " + filename);
        }
        
        return content;
    }
    
    static void writeFile(const string& filename, const string& content) {
        ofstream file(filename);
        
        if (!file.is_open()) {
            throw runtime_error("Cannot create file: " + filename);
        }
        
        file << content;
        
        if (file.fail()) {
            throw runtime_error("Error writing to file: " + filename);
        }
        
        cout << "File written successfully: " << filename << endl;
    }
    
    static void appendFile(const string& filename, const string& content) {
        ofstream file(filename, ios::app);
        
        if (!file.is_open()) {
            throw runtime_error("Cannot open file for appending: " + filename);
        }
        
        file << content;
        
        if (file.fail()) {
            throw runtime_error("Error appending to file: " + filename);
        }
        
        cout << "Content appended successfully: " << filename << endl;
    }
    
    static void copyFile(const string& source, const string& destination) {
        try {
            string content = readFile(source);
            writeFile(destination, content);
            cout << "File copied: " << source << " -> " << destination << endl;
        }
        catch (const runtime_error& e) {
            throw runtime_error("Copy failed: " + string(e.what()));
        }
    }
};

int main() {
    try {
        // Write file
        FileManager::writeFile("test.txt", "Hello, World!\n");
        
        // Read file
        string content = FileManager::readFile("test.txt");
        cout << "File content:\n" << content << endl;
        
        // Append to file
        FileManager::appendFile("test.txt", "New line added!\n");
        
        // Copy file
        FileManager::copyFile("test.txt", "test_copy.txt");
        
        // Try to read non-existent file
        FileManager::readFile("nonexistent.txt");
        
    }
    catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    return 0;
}
```

### Project 3: Bank Account with Exception Safety

```cpp
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

class InsufficientFundsException : public runtime_error {
private:
    double balance;
    double amount;

public:
    InsufficientFundsException(double bal, double amt)
        : runtime_error("Insufficient funds"),
          balance(bal), amount(amt) {}
    
    double getBalance() const { return balance; }
    double getAmount() const { return amount; }
    
    string getDetails() const {
        return "Attempted to withdraw $" + to_string(amount) +
               " but balance is only $" + to_string(balance);
    }
};

class BankAccount {
private:
    string accountNumber;
    string holderName;
    double balance;

public:
    BankAccount(string accNum, string name, double initialBalance)
        : accountNumber(accNum), holderName(name), balance(initialBalance) {
        
        if (initialBalance < 0) {
            throw invalid_argument("Initial balance cannot be negative");
        }
    }
    
    void deposit(double amount) {
        if (amount <= 0) {
            throw invalid_argument("Deposit amount must be positive");
        }
        
        balance += amount;
        cout << "Deposited: $" << amount << endl;
        cout << "New balance: $" << balance << endl;
    }
    
    void withdraw(double amount) {
        if (amount <= 0) {
            throw invalid_argument("Withdrawal amount must be positive");
        }
        
        if (amount > balance) {
            throw InsufficientFundsException(balance, amount);
        }
        
        balance -= amount;
        cout << "Withdrawn: $" << amount << endl;
        cout << "New balance: $" << balance << endl;
    }
    
    void transfer(BankAccount& destination, double amount) {
        if (amount <= 0) {
            throw invalid_argument("Transfer amount must be positive");
        }
        
        if (amount > balance) {
            throw InsufficientFundsException(balance, amount);
        }
        
        // Strong exception guarantee
        double tempBalance = balance;
        
        try {
            balance -= amount;
            destination.deposit(amount);
            cout << "Transfer successful: $" << amount << endl;
        }
        catch (...) {
            // Rollback on failure
            balance = tempBalance;
            throw;
        }
    }
    
    double getBalance() const {
        return balance;
    }
    
    void displayInfo() const {
        cout << "\n=== Account Information ===" << endl;
        cout << "Account: " << accountNumber << endl;
        cout << "Holder: " << holderName << endl;
        cout << "Balance: $" << balance << endl;
    }
};

int main() {
    try {
        BankAccount account1("ACC001", "John Doe", 1000.0);
        BankAccount account2("ACC002", "Jane Smith", 500.0);
        
        account1.displayInfo();
        
        // Valid operations
        account1.deposit(500);
        account1.withdraw(200);
        
        // Transfer
        account1.transfer(account2, 300);
        
        account1.displayInfo();
        account2.displayInfo();
        
        // This will throw InsufficientFundsException
        account1.withdraw(2000);
        
    }
    catch (const InsufficientFundsException& e) {
        cout << "\nInsufficient Funds Error:" << endl;
        cout << e.getDetails() << endl;
    }
    catch (const invalid_argument& e) {
        cout << "\nInvalid Argument: " << e.what() << endl;
    }
    catch (const exception& e) {
        cout << "\nUnexpected Error: " << e.what() << endl;
    }
    
    return 0;
}
```

---

## Best Practices

### ✅ DO:

1. **Catch by const reference**
    
    ```cpp
    catch (const exception& e) {
        // Best practice
    }
    ```
    
2. **Use standard exceptions when appropriate**
    
    ```cpp
    throw invalid_argument("Invalid input");
    throw out_of_range("Index out of bounds");
    ```
    
3. **Provide meaningful error messages**
    
    ```cpp
    throw runtime_error("Cannot open file: " + filename);
    ```
    
4. **Use RAII for resource management**
    
    ```cpp
    {
        ifstream file("data.txt");  // RAII
        // File automatically closed
    }
    ```
    
5. **Catch specific exceptions first**
    
    ```cpp
    catch (const out_of_range& e) { }
    catch (const exception& e) { }  // More general last
    ```
    
6. **Use noexcept for functions that don't throw**
    
    ```cpp
    void safeFunction() noexcept {
        // Guaranteed not to throw
    }
    ```
    

### ❌ DON'T:

1. **Don't catch by value**
    
    ```cpp
    // catch (exception e) { }  // Makes copy - inefficient
    ```
    
2. **Don't throw in destructors**
    
    ```cpp
    ~MyClass() {
        // throw runtime_error("Error");  // Never do this!
    }
    ```
    
3. **Don't swallow exceptions silently**
    
    ```cpp
    try {
        // code
    }
    catch (...) {
        // Empty - error hidden!
    }
    ```
    
4. **Don't catch everything without re-throwing**
    
    ```cpp
    catch (...) {
        cout << "Error" << endl;
        // Should re-throw or handle properly
    }
    ```
    
5. **Don't use exceptions for control flow**
    
    ```cpp
    // Bad - use if statement instead
    try {
        throw true;
    } catch (bool value) { }
    ```
    

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Not Catching by Reference

❌ **Wrong**:

```cpp
catch (exception e) {  // Slicing may occur!
    cout << e.what() << endl;
}
```

✅ **Correct**:

```cpp
catch (const exception& e) {  // No slicing
    cout << e.what() << endl;
}
```

### Mistake 2: Wrong Catch Order

❌ **Wrong**:

```cpp
catch (const exception& e) {
    // Catches everything
}
catch (const runtime_error& e) {
    // Never reached!
}
```

✅ **Correct**:

```cpp
catch (const runtime_error& e) {
    // Specific first
}
catch (const exception& e) {
    // General last
}
```

### Mistake 3: Memory Leak with Exceptions

❌ **Wrong**:

```cpp
void function() {
    int* data = new int[100];
    
    processData(data);  // Might throw!
    
    delete[] data;  // Never reached if exception thrown
}
```

✅ **Correct**:

```cpp
void function() {
    unique_ptr<int[]> data(new int[100]);  // RAII
    
    processData(data.get());  // Automatically cleaned up
}
```

### Mistake 4: Throwing in Destructor

❌ **Wrong**:

```cpp
~MyClass() {
    throw runtime_error("Error");  // Terminate called!
}
```

✅ **Correct**:

```cpp
~MyClass() noexcept {
    try {
        cleanup();
    }
    catch (...) {
        // Log error but don't throw
    }
}
```

---

## Exception Handling Flow

```
┌─────────────────────────────────────┐
│         Normal Execution            │
│    (Code runs sequentially)         │
└──────────────┬──────────────────────┘
               │
               ▼
         ┌──────────┐
         │Exception │
         │ Thrown?  │
         └────┬─────┘
              │
     ┌────────┴────────┐
    No                Yes
     │                 │
     ▼                 ▼
┌─────────┐      ┌──────────────┐
│Continue │      │Stack Unwinding│
│Normally │      │ (destructors  │
└─────────┘      │   called)     │
                 └────┬──────────┘
                      │
                      ▼
              ┌────────────────┐
              │ Catch Block    │
              │   Found?       │
              └────┬───────────┘
                   │
          ┌────────┴────────┐
         Yes               No
          │                 │
          ▼                 ▼
   ┌─────────────┐    ┌──────────┐
   │Handle       │    │ Terminate│
   │Exception    │    │ Program  │
   └──────┬──────┘    └──────────┘
          │
          ▼
   ┌─────────────┐
   │ Continue    │
   │ Execution   │
   └─────────────┘
```

---

## Quick Reference

### Throwing Exceptions

```cpp
throw exception_type("message");
throw;  // Re-throw current exception
```

### Catching Exceptions

```cpp
try {
    // Code
}
catch (const specific_exception& e) {
    // Handle specific
}
catch (const exception& e) {
    // Handle general
}
catch (...) {
    // Catch all
}
```

### Standard Exceptions

```cpp
invalid_argument    // Invalid argument
out_of_range        // Out of range
runtime_error       // Runtime error
overflow_error      // Overflow
logic_error         // Logic error
domain_error        // Domain error
bad_alloc           // Memory allocation failed
```

### Custom Exception

```cpp
class MyException : public exception {
    const char* what() const noexcept override {
        return "My error message";
    }
};
```

---

## Summary

### Key Concepts

1. **try-catch-throw** - Three pillars of exception handling
2. **Exception hierarchy** - Standard exception classes
3. **RAII** - Resource cleanup guaranteed
4. **Exception safety** - Basic, strong, no-throw guarantees
5. **Custom exceptions** - Extend standard exceptions
6. **Best practices** - Catch by reference, meaningful messages

### What You Can Do Now

✅ **Handle errors** gracefully without crashes  
✅ **Clean up resources** automatically with RAII  
✅ **Provide** user-friendly error messages  
✅ **Create** custom exception classes  
✅ **Write** exception-safe code  
✅ **Debug** more effectively  
✅ **Build** robust, professional applications

### Important Rules

⚠️ Catch by const reference  
⚠️ Never throw in destructors  
⚠️ Catch specific before general  
⚠️ Use RAII for resources  
⚠️ Provide meaningful messages  
⚠️ Use standard exceptions when possible

---

## Conclusion

**Exception Handling** transforms how you deal with errors:

- **Before**: Programs crash, resources leak, users confused
- **After**: Graceful error handling, automatic cleanup, clear messages

**Master exception handling**, and you'll write:

- **Robust** code that doesn't crash
- **Maintainable** code with clear error paths
- **Professional** code with proper cleanup
- **User-friendly** applications with helpful errors

Exception handling is not optional for professional C++ development—it's **essential**!

**Happy Coding!** 🚀

---

## Remember

_"Expect the unexpected and handle it gracefully"_

Good programs handle errors:

- Validate input
- Check operations
- Catch exceptions
- Clean up resources
- Inform users clearly

Exception handling is your safety net—use it wisely! 💪