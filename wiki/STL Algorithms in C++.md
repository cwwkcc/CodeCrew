## Introduction

You've mastered STL containers - powerful data structures that store your data. Now it's time to learn **STL Algorithms** - powerful operations that manipulate that data.

Instead of writing loops for sorting, searching, counting, or transforming data, STL provides optimized, tested algorithms that do the work for you.

**Real-World Analogy**:

- **Containers** = Kitchen storage (refrigerator, cabinets, pantry)
- **Algorithms** = Kitchen appliances (blender, mixer, oven)
- Together they make cooking (programming) efficient!

### Why Use STL Algorithms?

**Before STL Algorithms**:

```cpp
// Manual sorting - error-prone, time-consuming
for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
        if (arr[j] > arr[j + 1]) {
            swap(arr[j], arr[j + 1]);
        }
    }
}
```

**With STL Algorithms**:

```cpp
sort(arr.begin(), arr.end());  // One line, optimized!
```

### Benefits of STL Algorithms

✅ **Tested** - Used by millions of developers  
✅ **Optimized** - Highly efficient implementations  
✅ **Generic** - Work with any container  
✅ **Readable** - Intent is clear  
✅ **Maintainable** - Less code, fewer bugs  
✅ **Standardized** - Everyone knows them

---

## Required Header

```cpp
#include <algorithm>  // Most algorithms
#include <numeric>    // Numeric algorithms (accumulate, etc.)
using namespace std;
```

---

## Algorithm Categories

### 1. Non-Modifying Algorithms

Operations that don't change container contents:

- `find`, `count`, `search`, `equal`, `all_of`, `any_of`, `none_of`

### 2. Modifying Algorithms

Operations that change container contents:

- `copy`, `fill`, `replace`, `remove`, `unique`, `reverse`, `rotate`

### 3. Sorting Algorithms

Operations that order elements:

- `sort`, `stable_sort`, `partial_sort`, `nth_element`

### 4. Binary Search Algorithms

Operations on sorted ranges:

- `binary_search`, `lower_bound`, `upper_bound`, `equal_range`

### 5. Set Algorithms

Operations treating ranges as sets:

- `set_union`, `set_intersection`, `set_difference`

### 6. Heap Algorithms

Operations on heap data structures:

- `make_heap`, `push_heap`, `pop_heap`, `sort_heap`

### 7. Min/Max Algorithms

Finding extremes:

- `min`, `max`, `min_element`, `max_element`, `minmax_element`

### 8. Numeric Algorithms

Mathematical operations:

- `accumulate`, `inner_product`, `partial_sum`, `iota`

---

# 1. Sorting Algorithms

## sort() - Sort Elements

**Sorts elements in ascending order (by default).**

### Basic Usage

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    vector<int> numbers = {5, 2, 8, 1, 9, 3};
    
    sort(numbers.begin(), numbers.end());
    
    for (int num : numbers) {
        cout << num << " ";
    }
    // Output: 1 2 3 5 8 9
    
    return 0;
}
```

### Sort in Descending Order

```cpp
vector<int> numbers = {5, 2, 8, 1, 9, 3};

sort(numbers.begin(), numbers.end(), greater<int>());

// Output: 9 8 5 3 2 1
```

### Custom Comparison Function

```cpp
struct Student {
    string name;
    double gpa;
};

bool compareByGPA(const Student& a, const Student& b) {
    return a.gpa > b.gpa;  // Sort by GPA descending
}

int main() {
    vector<Student> students = {
        {"Alice", 3.8},
        {"Bob", 3.5},
        {"Charlie", 3.9}
    };
    
    sort(students.begin(), students.end(), compareByGPA);
    
    for (const auto& s : students) {
        cout << s.name << ": " << s.gpa << endl;
    }
    // Output:
    // Charlie: 3.9
    // Alice: 3.8
    // Bob: 3.5
    
    return 0;
}
```

### Sort with Lambda (C++11)

```cpp
vector<Student> students = {
    {"Alice", 3.8},
    {"Bob", 3.5},
    {"Charlie", 3.9}
};

// Sort by GPA descending using lambda
sort(students.begin(), students.end(),
     [](const Student& a, const Student& b) {
         return a.gpa > b.gpa;
     });
