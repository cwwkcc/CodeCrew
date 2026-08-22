> Part 01 introduced the event loop as an overview. This part goes all the way in. Every phase, every queue, exact ordering rules, the difference between process.nextTick and setImmediate, why microtasks always run before macrotasks, and how to reason about execution order for any piece of async code.

---

## Table of Contents

1. [The Problem the Event Loop Solves](#1-the-problem-the-event-loop-solves)
2. [The Call Stack](#2-the-call-stack)
3. [Event Loop Phases — The Six Phases](#3-event-loop-phases--the-six-phases)
4. [Phase 1 — Timers](#4-phase-1--timers)
5. [Phase 2 — Pending Callbacks](#5-phase-2--pending-callbacks)
6. [Phase 3 & 4 — Idle and Prepare](#6-phase-3--4--idle-and-prepare)
7. [Phase 5 — Poll](#7-phase-5--poll)
8. [Phase 6 — Check (setImmediate)](#8-phase-6--check-setimmediate)
9. [Phase 7 — Close Callbacks](#9-phase-7--close-callbacks)
10. [Microtasks — process.nextTick and Promises](#10-microtasks--processnexttick-and-promises)
11. [Exact Execution Order — The Complete Rules](#11-exact-execution-order--the-complete-rules)
12. [setTimeout vs setImmediate — Which Runs First?](#12-settimeout-vs-setimmediate--which-runs-first)
13. [process.nextTick vs Promise.then — Which Runs First?](#13-processnexttick-vs-promisethen--which-runs-first)
14. [Blocking the Event Loop](#14-blocking-the-event-loop)
15. [Keeping the Event Loop Alive — ref and unref](#15-keeping-the-event-loop-alive--ref-and-unref)
16. [Worked Examples — Predicting Output](#16-worked-examples--predicting-output)

---

## 1. The Problem the Event Loop Solves

A single-threaded process can only do one thing at a time. A web server needs to handle thousands of concurrent requests. These two facts seem incompatible. The event loop is how Node.js resolves the contradiction.

```
The naive solution: threads.
  One thread per request.
  1000 concurrent requests → 1000 threads.
  Problems: memory (~1MB per thread), context switching overhead,
            synchronisation complexity.

The event loop solution: never wait.
  Instead of blocking on I/O, register a callback and move on.
  When I/O completes, the callback is queued.
  The event loop picks it up when the call stack is empty.

  One thread handles thousands of "in progress" I/O operations.
  It only executes code when there is code to execute.
  It never sits idle waiting.
```

This only works because Node.js is designed around async I/O. The event loop is not a workaround — it is the architecture.

---

## 2. The Call Stack

The call stack is where function execution happens. It is a LIFO (Last In, First Out) structure.

```javascript
function c() {
  console.log("c");
}

function b() {
  c();
}

function a() {
  b();
}

a();
```

```
Stack at each point:

a() called:
  [ a ]

b() called inside a():
  [ a, b ]

c() called inside b():
  [ a, b, c ]

c() returns:
  [ a, b ]

b() returns:
  [ a ]

a() returns:
  []   ← stack is empty

When the stack is empty → event loop can pick up the next callback.
The event loop NEVER interrupts running code.
A callback only starts when the stack is completely empty.
```

### Stack Overflow

```javascript
function infinite() {
  return infinite(); // calls itself forever
}
infinite();
// RangeError: Maximum call stack size exceeded

// Default stack size: ~10,000–15,000 frames (depends on frame size).
// Each function call consumes stack space.
// Too many nested calls → stack overflow.
```

---

## 3. Event Loop Phases — The Six Phases

libuv implements the event loop as a sequence of phases. Each phase has a FIFO queue of callbacks. The event loop processes all callbacks in a phase before moving to the next.

```
┌─────────────────────────────────────────────────────┐
│                     Event Loop                      │
│                                                     │
│   ┌──────────┐                                      │
│   │  timers  │  setTimeout, setInterval callbacks   │
│   └────┬─────┘                                      │
│        │                                            │
│   ┌────▼──────────────┐                             │
│   │ pending callbacks │  I/O errors from prev tick  │
│   └────┬──────────────┘                             │
│        │                                            │
│   ┌────▼──────┐                                     │
│   │   idle,   │  internal use only                  │
│   │  prepare  │                                     │
│   └────┬──────┘                                     │
│        │                                            │
│   ┌────▼──────┐  ┌─────────────────────────────┐   │
│   │   poll    │◄─┤  incoming I/O events         │   │
│   └────┬──────┘  └─────────────────────────────┘   │
│        │                                            │
│   ┌────▼──────┐                                     │
│   │   check   │  setImmediate callbacks             │
│   └────┬──────┘                                     │
│        │                                            │
│   ┌────▼──────────────┐                             │
│   │ close callbacks   │  socket.on('close', ...)    │
│   └────┬──────────────┘                             │
│        │                                            │
│        └──────────────────────────────────────────┐ │
│                    (next iteration)                │ │
│  ◄─────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────┘

Between EVERY phase transition:
  → Drain process.nextTick queue (all of them)
  → Drain Promise microtask queue (all of them)
```

---

## 4. Phase 1 — Timers

The timers phase executes callbacks scheduled by `setTimeout()` and `setInterval()`.

```javascript
setTimeout(() => console.log("timer"), 100);
```

```
How timers work:

libuv maintains a min-heap of timers sorted by expiry time.

At the start of the timers phase:
  Check current time.
  Run all timers whose expiry time ≤ current time.

Key: the delay is a MINIMUM, not a guarantee.
  setTimeout(fn, 100) → fn runs AT LEAST 100ms later.
  If the poll phase is busy (blocking I/O), timers fire late.
  If the system is under heavy load, timers fire late.

setTimeout(fn, 0):
  Minimum delay is 1ms (per HTML spec, Node.js follows this).
  Actually scheduled with delay = max(1, specified_delay).
  Fires on the next timers phase where delay has elapsed.
  Does NOT mean "next tick" — there is no such guarantee.
```

### setInterval

```javascript
setInterval(() => {
  console.log("interval");
}, 1000);

// Reschedules itself automatically after each execution.
// Interval is measured from when the PREVIOUS callback STARTED,
// not from when it finished.
// If the callback takes 200ms: next fires at 1000ms from start, not 1200ms.
// If the callback takes longer than the interval: callbacks pile up.
```

---

## 5. Phase 2 — Pending Callbacks

This phase executes I/O callbacks that were deferred to the next loop iteration.

```
Specifically: handles callbacks for system errors from I/O operations.
For example: TCP socket errors, ECONNREFUSED errors.

These were deferred because they occurred during the poll phase
of the previous iteration and couldn't run then.

In practice: you rarely think about this phase explicitly.
It is an implementation detail of how libuv handles certain edge cases.
```

---

## 6. Phase 3 & 4 — Idle and Prepare

```
These phases are for Node.js internal use only.
You cannot schedule code here directly.

idle:    Used internally to run certain checks every event loop iteration.
prepare: Used internally to set up the poll phase.

Not relevant for application code.
Mentioned for completeness — you will see them in libuv documentation.
```

---

## 7. Phase 5 — Poll

The poll phase is the heart of the event loop. This is where Node.js waits for I/O.

```
The poll phase does two things:

1. Calculate how long to block (wait for I/O):
   - If there are setImmediate() callbacks scheduled → don't block at all
   - If there are timers about to expire → block for at most that long
   - Otherwise → block until an I/O event arrives

2. Process I/O events that have arrived:
   - Network connections received
   - File reads completed
   - DNS lookups completed
   - etc.
   Run the associated callbacks.

"Blocking" in the poll phase:
  This is controlled blocking — libuv asks the OS:
  "Watch these file descriptors. Wake me up when any has data,
   or after this timeout, whichever comes first."

  The OS uses epoll/kqueue/IOCP — zero CPU used while waiting.
  Node is not spinning. It is sleeping and will be woken by the OS.

This is the phase that makes Node.js efficient:
  While waiting for a database query, a file read, a network response —
  Node.js is sleeping, using zero CPU, waiting to be woken.
  When the data arrives, it wakes up and processes it immediately.
```

### Poll Phase Flow

```
Enter poll phase:

Is the poll queue empty?
  NO → execute all callbacks in the poll queue → re-check queue → ...
  YES:
    Are there setImmediate() callbacks scheduled?
      YES → exit poll, go to check phase
      NO:
        Are there timers about to expire?
          YES → wait until timer expires, then go back to timers phase
          NO:
            Wait (block) for I/O events with no timeout
            When event arrives → add to poll queue → process
```

---

## 8. Phase 6 — Check (setImmediate)

The check phase runs `setImmediate()` callbacks.

```javascript
setImmediate(() => console.log("immediate"));
```

```
setImmediate() schedules a callback to run in the CHECK phase
of the CURRENT event loop iteration — after the poll phase.

This gives you a guarantee:
  "Run this after I/O callbacks in this iteration,
   before the next timers phase."

Useful for: splitting long tasks across event loop iterations
            to allow I/O callbacks to interleave.

function processLargeArray(arr, index = 0) {
  if (index >= arr.length) return;

  // Process one item
  doExpensiveWork(arr[index]);

  // Yield to event loop before processing next item
  setImmediate(() => processLargeArray(arr, index + 1));
}
// Each iteration: process one item, then let I/O callbacks run.
// Prevents starvation of I/O during heavy computation.
```

---

## 9. Phase 7 — Close Callbacks

The final phase runs close event callbacks.

```javascript
const net = require("net");
const server = net.createServer();

server.on("close", () => {
  console.log("server closed"); // runs in close callbacks phase
});

server.close();
```

```
Triggered when:
  A socket or handle is closed abruptly (not gracefully).
  server.close() completes.
  socket.destroy() is called.

After this phase:
  If there are still active handles or timers → next iteration begins.
  If nothing is left → Node.js exits.
```

---

## 10. Microtasks — process.nextTick and Promises

Microtasks are not a phase. They run between every phase transition — and between every callback within a phase in Node.js v11+.

```
Two microtask queues:

1. process.nextTick queue
   Higher priority.
   Drains completely before Promise queue.

2. Promise microtask queue
   Promise.then(), Promise.catch(), Promise.finally()
   async/await resumes
   queueMicrotask()
```

### process.nextTick

```javascript
process.nextTick(() => {
  console.log("nextTick callback");
});
```

```
process.nextTick() schedules a callback to run:
  After the current operation (synchronous code or current callback) completes.
  Before the event loop moves to the next phase (or next callback).
  Before any I/O, timers, or setImmediate.

It is NOT part of the event loop phases.
It is a special queue processed after EVERY callback completes.

Name is confusing: "tick" does not mean "next event loop iteration."
It means "after this synchronous chunk of code finishes."
```

### Promise Microtasks

```javascript
Promise.resolve().then(() => {
  console.log("promise microtask");
});
```

```
Promise callbacks (then/catch/finally) run in the Promise microtask queue.
They run after process.nextTick is drained.
They run before any I/O callbacks, timers, or setImmediate.

async/await is syntactic sugar for Promises.
After an await, the continuation is a Promise microtask:

async function example() {
  console.log('before await');
  await somePromise();
  console.log('after await');  // this runs as a Promise microtask
}
```

### Microtask Queue Draining

```
IMPORTANT: microtask queues drain COMPLETELY before the event loop proceeds.

process.nextTick(() => {
  process.nextTick(() => {
    process.nextTick(() => {
      console.log('deeply nested nextTick');
    });
  });
});
// All three nextTicks run before any I/O callback or timer.

// DANGER: infinite nextTick loop starves the event loop:
function infinite() {
  process.nextTick(infinite);  // never lets event loop proceed
}
infinite();
// No I/O, timers, or setImmediate will ever run. App is stuck.
// Same danger with infinite Promise chains.
```

---

## 11. Exact Execution Order — The Complete Rules

```
When the call stack empties after any callback or synchronous code:

Step 1: Drain process.nextTick queue
  Run ALL nextTick callbacks, in order.
  If any nextTick adds more nextTick callbacks → run those too.
  Continue until nextTick queue is empty.

Step 2: Drain Promise microtask queue
  Run ALL Promise callbacks (then/catch/finally, await continuations).
  If any Promise callback adds more → run those too.
  If a Promise callback adds a nextTick → go back to Step 1.
  Continue until Promise queue is empty.

Step 3: Move to next event loop phase (or next callback in current phase)
  Repeat steps 1 and 2 after each callback.
```

### Node.js v11+ Change

```
Before v11: microtasks ran between PHASES only.
After v11:  microtasks run between each CALLBACK within a phase.

// Before v11:
setTimeout(() => console.log('timer 1'), 0);
setTimeout(() => console.log('timer 2'), 0);
Promise.resolve().then(() => console.log('promise'));

// Before v11: timer 1, timer 2, promise
// After v11:  timer 1, promise, timer 2

// After v11, after each timer callback, microtasks drain.
// So: timer 1 runs → microtasks drain (promise) → timer 2 runs.

Node.js v11+ behaviour is what you should assume.
It aligns with browser behaviour and is more intuitive.
```

---

## 12. setTimeout vs setImmediate — Which Runs First?

```javascript
setTimeout(() => console.log("timeout"), 0);
setImmediate(() => console.log("immediate"));
```

```
ANSWER: it depends on where this code runs.

Outside an I/O callback (e.g. main module):
  Non-deterministic. Order depends on system performance.

  Why: setTimeout(fn, 0) is scheduled with delay=1ms.
  By the time the event loop starts:
    If 1ms has passed → timers phase fires first → timeout first
    If 1ms has NOT passed → timers phase skipped → check phase → immediate first

  The timing of process startup is not deterministic.
  Both orders are possible. Do not rely on either.

Inside an I/O callback:
  setImmediate ALWAYS runs before setTimeout.
  Guaranteed.

  const fs = require('fs');
  fs.readFile('/tmp/test', () => {
    setTimeout(() => console.log('timeout'), 0);
    setImmediate(() => console.log('immediate'));
    // immediate always first
  });

  Why: after an I/O callback completes, the poll phase continues.
  After poll: check phase (setImmediate) runs.
  After check: timers phase (setTimeout) runs.
  setImmediate is always ahead of setTimeout in this context.

Practical rule:
  If you need "run after I/O, before next timer" → use setImmediate.
  If you need "run after at least Nms" → use setTimeout.
  Don't rely on their relative order in the main module.
```

---

## 13. process.nextTick vs Promise.then — Which Runs First?

```javascript
Promise.resolve().then(() => console.log("promise"));
process.nextTick(() => console.log("nextTick"));
```

```
ANSWER: nextTick ALWAYS runs before Promise.then.

Output: nextTick, promise

This is because:
  process.nextTick queue drains completely BEFORE Promise microtask queue.
  This is the fixed priority order.

Nested example:
  process.nextTick(() => {
    console.log('nextTick 1');
    Promise.resolve().then(() => console.log('promise inside nextTick'));
    process.nextTick(() => console.log('nextTick 2'));
  });
  Promise.resolve().then(() => console.log('promise 1'));

  Output:
    nextTick 1
    nextTick 2           ← nextTick added during nextTick runs before promises
    promise inside nextTick
    promise 1

  Why:
    nextTick queue: [nextTick 1]
    Run nextTick 1 → adds promise to Promise queue, adds nextTick 2 to nextTick queue
    nextTick queue: [nextTick 2]
    Run nextTick 2
    nextTick queue: [] → switch to Promise queue
    Promise queue: [promise inside nextTick, promise 1]
    Run promise inside nextTick
    Run promise 1
```

---

## 14. Blocking the Event Loop

Blocking the event loop is the most common Node.js performance mistake. When the event loop is blocked, ALL clients wait.

### What Blocks

```javascript
// Synchronous I/O — blocks for the duration of the OS call
const data = fs.readFileSync("/large/file"); // could be 50ms+

// CPU-intensive synchronous computation
function fibonacci(n) {
  if (n <= 1) return n;
  return fibonacci(n - 1) + fibonacci(n - 2);
}
fibonacci(45); // takes seconds — event loop frozen

// Large synchronous operations
const data = JSON.parse(hugeJsonString); // parsing 10MB JSON synchronously
const result = crypto.pbkdf2Sync(password, salt, 100000, 64, "sha512");

// Regex catastrophic backtracking
const re = /^(a+)+$/;
re.test("aaaaaaaaaaaaaaaaaaaaaaaaaab"); // exponential time

// Long synchronous loops
for (let i = 0; i < 1_000_000_000; i++) {
  /* nothing */
}
```

### What Doesn't Block

```javascript
// Async I/O — deferred to thread pool / OS
await fs.promises.readFile("/large/file"); // non-blocking ✓

// Network requests
await fetch("https://api.example.com"); // non-blocking ✓

// setTimeout / setImmediate
setTimeout(() => {}, 1000); // non-blocking ✓

// Properly chunked CPU work
async function processChunked(items) {
  for (let i = 0; i < items.length; i++) {
    process(items[i]);
    if (i % 1000 === 0) {
      await new Promise((resolve) => setImmediate(resolve));
      // yield to event loop every 1000 items
    }
  }
}
```

### Measuring Event Loop Lag

```javascript
// Measure event loop delay:
// A setTimeout(fn, 1) should fire after 1ms.
// If it fires after 50ms, the event loop was blocked for ~49ms.

let last = Date.now();
setInterval(() => {
  const now = Date.now();
  const lag = now - last - 1000; // expected 1000ms
  if (lag > 50) {
    console.warn(`Event loop lag: ${lag}ms`);
  }
  last = now;
}, 1000);

// Node.js built-in: perf_hooks monitorEventLoopDelay
const { monitorEventLoopDelay } = require("perf_hooks");
const histogram = monitorEventLoopDelay({ resolution: 20 });
histogram.enable();
setInterval(() => {
  console.log("mean lag:", histogram.mean / 1e6, "ms");
  console.log("p99  lag:", histogram.percentile(99) / 1e6, "ms");
  histogram.reset();
}, 5000);
```

---

## 15. Keeping the Event Loop Alive — ref and unref

Node.js exits when there is nothing left to do. Active handles (servers, timers, connections) keep it alive.

```javascript
// A server keeps the event loop alive:
const server = http.createServer();
server.listen(3000);
// Node does not exit — server is an active handle.

// A repeating timer keeps the event loop alive:
const timer = setInterval(() => {}, 1000);
// Node does not exit — timer is active.

// To stop keeping alive without cancelling:
timer.unref();
// Node CAN exit now, even though timer is still "set".
// If something ELSE keeps the event loop alive, timer still fires.
// If nothing else does, Node exits.

// To re-register as keeping alive:
timer.ref();

// Common use: a keep-alive timer for connections.
// You want the timer to fire IF the server is running,
// but you don't want the timer alone to keep Node running.

// process.exit() — force exit regardless of active handles
process.exit(0); // 0 = success
process.exit(1); // non-zero = failure
```

### When Node.js Exits Naturally

```
Node.js exits when:
  - Event loop has no more phases to run (all queues empty)
  - No active handles (no servers, no active timers, no open connections)
  - No active requests (no pending I/O)

Active handles that keep Node alive:
  HTTP/TCP servers (until .close() is called)
  setInterval timers (until clearInterval or .unref())
  Open network sockets
  File watchers (fs.watch)
  Child processes (until they exit)
  Worker threads (until they exit)
```

---

## 16. Worked Examples — Predicting Output

Work through these to solidify the mental model.

### Example 1 — Basic Order

```javascript
console.log("1");

setTimeout(() => console.log("2"), 0);

Promise.resolve().then(() => console.log("3"));

process.nextTick(() => console.log("4"));

console.log("5");
```

```
Trace:
  '1' — synchronous
  setTimeout registered (will fire in timers phase)
  Promise.then registered (microtask queue)
  process.nextTick registered (nextTick queue)
  '5' — synchronous

Call stack empties. Microtasks run:
  nextTick queue: [4] → '4'
  Promise queue:  [3] → '3'

Event loop: timers phase
  setTimeout fires → '2'

Output: 1, 5, 4, 3, 2
```

### Example 2 — Nested Microtasks

```javascript
process.nextTick(() => {
  console.log("A");
  process.nextTick(() => console.log("B"));
});

process.nextTick(() => console.log("C"));

Promise.resolve()
  .then(() => {
    console.log("D");
    process.nextTick(() => console.log("E"));
  })
  .then(() => console.log("F"));
```

```
Trace:
  Register nextTick(A), nextTick(C), Promise chain.
  Call stack empties.

nextTick queue: [A, C]
  Run A → logs 'A', registers nextTick(B)
  nextTick queue: [C, B]
  Run C → logs 'C'
  nextTick queue: [B]
  Run B → logs 'B'
  nextTick queue: []

Promise queue: [D handler]
  Run D handler → logs 'D', registers nextTick(E)
  nextTick queue: [E]   ← nextTick added during Promise callback

  Before running next Promise callback: drain nextTick queue
  Run E → logs 'E'

  Promise queue: [F handler]
  Run F handler → logs 'F'

Output: A, C, B, D, E, F
```

### Example 3 — I/O and setImmediate

```javascript
const fs = require("fs");

fs.readFile("/etc/hostname", () => {
  console.log("readFile callback");

  setTimeout(() => console.log("timeout inside I/O"), 0);
  setImmediate(() => console.log("immediate inside I/O"));
  process.nextTick(() => console.log("nextTick inside I/O"));
});

console.log("synchronous");
```

```
Trace:
  'synchronous' — runs immediately
  readFile starts (async, thread pool)
  Call stack empties, event loop starts.

  [some time later, file read completes]

  poll phase: readFile callback fires
    logs 'readFile callback'
    registers: setTimeout, setImmediate, nextTick

  readFile callback completes. Microtasks:
    nextTick: 'nextTick inside I/O'

  poll phase continues (nothing else in poll queue)
  → check phase: setImmediate fires
    'immediate inside I/O'

  → next iteration, timers phase: setTimeout fires
    'timeout inside I/O'

Output: synchronous, readFile callback, nextTick inside I/O,
        immediate inside I/O, timeout inside I/O
```

### Example 4 — async/await Under the Hood

```javascript
async function main() {
  console.log("1");
  await Promise.resolve();
  console.log("2");
}

main();
console.log("3");
```

```
async function is a Promise wrapper.
await Promise.resolve() suspends main() and schedules
its continuation as a Promise microtask.

Trace:
  main() called → '1'
  await Promise.resolve() → suspends main()
  main() returns a Promise (not yet resolved)
  '3' — synchronous (back in caller)
  Call stack empties.

  Microtask queue: [main() continuation]
  Run continuation → '2'

Output: 1, 3, 2
```

### Example 5 — The Full Picture

```javascript
console.log("start");

setTimeout(() => console.log("timeout 1"), 0);
setTimeout(() => console.log("timeout 2"), 0);

Promise.resolve()
  .then(() => {
    console.log("promise 1");
    process.nextTick(() => console.log("nextTick inside promise"));
  })
  .then(() => console.log("promise 2"));

process.nextTick(() => {
  console.log("nextTick 1");
  Promise.resolve().then(() => console.log("promise inside nextTick"));
});

process.nextTick(() => console.log("nextTick 2"));

console.log("end");
```

```
Trace:
  'start', 'end' — synchronous

nextTick queue: [nextTick1, nextTick2]
  Run nextTick1:
    'nextTick 1'
    registers Promise(promise inside nextTick)
  Run nextTick2:
    'nextTick 2'
  nextTick queue: []

Promise queue: [promise1 handler, promise inside nextTick]
  Run promise1 handler:
    'promise 1'
    registers nextTick(nextTick inside promise)
    .then chained → registers promise2 handler

    After this callback: drain nextTick queue
    nextTick queue: [nextTick inside promise]
    Run: 'nextTick inside promise'

  Promise queue: [promise inside nextTick, promise2 handler]
  Run promise inside nextTick:
    'promise inside nextTick'
  Run promise2 handler:
    'promise 2'

Event loop: timers phase
  'timeout 1'
  Microtasks: (none)
  'timeout 2'

Output: start, end, nextTick 1, nextTick 2, promise 1,
        nextTick inside promise, promise inside nextTick,
        promise 2, timeout 1, timeout 2
```

---

_Next: [Part 04 — Buffers & Streams](./Part%2004%20-%20Buffers%20%26%20Streams.md)_
