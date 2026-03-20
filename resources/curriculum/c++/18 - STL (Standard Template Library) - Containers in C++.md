## Introduction

You've learned to manage memory manually, create arrays, and build data structures from scratch. Now it's time to discover the **Standard Template Library (STL)** - a powerful collection of pre-built, tested, and optimized containers and algorithms that will transform how you write C++ code.

**Why use STL instead of building everything yourself?**

- **Already implemented** - Don't reinvent the wheel
- **Thoroughly tested** - Used by millions of developers
- **Optimized** - Efficient implementations
- **Generic** - Works with any data type
- **Industry standard** - Expected in professional code
- **Time-saving** - Write less code, accomplish more

**Real-World Analogy**: Building your own containers is like crafting your own tools. Using STL is like using professional power tools - faster, more reliable, and standardized.

---

## What is the STL?

The **Standard Template Library (STL)** is a powerful collection of C++ template classes and functions that provide:

1. **Containers** - Data structures (vector, list, map, set, etc.)
2. **Iterators** - Objects to traverse containers
3. **Algorithms** - Operations on containers (sort, search, etc.)
4. **Function Objects** - Objects that act like functions

### Benefits of STL

✅ **Type-safe** - Compile-time type checking  
✅ **Reusable** - Generic templates work with any type  
✅ **Efficient** - Optimized implementations  
✅ **Maintainable** - Standard, well-documented code  
✅ **Productive** - Write less, accomplish more  
✅ **Professional** - Industry standard

---

## STL Containers Overview

### Container Categories

**Sequence Containers** (ordered by position):

- `vector` - Dynamic array
- `deque` - Double-ended queue
- `list` - Doubly linked list
- `array` - Fixed-size array (C++11)
- `forward_list` - Singly linked list (C++11)

**Associative Containers** (ordered by key):

- `set` - Unique sorted elements
- `multiset` - Sorted elements (duplicates allowed)
- `map` - Key-value pairs (unique keys)
- `multimap` - Key-value pairs (duplicate keys)

**Unordered Associative Containers** (C++11):

- `unordered_set` - Unique elements (hash table)
- `unordered_multiset` - Elements with duplicates (hash table)
- `unordered_map` - Key-value pairs (hash table)
- `unordered_multimap` - Key-value pairs with duplicates (hash table)

**Container Adapters**:

- `stack` - LIFO (Last In, First Out)
- `queue` - FIFO (First In, First Out)
- `priority_queue` - Priority-based ordering

---

# 1. vector - Dynamic Array

## What is vector?

`vector` is a **dynamic array** that can grow or shrink in size automatically.

**Think of it as**: A smart array that manages its own memory and can resize itself.

### Including vector

```cpp
#include <vector>
using namespace std;
```

### Basic Declaration and Initialization

```cpp
// Empty vector
vector<int> numbers;

// Vector with initial size
vector<int> numbers(10);  // 10 elements, default initialized to 0

// Vector with size and initial value
vector<int> numbers(10, 5);  // 10 elements, all set to 5

// Initialize with values
vector<int> numbers = {1, 2, 3, 4, 5};

// Copy from another vector
vector<int> copy = numbers;
```

### Common Operations

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums;
    
    // Add elements
    nums.push_back(10);
    nums.push_back(20);
    nums.push_back(30);
    
    // Access elements
    cout << "First: " << nums[0] << endl;        // 10
    cout << "Second: " << nums.at(1) << endl;    // 20 (with bounds checking)
    cout << "Front: " << nums.front() << endl;   // 10
    cout << "Back: " << nums.back() << endl;     // 30
    
    // Size and capacity
    cout << "Size: " << nums.size() << endl;         // 3
    cout << "Capacity: " << nums.capacity() << endl; // >= 3
    cout << "Empty? " << nums.empty() << endl;       // 0 (false)
    
    // Modify
    nums[0] = 100;  // Change first element
    
    // Remove last element
    nums.pop_back();
    
    // Clear all elements
    nums.clear();
    
    return 0;
}
```

### Iterating Through vector

**Method 1: Index-based**

```cpp
vector<int> nums = {10, 20, 30, 40, 50};

