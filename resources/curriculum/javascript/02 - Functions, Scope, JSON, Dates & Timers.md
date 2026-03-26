> **Part 3 of 7.** The deepest part of core JavaScript — how functions really work, lexical scope, closures, the `this` keyword (preview), object methods, JSON, the Date API, and asynchronous timers.

---

## Table of Contents

1. [Functions: The Basics](#1-functions-the-basics)
2. [Function Types](#2-function-types)
3. [Callback Functions](#3-callback-functions)
4. [Variable Scope in Depth](#4-variable-scope-in-depth)
5. [Closures](#5-closures)
6. [Object Methods](#6-object-methods)
7. [JSON Handling](#7-json-handling)
8. [Dates & Time](#8-dates--time)
9. [setInterval & setTimeout](#9-setinterval--settimeout)

---

## 1. Functions: The Basics

A function is a **reusable block of code** that performs a specific task. In JavaScript, functions are **first-class citizens** — they can be stored in variables, passed as arguments, and returned from other functions.

### Function Declaration

```js
function greet(name) {
  return `Hello, ${name}!`;
}

// Key behaviors:
// 1. Hoisted — can be called BEFORE its declaration in the file
// 2. Creates a named function (name shows in stack traces — helpful for debugging)

sayHi("Alice"); // works even though it's declared below

function sayHi(name) {
  console.log(`Hi, ${name}`);
}
```

### Parameters vs Arguments

```js
// Parameters — the names in the function definition
function createUser(username, email, role) {  // username, email, role are parameters
  return { username, email, role };
}

// Arguments — the actual values you pass when calling
createUser("alice", "alice@example.com", "admin"); // these are arguments
```

### The `return` Statement

Every function in JS returns a value. If you don't write `return`, it returns `undefined`.

```js
function add(a, b) {
  return a + b; // explicit return
}

function logSomething() {
  console.log("hello"); // no return
}

const result = logSomething(); // undefined

// return exits the function immediately
function findUser(users, id) {
  for (const user of users) {
    if (user.id === id) return user; // exits on first match
  }
  return null; // fallback if not found
}
```

### Default Parameters

```js
// Without defaults — manually handle undefined
function createOrder(product, quantity, currency) {
  currency = currency !== undefined ? currency : "USD"; // verbose
}

// With ES6 default parameters
function createOrder(product, quantity = 1, currency = "USD") {
  return { product, quantity, currency };
}

createOrder("Laptop");              // { product: "Laptop", qty: 1, currency: "USD" }
createOrder("Laptop", 2);          // { product: "Laptop", qty: 2, currency: "USD" }
createOrder("Laptop", 2, "EUR");   // { product: "Laptop", qty: 2, currency: "EUR" }
createOrder("Laptop", undefined, "GBP"); // qty defaults to 1 (undefined triggers default)
createOrder("Laptop", null, "GBP");      // qty is null — null does NOT trigger default!
```

### Rest Parameters

Collect all remaining arguments into an array:

```js
function sum(...numbers) {
  return numbers.reduce((total, n) => total + n, 0);
}
sum(1, 2, 3, 4, 5); // 15

// Real-world: logging with context
function log(level, ...messages) {
  const timestamp = new Date().toISOString();
  console.log(`[${timestamp}] [${level.toUpperCase()}]`, ...messages);
}

log("info", "User logged in:", user.email);
log("error", "Failed to fetch:", url, "Status:", 404);
```

---

## 2. Function Types

### Function Expression

```js
// Stored in a variable — NOT hoisted (only the variable is hoisted, not the function)
const multiply = function(a, b) {
  return a * b;
};

// Named function expression — the name is only available inside the function (useful for recursion)
const factorial = function fact(n) {
  return n <= 1 ? 1 : n * fact(n - 1); // "fact" is accessible here
};
// fact(5) — ReferenceError outside
```

### Arrow Functions

Arrow functions are a concise syntax introduced in ES6. They differ from regular functions in one critical way: **they do not have their own `this`**.

```js
// Full form
const add = (a, b) => {
  return a + b;
};

// Implicit return — when body is a single expression, {} and return are optional
const add = (a, b) => a + b;

// Single parameter — parentheses optional
const double = n => n * 2;

// No parameters — parentheses required
const getTimestamp = () => Date.now();

// Returning an object — must wrap in parentheses (otherwise {} is parsed as function body)
const makeUser = (name, email) => ({ name, email });

// Multi-line still needs explicit return
const processUser = (user) => {
  const cleaned = user.name.trim().toLowerCase();
  return { ...user, name: cleaned };
};
```

### When to Use Each

```js
// Arrow functions are great for callbacks
const doubled = [1, 2, 3].map(n => n * 2);

// Regular functions are needed when you need `this` context
const timer = {
  seconds: 0,
  start() {
    // Arrow function: `this` from enclosing scope = the timer object ✓
    setInterval(() => {
      this.seconds++;
      console.log(this.seconds);
    }, 1000);

    // Regular function: `this` would be undefined (strict) or window ✗
    setInterval(function() {
      this.seconds++; // `this` is NOT timer here!
    }, 1000);
  },
};
```

### Immediately Invoked Function Expression (IIFE)

A function that runs immediately after being defined. Used to create isolated scope.

```js
// Prevents variables from leaking into global scope
(function() {
  const secret = "only lives in here";
  console.log("IIFE executed");
})();

// Arrow IIFE
(() => {
  const config = loadConfig();
  initApp(config);
})();

// IIFE with return value
const appConfig = (() => {
  const env = process.env.NODE_ENV;
  return {
    isProd: env === "production",
    apiUrl: env === "production"
      ? "https://api.example.com"
      : "http://localhost:3000",
  };
})();
```

### Generator Functions

Functions that can pause and resume, yielding multiple values:

```js
function* idGenerator() {
  let id = 1;
  while (true) {
    yield id++;
  }
}

const gen = idGenerator();
gen.next().value; // 1
gen.next().value; // 2
gen.next().value; // 3

// Real use: unique ID generation, infinite sequences, lazy evaluation
function* range(start, end, step = 1) {
  for (let i = start; i <= end; i += step) {
    yield i;
  }
}

for (const n of range(0, 100, 10)) {
  console.log(n); // 0, 10, 20, ... 100
}
```

---

## 3. Callback Functions

A **callback** is a function passed as an argument to another function, to be called at a later point.

### How Callbacks Work

```js
// The function that receives and calls the callback
function executeAction(action, callback) {
  console.log(`Executing: ${action}`);
  const result = { action, success: true, timestamp: Date.now() };
  callback(result); // call the callback with the result
}

// Passing a named function
function handleResult(result) {
  console.log("Result:", result);
}
executeAction("send email", handleResult);

// Passing an anonymous function
executeAction("send email", function(result) {
  console.log("Result:", result);
});

// Arrow function (most common in modern code)
executeAction("send email", (result) => {
  console.log("Result:", result);
});
```

### Callbacks as the Foundation of Async

Before Promises, all async operations in JS used callbacks:

```js
// Node.js file system (callback style)
const fs = require("fs");

fs.readFile("./config.json", "utf8", (error, data) => {
  if (error) {
    console.error("Failed to read config:", error.message);
    return; // always return after error handling
  }
  const config = JSON.parse(data);
  console.log("Config loaded:", config);
});
```

### Error-First Callback Convention (Node.js)

In Node.js, callbacks follow the convention `(error, result)`:

```js
function divideAsync(a, b, callback) {
  if (b === 0) {
    callback(new Error("Cannot divide by zero"), null); // error first
    return;
  }
  callback(null, a / b); // null error, then result
}

divideAsync(10, 2, (err, result) => {
  if (err) {
    console.error(err.message);
    return;
  }
  console.log(result); // 5
});
```

### Higher-Order Functions

A function that takes a function as argument or returns a function:

```js
// Built-in higher-order functions
[1, 2, 3].map(n => n * 2);         // map takes a callback
[1, 2, 3].filter(n => n > 1);      // filter takes a callback
setTimeout(() => {}, 1000);         // setTimeout takes a callback

// Custom higher-order function
function withLogging(fn, label) {
  return function(...args) {
    console.time(label);
    const result = fn(...args);
    console.timeEnd(label);
    return result;
  };
}

const timedFetch = withLogging(fetch, "API call");
const data = await timedFetch("/api/users");
```

### Real-World: Middleware Pattern (Like Express.js / NestJS)

This is fundamentally how Express middleware and NestJS guards work:

```js
// Simplified middleware chain
function runMiddleware(req, res, middlewares) {
  let index = 0;

  function next(error) {
    if (error) {
      return res.status(500).json({ error: error.message });
    }
    if (index >= middlewares.length) return;

    const middleware = middlewares[index++];
    middleware(req, res, next); // each middleware calls next() to proceed
  }

  next();
}

// Each middleware is a callback function
const authMiddleware = (req, res, next) => {
  const token = req.headers.authorization;
  if (!token) return res.status(401).json({ error: "No token" });
  req.user = verifyToken(token);
  next(); // pass control to next middleware
};

const logMiddleware = (req, res, next) => {
  console.log(`${req.method} ${req.url} — ${new Date().toISOString()}`);
  next();
};

runMiddleware(req, res, [logMiddleware, authMiddleware, routeHandler]);
```

---

## 4. Variable Scope in Depth

**Scope** determines which variables are accessible where. JavaScript uses **lexical scoping** — scope is determined by where code is written, not where it is called.

### The Scope Chain

When JavaScript looks up a variable, it starts in the current scope and works outward:

```js
const globalVar = "I'm global";

function outer() {
  const outerVar = "I'm in outer";

  function inner() {
    const innerVar = "I'm in inner";

    // inner can access all three:
    console.log(innerVar);  // ✓ own scope
    console.log(outerVar);  // ✓ outer's scope
    console.log(globalVar); // ✓ global scope
  }

  inner();
  console.log(innerVar); // ReferenceError! can't look inward
}
```

### Block Scope (`let` / `const`)

```js
{
  let blockLet = "block";
  const blockConst = "block";
  var blockVar = "leaks!";
}

console.log(blockVar);   // "leaks!" — var ignores block scope
console.log(blockLet);   // ReferenceError
console.log(blockConst); // ReferenceError

// Loop variable scope
for (var i = 0; i < 3; i++) { /* ... */ }
console.log(i); // 3 — var leaked out!

for (let j = 0; j < 3; j++) { /* ... */ }
console.log(j); // ReferenceError — let is block-scoped
```

### The Classic `var` Loop Bug

This is one of the most famous JS bugs, now historical since `let` solves it:

```js
// With var — all callbacks share the same `i`
for (var i = 0; i < 3; i++) {
  setTimeout(() => console.log(i), 100);
}
// prints: 3 3 3 — i is 3 by the time callbacks run

// With let — each iteration gets its own `i`
for (let i = 0; i < 3; i++) {
  setTimeout(() => console.log(i), 100);
}
// prints: 0 1 2 ✓
```

### Module Scope

In ES modules (`<script type="module">` or `.mjs` files), top-level variables are private to the module:

```js
// auth.js
const JWT_SECRET = "super-secret"; // private to this module
export function verifyToken(token) { /* uses JWT_SECRET */ }

// main.js
import { verifyToken } from "./auth.js";
console.log(JWT_SECRET); // ReferenceError — not exported
```

---

## 5. Closures

A **closure** is the combination of a function and the **lexical environment** (variable scope) in which it was created. The function "closes over" the variables from its outer scope, keeping them alive even after the outer function has returned.

### The Core Mechanism

```js
function makeCounter() {
  let count = 0; // this variable is "enclosed" in the returned function

  return function increment() {
    count++;
    return count;
  };
}

const counter = makeCounter();
// makeCounter() has finished — normally `count` would be garbage collected
// but it's NOT — because `counter` still holds a reference to it

counter(); // 1
counter(); // 2
counter(); // 3
```

### Each Closure Has Its Own Private State

```js
const counterA = makeCounter();
const counterB = makeCounter();

counterA(); // 1
counterA(); // 2
counterB(); // 1 — completely independent from counterA
counterA(); // 3
```

### Closures for Data Privacy

JavaScript didn't always have private class fields. Closures were (and still are) the primary way to create private state:

```js
function createUserSession(userId) {
  // These variables are PRIVATE — no outside code can access them directly
  let accessToken = null;
  let refreshToken = null;
  let expiresAt = null;

  return {
    // Public interface
    login(tokens) {
      accessToken = tokens.access;
      refreshToken = tokens.refresh;
      expiresAt = Date.now() + tokens.expiresIn * 1000;
    },

    getAccessToken() {
      if (Date.now() > expiresAt) throw new Error("Token expired");
      return accessToken;
    },

    isAuthenticated() {
      return accessToken !== null && Date.now() < expiresAt;
    },

    logout() {
      accessToken = null;
      refreshToken = null;
      expiresAt = null;
    },
  };
}

const session = createUserSession("user_123");
session.login({ access: "abc", refresh: "xyz", expiresIn: 900 });
session.getAccessToken(); // "abc"
// session.accessToken — undefined! Private via closure.
```

### Closures for Function Factories

```js
// Create specialized validator functions
function createValidator(min, max) {
  return function validate(value) {
    if (typeof value !== "number") return "Must be a number";
    if (value < min) return `Must be at least ${min}`;
    if (value > max) return `Must be at most ${max}`;
    return null; // null = no error
  };
}

const validateAge   = createValidator(0, 150);
const validateScore = createValidator(0, 100);
const validatePort  = createValidator(1, 65535);

validateAge(25);    // null (valid)
validateAge(-1);    // "Must be at least 0"
validateScore(105); // "Must be at most 100"
validatePort(3000); // null (valid)
```

### Closures for Memoization

```js
// Cache expensive computation results
function memoize(fn) {
  const cache = new Map(); // closed over — lives as long as the returned function

  return function(...args) {
    const key = JSON.stringify(args);
    if (cache.has(key)) {
      return cache.get(key); // return cached result
    }
    const result = fn.apply(this, args);
    cache.set(key, result);
    return result;
  };
}

// Expensive function: hit the DB to get user permissions
const getUserPermissions = memoize(async (userId) => {
  const perms = await db.query("SELECT * FROM permissions WHERE user_id = $1", [userId]);
  return perms;
});

// First call: hits DB
await getUserPermissions("user_123");
// Second call: returns from cache
await getUserPermissions("user_123");
```

---

## 6. Object Methods

When a function is a property of an object, it's called a **method**.

### Defining Methods

```js
const userService = {
  users: [],

  // Method shorthand (ES6)
  addUser(user) {
    this.users.push(user);
  },

  // Traditional
  findById: function(id) {
    return this.users.find(u => u.id === id);
  },

  // Arrow function — DON'T use for object methods (no own `this`)
  getCount: () => {
    return this.users.length; // `this` is NOT userService here!
  },
};
```

### The `this` Keyword (Full Treatment in Part 6)

`this` refers to the **execution context** — the object that is calling the method.

```js
const cart = {
  items: [],
  total: 0,

  addItem(name, price, qty = 1) {
    this.items.push({ name, price, qty });  // `this` = cart
    this.total += price * qty;
    return this; // enables method chaining!
  },

  applyDiscount(percent) {
    this.total *= (1 - percent / 100);
    return this;
  },

  checkout() {
    console.log(`Total: $${this.total.toFixed(2)}`);
    return this;
  },
};

// Method chaining!
cart
  .addItem("Laptop", 999)
  .addItem("Mouse", 49, 2)
  .applyDiscount(10)
  .checkout();
```

### `this` Binding Problems

```js
const api = {
  baseUrl: "https://api.example.com",

  // Problem: passing method as callback loses `this` binding
  fetchUsers() {
    return fetch(`${this.baseUrl}/users`); // `this` = api ✓ when called as api.fetchUsers()
  },
};

// When used as a callback — `this` is lost
setTimeout(api.fetchUsers, 1000); // `this` inside = undefined (strict) or window

// Fix 1: Arrow function wrapper
setTimeout(() => api.fetchUsers(), 1000);

// Fix 2: .bind() — creates new function with `this` permanently bound
setTimeout(api.fetchUsers.bind(api), 1000);

// Fix 3: Store reference
const fetchUsers = api.fetchUsers.bind(api);
```

### call, apply, bind

```js
function greet(greeting, punctuation) {
  return `${greeting}, ${this.name}${punctuation}`;
}

const alice = { name: "Alice" };
const bob = { name: "Bob" };

// .call(thisArg, arg1, arg2, ...) — invoke immediately with specified `this`
greet.call(alice, "Hello", "!");  // "Hello, Alice!"
greet.call(bob, "Hi", ".");       // "Hi, Bob."

// .apply(thisArg, [args]) — same but args as array
greet.apply(alice, ["Hello", "!"]);

// .bind(thisArg, arg1, ...) — returns NEW function with `this` permanently bound
const greetAlice = greet.bind(alice, "Hello");
greetAlice("!"); // "Hello, Alice!" — partially applied too
```

---

## 7. JSON Handling

**JSON** (JavaScript Object Notation) is a text format for data. It's how virtually all web APIs exchange data.

### JSON Syntax Rules

JSON is stricter than JavaScript object literal syntax:

- Keys **must** be double-quoted strings
- Values can be: string, number, boolean, null, array, or object
- No functions, no undefined, no comments, no trailing commas, no single quotes

```json
{
  "id": 1,
  "name": "Alice Chen",
  "email": "alice@example.com",
  "isActive": true,
  "score": 98.6,
  "tags": ["admin", "editor"],
  "address": {
    "city": "New York",
    "country": "US"
  },
  "deletedAt": null
}
```

### `JSON.stringify` — JS → JSON String

```js
const user = {
  id: 1,
  name: "Alice",
  createdAt: new Date("2024-01-15"),
  password: "hashed_pw",
  getFullName: function() { return this.name; }, // will be dropped
  undefinedProp: undefined, // will be dropped
};

JSON.stringify(user);
// '{"id":1,"name":"Alice","createdAt":"2024-01-15T00:00:00.000Z"}'
// Note: function and undefined are omitted, Date becomes string

// With indentation (for readable output / saving to file)
JSON.stringify(user, null, 2);
// Pretty-printed with 2-space indent

// With replacer — filter/transform what gets serialized
JSON.stringify(user, ["id", "name"]); // only include id and name
JSON.stringify(user, (key, value) => {
  if (key === "password") return undefined; // omit password!
  return value;
});
```

### `JSON.parse` — JSON String → JS Object

```js
const jsonString = '{"id":1,"name":"Alice","isActive":true}';

const user = JSON.parse(jsonString);
user.name;     // "Alice"
user.isActive; // true

// With reviver function — transform values during parsing
const data = '{"createdAt":"2024-01-15T08:30:00.000Z","amount":"99.99"}';

const parsed = JSON.parse(data, (key, value) => {
  if (key === "createdAt") return new Date(value); // convert string → Date
  if (key === "amount") return parseFloat(value);  // convert string → number
  return value;
});

parsed.createdAt instanceof Date; // true ✓
parsed.amount === 99.99;          // true ✓
```

### Safe JSON Parsing

`JSON.parse` throws if the string is invalid JSON. Always wrap in try/catch:

```js
function safeJsonParse(str, fallback = null) {
  try {
    return JSON.parse(str);
  } catch {
    return fallback;
  }
}

// Real use: parsing stored user preferences
const storedPrefs = localStorage.getItem("userPreferences");
const preferences = safeJsonParse(storedPrefs, {
  theme: "light",
  language: "en",
  notifications: true,
});
```

### JSON in API Requests/Responses

```js
// Sending JSON in a POST request
async function createUser(userData) {
  const response = await fetch("/api/users", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",  // REQUIRED — tells server it's JSON
      "Authorization": `Bearer ${getToken()}`,
    },
    body: JSON.stringify(userData), // serialize the object
  });

  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.message || "Failed to create user");
  }

  return response.json(); // parse the JSON response
}

// Using it
await createUser({
  name: "Alice",
  email: "alice@example.com",
  role: "admin",
});
```

### `structuredClone` vs `JSON` for Cloning

```js
const original = {
  date: new Date(),
  regex: /pattern/gi,
  nested: { value: 42 },
  fn: () => "I am a function",
};

// JSON.parse(JSON.stringify(x)) — simple but lossy
const jsonClone = JSON.parse(JSON.stringify(original));
jsonClone.date instanceof Date;   // FALSE — became string
jsonClone.regex instanceof RegExp; // FALSE — became {}
jsonClone.fn;                      // undefined — dropped

// structuredClone — handles more types
const deepClone = structuredClone(original);
deepClone.date instanceof Date;    // TRUE ✓
deepClone.regex instanceof RegExp; // TRUE ✓
// Note: functions are STILL not cloneable by structuredClone
```

---

## 8. Dates & Time

The `Date` object is JavaScript's built-in way to work with dates and times.

### Creating Dates

```js
const now = new Date();                          // current date and time
const specific = new Date("2024-01-15");         // from ISO string (UTC midnight)
const specific2 = new Date("2024-01-15T08:30:00"); // with time (local timezone)
const specific3 = new Date("2024-01-15T08:30:00Z"); // UTC explicitly

// From components (year, month 0-11, day, hour, min, sec, ms)
const d = new Date(2024, 0, 15, 8, 30, 0); // Jan 15, 2024 08:30 (month is 0-indexed!)

// From Unix timestamp (milliseconds since Jan 1, 1970 UTC)
const fromTimestamp = new Date(1705312200000);

// Unix timestamp of now
Date.now(); // milliseconds, e.g. 1705312200000
```

**The month gotcha:** January = 0, December = 11. Always.

### Getting Parts of a Date

```js
const d = new Date("2024-06-15T14:30:45.500Z");

d.getFullYear()    // 2024 (local timezone)
d.getMonth()       // 5 (0-indexed — June!)
d.getDate()        // 15 (day of month)
d.getDay()         // 6 (0=Sunday, 6=Saturday)
d.getHours()       // 14 (local timezone)
d.getMinutes()     // 30
d.getSeconds()     // 45
d.getMilliseconds()// 500
d.getTime()        // Unix timestamp in ms

// UTC versions (same but in UTC, not local timezone)
d.getUTCFullYear()
d.getUTCMonth()
// etc.

d.toISOString()    // "2024-06-15T14:30:45.500Z" — standard format for APIs
d.toLocaleDateString()  // "6/15/2024" (locale-dependent)
d.toLocaleTimeString()  // "2:30:45 PM"
d.toLocaleString()      // "6/15/2024, 2:30:45 PM"
```

### Formatting Dates for Display

The `Intl.DateTimeFormat` API gives you locale-aware formatting:

```js
const date = new Date("2024-06-15T14:30:00Z");

// Display in a specific locale
new Intl.DateTimeFormat("en-US").format(date);   // "6/15/2024"
new Intl.DateTimeFormat("en-GB").format(date);   // "15/06/2024"
new Intl.DateTimeFormat("de-DE").format(date);   // "15.6.2024"

// With options
new Intl.DateTimeFormat("en-US", {
  year: "numeric",
  month: "long",
  day: "numeric",
  weekday: "long",
}).format(date);
// "Saturday, June 15, 2024"

new Intl.DateTimeFormat("en-US", {
  hour: "2-digit",
  minute: "2-digit",
  hour12: true,
}).format(date);
// "10:30 AM" (in local TZ)

// Relative time formatting
const rtf = new Intl.RelativeTimeFormat("en", { numeric: "auto" });
rtf.format(-1, "day");    // "yesterday"
rtf.format(2, "day");     // "in 2 days"
rtf.format(-3, "month");  // "3 months ago"
```

### Date Arithmetic

```js
const now = new Date();

// Add 7 days
const nextWeek = new Date(now.getTime() + 7 * 24 * 60 * 60 * 1000);

// Add 30 days (better method)
const inThirtyDays = new Date(now);
inThirtyDays.setDate(now.getDate() + 30);

// Difference between two dates (in ms, then convert)
const start = new Date("2024-01-01");
const end = new Date("2024-06-15");
const diffMs = end - start;
const diffDays = Math.floor(diffMs / (1000 * 60 * 60 * 24)); // 166 days

// Check if a date is in the past
function isExpired(date) {
  return new Date(date) < new Date();
}

// Check if a JWT is about to expire (within 5 minutes)
function isTokenAboutToExpire(expiresAt) {
  const fiveMinutes = 5 * 60 * 1000;
  return new Date(expiresAt) - Date.now() < fiveMinutes;
}
```

### Real-World: Dashboard Date Ranges

```js
function getDateRange(range) {
  const now = new Date();
  const start = new Date(now);

  switch (range) {
    case "today":
      start.setHours(0, 0, 0, 0); // start of today
      return { start, end: now };

    case "last7days":
      start.setDate(now.getDate() - 7);
      return { start, end: now };

    case "last30days":
      start.setDate(now.getDate() - 30);
      return { start, end: now };

    case "thisMonth":
      start.setDate(1);
      start.setHours(0, 0, 0, 0); // first of this month
      return { start, end: now };

    case "lastMonth": {
      const firstOfThisMonth = new Date(now.getFullYear(), now.getMonth(), 1);
      const firstOfLastMonth = new Date(now.getFullYear(), now.getMonth() - 1, 1);
      return { start: firstOfLastMonth, end: firstOfThisMonth };
    }

    default:
      throw new Error(`Unknown range: ${range}`);
  }
}

const { start, end } = getDateRange("last30days");
const sales = await getSalesBetween(start.toISOString(), end.toISOString());
```

**Recommendation:** For production apps, use a library like **date-fns** (tree-shakeable, functional) or **Day.js** (lightweight, Moment.js-compatible API). The native Date API has timezone issues and awkward APIs.

---

## 9. setInterval & setTimeout

JavaScript is single-threaded but the browser (and Node.js) provide timer APIs that schedule callbacks via the **Event Loop**.

### `setTimeout` — Run Once After a Delay

```js
// setTimeout(callback, delayMs, ...args)
const timerId = setTimeout(() => {
  console.log("Runs after 2 seconds");
}, 2000);

// Cancel it before it runs
clearTimeout(timerId);

// setTimeout with 0ms delay
setTimeout(() => {
  console.log("2: runs after current call stack clears");
}, 0);
console.log("1: runs first");
// Output: "1: runs first", then "2: runs after..."
// The 0ms timeout doesn't run "immediately" — it queues in the event loop
```

### `setInterval` — Run Repeatedly

```js
// setInterval(callback, intervalMs)
const intervalId = setInterval(() => {
  console.log("Runs every second");
}, 1000);

// ALWAYS store the ID so you can stop it
// Clear it when done — failing to clear is a common memory leak
clearInterval(intervalId);

// Real-world: auto-refresh dashboard data every 30 seconds
let refreshInterval;

function startAutoRefresh() {
  // Clear any existing interval first (prevent duplicates)
  stopAutoRefresh();

  refreshInterval = setInterval(async () => {
    try {
      const data = await fetchDashboardData();
      updateUI(data);
    } catch (err) {
      console.error("Auto-refresh failed:", err);
    }
  }, 30_000); // 30 seconds — underscore separator for readability
}

function stopAutoRefresh() {
  if (refreshInterval) {
    clearInterval(refreshInterval);
    refreshInterval = null;
  }
}

// Start when page loads, stop when user leaves
document.addEventListener("visibilitychange", () => {
  if (document.hidden) {
    stopAutoRefresh(); // don't refresh when tab is hidden — save resources
  } else {
    startAutoRefresh();
  }
});
```

### Session Expiry Warning

```js
// Show a warning 5 minutes before the session expires
function setupSessionWarning(expiresAt) {
  const msUntilExpiry = new Date(expiresAt) - Date.now();
  const warningTime = msUntilExpiry - 5 * 60 * 1000; // 5 min before

  if (warningTime <= 0) {
    handleSessionExpired();
    return;
  }

  const warningTimer = setTimeout(() => {
    showBanner("Your session expires in 5 minutes. Save your work!");
  }, warningTime);

  const expiryTimer = setTimeout(() => {
    handleSessionExpired();
    clearTimeout(warningTimer); // safety
  }, msUntilExpiry);

  // Return cleanup function
  return () => {
    clearTimeout(warningTimer);
    clearTimeout(expiryTimer);
  };
}
```

### Debounce with setTimeout

**Debouncing** means waiting until a burst of events has stopped before acting:

```js
function debounce(fn, delay) {
  let timerId;

  return function(...args) {
    clearTimeout(timerId); // reset the timer on every call
    timerId = setTimeout(() => {
      fn.apply(this, args);
    }, delay);
  };
}

// Real-world: search input — wait until user stops typing (500ms)
const handleSearch = debounce(async (query) => {
  if (!query.trim()) return;
  const results = await searchProducts(query);
  renderResults(results);
}, 500);

searchInput.addEventListener("input", (e) => {
  handleSearch(e.target.value);
});
// Without debounce: fires for every keystroke — could be 100s of API calls
// With debounce: fires 500ms after the last keystroke — just 1 call
```

### Throttle with setTimeout

**Throttling** means executing at most once per time window, ignoring extra calls:

```js
function throttle(fn, limit) {
  let lastRun = 0;

  return function(...args) {
    const now = Date.now();
    if (now - lastRun >= limit) {
      lastRun = now;
      fn.apply(this, args);
    }
  };
}

// Real-world: track scroll position — max once every 200ms
const trackScroll = throttle(() => {
  const scrollPercent = window.scrollY / document.body.scrollHeight;
  updateProgressBar(scrollPercent);
}, 200);

window.addEventListener("scroll", trackScroll);
```

### The Event Loop (How Timers Really Work)

```js
console.log("1 — synchronous");

setTimeout(() => console.log("3 — setTimeout 0ms"), 0);

Promise.resolve().then(() => console.log("2 — microtask"));

console.log("4 — still synchronous");

// Output order: 1, 4, 2, 3
```

Why? The event loop prioritizes:

1. **Current call stack** — synchronous code (1 and 4)
2. **Microtask queue** — Promises, queueMicrotask (2)
3. **Task queue (macrotask queue)** — setTimeout, setInterval, I/O (3)

Microtasks (Promises) always run before the next macrotask (timer).

```
Call Stack Empty?
  → Drain entire Microtask Queue (all .then(), all awaits)
  → Pick one task from Macrotask Queue (setTimeout, setInterval, I/O)
  → Run it → drain Microtask Queue again → repeat
```

---

## Summary Cheat Sheet

```
Functions:
  Declaration:  hoisted, named, has own `this`
  Expression:   not hoisted, can be named for recursion
  Arrow:        concise, NO own `this`, NO arguments object
  Rest:         (...args) collects remaining into array
  Default:      param = value, triggered by undefined (not null)
  IIFE:         (function(){})() — isolated scope

Callbacks:
  Are just functions passed as arguments
  Error-first convention: (err, result) in Node.js
  Foundation of all higher-order functions and async

Scope:
  Block:    let/const inside {} 
  Function: all variables inside a function
  Lexical:  determined at write time, not call time
  Module:   file-level with ES modules

Closures:
  Function remembers its outer scope after parent returns
  Use for: private state, factories, memoization, debounce/throttle

JSON:
  stringify(obj, replacer, indent) → string
  parse(str, reviver) → object
  Always wrap parse in try/catch

Timers:
  setTimeout(fn, ms)  → runs once after delay, returns ID
  setInterval(fn, ms) → runs repeatedly, returns ID
  clearTimeout/clearInterval(id) → cancel
  Debounce: delay action until burst stops
  Throttle: max once per time window
  Microtasks (Promises) before Macrotasks (timers)
```

---

_Next: [Part 4 — Modern JavaScript: ES6+ Features](./part-4-modern-js.md)_