```

## stable_sort() - Stable Sorting

**Maintains relative order of equal elements.**

```cpp
vector<Student> students = {
    {"Alice", 3.5},
    {"Bob", 3.8},
    {"Charlie", 3.5},
    {"Diana", 3.8}
};

// Stable sort by GPA
stable_sort(students.begin(), students.end(),
            [](const Student& a, const Student& b) {
                return a.gpa > b.gpa;
            });

// Alice and Charlie keep their relative order (both 3.5)
// Bob and Diana keep their relative order (both 3.8)
```

## partial_sort() - Partial Sorting

**Sorts only the first N elements.**

```cpp
vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6};

// Sort only first 3 elements
partial_sort(numbers.begin(), numbers.begin() + 3, numbers.end());

// Output: 1 2 3 8 9 5 7 4 6
// First 3 are sorted, rest in undefined order
```

## nth_element() - Nth Element

**Partitions so that nth element is in correct sorted position.**

```cpp
vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6};

// Find median (5th element if sorted)
nth_element(numbers.begin(), numbers.begin() + 4, numbers.end());

cout << "Median: " << numbers[4] << endl;  // 5
// Elements before median are smaller, after are larger
```

---

# 2. Searching Algorithms

## find() - Linear Search

**Finds first occurrence of value.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 3, 6};

auto it = find(numbers.begin(), numbers.end(), 3);

if (it != numbers.end()) {
    cout << "Found at position: " << (it - numbers.begin()) << endl;  // 2
} else {
    cout << "Not found" << endl;
}
```

## find_if() - Find with Condition

**Finds first element satisfying condition.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 6};

// Find first even number
auto it = find_if(numbers.begin(), numbers.end(),
                  [](int n) { return n % 2 == 0; });

if (it != numbers.end()) {
    cout << "First even: " << *it << endl;  // 2
}
```

## binary_search() - Binary Search

**Checks if value exists in sorted range (returns bool).**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};

if (binary_search(numbers.begin(), numbers.end(), 5)) {
    cout << "5 found!" << endl;
}

if (!binary_search(numbers.begin(), numbers.end(), 10)) {
    cout << "10 not found!" << endl;
}
```

## lower_bound() - First Not Less

**Finds first element not less than value.**

```cpp
vector<int> numbers = {1, 2, 4, 4, 4, 6, 7, 8};

auto it = lower_bound(numbers.begin(), numbers.end(), 4);

cout << "Position: " << (it - numbers.begin()) << endl;  // 2
cout << "Value: " << *it << endl;  // 4 (first occurrence)
```

## upper_bound() - First Greater

**Finds first element greater than value.**

```cpp
vector<int> numbers = {1, 2, 4, 4, 4, 6, 7, 8};

auto it = upper_bound(numbers.begin(), numbers.end(), 4);

cout << "Position: " << (it - numbers.begin()) << endl;  // 5
cout << "Value: " << *it << endl;  // 6
```

---

# 3. Counting and Accumulating

## count() - Count Occurrences

**Counts how many times a value appears.**

```cpp
vector<int> numbers = {1, 2, 3, 2, 4, 2, 5};

int count = count(numbers.begin(), numbers.end(), 2);

cout << "2 appears " << count << " times" << endl;  // 3
```

## count_if() - Count with Condition

**Counts elements satisfying condition.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};

// Count even numbers
int evenCount = count_if(numbers.begin(), numbers.end(),
                         [](int n) { return n % 2 == 0; });

cout << "Even numbers: " << evenCount << endl;  // 4
```

## accumulate() - Sum/Reduce

**Accumulates values (requires `<numeric>`).**

```cpp
#include <numeric>

vector<int> numbers = {1, 2, 3, 4, 5};

// Sum all elements
int sum = accumulate(numbers.begin(), numbers.end(), 0);
cout << "Sum: " << sum << endl;  // 15

// Product of all elements
int product = accumulate(numbers.begin(), numbers.end(), 1,
                        [](int a, int b) { return a * b; });
cout << "Product: " << product << endl;  // 120
```

---

# 4. Modifying Algorithms

## fill() - Fill with Value

**Fills range with specified value.**

```cpp
vector<int> numbers(5);