for (int i = 0; i < nums.size(); i++) {
    cout << nums[i] << " ";
}
```

**Method 2: Range-based for loop (C++11)**

```cpp
for (int num : nums) {
    cout << num << " ";
}
```

**Method 3: Iterators**

```cpp
for (vector<int>::iterator it = nums.begin(); it != nums.end(); ++it) {
    cout << *it << " ";
}

// Or with auto (C++11)
for (auto it = nums.begin(); it != nums.end(); ++it) {
    cout << *it << " ";
}
```

### Important vector Methods

```cpp
vector<int> v = {1, 2, 3, 4, 5};

// Adding elements
v.push_back(6);              // Add to end
v.insert(v.begin(), 0);      // Insert at beginning
v.insert(v.begin() + 2, 99); // Insert at position 2

// Removing elements
v.pop_back();                // Remove last
v.erase(v.begin());          // Remove first
v.erase(v.begin() + 2);      // Remove at position 2
v.clear();                   // Remove all

// Resizing
v.resize(10);                // Resize to 10 elements
v.resize(20, 5);             // Resize to 20, new elements = 5

// Other operations
v.reserve(100);              // Reserve capacity (no resize)
v.shrink_to_fit();           // Reduce capacity to size
```

### Complete Example: Student Grades

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Student {
    string name;
    double grade;
};

int main() {
    vector<Student> students;
    
    // Add students
    students.push_back({"Alice", 85.5});
    students.push_back({"Bob", 92.0});
    students.push_back({"Charlie", 78.5});
    students.push_back({"Diana", 95.0});
    
    // Display all students
    cout << "=== All Students ===" << endl;
    for (const auto& student : students) {
        cout << student.name << ": " << student.grade << endl;
    }
    
    // Calculate average
    double sum = 0;
    for (const auto& student : students) {
        sum += student.grade;
    }
    double average = sum / students.size();
    cout << "\nClass Average: " << average << endl;
    
    // Find highest grade
    auto maxStudent = students[0];
    for (const auto& student : students) {
        if (student.grade > maxStudent.grade) {
            maxStudent = student;
        }
    }
    cout << "Highest Grade: " << maxStudent.name 
         << " with " << maxStudent.grade << endl;
    
    return 0;
}
```

---

# 2. list - Doubly Linked List

## What is list?

`list` is a **doubly linked list** where each element points to the next and previous elements.

**Best for**: Frequent insertions/deletions anywhere in the sequence.

### Basic Usage

```cpp
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> numbers;
    
    // Add elements
    numbers.push_back(10);    // Add to end
    numbers.push_front(5);    // Add to beginning
    numbers.push_back(20);
    
    // Display: 5 10 20
    for (int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Remove elements
    numbers.pop_front();      // Remove first
    numbers.pop_back();       // Remove last
    
    return 0;
}
```

### Common list Operations

```cpp
list<int> lst = {1, 2, 3, 4, 5};

// Adding
lst.push_front(0);           // Add at front
lst.push_back(6);            // Add at back

// Accessing (no [] operator!)
cout << lst.front() << endl; // First element
cout << lst.back() << endl;  // Last element

// Removing
lst.pop_front();             // Remove first
lst.pop_back();              // Remove last
lst.remove(3);               // Remove all elements with value 3

// Size
cout << lst.size() << endl;
cout << lst.empty() << endl;

// Sorting and reversing
lst.sort();                  // Sort in ascending order
lst.reverse();               // Reverse the list

// Removing duplicates
lst.unique();                // Remove consecutive duplicates
```

### list vs vector

|Feature|vector|list|
|---|---|---|
|Random Access|O(1) - Fast|O(n) - Slow|
|Insert/Delete at ends|O(1)|O(1)|
|Insert/Delete in middle|O(n)|O(1)|
|Memory|Contiguous|Non-contiguous|
|Use when|Random access needed|Frequent insert/delete|

---

# 3. deque - Double-Ended Queue

## What is deque?

`deque` (pronounced "deck") is like vector but allows fast insertion/deletion at **both ends**.

