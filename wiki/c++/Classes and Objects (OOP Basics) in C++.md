## Introduction

Imagine you're creating a video game with 100 characters. Each character has health, strength, name, and actions like attack and heal. Using what you know so far, you'd need separate variables and functions for everything - a nightmare!

**Object-Oriented Programming (OOP)** solves this by letting you create blueprints (classes) that combine data and functions together. It's one of the most powerful programming paradigms.

**What is OOP?**

**Object-Oriented Programming** is a programming paradigm that organizes code around **objects** rather than functions and logic. Objects are instances of **classes**, which are blueprints that define the properties (data) and behaviors (functions) that objects can have.

**Real-World Analogies**:

- **Cookie Cutter and Cookies**: Class = cookie cutter (blueprint), Objects = actual cookies
- **Car Blueprint**: Class = blueprint showing design, Objects = actual cars built from it
- **House Plan**: Class = architectural plan, Objects = houses built from that plan
- **Student ID Template**: Class = ID card template, Objects = individual student ID cards
- **Recipe**: Class = recipe, Objects = actual cakes you bake from it

---

## Why OOP Matters

### Without OOP - Messy and Unorganized

```cpp
// Student 1
string student1_name = "Alice";
int student1_age = 20;
double student1_gpa = 3.8;

void displayStudent1() {
    cout << student1_name << ", " << student1_age << ", " << student1_gpa << endl;
}

// Student 2
string student2_name = "Bob";
int student2_age = 21;
double student2_gpa = 3.5;

void displayStudent2() {
    cout << student2_name << ", " << student2_age << ", " << student2_gpa << endl;
}

// Imagine 100 students... impossible!
```

### With OOP - Clean and Organized

```cpp
class Student {
public:
    string name;
    int age;
    double gpa;
    
    void display() {
        cout << name << ", " << age << ", " << gpa << endl;
    }
};

// Create as many students as needed
Student student1, student2, student3;
student1.name = "Alice";
student1.display();
```

### Benefits of OOP

1. **Encapsulation** - Bundle data and functions together
2. **Reusability** - Create multiple objects from one class
3. **Organization** - Code is structured and maintainable
4. **Abstraction** - Hide complex details, show only essentials
5. **Real-world modeling** - Code mirrors real-world entities
6. **Scalability** - Easy to expand and modify
7. **Security** - Control access to data

---

## Classes vs Structures

You already know structures! Classes are very similar but more powerful.

### Structure (What You Know)

```cpp
struct Student {
    string name;
    int age;
    
    void display() {
        cout << name << ", " << age << endl;
    }
};
```

### Class (New!)

```cpp
class Student {
public:  // Access specifier
    string name;
    int age;
    
    void display() {
        cout << name << ", " << age << endl;
    }
};
```

### Key Differences

|Feature|struct|class|
|---|---|---|
|**Default access**|public|private|
|**Common use**|Simple data grouping|Complex objects with behavior|
|**OOP features**|Limited|Full OOP support|

**In C++**: `struct` and `class` are nearly identical. The only difference is default access!

---

## Basic Class Syntax

### Defining a Class

```cpp
class ClassName {
    // Access specifier (public, private, protected)
public:
    // Data members (variables)
    dataType memberVariable;
    
    // Member functions (methods)
    returnType memberFunction() {
        // Function body
    }
};  // Don't forget semicolon!
```

### Simple Example: Rectangle Class

```cpp
class Rectangle {
public:
    // Data members
    double length;
    double width;
    
    // Member functions
    double getArea() {
        return length * width;
    }
    
    double getPerimeter() {
        return 2 * (length + width);
    }
};
```

---

## Creating Objects

An **object** is an instance of a class - a real entity created from the blueprint.

### Declaring Objects

```cpp
class Student {
public:
    string name;
    int age;
    double gpa;
};

int main() {
    // Create objects (instances of Student class)
    Student student1;
    Student student2;
    Student student3;
    
    return 0;
}
```

### Accessing Members

Use the **dot operator** (`.`) to access members:

