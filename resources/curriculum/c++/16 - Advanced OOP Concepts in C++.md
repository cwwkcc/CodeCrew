## Introduction

You've learned the basics of classes and objects - creating blueprints and instances. Now it's time to unlock the **true power** of Object-Oriented Programming with advanced concepts that enable code reuse, flexibility, and elegant design.

These advanced concepts allow you to:

- **Reuse code** through inheritance
- **Create flexible interfaces** with polymorphism
- **Share data** across all objects with static members
- **Grant special access** with friend functions
- **Customize operators** for your classes

**Real-World Analogy**: Think of basic OOP as learning to build a house. Advanced OOP is learning to create house templates that can be extended (add a garage, pool) and customized (different paint, different materials) while reusing the core structure.

---

## Table of Contents

1. Inheritance
2. Polymorphism
3. Virtual Functions
4. Abstract Classes and Pure Virtual Functions
5. Operator Overloading
6. Friend Functions and Friend Classes
7. Static Members
8. Multiple Inheritance
9. Practical Projects

---

# 1. Inheritance

## What is Inheritance?

**Inheritance** allows a class to inherit properties and methods from another class. The new class can reuse, extend, or modify the behavior of the existing class.

**Real-World Analogies**:

- **Biology**: Children inherit traits from parents
- **Vehicles**: Car, Truck, Motorcycle all inherit from Vehicle
- **Employees**: Manager, Engineer, Salesperson all inherit from Employee
- **Shapes**: Circle, Rectangle, Triangle all inherit from Shape

### Benefits of Inheritance

1. **Code reusability** - Don't write the same code twice
2. **Extensibility** - Add new features to existing code
3. **Maintainability** - Fix bugs in one place
4. **Hierarchical classification** - Model real-world relationships
5. **Polymorphism** - Treat different objects uniformly

---

## Basic Inheritance Syntax

```cpp
// Base class (Parent class / Superclass)
class BaseClass {
    // Members
};

// Derived class (Child class / Subclass)
class DerivedClass : public BaseClass {
    // Inherits all members from BaseClass
    // Can add new members
};
```

### Simple Example

```cpp
#include <iostream>
#include <string>
using namespace std;

// Base class
class Animal {
public:
    string name;
    int age;
    
    void eat() {
        cout << name << " is eating." << endl;
    }
    
    void sleep() {
        cout << name << " is sleeping." << endl;
    }
};

// Derived class
class Dog : public Animal {
public:
    string breed;
    
    void bark() {
        cout << name << " says: Woof! Woof!" << endl;
    }
};

int main() {
    Dog myDog;
    
    // Inherited members from Animal
    myDog.name = "Buddy";
    myDog.age = 3;
    myDog.eat();
    myDog.sleep();
    
    // Dog's own members
    myDog.breed = "Golden Retriever";
    myDog.bark();
    
    return 0;
}
```

**Output**:

```
Buddy is eating.
Buddy is sleeping.
Buddy says: Woof! Woof!
```

---

## Types of Inheritance

### 1. Single Inheritance

One derived class inherits from one base class.

```cpp
class Vehicle {
public:
    void start() {
        cout << "Vehicle started" << endl;
    }
};

class Car : public Vehicle {
public:
    void honk() {
        cout << "Beep beep!" << endl;
    }
};
```

### 2. Multilevel Inheritance

Derived class becomes base class for another class.

```cpp
class Animal {
public:
    void breathe() {
        cout << "Breathing..." << endl;
    }
};

class Mammal : public Animal {
public:
    void feedMilk() {
        cout << "Feeding milk..." << endl;
    }
};

class Dog : public Mammal {
public:
    void bark() {
        cout << "Woof!" << endl;
    }
};

int main() {
    Dog d;
    d.breathe();   // From Animal
    d.feedMilk();  // From Mammal
    d.bark();      // From Dog
}
```

### 3. Hierarchical Inheritance

Multiple derived classes inherit from one base class.

```cpp
class Shape {
public:
    void draw() {
        cout << "Drawing shape..." << endl;
    }
};

class Circle : public Shape {
public:
    void drawCircle() {
        cout << "Drawing circle" << endl;
    }
};

class Rectangle : public Shape {
public:
    void drawRectangle() {
        cout << "Drawing rectangle" << endl;
    }
};

class Triangle : public Shape {
public:
    void drawTriangle() {
        cout << "Drawing triangle" << endl;
    }
};
```

