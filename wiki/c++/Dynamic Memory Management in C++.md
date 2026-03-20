## Introduction

So far, you've worked with variables that are created at compile time and destroyed automatically when they go out of scope. But what if you need to:

- Create arrays whose size isn't known until runtime?
- Control exactly when objects are created and destroyed?
- Build data structures like linked lists or trees?
- Manage memory efficiently in large applications?

**Dynamic Memory Management** gives you this control by allowing you to allocate and deallocate memory at runtime.

**Real-World Analogies**:

- **Hotel Rooms**: Static memory is like owning a house (fixed size). Dynamic memory is like booking hotel rooms (rent when needed, release when done)
- **Parking Lot**: Reserve a spot when you arrive, free it when you leave
- **Library Books**: Check out books when needed, return them when finished
- **Restaurant Tables**: Request a table, use it, then it's available for others

---

## Why Dynamic Memory Management Matters

### Static Memory (What You Know)

```cpp
int numbers[5];  // Fixed size - must know at compile time
```

**Limitations**:

- Size must be known at compile time
- Cannot change size
- Memory allocated on stack (limited)
- Automatically destroyed when out of scope

### Dynamic Memory (New!)

```cpp
int* numbers = new int[size];  // Size determined at runtime!
// Use the array
delete[] numbers;  // Manually free memory
```

**Advantages**:

- Size determined at runtime
- Can resize (by creating new array)
- Memory allocated on heap (much larger)
- Precise control over lifetime
- Essential for data structures

### Benefits of Dynamic Memory

1. **Flexibility** - Size determined at runtime
2. **Large memory** - Heap is much bigger than stack
3. **Persistence** - Lives beyond function scope
4. **Data structures** - Enable linked lists, trees, graphs
5. **Memory efficiency** - Allocate only what you need
6. **Resource management** - Precise control

---

## The Stack vs The Heap

### Memory Layout

```
┌─────────────────────┐
│    Stack            │  ← Local variables, function calls
│    (Limited)        │  ← Automatic management
├─────────────────────┤
│                     │
│    Heap             │  ← Dynamic memory
│    (Much Larger)    │  ← Manual management
│                     │
└─────────────────────┘
```

### Stack Memory

```cpp
void function() {
    int x = 10;        // On stack
    int arr[5];        // On stack
    
}  // x and arr automatically destroyed
```

**Characteristics**:

- Automatic allocation/deallocation
- Fast
- Limited size (typically 1-8 MB)
- LIFO (Last In, First Out)

### Heap Memory

```cpp
void function() {
    int* ptr = new int;        // On heap
    int* arr = new int[100];   // On heap
    
    delete ptr;
    delete[] arr;  // Must manually delete!
}
```

**Characteristics**:

- Manual allocation/deallocation
- Slower than stack
- Large size (limited by system RAM)
- Flexible lifetime

---

## The 'new' Operator

### Allocating Single Variable

**Syntax**:

```cpp
dataType* pointer = new dataType;
```

**Example**:

```cpp
int* ptr = new int;        // Allocate one int
*ptr = 42;                 // Assign value
cout << *ptr << endl;      // Use it
delete ptr;                // Free memory
```

### Allocating with Initialization

```cpp
int* ptr = new int(42);           // Initialize to 42
double* dptr = new double(3.14);  // Initialize to 3.14
string* sptr = new string("Hello"); // Initialize string
```

### Allocating Arrays

**Syntax**:

```cpp
dataType* pointer = new dataType[size];
```

**Example**:

```cpp
int size = 5;
int* numbers = new int[size];  // Allocate array of 5 ints

// Use array
for (int i = 0; i < size; i++) {
    numbers[i] = i * 10;
}

// Display
for (int i = 0; i < size; i++) {
    cout << numbers[i] << " ";
}

delete[] numbers;  // Free array memory
```

### Complete Example

