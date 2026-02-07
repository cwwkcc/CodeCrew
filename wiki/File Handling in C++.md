## Introduction

Imagine creating a game where players can save their progress, or a student database that remembers information even after the program closes. Without file handling, all your data disappears when the program ends!

**File Handling** allows your programs to:

- **Save data permanently** - Persist beyond program execution
- **Read existing data** - Load previously saved information
- **Update records** - Modify existing files
- **Exchange data** - Share information between programs

**Real-World Analogies**:

- **Notebook**: Reading = Opening and reading pages, Writing = Adding new entries
- **Library**: Files are books you can borrow (read), return (close), or write in (update)
- **Filing Cabinet**: Organize documents, retrieve them, update them, store them back
- **Recipe Book**: Read recipes (input), add new recipes (output), modify existing ones (update)

---

## Why File Handling Matters

### Without File Handling - Data Lost

```cpp
int main() {
    int score = 1000;
    cout << "Score: " << score << endl;
    return 0;
}
// Program ends → Score is gone forever!
```

### With File Handling - Data Persists

```cpp
int main() {
    int score = 1000;
    
    // Save to file
    ofstream file("score.txt");
    file << score;
    file.close();
    
    // Even after program ends, score.txt still has 1000!
    return 0;
}
```

### Benefits of File Handling

1. **Permanent storage** - Data survives program termination
2. **Data sharing** - Exchange information between programs
3. **Backup and recovery** - Preserve important information
4. **Large data processing** - Handle data too big for memory
5. **Configuration storage** - Save user preferences
6. **Logging** - Record program events and errors
7. **Database operations** - Simple data persistence

---

## File Handling Basics

### Required Header

```cpp
#include <fstream>  // File stream
```

### Three Main Classes

C++ provides three classes for file operations:

1. **`ofstream`** - **O**utput **F**ile **Stream** (Writing to files)
2. **`ifstream`** - **I**nput **F**ile **Stream** (Reading from files)
3. **`fstream`** - **F**ile **Stream** (Both reading and writing)

**Memory Aid**:

- **o**fstream → **o**utput → **w**rite
- **i**fstream → **i**nput → **r**ead
- **f**stream → **f**lexible → both

---

## Writing to Files (Output)

### Basic File Writing

**Steps**:

1. Include `<fstream>`
2. Create `ofstream` object
3. Write data using `<<` operator
4. Close the file

### Simple Example

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Create and open file for writing
    ofstream outFile("message.txt");
    
    // Check if file opened successfully
    if (!outFile) {
        cout << "Error opening file!" << endl;
        return 1;
    }
    
    // Write to file
    outFile << "Hello, File!" << endl;
    outFile << "This is line 2" << endl;
    outFile << "This is line 3" << endl;
    
    // Close file
    outFile.close();
    
    cout << "Data written to file successfully!" << endl;
    
    return 0;
}
```

**Result**: Creates `message.txt` with:

```
Hello, File!
This is line 2
This is line 3
```

### Alternative Opening Method

```cpp
ofstream outFile;
outFile.open("message.txt");

// Write data
outFile << "Hello!" << endl;

outFile.close();
```

### Writing Different Data Types

```cpp
ofstream outFile("data.txt");

// Write different types
int age = 25;
double gpa = 3.85;
string name = "Alice";

outFile << "Name: " << name << endl;
outFile << "Age: " << age << endl;
outFile << "GPA: " << gpa << endl;

outFile.close();
```

### File Opening Modes

You can specify how to open a file:

```cpp
// ios::out - Output mode (default for ofstream)
ofstream outFile("file.txt", ios::out);

// ios::app - Append mode (add to end of file)
ofstream outFile("file.txt", ios::app);

// ios::trunc - Truncate mode (delete existing content)
ofstream outFile("file.txt", ios::trunc);
```

**Common modes**:

- `ios::out` - Write (overwrites existing content)
- `ios::app` - Append (adds to end)
- `ios::trunc` - Truncate (clears file first)
- `ios::binary` - Binary mode

### Append Example

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write initial content
    ofstream file1("log.txt");
    file1 << "First entry" << endl;
    file1.close();
    
    // Append new content
    ofstream file2("log.txt", ios::app);
    file2 << "Second entry" << endl;
    file2 << "Third entry" << endl;
    file2.close();
    
    return 0;
}
```