### 4. Multiple Inheritance

One class inherits from multiple base classes.

```cpp
class Father {
public:
    void showFatherTrait() {
        cout << "Father's trait" << endl;
    }
};

class Mother {
public:
    void showMotherTrait() {
        cout << "Mother's trait" << endl;
    }
};

class Child : public Father, public Mother {
public:
    void showChildTrait() {
        cout << "Child's trait" << endl;
    }
};

int main() {
    Child c;
    c.showFatherTrait();  // From Father
    c.showMotherTrait();  // From Mother
    c.showChildTrait();   // Own method
}
```

---

## Access Specifiers in Inheritance

### Public Inheritance (Most Common)

```cpp
class Base {
public:
    int publicVar;
protected:
    int protectedVar;
private:
    int privateVar;
};

class Derived : public Base {
    // publicVar remains public
    // protectedVar remains protected
    // privateVar is NOT accessible
};
```

### Protected Inheritance

```cpp
class Derived : protected Base {
    // publicVar becomes protected
    // protectedVar remains protected
    // privateVar is NOT accessible
};
```

### Private Inheritance

```cpp
class Derived : private Base {
    // publicVar becomes private
    // protectedVar becomes private
    // privateVar is NOT accessible
};
```

### Access Table

|Base Class|public Inheritance|protected Inheritance|private Inheritance|
|---|---|---|---|
|public|public|protected|private|
|protected|protected|protected|private|
|private|Not accessible|Not accessible|Not accessible|

---

## Constructors and Destructors in Inheritance

### Constructor Call Order

**Constructors** are called from **base to derived**.

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    Base() {
        cout << "Base constructor called" << endl;
    }
};

class Derived : public Base {
public:
    Derived() {
        cout << "Derived constructor called" << endl;
    }
};

int main() {
    Derived d;
    return 0;
}
```

**Output**:

```
Base constructor called
Derived constructor called
```

### Destructor Call Order

**Destructors** are called from **derived to base** (reverse order).

```cpp
class Base {
public:
    ~Base() {
        cout << "Base destructor called" << endl;
    }
};

class Derived : public Base {
public:
    ~Derived() {
        cout << "Derived destructor called" << endl;
    }
};

int main() {
    Derived d;
    return 0;
}
```

**Output**:

```
Derived destructor called
Base destructor called
```

### Passing Arguments to Base Constructor

```cpp
class Person {
protected:
    string name;
    int age;

public:
    Person(string n, int a) : name(n), age(a) {
        cout << "Person constructor" << endl;
    }
};

class Student : public Person {
private:
    int studentID;

public:
    // Call base constructor with arguments
    Student(string n, int a, int id) : Person(n, a), studentID(id) {
        cout << "Student constructor" << endl;
    }
    
    void display() {
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "ID: " << studentID << endl;
    }
};

int main() {
    Student s("Alice", 20, 12345);
    s.display();
}
```

---

## Function Overriding

Derived class can **override** (replace) base class methods.

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    void makeSound() {
        cout << "Animal makes a sound" << endl;
    }
};

class Dog : public Animal {
public:
    void makeSound() {  // Overrides Animal::makeSound()
        cout << "Dog barks: Woof! Woof!" << endl;
    }
};

class Cat : public Animal {
public:
    void makeSound() {  // Overrides Animal::makeSound()
        cout << "Cat meows: Meow! Meow!" << endl;
    }
};

int main() {
    Animal a;
    Dog d;
    Cat c;
    
    a.makeSound();  // Animal makes a sound
    d.makeSound();  // Dog barks: Woof! Woof!
    c.makeSound();  // Cat meows: Meow! Meow!
    
    return 0;
}
```

### Calling Base Class Function from Derived Class

```cpp
class Base {
public:
    void display() {
        cout << "Base display" << endl;
    }
};

class Derived : public Base {
public:
    void display() {
        Base::display();  // Call base class function
        cout << "Derived display" << endl;
    }
};

int main() {
    Derived d;
    d.display();
}
```

