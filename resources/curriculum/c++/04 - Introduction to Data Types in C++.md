## What is a Data Type?

A **data type** tells the computer what kind of information you're storing and how much space to set aside for it. Think of it as choosing the right container for different items:

- **A water bottle** → for liquids (like `double` for decimal numbers)
- **A piggy bank** → for coins (like `int` for whole numbers)
- **A letter envelope** → for messages (like `string` for text)
- **A light switch** → for on/off (like `bool` for true/false)

Just as you wouldn't try to store water in an envelope or a letter in a piggy bank, you need to choose the right data type for the information you want to store.

---

## Why Do We Need Data Types?

### 1. Memory Management

Different types of data need different amounts of storage space. Data types help the computer know exactly how much memory to allocate.

**Example**:

- A single character like 'A' needs only 1 byte
- A whole number might need 4 bytes
- A decimal number might need 8 bytes

### 2. Prevent Errors

Data types prevent you from mixing incompatible operations.

**Example**:

```cpp
int age = "twenty-five";  // ✗ Error! Can't put text in a number variable
```

This error-checking helps you catch mistakes early.

### 3. Optimize Performance

When the compiler knows the exact type of data, it can generate more efficient machine code, making your program run faster.

### 4. Code Clarity

Data types make your code self-documenting. When you see `bool isActive`, you immediately know it's a true/false value.

### 5. Type Safety

C++ is a **strongly-typed** language, meaning once you declare a variable as a certain type, it stays that type. This prevents many common programming errors.

---

## Categories of Data Types in C++

C++ data types can be organized into three main categories:

### 1. Primitive (Built-in) Data Types

Basic types provided by C++ itself:

- Integer types (int, short, long, etc.)
- Floating-point types (float, double)
- Character type (char)
- Boolean type (bool)
- Void type (void)

### 2. Derived Data Types

Built from primitive types:

- Arrays
- Pointers
- References
- Functions

### 3. User-Defined Data Types

Created by programmers:

- Structures (struct)
- Classes (class)
- Unions (union)
- Enumerations (enum)

---

## Primitive Data Types in Detail

### Integer Data Types (Whole Numbers)

Integer types store whole numbers without decimal points. C++ provides several sizes to give you flexibility in memory usage vs. range.

#### 1. int (Integer)

The most commonly used integer type.

```cpp
int age = 25;
int score = -10;
int population = 1000000;
int temperature = 0;
```

**Size**: Typically 4 bytes (32 bits)  
**Range**: -2,147,483,648 to 2,147,483,647  
**Memory**: 4 bytes = 32 bits

**When to use**:

- Counting items
- Storing ages
- Game scores
- Year numbers
- Most general-purpose whole numbers

**Visual representation**:

```
Memory: [____4 bytes____]
Can store: -2 billion to +2 billion (approximately)
```

#### 2. short int (or just short)

A smaller integer type for when you don't need large numbers.

```cpp
short year = 2024;
short daysInMonth = 31;
short temperature = -15;
```

**Size**: Typically 2 bytes (16 bits)  
**Range**: -32,768 to 32,767  
**Memory**: 2 bytes = 16 bits

**When to use**:

- Small numbers
- When memory is limited
- Embedded systems
- Large arrays of small numbers

**Visual representation**:

```
Memory: [__2 bytes__]
Can store: -32,768 to 32,767
```

#### 3. long int (or just long)

A larger integer type for bigger numbers.

```cpp
long worldPopulation = 8000000000L;
long distanceKm = 384400L;  // Earth to Moon
long largeNumber = -999999999L;
```

**Size**: At least 4 bytes (often 4 or 8 bytes depending on system)  
**Range**: At least -2,147,483,648 to 2,147,483,647  
**Memory**: 4-8 bytes

**Note**: Add `L` suffix to long literals

**When to use**:

- Large numbers that might exceed int range
- Financial calculations with large values
- Scientific calculations

#### 4. long long int (or just long long)

The largest standard integer type.

