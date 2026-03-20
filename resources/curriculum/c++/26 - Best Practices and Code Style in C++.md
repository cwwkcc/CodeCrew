## Introduction

You've learned the **mechanics** of C++ - syntax, features, and techniques. Now it's time to learn the **art** of writing **professional, maintainable, and elegant** C++ code.

**Code is read far more often than it's written.** Good code style makes your programs:

- **Readable** - Others (and future you) can understand it
- **Maintainable** - Easy to modify and fix
- **Professional** - Shows mastery and care
- **Bug-resistant** - Clarity reduces errors
- **Collaborative** - Teams can work together effectively

**Real-World Analogy**:

- **Messy code** = Messy desk: Hard to find things, inefficient, unprofessional
- **Clean code** = Organized desk: Everything in its place, efficient, professional

---

## Table of Contents

1. Naming Conventions
2. Code Formatting and Layout
3. Comments and Documentation
4. Function Design
5. Class Design
6. Error Handling Practices
7. Memory Management
8. Modern C++ Best Practices
9. Performance Considerations
10. Code Organization
11. Common Anti-Patterns to Avoid

---

# 1. Naming Conventions

## Why Naming Matters

```cpp
// Bad - Unclear
int d;
void proc(int x);
class Mgr { };

// Good - Clear intent
int daysSinceCreation;
void processPayment(int amount);
class CustomerManager { };
```

**Good names are self-documenting!**

---

## Variable Naming

### Use Descriptive Names

```cpp
// ❌ Bad - Too short, unclear
int n;
double p;
string s;

// ✅ Good - Clear purpose
int numberOfStudents;
double productPrice;
string customerName;
```

### Boolean Variables

Prefix with `is`, `has`, `can`, `should`:

```cpp
// ✅ Good
bool isValid;
bool hasChildren;
bool canEdit;
bool shouldRetry;
bool isUserLoggedIn;

// ❌ Avoid
bool valid;        // Ambiguous
bool children;     // Looks like count
bool edit;         // Verb, not state
```

### Use Meaningful Context

```cpp
// ❌ Bad - Context unclear
string name;
int count;

// ✅ Good - Context clear
string customerName;
string productName;
int studentCount;
int errorCount;
```

### Avoid Magic Numbers

```cpp
// ❌ Bad
if (status == 1) { }
array[7] = value;

// ✅ Good
const int STATUS_ACTIVE = 1;
const int MAX_RETRY_ATTEMPTS = 7;

if (status == STATUS_ACTIVE) { }
array[MAX_RETRY_ATTEMPTS] = value;
```

---

## Function Naming

### Use Verb Phrases

```cpp
// ✅ Good - Actions are clear
void calculateTotalPrice();
bool validateEmail(string email);
string formatPhoneNumber(string phone);
int getUserAge();
void setUserName(string name);

// ❌ Avoid
void total();           // What about total?
bool email(string e);   // Is it checking? Sending?
string phone(string p); // Unclear action
```

### Common Verb Conventions

|Verb|Meaning|Example|
|---|---|---|
|`get`|Return a value|`getName()`|
|`set`|Assign a value|`setName()`|
|`is/has`|Return boolean|`isValid()`, `hasChildren()`|
|`calculate`|Compute and return|`calculateTotal()`|
|`validate`|Check validity|`validateInput()`|
|`process`|Perform operation|`processPayment()`|
|`create`|Construct object|`createUser()`|
|`delete/remove`|Delete item|`deleteRecord()`|
|`find/search`|Locate item|`findById()`|
|`update`|Modify existing|`updateStatus()`|

---

## Class Naming

### Use Nouns or Noun Phrases

```cpp
// ✅ Good
class Student { };
class BankAccount { };
class EmailValidator { };
class OrderProcessor { };
class DatabaseConnection { };

// ❌ Avoid
class ProcessOrder { };  // Sounds like function
class Validate { };      // Too generic
class DoStuff { };       // Meaningless
```

