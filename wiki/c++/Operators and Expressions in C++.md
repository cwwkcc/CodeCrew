## Introduction

Now that you know how to get input and display output, it's time to learn how to **DO something** with that data! Operators are the symbols that tell the computer to perform specific operations on values. Think of them as the "verbs" in your programming language.

**Real-World Analogy**:

- Variables are like containers holding ingredients
- Operators are like kitchen actions (mix, heat, cut)
- Expressions are like recipes combining ingredients and actions

---

## What is an Expression?

An **expression** is a combination of values, variables, and operators that produces a result.

**Examples**:

```cpp
5 + 3           // Expression that evaluates to 8
age * 2         // Expression using a variable
(a + b) / 2     // Expression with multiple operations
x = 10          // Assignment expression
```

**Every expression has**:

1. **Operands**: The values being operated on (5, 3, age, etc.)
2. **Operators**: The symbols that perform operations (+, -, *, etc.)
3. **Result**: The value produced by the expression

---

## Types of Operators in C++

C++ has several categories of operators:

1. **Arithmetic Operators** - Math operations
2. **Assignment Operators** - Assigning values
3. **Comparison Operators** - Comparing values
4. **Logical Operators** - Boolean logic
5. **Increment/Decrement Operators** - Adding/subtracting 1
6. **Bitwise Operators** - Bit-level operations
7. **Other Operators** - Miscellaneous operations

Let's explore each category in detail!

---

## 1. Arithmetic Operators

These operators perform mathematical calculations.

### Addition (+)

**Adds two numbers together**:

```cpp
#include <iostream>
using namespace std;

int main() {
    int a = 10;
    int b = 5;
    int sum = a + b;
    
    cout << "Sum: " << sum << endl;  // Output: 15
    
    // Can also use directly in output
    cout << "10 + 5 = " << (10 + 5) << endl;  // Output: 15
    
    return 0;
}
```

**Use cases**:

- Adding numbers
- Accumulating totals
- String concatenation (with strings)

```cpp
// Adding doubles
double price1 = 19.99;
double price2 = 29.99;
double total = price1 + price2;  // 49.98

// String concatenation
string firstName = "John";
string lastName = "Doe";
string fullName = firstName + " " + lastName;  // "John Doe"
```

### Subtraction (-)

**Subtracts the second number from the first**:

```cpp
int a = 20;
int b = 8;
int difference = a - b;  // 12

cout << "Difference: " << difference << endl;

// Can be used with negative numbers
int temperature = 5;
int drop = -10;
int newTemp = temperature - drop;  // 5 - (-10) = 15
```

**Use cases**:

- Finding differences
- Calculating change
- Countdown operations
- Temperature changes

### Multiplication (*)

**Multiplies two numbers**:

```cpp
int length = 5;
int width = 3;
int area = length * width;  // 15

cout << "Area: " << area << endl;

// Works with decimals
double price = 19.99;
int quantity = 3;
double total = price * quantity;  // 59.97
```

**Use cases**:

- Area/volume calculations
- Scaling values
- Price × quantity
- Converting units

### Division (/)

**Divides the first number by the second**:

```cpp
int a = 20;
int b = 4;
int quotient = a / b;  // 5

cout << "Quotient: " << quotient << endl;
```

**⚠️ IMPORTANT: Integer Division**

When dividing two integers, the result is also an integer (decimal part is discarded):

```cpp
int a = 10;
int b = 3;
int result = a / b;  // Result is 3, not 3.333...

cout << result << endl;  // Output: 3

// The decimal part is LOST, not rounded!
int x = 7;
int y = 2;
cout << x / y << endl;  // Output: 3 (not 3.5)
```

**Solution: Use double for accurate division**:

```cpp
double a = 10.0;
double b = 3.0;
double result = a / b;  // 3.33333...

cout << result << endl;  // Output: 3.33333

// Or cast integers to double
int x = 10;
int y = 3;
double result = (double)x / y;  // 3.33333
// or
double result = static_cast<double>(x) / y;
```

**Division by zero**:

```cpp
int a = 10;
int b = 0;
// int result = a / b;  // ⚠️ RUNTIME ERROR! Program crashes!

// Always check before dividing
if (b != 0) {
    int result = a / b;
} else {
    cout << "Error: Cannot divide by zero!" << endl;
}
```

### Modulus (%)

**Returns the remainder of division** (works only with integers):

