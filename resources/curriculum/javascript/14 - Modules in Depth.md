JavaScript modules let you split code across files and control exactly what each file exposes. Module 03 covered the basic import/export syntax. This file goes deeper — how the module system works, dynamic imports, module patterns in Node.js, and how bundlers interact with modules.

---

## Table of Contents

1. [How ES Modules Work](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#1-how-es-modules-work)
2. [Export Syntax — All Forms](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#2-export-syntax--all-forms)
3. [Import Syntax — All Forms](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#3-import-syntax--all-forms)
4. [Module Scope and the Live Binding](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#4-module-scope-and-the-live-binding)
5. [Dynamic Import](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#5-dynamic-import)
6. [Import Assertions and Import Maps](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#6-import-assertions-and-import-maps)
7. [CommonJS vs ES Modules](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#7-commonjs-vs-es-modules)
8. [Bundlers and Tree Shaking](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#8-bundlers-and-tree-shaking)
9. [Circular Dependencies](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#9-circular-dependencies)
10. [Module Patterns](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#10-module-patterns)

---

## 1. How ES Modules Work

When the browser (or Node.js) encounters `import`, it goes through three phases:

```
Phase 1: Construction
  ─────────────────────────────────────────────
  Parse every file.
  Recursively find all import statements.
  Build the complete module graph.
  No code executes yet.

Phase 2: Instantiation
  ─────────────────────────────────────────────
  Allocate memory for every exported binding.
  Wire up imports to the same memory locations.
  Values are not yet assigned — bindings exist but are empty.

Phase 3: Evaluation
  ─────────────────────────────────────────────
  Execute each module's top-level code, in dependency order.
  Fill in the exported bindings with actual values.
```

This three-phase approach is why:

- Circular dependencies can work (both sides see the same binding objects, even if values aren't filled yet)
- Top-level `await` is possible in modules (Phase 3 can be async)
- Static analysis is possible (imports are fully known before execution)

### ES Modules in the Browser

```html
<script type="module" src="app.js"></script>
```

```javascript
// app.js
import { greet } from "./utils.js";
greet("Ashan");
```

- `type="module"` is required — without it, `import` is a syntax error
- Module scripts are **deferred by default** (same as `defer`)
- Module scripts run in strict mode automatically
- Each module has its own scope — top-level variables are NOT global

### ES Modules in Node.js

Two ways to opt in:

```json
// package.json — treat all .js files as ESM
{ "type": "module" }
```

Or use the `.mjs` extension for individual module files.

```javascript
// Works in Node.js ESM
import { readFile } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";

// __dirname is not available in ESM — reconstruct it
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
```

---

## 2. Export Syntax — All Forms

### Named Exports

```javascript
// Export at declaration
export const API_URL = "https://api.paideon.lk";
export function greet(name) {
  return `Hello, ${name}`;
}
export class User {
  constructor(name) {
    this.name = name;
  }
}

// Export at the bottom (preferred — all exports visible in one place)
const API_URL = "https://api.paideon.lk";
function greet(name) {
  return `Hello, ${name}`;
}
class User {
  constructor(name) {
    this.name = name;
  }
}

export { API_URL, greet, User };

// Export with rename
export { greet as sayHello, User as UserModel };
```

### Default Export

One per module:

```javascript
// Export a value directly
export default function greet(name) { return `Hello, ${name}`; }

// Or at the bottom
function greet(name) { return `Hello, ${name}`; }
export default greet;

// Anonymous default (common for React components)
export default function() { return <div>Hello</div>; }
export default class { constructor() {} }
```

### Re-exporting

Aggregate and re-export from an index file:

```javascript
// src/utils/index.js — barrel file
export { formatDate } from "./date.js";
export { slugify } from "./string.js";
export { debounce } from "./function.js";

// Re-export everything
export * from "./date.js";

// Re-export default as named
export { default as DateUtils } from "./date.js";

// Re-export and rename
export { formatDate as format } from "./date.js";
```

```javascript
// Consumers import from one place
import { formatDate, slugify, debounce } from "./utils";
```

---

## 3. Import Syntax — All Forms

```javascript
// Named imports
import { greet, User } from "./utils.js";

// Named import with rename
import { greet as sayHello } from "./utils.js";

// Default import (name is arbitrary)
import greet from "./utils.js";
import MyGreeter from "./utils.js"; // same thing, different name

// Default + named in one line
import greet, { API_URL, User } from "./utils.js";

// Namespace import — all named exports in one object
import * as Utils from "./utils.js";
Utils.greet("Ashan");
Utils.API_URL;

// Side-effect only import — run the module, import nothing
import "./polyfills.js";
import "./register-service-worker.js";
```

### File Extensions and Resolution

```javascript
// In browsers — must include extension
import { greet } from "./utils.js"; // works
import { greet } from "./utils"; // fails in browser

// In Node.js with bundlers (Vite, webpack) — extension optional
import { greet } from "./utils"; // bundler resolves it

// Node built-ins with node: prefix (recommended in Node ESM)
import { readFile } from "node:fs/promises";

// Package imports
import React from "react"; // resolves to node_modules/react
```

---

## 4. Module Scope and the Live Binding

### Each Module Runs Once

No matter how many files import the same module, it runs exactly once. The same instance is shared:

```javascript
// counter.js
let count = 0;
export function increment() {
  count++;
}
export function getCount() {
  return count;
}

// a.js
import { increment } from "./counter.js";
increment(); // count is now 1

// b.js
import { increment, getCount } from "./counter.js";
increment(); // count is now 2
console.log(getCount()); // 2 — same counter module
```

This is useful for shared state (like singletons) but also a potential source of bugs if you expect isolation.

### Live Bindings

ES module exports are **live bindings** — not copies of values. When the exporting module updates a value, importers see the new value:

```javascript
// counter.js
export let count = 0;
export function increment() {
  count++;
}

// main.js
import { count, increment } from "./counter.js";
console.log(count); // 0
increment();
console.log(count); // 1 — live binding, reflects the update
```

This is different from CommonJS, where `module.exports = value` exports a copy.

---

## 5. Dynamic Import

Static `import` statements are resolved at parse time. Dynamic `import()` is a function that loads a module at runtime — on demand:

```javascript
// Load a module only when needed
const { greet } = await import("./utils.js");
greet("Ashan");

// In an event handler — lazy load a heavy feature
button.addEventListener("click", async () => {
  const { renderChart } = await import("./chart.js");
  renderChart(data);
});

// Conditional loading
const module = await import(
  isDevelopment ? "./dev-tools.js" : "./prod-logger.js"
);
```

Dynamic import returns a Promise that resolves to the module's namespace object (all named exports + `default`):

```javascript
const module = await import("./utils.js");
module.greet("Ashan");
module.default("Ashan"); // access default export
```

### Code Splitting with Dynamic Import

Bundlers (Vite, webpack) treat each dynamic import as a split point and create a separate chunk:

```javascript
// React lazy loading — component loads only when rendered
import { lazy, Suspense } from "react";

const Dashboard = lazy(() => import("./pages/Dashboard"));
const Profile = lazy(() => import("./pages/Profile"));

function App() {
  return (
    <Suspense fallback={<LoadingSpinner />}>
      <Routes>
        <Route path="/dashboard" element={<Dashboard />} />
        <Route path="/profile" element={<Profile />} />
      </Routes>
    </Suspense>
  );
}
```

Each route's bundle only downloads when the user navigates to it.

---

## 6. Import Assertions and Import Maps

### Import Assertions (JSON modules)

```javascript
// Import a JSON file directly as a module
import config from './config.json' assert { type: 'json' };
console.log(config.apiUrl);

// Dynamic import with assertion
const data = await import('./data.json', { assert: { type: 'json' } });
```

### Import Maps

Map bare specifiers to URLs — enables package imports in the browser without a bundler:

```html
<script type="importmap">
  {
    "imports": {
      "react": "https://esm.sh/react@18",
      "react-dom": "https://esm.sh/react-dom@18",
      "lodash": "https://esm.sh/lodash@4",
      "./utils": "/src/utils/index.js"
    }
  }
</script>

<script type="module">
  import React from "react"; // resolves to esm.sh URL
  import _ from "lodash";
</script>
```

---

## 7. CommonJS vs ES Modules

|                | CommonJS (CJS)                 | ES Modules (ESM)                        |
| -------------- | ------------------------------ | --------------------------------------- |
| Syntax         | `require()` / `module.exports` | `import` / `export`                     |
| Loading        | Synchronous                    | Asynchronous                            |
| Analysis       | Runtime                        | Static (parse time)                     |
| Exports        | Copies of values               | Live bindings                           |
| Tree-shaking   | No                             | Yes                                     |
| Default in     | Node.js (legacy)               | Browsers + modern Node                  |
| File extension | `.js` / `.cjs`                 | `.js` (with `"type":"module"`) / `.mjs` |

```javascript
// CommonJS
const fs = require("fs");
const { join } = require("path");
module.exports = { greet };
module.exports.greet = greet;

// ESM
import fs from "node:fs";
import { join } from "node:path";
export { greet };
export default greet;
```

### Interoperability

```javascript
// ESM can import CJS (Node.js)
import _ from "lodash"; // lodash is CJS — works, gets module.exports as default
import { merge } from "lodash"; // may or may not work — depends on the package

// CJS cannot natively require() ESM
const { greet } = require("./esm-module.mjs"); // ERROR — CJS cannot require ESM

// Workaround — dynamic import in CJS
const { greet } = await import("./esm-module.mjs");
```

---

## 8. Bundlers and Tree Shaking

### What Bundlers Do

Bundlers (Vite, webpack, Rollup, esbuild) take your module graph and produce optimised output:

1. **Resolve** — follow all imports, build the graph
2. **Transform** — TypeScript → JS, JSX → JS, SCSS → CSS
3. **Bundle** — combine modules into one (or a few) files
4. **Optimise** — minify, tree-shake, split code

### Tree Shaking

Tree shaking removes exports that are never imported. Works only with ES modules (static analysis):

```javascript
// utils.js — exports many functions
export function formatDate(date) { ... }
export function slugify(str)     { ... }
export function debounce(fn, ms) { ... }
export function throttle(fn, ms) { ... }

// main.js — only imports one
import { formatDate } from './utils.js';
```

After tree shaking, `slugify`, `debounce`, and `throttle` are NOT included in the bundle — dead code is eliminated.

### Why Tree Shaking Fails

```javascript
// BAD — named import from barrel that re-exports a side-effectful module
import { Button } from "@mui/material";
// Bundler may import the entire library if it cannot determine
// what else gets imported or if any module has side effects

// GOOD — direct path import (always tree-shakes cleanly)
import Button from "@mui/material/Button";

// BAD — dynamic property access defeats tree shaking
import * as Utils from "./utils";
const fn = Utils[userInput]; // bundler cannot know which exports are used

// BAD — CJS module — cannot tree-shake
const { formatDate } = require("./utils");
```

### `sideEffects` in package.json

Tell the bundler a package has no side effects (safe to tree-shake aggressively):

```json
// package.json
{
  "sideEffects": false
}

// Or list files that DO have side effects
{
  "sideEffects": ["./src/polyfills.js", "*.css"]
}
```

---

## 9. Circular Dependencies

A circular dependency is when A imports B and B imports A. ES modules handle this differently from CJS:

```javascript
// a.js
import { b } from "./b.js";
export const a = "A: " + b;

// b.js
import { a } from "./a.js";
export const b = "B: " + a;
```

In ESM, the bindings exist before evaluation. When `b.js` runs and tries to read `a`, the binding exists but has not been evaluated yet — it is `undefined` at that moment. The value fills in later.

**How to fix circular dependencies:**

```javascript
// Option 1 — extract shared code into a third module
// shared.js
export const BASE = "base";

// a.js
import { BASE } from "./shared.js";

// b.js
import { BASE } from "./shared.js";

// Option 2 — delay access using a function
// a.js
import { getB } from "./b.js";
export function getA() {
  return "A: " + getB();
}
// The function is not called until after both modules are evaluated
```

Circular dependencies are usually a sign that modules are not well-separated. Prefer extracting shared logic.

---

## 10. Module Patterns

### The Barrel File (Index Module)

Aggregate a folder's exports into a single entry point:

```
src/
  utils/
    date.js
    string.js
    number.js
    index.js   ← barrel
```

```javascript
// utils/index.js
export { formatDate, parseDate } from "./date.js";
export { slugify, truncate } from "./string.js";
export { clamp, lerp, formatNumber } from "./number.js";
```

```javascript
// Consumer
import { formatDate, slugify } from "./utils";
// instead of: import { formatDate } from './utils/date'
//             import { slugify } from './utils/string'
```

### Module as Singleton

A module runs once — use this for shared state:

```javascript
// store.js — a simple shared store
const state = { user: null, theme: "light" };

export function getState() {
  return { ...state };
}
export function setState(updates) {
  Object.assign(state, updates);
}
```

### Lazy Module Cache

Load a module once on first use, cache the result:

```javascript
let _heavyModule = null;

async function getHeavyModule() {
  if (!_heavyModule) {
    _heavyModule = await import("./heavy.js");
  }
  return _heavyModule;
}
```

### `import.meta`

Module-specific metadata:

```javascript
import.meta.url; // absolute URL of the current module file
import.meta.env; // Vite/bundler environment variables
import.meta.env.DEV; // true in development
import.meta.env.PROD; // true in production
import.meta.env.VITE_API_URL; // custom env var (must start with VITE_)

// Resolve a path relative to the current module
new URL("./assets/icon.svg", import.meta.url).href;
```

---

_End of JavaScript module._