### Naming Patterns

```cpp
// Managers
class UserManager { };
class FileManager { };

// Handlers
class EventHandler { };
class ErrorHandler { };

// Services
class EmailService { };
class PaymentService { };

// Utilities
class StringUtility { };
class MathHelper { };

// Interfaces (abstract classes)
class IPrintable { };    // I prefix (optional)
class Serializable { };  // -able suffix
```

---

## Naming Conventions Summary

### Popular C++ Styles

**1. Google C++ Style**

```cpp
// Variables: snake_case
int student_count;
string customer_name;

// Functions: PascalCase
void CalculateTotal();
bool ValidateInput();

// Classes: PascalCase
class BankAccount { };

// Constants: kPascalCase
const int kMaxRetries = 5;

// Member variables: snake_case with trailing underscore
class MyClass {
private:
    int member_variable_;
};
```

**2. camelCase Style**

```cpp
// Variables: camelCase
int studentCount;
string customerName;

// Functions: camelCase
void calculateTotal();
bool validateInput();

// Classes: PascalCase
class BankAccount { };

// Constants: UPPER_CASE
const int MAX_RETRIES = 5;

// Member variables: camelCase with m_ prefix
class MyClass {
private:
    int m_memberVariable;
};
```

**Choose one style and be consistent!**

---

# 2. Code Formatting and Layout

## Indentation

### Use Consistent Indentation

```cpp
// ✅ Good - 4 spaces (or 2, but be consistent)
void calculateTotal() {
    for (int i = 0; i < count; i++) {
        if (values[i] > 0) {
            total += values[i];
        }
    }
}

// ❌ Bad - Inconsistent
void calculateTotal() {
for (int i = 0; i < count; i++) {
      if (values[i] > 0) {
    total += values[i];
      }
}
}
```

**Rule**: Use 2 or 4 spaces. Never mix tabs and spaces!

---

## Braces Placement

### K&R Style (Opening brace on same line)

```cpp
void function() {
    if (condition) {
        // code
    } else {
        // code
    }
}
```

### Allman Style (Opening brace on new line)

```cpp
void function()
{
    if (condition)
    {
        // code
    }
    else
    {
        // code
    }
}
```

**Both are fine - just be consistent!**

---

## Line Length

### Keep Lines Under 80-120 Characters

```cpp
// ❌ Bad - Too long
void processCustomerPaymentWithDiscountAndTaxCalculationIncludingShippingCosts(int customerId, double amount, double discount) { }

// ✅ Good - Broken into multiple lines
void processCustomerPayment(
    int customerId,
    double amount,
    double discount,
    bool includeShipping
) {
    // Implementation
}
```

---

## Whitespace

### Use Whitespace for Readability

```cpp
// ❌ Bad - Cramped
int x=5;
if(x>0){y=x*2;}
for(int i=0;i<10;i++){sum+=i;}

// ✅ Good - Readable
int x = 5;
if (x > 0) {
    y = x * 2;
}

for (int i = 0; i < 10; i++) {
    sum += i;
}
```

### Blank Lines for Separation

```cpp
#include <iostream>
#include <vector>

using namespace std;

class BankAccount {
private:
    double balance;
    string accountNumber;

public:
    BankAccount(string number) : balance(0.0), accountNumber(number) {}
    
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }
    
    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            return true;
        }
        return false;
    }
};

int main() {
    BankAccount account("ACC001");
    account.deposit(1000);
    
    return 0;
}
```

**Use blank lines to separate logical sections.**

---

# 3. Comments and Documentation

## When to Comment

### Don't Comment the Obvious

```cpp
// ❌ Bad - Obvious
int count = 0;  // Set count to zero
count++;        // Increment count by one

// ✅ Good - Explain WHY
int retryCount = 0;  // Track failed connection attempts
retryCount++;        // Retry due to timeout
```

### Comment Complex Logic

