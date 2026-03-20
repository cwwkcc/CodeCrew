## Introduction

Imagine you need to store test scores for 50 students. Would you create 50 separate variables like `score1`, `score2`, `score3`...`score50`? Absolutely not! That's where **arrays** come in.

An **array** is a collection of elements of the same data type stored in contiguous memory locations. Think of it as a numbered list or a row of boxes, each holding one value.

**Real-World Analogies**:

- **Apartment Building**: Each apartment has a number (index), and all apartments are in one building (array)
- **Egg Carton**: 12 compartments (indices 0-11), each holding one egg (element)
- **Parking Lot**: Numbered parking spaces (indices), each can hold one car (value)
- **Bookshelf**: Books arranged in order, numbered from left to right
- **Class Roster**: List of student names, each at a specific position

---

## Why Arrays Matter

### Without Arrays

```cpp
// Storing 5 student scores
int score1 = 85;
int score2 = 90;
int score3 = 78;
int score4 = 92;
int score5 = 88;

// Calculating average
int sum = score1 + score2 + score3 + score4 + score5;
double average = sum / 5.0;

// What if we have 100 students? Impossible!
```

### With Arrays

```cpp
// Storing 5 student scores
int scores[5] = {85, 90, 78, 92, 88};

// Calculating average
int sum = 0;
for (int i = 0; i < 5; i++) {
    sum += scores[i];
}
double average = sum / 5.0;

// Easy to expand to 100 students - just change size!
```

### Benefits of Arrays

1. **Organize related data** - Group similar items together
2. **Easy access** - Access any element by index
3. **Efficient loops** - Process all elements easily
4. **Fixed size** - Size known at compile time
5. **Contiguous memory** - Fast access and cache-friendly
6. **Foundation for data structures** - Basis for lists, stacks, queues

---

## Array Basics

### Declaration

**Syntax**:

```cpp
dataType arrayName[size];
```

**Examples**:

```cpp
int numbers[5];           // Array of 5 integers
double prices[10];        // Array of 10 doubles
char letters[26];         // Array of 26 characters
bool flags[100];          // Array of 100 booleans
string names[50];         // Array of 50 strings
```

**Important**: Size must be a constant (known at compile time)

```cpp
// Valid
int scores[10];
const int SIZE = 20;
int values[SIZE];

// Invalid
int n = 10;
int array[n];  // Error! Size must be constant in standard C++
```

### Initialization

**Method 1: Initialize during declaration**

```cpp
int numbers[5] = {10, 20, 30, 40, 50};
```

**Method 2: Partial initialization**

```cpp
int numbers[5] = {10, 20};  // {10, 20, 0, 0, 0}
// Uninitialized elements become 0
```

**Method 3: Initialize all to zero**

```cpp
int numbers[5] = {0};  // {0, 0, 0, 0, 0}
int numbers[5] = {};   // Also works (C++11)
```

**Method 4: Let compiler determine size**

```cpp
int numbers[] = {10, 20, 30, 40, 50};  // Size automatically 5
```

**Method 5: Initialize after declaration**

```cpp
int numbers[5];
numbers[0] = 10;
numbers[1] = 20;
numbers[2] = 30;
numbers[3] = 40;
numbers[4] = 50;
```

### Accessing Elements

Arrays use **zero-based indexing** - first element is at index 0.

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

// Accessing elements
cout << numbers[0] << endl;  // 10 (first element)
cout << numbers[1] << endl;  // 20 (second element)
cout << numbers[4] << endl;  // 50 (fifth/last element)

// Modifying elements
numbers[0] = 100;  // Change first element to 100
numbers[2] = 300;  // Change third element to 300
```

**Visual representation**:

```
Array: numbers[5] = {10, 20, 30, 40, 50}

Index:     0    1    2    3    4
Value:    10   20   30   40   50
         [  ] [  ] [  ] [  ] [  ]
```

### Array Size

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

// Get size in bytes
int sizeInBytes = sizeof(numbers);  // 20 bytes (5 * 4)

// Get number of elements
int numElements = sizeof(numbers) / sizeof(numbers[0]);  // 5

cout << "Array has " << numElements << " elements" << endl;
```

---

## Working with Arrays

### Iterating Through Arrays (for loop)

**Standard for loop**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

