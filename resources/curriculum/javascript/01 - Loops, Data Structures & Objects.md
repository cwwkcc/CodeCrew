> Data is the raw material of every program. JavaScript gives you several containers for data — arrays, objects, Map, Set — and several tools for iterating over them. Understanding which container fits which problem, and how to move data between them efficiently, is the core skill of this file.

---

## Table of Contents

1. [Arrays](#1-arrays)
2. [Array Methods — Transformation](#2-array-methods--transformation)
3. [Array Methods — Search and Test](#3-array-methods--search-and-test)
4. [Array Methods — Reduction and Accumulation](#4-array-methods--reduction-and-accumulation)
5. [Loops](#5-loops)
6. [Objects](#6-objects)
7. [Destructuring](#7-destructuring)
8. [Spread and Rest](#8-spread-and-rest)
9. [Map and Set](#9-map-and-set)
10. [Choosing the Right Data Structure](#10-choosing-the-right-data-structure)

---

## 1. Arrays

An array is an ordered list of values. Values can be any type, and you can mix types (though you usually shouldn't).

```javascript
// Creating arrays
const empty = [];
const numbers = [1, 2, 3, 4, 5];
const names = ["Ashan", "Dineth", "Kavya"];
const mixed = [1, "hello", true, null]; // valid, but unusual

// Accessing elements
numbers[0]; // 1 — zero-indexed
numbers[4]; // 5
numbers.at(-1); // 5 — last element (modern, cleaner)
numbers.at(-2); // 4 — second to last

// Modifying
numbers[0] = 10; // replace first element
numbers.push(6); // add to end → [1, 2, 3, 4, 5, 6]
numbers.pop(); // remove from end → returns 6
numbers.unshift(0); // add to beginning → [0, 1, 2, 3, 4, 5]
numbers.shift(); // remove from beginning → returns 0

// Length
numbers.length; // 5

// Checking
Array.isArray(numbers); // true
Array.isArray("string"); // false
```

### Slicing and Splicing

```javascript
const arr = [1, 2, 3, 4, 5];

// slice — returns a NEW array (non-destructive)
arr.slice(1, 3); // [2, 3] — from index 1, up to (not including) 3
arr.slice(2); // [3, 4, 5] — from index 2 to end
arr.slice(-2); // [4, 5] — last 2 elements
// arr is unchanged

// splice — modifies the ORIGINAL array (destructive)
const removed = arr.splice(1, 2); // remove 2 elements starting at index 1
// arr is now [1, 4, 5], removed is [2, 3]

arr.splice(1, 0, 10, 20); // insert 10, 20 at index 1 without removing
// arr is now [1, 10, 20, 4, 5]

// Flattening nested arrays
[1, [2, 3], [4, [5]]]
  .flat() // [1, 2, 3, 4, [5]] — one level
  [(1, [2, 3], [4, [5]])].flat(2) // [1, 2, 3, 4, 5] — two levels
  [(1, [2, [3, [4]]])].flat(Infinity); // [1, 2, 3, 4] — all levels
```

---

## 2. Array Methods — Transformation

These methods return NEW arrays. They don't modify the original.

```javascript
const students = [
  { name: "Ashan", grade: 11, score: 82 },
  { name: "Dineth", grade: 12, score: 91 },
  { name: "Kavya", grade: 11, score: 67 },
  { name: "Saman", grade: 12, score: 55 },
];

// map — transform each element, returns new array of same length
const names = students.map((s) => s.name);
// ["Ashan", "Dineth", "Kavya", "Saman"]

const withGrade = students.map((s) => ({
  ...s,
  letterGrade:
    s.score >= 75 ? "A" : s.score >= 65 ? "B" : s.score >= 50 ? "C" : "F",
}));

// filter — keep only elements that pass a test
const grade11 = students.filter((s) => s.grade === 11);
// [{ name: "Ashan", ... }, { name: "Kavya", ... }]

const passed = students.filter((s) => s.score >= 50);

// Chaining: filter then map
const grade11Names = students.filter((s) => s.grade === 11).map((s) => s.name);
// ["Ashan", "Kavya"]

// flatMap — map then flatten one level
const words = ["hello world", "foo bar"].flatMap((s) => s.split(" "));
// ["hello", "world", "foo", "bar"]

// sort — MUTATES original, returns the same array (careful!)
const scores = [82, 91, 67, 55];
scores.sort((a, b) => a - b); // ascending: [55, 67, 82, 91]
scores.sort((a, b) => b - a); // descending: [91, 82, 67, 55]

// Sort objects
students.sort((a, b) => b.score - a.score); // highest score first
students.sort((a, b) => a.name.localeCompare(b.name)); // alphabetical

// To sort without mutating:
const sorted = [...students].sort((a, b) => b.score - a.score);
```

---

## 3. Array Methods — Search and Test

```javascript
const students = [
  { name: "Ashan", grade: 11, score: 82 },
  { name: "Dineth", grade: 12, score: 91 },
  { name: "Kavya", grade: 11, score: 67 },
];

// find — returns the FIRST element that passes, or undefined
const topStudent = students.find((s) => s.score > 85);
// { name: "Dineth", grade: 12, score: 91 }

// findIndex — returns the INDEX of the first match, or -1
const idx = students.findIndex((s) => s.name === "Kavya");
// 2

// findLast / findLastIndex — searches from the end (ES2023)
const lastGrade11 = students.findLast((s) => s.grade === 11);

// some — true if ANY element passes the test
students.some((s) => s.score > 90); // true
students.some((s) => s.score > 100); // false

// every — true if ALL elements pass the test
students.every((s) => s.score >= 50); // true (all passed)
students.every((s) => s.score >= 70); // false (Kavya: 67)

// includes — check if a VALUE is in the array (use with primitives)
[1, 2, 3].includes(2); // true
["a", "b"].includes("c"); // false

// indexOf — returns first index of value, or -1
[1, 2, 3, 2].indexOf(2); // 1
[1, 2, 3].indexOf(99); // -1
```

---

## 4. Array Methods — Reduction and Accumulation

```javascript
// reduce — accumulates a single value from an array
// Most powerful, most flexible array method

const scores = [82, 91, 67, 55, 78];

// Sum
const total = scores.reduce((acc, score) => acc + score, 0);
// 373

// Average
const avg = scores.reduce((acc, score) => acc + score, 0) / scores.length;
// 74.6

// Max (though Math.max(...scores) is simpler)
const max = scores.reduce(
  (acc, score) => (score > acc ? score : acc),
  -Infinity,
);
// 91

// Group by a property
const students = [
  { name: "Ashan", grade: 11 },
  { name: "Dineth", grade: 12 },
  { name: "Kavya", grade: 11 },
  { name: "Saman", grade: 12 },
];

const byGrade = students.reduce((groups, student) => {
  const key = student.grade;
  groups[key] ??= []; // create array if doesn't exist
  groups[key].push(student);
  return groups;
}, {});

// Result:
// {
//   11: [{ name: "Ashan" }, { name: "Kavya" }],
//   12: [{ name: "Dineth" }, { name: "Saman" }]
// }

// Build a lookup object from an array
const userById = students.reduce((map, student) => {
  map[student.name] = student;
  return map;
}, {});
// { Ashan: {...}, Dineth: {...}, Kavya: {...}, Saman: {...} }
```

---

## 5. Loops

### `for` loop (classic)

```javascript
for (let i = 0; i < 5; i++) {
  console.log(i); // 0, 1, 2, 3, 4
}

// Iterate with index
const arr = ["a", "b", "c"];
for (let i = 0; i < arr.length; i++) {
  console.log(i, arr[i]); // 0 "a", 1 "b", 2 "c"
}
```

### `for...of` — iterate over values

```javascript
// Arrays
for (const name of ["Ashan", "Dineth", "Kavya"]) {
  console.log(name);
}

// With index using entries()
for (const [index, name] of ["Ashan", "Dineth"].entries()) {
  console.log(index, name);  // 0 "Ashan", 1 "Dineth"
}

// Strings
for (const char of "KITS") {
  console.log(char);  // K, I, T, S
}

// Maps and Sets (covered later)
for (const [key, value] of myMap) { ... }
for (const item of mySet) { ... }
```

### `for...in` — iterate over object keys

```javascript
const student = { name: "Ashan", grade: 11, score: 82 };

for (const key in student) {
  console.log(key, student[key]);
  // name Ashan
  // grade 11
  // score 82
}

// Warning: for...in iterates over ALL enumerable properties,
// including inherited ones. Prefer Object.keys() for plain objects.
```

### `while` and `do...while`

```javascript
let attempts = 0;
while (attempts < 3) {
  const result = tryOperation();
  if (result.success) break;
  attempts++;
}

// do...while — body executes at least once
let input;
do {
  input = getUserInput();
} while (!isValid(input));
```

### `break` and `continue`

```javascript
// break — exit the loop entirely
for (const student of students) {
  if (student.score > 90) {
    console.log("Found top student:", student.name);
    break; // stop looking
  }
}

// continue — skip this iteration, continue to next
for (const student of students) {
  if (student.grade !== 11) continue; // skip non-grade-11
  console.log(student.name); // only grade 11 students
}
```

---

## 6. Objects

An object is a collection of key-value pairs. Keys are strings (or Symbols), values can be anything.

```javascript
// Object literal
const student = {
  name: "Ashan",
  grade: 11,
  scores: [82, 91, 78],
  address: {
    city: "Mathugama",
    district: "Kalutara",
  },
  greet() {
    return `Hi, I'm ${this.name}`;
  },
};

// Access properties
student.name; // "Ashan" — dot notation
student["name"]; // "Ashan" — bracket notation (use when key is dynamic)
student.address.city; // "Mathugama" — nested access
student.nonExistent; // undefined — no error

// Modify
student.grade = 12;
student.email = "ashan@school.lk"; // add new property
delete student.email; // remove property (avoid in production)

// Computed property names
const field = "score";
const obj = { [field]: 95 }; // { score: 95 }
```

### Object Methods

```javascript
const student = { name: "Ashan", grade: 11, score: 82 };

Object.keys(student); // ["name", "grade", "score"]
Object.values(student); // ["Ashan", 11, 82]
Object.entries(student); // [["name","Ashan"], ["grade",11], ["score",82]]

// Iterating over entries
for (const [key, value] of Object.entries(student)) {
  console.log(`${key}: ${value}`);
}

// Object.assign — shallow merge (mutates target)
const defaults = { role: "student", isActive: true };
const user = Object.assign({}, defaults, { name: "Ashan" });
// { role: "student", isActive: true, name: "Ashan" }

// Object.freeze — prevent modifications
const config = Object.freeze({ apiUrl: "https://api.cwwkcc.lk" });
config.apiUrl = "other"; // silently fails in sloppy mode, throws in strict
config.apiUrl; // still "https://api.cwwkcc.lk"

// Object.fromEntries — entries array → object
const entries = [
  ["name", "Ashan"],
  ["grade", 11],
];
Object.fromEntries(entries); // { name: "Ashan", grade: 11 }

// Useful pattern: transform object values
const doubled = Object.fromEntries(
  Object.entries({ a: 1, b: 2, c: 3 }).map(([k, v]) => [k, v * 2]),
);
// { a: 2, b: 4, c: 6 }
```

---

## 7. Destructuring

Destructuring extracts values from arrays or objects into named variables.

### Object Destructuring

```javascript
const student = { name: "Ashan", grade: 11, score: 82, city: "Mathugama" };

// Basic
const { name, grade } = student;
// name = "Ashan", grade = 11

// Rename
const { name: studentName, grade: year } = student;
// studentName = "Ashan", year = 11

// Default values
const { name, role = "student" } = student;
// role = "student" (wasn't in student object)

// Nested
const {
  address: { city, district },
} = { address: { city: "Mathugama", district: "Kalutara" } };
// city = "Mathugama", district = "Kalutara"

// Rest — collect remaining properties
const { name, ...rest } = student;
// name = "Ashan"
// rest = { grade: 11, score: 82, city: "Mathugama" }

// In function parameters — very common in React
function renderStudent({ name, grade, score = 0 }) {
  return `${name} (Grade ${grade}): ${score}`;
}
renderStudent(student);
```

### Array Destructuring

```javascript
const scores = [82, 91, 67, 55, 78];

const [first, second] = scores;
// first = 82, second = 91

// Skip elements
const [, , third] = scores;
// third = 67

// Rest
const [top, ...rest] = scores;
// top = 82, rest = [91, 67, 55, 78]

// Default values
const [a = 0, b = 0, c = 0] = [1, 2];
// a = 1, b = 2, c = 0

// Swap variables — classic trick
let x = 1,
  y = 2;
[x, y] = [y, x];
// x = 2, y = 1

// Destructuring from a function return
function getRange() {
  return [0, 100];
}
const [min, max] = getRange();
```

---

## 8. Spread and Rest

### Spread `...` — expand an iterable

```javascript
// Arrays
const a = [1, 2, 3];
const b = [4, 5, 6];
const combined = [...a, ...b]; // [1, 2, 3, 4, 5, 6]
const withMiddle = [...a, 99, ...b]; // [1, 2, 3, 99, 4, 5, 6]
const copy = [...a]; // shallow copy

// Objects
const defaults = { role: "student", active: true };
const override = { role: "teacher", name: "Ashan" };
const merged = { ...defaults, ...override };
// { role: "teacher", active: true, name: "Ashan" }
// Later properties override earlier ones

// Copy and modify — very common in React state updates
const student = { name: "Ashan", score: 82 };
const updated = { ...student, score: 90 };
// { name: "Ashan", score: 90 } — original unchanged

// Spread into function call
const numbers = [3, 1, 4, 1, 5, 9];
Math.max(...numbers); // 9 — same as Math.max(3, 1, 4, 1, 5, 9)
```

### Rest `...` — collect remaining items

```javascript
// In function parameters
function log(level, ...messages) {
  console.log(`[${level}]`, ...messages);
}
log("INFO", "Server started", "on port 3000");
// [INFO] Server started on port 3000

// In destructuring (already shown above)
const { name, ...rest } = student;
const [first, ...remaining] = array;
```

---

## 9. Map and Set

### `Map` — key-value pairs with any key type

```javascript
// Unlike objects, Map keys can be ANY type (objects, functions, etc.)
const map = new Map();

// Set
map.set("name", "Ashan");
map.set(42, "the answer");
map.set(true, "boolean key");
map.set({ id: 1 }, "object key"); // objects as keys!

// Get
map.get("name"); // "Ashan"
map.get(42); // "the answer"
map.get("missing"); // undefined

// Has
map.has("name"); // true
map.has("other"); // false

// Delete
map.delete("name");

// Size
map.size; // 3

// Iteration — Map preserves insertion order
for (const [key, value] of map) {
  console.log(key, value);
}

map.keys(); // iterable of keys
map.values(); // iterable of values
map.entries(); // iterable of [key, value] pairs

// Create from entries
const userMap = new Map([
  ["alice", { role: "admin" }],
  ["bob", { role: "user" }],
]);
```

**When to use Map over Object:**

- Keys that are not strings (numbers, objects, functions)
- Need to know the size easily (`.size`)
- Need to maintain insertion order reliably
- Frequently adding/removing keys (Map is optimised for this)

### `Set` — collection of unique values

```javascript
const set = new Set([1, 2, 3, 2, 1]); // duplicates removed
// Set {1, 2, 3}

set.add(4);
set.add(2); // already present — no change
set.has(3); // true
set.delete(1);
set.size; // 3

// Iteration
for (const value of set) {
  console.log(value);
}

// Most common use: deduplicate an array
const withDuplicates = [1, 2, 2, 3, 3, 3, 4];
const unique = [...new Set(withDuplicates)];
// [1, 2, 3, 4]

// Check membership efficiently
// Set.has() is O(1), Array.includes() is O(n)
const allowedRoles = new Set(["admin", "teacher", "staff"]);
allowedRoles.has("admin"); // true
allowedRoles.has("student"); // false

// Set operations
const a = new Set([1, 2, 3]);
const b = new Set([2, 3, 4]);

const union = new Set([...a, ...b]); // {1, 2, 3, 4}
const intersection = new Set([...a].filter((x) => b.has(x))); // {2, 3}
const difference = new Set([...a].filter((x) => !b.has(x))); // {1}
```

---

## 10. Choosing the Right Data Structure

```
Problem                                   Best choice
──────────────────────────────────────────────────────────────────
Ordered list of items                     Array
Unique collection of items                Set
Key-value pairs, string keys, fixed shape Object literal
Key-value pairs, any key type             Map
Key-value pairs, frequently added/removed Map
Deduplication                             Set
Lookup by key                             Object or Map
Maintaining insertion order               Map (Objects too in modern JS)
Membership check (fast)                   Set
```

```javascript
// Real-world example: building a student report
const students = [...];                    // Array — ordered list

// Unique subjects across all students
const subjects = new Set(
  students.flatMap(s => s.subjects)        // Set — unique values
);

// Quick lookup by student ID
const studentById = new Map(
  students.map(s => [s.id, s])            // Map — any key, fast lookup
);

// Student's info shape
const student = {                          // Object — fixed known shape
  name: "Ashan",
  grade: 11,
  scores: { math: 82, science: 91 },
};
```

---

## Summary

```
Arrays:
  Creation: [], Array.from(), new Array()
  Transformation (new array): map, filter, flatMap, sort (careful!)
  Search: find, findIndex, includes, indexOf, some, every
  Accumulation: reduce

Loops:
  for          — classic, use when you need index control
  for...of     — iterate values (arrays, strings, Maps, Sets)
  for...in     — iterate object keys (prefer Object.keys() + for...of)
  while        — when condition-driven
  break/continue — control loop flow

Objects:
  Access: dot notation or bracket notation
  Methods: Object.keys/values/entries, Object.assign, Object.fromEntries

Destructuring:
  Object: const { name, role = "student" } = user
  Array:  const [first, ...rest] = arr
  In function params: function fn({ name, age }) {}

Spread/Rest:
  Spread: expand into call or literal: [...arr], {...obj}
  Rest: collect remaining: function fn(...args), const { a, ...rest }

Map vs Object: Map when keys aren't strings, or need size, or frequent mutations
Set vs Array:  Set for unique values and fast membership checks
```

---

_Next: [02 — Functions, Scope, JSON, Dates & Timers](./02%20-%20Functions%2C%20Scope%2C%20JSON%2C%20Dates%20%26%20Timers.md)_
