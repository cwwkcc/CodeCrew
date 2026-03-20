## Introduction

Text is everywhere in programming - usernames, passwords, messages, emails, documents, and more. **Strings** are sequences of characters used to represent text data.

Imagine trying to store someone's name using individual variables: `char1 = 'J'`, `char2 = 'o'`, `char3 = 'h'`, `char4 = 'n'`. Terrible, right? Strings let you work with text as a single, manageable unit.

**Real-World Analogies**:

- **Sentence**: A string of words forming a complete thought
- **Book Title**: A string of characters representing a book's name
- **Phone Number**: A string of digits (stored as text, not a number)
- **Email Address**: A string with specific format rules
- **Social Media Post**: A string that can be short or very long

---

## Why Strings Matter

### Without Strings

```cpp
// Storing "Hello" - the hard way
char c1 = 'H';
char c2 = 'e';
char c3 = 'l';
char c4 = 'l';
char c5 = 'o';

// Changing it to "Hello World" - nightmare!
```

### With Strings

```cpp
// Storing "Hello" - the easy way
string message = "Hello";

// Changing it to "Hello World" - simple!
message = "Hello World";

// Combining strings
string greeting = message + "!";  // "Hello World!"
```

### Benefits of Strings

1. **Easy manipulation** - Concatenate, search, replace, extract
2. **Intuitive operations** - Works like you'd expect
3. **Built-in functions** - Length, comparison, substring, and more
4. **Dynamic size** - Grows and shrinks as needed
5. **Safe and convenient** - Easier than character arrays
6. **Rich functionality** - Extensive string processing capabilities

---

## Two Types of Strings in C++

C++ has two ways to work with strings:

### 1. C-Style Strings (Character Arrays)

The old way - inherited from C language.

```cpp
char name[50] = "John";  // Array of characters with null terminator '\0'
```

**Characteristics**:

- Fixed size (declared in advance)
- Must include null terminator `\0`
- Manual memory management
- Use functions from `<cstring>` library
- More complex to work with

### 2. C++ String Class (std::string)

The modern way - recommended for C++.

```cpp
#include <string>
string name = "John";  // No size limit, no null terminator needed
```

**Characteristics**:

- Dynamic size (grows automatically)
- No null terminator needed
- Automatic memory management
- Use methods from `<string>` library
- Much easier to work with
- **We'll focus on this!**

---

## C++ String Basics

### Including the String Library

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string message = "Hello, World!";
    cout << message << endl;
    return 0;
}
```

### String Declaration and Initialization

**Method 1: Direct initialization**

```cpp
string name = "Alice";
string city = "New York";
string message = "Hello, World!";
```

**Method 2: Constructor initialization**

```cpp
string name("Alice");
string city("New York");
```

**Method 3: Empty string**

```cpp
string empty1;           // Empty string
string empty2 = "";      // Empty string
string empty3("");       // Empty string
```

**Method 4: Repeated characters**

```cpp
string stars(10, '*');   // "**********" (10 asterisks)
string line(50, '-');    // 50 dashes
```

**Method 5: Copy from another string**

```cpp
string original = "Hello";
string copy = original;  // "Hello"
```

### String Input and Output

**Output (cout)**:

```cpp
string message = "Hello, World!";
cout << message << endl;  // Simple and straightforward
```

**Input - single word (cin)**:

```cpp
string name;
cout << "Enter your name: ";
cin >> name;  // Reads until whitespace
cout << "Hello, " << name << endl;

// Input: John Doe
// Stores: "John" (stops at space!)
```

**Input - full line with spaces (getline)**:

```cpp
string fullName;
cout << "Enter your full name: ";
getline(cin, fullName);  // Reads entire line
cout << "Hello, " << fullName << endl;

// Input: John Doe
// Stores: "John Doe" (includes space!)
```

**Mixing cin and getline (important!)**:

```cpp
int age;
string name;

cout << "Enter age: ";
cin >> age;