**Result** (`log.txt`):

```
First entry
Second entry
Third entry
```

---

## Reading from Files (Input)

### Basic File Reading

**Steps**:

1. Include `<fstream>`
2. Create `ifstream` object
3. Read data using `>>` or `getline()`
4. Close the file

### Simple Example

```cpp
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    // Open file for reading
    ifstream inFile("message.txt");
    
    // Check if file exists
    if (!inFile) {
        cout << "Error: File not found!" << endl;
        return 1;
    }
    
    // Read and display content
    string line;
    while (getline(inFile, line)) {
        cout << line << endl;
    }
    
    // Close file
    inFile.close();
    
    return 0;
}
```

### Reading Word by Word

```cpp
ifstream inFile("data.txt");

string word;
while (inFile >> word) {
    cout << word << endl;
}

inFile.close();
```

### Reading Different Data Types

```cpp
ifstream inFile("data.txt");

string name;
int age;
double gpa;

// Assuming file format:
// Alice
// 25
// 3.85

getline(inFile, name);
inFile >> age;
inFile >> gpa;

cout << "Name: " << name << endl;
cout << "Age: " << age << endl;
cout << "GPA: " << gpa << endl;

inFile.close();
```

### Reading Line by Line

```cpp
ifstream inFile("file.txt");
string line;
int lineNumber = 1;

while (getline(inFile, line)) {
    cout << lineNumber << ": " << line << endl;
    lineNumber++;
}

inFile.close();
```

### Checking End of File

```cpp
ifstream inFile("data.txt");
int number;

while (!inFile.eof()) {  // While not end of file
    inFile >> number;
    if (!inFile.eof()) {  // Avoid reading twice at end
        cout << number << endl;
    }
}

inFile.close();
```

**Better approach** (recommended):

```cpp
while (inFile >> number) {  // Reads until failure
    cout << number << endl;
}
```

---

## File Status Checking

### Check if File Opened Successfully

```cpp
ifstream inFile("data.txt");

if (!inFile) {
    cout << "Error opening file!" << endl;
    return 1;
}

// Or using is_open()
if (!inFile.is_open()) {
    cout << "File not opened!" << endl;
    return 1;
}
```

### Check End of File

```cpp
if (inFile.eof()) {
    cout << "Reached end of file" << endl;
}
```

### Check for Errors

```cpp
if (inFile.fail()) {
    cout << "Error reading file!" << endl;
}

if (inFile.bad()) {
    cout << "Fatal error!" << endl;
}
```

### Good Practice: Always Check

```cpp
ofstream outFile("data.txt");

if (!outFile) {
    cerr << "Error: Cannot create file!" << endl;
    return 1;
}

// Proceed with writing
outFile << "Data" << endl;
outFile.close();
```

---

## Reading and Writing Together (fstream)

### Using fstream for Both Operations

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write to file
    fstream file("data.txt", ios::out);
    
    if (!file) {
        cout << "Error opening file!" << endl;
        return 1;
    }
    
    file << "Hello, World!" << endl;
    file << "Line 2" << endl;
    file.close();
    
    // Read from file
    file.open("data.txt", ios::in);
    
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    
    file.close();
    
    return 0;
}
```

### Common fstream Modes

```cpp
// Read only
fstream file("data.txt", ios::in);

// Write only (overwrites)
fstream file("data.txt", ios::out);

// Append
fstream file("data.txt", ios::app);

// Read and write
fstream file("data.txt", ios::in | ios::out);

// Write at end (for reading too)
fstream file("data.txt", ios::in | ios::app);
```

---

## File Positioning

### Get Current Position

```cpp
ifstream file("data.txt");

// Get position
streampos position = file.tellg();
cout << "Current position: " << position << endl;
```

### Set Position

```cpp
ifstream file("data.txt");

// Move to beginning
file.seekg(0, ios::beg);

// Move to end
file.seekg(0, ios::end);

// Move 10 bytes from beginning
file.seekg(10, ios::beg);

// Move 5 bytes from current position
file.seekg(5, ios::cur);
```

### Example: Read File Size

```cpp
ifstream file("data.txt");

// Move to end
file.seekg(0, ios::end);

// Get position (file size)
streampos fileSize = file.tellg();

cout << "File size: " << fileSize << " bytes" << endl;