```cpp
// ✅ Good - Complex algorithm explained
// Boyer-Moore majority vote algorithm
// Time: O(n), Space: O(1)
int findMajorityElement(vector<int>& nums) {
    int candidate = 0;
    int count = 0;
    
    // Find candidate
    for (int num : nums) {
        if (count == 0) {
            candidate = num;
        }
        count += (num == candidate) ? 1 : -1;
    }
    
    return candidate;
}
```

### TODO and FIXME Comments

```cpp
// TODO: Add input validation
// FIXME: Memory leak when exception thrown
// HACK: Temporary workaround for bug #123
// NOTE: This assumes data is sorted
```

---

## Documentation Comments

### Function Documentation

```cpp
/**
 * Calculates the monthly payment for a loan.
 * 
 * @param principal The loan amount
 * @param annualRate The annual interest rate (e.g., 0.05 for 5%)
 * @param years The loan term in years
 * @return The monthly payment amount
 * @throws invalid_argument if any parameter is negative
 */
double calculateMonthlyPayment(double principal, double annualRate, int years) {
    if (principal < 0 || annualRate < 0 || years <= 0) {
        throw invalid_argument("Invalid loan parameters");
    }
    
    double monthlyRate = annualRate / 12;
    int numPayments = years * 12;
    
    return (principal * monthlyRate) / (1 - pow(1 + monthlyRate, -numPayments));
}
```

### Class Documentation

```cpp
/**
 * Represents a bank account with basic operations.
 * 
 * This class provides functionality for deposits, withdrawals,
 * and balance inquiries. All monetary values are in USD.
 * 
 * Example usage:
 *     BankAccount account("ACC001", 1000.0);
 *     account.deposit(500.0);
 *     account.withdraw(200.0);
 */
class BankAccount {
    // Implementation
};
```

---

# 4. Function Design

## Single Responsibility Principle

**Each function should do ONE thing well.**

```cpp
// ❌ Bad - Does too much
void processUser(User& user) {
    validateUser(user);
    saveToDatabase(user);
    sendEmailConfirmation(user);
    updateCache(user);
    logActivity(user);
}

// ✅ Good - Separated concerns
void processUser(User& user) {
    validateUser(user);
    saveUser(user);
    notifyUser(user);
}

void saveUser(User& user) {
    saveToDatabase(user);
    updateCache(user);
}

void notifyUser(User& user) {
    sendEmailConfirmation(user);
    logActivity(user);
}
```

---

## Function Length

**Keep functions short and focused (ideally < 20-30 lines).**

```cpp
// ❌ Bad - Too long
void processOrder(Order& order) {
    // 100+ lines of code...
}

// ✅ Good - Broken into smaller functions
void processOrder(Order& order) {
    validateOrder(order);
    calculateTotal(order);
    applyDiscounts(order);
    processPayment(order);
    updateInventory(order);
    sendConfirmation(order);
}
```

---

## Function Parameters

### Limit Number of Parameters

```cpp
// ❌ Bad - Too many parameters
void createUser(string name, string email, string phone, 
                string address, string city, string state, 
                string zip, int age, bool active) { }

// ✅ Good - Use structure
struct UserInfo {
    string name;
    string email;
    string phone;
    Address address;
    int age;
    bool active;
};

void createUser(const UserInfo& info) { }
```

### Use const References for Read-Only Parameters

```cpp
// ❌ Bad - Copies entire object
void displayStudent(Student student) {
    cout << student.getName() << endl;
}

// ✅ Good - No copy, read-only
void displayStudent(const Student& student) {
    cout << student.getName() << endl;
}
```

---

## Return Values

### Prefer Return Values Over Out Parameters

```cpp
// ❌ Less clear - Output parameter
void calculateTotal(const vector<int>& items, int& total) {
    total = 0;
    for (int item : items) {
        total += item;
    }
}

// ✅ Better - Return value
int calculateTotal(const vector<int>& items) {
    int total = 0;
    for (int item : items) {
        total += item;
    }
    return total;
}
```