cin.ignore();  // ⚠️ IMPORTANT! Clear the newline left by cin

cout << "Enter name: ";
getline(cin, name);
```

---

## String Operations

### Concatenation (Combining Strings)

**Using + operator**:

```cpp
string first = "Hello";
string second = " World";
string combined = first + second;  // "Hello World"

cout << combined << endl;
```

**Combining multiple strings**:

```cpp
string greeting = "Hello";
string name = "Alice";
string punctuation = "!";

string message = greeting + " " + name + punctuation;  // "Hello Alice!"
```

**Using += operator**:

```cpp
string message = "Hello";
message += " ";        // "Hello "
message += "World";    // "Hello World"
message += "!";        // "Hello World!"
```

**Appending strings**:

```cpp
string text = "Hello";
text.append(" World");  // "Hello World"
text.append("!!!");     // "Hello World!!!"
```

### String Length

**Get length**:

```cpp
string message = "Hello, World!";

int len1 = message.length();  // 13
int len2 = message.size();    // 13 (same as length())

cout << "Length: " << len1 << endl;
```

**Check if empty**:

```cpp
string str1 = "";
string str2 = "Hello";

if (str1.empty()) {
    cout << "String is empty" << endl;
}

if (!str2.empty()) {
    cout << "String is not empty" << endl;
}
```

### Accessing Characters

**Using [] operator (no bounds checking)**:

```cpp
string word = "Hello";

cout << word[0] << endl;  // 'H' (first character)
cout << word[1] << endl;  // 'e'
cout << word[4] << endl;  // 'o' (last character)

// Modify character
word[0] = 'J';  // Changes "Hello" to "Jello"
```

**Using at() method (with bounds checking)**:

```cpp
string word = "Hello";

cout << word.at(0) << endl;  // 'H'
cout << word.at(4) << endl;  // 'o'

word.at(0) = 'J';  // Changes to "Jello"

// word.at(10) would throw an exception (safer!)
```

**Get first and last character**:

```cpp
string word = "Hello";

char first = word.front();  // 'H'
char last = word.back();    // 'o'
```

### String Comparison

**Using comparison operators**:

```cpp
string str1 = "Hello";
string str2 = "Hello";
string str3 = "World";

// Equal
if (str1 == str2) {
    cout << "Strings are equal" << endl;
}

// Not equal
if (str1 != str3) {
    cout << "Strings are different" << endl;
}

// Lexicographic comparison (dictionary order)
if (str1 < str3) {
    cout << "Hello comes before World" << endl;  // True
}

if (str3 > str1) {
    cout << "World comes after Hello" << endl;   // True
}
```

**Using compare() method**:

```cpp
string str1 = "Hello";
string str2 = "World";

int result = str1.compare(str2);

if (result == 0) {
    cout << "Strings are equal" << endl;
} else if (result < 0) {
    cout << "str1 comes before str2" << endl;
} else {
    cout << "str1 comes after str2" << endl;
}
```

**Case-insensitive comparison**:

```cpp
#include <algorithm>

bool caseInsensitiveCompare(string str1, string str2) {
    // Convert both to lowercase
    transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    
    return str1 == str2;
}

// Usage
if (caseInsensitiveCompare("Hello", "HELLO")) {
    cout << "Same (ignoring case)" << endl;
}
```

---

## String Methods

### Substring

**Extract part of a string**:

```cpp
string text = "Hello, World!";

// substr(start_position, length)
string part1 = text.substr(0, 5);   // "Hello"
string part2 = text.substr(7, 5);   // "World"
string part3 = text.substr(7);      // "World!" (to end)

cout << part1 << endl;  // Hello
cout << part2 << endl;  // World
cout << part3 << endl;  // World!
```

### Find and Search

**Find substring**:

```cpp
string text = "Hello, World! Welcome to the World!";

// find() - returns position or string::npos if not found
size_t pos = text.find("World");

if (pos != string::npos) {
    cout << "Found at position: " << pos << endl;  // 7
} else {
    cout << "Not found" << endl;
}