**Output**:

```
Base display
Derived display
```

---

# 2. Polymorphism

## What is Polymorphism?

**Polymorphism** means "many forms". It allows objects of different classes to be treated as objects of a common base class.

**Real-World Analogy**: A person can be a student, employee, and customer at the same time. The same person behaves differently in each role.

### Types of Polymorphism

1. **Compile-time Polymorphism** (Static)
    
    - Function overloading
    - Operator overloading
2. **Runtime Polymorphism** (Dynamic)
    
    - Virtual functions
    - Function overriding

---

## Compile-Time Polymorphism

### Function Overloading (Already Learned)

```cpp
class Calculator {
public:
    int add(int a, int b) {
        return a + b;
    }
    
    double add(double a, double b) {
        return a + b;
    }
    
    int add(int a, int b, int c) {
        return a + b + c;
    }
};
```

---

## Runtime Polymorphism

### The Problem Without Virtual Functions

```cpp
class Animal {
public:
    void makeSound() {
        cout << "Animal sound" << endl;
    }
};

class Dog : public Animal {
public:
    void makeSound() {
        cout << "Woof!" << endl;
    }
};

int main() {
    Animal* ptr;
    Dog d;
    
    ptr = &d;
    ptr->makeSound();  // Calls Animal::makeSound() - Wrong!
    
    return 0;
}
```

**Output**: `Animal sound` (Not what we want!)

---

# 3. Virtual Functions

## What are Virtual Functions?

**Virtual functions** enable runtime polymorphism. They allow derived class functions to be called through base class pointers.

### Syntax

```cpp
class Base {
public:
    virtual void functionName() {
        // Base implementation
    }
};
```

### Example with Virtual Function

```cpp
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void makeSound() {  // Virtual function
        cout << "Animal sound" << endl;
    }
};

class Dog : public Animal {
public:
    void makeSound() override {  // Override base function
        cout << "Woof! Woof!" << endl;
    }
};

class Cat : public Animal {
public:
    void makeSound() override {
        cout << "Meow! Meow!" << endl;
    }
};

int main() {
    Animal* ptr;
    
    Dog d;
    Cat c;
    
    ptr = &d;
    ptr->makeSound();  // Calls Dog::makeSound() - Correct!
    
    ptr = &c;
    ptr->makeSound();  // Calls Cat::makeSound() - Correct!
    
    return 0;
}
```

**Output**:

```
Woof! Woof!
Meow! Meow!
```

### The `override` Keyword (C++11)

Use `override` to explicitly indicate you're overriding a virtual function.

```cpp
class Base {
public:
    virtual void func() { }
};

class Derived : public Base {
public:
    void func() override {  // Recommended
        // If Base::func() isn't virtual, compiler error
    }
};
```

---

## Virtual Destructor

**Important**: Always make destructor virtual in base class if you're using polymorphism!

### The Problem

```cpp
class Base {
public:
    ~Base() {  // Non-virtual destructor
        cout << "Base destructor" << endl;
    }
};

class Derived : public Base {
public:
    ~Derived() {
        cout << "Derived destructor" << endl;
    }
};

int main() {
    Base* ptr = new Derived();
    delete ptr;  // Only Base destructor called! Memory leak!
    return 0;
}
```

**Output**: `Base destructor` (Derived destructor not called!)

### The Solution

```cpp
class Base {
public:
    virtual ~Base() {  // Virtual destructor
        cout << "Base destructor" << endl;
    }
};

class Derived : public Base {
public:
    ~Derived() {
        cout << "Derived destructor" << endl;
    }
};

int main() {
    Base* ptr = new Derived();
    delete ptr;  // Both destructors called!
    return 0;
}
```

**Output**:

```
Derived destructor
Base destructor
```

---

# 4. Abstract Classes and Pure Virtual Functions

## What is an Abstract Class?

An **abstract class** is a class that cannot be instantiated. It serves as a base class for other classes.

### Pure Virtual Function

A **pure virtual function** has no implementation in the base class.

**Syntax**:

```cpp
virtual returnType functionName() = 0;  // = 0 makes it pure virtual
```

### Example

