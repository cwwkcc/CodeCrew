## What is a Variable?

A **variable** is like a labeled container or box in your computer's memory where you can store information. Just like you might have a box labeled "photos" to store your pictures, a variable has a name and stores data that your program can use and change.

Think of variables as the basic building blocks of programming - they're where you keep all the information your program needs to work with.

### Real-World Analogy

Imagine you're organizing your desk:

- **A drawer labeled "Pens"** → stores pens (similar to an integer variable storing numbers)
- **A folder labeled "Documents"** → stores papers (similar to a string variable storing text)
- **A jar labeled "Coins"** → stores money (similar to a float variable storing decimal numbers)

In C++, you create these "labeled containers" to store different types of information your program needs.

---

## Why Do We Need Variables?

### 1. Store Information

Variables let you save data so you can use it later in your program.

**Example**: Store a user's name, age, or score in a game.

### 2. Remember Values

Programs need to remember things between different parts of the code.

**Example**: Calculate a total price and use it multiple times for different purposes.

### 3. Make Changes

Variables can change their values as the program runs.

**Example**: Update a player's score every time they collect a point.

### 4. Make Code Readable

Using meaningful variable names makes your code easier to understand.

**Bad**: `int x = 25;`  
**Good**: `int studentAge = 25;`

### 5. Reusability

Store a value once and use it many times throughout your program.

**Example**: Store tax rate once, use it for all calculations.

---

## Parts of a Variable

Every variable in C++ has three important parts:

### 1. Data Type

Tells the computer what kind of information the variable will store (numbers, text, true/false, etc.).

**Example**: `int`, `float`, `char`, `string`, `bool`

### 2. Name (Identifier)

The label you give to the variable so you can refer to it in your code.

**Example**: `age`, `studentName`, `totalPrice`, `isGameOver`

### 3. Value

The actual data stored in the variable.

**Example**: `25`, `3.14`, `"Hello"`, `true`

### Complete Variable Declaration

```cpp
int age = 25;
```

Breaking it down:

- `int` → Data type (integer/whole number)
- `age` → Variable name
- `=` → Assignment operator (stores the value)
- `25` → The value being stored
- `;` → Semicolon (ends the statement)

---

## How to Create Variables in C++

### Declaration (Creating the Variable)

Telling C++ that you want to create a variable:

```cpp
int age;  // Created a variable named 'age' that can store whole numbers
```

At this point, the variable exists but doesn't have a specific value yet.

### Initialization (Giving it a Value)

Giving the variable its first value:

```cpp
int age = 25;  // Created and immediately gave it the value 25
```

Or you can do it in two steps:

```cpp
int age;       // Declaration
age = 25;      // Initialization
```

### Assignment (Changing the Value)

Changing the value of a variable after it's been created:

```cpp
int age = 25;   // Initial value
age = 26;       // Changed to 26 (maybe it's their birthday!)
```

---

## Data Types in C++

C++ is a **strongly-typed** language, which means you must specify what type of data each variable will hold. Let's explore the main data types:

### 1. Integer Types (Whole Numbers)

Used for counting, ages, quantities - any whole number without decimals.

#### int (Integer)

The most common type for whole numbers.

```cpp
int age = 25;
int score = 100;
int temperature = -5;
int population = 1000000;
```

**Size**: Usually 4 bytes (can store numbers from about -2 billion to +2 billion)

**When to use**:

- Counting items
- Storing ages
- Game scores
- Year numbers
- Any whole number

#### short (Short Integer)

For smaller whole numbers that don't need much space.

```cpp
short year = 2024;
short daysInMonth = 31;
```

**Size**: Usually 2 bytes (can store -32,768 to 32,767)

**When to use**: When you know numbers will be small and want to save memory.

#### long (Long Integer)

For very large whole numbers.

```cpp
long worldPopulation = 8000000000L;
long distanceToSun = 149600000L;
```

**Size**: Usually 4 or 8 bytes

**When to use**: Very large numbers that don't fit in regular int.

#### long long (Extra Long Integer)

