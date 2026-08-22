> Node.js is not magic. It is a specific combination of three things: V8 (a JavaScript engine), libuv (a C library for async I/O), and a set of built-in modules that bridge JavaScript to the operating system. Understanding what each piece does — and how they collaborate — makes everything else in Node.js make sense.

---

## Table of Contents

1. [What Node.js Actually Is](#1-what-nodejs-actually-is)
2. [V8 — The JavaScript Engine](#2-v8--the-javascript-engine)
3. [libuv — The Async I/O Engine](#3-libuv--the-async-io-engine)
4. [The Thread Model — Why "Single-Threaded" Is Misleading](#4-the-thread-model--why-single-threaded-is-misleading)
5. [The Event Loop — Overview](#5-the-event-loop--overview)
6. [How a Node.js Process Starts](#6-how-a-nodejs-process-starts)
7. [I/O in Node.js — What Happens When You Read a File](#7-io-in-nodejs--what-happens-when-you-read-a-file)
8. [The Node.js Architecture — All Together](#8-the-nodejs-architecture--all-together)
9. [Node.js vs Browser JavaScript](#9-nodejs-vs-browser-javascript)
10. [What Node.js Is Good At and What It Is Not](#10-what-nodejs-is-good-at-and-what-it-is-not)

---

## 1. What Node.js Actually Is

Node.js is a **runtime environment** for JavaScript outside the browser. It is not a framework, not a language, not a server. It is the environment that lets JavaScript code interact with the operating system.

```
The components:

┌─────────────────────────────────────────────────────┐
│                    YOUR CODE                        │
│              (JavaScript / TypeScript)              │
├─────────────────────────────────────────────────────┤
│                  NODE.JS BINDINGS                   │
│         (C++ code that bridges JS to OS)            │
├───────────────────┬─────────────────────────────────┤
│        V8         │            libuv                │
│  (runs your JS)   │  (handles async I/O, event loop)│
├───────────────────┴─────────────────────────────────┤
│                 OPERATING SYSTEM                    │
│        (file system, network, processes)            │
└─────────────────────────────────────────────────────┘

V8:     Takes your JavaScript, compiles it to machine code, runs it.
libuv:  Handles all I/O (files, network, timers) asynchronously.
        Written in C. Abstracts over OS differences (Linux, macOS, Windows).
Bindings: C++ code that lets JavaScript call into libuv and the OS.
          When you call fs.readFile(), JavaScript calls a C++ binding,
          which calls libuv, which asks the OS for the file.
```

Node.js was created by Ryan Dahl in 2009. The insight: JavaScript has no built-in I/O, which meant it could be designed from scratch to be async-first. Combined with V8's speed and the event loop model, it enables high-concurrency servers without threads.

---

## 2. V8 — The JavaScript Engine

V8 is Google's open-source JavaScript and WebAssembly engine, written in C++. It powers both Chrome and Node.js.

```
V8's job: take JavaScript source code → produce output.

Pipeline:

Source code (text)
    ↓
Parser
    → builds AST (Abstract Syntax Tree)
    → AST is a tree representation of your code's structure

AST
    ↓
Ignition (bytecode interpreter)
    → compiles AST to bytecode
    → starts executing immediately (fast startup)
    → collects runtime feedback: "this function runs 10,000 times"
    →                            "this argument is always a number"

Bytecode + feedback
    ↓
TurboFan (optimising compiler)
    → takes "hot" code (frequently executed)
    → uses feedback to make assumptions
    → compiles to highly optimised native machine code
    → runs 10–100× faster than bytecode

If an assumption breaks:
    → deoptimisation: discard compiled code, fall back to bytecode
    → recompile with new, correct assumptions
```

### What V8 Manages

```
Heap:
  Where objects, arrays, closures, and strings live in memory.
  Garbage collected — V8 automatically frees unreachable memory.

Stack:
  Where function call frames live.
  Each function call pushes a frame. Return pops it.
  Fixed size (default ~984KB in Node.js).
  Stack overflow = too many nested calls (infinite recursion).

Garbage collector:
  Generational GC. Two spaces:
    Young generation (nursery): newly allocated objects. GC runs frequently.
    Old generation: objects that survived multiple young GCs. GC runs less often.

  GC pauses:  V8 occasionally pauses JS execution to collect garbage.
  Minor GC:   Young generation only. Fast (~1ms).
  Major GC:   Full heap. Can cause visible pauses (10-100ms in large heaps).

  Incremental/concurrent GC: V8 does GC work in background threads
  to reduce pause duration. Full pauses are rare in modern V8.
```

### V8 and Node.js Version Relationship

```
Each Node.js release ships with a specific V8 version.
New V8 = new JavaScript features available in Node.js.

Node.js 20 → V8 11.3
Node.js 22 → V8 12.4

This is how new ES features (Array.groupBy, structuredClone, etc.)
become available in Node.js — V8 implements them.

Check V8 version: node -e "console.log(process.versions.v8)"
```

---

## 3. libuv — The Async I/O Engine

libuv is a C library written specifically for Node.js (now used by others too). It solves a hard problem: doing I/O operations asynchronously and portably across operating systems.

```
The problem libuv solves:

Reading a file synchronously:
  1. Ask OS for file
  2. WAIT (could be 5ms, could be 50ms — the CPU does nothing)
  3. OS returns data
  4. Continue

5ms might seem fine. But a server handling 1,000 requests/second
can't afford to wait 5ms per request on a single thread.
1000 × 5ms = 5 seconds of waiting per second. Impossible.

libuv's solution: don't wait. Delegate.

1. Tell OS: "start reading this file, call me when done"
2. Continue doing other things (handle other requests, run timers)
3. OS finishes reading → libuv gets notified → runs your callback
```

### How libuv Handles Different I/O

```
Network I/O (TCP, UDP):
  Uses OS-level async I/O directly.
    Linux:   epoll
    macOS:   kqueue
    Windows: IOCP (I/O Completion Ports)

  These are OS mechanisms that can watch thousands of sockets simultaneously
  and notify when any of them have data ready.
  Zero extra threads needed.

File system I/O:
  Most OS async file APIs are inconsistent or limited.
  libuv uses a THREAD POOL for file operations.

  Default thread pool size: 4 threads.
  Each thread can block on a file read without blocking the event loop.
  When a thread completes: result queued → callback runs in event loop.

  Configurable: UV_THREADPOOL_SIZE=8 node server.js

DNS resolution:
  Also uses the thread pool (getaddrinfo is blocking).
  Exception: dns.resolve() uses the OS resolver (async).
             dns.lookup() uses getaddrinfo (thread pool).

Timers (setTimeout, setInterval):
  libuv maintains a min-heap of timers.
  On each event loop iteration: check if any timers expired → run callbacks.
  No separate thread — pure event loop mechanics.
```

### The Thread Pool

```
libuv maintains a pool of threads for blocking operations:

┌────────────────────────────────────────┐
│            Event Loop Thread           │  ← your JS runs here
│         (the "main thread")            │
└──────────────┬─────────────────────────┘
               │ delegates blocking work
┌──────────────▼─────────────────────────┐
│            Thread Pool                 │
│  Thread 1 │ Thread 2 │ Thread 3 │ T4  │  ← blocking I/O runs here
└──────────────┬─────────────────────────┘
               │ result ready → queue callback
┌──────────────▼─────────────────────────┐
│         Callback Queue                 │
│  [file read done] [dns resolved] ...   │
└──────────────┬─────────────────────────┘
               │
┌──────────────▼─────────────────────────┐
│         Event Loop picks it up         │
│         → runs your callback           │
└────────────────────────────────────────┘

Key point: YOUR JavaScript always runs on ONE thread (the event loop thread).
The thread pool is internal to libuv — your JS doesn't run there.
The pool is used to avoid blocking the event loop, not to parallelise JS.
```

---

## 4. The Thread Model — Why "Single-Threaded" Is Misleading

```
"Node.js is single-threaded."

This is true and false at the same time.

TRUE:
  Your JavaScript code runs on a single thread.
  There is one call stack.
  Only one piece of JS runs at a time.
  No two JS functions run simultaneously.

FALSE (or misleading):
  The Node.js PROCESS has multiple threads.

  When you run node server.js:

  Thread 1: Event loop (V8 + your JS code)
  Thread 2: libuv thread pool worker
  Thread 3: libuv thread pool worker
  Thread 4: libuv thread pool worker
  Thread 5: libuv thread pool worker
  Thread 6: V8 garbage collector (concurrent marking)
  Thread 7: V8 garbage collector (concurrent sweeping)

  If you use worker_threads: more threads.

The single-threaded guarantee applies to JavaScript execution.
I/O and GC happen concurrently in background threads.

Why this matters:
  You never need mutexes or locks for your JS data structures.
  Two JS functions cannot run simultaneously → no data races in JS.

  But: if two async operations complete at the same time,
  their callbacks run sequentially, not simultaneously.
  The event loop serialises them.
```

---

## 5. The Event Loop — Overview

The event loop is the mechanism that lets Node.js do many things "at once" on a single thread. It is a continuous loop that checks for work to do.

```
Simplified view:

while (there is work to do) {
  1. Run all expired timers (setTimeout, setInterval)
  2. Run pending I/O callbacks
  3. Run setImmediate callbacks
  4. Run close callbacks (socket.on('close', ...))

  After each phase: drain the microtask queue
    → run all Promise .then() callbacks
    → run all process.nextTick() callbacks
}

When there is no more work:
  No pending timers, no pending I/O, no active servers
  → Node.js exits.

  As long as something is "registered" (open server, pending timer):
  → Node.js stays alive.
```

This is covered in full depth in Part 03. The key concept here: the event loop is the heartbeat of Node.js. It is what makes async work.

---

## 6. How a Node.js Process Starts

```
$ node app.js

1. Node.js initialises the V8 engine.

2. Node.js initialises libuv.

3. Node.js sets up the global scope:
   - global object (equivalent of browser's window)
   - process object
   - require / module (CommonJS) or import (ESM)
   - Built-in globals: Buffer, setTimeout, setInterval, etc.

4. Node.js loads and executes app.js:
   - Reads the file from disk
   - Parses it (V8 builds AST)
   - Compiles to bytecode (Ignition)
   - Executes top-level code synchronously

5. Top-level code runs:
   - Imports/requires are resolved (synchronous)
   - Servers are created, listeners registered
   - Initial setup code runs
   - This is the "synchronous bootstrap phase"

6. Top-level code finishes.
   The call stack is empty.

7. Event loop begins.
   Node.js checks: any timers? any I/O to watch? any servers?
   If yes → keeps running.
   If no → exits with code 0.

Example:

// app.js
console.log('1 - synchronous, runs at startup');

setTimeout(() => console.log('3 - timer callback'), 0);

Promise.resolve().then(() => console.log('2 - microtask'));

console.log('4 - also synchronous');

// Output: 1, 4, 2, 3
// 1 and 4: synchronous, in order
// 2: microtask, runs after sync code, before timers
// 3: timer callback, runs in event loop
```

---

## 7. I/O in Node.js — What Happens When You Read a File

Tracing a file read from JavaScript all the way down and back up.

```javascript
import { readFile } from "fs/promises";

const data = await readFile("/etc/hostname", "utf8");
console.log(data);
```

```
What actually happens:

1. readFile('/etc/hostname', 'utf8') is called.
   This is a JavaScript function in Node's built-in 'fs' module.

2. The 'fs' module calls a C++ binding:
   binding.open('/etc/hostname', flags, mode, callback)

3. The C++ binding calls libuv:
   uv_fs_open(loop, req, '/etc/hostname', flags, mode, callback)

4. libuv picks a thread from the thread pool.
   That thread calls the OS:
   open("/etc/hostname", O_RDONLY)   ← blocking syscall

   The thread BLOCKS here waiting for the OS.
   The event loop thread is FREE to do other things.

5. OS opens the file. Returns a file descriptor (fd = 3).
   The thread pool thread unblocks.

6. libuv queues the result in the event loop's I/O callback phase.

7. Event loop picks it up on next iteration:
   Calls the C++ callback with fd = 3.

8. Node reads the file content:
   Similar process — uv_fs_read → thread pool → OS read → callback.

9. File content returned to JavaScript as a Buffer or string.

10. The Promise resolves.
    await resumes execution.
    data = "/etc/hostname content\n"

11. console.log(data) runs.

Total: JavaScript made one function call.
       The actual work happened in a thread pool thread and the OS.
       JavaScript was never blocked.
```

### Synchronous vs Asynchronous

```javascript
import { readFileSync, readFile } from "fs";

// Synchronous — BLOCKS the event loop
const data = readFileSync("/etc/hostname", "utf8");
// While this runs: no other callbacks can fire, no other requests served.
// Acceptable: startup (reading config files once).
// Never: during request handling in a server.

// Asynchronous — does NOT block
readFile("/etc/hostname", "utf8", (err, data) => {
  if (err) throw err;
  console.log(data);
});
// Returns immediately. Callback fires when done.
// Event loop free to handle other things meanwhile.

// Promise-based (modern — use this)
import { readFile } from "fs/promises";
const data = await readFile("/etc/hostname", "utf8");
// async/await is syntactic sugar over Promises.
// The await pauses THIS function, not the event loop.
// Other code can run while waiting.
```

---

## 8. The Node.js Architecture — All Together

```
┌──────────────────────────────────────────────────────────────────┐
│                         YOUR CODE                                │
│                                                                  │
│   import express from 'express'                                  │
│   app.get('/users', async (req, res) => {                        │
│     const users = await db.query('SELECT * FROM users');         │
│     res.json(users);                                             │
│   });                                                            │
└───────────────────────────────┬──────────────────────────────────┘
                                │ uses
┌───────────────────────────────▼──────────────────────────────────┐
│                    NODE.js STANDARD LIBRARY                      │
│         fs, http, net, crypto, path, os, stream, ...             │
└───────────────────────────────┬──────────────────────────────────┘
                                │ calls
┌───────────────────────────────▼──────────────────────────────────┐
│                      NODE.js BINDINGS (C++)                      │
│          Bridges JavaScript calls to libuv and OS APIs           │
└────────────────┬──────────────────────────┬──────────────────────┘
                 │                          │
┌────────────────▼──────┐    ┌──────────────▼───────────────────────┐
│         V8            │    │               libuv                  │
│                       │    │                                      │
│  Executes JavaScript  │    │  ┌─────────────────────────────────┐ │
│  Manages heap/stack   │    │  │         Event Loop              │ │
│  JIT compilation      │    │  │  timers → I/O → check → close  │ │
│  Garbage collection   │    │  └─────────────────────────────────┘ │
│                       │    │  ┌─────────────────────────────────┐ │
│                       │    │  │         Thread Pool             │ │
│                       │    │  │   T1 │ T2 │ T3 │ T4            │ │
│                       │    │  └─────────────────────────────────┘ │
│                       │    │  ┌─────────────────────────────────┐ │
│                       │    │  │    OS Async I/O watchers        │ │
│                       │    │  │  epoll / kqueue / IOCP          │ │
│                       │    │  └─────────────────────────────────┘ │
└───────────────────────┘    └──────────────────────────────────────┘
                                          │
┌─────────────────────────────────────────▼──────────────────────────┐
│                        OPERATING SYSTEM                            │
│              File system, Network, Processes, Timers               │
└────────────────────────────────────────────────────────────────────┘
```

---

## 9. Node.js vs Browser JavaScript

Same language, different environments. The differences matter.

```
                    Node.js                 Browser
─────────────────────────────────────────────────────────────────
Global object       global                  window
                    (globalThis in both)

Module system       CommonJS (require)       ES Modules (import)
                    ES Modules (import)      (no require)

File system         fs module (full access)  No access
Network             net, http modules        fetch, WebSocket only
Process             process object           no process
Environment vars    process.env              no direct access
OS access           os module               no access
Child processes     child_process module     Web Workers (limited)
Threads             worker_threads           Web Workers

DOM                 Does not exist           document, window, etc.
Browser APIs        Does not exist           fetch, localStorage, etc.
                    (fetch added in v18)

Timers              setTimeout, setInterval  Same API, different impl
                    setImmediate (Node-only)  requestAnimationFrame

__dirname           Available (CommonJS)     Does not exist
__filename          Available (CommonJS)     Does not exist
import.meta.url     Available (ESM)          Available (ESM)
```

### APIs That Look the Same But Differ

```javascript
// fetch — added to Node.js in v18
// Browser fetch and Node.js fetch behave almost identically.
// Node.js fetch is built on undici (a fast HTTP client).
// Main difference: no browser-specific features (no CORS enforcement).

// setTimeout
setTimeout(() => {}, 1000);
// Node: returns a Timeout object (has .ref()/.unref() methods)
// Browser: returns a number (timer ID)

// console
console.log("hello");
// Node: writes to stdout (process.stdout)
// Browser: writes to DevTools console

// Buffer (Node.js only)
const buf = Buffer.from("hello", "utf8");
// Browsers have Uint8Array and ArrayBuffer but not Buffer.
// Buffer IS a Uint8Array subclass — they're interoperable.
```

---

## 10. What Node.js Is Good At and What It Is Not

Understanding the strengths and limits shapes how you architect applications.

### Node.js Excels At

```
I/O-bound workloads:
  Web servers handling many concurrent connections.
  API gateways, proxies, middleware.
  Real-time applications (chat, live updates, WebSockets).
  Microservices communicating over HTTP/gRPC.

Why: single-threaded event loop handles thousands of concurrent I/O
     operations without the overhead of thread-per-connection models.
     Low memory usage per connection vs Java/thread-per-request models.

Streaming data:
  Processing large files without loading into memory.
  Piping HTTP responses, transforming data on the fly.
  Node's stream API is built for this.

JavaScript everywhere:
  Same language on frontend and backend.
  Shared types (TypeScript), shared validation logic, shared utilities.
  Monorepo with code sharing between Next.js and NestJS.

npm ecosystem:
  Largest package registry in the world.
  Whatever you need, there's probably a package.
```

### Node.js Struggles With

```
CPU-bound workloads:
  Image processing, video encoding, ML inference, complex calculations.

  Why: a CPU-intensive operation blocks the event loop.
  While your JS is computing prime numbers, no HTTP requests can be served.

  The event loop is a single thread. If it's busy computing, it's not I/O-ing.

  const primes = computePrimesUpTo(1_000_000);  // takes 500ms
  // During these 500ms: ALL other requests are blocked.
  // Your server is effectively frozen.

  Mitigation:
    worker_threads: run CPU work in separate threads.
    child_process: spawn a separate process.
    Break work into chunks with setImmediate between chunks.
    Offload to a dedicated service (Python for ML, Go for encoding).

Heavy synchronous operations:
  Parsing a 100MB JSON file synchronously.
  Complex regex on large strings.
  Synchronous crypto operations on large data.
  Same problem: blocks the event loop.

Long-running computations in request handlers:
  A single slow request blocks all other requests.
  In Java/Go (thread-per-request): one slow request only blocks its thread.
  In Node.js: one blocking operation blocks everyone.
```

### The Concurrency Model Trade-off

```
Thread-per-request (Java, traditional Ruby):
  1000 concurrent requests → 1000 threads
  Each thread: ~1MB memory → 1GB just for threads
  Context switching overhead: significant
  Benefit: one blocked thread doesn't affect others.

Event loop (Node.js, nginx):
  1000 concurrent requests → 1 thread, 1000 active I/O operations
  Memory: very low (no per-request thread stack)
  Context switching: none (cooperative, not preemptive)
  Benefit: high concurrency with low memory.
  Risk: one blocking operation stalls everything.

Node.js is the right choice when:
  → Requests spend most of their time waiting for I/O (DB, network, files)
  → High concurrency with modest CPU per request
  → This describes: REST APIs, BFF layers, real-time servers

Node.js is the wrong choice when:
  → Requests spend most of their time computing
  → CPU-intensive per-request work (video processing, ML)
  → Long-running background jobs with heavy computation
```

---

_Next: [Part 02 — Modules](01%20-%20Modules.md)_
