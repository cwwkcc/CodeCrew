## Introduction

Imagine you need to print "Hello" 100 times. Would you write `cout << "Hello"` 100 times? Of course not! That's where **loops** come in.

**Loops** allow you to repeat a block of code multiple times without writing it over and over. They're one of the most powerful features in programming.

**Real-World Analogies**:

- **Washing dishes**: Repeat (pick dish → wash → rinse → dry) until all dishes are clean
- **Walking stairs**: Repeat (lift foot → step up) until you reach the top
- **Alarm clock**: Repeat (ring → snooze) until you turn it off
- **Reading pages**: Repeat (read page → turn page) until book ends

---

## Why Loops Matter

Without loops, you'd have to:

- Write the same code hundreds of times
- Manually handle each repetition
- Know exact number of repetitions in advance
- Create huge, unmanageable programs

**With loops, you can**:

- Process arrays of any size
- Validate input until correct
- Create games that run continuously
- Process files line by line
- Implement menus that keep running
- Generate patterns and sequences

---

## Types of Loops in C++

C++ provides three main types of loops:

1. **while loop** - Repeat while condition is true
2. **do-while loop** - Execute at least once, then repeat while true
3. **for loop** - Repeat a specific number of times

Plus special controls: 4. **break statement** - Exit loop immediately 5. **continue statement** - Skip to next iteration 6. **Nested loops** - Loops inside loops

Let's explore each in detail!

---

## 1. The while Loop

### What is a while Loop?

The **while loop** repeats code as long as a condition is true. It checks the condition **before** each iteration.

### Basic Syntax

```cpp
while (condition) {
    // Code to repeat
    // Must eventually make condition false!
}
```

**How it works**:

1. Check condition
2. If true → execute code block, go back to step 1
3. If false → exit loop, continue with program

### Simple Examples

**Example 1: Count from 1 to 5**

```cpp
#include <iostream>
using namespace std;

int main() {
    int count = 1;
    
    while (count <= 5) {
        cout << count << " ";
        count++;  // IMPORTANT: Increment or loop runs forever!
    }
    
    cout << endl;
    // Output: 1 2 3 4 5
    
    return 0;
}
```

**Execution Flow**:

```
count = 1
Check: 1 <= 5? → true → Print 1, count = 2
Check: 2 <= 5? → true → Print 2, count = 3
Check: 3 <= 5? → true → Print 3, count = 4
Check: 4 <= 5? → true → Print 4, count = 5
Check: 5 <= 5? → true → Print 5, count = 6
Check: 6 <= 5? → false → Exit loop
```

**Example 2: Sum numbers from 1 to 10**

```cpp
int sum = 0;
int number = 1;

while (number <= 10) {
    sum += number;
    number++;
}

cout << "Sum: " << sum << endl;  // Output: 55
```

**Example 3: Countdown**

```cpp
int countdown = 10;

while (countdown > 0) {
    cout << countdown << "... ";
    countdown--;
}

cout << "Blast off!" << endl;
// Output: 10... 9... 8... 7... 6... 5... 4... 3... 2... 1... Blast off!
```

**Example 4: Input Validation**

```cpp
int age;

cout << "Enter your age (1-120): ";
cin >> age;

while (age < 1 || age > 120) {
    cout << "Invalid! Enter age (1-120): ";
    cin >> age;
}

cout << "Valid age: " << age << endl;
```

### The Infinite Loop Problem

**⚠️ DANGER: Infinite Loop**

If the condition never becomes false, the loop runs forever!

```cpp
int count = 1;

while (count <= 5) {
    cout << count << " ";
    // Forgot to increment count!
    // Loop runs forever printing "1 1 1 1..."
}
```

**How to avoid**:

- Always modify the loop variable inside the loop
- Make sure condition can eventually become false
- Use Ctrl+C to stop a runaway program

### Complete Example: Number Guessing Game

```cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    // Initialize random number generator
    srand(time(0));
    
    int secretNumber = rand() % 100 + 1;  // Random 1-100
    int guess;
    int attempts = 0;
    
    cout << "=== Number Guessing Game ===" << endl;
    cout << "I'm thinking of a number between 1 and 100." << endl;
    
    while (guess != secretNumber) {
        cout << "\nEnter your guess: ";
        cin >> guess;
        attempts++;
        
        if (guess > secretNumber) {
            cout << "Too high! Try again." << endl;
        } else if (guess < secretNumber) {
            cout << "Too low! Try again." << endl;
        } else {
            cout << "🎉 Congratulations! You got it!" << endl;
            cout << "It took you " << attempts << " attempts." << endl;
        }
    }
    
    return 0;
}
```