```cpp
#include <iostream>
using namespace std;

// Abstract class
class Shape {
public:
    // Pure virtual functions
    virtual double getArea() = 0;
    virtual double getPerimeter() = 0;
    
    // Regular virtual function
    virtual void display() {
        cout << "This is a shape" << endl;
    }
};

// Concrete class (implements pure virtual functions)
class Circle : public Shape {
private:
    double radius;

public:
    Circle(double r) : radius(r) { }
    
    double getArea() override {
        return 3.14159 * radius * radius;
    }
    
    double getPerimeter() override {
        return 2 * 3.14159 * radius;
    }
};

class Rectangle : public Shape {
private:
    double length, width;

public:
    Rectangle(double l, double w) : length(l), width(w) { }
    
    double getArea() override {
        return length * width;
    }
    
    double getPerimeter() override {
        return 2 * (length + width);
    }
};

int main() {
    // Shape s;  // Error! Cannot instantiate abstract class
    
    Shape* shapes[2];
    
    shapes[0] = new Circle(5.0);
    shapes[1] = new Rectangle(4.0, 6.0);
    
    for (int i = 0; i < 2; i++) {
        cout << "Area: " << shapes[i]->getArea() << endl;
        cout << "Perimeter: " << shapes[i]->getPerimeter() << endl;
        cout << "---" << endl;
    }
    
    delete shapes[0];
    delete shapes[1];
    
    return 0;
}
```

### Interface (Pure Abstract Class)

A class with **only** pure virtual functions is called an **interface**.

```cpp
class Printable {
public:
    virtual void print() = 0;
    virtual ~Printable() { }
};

class Document : public Printable {
public:
    void print() override {
        cout << "Printing document..." << endl;
    }
};

class Image : public Printable {
public:
    void print() override {
        cout << "Printing image..." << endl;
    }
};
```

---

# 5. Operator Overloading

## What is Operator Overloading?

**Operator overloading** allows you to redefine operators (+, -, *, ==, etc.) for user-defined types.

### Syntax

```cpp
returnType operator symbol(parameters) {
    // Implementation
}
```

### Simple Example: Complex Number

```cpp
#include <iostream>
using namespace std;

class Complex {
private:
    double real;
    double imag;

public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) { }
    
    // Overload + operator
    Complex operator+(const Complex& c) {
        Complex temp;
        temp.real = real + c.real;
        temp.imag = imag + c.imag;
        return temp;
    }
    
    // Overload - operator
    Complex operator-(const Complex& c) {
        Complex temp;
        temp.real = real - c.real;
        temp.imag = imag - c.imag;
        return temp;
    }
    
    void display() {
        cout << real;
        if (imag >= 0)
            cout << " + " << imag << "i" << endl;
        else
            cout << " - " << -imag << "i" << endl;
    }
};

int main() {
    Complex c1(3.0, 4.0);
    Complex c2(1.0, 2.0);
    
    Complex c3 = c1 + c2;  // Calls c1.operator+(c2)
    Complex c4 = c1 - c2;
    
    cout << "c1: "; c1.display();
    cout << "c2: "; c2.display();
    cout << "c3 (c1 + c2): "; c3.display();
    cout << "c4 (c1 - c2): "; c4.display();
    
    return 0;
}
```

**Output**:

```
c1: 3 + 4i
c2: 1 + 2i
c3 (c1 + c2): 4 + 6i
c4 (c1 - c2): 2 + 2i
```

### Commonly Overloaded Operators

**Arithmetic**: +, -, *, /, %  
**Comparison**: ==, !=, <, >, <=, >=  
**Assignment**: =, +=, -=, *=, /=  
**Increment/Decrement**: ++, --  
**Stream**: <<, >>  
**Subscript**: []  
**Function call**: ()

### Overloading << Operator (Stream Insertion)

```cpp
class Point {
private:
    int x, y;

public:
    Point(int x = 0, int y = 0) : x(x), y(y) { }
    
    // Friend function to overload <<
    friend ostream& operator<<(ostream& out, const Point& p);
};

ostream& operator<<(ostream& out, const Point& p) {
    out << "(" << p.x << ", " << p.y << ")";
    return out;
}

int main() {
    Point p(3, 4);
    cout << "Point: " << p << endl;  // Point: (3, 4)
}
```