```cpp
long long bigNumber = 9223372036854775807LL;
long long starsInUniverse = 1000000000000000000LL;
long long milliseconds = 1609459200000LL;
```

**Size**: 8 bytes (64 bits)  
**Range**: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807  
**Memory**: 8 bytes = 64 bits

**Note**: Add `LL` suffix to long long literals

**When to use**:

- Extremely large numbers
- Timestamps in milliseconds
- Cryptography
- Big data applications
- Astronomical calculations

**Visual representation**:

```
Memory: [________8 bytes________]
Can store: -9 quintillion to +9 quintillion (approximately)
```

#### 5. unsigned Variations

Adding `unsigned` means the variable can only hold positive values (including zero), but doubles the positive range.

```cpp
unsigned int positiveOnly = 4000000000;  // Can't be negative
unsigned short smallPositive = 65000;
unsigned long largePositive = 4000000000UL;
unsigned long long hugePositive = 18000000000000000000ULL;
```

**Why use unsigned?**

- When values will never be negative (counts, sizes, indices)
- Need larger positive range
- Memory addresses
- Bit manipulation

**Comparison**:

```
signed int:   -2 billion to +2 billion
unsigned int:  0 to 4 billion (roughly)
```

**Integer Types Size Comparison Table**:

|Type|Size|Range (Signed)|Range (Unsigned)|
|---|---|---|---|
|short|2 bytes|-32,768 to 32,767|0 to 65,535|
|int|4 bytes|-2.1 billion to 2.1 billion|0 to 4.2 billion|
|long|4-8 bytes|At least ±2.1 billion|At least 0 to 4.2 billion|
|long long|8 bytes|±9.2 quintillion|0 to 18.4 quintillion|

---

### Floating-Point Data Types (Decimal Numbers)

Floating-point types store numbers with decimal points. The term "floating-point" refers to the decimal point that can "float" to different positions.

#### 1. float (Single Precision)

For decimal numbers with moderate precision.

```cpp
float price = 19.99f;
float temperature = 36.6f;
float pi = 3.14159f;
float percentage = 87.5f;
```

**Size**: 4 bytes (32 bits)  
**Precision**: 6-7 decimal digits  
**Range**: ±3.4 × 10³⁸ (approximately)

**Important**: Always add `f` or `F` suffix to float literals!

**When to use**:

- Graphics and game development (coordinates, colors)
- Sensor readings
- Prices and currency (with caution)
- When memory is limited
- Scientific calculations with moderate precision

**Visual representation**:

```
Memory: [____4 bytes____]
Precision: ~6-7 digits
Example: 3.141592 (can accurately represent)
         3.14159265359 (loses precision)
```

**Precision example**:

```cpp
float number = 123456.789f;
// Stored as approximately: 123456.8 (loses some precision)

float veryLarge = 123456789.123456789f;
// Stored as approximately: 123456792.0 (significant precision loss)
```

#### 2. double (Double Precision)

For decimal numbers with high precision.

```cpp
double preciseValue = 3.14159265359;
double scientificValue = 6.022e23;  // Scientific notation: 6.022 × 10²³
double accountBalance = 1234567.89;
double pi = 3.141592653589793;
```

**Size**: 8 bytes (64 bits)  
**Precision**: 15-16 decimal digits  
**Range**: ±1.7 × 10³⁰⁸ (approximately)

**When to use**:

- Scientific calculations
- Financial applications (for precision)
- Mathematical constants
- Geographic coordinates
- **Default choice for decimal numbers**

**Visual representation**:

```
Memory: [________8 bytes________]
Precision: ~15-16 digits
Example: 3.141592653589793 (accurate)
```

#### 3. long double (Extended Precision)

For extremely precise decimal numbers.

```cpp
long double veryPrecise = 3.14159265358979323846L;
long double constant = 2.71828182845904523536L;
long double astronomicalDistance = 1.496e11L;
```