```cpp
Student student1;

// Accessing data members
student1.name = "Alice";
student1.age = 20;
student1.gpa = 3.8;

// Accessing member functions
student1.display();
```

### Complete Example

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
public:
    string name;
    int age;
    double gpa;
    
    void display() {
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "GPA: " << gpa << endl;
    }
};

int main() {
    // Create object
    Student student1;
    
    // Set values
    student1.name = "Alice";
    student1.age = 20;
    student1.gpa = 3.8;
    
    // Use member function
    student1.display();
    
    return 0;
}
```

**Output**:

```
Name: Alice
Age: 20
GPA: 3.8
```

---

## Access Specifiers

Access specifiers control who can access class members.

### Three Access Specifiers

1. **public** - Accessible from anywhere
2. **private** - Accessible only within the class
3. **protected** - Accessible within class and derived classes (we'll learn later)

### Visual Representation

```cpp
class MyClass {
public:
    // Anyone can access these
    int publicVar;
    void publicFunction() { }

private:
    // Only class members can access these
    int privateVar;
    void privateFunction() { }

protected:
    // Class and derived classes can access these
    int protectedVar;
    void protectedFunction() { }
};
```

### Public Example

```cpp
class Student {
public:
    string name;  // Public - accessible from anywhere
    
    void display() {
        cout << name << endl;
    }
};

int main() {
    Student s;
    s.name = "Alice";  // OK - public member
    s.display();       // OK - public function
}
```

### Private Example

```cpp
class BankAccount {
private:
    double balance;  // Private - only class can access

public:
    void setBalance(double b) {
        balance = b;  // OK - inside class
    }
    
    double getBalance() {
        return balance;  // OK - inside class
    }
};

int main() {
    BankAccount account;
    // account.balance = 1000;  // ERROR! Private member
    account.setBalance(1000);   // OK - using public function
    cout << account.getBalance() << endl;  // OK
}
```

### Why Use Private?

**Encapsulation** - Hide internal details and control access

```cpp
class BankAccount {
private:
    double balance;

public:
    void deposit(double amount) {
        if (amount > 0) {  // Validation
            balance += amount;
        }
    }
    
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {  // Safety check
            balance -= amount;
        }
    }
};
```

**Without private**: Anyone could set `balance = -1000000`!  
**With private**: All changes go through validated functions!

---

## Member Functions

Functions inside a class are called **member functions** or **methods**.

### Defining Inside Class

```cpp
class Rectangle {
public:
    double length;
    double width;
    
    // Function defined inside class
    double getArea() {
        return length * width;
    }
};
```

### Defining Outside Class

```cpp
class Rectangle {
public:
    double length;
    double width;
    
    // Function declaration
    double getArea();
    double getPerimeter();
};

// Function definition outside class
double Rectangle::getArea() {
    return length * width;
}

double Rectangle::getPerimeter() {
    return 2 * (length + width);
}
```

**Note**: Use `ClassName::` before function name when defining outside!

### Calling Member Functions

```cpp
Rectangle rect;
rect.length = 5.0;
rect.width = 3.0;

double area = rect.getArea();
double perimeter = rect.getPerimeter();

cout << "Area: " << area << endl;
cout << "Perimeter: " << perimeter << endl;
```

---

## Constructors

A **constructor** is a special function that automatically runs when an object is created. It initializes the object.

### Why Constructors?

**Without constructor**:

```cpp
Student s;
// s.name is undefined
// s.age is undefined
// Must manually initialize everything
s.name = "Alice";
s.age = 20;
```

**With constructor**:

```cpp
Student s("Alice", 20);
// Automatically initialized!
```

### Constructor Syntax

```cpp
class ClassName {
public:
    // Constructor - same name as class, no return type
    ClassName() {
        // Initialization code
    }
};
```

### Default Constructor

Constructor with no parameters:

```cpp
class Student {
public:
    string name;
    int age;
    
    // Default constructor
    Student() {
        name = "Unknown";
        age = 0;
    }
};