// Print all elements
for (int i = 0; i < 5; i++) {
    cout << numbers[i] << " ";
}
cout << endl;
// Output: 10 20 30 40 50
```

**Range-based for loop (C++11)**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

// Print all elements
for (int num : numbers) {
    cout << num << " ";
}
cout << endl;
// Output: 10 20 30 40 50
```

### Getting Input into Arrays

```cpp
#include <iostream>
using namespace std;

int main() {
    const int SIZE = 5;
    int numbers[SIZE];
    
    cout << "Enter " << SIZE << " numbers:" << endl;
    
    for (int i = 0; i < SIZE; i++) {
        cout << "Number " << (i + 1) << ": ";
        cin >> numbers[i];
    }
    
    cout << "\nYou entered: ";
    for (int i = 0; i < SIZE; i++) {
        cout << numbers[i] << " ";
    }
    cout << endl;
    
    return 0;
}
```

### Common Array Operations

**Sum of elements**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};
int sum = 0;

for (int i = 0; i < 5; i++) {
    sum += numbers[i];
}

cout << "Sum: " << sum << endl;  // 150
```

**Average**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};
int sum = 0;

for (int i = 0; i < 5; i++) {
    sum += numbers[i];
}

double average = sum / 5.0;
cout << "Average: " << average << endl;  // 30
```

**Find maximum**:

```cpp
int numbers[5] = {10, 50, 30, 90, 20};
int max = numbers[0];

for (int i = 1; i < 5; i++) {
    if (numbers[i] > max) {
        max = numbers[i];
    }
}

cout << "Maximum: " << max << endl;  // 90
```

**Find minimum**:

```cpp
int numbers[5] = {10, 50, 30, 90, 20};
int min = numbers[0];

for (int i = 1; i < 5; i++) {
    if (numbers[i] < min) {
        min = numbers[i];
    }
}

cout << "Minimum: " << min << endl;  // 10
```

**Count occurrences**:

```cpp
int numbers[10] = {5, 3, 5, 7, 5, 2, 5, 8, 5, 1};
int target = 5;
int count = 0;

for (int i = 0; i < 10; i++) {
    if (numbers[i] == target) {
        count++;
    }
}

cout << target << " appears " << count << " times" << endl;  // 5 times
```

---

## Arrays and Functions

### Passing Arrays to Functions

**Important**: Arrays are passed by reference (automatically)

```cpp
#include <iostream>
using namespace std;

// Function to print array
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

// Function to modify array
void doubleValues(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] *= 2;  // Modifies original array!
    }
}

int main() {
    int numbers[5] = {1, 2, 3, 4, 5};
    
    cout << "Original: ";
    printArray(numbers, 5);
    
    doubleValues(numbers, 5);
    
    cout << "After doubling: ";
    printArray(numbers, 5);
    
    return 0;
}

// Output:
// Original: 1 2 3 4 5
// After doubling: 2 4 6 8 10
```

### Why Size Parameter is Needed

Arrays don't carry their size information when passed to functions!

```cpp
// Must pass size separately
void processArray(int arr[], int size) {
    // Can't use sizeof(arr) here - gives wrong result!
    for (int i = 0; i < size; i++) {
        // Process elements
    }
}
```

### Returning Arrays from Functions

**Can't return array directly**:

```cpp
// This doesn't work!
int[] createArray() {  // Error!
    int arr[5] = {1, 2, 3, 4, 5};
    return arr;  // Error!
}
```

**Use pointers (advanced)**:

```cpp
int* createArray(int size) {
    int* arr = new int[size];
    // Fill array
    return arr;
}
// Note: Need to delete[] later
```

**Better: Pass array as parameter**:

```cpp
void fillArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = i + 1;
    }
}

int main() {
    int numbers[5];
    fillArray(numbers, 5);
    // Now numbers = {1, 2, 3, 4, 5}
}
```

---

## Searching in Arrays

### Linear Search

Search sequentially from start to end.

```cpp
#include <iostream>
using namespace std;

int linearSearch(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;  // Found! Return index
        }
    }
    return -1;  // Not found
}

int main() {
    int numbers[7] = {10, 25, 30, 45, 50, 60, 75};
    int target = 45;
    
    int index = linearSearch(numbers, 7, target);
    
    if (index != -1) {
        cout << target << " found at index " << index << endl;
    } else {
        cout << target << " not found" << endl;
    }
    
    return 0;
}
```