### Overloading == Operator

```cpp
class Point {
private:
    int x, y;

public:
    Point(int x = 0, int y = 0) : x(x), y(y) { }
    
    bool operator==(const Point& p) const {
        return (x == p.x && y == p.y);
    }
    
    bool operator!=(const Point& p) const {
        return !(*this == p);
    }
};

int main() {
    Point p1(3, 4);
    Point p2(3, 4);
    Point p3(5, 6);
    
    if (p1 == p2) {
        cout << "p1 and p2 are equal" << endl;
    }
    
    if (p1 != p3) {
        cout << "p1 and p3 are not equal" << endl;
    }
}
```

### Overloading ++ Operator

```cpp
class Counter {
private:
    int count;

public:
    Counter(int c = 0) : count(c) { }
    
    // Prefix increment: ++count
    Counter& operator++() {
        ++count;
        return *this;
    }
    
    // Postfix increment: count++
    Counter operator++(int) {  // int is dummy parameter
        Counter temp = *this;
        count++;
        return temp;
    }
    
    void display() {
        cout << "Count: " << count << endl;
    }
};

int main() {
    Counter c(5);
    
    ++c;  // Prefix
    c.display();  // 6
    
    c++;  // Postfix
    c.display();  // 7
}
```

---

# 6. Friend Functions and Friend Classes

## Friend Functions

A **friend function** can access private and protected members of a class.

### Syntax

```cpp
class ClassName {
    friend returnType functionName(parameters);
};
```

### Example

```cpp
#include <iostream>
using namespace std;

class Box {
private:
    double width;

public:
    Box(double w) : width(w) { }
    
    // Declare friend function
    friend void printWidth(Box& b);
};

// Friend function definition
void printWidth(Box& b) {
    cout << "Width: " << b.width << endl;  // Can access private member!
}

int main() {
    Box box(10.5);
    printWidth(box);  // Width: 10.5
}
```

### Why Use Friend Functions?

1. **Operator overloading** - Stream operators (<<, >>)
2. **Access multiple classes** - Function needs access to multiple classes
3. **Special operations** - Operations that don't fit as member functions

## Friend Classes

One class can be a friend of another class.

```cpp
class B;  // Forward declaration

class A {
private:
    int dataA;

public:
    A(int d) : dataA(d) { }
    
    friend class B;  // B is friend of A
};

class B {
public:
    void display(A& a) {
        cout << "Data from A: " << a.dataA << endl;  // Can access private!
    }
};

int main() {
    A objA(42);
    B objB;
    
    objB.display(objA);
}
```

---

# 7. Static Members

## Static Data Members

**Static members** belong to the class, not to any object. All objects share the same static member.

### Syntax

```cpp
class ClassName {
private:
    static dataType variableName;
};

// Initialize outside class
dataType ClassName::variableName = value;
```

### Example: Count Objects

```cpp
#include <iostream>
using namespace std;

class Student {
private:
    static int count;  // Static member
    string name;

public:
    Student(string n) : name(n) {
        count++;  // Increment when object created
    }
    
    ~Student() {
        count--;  // Decrement when object destroyed
    }
    
    static int getCount() {  // Static member function
        return count;
    }
};

// Initialize static member
int Student::count = 0;

int main() {
    cout << "Initial count: " << Student::getCount() << endl;
    
    Student s1("Alice");
    Student s2("Bob");
    
    cout << "After creating 2 students: " << Student::getCount() << endl;
    
    {
        Student s3("Charlie");
        cout << "After creating 3rd student: " << Student::getCount() << endl;
    }  // s3 destroyed here
    
    cout << "After s3 destroyed: " << Student::getCount() << endl;
    
    return 0;
}
```

**Output**:

```
Initial count: 0
After creating 2 students: 2
After creating 3rd student: 3
After s3 destroyed: 2
```

## Static Member Functions

**Static member functions** can only access static members.

```cpp
class MyClass {
private:
    static int staticVar;
    int normalVar;

public:
    static void staticFunction() {
        staticVar = 10;      // OK
        // normalVar = 20;   // Error! Can't access non-static member
    }
};
```

