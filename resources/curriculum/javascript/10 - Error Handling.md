> Most tutorials teach you the happy path. Production code lives on the unhappy path. How you handle errors is the difference between an app that fails gracefully and one that crashes silently or leaks sensitive information to the user.

---

## Table of Contents

1. [The Error Object](#1-the-error-object)
2. [Built-in Error Types](#2-built-in-error-types)
3. [try / catch / finally](#3-try--catch--finally)
4. [Catching Specific Error Types](#4-catching-specific-error-types)
5. [Custom Error Classes](#5-custom-error-classes)
6. [Error Handling in Async Code](#6-error-handling-in-async-code)
7. [Promises: .catch() and Unhandled Rejections](#7-promises-catch-and-unhandled-rejections)
8. [The Problem with Swallowing Errors](#8-the-problem-with-swallowing-errors)
9. [Global Error Handlers](#9-global-error-handlers)
10. [Result Pattern — Errors as Values](#10-result-pattern--errors-as-values)
11. [Error Handling in Node.js / Express](#11-error-handling-in-nodejs--express)

---

## 1. The Error Object

When JavaScript throws an error, it creates an `Error` object. The two most important properties:

```javascript
const err = new Error("Something went wrong");

err.message; // "Something went wrong" — what you passed to the constructor
err.stack; // multiline string with the call stack at the point of creation
err.name; // "Error" — the type name (overridden in subclasses)
```

The `stack` property is the most useful for debugging:

```
Error: Something went wrong
    at createOrder (order.js:24:11)
    at handleCheckout (checkout.js:68:5)
    at async POST /checkout (routes.js:15:3)

Each line shows: function name, file, line number, column number
```

---

## 2. Built-in Error Types

JavaScript has several built-in error subclasses, each for a specific category of problem:

```javascript
// Error — base class, general purpose
throw new Error("something failed");

// TypeError — wrong type of value
null.property;             // TypeError: Cannot read properties of null
undefined();               // TypeError: undefined is not a function
42.toUpperCase();          // TypeError: toUpperCase is not a function

// ReferenceError — variable doesn't exist
console.log(undeclaredVar); // ReferenceError: undeclaredVar is not defined

// RangeError — value outside valid range
new Array(-1);              // RangeError: Invalid array length
(1234.5678).toFixed(200);   // RangeError: toFixed() digits out of range
function recurse() { recurse(); }
recurse();                  // RangeError: Maximum call stack size exceeded

// SyntaxError — code can't be parsed (usually thrown at parse time, not runtime)
JSON.parse("{ invalid }");  // SyntaxError: Unexpected token i in JSON

// URIError — malformed URI
decodeURIComponent("%");    // URIError: URI malformed

// EvalError — (rarely seen in modern code)
// AggregateError — multiple errors (Promise.any, Promise.allSettled)
Promise.any([
  Promise.reject(new Error("first")),
  Promise.reject(new Error("second")),
]).catch(err => {
  err instanceof AggregateError; // true
  err.errors;  // [Error: "first", Error: "second"]
});
```

---

## 3. `try` / `catch` / `finally`

```javascript
try {
  // Code that might throw
  const data = JSON.parse(userInput);
  processData(data);
} catch (err) {
  // Runs when an error is thrown in the try block
  // `err` is the thrown value (usually an Error object)
  console.error("Failed to process:", err.message);
} finally {
  // ALWAYS runs — whether or not an error was thrown
  // Use for cleanup: close files, release locks, reset state
  releaseResourceLock();
}
```

### `finally` always runs

```javascript
function readFile(path) {
  const file = openFile(path); // acquire resource

  try {
    return file.read(); // might throw
  } finally {
    file.close(); // ALWAYS closes, even if read() throws
    // Even if there's a `return` in the try block, finally runs first
  }
}

// Even with return in try:
function example() {
  try {
    return "from try";
  } finally {
    console.log("finally runs"); // logs BEFORE the function returns
  }
}
// Console: "finally runs"
// Returns: "from try"
```

### Rethrowing

Sometimes you want to catch an error, do something (log it), and rethrow:

```javascript
async function fetchUser(id) {
  try {
    const response = await fetch(`/api/users/${id}`);
    return response.json();
  } catch (err) {
    // Log for debugging, then rethrow so the caller can decide how to handle it
    logger.error("fetchUser failed", { userId: id, error: err.message });
    throw err; // rethrow — caller still gets the error
  }
}
```

---

## 4. Catching Specific Error Types

`catch` catches everything. Often you want to handle different errors differently:

```javascript
async function loadConfig(path) {
  try {
    const raw = await fs.readFile(path, "utf8");
    return JSON.parse(raw);
  } catch (err) {
    if (err instanceof SyntaxError) {
      // JSON was malformed — this is a user/config error
      throw new Error(
        `Config file ${path} contains invalid JSON: ${err.message}`,
      );
    }

    if (err.code === "ENOENT") {
      // File doesn't exist — return a default config
      return getDefaultConfig();
    }

    // Unknown error — rethrow (don't hide it)
    throw err;
  }
}
```

```javascript
// Discriminating on error properties (useful for API/network errors)
async function apiCall(url) {
  try {
    const res = await fetch(url);

    if (!res.ok) {
      // HTTP errors don't throw — you have to check res.ok
      const error = new Error(`HTTP ${res.status}: ${res.statusText}`);
      error.status = res.status;
      error.url = url;
      throw error;
    }

    return res.json();
  } catch (err) {
    if (err.name === "TypeError" && err.message.includes("fetch")) {
      // Network failure — no internet or server down
      throw new Error("Network error — check your connection");
    }

    if (err.status === 401) {
      // Unauthorised — clear session and redirect to login
      clearSession();
      redirectTo("/login");
      return;
    }

    if (err.status === 429) {
      // Rate limited — wait and retry
      await sleep(5000);
      return apiCall(url); // retry once
    }

    throw err; // other errors — rethrow
  }
}
```

---

## 5. Custom Error Classes

Create custom error classes to make errors identifiable, structured, and carrying extra information.

```javascript
// Base application error
class AppError extends Error {
  constructor(message, options = {}) {
    super(message);
    this.name = this.constructor.name; // "ValidationError", not "Error"

    // Capture stack trace, excluding this constructor from it
    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, this.constructor);
    }
  }
}

// HTTP-oriented errors (useful in API layer)
class HttpError extends AppError {
  constructor(message, statusCode, details = null) {
    super(message);
    this.statusCode = statusCode;
    this.details = details;
  }
}

class NotFoundError extends HttpError {
  constructor(resource, id) {
    super(`${resource} with id '${id}' not found`, 404);
    this.resource = resource;
    this.resourceId = id;
  }
}

class ValidationError extends HttpError {
  constructor(message, fields) {
    super(message, 422, fields);
    this.fields = fields;
  }
}

class UnauthorisedError extends HttpError {
  constructor(message = "Authentication required") {
    super(message, 401);
  }
}

class ForbiddenError extends HttpError {
  constructor(message = "You do not have permission to perform this action") {
    super(message, 403);
  }
}

// Domain errors (business logic layer)
class InsufficientStockError extends AppError {
  constructor(productId, requested, available) {
    super(
      `Cannot fulfil order: only ${available} units of product ${productId} available (requested ${requested})`,
    );
    this.productId = productId;
    this.requested = requested;
    this.available = available;
  }
}

class DuplicateEmailError extends AppError {
  constructor(email) {
    super(`An account with email '${email}' already exists`);
    this.email = email;
  }
}
```

```javascript
// Usage
async function createUser(data) {
  const existing = await db.user.findByEmail(data.email);
  if (existing) {
    throw new DuplicateEmailError(data.email);
  }

  const validation = validateUser(data);
  if (!validation.valid) {
    throw new ValidationError("User data is invalid", validation.fields);
  }

  return db.user.create(data);
}

// Handler
try {
  await createUser(formData);
} catch (err) {
  if (err instanceof DuplicateEmailError) {
    showFieldError("email", "This email is already registered");
  } else if (err instanceof ValidationError) {
    showFormErrors(err.fields);
  } else {
    showGenericError("Something went wrong. Please try again.");
    logger.error("createUser failed unexpectedly", err);
  }
}
```

---

## 6. Error Handling in Async Code

`async/await` turns asynchronous code into synchronous-looking code — including error handling with `try/catch`.

```javascript
// async functions ALWAYS return a Promise
// Errors thrown inside become rejected Promises

async function getUser(id) {
  const user = await db.user.findById(id);
  if (!user) throw new NotFoundError("User", id);
  return user;
}

// Calling the above:
async function handleRequest(id) {
  try {
    const user = await getUser(id);
    return user;
  } catch (err) {
    if (err instanceof NotFoundError) {
      return res.status(404).json({ error: err.message });
    }
    throw err; // unexpected — let the global handler deal with it
  }
}
```

### Parallel Async Operations — Handling Multiple Errors

```javascript
// Promise.all — fails fast (rejects if ANY promise rejects)
async function loadDashboard(userId) {
  try {
    const [user, posts, notifications] = await Promise.all([
      getUser(userId),
      getPosts(userId),
      getNotifications(userId),
    ]);
    return { user, posts, notifications };
  } catch (err) {
    // This catches the FIRST rejection only
    // Other promises may still be pending
    throw err;
  }
}

// Promise.allSettled — waits for all, collects results AND errors
async function loadDashboardRobust(userId) {
  const results = await Promise.allSettled([
    getUser(userId),
    getPosts(userId),
    getNotifications(userId),
  ]);

  const [userResult, postsResult, notifResult] = results;

  if (userResult.status === "rejected") {
    // User data is critical — can't continue without it
    throw userResult.reason;
  }

  return {
    user: userResult.value,
    posts: postsResult.status === "fulfilled" ? postsResult.value : [],
    notifications: notifResult.status === "fulfilled" ? notifResult.value : [],
    // Gracefully degrade: dashboard loads even if posts/notifications fail
  };
}
```

### Error Propagation in async Call Chains

```javascript
// Errors propagate upward through the await chain
async function step3() {
  throw new Error("step3 failed");
}
async function step2() {
  await step3();
} // step3's error propagates
async function step1() {
  await step2();
} // step2's error propagates

async function main() {
  try {
    await step1();
  } catch (err) {
    console.log(err.message); // "step3 failed"
    // stack trace shows: step3 → step2 → step1 → main
  }
}
```

---

## 7. Promises: `.catch()` and Unhandled Rejections

```javascript
// .catch() is equivalent to .then(undefined, onRejected)
fetch("/api/data")
  .then((res) => res.json())
  .then((data) => processData(data))
  .catch((err) => {
    // Catches ANY error from the chain above
    console.error("Pipeline failed:", err);
  });

// Always handle rejections — missing .catch() → unhandled rejection
fetch("/api/data").then((res) => res.json());
// If fetch fails → unhandled rejection → warning in Node, crash in strict mode

// .finally() for cleanup (runs regardless of success/failure)
let isLoading = true;
fetch("/api/data")
  .then((res) => res.json())
  .then((data) => render(data))
  .catch((err) => showError(err))
  .finally(() => {
    isLoading = false; // always stop the spinner
  });
```

---

## 8. The Problem with Swallowing Errors

Swallowing an error means catching it and doing nothing — the error disappears silently.

```javascript
// TERRIBLE — swallowing an error
async function saveUser(data) {
  try {
    await db.user.create(data);
  } catch (err) {
    // ← nothing here — error is gone
    // The caller thinks the save succeeded
    // The user thinks their data was saved
    // It wasn't
  }
}

// ALSO BAD — console.log and pretend it didn't happen
async function saveUser(data) {
  try {
    await db.user.create(data);
  } catch (err) {
    console.log(err); // logs it, but caller still thinks it succeeded
  }
}

// CORRECT options:
// 1. Rethrow
async function saveUser(data) {
  try {
    await db.user.create(data);
  } catch (err) {
    logger.error("saveUser failed", { data, error: err.message });
    throw err; // caller knows it failed
  }
}

// 2. Throw a more specific error
async function saveUser(data) {
  try {
    await db.user.create(data);
  } catch (err) {
    if (err.code === "23505") {
      // PostgreSQL unique violation
      throw new DuplicateEmailError(data.email);
    }
    throw err;
  }
}

// 3. Return a meaningful failure (only for expected, non-critical failures)
async function findUser(id) {
  try {
    return await db.user.findById(id);
  } catch (err) {
    if (err.code === "ECONNREFUSED") {
      // DB is down — return null and let caller show "try again"
      return null;
    }
    throw err; // unexpected — rethrow
  }
}
```

---

## 9. Global Error Handlers

Some errors aren't caught locally. Global handlers are your last line of defence.

```javascript
// In the browser:

// Catches synchronous errors and errors in event handlers
window.addEventListener("error", (event) => {
  const { message, filename, lineno, colno, error } = event;
  sendErrorReport({ message, filename, lineno, error: error?.stack });
  // Don't call event.preventDefault() unless you want to suppress the console error
});

// Catches unhandled Promise rejections
window.addEventListener("unhandledrejection", (event) => {
  const reason = event.reason; // the rejection value (Error or otherwise)
  sendErrorReport({
    type: "unhandledRejection",
    error: reason?.stack ?? reason,
  });
  event.preventDefault(); // suppress "Uncaught (in promise)" in console
});
```

```javascript
// In Node.js:

process.on("uncaughtException", (err) => {
  // A synchronous error that wasn't caught
  // After this, the process is in an unknown state
  logger.error("Uncaught exception", err);

  // Perform synchronous cleanup, then exit
  // DO NOT try to continue — the state may be corrupted
  process.exit(1);
});

process.on("unhandledRejection", (reason, promise) => {
  // A Promise was rejected with no .catch() handler
  logger.error("Unhandled rejection", { reason, promise });
  // In Node.js 15+, this crashes the process by default
  // In older versions, it's just a warning — add this handler to crash explicitly
  process.exit(1);
});
```

---

## 10. Result Pattern — Errors as Values

Inspired by Rust and functional programming, the **Result pattern** treats errors as values instead of thrown exceptions. This makes error handling explicit and prevents forgetting to handle errors.

```javascript
// Result type — a value that is either OK or an Error
class Ok {
  constructor(value) {
    this.value = value;
    this.ok = true;
  }
}

class Err {
  constructor(error) {
    this.error = error;
    this.ok = false;
  }
}

const ok = (value) => new Ok(value);
const err = (error) => new Err(error);

// Functions return Result instead of throwing
async function parseJson(str) {
  try {
    return ok(JSON.parse(str));
  } catch (e) {
    return err(new SyntaxError(`Invalid JSON: ${e.message}`));
  }
}

async function fetchUser(id) {
  try {
    const user = await db.user.findById(id);
    if (!user) return err(new NotFoundError("User", id));
    return ok(user);
  } catch (e) {
    return err(e);
  }
}

// Caller MUST check the result — no surprises
async function handleGetUser(id) {
  const result = await fetchUser(id);

  if (!result.ok) {
    if (result.error instanceof NotFoundError) {
      return res.status(404).json({ error: result.error.message });
    }
    return res.status(500).json({ error: "Internal server error" });
  }

  return res.json(result.value);
}
```

The Result pattern works well for operations that are _expected_ to fail sometimes (form validation, file reads, API calls). For truly unexpected errors (bugs, programming errors), normal exceptions are still appropriate.

---

## 11. Error Handling in Node.js / Express

```javascript
// Express: synchronous error — throw in route handler
app.get("/users/:id", (req, res, next) => {
  try {
    const user = getUserSync(req.params.id);
    res.json(user);
  } catch (err) {
    next(err); // pass to Express error handler
  }
});

// Express: async error — must use next(err) or express-async-errors
app.get("/users/:id", async (req, res, next) => {
  try {
    const user = await getUser(req.params.id);
    res.json(user);
  } catch (err) {
    next(err);
  }
});

// Central error handler — MUST have 4 parameters
app.use((err, req, res, next) => {
  // Log the error
  logger.error({
    message: err.message,
    stack: err.stack,
    path: req.path,
    method: req.method,
  });

  // Determine status code
  const statusCode = err instanceof HttpError ? err.statusCode : 500;

  // Don't leak internal details to the client in production
  const isProduction = process.env.NODE_ENV === "production";

  res.status(statusCode).json({
    error: statusCode < 500 ? err.message : "Internal server error",
    // Only expose details and stack in development
    ...(isProduction
      ? {}
      : {
          details: err.details,
          stack: err.stack,
        }),
  });
});
```

---

## Summary

```
Error types:
  Error, TypeError, ReferenceError, RangeError, SyntaxError, AggregateError
  Check type with instanceof to handle differently

try/catch/finally:
  catch — runs when try block throws
  finally — ALWAYS runs (cleanup: close files, release locks)
  Rethrow errors you can't handle: throw err

Custom error classes:
  class MyError extends Error — gives name, statusCode, structured fields
  instanceof works across the chain
  Makes error handling code readable and specific

Async errors:
  async/await — use try/catch normally
  Promise chains — use .catch()
  Promise.all — fails fast
  Promise.allSettled — collect all results, handle individually

Never swallow errors:
  Empty catch blocks hide bugs and corrupt state
  Always: rethrow, throw a better error, or return a meaningful failure

Global handlers:
  Browser: window.onerror, window.onunhandledrejection
  Node.js: process.on("uncaughtException"), process.on("unhandledRejection")
  Log everything. Exit on uncaughtException in Node.

Result pattern:
  { ok: true, value } | { ok: false, error }
  Makes errors explicit — caller must handle both cases
```

---

_Next: [11 — JavaScript Design Patterns](11%20-%20JavaScript%20design%20Patterns.md)_
