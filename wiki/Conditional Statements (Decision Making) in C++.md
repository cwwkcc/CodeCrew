## Introduction

Programs need to make decisions based on different conditions - just like we do in real life. Should I bring an umbrella? _If_ it's raining. Can I drive? _If_ I'm 18 or older.

**Conditional statements** allow your programs to execute different code based on whether conditions are true or false. They're the foundation of intelligent, responsive programs.

**Real-World Analogy**:

- **Traffic Light**: If red → stop, If yellow → slow down, If green → go
- **Thermostat**: If temperature < 68°F → turn on heat, else → turn off heat
- **ATM**: If PIN is correct → allow transaction, else → deny access

---

## Why Conditional Statements Matter

Without conditional statements, programs would:

- Always do the same thing (boring!)
- Can't respond to user input
- Can't handle different situations
- Can't make decisions

**With conditional statements, you can**:

- Create interactive programs
- Validate user input
- Handle errors gracefully
- Build games with rules
- Create smart applications

---

## Types of Conditional Statements in C++

1. **if statement** - Execute code if condition is true
2. **if-else statement** - Choose between two options
3. **else-if ladder** - Choose from multiple options
4. **Nested if statements** - Conditions within conditions
5. **switch-case statement** - Multiple specific values
6. **Ternary operator** - Shorthand conditional

Let's explore each in detail!

---

## 1. The if Statement

### Basic Syntax

```cpp
if (condition) {
    // Code to execute if condition is true
}
```

**How it works**:

1. Evaluate the condition
2. If true → execute the code block
3. If false → skip the code block
4. Continue with rest of program

### Simple Examples

**Example 1: Check if number is positive**

```cpp
#include <iostream>
using namespace std;

int main() {
    int number;
    
    cout << "Enter a number: ";
    cin >> number;
    
    if (number > 0) {
        cout << "The number is positive!" << endl;
    }
    
    cout << "Program ends." << endl;
    
    return 0;
}
```

**Execution Flow**:

```
If user enters 5:
  5 > 0 is true → "The number is positive!" is printed
  "Program ends." is printed

If user enters -3:
  -3 > 0 is false → "The number is positive!" is skipped
  "Program ends." is printed
```

**Example 2: Check voting eligibility**

```cpp
int age;

cout << "Enter your age: ";
cin >> age;

if (age >= 18) {
    cout << "You are eligible to vote!" << endl;
}
```

**Example 3: Check if passing grade**

```cpp
int score;

cout << "Enter your score: ";
cin >> score;

if (score >= 60) {
    cout << "Congratulations! You passed!" << endl;
}
```

### Multiple Statements in if Block

```cpp
int temperature;

cout << "Enter temperature (°F): ";
cin >> temperature;

if (temperature > 90) {
    cout << "It's very hot!" << endl;
    cout << "Drink plenty of water." << endl;
    cout << "Stay in shade." << endl;
}
```

### if Statement Without Braces (Not Recommended)

```cpp
// Works for single statement only
if (score >= 60)
    cout << "Passed" << endl;

// Better practice: Always use braces
if (score >= 60) {
    cout << "Passed" << endl;
}
```

**⚠️ Warning**: Without braces, only the first statement is controlled by if:

```cpp
// WRONG - indentation is misleading
if (temperature > 90)
    cout << "Hot!" << endl;
    cout << "Drink water!" << endl;  // This ALWAYS executes!

// CORRECT - use braces
if (temperature > 90) {
    cout << "Hot!" << endl;
    cout << "Drink water!" << endl;  // Only executes if condition is true
}
```

---

## 2. The if-else Statement

### Basic Syntax

```cpp
if (condition) {
    // Code if condition is true
} else {
    // Code if condition is false
}
```

**How it works**:

- If condition is true → execute first block
- If condition is false → execute else block
- Exactly ONE of the two blocks will execute

### Simple Examples

**Example 1: Even or Odd**

```cpp
#include <iostream>
using namespace std;

int main() {
    int number;
    
    cout << "Enter a number: ";
    cin >> number;
    
    if (number % 2 == 0) {
        cout << number << " is EVEN" << endl;
    } else {
        cout << number << " is ODD" << endl;
    }
    
    return 0;
}
```

**Example 2: Pass or Fail**

```cpp
int score;

cout << "Enter your score: ";
cin >> score;

if (score >= 60) {
    cout << "Result: PASS ✓" << endl;
    cout << "Well done!" << endl;
} else {
    cout << "Result: FAIL ✗" << endl;
    cout << "Better luck next time!" << endl;
}
```