### Accessing Static Members

```cpp
// Through class name (preferred)
ClassName::staticMember;
ClassName::staticFunction();

// Through object (also works)
object.staticMember;
object.staticFunction();
```

---

# 8. Multiple Inheritance

## What is Multiple Inheritance?

A class can inherit from **multiple** base classes.

### Syntax

```cpp
class Derived : public Base1, public Base2 {
    // Inherits from both Base1 and Base2
};
```

### Example

```cpp
#include <iostream>
using namespace std;

class Father {
protected:
    string fatherName;

public:
    void setFatherName(string name) {
        fatherName = name;
    }
};

class Mother {
protected:
    string motherName;

public:
    void setMotherName(string name) {
        motherName = name;
    }
};

class Child : public Father, public Mother {
private:
    string childName;

public:
    void setChildName(string name) {
        childName = name;
    }
    
    void display() {
        cout << "Father: " << fatherName << endl;
        cout << "Mother: " << motherName << endl;
        cout << "Child: " << childName << endl;
    }
};

int main() {
    Child c;
    
    c.setFatherName("John");
    c.setMotherName("Mary");
    c.setChildName("Tom");
    
    c.display();
    
    return 0;
}
```

## The Diamond Problem

**Problem**: Ambiguity when two base classes have a common ancestor.

```cpp
     A
    / \
   B   C
    \ /
     D
```

### Solution: Virtual Inheritance

```cpp
class A {
public:
    int data;
};

class B : virtual public A {
    // B virtually inherits from A
};

class C : virtual public A {
    // C virtually inherits from A
};

class D : public B, public C {
    // D has only ONE copy of A
};
```

---

# 9. Practical Projects

### Project 1: Employee Management System

```cpp
#include <iostream>
#include <string>
using namespace std;

// Abstract base class
class Employee {
protected:
    string name;
    int id;
    double baseSalary;

public:
    Employee(string n, int i, double sal) : name(n), id(i), baseSalary(sal) { }
    
    virtual double calculateSalary() = 0;  // Pure virtual
    
    virtual void display() {
        cout << "Name: " << name << endl;
        cout << "ID: " << id << endl;
        cout << "Base Salary: $" << baseSalary << endl;
    }
    
    virtual ~Employee() { }
};

class Manager : public Employee {
private:
    double bonus;

public:
    Manager(string n, int i, double sal, double b) 
        : Employee(n, i, sal), bonus(b) { }
    
    double calculateSalary() override {
        return baseSalary + bonus;
    }
    
    void display() override {
        Employee::display();
        cout << "Bonus: $" << bonus << endl;
        cout << "Total Salary: $" << calculateSalary() << endl;
    }
};

class Engineer : public Employee {
private:
    int projectsCompleted;
    double projectBonus;

public:
    Engineer(string n, int i, double sal, int projects) 
        : Employee(n, i, sal), projectsCompleted(projects), projectBonus(500.0) { }
    
    double calculateSalary() override {
        return baseSalary + (projectsCompleted * projectBonus);
    }
    
    void display() override {
        Employee::display();
        cout << "Projects Completed: " << projectsCompleted << endl;
        cout << "Total Salary: $" << calculateSalary() << endl;
    }
};

class Salesperson : public Employee {
private:
    double salesAmount;
    double commissionRate;

public:
    Salesperson(string n, int i, double sal, double sales) 
        : Employee(n, i, sal), salesAmount(sales), commissionRate(0.05) { }
    
    double calculateSalary() override {
        return baseSalary + (salesAmount * commissionRate);
    }
    
    void display() override {
        Employee::display();
        cout << "Sales Amount: $" << salesAmount << endl;
        cout << "Commission Rate: " << (commissionRate * 100) << "%" << endl;
        cout << "Total Salary: $" << calculateSalary() << endl;
    }
};

int main() {
    Employee* employees[3];
    
    employees[0] = new Manager("Alice Johnson", 101, 5000.0, 2000.0);
    employees[1] = new Engineer("Bob Smith", 102, 4000.0, 5);
    employees[2] = new Salesperson("Charlie Brown", 103, 3000.0, 50000.0);
    
    for (int i = 0; i < 3; i++) {
        cout << "\n=== Employee " << (i + 1) << " ===" << endl;
        employees[i]->display();
    }
    
    // Clean up
    for (int i = 0; i < 3; i++) {
        delete employees[i];
    }
    
    return 0;
}
```