### Basic Usage

```cpp
#include <iostream>
#include <deque>
using namespace std;

int main() {
    deque<int> dq;
    
    // Add elements
    dq.push_back(10);     // Add to back
    dq.push_front(5);     // Add to front
    dq.push_back(20);
    dq.push_front(1);
    
    // Display: 1 5 10 20
    for (int num : dq) {
        cout << num << " ";
    }
    cout << endl;
    
    // Access
    cout << "Front: " << dq.front() << endl;  // 1
    cout << "Back: " << dq.back() << endl;    // 20
    cout << "At index 2: " << dq[2] << endl;  // 10
    
    // Remove
    dq.pop_front();       // Remove from front
    dq.pop_back();        // Remove from back
    
    return 0;
}
```

---

# 4. set - Sorted Unique Elements

## What is set?

`set` stores **unique** elements in **sorted** order (automatically).

**Best for**: Maintaining sorted unique collection, fast lookups.

### Basic Usage

```cpp
#include <iostream>
#include <set>
using namespace std;

int main() {
    set<int> numbers;
    
    // Insert elements
    numbers.insert(30);
    numbers.insert(10);
    numbers.insert(20);
    numbers.insert(10);  // Duplicate - won't be added
    
    // Display: 10 20 30 (sorted automatically)
    for (int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Search
    if (numbers.find(20) != numbers.end()) {
        cout << "20 is in the set" << endl;
    }
    
    // Remove
    numbers.erase(20);
    
    // Size
    cout << "Size: " << numbers.size() << endl;
    
    return 0;
}
```

### Common set Operations

```cpp
set<int> s = {3, 1, 4, 1, 5, 9};  // Duplicates automatically removed

// Insert
s.insert(2);
s.insert(6);

// Find
auto it = s.find(4);
if (it != s.end()) {
    cout << "Found: " << *it << endl;
}

// Count (returns 0 or 1 for set)
if (s.count(5) > 0) {
    cout << "5 exists" << endl;
}

// Erase
s.erase(3);                // Erase by value
s.erase(s.begin());        // Erase by iterator

// Clear
s.clear();
```

### Example: Remove Duplicates

```cpp
#include <iostream>
#include <vector>
#include <set>
using namespace std;

int main() {
    vector<int> numbers = {1, 2, 2, 3, 4, 4, 4, 5, 6, 6};
    
    cout << "Original: ";
    for (int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
    
    // Convert to set (removes duplicates)
    set<int> uniqueNumbers(numbers.begin(), numbers.end());
    
    cout << "Unique: ";
    for (int num : uniqueNumbers) {
        cout << num << " ";
    }
    cout << endl;
    
    return 0;
}
```

---

# 5. map - Key-Value Pairs

## What is map?

`map` stores **key-value pairs** where keys are **unique** and **sorted**.

**Think of it as**: A dictionary where you look up values by keys.

### Basic Usage

```cpp
#include <iostream>
#include <map>
#include <string>
using namespace std;

int main() {
    map<string, int> ages;
    
    // Insert key-value pairs
    ages["Alice"] = 25;
    ages["Bob"] = 30;
    ages["Charlie"] = 22;
    
    // Access
    cout << "Alice's age: " << ages["Alice"] << endl;
    
    // Check if key exists
    if (ages.find("Bob") != ages.end()) {
        cout << "Bob's age: " << ages["Bob"] << endl;
    }
    
    // Iterate
    for (const auto& pair : ages) {
        cout << pair.first << " is " << pair.second << " years old" << endl;
    }
    
    // Remove
    ages.erase("Charlie");
    
    return 0;
}
```

### Common map Operations

```cpp
map<string, int> m;

// Insert
m["apple"] = 5;
m["banana"] = 3;
m.insert({"orange", 7});
m.insert(make_pair("grape", 10));

// Access
cout << m["apple"] << endl;      // 5
cout << m.at("banana") << endl;  // 3 (with bounds checking)

// Check existence
if (m.find("apple") != m.end()) {
    cout << "Apple found" << endl;
}

if (m.count("banana") > 0) {
    cout << "Banana exists" << endl;
}

// Iterate
for (const auto& [key, value] : m) {  // C++17
    cout << key << ": " << value << endl;
}

// Erase
m.erase("orange");
```

