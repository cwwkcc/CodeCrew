> Errors in Node.js come from multiple sources — synchronous throws, rejected promises, event emitters, and system-level failures — and each requires a different handling strategy. This part covers the complete error model: how errors are represented, how they propagate, and how to build a system that handles them predictably without crashing.

---

## Table of Contents

1. [The Error Object]
2. [Error Types in Node.js]
3. [Synchronous Errors — try/catch]
4. [Async Errors — Callbacks]
5. [Async Errors — Promises]
6. [Async Errors — async/await]
7. [EventEmitter Error Events]
8. [Unhandled Errors — The Process Events]
9. [Custom Error Classes]
10. [Operational vs Programmer Errors]
11. [Error Handling in HTTP Servers]
12. [The Error Propagation Contract]
---

## 1. The Error Object

Every error in Node.js is an instance of `Error` (or a subclass of it).

```javascript
const err = new Error('Something went wrong');

err.message;  // 'Something went wrong'
err.name;     // 'Error' (or subclass name)
err.stack;    // Multiline string: error message + call stack

console.log(err.stack);
// Error: Something went wrong
//     at Object.<anonymous> (/app/index.js:3:13)
//     at Module._compile (node:internal/modules/cjs/loader:1364:14)
//     at ...
```

The stack trace shows you exactly where in the code the error was created. `err.stack` is one of the most useful debugging tools in Node.js.

### Attaching Extra Information

The `Error` object is just an object — you can add any properties:

```javascript
const err = new Error('User not found');
err.statusCode = 404;
err.userId = 42;

// When you catch it elsewhere:
if (err.statusCode === 404) {
  // handle 404 specifically
}
```

---

## 2. Error Types in Node.js

### Built-in JavaScript Error Types

```javascript
new Error('generic')            // base type
new TypeError('wrong type')     // wrong type passed to operation
new RangeError('out of range')  // value outside valid range
new ReferenceError('not defined') // accessing undefined variable
new SyntaxError('invalid code') // usually from eval() or JSON.parse()
new URIError('bad URI')
```

### Node.js System Errors

When a system call fails (file not found, connection refused, permission denied), Node.js throws a system error with a `code` property:

```javascript
try {
  await fs.readFile('missing.txt');
} catch (err) {
  err.code;     // 'ENOENT' — no such file or directory
  err.message;  // "ENOENT: no such file or directory, open 'missing.txt'"
  err.path;     // 'missing.txt'
  err.syscall;  // 'open'
}
```

### Common System Error Codes

```
ENOENT      No such file or directory
EEXIST      File already exists
EACCES      Permission denied
EPERM       Operation not permitted
ECONNREFUSED  Connection refused (server not running)
ECONNRESET    Connection reset by peer (remote dropped the connection)
ETIMEDOUT   Connection timed out
EADDRINUSE  Port already in use
EMFILE      Too many open files
EISDIR      Is a directory (expected a file)
ENOTDIR     Not a directory (expected a directory)
ENOTEMPTY   Directory not empty (rm failed)
EPIPE       Broken pipe (writing to closed stream)
```

---

## 3. Synchronous Errors — try/catch

Synchronous errors propagate up the call stack. `try/catch` intercepts them.

```javascript
function parseJSON(str) {
  return JSON.parse(str);   // throws SyntaxError if invalid
}

try {
  const data = parseJSON('not valid json');
  console.log(data);
} catch (err) {
  console.error('Parse failed:', err.message);
  // 'Parse failed: Unexpected token o in JSON at position 0'
}

console.log('Execution continues here');
```

### Re-throwing

Catch what you can handle. Re-throw what you cannot.

```javascript
try {
  const data = parseJSON(input);
} catch (err) {
  if (err instanceof SyntaxError) {
    // Handle specifically
    return defaultValue;
  }
  // Don't know how to handle this — re-throw
  throw err;
}
```

### finally — Always Runs

```javascript
async function withConnection() {
  const conn = await db.connect();
  try {
    return await conn.query('SELECT * FROM users');
  } catch (err) {
    console.error('Query failed:', err);
    throw err;
  } finally {
    await conn.close();  // always runs, even if we threw
  }
}
```

---

## 4. Async Errors — Callbacks

In the callback pattern, errors are passed as the first argument. This is the **error-first callback** (or Node callback) convention.

```javascript
fs.readFile('file.txt', (err, data) => {
  if (err) {
    // handle the error — do NOT throw inside callbacks
    console.error('Read failed:', err.message);
    return;
  }
  // data is only valid if err is null
  console.log(data.toString());
});
```

### The Rules

```
1. Always check the first argument (err) before using data.
2. Never throw inside a callback — there's nothing to catch it.
   Throwing in a callback will crash the process.
3. Return after handling the error to prevent execution continuing.
```

```javascript
// WRONG — throws inside callback
fs.readFile('file.txt', (err, data) => {
  if (err) throw err;   // unhandled, crashes the process
  process(data);
});

// CORRECT
fs.readFile('file.txt', (err, data) => {
  if (err) {
    handleError(err);
    return;
  }
  process(data);
});
```