```cpp
int a = 10;
int b = 3;
int remainder = a % b;  // 1 (because 10 ÷ 3 = 3 remainder 1)

cout << "Remainder: " << remainder << endl;
```

**How it works**:

```
10 % 3:  10 ÷ 3 = 3 remainder 1  → Result: 1
17 % 5:  17 ÷ 5 = 3 remainder 2  → Result: 2
20 % 4:  20 ÷ 4 = 5 remainder 0  → Result: 0
7 % 10:   7 ÷ 10 = 0 remainder 7 → Result: 7
```

**Common uses**:

**1. Check if number is even or odd**:

```cpp
int number = 7;

if (number % 2 == 0) {
    cout << "Even" << endl;
} else {
    cout << "Odd" << endl;
}
```

**2. Get last digit of a number**:

```cpp
int number = 12345;
int lastDigit = number % 10;  // 5
```

**3. Cycle through values**:

```cpp
// Keep values in range 0-6 (days of week)
int day = 10;
int dayOfWeek = day % 7;  // 3
```

**4. Check divisibility**:

```cpp
int number = 15;

if (number % 3 == 0) {
    cout << "Divisible by 3" << endl;
}
if (number % 5 == 0) {
    cout << "Divisible by 5" << endl;
}
```

### Operator Precedence (Order of Operations)

**Just like in math, operations are performed in a specific order**:

**PEMDAS/BODMAS**:

1. **P**arentheses / **B**rackets
2. **E**xponents (not covered here) / **O**rders
3. **M**ultiplication and **D**ivision (left to right)
4. **A**ddition and **S**ubtraction (left to right)

```cpp
int result;

result = 5 + 3 * 2;     // 11 (not 16!)
// Calculation: 3 * 2 = 6, then 5 + 6 = 11

result = (5 + 3) * 2;   // 16
// Calculation: 5 + 3 = 8, then 8 * 2 = 16

result = 10 - 4 / 2;    // 8 (not 3!)
// Calculation: 4 / 2 = 2, then 10 - 2 = 8

result = (10 - 4) / 2;  // 3
// Calculation: 10 - 4 = 6, then 6 / 2 = 3
```

**Best practice**: Use parentheses to make your intentions clear!

```cpp
// Less clear
int result = a + b * c - d / e;

// More clear
int result = a + (b * c) - (d / e);
```

### Complete Arithmetic Example

```cpp
#include <iostream>
using namespace std;

int main() {
    double num1, num2;
    
    cout << "Enter first number: ";
    cin >> num1;
    
    cout << "Enter second number: ";
    cin >> num2;
    
    // Perform all operations
    cout << "\n=== Results ===" << endl;
    cout << "Addition: " << num1 + num2 << endl;
    cout << "Subtraction: " << num1 - num2 << endl;
    cout << "Multiplication: " << num1 * num2 << endl;
    
    if (num2 != 0) {
        cout << "Division: " << num1 / num2 << endl;
    } else {
        cout << "Division: Cannot divide by zero!" << endl;
    }
    
    // Modulus only works with integers
    int int1 = (int)num1;
    int int2 = (int)num2;
    if (int2 != 0) {
        cout << "Modulus: " << int1 % int2 << endl;
    }
    
    return 0;
}
```

---

## 2. Assignment Operators

These operators assign values to variables.

### Simple Assignment (=)

**Assigns a value to a variable**:

```cpp
int age = 25;           // Assign 25 to age
double price = 19.99;   // Assign 19.99 to price
char grade = 'A';       // Assign 'A' to grade

// Can assign result of expression
int sum = 10 + 5;       // sum = 15
int product = 3 * 4;    // product = 12

// Can assign value of another variable
int x = 10;
int y = x;              // y = 10
```

**⚠️ Common mistake**: Confusing `=` (assignment) with `==` (comparison)

```cpp
int x = 5;      // Correct: Assigns 5 to x
// if (x = 5)   // Wrong! This ASSIGNS 5 to x (always true)
if (x == 5)     // Correct: Compares x with 5
```

### Compound Assignment Operators

**Shorthand for common operations**:

#### Addition Assignment (+=)

```cpp
int score = 100;
score = score + 10;  // Traditional way
score += 10;         // Shorthand (same result)

// Both add 10 to score
```

**More examples**:

```cpp
int total = 50;
total += 25;      // total = 75
total += 100;     // total = 175

string message = "Hello";
message += " World";  // message = "Hello World"
```