---

# 5. Class Design

## Encapsulation

**Keep data members private, provide public interface.**

```cpp
// ❌ Bad - Public data
class BankAccount {
public:
    double balance;  // Anyone can modify!
};

// ✅ Good - Encapsulated
class BankAccount {
private:
    double balance;

public:
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }
    
    double getBalance() const {
        return balance;
    }
};
```

---

## Class Organization

**Organize class members logically.**

```cpp
class MyClass {
public:
    // 1. Constructors and destructor
    MyClass();
    MyClass(int value);
    ~MyClass();
    
    // 2. Public member functions
    void doSomething();
    int getValue() const;
    
    // 3. Operator overloads
    MyClass& operator=(const MyClass& other);
    
protected:
    // Protected members
    
private:
    // 4. Private member functions
    void helperFunction();
    
    // 5. Private member variables
    int value_;
    string name_;
};
```

---

## Const Correctness

**Use const wherever possible.**

```cpp
class Student {
private:
    string name;
    double gpa;

public:
    // Const member functions don't modify object
    string getName() const { return name; }
    double getGPA() const { return gpa; }
    bool isPassing() const { return gpa >= 2.0; }
    
    // Non-const member functions can modify
    void setName(const string& newName) { name = newName; }
    void setGPA(double newGPA) { gpa = newGPA; }
};

// Const object can only call const methods
const Student student("Alice", 3.8);
cout << student.getName() << endl;  // OK
// student.setName("Bob");          // Error! Can't modify const object
```

---

## Initialize Member Variables

**Use initialization lists, not assignment in constructor body.**

```cpp
// ❌ Bad - Assignment in body
class Student {
private:
    string name;
    int age;
    double gpa;

public:
    Student(string n, int a, double g) {
        name = n;    // Assignment
        age = a;
        gpa = g;
    }
};

// ✅ Good - Initialization list
class Student {
private:
    string name;
    int age;
    double gpa;

public:
    Student(string n, int a, double g) 
        : name(n), age(a), gpa(g) {  // Direct initialization
        // Constructor body
    }
};
```

---

# 6. Error Handling Practices

## Use Exceptions for Exceptional Cases

```cpp
// ✅ Good - Exception for error
double divide(double a, double b) {
    if (b == 0) {
        throw invalid_argument("Division by zero");
    }
    return a / b;
}

// Use exceptions for unexpected errors, not control flow
```

---

## Validate Input

```cpp
void setAge(int age) {
    if (age < 0 || age > 150) {
        throw invalid_argument("Invalid age: " + to_string(age));
    }
    this->age = age;
}

void processPayment(double amount) {
    if (amount <= 0) {
        throw invalid_argument("Amount must be positive");
    }
    
    if (amount > balance) {
        throw runtime_error("Insufficient funds");
    }
    
    balance -= amount;
}
```

---

## RAII for Resource Management

```cpp
// ✅ Good - RAII ensures cleanup
class FileHandler {
private:
    ofstream file;

public:
    FileHandler(const string& filename) {
        file.open(filename);
        if (!file) {
            throw runtime_error("Cannot open file: " + filename);
        }
    }
    
    ~FileHandler() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    void write(const string& data) {
        file << data;
    }
};

// Usage - automatic cleanup even with exceptions
void processFile() {
    FileHandler handler("data.txt");
    handler.write("Hello");
    // File automatically closed when handler goes out of scope
}
```

---

# 7. Memory Management

## Prefer Smart Pointers

```cpp
// ❌ Avoid - Manual memory management
void function() {
    MyClass* obj = new MyClass();
    // What if exception thrown? Memory leak!
    delete obj;
}

// ✅ Good - Smart pointers
void function() {
    unique_ptr<MyClass> obj = make_unique<MyClass>();
    // Automatically cleaned up
}

void function() {
    shared_ptr<MyClass> obj = make_shared<MyClass>();
    // Reference counted, cleaned up when last reference gone
}
```