**Example 3: Positive or Negative**

```cpp
int number;

cout << "Enter a number: ";
cin >> number;

if (number >= 0) {
    cout << "The number is positive or zero." << endl;
} else {
    cout << "The number is negative." << endl;
}
```

### Complete Example: Simple Login

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string password;
    const string CORRECT_PASSWORD = "secret123";
    
    cout << "Enter password: ";
    cin >> password;
    
    if (password == CORRECT_PASSWORD) {
        cout << "✓ Access granted!" << endl;
        cout << "Welcome to the system." << endl;
    } else {
        cout << "✗ Access denied!" << endl;
        cout << "Incorrect password." << endl;
    }
    
    return 0;
}
```

---

## 3. The else-if Ladder

### Basic Syntax

```cpp
if (condition1) {
    // Code if condition1 is true
} else if (condition2) {
    // Code if condition1 is false AND condition2 is true
} else if (condition3) {
    // Code if condition1 and condition2 are false AND condition3 is true
} else {
    // Code if all conditions are false
}
```

**How it works**:

1. Check condition1 - if true, execute block and skip rest
2. If false, check condition2 - if true, execute and skip rest
3. Continue checking until one is true
4. If all are false, execute else block (if present)
5. **Only ONE block executes**

### Simple Examples

**Example 1: Letter Grade**

```cpp
#include <iostream>
using namespace std;

int main() {
    int score;
    
    cout << "Enter your score (0-100): ";
    cin >> score;
    
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
    
    return 0;
}
```

**Execution Flow for score = 85**:

```
Check: 85 >= 90? → false, continue
Check: 85 >= 80? → TRUE! Print "Grade: B" and STOP
(Don't check remaining conditions)
```

**Example 2: Temperature Description**

```cpp
int temp;

cout << "Enter temperature (°F): ";
cin >> temp;

if (temp >= 100) {
    cout << "Extremely hot!" << endl;
} else if (temp >= 85) {
    cout << "Very hot" << endl;
} else if (temp >= 70) {
    cout << "Warm" << endl;
} else if (temp >= 55) {
    cout << "Cool" << endl;
} else if (temp >= 32) {
    cout << "Cold" << endl;
} else {
    cout << "Freezing!" << endl;
}
```

**Example 3: Age Categories**

```cpp
int age;

cout << "Enter age: ";
cin >> age;

if (age < 0) {
    cout << "Invalid age!" << endl;
} else if (age <= 12) {
    cout << "Category: Child" << endl;
} else if (age <= 19) {
    cout << "Category: Teenager" << endl;
} else if (age <= 59) {
    cout << "Category: Adult" << endl;
} else if (age <= 120) {
    cout << "Category: Senior" << endl;
} else {
    cout << "Invalid age!" << endl;
}
```

### Complete Example: BMI Calculator with Categories

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
    
    cout << fixed << setprecision(1);
    cout << "\nYour BMI: " << bmi << endl;
    
    // Determine category
    if (bmi < 18.5) {
        cout << "Category: Underweight" << endl;
        cout << "Recommendation: Gain weight" << endl;
    } else if (bmi < 25) {
        cout << "Category: Normal weight" << endl;
        cout << "Recommendation: Maintain current weight" << endl;
    } else if (bmi < 30) {
        cout << "Category: Overweight" << endl;
        cout << "Recommendation: Lose weight" << endl;
    } else {
        cout << "Category: Obese" << endl;
        cout << "Recommendation: Consult a doctor" << endl;
    }
    
    return 0;
}
```

---

## 4. Nested if Statements

### What are Nested if Statements?

**Nested if** means putting an if statement inside another if statement.

**Syntax**:

```cpp
if (outer_condition) {
    // Outer block
    
    if (inner_condition) {
        // Inner block - only executes if BOTH conditions are true
    }
}
```

### Simple Examples

**Example 1: Nested Validation**

```cpp
int age;

cout << "Enter your age: ";
cin >> age;

if (age >= 18) {
    // First condition: Adult
    cout << "You are an adult." << endl;
    
    if (age >= 65) {
        // Second condition: Senior
        cout << "You qualify for senior discount!" << endl;
    }
}
```

**Execution Flow**:

```
If age = 70:
  70 >= 18? → true → Print "You are an adult."
  70 >= 65? → true → Print "You qualify for senior discount!"

If age = 25:
  25 >= 18? → true → Print "You are an adult."
  25 >= 65? → false → Don't print discount message

If age = 15:
  15 >= 18? → false → Skip entire outer block
```

**Example 2: Number Sign and Even/Odd**

```cpp
int number;

cout << "Enter a number: ";
cin >> number;

if (number > 0) {
    cout << "Positive number" << endl;
    
    if (number % 2 == 0) {
        cout << "And it's EVEN" << endl;
    } else {
        cout << "And it's ODD" << endl;
    }
} else if (number < 0) {
    cout << "Negative number" << endl;
    
    if (number % 2 == 0) {
        cout << "And it's EVEN" << endl;
    } else {
        cout << "And it's ODD" << endl;
    }
} else {
    cout << "The number is ZERO" << endl;
}
```

**Example 3: Login with Password Strength**

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string username, password;
    
    cout << "Enter username: ";
    cin >> username;
    
    cout << "Enter password: ";
    cin >> password;
    
    if (username == "admin") {
        cout << "Valid username!" << endl;
        
        if (password.length() >= 8) {
            cout << "Password length is good." << endl;
            
            if (password == "admin123") {
                cout << "✓ Login successful!" << endl;
            } else {
                cout << "✗ Wrong password!" << endl;
            }
        } else {
            cout << "✗ Password too short!" << endl;
        }
    } else {
        cout << "✗ Invalid username!" << endl;
    }
    
    return 0;
}
```

### Complex Nested Example: Ticket Pricing

```cpp
#include <iostream>
using namespace std;