fill(numbers.begin(), numbers.end(), 7);

// numbers = {7, 7, 7, 7, 7}
```

## generate() - Generate Values

**Fills range using generator function.**

```cpp
vector<int> numbers(5);

int value = 1;
generate(numbers.begin(), numbers.end(),
         [&value]() { return value++; });

// numbers = {1, 2, 3, 4, 5}
```

## replace() - Replace Values

**Replaces all occurrences of a value.**

```cpp
vector<int> numbers = {1, 2, 3, 2, 4, 2, 5};

replace(numbers.begin(), numbers.end(), 2, 99);

// numbers = {1, 99, 3, 99, 4, 99, 5}
```

## replace_if() - Replace with Condition

**Replaces elements satisfying condition.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 6};

// Replace even numbers with 0
replace_if(numbers.begin(), numbers.end(),
           [](int n) { return n % 2 == 0; }, 0);

// numbers = {1, 0, 3, 0, 5, 0}
```

## remove() - Remove Values

**"Removes" elements (moves to end, returns new end iterator).**

```cpp
vector<int> numbers = {1, 2, 3, 2, 4, 2, 5};

auto newEnd = remove(numbers.begin(), numbers.end(), 2);

// Actually erase removed elements
numbers.erase(newEnd, numbers.end());

// numbers = {1, 3, 4, 5}
```

## remove_if() - Remove with Condition

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8};

// Remove even numbers
auto newEnd = remove_if(numbers.begin(), numbers.end(),
                        [](int n) { return n % 2 == 0; });

numbers.erase(newEnd, numbers.end());

// numbers = {1, 3, 5, 7}
```

## unique() - Remove Consecutive Duplicates

**Removes consecutive duplicate elements.**

```cpp
vector<int> numbers = {1, 1, 2, 2, 2, 3, 3, 4, 5, 5};

auto newEnd = unique(numbers.begin(), numbers.end());
numbers.erase(newEnd, numbers.end());

// numbers = {1, 2, 3, 4, 5}
```

---

# 5. Copying and Transforming

## copy() - Copy Elements

**Copies elements from one range to another.**

```cpp
vector<int> source = {1, 2, 3, 4, 5};
vector<int> destination(5);

copy(source.begin(), source.end(), destination.begin());

// destination = {1, 2, 3, 4, 5}
```

## copy_if() - Copy with Condition

**Copies only elements satisfying condition.**

```cpp
vector<int> source = {1, 2, 3, 4, 5, 6, 7, 8};
vector<int> destination;

// Copy even numbers
copy_if(source.begin(), source.end(), back_inserter(destination),
        [](int n) { return n % 2 == 0; });

// destination = {2, 4, 6, 8}
```

## transform() - Transform Elements

**Applies function to each element.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};
vector<int> squared(5);

transform(numbers.begin(), numbers.end(), squared.begin(),
          [](int n) { return n * n; });

// squared = {1, 4, 9, 16, 25}
```

**In-place transformation**:

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};

transform(numbers.begin(), numbers.end(), numbers.begin(),
          [](int n) { return n * 2; });

// numbers = {2, 4, 6, 8, 10}
```

---

# 6. Reversing and Rotating

## reverse() - Reverse Elements

**Reverses order of elements.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};

reverse(numbers.begin(), numbers.end());

// numbers = {5, 4, 3, 2, 1}
```

## rotate() - Rotate Elements

**Rotates elements around a point.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5};

// Rotate left by 2 positions
rotate(numbers.begin(), numbers.begin() + 2, numbers.end());

// numbers = {3, 4, 5, 1, 2}
```

---

# 7. Permutations and Partitioning

## next_permutation() - Next Permutation

**Generates next lexicographic permutation.**

```cpp
vector<int> numbers = {1, 2, 3};

cout << "All permutations:" << endl;
do {
    for (int n : numbers) {
        cout << n << " ";
    }
    cout << endl;
} while (next_permutation(numbers.begin(), numbers.end()));

// Output:
// 1 2 3
// 1 3 2
// 2 1 3
// 2 3 1
// 3 1 2
// 3 2 1
```

## partition() - Partition Elements

**Partitions elements based on condition.**

```cpp
vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8};