For extremely large whole numbers.

```cpp
long long bigNumber = 9223372036854775807LL;
```

**Size**: 8 bytes (massive range!)

**When to use**: Scientific calculations, cryptography, very large quantities.

#### unsigned Variations

Adding `unsigned` before any integer type means it can only store positive numbers (0 and above), but can store larger positive values.

```cpp
unsigned int positiveOnly = 50;        // 0 to ~4 billion
unsigned short smallPositive = 100;    // 0 to 65,535
unsigned long largePositive = 1000000; // 0 to very large
```

**When to use**: When you know the value will never be negative (like population count, array sizes).

### 2. Floating-Point Types (Decimal Numbers)

Used for numbers with decimal points - prices, measurements, scientific data.

#### float (Floating Point)

For decimal numbers with moderate precision.

```cpp
float price = 19.99f;
float temperature = 36.6f;
float pi = 3.14159f;
```

**Size**: 4 bytes  
**Precision**: About 6-7 decimal digits

**Note**: Always add `f` at the end of float numbers!

**When to use**:

- Prices
- Temperatures
- Percentages
- Graphics coordinates

#### double (Double Precision)

For decimal numbers with high precision.

```cpp
double preciseValue = 3.14159265359;
double scientificValue = 6.022e23;  // Scientific notation
double accountBalance = 1234567.89;
```

**Size**: 8 bytes  
**Precision**: About 15-16 decimal digits

**When to use**:

- Scientific calculations
- Financial applications requiring precision
- Mathematical constants
- When float isn't accurate enough

#### long double (Extended Precision)

For extremely precise decimal numbers.

```cpp
long double veryPrecise = 3.14159265358979323846L;
```

**Size**: Usually 8-16 bytes  
**Precision**: Even more than double

**When to use**: Extremely precise scientific or mathematical calculations.

### 3. Character Type

Used for storing single characters - letters, digits, symbols.

#### char (Character)

Stores a single character.

```cpp
char grade = 'A';
char symbol = '@';
char digit = '9';
char letter = 'x';
```

**Size**: 1 byte

**Important**:

- Single characters use single quotes `'A'`
- Can also store small numbers (-128 to 127)

**When to use**:

- Single letters
- Symbols
- Yes/No responses (Y/N)
- Grade letters

**Special characters** (escape sequences):

```cpp
char newline = '\n';      // New line
char tab = '\t';          // Tab
char backslash = '\\';    // Backslash
char quote = '\'';        // Single quote
```

### 4. Boolean Type (True/False)

Used for storing true or false values - perfect for yes/no, on/off situations.

#### bool (Boolean)

Stores only two values: `true` or `false`.

```cpp
bool isRaining = true;
bool isGameOver = false;
bool hasPermission = true;
bool isStudent = false;
```

**Size**: 1 byte (but only uses 1 bit of information!)

**When to use**:

- Flags and switches
- Status checks
- Conditions
- Game states (is player alive, is level complete)

**Important**:

- `true` and `false` are keywords (lowercase, no quotes)
- In conditions, 0 = false, anything else = true

### 5. String Type (Text)

Used for storing multiple characters - words, sentences, names.

#### string (String of Characters)

Stores text of any length.

```cpp
#include <string>  // Must include this!

string name = "John Doe";
string message = "Hello, World!";
string email = "user@example.com";
string emptyString = "";
```

**Important**:

- Text uses double quotes `"Hello"`
- Must include `<string>` library at the top of your program
- Technically not a primitive type, but very commonly used

**When to use**:

- Names
- Addresses
- Messages
- Any text longer than one character

---

## Variable Naming Rules and Best Practices

### Rules (Must Follow - or your code won't compile!)

#### 1. Can Contain

- Letters (a-z, A-Z)
- Digits (0-9)
- Underscore (_)

```cpp
int age25;          // ✓ Valid
int student_name;   // ✓ Valid
int _temp;          // ✓ Valid
```

#### 2. Must Start With

- A letter (a-z, A-Z)
- Or an underscore (_)