```cpp
#include <iostream>
using namespace std;

int main() {
    int size;
    
    cout << "How many numbers? ";
    cin >> size;
    
    // Dynamically allocate array
    int* numbers = new int[size];
    
    // Input values
    cout << "Enter " << size << " numbers:" << endl;
    for (int i = 0; i < size; i++) {
        cin >> numbers[i];
    }
    
    // Calculate sum
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += numbers[i];
    }
    
    cout << "Sum: " << sum << endl;
    cout << "Average: " << (double)sum / size << endl;
    
    // Free memory
    delete[] numbers;
    
    return 0;
}
```

---

## The 'delete' Operator

### Deleting Single Variable

```cpp
int* ptr = new int(42);
// Use ptr
delete ptr;  // Free memory
```

### Deleting Arrays

```cpp
int* arr = new int[10];
// Use array
delete[] arr;  // Note the []!
```

### ⚠️ CRITICAL: delete vs delete[]

```cpp
// Single object
int* ptr = new int;
delete ptr;        // Correct

// Array
int* arr = new int[10];
delete[] arr;      // Correct - use delete[]
```

**WRONG**:

```cpp
int* arr = new int[10];
delete arr;        // ❌ WRONG! Memory leak!
```

**Rule**: Use `delete[]` for arrays, `delete` for single objects!

---

## Memory Leaks

### What is a Memory Leak?

A **memory leak** occurs when dynamically allocated memory is not freed, causing your program to gradually consume more and more memory.

### Example of Memory Leak

```cpp
void badFunction() {
    int* ptr = new int(42);
    // Forgot to delete!
}  // ptr goes out of scope, but memory is NOT freed!

int main() {
    for (int i = 0; i < 1000000; i++) {
        badFunction();  // Leaks memory every call!
    }
    // Program has leaked millions of integers!
}
```

### Fixing Memory Leaks

```cpp
void goodFunction() {
    int* ptr = new int(42);
    // Use ptr
    delete ptr;  // ✓ Properly freed
}
```

### Common Causes of Memory Leaks

1. **Forgetting to delete**:
    
    ```cpp
    int* ptr = new int;
    // No delete!
    ```
    
2. **Early return**:
    
    ```cpp
    void function() {
        int* ptr = new int;
        if (condition) {
            return;  // Leak! Forgot to delete
        }
        delete ptr;
    }
    ```
    
3. **Exception thrown**:
    
    ```cpp
    void function() {
        int* ptr = new int;
        throw exception();  // Leak if exception occurs
        delete ptr;  // Never reached!
    }
    ```
    
4. **Losing pointer**:
    
    ```cpp
    int* ptr = new int(10);
    ptr = new int(20);  // Leak! Lost first allocation
    ```
    

---

## Dangling Pointers

### What is a Dangling Pointer?

A **dangling pointer** points to memory that has been freed.

### Example

```cpp
int* ptr = new int(42);
delete ptr;
cout << *ptr << endl;  // ❌ Dangling pointer! Undefined behavior
```

### Avoiding Dangling Pointers

**Set pointer to nullptr after delete**:

```cpp
int* ptr = new int(42);
delete ptr;
ptr = nullptr;  // ✓ Now safe

if (ptr != nullptr) {
    cout << *ptr << endl;  // Won't execute
}
```

### Double Delete Problem

```cpp
int* ptr = new int(42);
delete ptr;
delete ptr;  // ❌ Double delete! Undefined behavior
```

**Solution**:

```cpp
int* ptr = new int(42);
delete ptr;
ptr = nullptr;
delete ptr;  // Safe - deleting nullptr does nothing
```

---

## Dynamic Arrays

### Creating Dynamic Arrays

```cpp
int size;
cout << "Enter size: ";
cin >> size;

int* arr = new int[size];  // Runtime size!

// Use array
for (int i = 0; i < size; i++) {
    arr[i] = i + 1;
}

delete[] arr;
```