// Find starting from position
size_t pos2 = text.find("World", 10);  // Start searching from position 10
cout << "Second occurrence at: " << pos2 << endl;  // 30
```

**Find last occurrence**:

```cpp
string text = "Hello, World! Welcome to the World!";

size_t lastPos = text.rfind("World");  // Search from end
cout << "Last occurrence at: " << lastPos << endl;  // 30
```

**Find character**:

```cpp
string text = "Hello, World!";

size_t pos = text.find('o');
cout << "First 'o' at position: " << pos << endl;  // 4

size_t lastO = text.rfind('o');
cout << "Last 'o' at position: " << lastO << endl;  // 8
```

**Find any of several characters**:

```cpp
string text = "Hello, World!";

// Find first vowel
size_t pos = text.find_first_of("aeiou");
cout << "First vowel at: " << pos << endl;  // 1 ('e')

// Find first consonant
size_t pos2 = text.find_first_not_of("aeiou ");
cout << "First consonant at: " << pos2 << endl;  // 0 ('H')
```

### Insert and Erase

**Insert**:

```cpp
string text = "Hello World";

// insert(position, string)
text.insert(5, ",");  // "Hello, World"
cout << text << endl;

text.insert(0, "Say: ");  // "Say: Hello, World"
cout << text << endl;
```

**Erase**:

```cpp
string text = "Hello, World!";

// erase(start_position, length)
text.erase(5, 2);  // Remove ", " → "HelloWorld!"
cout << text << endl;

// erase(position) - removes from position to end
string text2 = "Hello, World!";
text2.erase(5);  // "Hello"
cout << text2 << endl;
```

### Replace

**Replace part of string**:

```cpp
string text = "Hello, World!";

// replace(start_position, length, new_string)
text.replace(7, 5, "C++");  // "Hello, C++!"
cout << text << endl;
```

**Replace all occurrences**:

```cpp
string text = "Hello World! World is beautiful!";
string oldWord = "World";
string newWord = "Earth";

size_t pos = 0;
while ((pos = text.find(oldWord, pos)) != string::npos) {
    text.replace(pos, oldWord.length(), newWord);
    pos += newWord.length();
}

cout << text << endl;  // "Hello Earth! Earth is beautiful!"
```

### Clear

**Empty the string**:

```cpp
string text = "Hello, World!";
cout << "Before: " << text << endl;

text.clear();
cout << "After: " << text << endl;  // Empty
cout << "Length: " << text.length() << endl;  // 0
```

---

## String Transformation

### Convert to Uppercase

```cpp
#include <algorithm>
#include <string>

string text = "hello world";

// Convert to uppercase
transform(text.begin(), text.end(), text.begin(), ::toupper);

cout << text << endl;  // "HELLO WORLD"
```

**Using a function**:

```cpp
string toUpperCase(string str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = toupper(str[i]);
    }
    return str;
}

// Usage
string result = toUpperCase("hello");  // "HELLO"
```

### Convert to Lowercase

```cpp
#include <algorithm>

string text = "HELLO WORLD";

// Convert to lowercase
transform(text.begin(), text.end(), text.begin(), ::tolower);