```cpp
int age;       // ✓ Valid
int _count;    // ✓ Valid
int 9grade;    // ✗ Invalid - starts with digit
```

#### 3. Cannot Contain

- Spaces
- Special characters (@, #, $, %, etc.)

```cpp
int student age;   // ✗ Invalid - has space
int price$;        // ✗ Invalid - has $
int email@;        // ✗ Invalid - has @
```

#### 4. Cannot Use Reserved Keywords

C++ has special words that have specific meanings and can't be used as variable names:

```cpp
int int;      // ✗ Invalid - 'int' is a keyword
int class;    // ✗ Invalid - 'class' is a keyword
int return;   // ✗ Invalid - 'return' is a keyword
```

**Common C++ keywords to avoid**: `int`, `float`, `double`, `char`, `bool`, `if`, `else`, `while`, `for`, `return`, `class`, `void`, `true`, `false`, `new`, `delete`, etc.

#### 5. Case Sensitive

C++ treats uppercase and lowercase as different:

```cpp
int age = 25;
int Age = 30;      // Different variable!
int AGE = 35;      // Also different!
```

These are three separate variables!

### Best Practices (Should Follow - for clean, readable code)

#### 1. Use Meaningful Names

**Bad**:

```cpp
int x = 25;
int a = 1000;
```

**Good**:

```cpp
int studentAge = 25;
int accountBalance = 1000;
```

#### 2. Use camelCase or snake_case

**camelCase** (first word lowercase, capitalize first letter of other words):

```cpp
int studentAge = 20;
int totalPrice = 99.99;
string firstName = "John";
```

**snake_case** (all lowercase with underscores):

```cpp
int student_age = 20;
int total_price = 99.99;
string first_name = "John";
```

Choose one style and be consistent!

#### 3. Don't Make Names Too Short

**Bad**:

```cpp
int a;      // What is 'a'?
int tmp;    // Temporary what?
int x;      // 'x' tells us nothing
```

**Good**:

```cpp
int userAge;
int temporaryStorage;
int coordinateX;
```

#### 4. Don't Make Names Too Long

**Bad**:

```cpp
int theCurrentTemperatureInCelsiusOfTheRoom = 22;
```

**Good**:

```cpp
int roomTemperature = 22;  // or roomTempCelsius
```

#### 5. Use Descriptive Names for Purpose

```cpp
// Good variable names tell you what they're for:
int itemCount = 10;
double accountBalance = 1250.50;
bool isLoggedIn = true;
string userEmail = "user@email.com";
```

#### 6. Boolean Variables Should Ask Questions

Prefix with `is`, `has`, `can`, `should`:

```cpp
bool isValid = true;
bool hasPermission = false;
bool canEdit = true;
bool shouldSave = false;
```

#### 7. Constants Should Be Uppercase

For values that never change:

```cpp
const int MAX_STUDENTS = 100;
const double PI = 3.14159;
const int DAYS_IN_WEEK = 7;
```

---

## Variable Scope

**Scope** determines where in your code a variable can be used. It's like asking "where is this variable visible?"

### Local Variables

Declared inside a function or block - only accessible within that block.

```cpp
#include <iostream>
using namespace std;

int main() {
    int localVariable = 10;  // Local to main()
    
    if (true) {
        int blockVariable = 20;  // Local to this if block
        cout << localVariable;   // ✓ Can access
        cout << blockVariable;   // ✓ Can access
    }
    
    cout << localVariable;   // ✓ Can access
    cout << blockVariable;   // ✗ Error! Outside its scope
    
    return 0;
}
```

### Global Variables

Declared outside all functions - accessible everywhere in the file.

```cpp
#include <iostream>
using namespace std;

int globalVariable = 100;  // Global variable

void function1() {
    cout << globalVariable;  // ✓ Can access
}

void function2() {
    cout << globalVariable;  // ✓ Can access
}

int main() {
    cout << globalVariable;  // ✓ Can access
    return 0;
}
```

**Warning**: Global variables should be used sparingly! They can make code harder to debug and understand.

### Block Scope

Variables declared inside `{ }` braces are only accessible within those braces.

```cpp
int main() {
    int x = 10;
    
    {
        int y = 20;
        cout << x;  // ✓ Works
        cout << y;  // ✓ Works
    }
    
    cout << x;  // ✓ Works
    cout << y;  // ✗ Error! y doesn't exist here
    
    return 0;
}
```

---

## Constants

Constants are variables whose values cannot be changed after initialization. Think of them as "read-only" variables.

### Using const Keyword

```cpp
const int MAX_SCORE = 100;
const double PI = 3.14159;
const string COMPANY_NAME = "TechCorp";

// Trying to change will cause an error:
MAX_SCORE = 200;  // ✗ Error! Cannot modify a const
```

### Why Use Constants?

1. **Prevent accidental changes**: Values that should never change
2. **Make code clearer**: Reader knows this value is fixed
3. **Easy to update**: Change in one place, affects everywhere
4. **Performance**: Compiler can optimize better

```cpp
// Instead of using "magic numbers" throughout code:
if (score > 100) { }
if (points > 100) { }
if (total > 100) { }

// Use a constant:
const int MAX_SCORE = 100;
if (score > MAX_SCORE) { }
if (points > MAX_SCORE) { }
if (total > MAX_SCORE) { }
// Now if max changes to 200, change it in ONE place!
```

---

## Type Conversion (Casting)

Sometimes you need to convert one data type to another.

### Implicit Conversion (Automatic)

C++ automatically converts when it's "safe" (no data loss):

```cpp
int wholeNumber = 10;
double decimalNumber = wholeNumber;  // int → double (safe)
// decimalNumber is now 10.0

char letter = 'A';
int asciiValue = letter;  // char → int
// asciiValue is 65 (ASCII code of 'A')
```

### Explicit Conversion (Manual Casting)

You tell C++ to convert, even if data might be lost:

```cpp
double price = 19.99;
int roundedPrice = (int)price;  // Loses .99, becomes 19

// Another syntax:
int wholePrice = int(price);    // Same result

// Modern C++ style:
int modernPrice = static_cast<int>(price);
```

**Warning**: Be careful! You can lose data:

```cpp
double precise = 3.14159;
int notPrecise = (int)precise;  // Becomes 3 - lost .14159!

int large = 1000;
char small = (char)large;  // Might lose data - char is tiny!
```

---

## Common Operations with Variables

### Arithmetic Operations

```cpp
int a = 10;
int b = 3;

int sum = a + b;         // 13 (addition)
int difference = a - b;  // 7 (subtraction)
int product = a * b;     // 30 (multiplication)
int quotient = a / b;    // 3 (division - integer division!)
int remainder = a % b;   // 1 (modulus - remainder)

double preciseDiv = (double)a / b;  // 3.33333...
```

### Assignment Operations

```cpp
int score = 100;

score = score + 10;  // Add 10, score is now 110
score += 10;         // Shorthand for above, now 120

score = score - 5;   // Subtract 5
score -= 5;          // Shorthand

score = score * 2;   // Multiply by 2
score *= 2;          // Shorthand

score = score / 2;   // Divide by 2
score /= 2;          // Shorthand

score = score % 3;   // Modulus
score %= 3;          // Shorthand
```

### Increment and Decrement

```cpp
int count = 5;

count++;    // Increment by 1, count is now 6
count--;    // Decrement by 1, count is now 5

++count;    // Same as count++, but used in different contexts
--count;    // Same as count--
```

**Difference between `++` before and after**:

```cpp
int a = 5;
int b = a++;  // b gets 5, then a becomes 6 (post-increment)

int x = 5;
int y = ++x;  // x becomes 6, then y gets 6 (pre-increment)
```

---

## Working with Multiple Variables

### Declaring Multiple Variables

```cpp
// Same type, separate lines:
int age;
int score;
int level;

// Same type, same line:
int age, score, level;

// Same type, same line with values:
int age = 25, score = 100, level = 5;

// Different types (must be separate):
int age = 25;
double salary = 50000.50;
string name = "John";
```

### Swapping Variables

```cpp
int a = 5;
int b = 10;

// Method 1: Using temporary variable
int temp = a;
a = b;
b = temp;
// Now a = 10, b = 5

// Method 2: Without temporary (mathematical trick)
a = a + b;  // a = 15
b = a - b;  // b = 5
a = a - b;  // a = 10
```

---

## Input and Output with Variables

### Output (Displaying Variables)

```cpp
#include <iostream>
using namespace std;

int age = 25;
string name = "Alice";
double salary = 50000.50;

cout << "Name: " << name << endl;
cout << "Age: " << age << endl;
cout << "Salary: $" << salary << endl;

// Output:
// Name: Alice
// Age: 25
// Salary: $50000.50
```

### Input (Getting User Input)

```cpp
#include <iostream>
using namespace std;

int age;
string name;
double salary;

cout << "Enter your name: ";
cin >> name;  // Reads until space

cout << "Enter your age: ";
cin >> age;

cout << "Enter your salary: ";
cin >> salary;

cout << "Hello " << name << ", age " << age << endl;
```

### Reading Strings with Spaces

```cpp
#include <iostream>
#include <string>
using namespace std;

string fullName;

cout << "Enter your full name: ";
getline(cin, fullName);  // Reads entire line including spaces

cout << "Hello, " << fullName << endl;
```

---

## Common Mistakes and How to Avoid Them

### 1. Uninitialized Variables

**Problem**:

```cpp
int score;
cout << score;  // Prints random garbage value!
```

**Solution**:

```cpp
int score = 0;  // Always initialize!
cout << score;  // Prints 0
```

### 2. Integer Division Surprise

**Problem**:

```cpp
int result = 5 / 2;  // result is 2, not 2.5!
```

**Why**: Both are integers, so C++ does integer division (throws away decimal).

**Solution**:

```cpp
double result = 5.0 / 2.0;  // result is 2.5
// or
double result = (double)5 / 2;  // Cast one to double
```

### 3. Mixing Up = and ==

**Problem**:

```cpp
int x = 10;
if (x = 5) {  // Assigns 5 to x, doesn't compare!
    // This always runs
}
```

**Solution**:

```cpp
if (x == 5) {  // Correctly compares x with 5
    // Runs only if x equals 5
}
```

### 4. Forgetting Semicolons

**Problem**:

```cpp
int age = 25  // Missing semicolon
```

**Solution**:

```cpp
int age = 25;  // Always end statements with ;
```

### 5. Using Variables Outside Scope

**Problem**:

```cpp
if (true) {
    int temp = 10;
}
cout << temp;  // Error! temp doesn't exist here
```

**Solution**:

```cpp
int temp;  // Declare outside if you need it outside
if (true) {
    temp = 10;
}
cout << temp;  // Works now
```

### 6. Type Mismatch

**Problem**:

```cpp
int age = "25";  // Error! Can't put string in int
```

**Solution**:

```cpp
int age = 25;  // Use correct type
// or
string age = "25";  // If you really need a string
```

---

## Practical Examples

### Example 1: Simple Calculator

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
    
    if (operation == '+') {
        result = num1 + num2;
    } else if (operation == '-') {
        result = num1 - num2;
    } else if (operation == '*') {
        result = num1 * num2;
    } else if (operation == '/') {
        result = num1 / num2;
    }
    
    cout << "Result: " << result << endl;
    
    return 0;
}
```

### Example 2: Temperature Converter

```cpp
#include <iostream>
using namespace std;