### 2D Dynamic Arrays

**Method 1: Array of pointers**

```cpp
int rows = 3, cols = 4;

// Allocate array of pointers
int** matrix = new int*[rows];

// Allocate each row
for (int i = 0; i < rows; i++) {
    matrix[i] = new int[cols];
}

// Use matrix
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        matrix[i][j] = i * cols + j;
    }
}

// Free memory (reverse order)
for (int i = 0; i < rows; i++) {
    delete[] matrix[i];  // Free each row
}
delete[] matrix;  // Free array of pointers
```

**Visual representation**:

```
matrix → [ptr][ptr][ptr]    ← Array of pointers
          ↓    ↓    ↓
         [  ][  ][  ][  ]   ← Row 0
         [  ][  ][  ][  ]   ← Row 1
         [  ][  ][  ][  ]   ← Row 2
```

**Method 2: Single allocation (preferred for efficiency)**

```cpp
int rows = 3, cols = 4;

// Allocate as single block
int* matrix = new int[rows * cols];

// Access using formula: matrix[i * cols + j]
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        matrix[i * cols + j] = i * cols + j;
    }
}

delete[] matrix;  // Single delete
```

---

## Dynamic Objects

### Creating Dynamic Objects

```cpp
class Student {
public:
    string name;
    int age;
    
    Student(string n, int a) : name(n), age(a) {
        cout << "Student created: " << name << endl;
    }
    
    ~Student() {
        cout << "Student destroyed: " << name << endl;
    }
};

int main() {
    // Static object
    Student s1("Alice", 20);  // On stack
    
    // Dynamic object
    Student* s2 = new Student("Bob", 21);  // On heap
    
    cout << s2->name << endl;  // Access with ->
    
    delete s2;  // Must delete! Calls destructor
    
    return 0;
}  // s1 automatically destroyed
```

### Array of Dynamic Objects

```cpp
int numStudents = 3;
Student* students = new Student[numStudents]{
    Student("Alice", 20),
    Student("Bob", 21),
    Student("Charlie", 19)
};

// Use students
for (int i = 0; i < numStudents; i++) {
    cout << students[i].name << endl;
}

delete[] students;  // Calls destructor for each
```

---

## this Pointer and Dynamic Memory

### Returning 'this' for Method Chaining

```cpp
class Counter {
private:
    int count;

public:
    Counter() : count(0) { }
    
    Counter* increment() {
        count++;
        return this;  // Return pointer to self
    }
    
    Counter* decrement() {
        count--;
        return this;
    }
    
    void display() {
        cout << "Count: " << count << endl;
    }
};

int main() {
    Counter* c = new Counter();
    
    c->increment()->increment()->increment();
    c->display();  // Count: 3
    
    delete c;
}
```

---

## The Rule of Three

When a class manages resources (like dynamic memory), you need three special functions:

1. **Destructor** - Free resources
2. **Copy Constructor** - Deep copy resources
3. **Copy Assignment Operator** - Deep copy during assignment

### Without Rule of Three (Shallow Copy Problem)

```cpp
class Array {
private:
    int* data;
    int size;

public:
    Array(int s) : size(s) {
        data = new int[size];
    }
    
    ~Array() {
        delete[] data;
    }
};

int main() {
    Array a1(5);
    Array a2 = a1;  // Shallow copy - both point to same data!
    
    return 0;
}  // Double delete! Crash!
```

### With Rule of Three (Deep Copy)