// Partition: even numbers first, odd second
partition(numbers.begin(), numbers.end(),
          [](int n) { return n % 2 == 0; });

// Possible result: {2, 4, 6, 8, 1, 3, 5, 7}
```

---

# 8. Min/Max Algorithms

## min() and max()

**Find minimum and maximum of two values.**

```cpp
int a = 5, b = 10;

cout << "Min: " << min(a, b) << endl;  // 5
cout << "Max: " << max(a, b) << endl;  // 10

// With custom comparison
string s1 = "apple", s2 = "banana";
cout << min(s1, s2) << endl;  // apple (alphabetically)
```

## min_element() and max_element()

**Find minimum and maximum in range.**

```cpp
vector<int> numbers = {3, 1, 4, 1, 5, 9, 2, 6};

auto minIt = min_element(numbers.begin(), numbers.end());
auto maxIt = max_element(numbers.begin(), numbers.end());

cout << "Min: " << *minIt << endl;  // 1
cout << "Max: " << *maxIt << endl;  // 9

cout << "Min position: " << (minIt - numbers.begin()) << endl;
cout << "Max position: " << (maxIt - numbers.begin()) << endl;
```

## minmax_element() - Both at Once

**Finds both min and max in one pass.**

```cpp
vector<int> numbers = {3, 1, 4, 1, 5, 9, 2, 6};

auto [minIt, maxIt] = minmax_element(numbers.begin(), numbers.end());

cout << "Min: " << *minIt << endl;  // 1
cout << "Max: " << *maxIt << endl;  // 9
```

---

# 9. Checking Conditions

## all_of() - All Match

**Checks if all elements satisfy condition.**

```cpp
vector<int> numbers = {2, 4, 6, 8, 10};

// Check if all are even
if (all_of(numbers.begin(), numbers.end(),
           [](int n) { return n % 2 == 0; })) {
    cout << "All are even" << endl;
}
```

## any_of() - Any Match

**Checks if any element satisfies condition.**

```cpp
vector<int> numbers = {1, 3, 5, 7, 8};

// Check if any are even
if (any_of(numbers.begin(), numbers.end(),
           [](int n) { return n % 2 == 0; })) {
    cout << "At least one is even" << endl;
}
```

## none_of() - None Match

**Checks if no elements satisfy condition.**

```cpp
vector<int> numbers = {1, 3, 5, 7, 9};

// Check if none are even
if (none_of(numbers.begin(), numbers.end(),
            [](int n) { return n % 2 == 0; })) {
    cout << "None are even" << endl;
}
```

---

# 10. Set Operations

## set_union() - Union of Sets

**Computes union of two sorted ranges.**

```cpp
vector<int> set1 = {1, 2, 3, 4, 5};
vector<int> set2 = {3, 4, 5, 6, 7};
vector<int> result;

set_union(set1.begin(), set1.end(),
          set2.begin(), set2.end(),
          back_inserter(result));

// result = {1, 2, 3, 4, 5, 6, 7}
```

## set_intersection() - Intersection

**Computes intersection of two sorted ranges.**

```cpp
vector<int> set1 = {1, 2, 3, 4, 5};
vector<int> set2 = {3, 4, 5, 6, 7};
vector<int> result;

set_intersection(set1.begin(), set1.end(),
                 set2.begin(), set2.end(),
                 back_inserter(result));

// result = {3, 4, 5}
```

## set_difference() - Difference

**Computes elements in first but not second.**

```cpp
vector<int> set1 = {1, 2, 3, 4, 5};
vector<int> set2 = {3, 4, 5, 6, 7};
vector<int> result;

set_difference(set1.begin(), set1.end(),
               set2.begin(), set2.end(),
               back_inserter(result));

