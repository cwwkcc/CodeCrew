## Assignment Overview

Create a program that collects detailed information about a student and displays it in a formatted report. This assignment will help you practice **all** the concepts we've learned so far.

---

## What You Need to Use

This assignment requires you to use:

- ✅ All data types: `int`, `short`, `long`, `long long`, `float`, `double`, `char`, `string`
- ✅ Variables (declaring and assigning values)
- ✅ `cin` for input
- ✅ `cout` for output
- ✅ `#include` directives
- ✅ Basic program structure (`main()` function)

---

## Assignment Requirements

### Create a program that collects the following information from the user:

1. **Personal Information**
    
    - Full name (`string`)
    - Age (`short`) - Students are typically young, short is enough
    - Gender (single character: M/F/O) (`char`)
    - Student ID number (`long`) - Large numbers need long
2. **Academic Information**
    
    - Year of enrollment (e.g., 2024) (`int`)
    - Total days attended this semester (`int`)
    - Grade Point Average / GPA (e.g., 3.85) (`float`)
3. **Physical Measurements**
    
    - Height in meters (e.g., 1.75) (`double`) - More precision needed
    - Weight in kilograms (e.g., 65.5) (`float`)
4. **Contact Information**
    
    - Phone number (as a number, no dashes) (`long long`) - Phone numbers are very long
    - Postal/ZIP code (`int`)

---

## Program Structure

Your program should:

1. **Display a welcome message** at the start
2. **Prompt the user** for each piece of information clearly
3. **Store** each input in the appropriate variable with the correct data type
4. **Display all collected information** in a formatted report at the end

---

## Sample Output

Here's what your program should look like when running:

```
========================================
    STUDENT INFORMATION SYSTEM
========================================

Please enter the following information:

--- Personal Information ---
Full Name: John Michael Smith
Age: 19
Gender (M/F/O): M
Student ID: 202401234

--- Academic Information ---
Year of Enrollment: 2024
Total Days Attended: 145
GPA: 3.85

--- Physical Measurements ---
Height (in meters): 1.75
Weight (in kg): 70.5

--- Contact Information ---
Phone Number: 1234567890
Postal Code: 12345

========================================
       STUDENT INFORMATION REPORT
========================================

Personal Information:
  Name: John Michael Smith
  Age: 19 years old
  Gender: M
  Student ID: 202401234

Academic Information:
  Enrollment Year: 2024
  Days Attended: 145 days
  GPA: 3.85

Physical Measurements:
  Height: 1.75 meters
  Weight: 70.5 kg

Contact Information:
  Phone: 1234567890
  Postal Code: 12345

========================================
Thank you for using the Student Information System!
========================================
```

---

## Detailed Instructions

### Step 1: Set Up Your Program

```cpp
#include <iostream>
#include <string>

int main() {
    
    // Your code goes here
    
    return 0;
}
```

**Note**: You need `#include <string>` to use the `string` data type!

### Step 2: Declare ALL Variables

Declare variables with the **correct data types**:

```cpp
// Personal Information
string fullName;
short age;
char gender;
long studentID;

// Academic Information
int enrollmentYear;
int daysAttended;
float gpa;

// Physical Measurements
double height;
float weight;

// Contact Information
long long phoneNumber;
int postalCode;
```

**Important**: Make sure each variable uses the **exact data type** specified in the requirements!

### Step 3: Display Welcome Message

```cpp
std::cout << "========================================\n";
std::cout << "    STUDENT INFORMATION SYSTEM\n";
std::cout << "========================================\n\n";
std::cout << "Please enter the following information:\n\n";
```

### Step 4: Get Input from User

For **each variable**, you need to:

1. Display a prompt telling the user what to enter
2. Use `cin` or `getline` to get the input

**Example for age**:

```cpp
std::cout << "Age: ";
std::cin >> age;
```

**Special Note for Full Name**: Since names can have spaces, you need to use `getline`:

```cpp
std::cout << "Full Name: ";
std::cin.ignore();  // Clear any leftover newline
std::getline(std::cin, fullName);
```