**Time Complexity**: O(n) - might check all elements

### Binary Search

Efficient search for **sorted** arrays.

```cpp
#include <iostream>
using namespace std;

int binarySearch(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;  // Found!
        }
        
        if (arr[mid] < target) {
            left = mid + 1;  // Search right half
        } else {
            right = mid - 1;  // Search left half
        }
    }
    
    return -1;  // Not found
}

int main() {
    int numbers[7] = {10, 25, 30, 45, 50, 60, 75};  // Must be sorted!
    int target = 45;
    
    int index = binarySearch(numbers, 7, target);
    
    if (index != -1) {
        cout << target << " found at index " << index << endl;
    } else {
        cout << target << " not found" << endl;
    }
    
    return 0;
}
```

**Time Complexity**: O(log n) - much faster for large arrays!

**Visual representation of binary search for target=45**:

```
Array: [10, 25, 30, 45, 50, 60, 75]
        L           M            R
        
Step 1: mid=3, arr[3]=45 → Found!
```

---

## Sorting Arrays

### Bubble Sort

Simple but slow sorting algorithm.

```cpp
#include <iostream>
using namespace std;

void bubbleSort(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int main() {
    int numbers[6] = {64, 34, 25, 12, 22, 11};
    
    cout << "Before sorting: ";
    printArray(numbers, 6);
    
    bubbleSort(numbers, 6);
    
    cout << "After sorting: ";
    printArray(numbers, 6);
    
    return 0;
}

// Output:
// Before sorting: 64 34 25 12 22 11
// After sorting: 11 12 22 25 34 64
```

**How bubble sort works**:

```
Pass 1: Compare adjacent pairs, swap if needed
[64, 34, 25, 12, 22, 11]
[34, 64, 25, 12, 22, 11]
[34, 25, 64, 12, 22, 11]
[34, 25, 12, 64, 22, 11]
[34, 25, 12, 22, 64, 11]
[34, 25, 12, 22, 11, 64] ← Largest at end

Pass 2: Repeat for remaining elements
... continues until sorted
```

### Selection Sort

Find minimum and place it at the beginning.

```cpp
void selectionSort(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        int minIndex = i;
        
        // Find minimum in remaining array
        for (int j = i + 1; j < size; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        
        // Swap with first position of unsorted part
        if (minIndex != i) {
            int temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }
}
```

### Insertion Sort

Build sorted array one element at a time.

```cpp
void insertionSort(int arr[], int size) {
    for (int i = 1; i < size; i++) {
        int key = arr[i];
        int j = i - 1;
        
        // Move elements greater than key one position ahead
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        
        arr[j + 1] = key;
    }
}
```

---

## Multi-Dimensional Arrays

### Two-Dimensional Arrays (2D)

Arrays of arrays - like a table or matrix.

**Declaration and initialization**:

```cpp
// Method 1: Declare then initialize
int matrix[3][4];  // 3 rows, 4 columns

// Method 2: Initialize during declaration
int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

// Method 3: Single line
int matrix[3][4] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
```

**Visual representation**:

```
matrix[3][4]:
         Col 0  Col 1  Col 2  Col 3
Row 0:     1      2      3      4
Row 1:     5      6      7      8
Row 2:     9     10     11     12
```

**Accessing elements**:

```cpp
int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

cout << matrix[0][0] << endl;  // 1 (first row, first column)
cout << matrix[0][3] << endl;  // 4 (first row, last column)
cout << matrix[2][1] << endl;  // 10 (third row, second column)

matrix[1][2] = 100;  // Change value at row 1, column 2
```

**Iterating through 2D arrays**:

```cpp
int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

// Nested loops
for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 4; col++) {
        cout << matrix[row][col] << " ";
    }
    cout << endl;
}

// Output:
// 1 2 3 4
// 5 6 7 8
// 9 10 11 12
```

### Getting Input for 2D Arrays

```cpp
#include <iostream>
using namespace std;

int main() {
    const int ROWS = 3;
    const int COLS = 3;
    int matrix[ROWS][COLS];
    
    cout << "Enter 9 numbers for 3x3 matrix:" << endl;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << "Element [" << i << "][" << j << "]: ";
            cin >> matrix[i][j];
        }
    }
    
    cout << "\nMatrix:" << endl;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    
    return 0;
}
```