---

## 2. The do-while Loop

### What is a do-while Loop?

The **do-while loop** executes code **at least once**, then repeats while condition is true. It checks the condition **after** each iteration.

**Key Difference from while**:

- **while**: Check first, then execute (might not execute at all)
- **do-while**: Execute first, then check (always executes at least once)

### Basic Syntax

```cpp
do {
    // Code to execute
    // Runs at least once!
} while (condition);  // Note the semicolon!
```

### Simple Examples

**Example 1: Menu that runs at least once**

```cpp
#include <iostream>
using namespace std;

int main() {
    int choice;
    
    do {
        cout << "\n=== Menu ===" << endl;
        cout << "1. Play Game" << endl;
        cout << "2. High Scores" << endl;
        cout << "3. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        if (choice == 1) {
            cout << "Starting game..." << endl;
        } else if (choice == 2) {
            cout << "Showing scores..." << endl;
        } else if (choice == 3) {
            cout << "Goodbye!" << endl;
        } else {
            cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 3);
    
    return 0;
}
```

**Example 2: Input validation (must ask at least once)**

```cpp
int age;

do {
    cout << "Enter your age (1-120): ";
    cin >> age;
    
    if (age < 1 || age > 120) {
        cout << "Invalid age! Try again." << endl;
    }
} while (age < 1 || age > 120);

cout << "Valid age: " << age << endl;
```

**Example 3: Password validation**

```cpp
string password;

do {
    cout << "Create a password (min 8 characters): ";
    cin >> password;
    
    if (password.length() < 8) {
        cout << "Too short! Must be at least 8 characters." << endl;
    }
} while (password.length() < 8);

cout << "Password created successfully!" << endl;
```

### Comparison: while vs do-while

**while loop** (condition first):

```cpp
int x = 10;

while (x < 5) {
    cout << "This never prints" << endl;
    x++;
}
// Loop body never executes because 10 is not < 5
```

**do-while loop** (execute first):

```cpp
int x = 10;

do {
    cout << "This prints once!" << endl;
    x++;
} while (x < 5);
// Loop body executes once, then condition is checked
```

### Complete Example: ATM with Validation

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    double balance = 1000.00;
    int choice;
    double amount;
    
    cout << "=== ATM System ===" << endl;
    
    do {
        // Display menu
        cout << "\n--- Main Menu ---" << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Deposit" << endl;
        cout << "3. Withdraw" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter choice (1-4): ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << fixed << setprecision(2);
                cout << "\nCurrent Balance: $" << balance << endl;
                break;
            
            case 2:
                do {
                    cout << "\nEnter deposit amount: $";
                    cin >> amount;
                    
                    if (amount <= 0) {
                        cout << "Invalid amount! Must be positive." << endl;
                    }
                } while (amount <= 0);
                
                balance += amount;
                cout << "Deposit successful!" << endl;
                cout << "New balance: $" << balance << endl;
                break;
            
            case 3:
                do {
                    cout << "\nEnter withdrawal amount: $";
                    cin >> amount;
                    
                    if (amount <= 0) {
                        cout << "Invalid amount! Must be positive." << endl;
                    } else if (amount > balance) {
                        cout << "Insufficient funds! Balance: $" << balance << endl;
                    }
                } while (amount <= 0 || amount > balance);
                
                balance -= amount;
                cout << "Withdrawal successful!" << endl;
                cout << "New balance: $" << balance << endl;
                break;
            
            case 4:
                cout << "\nThank you for using ATM!" << endl;
                break;
            
            default:
                cout << "\nInvalid choice! Please select 1-4." << endl;
                break;
        }
        
    } while (choice != 4);
    
    return 0;
}
```

---

## 3. The for Loop

### What is a for Loop?

The **for loop** is perfect when you know **exactly how many times** to repeat. It's the most commonly used loop for counting.

### Basic Syntax

```cpp
for (initialization; condition; update) {
    // Code to repeat
}
```

**Three parts**:

1. **Initialization**: Run once at the start (usually set counter)
2. **Condition**: Check before each iteration (continue while true)
3. **Update**: Run after each iteration (usually increment counter)

### How for Loop Works

```cpp
for (int i = 0; i < 5; i++) {
    cout << i << " ";
}
```

**Execution Flow**:

```
1. Initialize: i = 0
2. Check: 0 < 5? → true → Print 0
3. Update: i++ → i = 1
4. Check: 1 < 5? → true → Print 1
5. Update: i++ → i = 2
6. Check: 2 < 5? → true → Print 2
7. Update: i++ → i = 3
8. Check: 3 < 5? → true → Print 3
9. Update: i++ → i = 4
10. Check: 4 < 5? → true → Print 4
11. Update: i++ → i = 5
12. Check: 5 < 5? → false → Exit loop