// result = {1, 2}
```

---

## Practical Examples and Projects

### Project 1: Grade Analyzer

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
using namespace std;

class GradeAnalyzer {
private:
    vector<double> grades;

public:
    void addGrade(double grade) {
        grades.push_back(grade);
    }
    
    void displayStatistics() {
        if (grades.empty()) {
            cout << "No grades entered!" << endl;
            return;
        }
        
        // Sort grades
        vector<double> sorted = grades;
        sort(sorted.begin(), sorted.end());
        
        // Calculate statistics
        double sum = accumulate(grades.begin(), grades.end(), 0.0);
        double average = sum / grades.size();
        
        double highest = *max_element(grades.begin(), grades.end());
        double lowest = *min_element(grades.begin(), grades.end());
        
        // Median
        double median;
        if (sorted.size() % 2 == 0) {
            median = (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0;
        } else {
            median = sorted[sorted.size()/2];
        }
        
        // Count passing (>= 60)
        int passing = count_if(grades.begin(), grades.end(),
                              [](double g) { return g >= 60; });
        
        // Display
        cout << fixed << setprecision(2);
        cout << "\n=== Grade Statistics ===" << endl;
        cout << "Total Grades: " << grades.size() << endl;
        cout << "Average: " << average << endl;
        cout << "Median: " << median << endl;
        cout << "Highest: " << highest << endl;
        cout << "Lowest: " << lowest << endl;
        cout << "Passing: " << passing << " (" 
             << (passing * 100.0 / grades.size()) << "%)" << endl;
        
        // Grade distribution
        cout << "\n=== Grade Distribution ===" << endl;
        cout << "A (90-100): " << count_if(grades.begin(), grades.end(),
                                          [](double g) { return g >= 90; }) << endl;
        cout << "B (80-89): " << count_if(grades.begin(), grades.end(),
                                         [](double g) { return g >= 80 && g < 90; }) << endl;
        cout << "C (70-79): " << count_if(grades.begin(), grades.end(),
                                         [](double g) { return g >= 70 && g < 80; }) << endl;
        cout << "D (60-69): " << count_if(grades.begin(), grades.end(),
                                         [](double g) { return g >= 60 && g < 70; }) << endl;
        cout << "F (<60): " << count_if(grades.begin(), grades.end(),
                                       [](double g) { return g < 60; }) << endl;
    }
    
    void displaySortedGrades() {
        vector<double> sorted = grades;
        sort(sorted.begin(), sorted.end(), greater<double>());
        
        cout << "\n=== Sorted Grades (Descending) ===" << endl;
        for (double grade : sorted) {
            cout << grade << " ";
        }
        cout << endl;
    }
};

int main() {
    GradeAnalyzer analyzer;
    
    // Add sample grades
    analyzer.addGrade(85.5);
    analyzer.addGrade(92.0);
    analyzer.addGrade(78.5);
    analyzer.addGrade(95.0);
    analyzer.addGrade(88.0);
    analyzer.addGrade(72.5);
    analyzer.addGrade(91.0);
    analyzer.addGrade(55.0);
    
    analyzer.displayStatistics();
    analyzer.displaySortedGrades();
    
    return 0;
}
```

### Project 2: Text Processor

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

class TextProcessor {
private:
    string text;

public:
    void setText(string t) {
        text = t;
    }
    
    void toUpperCase() {
        transform(text.begin(), text.end(), text.begin(), ::toupper);
        cout << "Uppercase: " << text << endl;
    }
    
    void toLowerCase() {
        transform(text.begin(), text.end(), text.begin(), ::tolower);
        cout << "Lowercase: " << text << endl;
    }
    
    void reverseText() {
        reverse(text.begin(), text.end());
        cout << "Reversed: " << text << endl;
    }
    
    void removeDuplicates() {
        sort(text.begin(), text.end());
        auto newEnd = unique(text.begin(), text.end());
        text.erase(newEnd, text.end());
        cout << "Unique characters: " << text << endl;
    }
    
    void countVowels() {
        int count = count_if(text.begin(), text.end(),
                            [](char c) {
                                c = tolower(c);
                                return c == 'a' || c == 'e' || c == 'i' ||
                                       c == 'o' || c == 'u';
                            });
        cout << "Vowels: " << count << endl;
    }
    
    void countDigits() {
        int count = count_if(text.begin(), text.end(), ::isdigit);
        cout << "Digits: " << count << endl;
    }
    