file.close();
```

---

## Working with Structures

### Writing Structures to Files

```cpp
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Student {
    char name[50];
    int age;
    double gpa;
};

int main() {
    Student s1 = {"Alice", 20, 3.8};
    
    // Write structure to file
    ofstream outFile("student.dat", ios::binary);
    
    if (!outFile) {
        cout << "Error opening file!" << endl;
        return 1;
    }
    
    outFile.write((char*)&s1, sizeof(Student));
    
    outFile.close();
    
    cout << "Student data saved!" << endl;
    
    return 0;
}
```

### Reading Structures from Files

```cpp
Student s2;

// Read structure from file
ifstream inFile("student.dat", ios::binary);

if (!inFile) {
    cout << "Error opening file!" << endl;
    return 1;
}

inFile.read((char*)&s2, sizeof(Student));

cout << "Name: " << s2.name << endl;
cout << "Age: " << s2.age << endl;
cout << "GPA: " << s2.gpa << endl;

inFile.close();
```

### Writing Multiple Structures

```cpp
struct Student {
    char name[50];
    int age;
    double gpa;
};

int main() {
    Student students[3] = {
        {"Alice", 20, 3.8},
        {"Bob", 21, 3.5},
        {"Charlie", 19, 3.9}
    };
    
    ofstream outFile("students.dat", ios::binary);
    
    for (int i = 0; i < 3; i++) {
        outFile.write((char*)&students[i], sizeof(Student));
    }
    
    outFile.close();
    
    return 0;
}
```

### Reading Multiple Structures

```cpp
ifstream inFile("students.dat", ios::binary);

Student s;

while (inFile.read((char*)&s, sizeof(Student))) {
    cout << "Name: " << s.name << endl;
    cout << "Age: " << s.age << endl;
    cout << "GPA: " << s.gpa << endl;
    cout << "---" << endl;
}

inFile.close();
```

---

## Practical Examples and Projects

### Example 1: Simple Note-Taking Application

```cpp
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void writeNote() {
    ofstream file("notes.txt", ios::app);
    
    if (!file) {
        cout << "Error opening file!" << endl;
        return;
    }
    
    cin.ignore();
    cout << "Enter your note: ";
    string note;
    getline(cin, note);
    
    file << note << endl;
    file.close();
    
    cout << "Note saved!" << endl;
}

void readNotes() {
    ifstream file("notes.txt");
    
    if (!file) {
        cout << "No notes found!" << endl;
        return;
    }
    
    cout << "\n=== Your Notes ===" << endl;
    string line;
    int count = 1;
    
    while (getline(file, line)) {
        cout << count << ". " << line << endl;
        count++;
    }
    
    file.close();
}

int main() {
    int choice;
    
    do {
        cout << "\n=== Note Taking App ===" << endl;
        cout << "1. Add Note" << endl;
        cout << "2. View Notes" << endl;
        cout << "3. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                writeNote();
                break;
            
            case 2:
                readNotes();
                break;
            
            case 3:
                cout << "Goodbye!" << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 3);
    
    return 0;
}
```

### Example 2: Student Records System

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

struct Student {
    int id;
    char name[50];
    int age;
    double gpa;
};

void addStudent() {
    Student s;
    
    cout << "Enter Student ID: ";
    cin >> s.id;
    
    cin.ignore();
    cout << "Enter Name: ";
    cin.getline(s.name, 50);
    
    cout << "Enter Age: ";
    cin >> s.age;
    
    cout << "Enter GPA: ";
    cin >> s.gpa;
    
    ofstream file("students.dat", ios::binary | ios::app);
    
    if (!file) {
        cout << "Error opening file!" << endl;
        return;
    }
    
    file.write((char*)&s, sizeof(Student));
    file.close();
    
    cout << "Student added successfully!" << endl;
}

void displayAllStudents() {
    ifstream file("students.dat", ios::binary);
    
    if (!file) {
        cout << "No records found!" << endl;
        return;
    }
    
    Student s;
    
    cout << "\n=== Student Records ===" << endl;
    cout << setw(5) << "ID" << setw(20) << "Name" 
         << setw(5) << "Age" << setw(8) << "GPA" << endl;
    cout << string(38, '-') << endl;
    
    while (file.read((char*)&s, sizeof(Student))) {
        cout << setw(5) << s.id
             << setw(20) << s.name
             << setw(5) << s.age
             << setw(8) << fixed << setprecision(2) << s.gpa << endl;
    }
    
    file.close();
}

void searchStudent() {
    int searchId;
    cout << "Enter Student ID to search: ";
    cin >> searchId;
    
    ifstream file("students.dat", ios::binary);
    
    if (!file) {
        cout << "No records found!" << endl;
        return;
    }
    
    Student s;
    bool found = false;
    
    while (file.read((char*)&s, sizeof(Student))) {
        if (s.id == searchId) {
            cout << "\n=== Student Found ===" << endl;
            cout << "ID: " << s.id << endl;
            cout << "Name: " << s.name << endl;
            cout << "Age: " << s.age << endl;
            cout << "GPA: " << s.gpa << endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        cout << "Student not found!" << endl;
    }
    
    file.close();
}

int main() {
    int choice;
    
    do {
        cout << "\n=== Student Management System ===" << endl;
        cout << "1. Add Student" << endl;
        cout << "2. Display All Students" << endl;
        cout << "3. Search Student" << endl;
        cout << "4. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                addStudent();
                break;
            
            case 2:
                displayAllStudents();
                break;
            
            case 3:
                searchStudent();
                break;
            
            case 4:
                cout << "Goodbye!" << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 4);
    
    return 0;
}
```