cout << text << endl;  // "hello world"
```

**Using a function**:

```cpp
string toLowerCase(string str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

// Usage
string result = toLowerCase("HELLO");  // "hello"
```

### Reverse String

```cpp
#include <algorithm>

string text = "Hello";

reverse(text.begin(), text.end());

cout << text << endl;  // "olleH"
```

**Manual reversal**:

```cpp
string reverseString(string str) {
    string reversed = "";
    for (int i = str.length() - 1; i >= 0; i--) {
        reversed += str[i];
    }
    return reversed;
}

// Or swap characters
string reverseString2(string str) {
    int left = 0;
    int right = str.length() - 1;
    
    while (left < right) {
        swap(str[left], str[right]);
        left++;
        right--;
    }
    
    return str;
}
```

---

## String Conversion

### String to Number

```cpp
#include <string>

// String to integer
string numStr = "123";
int num = stoi(numStr);  // 123
cout << num + 10 << endl;  // 133

// String to long
string longStr = "1234567890";
long longNum = stol(longStr);

// String to long long
string llStr = "12345678901234";
long long llNum = stoll(llStr);

// String to float
string floatStr = "3.14";
float floatNum = stof(floatStr);  // 3.14

// String to double
string doubleStr = "3.14159";
double doubleNum = stod(doubleStr);  // 3.14159
```

**With error handling**:

```cpp
#include <iostream>
#include <string>
using namespace std;

int main() {
    string input;
    
    cout << "Enter a number: ";
    cin >> input;
    
    try {
        int number = stoi(input);
        cout << "You entered: " << number << endl;
    }
    catch (invalid_argument&) {
        cout << "Error: Not a valid number!" << endl;
    }
    catch (out_of_range&) {
        cout << "Error: Number too large!" << endl;
    }
    
    return 0;
}
```

### Number to String

```cpp
#include <string>

// Integer to string
int num = 123;
string str1 = to_string(num);  // "123"

// Double to string
double pi = 3.14159;
string str2 = to_string(pi);  // "3.141590"

// Long to string
long bigNum = 123456789;
string str3 = to_string(bigNum);  // "123456789"

cout << str1 << endl;  // 123
cout << str2 << endl;  // 3.141590
```

---

## Common String Problems

### Problem 1: Check if Palindrome

A palindrome reads the same forwards and backwards.

```cpp
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

bool isPalindrome(string str) {
    // Remove spaces and convert to lowercase
    string cleaned = "";
    for (char c : str) {
        if (c != ' ') {
            cleaned += tolower(c);
        }
    }
    
    // Compare with reverse
    string reversed = cleaned;
    reverse(reversed.begin(), reversed.end());
    
    return cleaned == reversed;
}

int main() {
    cout << isPalindrome("radar") << endl;        // 1 (true)
    cout << isPalindrome("hello") << endl;        // 0 (false)
    cout << isPalindrome("A man a plan a canal Panama") << endl;  // 1
    
    return 0;
}
```

**Alternative approach (two pointers)**:

```cpp
bool isPalindrome2(string str) {
    int left = 0;
    int right = str.length() - 1;
    
    while (left < right) {
        if (str[left] != str[right]) {
            return false;
        }
        left++;
        right--;
    }
    
    return true;
}
```

### Problem 2: Count Vowels and Consonants

```cpp
#include <iostream>
#include <string>
using namespace std;

void countVowelsConsonants(string str) {
    int vowels = 0;
    int consonants = 0;
    
    for (char c : str) {
        c = tolower(c);
        
        if (c >= 'a' && c <= 'z') {
            if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
                vowels++;
            } else {
                consonants++;
            }
        }
    }
    
    cout << "Vowels: " << vowels << endl;
    cout << "Consonants: " << consonants << endl;
}

int main() {
    countVowelsConsonants("Hello World");
    // Output:
    // Vowels: 3
    // Consonants: 7
    
    return 0;
}
```

### Problem 3: Count Words in String

```cpp
int countWords(string str) {
    int count = 0;
    bool inWord = false;
    
    for (char c : str) {
        if (c == ' ' || c == '\t' || c == '\n') {
            inWord = false;
        } else if (!inWord) {
            count++;
            inWord = true;
        }
    }
    
    return count;
}

// Usage
string text = "Hello World! How are you?";
cout << "Words: " << countWords(text) << endl;  // 5
```

### Problem 4: Remove Spaces

```cpp
string removeSpaces(string str) {
    string result = "";
    
    for (char c : str) {
        if (c != ' ') {
            result += c;
        }
    }
    
    return result;
}

// Usage
string text = "Hello World";
cout << removeSpaces(text) << endl;  // "HelloWorld"
```

**Using erase-remove idiom**:

```cpp
#include <algorithm>