Output: 0 1 2 3 4
```

### Simple Examples

**Example 1: Print 1 to 10**

```cpp
for (int i = 1; i <= 10; i++) {
    cout << i << " ";
}
cout << endl;
// Output: 1 2 3 4 5 6 7 8 9 10
```

**Example 2: Print even numbers**

```cpp
for (int i = 2; i <= 20; i += 2) {
    cout << i << " ";
}
cout << endl;
// Output: 2 4 6 8 10 12 14 16 18 20
```

**Example 3: Countdown**

```cpp
for (int i = 10; i > 0; i--) {
    cout << i << " ";
}
cout << "Blast off!" << endl;
// Output: 10 9 8 7 6 5 4 3 2 1 Blast off!
```

**Example 4: Multiplication table**

```cpp
int number = 5;

cout << "Multiplication table for " << number << ":" << endl;

for (int i = 1; i <= 10; i++) {
    cout << number << " × " << i << " = " << (number * i) << endl;
}
```

**Example 5: Sum of numbers**

```cpp
int sum = 0;

for (int i = 1; i <= 100; i++) {
    sum += i;
}

cout << "Sum of 1 to 100: " << sum << endl;  // 5050
```

**Example 6: Factorial**

```cpp
int n = 5;
int factorial = 1;

for (int i = 1; i <= n; i++) {
    factorial *= i;
}

cout << n << "! = " << factorial << endl;  // 120
```

### for Loop Variations

**Skip by different amounts**:

```cpp
// Count by 5s
for (int i = 0; i <= 50; i += 5) {
    cout << i << " ";
}
// Output: 0 5 10 15 20 25 30 35 40 45 50
```

**Multiple variables**:

```cpp
for (int i = 0, j = 10; i < 5; i++, j--) {
    cout << "i=" << i << " j=" << j << endl;
}
// Output:
// i=0 j=10
// i=1 j=9
// i=2 j=8
// i=3 j=7
// i=4 j=6
```

**Infinite for loop** (be careful!):

```cpp
for (;;) {
    // Runs forever!
    // Use break to exit
}
```

### Complete Example: Pattern Printing

```cpp
#include <iostream>
using namespace std;

int main() {
    int rows;
    
    cout << "Enter number of rows: ";
    cin >> rows;
    
    // Right triangle
    cout << "\n--- Right Triangle ---" << endl;
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= i; j++) {
            cout << "* ";
        }
        cout << endl;
    }
    
    // Inverted triangle
    cout << "\n--- Inverted Triangle ---" << endl;
    for (int i = rows; i >= 1; i--) {
        for (int j = 1; j <= i; j++) {
            cout << "* ";
        }
        cout << endl;
    }
    
    // Number pyramid
    cout << "\n--- Number Pyramid ---" << endl;
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= i; j++) {
            cout << j << " ";
        }
        cout << endl;
    }
    
    return 0;
}
```

---

## 4. Loop Control: break and continue

### The break Statement

**break** immediately exits the loop, regardless of the condition.

**Syntax**:

```cpp
while (condition) {
    if (some_condition) {
        break;  // Exit loop immediately
    }
}
```

**Example 1: Exit on specific input**

```cpp
#include <iostream>
using namespace std;