int main() {
    int age;
    char day;
    double price;
    
    cout << "=== Movie Ticket Pricing ===" << endl;
    cout << "Enter age: ";
    cin >> age;
    
    cout << "Is it a weekend? (Y/N): ";
    cin >> day;
    
    if (age < 0 || age > 120) {
        cout << "Invalid age!" << endl;
    } else {
        // Determine base price by age
        if (age <= 12) {
            // Child
            price = 5.00;
            cout << "Category: Child" << endl;
        } else if (age >= 65) {
            // Senior
            price = 6.00;
            cout << "Category: Senior" << endl;
        } else {
            // Adult
            price = 10.00;
            cout << "Category: Adult" << endl;
        }
        
        // Apply weekend surcharge
        if (day == 'Y' || day == 'y') {
            price += 2.00;
            cout << "Weekend surcharge: +$2.00" << endl;
        }
        
        cout << "Total ticket price: $" << price << endl;
    }
    
    return 0;
}
```

---

## 5. The switch-case Statement

### What is switch-case?

**switch-case** is used when you want to compare a variable against **multiple specific values**. It's cleaner than many else-if statements when checking for exact matches.

### Basic Syntax

```cpp
switch (variable) {
    case value1:
        // Code for value1
        break;
    
    case value2:
        // Code for value2
        break;
    
    case value3:
        // Code for value3
        break;
    
    default:
        // Code if no case matches
        break;
}
```

**Important Rules**:

1. **switch** works with integers and characters (not strings in basic C++)
2. Each **case** must have a constant value
3. **break** is needed to exit the switch (or cases "fall through")
4. **default** is optional (like else)

### Simple Examples

**Example 1: Day of the Week**

```cpp
#include <iostream>
using namespace std;

int main() {
    int day;
    
    cout << "Enter day number (1-7): ";
    cin >> day;
    
    switch (day) {
        case 1:
            cout << "Monday" << endl;
            break;
        
        case 2:
            cout << "Tuesday" << endl;
            break;
        
        case 3:
            cout << "Wednesday" << endl;
            break;
        
        case 4:
            cout << "Thursday" << endl;
            break;
        
        case 5:
            cout << "Friday" << endl;
            break;
        
        case 6:
            cout << "Saturday" << endl;
            break;
        
        case 7:
            cout << "Sunday" << endl;
            break;
        
        default:
            cout << "Invalid day!" << endl;
            break;
    }
    
    return 0;
}
```

**Example 2: Simple Calculator**

```cpp
#include <iostream>
using namespace std;

