> Most real programs talk to the outside world — fetching data from a server, reading a file, waiting for a timer. These operations take time. Asynchronous JavaScript is how you write code that does useful work while waiting. This is the foundation of every web app, every API call, every real-time feature.

---

## Table of Contents

1. [The Problem Async Solves](#1-the-problem-async-solves)
2. [Callbacks — The Old Way](#2-callbacks--the-old-way)
3. [Promises — In Depth](#3-promises--in-depth)
4. [async / await](#4-async--await)
5. [Error Handling in Async Code](#5-error-handling-in-async-code)
6. [Parallel Async Operations](#6-parallel-async-operations)
7. [fetch and the Network](#7-fetch-and-the-network)
8. [Aborting Requests](#8-aborting-requests)
9. [Building an API Client](#9-building-an-api-client)
10. [Common Async Patterns](#10-common-async-patterns)

---

## 1. The Problem Async Solves

JavaScript is single-threaded. If you write blocking code that waits for something, the entire program freezes — no user input, no rendering, nothing.

```javascript
// BLOCKING — imagine this takes 3 seconds
const data = readFileFromDisk("data.json"); // blocks EVERYTHING for 3 seconds
render(data);

// Meanwhile: user can't click buttons, page can't animate, nothing works
// In the browser: the page completely freezes
// In Node.js: no other requests can be served
```

The solution: instead of waiting, _describe what to do when the result arrives_, and continue with other work.

```javascript
// NON-BLOCKING — register what to do, keep going
readFileFromDisk("data.json", (data) => {
  render(data); // called when file is ready
});
// Code continues here immediately, file is loading in the background
```

---

## 2. Callbacks — The Old Way

A callback is a function you pass to another function, to be called when an async operation completes.

```javascript
// Node.js style: error-first callbacks
fs.readFile("data.json", "utf8", (err, data) => {
  if (err) {
    console.error("Read failed:", err);
    return;
  }
  console.log("Got:", data);
});

// Browser: addEventListener is a callback
button.addEventListener("click", (event) => {
  handleClick(event);
});
```

### Callback Hell — Why Callbacks Fail at Scale

```javascript
// Each async operation needs another callback — they nest deeply
login(credentials, (err, user) => {
  if (err) return handleError(err);

  fetchProfile(user.id, (err, profile) => {
    if (err) return handleError(err);

    fetchPosts(profile.id, (err, posts) => {
      if (err) return handleError(err);

      fetchComments(posts[0].id, (err, comments) => {
        if (err) return handleError(err);
        render({ user, profile, posts, comments });
        // This is "callback hell" — impossible to maintain
      });
    });
  });
});
```

Promises and async/await solve this.

---

## 3. Promises — In Depth

A Promise is an object representing an asynchronous operation that will eventually produce a value (or fail).

```
Promise states:
  pending    — operation in progress
  fulfilled  — completed successfully (has a value)
  rejected   — failed (has a reason/error)

Once settled (fulfilled or rejected), a Promise cannot change state.
```

### Creating Promises

```javascript
// Wrapping a callback-based API in a Promise
function readFile(path) {
  return new Promise((resolve, reject) => {
    fs.readFile(path, "utf8", (err, data) => {
      if (err)
        reject(err); // fulfilled with error
      else resolve(data); // fulfilled with data
    });
  });
}

// Immediately resolved/rejected
const resolved = Promise.resolve(42);
const rejected = Promise.reject(new Error("instant fail"));
```

### Consuming Promises

```javascript
// .then() — handle success, returns a NEW Promise
// .catch() — handle failure, returns a NEW Promise
// .finally() — always runs, doesn't affect the chain

readFile("data.json")
  .then((data) => {
    return JSON.parse(data); // return value becomes next .then()'s argument
  })
  .then((obj) => {
    return fetchDetails(obj.id); // returning a Promise chains into it
  })
  .then((details) => {
    render(details);
  })
  .catch((err) => {
    // Catches errors from ANY step above
    console.error("Pipeline failed:", err);
  })
  .finally(() => {
    hideSpinner(); // always runs, chain value passes through unchanged
  });
```

### Promise Chaining Rules

```javascript
// .then() can return:
// 1. A value — next .then() receives it
Promise.resolve(1)
  .then((n) => n + 1) // returns 2
  .then((n) => n * 2) // receives 2, returns 4
  .then(console.log); // logs 4

// 2. A Promise — chain waits for it to resolve
Promise.resolve("userId")
  .then((id) => fetchUser(id)) // fetchUser returns a Promise
  .then((user) => console.log(user)); // receives the user when fetchUser resolves

// 3. Throw — error propagates to next .catch()
Promise.resolve("data")
  .then((data) => {
    throw new Error("bad data");
  })
  .then(() => {
    /* skipped */
  })
  .catch((err) => console.error(err.message)); // "bad data"
```

---

## 4. `async` / `await`

`async/await` is syntax sugar over Promises. It makes async code read like synchronous code.

```javascript
// An async function always returns a Promise
async function greet() {
  return "Hello"; // same as: return Promise.resolve("Hello")
}
greet().then(console.log); // "Hello"

// await — pause until a Promise resolves, then unwrap the value
// Only valid INSIDE an async function
async function loadDashboard(userId) {
  const user = await fetchUser(userId); // waits for fetchUser
  const posts = await fetchPosts(user.id); // waits for fetchPosts
  const metrics = await fetchMetrics(user.id); // waits for fetchMetrics

  return { user, posts, metrics };
}
```

### Top-Level `await` (ES2022, in modules)

```javascript
// In ES modules, await can be used at the top level
// config.js
const config = await fetch("/api/config").then((r) => r.json());
export { config };
// This module won't finish loading until the fetch resolves
```

### Sequential vs Parallel

```javascript
// SEQUENTIAL — each waits for the previous (slow if independent)
async function loadSequential(userId) {
  const user = await fetchUser(userId); // 200ms
  const posts = await fetchPosts(userId); // 300ms — starts AFTER user
  const stats = await fetchStats(userId); // 100ms — starts AFTER posts
  // Total: ~600ms
}

// PARALLEL — all start at the same time (fast)
async function loadParallel(userId) {
  const [user, posts, stats] = await Promise.all([
    fetchUser(userId), // 200ms
    fetchPosts(userId), // 300ms — starts SAME TIME
    fetchStats(userId), // 100ms — starts SAME TIME
  ]);
  // Total: ~300ms (limited by slowest)
}

// Rule: if operations are INDEPENDENT, run them in parallel
// If B depends on A's result, they must be sequential
async function dependent(userId) {
  const user = await fetchUser(userId); // must come first
  const details = await fetchDetails(user.profileId); // needs user.profileId
  // These two MUST be sequential
}
```

---

## 5. Error Handling in Async Code

```javascript
// try/catch works with async/await
async function loadUser(id) {
  try {
    const response = await fetch(`/api/users/${id}`);
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }
    return await response.json();
  } catch (err) {
    console.error("Failed to load user:", err.message);
    return null; // return a safe default
  }
}

// Errors propagate upward through await chains
async function step3() {
  throw new Error("step3 failed");
}
async function step2() {
  return await step3();
}
async function step1() {
  return await step2();
}

async function main() {
  try {
    await step1();
  } catch (err) {
    // Catches step3's error — propagated through step2 and step1
    console.error(err.message); // "step3 failed"
  }
}

// Async functions that throw become rejected Promises
const rejected = loadUser("invalid");
rejected.catch((err) => console.error(err));
// OR use await in try/catch
```

### Handling Errors Without try/catch

```javascript
// .catch() on an async function call
async function loadData() {
  throw new Error("failed");
}

loadData()
  .then((data) => render(data))
  .catch((err) => showError(err)); // catches thrown errors too

// Safe wrapper — never throws, always returns { data } or { error }
async function safeLoad(fn) {
  try {
    const data = await fn();
    return { data, error: null };
  } catch (error) {
    return { data: null, error };
  }
}

const { data, error } = await safeLoad(() => fetchUser(id));
if (error) showError(error);
else render(data);
```

---

## 6. Parallel Async Operations

```javascript
// Promise.all — all must succeed, fails fast on first rejection
const [user, posts, config] = await Promise.all([
  fetchUser(id),
  fetchPosts(id),
  fetchConfig(),
]);
// If ANY rejects → entire Promise.all rejects immediately

// Promise.allSettled — wait for ALL, regardless of success/failure
const results = await Promise.allSettled([
  fetchUser(id),
  fetchPosts(id),
  fetchConfig(),
]);

results.forEach((result) => {
  if (result.status === "fulfilled") {
    console.log("Success:", result.value);
  } else {
    console.error("Failed:", result.reason);
  }
});
// Never rejects — always resolves with array of { status, value/reason }

// Promise.race — settles as soon as FIRST promise settles
// Useful for timeouts
function withTimeout(promise, ms) {
  const timeout = new Promise((_, reject) =>
    setTimeout(() => reject(new Error(`Timed out after ${ms}ms`)), ms),
  );
  return Promise.race([promise, timeout]);
}

const user = await withTimeout(fetchUser(id), 5000);
// Throws if fetchUser takes more than 5 seconds

// Promise.any — resolves with FIRST success, rejects only if ALL fail
// Useful for trying multiple sources
const data = await Promise.any([
  fetchFromPrimaryServer(),
  fetchFromBackupServer(),
  fetchFromCache(),
]);
// Returns first one that succeeds; throws AggregateError if all fail
```

---

## 7. `fetch` and the Network

`fetch` is the browser's built-in HTTP client. It returns a Promise.

```javascript
// Basic GET request
const response = await fetch("https://api.cwwkcc.lk/students");

// IMPORTANT: fetch only rejects on NETWORK errors (no internet, server unreachable)
// HTTP error status codes (4xx, 5xx) do NOT cause rejection — check response.ok
if (!response.ok) {
  throw new Error(`HTTP error: ${response.status} ${response.statusText}`);
}

const students = await response.json(); // parse JSON body

// Other response body methods:
await response.text(); // plain text
await response.blob(); // binary data (images, files)
await response.arrayBuffer(); // raw binary
await response.formData(); // form data

// Response properties
response.status; // 200, 201, 404, 500...
response.statusText; // "OK", "Not Found"...
response.ok; // true if status 200-299
response.url; // final URL (after redirects)
response.headers.get("content-type"); // read a response header
```

### POST, PUT, PATCH, DELETE

```javascript
// POST with JSON body
const response = await fetch("/api/students", {
  method: "POST",
  headers: {
    "Content-Type": "application/json",
    Authorization: `Bearer ${accessToken}`,
  },
  body: JSON.stringify({
    name: "Ashan",
    grade: 11,
    email: "ashan@school.lk",
  }),
});

// PATCH — partial update
await fetch(`/api/students/${id}`, {
  method: "PATCH",
  headers: { "Content-Type": "application/json" },
  body: JSON.stringify({ score: 91 }),
});

// DELETE
await fetch(`/api/students/${id}`, {
  method: "DELETE",
  headers: { Authorization: `Bearer ${token}` },
});

// File upload with FormData
const formData = new FormData();
formData.append("file", fileInput.files[0]);
formData.append("studentId", "u123");

await fetch("/api/upload", {
  method: "POST",
  // Don't set Content-Type — browser sets it with the boundary automatically
  body: formData,
});
```

---

## 8. Aborting Requests

```javascript
// AbortController lets you cancel a fetch
const controller = new AbortController();

// Cancel after 10 seconds
const timeoutId = setTimeout(() => controller.abort(), 10_000);

try {
  const response = await fetch("/api/slow-endpoint", {
    signal: controller.signal,
  });
  clearTimeout(timeoutId); // cancel the timeout if request succeeded
  return await response.json();
} catch (err) {
  if (err.name === "AbortError") {
    console.log("Request was cancelled");
    return null;
  }
  throw err; // rethrow other errors
}

// Cancel when user navigates away (React pattern)
useEffect(() => {
  const controller = new AbortController();

  async function load() {
    try {
      const data = await fetchUser(id, { signal: controller.signal });
      setUser(data);
    } catch (err) {
      if (err.name !== "AbortError") setError(err);
    }
  }

  load();

  return () => controller.abort(); // cancel on unmount or id change
}, [id]);
```

---

## 9. Building an API Client

A reusable API client centralises request configuration and error handling.

```javascript
class ApiClient {
  #baseUrl;
  #getToken;

  constructor(baseUrl, getToken) {
    this.#baseUrl = baseUrl;
    this.#getToken = getToken;
  }

  async #request(method, endpoint, options = {}) {
    const { body, params, signal } = options;

    const url = new URL(`${this.#baseUrl}${endpoint}`);
    if (params) {
      Object.entries(params).forEach(([k, v]) => {
        if (v !== undefined && v !== null) url.searchParams.set(k, v);
      });
    }

    const token = this.#getToken();
    const headers = {
      "Content-Type": "application/json",
      ...(token ? { Authorization: `Bearer ${token}` } : {}),
    };

    const response = await fetch(url.toString(), {
      method,
      headers,
      body: body ? JSON.stringify(body) : undefined,
      signal,
    });

    if (response.status === 401) {
      this.#onUnauthorised();
      throw new Error("Unauthorised");
    }

    if (!response.ok) {
      const error = await response
        .json()
        .catch(() => ({ message: response.statusText }));
      throw Object.assign(new Error(error.message ?? "Request failed"), {
        status: response.status,
        details: error,
      });
    }

    if (response.status === 204) return null; // No Content
    return response.json();
  }

  #onUnauthorised() {
    localStorage.removeItem("token");
    window.location.href = "/login";
  }

  get(endpoint, params, signal) {
    return this.#request("GET", endpoint, { params, signal });
  }
  post(endpoint, body, signal) {
    return this.#request("POST", endpoint, { body, signal });
  }
  put(endpoint, body, signal) {
    return this.#request("PUT", endpoint, { body, signal });
  }
  patch(endpoint, body, signal) {
    return this.#request("PATCH", endpoint, { body, signal });
  }
  delete(endpoint, signal) {
    return this.#request("DELETE", endpoint, { signal });
  }
}

// Usage
const api = new ApiClient("https://api.cwwkcc.lk", () =>
  localStorage.getItem("token"),
);

const students = await api.get("/students", { grade: 11, limit: 20 });
const newStudent = await api.post("/students", { name: "Ashan", grade: 11 });
await api.delete(`/students/${id}`);
```

---

## 10. Common Async Patterns

### Sequential with loop

```javascript
// Process items one at a time (order matters, or API has rate limits)
async function processSequentially(items) {
  const results = [];
  for (const item of items) {
    const result = await processItem(item);
    results.push(result);
  }
  return results;
}

// Common mistake: map with async doesn't await each
const results = await items.map(async (item) => processItem(item));
// returns array of Promises, not values!
// Fix:
const results = await Promise.all(items.map((item) => processItem(item)));
```

### Polling

```javascript
async function pollUntilComplete(taskId, interval = 2000, maxAttempts = 30) {
  for (let attempt = 0; attempt < maxAttempts; attempt++) {
    const status = await checkTaskStatus(taskId);

    if (status === "complete") return await getTaskResult(taskId);
    if (status === "failed") throw new Error(`Task ${taskId} failed`);

    await new Promise((resolve) => setTimeout(resolve, interval));
  }
  throw new Error(`Task ${taskId} did not complete in time`);
}
```

### Retry with exponential backoff

```javascript
async function withRetry(fn, maxAttempts = 3) {
  for (let attempt = 1; attempt <= maxAttempts; attempt++) {
    try {
      return await fn();
    } catch (err) {
      if (attempt === maxAttempts) throw err;

      const delay = Math.min(1000 * 2 ** attempt, 30_000); // 2s, 4s, 8s... max 30s
      console.warn(`Attempt ${attempt} failed, retrying in ${delay}ms...`);
      await new Promise((resolve) => setTimeout(resolve, delay));
    }
  }
}

const data = await withRetry(() => fetchCriticalData());
```

### Concurrency limit

```javascript
// Run at most N tasks at a time (avoid overwhelming an API)
async function withConcurrencyLimit(tasks, limit) {
  const results = [];
  const executing = new Set();

  for (const task of tasks) {
    const promise = task().then((result) => {
      executing.delete(promise);
      return result;
    });

    executing.add(promise);
    results.push(promise);

    if (executing.size >= limit) {
      await Promise.race(executing); // wait for one to finish before starting next
    }
  }

  return Promise.all(results);
}

const tasks = studentIds.map((id) => () => fetchStudentDetails(id));
const details = await withConcurrencyLimit(tasks, 5); // max 5 at once
```

---

## Summary

```
The problem:
  JS is single-threaded. Blocking operations freeze everything.
  Async code describes what to do when results arrive, then continues.

Promises:
  Three states: pending → fulfilled or rejected (final)
  .then(onFulfilled) — chain on success, returns new Promise
  .catch(onRejected) — handle errors from entire chain above
  .finally(fn)       — always runs, doesn't change chain value

async/await:
  async function — always returns a Promise
  await          — pause until Promise resolves, unwrap value
  try/catch      — error handling for async code

Sequential vs Parallel:
  Sequential: for...of with await (when B depends on A)
  Parallel:   Promise.all([a, b, c]) (when independent — much faster)

Promise utilities:
  Promise.all         — all succeed, or fail fast on first rejection
  Promise.allSettled  — wait for all, never rejects, collect all results
  Promise.race        — first settled wins (timeouts)
  Promise.any         — first fulfilled wins (fallbacks)

fetch:
  GET:    fetch(url)
  POST:   fetch(url, { method:"POST", headers, body: JSON.stringify(data) })
  Always check response.ok — HTTP errors don't reject fetch!
  AbortController — cancel requests on timeout or unmount

Patterns:
  withTimeout     — race against a timeout Promise
  withRetry       — exponential backoff on failure
  pollUntilDone   — check status repeatedly
  concurrencyLimit — throttle parallel work
```

---

_Next: [07 — The JavaScript Engine, Runtime & Memory](./07%20-%20The%20JavaScript%20Engine%2C%20Runtime%20%26%20Memory.md)_
