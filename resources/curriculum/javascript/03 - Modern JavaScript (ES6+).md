> ES6 (released in 2015) and the yearly updates that followed transformed JavaScript from a scripting language into a serious application language. These features aren't optional extras — they're the baseline of every modern codebase. React, Node.js, and TypeScript all assume you know them.

---

## Table of Contents

1. [Arrow Functions — In Full](#1-arrow-functions--in-full)
2. [Template Literals — In Full](#2-template-literals--in-full)
3. [ES Modules (import / export)](#3-es-modules-import--export)
4. [Classes (Syntax Overview)](#4-classes-syntax-overview)
5. [Symbols and Well-Known Symbols](#5-symbols-and-well-known-symbols)
6. [Iterators and Generators](#6-iterators-and-generators)
7. [Promises — First Look](#7-promises--first-look)
8. [New Object and Array Features](#8-new-object-and-array-features)
9. [Logical Assignment Operators](#9-logical-assignment-operators)
10. [globalThis, structuredClone, and Other Globals](#10-globalthis-structuredclone-and-other-globals)

---

## 1. Arrow Functions — In Full

Arrow functions are shorter syntax for function expressions, with one important behavioural difference: they don't have their own `this`.

```javascript
// Syntax forms
const fn1 = (x) => x * 2; // single param, expression body
const fn2 = (x, y) => x + y; // multiple params
const fn3 = () => "no params"; // no params — parens required
const fn4 = (x) => ({ value: x }); // returning object — wrap in ()
const fn5 = (x) => {
  // full body — need return keyword
  const doubled = x * 2;
  return doubled + 1;
};

// When NOT to use arrow functions:
// 1. Object methods (need their own `this`)
const user = {
  name: "Ashan",
  greet: () => `Hello ${this?.name}`, // ✗ this is not user
  greetCorrect() {
    return `Hello ${this.name}`;
  }, // ✓
};

// 2. Constructors (can't use `new` with arrow functions)
const Person = (name) => {
  this.name = name;
};
new Person("Ashan"); // TypeError: Person is not a constructor

// Arrow functions are perfect for:
// - Callbacks: array methods, event handlers, setTimeout
// - Functions that should inherit `this` from surrounding scope
const timer = {
  count: 0,
  start() {
    setInterval(() => {
      this.count++; // `this` is the timer object ✓
      console.log(this.count);
    }, 1000);
  },
};
```

---

## 2. Template Literals — In Full

```javascript
// Basic interpolation
const name = "CWWKCC";
const year = 1873;
`${name} was founded in ${year}`; // any expression works in ${}
`2 + 2 = ${2 + 2}`;
`${isActive ? "Active" : "Inactive"}`;

// Multi-line (preserves newlines and indentation)
const html = `
  <div class="card">
    <h2>${title}</h2>
    <p>${body}</p>
  </div>
`.trim();

// Tagged templates — advanced, but important to recognise in libraries
// A tag is a function that processes the template
function highlight(strings, ...values) {
  return strings.reduce((result, str, i) => {
    const value = values[i] !== undefined ? `<mark>${values[i]}</mark>` : "";
    return result + str + value;
  }, "");
}

const score = 91;
highlight`Student scored ${score} out of 100`;
// "Student scored <mark>91</mark> out of 100"

// gql (GraphQL), css (styled-components), sql (SQL libraries) all use tagged templates
// You don't write tags often, but you use them constantly through libraries
```

---

## 3. ES Modules (import / export)

Modules split code across files, with explicit imports and exports. This replaced the old `<script>` soup and CommonJS `require()`.

```javascript
// ─── math.js ───
// Named exports — can export multiple things
export const PI = 3.14159;

export function add(a, b) {
  return a + b;
}

export function multiply(a, b) {
  return a * b;
}

// Default export — one per file, often the main thing the file provides
export default class Calculator {
  add(a, b) {
    return a + b;
  }
}
```

```javascript
// ─── main.js ───
// Named imports
import { PI, add, multiply } from "./math.js";

// Rename on import
import { add as sum } from "./math.js";

// Import default
import Calculator from "./math.js";

// Import default AND named together
import Calculator, { PI, add } from "./math.js";

// Import everything as a namespace object
import * as math from "./math.js";
math.PI; // 3.14159
math.add(2, 3); // 5
math.default; // the Calculator class

// Re-export (barrel file pattern)
// index.js — re-exports from multiple files as a single module
export { add, multiply } from "./math.js";
export { formatDate } from "./dates.js";
export { fetchUser } from "./api.js";
// Consumers: import { add, fetchUser } from "./index.js"
```

### Module Characteristics

```javascript
// Modules are singletons — the same instance is shared everywhere it's imported
// module.js
let count = 0;
export const increment = () => ++count;
export const getCount = () => count;

// file1.js
import { increment } from "./module.js";
increment(); // count → 1

// file2.js — same module instance
import { getCount } from "./module.js";
getCount(); // 1 — shared state

// Modules run once — the first time they're imported
// Subsequent imports get the cached module

// Dynamic imports — load a module on demand (code splitting)
async function loadChartLibrary() {
  const { Chart } = await import("./chart.js");
  // Chart module loaded only when needed
  return new Chart();
}

// Used in Next.js for:
const HeavyComponent = dynamic(() => import("./HeavyComponent"), {
  loading: () => <Spinner />,
});
```

---

## 4. Classes (Syntax Overview)

Full depth in File 09 (Prototypes). Here's the syntax you'll use every day.

```javascript
class Animal {
  // Class field (ES2022)
  #sound = "..."; // private — only accessible inside the class

  constructor(name, species) {
    this.name = name;
    this.species = species;
  }

  // Instance method
  speak() {
    return `${this.name} says ${this.#sound}`;
  }

  // Static method — called on the class, not instances
  static create(name, species) {
    return new Animal(name, species);
  }

  // Getter
  get label() {
    return `${this.name} (${this.species})`;
  }

  // Setter
  set label(value) {
    [this.name, this.species] = value.split(" ");
  }
}

class Dog extends Animal {
  #breed;

  constructor(name, breed) {
    super(name, "Canis lupus familiaris"); // must call super() first
    this.#breed = breed;
  }

  // Override parent method
  speak() {
    return `${this.name} barks!`;
  }

  describe() {
    return `${super.speak()} (${this.#breed})`;
  }
}

const rex = new Dog("Rex", "Labrador");
rex.speak(); // "Rex barks!"
rex.label; // "Rex (Canis lupus familiaris)"
rex instanceof Dog; // true
rex instanceof Animal; // true
```

---

## 5. Symbols and Well-Known Symbols

Symbols are unique, immutable primitive values. They're used as guaranteed-unique property keys.

```javascript
// Every Symbol is unique
const s1 = Symbol("id");
const s2 = Symbol("id");
s1 === s2; // false

// Use as unique object keys — won't clash with other properties
const ID = Symbol("id");
const user = {
  name: "Ashan",
  [ID]: "u-12345", // symbol key
};
user.name; // "Ashan"
user[ID]; // "u-12345"

// Symbol keys are NOT included in:
Object.keys(user); // ["name"] — no symbol keys
Object.entries(user); // [["name", "Ashan"]] — no symbol keys
JSON.stringify(user); // '{"name":"Ashan"}' — symbol keys dropped
for (const key in user) {
} // only "name"

// Retrieve symbols:
Object.getOwnPropertySymbols(user); // [Symbol(id)]
```

### Well-Known Symbols — Making Objects Behave Like Built-ins

```javascript
// Symbol.iterator — makes an object iterable with for...of
class Range {
  constructor(start, end) {
    this.start = start;
    this.end = end;
  }

  [Symbol.iterator]() {
    let current = this.start;
    const end = this.end;

    return {
      next() {
        if (current <= end) {
          return { value: current++, done: false };
        }
        return { value: undefined, done: true };
      },
    };
  }
}

const range = new Range(1, 5);
for (const n of range) {
  console.log(n); // 1, 2, 3, 4, 5
}
[...range]; // [1, 2, 3, 4, 5]

// Symbol.toPrimitive — control how an object converts to a primitive
class Money {
  constructor(amount, currency) {
    this.amount = amount;
    this.currency = currency;
  }

  [Symbol.toPrimitive](hint) {
    if (hint === "number") return this.amount;
    if (hint === "string") return `${this.amount} ${this.currency}`;
    return this.amount;
  }
}

const price = new Money(1500, "LKR");
+price; // 1500 (number hint)
`${price}`; // "1500 LKR" (string hint)
price + 500; // 2000 (default hint → number)
```

---

## 6. Iterators and Generators

An **iterator** is any object with a `next()` method that returns `{ value, done }`. A **generator** is a function that produces iterators using `yield`.

```javascript
// Generator function — note the *
function* count(start, end) {
  for (let i = start; i <= end; i++) {
    yield i; // pause here, return i, resume on next call
  }
}

const gen = count(1, 3);
gen.next(); // { value: 1, done: false }
gen.next(); // { value: 2, done: false }
gen.next(); // { value: 3, done: false }
gen.next(); // { value: undefined, done: true }

// Generators are iterable
for (const n of count(1, 5)) {
  console.log(n); // 1, 2, 3, 4, 5
}
[...count(1, 5)]; // [1, 2, 3, 4, 5]

// Infinite sequence — only compute as needed (lazy)
function* fibonacci() {
  let a = 0,
    b = 1;
  while (true) {
    yield a;
    [a, b] = [b, a + b];
  }
}

function take(n, iterable) {
  const result = [];
  for (const value of iterable) {
    result.push(value);
    if (result.length === n) break;
  }
  return result;
}

take(8, fibonacci()); // [0, 1, 1, 2, 3, 5, 8, 13]
```

Generators underlie async/await under the hood, and are used in state management libraries like Redux Saga.

---

## 7. Promises — First Look

A `Promise` represents a value that isn't available yet but will be in the future.

```javascript
// Creating a Promise
const promise = new Promise((resolve, reject) => {
  // Do async work...
  setTimeout(() => {
    const success = Math.random() > 0.5;
    if (success) {
      resolve("data loaded"); // fulfilled with a value
    } else {
      reject(new Error("load failed")); // rejected with an error
    }
  }, 1000);
});

// Consuming a Promise
promise
  .then((value) => console.log("Got:", value)) // runs on resolve
  .catch((err) => console.error("Error:", err)) // runs on reject
  .finally(() => console.log("Always runs")); // always runs

// Chaining — each .then() returns a new Promise
fetch("/api/user/1")
  .then((response) => response.json()) // parse JSON
  .then((user) => fetchPosts(user.id)) // use result in next call
  .then((posts) => renderPosts(posts)) // use next result
  .catch((err) => showError(err)); // any error in the chain

// Promise static methods
Promise.resolve(42); // immediately fulfilled with 42
Promise.reject(new Error("nope")); // immediately rejected
```

Full depth (async/await, error handling, all parallel utilities) in File 06.

---

## 8. New Object and Array Features

```javascript
// Object shorthand — when key name = variable name
const name = "Ashan";
const grade = 11;
const student = { name, grade }; // same as { name: name, grade: grade }

// Computed property names
const field = "score";
const obj = { [field]: 95, [`${field}_max`]: 100 };
// { score: 95, score_max: 100 }

// Optional catch binding (ES2019)
try {
  JSON.parse(invalid);
} catch {
  // no (err) needed if you don't use the error
  return null;
}

// Object.fromEntries (ES2019)
const entries = [
  ["a", 1],
  ["b", 2],
];
Object.fromEntries(entries); // { a: 1, b: 2 }

// Combine with Map.entries():
const map = new Map([
  ["x", 10],
  ["y", 20],
]);
Object.fromEntries(map); // { x: 10, y: 20 }

// Array.from (convert array-like to real array)
Array.from("hello"); // ["h", "e", "l", "l", "o"]
Array.from({ length: 5 }, (_, i) => i); // [0, 1, 2, 3, 4]
Array.from(new Set([1, 2, 3])); // [1, 2, 3]

// Array.at() (ES2022) — negative indexing
const arr = [1, 2, 3, 4, 5];
arr.at(0); // 1
arr.at(-1); // 5 — last element
arr.at(-2); // 4

// Array grouping (ES2024)
const students = [
  { name: "Ashan", grade: 11 },
  { name: "Dineth", grade: 12 },
  { name: "Kavya", grade: 11 },
];
Object.groupBy(students, (s) => s.grade);
// { 11: [{name:"Ashan"...}, {name:"Kavya"...}], 12: [{name:"Dineth"...}] }
```

---

## 9. Logical Assignment Operators

These combine logical operators with assignment — extremely useful for defaults and conditional updates.

```javascript
// ??= — assign if null or undefined
let config = {};
config.timeout ??= 5000; // assigns 5000 (was undefined)
config.timeout ??= 3000; // no change — already 5000
config.retries ??= 3; // assigns 3
config.debug ??= false; // assigns false

// ||= — assign if falsy (null, undefined, 0, "", false, NaN)
let name = "";
name ||= "Anonymous"; // assigns "Anonymous" — "" is falsy

let count = 0;
count ||= 10; // assigns 10 — 0 is falsy (careful! may be unintended)

// &&= — assign if truthy (update only if value exists)
let user = { name: "Ashan", role: "student" };
user.role &&= user.role.toUpperCase(); // user.role is truthy → assigned "STUDENT"

let guest = null;
guest &&= doSomething(); // guest is null (falsy) → skipped, guest stays null
```

---

## 10. `globalThis`, `structuredClone`, and Other Globals

```javascript
// globalThis — the global object, regardless of environment
// In browser: window
// In Node.js: global
// In Workers: self
// globalThis works everywhere
globalThis.setTimeout; // the global setTimeout
globalThis.fetch; // the global fetch

// structuredClone — deep clone almost anything (ES2022)
const original = {
  name: "Ashan",
  scores: [82, 91],
  date: new Date(),
  nested: { deeply: { value: 42 } },
};
const clone = structuredClone(original);
clone.scores.push(100); // doesn't affect original
clone.nested.deeply.value = 99; // doesn't affect original
// Also supports: Map, Set, Date, RegExp, ArrayBuffer, and more
// Does NOT support: functions, DOM nodes, class instances with methods

// queueMicrotask — schedule a microtask (runs before next macrotask)
queueMicrotask(() => {
  console.log("this runs after current sync code, before setTimeout");
});

// crypto.randomUUID — generate a UUID (ES2022, secure)
crypto.randomUUID(); // "a3b4c5d6-e7f8-1234-5678-abcdef012345"

// AbortController — cancel fetch and other async operations
const controller = new AbortController();

setTimeout(() => controller.abort(), 3000); // cancel after 3 seconds

try {
  const response = await fetch("/api/slow-endpoint", {
    signal: controller.signal,
  });
} catch (err) {
  if (err.name === "AbortError") {
    console.log("Request was cancelled");
  }
}
```

---

## Summary

```
Arrow functions:
  No own `this` — inherits from surrounding scope
  Great for callbacks, bad for object methods and constructors

Template literals:
  `${expression}` — any expression works
  Multi-line strings, tagged templates (used by styled-components, gql, etc.)

ES Modules:
  import/export — the modern standard
  Named exports: export function fn() {}, import { fn }
  Default export: export default, import anything from "..."
  Modules are singletons, run once, cached
  Dynamic import(): for code splitting

Symbols:
  Unique, immutable keys — won't clash
  Well-known symbols: Symbol.iterator, Symbol.toPrimitive — customise built-in behaviour

Generators:
  function* with yield — lazy sequences, infinite data, async/await internals

Promises:
  Represents a future value: fulfilled or rejected
  .then().catch().finally() — consume the result
  Full treatment in File 06

New syntax:
  Object shorthand, computed keys, optional catch
  Object.fromEntries, Array.from, Array.at, Object.groupBy
  ??=, ||=, &&= — logical assignment operators

Modern globals:
  globalThis, structuredClone, crypto.randomUUID, AbortController
```

---

_Next: [04 — DOM Manipulation & Events](./04%20-%20DOM%20Manipulation%20%26%20Events.md)_