int main() {
    int number;
    
    cout << "Enter numbers (0 to stop):" << endl;
    
    while (true) {  // Infinite loop
        cin >> number;
        
        if (number == 0) {
            break;  // Exit loop when 0 is entered
        }
        
        cout << "You entered: " << number << endl;
    }
    
    cout << "Loop ended!" << endl;
    
    return 0;
}
```

**Example 2: Find first occurrence**

```cpp
int numbers[] = {5, 3, 8, 2, 9, 1};
int search = 8;
bool found = false;

for (int i = 0; i < 6; i++) {
    if (numbers[i] == search) {
        cout << "Found " << search << " at index " << i << endl;
        found = true;
        break;  // Stop searching once found
    }
}

if (!found) {
    cout << search << " not found." << endl;
}
```

**Example 3: Password with limited attempts**

```cpp
string correctPassword = "secret123";
string password;
int attempts = 0;
int maxAttempts = 3;

while (attempts < maxAttempts) {
    cout << "Enter password: ";
    cin >> password;
    
    if (password == correctPassword) {
        cout << "Access granted!" << endl;
        break;  // Exit loop on success
    }
    
    attempts++;
    
    if (attempts < maxAttempts) {
        cout << "Wrong password! " << (maxAttempts - attempts) 
             << " attempts remaining." << endl;
    }
}

if (attempts == maxAttempts) {
    cout << "Account locked!" << endl;
}
```

### The continue Statement

**continue** skips the rest of the current iteration and jumps to the next iteration.

**Syntax**:

```cpp
for (int i = 0; i < 10; i++) {
    if (some_condition) {
        continue;  // Skip to next iteration
    }
    // This code is skipped when continue executes
}
```

**Example 1: Skip even numbers**

```cpp
for (int i = 1; i <= 10; i++) {
    if (i % 2 == 0) {
        continue;  // Skip even numbers
    }
    
    cout << i << " ";  // Only odd numbers print
}
cout << endl;
// Output: 1 3 5 7 9
```

**Example 2: Skip negative numbers**

```cpp
int numbers[] = {5, -3, 8, -2, 9, -1, 4};

cout << "Positive numbers: ";
for (int i = 0; i < 7; i++) {
    if (numbers[i] < 0) {
        continue;  // Skip negative numbers
    }
    
    cout << numbers[i] << " ";
}
cout << endl;
// Output: Positive numbers: 5 8 9 4
```

**Example 3: Skip invalid input**

```cpp
int sum = 0;
int count = 0;

cout << "Enter 5 positive numbers:" << endl;

for (int i = 0; i < 5; i++) {
    int num;
    cin >> num;
    
    if (num <= 0) {
        cout << "Invalid! Skipping..." << endl;
        continue;  // Skip to next iteration
    }
    
    sum += num;
    count++;
}

if (count > 0) {
    cout << "Average: " << (double)sum / count << endl;
}
```

### break vs continue Comparison

```cpp
// Using break
for (int i = 1; i <= 10; i++) {
    if (i == 5) {
        break;  // Stop completely
    }
    cout << i << " ";
}
// Output: 1 2 3 4

// Using continue
for (int i = 1; i <= 10; i++) {
    if (i == 5) {
        continue;  // Skip 5, continue with 6
    }
    cout << i << " ";
}
// Output: 1 2 3 4 6 7 8 9 10
```

---

## 5. Nested Loops

### What are Nested Loops?

**Nested loops** are loops inside other loops. The inner loop completes all its iterations for each iteration of the outer loop.

### Basic Syntax

```cpp
for (initialization1; condition1; update1) {
    // Outer loop
    
    for (initialization2; condition2; update2) {
        // Inner loop
        // Executes multiple times per outer loop iteration
    }
}
```

### Simple Examples

**Example 1: Multiplication table (1-10)**

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    cout << "Multiplication Table (1-10):" << endl;
    
    for (int i = 1; i <= 10; i++) {
        for (int j = 1; j <= 10; j++) {
            cout << setw(4) << (i * j);
        }
        cout << endl;
    }
    
    return 0;
}
```

**Execution Flow** (for i=1):

```
Outer: i=1
  Inner: j=1 → Print 1
  Inner: j=2 → Print 2
  Inner: j=3 → Print 3
  ... (continues to j=10)
Outer: i=2
  Inner: j=1 → Print 2
  Inner: j=2 → Print 4
  ... (and so on)
```

**Example 2: Rectangle pattern**