### Example: Word Frequency Counter

```cpp
#include <iostream>
#include <map>
#include <string>
#include <sstream>
using namespace std;

int main() {
    string text = "hello world hello programming world hello";
    
    map<string, int> wordCount;
    
    // Count words
    stringstream ss(text);
    string word;
    
    while (ss >> word) {
        wordCount[word]++;
    }
    
    // Display results
    cout << "=== Word Frequencies ===" << endl;
    for (const auto& [word, count] : wordCount) {
        cout << word << ": " << count << endl;
    }
    
    return 0;
}
```

**Output**:

```
=== Word Frequencies ===
hello: 3
programming: 1
world: 2
```

---

# 6. unordered_map - Hash Table

## What is unordered_map?

`unordered_map` is like `map` but uses a **hash table** for faster access (not sorted).

**Best for**: Fast lookups when order doesn't matter.

### Basic Usage

```cpp
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

int main() {
    unordered_map<string, int> phoneBook;
    
    // Insert
    phoneBook["Alice"] = 1234567890;
    phoneBook["Bob"] = 9876543210;
    phoneBook["Charlie"] = 5555555555;
    
    // Access - O(1) average
    cout << "Alice's number: " << phoneBook["Alice"] << endl;
    
    // Search - O(1) average
    if (phoneBook.find("Bob") != phoneBook.end()) {
        cout << "Bob found!" << endl;
    }
    
    return 0;
}
```

### map vs unordered_map

|Feature|map|unordered_map|
|---|---|---|
|Ordering|Sorted by key|No ordering|
|Search|O(log n)|O(1) average|
|Insert|O(log n)|O(1) average|
|Memory|Less|More (hash table)|
|Use when|Need sorted keys|Need fast access|

---

# 7. stack - LIFO Container

## What is stack?

`stack` is a **Last In, First Out (LIFO)** container adapter.

**Think of it as**: A stack of plates - add and remove from top only.

### Basic Usage

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> s;
    
    // Push elements
    s.push(10);
    s.push(20);
    s.push(30);
    
    // Top element
    cout << "Top: " << s.top() << endl;  // 30
    
    // Pop elements (LIFO)
    while (!s.empty()) {
        cout << s.top() << " ";
        s.pop();
    }
    // Output: 30 20 10
    
    return 0;
}
```

### Example: Balanced Parentheses

```cpp
#include <iostream>
#include <stack>
#include <string>
using namespace std;

bool isBalanced(string expr) {
    stack<char> s;
    
    for (char ch : expr) {
        if (ch == '(' || ch == '{' || ch == '[') {
            s.push(ch);
        }
        else if (ch == ')' || ch == '}' || ch == ']') {
            if (s.empty()) return false;
            
            char top = s.top();
            s.pop();
            
            if ((ch == ')' && top != '(') ||
                (ch == '}' && top != '{') ||
                (ch == ']' && top != '[')) {
                return false;
            }
        }
    }
    
    return s.empty();
}

int main() {
    cout << isBalanced("(){}[]") << endl;      // 1 (true)
    cout << isBalanced("({[]})") << endl;      // 1 (true)
    cout << isBalanced("({[}])") << endl;      // 0 (false)
    cout << isBalanced("((()") << endl;        // 0 (false)
    
    return 0;
}
```

---

# 8. queue - FIFO Container

## What is queue?

`queue` is a **First In, First Out (FIFO)** container adapter.

**Think of it as**: A line at a store - first person in is first served.

### Basic Usage

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    
    // Add elements
    q.push(10);
    q.push(20);
    q.push(30);
    
    // Front and back
    cout << "Front: " << q.front() << endl;  // 10
    cout << "Back: " << q.back() << endl;    // 30
    
    // Remove elements (FIFO)
    while (!q.empty()) {
        cout << q.front() << " ";
        q.pop();
    }
    // Output: 10 20 30
    
    return 0;
}
```