**Size**: Usually 8, 12, or 16 bytes (system-dependent)  
**Precision**: 18-19+ decimal digits  
**Range**: Larger than double

**Note**: Add `L` suffix to long double literals

**When to use**:

- Extremely precise scientific calculations
- Advanced mathematical computations
- When double precision isn't enough
- Research and academic applications

**Floating-Point Types Comparison**:

|Type|Size|Precision|Example Use Case|
|---|---|---|---|
|float|4 bytes|6-7 digits|Game graphics, basic calculations|
|double|8 bytes|15-16 digits|Scientific work, financial apps|
|long double|8-16 bytes|18+ digits|Research, advanced mathematics|

**Special Floating-Point Values**:

```cpp
#include <iostream>
#include <cmath>
using namespace std;

double positive_infinity = INFINITY;
double negative_infinity = -INFINITY;
double not_a_number = NAN;

// Results of certain operations:
double result1 = 1.0 / 0.0;     // inf (infinity)
double result2 = -1.0 / 0.0;    // -inf
double result3 = 0.0 / 0.0;     // nan (not a number)
double result4 = sqrt(-1);      // nan
```

---

### Character Data Type

The character type stores single characters and small integers.

#### char (Character)

Stores a single character or a small integer.

```cpp
char grade = 'A';
char symbol = '@';
char digit = '9';
char letter = 'x';
char newline = '\n';
```

**Size**: 1 byte (8 bits)  
**Range**:

- Signed: -128 to 127
- Unsigned: 0 to 255

**Important Notes**:

- Single characters use **single quotes**: `'A'`
- Strings use double quotes: `"Hello"`
- Internally stored as ASCII/Unicode number

**When to use**:

- Single letters or symbols
- Yes/No responses (Y/N)
- Grade letters (A, B, C, D, F)
- Menu choices
- Single-character input

**Character Storage (ASCII)**:

Behind the scenes, characters are stored as numbers (ASCII codes):

```cpp
char letter = 'A';
int code = letter;  // code = 65 (ASCII code for 'A')

char digit = '5';
int digitCode = digit;  // digitCode = 53 (not 5!)

cout << 'A' << endl;  // Prints: A
cout << (int)'A' << endl;  // Prints: 65
```

**Common ASCII Values**:

```
'0' to '9'  → 48 to 57
'A' to 'Z'  → 65 to 90
'a' to 'z'  → 97 to 122
' ' (space) → 32
'\n' (newline) → 10
```

**Escape Sequences (Special Characters)**:

```cpp
char newline = '\n';      // New line
char tab = '\t';          // Horizontal tab
char backslash = '\\';    // Backslash \
char singleQuote = '\'';  // Single quote '
char doubleQuote = '\"';  // Double quote "
char nullChar = '\0';     // Null character (end of string)
char backspace = '\b';    // Backspace
char carriageReturn = '\r'; // Carriage return
```

**Example using escape sequences**:

```cpp
cout << "Hello\nWorld";     // Hello (new line) World
cout << "Name:\tJohn";      // Name:    John (with tab)
cout << "Path: C:\\folder"; // Path: C:\folder
cout << "She said \"Hi\"";  // She said "Hi"
```

**Character Variants**:

```cpp
char normalChar = 'A';          // 1 byte, ASCII
unsigned char uChar = 200;      // 1 byte, 0-255
signed char sChar = -50;        // 1 byte, -128 to 127

wchar_t wideChar = L'Ω';        // Wide character (Unicode)
char16_t utf16Char = u'€';     // UTF-16 character
char32_t utf32Char = U'😀';    // UTF-32 character
```

---

### Boolean Data Type

The boolean type stores only true or false values.

#### bool (Boolean)

Stores logical values: true or false.

```cpp
bool isRaining = true;
bool isGameOver = false;
bool hasLicense = true;
bool isAdult = false;
bool isPrime = true;
```

**Size**: 1 byte (but only uses 1 bit of information!)  
**Values**: Only `true` or `false`

**When to use**:

- Flags (on/off states)
- Conditions
- Status indicators
- Yes/no questions
- Game states (alive/dead, won/lost)
- Permissions (allowed/denied)

**Important Notes**:

- `true` and `false` are keywords (lowercase, no quotes)
- In numeric context: `false` = 0, `true` = 1 (or any non-zero)
- Any non-zero value converts to `true`
- Zero converts to `false`

**Boolean in Numeric Context**:

```cpp
bool flag = true;
int value = flag;           // value = 1

bool flag2 = false;
int value2 = flag2;         // value2 = 0

// Conversions:
bool b1 = 0;                // false
bool b2 = 1;                // true
bool b3 = 42;               // true (any non-zero)
bool b4 = -5;               // true
```

**Boolean Operations**:

```cpp
bool a = true;
bool b = false;

bool andResult = a && b;    // false (AND - both must be true)
bool orResult = a || b;     // true (OR - at least one true)
bool notResult = !a;        // false (NOT - opposite)

// Comparison results are boolean:
bool isEqual = (5 == 5);           // true
bool isGreater = (10 > 5);         // true
bool isLessOrEqual = (3 <= 2);     // false
bool isNotEqual = (4 != 4);        // false
```

**Practical Boolean Examples**:

```cpp
int age = 18;
bool isAdult = (age >= 18);        // true

int score = 85;
bool passed = (score >= 60);       // true
bool gotA = (score >= 90);         // false

string password = "secret123";
bool isValidLength = (password.length() >= 8);  // true

bool isWeekend = (day == "Saturday" || day == "Sunday");
bool canVote = (age >= 18 && isCitizen == true);
```

---

### Void Data Type

The void type represents "no value" or "no type."

#### void

Indicates absence of data.

```cpp
void printMessage() {
    cout << "Hello!";
    // No return value
}

void greet(string name) {
    cout << "Hi, " << name;
    return;  // Optional for void functions
}
```

**When to use**:

- Functions that don't return a value
- Generic pointers (void*)
- Indicating "nothing" or "no type"

**Important**: You cannot create void variables!

```cpp
void value;  // ✗ Error! Cannot declare void variable
```

---

## Derived Data Types

### 1. Arrays

Collection of elements of the same type stored in contiguous memory.

```cpp
int numbers[5];                    // Array of 5 integers
int scores[5] = {90, 85, 92, 78, 88};
char letters[3] = {'A', 'B', 'C'};
double prices[4] = {19.99, 29.99, 39.99, 49.99};

// Accessing elements:
cout << scores[0];  // First element: 90
cout << scores[4];  // Last element: 88
```

**Key points**:

- Fixed size (declared at compile time)
- Zero-indexed (first element is index 0)
- All elements same type
- Stored in contiguous memory

### 2. Pointers

Variables that store memory addresses.

```cpp
int age = 25;
int* ptr = &age;  // Pointer to int, stores address of age

cout << age;      // Prints: 25 (value)
cout << &age;     // Prints: 0x7ffd5... (address)
cout << ptr;      // Prints: 0x7ffd5... (address stored in ptr)
cout << *ptr;     // Prints: 25 (value at address)
```

**When to use**:

- Dynamic memory allocation
- Passing large data efficiently
- Data structures (linked lists, trees)
- Function parameters (pass by reference)

### 3. References

Aliases (alternative names) for variables.

```cpp
int original = 10;
int& ref = original;  // ref is another name for original

ref = 20;
cout << original;  // Prints: 20 (both changed!)
cout << ref;       // Prints: 20
```

**Difference from pointers**:

- Must be initialized when declared
- Cannot be null
- Cannot be reassigned
- Cleaner syntax

### 4. Functions

Functions have types based on their return value.

```cpp
int add(int a, int b) {        // Returns int
    return a + b;
}

double divide(int a, int b) {  // Returns double
    return (double)a / b;
}

bool isEven(int n) {           // Returns bool
    return (n % 2 == 0);
}

void printHello() {            // Returns nothing (void)
    cout << "Hello!";
}
```