### Example 3: Score Tracker

```cpp
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void saveScore(int score) {
    ofstream file("highscore.txt");
    
    if (!file) {
        cout << "Error saving score!" << endl;
        return;
    }
    
    file << score;
    file.close();
    
    cout << "Score saved: " << score << endl;
}

int loadScore() {
    ifstream file("highscore.txt");
    
    if (!file) {
        return 0;  // No previous score
    }
    
    int score;
    file >> score;
    file.close();
    
    return score;
}

int main() {
    int highScore = loadScore();
    int currentScore;
    
    cout << "Current High Score: " << highScore << endl;
    
    cout << "Enter your score: ";
    cin >> currentScore;
    
    if (currentScore > highScore) {
        cout << "New High Score!" << endl;
        saveScore(currentScore);
    } else {
        cout << "Score: " << currentScore << endl;
        cout << "High Score remains: " << highScore << endl;
    }
    
    return 0;
}
```

### Example 4: Contact Manager

```cpp
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Contact {
    char name[50];
    char phone[15];
    char email[50];
};

void addContact() {
    Contact c;
    
    cin.ignore();
    cout << "Enter Name: ";
    cin.getline(c.name, 50);
    
    cout << "Enter Phone: ";
    cin.getline(c.phone, 15);
    
    cout << "Enter Email: ";
    cin.getline(c.email, 50);
    
    ofstream file("contacts.dat", ios::binary | ios::app);
    
    if (!file) {
        cout << "Error saving contact!" << endl;
        return;
    }
    
    file.write((char*)&c, sizeof(Contact));
    file.close();
    
    cout << "Contact added!" << endl;
}

void displayContacts() {
    ifstream file("contacts.dat", ios::binary);
    
    if (!file) {
        cout << "No contacts found!" << endl;
        return;
    }
    
    Contact c;
    int count = 1;
    
    cout << "\n=== Contacts ===" << endl;
    while (file.read((char*)&c, sizeof(Contact))) {
        cout << "\nContact " << count << ":" << endl;
        cout << "Name: " << c.name << endl;
        cout << "Phone: " << c.phone << endl;
        cout << "Email: " << c.email << endl;
        count++;
    }
    
    file.close();
}

void searchContact() {
    string searchName;
    
    cin.ignore();
    cout << "Enter name to search: ";
    getline(cin, searchName);
    
    ifstream file("contacts.dat", ios::binary);
    
    if (!file) {
        cout << "No contacts found!" << endl;
        return;
    }
    
    Contact c;
    bool found = false;
    
    while (file.read((char*)&c, sizeof(Contact))) {
        if (searchName == c.name) {
            cout << "\n=== Contact Found ===" << endl;
            cout << "Name: " << c.name << endl;
            cout << "Phone: " << c.phone << endl;
            cout << "Email: " << c.email << endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        cout << "Contact not found!" << endl;
    }
    
    file.close();
}

int main() {
    int choice;
    
    do {
        cout << "\n=== Contact Manager ===" << endl;
        cout << "1. Add Contact" << endl;
        cout << "2. Display All Contacts" << endl;
        cout << "3. Search Contact" << endl;
        cout << "4. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                addContact();
                break;
            
            case 2:
                displayContacts();
                break;
            
            case 3:
                searchContact();
                break;
            
            case 4:
                cout << "Goodbye!" << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 4);
    
    return 0;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Forgetting to Include <fstream>

❌ **Wrong**:

```cpp
#include <iostream>
using namespace std;