int main() {
    double num1, num2, result;
    char operation;
    
    cout << "Enter first number: ";
    cin >> num1;
    
    cout << "Enter operation (+, -, *, /): ";
    cin >> operation;
    
    cout << "Enter second number: ";
    cin >> num2;
    
    switch (operation) {
        case '+':
            result = num1 + num2;
            cout << "Result: " << result << endl;
            break;
        
        case '-':
            result = num1 - num2;
            cout << "Result: " << result << endl;
            break;
        
        case '*':
            result = num1 * num2;
            cout << "Result: " << result << endl;
            break;
        
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
                cout << "Result: " << result << endl;
            } else {
                cout << "Error: Division by zero!" << endl;
            }
            break;
        
        default:
            cout << "Invalid operation!" << endl;
            break;
    }
    
    return 0;
}
```

**Example 3: Grade to GPA**

```cpp
char grade;

cout << "Enter your grade (A, B, C, D, F): ";
cin >> grade;

switch (grade) {
    case 'A':
    case 'a':  // Handle both upper and lower case
        cout << "GPA: 4.0" << endl;
        break;
    
    case 'B':
    case 'b':
        cout << "GPA: 3.0" << endl;
        break;
    
    case 'C':
    case 'c':
        cout << "GPA: 2.0" << endl;
        break;
    
    case 'D':
    case 'd':
        cout << "GPA: 1.0" << endl;
        break;
    
    case 'F':
    case 'f':
        cout << "GPA: 0.0" << endl;
        break;
    
    default:
        cout << "Invalid grade!" << endl;
        break;
}
```

### Understanding break and Fall-through

**Without break** - cases "fall through":

```cpp
int number = 2;

switch (number) {
    case 1:
        cout << "One" << endl;
        // No break!
    
    case 2:
        cout << "Two" << endl;
        // No break!
    
    case 3:
        cout << "Three" << endl;
        break;
}

// Output:
// Two
// Three
```

**Intentional fall-through** (grouping cases):

```cpp
int month;

cout << "Enter month (1-12): ";
cin >> month;

switch (month) {
    case 12:
    case 1:
    case 2:
        cout << "Season: Winter" << endl;
        break;
    
    case 3:
    case 4:
    case 5:
        cout << "Season: Spring" << endl;
        break;
    
    case 6:
    case 7:
    case 8:
        cout << "Season: Summer" << endl;
        break;
    
    case 9:
    case 10:
    case 11:
        cout << "Season: Fall" << endl;
        break;
    
    default:
        cout << "Invalid month!" << endl;
        break;
}
```

### Complete Example: Menu System

```cpp
#include <iostream>
using namespace std;

int main() {
    int choice;
    
    cout << "=== Main Menu ===" << endl;
    cout << "1. Start New Game" << endl;
    cout << "2. Load Game" << endl;
    cout << "3. Settings" << endl;
    cout << "4. Help" << endl;
    cout << "5. Exit" << endl;
    cout << "\nEnter your choice (1-5): ";
    cin >> choice;
    
    switch (choice) {
        case 1:
            cout << "\n--- Starting New Game ---" << endl;
            cout << "Creating new player..." << endl;
            cout << "Loading world..." << endl;
            cout << "Game started!" << endl;
            break;
        
        case 2:
            cout << "\n--- Load Game ---" << endl;
            cout << "Looking for saved games..." << endl;
            cout << "No saved games found!" << endl;
            break;
        
        case 3:
            cout << "\n--- Settings ---" << endl;
            cout << "1. Audio Settings" << endl;
            cout << "2. Video Settings" << endl;
            cout << "3. Controls" << endl;
            break;
        
        case 4:
            cout << "\n--- Help ---" << endl;
            cout << "Visit our website for tutorials." << endl;
            cout << "www.example.com/help" << endl;
            break;
        
        case 5:
            cout << "\nThanks for playing! Goodbye!" << endl;
            break;
        
        default:
            cout << "\nInvalid choice! Please select 1-5." << endl;
            break;
    }
    
    return 0;
}
```

---

## 6. The Ternary Operator (Conditional Operator)

### What is the Ternary Operator?

A **shorthand** way to write simple if-else statements in one line.

### Syntax

```cpp
condition ? value_if_true : value_if_false
```

**How it works**:

- Evaluate condition
- If true → return value_if_true
- If false → return value_if_false

### Simple Examples

**Example 1: Find Maximum**

```cpp
int a = 10, b = 20;
int max = (a > b) ? a : b;

cout << "Maximum: " << max << endl;  // 20

// Equivalent to:
int max;
if (a > b) {
    max = a;
} else {
    max = b;
}
```

**Example 2: Even or Odd**

```cpp
int number = 7;
string result = (number % 2 == 0) ? "Even" : "Odd";

cout << number << " is " << result << endl;  // 7 is Odd
```

**Example 3: Pass or Fail**

```cpp
int score = 75;
string status = (score >= 60) ? "PASS" : "FAIL";