---

## User-Defined Data Types

### 1. Structures (struct)

Group different data types together.

```cpp
struct Student {
    string name;
    int age;
    double gpa;
};

// Usage:
Student student1;
student1.name = "Alice";
student1.age = 20;
student1.gpa = 3.8;

Student student2 = {"Bob", 22, 3.5};

cout << student1.name << " is " << student1.age << " years old.";
```

**When to use**:

- Grouping related data
- Creating simple data containers
- C-style programming
- When you don't need methods

### 2. Classes (class)

Like structures but with more features (encapsulation, methods, access control).

```cpp
class Rectangle {
private:
    double width;
    double height;
    
public:
    Rectangle(double w, double h) {
        width = w;
        height = h;
    }
    
    double area() {
        return width * height;
    }
    
    double perimeter() {
        return 2 * (width + height);
    }
};

// Usage:
Rectangle rect(5.0, 3.0);
cout << "Area: " << rect.area();  // 15.0
```

**When to use**:

- Object-oriented programming
- Complex data with behaviors
- Encapsulation needed
- Inheritance and polymorphism

### 3. Unions (union)

Store different types in the same memory location (only one at a time).

```cpp
union Data {
    int integer;
    float decimal;
    char character;
};

Data data;
data.integer = 10;
cout << data.integer;  // 10

data.decimal = 3.14f;  // Overwrites integer!
cout << data.decimal;  // 3.14
cout << data.integer;  // Garbage value now
```

**When to use**:

- Memory-constrained environments
- Type punning
- Low-level programming
- Embedded systems

### 4. Enumerations (enum)

Define named integer constants.

```cpp
enum Color {
    RED,      // 0
    GREEN,    // 1
    BLUE,     // 2
    YELLOW    // 3
};

Color favoriteColor = RED;

if (favoriteColor == RED) {
    cout << "You like red!";
}

// Custom values:
enum Status {
    SUCCESS = 1,
    ERROR = -1,
    PENDING = 0
};
```

**Strongly-typed enums (C++11)**:

```cpp
enum class TrafficLight {
    RED,
    YELLOW,
    GREEN
};

TrafficLight light = TrafficLight::RED;

if (light == TrafficLight::RED) {
    cout << "Stop!";
}
```

**When to use**:

- Limited set of named options
- State machines
- Menu choices
- Status codes
- Better than magic numbers

---

## Type Modifiers

Modifiers change the properties of basic data types:

### 1. signed (Default for int types)

Can store both positive and negative values.

```cpp
signed int temperature = -10;
signed short altitude = -500;
```

### 2. unsigned

Can only store non-negative values (0 and positive).

```cpp
unsigned int age = 25;          // 0 to 4,294,967,295
unsigned short port = 8080;     // 0 to 65,535
```

### 3. short

Makes integer types smaller (uses less memory).

```cpp
short int smallNumber = 100;
short year = 2024;
```

### 4. long

Makes integer types larger (uses more memory, bigger range).

```cpp
long int bigNumber = 1000000000L;
long population = 8000000000L;
```

### Modifier Combinations

You can combine modifiers:

```cpp
unsigned short int smallPositive = 60000;
unsigned long long int huge = 18000000000000000000ULL;
signed long int largeWithSign = -999999999L;
```

---

## Type Sizes and Ranges

### Checking Type Sizes

```cpp
#include <iostream>
#include <limits>
using namespace std;

int main() {
    cout << "Type Sizes:" << endl;
    cout << "char: " << sizeof(char) << " bytes" << endl;
    cout << "short: " << sizeof(short) << " bytes" << endl;
    cout << "int: " << sizeof(int) << " bytes" << endl;
    cout << "long: " << sizeof(long) << " bytes" << endl;
    cout << "long long: " << sizeof(long long) << " bytes" << endl;
    cout << "float: " << sizeof(float) << " bytes" << endl;
    cout << "double: " << sizeof(double) << " bytes" << endl;
    cout << "bool: " << sizeof(bool) << " bytes" << endl;
    
    return 0;
}
```