### 2D Array Operations

**Sum of all elements**:

```cpp
int matrix[3][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};

int sum = 0;
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        sum += matrix[i][j];
    }
}
cout << "Sum: " << sum << endl;  // 45
```

**Sum of each row**:

```cpp
int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

for (int i = 0; i < 3; i++) {
    int rowSum = 0;
    for (int j = 0; j < 4; j++) {
        rowSum += matrix[i][j];
    }
    cout << "Row " << i << " sum: " << rowSum << endl;
}

// Output:
// Row 0 sum: 10
// Row 1 sum: 26
// Row 2 sum: 42
```

**Sum of each column**:

```cpp
for (int j = 0; j < 4; j++) {
    int colSum = 0;
    for (int i = 0; i < 3; i++) {
        colSum += matrix[i][j];
    }
    cout << "Column " << j << " sum: " << colSum << endl;
}

// Output:
// Column 0 sum: 15
// Column 1 sum: 18
// Column 2 sum: 21
// Column 3 sum: 24
```

**Matrix addition**:

```cpp
int A[2][2] = {{1, 2}, {3, 4}};
int B[2][2] = {{5, 6}, {7, 8}};
int C[2][2];

for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
        C[i][j] = A[i][j] + B[i][j];
    }
}

// C = {{6, 8}, {10, 12}}
```

**Transpose matrix**:

```cpp
int matrix[3][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};

int transpose[3][3];

for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        transpose[j][i] = matrix[i][j];
    }
}

// Original:       Transpose:
// 1 2 3           1 4 7
// 4 5 6           2 5 8
// 7 8 9           3 6 9
```

---

## Character Arrays (C-Strings)

### C-Style Strings

Arrays of characters ending with null character `\0`.

```cpp
#include <iostream>
#include <cstring>
using namespace std;

int main() {
    // Declaration and initialization
    char name1[20] = "John";  // Automatically adds \0
    char name2[] = {'J', 'o', 'h', 'n', '\0'};  // Manual null terminator
    
    // Input
    char name3[50];
    cout << "Enter name: ";
    cin >> name3;  // Stops at whitespace
    
    // Input with spaces
    char fullName[100];
    cout << "Enter full name: ";
    cin.getline(fullName, 100);
    
    // String functions (from <cstring>)
    char str1[20] = "Hello";
    char str2[20] = "World";
    
    // Length
    cout << "Length: " << strlen(str1) << endl;  // 5
    
    // Copy
    char copy[20];
    strcpy(copy, str1);
    cout << "Copy: " << copy << endl;  // Hello
    
    // Concatenate
    char combined[40] = "Hello ";
    strcat(combined, "World");
    cout << "Combined: " << combined << endl;  // Hello World
    
    // Compare
    if (strcmp(str1, str2) == 0) {
        cout << "Equal" << endl;
    } else {
        cout << "Not equal" << endl;
    }
    
    return 0;
}
```

**Common C-string functions**:

```cpp
strlen(str)         // Get length
strcpy(dest, src)   // Copy string
strcat(dest, src)   // Concatenate
strcmp(str1, str2)  // Compare (0 if equal)
strchr(str, ch)     // Find character
strstr(str, substr) // Find substring
```

---

## Practical Examples and Projects

### Example 1: Student Grade System