string removeSpaces2(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}
```

### Problem 5: Reverse Words in String

```cpp
#include <sstream>
#include <vector>

string reverseWords(string str) {
    stringstream ss(str);
    vector<string> words;
    string word;
    
    // Extract words
    while (ss >> word) {
        words.push_back(word);
    }
    
    // Reverse words vector
    reverse(words.begin(), words.end());
    
    // Combine back
    string result = "";
    for (int i = 0; i < words.size(); i++) {
        result += words[i];
        if (i < words.size() - 1) {
            result += " ";
        }
    }
    
    return result;
}

// Usage
string text = "Hello World from C++";
cout << reverseWords(text) << endl;  // "C++ from World Hello"
```

### Problem 6: Check Anagram

Two strings are anagrams if they contain the same characters in different order.

```cpp
#include <algorithm>

bool isAnagram(string str1, string str2) {
    // Remove spaces and convert to lowercase
    str1.erase(remove(str1.begin(), str1.end(), ' '), str1.end());
    str2.erase(remove(str2.begin(), str2.end(), ' '), str2.end());
    
    transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    
    // Sort and compare
    sort(str1.begin(), str1.end());
    sort(str2.begin(), str2.end());
    
    return str1 == str2;
}

// Usage
cout << isAnagram("listen", "silent") << endl;  // 1 (true)
cout << isAnagram("hello", "world") << endl;    // 0 (false)
```

### Problem 7: Find Longest Word

```cpp
#include <sstream>

string findLongestWord(string str) {
    stringstream ss(str);
    string word, longest = "";
    
    while (ss >> word) {
        if (word.length() > longest.length()) {
            longest = word;
        }
    }
    
    return longest;
}

// Usage
string text = "The quick brown fox jumps";
cout << findLongestWord(text) << endl;  // "quick" or "brown" or "jumps"
```

---

## Practical Examples and Projects

### Example 1: Text Analysis Tool

```cpp
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

void analyzeText(string text) {
    int characters = 0;
    int letters = 0;
    int digits = 0;
    int spaces = 0;
    int special = 0;
    int vowels = 0;
    int consonants = 0;
    
    for (char c : text) {
        characters++;
        
        if (isalpha(c)) {
            letters++;
            char lower = tolower(c);
            if (lower == 'a' || lower == 'e' || lower == 'i' || 
                lower == 'o' || lower == 'u') {
                vowels++;
            } else {
                consonants++;
            }
        } else if (isdigit(c)) {
            digits++;
        } else if (isspace(c)) {
            spaces++;
        } else {
            special++;
        }
    }
    
    cout << "=== Text Analysis ===" << endl;
    cout << "Total characters: " << characters << endl;
    cout << "Letters: " << letters << endl;
    cout << "  Vowels: " << vowels << endl;
    cout << "  Consonants: " << consonants << endl;
    cout << "Digits: " << digits << endl;
    cout << "Spaces: " << spaces << endl;
    cout << "Special characters: " << special << endl;
}

int main() {
    string text;
    
    cout << "Enter text: ";
    getline(cin, text);
    
    analyzeText(text);
    
    return 0;
}
```

### Example 2: Password Validator

```cpp
#include <iostream>
#include <string>
using namespace std;

bool hasMinLength(string pwd, int minLen) {
    return pwd.length() >= minLen;
}

bool hasUpperCase(string pwd) {
    for (char c : pwd) {
        if (isupper(c)) return true;
    }
    return false;
}

bool hasLowerCase(string pwd) {
    for (char c : pwd) {
        if (islower(c)) return true;
    }
    return false;
}

bool hasDigit(string pwd) {
    for (char c : pwd) {
        if (isdigit(c)) return true;
    }
    return false;
}

bool hasSpecialChar(string pwd) {
    string special = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    
    for (char c : pwd) {
        if (special.find(c) != string::npos) {
            return true;
        }
    }
    return false;
}