### Getting Type Limits

```cpp
#include <iostream>
#include <limits>
using namespace std;

int main() {
    cout << "int range: " 
         << numeric_limits<int>::min() << " to " 
         << numeric_limits<int>::max() << endl;
         
    cout << "double range: " 
         << numeric_limits<double>::min() << " to " 
         << numeric_limits<double>::max() << endl;
         
    return 0;
}
```

### Memory Size Comparison Chart

```
Type              Size        Range/Precision
─────────────────────────────────────────────────────────
char              1 byte      -128 to 127 (or 0 to 255)
bool              1 byte      true or false
short             2 bytes     -32,768 to 32,767
int               4 bytes     ±2.1 billion (approx)
long              4-8 bytes   At least ±2.1 billion
long long         8 bytes     ±9.2 quintillion (approx)
float             4 bytes     6-7 digits precision
double            8 bytes     15-16 digits precision
long double       8-16 bytes  18+ digits precision
```

---

## Type Conversion and Casting

### Implicit Conversion (Automatic)

C++ automatically converts when it's safe (no data loss).

```cpp
int wholeNumber = 10;
double decimal = wholeNumber;  // int → double (safe)
// decimal is now 10.0

char letter = 'A';
int asciiValue = letter;  // char → int
// asciiValue is 65

float smallFloat = 3.14f;
double bigDouble = smallFloat;  // float → double (safe)
```

**Safe conversions** (widening):

```
char → short → int → long → long long
float → double → long double
```

### Explicit Conversion (Manual Casting)

You tell C++ to convert, even if data might be lost.

#### C-Style Cast

```cpp
double price = 19.99;
int dollars = (int)price;  // Becomes 19 (loses .99)
```

#### Function-Style Cast

```cpp
double value = 3.14;
int whole = int(value);  // Becomes 3
```

#### C++ Style Casts (Recommended)

```cpp
// static_cast - safest, most common
double num = 3.14;
int rounded = static_cast<int>(num);

// const_cast - removes const
const int x = 10;
int* ptr = const_cast<int*>(&x);

// reinterpret_cast - dangerous, low-level
int value = 65;
char* charPtr = reinterpret_cast<char*>(&value);

// dynamic_cast - for polymorphic classes
// Used with inheritance and pointers/references
```

### Type Conversion Examples

```cpp
// Integer division (data loss):
int a = 5;
int b = 2;
int result = a / b;  // result = 2 (not 2.5!)

// Fix with casting:
double result = (double)a / b;  // result = 2.5

// Overflow example:
short small = 32767;  // Maximum value
small = small + 1;    // Wraps to -32768! (overflow)

// Precision loss:
double precise = 3.14159265359;
float lessPrec = (float)precise;  // Loses some precision
```

---

## Choosing the Right Data Type

### Decision Flow Chart

```
Need to store data?
├─ Whole numbers?
│  ├─ Small range (-32K to 32K)? → short
│  ├─ Normal range (±2 billion)? → int
│  ├─ Large numbers? → long or long long
│  └─ Only positive? → unsigned version
│
├─ Decimal numbers?
│  ├─ Basic precision? → float
│  ├─ Standard precision? → double (recommended)
│  └─ High precision? → long double
│
├─ Single character? → char
│
├─ True/False? → bool
│
├─ Text/words? → string
│
└─ Mix of types? → struct or class
```

### Best Practices for Choosing Types

#### 1. Default Choices

For most situations, use these:

- **Whole numbers**: `int`
- **Decimals**: `double`
- **Text**: `string`
- **Single character**: `char`
- **True/False**: `bool`

#### 2. Use Unsigned for Non-Negative Values

```cpp
unsigned int studentCount = 50;  // Can't be negative
unsigned int arraySize = 100;    // Sizes are never negative
```

#### 3. Use Smaller Types to Save Memory

