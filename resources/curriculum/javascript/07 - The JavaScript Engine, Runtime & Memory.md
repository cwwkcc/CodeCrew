> You've written JavaScript for a while. But what actually _runs_ it? Understanding the engine, the call stack, the heap, and garbage collection changes how you write code — and explains bugs you've seen but couldn't explain.

---

## Table of Contents

1. [What a JavaScript Engine Is](#1-what-a-javascript-engine-is)
2. [How V8 Compiles Your Code](#2-how-v8-compiles-your-code)
3. [The Call Stack](#3-the-call-stack)
4. [Stack Overflow — What Actually Happens](#4-stack-overflow--what-actually-happens)
5. [The Memory Heap](#5-the-memory-heap)
6. [Stack vs Heap — What Goes Where](#6-stack-vs-heap--what-goes-where)
7. [Garbage Collection](#7-garbage-collection)
8. [Memory Leaks — What Causes Them](#8-memory-leaks--what-causes-them)
9. [The Event Loop (JavaScript Runtime Model)](#9-the-event-loop-javascript-runtime-model)
10. [Microtasks vs Macrotasks](#10-microtasks-vs-macrotasks)
11. [Why JavaScript Is Single-Threaded](#11-why-javascript-is-single-threaded)

---

## 1. What a JavaScript Engine Is

JavaScript is not directly understood by your CPU. A **JavaScript engine** is the program that reads your JS code and executes it.

```
Your JS code (text)
       ↓
  JS Engine (e.g. V8)
       ↓
  Machine code (CPU instructions)
```

Major engines:

```
V8           → Chrome, Node.js, Deno, Edge (Chromium)
SpiderMonkey → Firefox
JavaScriptCore (Nitro) → Safari, Bun
Hermes       → React Native (mobile)
```

When you run `node app.js`, Node.js uses V8 to execute your code. When Chrome runs a webpage, it also uses V8. Same engine, different host environment.

The **host environment** provides things the engine itself doesn't have: timers (`setTimeout`), the DOM (browser), file system access (Node.js), network access. V8 alone has none of these — it just understands JavaScript.

---

## 2. How V8 Compiles Your Code

V8 does not interpret JavaScript line-by-line. It compiles it — but smartly, using a two-stage approach:

```
Your JS code
     ↓
  Parsing
  ├── Lexing (tokenization): breaks text into tokens
  │     "const x = 5 + 3" → [const] [x] [=] [5] [+] [3]
  └── Parsing: builds an AST (Abstract Syntax Tree)
     ↓
  AST (Abstract Syntax Tree)
  — a tree structure representing your code's meaning
     ↓
  Ignition (Interpreter)
  — converts AST to bytecode and executes it immediately
  — fast startup, not optimised
     ↓
  (runs while profiling "hot" code — frequently-called functions)
     ↓
  TurboFan (Optimising Compiler)
  — takes hot bytecode + profile data
  — compiles to highly optimised machine code
  — runs much faster
```

### Why This Matters: Hidden Classes and Deoptimisation

V8 creates internal **hidden classes** to optimise property access on objects. When you change an object's shape, V8 has to deoptimise.

```javascript
// BAD — changes object shape after creation
function createUser(name, role) {
  const user = {};
  user.name = name;         // shape 1: { name }
  user.role = role;         // shape 2: { name, role }
  if (role === "admin") {
    user.permissions = [];  // shape 3: { name, role, permissions }
  }
  return user;
}

// GOOD — consistent object shape from the start
function createUser(name, role) {
  return {
    name,
    role,
    permissions: role === "admin" ? [] : null,
    // same shape always — V8 can optimise aggressively
  };
}

// BAD — deleting properties also breaks hidden classes
const obj = { x: 1, y: 2 };
delete obj.x;  // V8 has to switch to a "dictionary mode" — much slower
// Instead: set to null or undefined if you want to "empty" a property
obj.x = null;
```

---

## 3. The Call Stack

The **call stack** is how JavaScript tracks what function is currently executing, and what to return to when it finishes.

It's a stack data structure: last in, first out (LIFO).

```javascript
function greet(name) {
  return `Hello, ${name}`;
}

function welcome(user) {
  const message = greet(user.name);
  return message;
}

function main() {
  const user = { name: "Dineth" };
  console.log(welcome(user));
}

main();
```

Call stack progression:

```
Step 1: main() is called
Stack: [main]

Step 2: main() calls welcome()
Stack: [main, welcome]

Step 3: welcome() calls greet()
Stack: [main, welcome, greet]

Step 4: greet() returns "Hello, Dineth"
Stack: [main, welcome]         ← greet popped off

Step 5: welcome() returns the message
Stack: [main]                  ← welcome popped off

Step 6: main() calls console.log(), then returns
Stack: []                      ← empty — program done
```

Each entry in the call stack is a **stack frame**. A frame contains:

- The function being executed
- Its local variables
- Where to return to when it finishes

---

## 4. Stack Overflow — What Actually Happens

The call stack has a maximum size. If you push too many frames — typically through infinite recursion — you exceed it.

```javascript
// Classic stack overflow: infinite recursion
function factorial(n) {
  return n * factorial(n - 1);  // forgot base case
}

factorial(10000);
// RangeError: Maximum call stack size exceeded
```

```
Stack grows with every call:
[factorial(10000)]
[factorial(10000), factorial(9999)]
[factorial(10000), factorial(9999), factorial(9998)]
...
[factorial(10000), ..., factorial(1), factorial(0), factorial(-1)...]
→ Stack limit hit → RangeError thrown
```

The fix — always have a base case:

```javascript
function factorial(n) {
  if (n <= 1) return 1;           // base case — stops recursion
  return n * factorial(n - 1);
}
```

### Tail Call Optimisation (TCO)

In strict mode, V8 can theoretically optimise **tail calls** — where the recursive call is the _last thing_ a function does — so the stack doesn't grow. In practice, V8's TCO support is limited. For deep recursion, use iteration instead:

```javascript
// Iterative factorial — no stack growth
function factorial(n) {
  let result = 1;
  for (let i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}
```

---

## 5. The Memory Heap

The **heap** is where objects, arrays, and functions are stored. It's a large, unstructured region of memory — unlike the stack, it doesn't have a strict size limit (up to available system memory).

```
Memory model (simplified):

STACK                    HEAP
─────────────────        ──────────────────────────────────
│ main frame:   │        │                                │
│   user = ─────────────▶  { name: "Dineth", age: 20 }  │
│   x = 42     │        │                                │
│               │        │  [1, 2, 3, 4, 5]              │
│ welcome frame:│        │                                │
│   message = ──────────▶  "Hello, Dineth"               │
─────────────────        ──────────────────────────────────

Stack stores: primitives (numbers, booleans) directly,
              and references (pointers) to heap objects.
Heap stores:  objects, arrays, functions, strings (in V8, strings
              can be stack or heap depending on size and reuse).
```

---

## 6. Stack vs Heap — What Goes Where

```javascript
// Primitives: stored by VALUE on the stack
let a = 42;
let b = a;     // b gets a COPY of 42
b = 100;
console.log(a); // 42 — a was not affected

// Objects: stored by REFERENCE — pointer on stack, object on heap
let user1 = { name: "Alice" };
let user2 = user1;           // user2 gets a COPY of the POINTER
user2.name = "Bob";
console.log(user1.name);     // "Bob" — both point to same object

// To truly copy an object: spread or structuredClone
let user3 = { ...user1 };    // shallow copy — new object on heap
let user4 = structuredClone(user1);  // deep copy (nested objects too)
user3.name = "Charlie";
console.log(user1.name);     // "Bob" — user1 unaffected
```

```
Types stored on the stack (by value):
  number, boolean, undefined, null, symbol, bigint

Types stored on the heap (by reference):
  object, array, function, Map, Set, Date, RegExp

Strings are a special case: V8 interns and deduplicates short strings,
and some small strings may live on the stack. For practical purposes,
treat strings as immutable values — mutations create new strings.
```

---

## 7. Garbage Collection

JavaScript manages memory automatically. You don't `malloc`/`free` like in C. The **garbage collector (GC)** reclaims memory that is no longer reachable.

### The Reachability Model

An object is **reachable** if it can be accessed from a root (global scope, current call stack, closures). Unreachable objects are garbage.

```javascript
let user = { name: "Alice" };
// { name: "Alice" } is reachable via `user`

user = null;
// { name: "Alice" } is now unreachable — nothing references it
// GC will reclaim its memory
```

### Mark-and-Sweep (V8's primary GC algorithm)

```
Phase 1 — Mark:
  Start from roots (global, stack)
  Traverse all references recursively
  Mark every reachable object

Phase 2 — Sweep:
  Walk the heap
  Any object NOT marked → collect its memory
  Reset marks for next cycle
```

```javascript
// Example of what GC tracks:

function processOrder(orderId) {
  const order = fetchOrder(orderId);    // order allocated on heap
  const total = calculateTotal(order);  // total: number (stack)
  sendConfirmation(order.email, total);
  // function returns — order goes out of scope
  // if nothing else references it, GC will collect it
}
```

### V8's Generational GC

V8 uses a **generational** strategy because most objects die young (created and quickly discarded):

```
Heap is divided into:

Young Generation (Scavenger — fast GC, runs frequently)
├── Nursery: newly allocated objects go here
└── Intermediate: objects that survived one GC cycle

Old Generation (Mark-Sweep-Compact — slow GC, runs less often)
└── Objects that survived multiple young GC cycles

Logic: if an object is still alive after several GC cycles,
it's probably going to live for a while — promote it to old gen.
Most objects die in the nursery — young GC is fast because
it only looks at a small portion of the heap.
```

---

## 8. Memory Leaks — What Causes Them

A **memory leak** is when memory that should be freed is still referenced — preventing GC from collecting it. In JavaScript, leaks don't crash immediately — they cause gradual performance degradation.

### 1. Forgotten Event Listeners

```javascript
// LEAK: listener added every time modal opens, never removed
function openModal() {
  const modal = document.getElementById("modal");
  
  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") modal.close();
  });
  // Every call to openModal adds ANOTHER listener
  // Old listeners are never removed → they accumulate
}

// FIX: keep a reference to the listener function and remove it
function openModal() {
  const modal = document.getElementById("modal");
  
  const handleKeydown = (e) => {
    if (e.key === "Escape") {
      modal.close();
      document.removeEventListener("keydown", handleKeydown); // clean up
    }
  };
  
  document.addEventListener("keydown", handleKeydown);
}
```

### 2. Closures Holding Large Objects

```javascript
// LEAK: closure keeps a reference to a large array
function setup() {
  const largeData = new Array(1_000_000).fill("data"); // 1M items
  
  const button = document.getElementById("btn");
  button.addEventListener("click", () => {
    // This closure captures largeData
    // Even if you only use largeData.length, the entire array is retained
    console.log("Clicked, data size:", largeData.length);
  });
  // largeData is never freed as long as the button exists
}

// FIX: capture only what you need
function setup() {
  const largeData = new Array(1_000_000).fill("data");
  const dataSize = largeData.length;  // capture only what's needed
  // largeData can now be GC'd after setup() returns
  
  document.getElementById("btn").addEventListener("click", () => {
    console.log("Clicked, data size:", dataSize);  // uses number, not array
  });
}
```

### 3. Detached DOM Nodes

```javascript
// LEAK: JS holds reference to a removed DOM node
const elements = {};

function storeButton() {
  elements.button = document.getElementById("submit-btn");
}

function removeButton() {
  document.body.removeChild(document.getElementById("submit-btn"));
  // The button is removed from the DOM
  // But elements.button still references it → it can't be GC'd
  // The entire subtree under the button is also retained!
}

// FIX: null out references when done
function removeButton() {
  document.body.removeChild(document.getElementById("submit-btn"));
  elements.button = null;  // now GC can collect it
}
```

### 4. Global Variables

```javascript
// LEAK: accidental global (missing let/const/var)
function processData() {
  result = computeResult();  // no declaration keyword → global variable!
  // `result` is now on the global object (window in browser)
  // Lives for the entire lifetime of the page
}

// In strict mode this throws a ReferenceError — use "use strict" or ESM
// In non-strict mode it silently creates a global — dangerous
```

### 5. Timers Not Cleared

```javascript
// LEAK: interval keeps a callback alive indefinitely
function startPolling() {
  const data = fetchLargeDataset();
  
  setInterval(() => {
    // This closure captures `data`
    // The interval keeps running even if you navigate away
    processChunk(data);
  }, 1000);
}

// FIX: store the ID and clear when done
let intervalId;

function startPolling() {
  const data = fetchLargeDataset();
  intervalId = setInterval(() => processChunk(data), 1000);
}

function stopPolling() {
  clearInterval(intervalId);  // closure and data can now be GC'd
}
```

---

## 9. The Event Loop (JavaScript Runtime Model)

JavaScript is single-threaded — one call stack, one thing executing at a time. But it handles async operations without blocking. The **event loop** is the mechanism that makes this possible.

```
JavaScript Runtime (in the browser / Node.js):

┌─────────────────────────────────────────────┐
│              Call Stack                      │
│  (currently executing code)                 │
└──────────────────────┬──────────────────────┘
                       │ when empty, event loop
                       │ picks next task
┌──────────────────────▼──────────────────────┐
│           Microtask Queue                    │
│  Promise callbacks, queueMicrotask,         │
│  MutationObserver                           │
│  → ALWAYS drained completely before         │
│    moving to macrotasks                     │
└──────────────────────┬──────────────────────┘
                       │ when microtask queue is empty
┌──────────────────────▼──────────────────────┐
│           Macrotask Queue (Task Queue)       │
│  setTimeout, setInterval, I/O callbacks,   │
│  UI events (click, keydown), postMessage    │
│  → ONE macrotask is picked per loop turn   │
└─────────────────────────────────────────────┘
```

### Event Loop Step-by-Step

```
Loop iteration ("tick"):

1. Execute all code currently on the call stack
2. When stack is empty:
   a. Drain the entire microtask queue (run all microtasks)
      → If microtasks add more microtasks, run those too
      → Keep going until microtask queue is completely empty
   b. Pick ONE macrotask from the macrotask queue
   c. Put it on the call stack and execute it
   d. After that macrotask finishes, drain microtasks again (step 2a)
3. Repeat
```

```javascript
console.log("1 — synchronous");

setTimeout(() => console.log("2 — setTimeout (macrotask)"), 0);

Promise.resolve().then(() => console.log("3 — Promise (microtask)"));

console.log("4 — synchronous");

// Output:
// 1 — synchronous
// 4 — synchronous       ← stack drains synchronous code first
// 3 — Promise (microtask) ← microtask queue drained before any macrotask
// 2 — setTimeout (macrotask) ← macrotask picked after microtasks
```

---

## 10. Microtasks vs Macrotasks

```
MICROTASKS (high priority — run before the next macrotask):
  Promise .then() / .catch() / .finally() callbacks
  async/await continuations (they compile to Promise callbacks)
  queueMicrotask(fn)
  MutationObserver callbacks (browser)

MACROTASKS (lower priority — one per loop turn):
  setTimeout(fn, delay)
  setInterval(fn, delay)
  setImmediate(fn)         [Node.js only]
  I/O callbacks            (file reads, network responses in Node.js)
  UI events                (click, keydown, scroll — browser)
  MessageChannel / postMessage
```

### Practical Example: Stacking Microtasks

```javascript
Promise.resolve()
  .then(() => {
    console.log("microtask 1");
    // Adding another microtask FROM WITHIN a microtask
    return Promise.resolve();
  })
  .then(() => console.log("microtask 2"))
  .then(() => console.log("microtask 3"));

setTimeout(() => console.log("macrotask"), 0);

// Output:
// microtask 1
// microtask 2
// microtask 3
// macrotask
// ↑ ALL microtasks drain before the setTimeout runs,
//   even though setTimeout was scheduled first
```

### Why This Matters in React

React's `useState` batches state updates. Understanding that React flushes updates as microtasks (in React 18+) explains why:

```javascript
// In React 18: multiple state updates in async code are batched
async function handleClick() {
  setCount(c => c + 1);
  setName("Alice");
  // Before React 18: two separate re-renders
  // React 18+: batched into one re-render (scheduled as microtask)
}
```

---

## 11. Why JavaScript Is Single-Threaded

JavaScript was designed for browsers in 1995. The DOM — the tree of HTML elements — is not thread-safe. If two threads could modify the DOM simultaneously, you'd get race conditions: one thread inserting a node while another is iterating over it, corrupting the tree.

The single-threaded model eliminates this entire class of bugs. You never need locks, mutexes, or synchronisation primitives for DOM access — there's only ever one thread doing it.

```
The trade-off:
  ✓ No race conditions on shared state
  ✓ No locks, no deadlocks, simpler mental model
  ✗ CPU-intensive work blocks the thread
  ✗ Can't use multiple CPU cores directly

For CPU-intensive work: Web Workers (browser) or Worker threads (Node.js)
  — separate JS environments with their own call stack and heap
  — communicate via message passing (no shared memory by default)
  — the DOM is not accessible from workers (intentional)
```

```javascript
// Blocking the event loop — NEVER do this in production
function blockFor2Seconds() {
  const end = Date.now() + 2000;
  while (Date.now() < end) {}  // busy-wait — freezes EVERYTHING
  // No clicks, no network responses, no timers fire during this
}

// For heavy computation: use a Web Worker
const worker = new Worker("heavy-computation.js");
worker.postMessage({ data: largeDataset });
worker.onmessage = (e) => {
  console.log("Result:", e.data);  // main thread unblocked
};
```

---

## Summary

```
JS Engine (V8):
  Parses JS → AST → bytecode (Ignition) → optimised machine code (TurboFan)
  Consistent object shapes → V8 optimises aggressively (hidden classes)
  Avoid: deleting properties, adding properties after creation

Call Stack:
  Tracks executing functions (LIFO)
  Stack frame: local variables + return address
  Stack overflow: unbounded recursion

Memory:
  Stack: primitives by value, references (pointers) by value
  Heap: objects, arrays, functions — shared by reference
  Primitives: immutable, copied on assignment
  Objects: mutable, assignment copies the pointer

Garbage Collection:
  Mark-and-sweep: mark reachable objects, collect the rest
  Generational: young gen (frequent, fast) + old gen (rare, slow)
  Common leaks: forgotten listeners, closures, detached nodes, globals, timers

Event Loop:
  Call stack → microtasks (drained fully) → one macrotask → repeat
  Microtasks: Promise callbacks, async/await continuations
  Macrotasks: setTimeout, I/O, UI events

Single-threaded: no DOM race conditions, but CPU work blocks everything
  Heavy work → Web Workers / Worker Threads
```

---

_Next: [08 — Closures In Depth](./08%20-%20Closures%20In%20Depth.md)_