int calculateStrength(string pwd) {
    int strength = 0;
    
    if (hasMinLength(pwd, 8)) strength += 20;
    if (hasMinLength(pwd, 12)) strength += 10;
    if (hasUpperCase(pwd)) strength += 20;
    if (hasLowerCase(pwd)) strength += 20;
    if (hasDigit(pwd)) strength += 15;
    if (hasSpecialChar(pwd)) strength += 15;
    
    return strength;
}

int main() {
    string password;
    
    do {
        cout << "\nCreate a password: ";
        cin >> password;
        
        cout << "\n--- Password Requirements ---" << endl;
        cout << (hasMinLength(password, 8) ? "✓" : "✗") 
             << " At least 8 characters" << endl;
        cout << (hasUpperCase(password) ? "✓" : "✗") 
             << " Contains uppercase letter" << endl;
        cout << (hasLowerCase(password) ? "✓" : "✗") 
             << " Contains lowercase letter" << endl;
        cout << (hasDigit(password) ? "✓" : "✗") 
             << " Contains digit" << endl;
        cout << (hasSpecialChar(password) ? "✓" : "✗") 
             << " Contains special character" << endl;
        
        int strength = calculateStrength(password);
        cout << "\nStrength: " << strength << "/100 ";
        
        if (strength >= 80) {
            cout << "(Strong)" << endl;
        } else if (strength >= 60) {
            cout << "(Medium)" << endl;
        } else {
            cout << "(Weak)" << endl;
        }
        
        if (strength < 80) {
            cout << "\nPassword not strong enough. Try again!" << endl;
        } else {
            cout << "\nPassword accepted!" << endl;
            break;
        }
        
    } while (true);
    
    return 0;
}
```

### Example 3: Simple Encryption/Decryption (Caesar Cipher)

```cpp
#include <iostream>
#include <string>
using namespace std;

string encrypt(string text, int shift) {
    string result = "";
    
    for (char c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            c = (c - base + shift) % 26 + base;
        }
        result += c;
    }
    
    return result;
}

string decrypt(string text, int shift) {
    return encrypt(text, 26 - shift);  // Reverse shift
}

int main() {
    string message;
    int shift;
    int choice;
    
    cout << "=== Caesar Cipher ===" << endl;
    cout << "1. Encrypt" << endl;
    cout << "2. Decrypt" << endl;
    cout << "Choice: ";
    cin >> choice;
    
    cin.ignore();
    cout << "Enter message: ";
    getline(cin, message);
    
    cout << "Enter shift (1-25): ";
    cin >> shift;
    
    string result;
    if (choice == 1) {
        result = encrypt(message, shift);
        cout << "\nEncrypted: " << result << endl;
    } else {
        result = decrypt(message, shift);
        cout << "\nDecrypted: " << result << endl;
    }
    
    return 0;
}
```

### Example 4: String Manipulation Menu

```cpp
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

void displayMenu();
string reverseString(string str);
string toUpperCase(string str);
string toLowerCase(string str);
int countVowels(string str);
bool isPalindrome(string str);

int main() {
    string text;
    int choice;
    
    cout << "Enter a string: ";
    getline(cin, text);
    
    do {
        displayMenu();
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Reversed: " << reverseString(text) << endl;
                break;
            
            case 2:
                cout << "Uppercase: " << toUpperCase(text) << endl;
                break;
            
            case 3:
                cout << "Lowercase: " << toLowerCase(text) << endl;
                break;
            
            case 4:
                cout << "Length: " << text.length() << endl;
                break;
            
            case 5:
                cout << "Vowels: " << countVowels(text) << endl;
                break;
            
            case 6:
                cout << "Palindrome: " << (isPalindrome(text) ? "Yes" : "No") << endl;
                break;
            
            case 7:
                cout << "Goodbye!" << endl;
                break;
            
            default:
                cout << "Invalid choice!" << endl;
        }
        
        cout << endl;
        
    } while (choice != 7);
    
    return 0;
}

