> Node.js has two module systems: CommonJS (the original, from 2009) and ES Modules (the standard, added in Node 12). They look similar but behave differently in ways that cause real confusion. This part covers both fully — how they load, how they resolve, how they interop, and why the differences exist.

---

## Table of Contents

1. [Why Modules Exist](#1-why-modules-exist)
2. [CommonJS — The Original System](#2-commonjs--the-original-system)
3. [How require() Works — The Resolution Algorithm](#3-how-require-works--the-resolution-algorithm)
4. [The Module Wrapper — What Node Adds Around Your Code](#4-the-module-wrapper--what-node-adds-around-your-code)
5. [Module Caching](#5-module-caching)
6. [Circular Dependencies in CommonJS](#6-circular-dependencies-in-commonjs)
7. [ES Modules — The Standard System](#7-es-modules--the-standard-system)
8. [How import Works — Static Analysis and Linking](#8-how-import-works--static-analysis-and-linking)
9. [CommonJS vs ES Modules — The Key Differences](#9-commonjs-vs-es-modules--the-key-differences)
10. [Interoperability — Mixing CJS and ESM](#10-interoperability--mixing-cjs-and-esm)
11. [The package.json Fields That Control Modules](#11-the-packagejson-fields-that-control-modules)
12. [__dirname and __filename in ESM](#12-__dirname-and-__filename-in-esm)
13. [Barrel Files and Re-exports](#13-barrel-files-and-re-exports)
14. [Dynamic import()](#14-dynamic-import)

---

## 1. Why Modules Exist

Before modules, all JavaScript in a browser shared one global scope. Every variable was global. Every script could overwrite every other script's variables.

```javascript
// script1.js
var user = { name: 'Alice' };

// script2.js
var user = { name: 'Bob' };  // silently overwrites script1's user

// script3.js
console.log(user.name);  // "Bob" — which one did you expect?
```

Node.js was designed from the start with a module system. Each file is its own scope. Nothing leaks unless explicitly exported.

```javascript
// user.js
const name = 'Alice';          // private to this file
module.exports = { name };     // explicitly exported

// main.js
const user = require('./user');
console.log(user.name);        // "Alice"
console.log(name);             // ReferenceError — not in scope
```

---

## 2. CommonJS — The Original System

CommonJS (CJS) is the module system Node.js shipped with. Every `.js` file in a Node.js project is a CommonJS module by default (unless configured otherwise).

### Exporting

```javascript
// Single export
module.exports = function add(a, b) { return a + b; };

// Object of exports
module.exports = {
  add: (a, b) => a + b,
  subtract: (a, b) => a - b,
};

// Named exports shorthand
exports.add = (a, b) => a + b;
exports.subtract = (a, b) => a - b;
// exports is a reference to module.exports
// ONLY works if you never reassign module.exports

// The trap:
exports = { add: (a, b) => a + b };  // WRONG — reassigns local variable
                                      // module.exports is now still {}
                                      // exports no longer points to module.exports
```

### Importing

```javascript
// Import the whole export
const math = require('./math');
math.add(1, 2);

// Destructure on import
const { add, subtract } = require('./math');
add(1, 2);

// Import a default export
const add = require('./add');  // when module.exports = function...
add(1, 2);

// Import built-in module
const fs = require('fs');
const path = require('path');

// Import npm package
const express = require('express');

// Import with path resolution
const config = require('./config');          // ./config.js or ./config/index.js
const utils = require('../shared/utils');    // relative path
const lodash = require('lodash');            // node_modules
```

---

## 3. How require() Works — The Resolution Algorithm

When you call `require('something')`, Node.js follows a specific algorithm to find the file.

### Step 1 — Is it a core module?

```
require('fs')
require('path')
require('http')
require('crypto')

Node checks its built-in module list first.
If it's a core module → load it, done.
Core modules always win over user files.
You cannot shadow 'fs' with your own fs.js.
```

### Step 2 — Is it a relative or absolute path?

```
require('./utils')
require('../config')
require('/absolute/path/to/file')

Starts with ./ or ../ or / → file resolution.

For require('./utils'), Node tries in order:
  1. ./utils           (exact path — no extension)
  2. ./utils.js
  3. ./utils.json      (JSON files are auto-parsed)
  4. ./utils.node      (native addon)
  5. ./utils/index.js  (directory with index file)
  6. ./utils/index.json
  7. ./utils/index.node

First match wins. If none found → Error: Cannot find module './utils'
```

### Step 3 — Package resolution (node_modules)

```
require('express')
require('lodash/debounce')

Not a core module, not a relative path → search node_modules.

Starting from the directory of the requiring file,
Node walks UP the directory tree looking for node_modules:

File: /projects/app/src/server.js
  Check: /projects/app/src/node_modules/express
  Check: /projects/app/node_modules/express          ← found here
  Check: /projects/node_modules/express
  Check: /node_modules/express

Once found in node_modules/express:
  Read: node_modules/express/package.json
  Find: "main" field → "index.js"
  Load: node_modules/express/index.js

Why walk up?
  Monorepos: packages at root can be shared by all packages.
  Nested dependencies: each package can have its own node_modules
  with its own versions of dependencies.
```

### Visualising the full algorithm

```
require(X) from file /dir/file.js:

1. If X is a core module → return it

2. If X starts with '/', './', or '../':
   a. Try load as FILE(X):
      - X, X.js, X.json, X.node
   b. Try load as DIRECTORY(X):
      - X/package.json → "main" field
      - X/index.js, X/index.json, X/index.node
   c. Not found → throw

3. Walk node_modules:
   dirs = ['/dir/node_modules', '/node_modules'] (all parent dirs)
   for each dir in dirs:
     Try load as FILE(dir/X)
     Try load as DIRECTORY(dir/X)
   Not found → throw

4. throw "Cannot find module X"
```

---

## 4. The Module Wrapper — What Node Adds Around Your Code

Every CommonJS module file is NOT executed as-is. Node.js wraps it in a function before executing it.

```javascript
// What you write (math.js):
const PI = 3.14159;
exports.area = (r) => PI * r * r;

// What Node.js actually executes:
(function(exports, require, module, __filename, __dirname) {
  const PI = 3.14159;
  exports.area = (r) => PI * r * r;
});
```

This wrapper is why:

```
exports     → the module.exports shorthand reference
require     → the require function (scoped to this file's directory)
module      → the Module object for this file
__filename  → absolute path to this file: '/projects/app/math.js'
__dirname   → absolute path to this file's directory: '/projects/app'

These are NOT globals. They look global but are function parameters.
Each module gets its own copies.

const x = 5;  → scoped to the wrapper function → private to this file
               → NOT accessible from other modules
               → NOT on the global object
```

You can see the actual wrapper:

```javascript
const Module = require('module');
console.log(Module.wrapper);
// [
//   '(function(exports, require, module, __filename, __dirname) { ',
//   '\n});'
// ]
```

---

## 5. Module Caching

`require()` caches modules after the first load. The second call to `require('./config')` returns the same object — it does not re-execute the file.

```javascript
// counter.js
let count = 0;
module.exports = {
  increment: () => ++count,
  get: () => count,
};

// a.js
const counter = require('./counter');
counter.increment();
counter.increment();
console.log(counter.get());  // 2

// b.js
const counter = require('./counter');  // same cached instance
console.log(counter.get());  // also 2 — same object as a.js got

// main.js
require('./a');
require('./b');
// counter.js is only executed ONCE
// both a.js and b.js share the same counter object
```

### The Cache Object

```javascript
// Inspect the cache
console.log(require.cache);
// {
//   '/projects/app/counter.js': Module { exports: {...}, loaded: true, ... },
//   '/projects/app/a.js': Module { ... },
//   ...
// }

// Cache key is the RESOLVED absolute path.
// require('./counter') and require('../app/counter') from different files
// both resolve to the same absolute path → same cache entry.

// Clearing the cache (for testing or hot reload):
delete require.cache[require.resolve('./counter')];
const freshCounter = require('./counter');  // re-executes the file
```

### Caching Implications

```javascript
// Modules are singletons by default.
// Any state in a module is shared across all importers.

// config.js — a singleton pattern
let config = null;

module.exports = {
  init(options) {
    config = options;
  },
  get() {
    return config;
  },
};

// main.js
require('./config').init({ db: 'postgres://...' });

// anywhere else in the app:
const cfg = require('./config').get();
// Returns the same config object set in main.js.
// This is the "module singleton" pattern.
```

---

## 6. Circular Dependencies in CommonJS

Circular dependencies happen when A requires B, and B requires A. Node.js handles this but the result is often surprising.

```javascript
// a.js
console.log('a: start');
const b = require('./b');
console.log('a: b.done =', b.done);
module.exports = { done: true };
console.log('a: end');

// b.js
console.log('b: start');
const a = require('./a');
console.log('b: a.done =', a.done);
module.exports = { done: true };
console.log('b: end');

// main.js
require('./a');
```

```
Output:
a: start
b: start
b: a.done = undefined    ← a.js hasn't finished yet!
b: end
a: b.done = true
a: end

Why:
1. main.js requires a.js → Node starts executing a.js
2. a.js requires b.js → Node starts executing b.js
3. b.js requires a.js → Node sees a.js is in cache (partially executed)
   → returns the CURRENT (incomplete) module.exports of a.js
   → at this point, a.js hasn't set module.exports yet → it's {}
4. b.js gets {} for a, so a.done = undefined
5. b.js finishes. b.done = true
6. a.js continues. b.done = true. ✓
7. a.js sets its own module.exports.done = true
```

### How to Handle Circular Dependencies

```javascript
// Option 1: Move the require() call inside the function that needs it
// (lazy require — defers until a.js has fully loaded)

// a.js
module.exports = {
  getB() {
    const b = require('./b');  // required lazily, inside function
    return b;
  }
};

// Option 2: Restructure to extract shared code to a third module
// a.js and b.js both require shared.js — no cycle

// Option 3: Use dependency injection
// Pass dependencies as parameters instead of requiring them

// Best advice: circular dependencies are a design smell.
// They usually indicate two modules that should be one,
// or a third module that should hold the shared logic.
```

---

## 7. ES Modules — The Standard System

ES Modules (ESM) is the official JavaScript module standard (ES2015/ES6). It is now the preferred system in Node.js.

### Syntax

```javascript
// Named exports
export const PI = 3.14159;
export function add(a, b) { return a + b; }
export class Calculator { ... }

// Default export (one per file)
export default function multiply(a, b) { return a * b; }

// Export list
const x = 1, y = 2;
export { x, y };
export { x as horizontal, y as vertical };  // rename on export

// Re-export from another module
export { add } from './math.js';
export * from './utils.js';
export { default as helper } from './helper.js';
```

```javascript
// Named imports
import { add, PI } from './math.js';

// Default import
import multiply from './multiply.js';

// Named + default
import multiply, { add, PI } from './math.js';

// Rename on import
import { add as sum } from './math.js';

// Import everything as namespace
import * as math from './math.js';
math.add(1, 2);
math.PI;

// Side-effect only import (runs the module, imports nothing)
import './setup.js';

// Built-in modules — use 'node:' prefix (recommended)
import { readFile } from 'node:fs/promises';
import { join } from 'node:path';

// npm packages
import express from 'express';
```

### Enabling ESM in Node.js

```
Three ways to use ESM:

1. File extension .mjs
   rename file.js → file.mjs → treated as ESM automatically

2. package.json "type": "module"
   { "type": "module" }
   → all .js files in this package are treated as ESM
   → .cjs extension forces CommonJS even with type:module

3. TypeScript with "module": "NodeNext" or "module": "ESNext"
   TypeScript compiles to either CJS or ESM based on tsconfig.
   NestJS uses CJS by default (CommonJS output from TypeScript compiler).
```

---

## 8. How import Works — Static Analysis and Linking

ESM loading is fundamentally different from CommonJS. This is the most important conceptual difference.

### CommonJS: Dynamic, Synchronous, Runtime

```javascript
// require() is a function call — happens at runtime
const lib = require('./lib');  // executes lib.js right now, synchronously

// You CAN require() conditionally:
if (process.env.NODE_ENV === 'test') {
  const mock = require('./mock-db');
}

// You CAN require() dynamically:
const name = 'lodash';
const _ = require(name);  // works

// You CAN require() inside functions:
function getDb() {
  return require('./db');  // lazy load
}
```

### ESM: Static, Asynchronous, Parse-Time

```javascript
// import is a declaration — processed before code runs
import { readFile } from 'node:fs/promises';
// Node KNOWS about this import before executing any code.

// You CANNOT import conditionally:
if (condition) {
  import { x } from './x.js';  // SyntaxError
}

// You CANNOT import with a dynamic name:
const name = './lib.js';
import something from name;  // SyntaxError

// import declarations must be at the TOP LEVEL — not inside functions.
```

### Why ESM is Static

```
Because import is static, tools can analyse your code WITHOUT running it.

Tree shaking:
  Bundlers (Webpack, Rollup, esbuild) read your imports statically.
  They know exactly which exports are used.
  Unused exports are eliminated from the final bundle.
  
  import { debounce } from 'lodash-es';
  // Bundler: only debounce is imported → only include debounce in bundle
  // Result: ~1KB instead of ~70KB for all of lodash
  
  With CommonJS require():
  const _ = require('lodash');
  // Bundler can't know at parse time which methods will be used
  // → includes entire lodash

Circular dependency handling:
  ESM: links are resolved before execution.
       Both files can reference each other's exports.
       The value is a LIVE BINDING — always reflects current value.
  
  CJS: require() executes the file at call time.
       Gets whatever was exported at that moment (snapshot).
```

### ESM Loading Phases

```
Three phases before any code runs:

Phase 1 — Construction (parsing):
  Download and parse all modules in the dependency graph.
  Build the import/export map.
  Does NOT execute any code.

Phase 2 — Instantiation (linking):
  Allocate memory for all exports.
  Connect imports to their corresponding exports.
  Exports exist but have no VALUE yet (temporal dead zone).

Phase 3 — Evaluation (execution):
  Execute each module's code, top to bottom.
  Assigns actual values to exports.
  Each module executes once (like CJS caching).

This is why ESM can handle circular dependencies better:
  Both modules are linked before either executes.
  A can reference B's exports and B can reference A's.
  As long as the VALUE is set before it's READ, no problem.
```

### Live Bindings

```javascript
// counter.mjs
export let count = 0;
export function increment() { count++; }

// main.mjs
import { count, increment } from './counter.mjs';

console.log(count);  // 0
increment();
console.log(count);  // 1  ← the import is a LIVE BINDING

// In CommonJS, this would NOT work:
// const { count, increment } = require('./counter');
// increment();
// console.log(count);  // still 0 — destructured a COPY of the value
```

---

## 9. CommonJS vs ES Modules — The Key Differences

```
                    CommonJS                    ES Modules
─────────────────────────────────────────────────────────────────────
Syntax              require() / module.exports   import / export
Resolution          Runtime                      Parse-time (static)
Loading             Synchronous                  Asynchronous
Execution           On require() call            After full graph parsed
Binding             Value copy (snapshot)        Live binding (reference)
Top-level await     No                           Yes
Tree-shakeable      No (dynamic)                 Yes (static)
__dirname           Available                    Not available (use import.meta)
__filename          Available                    Not available (use import.meta)
.json import        Yes (auto-parsed)            With assert { type: 'json' }*
Default in Node     Yes                          Only with type:module or .mjs
Conditional import  Yes (if/require)             Only with dynamic import()

*import assertions syntax is still evolving
```

### Top-Level await

```javascript
// Only available in ES Modules
// main.mjs

const config = await fetchConfig();  // at the top level of the file
// This works in ESM.
// The module waits for the promise before its exports are available.

// In CommonJS: top-level await is a SyntaxError.
// You need an async IIFE:
(async () => {
  const config = await fetchConfig();
  // ...
})();
```

---

## 10. Interoperability — Mixing CJS and ESM

In a real codebase you will encounter both. The rules for interop are specific.

### ESM importing CJS

```javascript
// lib.cjs (CommonJS)
module.exports = { name: 'Alice', greet() { return 'hello'; } };

// main.mjs (ESM importing CJS)
import lib from './lib.cjs';          // ✓ default import gets module.exports
console.log(lib.name);                // 'Alice'
console.log(lib.greet());             // 'hello'

// Named imports from CJS: NOT statically analysable
import { name } from './lib.cjs';     // may work, may not
// Node.js attempts to detect named exports from CJS via static analysis.
// Not reliable. Always use default import for CJS modules.
```

### CJS importing ESM

```javascript
// utils.mjs (ESM)
export const PI = 3.14;
export default function add(a, b) { return a + b; }

// main.cjs (CJS trying to import ESM)
const utils = require('./utils.mjs');  // ✗ ERR_REQUIRE_ESM

// CJS cannot synchronously require() an ESM module.
// ESM loading is async. require() is sync. Incompatible.

// Solution: use dynamic import() (which IS async)
async function loadUtils() {
  const utils = await import('./utils.mjs');
  console.log(utils.PI);       // 3.14
  console.log(utils.default);  // the add function
}
loadUtils();
```

### npm packages and dual CJS/ESM

```json
// package.json of a dual-mode package
{
  "name": "my-lib",
  "main": "./dist/cjs/index.js",      // for require()
  "module": "./dist/esm/index.js",    // for bundlers (not Node.js)
  "exports": {
    ".": {
      "import": "./dist/esm/index.js",   // for import
      "require": "./dist/cjs/index.js"   // for require()
    }
  }
}
```

The `exports` field (subpath exports) is the modern way. Node.js picks the right version automatically.

---

## 11. The package.json Fields That Control Modules

```json
{
  "name": "my-app",
  "version": "1.0.0",

  "type": "module",
  // "module" → all .js files are ESM
  // "commonjs" → all .js files are CJS (default if omitted)
  // Override per-file with .mjs (force ESM) or .cjs (force CJS)

  "main": "./dist/index.js",
  // Entry point for require() — legacy, still widely used
  // Ignored when "exports" is present (for the root export)

  "exports": {
    ".": {
      "import": "./dist/esm/index.js",
      "require": "./dist/cjs/index.js",
      "types": "./dist/types/index.d.ts"
    },
    "./utils": {
      "import": "./dist/esm/utils.js",
      "require": "./dist/cjs/utils.js"
    }
  },
  // "exports" is the modern entry point field.
  // Supports conditional exports (import vs require).
  // Also: BLOCKS access to files not listed here.
  //   require('my-lib/internal/secret') → ERR_PACKAGE_PATH_NOT_EXPORTED
  //   (if 'my-lib/internal/secret' is not in exports)

  "imports": {
    "#utils": "./src/utils.js",
    "#config": {
      "development": "./src/config.dev.js",
      "default": "./src/config.prod.js"
    }
  }
  // Private imports — subpath imports for THIS package only.
  // import { x } from '#utils'  →  resolves to ./src/utils.js
  // The # prefix distinguishes from npm packages.
  // Useful for internal aliases (like TypeScript paths but for runtime).
}
```

---

## 12. __dirname and __filename in ESM

CommonJS provides `__dirname` and `__filename` as part of the module wrapper. In ESM they don't exist.

```javascript
// CommonJS
console.log(__filename);  // '/projects/app/src/server.js'
console.log(__dirname);   // '/projects/app/src'

// ES Modules — they don't exist
console.log(__dirname);   // ReferenceError: __dirname is not defined

// ESM equivalent using import.meta.url
import { fileURLToPath } from 'node:url';
import { dirname } from 'node:path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

console.log(__filename);  // '/projects/app/src/server.js'
console.log(__dirname);   // '/projects/app/src'

// Or use import.meta directly
console.log(import.meta.url);  // 'file:///projects/app/src/server.js'
// import.meta.url is always a file:// URL, not a path.
// fileURLToPath converts it to a regular path string.

// Reading a file relative to the current module (common pattern):
import { readFile } from 'node:fs/promises';
import { join } from 'node:path';

const data = await readFile(join(__dirname, 'data.json'), 'utf8');

// In Node.js 21+, there's a shorter way:
const data = await readFile(
  new URL('./data.json', import.meta.url),
  'utf8'
);
// URL constructor with a base URL resolves relative paths.
// Avoids needing __dirname entirely.
```

---

## 13. Barrel Files and Re-exports

A barrel file is an `index.js` that re-exports from multiple files in a directory. It creates a single public API for a module.

```
src/
  math/
    add.js
    subtract.js
    multiply.js
    index.js     ← barrel file
```

```javascript
// math/add.js
export function add(a, b) { return a + b; }

// math/subtract.js
export function subtract(a, b) { return a - b; }

// math/multiply.js
export function multiply(a, b) { return a * b; }

// math/index.js  (barrel)
export { add } from './add.js';
export { subtract } from './subtract.js';
export { multiply } from './multiply.js';

// Consumer:
import { add, subtract } from './math';
// Instead of:
import { add } from './math/add.js';
import { subtract } from './math/subtract.js';
```

### The Barrel File Performance Problem

```
Barrels are convenient but hurt performance in two ways:

1. In Node.js (development, ts-node, jest):
   import { add } from './math';
   → Node resolves ./math → ./math/index.js
   → index.js imports add.js, subtract.js, multiply.js
   → All three files loaded, even if you only use 'add'
   
   Thousands of files × barrel imports = slow startup times.
   Common cause of slow Jest test runs and slow ts-node startup.

2. In bundlers (without tree-shaking):
   Same problem — bundler may include all barrel re-exports.

Best practices:
  Use barrels for public-facing library APIs (clean interface).
  Avoid deep barrel nesting (barrels that import barrels that import barrels).
  For internal code in a monorepo: direct imports are often faster.
  NestJS modules + TypeScript paths handle this cleanly at the framework level.
```

---

## 14. Dynamic import()

`import()` is the async function that lets you load a module at runtime. Works in both CJS and ESM.

```javascript
// Static import: resolved at parse time
import { add } from './math.js';

// Dynamic import: resolved at runtime, returns a Promise
const math = await import('./math.js');
math.add(1, 2);

// Conditional loading
async function loadFeature(name) {
  const module = await import(`./features/${name}.js`);
  return module.default;
}

// Lazy loading heavy dependencies
async function processImage(buffer) {
  // Only load sharp when actually needed
  const { default: sharp } = await import('sharp');
  return sharp(buffer).resize(800).toBuffer();
}

// In a CJS file — the ONLY way to load ESM
// (require() cannot load ESM)
async function loadEsmLib() {
  const { greet } = await import('./esm-only-lib.mjs');
  return greet('world');
}

// import() always returns a Module namespace object
const mod = await import('./math.js');
// {
//   add: [Function: add],
//   subtract: [Function: subtract],
//   default: [Function: default],  ← if there's a default export
//   [Symbol(Symbol.toStringTag)]: 'Module'
// }

// Destructure directly:
const { add, default: multiply } = await import('./math.js');
```

### Dynamic import vs require()

```
                    require()               import()
─────────────────────────────────────────────────────────────────
Returns             The exports object      Promise<Module namespace>
Synchronous         Yes                     No (always async)
Works in CJS        Yes                     Yes
Works in ESM        No                      Yes
Can load ESM        No                      Yes
Can load CJS        Yes                     Yes
Cache               require.cache           Same module cache
```

---

_Next: [Part 03 — The Event Loop in Depth](02%20-%20The%20Event%20Loop%20in%20Depth.md)_