```cpp
int rows = 4;
int cols = 6;

for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        cout << "* ";
    }
    cout << endl;
}

// Output:
// * * * * * *
// * * * * * *
// * * * * * *
// * * * * * *
```

**Example 3: Right triangle**

```cpp
int rows = 5;

for (int i = 1; i <= rows; i++) {
    for (int j = 1; j <= i; j++) {
        cout << "* ";
    }
    cout << endl;
}

// Output:
// *
// * *
// * * *
// * * * *
// * * * * *
```

**Example 4: Number pyramid**

```cpp
int rows = 4;

for (int i = 1; i <= rows; i++) {
    for (int j = 1; j <= i; j++) {
        cout << j << " ";
    }
    cout << endl;
}

// Output:
// 1
// 1 2
// 1 2 3
// 1 2 3 4
```

### Complete Example: Advanced Patterns

```cpp
#include <iostream>
using namespace std;

int main() {
    int n = 5;
    
    // Pattern 1: Pyramid
    cout << "--- Pyramid ---" << endl;
    for (int i = 1; i <= n; i++) {
        // Print spaces
        for (int j = 1; j <= n - i; j++) {
            cout << " ";
        }
        // Print stars
        for (int j = 1; j <= 2 * i - 1; j++) {
            cout << "*";
        }
        cout << endl;
    }
    
    // Pattern 2: Diamond
    cout << "\n--- Diamond ---" << endl;
    // Upper half
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n - i; j++) {
            cout << " ";
        }
        for (int j = 1; j <= 2 * i - 1; j++) {
            cout << "*";
        }
        cout << endl;
    }
    // Lower half
    for (int i = n - 1; i >= 1; i--) {
        for (int j = 1; j <= n - i; j++) {
            cout << " ";
        }
        for (int j = 1; j <= 2 * i - 1; j++) {
            cout << "*";
        }
        cout << endl;
    }
    
    // Pattern 3: Hollow rectangle
    cout << "\n--- Hollow Rectangle ---" << endl;
    int rows = 5, cols = 8;
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= cols; j++) {
            if (i == 1 || i == rows || j == 1 || j == cols) {
                cout << "* ";
            } else {
                cout << "  ";
            }
        }
        cout << endl;
    }
    
    return 0;
}
```

### Nested Loop with break/continue

```cpp
// Find first pair that sums to 10
bool found = false;

for (int i = 1; i <= 10 && !found; i++) {
    for (int j = 1; j <= 10; j++) {
        if (i + j == 10) {
            cout << "Found: " << i << " + " << j << " = 10" << endl;
            found = true;
            break;  // Breaks inner loop only
        }
    }
}
```

---

## Choosing the Right Loop

### Decision Guide

**Use for loop when**:

- You know exact number of iterations
- Counting with a counter variable
- Processing arrays/collections
- Generating patterns

**Use while loop when**:

- Number of iterations is unknown
- Loop depends on user input
- Processing until condition changes
- Reading files until end

**Use do-while loop when**:

- Need to execute at least once
- Menu systems
- Input validation
- Asking for confirmation

### Comparison Table

|Feature|for|while|do-while|
|---|---|---|---|
|**Check condition**|Before each iteration|Before each iteration|After each iteration|
|**Minimum executions**|0 (if condition false)|0 (if condition false)|1 (always)|
|**Best for**|Counting, known iterations|Unknown iterations|At least one execution|
|**Syntax**|Compact|Simple|Simple|
|**Common use**|Arrays, patterns, counting|Validation, file reading|Menus, input validation|

---

## Practical Examples and Projects

### Example 1: Prime Number Checker

```cpp
#include <iostream>
using namespace std;

int main() {
    int number;
    bool isPrime = true;
    
    cout << "Enter a number: ";
    cin >> number;
    
    if (number <= 1) {
        isPrime = false;
    } else {
        // Check for factors from 2 to sqrt(number)
        for (int i = 2; i * i <= number; i++) {
            if (number % i == 0) {
                isPrime = false;
                break;  // Found a factor, not prime
            }
        }
    }
    
    if (isPrime) {
        cout << number << " is a PRIME number." << endl;
    } else {
        cout << number << " is NOT a prime number." << endl;
    }
    
    return 0;
}
```

### Example 2: Fibonacci Sequence