cout << "Status: " << status << endl;  // Status: PASS
```

**Example 4: Absolute Value**

```cpp
int number = -5;
int absolute = (number >= 0) ? number : -number;

cout << "Absolute value: " << absolute << endl;  // 5
```

### Nested Ternary (Not Recommended)

```cpp
int score = 85;

// Works but hard to read
string grade = (score >= 90) ? "A" :
               (score >= 80) ? "B" :
               (score >= 70) ? "C" :
               (score >= 60) ? "D" : "F";

// Better: Use if-else for multiple conditions
string grade;
if (score >= 90) {
    grade = "A";
} else if (score >= 80) {
    grade = "B";
} else if (score >= 70) {
    grade = "C";
} else if (score >= 60) {
    grade = "D";
} else {
    grade = "F";
}
```

### When to Use Ternary

**Good use cases**:

```cpp
// Simple assignment
int max = (a > b) ? a : b;

// Simple output
cout << (age >= 18 ? "Adult" : "Minor") << endl;

// Return value
return (x > 0) ? 1 : -1;
```

**Avoid ternary for**:

- Complex conditions
- Multiple statements
- Nested ternaries
- When readability suffers

---

## Comparison: if-else vs switch-case

### When to Use if-else

```cpp
// Range checking
if (score >= 90) {
    cout << "A" << endl;
} else if (score >= 80) {
    cout << "B" << endl;
}

// Complex conditions
if (age >= 18 && hasLicense) {
    cout << "Can drive" << endl;
}

// String comparison (basic C++)
if (username == "admin") {
    cout << "Welcome admin!" << endl;
}
```

### When to Use switch-case

```cpp
// Specific value matching
switch (dayOfWeek) {
    case 1: cout << "Monday"; break;
    case 2: cout << "Tuesday"; break;
    // etc.
}

// Character menu
switch (menuChoice) {
    case 'A': /* action */ break;
    case 'B': /* action */ break;
}

// Cleaner for many exact matches
```

### Comparison Table

|Feature|if-else|switch-case|
|---|---|---|
|**Conditions**|Any boolean expression|Only equality checks|
|**Data types**|Any|int, char (not string*)|
|**Ranges**|Yes (`>`, `<`, `>=`, `<=`)|No|
|**Multiple conditions**|Yes (`&&`, `||
|**Readability**|Good for few options|Better for many options|
|**Performance**|Slightly slower|Slightly faster|

*Modern C++ (C++17+) supports strings in switch with special techniques, but not in basic C++.

---

## Practical Examples and Projects

### Example 1: Simple ATM System

```cpp
#include <iostream>
using namespace std;

int main() {
    double balance = 1000.00;
    int choice;
    double amount;
    
    cout << "=== ATM System ===" << endl;
    cout << "1. Check Balance" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. Withdraw" << endl;
    cout << "4. Exit" << endl;
    cout << "\nEnter choice: ";
    cin >> choice;
    
    switch (choice) {
        case 1:
            cout << "\nCurrent Balance: $" << balance << endl;
            break;
        
        case 2:
            cout << "\nEnter amount to deposit: $";
            cin >> amount;
            
            if (amount > 0) {
                balance += amount;
                cout << "Deposit successful!" << endl;
                cout << "New balance: $" << balance << endl;
            } else {
                cout << "Invalid amount!" << endl;
            }
            break;
        
        case 3:
            cout << "\nEnter amount to withdraw: $";
            cin >> amount;
            
            if (amount > 0) {
                if (amount <= balance) {
                    balance -= amount;
                    cout << "Withdrawal successful!" << endl;
                    cout << "New balance: $" << balance << endl;
                } else {
                    cout << "Insufficient funds!" << endl;
                    cout << "Current balance: $" << balance << endl;
                }
            } else {
                cout << "Invalid amount!" << endl;
            }
            break;
        
        case 4:
            cout << "\nThank you for using ATM!" << endl;
            break;
        
        default:
            cout << "\nInvalid choice!" << endl;
            break;
    }
    
    return 0;
}
```

### Example 2: Triangle Classifier