```cpp
#include <iostream>
#include <iomanip>
using namespace std;

const int MAX_STUDENTS = 50;

void inputGrades(double grades[], int &count);
void displayGrades(double grades[], int count);
void calculateStatistics(double grades[], int count);
char getLetterGrade(double score);

int main() {
    double grades[MAX_STUDENTS];
    int count = 0;
    int choice;
    
    do {
        cout << "\n=== Grade Management System ===" << endl;
        cout << "1. Enter Grades" << endl;
        cout << "2. Display Grades" << endl;
        cout << "3. Statistics" << endl;
        cout << "4. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                inputGrades(grades, count);
                break;
            
            case 2:
                if (count > 0) {
                    displayGrades(grades, count);
                } else {
                    cout << "No grades entered!" << endl;
                }
                break;
            
            case 3:
                if (count > 0) {
                    calculateStatistics(grades, count);
                } else {
                    cout << "No grades entered!" << endl;
                }
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

void inputGrades(double grades[], int &count) {
    cout << "How many students? ";
    cin >> count;
    
    if (count > MAX_STUDENTS) {
        cout << "Maximum " << MAX_STUDENTS << " students!" << endl;
        count = MAX_STUDENTS;
    }
    
    for (int i = 0; i < count; i++) {
        do {
            cout << "Grade for student " << (i + 1) << " (0-100): ";
            cin >> grades[i];
            
            if (grades[i] < 0 || grades[i] > 100) {
                cout << "Invalid! Must be 0-100." << endl;
            }
        } while (grades[i] < 0 || grades[i] > 100);
    }
}

void displayGrades(double grades[], int count) {
    cout << fixed << setprecision(2);
    cout << "\n=== Student Grades ===" << endl;
    
    for (int i = 0; i < count; i++) {
        cout << "Student " << setw(2) << (i + 1) << ": " 
             << setw(6) << grades[i] << " (" 
             << getLetterGrade(grades[i]) << ")" << endl;
    }
}

void calculateStatistics(double grades[], int count) {
    double sum = 0;
    double highest = grades[0];
    double lowest = grades[0];
    
    for (int i = 0; i < count; i++) {
        sum += grades[i];
        if (grades[i] > highest) highest = grades[i];
        if (grades[i] < lowest) lowest = grades[i];
    }
    
    double average = sum / count;
    
    // Count letter grades
    int countA = 0, countB = 0, countC = 0, countD = 0, countF = 0;
    
    for (int i = 0; i < count; i++) {
        char grade = getLetterGrade(grades[i]);
        switch (grade) {
            case 'A': countA++; break;
            case 'B': countB++; break;
            case 'C': countC++; break;
            case 'D': countD++; break;
            case 'F': countF++; break;
        }
    }
    
    cout << fixed << setprecision(2);
    cout << "\n=== Statistics ===" << endl;
    cout << "Number of students: " << count << endl;
    cout << "Average: " << average << " (" << getLetterGrade(average) << ")" << endl;
    cout << "Highest: " << highest << endl;
    cout << "Lowest: " << lowest << endl;
    
    cout << "\n--- Grade Distribution ---" << endl;
    cout << "A: " << countA << " students" << endl;
    cout << "B: " << countB << " students" << endl;
    cout << "C: " << countC << " students" << endl;
    cout << "D: " << countD << " students" << endl;
    cout << "F: " << countF << " students" << endl;
}

char getLetterGrade(double score) {
    if (score >= 90) return 'A';
    else if (score >= 80) return 'B';
    else if (score >= 70) return 'C';
    else if (score >= 60) return 'D';
    else return 'F';
}
```

### Example 2: Tic-Tac-Toe Game

```cpp
#include <iostream>
using namespace std;

const int SIZE = 3;

void initializeBoard(char board[SIZE][SIZE]);
void displayBoard(char board[SIZE][SIZE]);
bool makeMove(char board[SIZE][SIZE], int row, int col, char player);
bool checkWin(char board[SIZE][SIZE], char player);
bool checkDraw(char board[SIZE][SIZE]);

int main() {
    char board[SIZE][SIZE];
    char currentPlayer = 'X';
    int row, col;
    bool gameOver = false;
    
    initializeBoard(board);
    
    cout << "=== Tic-Tac-Toe ===" << endl;
    cout << "Positions: 1-3 for rows and columns" << endl;
    
    while (!gameOver) {
        displayBoard(board);
        
        cout << "\nPlayer " << currentPlayer << "'s turn" << endl;
        cout << "Enter row (1-3): ";
        cin >> row;
        cout << "Enter column (1-3): ";
        cin >> col;
        
        // Convert to 0-indexed
        row--;
        col--;
        
        if (makeMove(board, row, col, currentPlayer)) {
            if (checkWin(board, currentPlayer)) {
                displayBoard(board);
                cout << "\nPlayer " << currentPlayer << " wins!" << endl;
                gameOver = true;
            } else if (checkDraw(board)) {
                displayBoard(board);
                cout << "\nIt's a draw!" << endl;
                gameOver = true;
            } else {
                // Switch player
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        } else {
            cout << "Invalid move! Try again." << endl;
        }
    }
    
    return 0;
}

void initializeBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

void displayBoard(char board[SIZE][SIZE]) {
    cout << "\n";
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << " " << board[i][j];
            if (j < SIZE - 1) cout << " |";
        }
        cout << endl;
        if (i < SIZE - 1) cout << "---|---|---" << endl;
    }
}

bool makeMove(char board[SIZE][SIZE], int row, int col, char player) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        return false;  // Out of bounds
    }
    
    if (board[row][col] != ' ') {
        return false;  // Position occupied
    }
    
    board[row][col] = player;
    return true;
}

bool checkWin(char board[SIZE][SIZE], char player) {
    // Check rows
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return true;
        }
    }
    
    // Check columns
    for (int j = 0; j < SIZE; j++) {
        if (board[0][j] == player && board[1][j] == player && board[2][j] == player) {
            return true;
        }
    }
    
    // Check diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return true;
    }
    
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return true;
    }
    
    return false;
}

bool checkDraw(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') {
                return false;  // Empty space found
            }
        }
    }
    return true;  // No empty spaces
}
```