---

## 5. Async Errors — Promises

A rejected promise carries the error. Handle it with `.catch()` or in a `try/catch` with `await`.

```javascript
fs.promises.readFile('file.txt', 'utf8')
  .then(data => process(data))
  .catch(err => console.error('Read failed:', err.message));
```

### Chained Promises

```javascript
fetchUser(id)
  .then(user => fetchPosts(user.id))
  .then(posts => formatPosts(posts))
  .then(result => res.json(result))
  .catch(err => {
    // catches errors from any step in the chain
    res.status(500).json({ error: err.message });
  });
```

### Unhandled Promise Rejections

A promise that rejects without a `.catch()` is an **unhandled rejection**. In Node.js 15+, this crashes the process by default (previously it just emitted a warning).

```javascript
// This will crash in Node 15+
Promise.reject(new Error('Oops'));  // no .catch()

// Always handle rejections
Promise.reject(new Error('Oops')).catch(err => {
  console.error(err);
});
```

---

## 6. Async Errors — async/await

`async/await` lets you use `try/catch` with async code — the most readable approach.

```javascript
async function processFile(path) {
  try {
    const data = await fs.promises.readFile(path, 'utf8');
    const parsed = JSON.parse(data);
    return parsed;
  } catch (err) {
    if (err.code === 'ENOENT') {
      console.log('File not found, using defaults');
      return {};
    }
    if (err instanceof SyntaxError) {
      console.error('Invalid JSON in file');
      return {};
    }
    throw err;  // unexpected error — re-throw
  }
}
```

### Handling Multiple Operations

```javascript
async function main() {
  // Option 1 — one try/catch wraps everything
  try {
    const user   = await fetchUser(id);
    const posts  = await fetchPosts(user.id);
    const result = await formatPosts(posts);
    return result;
  } catch (err) {
    // This catches errors from any of the three awaits
    throw err;
  }

  // Option 2 — individual error handling
  let user;
  try {
    user = await fetchUser(id);
  } catch (err) {
    throw new Error(`Could not fetch user ${id}: ${err.message}`);
  }

  // user is definitely set here
}
```

### Async forEach Trap

```javascript
// WRONG — errors in the async callback are swallowed
items.forEach(async (item) => {
  await processItem(item);  // if this throws, nobody catches it
});

// CORRECT — use for...of
for (const item of items) {
  await processItem(item);  // errors propagate up to the outer try/catch
}

// Or Promise.all for parallel execution
await Promise.all(items.map(item => processItem(item)));
```

---

## 7. EventEmitter Error Events

Classes that extend `EventEmitter` (including streams and the HTTP server) emit `'error'` events when something goes wrong.

```javascript
const { EventEmitter } = require('node:events');

const emitter = new EventEmitter();

// If 'error' is emitted with no listener, Node throws it — crashing the process
// ALWAYS add an error listener to EventEmitters

emitter.on('error', (err) => {
  console.error('Emitter error:', err.message);
});

emitter.emit('error', new Error('Something broke'));
```

### Streams

```javascript
const readable = fs.createReadStream('missing-file.txt');

// Without an error listener — crash
// With an error listener — handled
readable.on('error', (err) => {
  console.error('Stream error:', err.code);
});

// pipeline handles errors properly — always prefer it
const { pipeline } = require('node:stream/promises');

try {
  await pipeline(
    fs.createReadStream('input.txt'),
    fs.createWriteStream('output.txt'),
  );
} catch (err) {
  console.error('Pipeline failed:', err.message);
}
```

---

## 8. Unhandled Errors — The Process Events

Node.js emits process-level events when errors go unhandled.

```javascript
// Unhandled promise rejection
process.on('unhandledRejection', (reason, promise) => {
  console.error('Unhandled Rejection at:', promise, 'reason:', reason);
  // In production: log to error tracking service, then exit
  process.exit(1);
});

// Uncaught synchronous exception
process.on('uncaughtException', (err, origin) => {
  console.error('Uncaught Exception:', err);
  console.error('Origin:', origin);
  // The process is in an undefined state — do minimal cleanup and exit
  process.exit(1);
});
```

### Why You Should Exit on Uncaught Errors

When an uncaught exception occurs, the process is in an unknown state. Memory may be corrupted, connections may be in mid-operation, data may be partially written. Continuing to run is dangerous.

```
The correct response to an uncaught exception:
  1. Log the error (so you can diagnose it)
  2. Exit the process (so a process manager can restart it)

NOT:
  - Silently swallow the error
  - Try to continue running
```

Use a process manager (PM2, systemd, Docker restart policy) to automatically restart the process after an exit.

---

## 9. Custom Error Classes

Custom error classes let you distinguish error types programmatically and attach structured data.