```cpp
#include <iostream>
using namespace std;

int main() {
    int n;
    
    cout << "How many Fibonacci numbers to generate? ";
    cin >> n;
    
    if (n <= 0) {
        cout << "Invalid input!" << endl;
        return 1;
    }
    
    long long first = 0, second = 1, next;
    
    cout << "Fibonacci Sequence: ";
    
    for (int i = 1; i <= n; i++) {
        if (i == 1) {
            cout << first << " ";
        } else if (i == 2) {
            cout << second << " ";
        } else {
            next = first + second;
            cout << next << " ";
            first = second;
            second = next;
        }
    }
    
    cout << endl;
    
    return 0;
}
```

### Example 3: Simple Banking System

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    double balance = 1000.00;
    int choice;
    double amount;
    bool running = true;
    
    cout << "=== Simple Bank ===" << endl;
    cout << "Starting balance: $" << balance << endl;
    
    while (running) {
        cout << "\n--- Menu ---" << endl;
        cout << "1. Deposit" << endl;
        cout << "2. Withdraw" << endl;
        cout << "3. Check Balance" << endl;
        cout << "4. Transaction History" << endl;
        cout << "5. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Enter deposit amount: $";
                cin >> amount;
                
                if (amount > 0) {
                    balance += amount;
                    cout << "Deposited $" << amount << endl;
                } else {
                    cout << "Invalid amount!" << endl;
                }
                break;
            
            case 2:
                cout << "Enter withdrawal amount: $";
                cin >> amount;
                
                if (amount > 0 && amount <= balance) {
                    balance -= amount;
                    cout << "Withdrawn $" << amount << endl;
                } else if (amount > balance) {
                    cout << "Insufficient funds!" << endl;
                } else {
                    cout << "Invalid amount!" << endl;
                }
                break;
            
            case 3:
                cout << fixed << setprecision(2);
                cout << "Current balance: $" << balance << endl;
                break;
            
            case 4:
                cout << "Feature coming soon!" << endl;
                break;
            
            case 5:
                cout << "Thank you for banking with us!" << endl;
                running = false;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
                break;
        }
    }
    
    return 0;
}
```

### Example 4: Grade Statistics Calculator

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    int numStudents;
    double score, sum = 0, highest = 0, lowest = 100;
    int countA = 0, countB = 0, countC = 0, countD = 0, countF = 0;
    
    cout << "How many students? ";
    cin >> numStudents;
    
    if (numStudents <= 0) {
        cout << "Invalid number of students!" << endl;
        return 1;
    }
    
    // Input scores
    for (int i = 1; i <= numStudents; i++) {
        do {
            cout << "Enter score for student " << i << " (0-100): ";
            cin >> score;
            
            if (score < 0 || score > 100) {
                cout << "Invalid score! Try again." << endl;
            }
        } while (score < 0 || score > 100);
        
        // Update statistics
        sum += score;
        
        if (score > highest) highest = score;
        if (score < lowest) lowest = score;
        
        // Count grades
        if (score >= 90) countA++;
        else if (score >= 80) countB++;
        else if (score >= 70) countC++;
        else if (score >= 60) countD++;
        else countF++;
    }
    
    // Calculate average
    double average = sum / numStudents;
    
    // Display results
    cout << fixed << setprecision(2);
    cout << "\n=== Grade Statistics ===" << endl;
    cout << "Number of students: " << numStudents << endl;
    cout << "Average score: " << average << endl;
    cout << "Highest score: " << highest << endl;
    cout << "Lowest score: " << lowest << endl;
    
    cout << "\n--- Grade Distribution ---" << endl;
    cout << "A (90-100): " << countA << " students" << endl;
    cout << "B (80-89):  " << countB << " students" << endl;
    cout << "C (70-79):  " << countC << " students" << endl;
    cout << "D (60-69):  " << countD << " students" << endl;
    cout << "F (0-59):   " << countF << " students" << endl;
    
    return 0;
}
```