### Example 3: Inventory Management System

```cpp
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

const int MAX_ITEMS = 100;

struct Item {
    int id;
    string name;
    int quantity;
    double price;
};

void addItem(Item inventory[], int &count);
void displayInventory(Item inventory[], int count);
void searchItem(Item inventory[], int count);
void updateQuantity(Item inventory[], int count);
double calculateTotalValue(Item inventory[], int count);

int main() {
    Item inventory[MAX_ITEMS];
    int itemCount = 0;
    int choice;
    
    do {
        cout << "\n=== Inventory Management ===" << endl;
        cout << "1. Add Item" << endl;
        cout << "2. Display Inventory" << endl;
        cout << "3. Search Item" << endl;
        cout << "4. Update Quantity" << endl;
        cout << "5. Total Inventory Value" << endl;
        cout << "6. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                addItem(inventory, itemCount);
                break;
            
            case 2:
                displayInventory(inventory, itemCount);
                break;
            
            case 3:
                searchItem(inventory, itemCount);
                break;
            
            case 4:
                updateQuantity(inventory, itemCount);
                break;
            
            case 5:
                cout << fixed << setprecision(2);
                cout << "Total Inventory Value: $" 
                     << calculateTotalValue(inventory, itemCount) << endl;
                break;
            
            case 6:
                cout << "Exiting..." << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 6);
    
    return 0;
}

void addItem(Item inventory[], int &count) {
    if (count >= MAX_ITEMS) {
        cout << "Inventory full!" << endl;
        return;
    }
    
    Item newItem;
    
    cout << "Enter Item ID: ";
    cin >> newItem.id;
    
    cin.ignore();  // Clear newline
    cout << "Enter Item Name: ";
    getline(cin, newItem.name);
    
    cout << "Enter Quantity: ";
    cin >> newItem.quantity;
    
    cout << "Enter Price: $";
    cin >> newItem.price;
    
    inventory[count] = newItem;
    count++;
    
    cout << "Item added successfully!" << endl;
}

void displayInventory(Item inventory[], int count) {
    if (count == 0) {
        cout << "Inventory is empty!" << endl;
        return;
    }
    
    cout << fixed << setprecision(2);
    cout << "\n=== Inventory ===" << endl;
    cout << setw(5) << "ID" << setw(20) << "Name" 
         << setw(10) << "Qty" << setw(10) << "Price" 
         << setw(12) << "Total" << endl;
    cout << string(57, '-') << endl;
    
    for (int i = 0; i < count; i++) {
        double total = inventory[i].quantity * inventory[i].price;
        cout << setw(5) << inventory[i].id
             << setw(20) << inventory[i].name
             << setw(10) << inventory[i].quantity
             << setw(10) << inventory[i].price
             << setw(12) << total << endl;
    }
}

void searchItem(Item inventory[], int count) {
    int searchId;
    cout << "Enter Item ID to search: ";
    cin >> searchId;
    
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (inventory[i].id == searchId) {
            cout << "\nItem Found:" << endl;
            cout << "ID: " << inventory[i].id << endl;
            cout << "Name: " << inventory[i].name << endl;
            cout << "Quantity: " << inventory[i].quantity << endl;
            cout << fixed << setprecision(2);
            cout << "Price: $" << inventory[i].price << endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        cout << "Item not found!" << endl;
    }
}

void updateQuantity(Item inventory[], int count) {
    int searchId, newQuantity;
    
    cout << "Enter Item ID: ";
    cin >> searchId;
    
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (inventory[i].id == searchId) {
            cout << "Current Quantity: " << inventory[i].quantity << endl;
            cout << "Enter New Quantity: ";
            cin >> newQuantity;
            
            inventory[i].quantity = newQuantity;
            cout << "Quantity updated!" << endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        cout << "Item not found!" << endl;
    }
}

double calculateTotalValue(Item inventory[], int count) {
    double total = 0;
    
    for (int i = 0; i < count; i++) {
        total += inventory[i].quantity * inventory[i].price;
    }
    
    return total;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Array Index Out of Bounds

**Wrong**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

cout << numbers[5] << endl;  // ⚠️ Out of bounds! Index 0-4 only
cout << numbers[10] << endl; // ⚠️ Way out of bounds!

numbers[5] = 100;  // ⚠️ Overwrites memory - dangerous!
```