---

## Avoid Manual new/delete

```cpp
// ❌ Avoid
int* arr = new int[100];
// ... use arr ...
delete[] arr;

// ✅ Better - Use vector
vector<int> arr(100);
// Automatically managed
```

---

## Rule of Zero/Three/Five

**If you don't manage resources, compiler-generated functions are fine (Rule of Zero).**

```cpp
// ✅ Good - Rule of Zero
class Student {
private:
    string name;      // Manages its own memory
    vector<int> scores;  // Manages its own memory

public:
    // No need for destructor, copy constructor, or copy assignment
    // Compiler-generated versions work correctly
};
```

**If you manage resources, implement all three (Rule of Three) or all five (Rule of Five).**

```cpp
// ✅ Good - Rule of Three
class Buffer {
private:
    int* data;
    int size;

public:
    // Destructor
    ~Buffer() {
        delete[] data;
    }
    
    // Copy constructor
    Buffer(const Buffer& other) : size(other.size) {
        data = new int[size];
        copy(other.data, other.data + size, data);
    }
    
    // Copy assignment
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            copy(other.data, other.data + size, data);
        }
        return *this;
    }
};
```

---

# 8. Modern C++ Best Practices

## Use auto for Type Deduction

```cpp
// ✅ Good - Concise and clear
auto student = make_unique<Student>("Alice", 20);
auto it = myMap.find(key);
auto result = complexFunctionCall();

// Don't overuse - keep readability
auto x = 5;  // OK, obvious
auto y = processData();  // Less clear - what type is returned?
```

---

## Use Range-Based For Loops

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};

// ❌ Old style
for (int i = 0; i < numbers.size(); i++) {
    cout << numbers[i] << endl;
}

// ✅ Modern
for (int num : numbers) {
    cout << num << endl;
}

// ✅ With const reference (efficient)
for (const auto& student : students) {
    student.display();
}
```

---

## Use nullptr Instead of NULL

```cpp
// ❌ Old
int* ptr = NULL;
if (ptr == NULL) { }

// ✅ Modern
int* ptr = nullptr;
if (ptr == nullptr) { }
```

---

## Use override Keyword

```cpp
class Base {
public:
    virtual void display() { }
};

class Derived : public Base {
public:
    // ✅ Good - Explicit override
    void display() override { }
    
    // Compiler catches mistakes
    // void displya() override { }  // Error - typo caught!
};
```

---

## Use enum class Instead of enum

```cpp
// ❌ Old enum - pollutes namespace
enum Color { RED, GREEN, BLUE };
Color c = RED;

// ✅ Modern enum class - scoped
enum class Color { Red, Green, Blue };
Color c = Color::Red;
```

---

# 9. Performance Considerations

## Pass Large Objects by const Reference

```cpp
// ❌ Bad - Copies entire vector
void processData(vector<int> data) {
    // ...
}

// ✅ Good - No copy
void processData(const vector<int>& data) {
    // ...
}
```

---

## Reserve Vector Capacity

```cpp
// ❌ Inefficient - Multiple reallocations
vector<int> numbers;
for (int i = 0; i < 10000; i++) {
    numbers.push_back(i);
}

// ✅ Efficient - Single allocation
vector<int> numbers;
numbers.reserve(10000);
for (int i = 0; i < 10000; i++) {
    numbers.push_back(i);
}
```

---

## Choose Correct Container

```cpp
// Fast random access needed? Use vector
vector<int> data;

// Frequent insert/delete in middle? Use list
list<int> data;

// Need unique sorted elements? Use set
set<int> data;

// Need fast lookup by key? Use unordered_map
unordered_map<string, int> data;
```

---

## Avoid Premature Optimization

```cpp
// ❌ Bad - Premature optimization
// Optimizing before measuring performance