int main() {
    Student s;  // Calls default constructor
    cout << s.name << endl;  // "Unknown"
}
```

### Parameterized Constructor

Constructor with parameters:

```cpp
class Student {
public:
    string name;
    int age;
    double gpa;
    
    // Parameterized constructor
    Student(string n, int a, double g) {
        name = n;
        age = a;
        gpa = g;
    }
};

int main() {
    Student s1("Alice", 20, 3.8);
    cout << s1.name << endl;  // "Alice"
}
```

### Multiple Constructors (Overloading)

```cpp
class Student {
public:
    string name;
    int age;
    double gpa;
    
    // Default constructor
    Student() {
        name = "Unknown";
        age = 0;
        gpa = 0.0;
    }
    
    // Constructor with name only
    Student(string n) {
        name = n;
        age = 0;
        gpa = 0.0;
    }
    
    // Constructor with all parameters
    Student(string n, int a, double g) {
        name = n;
        age = a;
        gpa = g;
    }
};

int main() {
    Student s1;                      // Uses default
    Student s2("Bob");               // Uses second constructor
    Student s3("Charlie", 21, 3.5);  // Uses third constructor
}
```

### Constructor Initialization List (Preferred)

```cpp
class Student {
public:
    string name;
    int age;
    double gpa;
    
    // Using initialization list (more efficient)
    Student(string n, int a, double g) : name(n), age(a), gpa(g) {
        // Constructor body (optional)
    }
};
```

---

## Destructors

A **destructor** is a special function that runs when an object is destroyed. It cleans up resources.

### Destructor Syntax

```cpp
class ClassName {
public:
    // Constructor
    ClassName() {
        cout << "Object created!" << endl;
    }
    
    // Destructor - same name with ~ prefix, no parameters
    ~ClassName() {
        cout << "Object destroyed!" << endl;
    }
};
```

### Example

```cpp
#include <iostream>
using namespace std;

class Student {
public:
    string name;
    
    Student(string n) : name(n) {
        cout << name << " enrolled!" << endl;
    }
    
    ~Student() {
        cout << name << " graduated!" << endl;
    }
};

int main() {
    Student s1("Alice");
    Student s2("Bob");
    
    cout << "Program running..." << endl;
    
    return 0;
}  // Destructors called here automatically
```

**Output**:

```
Alice enrolled!
Bob enrolled!
Program running...
Bob graduated!
Alice graduated!
```

**Note**: Destructors are called in reverse order of creation!

---

## Getters and Setters

**Getters** and **Setters** are functions to access and modify private data safely.

### Why Use Getters and Setters?

Direct access to data is risky:

```cpp
student.age = -5;  // Invalid but allowed!
student.gpa = 10.0;  // Invalid but allowed!
```

With getters and setters:

```cpp
student.setAge(-5);  // Can validate and reject!
```

### Basic Example

```cpp
class Student {
private:
    string name;
    int age;
    double gpa;

public:
    // Setters (mutators)
    void setName(string n) {
        name = n;
    }
    
    void setAge(int a) {
        if (a > 0 && a < 120) {  // Validation
            age = a;
        } else {
            cout << "Invalid age!" << endl;
        }
    }
    
    void setGPA(double g) {
        if (g >= 0.0 && g <= 4.0) {  // Validation
            gpa = g;
        } else {
            cout << "Invalid GPA!" << endl;
        }
    }
    
    // Getters (accessors)
    string getName() {
        return name;
    }
    
    int getAge() {
        return age;
    }
    
    double getGPA() {
        return gpa;
    }
};

int main() {
    Student s;
    
    s.setName("Alice");
    s.setAge(20);     // OK
    s.setGPA(3.8);    // OK
    
    s.setAge(-5);     // Rejected - Invalid!
    s.setGPA(10.0);   // Rejected - Invalid!
    
    cout << s.getName() << endl;
}
```

### Benefits of Getters/Setters

1. **Validation** - Check data before setting
2. **Encapsulation** - Hide internal representation
3. **Read-only** - Provide getter without setter
4. **Write-only** - Provide setter without getter
5. **Computed values** - Calculate on-the-fly

**Computed value example**:

```cpp
class Rectangle {
private:
    double length;
    double width;

public:
    void setLength(double l) { length = l; }
    void setWidth(double w) { width = w; }
    