When dealing with large arrays:

```cpp
// If storing millions of small numbers:
short numbers[1000000];  // Saves memory vs int array
```

#### 4. Use Appropriate Precision

```cpp
float gameCoordinates = 10.5f;  // Fine for games
double scientificData = 3.14159265359;  // Science needs precision
```

#### 5. Avoid Type Mixing in Calculations

```cpp
// Can cause unexpected results:
int a = 5;
double b = 2.5;
auto result = a + b;  // Mixed types

// Better:
double a = 5.0;
double b = 2.5;
double result = a + b;  // Same types
```

---

## Common Data Type Mistakes

### 1. Integer Division Surprise

```cpp
// Problem:
int result = 5 / 2;  // result = 2 (not 2.5!)

// Solution:
double result = 5.0 / 2.0;  // result = 2.5
// or
double result = (double)5 / 2;
```

### 2. Overflow/Underflow

```cpp
// Problem:
short num = 32767;  // Max value for short
num = num + 1;      // Wraps to -32768!

// Solution:
int num = 32767;    // Use larger type
num = num + 1;      // Now 32768
```

### 3. Precision Loss

```cpp
// Problem:
float f = 0.1f + 0.2f;
if (f == 0.3f) {  // Might be false due to precision!
    cout << "Equal";
}

// Solution:
double d = 0.1 + 0.2;
if (abs(d - 0.3) < 0.0001) {  // Check within small range
    cout << "Equal enough";
}
```

### 4. Char vs String Confusion

```cpp
// Problem:
char name = "John";  // ✗ Error! char is single character

// Solution:
char initial = 'J';  // ✓ Single character
string name = "John";  // ✓ Text string
```

### 5. Unintended Type Conversion

```cpp
// Problem:
int total = 0;
int count = 3;
double average = total / count;  // Integer division! average = 0.0

// Solution:
double average = (double)total / count;  // average = 0.0 (correct type)
```

### 6. Negative Unsigned

```cpp
// Problem:
unsigned int value = -5;  // Wraps to huge positive number!

// Solution:
int value = -5;  // Use signed if negative values possible
```

---

## Practical Examples

### Example 1: Student Grade System

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    // Different data types for different information
    string studentName = "Alice Johnson";
    unsigned int studentID = 12345;
    unsigned short age = 20;
    char grade = 'A';
    double gpa = 3.85;
    bool isFullTime = true;
    unsigned int creditsCompleted = 90;
    
    cout << "Student Information:" << endl;
    cout << "Name: " << studentName << endl;
    cout << "ID: " << studentID << endl;
    cout << "Age: " << age << endl;
    cout << "Grade: " << grade << endl;
    cout << "GPA: " << gpa << endl;
    cout << "Full-time: " << (isFullTime ? "Yes" : "No") << endl;
    cout << "Credits: " << creditsCompleted << endl;
    
    return 0;
}
```

### Example 2: Temperature Converter

```cpp
#include <iostream>
using namespace std;

int main() {
    double celsius, fahrenheit, kelvin;
    
    cout << "Enter temperature in Celsius: ";
    cin >> celsius;
    
    fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
    kelvin = celsius + 273.15;
    
    cout << celsius << "°C = " << fahrenheit << "°F" << endl;
    cout << celsius << "°C = " << kelvin << "K" << endl;
    
    return 0;
}
```

### Example 3: Banking System

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string accountHolder = "John Doe";
    unsigned long accountNumber = 1234567890;
    double balance = 15750.50;
    bool isActive = true;
    char accountType = 'S';  // S=Savings, C=Checking
    unsigned int transactionCount = 145;
    
    // Deposit
    double depositAmount = 500.00;
    balance += depositAmount;
    transactionCount++;
    
    // Withdraw
    double withdrawAmount = 200.00;
    if (balance >= withdrawAmount) {
        balance -= withdrawAmount;
        transactionCount++;
    }
    
    cout << "Account: " << accountNumber << endl;
    cout << "Holder: " << accountHolder << endl;
    cout << "Type: " << (accountType == 'S' ? "Savings" : "Checking") << endl;
    cout << "Balance: $" << balance << endl;
    cout << "Transactions: " << transactionCount << endl;
    cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
    
    return 0;
}
```