int main() {
    ofstream file("data.txt");  // Error! ofstream not defined
}
```

✅ **Correct**:

```cpp
#include <iostream>
#include <fstream>  // Include this!
using namespace std;

int main() {
    ofstream file("data.txt");  // Works!
}
```

### Mistake 2: Not Checking if File Opened

❌ **Wrong**:

```cpp
ifstream file("data.txt");
string line;
getline(file, line);  // What if file doesn't exist?
```

✅ **Correct**:

```cpp
ifstream file("data.txt");

if (!file) {
    cout << "Error: File not found!" << endl;
    return 1;
}

string line;
getline(file, line);
```

### Mistake 3: Forgetting to Close Files

❌ **Wrong**:

```cpp
ofstream file("data.txt");
file << "Hello";
// Forgot to close!
```

✅ **Correct**:

```cpp
ofstream file("data.txt");
file << "Hello";
file.close();  // Always close!
```

### Mistake 4: Wrong File Mode

❌ **Wrong**:

```cpp
// Trying to read from output-only stream
ofstream file("data.txt");
string line;
getline(file, line);  // Error! Can't read from ofstream
```

✅ **Correct**:

```cpp
// Use ifstream for reading
ifstream file("data.txt");
string line;
getline(file, line);  // Works!
```

### Mistake 5: Overwriting Instead of Appending

❌ **Wrong**:

```cpp
ofstream file("log.txt");  // Overwrites existing content!
file << "New entry" << endl;
```

✅ **Correct** (if you want to append):

```cpp
ofstream file("log.txt", ios::app);  // Appends to end
file << "New entry" << endl;
```

### Mistake 6: Not Using Binary Mode for Structures

❌ **Wrong**:

```cpp
struct Student { int id; char name[50]; };
Student s = {1, "Alice"};

ofstream file("data.txt");  // Text mode - will corrupt data!
file.write((char*)&s, sizeof(Student));
```

✅ **Correct**:

```cpp
ofstream file("data.dat", ios::binary);  // Binary mode!
file.write((char*)&s, sizeof(Student));
```

### Mistake 7: Reading Beyond End of File

❌ **Wrong**:

```cpp
ifstream file("data.txt");
string line;

while (!file.eof()) {
    getline(file, line);
    cout << line << endl;  // Might print last line twice!
}
```

✅ **Correct**:

```cpp
ifstream file("data.txt");
string line;

