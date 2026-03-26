> **Series overview:** This is file 1 of 7. It covers the absolute core of JavaScript: what it is, how it runs, variables, data types, operators, strings, type conversion, and conditionals. Every concept is explained from first principles, not just "how" but "why".

---

## Table of Contents

1. [What Is JavaScript?](#1-what-is-javascript)
2. [How JavaScript Executes Code](#2-how-javascript-executes-code)
3. [Setting Up a Dev Environment](#3-setting-up-a-dev-environment)
4. [Connecting JS to HTML](#4-connecting-js-to-html)
5. [The Console & Comments](#5-the-console--comments)
6. [Mastering Variables](#6-mastering-variables)
7. [Working with Numbers](#7-working-with-numbers)
8. [Booleans](#8-booleans)
9. [Comparison Operators](#9-comparison-operators)
10. [Manipulating Strings](#10-manipulating-strings)
11. [Type Conversion & Coercion](#11-type-conversion--coercion)
12. [Conditional Statements](#12-conditional-statements)
13. [Switch Statements](#13-switch-statements)

---

## 1. What Is JavaScript?

JavaScript is a **high-level, interpreted, dynamically-typed, single-threaded, garbage-collected** programming language with **first-class functions** and a **prototype-based object model**.

Let's unpack each of those:

|Term|What it means|
|---|---|
|High-level|You don't manage memory manually like in C/C++|
|Interpreted|Code is run line by line, not compiled to machine code ahead of time (though modern engines JIT-compile it)|
|Dynamically typed|A variable's type is determined at runtime, not when you write the code|
|Single-threaded|Only one thing runs at a time — there is one call stack|
|Garbage-collected|The engine automatically frees unused memory|
|First-class functions|Functions are values — you can pass them around like strings or numbers|
|Prototype-based|Objects inherit from other objects, not from classes (classes in JS are syntactic sugar)|

### Where Does JS Run?

Originally only in browsers. Today:

- **Browser** — manipulates the DOM, handles events, talks to APIs
- **Node.js** — server-side JS (what your NestJS backend runs on)
- **Deno / Bun** — newer server runtimes
- **React Native / Electron** — mobile and desktop apps

### The JS Engine

Every browser has a JS engine:

|Browser|Engine|
|---|---|
|Chrome / Edge|V8|
|Firefox|SpiderMonkey|
|Safari|JavaScriptCore (Nitro)|

Node.js also uses V8. The engine does:

1. **Parsing** — turns your source code into an AST (Abstract Syntax Tree)
2. **Compilation** — V8 compiles JS to machine code via JIT (Just In Time) compilation
3. **Execution** — runs it

---

## 2. How JavaScript Executes Code

Understanding execution is critical. JS is single-threaded but non-blocking thanks to the **Event Loop**.

### The Call Stack

Every time a function is called, a new **stack frame** is pushed onto the call stack. When the function returns, its frame is popped.

```js
function greet(name) {
  return `Hello, ${name}`;  // step 3: this executes
}

function main() {
  const msg = greet("Aiden"); // step 2: greet() is pushed onto stack
  console.log(msg);           // step 4: greet() popped, console.log pushed
}

main(); // step 1: main() is pushed onto stack
```

Stack trace (bottom → top):

```
greet()   ← currently executing
main()
(anonymous) ← global scope
```

### Global Execution Context

When your JS file first runs, the engine creates a **Global Execution Context (GEC)**. It does two things in order:

1. **Creation phase** — scans the code, allocates memory for variables and function declarations (this is **hoisting**)
2. **Execution phase** — runs the code line by line

```js
console.log(x);     // undefined — not an error! x was hoisted
var x = 10;
console.log(x);     // 10
```

`var` declarations are hoisted to the top of their scope and initialized to `undefined`. `let` and `const` are hoisted too but NOT initialized — accessing them before declaration throws a `ReferenceError` (the **Temporal Dead Zone**).

```js
console.log(y); // ReferenceError: Cannot access 'y' before initialization
let y = 5;
```

---

## 3. Setting Up a Dev Environment

### Minimum Setup

- A browser (Chrome or Firefox for best DevTools)
- A code editor — **VS Code** is the industry standard

### Recommended VS Code Extensions

- **ESLint** — catches errors and enforces style
- **Prettier** — auto-formats code
- **Path IntelliSense** — autocompletes file paths
- **Live Server** — auto-reloads HTML files in browser

### Folder Structure (Real Project)

```
project/
├── index.html
├── styles/
│   └── main.css
├── scripts/
│   └── main.js
│   └── auth.js
│   └── dashboard.js
└── assets/
    └── logo.png
```

---

## 4. Connecting JS to HTML

There are three ways to include JavaScript in HTML:

### Inline (Avoid)

```html
<button onclick="alert('clicked')">Click me</button>
```

This mixes logic with markup — hard to maintain. Avoid in real projects.

### Internal Script Tag

```html
<script>
  console.log("Hello from internal script");
</script>
```

### External File (Preferred)

```html
<!-- In <head>: blocks HTML parsing — avoid unless needed -->
<script src="scripts/main.js"></script>

<!-- At end of <body>: DOM is fully parsed before JS runs -->
<script src="scripts/main.js"></script>

<!-- With defer: downloads in parallel, executes after HTML is parsed -->
<script src="scripts/main.js" defer></script>

<!-- With async: downloads in parallel, executes as soon as downloaded (no order guarantee) -->
<script src="scripts/analytics.js" async></script>
```

**Real-world rule:** Use `defer` for almost everything. Use `async` only for scripts that are independent (e.g., analytics).

### Why `defer` Is Important

Without `defer` in the `<head>`, the browser halts HTML parsing when it hits your `<script>` tag, downloads the file, executes it, then resumes. If your JS tries to access a DOM element that hasn't been parsed yet, it gets `null`.

```html
<head>
  <script src="main.js"></script> <!-- main.js runs BEFORE body exists -->
</head>
<body>
  <div id="app"></div>
</body>
```

```js
// main.js
document.getElementById("app"); // null — DOM isn't ready yet!
```

With `defer`, this is solved — the script runs after the entire HTML document is parsed.

---

## 5. The Console & Comments

### The Console Object

The `console` object is your primary debugging tool. It's available in both browsers and Node.js.

```js
console.log("Standard output");
console.warn("Something might be wrong"); // Yellow in DevTools
console.error("Something IS wrong");      // Red in DevTools
console.info("FYI message");

// Logging objects (very useful)
const user = { id: 1, name: "Aiden", role: "admin" };
console.log(user);         // Prints the object
console.table([user]);     // Prints as a neat table

// Timing — useful for performance checks
console.time("fetchUsers");
// ...some operation
console.timeEnd("fetchUsers"); // "fetchUsers: 23.4ms"

// Grouping logs
console.group("Auth Flow");
console.log("Token validated");
console.log("User loaded");
console.groupEnd();

// Counting how many times a line is hit
for (let i = 0; i < 3; i++) {
  console.count("loop ran"); // "loop ran: 1", "loop ran: 2", etc.
}
```

### Comments

```js
// Single-line comment — use for quick notes

/*
  Multi-line comment
  Use for explaining complex logic blocks
  or temporarily disabling code
*/

/**
 * JSDoc comment — the standard for documenting functions
 * These are read by editors and tools to provide autocomplete hints
 *
 * @param {string} username - The user's login name
 * @param {string} password - The plain-text password (will be hashed)
 * @returns {Promise<Object>} The authenticated user object
 */
async function loginUser(username, password) {
  // ...implementation
}
```

**Real-world tip:** Write comments that explain _why_, not _what_. The code already says _what_ it does.

```js
// Bad comment:
// Add 1 to i
i++;

// Good comment:
// JWT tokens expire every 15 minutes — bump counter to trigger refresh
refreshAttempts++;
```

---

## 6. Mastering Variables

Variables are named containers for data. JavaScript has three ways to declare them: `var`, `let`, and `const`. Understanding the difference is foundational.

### `var` — The Old Way (Mostly Avoid)

```js
var username = "aiden";
```

Problems with `var`:

1. **Function-scoped**, not block-scoped — leaks out of `if`, `for`, etc.
2. **Can be re-declared** in the same scope (silent bugs)
3. **Hoisted and initialized to `undefined`** (can lead to confusion)

```js
function checkAge() {
  if (true) {
    var age = 25; // you'd expect this to be block-scoped
  }
  console.log(age); // 25 — it leaked out of the if block!
}
```

### `let` — Block-Scoped, Reassignable

```js
let count = 0;
count = 1; // OK — can reassign

if (true) {
  let blockVar = "I'm block-scoped";
}
console.log(blockVar); // ReferenceError — doesn't exist here
```

Use `let` when the value needs to change (counters, loop variables, state that gets updated).

### `const` — Block-Scoped, Not Reassignable

```js
const MAX_LOGIN_ATTEMPTS = 5;
MAX_LOGIN_ATTEMPTS = 10; // TypeError: Assignment to constant variable

const user = { name: "Aiden" };
user.name = "Alex";  // OK! You're mutating the object, not reassigning the variable
user = {};           // TypeError — you can't make user point to a new object
```

**`const` doesn't mean immutable.** For primitive values (numbers, strings, booleans), it's effectively immutable. For objects and arrays, the _reference_ is locked but the _contents_ can change.

### The Rule of Thumb

> Default to `const`. Upgrade to `let` only when you need to reassign. Never use `var`.

```js
// Real auth module example
const JWT_SECRET = process.env.JWT_SECRET; // never changes — const
const tokenExpiry = "15m";                  // config value — const

let loginAttempts = 0;                      // increments — let
let isAuthenticated = false;                // flips — let

loginAttempts++;
isAuthenticated = true;
```

### Variable Naming Conventions

```js
// camelCase — standard for variables and functions
const userToken = "...";
function validateEmail() {}

// SCREAMING_SNAKE_CASE — constants / config
const MAX_RETRIES = 3;
const API_BASE_URL = "https://api.example.com";

// PascalCase — classes, constructor functions, React components
class UserService {}
function ProductCard() {}

// _prefixed — private by convention (not enforced by JS)
const _internalState = {};

// $ prefix — used by jQuery and sometimes for DOM element refs
const $modal = document.getElementById("modal");
```

### Scope Deep Dive

**Block scope** — anything inside `{}`

```js
{
  let x = 10; // only lives here
}
console.log(x); // ReferenceError
```

**Function scope** — variables declared inside a function

```js
function makeToken() {
  const secret = "abc123"; // only lives inside makeToken
}
console.log(secret); // ReferenceError
```

**Module scope** — in ES modules (`.mjs` or `type="module"`), top-level variables are scoped to the file, not global.

**Global scope** — variables declared outside any function or block. In browsers: `window`. In Node.js: `global`. Avoid polluting global scope.

### Closures (Preview — Also in Part 3)

A closure is when a function remembers the variables from the scope where it was created, even after that scope has finished executing.

```js
// Real example: rate limiter
function createRateLimiter(maxAttempts) {
  let attempts = 0; // this variable is "enclosed" over

  return function tryLogin() {
    if (attempts >= maxAttempts) {
      return "Too many attempts. Account locked.";
    }
    attempts++;
    return `Attempt ${attempts} of ${maxAttempts}`;
  };
}

const limiter = createRateLimiter(3);
console.log(limiter()); // "Attempt 1 of 3"
console.log(limiter()); // "Attempt 2 of 3"
console.log(limiter()); // "Attempt 3 of 3"
console.log(limiter()); // "Too many attempts. Account locked."
```

---

## 7. Working with Numbers

JavaScript has only **one number type** — 64-bit floating point (IEEE 754 double precision). There's no separate int type.

```js
const price = 29.99;
const quantity = 3;
const total = price * quantity; // 89.97000000000001 — floating point precision!
```

### Arithmetic Operators

```js
10 + 3   // 13  — addition
10 - 3   // 7   — subtraction
10 * 3   // 30  — multiplication
10 / 3   // 3.3333... — division (always float)
10 % 3   // 1   — modulo (remainder)
10 ** 3  // 1000 — exponentiation (ES2016)
```

### The Floating Point Problem

```js
0.1 + 0.2 === 0.3 // false! It's 0.30000000000000004
```

This is not a JS bug — it's how IEEE 754 floating point works in every language. Solutions:

```js
// Option 1: Round to a fixed number of decimal places
const result = (0.1 + 0.2).toFixed(2); // "0.30" (string!)
parseFloat((0.1 + 0.2).toFixed(2));    // 0.3 (number)

// Option 2: Work in integers (multiply by 100 for cents)
// Real-world: NEVER store money as floating point!
const priceInCents = 1099; // $10.99
const taxInCents = Math.round(priceInCents * 0.08); // 88 cents
const totalInCents = priceInCents + taxInCents;     // 1187 cents = $11.87
const displayPrice = (totalInCents / 100).toFixed(2); // "11.87"
```

### The Math Object

```js
Math.round(4.5)    // 5  — rounds to nearest integer
Math.floor(4.9)    // 4  — always rounds down
Math.ceil(4.1)     // 5  — always rounds up
Math.abs(-10)      // 10 — absolute value
Math.max(1, 5, 3)  // 5  — largest value
Math.min(1, 5, 3)  // 1  — smallest value
Math.sqrt(16)      // 4  — square root
Math.pow(2, 10)    // 1024 — same as 2 ** 10
Math.PI            // 3.141592653589793
Math.random()      // 0.0 to 0.9999... — random float

// Random integer between min and max (inclusive)
function randomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}
randomInt(1, 6); // simulates a dice roll
```

### Special Number Values

```js
Infinity      // result of dividing by zero, or a number too large
-Infinity
NaN           // "Not a Number" — result of invalid math
typeof NaN    // "number" — confusingly, NaN IS of type number

// Checking for NaN (very important)
NaN === NaN    // false! NaN is not equal to itself
isNaN("abc")   // true  — but also coerces strings first (can be misleading)
Number.isNaN("abc")  // false — strict, only true for actual NaN
Number.isNaN(NaN)    // true
Number.isFinite(Infinity) // false
Number.isInteger(4.0)     // true
Number.isInteger(4.5)     // false
```

### Number Methods

```js
const n = 123.456789;

n.toFixed(2)       // "123.46" — fixed decimal places (returns string)
n.toPrecision(5)   // "123.46" — total significant digits (returns string)
n.toString()       // "123.456789"
n.toString(2)      // binary representation
n.toString(16)     // hex representation

Number.parseInt("42px")   // 42 — parses leading integer
Number.parseFloat("3.14abc") // 3.14
Number.parseInt("abc")    // NaN
```

### Real-World Example: E-Commerce Price Calculations

```js
// prices always stored as integers (cents) in DB — never floats
const cartItems = [
  { name: "Laptop Stand", priceInCents: 4999, qty: 1 },
  { name: "USB Cable",    priceInCents: 999,  qty: 3 },
  { name: "Mousepad",     priceInCents: 1499, qty: 2 },
];

const subtotal = cartItems.reduce((sum, item) => {
  return sum + item.priceInCents * item.qty;
}, 0);

const TAX_RATE = 0.08; // 8%
const tax = Math.round(subtotal * TAX_RATE);
const total = subtotal + tax;

// Format for display
function formatCurrency(cents) {
  return `$${(cents / 100).toFixed(2)}`;
}

console.log(`Subtotal: ${formatCurrency(subtotal)}`); // $10,491
console.log(`Tax:      ${formatCurrency(tax)}`);
console.log(`Total:    ${formatCurrency(total)}`);
```

---

## 8. Booleans

A boolean is the simplest data type: `true` or `false`. Every conditional check in JS ultimately resolves to a boolean.

### Truthy and Falsy Values

This is one of the most important concepts in JavaScript. Every value in JS is either **truthy** or **falsy** — meaning it behaves like `true` or `false` when used in a boolean context.

**Falsy values (there are exactly 6):**

```js
false
0
-0
0n          // BigInt zero
""          // empty string
null
undefined
NaN
```

**Everything else is truthy**, including:

```js
"0"         // non-empty string — truthy!
[]          // empty array — truthy!
{}          // empty object — truthy!
function(){}
-1
Infinity
```

### Why This Matters in Real Code

```js
// Auth guard: check if user token exists
const token = localStorage.getItem("authToken");

if (token) {
  // token is a non-empty string → truthy → runs
  redirectToDashboard();
} else {
  // token is null (not found) → falsy → runs
  redirectToLogin();
}

// Checking if an API response has data
const users = await fetchUsers();
if (users.length) {  // 0 is falsy, any other length is truthy
  renderTable(users);
} else {
  showEmptyState();
}
```

### Boolean Conversion

```js
Boolean(0)          // false
Boolean("")         // false
Boolean(null)       // false
Boolean(undefined)  // false
Boolean(NaN)        // false
Boolean("hello")    // true
Boolean([])         // true
Boolean({})         // true

// Shorthand with double NOT (!!)
!!0         // false
!!"hello"   // true
!!null      // false
!![]        // true
```

The `!!` pattern is very common in real code to explicitly convert a value to a boolean:

```js
// Does this user have admin permissions?
const isAdmin = !!(user.roles && user.roles.includes("admin"));
```

---

## 9. Comparison Operators

### Loose vs Strict Equality

This is where many JS bugs are born.

```js
// == (loose equality) — performs type coercion before comparing
1 == "1"     // true  — "1" coerced to number 1
0 == false   // true  — false coerced to 0
null == undefined // true — special case
[] == false  // true  — bizarre but real

// === (strict equality) — no coercion, type AND value must match
1 === "1"    // false — different types
1 === 1      // true
null === undefined // false
```

**Rule: Always use `===` and `!==` in real code.** Never use `==` unless you have a very specific reason.

### Comparison Operators

```js
5 > 3    // true  — greater than
5 < 3    // false — less than
5 >= 5   // true  — greater than or equal
5 <= 4   // false — less than or equal
5 !== 3  // true  — strict not-equal
5 != "5" // false — loose not-equal (coerces "5" to 5)
```

### Comparing Strings

Strings are compared lexicographically (character by character, Unicode code point values):

```js
"apple" < "banana"  // true  — 'a' (97) < 'b' (98)
"Z" < "a"           // true  — uppercase letters have lower code points
"10" > "9"          // false! — "1" (49) < "9" (57) as strings
Number("10") > Number("9") // true — compare as numbers
```

### Comparing Objects

```js
// Objects are compared by REFERENCE, not value
const a = { id: 1 };
const b = { id: 1 };
const c = a;

a === b  // false — different objects in memory
a === c  // true  — same reference
```

This is why comparing two objects for equality requires a deep comparison:

```js
// Shallow check (one level deep)
function shallowEqual(obj1, obj2) {
  const keys1 = Object.keys(obj1);
  const keys2 = Object.keys(obj2);
  if (keys1.length !== keys2.length) return false;
  return keys1.every(key => obj1[key] === obj2[key]);
}

// For deep comparison, use a library like Lodash
import _ from "lodash";
_.isEqual({ a: { b: 1 } }, { a: { b: 1 } }); // true
```

### Spaceship-like Sorting

The comparison operators are heavily used in `.sort()`:

```js
// Sort products by price (ascending)
products.sort((a, b) => a.price - b.price);
// If a.price - b.price is negative, a comes first
// If positive, b comes first
// If 0, order unchanged
```

---

## 10. Manipulating Strings

Strings in JavaScript are **immutable sequences of UTF-16 code units**. Every string method returns a _new_ string — it never modifies the original.

### Creating Strings

```js
const single = 'Hello';
const double = "World";
const template = `Hello, ${single}`; // template literal — covered in Part 4
```

### String Properties and Basic Methods

```js
const email = "User@Example.COM";

email.length        // 17 — number of characters
email.toLowerCase() // "user@example.com"
email.toUpperCase() // "USER@EXAMPLE.COM"
email.trim()        // removes whitespace from both ends
email.trimStart()   // removes leading whitespace only
email.trimEnd()     // removes trailing whitespace only
```

### Searching Within Strings

```js
const url = "https://api.example.com/users/123";

url.includes("api")          // true
url.startsWith("https")      // true
url.endsWith(".com/users/123") // true

url.indexOf("example")       // 12 — first occurrence index, -1 if not found
url.lastIndexOf("1")         // 32 — last occurrence index

// Regex search
url.search(/\/users\/\d+/)   // 27 — index of match, -1 if not found
url.match(/\/users\/(\d+)/)  // ["/users/123", "123", ...] — full match result
```

### Extracting Parts of Strings

```js
const str = "Hello, World!";

str.slice(7, 12)       // "World" — slice(start, end) — end is exclusive
str.slice(7)           // "World!" — from index 7 to end
str.slice(-6)          // "World!" — negative = from end
str.substring(7, 12)   // "World" — similar to slice but no negatives
str.charAt(0)          // "H"
str[0]                 // "H" — same as charAt
str.charCodeAt(0)      // 72 — Unicode code point of "H"
```

### Replacing Content

```js
const greeting = "Hello, John!";

greeting.replace("John", "Jane")     // "Hello, Jane!" — replaces first match
greeting.replaceAll("l", "L")        // "HeLLo, John!" — replaces all
greeting.replace(/john/i, "Jane")    // "Hello, Jane!" — regex, case-insensitive

// Real-world: sanitizing user input
function sanitizeUsername(input) {
  return input
    .trim()
    .toLowerCase()
    .replace(/[^a-z0-9_]/g, ""); // remove anything that's not alphanumeric or _
}
sanitizeUsername("  John.Doe! "); // "johndoe"
```

### Splitting and Joining

```js
// split — breaks a string into an array
"a,b,c,d".split(",")       // ["a", "b", "c", "d"]
"hello".split("")           // ["h", "e", "l", "l", "o"]
"one two three".split(" ") // ["one", "two", "three"]

// join — combines an array into a string
["2024", "01", "15"].join("-") // "2024-01-15"

// Real-world: parsing a JWT (the payload is base64-encoded JSON)
const token = "header.payload.signature";
const [header, payload, sig] = token.split(".");
// decode the payload:
const decoded = JSON.parse(atob(payload));
```

### Padding and Repeating

```js
"5".padStart(3, "0")   // "005" — useful for formatting IDs, times
"5".padEnd(3, "0")     // "500"
"=".repeat(20)         // "====================" — separator lines

// Formatting order numbers
const orderId = 42;
const formatted = String(orderId).padStart(8, "0"); // "00000042"
```

### Template Literals (Quick Preview)

```js
const name = "Aiden";
const role = "admin";
const greeting = `Welcome back, ${name}! You are logged in as: ${role.toUpperCase()}`;

// Multi-line strings (no \n needed)
const emailBody = `
  Dear ${name},
  
  Your order #${orderId} has been shipped.
  
  Regards,
  The Team
`;
```

---

## 11. Type Conversion & Coercion

### The Two Types of Conversion

**Explicit (Type Casting)** — you intentionally convert:

```js
Number("42")      // 42
Number("3.14")    // 3.14
Number("")        // 0
Number("abc")     // NaN
Number(true)      // 1
Number(false)     // 0
Number(null)      // 0
Number(undefined) // NaN

String(42)        // "42"
String(true)      // "true"
String(null)      // "null"
String(undefined) // "undefined"

Boolean(0)        // false
Boolean("")       // false
Boolean("hello")  // true
Boolean([])       // true (!)

parseInt("42px")        // 42 — parses until non-numeric
parseFloat("3.14 kg")   // 3.14
parseInt("0xFF", 16)    // 255 — hexadecimal to decimal
```

**Implicit (Type Coercion)** — JS does it automatically (the sneaky one):

```js
"5" + 3         // "53" — + with a string triggers concatenation
"5" - 3         // 2    — -, *, / always convert to numbers
"5" * "3"       // 15
true + true     // 2
null + 1        // 1
undefined + 1   // NaN
[] + {}         // "[object Object]"
{} + []         // 0 (in some contexts!)
```

### The `+` Operator Trap

The `+` operator is overloaded: it does both addition AND string concatenation.

- If **either** operand is a string, it concatenates.
- Otherwise, it adds.

```js
1 + 2          // 3
1 + "2"        // "12"  — watch out!
1 + 2 + "3"    // "33"  — left to right: 1+2=3, then 3+"3"="33"
"1" + 2 + 3    // "123" — "1"+2="12", then "12"+3="123"
```

### Real-World Coercion Bug

```js
// User input from a form field is ALWAYS a string
const ageInput = document.getElementById("age").value; // "25" (string!)
const nextYear = ageInput + 1; // "251" — NOT 26!

// Fix: always convert input to a number
const age = Number(ageInput); // or +ageInput or parseInt(ageInput)
const nextYear = age + 1;     // 26 ✓
```

### Safe Type Checking

```js
typeof 42           // "number"
typeof "hello"      // "string"
typeof true         // "boolean"
typeof undefined    // "undefined"
typeof null         // "object" — historic bug in JS, null is NOT an object
typeof {}           // "object"
typeof []           // "object" — arrays are objects too!
typeof function(){} // "function"

// Better checks:
Array.isArray([])        // true
Array.isArray({})        // false
value === null           // only way to check for null
typeof x === "undefined" // or: x === undefined

// Real check: is this a valid user object?
function isValidUser(user) {
  return (
    user !== null &&
    typeof user === "object" &&
    typeof user.id === "number" &&
    typeof user.email === "string"
  );
}
```

---

## 12. Conditional Statements

### `if` / `else if` / `else`

```js
// Basic structure
if (condition) {
  // runs if condition is truthy
} else if (anotherCondition) {
  // runs if first was falsy and this is truthy
} else {
  // runs if all above were falsy
}
```

### Real-World Example: Auth Access Control

```js
function getAccessLevel(user) {
  if (!user) {
    return "no-access"; // not logged in
  }

  if (user.isBanned) {
    return "banned";
  }

  if (user.role === "superadmin") {
    return "full-access";
  } else if (user.role === "admin") {
    return "admin-access";
  } else if (user.role === "moderator") {
    return "moderate-access";
  } else {
    return "read-only";
  }
}
```

### Early Return Pattern (Guard Clauses)

Instead of deeply nested if/else, validate and return early:

```js
// Nested (hard to read)
function processOrder(order) {
  if (order) {
    if (order.items.length > 0) {
      if (order.paymentMethod) {
        // actual logic here — buried 3 levels deep
      }
    }
  }
}

// Guard clauses (clean, preferred in real projects)
function processOrder(order) {
  if (!order) throw new Error("Order is required");
  if (order.items.length === 0) throw new Error("Cart is empty");
  if (!order.paymentMethod) throw new Error("Payment method required");

  // actual logic here — clean and flat
  return chargeAndFulfill(order);
}
```

### Ternary Operator

The ternary is a one-line if/else that returns a value:

```js
condition ? valueIfTrue : valueIfFalse;

// Example
const label = user.isLoggedIn ? "Dashboard" : "Login";
const price = product.onSale ? product.salePrice : product.regularPrice;
```

Don't nest ternaries — it becomes unreadable:

```js
// Bad:
const status = isAdmin ? "admin" : isMod ? "mod" : isUser ? "user" : "guest";

// Better: use if/else or a lookup object
const roleMap = { admin: "admin", moderator: "mod", user: "user" };
const status = roleMap[user.role] ?? "guest";
```

### Short-Circuit Evaluation

Logical operators `&&` and `||` can be used as conditionals because they return values, not just `true`/`false`:

```js
// && returns the first falsy value, or the last value if all are truthy
"hello" && "world"   // "world"
null && "world"      // null (short-circuits, never evaluates "world")

// || returns the first truthy value, or the last value if all are falsy
null || "default"    // "default"
"value" || "default" // "value"

// Real use: default values
const username = inputValue || "Anonymous";
const port = process.env.PORT || 3000;

// Conditional rendering (common in React)
const isLoggedIn = true;
const element = isLoggedIn && <Dashboard />; // renders Dashboard only if logged in

// ?? (Nullish Coalescing) — only falls back on null/undefined, NOT on 0 or ""
const timeout = config.timeout ?? 5000; // 0 is valid, so use ?? not ||
const name = user.displayName ?? user.email ?? "Anonymous";
```

### The `?.` Optional Chaining Operator

Safely access nested properties without crashing when something is `null` or `undefined`:

```js
const user = null;

// Without optional chaining:
const city = user && user.address && user.address.city; // verbose

// With optional chaining:
const city = user?.address?.city; // undefined if any step is null/undefined

// Works on methods too:
user?.getPermissions?.(); // calls only if user and getPermissions exist

// Real-world: API response may have optional nested data
const avatarUrl = response?.data?.user?.profile?.avatar?.url ?? "/default-avatar.png";
```

---

## 13. Switch Statements

Switch is useful when you have one value that you're comparing against many known cases.

### Syntax

```js
switch (expression) {
  case value1:
    // code
    break; // CRITICAL: without break, falls through to next case
  case value2:
    // code
    break;
  default:
    // runs if no case matched
}
```

### The Fall-Through Behavior

```js
const day = "Monday";

switch (day) {
  case "Monday":
  case "Tuesday":
  case "Wednesday":
  case "Thursday":
  case "Friday":
    console.log("Weekday");
    break; // break here — applies to all above cases
  case "Saturday":
  case "Sunday":
    console.log("Weekend");
    break;
  default:
    console.log("Unknown day");
}
```

### Real-World: HTTP Status Code Handler

```js
function handleApiResponse(statusCode) {
  switch (statusCode) {
    case 200:
    case 201:
      return { success: true, message: "Request successful" };

    case 400:
      return { success: false, message: "Bad request — check your input" };

    case 401:
      // Token expired — redirect to login
      clearAuthToken();
      redirectToLogin();
      return { success: false, message: "Unauthorized" };

    case 403:
      return { success: false, message: "You don't have permission to do that" };

    case 404:
      return { success: false, message: "Resource not found" };

    case 429:
      return { success: false, message: "Too many requests — slow down" };

    case 500:
    case 502:
    case 503:
      logErrorToService({ statusCode });
      return { success: false, message: "Server error — try again later" };

    default:
      return { success: false, message: `Unexpected status: ${statusCode}` };
  }
}
```

### When to Use Switch vs if/else vs Lookup Objects

```js
// if/else — best for range checks or complex conditions
if (score >= 90) grade = "A";
else if (score >= 80) grade = "B";

// switch — best for exact matches against a single variable
switch (action.type) { /* ... */ }

// Lookup object (often the cleanest) — best for mapping one value to another
const statusMessages = {
  200: "OK",
  404: "Not Found",
  500: "Server Error",
};
const message = statusMessages[statusCode] ?? "Unknown";
```

---

## Summary Cheat Sheet

```
Types:         number, string, boolean, null, undefined, object, symbol, bigint
Variables:     const (default) > let (reassignable) > var (avoid)
Equality:      === and !== always; never == or !=
Falsy values:  false, 0, "", null, undefined, NaN
String ops:    .toLowerCase(), .trim(), .includes(), .slice(), .split(), .replace()
Type check:    typeof, Array.isArray(), === null
Null safety:   ?. (optional chaining), ?? (nullish coalescing)
Conditionals:  if/else with guard clauses, ternary for inline, switch for enums
```

---

_Next: [Part 2 — Loops, Logical Operators, Arrays, and Objects](./part-2-loops-and-data.md)_