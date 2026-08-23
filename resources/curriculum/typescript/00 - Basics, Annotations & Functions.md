> **Series overview:** This is file 1 of 5. It covers what TypeScript is and why it exists, the full annotation system, type inference, the `any` type, function annotations (parameters, defaults, return types), `void`, and the `never` keyword in full depth.

---

## Table of Contents

1. [What Is TypeScript?](#1-what-is-typescript)
2. [Course & Project Setup](#2-course--project-setup)
3. [Annotations In Depth](#3-annotations-in-depth)
4. [Type Inference](#4-type-inference)
5. [The `any` Type](#5-the-any-type)
6. [Function Parameter Annotations](#6-function-parameter-annotations)
7. [Default Parameter Values](#7-default-parameter-values)
8. [Function Return Annotations](#8-function-return-annotations)
9. [Void Functions](#9-void-functions)
10. [The `never` Keyword In Depth](#10-the-never-keyword-in-depth)

---

## 1. What Is TypeScript?

TypeScript is a **statically-typed superset of JavaScript** developed and maintained by Microsoft. Every valid JavaScript file is also valid TypeScript — TypeScript just adds an optional, powerful type system on top.

### The Core Problem TypeScript Solves

JavaScript is dynamically typed. This is flexible, but it leads to entire classes of bugs that only appear at runtime — often in production, often affecting users.

```js
// Pure JavaScript — no errors until runtime
function calculateTotal(price, quantity) {
  return price * quantity;
}

calculateTotal("99.99", 3); // "99.9999.9999.99" — string repetition, not multiplication!
```

With TypeScript:

```ts
function calculateTotal(price: number, quantity: number): number {
  return price * quantity;
}

calculateTotal("99.99", 3);
// TS Error: Argument of type 'string' is not assignable to parameter of type 'number'
// — caught at COMPILE TIME, before any code runs
```

### TypeScript vs JavaScript: The Key Differences

| Feature              | JavaScript        | TypeScript                                 |
| -------------------- | ----------------- | ------------------------------------------ |
| Type system          | Dynamic (runtime) | Static (compile-time)                      |
| Type errors          | Runtime crash     | Compile-time error                         |
| IDE support          | Basic             | Excellent (full autocomplete, refactoring) |
| Runs in browser/Node | Directly          | Must compile to JS first                   |
| Learning curve       | Lower             | Slightly higher                            |
| Refactoring safety   | Low               | High                                       |
| Large codebases      | Fragile           | Manageable                                 |

### How TypeScript Works

```
Your .ts file
    ↓
TypeScript Compiler (tsc) — checks types, reports errors
    ↓
Plain .js file — this is what actually runs in browser/Node
    ↓
No type information survives at runtime
   (TypeScript types are ERASED — they're a compile-time tool only)
```

### Why TypeScript in Real Projects

1. **Catch bugs early** — find 15–30% of bugs before they ship
2. **Self-documenting code** — types ARE documentation that's always in sync
3. **IDE superpowers** — accurate autocomplete, rename refactoring, go-to-definition
4. **Confidence when refactoring** — change a type, and the compiler shows you every place that breaks
5. **Better team collaboration** — function signatures tell you exactly what they expect and return

```ts
// TypeScript function signature IS the documentation
async function createUser(
  data: CreateUserDTO,
  options?: { sendWelcomeEmail: boolean; role: UserRole },
): Promise<User> {
  // IDE knows the exact shape of `data` and `options`
  // You can't pass the wrong thing without the compiler screaming
}
```

---

## 2. Course & Project Setup

### Installing TypeScript

```bash
# Global install — gives you the `tsc` command
npm install -g typescript

# Check version
tsc --version  # Version 5.x.x

# In a project (preferred — version pinned to project)
npm install --save-dev typescript
```

### Initializing a TypeScript Project

```bash
# Creates tsconfig.json — the project configuration file
tsc --init
```

### `tsconfig.json` — The Project Config

```json
{
  "compilerOptions": {
    // Output settings
    "target": "ES2020", // Which JS version to compile to
    "module": "CommonJS", // Module system: CommonJS (Node), ESNext (browser/bundler)
    "outDir": "./dist", // Where compiled JS files go
    "rootDir": "./src", // Where your TS source files are

    // Type checking strictness
    "strict": true, // Enable all strict checks (ALWAYS use this)
    "noImplicitAny": true, // Error on implicit `any` type
    "strictNullChecks": true, // null and undefined are not assignable to other types
    "strictFunctionTypes": true, // Strict function parameter checking
    "noImplicitReturns": true, // All code paths must return a value
    "noUnusedLocals": true, // Error on unused local variables
    "noUnusedParameters": true, // Error on unused function parameters

    // Module resolution
    "moduleResolution": "node", // How to resolve imports
    "esModuleInterop": true, // Allow default imports from CommonJS modules
    "resolveJsonModule": true, // Allow importing .json files

    // Source maps (for debugging — maps compiled JS back to TS)
    "sourceMap": true,

    // Type definitions
    "lib": ["ES2020", "DOM"], // Which built-in types to include
    "types": ["node"], // Explicit @types packages to include

    // Path aliases
    "baseUrl": "./src",
    "paths": {
      "@/*": ["./*"] // import from "@/services/auth" instead of "../../services/auth"
    }
  },
  "include": ["src/**/*"], // Which files to compile
  "exclude": ["node_modules", "dist", "**/*.test.ts"]
}
```

### Compiling TypeScript

```bash
# Compile once
tsc

# Watch mode — recompiles on every file save
tsc --watch

# Compile a single file (no tsconfig)
tsc app.ts

# Compile and run immediately (development)
ts-node src/app.ts

# Using tsx (faster alternative to ts-node)
npx tsx src/app.ts
```

### Recommended Dev Setup

```bash
# For a Node.js project (like NestJS backend)
npm install --save-dev typescript ts-node @types/node nodemon

# nodemon + ts-node — auto-restart on file changes
# package.json scripts:
{
  "scripts": {
    "dev": "nodemon --exec ts-node src/index.ts",
    "build": "tsc",
    "start": "node dist/index.js"
  }
}
```

### Project Structure

```
my-project/
├── src/
│   ├── index.ts
│   ├── types/
│   │   └── index.ts        // all shared type definitions
│   ├── models/
│   │   └── user.ts
│   └── services/
│       └── auth.service.ts
├── dist/                   // compiled output (git-ignored)
├── tsconfig.json
└── package.json
```

---

## 3. Annotations In Depth

A **type annotation** is your explicit instruction to TypeScript about what type a variable, parameter, or return value should be.

```ts
// Syntax: variableName: Type = value
let variableName: Type;
```

### Primitive Type Annotations

```ts
// The three core primitive types
let username: string = "alice";
let age: number = 30;
let isActive: boolean = true;

// TypeScript also has these primitives
let bigNumber: bigint = 9007199254740993n;
let uniqueKey: symbol = Symbol("key");
let nothing: null = null;
let notSet: undefined = undefined;
```

### Why Annotation at Declaration?

When you declare a variable without a value, TypeScript can't infer the type — you must annotate:

```ts
// ❌ Without annotation — TypeScript infers `any` (in non-strict mode) or errors
let result; // type is `any` — we lost all type safety

// ✅ With annotation — TypeScript knows what to expect
let result: string;
result = "success";
result = 42; // TS Error: Type 'number' is not assignable to type 'string'
```

### Annotating Complex Types

```ts
// Arrays (see Part 2 for full detail)
let scores: number[] = [95, 87, 92];
let names: string[] = ["Alice", "Bob"];

// Objects (see Part 2 for full detail)
let user: { id: number; name: string; email: string } = {
  id: 1,
  name: "Alice",
  email: "alice@example.com",
};

// Functions (see sections 6–9)
let greet: (name: string) => string;
greet = (name) => `Hello, ${name}`;

// Union types (see Part 2)
let id: number | string = 1;
id = "user_abc"; // also valid
```

### The Type Annotation vs Value

The annotation and value are separate things. TypeScript only uses annotations for its static analysis — they don't exist at runtime.

```ts
const PORT: number = 3000;
// At runtime, this is just: const PORT = 3000;
// The `: number` is completely stripped during compilation
```

---

## 4. Type Inference

TypeScript doesn't require annotations everywhere. It's smart enough to **infer** types from context.

### Basic Inference

```ts
// TypeScript infers `number` from the literal value
const maxRetries = 3; // type: 3 (literal type — const gets literal type)
let maxRetries = 3; // type: number (let gets widened type)

const greeting = "Hello"; // type: "Hello" (literal)
let greeting = "Hello"; // type: string

const isReady = true; // type: true (literal)
let isReady = true; // type: boolean
```

### Why `const` Gets Literal Types

```ts
const direction = "north"; // type: "north" — can never be anything else
let direction = "north"; // type: string — could be reassigned to "south"
```

This matters for discriminated unions (Part 2).

### Inference in Functions

```ts
// Return type inferred from the return value
function add(a: number, b: number) {
  return a + b; // TypeScript infers return type: number
}

const result = add(1, 2); // TypeScript infers `result` as number

// Inferred from array methods
const users = [
  { id: 1, name: "Alice" },
  { id: 2, name: "Bob" },
];
// TypeScript infers: { id: number; name: string }[]

const names = users.map((u) => u.name);
// TypeScript infers: string[]

const firstUser = users[0];
// TypeScript infers: { id: number; name: string }
firstUser.name; // ✓ autocomplete works — TS knows the shape
```

### Inference from Destructuring

```ts
const user = { id: 1, name: "Alice", role: "admin" as const };

const { id, name, role } = user;
// id:   number
// name: string
// role: "admin"

const [first, ...rest] = [1, 2, 3, 4];
// first: number
// rest:  number[]
```

### When to Annotate vs Let TypeScript Infer

**Let TypeScript infer when:**

- You're initializing a variable with a value
- Function return types (usually inferred correctly)
- Local variables inside functions

**Annotate explicitly when:**

- Declaring a variable without an initial value
- Function parameters (TypeScript cannot infer these)
- You want to be more restrictive than inference would give
- Public API / exported functions (for documentation)
- The inferred type is too wide for your intent

```ts
// Infer ✓ — obvious from context
const total = price * quantity;
const upperName = name.toUpperCase();

// Annotate explicitly ✓ — declared without value
let accessToken: string;
let currentUser: User | null;

// Annotate explicitly ✓ — restrict to specific subtype
const status: "pending" | "active" | "inactive" = "pending";
// Without annotation, this would just be `string`
```

### Inference with `as const`

`as const` tells TypeScript to infer the most specific (narrowest) type possible:

```ts
const config = {
  endpoint: "/api/users",
  method: "GET",
  retries: 3,
}; // type: { endpoint: string; method: string; retries: number }

const config = {
  endpoint: "/api/users",
  method: "GET",
  retries: 3,
} as const;
// type: { readonly endpoint: "/api/users"; readonly method: "GET"; readonly retries: 3 }
// Every property becomes readonly and has its literal type
```

---

## 5. The `any` Type

`any` is TypeScript's escape hatch. A variable of type `any` can hold any value and TypeScript stops checking it.

```ts
let value: any = "hello";
value = 42; // OK
value = true; // OK
value = { id: 1 }; // OK
value.anything; // OK — no error (but crashes at runtime if value doesn't have `anything`)
value(); // OK — no error (crashes at runtime if value is not a function)
```

### Why `any` Is Dangerous

```ts
function processInput(input: any) {
  return input.toUpperCase(); // TypeScript won't catch this
}

processInput("hello"); // ✓ works at runtime
processInput(42); // ✗ crashes: TypeError: input.toUpperCase is not a function
processInput(null); // ✗ crashes: TypeError: Cannot read properties of null
```

The moment you use `any`, you lose ALL TypeScript benefits for that value. It's contagious — anything derived from `any`is also `any`:

```ts
let data: any = fetchSomething();
const name = data.user.name; // name is `any` — TypeScript gave up
const upper = name.toUpperCase(); // also `any` — it's spreading
```

### When `any` is Acceptable (Rare)

```ts
// 1. Migrating a JavaScript codebase to TypeScript — temporary
// 2. Third-party library with no types and no time to write them
// 3. JSON.parse — the result is genuinely unknown
const parsed: any = JSON.parse(rawJson);

// 4. Dynamic object manipulation (better to use Record<string, unknown>)
```

### `any` vs `unknown` — Prefer `unknown`

`unknown` is the type-safe alternative to `any`. You can assign anything to `unknown`, but you can't DO anything with it until you narrow its type.

```ts
let value: unknown = fetchData();

// ❌ Can't use it without narrowing
value.toUpperCase(); // TS Error: Object is of type 'unknown'

// ✅ Must narrow first
if (typeof value === "string") {
  value.toUpperCase(); // OK — TypeScript knows it's a string here
}

// Real-world: safe JSON parsing
function parseJson(raw: string): unknown {
  try {
    return JSON.parse(raw);
  } catch {
    return null;
  }
}

const data = parseJson(rawString);
// data is `unknown` — you must validate before using it
if (data !== null && typeof data === "object" && "name" in data) {
  console.log((data as { name: string }).name);
}
```

### Disabling `any` in Your Project

In `tsconfig.json` with `"strict": true` or `"noImplicitAny": true`, TypeScript will error if it would implicitly infer `any`:

```ts
// With noImplicitAny: true
function greet(name) {
  // TS Error: Parameter 'name' implicitly has an 'any' type
  return `Hello, ${name}`;
}

// Fix: annotate explicitly
function greet(name: string) {
  // ✓
  return `Hello, ${name}`;
}
```

---

## 6. Function Parameter Annotations

Function parameters are the most important place to annotate — TypeScript can never infer them from usage alone.

### Basic Parameter Annotations

```ts
// Syntax: paramName: Type
function greet(name: string): string {
  return `Hello, ${name}`;
}

function calculateTax(amount: number, taxRate: number): number {
  return amount * taxRate;
}

function setActive(userId: number, isActive: boolean): void {
  // ...
}
```

### Multiple Parameters

```ts
// All parameters annotated
function createApiRequest(
  endpoint: string,
  method: string,
  body: object,
  timeout: number,
): Promise<Response> {
  return fetch(endpoint, {
    method,
    body: JSON.stringify(body),
    signal: AbortSignal.timeout(timeout),
  });
}
```

### Object Parameters

```ts
// Inline object type (common for options bags)
function initServer(config: {
  port: number;
  host: string;
  debug: boolean;
}): void {
  // config.port is typed as number
  // config.host is typed as string
  // config.debug is typed as boolean
}

initServer({ port: 3000, host: "localhost", debug: true });
initServer({ port: "3000", host: "localhost", debug: true });
// TS Error: Type 'string' is not assignable to type 'number'

// Better: use a Type Alias (Part 2) for reusable parameter shapes
type ServerConfig = { port: number; host: string; debug: boolean };
function initServer(config: ServerConfig): void { ... }
```

### Optional Parameters

A parameter is optional when marked with `?`. It becomes `T | undefined`.

```ts
function greet(name: string, title?: string): string {
  if (title) {
    return `Hello, ${title} ${name}`;
  }
  return `Hello, ${name}`;
}

greet("Alice"); // "Hello, Alice"
greet("Alice", "Dr."); // "Hello, Dr. Alice"

// Inside the function, TypeScript knows title is `string | undefined`
function logRequest(url: string, method?: string) {
  const m = method; // type: string | undefined
  const m2 = method ?? "GET"; // type: string (narrowed)
  console.log(`${m2} ${url}`);
}
```

### Rest Parameters

```ts
// All remaining args collected into a typed array
function sum(...numbers: number[]): number {
  return numbers.reduce((total, n) => total + n, 0);
}
sum(1, 2, 3, 4, 5); // 15

// With some fixed params
function log(level: "info" | "warn" | "error", ...messages: string[]): void {
  console.log(`[${level.toUpperCase()}]`, ...messages);
}
log("info", "Server started", "Port: 3000");
log("error", "DB connection failed");
```

### Function Type Annotations

You can annotate variables that hold functions:

```ts
// Arrow function type syntax: (param: Type) => ReturnType
let handler: (event: MouseEvent) => void;
let transform: (input: string) => string;
let validate: (value: unknown) => value is string; // type predicate — Part 4

// As an object property
type EventHandlers = {
  onClick: (event: MouseEvent) => void;
  onChange: (value: string) => void;
  onSubmit: (data: FormData) => Promise<void>;
};
```

---

## 7. Default Parameter Values

Default values in TypeScript work exactly like JavaScript, but TypeScript also infers the type from the default:

```ts
// TypeScript infers `currency` has type `string` from the default value
function formatPrice(amount: number, currency = "USD"): string {
  return `${amount.toFixed(2)} ${currency}`;
}

formatPrice(99.99); // "99.99 USD"
formatPrice(99.99, "EUR"); // "99.99 EUR"
formatPrice(99.99, 42); // TS Error: Type 'number' is not assignable to type 'string'
```

### Default + Explicit Annotation

Sometimes you want to restrict the default to a more specific type:

```ts
// Without explicit annotation: role is inferred as `string`
function createUser(name: string, role = "user") {
  // role: string — too wide, "admin" and "superadmin" are also strings
}

// With explicit annotation: role is restricted to the union
function createUser(
  name: string,
  role: "user" | "admin" | "moderator" = "user",
): User {
  // role: "user" | "admin" | "moderator"
}

createUser("Alice"); // role = "user"
createUser("Alice", "admin"); // role = "admin"
createUser("Alice", "hacker"); // TS Error: not assignable to "user" | "admin" | "moderator"
```

### Defaults and `undefined`

A default is only used when the argument is `undefined` — same as JavaScript:

```ts
function connect(
  host: string,
  port: number = 5432,
  ssl: boolean = true
): void { ... }

connect("localhost");                    // port=5432, ssl=true
connect("localhost", undefined, false);  // port=5432, ssl=false (undefined triggers default)
connect("localhost", 3306, false);       // port=3306, ssl=false
```

### Real-World: API Request Options

```ts
interface RequestOptions {
  method?: "GET" | "POST" | "PUT" | "PATCH" | "DELETE";
  timeout?: number;
  retries?: number;
  headers?: Record<string, string>;
}

async function apiRequest(
  url: string,
  {
    method = "GET",
    timeout = 10_000,
    retries = 3,
    headers = {},
  }: RequestOptions = {}, // Default the whole options object to {} so it's optional
): Promise<unknown> {
  // method: "GET" | "POST" | "PUT" | "PATCH" | "DELETE"
  // timeout: number
  // retries: number
  // headers: Record<string, string>
}

apiRequest("/api/users"); // fully optional options
apiRequest("/api/users", { method: "POST", timeout: 5000 });
```

---

## 8. Function Return Annotations

The return type annotation goes after the parameter list, preceded by `:`.

```ts
function add(a: number, b: number): number {
  return a + b;
}

function getName(user: User): string {
  return user.name;
}

async function fetchUser(id: number): Promise<User> {
  const response = await fetch(`/api/users/${id}`);
  return response.json();
}
```

### Why Annotate Return Types?

TypeScript can infer return types, so why annotate explicitly?

1. **Self-documenting API** — callers immediately see what they'll get
2. **Prevent accidental widening** — inference might give a wider type than you intend
3. **Catch forgotten return paths** — TypeScript errors if you return the wrong type on ANY path
4. **Exported functions** — type information in `.d.ts` files is clearer

```ts
// Catches bugs on all return paths
function getStatusColor(status: string): string {
  if (status === "active") return "green";
  if (status === "inactive") return "gray";
  if (status === "banned") return "red";
  // TS Error: Function lacks ending return statement and return type does not include 'undefined'
  // — you forgot the default case!
}

// Fixed
function getStatusColor(status: string): string {
  if (status === "active") return "green";
  if (status === "inactive") return "gray";
  if (status === "banned") return "red";
  return "black"; // default ✓
}
```

### Returning Complex Types

```ts
// Returning a specific object shape
function parseToken(token: string): {
  userId: string;
  role: string;
  expiresAt: Date;
} {
  const payload = jwt.verify(token, SECRET) as any;
  return {
    userId: payload.sub,
    role: payload.role,
    expiresAt: new Date(payload.exp * 1000),
  };
}

// Better: use a named type
type TokenPayload = {
  userId: string;
  role: string;
  expiresAt: Date;
};

function parseToken(token: string): TokenPayload { ... }
```

### Returning Union Types

```ts
// Can return either a User or null
function findUser(id: number): User | null {
  const user = users.find((u) => u.id === id);
  return user ?? null;
}

const user = findUser(1);
user.name; // TS Error: Object is possibly 'null'

if (user !== null) {
  user.name; // ✓ — TypeScript narrowed type to User
}

// Or with optional chaining
const name = findUser(1)?.name; // string | undefined
```

### Returning Promises

```ts
// Promise<T> means the async function resolves to T
async function getUsers(): Promise<User[]> {
  const response = await fetch("/api/users");
  return response.json(); // TypeScript trusts this returns User[]
}

// Explicitly handling errors in return type
async function safeGetUser(id: number): Promise<User | null> {
  try {
    const response = await fetch(`/api/users/${id}`);
    if (!response.ok) return null;
    return response.json();
  } catch {
    return null;
  }
}
```

---

## 9. Void Functions

`void` means a function **does not return a meaningful value**. This is the return type for functions that are called for their side effects only.

```ts
function logMessage(message: string): void {
  console.log(`[${new Date().toISOString()}] ${message}`);
  // No return statement needed
  // Or: return; (empty return is OK)
  // But: return "something"; would be a TS Error
}

// Event handlers
function handleClick(event: MouseEvent): void {
  event.preventDefault();
  processClick(event.target);
}

// Setters / mutating functions
function setLoading(state: boolean): void {
  document.body.classList.toggle("loading", state);
}
```

### `void` vs `undefined`

These are similar but not identical:

```ts
// `void` function — TypeScript won't let you use the return value meaningfully
function logA(): void {
  console.log("a");
}

// `undefined` return — the function explicitly returns undefined
function logB(): undefined {
  console.log("b");
  return undefined; // must explicitly return undefined
}

const a = logA(); // a: void — you can't do anything with it
const b = logB(); // b: undefined

// In practice: use void for "fire and forget" functions
// TypeScript allows assigning a void-returning callback to a () => undefined callback
```

### `void` in Callback Signatures

```ts
// When you declare a callback as void, it means you don't care what the callback returns
type ForEachCallback<T> = (item: T, index: number) => void;

// This allows the caller to use a function that DOES return something
[1, 2, 3].forEach((n) => n * 2);
// forEach's callback is typed as (value: number, ...) => void
// The returned value from `n * 2` is just ignored — this is fine with `void`
```

### Real-World Usage

```ts
// Service with side effects — no return value
class NotificationService {
  sendEmail(to: string, subject: string, body: string): void {
    emailQueue.push({ to, subject, body });
  }

  // Async void — fire and forget
  async sendPushNotification(userId: string, message: string): Promise<void> {
    const token = await getDeviceToken(userId);
    await pushProvider.send(token, message);
    // No return value — called for side effect
  }
}

// Usage
const notifications = new NotificationService();
notifications.sendEmail("alice@example.com", "Welcome!", "...");
// We don't care about a return value — we just want the email sent
```

---

## 10. The `never` Keyword In Depth

`never` represents a type that **can never occur**. It's used for:

1. Functions that **never return** (throw always, or infinite loop)
2. **Exhaustiveness checking** — making sure you've handled all cases
3. Impossible type intersections

### Functions That Never Return

```ts
// A function that always throws — never returns normally
function throwError(message: string): never {
  throw new Error(message);
  // Nothing after throw can ever execute
  // TypeScript marks return type as `never`
}

// A function with an infinite loop
function runForever(): never {
  while (true) {
    processNextEvent();
  }
  // This function never returns — never is correct
}

// How `never` helps in conditional branches
function getUser(id: number): User {
  const user = db.find(id);
  if (!user) {
    throwError(`User ${id} not found`); // return type is `never`
    // TypeScript knows this branch NEVER continues past here
    // So no return needed after this call
  }
  return user; // TypeScript knows `user` is User here (not User | undefined)
}
```

### `never` for Exhaustiveness Checking

This is the most powerful use of `never` in real code. When you have a discriminated union and want TypeScript to error if you add a new case but forget to handle it:

```ts
type PaymentStatus =
  "pending" | "processing" | "completed" | "failed" | "refunded";

function handlePaymentStatus(status: PaymentStatus): string {
  switch (status) {
    case "pending":
      return "Awaiting payment";
    case "processing":
      return "Processing...";
    case "completed":
      return "Payment successful";
    case "failed":
      return "Payment failed";
    case "refunded":
      return "Amount refunded";
    default: {
      // If all cases are handled above, `status` here has type `never`
      // If you add a new status and forget to handle it,
      // TypeScript errors here because `status` would have that new type
      const exhaustiveCheck: never = status;
      throw new Error(`Unhandled payment status: ${exhaustiveCheck}`);
    }
  }
}

// Now you add a new status:
type PaymentStatus =
  "pending" | "processing" | "completed" | "failed" | "refunded" | "disputed";
// TS Error on the `never` line: Type '"disputed"' is not assignable to type 'never'
// — TypeScript tells you exactly which case you forgot to handle!
```

### Real-World: NestJS-style Action Dispatcher

```ts
type UserAction =
  | { type: "CREATE"; payload: CreateUserDTO }
  | { type: "UPDATE"; id: string; payload: UpdateUserDTO }
  | { type: "DELETE"; id: string }
  | { type: "ACTIVATE"; id: string };

function handleUserAction(action: UserAction): Promise<void> {
  switch (action.type) {
    case "CREATE":
      return userService.create(action.payload);
    case "UPDATE":
      return userService.update(action.id, action.payload);
    case "DELETE":
      return userService.delete(action.id);
    case "ACTIVATE":
      return userService.activate(action.id);
    default:
      // action here is `never` — all cases handled
      const _exhaustive: never = action;
      throw new Error(`Unknown action type`);
  }
}
```

### `never` in Type-Level Operations

```ts
// `never` in union — disappears (identity element for union)
type A = string | never; // string

// `never` in intersection — collapses everything
type B = string & number; // never — a type that is both string AND number is impossible

// Conditional type using never to filter
type NonNullable<T> = T extends null | undefined ? never : T;
// Removes null and undefined from a type
type StringOrNull = string | null | undefined;
type JustString = NonNullable<StringOrNull>; // string

// Filter types from a union
type Primitives = string | number | boolean | object | null;
type OnlyStrings = Primitives extends string ? Primitives : never;
// Distribution: (string extends string ? string : never) | (number extends string ? number : never) | ...
// = string | never | never | never | never
// = string
```

### `never` as Function Return When Combined with Type Guards

```ts
// Assert function — throws if condition is false, narrows type otherwise
function assert(condition: boolean, message: string): asserts condition {
  if (!condition) throw new Error(message);
}

function assertNonNull<T>(
  value: T,
  message: string,
): asserts value is NonNullable<T> {
  if (value == null) throw new Error(message);
}

// Usage
const user = await findUser(id);
assertNonNull(user, `User ${id} not found`);
user.name; // TypeScript knows `user` is not null/undefined here
```

---

## Summary Cheat Sheet

```
TypeScript:
  Superset of JS — adds static types, compiled away at runtime
  Catches bugs at compile time, not runtime
  `tsc --init` creates tsconfig.json
  `"strict": true` in tsconfig — always enable

Annotations: variableName: Type
  string | number | boolean | null | undefined | symbol | bigint
  Annotate: params, uninitialized vars, public APIs
  Infer: initialized vars, local variables, most return types

Type Inference:
  const → literal type ("north", 42, true)
  let → widened type (string, number, boolean)
  `as const` → deep literal + readonly

any:
  Disables all type checking — avoid
  Use `unknown` instead — forces narrowing before use
  `noImplicitAny: true` in tsconfig prevents accidental any

Functions:
  Params:      fn(name: string, age: number)
  Optional:    fn(name: string, title?: string)      → title is string | undefined
  Default:     fn(name: string, role = "user")       → type inferred from default
  Rest:        fn(...args: string[])
  Return:      fn(a: number): number
  Async:       async fn(): Promise<User>
  Void:        fn(): void                            → no return value
  Never:       fn(): never                           → never returns (throws/loops forever)

never:
  Functions that always throw or loop forever
  Exhaustiveness checks in switch/if-else (MOST IMPORTANT USE)
  Impossible type intersections (string & number = never)
  Add `const _: never = value` in default branch to catch missing cases
```

---

_Next: [Part 2 — Type System Deep Dive](./ts-part-2-types.md)_
