> Functions are more than reusable blocks of code in JavaScript — they are first-class values that can be passed, returned, and stored. This file deepens your understanding of functions: how scope really works, what closures are (first look), how to compose behaviour with higher-order functions, and the practical APIs for JSON, dates, and timing.

---

## Table of Contents

1. [Functions as First-Class Values](#1-functions-as-first-class-values)
2. [Higher-Order Functions](#2-higher-order-functions)
3. [Closures — First Look](#3-closures--first-look)
4. [The `this` Keyword](#4-the-this-keyword)
5. [call, apply, bind](#5-call-apply-bind)
6. [JSON](#6-json)
7. [Dates](#7-dates)
8. [Timers](#8-timers)

---

## 1. Functions as First-Class Values

In JavaScript, functions are values — the same as numbers, strings, and objects. This means:

```javascript
// Store a function in a variable
const greet = function(name) { return `Hello, ${name}`; };

// Store in an object property (method)
const user = {
  name: "Ashan",
  greet() { return `Hello, I'm ${this.name}`; },
};

// Store in an array
const handlers = [
  (x) => x * 2,
  (x) => x + 10,
  (x) => x ** 2,
];
handlers[0](5);  // 10

// Pass a function as an argument
function applyTwice(fn, value) {
  return fn(fn(value));
}
applyTwice(x => x * 2, 3);  // 12  (3 → 6 → 12)

// Return a function from a function
function makeMultiplier(factor) {
  return function(n) { return n * factor; };
}
const triple = makeMultiplier(3);
triple(5);   // 15
triple(10);  // 30
```

This is the foundation of functional programming in JavaScript and explains how React hooks, array methods, and event handlers all work.

---

## 2. Higher-Order Functions

A **higher-order function** is a function that takes another function as an argument, or returns a function.

You've already seen the built-in higher-order functions: `map`, `filter`, `reduce`, `forEach`, `sort`. Here's how to write your own.

```javascript
// Callback pattern
function retry(fn, maxAttempts, delay) {
  let attempts = 0;

  return async function(...args) {
    while (attempts < maxAttempts) {
      try {
        return await fn(...args);
      } catch (err) {
        attempts++;
        if (attempts === maxAttempts) throw err;
        await new Promise(resolve => setTimeout(resolve, delay));
      }
    }
  };
}

const fetchWithRetry = retry(fetchUserData, 3, 1000);
await fetchWithRetry("u123");

// Composition — apply functions in sequence
const compose = (...fns) => x => fns.reduceRight((v, f) => f(v), x);
const pipe    = (...fns) => x => fns.reduce((v, f) => f(v), x);

const processName = pipe(
  s => s.trim(),
  s => s.toLowerCase(),
  s => s.replace(/\s+/g, "-"),
);
processName("  Hello World  ");  // "hello-world"
```

### `forEach`

```javascript
// forEach — iterate with a callback, returns undefined
// Use when you want side effects, not a new array
const students = ["Ashan", "Dineth", "Kavya"];

students.forEach((name, index) => {
  console.log(`${index + 1}. ${name}`);
});
// 1. Ashan
// 2. Dineth
// 3. Kavya

// forEach cannot be stopped with break — use for...of if you need break
// forEach doesn't return anything — use map if you need a new array
```

---

## 3. Closures — First Look

A closure is a function that remembers the variables from the scope where it was **defined**, even after that scope has finished executing.

```javascript
function makeCounter(start = 0) {
  let count = start;  // this variable is "closed over"

  return {
    increment() { return ++count; },
    decrement() { return --count; },
    reset()     { count = start; },
    value()     { return count; },
  };
}

const counter = makeCounter(10);
counter.increment();  // 11
counter.increment();  // 12
counter.decrement();  // 11
counter.value();      // 11

// count is private — inaccessible from outside
counter.count;        // undefined
```

Closures are used constantly in JavaScript: for data privacy, for factory functions, for event handlers that remember context. The full treatment is in File 08.

---

## 4. The `this` Keyword

`this` refers to the object that is the current context of execution. Its value depends on **how** a function is called, not where it's defined (except for arrow functions).

```javascript
// 1. Method call — `this` is the object before the dot
const user = {
  name: "Ashan",
  greet() {
    return `Hello, I'm ${this.name}`;
  },
};
user.greet();  // "Hello, I'm Ashan" — this = user

// 2. Plain function call — `this` is undefined (strict mode) or global
function standalone() {
  console.log(this);
}
standalone();  // undefined (in strict mode / ES modules)

// 3. Arrow functions — `this` is inherited from the enclosing scope
const obj = {
  name: "Ashan",
  greet: () => {
    // Arrow function: this is NOT obj, it's the surrounding scope
    return `Hello, I'm ${this?.name}`;  // this.name is undefined
  },
  greetCorrect() {
    // Regular method — this works
    const inner = () => `Inner sees: ${this.name}`;
    // Arrow inside method: inherits method's `this`
    return inner();
  },
};
obj.greet();         // "Hello, I'm undefined" — arrow ignores obj
obj.greetCorrect();  // "Inner sees: Ashan" — arrow inherits method's this

// 4. Context loss — common bug
const greet = user.greet;  // detached from object
greet();  // undefined — `this` is no longer `user`

// Fix: bind, arrow wrapper, or class field arrow (covered in File 09)
```

---

## 5. `call`, `apply`, `bind`

These methods let you control what `this` is inside a function.

```javascript
function greet(greeting, punctuation) {
  return `${greeting}, I'm ${this.name}${punctuation}`;
}

const user = { name: "Ashan" };

// call — invoke with explicit this, args passed individually
greet.call(user, "Hello", "!");      // "Hello, I'm Ashan!"

// apply — invoke with explicit this, args passed as array
greet.apply(user, ["Hello", "!"]);   // "Hello, I'm Ashan!"

// bind — returns a NEW function with this permanently set
const boundGreet = greet.bind(user);
boundGreet("Hey", ".");              // "Hey, I'm Ashan."

// bind with pre-filled args (partial application)
const helloGreet = greet.bind(user, "Hello");
helloGreet("!");  // "Hello, I'm Ashan!"
helloGreet("?");  // "Hello, I'm Ashan?"
```

---

## 6. JSON

JSON (JavaScript Object Notation) is the universal format for exchanging data between a client and a server.

```javascript
// JSON.stringify — JS value → JSON string
const student = {
  name: "Ashan",
  grade: 11,
  scores: [82, 91, 67],
  isActive: true,
  address: { city: "Mathugama" },
};

const json = JSON.stringify(student);
// '{"name":"Ashan","grade":11,"scores":[82,91,67],"isActive":true,"address":{"city":"Mathugama"}}'

// Pretty-print with indentation
JSON.stringify(student, null, 2);
// {
//   "name": "Ashan",
//   "grade": 11,
//   ...
// }

// Selective fields (replacer array)
JSON.stringify(student, ["name", "grade"]);
// '{"name":"Ashan","grade":11}'

// JSON.parse — JSON string → JS value
const parsed = JSON.parse(json);
parsed.name;         // "Ashan"
parsed.scores[0];    // 82
```

### What JSON Can and Cannot Represent

```javascript
// JSON supports:
//   strings, numbers, booleans, null, arrays, objects

// JSON does NOT support — these are dropped or converted:
JSON.stringify({ fn: () => "hello" });       // '{}'  — functions dropped
JSON.stringify({ date: new Date() });         // '{"date":"2026-03-15T..."}' — Date → string
JSON.stringify({ undef: undefined });         // '{}'  — undefined dropped
JSON.stringify({ inf: Infinity });            // '{"inf":null}' — Infinity → null
JSON.stringify({ nan: NaN });                 // '{"nan":null}' — NaN → null

// Deep clone using JSON (simple but lossy)
const clone = JSON.parse(JSON.stringify(obj));
// Loses: functions, undefined, Date objects, Infinity, NaN, Map, Set
// Use structuredClone() for a proper deep clone
const betterClone = structuredClone(obj);  // preserves more types
```

### Error Handling

```javascript
// JSON.parse throws SyntaxError on invalid JSON
try {
  const data = JSON.parse(userInput);
  processData(data);
} catch (err) {
  if (err instanceof SyntaxError) {
    console.error("Invalid JSON:", err.message);
  }
}
```

---

## 7. Dates

JavaScript's `Date` object is notoriously awkward. Know its quirks.

```javascript
// Creating dates
const now = new Date();                          // current date/time
const specific = new Date("2026-01-15");         // from ISO string
const fromMs = new Date(1673740800000);          // from Unix timestamp (ms)
const explicit = new Date(2026, 0, 15, 10, 30); // year, month (0-indexed!), day, hour, min

// WARNING: month is 0-indexed
// January = 0, February = 1, ..., December = 11

// Getting components
const d = new Date("2026-03-15T10:30:00");
d.getFullYear();    // 2026
d.getMonth();       // 2 (March — 0-indexed!)
d.getDate();        // 15 (day of month)
d.getDay();         // 0-6 (0 = Sunday)
d.getHours();       // 10
d.getMinutes();     // 30
d.getSeconds();     // 0
d.getTime();        // Unix timestamp in milliseconds
d.toISOString();    // "2026-03-15T10:30:00.000Z" — best for storage

// Comparing dates — compare timestamps
const d1 = new Date("2026-01-01");
const d2 = new Date("2026-06-01");
d1 < d2;   // true
d1.getTime() === d2.getTime();  // false

// Date arithmetic — work in milliseconds
const ONE_DAY_MS = 24 * 60 * 60 * 1000;  // 86400000
const tomorrow = new Date(Date.now() + ONE_DAY_MS);
const daysUntil = (targetDate) => {
  const diff = targetDate.getTime() - Date.now();
  return Math.ceil(diff / ONE_DAY_MS);
};

// Formatting — use Intl.DateTimeFormat for locale-aware output
const formatter = new Intl.DateTimeFormat("en-LK", {
  year: "numeric",
  month: "long",
  day: "numeric",
});
formatter.format(new Date());  // "March 15, 2026"

// Or use toLocaleDateString
new Date().toLocaleDateString("si-LK");  // Sinhala locale
new Date().toLocaleDateString("en-US", { weekday: "long", month: "long", day: "numeric" });
// "Saturday, March 15"

// For serious date work, use a library like date-fns or Temporal API (upcoming)
```

---

## 8. Timers

JavaScript timers are not guaranteed to run exactly on time — they're scheduled for "at least this long from now" and run on the event loop.

### `setTimeout` — run once after a delay

```javascript
// Basic
const timerId = setTimeout(() => {
  console.log("This runs after 1 second");
}, 1000);

// Cancel before it fires
clearTimeout(timerId);

// With arguments
setTimeout((name, score) => {
  console.log(`${name}: ${score}`);
}, 500, "Ashan", 82);

// 0ms delay — runs after current synchronous code completes
setTimeout(() => console.log("deferred"), 0);
console.log("synchronous");
// Output: "synchronous" then "deferred"
// — setTimeout is always async, even with 0ms
```

### `setInterval` — run repeatedly

```javascript
let count = 0;

const intervalId = setInterval(() => {
  count++;
  console.log(`Tick ${count}`);

  if (count >= 5) {
    clearInterval(intervalId);  // stop after 5 ticks
    console.log("Done");
  }
}, 1000);

// Real-world: polling for updates
function startPolling(fetchFn, interval = 30_000) {
  fetchFn();  // run immediately

  const id = setInterval(fetchFn, interval);

  return () => clearInterval(id);  // return a cleanup function
}

const stopPolling = startPolling(() => refreshNotifications(), 30_000);
// Later: stopPolling() — stop polling
```

### Debounce and Throttle

These are critical patterns for performance with events that fire rapidly (input, scroll, resize).

```javascript
// Debounce — wait until the user stops firing events for N ms
function debounce(fn, delay) {
  let timerId;
  return function(...args) {
    clearTimeout(timerId);  // cancel previous timer
    timerId = setTimeout(() => fn.apply(this, args), delay);
  };
}

// Usage: search input — only search 300ms after typing stops
const search = debounce(async (query) => {
  const results = await fetchResults(query);
  renderResults(results);
}, 300);

inputElement.addEventListener("input", (e) => search(e.target.value));

// Throttle — run at most once every N ms
function throttle(fn, limit) {
  let lastRun = 0;
  return function(...args) {
    const now = Date.now();
    if (now - lastRun >= limit) {
      lastRun = now;
      return fn.apply(this, args);
    }
  };
}

// Usage: scroll handler — only runs every 100ms max
const onScroll = throttle(() => {
  updateScrollPosition();
}, 100);

window.addEventListener("scroll", onScroll);
```

### `requestAnimationFrame` — for animations

```javascript
// For smooth animations — synced to browser's refresh rate (~60fps)
// Not setTimeout(fn, 16) — that's less accurate and more CPU-intensive

function animate(element) {
  let start = null;
  const duration = 1000;  // 1 second

  function step(timestamp) {
    if (!start) start = timestamp;
    const progress = Math.min((timestamp - start) / duration, 1);

    element.style.opacity = progress;  // fade in over 1 second

    if (progress < 1) {
      requestAnimationFrame(step);     // schedule next frame
    }
  }

  requestAnimationFrame(step);
}
```

---

## Summary

```
Functions as values:
  Assign to variables, pass as arguments, return from functions
  This enables: callbacks, higher-order functions, functional composition

Higher-order functions:
  Takes a function as argument OR returns a function
  Examples: map, filter, reduce, forEach, sort

Closures:
  Functions remember variables from their defining scope
  Used for: private state, factory functions, event handlers

this keyword:
  Method call: this = the object before the dot
  Plain function: this = undefined (strict mode)
  Arrow function: this = lexically inherited (from enclosing scope)
  Context loss: detaching a method loses its this

call/apply/bind:
  All three set `this` explicitly
  call(this, arg1, arg2)   — invoke immediately, args listed
  apply(this, [arg1, arg2]) — invoke immediately, args as array
  bind(this, ...args)      — return new function with this set

JSON:
  stringify: JS value → JSON string
  parse: JSON string → JS value
  What's lost: functions, undefined, Date (→ string), Infinity/NaN (→ null)
  Error: JSON.parse throws SyntaxError on invalid input

Dates:
  new Date() — current time
  new Date("ISO string") — preferred input format
  Month is 0-indexed — January = 0
  .toISOString() — best format for storage/comparison
  Use Intl.DateTimeFormat for locale-aware display

Timers:
  setTimeout(fn, ms) — run once after delay
  setInterval(fn, ms) — run repeatedly
  Always store the ID so you can clear it
  Debounce: wait N ms after last call
  Throttle: run at most once per N ms
```

---

_Next: [03 — Modern JavaScript (ES6+)](./03%20-%20Modern%20JavaScript%20\(ES6%2B\).md)_