    // Computed getter - calculates area
    double getArea() {
        return length * width;
    }
};
```

---

## The 'this' Pointer

`this` is a pointer to the current object.

### Why Use 'this'?

**Problem**: Parameter names same as member names

```cpp
class Student {
private:
    string name;

public:
    void setName(string name) {
        name = name;  // Confusing! Which name is which?
    }
};
```

**Solution**: Use `this`

```cpp
class Student {
private:
    string name;

public:
    void setName(string name) {
        this->name = name;  // this->name is member, name is parameter
    }
};
```

### Uses of 'this'

**1. Disambiguate names**:

```cpp
void setAge(int age) {
    this->age = age;
}
```

**2. Return current object**:

```cpp
class Counter {
private:
    int count;

public:
    Counter& increment() {
        count++;
        return *this;  // Return reference to current object
    }
};

// Allows chaining
Counter c;
c.increment().increment().increment();
```

**3. Compare objects**:

```cpp
bool equals(Student& other) {
    return this->name == other.name && this->age == other.age;
}
```

---

## Practical Examples and Projects

### Example 1: Bank Account Class

```cpp
#include <iostream>
#include <string>
using namespace std;

class BankAccount {
private:
    string accountNumber;
    string holderName;
    double balance;

public:
    // Constructor
    BankAccount(string accNum, string name, double initialBalance) {
        accountNumber = accNum;
        holderName = name;
        balance = initialBalance;
    }
    
    // Deposit money
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposited: $" << amount << endl;
            cout << "New balance: $" << balance << endl;
        } else {
            cout << "Invalid deposit amount!" << endl;
        }
    }
    
    // Withdraw money
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            cout << "Withdrawn: $" << amount << endl;
            cout << "New balance: $" << balance << endl;
        } else if (amount > balance) {
            cout << "Insufficient funds!" << endl;
        } else {
            cout << "Invalid withdrawal amount!" << endl;
        }
    }
    
    // Display account info
    void displayInfo() {
        cout << "\n=== Account Information ===" << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << "Holder Name: " << holderName << endl;
        cout << "Balance: $" << balance << endl;
    }
    
    // Getter for balance
    double getBalance() {
        return balance;
    }
};

int main() {
    // Create account
    BankAccount account("ACC001", "John Doe", 1000.0);
    
    account.displayInfo();
    
    account.deposit(500);
    account.withdraw(200);
    account.withdraw(2000);  // Insufficient funds
    
    account.displayInfo();
    
    return 0;
}
```

### Example 2: Student Management System

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
private:
    int id;
    string name;
    int age;
    double gpa;

public:
    // Constructor
    Student() {
        id = 0;
        name = "Unknown";
        age = 0;
        gpa = 0.0;
    }
    
    Student(int i, string n, int a, double g) {
        id = i;
        name = n;
        age = a;
        gpa = g;
    }
    
    // Setters with validation
    void setID(int i) {
        if (i > 0) {
            id = i;
        }
    }
    
    void setName(string n) {
        name = n;
    }
    
    void setAge(int a) {
        if (a > 0 && a < 100) {
            age = a;
        }
    }
    
    void setGPA(double g) {
        if (g >= 0.0 && g <= 4.0) {
            gpa = g;
        }
    }
    
    // Getters
    int getID() { return id; }
    string getName() { return name; }
    int getAge() { return age; }
    double getGPA() { return gpa; }
    
    // Member functions
    void display() {
        cout << "\n=== Student Information ===" << endl;
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "GPA: " << gpa << endl;
    }
    
    char getGrade() {
        if (gpa >= 3.7) return 'A';
        else if (gpa >= 3.0) return 'B';
        else if (gpa >= 2.0) return 'C';
        else if (gpa >= 1.0) return 'D';
        else return 'F';
    }
    
    bool isPassing() {
        return gpa >= 2.0;
    }
};

int main() {
    // Create students
    Student s1(101, "Alice Johnson", 20, 3.8);
    Student s2(102, "Bob Smith", 21, 2.5);
    Student s3;  // Default constructor
    
    s3.setID(103);
    s3.setName("Charlie Brown");
    s3.setAge(19);
    s3.setGPA(3.2);
    
    // Display students
    s1.display();
    cout << "Grade: " << s1.getGrade() << endl;
    cout << "Status: " << (s1.isPassing() ? "Passing" : "Failing") << endl;
    
    s2.display();
    cout << "Grade: " << s2.getGrade() << endl;
    
    s3.display();
    
    return 0;
}
```