### Example 5: Simple Calendar Display

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    int month, year, startDay, daysInMonth;
    
    cout << "Enter month (1-12): ";
    cin >> month;
    
    cout << "Enter year: ";
    cin >> year;
    
    cout << "Enter starting day (0=Sunday, 1=Monday, ..., 6=Saturday): ";
    cin >> startDay;
    
    // Determine days in month
    if (month == 2) {
        // Check leap year
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            daysInMonth = 29;
        } else {
            daysInMonth = 28;
        }
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        daysInMonth = 30;
    } else {
        daysInMonth = 31;
    }
    
    // Display calendar
    cout << "\n--- Calendar ---" << endl;
    cout << "Sun Mon Tue Wed Thu Fri Sat" << endl;
    
    // Print leading spaces
    for (int i = 0; i < startDay; i++) {
        cout << "    ";
    }
    
    // Print days
    int currentDay = startDay;
    for (int day = 1; day <= daysInMonth; day++) {
        cout << setw(3) << day << " ";
        currentDay++;
        
        if (currentDay % 7 == 0) {
            cout << endl;
        }
    }
    
    cout << endl;
    
    return 0;
}
```

---

## Common Loop Patterns

### Pattern 1: Accumulator (Sum/Product)

```cpp
// Sum
int sum = 0;
for (int i = 1; i <= n; i++) {
    sum += i;
}

// Product
int product = 1;
for (int i = 1; i <= n; i++) {
    product *= i;
}
```

### Pattern 2: Counter

```cpp
int count = 0;
for (int i = 0; i < n; i++) {
    if (condition) {
        count++;
    }
}
```

### Pattern 3: Find Maximum/Minimum

```cpp
int max = numbers[0];
for (int i = 1; i < size; i++) {
    if (numbers[i] > max) {
        max = numbers[i];
    }
}
```

### Pattern 4: Search

```cpp
bool found = false;
int position = -1;

for (int i = 0; i < size; i++) {
    if (numbers[i] == target) {
        found = true;
        position = i;
        break;
    }
}
```

### Pattern 5: Validation Loop

```cpp
int input;
do {
    cout << "Enter valid input: ";
    cin >> input;
    
    if (invalid) {
        cout << "Error!" << endl;
    }
} while (invalid);
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Infinite Loop (Forgot to Update)

**Wrong**:

```cpp
int count = 0;

while (count < 10) {
    cout << count << endl;
    // Forgot count++!
    // Runs forever!
}
```

**Correct**:

```cpp
int count = 0;

while (count < 10) {
    cout << count << endl;
    count++;  // Update the counter
}
```

### Mistake 2: Off-by-One Error

**Wrong**:

```cpp
// Meant to print 1 to 10, but prints 1 to 9
for (int i = 1; i < 10; i++) {
    cout << i << " ";
}
```

**Correct**:

```cpp
for (int i = 1; i <= 10; i++) {  // Use <= not <
    cout << i << " ";
}
```

### Mistake 3: Semicolon After Loop Header

**Wrong**:

```cpp
for (int i = 0; i < 5; i++);  // ⚠️ Semicolon here!
{
    cout << i << endl;  // Never executes with i
}
```

**Correct**:

```cpp
for (int i = 0; i < 5; i++) {  // No semicolon
    cout << i << endl;
}
```

### Mistake 4: Modifying Loop Variable Incorrectly

**Wrong**:

```cpp
for (int i = 0; i < 10; i++) {
    cout << i << endl;
    i++;  // ⚠️ Additional increment - skips numbers!
}
// Prints: 0 2 4 6 8
```

**Correct**:

```cpp
for (int i = 0; i < 10; i++) {
    cout << i << endl;
    // Don't modify i inside loop when using for
}
```

### Mistake 5: Wrong Loop Type

**Wrong** (using while when for is better):

```cpp
int i = 0;
while (i < 10) {
    cout << i << endl;
    i++;
}
```

**Better**:

```cpp
for (int i = 0; i < 10; i++) {
    cout << i << endl;
}
```

### Mistake 6: Not Initializing Variables

**Wrong**:

```cpp
int sum;  // Not initialized!

for (int i = 0; i < 10; i++) {
    sum += i;  // Adds to garbage value
}
```

**Correct**:

```cpp
int sum = 0;  // Initialize to 0

for (int i = 0; i < 10; i++) {
    sum += i;
}
```

### Mistake 7: Nested Loop Logic Errors

**Wrong**:

```cpp
// Meant to print 5x5 square, but prints 5x1
for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; i++) {  // ⚠️ Incrementing i instead of j!
        cout << "* ";
    }
    cout << endl;
}
```

**Correct**:

```cpp
for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {  // Increment j
        cout << "* ";
    }
    cout << endl;
}
```