    void removeSpaces() {
        auto newEnd = remove(text.begin(), text.end(), ' ');
        text.erase(newEnd, text.end());
        cout << "Without spaces: " << text << endl;
    }
    
    void isPalindrome() {
        string temp = text;
        // Remove spaces and convert to lowercase
        auto newEnd = remove(temp.begin(), temp.end(), ' ');
        temp.erase(newEnd, temp.end());
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        
        string reversed = temp;
        reverse(reversed.begin(), reversed.end());
        
        if (temp == reversed) {
            cout << "\"" << text << "\" is a palindrome!" << endl;
        } else {
            cout << "\"" << text << "\" is NOT a palindrome." << endl;
        }
    }
};

int main() {
    TextProcessor processor;
    string input;
    
    cout << "Enter text: ";
    getline(cin, input);
    
    processor.setText(input);
    
    cout << "\n=== Text Analysis ===" << endl;
    processor.countVowels();
    processor.countDigits();
    processor.isPalindrome();
    
    cout << "\n=== Transformations ===" << endl;
    processor.toUpperCase();
    processor.toLowerCase();
    processor.reverseText();
    processor.removeSpaces();
    
    return 0;
}
```

### Project 3: Data Filter and Transformer

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

class DataProcessor {
private:
    vector<int> data;

public:
    void setData(vector<int> d) {
        data = d;
    }
    
    void displayData(string label = "Data") {
        cout << label << ": ";
        for (int n : data) {
            cout << n << " ";
        }
        cout << endl;
    }
    
    void sortData() {
        sort(data.begin(), data.end());
        displayData("Sorted");
    }
    
    void filterEven() {
        vector<int> evens;
        copy_if(data.begin(), data.end(), back_inserter(evens),
                [](int n) { return n % 2 == 0; });
        
        cout << "Even numbers: ";
        for (int n : evens) {
            cout << n << " ";
        }
        cout << endl;
    }
    
    void filterOdd() {
        vector<int> odds;
        copy_if(data.begin(), data.end(), back_inserter(odds),
                [](int n) { return n % 2 != 0; });
        
        cout << "Odd numbers: ";
        for (int n : odds) {
            cout << n << " ";
        }
        cout << endl;
    }
    
    void square() {
        transform(data.begin(), data.end(), data.begin(),
                 [](int n) { return n * n; });
        displayData("Squared");
    }
    
    void removeNegatives() {
        auto newEnd = remove_if(data.begin(), data.end(),
                               [](int n) { return n < 0; });
        data.erase(newEnd, data.end());
        displayData("Without negatives");
    }
    
    void statistics() {
        if (data.empty()) return;
        
        int sum = accumulate(data.begin(), data.end(), 0);
        int product = accumulate(data.begin(), data.end(), 1,
                                [](int a, int b) { return a * b; });
        
        auto [minIt, maxIt] = minmax_element(data.begin(), data.end());
        
        cout << "\n=== Statistics ===" << endl;
        cout << "Sum: " << sum << endl;
        cout << "Product: " << product << endl;
        cout << "Average: " << (double)sum / data.size() << endl;
        cout << "Min: " << *minIt << endl;
        cout << "Max: " << *maxIt << endl;
    }
};

int main() {
    DataProcessor processor;
    
    vector<int> data = {5, -2, 8, 1, -9, 3, 7, -4, 6, 10};
    
    processor.setData(data);
    processor.displayData("Original");
    
    processor.statistics();
    processor.filterEven();
    processor.filterOdd();
    processor.removeNegatives();
    processor.sortData();
    processor.square();
    
    return 0;
}
```

---

## Algorithm Complexity Reference

|Algorithm|Time Complexity|Notes|
|---|---|---|
|**sort**|O(n log n)|Average case|
|**stable_sort**|O(n log n)|Maintains order|
|**partial_sort**|O(n log k)|k = elements to sort|
|**find**|O(n)|Linear search|
|**binary_search**|O(log n)|Requires sorted range|
|**count**|O(n)|Linear scan|
|**accumulate**|O(n)|Linear scan|
|**reverse**|O(n)|Linear|
|**unique**|O(n)|Adjacent duplicates|
|**transform**|O(n)|Applies function|
|**copy**|O(n)|Linear copy|
|**fill**|O(n)|Linear fill|
|**min_element**|O(n)|Linear scan|
|**max_element**|O(n)|Linear scan|