### Example 3: Rectangle Class with Full Features

```cpp
#include <iostream>
using namespace std;

class Rectangle {
private:
    double length;
    double width;

public:
    // Constructors
    Rectangle() : length(1.0), width(1.0) {
        cout << "Rectangle created with default size" << endl;
    }
    
    Rectangle(double l, double w) : length(l), width(w) {
        cout << "Rectangle created: " << l << "x" << w << endl;
    }
    
    // Destructor
    ~Rectangle() {
        cout << "Rectangle destroyed" << endl;
    }
    
    // Setters
    void setLength(double l) {
        if (l > 0) {
            length = l;
        } else {
            cout << "Invalid length!" << endl;
        }
    }
    
    void setWidth(double w) {
        if (w > 0) {
            width = w;
        } else {
            cout << "Invalid width!" << endl;
        }
    }
    
    // Getters
    double getLength() { return length; }
    double getWidth() { return width; }
    
    // Computed properties
    double getArea() {
        return length * width;
    }
    
    double getPerimeter() {
        return 2 * (length + width);
    }
    
    bool isSquare() {
        return length == width;
    }
    
    // Display
    void display() {
        cout << "\n=== Rectangle Info ===" << endl;
        cout << "Length: " << length << endl;
        cout << "Width: " << width << endl;
        cout << "Area: " << getArea() << endl;
        cout << "Perimeter: " << getPerimeter() << endl;
        cout << "Is Square: " << (isSquare() ? "Yes" : "No") << endl;
    }
};

int main() {
    Rectangle r1;
    r1.display();
    
    Rectangle r2(5.0, 3.0);
    r2.display();
    
    Rectangle r3(4.0, 4.0);
    r3.display();
    
    return 0;
}
```

### Example 4: Circle Class

```cpp
#include <iostream>
#include <cmath>
using namespace std;

class Circle {
private:
    double radius;
    const double PI = 3.14159;

public:
    // Constructor
    Circle(double r = 1.0) {
        setRadius(r);
    }
    
    // Setter with validation
    void setRadius(double r) {
        if (r > 0) {
            radius = r;
        } else {
            cout << "Invalid radius! Setting to 1.0" << endl;
            radius = 1.0;
        }
    }
    
    // Getter
    double getRadius() {
        return radius;
    }
    
    // Computed properties
    double getArea() {
        return PI * radius * radius;
    }
    
    double getCircumference() {
        return 2 * PI * radius;
    }
    
    double getDiameter() {
        return 2 * radius;
    }
    
    // Display
    void display() {
        cout << "\n=== Circle Information ===" << endl;
        cout << "Radius: " << radius << endl;
        cout << "Diameter: " << getDiameter() << endl;
        cout << "Area: " << getArea() << endl;
        cout << "Circumference: " << getCircumference() << endl;
    }
};

int main() {
    Circle c1(5.0);
    c1.display();
    
    Circle c2;  // Default radius = 1.0
    c2.display();
    
    c2.setRadius(10.0);
    c2.display();
    
    return 0;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Forgetting Semicolon After Class

❌ **Wrong**:

```cpp
class Student {
    string name;
    int age;
}  // Missing semicolon!