### Project 2: Shape Calculator with Polymorphism

```cpp
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class Shape {
protected:
    string color;

public:
    Shape(string c = "White") : color(c) { }
    
    virtual double getArea() = 0;
    virtual double getPerimeter() = 0;
    virtual void display() = 0;
    
    virtual ~Shape() { }
    
    string getColor() const { return color; }
    void setColor(string c) { color = c; }
};

class Circle : public Shape {
private:
    double radius;
    const double PI = 3.14159;

public:
    Circle(double r, string c = "White") : Shape(c), radius(r) { }
    
    double getArea() override {
        return PI * radius * radius;
    }
    
    double getPerimeter() override {
        return 2 * PI * radius;
    }
    
    void display() override {
        cout << "Circle:" << endl;
        cout << "  Color: " << color << endl;
        cout << "  Radius: " << radius << endl;
        cout << "  Area: " << getArea() << endl;
        cout << "  Perimeter: " << getPerimeter() << endl;
    }
};

class Rectangle : public Shape {
private:
    double length, width;

public:
    Rectangle(double l, double w, string c = "White") 
        : Shape(c), length(l), width(w) { }
    
    double getArea() override {
        return length * width;
    }
    
    double getPerimeter() override {
        return 2 * (length + width);
    }
    
    void display() override {
        cout << "Rectangle:" << endl;
        cout << "  Color: " << color << endl;
        cout << "  Length: " << length << endl;
        cout << "  Width: " << width << endl;
        cout << "  Area: " << getArea() << endl;
        cout << "  Perimeter: " << getPerimeter() << endl;
    }
};

class Triangle : public Shape {
private:
    double side1, side2, side3;

public:
    Triangle(double s1, double s2, double s3, string c = "White")
        : Shape(c), side1(s1), side2(s2), side3(s3) { }
    
    double getArea() override {
        double s = (side1 + side2 + side3) / 2;  // Semi-perimeter
        return sqrt(s * (s - side1) * (s - side2) * (s - side3));
    }
    
    double getPerimeter() override {
        return side1 + side2 + side3;
    }
    
    void display() override {
        cout << "Triangle:" << endl;
        cout << "  Color: " << color << endl;
        cout << "  Sides: " << side1 << ", " << side2 << ", " << side3 << endl;
        cout << "  Area: " << getArea() << endl;
        cout << "  Perimeter: " << getPerimeter() << endl;
    }
};

int main() {
    vector<Shape*> shapes;
    
    shapes.push_back(new Circle(5.0, "Red"));
    shapes.push_back(new Rectangle(4.0, 6.0, "Blue"));
    shapes.push_back(new Triangle(3.0, 4.0, 5.0, "Green"));
    
    double totalArea = 0;
    
    for (Shape* shape : shapes) {
        shape->display();
        totalArea += shape->getArea();
        cout << endl;
    }
    
    cout << "Total area of all shapes: " << totalArea << endl;
    
    // Clean up
    for (Shape* shape : shapes) {
        delete shape;
    }
    
    return 0;
}
```

### Project 3: Bank Account Hierarchy