```cpp
#include <iostream>
using namespace std;

int main() {
    double side1, side2, side3;
    
    cout << "=== Triangle Classifier ===" << endl;
    cout << "Enter three sides: ";
    cin >> side1 >> side2 >> side3;
    
    // Check if it's a valid triangle
    if (side1 <= 0 || side2 <= 0 || side3 <= 0) {
        cout << "Error: Sides must be positive!" << endl;
    } else if (side1 + side2 <= side3 || 
               side1 + side3 <= side2 || 
               side2 + side3 <= side1) {
        cout << "Not a valid triangle!" << endl;
        cout << "(Sum of any two sides must be greater than the third)" << endl;
    } else {
        cout << "\nValid triangle!" << endl;
        
        // Classify by sides
        if (side1 == side2 && side2 == side3) {
            cout << "Type by sides: Equilateral (all sides equal)" << endl;
        } else if (side1 == side2 || side2 == side3 || side1 == side3) {
            cout << "Type by sides: Isosceles (two sides equal)" << endl;
        } else {
            cout << "Type by sides: Scalene (all sides different)" << endl;
        }
        
        // Classify by angles (using Pythagorean theorem)
        double a = side1, b = side2, c = side3;
        
        // Make sure c is the longest side
        if (a > c) { double temp = a; a = c; c = temp; }
        if (b > c) { double temp = b; b = c; c = temp; }
        
        double cSquared = c * c;
        double sumSquares = (a * a) + (b * b);
        
        if (cSquared == sumSquares) {
            cout << "Type by angles: Right triangle" << endl;
        } else if (cSquared > sumSquares) {
            cout << "Type by angles: Obtuse triangle" << endl;
        } else {
            cout << "Type by angles: Acute triangle" << endl;
        }
    }
    
    return 0;
}
```

### Example 3: Shipping Cost Calculator

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    double weight, distance, baseCost, totalCost;
    char shippingType;
    
    cout << "=== Shipping Cost Calculator ===" << endl;
    
    cout << "Enter package weight (kg): ";
    cin >> weight;
    
    cout << "Enter shipping distance (km): ";
    cin >> distance;
    
    cout << "\nShipping Types:" << endl;
    cout << "S - Standard (5-7 days)" << endl;
    cout << "E - Express (2-3 days)" << endl;
    cout << "O - Overnight (next day)" << endl;
    cout << "Select shipping type: ";
    cin >> shippingType;
    
    // Input validation
    if (weight <= 0 || distance <= 0) {
        cout << "\nError: Weight and distance must be positive!" << endl;
        return 1;
    }
    
    // Calculate base cost by weight
    if (weight <= 1) {
        baseCost = 5.00;
    } else if (weight <= 5) {
        baseCost = 10.00;
    } else if (weight <= 10) {
        baseCost = 15.00;
    } else {
        baseCost = 15.00 + (weight - 10) * 2.00;  // $2 per kg over 10kg
    }
    
    // Add distance charge
    if (distance > 100) {
        baseCost += (distance / 100) * 5.00;  // $5 per 100km
    }
    
    // Apply shipping type multiplier
    switch (shippingType) {
        case 'S':
        case 's':
            totalCost = baseCost * 1.0;  // No multiplier
            cout << "\nShipping Type: Standard" << endl;
            break;
        
        case 'E':
        case 'e':
            totalCost = baseCost * 1.5;  // 50% more
            cout << "\nShipping Type: Express" << endl;
            break;
        
        case 'O':
        case 'o':
            totalCost = baseCost * 2.0;  // Double
            cout << "\nShipping Type: Overnight" << endl;
            break;
        
        default:
            cout << "\nError: Invalid shipping type!" << endl;
            return 1;
    }
    
    // Display results
    cout << fixed << setprecision(2);
    cout << "Base Cost: $" << baseCost << endl;
    cout << "Total Cost: $" << totalCost << endl;
    
    return 0;
}
```

### Example 4: Leap Year Checker with Explanation

```cpp
#include <iostream>
using namespace std;