int main() {
    // ...
}
```

✅ **Correct**:

```cpp
class Student {
    string name;
    int age;
};  // Semicolon required!
```

### Mistake 2: Accessing Private Members

❌ **Wrong**:

```cpp
class Student {
private:
    string name;
};

int main() {
    Student s;
    s.name = "Alice";  // ERROR! name is private
}
```

✅ **Correct**:

```cpp
class Student {
private:
    string name;

public:
    void setName(string n) {
        name = n;
    }
};

int main() {
    Student s;
    s.setName("Alice");  // OK - using public setter
}
```

### Mistake 3: Constructor with Return Type

❌ **Wrong**:

```cpp
class Student {
public:
    void Student() {  // Wrong! Constructor has no return type
        // ...
    }
};
```

✅ **Correct**:

```cpp
class Student {
public:
    Student() {  // No return type!
        // ...
    }
};
```

### Mistake 4: Not Initializing Objects

❌ **Wrong**:

```cpp
class Student {
public:
    string name;
    int age;
};

int main() {
    Student s;
    cout << s.age << endl;  // Undefined value!
}
```

✅ **Correct**:

```cpp
class Student {
public:
    string name;
    int age;
    
    Student() {  // Constructor initializes
        name = "Unknown";
        age = 0;
    }
};

int main() {
    Student s;
    cout << s.age << endl;  // 0 - defined value
}
```

### Mistake 5: Confusing Class Definition and Object Creation

❌ **Wrong**:

```cpp
Student {  // Wrong! Not how you create an object
    string name;
};
```

✅ **Correct**:

```cpp
// Define class
class Student {
public:
    string name;
};

// Create object
Student s;
```

### Mistake 6: Modifying const Objects

❌ **Wrong**:

```cpp
class Student {
public:
    void setName(string n) {
        name = n;
    }
};

int main() {
    const Student s;
    s.setName("Alice");  // ERROR! Can't modify const object
}
```

✅ **Correct**:

```cpp
// Mark functions that don't modify as const
class Student {
public:
    void setName(string n) {
        name = n;
    }
    
    string getName() const {  // const function
        return name;
    }
};
```

### Mistake 7: Memory Leaks with Pointers

❌ **Wrong**:

```cpp
class Student {
private:
    int* scores;
public:
    Student() {
        scores = new int[10];
    }
    // Missing destructor - memory leak!
};
```

✅ **Correct**:

```cpp
class Student {
private:
    int* scores;
public:
    Student() {
        scores = new int[10];
    }
    