```javascript
// Base application error
class AppError extends Error {
  constructor(message, options = {}) {
    super(message);
    this.name        = this.constructor.name;
    this.statusCode  = options.statusCode  ?? 500;
    this.isOperational = options.isOperational ?? true;
    Error.captureStackTrace(this, this.constructor);
  }
}

// Specific error types
class NotFoundError extends AppError {
  constructor(resource, id) {
    super(`${resource} with id ${id} not found`);
    this.statusCode = 404;
    this.resource   = resource;
    this.id         = id;
  }
}

class ValidationError extends AppError {
  constructor(field, message) {
    super(`Validation failed: ${message}`);
    this.statusCode = 400;
    this.field      = field;
  }
}

class AuthError extends AppError {
  constructor(message = 'Unauthorised') {
    super(message);
    this.statusCode = 401;
  }
}

class ForbiddenError extends AppError {
  constructor(message = 'Forbidden') {
    super(message);
    this.statusCode = 403;
  }
}
```

### Using Custom Errors

```javascript
async function getUser(id) {
  const user = await db.findUser(id);
  if (!user) {
    throw new NotFoundError('User', id);
  }
  return user;
}

// Catching:
try {
  const user = await getUser(42);
} catch (err) {
  if (err instanceof NotFoundError) {
    // handle specifically
  } else if (err instanceof AppError) {
    // generic app error
  } else {
    // unexpected — re-throw or escalate
    throw err;
  }
}
```

---

## 10. Operational vs Programmer Errors

A useful mental model divides all errors into two categories:

```
Operational errors:
  Expected failures at runtime.
  Part of normal operation — they will happen.
  You should handle them gracefully.

  Examples:
    - User not found (404)
    - Invalid request body (400)
    - Database connection timeout
    - File not found
    - Network request failed

  Response: Handle the specific case, return an appropriate error response,
            continue running.

Programmer errors (bugs):
  Mistakes in the code itself.
  Should never happen in a correct program.
  Indicate that your code has a defect.

  Examples:
    - TypeError: cannot read property of undefined
    - Calling a function with wrong number of arguments
    - Passing a string where a number was expected
    - Logic errors

  Response: Log it, crash the process, fix the bug.
            Do NOT try to recover — the process is in an unknown state.
```

```javascript
class AppError extends Error {
  constructor(message, options = {}) {
    super(message);
    this.isOperational = options.isOperational ?? true;
    // isOperational = true  → expected failure, handle gracefully
    // isOperational = false → bug, crash and restart
  }
}

process.on('uncaughtException', (err) => {
  if (err.isOperational) {
    // Somehow reached top level despite being operational — log and continue
    console.error('Operational error at top level:', err);
  } else {
    // Programmer error — unknown state, must exit
    console.error('FATAL programmer error:', err);
    process.exit(1);
  }
});
```

---

## 11. Error Handling in HTTP Servers

The standard pattern for Express-style error handling:

```javascript
const http = require('node:http');

// Centralised error handler
function handleError(err, res) {
  const statusCode = err.statusCode ?? 500;
  const isOperational = err.isOperational ?? false;

  // Don't leak internal error details to clients in production
  const message = isOperational
    ? err.message
    : 'An unexpected error occurred';

  res.writeHead(statusCode, { 'Content-Type': 'application/json' });
  res.end(JSON.stringify({ error: message }));

  // Log everything regardless
  console.error({
    statusCode,
    message:    err.message,
    stack:      err.stack,
    isOperational,
  });
}

// Request handler
const server = http.createServer(async (req, res) => {
  try {
    await router(req, res);
  } catch (err) {
    handleError(err, res);
  }
});
```

### Async Error Propagation in Express

Express 4 does not catch errors from async route handlers automatically. You must either use a wrapper or upgrade to Express 5.

```javascript
// Express 4 — wrap every async handler
const asyncHandler = (fn) => (req, res, next) => {
  Promise.resolve(fn(req, res, next)).catch(next);
};

app.get('/users/:id', asyncHandler(async (req, res) => {
  const user = await getUser(req.params.id);  // throws NotFoundError if not found
  res.json(user);
}));

// Express error handling middleware — must have 4 parameters
app.use((err, req, res, next) => {
  const statusCode = err.statusCode ?? 500;
  res.status(statusCode).json({ error: err.message });
});
```

---

## 12. The Error Propagation Contract

A consistent rule for every function you write:

```
A function should either:
  (a) Return a value (success), or
  (b) Throw / reject with an Error (failure)

It should NEVER:
  - Return undefined/null to signal failure (use Result types or throw)
  - Throw a non-Error value like a string: throw 'error message'
    (strings have no stack trace)
  - Swallow errors silently:
      try { ... } catch (err) {}  ← never do this

For async functions:
  - Always return a Promise that resolves (success) or rejects (failure)
  - Never mix callbacks and promises in the same interface
```

```javascript
// WRONG — null to signal failure
async function findUser(id) {
  const user = await db.query(id);
  return user ?? null;  // caller must check for null
}

// WRONG — throw a string
throw 'User not found';  // no stack trace, no .message

// CORRECT — throw an Error
throw new NotFoundError('User', id);

// CORRECT — return value (caller never needs to handle failure path)
// only if the absence of a value is a valid, expected state:
async function findOptionalUser(id) {
  return await db.query(id) ?? null;
  // document clearly that null is a valid return, not an error
}
```