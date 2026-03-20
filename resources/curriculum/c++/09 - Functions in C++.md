## Introduction

Imagine you're building a house. You don't create custom tools for every nail - you use the same hammer repeatedly. Similarly, in programming, **functions** let you write code once and use it many times.

A **function** is a block of code that performs a specific task. You can call (use) it whenever needed, making your programs organized, reusable, and easier to understand.

**Real-World Analogies**:

- **Vending Machine**: Input (money + selection) → Process (dispense item) → Output (snack)
- **Calculator**: Input (2 numbers + operation) → Process (calculate) → Output (result)
- **Microwave**: Input (food + time) → Process (heat) → Output (warm food)
- **ATM**: Input (card + PIN) → Process (verify) → Output (cash/receipt)

---

## Why Functions Matter

### Without Functions

```cpp
// Calculate area of rectangle 1
int length1 = 5;
int width1 = 3;
int area1 = length1 * width1;
cout << "Area 1: " << area1 << endl;

// Calculate area of rectangle 2
int length2 = 8;
int width2 = 4;
int area2 = length2 * width2;
cout << "Area 2: " << area2 << endl;

// Calculate area of rectangle 3
int length3 = 10;
int width3 = 6;
int area3 = length3 * width3;
cout << "Area 3: " << area3 << endl;

// Repetitive and error-prone!
```

### With Functions

```cpp
int calculateArea(int length, int width) {
    return length * width;
}

// Use the function
cout << "Area 1: " << calculateArea(5, 3) << endl;
cout << "Area 2: " << calculateArea(8, 4) << endl;
cout << "Area 3: " << calculateArea(10, 6) << endl;

// Clean, reusable, maintainable!
```

### Benefits of Functions

1. **Code Reusability** - Write once, use many times
2. **Organization** - Break large programs into manageable pieces
3. **Readability** - Self-documenting code with meaningful names
4. **Maintainability** - Fix bugs in one place
5. **Testing** - Test individual functions separately
6. **Collaboration** - Team members work on different functions
7. **Abstraction** - Hide complex details behind simple interfaces

---

## Function Basics

### Anatomy of a Function

```cpp
returnType functionName(parameterType parameterName) {
    // Function body
    // Code to execute
    return value;  // If returnType is not void
}
```

**Parts explained**:

1. **Return Type**: What type of data the function gives back (int, double, void, etc.)
2. **Function Name**: Descriptive name following naming rules
3. **Parameters**: Input values (can be zero or more)
4. **Function Body**: Code that executes when function is called
5. **Return Statement**: Sends result back to caller

### Simple Function Example

```cpp
#include <iostream>
using namespace std;

// Function definition
void greet() {
    cout << "Hello, World!" << endl;
}

int main() {
    greet();  // Function call
    greet();  // Can call multiple times
    greet();
    
    return 0;
}

// Output:
// Hello, World!
// Hello, World!
// Hello, World!
```

---

## Function Declaration and Definition

### Two Ways to Structure Functions

**Method 1: Definition before main()**

```cpp
#include <iostream>
using namespace std;

// Function definition comes before main
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);
    cout << result << endl;  // 8
    return 0;
}
```

**Method 2: Declaration before main, Definition after** (Recommended)

```cpp
#include <iostream>
using namespace std;

// Function prototype (declaration)
int add(int a, int b);

int main() {
    int result = add(5, 3);
    cout << result << endl;  // 8
    return 0;
}

// Function definition
int add(int a, int b) {
    return a + b;
}
```

### Function Prototype (Declaration)

A **function prototype** tells the compiler about the function before it's used.

**Syntax**:

```cpp
returnType functionName(parameterTypes);
```

**Examples**:

```cpp
int add(int a, int b);           // Full prototype
int add(int, int);               // Parameter names optional
void printMessage();             // No parameters
double calculateAverage(int[], int);  // Array parameter
```

**Why use prototypes?**

- Call functions in any order
- Compiler checks for errors early
- Standard practice in C++
- Required when functions call each other

---

## Return Types

### void Functions (No Return Value)

Functions that perform an action but don't return a value.

```cpp
void printLine() {
    cout << "=================" << endl;
}

void greetUser(string name) {
    cout << "Hello, " << name << "!" << endl;
}

void displayMenu() {
    cout << "1. New Game" << endl;
    cout << "2. Load Game" << endl;
    cout << "3. Exit" << endl;
}

int main() {
    printLine();
    greetUser("Alice");
    displayMenu();
    
    return 0;
}
```

### Functions that Return Values

**int return type**:

```cpp
int add(int a, int b) {
    return a + b;
}

int getAge() {
    int age;
    cout << "Enter age: ";
    cin >> age;
    return age;
}

int main() {
    int sum = add(10, 5);          // sum = 15
    int userAge = getAge();        // Gets input and returns it
    
    return 0;
}
```

**double return type**:

```cpp
double calculateAverage(double num1, double num2) {
    return (num1 + num2) / 2.0;
}

double celsiusToFahrenheit(double celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}
```

**bool return type**:

```cpp
bool isEven(int number) {
    return (number % 2 == 0);
}

bool isPrime(int n) {
    if (n <= 1) return false;
    
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    
    return true;
}

int main() {
    if (isEven(10)) {
        cout << "10 is even" << endl;
    }
    
    if (isPrime(17)) {
        cout << "17 is prime" << endl;
    }
    
    return 0;
}
```

**char return type**:

```cpp
char getGrade(int score) {
    if (score >= 90) return 'A';
    else if (score >= 80) return 'B';
    else if (score >= 70) return 'C';
    else if (score >= 60) return 'D';
    else return 'F';
}
```

**string return type**:

```cpp
#include <string>

string getFullName(string first, string last) {
    return first + " " + last;
}

string getDayName(int day) {
    switch (day) {
        case 1: return "Monday";
        case 2: return "Tuesday";
        case 3: return "Wednesday";
        case 4: return "Thursday";
        case 5: return "Friday";
        case 6: return "Saturday";
        case 7: return "Sunday";
        default: return "Invalid";
    }
}
```

---

## Parameters and Arguments

### Terminology

- **Parameter**: Variable in function definition (placeholder)
- **Argument**: Actual value passed when calling function

```cpp
// Parameters: a and b
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);  // Arguments: 5 and 3
    return 0;
}
```

### No Parameters

```cpp
void sayHello() {
    cout << "Hello!" << endl;
}

int getRandomNumber() {
    return rand() % 100;
}
```

### Single Parameter

```cpp
void printSquare(int n) {
    cout << n << " squared is " << (n * n) << endl;
}

bool isPositive(int number) {
    return number > 0;
}
```

### Multiple Parameters

```cpp
int add(int a, int b) {
    return a + b;
}

double calculateRectangleArea(double length, double width) {
    return length * width;
}

void displayPersonInfo(string name, int age, double height) {
    cout << "Name: " << name << endl;
    cout << "Age: " << age << endl;
    cout << "Height: " << height << " m" << endl;
}
```

### Default Parameters

Parameters can have default values if not provided.

```cpp
void greet(string name = "Guest") {
    cout << "Hello, " << name << "!" << endl;
}

double power(double base, int exponent = 2) {
    double result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

int main() {
    greet();              // Uses default: "Hello, Guest!"
    greet("Alice");       // Uses provided: "Hello, Alice!"
    
    cout << power(5);     // 5^2 = 25
    cout << power(5, 3);  // 5^3 = 125
    
    return 0;
}
```

**Rules for default parameters**:

- Must be rightmost parameters
- Cannot skip middle parameters

```cpp
// Valid
void func1(int a, int b = 5, int c = 10);
void func2(int a, int b = 5);

// Invalid
void func3(int a = 5, int b);  // Default must be rightmost
```

---

## Pass by Value vs Pass by Reference

### Pass by Value (Default)

A **copy** of the argument is passed. Changes inside function don't affect original.

```cpp
#include <iostream>
using namespace std;

void modifyValue(int x) {
    x = 100;  // Changes only the copy
    cout << "Inside function: " << x << endl;
}

int main() {
    int num = 5;
    
    cout << "Before function: " << num << endl;  // 5
    modifyValue(num);                             // Inside: 100
    cout << "After function: " << num << endl;   // 5 (unchanged!)
    
    return 0;
}
```

**Execution**:

```
num = 5
Call modifyValue(num)
  → Copy created: x = 5
  → x changed to 100
  → Function ends, copy destroyed
num still = 5 (original unchanged)
```

### Pass by Reference

The **actual variable** is passed. Changes inside function **do** affect original.

**Syntax**: Use `&` before parameter name

```cpp
#include <iostream>
using namespace std;

void modifyValue(int &x) {  // Note the &
    x = 100;  // Changes the original!
    cout << "Inside function: " << x << endl;
}

int main() {
    int num = 5;
    
    cout << "Before function: " << num << endl;  // 5
    modifyValue(num);                             // Inside: 100
    cout << "After function: " << num << endl;   // 100 (changed!)
    
    return 0;
}
```

### When to Use Each

**Pass by Value**:

- For small data types (int, char, bool, etc.)
- When you don't want to modify original
- For "read-only" parameters

**Pass by Reference**:

- When you want to modify original variable
- For large objects (arrays, strings, structures)
- To return multiple values
- For efficiency (avoid copying large data)

### Practical Example: Swap Function

**Using pass by value** (doesn't work):

```cpp
void swap(int a, int b) {
    int temp = a;
    a = b;
    b = temp;
    // Only swaps the copies!
}

int main() {
    int x = 5, y = 10;
    swap(x, y);
    cout << x << ", " << y << endl;  // Still 5, 10
    return 0;
}
```

**Using pass by reference** (works):

```cpp
void swap(int &a, int &b) {  // Note the &
    int temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 5, y = 10;
    swap(x, y);
    cout << x << ", " << y << endl;  // Now 10, 5
    return 0;
}
```

### Multiple Return Values Using References

```cpp
void getRectangleInfo(double length, double width, 
                      double &area, double &perimeter) {
    area = length * width;
    perimeter = 2 * (length + width);
}

int main() {
    double a, p;
    
    getRectangleInfo(5.0, 3.0, a, p);
    
    cout << "Area: " << a << endl;          // 15
    cout << "Perimeter: " << p << endl;     // 16
    
    return 0;
}
```

---

## Function Overloading

**Function overloading** allows multiple functions with the same name but different parameters.

### Same Name, Different Parameters

```cpp
#include <iostream>
using namespace std;

// Function 1: Two integers
int add(int a, int b) {
    return a + b;
}

// Function 2: Three integers
int add(int a, int b, int c) {
    return a + b + c;
}

// Function 3: Two doubles
double add(double a, double b) {
    return a + b;
}

int main() {
    cout << add(5, 3) << endl;           // Calls function 1: 8
    cout << add(5, 3, 2) << endl;        // Calls function 2: 10
    cout << add(5.5, 3.2) << endl;       // Calls function 3: 8.7
    
    return 0;
}
```

**Compiler decides which function to call based on**:

1. Number of arguments
2. Type of arguments
3. Order of arguments

### More Overloading Examples

```cpp
// Display function overloaded
void display(int x) {
    cout << "Integer: " << x << endl;
}

void display(double x) {
    cout << "Double: " << x << endl;
}

void display(string x) {
    cout << "String: " << x << endl;
}

int main() {
    display(42);         // Calls first version
    display(3.14);       // Calls second version
    display("Hello");    // Calls third version
    
    return 0;
}
```

### What Doesn't Count as Overloading

**Return type alone is NOT enough**:

```cpp
// Error! Can't overload based on return type only
int getValue() { return 5; }
double getValue() { return 5.5; }  // Compile error!
```

**Must differ in parameters**:

```cpp
// Valid - different number of parameters
void print(int x);
void print(int x, int y);

// Valid - different types
void print(int x);
void print(double x);

// Invalid - only return type differs
int get();
double get();  // Error!
```

---

## Recursion

**Recursion** is when a function calls itself. It's useful for problems that can be broken into smaller, similar subproblems.

### Basic Recursion Concept

```cpp
void countdown(int n) {
    if (n <= 0) {
        cout << "Blast off!" << endl;
        return;  // Base case - stop recursion
    }
    
    cout << n << " ";
    countdown(n - 1);  // Recursive call
}

int main() {
    countdown(5);
    // Output: 5 4 3 2 1 Blast off!
    return 0;
}
```

**How it works**:

```
countdown(5)
  → print 5, call countdown(4)
      → print 4, call countdown(3)
          → print 3, call countdown(2)
              → print 2, call countdown(1)
                  → print 1, call countdown(0)
                      → print "Blast off!", return
```

### Factorial Using Recursion

**Mathematical definition**:

- 5! = 5 × 4 × 3 × 2 × 1 = 120
- n! = n × (n-1)!
- 0! = 1 (base case)

```cpp
int factorial(int n) {
    // Base case
    if (n <= 1) {
        return 1;
    }
    
    // Recursive case
    return n * factorial(n - 1);
}

int main() {
    cout << "5! = " << factorial(5) << endl;  // 120
    return 0;
}
```

**Execution trace for factorial(5)**:

```
factorial(5) = 5 * factorial(4)
               5 * (4 * factorial(3))
               5 * (4 * (3 * factorial(2)))
               5 * (4 * (3 * (2 * factorial(1))))
               5 * (4 * (3 * (2 * 1)))
               5 * (4 * (3 * 2))
               5 * (4 * 6)
               5 * 24
               120
```

### Fibonacci Using Recursion

```cpp
int fibonacci(int n) {
    // Base cases
    if (n <= 1) {
        return n;
    }
    
    // Recursive case
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    cout << "Fibonacci sequence:" << endl;
    for (int i = 0; i < 10; i++) {
        cout << fibonacci(i) << " ";
    }
    cout << endl;
    // Output: 0 1 1 2 3 5 8 13 21 34
    
    return 0;
}
```

### Sum of Digits Using Recursion

```cpp
int sumDigits(int n) {
    // Base case
    if (n == 0) {
        return 0;
    }
    
    // Recursive case
    return (n % 10) + sumDigits(n / 10);
}

int main() {
    cout << sumDigits(12345) << endl;  // 1+2+3+4+5 = 15
    return 0;
}
```

### Important Notes About Recursion

**Must have**:

1. **Base case** - Condition to stop recursion
2. **Recursive case** - Function calls itself
3. **Progress toward base case** - Each call gets closer to stopping

**⚠️ Warning: Stack Overflow**

```cpp
// Infinite recursion - no base case!
void badRecursion(int n) {
    cout << n << endl;
    badRecursion(n);  // Never stops!
}
```

**Recursion vs Iteration**:

- Recursion: Often clearer, uses more memory (function calls)
- Iteration: Often faster, uses less memory

---

## Scope and Lifetime

### Local Variables

Variables declared inside a function - only accessible within that function.

```cpp
void function1() {
    int x = 10;  // Local to function1
    cout << x << endl;
}

void function2() {
    // cout << x << endl;  // Error! x doesn't exist here
    int x = 20;  // Different x, local to function2
    cout << x << endl;
}

int main() {
    function1();  // Prints 10
    function2();  // Prints 20
    // cout << x << endl;  // Error! x doesn't exist here
    return 0;
}
```

### Global Variables

Variables declared outside all functions - accessible everywhere.

```cpp
#include <iostream>
using namespace std;

int globalVar = 100;  // Global variable

void function1() {
    cout << "Function1: " << globalVar << endl;  // Can access
    globalVar = 200;  // Can modify
}

void function2() {
    cout << "Function2: " << globalVar << endl;  // Can access
}

int main() {
    cout << "Main: " << globalVar << endl;     // 100
    function1();                                 // Changes to 200
    function2();                                 // Prints 200
    return 0;
}
```

**⚠️ Warning**: Avoid global variables when possible!

- Makes code harder to understand
- Can lead to unexpected bugs
- Functions are less reusable

### Variable Shadowing

Local variable with same name as global variable "shadows" the global.

```cpp
int x = 100;  // Global

void function() {
    int x = 50;  // Local - shadows global
    cout << x << endl;  // Prints 50 (local)
    
    cout << ::x << endl;  // Prints 100 (global, using :: operator)
}
```

### Static Local Variables

Regular local variables are destroyed when function ends. **Static** variables persist between function calls.

```cpp
void counter() {
    static int count = 0;  // Initialized only once
    count++;
    cout << "Count: " << count << endl;
}

int main() {
    counter();  // Count: 1
    counter();  // Count: 2
    counter();  // Count: 3
    return 0;
}
```

---

## Practical Examples and Projects

### Example 1: Calculator with Functions

```cpp
#include <iostream>
using namespace std;

// Function prototypes
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
void displayMenu();
char getChoice();
void getNumbers(double &num1, double &num2);

int main() {
    double num1, num2, result;
    char choice;
    
    do {
        displayMenu();
        choice = getChoice();
        
        if (choice == 'Q' || choice == 'q') {
            break;
        }
        
        getNumbers(num1, num2);
        
        switch (choice) {
            case 'A':
            case 'a':
                result = add(num1, num2);
                cout << "Result: " << result << endl;
                break;
            
            case 'S':
            case 's':
                result = subtract(num1, num2);
                cout << "Result: " << result << endl;
                break;
            
            case 'M':
            case 'm':
                result = multiply(num1, num2);
                cout << "Result: " << result << endl;
                break;
            
            case 'D':
            case 'd':
                if (num2 != 0) {
                    result = divide(num1, num2);
                    cout << "Result: " << result << endl;
                } else {
                    cout << "Error: Division by zero!" << endl;
                }
                break;
            
            default:
                cout << "Invalid choice!" << endl;
                break;
        }
        
        cout << endl;
        
    } while (true);
    
    cout << "Thank you for using calculator!" << endl;
    
    return 0;
}

void displayMenu() {
    cout << "=== Calculator ===" << endl;
    cout << "A - Addition" << endl;
    cout << "S - Subtraction" << endl;
    cout << "M - Multiplication" << endl;
    cout << "D - Division" << endl;
    cout << "Q - Quit" << endl;
}

char getChoice() {
    char choice;
    cout << "Enter choice: ";
    cin >> choice;
    return choice;
}

void getNumbers(double &num1, double &num2) {
    cout << "Enter first number: ";
    cin >> num1;
    cout << "Enter second number: ";
    cin >> num2;
}

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    return a / b;
}
```

### Example 2: Temperature Conversion System

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

// Function prototypes
double celsiusToFahrenheit(double celsius);
double fahrenheitToCelsius(double fahrenheit);
double celsiusToKelvin(double celsius);
double kelvinToCelsius(double kelvin);
void displayMenu();
void performConversion(int choice);

int main() {
    int choice;
    
    do {
        displayMenu();
        cout << "Enter choice (1-7): ";
        cin >> choice;
        
        if (choice >= 1 && choice <= 6) {
            performConversion(choice);
        } else if (choice == 7) {
            cout << "Goodbye!" << endl;
        } else {
            cout << "Invalid choice!" << endl;
        }
        
        cout << endl;
        
    } while (choice != 7);
    
    return 0;
}

void displayMenu() {
    cout << "=== Temperature Converter ===" << endl;
    cout << "1. Celsius to Fahrenheit" << endl;
    cout << "2. Fahrenheit to Celsius" << endl;
    cout << "3. Celsius to Kelvin" << endl;
    cout << "4. Kelvin to Celsius" << endl;
    cout << "5. Fahrenheit to Kelvin" << endl;
    cout << "6. Kelvin to Fahrenheit" << endl;
    cout << "7. Exit" << endl;
}

void performConversion(int choice) {
    double temp, result;
    
    cout << "Enter temperature: ";
    cin >> temp;
    
    cout << fixed << setprecision(2);
    
    switch (choice) {
        case 1:
            result = celsiusToFahrenheit(temp);
            cout << temp << "°C = " << result << "°F" << endl;
            break;
        
        case 2:
            result = fahrenheitToCelsius(temp);
            cout << temp << "°F = " << result << "°C" << endl;
            break;
        
        case 3:
            result = celsiusToKelvin(temp);
            cout << temp << "°C = " << result << "K" << endl;
            break;
        
        case 4:
            result = kelvinToCelsius(temp);
            cout << temp << "K = " << result << "°C" << endl;
            break;
        
        case 5:
            result = celsiusToKelvin(fahrenheitToCelsius(temp));
            cout << temp << "°F = " << result << "K" << endl;
            break;
        
        case 6:
            result = celsiusToFahrenheit(kelvinToCelsius(temp));
            cout << temp << "K = " << result << "°F" << endl;
            break;
    }
}

double celsiusToFahrenheit(double celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

double fahrenheitToCelsius(double fahrenheit) {
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

double celsiusToKelvin(double celsius) {
    return celsius + 273.15;
}

double kelvinToCelsius(double kelvin) {
    return kelvin - 273.15;
}
```

### Example 3: Grade Management System

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

// Function prototypes
void displayMenu();
void enterGrades(double grades[], int &count);
double calculateAverage(double grades[], int count);
double findHighest(double grades[], int count);
double findLowest(double grades[], int count);
char getLetterGrade(double score);
void displayStatistics(double grades[], int count);
void displayGrades(double grades[], int count);

const int MAX_STUDENTS = 100;

int main() {
    double grades[MAX_STUDENTS];
    int count = 0;
    int choice;
    
    do {
        displayMenu();
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                enterGrades(grades, count);
                break;
            
            case 2:
                if (count > 0) {
                    displayGrades(grades, count);
                } else {
                    cout << "No grades entered yet!" << endl;
                }
                break;
            
            case 3:
                if (count > 0) {
                    displayStatistics(grades, count);
                } else {
                    cout << "No grades entered yet!" << endl;
                }
                break;
            
            case 4:
                cout << "Exiting program..." << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
                break;
        }
        
        cout << endl;
        
    } while (choice != 4);
    
    return 0;
}

void displayMenu() {
    cout << "=== Grade Management System ===" << endl;
    cout << "1. Enter Grades" << endl;
    cout << "2. Display Grades" << endl;
    cout << "3. Display Statistics" << endl;
    cout << "4. Exit" << endl;
}

void enterGrades(double grades[], int &count) {
    cout << "How many students? ";
    cin >> count;
    
    if (count > MAX_STUDENTS) {
        cout << "Maximum " << MAX_STUDENTS << " students allowed!" << endl;
        count = MAX_STUDENTS;
    }
    
    for (int i = 0; i < count; i++) {
        do {
            cout << "Enter grade for student " << (i + 1) << " (0-100): ";
            cin >> grades[i];
            
            if (grades[i] < 0 || grades[i] > 100) {
                cout << "Invalid grade! Try again." << endl;
            }
        } while (grades[i] < 0 || grades[i] > 100);
    }
    
    cout << "Grades entered successfully!" << endl;
}

void displayGrades(double grades[], int count) {
    cout << "\n=== Student Grades ===" << endl;
    cout << fixed << setprecision(2);
    
    for (int i = 0; i < count; i++) {
        cout << "Student " << (i + 1) << ": " 
             << grades[i] << " (" 
             << getLetterGrade(grades[i]) << ")" << endl;
    }
}

void displayStatistics(double grades[], int count) {
    double average = calculateAverage(grades, count);
    double highest = findHighest(grades, count);
    double lowest = findLowest(grades, count);
    
    cout << fixed << setprecision(2);
    cout << "\n=== Statistics ===" << endl;
    cout << "Number of students: " << count << endl;
    cout << "Average grade: " << average << " (" 
         << getLetterGrade(average) << ")" << endl;
    cout << "Highest grade: " << highest << endl;
    cout << "Lowest grade: " << lowest << endl;
}

double calculateAverage(double grades[], int count) {
    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += grades[i];
    }
    return sum / count;
}

double findHighest(double grades[], int count) {
    double highest = grades[0];
    for (int i = 1; i < count; i++) {
        if (grades[i] > highest) {
            highest = grades[i];
        }
    }
    return highest;
}

double findLowest(double grades[], int count) {
    double lowest = grades[0];
    for (int i = 1; i < count; i++) {
        if (grades[i] < lowest) {
            lowest = grades[i];
        }
    }
    return lowest;
}

char getLetterGrade(double score) {
    if (score >= 90) return 'A';
    else if (score >= 80) return 'B';
    else if (score >= 70) return 'C';
    else if (score >= 60) return 'D';
    else return 'F';
}
```

### Example 4: Number Theory Functions

```cpp
#include <iostream>
#include <cmath>
using namespace std;

// Function prototypes
bool isPrime(int n);
int gcd(int a, int b);
int lcm(int a, int b);
bool isPerfect(int n);
int sumOfDivisors(int n);
void printDivisors(int n);
int countDigits(int n);
int sumOfDigits(int n);
int reverseNumber(int n);
bool isPalindrome(int n);

int main() {
    int num1, num2;
    
    cout << "Enter first number: ";
    cin >> num1;
    
    cout << "Enter second number: ";
    cin >> num2;
    
    // Prime check
    cout << "\n--- Prime Check ---" << endl;
    cout << num1 << (isPrime(num1) ? " is" : " is not") << " prime" << endl;
    cout << num2 << (isPrime(num2) ? " is" : " is not") << " prime" << endl;
    
    // GCD and LCM
    cout << "\n--- GCD and LCM ---" << endl;
    cout << "GCD(" << num1 << ", " << num2 << ") = " << gcd(num1, num2) << endl;
    cout << "LCM(" << num1 << ", " << num2 << ") = " << lcm(num1, num2) << endl;
    
    // Perfect number
    cout << "\n--- Perfect Number ---" << endl;
    cout << num1 << (isPerfect(num1) ? " is" : " is not") << " perfect" << endl;
    
    // Divisors
    cout << "\n--- Divisors of " << num1 << " ---" << endl;
    printDivisors(num1);
    cout << "Sum of divisors: " << sumOfDivisors(num1) << endl;
    
    // Digit operations
    cout << "\n--- Digit Operations for " << num1 << " ---" << endl;
    cout << "Number of digits: " << countDigits(num1) << endl;
    cout << "Sum of digits: " << sumOfDigits(num1) << endl;
    cout << "Reversed number: " << reverseNumber(num1) << endl;
    cout << num1 << (isPalindrome(num1) ? " is" : " is not") << " a palindrome" << endl;
    
    return 0;
}

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    
    return true;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcm(int a, int b) {
    return (a * b) / gcd(a, b);
}

bool isPerfect(int n) {
    return sumOfDivisors(n) == n;
}

int sumOfDivisors(int n) {
    int sum = 0;
    for (int i = 1; i <= n / 2; i++) {
        if (n % i == 0) {
            sum += i;
        }
    }
    return sum;
}

void printDivisors(int n) {
    cout << "Divisors: ";
    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            cout << i << " ";
        }
    }
    cout << endl;
}

int countDigits(int n) {
    if (n == 0) return 1;
    int count = 0;
    while (n != 0) {
        count++;
        n /= 10;
    }
    return count;
}

int sumOfDigits(int n) {
    int sum = 0;
    while (n != 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

int reverseNumber(int n) {
    int reversed = 0;
    while (n != 0) {
        reversed = reversed * 10 + (n % 10);
        n /= 10;
    }
    return reversed;
}

bool isPalindrome(int n) {
    return n == reverseNumber(n);
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Forgetting Return Statement

**Wrong**:

```cpp
int add(int a, int b) {
    int sum = a + b;
    // Forgot return!
}  // Warning: no return statement
```

**Correct**:

```cpp
int add(int a, int b) {
    int sum = a + b;
    return sum;
}
```

### Mistake 2: Return Type Mismatch

**Wrong**:

```cpp
int divide(int a, int b) {
    return a / b;  // Integer division loses decimal!
}

int main() {
    cout << divide(10, 3);  // Prints 3, not 3.333...
}
```

**Correct**:

```cpp
double divide(double a, double b) {
    return a / b;  // Returns double
}

int main() {
    cout << divide(10.0, 3.0);  // Prints 3.33333...
}
```

### Mistake 3: Not Using Function Prototype

**Wrong**:

```cpp
int main() {
    int result = add(5, 3);  // Error! add() not declared yet
    return 0;
}

int add(int a, int b) {
    return a + b;
}
```

**Correct**:

```cpp
int add(int a, int b);  // Prototype

int main() {
    int result = add(5, 3);  // Works!
    return 0;
}

int add(int a, int b) {
    return a + b;
}
```

### Mistake 4: Confusing Pass by Value and Reference

**Wrong** (expecting to modify original):

```cpp
void increment(int x) {  // Pass by value
    x++;
}

int main() {
    int num = 5;
    increment(num);
    cout << num;  // Still 5!
}
```

**Correct** (pass by reference):

```cpp
void increment(int &x) {  // Pass by reference
    x++;
}

int main() {
    int num = 5;
    increment(num);
    cout << num;  // Now 6
}
```

### Mistake 5: Infinite Recursion

**Wrong**:

```cpp
int factorial(int n) {
    return n * factorial(n - 1);  // No base case!
}  // Stack overflow!
```

**Correct**:

```cpp
int factorial(int n) {
    if (n <= 1) return 1;  // Base case
    return n * factorial(n - 1);
}
```

### Mistake 6: Scope Confusion

**Wrong**:

```cpp
void function() {
    int x = 10;
}

int main() {
    cout << x;  // Error! x doesn't exist here
}
```

**Correct**:

```cpp
int function() {
    int x = 10;
    return x;  // Return the value
}

int main() {
    int x = function();
    cout << x;  // Works!
}
```

### Mistake 7: Modifying Parameters You Didn't Mean To

**Wrong**:

```cpp
void printDouble(int &x) {  // Accidentally used reference
    cout << x * 2;
    x = x * 2;  // Modifies original!
}
```

**Correct**:

```cpp
void printDouble(int x) {  // Pass by value
    cout << x * 2;
    // Original not modified
}
```

---

## Practice Exercises

### Exercise 1: Function Library

Create a library of utility functions:

- `bool isEven(int n)` - Check if number is even
- `bool isOdd(int n)` - Check if number is odd
- `int max(int a, int b)` - Return larger of two numbers
- `int min(int a, int b)` - Return smaller of two numbers
- `int power(int base, int exp)` - Calculate base^exp
- `double average(int arr[], int size)` - Calculate array average

### Exercise 2: String Functions

Create string manipulation functions:

- `int countVowels(string s)` - Count vowels in string
- `string reverseString(string s)` - Reverse a string
- `bool isPalindrome(string s)` - Check if palindrome
- `string toUpperCase(string s)` - Convert to uppercase
- `string toLowerCase(string s)` - Convert to lowercase
- `int countWords(string s)` - Count words in string

### Exercise 3: Array Functions

Create array processing functions:

- `void fillArray(int arr[], int size)` - Fill array with user input
- `void printArray(int arr[], int size)` - Display array
- `int findMax(int arr[], int size)` - Find maximum value
- `int findMin(int arr[], int size)` - Find minimum value
- `void sortArray(int arr[], int size)` - Sort array (bubble sort)
- `int linearSearch(int arr[], int size, int target)` - Search for value

### Exercise 4: Geometric Calculator

Create functions for geometric calculations:

- `double circleArea(double radius)` - Area of circle
- `double circlePerimeter(double radius)` - Perimeter of circle
- `double rectangleArea(double length, double width)` - Rectangle area
- `double triangleArea(double base, double height)` - Triangle area
- `double sphereVolume(double radius)` - Volume of sphere
- `double cylinderVolume(double radius, double height)` - Cylinder volume

### Exercise 5: Banking System

Create a complete banking system with functions:

- `double deposit(double balance, double amount)` - Add money
- `double withdraw(double balance, double amount)` - Remove money
- `double calculateInterest(double balance, double rate)` - Calculate interest
- `void displayBalance(double balance)` - Show balance
- `bool validateAmount(double amount)` - Validate input
- Menu-driven program using all functions

### Exercise 6: Prime Number Tools

Create prime number utility functions:

- `bool isPrime(int n)` - Check if prime
- `void printPrimes(int start, int end)` - Print primes in range
- `int countPrimes(int start, int end)` - Count primes in range
- `int nextPrime(int n)` - Find next prime after n
- `void primeFactors(int n)` - Print prime factorization

### Exercise 7: Password Validator

Create password validation system:

- `bool hasMinLength(string pwd, int minLen)` - Check length
- `bool hasUpperCase(string pwd)` - Check for uppercase
- `bool hasLowerCase(string pwd)` - Check for lowercase
- `bool hasDigit(string pwd)` - Check for digit
- `bool hasSpecialChar(string pwd)` - Check for special character
- `int calculateStrength(string pwd)` - Return strength score (0-100)
- Main program that validates and rates passwords

### Exercise 8: Recursive Challenges

Implement these using recursion:

- `int sumRange(int n)` - Sum from 1 to n
- `int power(int base, int exp)` - Calculate power
- `string toBinary(int n)` - Convert to binary string
- `int gcd(int a, int b)` - Greatest common divisor
- `void printPattern(int n)` - Print pattern recursively
- `bool isPalindrome(string s)` - Check palindrome recursively

---

## Summary and Best Practices

### Key Takeaways

1. **Functions** organize code into reusable blocks
2. **Return type** specifies what function gives back
3. **Parameters** are inputs to the function
4. **Pass by value** copies data (doesn't modify original)
5. **Pass by reference** (`&`) allows modification
6. **Overloading** allows same name with different parameters
7. **Recursion** is function calling itself

### Function Design Principles

**Good function design**:

- **Single Responsibility** - One function, one job
- **Meaningful Names** - Function name describes what it does
- **Small Size** - Keep functions short and focused
- **No Side Effects** - Don't modify global state unexpectedly
- **Good Documentation** - Comment what function does

### Best Practices Checklist

✅ **Use prototypes** - Declare before main()  
✅ **Meaningful names** - `calculateAverage()` not `func1()`  
✅ **Consistent style** - Pick naming convention and stick to it  
✅ **Return appropriate types** - Match return type to what you're returning  
✅ **Validate inputs** - Check for invalid data  
✅ **Use pass by reference** - For large data or when modifying  
✅ **Avoid global variables** - Pass data through parameters  
✅ **Document functions** - Comment what they do  
✅ **Test thoroughly** - Test with various inputs  
✅ **Keep it simple** - Don't overcomplicate

### Quick Reference

**Function template**:

```cpp
// Prototype
returnType functionName(parameterTypes);

// Definition
returnType functionName(parameters) {
    // Body
    return value;
}

// Call
result = functionName(arguments);
```

**Common patterns**:

```cpp
// No parameters, no return
void printMessage();

// Parameters, no return
void display(int x, int y);

// No parameters, returns value
int getRandomNumber();

// Parameters, returns value
int add(int a, int b);

// Pass by reference
void swap(int &a, int &b);

// Default parameters
void print(string msg = "Hello");

// Overloading
int max(int a, int b);
double max(double a, double b);

// Recursion
int factorial(int n);
```

---

## Conclusion

Functions are fundamental to programming. They allow you to:

- **Write reusable code** - Don't repeat yourself
- **Organize programs** - Break into manageable pieces
- **Abstract complexity** - Hide implementation details
- **Test easily** - Test individual functions
- **Collaborate** - Team members work on different functions

**Mastering functions enables you to**:

- Build complex programs from simple building blocks
- Create clean, maintainable code
- Work efficiently on large projects
- Implement algorithms and data structures
- Develop professional-quality software

**Next steps**:

1. Practice all the exercises
2. Create your own function library
3. Build complete programs using functions
4. Move on to learning **Arrays** (collections of data)

With functions mastered, you can now write organized, professional programs!

**Happy Coding!** 🚀