int main() {
    int year;
    bool isLeap;
    
    cout << "Enter a year: ";
    cin >> year;
    
    if (year < 0) {
        cout << "Invalid year!" << endl;
        return 1;
    }
    
    // Leap year logic
    if (year % 4 != 0) {
        // Not divisible by 4 → not a leap year
        isLeap = false;
        cout << year << " is NOT a leap year." << endl;
        cout << "Reason: Not divisible by 4" << endl;
    } else {
        // Divisible by 4
        if (year % 100 != 0) {
            // Not a century year → is a leap year
            isLeap = true;
            cout << year << " IS a leap year!" << endl;
            cout << "Reason: Divisible by 4 and not a century year" << endl;
        } else {
            // Century year - check if divisible by 400
            if (year % 400 == 0) {
                isLeap = true;
                cout << year << " IS a leap year!" << endl;
                cout << "Reason: Century year divisible by 400" << endl;
            } else {
                isLeap = false;
                cout << year << " is NOT a leap year." << endl;
                cout << "Reason: Century year not divisible by 400" << endl;
            }
        }
    }
    
    // Show February days
    if (isLeap) {
        cout << "February has 29 days this year." << endl;
    } else {
        cout << "February has 28 days this year." << endl;
    }
    
    return 0;
}
```

### Example 5: Simple Quiz Game

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    int score = 0;
    int answer;
    string textAnswer;
    
    cout << "=== C++ Quiz Game ===" << endl;
    cout << "Answer the following questions:\n" << endl;
    
    // Question 1
    cout << "Question 1: What is 15 + 27?" << endl;
    cout << "Your answer: ";
    cin >> answer;
    
    if (answer == 42) {
        cout << "✓ Correct!" << endl;
        score++;
    } else {
        cout << "✗ Wrong! The answer is 42." << endl;
    }
    cout << endl;
    
    // Question 2
    cout << "Question 2: Is C++ case-sensitive? (yes/no)" << endl;
    cout << "Your answer: ";
    cin >> textAnswer;
    
    if (textAnswer == "yes" || textAnswer == "Yes" || textAnswer == "YES") {
        cout << "✓ Correct!" << endl;
        score++;
    } else {
        cout << "✗ Wrong! C++ is case-sensitive." << endl;
    }
    cout << endl;
    
    // Question 3
    cout << "Question 3: What does '==' do?" << endl;
    cout << "1. Assigns a value" << endl;
    cout << "2. Compares two values" << endl;
    cout << "3. Adds two numbers" << endl;
    cout << "Your answer (1-3): ";
    cin >> answer;
    
    switch (answer) {
        case 2:
            cout << "✓ Correct!" << endl;
            score++;
            break;
        case 1:
            cout << "✗ Wrong! '=' assigns, '==' compares." << endl;
            break;
        case 3:
            cout << "✗ Wrong! '+' adds numbers." << endl;
            break;
        default:
            cout << "✗ Invalid answer!" << endl;
            break;
    }
    cout << endl;
    
    // Show results
    cout << "=== Quiz Complete ===" << endl;
    cout << "Your score: " << score << "/3" << endl;
    
    if (score == 3) {
        cout << "Perfect! Excellent work! 🌟" << endl;
    } else if (score == 2) {
        cout << "Good job! Almost perfect! 👍" << endl;
    } else if (score == 1) {
        cout << "Not bad, keep studying! 📚" << endl;
    } else {
        cout << "Keep practicing! You'll improve! 💪" << endl;
    }
    
    return 0;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Using = Instead of ==

**Wrong**:

```cpp
int x = 5;

if (x = 10) {  // ⚠️ This ASSIGNS 10 to x, doesn't compare!
    cout << "x is 10" << endl;  // Always executes
}
```

**Correct**:

```cpp
if (x == 10) {  // Compares x with 10
    cout << "x is 10" << endl;
}
```

### Mistake 2: Forgetting Braces

**Wrong**:

```cpp
if (temperature > 90)
    cout << "It's hot!" << endl;
    cout << "Drink water!" << endl;  // Always executes!
```

**Correct**:

```cpp
if (temperature > 90) {
    cout << "It's hot!" << endl;
    cout << "Drink water!" << endl;  // Only executes if condition is true
}
```

### Mistake 3: Semicolon After if

**Wrong**:

```cpp
if (score >= 60);  // ⚠️ Semicolon here!
{
    cout << "Passed" << endl;  // Always executes
}
```

**Correct**:

```cpp
if (score >= 60) {  // No semicolon
    cout << "Passed" << endl;
}
```

### Mistake 4: Forgetting break in switch

**Wrong**:

```cpp
switch (day) {
    case 1:
        cout << "Monday" << endl;
        // No break! Falls through to case 2
    
    case 2:
        cout << "Tuesday" << endl;
        break;
}

// If day = 1, prints both "Monday" and "Tuesday"
```

**Correct**:

```cpp
switch (day) {
    case 1:
        cout << "Monday" << endl;
        break;  // Exits switch
    
    case 2:
        cout << "Tuesday" << endl;
        break;
}
```

### Mistake 5: Incorrect Logical Operators

**Wrong**:

```cpp
if (age >= 18 && <= 65) {  // ⚠️ Syntax error!
    // ...
}
```

**Correct**:

```cpp
if (age >= 18 && age <= 65) {  // Must specify variable twice
    // ...
}
```

### Mistake 6: Comparing Floating Point Numbers

**Problematic**:

```cpp
double value = 0.1 + 0.2;