```cpp
class Array {
private:
    int* data;
    int size;

public:
    // Constructor
    Array(int s) : size(s) {
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = 0;
        }
    }
    
    // Destructor
    ~Array() {
        delete[] data;
    }
    
    // Copy Constructor
    Array(const Array& other) : size(other.size) {
        data = new int[size];  // Deep copy
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
    
    // Copy Assignment Operator
    Array& operator=(const Array& other) {
        if (this != &other) {  // Check self-assignment
            delete[] data;  // Free old data
            
            size = other.size;
            data = new int[size];  // Allocate new
            
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];  // Copy
            }
        }
        return *this;
    }
    
    int& operator[](int index) {
        return data[index];
    }
};

int main() {
    Array a1(5);
    a1[0] = 10;
    
    Array a2 = a1;       // Copy constructor
    Array a3(3);
    a3 = a1;             // Copy assignment
    
    return 0;
}  // All destructors work correctly!
```

---

## Smart Pointers (C++11 and Later)

Modern C++ provides **smart pointers** that automatically manage memory.

### Problems with Raw Pointers

```cpp
void function() {
    int* ptr = new int(42);
    // If exception occurs, delete never called!
    delete ptr;
}
```

### unique_ptr

**Exclusive ownership** - Only one pointer can own the resource.

```cpp
#include <memory>
using namespace std;

int main() {
    unique_ptr<int> ptr(new int(42));  // Or: make_unique<int>(42)
    
    cout << *ptr << endl;  // Use like regular pointer
    
    // No delete needed! Automatically freed
    
    return 0;
}
```

**Cannot be copied**:

```cpp
unique_ptr<int> ptr1(new int(42));
// unique_ptr<int> ptr2 = ptr1;  // Error! Cannot copy

unique_ptr<int> ptr2 = move(ptr1);  // ✓ Can move
// ptr1 is now nullptr
```

### shared_ptr

**Shared ownership** - Multiple pointers can share the resource.

```cpp
#include <memory>

int main() {
    shared_ptr<int> ptr1(new int(42));  // Or: make_shared<int>(42)
    
    {
        shared_ptr<int> ptr2 = ptr1;  // ✓ Can copy
        cout << *ptr2 << endl;
        cout << "Count: " << ptr1.use_count() << endl;  // 2
    }  // ptr2 destroyed, but memory still alive
    
    cout << "Count: " << ptr1.use_count() << endl;  // 1
    
    return 0;
}  // Memory automatically freed when last shared_ptr is destroyed
```

### weak_ptr

**Non-owning reference** - Observes shared_ptr without affecting count.

```cpp
shared_ptr<int> sptr = make_shared<int>(42);
weak_ptr<int> wptr = sptr;  // Weak reference

cout << wptr.use_count() << endl;  // 1 (only sptr owns)

if (auto locked = wptr.lock()) {  // Try to access
    cout << *locked << endl;
}
```

### Using Smart Pointers (Best Practice)

```cpp
#include <memory>
#include <vector>

class Student {
public:
    string name;
    Student(string n) : name(n) {
        cout << "Created: " << name << endl;
    }
    ~Student() {
        cout << "Destroyed: " << name << endl;
    }
};

int main() {
    // Modern way - no manual delete needed!
    auto s1 = make_unique<Student>("Alice");
    auto s2 = make_shared<Student>("Bob");
    
    vector<shared_ptr<Student>> students;
    students.push_back(make_shared<Student>("Charlie"));
    students.push_back(make_shared<Student>("David"));
    
    // All memory automatically managed!
    
    return 0;
}
```

---

## Practical Examples and Projects

### Example 1: Dynamic Array Manager

```cpp
#include <iostream>
using namespace std;

class DynamicArray {
private:
    int* data;
    int size;
    int capacity;

public:
    DynamicArray(int cap = 10) : size(0), capacity(cap) {
        data = new int[capacity];
    }
    
    ~DynamicArray() {
        delete[] data;
    }
    
    void add(int value) {
        if (size >= capacity) {
            resize();
        }
        data[size++] = value;
    }
    
    void resize() {
        capacity *= 2;
        int* newData = new int[capacity];
        
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        
        delete[] data;
        data = newData;
        
        cout << "Resized to capacity: " << capacity << endl;
    }
    
    int get(int index) const {
        if (index >= 0 && index < size) {
            return data[index];
        }
        throw out_of_range("Index out of bounds");
    }
    
    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
    
    void display() const {
        cout << "Array [size=" << size << ", capacity=" << capacity << "]: ";
        for (int i = 0; i < size; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    DynamicArray arr(5);
    
    for (int i = 1; i <= 12; i++) {
        arr.add(i * 10);
        arr.display();
    }
    
    return 0;
}
```