int main() {
    double celsius, fahrenheit;
    
    cout << "Enter temperature in Celsius: ";
    cin >> celsius;
    
    fahrenheit = (celsius * 9.0 / 5.0) + 32;
    
    cout << celsius << "°C = " << fahrenheit << "°F" << endl;
    
    return 0;
}
```

### Example 3: Age Calculator

```cpp
#include <iostream>
using namespace std;

int main() {
    int birthYear, currentYear, age;
    
    cout << "Enter current year: ";
    cin >> currentYear;
    
    cout << "Enter your birth year: ";
    cin >> birthYear;
    
    age = currentYear - birthYear;
    
    cout << "You are " << age << " years old." << endl;
    
    return 0;
}
```

### Example 4: Circle Area Calculator

```cpp
#include <iostream>
using namespace std;

int main() {
    const double PI = 3.14159;
    double radius, area;
    
    cout << "Enter circle radius: ";
    cin >> radius;
    
    area = PI * radius * radius;
    
    cout << "Circle area: " << area << endl;
    
    return 0;
}
```

---

## Memory and Storage

### How Variables are Stored

When you create a variable, the computer allocates memory space for it:

```cpp
char letter = 'A';      // Uses 1 byte of memory
int age = 25;           // Uses 4 bytes (typically)
double price = 19.99;   // Uses 8 bytes
```

### Checking Variable Size

```cpp
#include <iostream>
using namespace std;