### Example 4: Game Score Tracker

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string playerName = "ProGamer123";
    unsigned int score = 0;
    unsigned short level = 1;
    unsigned short lives = 3;
    bool hasShield = false;
    double healthPercentage = 100.0;
    unsigned long long totalXP = 0;
    
    // Game events
    score += 100;           // Collected item
    totalXP += 50;
    
    healthPercentage -= 25.5;  // Took damage
    
    if (healthPercentage <= 0 && lives > 0) {
        lives--;
        healthPercentage = 100.0;
    }
    
    cout << "Player: " << playerName << endl;
    cout << "Level: " << level << endl;
    cout << "Score: " << score << endl;
    cout << "Lives: " << lives << endl;
    cout << "Health: " << healthPercentage << "%" << endl;
    cout << "XP: " << totalXP << endl;
    cout << "Shield: " << (hasShield ? "Active" : "Inactive") << endl;
    
    return 0;
}
```

---

## Modern C++ Type Features

### 1. auto Keyword (C++11)

Lets compiler automatically deduce the type.

```cpp
auto age = 25;              // int
auto price = 19.99;         // double
auto name = "John";         // const char*
auto isActive = true;       // bool
auto letter = 'A';          // char

// Useful for complex types:
auto result = someComplexFunction();
```

**Pros**: Less typing, adapts to changes  
**Cons**: Can be less readable

### 2. decltype (C++11)

Gets the type of an expression.

```cpp
int x = 10;
decltype(x) y = 20;  // y is also int

auto a = 5;
decltype(a) b = 10;  // b is same type as a
```

### 3. Type Aliases (using)

Create alternative names for types.

```cpp
using Score = unsigned int;
using Price = double;
using PlayerID = unsigned long long;

Score gameScore = 1000;
Price productPrice = 49.99;
PlayerID player = 123456789;
```

### 4. Strongly-typed Enums

```cpp
enum class Color {
    RED,
    GREEN,
    BLUE
};

Color favoriteColor = Color::RED;  // Must use Color::
```

---

## Summary: Key Takeaways

### Essential Concepts

1. **Data types define storage**: How much memory and what kind of data
2. **Choose wisely**: Right type prevents errors and saves memory
3. **Type safety**: C++ prevents type mixing errors
4. **Conversions exist**: But use carefully to avoid data loss
5. **Modern features help**: auto, decltype, type aliases

### Quick Reference Guide

**For whole numbers**:

- Small: `short`
- Normal: `int`
- Large: `long` or `long long`
- Non-negative: add `unsigned`

**For decimals**:

- Basic: `float`
- Standard: `double` (recommended)
- Precise: `long double`

**For text**:

- Single character: `char`
- Multiple characters: `string`

**For logic**:

- True/False: `bool`

**For nothing**:

- No return: `void`

### Best Practices Checklist

✓ Choose the smallest type that fits your needs  
✓ Use unsigned for values that can't be negative  
✓ Prefer double over float for decimals  
✓ Initialize variables when declared  
✓ Be careful with type conversions  
✓ Use meaningful variable names  
✓ Check for overflow in calculations  
✓ Understand precision limits

---

## Final Thoughts

Data types are the foundation of programming in C++. Understanding them deeply helps you:

- Write more efficient code
- Avoid bugs and errors
- Make better design decisions
- Optimize memory usage
- Communicate intent clearly

Start with the basics (int, double, bool, char, string) and gradually explore more advanced types as you need them. The key is practice - the more you use different data types, the more natural choosing the right one becomes.

**Remember**: The goal isn't to memorize every detail, but to understand the concepts well enough to make informed decisions and know where to look when you need specific information.

**Happy coding!** 🚀