if (value == 0.3) {  // Might be false due to precision!
    // Might not execute
}
```

**Better**:

```cpp
double value = 0.1 + 0.2;
double epsilon = 0.0001;

if (abs(value - 0.3) < epsilon) {  // Check if close enough
    // More reliable
}
```

### Mistake 7: Unreachable Code

**Wrong**:

```cpp
if (score >= 90) {
    cout << "A" << endl;
} else if (score >= 80) {
    cout << "B" << endl;
} else if (score >= 90) {  // ⚠️ Never reached!
    cout << "A+" << endl;
}
```

**Correct**:

```cpp
if (score >= 90) {
    cout << "A" << endl;
} else if (score >= 80) {
    cout << "B" << endl;
}
// Put more specific conditions first
```

---

## Practice Exercises

### Exercise 1: Electricity Bill Calculator

**Requirements**:

- Read units consumed
- Calculate bill:
    - 0-50 units: $0.50/unit
    - 51-150 units: $0.75/unit
    - 151-250 units: $1.20/unit
    - Above 250: $1.50/unit
- Add 20% tax
- Display total

### Exercise 2: Rock, Paper, Scissors Game

**Requirements**:

- User enters choice (R/P/S)
- Computer randomly chooses (hint: use rand() % 3)
- Determine winner
- Display result

### Exercise 3: Date Validator

**Requirements**:

- Read day, month, year
- Check if valid date
- Consider:
    - Days in each month
    - Leap years
    - Month range (1-12)
    - Day range based on month

### Exercise 4: Student Grade System

**Requirements**:

- Read 5 subject marks
- Calculate average
- Determine letter grade (A-F)
- Check pass/fail (40+ in each subject AND 50+ average)
- Display detailed report

### Exercise 5: Simple Banking Menu

**Requirements**:

- Create menu: Create Account, Deposit, Withdraw, Check Balance, Exit
- Implement each option with validation
- Use switch-case for menu
- Keep running until user exits

---

## Summary and Best Practices

### Key Takeaways

1. **if** - Execute code if condition is true
2. **if-else** - Choose between two options
3. **else-if** - Choose from multiple options
4. **Nested if** - Conditions within conditions
5. **switch-case** - Multiple specific values
6. **Ternary** - Shorthand for simple if-else

### Decision Making Flowchart

```
Start
  ↓
Need to make decision?
  ↓
Check condition
  ↓
True? → Execute true block
  ↓
False? → Execute else block (if present)
  ↓
Continue program
```

### Best Practices Checklist

✅ **Always use braces `{}`** - Even for single statements  
✅ **Use `==` for comparison** - Not `=`  
✅ **Add `break` in switch** - Unless intentional fall-through  
✅ **Put specific conditions first** - In else-if chains  
✅ **Validate input** - Check ranges and types  
✅ **Use switch for exact matches** - if-else for ranges  
✅ **Comment complex conditions** - Explain the logic  
✅ **Avoid deep nesting** - Keep it readable  
✅ **Test all paths** - Make sure all cases work

### Quick Reference

**if statement**:

```cpp
if (condition) {
    // code
}
```

**if-else**:

```cpp
if (condition) {
    // code if true
} else {
    // code if false
}
```

**else-if ladder**:

```cpp
if (condition1) {
    // code
} else if (condition2) {
    // code
} else {
    // code
}
```

**switch-case**:

```cpp
switch (variable) {
    case value1:
        // code
        break;
    default:
        // code
        break;
}
```

**Ternary**:

```cpp
result = (condition) ? value_if_true : value_if_false;
```

---

## Conclusion

Conditional statements are the foundation of decision-making in programming. They allow your programs to:

- **Respond to user input**
- **Validate data**
- **Handle different scenarios**
- **Create intelligent behavior**
- **Control program flow**

**Mastering conditionals enables you to**:

- Build interactive applications
- Create games with rules
- Validate user input
- Implement business logic
- Make smart programs

**Next steps**:

1. Practice with all the exercises
2. Build small decision-based programs
3. Combine with operators learned earlier
4. Move on to learning **Loops** (repetition)

With conditional statements mastered, you can now make programs that think and adapt!

**Happy Coding!** 🚀