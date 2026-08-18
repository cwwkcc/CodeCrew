> Closures are the most important concept in JavaScript that most people learn wrong. They're not a "feature" — they're how JavaScript _works_. Every function you write uses closures. Once you truly understand them, a whole class of confusing bugs disappears.

---

## Table of Contents

1. [What a Closure Actually Is](#1-what-a-closure-actually-is)
2. [Lexical Scope — The Foundation](#2-lexical-scope--the-foundation)
3. [How Closures Are Created](#3-how-closures-are-created)
4. [Closures Over Time — The Classic Bug](#4-closures-over-time--the-classic-bug)
5. [Practical Patterns: Data Privacy](#5-practical-patterns-data-privacy)
6. [Practical Patterns: Function Factories](#6-practical-patterns-function-factories)
7. [Practical Patterns: Memoisation](#7-practical-patterns-memoisation)
8. [Practical Patterns: Partial Application](#8-practical-patterns-partial-application)
9. [Closures and the Event Loop](#9-closures-and-the-event-loop)
10. [Closures in React (useState, useEffect, useCallback)](#10-closures-in-react-usestate-useeffect-usecallback)
11. [Memory and Closures](#11-memory-and-closures)

---

## 1. What a Closure Actually Is

A **closure** is a function that remembers the variables from the scope where it was _defined_, even after that scope has finished executing.

Every function in JavaScript is a closure. There is no on/off switch — closures are always happening.

```javascript
function makeCounter() {
  let count = 0; // this variable lives in makeCounter's scope

  function increment() {
    count++; // increment "closes over" count
    console.log(count);
  }

  return increment;
}

const counter = makeCounter();
// makeCounter() has finished executing.
// Normally, `count` would be garbage collected.
// But `increment` still references it — so it lives on.

counter(); // 1
counter(); // 2
counter(); // 3
```

`makeCounter()` returned `increment`. The `increment` function carries a **reference to the `count` variable** with it — that binding is the closure.

The key insight: closures capture **variables**, not values. `count` is a live variable — when it changes, `increment` sees the new value.

---

## 2. Lexical Scope — The Foundation

**Lexical scope** means: a function can access variables from the scope where it was _written_, not where it was _called_.

```javascript
const name = "Global";

function outer() {
  const name = "Outer";

  function inner() {
    console.log(name); // "Outer" — inner sees where IT was defined
  }

  return inner;
}

const fn = outer();
fn(); // "Outer" — NOT "Global", even though fn is called in global scope
```

JavaScript resolves variable names by walking up the **scope chain** — starting at the current function, then its enclosing function, all the way up to global scope.

```
Scope chain for `inner`:

inner's scope: { }                  (no `name` here)
       ↓
outer's scope: { name: "Outer" }    ← found! Use this.
       ↓
global scope:  { name: "Global" }   (never reached)
```

This resolution happens at **write time** (when the code is parsed), not at call time. That's what "lexical" means — it's based on the location in the source code.

---

## 3. How Closures Are Created

A closure is created every time a function is created that references variables from its outer scope.

```javascript
function outer() {
  let x = 10;

  // This function is a closure — it closes over `x`
  const inner = function () {
    return x * 2;
  };

  return inner;
}

// Arrow functions are closures too
function makeMultiplier(factor) {
  return (n) => n * factor; // closes over `factor`
}

const double = makeMultiplier(2);
const triple = makeMultiplier(3);

double(5); // 10
triple(5); // 15
// double and triple are separate closures, each with their own `factor`
```

### Each Closure Has Its Own Scope

```javascript
function makeCounter() {
  let count = 0;
  return {
    increment: () => ++count,
    decrement: () => --count,
    value: () => count,
  };
}

const counterA = makeCounter();
const counterB = makeCounter();

counterA.increment();
counterA.increment();
counterA.increment();
counterB.increment();

console.log(counterA.value()); // 3
console.log(counterB.value()); // 1
// Each call to makeCounter creates a new scope with its own `count`
// counterA and counterB are completely independent
```

---

## 4. Closures Over Time — The Classic Bug

Because closures capture **variables** (not values), there's a classic bug that catches everyone the first time.

```javascript
// THE CLASSIC BUG
for (var i = 0; i < 3; i++) {
  setTimeout(() => {
    console.log(i); // you expect 0, 1, 2
  }, 100);
}

// Actual output:
// 3
// 3
// 3
```

**Why?** `var` is function-scoped (or global if not in a function). There is only **one** `i` variable, shared by all three closures. By the time the `setTimeout` callbacks run (100ms later), the loop has finished and `i` is `3`. All three closures see the same `i`— and it's `3`.

```
After the loop:       i = 3

setTimeout fires →  closure 1 reads `i` → 3
setTimeout fires →  closure 2 reads `i` → 3
setTimeout fires →  closure 3 reads `i` → 3
```

### Fix 1: Use `let` (block-scoped)

```javascript
for (let i = 0; i < 3; i++) {
  setTimeout(() => {
    console.log(i); // 0, 1, 2 ✓
  }, 100);
}
// `let` creates a NEW `i` binding for each iteration of the loop
// Each closure captures a different variable
```

### Fix 2: IIFE (Immediately Invoked Function Expression) — pre-ES6 pattern

```javascript
for (var i = 0; i < 3; i++) {
  (function (j) {
    // j is a NEW variable per iteration
    setTimeout(() => {
      console.log(j); // 0, 1, 2 ✓
    }, 100);
  })(i); // immediately called with the current value of i
}
```

### Fix 3: Capture in a variable

```javascript
for (var i = 0; i < 3; i++) {
  const captured = i; // const creates a new binding each iteration
  setTimeout(() => {
    console.log(captured); // 0, 1, 2 ✓
  }, 100);
}
```

---

## 5. Practical Patterns: Data Privacy

Closures are JavaScript's mechanism for private state — variables that exist but cannot be accessed from outside.

```javascript
// Module pattern: public API, private state
function createUserSession() {
  // These are PRIVATE — no code outside this function can touch them
  let userId = null;
  let token = null;
  let expiresAt = null;

  // Only these returned functions have access
  return {
    login(id, authToken, ttlMs) {
      userId = id;
      token = authToken;
      expiresAt = Date.now() + ttlMs;
    },

    logout() {
      userId = null;
      token = null;
      expiresAt = null;
    },

    isLoggedIn() {
      return userId !== null && Date.now() < expiresAt;
    },

    getToken() {
      if (!this.isLoggedIn()) throw new Error("Not authenticated");
      return token;
    },

    // userId is never exposed directly — only through controlled methods
    getUserId() {
      if (!this.isLoggedIn()) return null;
      return userId;
    },
  };
}

const session = createUserSession();
session.login("u123", "eyJhbGc...", 3600_000);

console.log(session.isLoggedIn()); // true
console.log(session.getToken()); // "eyJhbGc..."

// Cannot access private state directly:
console.log(session.userId); // undefined — not exposed
console.log(session.token); // undefined — not exposed
```

---

## 6. Practical Patterns: Function Factories

A function factory returns specialised functions, each with their own closed-over configuration.

```javascript
// Validator factory
function createValidator(rules) {
  return function validate(data) {
    const errors = {};

    for (const [field, fieldRules] of Object.entries(rules)) {
      for (const rule of fieldRules) {
        const error = rule(data[field], field);
        if (error) {
          errors[field] = error;
          break;
        }
      }
    }

    return {
      valid: Object.keys(errors).length === 0,
      errors,
    };
  };
}

// Rules — pure functions that return an error string or null
const required = (value, field) =>
  value == null || value === "" ? `${field} is required` : null;

const minLength = (min) => (value, field) =>
  value && value.length < min
    ? `${field} must be at least ${min} characters`
    : null;

const isEmail = (value, field) =>
  value && !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(value)
    ? `${field} must be a valid email`
    : null;

// Create specialised validators — each closes over its own `rules`
const validateUser = createValidator({
  name: [required, minLength(2)],
  email: [required, isEmail],
  password: [required, minLength(8)],
});

const validateLoginForm = createValidator({
  email: [required, isEmail],
  password: [required],
});

// Usage
const result = validateUser({
  name: "D",
  email: "not-email",
  password: "1234",
});
console.log(result);
// {
//   valid: false,
//   errors: {
//     name: "name must be at least 2 characters",
//     email: "email must be a valid email",
//     password: "password must be at least 8 characters",
//   }
// }
```

---

## 7. Practical Patterns: Memoisation

Memoisation caches function results — if the same arguments are passed again, return the cached result instead of recomputing.

```javascript
function memoize(fn) {
  const cache = new Map(); // closed over — persists between calls

  return function (...args) {
    const key = JSON.stringify(args);

    if (cache.has(key)) {
      return cache.get(key);
    }

    const result = fn.apply(this, args);
    cache.set(key, result);
    return result;
  };
}

// Expensive computation
function fibonacci(n) {
  if (n <= 1) return n;
  return fibonacci(n - 1) + fibonacci(n - 2);
}

const memoFib = memoize(fibonacci);

console.time("first");
memoFib(40); // computes — slow
console.timeEnd("first"); // ~1000ms

console.time("second");
memoFib(40); // returns cached result immediately
console.timeEnd("second"); // ~0ms
```

```javascript
// Real-world: memoising an API call
function createApiClient(baseUrl) {
  const cache = new Map();

  return {
    async get(endpoint, ttlMs = 60_000) {
      const cacheKey = endpoint;
      const cached = cache.get(cacheKey);

      if (cached && Date.now() < cached.expiresAt) {
        return cached.data; // return cached, skip network
      }

      const response = await fetch(`${baseUrl}${endpoint}`);
      const data = await response.json();

      cache.set(cacheKey, {
        data,
        expiresAt: Date.now() + ttlMs,
      });

      return data;
    },

    invalidate(endpoint) {
      cache.delete(endpoint);
    },
  };
}

const api = createApiClient("https://api.cwwkcc.lk");
await api.get("/students"); // hits network
await api.get("/students"); // returns cache — instant
```

---

## 8. Practical Patterns: Partial Application

Partial application creates a new function with some arguments pre-filled.

```javascript
function partial(fn, ...presetArgs) {
  return function (...laterArgs) {
    return fn(...presetArgs, ...laterArgs);
  };
}

// General-purpose fetch wrapper
async function apiRequest(method, baseUrl, endpoint, data) {
  const response = await fetch(`${baseUrl}${endpoint}`, {
    method,
    body: data ? JSON.stringify(data) : undefined,
    headers: { "Content-Type": "application/json" },
  });
  return response.json();
}

// Specialised functions via partial application
const getFromApi = partial(apiRequest, "GET", "https://api.example.com");
const postToApi = partial(apiRequest, "POST", "https://api.example.com");

// Clean call sites
await getFromApi("/users");
await postToApi("/users", { name: "Alice", email: "alice@example.com" });

// Another example: event logging
function logEvent(level, service, message) {
  console.log(`[${level}] [${service}] ${message}`);
}

const logInfo = partial(logEvent, "INFO", "AuthService");
const logError = partial(logEvent, "ERROR", "AuthService");

logInfo("User logged in"); // [INFO] [AuthService] User logged in
logError("Token verification failed"); // [ERROR] [AuthService] Token verification failed
```

---

## 9. Closures and the Event Loop

Understanding closures and the event loop together explains async behaviour that otherwise seems magical.

```javascript
let result = null;

function fetchData() {
  setTimeout(() => {
    result = "data loaded"; // closure modifies outer `result`
  }, 1000);
}

fetchData();
console.log(result); // null — setTimeout hasn't fired yet
// The closure will update `result` after 1 second
// but the synchronous code has already moved on

// To use the result:
setTimeout(() => {
  console.log(result); // "data loaded" — this closure runs after the first
}, 1500);
```

```javascript
// Stale closures in async code
async function handleSubmit() {
  const formData = { name: "Alice" };

  await saveToDatabase(formData); // takes 2 seconds

  // By the time this runs, is formData still what we expect?
  // YES — formData is in the closure and hasn't changed
  console.log("Saved:", formData.name); // "Alice"

  // But if formData was a mutable reference shared elsewhere:
  // formData might have been mutated during the await
  // Always be careful with shared mutable state in async functions
}
```

---

## 10. Closures in React (useState, useEffect, useCallback)

React relies on closures deeply. Most React bugs involving stale data are closure bugs.

### The Stale Closure Problem in useEffect

```javascript
import { useState, useEffect } from "react";

function Counter() {
  const [count, setCount] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      // This closure captured `count` when the effect ran (on mount)
      // `count` is 0 — it never updates inside this closure
      console.log("count:", count); // always 0!
      setCount(count + 1); // always sets to 0 + 1 = 1
    }, 1000);

    return () => clearInterval(interval);
  }, []); // empty deps — effect runs only once → count is stale

  return <div>{count}</div>; // stuck at 1
}

// FIX: use the functional update form (doesn't need the current count)
useEffect(() => {
  const interval = setInterval(() => {
    setCount((prev) => prev + 1); // `prev` is always the latest value
  }, 1000);

  return () => clearInterval(interval);
}, []); // safe to leave empty now
```

### The Stale Closure in useCallback

```javascript
import { useState, useCallback } from "react";

function SearchComponent({ onSearch }) {
  const [query, setQuery] = useState("");

  // STALE: this closes over `query` when created
  // If query changes, the callback is NOT recreated (empty deps)
  // → it always sends the initial empty string
  const handleSubmit = useCallback(() => {
    onSearch(query); // query is stale
  }, []); // missing dependency!

  // FIX: include all closed-over values in the deps array
  const handleSubmitFixed = useCallback(() => {
    onSearch(query);
  }, [query, onSearch]); // recreated whenever query changes

  return (
    <div>
      <input value={query} onChange={(e) => setQuery(e.target.value)} />
      <button onClick={handleSubmitFixed}>Search</button>
    </div>
  );
}
```

### Why React's Exhaustive Deps Rule Exists

The `eslint-plugin-react-hooks` rule `exhaustive-deps` warns you when your `useEffect`/`useCallback`/`useMemo` is closing over values not listed in the deps array. This is a **closure bug detector**. When you ignore it, you get stale closures.

```javascript
// The lint rule catches this:
const [userId, setUserId] = useState(null);

useEffect(() => {
  fetchUserData(userId); // closes over userId
}, []);
// ⚠️ Warning: React Hook useEffect has a missing dependency: 'userId'
// If userId changes, the effect won't re-run → stale userId
```

---

## 11. Memory and Closures

Closures keep their closed-over variables alive as long as the closure itself is alive. This is correct behaviour, but it can cause accidental memory retention.

```javascript
// Each call to makeHandler creates a closure that holds `cache`
function makeHandler(largeConfig) {
  const cache = buildCache(largeConfig); // potentially large

  return function handler(request) {
    // handler closes over `cache`
    return processRequest(request, cache);
  };
}

// If you create many handlers and never discard them:
const handlers = [];
for (let i = 0; i < 1000; i++) {
  handlers.push(makeHandler(configs[i]));
}
// 1000 closures, each holding a `cache` → large memory footprint
// handlers array keeps all caches alive as long as it exists

// FIX: discard handlers when done
handlers.length = 0; // or handlers.splice(0) — caches can now be GC'd
```

```javascript
// Pattern: lazily close over only what's needed

// BAD — closes over entire `options` object (even fields not needed)
function createLogger(options) {
  return function log(message) {
    if (options.debug) {
      // only needs one field
      console.log(`[DEBUG] [${options.service}] ${message}`);
    }
  };
}

// BETTER — destructure immediately, close over only what's needed
function createLogger({ debug, service }) {
  return function log(message) {
    if (debug) {
      console.log(`[DEBUG] [${service}] ${message}`);
    }
  };
  // `options` is not in the closure — can be GC'd
}
```

---

## Summary

```
What a closure is:
  A function that retains access to variables from its defining scope
  Even after that scope's execution has finished
  Closures capture VARIABLES (live bindings), not snapshots of values

Lexical scope:
  Variable resolution based on WHERE the function is written
  Scope chain: current scope → enclosing scope → ... → global scope
  Determined at parse time, not call time

Classic closure bug:
  var in loops — one shared variable, all closures see the final value
  Fix: use let (block-scoped) or capture in an IIFE/const

Practical closure patterns:
  Data privacy:        private variables inaccessible from outside
  Function factories:  specialised functions sharing configuration
  Memoisation:         caching results in a closed-over Map
  Partial application: pre-filling arguments for cleaner call sites

React and closures:
  useState/useEffect/useCallback all involve closures
  Stale closure = closure captured an old value of a variable
  Fix stale closures: add to deps array, or use functional setState

Memory:
  Closures keep closed-over variables alive — watch for accidental retention
  Unsubscribe listeners, clear intervals, null references when done
```

---

_Next: [09 — Prototypes & the Prototype Chain](./09%20-%20Prototypes%20%26%20the%20Prototype%20Chain.md)_