```cpp
#include <iostream>
#include <string>
using namespace std;

class Account {
protected:
    string accountNumber;
    string holderName;
    double balance;
    static int accountCount;

public:
    Account(string accNum, string name, double bal) 
        : accountNumber(accNum), holderName(name), balance(bal) {
        accountCount++;
    }
    
    virtual ~Account() {
        accountCount--;
    }
    
    virtual void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposited: $" << amount << endl;
        }
    }
    
    virtual bool withdraw(double amount) = 0;  // Pure virtual
    
    virtual void display() {
        cout << "Account Number: " << accountNumber << endl;
        cout << "Holder Name: " << holderName << endl;
        cout << "Balance: $" << balance << endl;
    }
    
    static int getAccountCount() {
        return accountCount;
    }
    
    double getBalance() const { return balance; }
};

int Account::accountCount = 0;

class SavingsAccount : public Account {
private:
    double interestRate;
    int withdrawalLimit;
    int withdrawalCount;

public:
    SavingsAccount(string accNum, string name, double bal, double rate)
        : Account(accNum, name, bal), interestRate(rate), 
          withdrawalLimit(3), withdrawalCount(0) { }
    
    bool withdraw(double amount) override {
        if (withdrawalCount >= withdrawalLimit) {
            cout << "Withdrawal limit reached!" << endl;
            return false;
        }
        
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            withdrawalCount++;
            cout << "Withdrawn: $" << amount << endl;
            return true;
        }
        
        cout << "Invalid withdrawal amount!" << endl;
        return false;
    }
    
    void addInterest() {
        double interest = balance * interestRate / 100;
        balance += interest;
        cout << "Interest added: $" << interest << endl;
    }
    
    void display() override {
        cout << "\n=== Savings Account ===" << endl;
        Account::display();
        cout << "Interest Rate: " << interestRate << "%" << endl;
        cout << "Withdrawals: " << withdrawalCount << "/" << withdrawalLimit << endl;
    }
};

class CheckingAccount : public Account {
private:
    double overdraftLimit;

public:
    CheckingAccount(string accNum, string name, double bal, double overdraft)
        : Account(accNum, name, bal), overdraftLimit(overdraft) { }
    
    bool withdraw(double amount) override {
        if (amount > 0 && amount <= balance + overdraftLimit) {
            balance -= amount;
            cout << "Withdrawn: $" << amount << endl;
            
            if (balance < 0) {
                cout << "Warning: Using overdraft!" << endl;
            }
            
            return true;
        }
        
        cout << "Withdrawal exceeds limit!" << endl;
        return false;
    }
    
    void display() override {
        cout << "\n=== Checking Account ===" << endl;
        Account::display();
        cout << "Overdraft Limit: $" << overdraftLimit << endl;
        
        if (balance < 0) {
            cout << "Overdraft Used: $" << (-balance) << endl;
        }
    }
};

int main() {
    cout << "Total accounts: " << Account::getAccountCount() << endl;
    
    SavingsAccount savings("SAV001", "Alice", 1000.0, 3.5);
    CheckingAccount checking("CHK001", "Bob", 500.0, 200.0);
    
    cout << "\nTotal accounts: " << Account::getAccountCount() << endl;
    
    savings.deposit(500);
    savings.withdraw(200);
    savings.addInterest();
    savings.display();
    
    checking.deposit(300);
    checking.withdraw(600);
    checking.withdraw(300);
    checking.display();
    
    return 0;
}
```

---

## Summary

### Key Concepts Mastered

1. ✅ **Inheritance** - Code reuse through class hierarchies
2. ✅ **Polymorphism** - One interface, multiple implementations
3. ✅ **Virtual Functions** - Runtime polymorphism
4. ✅ **Abstract Classes** - Pure virtual functions for interfaces
5. ✅ **Operator Overloading** - Custom operators for classes
6. ✅ **Friend Functions** - Special access to private members
7. ✅ **Static Members** - Class-level data and functions
8. ✅ **Multiple Inheritance** - Inherit from multiple bases

### Best Practices

✅ Make destructors virtual in base classes  
✅ Use `override` keyword for clarity  
✅ Prefer composition over inheritance when possible  
✅ Use abstract classes to define interfaces  
✅ Initialize base class constructors properly  
✅ Be careful with multiple inheritance  
✅ Use friend functions sparingly  
✅ Make static members truly class-wide

### Common Patterns

**Factory Pattern**: Create objects polymorphically  
**Template Method**: Define algorithm skeleton in base class  
**Strategy Pattern**: Swap algorithms using polymorphism  
**Observer Pattern**: Notify dependent objects

---

## Conclusion

You've now mastered **Advanced OOP Concepts**! These powerful techniques enable:

- **Elegant design** through inheritance hierarchies
- **Flexible code** with polymorphism
- **Clean interfaces** using abstract classes
- **Natural syntax** with operator overloading
- **Efficient sharing** with static members

**Congratulations!** You can now design and implement professional object-oriented systems in C++! 🎉

**Happy Coding!** 🚀