### Example 2: Simple Linked List

```cpp
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    
    Node(int val) : data(val), next(nullptr) { }
};

class LinkedList {
private:
    Node* head;
    int size;

public:
    LinkedList() : head(nullptr), size(0) { }
    
    ~LinkedList() {
        clear();
    }
    
    void insert(int value) {
        Node* newNode = new Node(value);
        
        if (head == nullptr) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        
        size++;
    }
    
    void display() const {
        Node* current = head;
        
        cout << "List: ";
        while (current != nullptr) {
            cout << current->data << " -> ";
            current = current->next;
        }
        cout << "NULL" << endl;
    }
    
    bool search(int value) const {
        Node* current = head;
        
        while (current != nullptr) {
            if (current->data == value) {
                return true;
            }
            current = current->next;
        }
        
        return false;
    }
    
    void remove(int value) {
        if (head == nullptr) return;
        
        if (head->data == value) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size--;
            return;
        }
        
        Node* current = head;
        while (current->next != nullptr) {
            if (current->next->data == value) {
                Node* temp = current->next;
                current->next = current->next->next;
                delete temp;
                size--;
                return;
            }
            current = current->next;
        }
    }
    
    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        size = 0;
    }
    
    int getSize() const { return size; }
};

int main() {
    LinkedList list;
    
    list.insert(10);
    list.insert(20);
    list.insert(30);
    list.insert(40);
    
    list.display();
    
    cout << "Search 30: " << (list.search(30) ? "Found" : "Not found") << endl;
    
    list.remove(20);
    list.display();
    
    return 0;
}
```

### Example 3: Student Database with Dynamic Memory

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
public:
    int id;
    string name;
    double gpa;
    
    Student(int i = 0, string n = "", double g = 0.0) 
        : id(i), name(n), gpa(g) { }
    
    void display() const {
        cout << "ID: " << id << ", Name: " << name << ", GPA: " << gpa << endl;
    }
};

class StudentDatabase {
private:
    Student** students;
    int count;
    int capacity;

public:
    StudentDatabase(int cap = 10) : count(0), capacity(cap) {
        students = new Student*[capacity];
    }
    
    ~StudentDatabase() {
        for (int i = 0; i < count; i++) {
            delete students[i];
        }
        delete[] students;
    }
    
    void addStudent(int id, string name, double gpa) {
        if (count >= capacity) {
            resize();
        }
        
        students[count++] = new Student(id, name, gpa);
        cout << "Student added successfully!" << endl;
    }
    
    void resize() {
        capacity *= 2;
        Student** newStudents = new Student*[capacity];
        
        for (int i = 0; i < count; i++) {
            newStudents[i] = students[i];
        }
        
        delete[] students;
        students = newStudents;
    }
    
    void displayAll() const {
        if (count == 0) {
            cout << "No students in database." << endl;
            return;
        }
        
        cout << "\n=== Student Database ===" << endl;
        for (int i = 0; i < count; i++) {
            students[i]->display();
        }
    }
    
    Student* search(int id) const {
        for (int i = 0; i < count; i++) {
            if (students[i]->id == id) {
                return students[i];
            }
        }
        return nullptr;
    }
    
    void removeStudent(int id) {
        for (int i = 0; i < count; i++) {
            if (students[i]->id == id) {
                delete students[i];
                
                for (int j = i; j < count - 1; j++) {
                    students[j] = students[j + 1];
                }
                
                count--;
                cout << "Student removed!" << endl;
                return;
            }
        }
        
        cout << "Student not found!" << endl;
    }
};