// ✅ Good - Optimize after profiling
// 1. Write clear code first
// 2. Profile to find bottlenecks
// 3. Optimize only hot spots
// 4. Measure improvement
```

**"Premature optimization is the root of all evil" - Donald Knuth**

---

# 10. Code Organization

## Header Guards

```cpp
// ✅ Good - Include guard
#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H

class BankAccount {
    // Class definition
};

#endif  // BANK_ACCOUNT_H

// ✅ Alternative - #pragma once (most compilers)
#pragma once

class BankAccount {
    // Class definition
};
```

---

## File Organization

```
project/
├── include/           # Header files
│   ├── BankAccount.h
│   └── Customer.h
├── src/              # Implementation files
│   ├── BankAccount.cpp
│   ├── Customer.cpp
│   └── main.cpp
├── tests/            # Test files
│   └── BankAccountTest.cpp
└── README.md
```

---

## Keep Related Code Together

```cpp
// ✅ Good - Related functions grouped
class BankAccount {
public:
    // Account operations
    void deposit(double amount);
    void withdraw(double amount);
    double getBalance() const;
    
    // Transaction history
    void addTransaction(const Transaction& t);
    vector<Transaction> getHistory() const;
    
    // Account management
    void close();
    bool isActive() const;
};
```

---

# 11. Common Anti-Patterns to Avoid

## God Object

**One class that does everything.**

```cpp
// ❌ Bad - God object
class Application {
    void connectDatabase();
    void processPayment();
    void sendEmail();
    void generateReport();
    void validateUser();
    void updateCache();
    // ... 50 more methods
};

// ✅ Good - Separated responsibilities
class DatabaseManager { };
class PaymentProcessor { };
class EmailService { };
class ReportGenerator { };
class UserValidator { };
```

---

## Magic Numbers

```cpp
// ❌ Bad
if (status == 3) { }
for (int i = 0; i < 7; i++) { }

// ✅ Good
const int STATUS_COMPLETED = 3;
const int DAYS_IN_WEEK = 7;

if (status == STATUS_COMPLETED) { }
for (int i = 0; i < DAYS_IN_WEEK; i++) { }
```

---

## Deep Nesting

```cpp
// ❌ Bad - Too many nested levels
void processOrder(Order order) {
    if (order.isValid()) {
        if (order.hasItems()) {
            if (order.customer.isActive()) {
                if (order.total > 0) {
                    if (processPayment(order)) {
                        shipOrder(order);
                    }
                }
            }
        }
    }
}

// ✅ Good - Early returns, guard clauses
void processOrder(Order order) {
    if (!order.isValid()) return;
    if (!order.hasItems()) return;
    if (!order.customer.isActive()) return;
    if (order.total <= 0) return;
    
    if (processPayment(order)) {
        shipOrder(order);
    }
}
```

---

## Long Parameter Lists

```cpp
// ❌ Bad
void createAccount(string name, string email, string phone, 
                   string address, string city, string state,
                   string zip, int age, bool premium);

// ✅ Good
struct AccountInfo {
    string name, email, phone;
    Address address;
    int age;
    bool premium;
};

void createAccount(const AccountInfo& info);
```

---

## Ignoring Return Values

```cpp
// ❌ Bad - Ignoring important return values
file.read(buffer, size);  // What if read failed?
vector.erase(item);       // Was item found and erased?

// ✅ Good - Check return values
if (!file.read(buffer, size)) {
    // Handle error
}

auto it = vector.erase(item);
if (it == vector.end()) {
    // Item not found
}
```

---

## Complete Example: Before and After

### Before - Poor Style

```cpp
#include <iostream>
#include <string>
using namespace std;
class b{public:string n;int a;double g;void d(){cout<<n<<" "<<a<<" "<<g<<endl;}};
int main(){b s;s.n="John";s.a=20;s.g=3.5;s.d();return 0;}
```

### After - Good Style

```cpp
#include <iostream>
#include <string>

using namespace std;