**Correct**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

// Valid indices: 0 to 4
cout << numbers[0] << endl;  // First element
cout << numbers[4] << endl;  // Last element

// Use size constant
const int SIZE = 5;
int numbers[SIZE];

for (int i = 0; i < SIZE; i++) {  // i goes from 0 to 4
    cout << numbers[i] << endl;
}
```

### Mistake 2: Not Initializing Array

**Wrong**:

```cpp
int numbers[5];  // Contains garbage values!

for (int i = 0; i < 5; i++) {
    cout << numbers[i] << endl;  // Prints random numbers
}
```

**Correct**:

```cpp
int numbers[5] = {0};  // Initialize all to 0

// Or initialize explicitly
int numbers[5];
for (int i = 0; i < 5; i++) {
    numbers[i] = 0;
}
```

### Mistake 3: Comparing Arrays with ==

**Wrong**:

```cpp
int arr1[3] = {1, 2, 3};
int arr2[3] = {1, 2, 3};

if (arr1 == arr2) {  // ⚠️ Compares addresses, not contents!
    cout << "Equal";
}
```

**Correct**:

```cpp
int arr1[3] = {1, 2, 3};
int arr2[3] = {1, 2, 3};
bool equal = true;

for (int i = 0; i < 3; i++) {
    if (arr1[i] != arr2[i]) {
        equal = false;
        break;
    }
}

if (equal) {
    cout << "Equal";
}
```

### Mistake 4: Forgetting Null Terminator in C-Strings

**Wrong**:

```cpp
char name[4] = {'J', 'o', 'h', 'n'};  // No null terminator!
cout << name << endl;  // May print garbage after "John"
```

**Correct**:

```cpp
char name[5] = {'J', 'o', 'h', 'n', '\0'};  // Null terminator
// Or
char name[] = "John";  // Automatically adds \0
```

### Mistake 5: Array Size Mismatch in Functions

**Wrong**:

```cpp
void processArray(int arr[]) {
    int size = sizeof(arr) / sizeof(arr[0]);  // ⚠️ Wrong size!
    // sizeof(arr) returns pointer size, not array size
}
```

**Correct**:

```cpp
void processArray(int arr[], int size) {  // Pass size separately
    for (int i = 0; i < size; i++) {
        // Process elements
    }
}
```

### Mistake 6: Returning Local Array

**Wrong**:

```cpp
int* createArray() {
    int arr[5] = {1, 2, 3, 4, 5};
    return arr;  // ⚠️ Returns address of local array!
}  // arr is destroyed here - dangling pointer!
```

**Correct**:

```cpp
void fillArray(int arr[], int size) {  // Pass array from caller
    for (int i = 0; i < size; i++) {
        arr[i] = i + 1;
    }
}

int main() {
    int numbers[5];
    fillArray(numbers, 5);
    // Now numbers = {1, 2, 3, 4, 5}
}
```

### Mistake 7: Confusing Array Size and Last Index

**Wrong**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

for (int i = 0; i <= 5; i++) {  // ⚠️ Goes to index 5!
    cout << numbers[i] << endl;
}
```

**Correct**:

```cpp
int numbers[5] = {10, 20, 30, 40, 50};

for (int i = 0; i < 5; i++) {  // i goes 0 to 4
    cout << numbers[i] << endl;
}
```