---

# 9. priority_queue - Priority-Based Queue

## What is priority_queue?

`priority_queue` is a queue where elements are ordered by **priority** (largest element first by default).

### Basic Usage

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    priority_queue<int> pq;
    
    // Insert elements
    pq.push(30);
    pq.push(10);
    pq.push(50);
    pq.push(20);
    
    // Elements come out in descending order
    while (!pq.empty()) {
        cout << pq.top() << " ";
        pq.pop();
    }
    // Output: 50 30 20 10
    
    return 0;
}
```

### Min-Heap (Smallest First)

```cpp
// Min-heap priority queue
priority_queue<int, vector<int>, greater<int>> minPQ;

minPQ.push(30);
minPQ.push(10);
minPQ.push(50);
minPQ.push(20);

while (!minPQ.empty()) {
    cout << minPQ.top() << " ";
    minPQ.pop();
}
// Output: 10 20 30 50
```

---

## Practical Examples and Projects

### Project 1: To-Do List Manager

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Task {
    int id;
    string description;
    bool completed;
    
    Task(int i, string desc) : id(i), description(desc), completed(false) {}
};

class ToDoList {
private:
    vector<Task> tasks;
    int nextId;

public:
    ToDoList() : nextId(1) {}
    
    void addTask(string description) {
        tasks.push_back(Task(nextId++, description));
        cout << "Task added successfully!" << endl;
    }
    
    void displayTasks() {
        if (tasks.empty()) {
            cout << "No tasks!" << endl;
            return;
        }
        
        cout << "\n=== To-Do List ===" << endl;
        for (const auto& task : tasks) {
            cout << task.id << ". ";
            cout << "[" << (task.completed ? "X" : " ") << "] ";
            cout << task.description << endl;
        }
    }
    
    void completeTask(int id) {
        for (auto& task : tasks) {
            if (task.id == id) {
                task.completed = true;
                cout << "Task marked as completed!" << endl;
                return;
            }
        }
        cout << "Task not found!" << endl;
    }
    
    void removeTask(int id) {
        auto it = remove_if(tasks.begin(), tasks.end(),
                           [id](const Task& t) { return t.id == id; });
        
        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            cout << "Task removed!" << endl;
        } else {
            cout << "Task not found!" << endl;
        }
    }
};

int main() {
    ToDoList todoList;
    int choice, id;
    string description;
    
    do {
        cout << "\n=== To-Do List Manager ===" << endl;
        cout << "1. Add Task" << endl;
        cout << "2. View Tasks" << endl;
        cout << "3. Complete Task" << endl;
        cout << "4. Remove Task" << endl;
        cout << "5. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1:
                cout << "Enter task: ";
                getline(cin, description);
                todoList.addTask(description);
                break;
            
            case 2:
                todoList.displayTasks();
                break;
            
            case 3:
                cout << "Enter task ID to complete: ";
                cin >> id;
                todoList.completeTask(id);
                break;
            
            case 4:
                cout << "Enter task ID to remove: ";
                cin >> id;
                todoList.removeTask(id);
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

### Project 2: Contact Directory

```cpp
#include <iostream>
#include <map>
#include <string>
using namespace std;

struct Contact {
    string name;
    string phone;
    string email;
};

class ContactDirectory {
private:
    map<string, Contact> contacts;  // Key: name

public:
    void addContact() {
        Contact c;
        
        cout << "Enter name: ";
        getline(cin, c.name);
        
        if (contacts.find(c.name) != contacts.end()) {
            cout << "Contact already exists!" << endl;
            return;
        }
        
        cout << "Enter phone: ";
        getline(cin, c.phone);
        
        cout << "Enter email: ";
        getline(cin, c.email);
        
        contacts[c.name] = c;
        cout << "Contact added!" << endl;
    }
    
    void searchContact() {
        string name;
        cout << "Enter name to search: ";
        getline(cin, name);
        
        auto it = contacts.find(name);
        if (it != contacts.end()) {
            cout << "\n=== Contact Found ===" << endl;
            cout << "Name: " << it->second.name << endl;
            cout << "Phone: " << it->second.phone << endl;
            cout << "Email: " << it->second.email << endl;
        } else {
            cout << "Contact not found!" << endl;
        }
    }
    