/**
 * Represents a student with basic information.
 */
class Student {
private:
    string name;
    int age;
    double gpa;

public:
    /**
     * Creates a student with specified information.
     */
    Student(const string& studentName, int studentAge, double studentGPA)
        : name(studentName), age(studentAge), gpa(studentGPA) {
        validateAge(studentAge);
        validateGPA(studentGPA);
    }
    
    // Getters
    string getName() const { return name; }
    int getAge() const { return age; }
    double getGPA() const { return gpa; }
    
    /**
     * Displays student information in a readable format.
     */
    void display() const {
        cout << "Student: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "GPA: " << gpa << endl;
    }

private:
    void validateAge(int age) const {
        if (age < 0 || age > 150) {
            throw invalid_argument("Invalid age");
        }
    }
    
    void validateGPA(double gpa) const {
        if (gpa < 0.0 || gpa > 4.0) {
            throw invalid_argument("GPA must be between 0.0 and 4.0");
        }
    }
};

int main() {
    try {
        Student student("John Doe", 20, 3.5);
        student.display();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
```

---

## Best Practices Checklist

### Naming

- [ ] Variables have descriptive names
- [ ] Functions use verb phrases
- [ ] Classes use noun phrases
- [ ] Constants are clearly identified
- [ ] Consistent naming convention used

### Formatting

- [ ] Consistent indentation (2 or 4 spaces)
- [ ] Reasonable line length (< 120 chars)
- [ ] Proper use of whitespace
- [ ] Consistent brace style

### Comments

- [ ] Complex logic explained
- [ ] Public API documented
- [ ] No obvious comments
- [ ] TODO/FIXME marked

### Functions

- [ ] Single responsibility
- [ ] Reasonable length (< 30 lines)
- [ ] Limited parameters (< 4)
- [ ] const correctness

### Classes

- [ ] Data members private
- [ ] Logical organization
- [ ] Proper initialization
- [ ] const member functions marked

### Error Handling

- [ ] Input validated
- [ ] Exceptions used appropriately
- [ ] Resources managed with RAII
- [ ] Error messages clear

### Memory

- [ ] Smart pointers used
- [ ] No manual new/delete
- [ ] Rule of Zero/Three/Five followed
- [ ] No memory leaks

### Modern C++

- [ ] auto used appropriately
- [ ] Range-based for loops
- [ ] nullptr instead of NULL
- [ ] override keyword used
- [ ] enum class used

### Performance

- [ ] Large objects passed by reference
- [ ] Correct container chosen
- [ ] Vector capacity reserved
- [ ] Optimize only after profiling

### Organization

- [ ] Header guards used
- [ ] Related code grouped
- [ ] Logical file structure
- [ ] Dependencies minimized

---

## Summary

### Key Principles

1. **Clarity over cleverness** - Write code that's easy to understand
2. **Consistency** - Follow one style throughout
3. **Simplicity** - Keep it simple, don't over-engineer
4. **Maintainability** - Think of future readers
5. **Safety** - Prevent errors through good design

### The Three C's of Good Code

1. **Clean** - Well-formatted, organized, readable
2. **Clear** - Self-documenting, obvious intent
3. **Correct** - Works as expected, handles errors

### Remember

**"Any fool can write code that a computer can understand. Good programmers write code that humans can understand."** - Martin Fowler

**"Code is read much more often than it is written."** - Guido van Rossum

---

## Conclusion

Good code style is not about being pedantic - it's about **professionalism** and **respect**:

- **Respect for yourself** - Future you will thank present you
- **Respect for teammates** - Easy for others to understand and modify
- **Respect for users** - Fewer bugs, better quality

Master these practices, and you'll write code that:

- ✅ Is easy to understand
- ✅ Is easy to maintain
- ✅ Has fewer bugs
- ✅ Looks professional
- ✅ Gets you hired!

**Your code is your signature - make it clean!** ✨

**Happy Coding!** 🚀