#### Subtraction Assignment (-=)

```cpp
int health = 100;
health -= 20;     // health = 80
health -= 30;     // health = 50

// Same as: health = health - 20
```

#### Multiplication Assignment (*=)

```cpp
int value = 5;
value *= 3;       // value = 15

// Same as: value = value * 3

double price = 10.0;
price *= 1.5;     // price = 15.0 (50% increase)
```

#### Division Assignment (/=)

```cpp
int total = 100;
total /= 4;       // total = 25

// Same as: total = total / 4

double amount = 50.0;
amount /= 2;      // amount = 25.0
```

#### Modulus Assignment (%=)

```cpp
int number = 17;
number %= 5;      // number = 2 (17 % 5 = 2)

// Same as: number = number % 5
```

### Comparison of Assignment Styles

```cpp
// Traditional style
int x = 10;
x = x + 5;
x = x * 2;
x = x - 3;

// Compound assignment style (shorter)
int x = 10;
x += 5;
x *= 2;
x -= 3;

// Both give same result!
```

### Complete Assignment Example

```cpp
#include <iostream>
using namespace std;

int main() {
    int score = 100;
    
    cout << "Initial score: " << score << endl;
    
    // Gain points
    score += 50;
    cout << "After bonus: " << score << endl;  // 150
    
    // Lose points
    score -= 20;
    cout << "After penalty: " << score << endl;  // 130
    
    // Double the score
    score *= 2;
    cout << "After doubling: " << score << endl;  // 260
    
    // Divide by 4
    score /= 4;
    cout << "After dividing: " << score << endl;  // 65
    
    return 0;
}
```

---

## 3. Increment and Decrement Operators

**Special operators to add or subtract 1** (very common in programming).

### Increment (++)

**Adds 1 to a variable**:

```cpp
int count = 5;
count++;        // count is now 6
// Same as: count = count + 1;
// Same as: count += 1;
```

### Decrement (--)

**Subtracts 1 from a variable**:

```cpp
int lives = 3;
lives--;        // lives is now 2
// Same as: lives = lives - 1;
// Same as: lives -= 1;
```

### Prefix vs Postfix

**There are TWO ways to use ++ and --**:

#### Postfix (variable++)

**Use the value FIRST, then increment**:

```cpp
int x = 5;
int y = x++;    // y = 5, then x becomes 6

cout << "x: " << x << endl;  // 6
cout << "y: " << y << endl;  // 5
```

**What happens**:

1. Assign current value of x to y (5)
2. Then increment x to 6

#### Prefix (++variable)

**Increment FIRST, then use the value**:

```cpp
int x = 5;
int y = ++x;    // x becomes 6, then y = 6

cout << "x: " << x << endl;  // 6
cout << "y: " << y << endl;  // 6
```

**What happens**:

1. Increment x to 6
2. Then assign new value to y (6)

### Visual Comparison

```cpp
// Postfix example
int a = 10;
int b = a++;
// a is now 11, b is 10

// Prefix example
int x = 10;
int y = ++x;
// x is now 11, y is 11
```

**When used alone (not in assignment), both are the same**:

```cpp
int count = 0;
count++;    // count = 1
++count;    // count = 2
// Both do the same thing when used alone!
```

### Practical Examples

```cpp
// Counting up
int level = 1;
level++;        // Level up! level = 2
level++;        // level = 3

// Counting down
int lives = 3;
lives--;        // Lost a life! lives = 2
lives--;        // lives = 1

// Loop counter (you'll use this a lot!)
for (int i = 0; i < 10; i++) {  // i++ increments after each loop
    cout << i << " ";
}
// Output: 0 1 2 3 4 5 6 7 8 9
```

### Common Use Cases

```cpp
// 1. Counters
int itemCount = 0;
itemCount++;  // Add an item

// 2. Lives in a game
int playerLives = 3;
playerLives--;  // Player got hit

// 3. Array indexing
int index = 0;
cout << array[index++];  // Use current index, then move to next

// 4. Loops
int i = 0;
while (i < 10) {
    cout << i << endl;
    i++;  // Move to next number
}
```

---

## 4. Comparison Operators

**Compare two values and return true or false** (boolean result).

### Equal to (==)

**Checks if two values are equal**:

```cpp
int a = 5;
int b = 5;
bool result = (a == b);  // true

cout << (10 == 10) << endl;  // true (1)
cout << (5 == 3) << endl;    // false (0)

if (age == 18) {
    cout << "You are exactly 18!" << endl;
}
```

**⚠️ Don't confuse with =**:

```cpp
int x = 5;    // Assignment (gives x the value 5)
x == 5;       // Comparison (checks if x equals 5)
```

### Not equal to (!=)

**Checks if two values are NOT equal**:

```cpp
int a = 5;
int b = 3;
bool result = (a != b);  // true

if (password != "1234") {
    cout << "Wrong password!" << endl;
}

if (age != 0) {
    cout << "Valid age" << endl;
}
```

### Greater than (>)

**Checks if left value is greater than right**:

```cpp
cout << (10 > 5) << endl;   // true
cout << (3 > 8) << endl;    // false
cout << (5 > 5) << endl;    // false (not greater, equal)

if (score > 100) {
    cout << "Excellent!" << endl;
}

if (age > 18) {
    cout << "Adult" << endl;
}
```

### Less than (<)

**Checks if left value is less than right**:

```cpp
cout << (3 < 5) << endl;    // true
cout << (10 < 2) << endl;   // false
cout << (5 < 5) << endl;    // false

if (temperature < 0) {
    cout << "Freezing!" << endl;
}

if (score < 60) {
    cout << "Failed" << endl;
}
```

### Greater than or equal to (>=)

**Checks if left value is greater than OR equal to right**:

```cpp
cout << (10 >= 5) << endl;   // true
cout << (5 >= 5) << endl;    // true (equal counts!)
cout << (3 >= 8) << endl;    // false

if (age >= 18) {
    cout << "Can vote" << endl;
}

if (score >= 90) {
    cout << "Grade: A" << endl;
}
```

### Less than or equal to (<=)

**Checks if left value is less than OR equal to right**:

```cpp
cout << (3 <= 5) << endl;    // true
cout << (5 <= 5) << endl;    // true
cout << (10 <= 2) << endl;   // false

if (age <= 12) {
    cout << "Child ticket" << endl;
}

if (score <= 59) {
    cout << "Failed" << endl;
}
```

### Comparison Examples

```cpp
#include <iostream>
using namespace std;

int main() {
    int score;
    
    cout << "Enter your score (0-100): ";
    cin >> score;
    
    // Determine grade using comparisons
    if (score >= 90) {
        cout << "Grade: A" << endl;
    } else if (score >= 80) {
        cout << "Grade: B" << endl;
    } else if (score >= 70) {
        cout << "Grade: C" << endl;
    } else if (score >= 60) {
        cout << "Grade: D" << endl;
    } else {
        cout << "Grade: F" << endl;
    }
    
    // Check pass/fail
    if (score >= 60) {
        cout << "Status: PASS" << endl;
    } else {
        cout << "Status: FAIL" << endl;
    }
    
    return 0;
}
```

### Comparison Operator Summary

|Operator|Meaning|Example|Result (if a=5, b=3)|
|---|---|---|---|
|`==`|Equal to|`a == b`|false|
|`!=`|Not equal to|`a != b`|true|
|`>`|Greater than|`a > b`|true|
|`<`|Less than|`a < b`|false|
|`>=`|Greater or equal|`a >= b`|true|
|`<=`|Less or equal|`a <= b`|false|

---

## 5. Logical Operators

**Combine multiple conditions** (work with boolean values).

### AND Operator (&&)

**Returns true only if BOTH conditions are true**:

```cpp
// Truth table for AND
true  && true   = true
true  && false  = false
false && true   = false
false && false  = false
```

**Examples**:

```cpp
int age = 25;
bool hasLicense = true;

// Can drive if age >= 18 AND has license
if (age >= 18 && hasLicense) {
    cout << "Can drive" << endl;
}

// Check if number is in range
int score = 85;
if (score >= 0 && score <= 100) {
    cout << "Valid score" << endl;
}

// Multiple conditions
if (age >= 18 && age <= 65 && hasLicense) {
    cout << "Eligible for standard insurance" << endl;
}
```

### OR Operator (||)

**Returns true if AT LEAST ONE condition is true**:

```cpp
// Truth table for OR
true  || true   = true
true  || false  = true
false || true   = true
false || false  = false
```

**Examples**:

```cpp
int day = 6;  // 0=Monday, 6=Sunday

// Weekend if Saturday (5) OR Sunday (6)
if (day == 5 || day == 6) {
    cout << "It's the weekend!" << endl;
}

// Discount for children or seniors
int age = 70;
if (age < 12 || age >= 65) {
    cout << "Eligible for discount" << endl;
}

// Check for valid input
char answer;
cin >> answer;
if (answer == 'Y' || answer == 'y') {
    cout << "You said yes" << endl;
}
```

### NOT Operator (!)

**Reverses the boolean value** (true becomes false, false becomes true):

```cpp
// Truth table for NOT
!true  = false
!false = true
```

**Examples**:

```cpp
bool isRaining = false;

if (!isRaining) {
    cout << "Go outside!" << endl;
}

bool gameOver = false;
if (!gameOver) {
    cout << "Keep playing!" << endl;
}

// Double negative (!! returns to original)
bool value = true;
cout << !!value << endl;  // true
```

### Combining Logical Operators

```cpp
int age = 25;
bool hasTicket = true;
bool hasID = true;

// AND + OR
if ((age >= 18 && hasID) || hasTicket) {
    cout << "Can enter" << endl;
}

// Complex condition
int score = 85;
bool attendedClass = true;
bool submittedAssignment = true;

if (score >= 60 && (attendedClass || submittedAssignment)) {
    cout << "Passed the course" << endl;
}

// Using NOT
bool isWeekend = false;
bool isHoliday = false;

if (!isWeekend && !isHoliday) {
    cout << "It's a work day" << endl;
}
```

### Short-Circuit Evaluation

**Important**: C++ uses short-circuit evaluation for && and ||:

**For AND (&&)**:

- If left side is false, right side is NOT evaluated
- (because false && anything = false)

```cpp
int x = 0;
if (x != 0 && 10 / x > 2) {  // Second part never runs!
    // This prevents division by zero
}
```

**For OR (||)**:

- If left side is true, right side is NOT evaluated
- (because true || anything = true)

```cpp
bool found = true;
if (found || searchArray()) {  // searchArray() never runs!
    // Already found, no need to search
}
```

### Practical Example: Login System

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string username, password;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    
    const string CORRECT_USER = "admin";
    const string CORRECT_PASS = "password123";
    
    while (attempts < MAX_ATTEMPTS) {
        cout << "Username: ";
        cin >> username;
        
        cout << "Password: ";
        cin >> password;
        
        // Check if BOTH username AND password are correct
        if (username == CORRECT_USER && password == CORRECT_PASS) {
            cout << "Login successful!" << endl;
            break;
        } else {
            attempts++;
            cout << "Login failed! ";
            
            if (attempts < MAX_ATTEMPTS) {
                cout << "Attempts remaining: " << (MAX_ATTEMPTS - attempts) << endl;
            } else {
                cout << "Account locked!" << endl;
            }
        }
    }
    
    return 0;
}
```

### Logical Operators Summary

|Operator|Name|Description|Example|
|---|---|---|---|
|`&&`|AND|True if both are true|`(a > 5 && b < 10)`|
|`||`|OR|
|`!`|NOT|Reverses boolean value|`!isGameOver`|

---

## 6. Other Important Operators

### Ternary Operator (? :)

**Shorthand for if-else** (conditional operator):

**Syntax**:

```cpp
condition ? value_if_true : value_if_false
```

**Examples**:

```cpp
int age = 20;
string status = (age >= 18) ? "Adult" : "Minor";
// If age >= 18, status = "Adult", otherwise "Minor"

// Traditional if-else equivalent:
string status;
if (age >= 18) {
    status = "Adult";
} else {
    status = "Minor";
}

// More examples
int a = 10, b = 20;
int max = (a > b) ? a : b;  // max = 20

int score = 85;
char grade = (score >= 90) ? 'A' : 
             (score >= 80) ? 'B' : 
             (score >= 70) ? 'C' : 'F';

// Use in output
cout << "You are " << (age >= 18 ? "eligible" : "not eligible") << " to vote" << endl;
```

### Comma Operator (,)

**Evaluates multiple expressions, returns the last one**:

```cpp
int a, b, c;
a = (b = 5, c = 10, b + c);  // a = 15

// Rarely used, mainly in for loops
for (int i = 0, j = 10; i < j; i++, j--) {
    cout << i << " " << j << endl;
}
```

### Sizeof Operator

**Returns the size (in bytes) of a data type or variable**:

```cpp
cout << sizeof(int) << endl;        // Usually 4
cout << sizeof(double) << endl;     // Usually 8
cout << sizeof(char) << endl;       // 1