**OR** if you ask for the name FIRST (before any other input), you can just use:

```cpp
std::cout << "Full Name: ";
std::getline(std::cin, fullName);
```

### Step 5: Display the Report

After collecting all information, display it in a formatted report:

```cpp
std::cout << "\n========================================\n";
std::cout << "       STUDENT INFORMATION REPORT\n";
std::cout << "========================================\n\n";

std::cout << "Personal Information:\n";
std::cout << "  Name: " << fullName << "\n";
std::cout << "  Age: " << age << " years old\n";
// ... continue for all variables
```

### Step 6: End with Thank You Message

```cpp
std::cout << "\n========================================\n";
std::cout << "Thank you for using the Student Information System!\n";
std::cout << "========================================\n";
```

---

## Common Mistakes to Avoid

❌ **Using the wrong data type**

```cpp
int gpa;  // WRONG! GPA has decimals, use float
```

✅ **Correct**:

```cpp
float gpa;  // RIGHT! Can store decimal values
```

---

❌ Forgetting to include `<string>` 

```
cpp
#include <iostream>
// Missing: #include <string>

int main() {
    string name;  // ERROR! string not defined
}
```

✅ **Correct**:

```cpp
#include <iostream>
#include <string>  // Don't forget this!

int main() {
    string name;  // Works!
}
```

---

❌ **Using cin for strings with spaces**

```cpp
string fullName;
std::cin >> fullName;  // Only gets first word!
// Input: "John Smith" → Stores only "John"
```

✅ **Correct**:

```cpp
string fullName;
std::getline(std::cin, fullName);  // Gets entire line!
// Input: "John Smith" → Stores "John Smith"
```

---

❌ **Not declaring variables before using them**

```cpp
std::cin >> age;  // ERROR! age not declared
int age;          // Too late!
```

✅ **Correct**:

```cpp
int age;          // Declare first
std::cin >> age;  // Then use
```

---

## Tips for Success

💡 **Tip 1**: Declare ALL your variables at the beginning of main()

💡 **Tip 2**: Test your program with sample data to make sure it works

💡 **Tip 3**: Pay attention to which data type is required for each variable

💡 **Tip 4**: Use meaningful variable names (e.g., `studentID` not `x`)

💡 **Tip 5**: Make your output easy to read with clear labels

💡 **Tip 6**: If you ask for the full name FIRST, you don't need `cin.ignore()` before `getline()`

---

## Submission Guidelines

1. **File Name**: Save your program as `student_info.cpp`
    
2. **Comments**: Add a comment at the top with:
    
    ```cpp
    // Student Name: [Your Name]
    // Assignment: Student Information System
    // Date: [Today's Date]
    ```
    
3. **Test**: Run your program at least twice with different data to ensure it works
    
4. **Submit**: Submit your `.cpp` file
    

---

## Bonus Challenge 

Make your output look even nicer by:

- Adding more decorative lines
- Organizing information into clear sections
- Using creative formatting

Example:

```
╔════════════════════════════════════╗
║   STUDENT INFORMATION SYSTEM       ║
╔════════════════════════════════════╗
```

(You can use characters like `=`, `-`, `|`, `*` to make boxes and lines)

---

## Need Help?

If you get stuck:

1. ✅ Check that you included both `<iostream>` AND `<string>`
2. ✅ Make sure every variable is declared with the right data type
3. ✅ Verify you're using `cin >>` for single values and `getline()` for full names
4. ✅ Check for typos in variable names
5. ✅ Make sure you have semicolons at the end of statements

---

## Learning Objectives

By completing this assignment, you will demonstrate:

- ✓ Understanding of different data types and when to use each
- ✓ Ability to declare and use variables
- ✓ Knowledge of input operations (`cin`, `getline`)
- ✓ Knowledge of output operations (`cout`)
- ✓ Ability to create a complete, working C++ program
- ✓ Understanding of program structure and organization


---

## Good Luck! 🚀

Remember: This assignment uses everything you've learned so far. Take your time, be careful with data types, and test your program thoroughly!

**Happy Coding!** 💻