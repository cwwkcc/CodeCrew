> Every JavaScript program — from a form validator to a full-stack web app — is built from a small set of primitives. This file covers those primitives: how values are stored, how logic flows, and how code is organised into functions. Get these right and everything else is pattern recognition.

---

## Table of Contents

1. [How JavaScript Runs](#1-how-javascript-runs)
2. [Variables: let, const, var](#2-variables-let-const-var)
3. [Data Types](#3-data-types)
4. [Type Coercion and Equality](#4-type-coercion-and-equality)
5. [Operators](#5-operators)
6. [Strings In Depth](#6-strings-in-depth)
7. [Conditionals](#7-conditionals)
8. [Functions](#8-functions)
9. [Scope](#9-scope)
10. [Hoisting](#10-hoisting)

---

## 1. How JavaScript Runs

JavaScript is an interpreted/JIT-compiled language. You write a `.js` file — the engine reads it, compiles it to machine code on the fly, and executes it.

```
your code (text)  →  JS Engine (V8)  →  output
```

In the **browser**, V8 is embedded in Chrome. It can access the DOM, browser APIs, and the network.

In **Node.js**, V8 runs standalone. It can access the file system, network, and OS — but not the DOM.

Code runs top to bottom, one statement at a time, on a single thread. Asynchronous operations (timers, network requests) are handled by the event loop — covered in File 06.

---

## 2. Variables: `let`, `const`, `var`

A variable is a named container for a value.

```javascript
let age = 20;          // can be reassigned
const name = "Ashan";  // cannot be reassigned
var legacy = true;     // old syntax — avoid (explained below)
```

### `const` vs `let`

```javascript
// const: the binding cannot be reassigned
const school = "CWWKCC";
school = "Other";  // TypeError: Assignment to constant variable

// But if the value is an object, the OBJECT can still be mutated
const user = { name: "Ashan" };
user.name = "Dineth";   // ✓ — modifying the object
user = {};              // TypeError — reassigning the variable

// Rule of thumb: use const by default, let when you need to reassign
```

### Why `var` is problematic

```javascript
// var is function-scoped, not block-scoped
if (true) {
  var x = 10;  // declared inside a block
}
console.log(x);  // 10 — leaked out of the if block!

if (true) {
  let y = 10;
}
console.log(y);  // ReferenceError — y doesn't exist here

// var is also hoisted (declared at the top of the function)
console.log(z);  // undefined — not an error with var!
var z = 5;

// let and const are NOT accessible before their declaration:
console.log(w);  // ReferenceError
let w = 5;
```

Use `const` and `let`. Never use `var` in modern code.

---

## 3. Data Types

JavaScript has **8 data types**. 7 are primitives, 1 is `object`.

### Primitives (stored by value)

```javascript
// number — all numbers, integers and floats, are the same type
let count = 42;
let price = 19.99;
let negative = -7;

// Special number values:
Infinity       // 1 / 0
-Infinity      // -1 / 0
NaN            // "Not a Number" — result of invalid math
Number.MAX_SAFE_INTEGER  // 9007199254740991 (2^53 - 1)

// string — text, always immutable
let greeting = "hello";
let name = 'world';
let template = `${greeting}, ${name}`;  // template literal

// boolean
let isActive = true;
let isDone = false;

// undefined — variable declared but not assigned
let x;
console.log(x);  // undefined

// null — intentional absence of value
let selectedUser = null;  // "nothing is selected"

// symbol — unique, used as object keys (advanced)
const id = Symbol("id");

// bigint — integers larger than Number.MAX_SAFE_INTEGER
const huge = 9007199254740992n;  // note the `n` suffix
```

### `object` (stored by reference)

```javascript
// Objects, arrays, and functions are all "object" type
typeof {};           // "object"
typeof [];           // "object"
typeof null;         // "object" ← famous JS bug, null is NOT an object

// Check for array specifically:
Array.isArray([]);   // true

// Check for null specifically:
someValue === null;  // true
```

### `typeof` operator

```javascript
typeof 42;           // "number"
typeof "hello";      // "string"
typeof true;         // "boolean"
typeof undefined;    // "undefined"
typeof null;         // "object"  ← the bug
typeof {};           // "object"
typeof [];           // "object"
typeof function(){}; // "function"
typeof Symbol();     // "symbol"
typeof 42n;          // "bigint"
```

---

## 4. Type Coercion and Equality

JavaScript automatically converts types in some situations. This is called **type coercion**.

```javascript
// String conversion
"5" + 3    // "53" — 3 is coerced to string, then concatenated
"5" - 3    // 2    — "5" is coerced to number, then subtracted
"5" * "3"  // 15   — both coerced to number
+"5"       // 5    — unary + converts string to number
+true      // 1
+false     // 0
+null      // 0
+undefined // NaN

// Truthiness — values that behave like false in boolean contexts
// Falsy values (there are exactly 6):
false, 0, "", null, undefined, NaN

// Everything else is truthy — including:
"0"        // truthy (non-empty string)
[]         // truthy (empty array)
{}         // truthy (empty object)
```

### `==` vs `===`

```javascript
// == (loose equality) — coerces types before comparing
0 == false    // true  (false → 0)
"" == false   // true  (both → 0)
null == undefined  // true (special case)
1 == "1"      // true  ("1" → 1)

// === (strict equality) — NO coercion, checks type AND value
0 === false   // false (different types)
1 === "1"     // false (different types)
null === undefined  // false (different types)
1 === 1       // true

// Always use ===
// The only acceptable use of == is: someValue == null
// which is shorthand for (someValue === null || someValue === undefined)
```

---

## 5. Operators

### Arithmetic

```javascript
5 + 3    // 8    addition
5 - 3    // 2    subtraction
5 * 3    // 15   multiplication
10 / 3   // 3.333... division (always float)
10 % 3   // 1    modulo (remainder)
2 ** 10  // 1024 exponentiation

// Increment / decrement
let x = 5;
x++;   // post-increment: returns 5, then x becomes 6
++x;   // pre-increment: x becomes 7, then returns 7
x--;   // post-decrement
--x;   // pre-decrement
```

### Assignment

```javascript
let x = 10;
x += 5;    // x = x + 5  → 15
x -= 3;    // x = x - 3  → 12
x *= 2;    // x = x * 2  → 24
x /= 4;    // x = x / 4  → 6
x **= 2;   // x = x ** 2 → 36
x %= 10;   // x = x % 10 → 6
```

### Logical

```javascript
true && false   // false  — AND: both must be true
true || false   // true   — OR: at least one must be true
!true           // false  — NOT: flips the boolean

// Short-circuit evaluation — critical to understand
// && returns the first falsy value, or the last value if all truthy
false && doExpensiveWork()  // doExpensiveWork() is NEVER called
"hello" && "world"          // "world" (both truthy, returns last)
0 && "hello"                // 0 (0 is falsy, short-circuits)

// || returns the first truthy value, or the last value if all falsy
null || "default"           // "default"
"value" || "default"        // "value"
null || undefined || 0      // 0 (all falsy, returns last)

// Common pattern: default values
const username = inputValue || "Anonymous";
```

### Nullish Coalescing `??`

```javascript
// ?? returns right side ONLY if left side is null or undefined
// (unlike || which triggers on ANY falsy value)

null ?? "default"       // "default"
undefined ?? "default"  // "default"
0 ?? "default"          // 0      ← 0 is NOT null/undefined
"" ?? "default"         // ""     ← "" is NOT null/undefined
false ?? "default"      // false  ← false is NOT null/undefined

// When to prefer ?? over ||:
// If 0, "", or false are valid values, use ?? to avoid replacing them
const count = data.count ?? 0;  // safe: won't replace 0 with 0
const count2 = data.count || 0; // unsafe: if data.count is 0, still returns 0
                                 // (coincidentally fine here, but concept matters)
```

### Optional Chaining `?.`

```javascript
// Safely access nested properties that might be null/undefined
const user = null;

user.name         // TypeError: Cannot read properties of null
user?.name        // undefined — no error

const street = user?.address?.street?.name;  // undefined — no error
// Without optional chaining:
const street2 = user && user.address && user.address.street && user.address.street.name;

// Also works for method calls and array access:
user?.greet()        // undefined if user is null (doesn't call greet)
arr?.[0]             // undefined if arr is null
```

---

## 6. Strings In Depth

Strings are immutable sequences of Unicode characters.

```javascript
const s = "Hello, World!";

// Length
s.length           // 13

// Access (read-only — strings are immutable)
s[0]               // "H"
s.at(-1)           // "!" — negative index from end

// Searching
s.includes("World")         // true
s.startsWith("Hello")       // true
s.endsWith("!")             // true
s.indexOf("o")              // 4 (first occurrence)
s.lastIndexOf("o")          // 8 (last occurrence)

// Transformation — all return NEW strings (strings are immutable)
s.toUpperCase()             // "HELLO, WORLD!"
s.toLowerCase()             // "hello, world!"
s.trim()                    // removes leading/trailing whitespace
s.trimStart()               // removes leading whitespace only
s.trimEnd()                 // removes trailing whitespace only
s.replace("World", "KITS")  // "Hello, KITS!"
s.replaceAll("l", "L")      // "HeLLo, WorLd!"

// Splitting and joining
"a,b,c".split(",")          // ["a", "b", "c"]
["a", "b", "c"].join("-")   // "a-b-c"

// Padding
"5".padStart(3, "0")        // "005"
"5".padEnd(3, "0")          // "500"

// Substrings
s.slice(7, 12)              // "World"
s.slice(-6)                 // "orld!" — from 6 chars before the end

// Template literals — the modern way to compose strings
const name = "Ashan";
const grade = 11;
const message = `Student ${name} is in grade ${grade}.`;
// Any expression works inside ${}:
const price = `Total: ${(quantity * unitPrice).toFixed(2)} LKR`;

// Multi-line strings
const html = `
  <div class="card">
    <h2>${title}</h2>
    <p>${description}</p>
  </div>
`;
```

---

## 7. Conditionals

### `if / else if / else`

```javascript
const score = 75;

if (score >= 75) {
  console.log("Distinction");
} else if (score >= 65) {
  console.log("Merit");
} else if (score >= 50) {
  console.log("Pass");
} else {
  console.log("Fail");
}
```

### Ternary operator

```javascript
// condition ? valueIfTrue : valueIfFalse
const label = score >= 50 ? "Pass" : "Fail";

// Nested ternary — use sparingly, gets unreadable fast
const grade = score >= 75 ? "A" : score >= 65 ? "B" : score >= 50 ? "C" : "F";
```

### `switch`

```javascript
const day = "Monday";

switch (day) {
  case "Monday":
  case "Tuesday":
  case "Wednesday":
  case "Thursday":
  case "Friday":
    console.log("Weekday");
    break;  // MUST break or falls through to next case
  case "Saturday":
  case "Sunday":
    console.log("Weekend");
    break;
  default:
    console.log("Unknown day");
}
```

### Nullish assignment operators

```javascript
let config = {};

// ??= assigns only if left side is null or undefined
config.timeout ??= 5000;    // config.timeout = 5000 (was undefined)
config.timeout ??= 3000;    // no change — timeout is already 5000

// ||= assigns if left side is falsy
config.retries ||= 3;       // assigns 3

// &&= assigns if left side is truthy
config.debug &&= false;     // if debug is truthy, set to false
```

---

## 8. Functions

A function is a reusable block of code.

```javascript
// Function declaration — hoisted (can be called before it's defined)
function greet(name) {
  return `Hello, ${name}!`;
}

// Function expression — NOT hoisted
const greet2 = function(name) {
  return `Hello, ${name}!`;
};

// Arrow function — most common in modern JS
const greet3 = (name) => `Hello, ${name}!`;

// Calling a function
greet("Ashan");    // "Hello, Ashan!"
greet3("Dineth");  // "Hello, Dineth!"
```

### Parameters

```javascript
// Default parameters
function createUser(name, role = "student", isActive = true) {
  return { name, role, isActive };
}
createUser("Ashan");               // { name: "Ashan", role: "student", isActive: true }
createUser("Ashan", "teacher");    // { name: "Ashan", role: "teacher", isActive: true }

// Rest parameters — collects remaining args into an array
function sum(...numbers) {
  return numbers.reduce((total, n) => total + n, 0);
}
sum(1, 2, 3, 4, 5);  // 15

// Spread operator — expands an array as arguments
const nums = [1, 2, 3];
Math.max(...nums);    // 3  (same as Math.max(1, 2, 3))
```

### Return values

```javascript
// A function with no return statement returns undefined
function logMessage(msg) {
  console.log(msg);
  // implicit: return undefined
}

// Early return — often cleaner than nested if/else
function getDiscount(price, memberType) {
  if (memberType === "gold") return price * 0.2;
  if (memberType === "silver") return price * 0.1;
  return 0;
}
```

### Arrow functions

```javascript
// Short syntax when body is a single expression
const double = x => x * 2;          // single param: no parens needed
const add = (a, b) => a + b;        // multiple params: parens required
const getObj = () => ({ key: "val" }); // returning object: wrap in ()

// Full body with curly braces
const process = (data) => {
  const cleaned = data.trim();
  return cleaned.toUpperCase();
};

// Key difference: arrow functions DON'T have their own `this`
// They inherit `this` from the surrounding scope
// This matters for event handlers and class methods — covered in later files
```

---

## 9. Scope

Scope determines where a variable is accessible.

```javascript
// Global scope — accessible everywhere
const globalVar = "I am global";

function outer() {
  // Function scope — accessible inside outer() and anything inside it
  const outerVar = "I am in outer";

  function inner() {
    // Also has access to outerVar (scope chain)
    const innerVar = "I am in inner";

    console.log(globalVar);  // ✓
    console.log(outerVar);   // ✓
    console.log(innerVar);   // ✓
  }

  inner();
  console.log(innerVar);  // ReferenceError — innerVar is not in scope here
}

// Block scope — let and const are confined to the nearest {}
{
  let blockVar = "block";
  const blockConst = "also block";
}
console.log(blockVar);   // ReferenceError
console.log(blockConst); // ReferenceError

// var ignores blocks (function-scoped or global):
{
  var leaky = "I leak";
}
console.log(leaky);  // "I leak" — bad!
```

### The Scope Chain

When JavaScript looks up a variable, it starts at the current scope and walks upward:

```
inner's scope      → look here first
     ↓ not found?
outer's scope      → look here
     ↓ not found?
global scope       → look here
     ↓ not found?
ReferenceError
```

---

## 10. Hoisting

Hoisting is JavaScript's behaviour of moving declarations to the top of their scope before execution.

```javascript
// Function declarations are fully hoisted — callable before definition
sayHello();  // ✓ "Hello!" — works even though defined below

function sayHello() {
  console.log("Hello!");
}

// var declarations are hoisted (but NOT their values)
console.log(x);  // undefined — declared but not yet assigned
var x = 5;
console.log(x);  // 5

// let and const are hoisted but NOT initialised
// Accessing them before their declaration is a ReferenceError
console.log(y);  // ReferenceError: Cannot access 'y' before initialisation
let y = 10;
// This is called the "Temporal Dead Zone" — the region between the
// start of the block and the variable's declaration line
```

```javascript
// Function expressions are NOT hoisted
greet();  // TypeError: greet is not a function

const greet = function() {
  console.log("Hello!");
};
// (would also fail with `let greet = ...` or `var greet = ...`)
// Only function DECLARATIONS are fully hoisted
```

---

## Summary

```
Variables:
  const   — default choice, can't reassign
  let     — use when you need to reassign
  var     — never use in modern code

Types (8 total):
  Primitives: number, string, boolean, undefined, null, symbol, bigint
  Reference:  object (includes arrays and functions)

Key operators:
  ===   strict equality (always use this)
  ??    nullish coalescing (null/undefined only)
  ?.    optional chaining (safe property access)
  &&    logical AND (short-circuits)
  ||    logical OR (short-circuits)

Functions:
  declaration → hoisted, can call before definition
  expression  → not hoisted
  arrow       → no own `this`, concise syntax

Scope:
  let/const → block-scoped
  var       → function-scoped (avoid)
  scope chain walks upward looking for variables
```

---

_Next: [01 — Loops, Data Structures & Objects](./01%20-%20Loops%2C%20Data%20Structures%20%26%20Objects.md)_