while (getline(file, line)) {  // Reads until failure
    cout << line << endl;
}
```

---

## Best Practices

### ✅ DO:

1. **Always include <fstream>**
    
    ```cpp
    #include <fstream>
    ```
    
2. **Check if file opened successfully**
    
    ```cpp
    if (!file) {
        // Handle error
    }
    ```
    
3. **Close files when done**
    
    ```cpp
    file.close();
    ```
    
4. **Use appropriate file mode**
    
    ```cpp
    ofstream file("data.txt", ios::app);  // Append
    ```
    
5. **Use binary mode for structures**
    
    ```cpp
    ofstream file("data.dat", ios::binary);
    ```
    
6. **Use meaningful file names**
    
    ```cpp
    ofstream file("student_records.dat");  // Clear
    ```
    

### ❌ DON'T:

1. **Don't forget to close files**
    
    ```cpp
    // file.close();  // Don't forget!
    ```
    
2. **Don't ignore file errors**
    
    ```cpp
    ofstream file("data.txt");
    // Check if opened!
    ```
    
3. **Don't mix text and binary modes**
    
    ```cpp
    // Pick one and stick with it
    ```
    
4. **Don't hardcode file paths unnecessarily**
    
    ```cpp
    // string filename = "data.txt";  // Better
    ```
    

---

## File Handling Summary Table

|Operation|Class|Mode|Example|
|---|---|---|---|
|Write (overwrite)|ofstream|ios::out|`ofstream file("data.txt");`|
|Write (append)|ofstream|ios::app|`ofstream file("data.txt", ios::app);`|
|Read|ifstream|ios::in|`ifstream file("data.txt");`|
|Read & Write|fstream|ios::in \| ios::out|`fstream file("data.txt", ios::in \| ios::out);`|
|Binary write|ofstream|ios::binary|`ofstream file("data.dat", ios::binary);`|
|Binary read|ifstream|ios::binary|`ifstream file("data.dat", ios::binary);`|

---

## Practice Exercises

### Exercise 1: Text File Writer

Write a program that:

- Asks user for text input
- Saves it to a file
- Allows user to view the saved text

### Exercise 2: Number File Processor

Create a program that:

- Writes 10 random numbers to a file
- Reads them back
- Calculates and displays their sum and average

### Exercise 3: Password Manager

Build a simple password manager that:

- Saves website name and password to file
- Displays all saved passwords
- Searches for password by website name

### Exercise 4: Grade Book

Create a grade tracking system that:

- Stores student names and grades in a file
- Displays all students and grades
- Calculates class average
- Finds highest and lowest grades

### Exercise 5: Diary Application

Develop a diary app that:

- Saves dated entries to a file
- Displays all entries with dates
- Allows searching by date
- Appends new entries without deleting old ones

### Exercise 6: Inventory System

Build an inventory manager that:

- Saves product ID, name, and quantity to binary file
- Displays all products
- Updates product quantity
- Searches for products by ID

### Exercise 7: Configuration File

Create a program that:

- Saves user preferences to a config file
- Loads preferences on startup
- Allows updating preferences
- Uses key-value pairs (e.g., "theme=dark")

### Exercise 8: File Copy Program

Write a program that:

- Reads content from one file
- Copies it to another file
- Shows progress during copy
- Handles errors gracefully

---

## Quick Reference

### Include Header

```cpp
#include <fstream>
```

### Writing to File

```cpp
ofstream file("data.txt");
file << "Hello" << endl;
file.close();
```

### Reading from File

```cpp
ifstream file("data.txt");
string line;
while (getline(file, line)) {
    cout << line << endl;
}
file.close();
```

### Append to File

```cpp
ofstream file("data.txt", ios::app);
file << "New line" << endl;
file.close();
```

### Binary Read/Write

```cpp
// Write
ofstream out("data.dat", ios::binary);
out.write((char*)&data, sizeof(data));
out.close();

// Read
ifstream in("data.dat", ios::binary);
in.read((char*)&data, sizeof(data));
in.close();
```

### Check File Status

```cpp
if (!file) {
    // Error handling
}

if (file.is_open()) {
    // File is open
}

if (file.eof()) {
    // End of file
}
```

---

## Summary

### Key Concepts

1. **ofstream** - Writing to files
2. **ifstream** - Reading from files
3. **fstream** - Both reading and writing
4. **File modes** - How to open files (read, write, append, binary)
5. **Error checking** - Always verify file operations
6. **Binary vs Text** - Different modes for different data

### What You Can Do With File Handling

✓ Save game progress  
✓ Store user data permanently  
✓ Create databases  
✓ Log program events  
✓ Exchange data between programs  
✓ Process large datasets  
✓ Backup and restore information

### Important Rules

⚠️ Always include `<fstream>`  
⚠️ Check if file opened successfully  
⚠️ Close files after use  
⚠️ Use correct file mode  
⚠️ Use binary mode for structures  
⚠️ Handle file errors gracefully

---

## Next Steps

After mastering file handling, you can explore:

- **Exception Handling** - Better error management
- **Database Integration** - SQL databases
- **JSON/XML Parsing** - Structured data formats
- **Networking** - Send files over network
- **Compression** - ZIP file handling
- **Encryption** - Secure file storage

---

## Conclusion

File handling is essential for creating practical, real-world applications. It allows your programs to:

- **Remember information** - Data persists between runs
- **Share data** - Exchange information with other programs
- **Process large datasets** - Handle more data than fits in memory
- **Create robust applications** - Professional-grade software

Master file handling, and you'll unlock the ability to create programs that store and retrieve data permanently!

**Happy Coding!** 🚀

---

## Remember

_"Files are the bridge between temporary memory and permanent storage"_

Every professional application uses files:

- Games save progress
- Databases store records
- Applications remember settings
- Systems log events

Learn file handling well, and you'll be able to create truly useful, persistent applications! 💾