---

## Best Practices

### ✅ DO:

1. **Use algorithms instead of manual loops**
    
    ```cpp
    // Good
    sort(v.begin(), v.end());
    
    // Avoid
    // Manual bubble sort...
    ```
    
2. **Use lambdas for simple conditions**
    
    ```cpp
    auto it = find_if(v.begin(), v.end(),
                      [](int n) { return n > 10; });
    ```
    
3. **Use back_inserter for dynamic insertion**
    
    ```cpp
    copy_if(source.begin(), source.end(), back_inserter(dest),
            [](int n) { return n % 2 == 0; });
    ```
    
4. **Chain algorithms when appropriate**
    
    ```cpp
    sort(v.begin(), v.end());
    auto newEnd = unique(v.begin(), v.end());
    v.erase(newEnd, v.end());
    ```
    
5. **Use auto for iterators**
    
    ```cpp
    auto it = find(v.begin(), v.end(), 42);
    ```
    

### ❌ DON'T:

1. **Don't forget to erase after remove**
    
    ```cpp
    // Wrong
    remove(v.begin(), v.end(), 5);
    
    // Correct
    v.erase(remove(v.begin(), v.end(), 5), v.end());
    ```
    
2. **Don't use algorithms on unsorted ranges when sorted required**
    
    ```cpp
    // Wrong - binary_search requires sorted range
    binary_search(unsorted.begin(), unsorted.end(), 5);
    ```
    
3. **Don't modify container while iterating**
    
    ```cpp
    // Dangerous
    for (auto it = v.begin(); it != v.end(); ++it) {
        v.push_back(*it);  // Invalidates iterators!
    }
    ```
    

---

## Quick Reference Guide

### Sorting

```cpp
sort(v.begin(), v.end());                    // Ascending
sort(v.begin(), v.end(), greater<int>());    // Descending
sort(v.begin(), v.end(), custom_compare);    // Custom
```

### Searching

```cpp
find(v.begin(), v.end(), value);             // Linear search
binary_search(v.begin(), v.end(), value);    // Binary search
find_if(v.begin(), v.end(), predicate);      // Conditional
```

### Counting

```cpp
count(v.begin(), v.end(), value);            // Count value
count_if(v.begin(), v.end(), predicate);     // Count matching
```

### Min/Max

```cpp
min_element(v.begin(), v.end());             // Iterator to min
max_element(v.begin(), v.end());             // Iterator to max
minmax_element(v.begin(), v.end());          // Both
```

### Transforming

```cpp
transform(v.begin(), v.end(), v.begin(), func);  // In-place
reverse(v.begin(), v.end());                     // Reverse
replace(v.begin(), v.end(), old, new);           // Replace
```

### Removing

```cpp
v.erase(remove(v.begin(), v.end(), value), v.end());      // Remove value
v.erase(remove_if(v.begin(), v.end(), pred), v.end());    // Remove matching
v.erase(unique(v.begin(), v.end()), v.end());             // Remove duplicates
```

---

## Summary

### Key Takeaways

1. **STL algorithms** - Pre-built, tested, optimized operations
2. **Generic** - Work with any container supporting iterators
3. **Composable** - Chain algorithms for complex operations
4. **Efficient** - Optimized implementations
5. **Readable** - Intent is clear
6. **Maintainable** - Less code, fewer bugs

### What You Can Do Now

✅ **Sort** data with one line  
✅ **Search** efficiently in collections  
✅ **Transform** data without manual loops  
✅ **Count** and accumulate with ease  
✅ **Filter** data based on conditions  
✅ **Manipulate** collections professionally  
✅ **Write** cleaner, more expressive code

---

## Conclusion

**STL Algorithms** complete the STL toolkit. Combined with containers, they provide:

- **Power** - Complex operations made simple
- **Efficiency** - Optimized implementations
- **Expressiveness** - Clear intent
- **Professionalism** - Industry-standard code
- **Productivity** - Write less, accomplish more

**You've now mastered the complete STL foundation!**

**Happy Coding!** 🚀