---

## Practice Exercises

### Exercise 1: Array Basics

Write programs that:

- Find sum and average of array elements
- Find maximum and minimum values
- Count positive, negative, and zero elements
- Reverse an array
- Copy one array to another

### Exercise 2: Array Searching

Implement:

- Linear search function
- Binary search function (for sorted array)
- Find all occurrences of a value
- Find second largest element
- Check if array is sorted

### Exercise 3: Array Sorting

Implement sorting algorithms:

- Bubble sort
- Selection sort
- Insertion sort
- Compare their performance

### Exercise 4: Array Operations

Create functions to:

- Merge two sorted arrays
- Remove duplicates from array
- Rotate array left/right by k positions
- Find missing number in sequence
- Check if two arrays are equal

### Exercise 5: 2D Array Operations

Implement:

- Matrix addition
- Matrix subtraction
- Matrix multiplication
- Transpose of matrix
- Find sum of diagonal elements

### Exercise 6: Statistical Analysis

Write a program that:

- Reads test scores for N students
- Calculates mean, median, mode
- Finds standard deviation
- Creates frequency distribution
- Displays histogram

### Exercise 7: Word Processing

Create a program that:

- Reads multiple words into array
- Sorts words alphabetically
- Finds longest/shortest word
- Counts total characters
- Searches for specific word

### Exercise 8: Game Development

Implement:

- Snake game board (2D array)
- Minesweeper board
- Connect Four game
- Sudoku solver
- Magic square generator

---

## Summary and Best Practices

### Key Takeaways

1. **Arrays** store multiple values of same type
2. **Zero-indexed** - first element at index 0
3. **Fixed size** - determined at compile time
4. **Pass by reference** - array modifications persist
5. **Bounds checking** - programmer's responsibility
6. **Multi-dimensional** - arrays of arrays

### Array Design Principles

**Good array usage**:

- **Use constants** for array sizes
- **Initialize arrays** before use
- **Check bounds** before accessing
- **Pass size** with array to functions
- **Use meaningful names** for clarity

### Best Practices Checklist

✅ **Declare size as constant** - Easy to modify  
✅ **Initialize before use** - Avoid garbage values  
✅ **Check array bounds** - Prevent crashes  
✅ **Use loops** for processing - Don't repeat code  
✅ **Pass size to functions** - Don't rely on sizeof  
✅ **Use range-based for** when possible (C++11)  
✅ **Validate input** for array indices  
✅ **Document array purpose** - Comment what it stores  
✅ **Consider vector** for dynamic size needs  
✅ **Test boundary conditions** - First, last, empty

### Quick Reference

**Declaration and initialization**:

```cpp
int arr[10];                    // Declare
int arr[5] = {1, 2, 3, 4, 5};  // Initialize
int arr[] = {1, 2, 3};         // Auto size
int arr[5] = {0};              // All zeros
```

**Accessing**:

```cpp
arr[0]      // First element
arr[4]      // Last element (if size=5)
arr[i]      // Element at index i
```

**Loops**:

```cpp
// Standard for
for (int i = 0; i < size; i++) {
    process(arr[i]);
}

// Range-based for (C++11)
for (int elem : arr) {
    process(elem);
}
```

**Functions**:

```cpp
void func(int arr[], int size);  // Pass array
int linearSearch(int arr[], int size, int target);
void bubbleSort(int arr[], int size);
```

**2D Arrays**:

```cpp
int matrix[3][4];  // 3 rows, 4 columns

for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
        process(matrix[i][j]);
    }
}
```

---

## Conclusion

Arrays are fundamental to programming. They allow you to:

- **Store collections** of related data
- **Process data efficiently** using loops
- **Organize information** logically
- **Build complex data structures** (stacks, queues, etc.)
- **Implement algorithms** (sorting, searching)

**Mastering arrays enables you to**:

- Work with multiple related values
- Process large datasets
- Implement efficient algorithms
- Build real-world applications
- Prepare for advanced data structures

**Next steps**:

1. Practice all the exercises
2. Implement various algorithms
3. Build array-based projects
4. Explore dynamic arrays (vectors) in C++ STL
5. Move on to **Pointers** for deeper understanding

With arrays mastered, you have a powerful tool for organizing and processing data!

**Happy Coding!** 🚀