int arr[10];
cout << sizeof(arr) << endl;        // 40 (10 * 4)

int x = 100;
cout << sizeof(x) << endl;          // 4
```

### Type Cast Operators

**Convert from one data type to another**:

```cpp
// C-style cast
int a = 10;
int b = 3;
double result = (double)a / b;  // 3.33333

// C++ style casts (recommended)
double result = static_cast<double>(a) / b;

// Other examples
double pi = 3.14159;
int truncated = static_cast<int>(pi);  // 3

char ch = 'A';
int ascii = static_cast<int>(ch);  // 65
```

---

## Operator Precedence Table

**Order of evaluation** (from highest to lowest priority):

|Priority|Operator|Description|
|---|---|---|
|1|`()`|Parentheses|
|2|`++`, `--`|Increment/Decrement (postfix)|
|3|`++`, `--`, `!`|Increment/Decrement (prefix), NOT|
|4|`*`, `/`, `%`|Multiplication, Division, Modulus|
|5|`+`, `-`|Addition, Subtraction|
|6|`<`, `<=`, `>`, `>=`|Comparison|
|7|`==`, `!=`|Equality|
|8|`&&`|Logical AND|
|9|`||
|10|`?:`|Ternary|
|11|`=`, `+=`, `-=`, etc.|Assignment|

**When in doubt, use parentheses!**

```cpp
// Confusing
int result = a + b * c > d && e || f;

// Clear
int result = ((a + (b * c)) > d) && (e || f);
```

---

## Practical Examples and Projects

### Example 1: BMI Calculator

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    double weight, height, bmi;
    
    cout << "=== BMI Calculator ===" << endl;
    cout << "Enter weight (kg): ";
    cin >> weight;
    
    cout << "Enter height (m): ";
    cin >> height;
    
    // Calculate BMI
    bmi = weight / (height * height);
    
    cout << fixed << setprecision(2);
    cout << "\nYour BMI: " << bmi << endl;
    
    // Determine category
    string category;
    if (bmi < 18.5) {
        category = "Underweight";
    } else if (bmi < 25) {
        category = "Normal weight";
    } else if (bmi < 30) {
        category = "Overweight";
    } else {
        category = "Obese";
    }
    
    cout << "Category: " << category << endl;
    
    return 0;
}
```

### Example 2: Grade Calculator with Multiple Tests

```cpp
#include <iostream>
using namespace std;

int main() {
    double test1, test2, test3, average;
    
    cout << "=== Grade Calculator ===" << endl;
    
    cout << "Enter Test 1 score: ";
    cin >> test1;
    
    cout << "Enter Test 2 score: ";
    cin >> test2;
    
    cout << "Enter Test 3 score: ";
    cin >> test3;
    
    // Calculate average
    average = (test1 + test2 + test3) / 3.0;
    
    cout << "\nAverage: " << average << endl;
    
    // Determine letter grade using ternary operator
    char grade = (average >= 90) ? 'A' :
                 (average >= 80) ? 'B' :
                 (average >= 70) ? 'C' :
                 (average >= 60) ? 'D' : 'F';
    
    cout << "Letter Grade: " << grade << endl;
    
    // Pass/Fail status
    string status = (average >= 60) ? "PASS" : "FAIL";
    cout << "Status: " << status << endl;
    
    // Additional feedback
    if (grade == 'A' && average >= 95) {
        cout << "Excellent work! Outstanding!" << endl;
    } else if (grade == 'F' || average < 50) {
        cout << "Please see instructor for help." << endl;
    }
    
    return 0;
}
```

### Example 3: Shopping Cart Total

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    double price1, price2, price3;
    int quantity1, quantity2, quantity3;
    double subtotal, tax, discount = 0, total;
    
    const double TAX_RATE = 0.08;  // 8% tax
    const double DISCOUNT_THRESHOLD = 100.0;
    const double DISCOUNT_RATE = 0.10;  // 10% discount
    
    cout << "=== Shopping Cart ===" << endl;
    
    // Item 1
    cout << "Item 1 price: $";
    cin >> price1;
    cout << "Quantity: ";
    cin >> quantity1;
    
    // Item 2
    cout << "Item 2 price: $";
    cin >> price2;
    cout << "Quantity: ";
    cin >> quantity2;
    
    // Item 3
    cout << "Item 3 price: $";
    cin >> price3;
    cout << "Quantity: ";
    cin >> quantity3;
    
    // Calculate subtotal
    subtotal = (price1 * quantity1) + 
               (price2 * quantity2) + 
               (price3 * quantity3);
    
    // Apply discount if subtotal >= $100
    if (subtotal >= DISCOUNT_THRESHOLD) {
        discount = subtotal * DISCOUNT_RATE;
        cout << "\nDiscount applied! (10% off orders over $100)" << endl;
    }
    
    // Calculate tax and total
    double afterDiscount = subtotal - discount;
    tax = afterDiscount * TAX_RATE;
    total = afterDiscount + tax;
    
    // Display receipt
    cout << fixed << setprecision(2);
    cout << "\n=== Receipt ===" << endl;
    cout << "Subtotal:  $" << subtotal << endl;
    
    if (discount > 0) {
        cout << "Discount:  -$" << discount << endl;
        cout << "After Discount: $" << afterDiscount << endl;
    }
    
    cout << "Tax (8%):  $" << tax << endl;
    cout << "Total:     $" << total << endl;
    
    return 0;
}
```

