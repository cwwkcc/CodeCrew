> **Part 7 of 7.** The full async story — sync vs async, the event loop in depth, callback hell, Promises from scratch, async/await, reading files, JSON data, the Fetch API, and comprehensive error handling.

---

## Table of Contents

1. [Sync vs Async Programming](#1-sync-vs-async-programming)
2. [Real-World Async Examples](#2-real-world-async-examples)
3. [Callback Hell](#3-callback-hell)
4. [Promises — From Scratch](#4-promises--from-scratch)
5. [Promise Combinators](#5-promise-combinators)
6. [Async / Await](#6-async--await)
7. [Reading Textual Data](#7-reading-textual-data)
8. [Handling JSON Data](#8-handling-json-data)
9. [Fetching Data from APIs](#9-fetching-data-from-apis)
10. [Error Handling](#10-error-handling)

---

## 1. Sync vs Async Programming

### Synchronous (Blocking)

Synchronous code runs **line by line, in order**. Each line must finish before the next begins. If one line is slow (disk read, network call), the entire thread waits — blocking everything including the UI.

```js
// Synchronous — predictable, sequential
console.log("Step 1");
const result = computeSomething(); // blocks until done
console.log("Step 2 — only runs after computeSomething finishes");
console.log("Step 3");
```

Synchronous code is fine for CPU-bound work (math, parsing, sorting). It's a problem for **I/O-bound work** (waiting on the network, disk, database) — you'd be blocking the thread just to wait.

### Asynchronous (Non-Blocking)

You start an operation, register a callback (or return a Promise), and continue running other code immediately. When the operation completes, your callback runs.

```js
console.log("1 — start");

setTimeout(() => {
  console.log("3 — this runs after the stack clears");
}, 0); // even 0ms is async — goes through the event loop

console.log("2 — runs before the timeout callback");
// Output: 1, 2, 3
```

### The Event Loop — The Full Picture

JavaScript is **single-threaded**: one call stack, one thing at a time. But the runtime (browser or Node.js) has background threads for I/O. Here's how async work gets done:

```
┌──────────────────────────────────────────────┐
│                  Call Stack                  │ ← JS runs here, one frame at a time
├──────────────────────────────────────────────┤
│             Web APIs / Node APIs             │ ← Background threads handle timers,
│  setTimeout, fetch, fs.readFile, DOM events  │   network, disk I/O
├──────────────────────────────────────────────┤
│           Microtask Queue (priority)         │ ← Promise .then(), queueMicrotask()
├──────────────────────────────────────────────┤
│            Macrotask Queue (tasks)           │ ← setTimeout, setInterval, I/O events
└──────────────────────────────────────────────┘

Event Loop algorithm:
  1. Execute everything on the call stack (synchronous code)
  2. Drain the ENTIRE microtask queue (all Promises resolve)
  3. Pick ONE macrotask (setTimeout/setInterval/I/O)
  4. Run it → go back to step 2
  5. Repeat
```

```js
// Demonstrating the full order
console.log("A"); // sync

setTimeout(() => console.log("E"), 0); // macrotask

Promise.resolve()
  .then(() => console.log("C"))  // microtask
  .then(() => console.log("D")); // microtask (chained)

console.log("B"); // sync

// Output: A, B, C, D, E
// A, B — sync stack
// C, D — microtask queue (drained before any macrotask)
// E     — macrotask
```

### Why Microtasks Run Before Macrotasks

This matters in real code:

```js
async function fetchUser(id) {
  const user = await db.findUser(id);     // awaits = microtask
  const perms = await db.getPerms(id);   // awaits = microtask
  return { user, perms };
}

setTimeout(checkSessionExpiry, 0);  // macrotask — won't run until fetchUser resolves

fetchUser(1).then(render);  // microtasks complete first
```

---

## 2. Real-World Async Examples

### Why Async Matters in a Web App

```js
// Synchronous DB call — BLOCKS everything (hypothetical, never do this)
function getUserSync(id) {
  const result = db.querySync(`SELECT * FROM users WHERE id = ${id}`);
  // During this DB round-trip (maybe 50ms), NOTHING else can run:
  // - No other HTTP requests processed
  // - No UI updates
  // - No timers fire
  return result;
}

// Asynchronous — proper way
async function getUser(id) {
  const result = await db.query("SELECT * FROM users WHERE id = $1", [id]);
  // While waiting for DB: event loop is free to handle other requests, timers, etc.
  return result;
}
```

### The Async Timeline

```
Time ─────────────────────────────────────────────────────────▶

Request 1: ──[start]──[waiting for DB]────────────────[got result]──[send response]
                              │
                              └── Event loop is free
                                         │
Request 2:               ──[start]──[waiting for DB]──────[got result]──[send response]
                                              │
Request 3:                        ──[start]──[waiting...]──[got result]──[send response]

Synchronous server would handle ONE request at a time.
Async server handles thousands concurrently.
```

---

## 3. Callback Hell

Before Promises (pre-2015), all async code used callbacks. When operations depend on each other, callbacks nest — leading to the infamous "pyramid of doom."

### The Problem

```js
// Loading a user's dashboard: fetch user → fetch orders → fetch recommendations
// Each step depends on the previous result

getUser(userId, function(err, user) {
  if (err) return handleError(err);

  getOrders(user.id, function(err, orders) {
    if (err) return handleError(err);

    getRecommendations(user.preferences, function(err, recs) {
      if (err) return handleError(err);

      getInventory(recs.map(r => r.productId), function(err, inventory) {
        if (err) return handleError(err);

        // Finally do something with all this data
        // By this point we're 4 levels deep
        renderDashboard({
          user,
          orders,
          recommendations: recs.filter(r => inventory[r.productId]?.inStock),
        });
      });
    });
  });
});
```

### Why It's a Problem

1. **Deeply nested** — hard to read, hard to reason about
2. **Error handling is repetitive** — every level needs its own `if (err)` check
3. **Control flow is hard** — doing things in parallel (not just sequential) becomes a nightmare
4. **Debugging is painful** — stack traces are confusing

### The Parallel Callback Problem

```js
// Fetching user AND orders at the same time (not sequential)
// This is genuinely hard with callbacks:

let user, orders, errors = [];

function checkDone() {
  if (user !== undefined && orders !== undefined) {
    if (errors.length) return handleError(errors[0]);
    renderDashboard({ user, orders });
  }
}

getUser(userId, (err, result) => {
  if (err) { errors.push(err); checkDone(); return; }
  user = result;
  checkDone();
});

getOrders(userId, (err, result) => {
  if (err) { errors.push(err); checkDone(); return; }
  orders = result;
  checkDone();
});

// This is disgusting. Promise.all() does this in one line.
```

---

## 4. Promises — From Scratch

A **Promise** is an object representing the **eventual result** of an asynchronous operation. It is in one of three states:

- **Pending** — operation not yet complete
- **Fulfilled** — operation completed successfully (has a value)
- **Rejected** — operation failed (has a reason/error)

Once settled (fulfilled or rejected), a Promise's state **never changes**.

### Creating a Promise

```js
const promise = new Promise((resolve, reject) => {
  // This function runs immediately and synchronously
  // `resolve(value)` — fulfills the promise with value
  // `reject(reason)` — rejects the promise with reason

  // Simulate async work
  setTimeout(() => {
    const success = Math.random() > 0.5;
    if (success) {
      resolve({ id: 1, name: "Alice" }); // fulfill
    } else {
      reject(new Error("Failed to fetch user")); // reject
    }
  }, 1000);
});
```

### Consuming a Promise with `.then()` / `.catch()` / `.finally()`

```js
promise
  .then(user => {
    // Runs if promise was FULFILLED
    // Receives the resolve value
    console.log("Got user:", user);
    return user.id; // return value becomes the next .then()'s input
  })
  .then(userId => {
    console.log("User ID:", userId);
    return fetchOrders(userId); // can return another promise — chains!
  })
  .catch(error => {
    // Runs if ANY promise in the chain was REJECTED
    // Also catches errors thrown in .then() handlers
    console.error("Something failed:", error.message);
    return []; // recover — chain continues with this value
  })
  .finally(() => {
    // Runs regardless of success or failure
    hideLoadingSpinner();
  });
```

### Promise Chaining

The key insight: `.then()` **always returns a new Promise**. You can chain them.

```js
// Each .then() receives the return value of the previous one
// If you return a Promise from .then(), the chain waits for it

fetchUser(userId)                           // Promise<User>
  .then(user => fetchOrders(user.id))       // returns Promise<Order[]>
  .then(orders => {
    const recentOrders = orders.filter(o => isRecent(o.date));
    return recentOrders;                    // returns plain value — auto-wrapped in Promise
  })
  .then(recentOrders => renderOrders(recentOrders))
  .catch(err => showErrorMessage(err.message))
  .finally(() => setLoading(false));
```

### Promisifying Callback-Based APIs

Converting Node.js callback-style functions to Promises:

```js
// Manual promisify
function readFilePromise(path, encoding) {
  return new Promise((resolve, reject) => {
    fs.readFile(path, encoding, (err, data) => {
      if (err) reject(err);
      else resolve(data);
    });
  });
}

// Node.js has a built-in util.promisify
const { promisify } = require("util");
const readFile = promisify(fs.readFile);

// Usage
readFile("./config.json", "utf8")
  .then(data => JSON.parse(data))
  .then(config => initApp(config))
  .catch(err => console.error("Failed to load config:", err));
```

### Building Your Own Promise-Based API

```js
// Real-world: wrapping IndexedDB (complex callback-based browser API)
function openDatabase(name, version) {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open(name, version);

    request.onerror = () => reject(request.error);
    request.onsuccess = () => resolve(request.result);

    request.onupgradeneeded = (event) => {
      const db = event.target.result;
      if (!db.objectStoreNames.contains("sessions")) {
        db.createObjectStore("sessions", { keyPath: "id" });
      }
    };
  });
}

// Real-world: delay utility
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

// Usage: retry with delay
async function retryWithDelay(fn, retries = 3, delayMs = 1000) {
  for (let i = 0; i < retries; i++) {
    try {
      return await fn();
    } catch (err) {
      if (i === retries - 1) throw err; // last attempt — rethrow
      console.warn(`Attempt ${i + 1} failed, retrying in ${delayMs}ms...`);
      await sleep(delayMs * 2 ** i); // exponential backoff: 1s, 2s, 4s
    }
  }
}
```

---

## 5. Promise Combinators

These static methods let you work with multiple Promises at once.

### `Promise.all` — All Must Succeed

Runs all promises in parallel. Resolves when ALL resolve. Rejects immediately if ANY reject ("fail fast").

```js
// Fetch user, orders, and recommendations in parallel (not sequentially)
const [user, orders, recommendations] = await Promise.all([
  fetchUser(userId),
  fetchOrders(userId),
  fetchRecommendations(userId),
]);

// All three requests fire simultaneously!
// Total time = slowest request (not sum of all)
```

```js
// Real-world: dashboard data loading
async function loadDashboard(userId) {
  setLoading(true);
  try {
    const [profile, stats, notifications, recentActivity] = await Promise.all([
      api.getProfile(userId),
      api.getStats(userId),
      api.getNotifications(userId),
      api.getActivity(userId, { limit: 10 }),
    ]);

    return { profile, stats, notifications, recentActivity };
  } finally {
    setLoading(false);
  }
}
```

**Gotcha:** If any Promise rejects, Promise.all rejects with that error and the other results are lost. If you need partial results even when some fail, use `allSettled`.

### `Promise.allSettled` — Wait for All, Get All Results

Waits for ALL promises to settle (fulfill OR reject). Never rejects itself. Returns array of result objects.

```js
const results = await Promise.allSettled([
  fetchUser(1),
  fetchUser(2),   // this one might fail
  fetchUser(3),
]);

results.forEach(result => {
  if (result.status === "fulfilled") {
    console.log("Got user:", result.value);
  } else {
    console.error("Failed:", result.reason.message);
  }
});

// Real-world: send emails to a batch, report per-email results
async function sendBulkEmails(recipients) {
  const results = await Promise.allSettled(
    recipients.map(r => sendEmail(r.email, buildEmailContent(r)))
  );

  const sent = results.filter(r => r.status === "fulfilled").length;
  const failed = results.filter(r => r.status === "rejected");

  console.log(`Sent: ${sent}/${recipients.length}`);
  failed.forEach((f, i) => {
    console.error(`Failed for ${recipients[i].email}: ${f.reason.message}`);
  });
}
```

### `Promise.race` — First One Wins

Resolves or rejects with the first Promise to settle. All others are ignored (but still run in the background).

```js
// Timeout pattern: race a fetch against a timer
function fetchWithTimeout(url, timeoutMs = 5000) {
  const fetchPromise = fetch(url);
  const timeoutPromise = new Promise((_, reject) =>
    setTimeout(() => reject(new Error(`Request timed out after ${timeoutMs}ms`)), timeoutMs)
  );
  return Promise.race([fetchPromise, timeoutPromise]);
}

// Retry with first success: try multiple servers, use whichever responds first
const data = await Promise.race([
  fetch("https://primary-server.com/api/data"),
  fetch("https://backup-server.com/api/data"),
]);
```

### `Promise.any` — First Success

Resolves with the first fulfilled promise. Only rejects if ALL reject (AggregateError).

```js
// Try multiple CDNs, use whichever loads first successfully
const resource = await Promise.any([
  fetch("https://cdn1.example.com/resource.js"),
  fetch("https://cdn2.example.com/resource.js"),
  fetch("https://cdn3.example.com/resource.js"),
]);
// Even if cdn1 and cdn2 fail, cdn3 can still succeed
```

### Combinator Summary

|Method|Resolves when|Rejects when|Use case|
|---|---|---|---|
|`all`|ALL fulfill|ANY rejects|Need all results, any failure is fatal|
|`allSettled`|ALL settle|Never|Need all results regardless of failures|
|`race`|FIRST settles|FIRST rejects|Timeout patterns, fastest server|
|`any`|FIRST fulfills|ALL reject|Fallback sources, any success works|

---

## 6. Async / Await

`async/await` is syntactic sugar over Promises. It makes async code look and behave like synchronous code — easier to read, write, and debug.

### Basic Syntax

```js
// An `async` function always returns a Promise
async function fetchUser(id) {
  // `await` pauses execution until the Promise resolves
  // The paused code doesn't block the event loop — other tasks can run
  const response = await fetch(`/api/users/${id}`);
  const user = await response.json();
  return user; // auto-wrapped in Promise.resolve(user)
}

// Consuming it
fetchUser(1).then(user => console.log(user));
// or with await in another async function
const user = await fetchUser(1);
```

### `await` Pauses the Current Function, Not the Thread

```js
async function loadData() {
  console.log("A — before await");
  const data = await fetch("/api/data"); // pause THIS function
  console.log("C — after await (data arrived)");
  return data;
}

loadData();
console.log("B — runs while loadData is paused at await");

// Output: A, B, C
```

### Replacing Promise Chains with async/await

```js
// Promise chain version
function loadUserDashboard(userId) {
  return fetchUser(userId)
    .then(user => {
      return fetchOrders(user.id)
        .then(orders => ({ user, orders }));
    })
    .then(({ user, orders }) => fetchRecs(user.preferences, orders))
    .then(recs => renderDashboard(recs))
    .catch(err => showError(err));
}

// async/await version — same behavior, much clearer
async function loadUserDashboard(userId) {
  try {
    const user = await fetchUser(userId);
    const orders = await fetchOrders(user.id);
    const recs = await fetchRecs(user.preferences, orders);
    renderDashboard(recs);
  } catch (err) {
    showError(err);
  }
}
```

### Sequential vs Parallel with async/await

```js
// ❌ SEQUENTIAL — each waits for the previous (slow, 3× the time)
async function loadDashboardSlow(userId) {
  const user    = await fetchUser(userId);     // wait ~100ms
  const orders  = await fetchOrders(userId);   // wait ~150ms
  const profile = await fetchProfile(userId);  // wait ~80ms
  // Total: ~330ms
}

// ✅ PARALLEL — all run at the same time
async function loadDashboardFast(userId) {
  const [user, orders, profile] = await Promise.all([
    fetchUser(userId),
    fetchOrders(userId),
    fetchProfile(userId),
  ]);
  // Total: ~150ms (slowest request)
}

// ✅ PARALLEL with individual error handling
async function loadDashboardRobust(userId) {
  const [userResult, ordersResult] = await Promise.allSettled([
    fetchUser(userId),
    fetchOrders(userId),
  ]);

  const user   = userResult.status === "fulfilled" ? userResult.value : null;
  const orders = ordersResult.status === "fulfilled" ? ordersResult.value : [];

  return { user, orders };
}
```

### Async in Loops

```js
const userIds = [1, 2, 3, 4, 5];

// ❌ forEach — doesn't await! All run and finish without waiting
userIds.forEach(async (id) => {
  const user = await fetchUser(id); // forEach doesn't care about this Promise
  processUser(user); // order not guaranteed, errors swallowed
});

// ✅ Sequential: for...of with await
for (const id of userIds) {
  const user = await fetchUser(id); // waits for each
  await processUser(user);
}
// Predictable order, easy error handling — but slow (sequential)

// ✅ Parallel: Promise.all + map
const users = await Promise.all(userIds.map(id => fetchUser(id)));
// All fire at once — fast

// ✅ Parallel with concurrency limit (rate limiting)
async function mapWithConcurrency(items, fn, concurrency = 5) {
  const results = [];
  for (let i = 0; i < items.length; i += concurrency) {
    const batch = items.slice(i, i + concurrency);
    const batchResults = await Promise.all(batch.map(fn));
    results.push(...batchResults);
  }
  return results;
}

// Send emails 10 at a time (not all 1000 simultaneously)
const results = await mapWithConcurrency(emailList, sendEmail, 10);
```

### Top-Level Await (ES2022)

In ES modules, you can use `await` at the top level (outside any async function):

```js
// config.js (ES module)
const config = await fetch("/api/config").then(r => r.json());
export const API_URL = config.apiUrl;
export const TIMEOUT = config.timeout;

// Any module importing this will wait for the fetch to complete
```

---

## 7. Reading Textual Data

### In the Browser

```js
// Reading a file selected by the user (File API)
const fileInput = document.querySelector("input[type='file']");

fileInput.addEventListener("change", async (event) => {
  const file = event.target.files[0];
  if (!file) return;

  // Read as text
  const text = await file.text();
  console.log("File content:", text);

  // Read as ArrayBuffer (for binary files)
  const buffer = await file.arrayBuffer();

  // Read as data URL (for images)
  const dataUrl = await file.readAsDataURL?.() ?? await readAsDataURL(file);
});

// FileReader (older API)
function readFileAsText(file) {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = (e) => resolve(e.target.result);
    reader.onerror = () => reject(reader.error);
    reader.readAsText(file, "utf-8");
  });
}

// Reading a CSV file and parsing it
async function parseCSVFile(file) {
  const text = await file.text();
  const lines = text.trim().split("\n");
  const headers = lines[0].split(",").map(h => h.trim());

  return lines.slice(1).map(line => {
    const values = line.split(",");
    return headers.reduce((obj, header, i) => {
      obj[header] = values[i]?.trim() ?? "";
      return obj;
    }, {});
  });
}
```

### In Node.js

```js
const fs = require("fs");
const path = require("path");

// Modern: fs.promises (returns Promises — use with async/await)
const { readFile, writeFile, readdir, stat } = require("fs/promises");

async function readConfig(configPath) {
  try {
    const raw = await readFile(configPath, "utf-8");
    return JSON.parse(raw);
  } catch (err) {
    if (err.code === "ENOENT") {
      // File doesn't exist — return defaults
      return { port: 3000, debug: false };
    }
    throw err;
  }
}

async function processLogFile(logPath) {
  const content = await readFile(logPath, "utf-8");
  const lines = content.split("\n").filter(Boolean);

  return lines.map(line => {
    const match = line.match(/^\[(\w+)\] (\d{4}-\d{2}-\d{2}) (.+)$/);
    if (!match) return null;
    const [, level, date, message] = match;
    return { level, date: new Date(date), message };
  }).filter(Boolean);
}

// Reading large files with streams (memory-efficient)
const { createReadStream } = require("fs");
const { createInterface } = require("readline");

async function processLargeCSV(filePath, processRow) {
  const fileStream = createReadStream(filePath);
  const rl = createInterface({ input: fileStream, crlfDelay: Infinity });

  let isFirstLine = true;
  let headers = [];

  for await (const line of rl) {
    if (isFirstLine) {
      headers = line.split(",").map(h => h.trim());
      isFirstLine = false;
      continue;
    }
    const values = line.split(",");
    const row = Object.fromEntries(headers.map((h, i) => [h, values[i]?.trim()]));
    await processRow(row);
  }
}

// Usage: process a 10GB CSV without loading it all into memory
await processLargeCSV("./orders.csv", async (row) => {
  await db.orders.upsert(row);
});
```

---

## 8. Handling JSON Data

### Fetching and Parsing JSON

```js
// The modern way — fetch automatically handles JSON
async function getUsers() {
  const response = await fetch("/api/users");

  if (!response.ok) {
    // response.json() might still have error details
    const errorData = await response.json().catch(() => ({}));
    throw new Error(errorData.message ?? `HTTP ${response.status}`);
  }

  return response.json(); // Returns Promise<any> — parses body as JSON
}
```

### Storing and Loading JSON in Node.js

```js
const { readFile, writeFile } = require("fs/promises");

async function loadData(filePath, defaultData = {}) {
  try {
    const raw = await readFile(filePath, "utf-8");
    return JSON.parse(raw);
  } catch (err) {
    if (err.code === "ENOENT") return defaultData;
    if (err instanceof SyntaxError) {
      console.error(`Malformed JSON in ${filePath}:`, err.message);
      return defaultData;
    }
    throw err;
  }
}

async function saveData(filePath, data) {
  const json = JSON.stringify(data, null, 2); // pretty-print
  await writeFile(filePath, json, "utf-8");
}

// Atomic write — prevents corrupt file if process crashes mid-write
async function saveDataAtomic(filePath, data) {
  const tmpPath = `${filePath}.tmp`;
  try {
    await writeFile(tmpPath, JSON.stringify(data, null, 2), "utf-8");
    await require("fs/promises").rename(tmpPath, filePath); // atomic on most systems
  } catch (err) {
    await require("fs/promises").unlink(tmpPath).catch(() => {});
    throw err;
  }
}
```

### Transforming JSON for Different Contexts

```js
// API returns snake_case, your app uses camelCase
function toCamelCase(str) {
  return str.replace(/_([a-z])/g, (_, letter) => letter.toUpperCase());
}

function transformKeys(obj, transform) {
  if (Array.isArray(obj)) return obj.map(item => transformKeys(item, transform));
  if (obj !== null && typeof obj === "object") {
    return Object.fromEntries(
      Object.entries(obj).map(([key, value]) => [
        transform(key),
        transformKeys(value, transform),
      ])
    );
  }
  return obj;
}

// Convert entire API response from snake_case to camelCase
const rawResponse = {
  user_id: 1,
  full_name: "Alice Chen",
  email_address: "alice@example.com",
  created_at: "2024-01-15",
  order_history: [{ order_id: 1, order_total: 99.99 }],
};

const camelCased = transformKeys(rawResponse, toCamelCase);
// { userId: 1, fullName: "Alice Chen", emailAddress: "...", orderHistory: [...] }
```

### JSON Schema Validation

```js
// Simple validator for API responses (without a library)
function validateUserSchema(data) {
  const errors = [];

  if (typeof data.id !== "number") errors.push("id must be a number");
  if (typeof data.name !== "string" || !data.name.trim()) errors.push("name is required");
  if (typeof data.email !== "string" || !data.email.includes("@")) errors.push("valid email required");
  if (!["user", "admin", "moderator"].includes(data.role)) errors.push("invalid role");

  return { valid: errors.length === 0, errors };
}

// In production: use Zod, Joi, or Yup for full schema validation
import { z } from "zod";

const UserSchema = z.object({
  id: z.number().positive(),
  name: z.string().min(1).max(100),
  email: z.string().email(),
  role: z.enum(["user", "admin", "moderator"]),
  isActive: z.boolean().default(true),
  createdAt: z.string().datetime().optional(),
});

async function fetchAndValidateUser(id) {
  const raw = await fetch(`/api/users/${id}`).then(r => r.json());
  const result = UserSchema.safeParse(raw);

  if (!result.success) {
    console.error("API returned invalid data:", result.error.issues);
    throw new Error("Invalid API response shape");
  }

  return result.data; // fully typed, validated
}
```

---

## 9. Fetching Data from APIs

### The Fetch API — Complete Guide

`fetch` is the modern browser and Node.js (18+) API for HTTP requests.

```js
// Basic GET
const response = await fetch("https://api.example.com/users");
const users = await response.json();
```

### The Response Object

```js
const response = await fetch(url);

response.ok;          // true if status 200–299
response.status;      // 200, 201, 400, 401, 404, 500, etc.
response.statusText;  // "OK", "Not Found", etc.
response.headers;     // Headers object
response.url;         // final URL (after redirects)
response.redirected;  // true if redirected

// Reading the body (can only be read ONCE)
await response.json();        // parse as JSON
await response.text();        // parse as string
await response.blob();        // parse as Blob (for files/images)
await response.arrayBuffer(); // parse as ArrayBuffer (binary)
await response.formData();    // parse as FormData

// Clone to read body multiple times
const clone = response.clone();
const text = await clone.text();
const json = await response.json();
```

### Complete Request Options

```js
const response = await fetch(url, {
  method: "POST",      // "GET" (default), "POST", "PUT", "PATCH", "DELETE"

  headers: {
    "Content-Type": "application/json",
    "Authorization": `Bearer ${token}`,
    "Accept": "application/json",
    "X-Request-ID": crypto.randomUUID(),
  },

  body: JSON.stringify(data), // for POST/PUT/PATCH — must match Content-Type

  // Caching
  cache: "no-cache",  // "default", "no-cache", "no-store", "reload", "force-cache"

  // Credentials (cookies)
  credentials: "include",  // "omit" (default), "same-origin", "include"

  // Redirect handling
  redirect: "follow",  // "follow" (default), "error", "manual"

  // Abort signal (for cancellation)
  signal: abortController.signal,

  // Mode (CORS)
  mode: "cors", // "cors" (default), "no-cors", "same-origin"
});
```

### Building a Reusable API Client

```js
class ApiClient {
  #baseUrl;
  #defaultHeaders;
  #getToken;

  constructor({ baseUrl, getToken }) {
    this.#baseUrl = baseUrl;
    this.#getToken = getToken;
    this.#defaultHeaders = {
      "Content-Type": "application/json",
      "Accept": "application/json",
    };
  }

  async #buildHeaders(extraHeaders = {}) {
    const headers = { ...this.#defaultHeaders, ...extraHeaders };
    const token = await this.#getToken?.();
    if (token) headers["Authorization"] = `Bearer ${token}`;
    return headers;
  }

  async #request(method, path, { body, headers = {}, signal } = {}) {
    const url = `${this.#baseUrl}${path}`;
    const requestHeaders = await this.#buildHeaders(headers);

    const options = {
      method,
      headers: requestHeaders,
      signal,
    };

    if (body !== undefined) {
      options.body = JSON.stringify(body);
    }

    let response;
    try {
      response = await fetch(url, options);
    } catch (err) {
      if (err.name === "AbortError") throw new Error("Request cancelled");
      throw new Error(`Network error: ${err.message}`);
    }

    // Handle auth errors globally
    if (response.status === 401) {
      this.#handleAuthError();
      throw new UnauthorizedError();
    }

    // Parse response
    const contentType = response.headers.get("Content-Type") ?? "";
    const data = contentType.includes("application/json")
      ? await response.json()
      : await response.text();

    if (!response.ok) {
      const message = data?.message ?? data ?? `HTTP ${response.status}`;
      const error = new ApiError(message, response.status, data?.code);
      throw error;
    }

    return data;
  }

  #handleAuthError() {
    // Emit event, redirect to login, etc.
    window.dispatchEvent(new CustomEvent("auth:expired"));
  }

  get(path, options) { return this.#request("GET", path, options); }
  post(path, body, options) { return this.#request("POST", path, { ...options, body }); }
  put(path, body, options) { return this.#request("PUT", path, { ...options, body }); }
  patch(path, body, options) { return this.#request("PATCH", path, { ...options, body }); }
  delete(path, options) { return this.#request("DELETE", path, options); }
}

// Usage
const api = new ApiClient({
  baseUrl: "https://api.example.com",
  getToken: () => authService.getAccessToken(),
});

const user = await api.get("/users/1");
await api.post("/users", { name: "Alice", email: "alice@example.com" });
await api.patch(`/users/${id}`, { name: "Alice Chen" });
await api.delete(`/users/${id}`);
```

### Request Cancellation with AbortController

```js
// Cancel a fetch when the user navigates away or changes search term
let currentController = null;

async function searchProducts(query) {
  // Cancel previous request if still in flight
  currentController?.abort();
  currentController = new AbortController();

  try {
    const products = await api.get(
      `/products?q=${encodeURIComponent(query)}`,
      { signal: currentController.signal }
    );
    renderResults(products);
  } catch (err) {
    if (err.name === "AbortError" || err.message === "Request cancelled") {
      return; // Ignore — user already typed something new
    }
    showError(err.message);
  }
}

// Clean up on component unmount (React)
useEffect(() => {
  const controller = new AbortController();
  fetchData({ signal: controller.signal });
  return () => controller.abort(); // cleanup
}, []);
```

### Pagination Patterns

```js
// Offset-based pagination
async function fetchAllPages(endpoint, pageSize = 100) {
  const allItems = [];
  let page = 1;
  let hasMore = true;

  while (hasMore) {
    const { items, total } = await api.get(
      `${endpoint}?page=${page}&limit=${pageSize}`
    );
    allItems.push(...items);
    hasMore = allItems.length < total;
    page++;
  }

  return allItems;
}

// Cursor-based pagination (more efficient for large datasets)
async function* fetchPagesByCursor(endpoint, pageSize = 50) {
  let cursor = null;

  while (true) {
    const params = new URLSearchParams({ limit: pageSize });
    if (cursor) params.set("cursor", cursor);

    const { items, nextCursor } = await api.get(`${endpoint}?${params}`);
    yield items;

    if (!nextCursor) break;
    cursor = nextCursor;
  }
}

// Usage — process each page as it arrives (memory-efficient)
for await (const page of fetchPagesByCursor("/orders")) {
  await processBatch(page);
}
```

### Real-World: Auth API Module

```js
const authApi = {
  async login(email, password) {
    return api.post("/auth/login", { email, password });
  },

  async logout() {
    return api.post("/auth/logout");
  },

  async refreshToken(refreshToken) {
    // Don't use the main api client here (would create infinite loop on 401)
    const response = await fetch(`${BASE_URL}/auth/refresh`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ refreshToken }),
    });
    if (!response.ok) throw new Error("Token refresh failed");
    return response.json();
  },

  async register(userData) {
    return api.post("/auth/register", userData);
  },

  async verifyEmail(token) {
    return api.post("/auth/verify-email", { token });
  },

  async requestPasswordReset(email) {
    return api.post("/auth/forgot-password", { email });
  },

  async resetPassword(token, newPassword) {
    return api.post("/auth/reset-password", { token, newPassword });
  },
};
```

---

## 10. Error Handling

### Types of Errors

```js
// 1. Synchronous errors — thrown immediately
throw new Error("Something went wrong");
throw new TypeError("Expected a string");
throw new RangeError("Value out of range");

// 2. Promise rejections — async errors
const promise = Promise.reject(new Error("Async failure"));
fetch("bad-url").catch(err => console.error(err)); // network error

// 3. Unhandled promise rejections — the worst kind (crash the process in Node.js)
fetch("bad-url"); // Promise rejected but no .catch() — bad!
```

### try/catch/finally

```js
async function createUser(data) {
  try {
    // Validate — sync throw
    if (!data.email) throw new ValidationError([{ field: "email", message: "Required" }]);

    // DB operation — async, might reject
    const existing = await db.users.findByEmail(data.email);
    if (existing) throw new ConflictError("Email already registered");

    const user = await db.users.create(data);
    await emailService.sendWelcome(user.email);

    return user;

  } catch (err) {
    // Handle specific error types differently
    if (err instanceof ValidationError) {
      throw err; // re-throw to let caller handle
    }
    if (err instanceof ConflictError) {
      throw err;
    }
    // Unknown error — log it (it might be a bug)
    logger.error("Unexpected error in createUser:", { err, data: omit(data, "password") });
    throw new AppError("Failed to create user", 500);

  } finally {
    // Always runs — good for cleanup
    // Release DB connection, clear temp files, stop loading state
  }
}
```

### Global Error Handling

```js
// Browser: catch unhandled promise rejections
window.addEventListener("unhandledrejection", (event) => {
  console.error("Unhandled promise rejection:", event.reason);
  event.preventDefault(); // prevent default browser behavior (console error)
  reportErrorToService(event.reason);
});

// Browser: catch uncaught synchronous errors
window.addEventListener("error", (event) => {
  console.error("Uncaught error:", event.error);
  reportErrorToService(event.error);
});

// Node.js
process.on("unhandledRejection", (reason, promise) => {
  console.error("Unhandled Rejection at:", promise, "reason:", reason);
  // In production: log, then gracefully shut down
  process.exit(1);
});

process.on("uncaughtException", (error) => {
  console.error("Uncaught Exception:", error);
  process.exit(1); // Always exit after uncaught exception
});
```

### Error Hierarchy for a Real App

```js
// Base error
class AppError extends Error {
  constructor(message, statusCode = 500, code = "INTERNAL_ERROR", details = null) {
    super(message);
    this.name = this.constructor.name;
    this.statusCode = statusCode;
    this.code = code;
    this.details = details;
    this.isOperational = true;
    this.timestamp = new Date().toISOString();
    Error.captureStackTrace(this, this.constructor);
  }

  toJSON() {
    return {
      error: {
        code: this.code,
        message: this.message,
        details: this.details,
        timestamp: this.timestamp,
      },
    };
  }
}

class ValidationError extends AppError {
  constructor(fields) {
    super("Validation failed", 400, "VALIDATION_ERROR", fields);
  }
}

class AuthError extends AppError {
  constructor(msg = "Authentication required") {
    super(msg, 401, "UNAUTHORIZED");
  }
}

class ForbiddenError extends AppError {
  constructor(action = "perform this action") {
    super(`You are not allowed to ${action}`, 403, "FORBIDDEN");
  }
}

class NotFoundError extends AppError {
  constructor(resource = "Resource") {
    super(`${resource} not found`, 404, "NOT_FOUND");
  }
}

class ConflictError extends AppError {
  constructor(msg = "Resource already exists") {
    super(msg, 409, "CONFLICT");
  }
}

class RateLimitError extends AppError {
  constructor(retryAfterMs) {
    super("Too many requests", 429, "RATE_LIMITED", { retryAfterMs });
  }
}
```

### Error Handling Middleware (Express-style)

```js
// Centralized error handler — all errors funnel here
function globalErrorHandler(err, req, res, next) {
  // Operational errors — expected, safe to tell the client about
  if (err instanceof AppError && err.isOperational) {
    return res.status(err.statusCode).json(err.toJSON());
  }

  // Programming errors — unexpected bugs
  // Log with full detail for developers, hide from client
  logger.error({
    message: err.message,
    stack: err.stack,
    url: req.url,
    method: req.method,
    body: req.body,
    userId: req.user?.id,
  });

  return res.status(500).json({
    error: {
      code: "INTERNAL_ERROR",
      message: "An unexpected error occurred",
    },
  });
}
```

### The Result Pattern (Alternative to throw)

Inspired by Rust and Go — return `[error, data]` instead of throwing:

```js
async function safeAsync(promise) {
  try {
    return [null, await promise];
  } catch (error) {
    return [error, null];
  }
}

// Usage — no try/catch needed at call site
async function handleSignup(req) {
  const [validationErr, cleanData] = await safeAsync(validateSignupData(req.body));
  if (validationErr) return res.status(400).json({ error: validationErr.message });

  const [createErr, user] = await safeAsync(userService.create(cleanData));
  if (createErr) return res.status(500).json({ error: "Failed to create user" });

  const [emailErr] = await safeAsync(emailService.sendWelcome(user.email));
  if (emailErr) logger.warn("Welcome email failed:", emailErr); // non-fatal

  return res.status(201).json({ user: user.toJSON() });
}
```

### Retry Logic with Error Handling

```js
async function withRetry(fn, options = {}) {
  const {
    retries = 3,
    delay = 1000,
    backoff = 2,
    retryOn = (err) => err.statusCode >= 500 || err.code === "NETWORK_ERROR",
  } = options;

  let lastError;

  for (let attempt = 1; attempt <= retries; attempt++) {
    try {
      return await fn();
    } catch (err) {
      lastError = err;

      if (attempt === retries || !retryOn(err)) {
        break; // don't retry — last attempt or non-retryable error
      }

      const waitMs = delay * backoff ** (attempt - 1);
      console.warn(`Attempt ${attempt} failed. Retrying in ${waitMs}ms...`);
      await sleep(waitMs);
    }
  }

  throw lastError;
}

// Usage
const user = await withRetry(
  () => api.get("/users/1"),
  {
    retries: 3,
    delay: 500,
    retryOn: (err) => [429, 500, 502, 503, 504].includes(err.statusCode),
  }
);
```

### Circuit Breaker Pattern

Stops calling a failing service for a period to prevent cascade failures:

```js
class CircuitBreaker {
  #state = "closed"; // "closed" = working, "open" = failing, "half-open" = testing
  #failureCount = 0;
  #lastFailureTime = null;
  #threshold;
  #timeout;

  constructor(threshold = 5, timeoutMs = 30_000) {
    this.#threshold = threshold;
    this.#timeout = timeoutMs;
  }

  async call(fn) {
    if (this.#state === "open") {
      const elapsed = Date.now() - this.#lastFailureTime;
      if (elapsed < this.#timeout) {
        throw new Error("Circuit breaker is OPEN — service unavailable");
      }
      this.#state = "half-open";
    }

    try {
      const result = await fn();
      this.#onSuccess();
      return result;
    } catch (err) {
      this.#onFailure();
      throw err;
    }
  }

  #onSuccess() {
    this.#failureCount = 0;
    this.#state = "closed";
  }

  #onFailure() {
    this.#failureCount++;
    this.#lastFailureTime = Date.now();
    if (this.#failureCount >= this.#threshold) {
      this.#state = "open";
      console.error(`Circuit breaker OPENED after ${this.#threshold} failures`);
    }
  }

  get state() { return this.#state; }
}

const emailBreaker = new CircuitBreaker(5, 30_000);

async function sendEmail(to, content) {
  return emailBreaker.call(() => emailProvider.send(to, content));
}
```

---

## Summary — The Full Async Picture

```
Event Loop:
  Call Stack → Microtask Queue (Promises) → Macrotask Queue (setTimeout/I/O)
  Microtasks always drain completely before next macrotask

Promises:
  new Promise((resolve, reject) => {})
  States: pending → fulfilled | rejected (immutable once settled)
  .then(onFulfill) → new Promise  .catch(onReject)  .finally(always)
  Chaining: return value/Promise from .then() passes to next

Promise combinators:
  all([...])        — parallel, fail-fast
  allSettled([...]) — parallel, get all results
  race([...])       — first to settle wins
  any([...])        — first to SUCCEED wins

async/await:
  async fn always returns a Promise
  await unwraps a Promise, pauses function (not thread)
  Sequential: await one, then another (adds up time)
  Parallel:   await Promise.all([a(), b(), c()])
  Loops:      for...of with await; never forEach with async

Error handling:
  try/catch/finally for all async code
  Global: unhandledrejection, uncaughtException
  Error hierarchy: AppError → specific subclasses
  Retry: exponential backoff for transient failures
  Circuit breaker: stop hammering failing services

Fetch:
  fetch(url, { method, headers, body, signal })
  response.ok, .status, .json(), .text()
  Always check response.ok before parsing body
  AbortController for cancellation
  Build a reusable ApiClient class
```

---

_This is the final part of the 7-part JavaScript Deep Documentation series._

```
Part 1: Foundations        — types, variables, operators, strings, conditionals
Part 2: Loops & Data       — loops, arrays, objects, logical operators
Part 3: Functions          — closures, scope, callbacks, JSON, dates, timers
Part 4: Modern JS (ES6+)   — destructuring, spread, arrow fns, array helpers, Map/Set
Part 5: DOM & Events       — DOM manipulation, styles, events, keyboard, delegation
Part 6: OOP                — this, prototypes, classes, encapsulation, inheritance, polymorphism
Part 7: Async & APIs       — event loop, Promises, async/await, Fetch, error handling
```