void displayMenu() {
    cout << "\n=== String Operations ===" << endl;
    cout << "1. Reverse" << endl;
    cout << "2. Uppercase" << endl;
    cout << "3. Lowercase" << endl;
    cout << "4. Length" << endl;
    cout << "5. Count Vowels" << endl;
    cout << "6. Check Palindrome" << endl;
    cout << "7. Exit" << endl;
}

string reverseString(string str) {
    reverse(str.begin(), str.end());
    return str;
}

string toUpperCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

string toLowerCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

int countVowels(string str) {
    int count = 0;
    for (char c : str) {
        c = tolower(c);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

bool isPalindrome(string str) {
    string cleaned = "";
    for (char c : str) {
        if (isalpha(c)) {
            cleaned += tolower(c);
        }
    }
    
    string reversed = cleaned;
    reverse(reversed.begin(), reversed.end());
    
    return cleaned == reversed;
}
```

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Forgetting to Include <string>

**Wrong**:

```cpp
#include <iostream>
using namespace std;

int main() {
    string name = "John";  // Error! string not defined
    return 0;
}
```

**Correct**:

```cpp
#include <iostream>
#include <string>  // Don't forget this!
using namespace std;

int main() {
    string name = "John";  // Works!
    return 0;
}
```

### Mistake 2: Comparing Strings with strcpy

**Wrong**:

```cpp
string str1 = "Hello";
string str2 = "Hello";

if (strcmp(str1, str2) == 0) {  // Error! strcmp is for C-strings
    // ...
}
```

**Correct**:

```cpp
string str1 = "Hello";
string str2 = "Hello";

if (str1 == str2) {  // Simple comparison
    // ...
}
```

### Mistake 3: Index Out of Bounds

**Wrong**:

```cpp
string word = "Hello";
cout << word[10] << endl;  // Out of bounds! Undefined behavior
```

**Correct**:

```cpp
string word = "Hello";

// Use at() for bounds checking
try {
    cout << word.at(10) << endl;
}
catch (out_of_range& e) {
    cout << "Index out of range!" << endl;
}

// Or check bounds first
if (10 < word.length()) {
    cout << word[10] << endl;
}
```

### Mistake 4: Not Handling cin >> and getline

**Wrong**:

````cpp
int age;
string name;

cin >> age;
getline(cin, name);  // Skips! Gets empty string

### Correct**:
```cpp
int age;
string name;

cin >> age;
cin.ignore();  // Clear the newline
getline(cin, name);  // Now works correctly
````

### Mistake 5: Modifying String During Iteration

**Wrong**:

```cpp
string text = "Hello World";

for (int i = 0; i < text.length(); i++) {
    if (text[i] == ' ') {
        text.erase(i, 1);  // Changes length during loop!
    }
}
```

**Correct**:

```cpp
string text = "Hello World";
string result = "";

for (char c : text) {
    if (c != ' ') {
        result += c;
    }
}

text = result;
```

### Mistake 6: Concatenating with char

**Wrong**:

```cpp
string text = "Hello";
text = text + 'W';  // Might not work as expected
```

**Correct**:

```cpp
string text = "Hello";
text += 'W';  // Append character

// Or
text = text + string(1, 'W');
```

### Mistake 7: Assuming ASCII Order

**Wrong**:

```cpp
// Assuming 'Z' + 1 = 'A'
char c = 'Z';
c = c + 1;  // Results in '[', not 'A'!
```

**Correct**:

```cpp
char c = 'Z';
if (c == 'Z') {
    c = 'A';  // Wrap around manually
} else {
    c++;
}
```

---

## Practice Exercises

### Exercise 1: String Basics

Write programs that:

- Count characters, words, and sentences in a string
- Find and count specific character in string
- Check if string starts/ends with specific substring
- Remove leading/trailing spaces
- Split string into array of words

### Exercise 2: String Manipulation

Implement functions to:

- Reverse each word in a sentence
- Title case conversion (capitalize first letter of each word)
- Remove duplicate characters
- Insert string at specific position
- Extract all numbers from string

### Exercise 3: String Validation

Create validators for:

- Email address format
- Phone number format
- Credit card number (Luhn algorithm)
- URL format
- IPv4 address format

### Exercise 4: Pattern Matching

Implement:

- Count occurrences of substring
- Find all positions of substring
- Replace all occurrences
- Find longest common substring
- String pattern matching (simple wildcard)

### Exercise 5: Text Processing

Build programs for:

- Word frequency counter
- Text statistics (avg word length, etc.)
- Remove HTML tags from text
- Convert Roman numerals to integers
- Implement basic spell checker

### Exercise 6: Ciphers and Encoding

Create:

- ROT13 cipher
- Vigenère cipher
- Base64 encoding/decoding (simplified)
- Morse code translator
- Pig Latin translator

### Exercise 7: String Games

Implement:

- Hangman game
- Word scramble game
- Acronym generator
- Rhyme finder
- Crossword puzzle solver (basic)

### Exercise 8: Advanced Problems

Solve:

- Longest palindromic substring
- String compression (run-length encoding)
- Edit distance between two strings
- Generate all permutations of string
- Find longest substring without repeating characters

---

## Summary and Best Practices

### Key Takeaways

1. **C++ strings** are dynamic and easy to use
2. **Use string class** instead of C-style strings
3. **Many built-in methods** for manipulation
4. **Comparison operators** work intuitively
5. **Conversion functions** between strings and numbers
6. **Iteration** is simple with range-based for loops
7. **Include <string>** header file

### String Design Principles

**Good string usage**:

- **Use const string&** for function parameters (avoid copying)
- **Check bounds** when accessing characters
- **Validate input** before processing
- **Use appropriate methods** (don't reinvent the wheel)
- **Handle empty strings** gracefully

### Best Practices Checklist

✅ **Include <string> header**  
✅ **Use string, not char arrays** (unless interfacing with C)  
✅ **Check .empty() before accessing**  
✅ **Use .at() for bounds checking** in critical code  
✅ **Pass strings by const reference** to functions  
✅ **Use getline() for input with spaces**  
✅ **Remember cin.ignore() after cin >>**  
✅ **Use comparison operators** (==, !=, <, >) for comparing  
✅ **Check find() result** against string::npos  
✅ **Validate conversions** (stoi, etc.) with try-catch

### Quick Reference

**Creation**:

```cpp
string s1 = "Hello";
string s2(10, 'x');  // "xxxxxxxxxx"
string s3 = s1;      // Copy
```

**Input/Output**:

```cpp
cin >> s;           // Single word
getline(cin, s);    // Full line
cout << s;          // Output
```

**Common operations**:

```cpp
s.length()          // Get length
s.empty()           // Check if empty
s[i]                // Access character
s.at(i)             // Safe access
s + "text"          // Concatenate
s.append("text")    // Append
s.substr(pos, len)  // Extract substring
s.find("text")      // Find substring
s.replace(pos, len, "new")  // Replace
s.erase(pos, len)   // Remove
s.insert(pos, "text")       // Insert
```

**Conversion**:

```cpp
stoi(s)             // String to int
stod(s)             // String to double
to_string(num)      // Number to string
```

---

## Conclusion

Strings are essential for:

- **Text processing** - Analyzing and manipulating text
- **User interfaces** - Displaying and collecting information
- **Data validation** - Checking input formats
- **File I/O** - Reading and writing text files
- **Communication** - Messages, protocols, APIs

**Mastering strings enables you to**:

- Build text-based applications
- Process and analyze textual data
- Validate user input effectively
- Implement algorithms on text
- Create interactive programs

**Next steps**:

1. Practice all the exercises
2. Build text-processing programs
3. Explore regular expressions (advanced)
4. Move on to **Pointers** for deeper C++ understanding
5. Learn about **File Handling** for persistent text storage

With strings mastered, you can handle text data like a pro!

**Happy Coding!** 🚀