### Example 4: Leap Year Checker

```cpp
#include <iostream>
using namespace std;

int main() {
    int year;
    bool isLeapYear;
    
    cout << "Enter a year: ";
    cin >> year;
    
    // Leap year rules:
    // 1. Divisible by 4
    // 2. If divisible by 100, must also be divisible by 400
    
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            // Century year - must be divisible by 400
            isLeapYear = (year % 400 == 0);
        } else {
            // Not a century year - is leap year
            isLeapYear = true;
        }
    } else {
        // Not divisible by 4 - not a leap year
        isLeapYear = false;
    }
    
    // Alternative: One-line version
    // isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    
    if (isLeapYear) {
        cout << year << " is a leap year!" << endl;
        cout << "February has 29 days." << endl;
    } else {
        cout << year << " is NOT a leap year." << endl;
        cout << "February has 28 days." << endl;
    }
    
    return 0;
}
```

### Example 5: Simple Interest vs Compound Interest

```cpp
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

int main() {
    double principal, rate, time;
    double simpleInterest, simpleTotal;
    double compoundInterest, compoundTotal;
    
    cout << "=== Interest Calculator ===" << endl;
    
    cout << "Enter principal amount: $";
    cin >> principal;
    
    cout << "Enter annual interest rate (%): ";
    cin >> rate;
    
    cout << "Enter time period (years): ";
    cin >> time;
    
    // Convert percentage to decimal
    rate = rate / 100.0;
    
    // Calculate Simple Interest
    // Formula: SI = P * R * T
    simpleInterest = principal * rate * time;
    simpleTotal = principal + simpleInterest;
    
    // Calculate Compound Interest (compounded annually)
    // Formula: A = P(1 + R)^T
    compoundTotal = principal * pow(1 + rate, time);
    compoundInterest = compoundTotal - principal;
    
    // Display results
    cout << fixed << setprecision(2);
    cout << "\n=== Simple Interest ===" << endl;
    cout << "Interest: $" << simpleInterest << endl;
    cout << "Total Amount: $" << simpleTotal << endl;
    
    cout << "\n=== Compound Interest ===" << endl;
    cout << "Interest: $" << compoundInterest << endl;
    cout << "Total Amount: $" << compoundTotal << endl;
    
    cout << "\nDifference: $" << (compoundInterest - simpleInterest) << endl;
    
    return 0;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Integer Division

**Problem**:

```cpp
int result = 5 / 2;  // Result is 2, not 2.5!
```

**Solution**:

```cpp
double result = 5.0 / 2.0;  // Result is 2.5
// or
double result = static_cast<double>(5) / 2;
```

### Mistake 2: Assignment vs Comparison

**Problem**:

```cpp
if (x = 5) {  // This ASSIGNS 5 to x, doesn't compare!
    // Always executes
}
```

**Solution**:

```cpp
if (x == 5) {  // Correct comparison
    // Only executes if x equals 5
}
```

### Mistake 3: Division by Zero

**Problem**:

```cpp
int result = 10 / 0;  // Runtime error!
```

**Solution**:

```cpp
if (divisor != 0) {
    int result = 10 / divisor;
} else {
    cout << "Error: Cannot divide by zero!" << endl;
}
```

### Mistake 4: Modulus with Doubles

**Problem**:

```cpp
double result = 5.5 % 2.3;  // Error! % only works with integers
```

**Solution**:

```cpp
// Use fmod for doubles
#include <cmath>
double result = fmod(5.5, 2.3);
```

### Mistake 5: Wrong Operator Precedence

**Problem**:

```cpp
int result = 5 + 3 * 2;  // Might expect 16, but get 11
```

**Solution**:

```cpp
int result = (5 + 3) * 2;  // Use parentheses for clarity: 16
```

### Mistake 6: Logical Operator Confusion

**Problem**:

```cpp
if (age >= 18 && <= 65) {  // Syntax error!
}
```

**Solution**:

```cpp
if (age >= 18 && age <= 65) {  // Must specify variable twice
}
```

---

## Practice Exercises

### Exercise 1: Temperature Converter

Create a program that:

- Asks if converting Celsius to Fahrenheit or vice versa
- Takes temperature input
- Uses the formulas:
    - F = (C × 9/5) + 32
    - C = (F - 32) × 5/9
- Displays the result

### Exercise 2: Electricity Bill Calculator

Create a program that:

- Takes number of units consumed
- Calculates bill using:
    - First 100 units: $0.50 per unit
    - Next 100 units: $0.75 per unit
    - Above 200 units: $1.20 per unit
- Adds 5% tax
- Displays total bill

### Exercise 3: Quadratic Equation Solver

Create a program that:

- Takes coefficients a, b, c
- Calculates discriminant (b² - 4ac)
- Determines if equation has:
    - Two real roots
    - One real root
    - No real roots
- Displays the roots if they exist

### Exercise 4: Time Calculator

Create a program that:

- Takes time in seconds
- Converts to hours, minutes, and seconds
- Uses division and modulus operators
- Example: 3665 seconds = 1 hour, 1 minute, 5 seconds

### Exercise 5: Discount Calculator

Create a program that:

- Takes original price
- Applies discounts based on price:
    - $0-$50: No discount
    - $51-$100: 5% off
    - $101-$200: 10% off
    - Above $200: 15% off
- Calculates final price
- Shows amount saved

---

## Summary and Best Practices

### Key Takeaways

1. **Arithmetic operators** (+, -, *, /, %) perform math operations
2. **Assignment operators** (=, +=, -=, etc.) store values in variables
3. **Comparison operators** (==, !=, <, >, etc.) compare values
4. **Logical operators** (&&, ||, !) combine conditions
5. **Increment/Decrement** (++, --) add/subtract 1
6. **Operator precedence** determines order of operations

### Best Practices Checklist

✅ **Use parentheses** for clarity in complex expressions  
✅ **Check for division by zero** before dividing  
✅ **Use double for accurate division** when decimals matter  
✅ **Don't confuse = with ==** (assignment vs comparison)  
✅ **Use compound operators** (+=, -=) for cleaner code  
✅ **Validate input** before using in calculations  
✅ **Use meaningful variable names** in expressions  
✅ **Comment complex expressions** to explain logic

### Quick Reference

**Arithmetic**:

```cpp
a + b   // Addition
a - b   // Subtraction
a * b   // Multiplication
a / b   // Division
a % b   // Modulus (remainder)
```

**Comparison**:

```cpp
a == b  // Equal
a != b  // Not equal
a > b   // Greater than
a < b   // Less than
a >= b  // Greater or equal
a <= b  // Less or equal
```

**Logical**:

```cpp
a && b  // AND (both true)
a || b  // OR (at least one true)
!a      // NOT (opposite)
```

**Shortcuts**:

```cpp
a += 5  // Same as: a = a + 5
a++     // Same as: a = a + 1
a--     // Same as: a = a - 1
```

---

## Conclusion

Operators are the foundation of programming logic. They allow you to:

- Perform calculations
- Make comparisons
- Combine conditions
- Modify values efficiently

**Mastering operators enables you to**:

- Write mathematical programs
- Create decision-making logic
- Solve real-world problems
- Build interactive applications

**Next steps**:

1. Practice with the exercises above
2. Experiment with different operator combinations
3. Build small calculator programs
4. Move on to learning Conditional Statements (if/else)

With operators mastered, you're ready to make your programs truly interactive and intelligent!

**Happy Coding!** 🚀