int main() {
    cout << "Size of int: " << sizeof(int) << " bytes" << endl;
    cout << "Size of double: " << sizeof(double) << " bytes" << endl;
    cout << "Size of char: " << sizeof(char) << " bytes" << endl;
    
    int age = 25;
    cout << "Size of age variable: " << sizeof(age) << " bytes" << endl;
    
    return 0;
}
```

### Variable Addresses

Every variable is stored at a specific memory address (location):

```cpp
int age = 25;
cout << "Value: " << age << endl;
cout << "Address: " << &age << endl;  // & gives memory address

// Output might be:
// Value: 25
// Address: 0x7ffeeb3c4a8c (hexadecimal memory address)
```

---

## Summary: Key Takeaways

### What We've Learned

1. **Variables are containers**: They store data your program needs
2. **Every variable has a type**: int, double, char, bool, string, etc.
3. **Names matter**: Use meaningful, descriptive names
4. **Scope is important**: Variables are only accessible where they're defined
5. **Initialize your variables**: Don't use random values
6. **Constants are useful**: For values that shouldn't change

### Essential Concepts

- **Declaration**: Creating a variable
- **Initialization**: Giving it a first value
- **Assignment**: Changing its value
- **Data Type**: What kind of data it holds
- **Scope**: Where the variable can be used
- **Casting**: Converting between types

### Variable Checklist

When creating a variable, ask yourself:

1. ✓ What type of data am I storing? (Choose appropriate data type)
2. ✓ What should I name it? (Use meaningful name)
3. ✓ Where will I use it? (Consider scope)
4. ✓ Will it change? (Consider using const if not)
5. ✓ Did I initialize it? (Don't use garbage values)

### Common Variable Patterns

```cpp
// Reading input:
int userInput;
cin >> userInput;

// Calculations:
double result = value1 + value2;

// Conditions:
bool isValid = (score > 0);

// Counting:
int count = 0;
count++;

// Accumulating:
int total = 0;
total += newValue;
```

---

## Next Steps

Now that you understand variables, you can:

1. **Practice**: Create small programs using different variable types
2. **Experiment**: Try all data types and operations
3. **Learn**: Move on to operators, conditionals, and loops
4. **Build**: Start creating simple programs that use variables

### Practice Exercises

Try creating programs that:

1. Calculate the area and perimeter of a rectangle
2. Convert kilometers to miles
3. Swap two numbers
4. Calculate simple interest
5. Find the average of three numbers
6. Check if a number is even or odd
7. Calculate your age in days
8. Convert hours to seconds

---

## Final Thoughts

Variables are the foundation of programming. Every program you write will use variables to store and manipulate data. Understanding how to properly declare, initialize, and use variables is crucial for becoming a proficient programmer.

Remember:

- Start with simple examples
- Always initialize your variables
- Use meaningful names
- Choose the right data type for your needs
- Practice, practice, practice!

As you continue learning C++, you'll see variables used in increasingly sophisticated ways, but the fundamentals remain the same - they're containers that hold data your program needs to work with.

**Happy coding!** 🚀