    void displayAll() {
        if (contacts.empty()) {
            cout << "No contacts!" << endl;
            return;
        }
        
        cout << "\n=== All Contacts ===" << endl;
        for (const auto& [name, contact] : contacts) {
            cout << "\nName: " << contact.name << endl;
            cout << "Phone: " << contact.phone << endl;
            cout << "Email: " << contact.email << endl;
            cout << "---" << endl;
        }
    }
    
    void removeContact() {
        string name;
        cout << "Enter name to remove: ";
        getline(cin, name);
        
        if (contacts.erase(name)) {
            cout << "Contact removed!" << endl;
        } else {
            cout << "Contact not found!" << endl;
        }
    }
};

int main() {
    ContactDirectory directory;
    int choice;
    
    do {
        cout << "\n=== Contact Directory ===" << endl;
        cout << "1. Add Contact" << endl;
        cout << "2. Search Contact" << endl;
        cout << "3. Display All Contacts" << endl;
        cout << "4. Remove Contact" << endl;
        cout << "5. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1:
                directory.addContact();
                break;
            
            case 2:
                directory.searchContact();
                break;
            
            case 3:
                directory.displayAll();
                break;
            
            case 4:
                directory.removeContact();
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

### Project 3: Text Analyzer

```cpp
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;

class TextAnalyzer {
private:
    string text;
    map<string, int> wordFrequency;
    set<string> uniqueWords;

public:
    void setText(string t) {
        text = t;
        analyze();
    }
    
    void analyze() {
        wordFrequency.clear();
        uniqueWords.clear();
        
        stringstream ss(text);
        string word;
        
        while (ss >> word) {
            // Convert to lowercase
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            
            // Remove punctuation
            word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
            
            if (!word.empty()) {
                wordFrequency[word]++;
                uniqueWords.insert(word);
            }
        }
    }
    
    void displayStatistics() {
        cout << "\n=== Text Statistics ===" << endl;
        cout << "Total words: " << getTotalWords() << endl;
        cout << "Unique words: " << uniqueWords.size() << endl;
    }
    
    void displayWordFrequencies() {
        cout << "\n=== Word Frequencies ===" << endl;
        for (const auto& [word, count] : wordFrequency) {
            cout << word << ": " << count << endl;
        }
    }
    
    void displayMostCommon(int n) {
        // Convert map to vector for sorting by value
        vector<pair<string, int>> words(wordFrequency.begin(), wordFrequency.end());
        
        // Sort by frequency (descending)
        sort(words.begin(), words.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
        
        cout << "\n=== Top " << n << " Most Common Words ===" << endl;
        for (int i = 0; i < min(n, (int)words.size()); i++) {
            cout << i + 1 << ". " << words[i].first 
                 << " (" << words[i].second << " times)" << endl;
        }
    }
    
    int getTotalWords() {
        int total = 0;
        for (const auto& [word, count] : wordFrequency) {
            total += count;
        }
        return total;
    }
};

int main() {
    TextAnalyzer analyzer;
    string text;
    
    cout << "Enter text (end with empty line):" << endl;
    string line;
    while (getline(cin, line) && !line.empty()) {
        text += line + " ";
    }
    
    analyzer.setText(text);
    analyzer.displayStatistics();
    analyzer.displayWordFrequencies();
    analyzer.displayMostCommon(5);
    
    return 0;
}
```

---

## Container Comparison Table

|Container|Access|Insert/Delete|Sorted|Unique|Use Case|
|---|---|---|---|---|---|
|**vector**|O(1) random|O(n) middle|No|No|General purpose, dynamic array|
|**list**|O(n)|O(1) anywhere|No|No|Frequent insert/delete|
|**deque**|O(1) random|O(1) ends|No|No|Queue/deque operations|
|**set**|O(log n)|O(log n)|Yes|Yes|Sorted unique elements|
|**map**|O(log n)|O(log n)|Yes (key)|Yes (key)|Key-value pairs|
|**unordered_set**|O(1) avg|O(1) avg|No|Yes|Fast unique lookup|
|**unordered_map**|O(1) avg|O(1) avg|No|Yes (key)|Fast key-value lookup|
|**stack**|O(1) top|O(1) top|No|No|LIFO operations|
|**queue**|O(1) front|O(1) ends|No|No|FIFO operations|
|**priority_queue**|O(1) top|O(log n)|Yes (priority)|No|Priority-based access|

---

## When to Use Which Container?

### Choose **vector** when:

- ✅ Random access needed
- ✅ Mostly adding/removing at end
- ✅ Memory efficiency important
- ✅ Default choice for most cases

### Choose **list** when:

- ✅ Frequent insert/delete in middle
- ✅ No random access needed
- ✅ Elements move around often

### Choose **deque** when:

- ✅ Insert/delete at both ends
- ✅ Random access needed
- ✅ Queue-like operations

### Choose **set** when:

- ✅ Need unique elements
- ✅ Need sorted order
- ✅ Fast lookup/insert/delete

### Choose **map** when:

- ✅ Key-value associations
- ✅ Need sorted keys
- ✅ Dictionary-like structure

### Choose **unordered_map** when:

- ✅ Key-value associations
- ✅ Fastest possible lookup
- ✅ Order doesn't matter

### Choose **stack** when:

- ✅ LIFO behavior needed
- ✅ Undo operations
- ✅ Expression evaluation

### Choose **queue** when:

- ✅ FIFO behavior needed
- ✅ Task scheduling
- ✅ BFS algorithms

### Choose **priority_queue** when:

- ✅ Always need highest/lowest element
- ✅ Task scheduling by priority
- ✅ Dijkstra's algorithm

---

## Best Practices

### ✅ DO:

1. **Use the right container**
    
    ```cpp
    vector<int> v;  // For most cases
    map<string, int> m;  // For key-value
    ```
    
2. **Reserve capacity for vectors**
    
    ```cpp
    vector<int> v;
    v.reserve(1000);  // If you know size
    ```
    
3. **Use const reference for iteration**
    
    ```cpp
    for (const auto& item : container) {
        // Read-only
    }
    ```
    
4. **Use auto for iterators**
    
    ```cpp
    auto it = myMap.find(key);
    ```
    
5. **Check before accessing**
    
    ```cpp
    if (myMap.find(key) != myMap.end()) {
        // Safe to access
    }
    ```
    

### ❌ DON'T:

1. **Don't use [] for maps if key might not exist**
    
    ```cpp
    // map["key"];  // Creates entry if missing!
    map.at("key");  // Throws if missing (safer)
    ```
    
2. **Don't modify container while iterating**
    
    ```cpp
    // for (auto it = v.begin(); it != v.end(); ++it) {
    //     v.push_back(x);  // Dangerous!
    // }
    ```
    
3. **Don't forget to check empty()**
    
    ```cpp
    if (!stack.empty()) {
        stack.top();  // Safe
    }
    ```
    

---

## Summary

### Key Takeaways

1. **STL** - Standard, tested, optimized containers
2. **vector** - Dynamic array (most common)
3. **list** - Linked list (frequent insert/delete)
4. **map** - Key-value pairs (sorted)
5. **set** - Unique sorted elements
6. **stack/queue** - Special access patterns
7. **Iterators** - Generic way to traverse containers

### What You Can Do Now

✅ Use professional-grade containers  
✅ Choose optimal container for each problem  
✅ Write cleaner, more maintainable code  
✅ Solve problems faster with STL  
✅ Build complex applications efficiently  
✅ Focus on logic, not implementation details

---

## Conclusion

The **STL containers** are essential tools for professional C++ programming. They provide:

- **Efficiency** - Optimized implementations
- **Safety** - Well-tested code
- **Productivity** - Write less, accomplish more
- **Standardization** - Industry-standard code
- **Flexibility** - Right tool for each job

Master these containers, and you'll write better C++ code faster!

**Happy Coding!** 🚀