---

## Practice Exercises

### Exercise 1: Multiplication Table Generator

**Requirements**:

- Ask user for a number (1-12)
- Display full multiplication table for that number
- Format output neatly
- Ask if they want another table

### Exercise 2: Perfect Number Finder

**Requirements**:

- A perfect number equals the sum of its divisors (excluding itself)
- Example: 6 = 1 + 2 + 3
- Find all perfect numbers between 1 and 10,000
- Display each perfect number and its divisors

### Exercise 3: Password Strength Checker

**Requirements**:

- Ask user to create a password
- Check:
    - At least 8 characters
    - Contains uppercase letter
    - Contains lowercase letter
    - Contains digit
    - Contains special character
- Keep asking until password meets all requirements
- Show which requirements are missing

### Exercise 4: Number Pyramid Generator

**Requirements**:

- Ask user for height
- Generate patterns:
    - Right triangle of numbers
    - Inverted triangle
    - Full pyramid
    - Diamond shape
- Let user choose pattern type

### Exercise 5: Simple Calculator with History

**Requirements**:

- Menu: Add, Subtract, Multiply, Divide, Show History, Clear History, Exit
- Store last 10 calculations
- Display them when requested
- Continue until user exits
- Validate all inputs

### Exercise 6: GCD and LCM Calculator

**Requirements**:

- Ask for two numbers
- Calculate GCD using Euclidean algorithm
- Calculate LCM using: LCM = (a × b) / GCD
- Display both results
- Ask if they want to calculate again

### Exercise 7: Collatz Conjecture

**Requirements**:

- Ask for a starting number
- Apply rules:
    - If even: divide by 2
    - If odd: multiply by 3 and add 1
- Continue until reaching 1
- Display the sequence
- Count how many steps it took

### Exercise 8: Prime Numbers in Range

**Requirements**:

- Ask for start and end numbers
- Find all prime numbers in that range
- Display them 10 per line
- Show total count
- Calculate percentage of primes in range

---

## Summary and Best Practices

### Key Takeaways

1. **for loop** - Best for known number of iterations
2. **while loop** - Best for unknown iterations
3. **do-while loop** - Execute at least once
4. **break** - Exit loop immediately
5. **continue** - Skip to next iteration
6. **Nested loops** - Loops inside loops

### Loop Selection Flowchart

```
Do you know exact number of iterations?
  ├─ Yes → Use for loop
  └─ No → Must execute at least once?
           ├─ Yes → Use do-while loop
           └─ No → Use while loop
```

### Best Practices Checklist

✅ **Initialize loop variables** - Don't use garbage values  
✅ **Update loop condition** - Avoid infinite loops  
✅ **Use appropriate loop type** - Choose based on situation  
✅ **Be careful with semicolons** - No semicolon after loop header  
✅ **Watch for off-by-one errors** - Test boundary conditions  
✅ **Indent nested loops** - Make structure clear  
✅ **Use break sparingly** - Makes code harder to follow  
✅ **Initialize accumulators** - sum = 0, product = 1  
✅ **Comment complex loops** - Explain what they do  
✅ **Test with edge cases** - 0, 1, large numbers

### Quick Reference

**for loop**:

```cpp
for (int i = 0; i < n; i++) {
    // code
}
```

**while loop**:

```cpp
while (condition) {
    // code
    // update condition
}
```

**do-while loop**:

```cpp
do {
    // code
    // update condition
} while (condition);
```

**break**:

```cpp
if (condition) {
    break;  // exit loop
}
```

**continue**:

```cpp
if (condition) {
    continue;  // skip to next iteration
}
```

---

## Conclusion

Loops are essential for:

- **Automating repetitive tasks**
- **Processing collections of data**
- **Validating user input**
- **Creating interactive programs**
- **Generating patterns and sequences**

**Mastering loops enables you to**:

- Process arrays and data structures
- Create games and interactive applications
- Validate and sanitize input
- Implement complex algorithms
- Build efficient, powerful programs

**Next steps**:

1. Practice all the exercises
2. Experiment with different loop types
3. Combine loops with conditionals
4. Move on to learning **Functions** (code reusability)

With loops mastered, you can now create programs that automate tasks and process large amounts of data efficiently!

**Happy Coding!** 🚀