    ~Student() {
        delete[] scores;  // Clean up!
    }
};
```

---

## Best Practices

### ✅ DO:

1. **Use meaningful class names**
    
    ```cpp
    class BankAccount { };  // Good
    class BA { };           // Bad
    ```
    
2. **Make data members private**
    
    ```cpp
    class Student {
    private:
        string name;  // Protected data
    public:
        void setName(string n) { name = n; }
    };
    ```
    
3. **Provide constructors**
    
    ```cpp
    class Student {
    public:
        Student() { /* initialize */ }
    };
    ```
    
4. **Use initialization lists**
    
    ```cpp
    Student(string n, int a) : name(n), age(a) { }
    ```
    
5. **Validate in setters**
    
    ```cpp
    void setAge(int a) {
        if (a > 0) age = a;
    }
    ```
    
6. **Use const for functions that don't modify**
    
    ```cpp
    double getArea() const {
        return length * width;
    }
    ```
    

### ❌ DON'T:

1. **Don't make everything public**
    
    ```cpp
    // Bad - no encapsulation
    class Student {
    public:
        string name;
        int age;
    };
    ```
    
2. **Don't forget destructors for dynamic memory**
    
    ```cpp
    ~Student() {
        delete[] dynamicArray;
    }
    ```
    
3. **Don't use global variables instead of class members**
    
4. **Don't create objects unnecessarily**
    
5. **Don't expose implementation details**
    

---

## Practice Exercises

### Exercise 1: Book Class

Create a `Book` class with:

- Private: title, author, pages, price
- Constructor with parameters
- Getters and setters
- Display function
- Function to check if it's a long book (>300 pages)

### Exercise 2: Counter Class

Create a `Counter` class with:

- Private counter starting at 0
- Functions: increment, decrement, reset, getValue
- Prevent counter from going below 0

### Exercise 3: Temperature Class

Create a `Temperature` class with:

- Private celsius value
- Setters/getters for Celsius, Fahrenheit, Kelvin
- Display in all three formats

### Exercise 4: Time Class

Create a `Time` class with:

- Private: hours, minutes, seconds
- Constructor with validation
- Add and subtract time
- Display in HH:MM:SS format

### Exercise 5: Employee Class

Create an `Employee` class with:

- Private: name, ID, salary, department
- Constructor and destructor
- Calculate annual salary
- Give raise (percentage)
- Display employee information

### Exercise 6: Point Class

Create a `Point` class for 2D coordinates:

- Private: x, y
- Calculate distance from origin
- Calculate distance from another point
- Check if point is in first quadrant

### Exercise 7: Library System

Create a library management system with:

- Book class (title, author, ISBN, available status)
- Functions to check out and return books
- Display book information
- Search functionality

### Exercise 8: Shopping Cart

Create a shopping system with:

- Product class (name, price, quantity)
- Calculate total price
- Apply discount
- Display receipt

---

## Quick Reference

### Class Definition

```cpp
class ClassName {
private:
    // Private members
    
public:
    // Public members
    
protected:
    // Protected members
};  // Semicolon!
```

### Constructor

```cpp
ClassName() { }  // Default
ClassName(params) : member(value) { }  // Parameterized
```

### Destructor

```cpp
~ClassName() { }
```

### Creating Objects

```cpp
ClassName obj1;              // Default constructor
ClassName obj2(params);      // Parameterized constructor
ClassName* ptr = new ClassName();  // Dynamic
```

### Accessing Members

```cpp
obj.member;       // Dot operator for objects
ptr->member;      // Arrow operator for pointers
```

### Getters and Setters

```cpp
void setMember(Type value) {
    member = value;
}

Type getMember() const {
    return member;
}
```

---

## Summary

### Key Concepts

1. **Class** - Blueprint for creating objects
2. **Object** - Instance of a class
3. **Encapsulation** - Bundle data and functions, hide details
4. **Access specifiers** - public, private, protected
5. **Constructor** - Initialize objects
6. **Destructor** - Clean up resources
7. **this pointer** - Reference to current object

### Four Pillars of OOP (Basics Covered)

1. ✅ **Encapsulation** - Bundling data and methods
2. ⏳ **Inheritance** - (Next topic)
3. ⏳ **Polymorphism** - (Advanced topic)
4. ✅ **Abstraction** - Hiding complexity

### What You Can Do Now

✓ Create custom data types  
✓ Model real-world entities  
✓ Protect data with encapsulation  
✓ Write organized, maintainable code  
✓ Build scalable applications  
✓ Use objects and classes effectively

---

## Next Steps

After mastering OOP basics, you'll learn:

- **Inheritance** - Create class hierarchies
- **Polymorphism** - One interface, multiple implementations
- **Operator overloading** - Custom operators for classes
- **Friend functions** - Special access to private members
- **Static members** - Class-level data and functions
- **Abstract classes** - Pure virtual functions

---

## Conclusion

Object-Oriented Programming transforms the way you write code. Instead of scattered variables and functions, you create organized, reusable, and maintainable structures.

Classes and objects allow you to:

- **Model real-world** entities accurately
- **Protect data** with encapsulation
- **Reuse code** efficiently
- **Build complex** systems systematically
- **Think in objects** rather than procedures

Master OOP, and you'll write professional, scalable, and elegant code!

**Happy Coding!** 🚀

---

## Remember

_"Think in objects, not in procedures"_

OOP changes your mindset:

- Not: "What functions do I need?"
- But: "What objects exist and what can they do?"

This paradigm shift makes you a better programmer and opens the door to advanced software design!

🎯 **You've now entered the world of Object-Oriented Programming!**