int main() {
    StudentDatabase db;
    int choice, id;
    string name;
    double gpa;
    
    do {
        cout << "\n=== Student Management System ===" << endl;
        cout << "1. Add Student" << endl;
        cout << "2. Display All Students" << endl;
        cout << "3. Search Student" << endl;
        cout << "4. Remove Student" << endl;
        cout << "5. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Name: ";
                getline(cin, name);
                cout << "Enter GPA: ";
                cin >> gpa;
                db.addStudent(id, name, gpa);
                break;
            
            case 2:
                db.displayAll();
                break;
            
            case 3:
                cout << "Enter ID to search: ";
                cin >> id;
                {
                    Student* s = db.search(id);
                    if (s != nullptr) {
                        cout << "Student found:" << endl;
                        s->display();
                    } else {
                        cout << "Student not found!" << endl;
                    }
                }
                break;
            
            case 4:
                cout << "Enter ID to remove: ";
                cin >> id;
                db.removeStudent(id);
                break;
            
            case 5:
                cout << "Goodbye!" << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 5);
    
    return 0;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Forgetting to Delete

❌ **Wrong**:

```cpp
void function() {
    int* ptr = new int(42);
    // Forgot delete!
}  // Memory leak!
```

✅ **Correct**:

```cpp
void function() {
    int* ptr = new int(42);
    // Use ptr
    delete ptr;  // Always delete!
}
```

### Mistake 2: Using delete Instead of delete[]

❌ **Wrong**:

```cpp
int* arr = new int[10];
delete arr;  // Wrong! Should be delete[]
```

✅ **Correct**:

```cpp
int* arr = new int[10];
delete[] arr;  // Correct!
```

### Mistake 3: Using After Delete

❌ **Wrong**:

```cpp
int* ptr = new int(42);
delete ptr;
cout << *ptr << endl;  // Dangling pointer!
```

✅ **Correct**:

```cpp
int* ptr = new int(42);
delete ptr;
ptr = nullptr;  // Set to nullptr

if (ptr != nullptr) {
    cout << *ptr << endl;
}
```

### Mistake 4: Double Delete

❌ **Wrong**:

```cpp
int* ptr = new int(42);
delete ptr;
delete ptr;  // Double delete! Undefined behavior
```

✅ **Correct**:

```cpp
int* ptr = new int(42);
delete ptr;
ptr = nullptr;
delete ptr;  // Safe - deleting nullptr is OK
```

### Mistake 5: Memory Leak in Loop

❌ **Wrong**:

```cpp
for (int i = 0; i < 1000; i++) {
    int* ptr = new int(i);
    // No delete - leaks 1000 integers!
}
```

✅ **Correct**:

```cpp
for (int i = 0; i < 1000; i++) {
    int* ptr = new int(i);
    // Use ptr
    delete ptr;  // Delete each iteration
}
```

### Mistake 6: Shallow Copy

❌ **Wrong**:

```cpp
class Array {
    int* data;
public:
    Array(int size) { data = new int[size]; }
    ~Array() { delete[] data; }
    // Default copy constructor does shallow copy!
};

Array a1(10);
Array a2 = a1;  // Both point to same data!
// Double delete when destructors run!
```

✅ **Correct**:

```cpp
class Array {
    int* data;
    int size;
public:
    Array(int s) : size(s) { data = new int[size]; }
    ~Array() { delete[] data; }
    
    // Deep copy constructor
    Array(const Array& other) : size(other.size) {
        data = new int[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
};
```

### Mistake 7: Not Checking new

❌ **Wrong**:

```cpp
int* huge = new int[1000000000];  // Might fail!
*huge = 42;  // If allocation failed, crash!
```

✅ **Correct**:

```cpp
int* huge = new(nothrow) int[1000000000];
if (huge == nullptr) {
    cout << "Allocation failed!" << endl;
    return 1;
}
*huge = 42;
delete[] huge;
```

---

## Best Practices

### ✅ DO:

1. **Always delete what you new**
    
    ```cpp
    int* ptr = new int;
    delete ptr;
    ```
    
2. **Use delete[] for arrays**
    
    ```cpp
    int* arr = new int[10];
    delete[] arr;
    ```
    
3. **Set pointers to nullptr after delete**
    
    ```cpp
    delete ptr;
    ptr = nullptr;
    ```
    
4. **Use smart pointers when possible**
    
    ```cpp
    auto ptr = make_unique<int>(42);
    ```
    
5. **Implement Rule of Three for resource-managing classes**
    
    ```cpp
    // Destructor, Copy Constructor, Copy Assignment
    ```
    
6. **Check allocation success for large allocations**
    
    ```cpp
    int* ptr = new(nothrow) int[huge_size];
    if (ptr == nullptr) { /* handle */ }
    ```
    

### ❌ DON'T:

1. **Don't forget to delete**
    
    ```cpp
    // int* ptr = new int;  // Leak!
    ```
    
2. **Don't use after delete**
    
    ```cpp
    // delete ptr;
    // *ptr = 42;  // Dangling pointer!
    ```
    
3. **Don't delete stack variables**
    
    ```cpp
    // int x = 10;
    // delete &x;  // Wrong! x is on stack!
    ```
    
4. **Don't mix new/delete with malloc/free**
    
    ```cpp
    // int* ptr = (int*)malloc(sizeof(int));
    // delete ptr;  // Wrong! Use free()
    ```
    
5. **Don't return pointers to local variables**
    
    ```cpp
    // int* func() {
    //     int x = 10;
    //     return &x;  // Dangling pointer!
    // }
    ```
    

---

## Quick Reference

### Allocation

```cpp
// Single variable
int* ptr = new int;
int* ptr = new int(42);

// Array
int* arr = new int[size];

// Object
Student* s = new Student("Alice", 20);

// Smart pointers
auto ptr = make_unique<int>(42);
auto sptr = make_shared<int>(42);
```

### Deallocation

```cpp
// Single variable
delete ptr;

// Array
delete[] arr;

// Smart pointers
// Automatic - no delete needed!
```

### Safety

```cpp
// After delete
delete ptr;
ptr = nullptr;

// Check before use
if (ptr != nullptr) {
    *ptr = 42;
}
```

### Rule of Three

```cpp
class MyClass {
public:
    ~MyClass();                          // Destructor
    MyClass(const MyClass&);             // Copy constructor
    MyClass& operator=(const MyClass&);  // Copy assignment
};
```

---

## Summary

### Key Concepts

1. **new** - Allocate memory dynamically
2. **delete** - Free allocated memory
3. **delete[]** - Free array memory
4. **Memory leaks** - Forgetting to free memory
5. **Dangling pointers** - Using freed memory
6. **Rule of Three** - Deep copy for resource management
7. **Smart pointers** - Automatic memory management

### Memory Management Checklist

✅ Every `new` has a matching `delete`  
✅ Use `delete[]` for arrays  
✅ Set pointers to `nullptr` after delete  
✅ Check for self-assignment in operator=  
✅ Implement deep copy when managing resources  
✅ Use smart pointers when possible  
✅ Prefer stack allocation when appropriate

---

## Conclusion

Dynamic memory management gives you powerful control over your program's memory, enabling:

- **Flexible data structures** - Linked lists, trees, graphs
- **Runtime-sized arrays** - Size determined when needed
- **Efficient memory use** - Allocate only what you need
- **Long-lived objects** - Objects that outlive their scope
- **Complex applications** - Professional software development

**Master dynamic memory**, and you unlock the full potential of C++ for building sophisticated, efficient applications!

**Remember**: With great power comes great responsibility. Always free what you allocate!

**Happy Coding!** 🚀