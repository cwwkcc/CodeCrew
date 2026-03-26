> **Part 4 of 7.** Everything that changed with ES6 (2015) and beyond: template literals, arrow functions (deep dive), enhanced object literals, destructuring, spread/rest, iterators, array helpers, Map/Set/Symbol.

---

## Table of Contents

1. [Template Literals](#1-template-literals)
2. [Arrow Functions (Deep Dive)](#2-arrow-functions-deep-dive)
3. [Enhanced Object Literals](#3-enhanced-object-literals)
4. [Default Parameters (Deep Dive)](#4-default-parameters-deep-dive)
5. [Spread Operator](#5-spread-operator)
6. [Rest Operator](#6-rest-operator)
7. [Array Destructuring](#7-array-destructuring)
8. [Object Destructuring](#8-object-destructuring)
9. [Advanced & Nested Destructuring](#9-advanced--nested-destructuring)
10. [Ternary Operator (Advanced)](#10-ternary-operator-advanced)
11. [for...in Loop](#11-forin-loop)
12. [for...of Loop](#12-forof-loop)
13. [forEach](#13-foreach)
14. [map](#14-map)
15. [filter](#15-filter)
16. [find & findIndex](#16-find--findindex)
17. [every & some](#17-every--some)
18. [reduce](#18-reduce)
19. [Map Data Structure](#19-map-data-structure)
20. [Set Data Structure](#20-set-data-structure)
21. [Symbols](#21-symbols)

---

## 1. Template Literals

Template literals (backtick strings) were the first ES6 feature everyone loved.

### Basic Interpolation

```js
// Before ES6
const msg = "Hello, " + user.name + "! You have " + count + " messages.";

// ES6 template literal
const msg = `Hello, ${user.name}! You have ${count} messages.`;

// Any expression works inside ${}
const price = `Total: $${(quantity * unitPrice * (1 - discount)).toFixed(2)}`;
const status = `Status: ${user.isActive ? "Active" : "Inactive"}`;
const debug = `User object: ${JSON.stringify(user, null, 2)}`;
```

### Multi-line Strings

```js
// Before: required \n
const html = "<div class=\"container\">\n  <h1>Hello</h1>\n</div>";

// Template literal preserves real newlines
const html = `
  <div class="container">
    <h1>Hello, ${user.name}</h1>
    <p>Last login: ${formatDate(user.lastLogin)}</p>
  </div>
`.trim(); // .trim() removes the leading newline
```

### Tagged Template Literals

A tag is a function that processes a template literal — a powerful and underused feature.

```js
// Tag function receives (strings, ...values)
function highlight(strings, ...values) {
  return strings.reduce((result, str, i) => {
    const value = values[i - 1];
    return result + `<mark>${value}</mark>` + str;
  });
}

const term = "JavaScript";
const count = 42;
const msg = highlight`Found ${count} results for "${term}"`;
// "Found <mark>42</mark> results for "<mark>JavaScript</mark>""
```

### Real-World: SQL Query Building (Safe)

```js
// Tagged template for safe parameterized SQL queries
function sql(strings, ...values) {
  const query = strings.reduce((result, str, i) => {
    return result + str + (i < values.length ? `$${i + 1}` : "");
  }, "");
  return { query, values };
}

const userId = 123;
const role = "admin";

const { query, values } = sql`
  SELECT * FROM users
  WHERE id = ${userId}
    AND role = ${role}
    AND is_active = true
`;
// query: "SELECT * FROM users WHERE id = $1 AND role = $2 AND is_active = true"
// values: [123, "admin"]
// This PREVENTS SQL injection automatically
```

### Real-World: Email Template

```js
function buildWelcomeEmail({ name, verificationUrl, expiresIn }) {
  return {
    subject: `Welcome to Paideon, ${name}!`,
    html: `
      <div style="font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto;">
        <h1>Welcome, ${name}!</h1>
        <p>Thanks for signing up. Please verify your email address:</p>
        <a href="${verificationUrl}" style="background: #4F46E5; color: white; padding: 12px 24px; border-radius: 6px; text-decoration: none;">
          Verify Email
        </a>
        <p><small>This link expires in ${expiresIn}.</small></p>
      </div>
    `.trim(),
    text: `Welcome, ${name}!\n\nVerify your email: ${verificationUrl}\n\nExpires in: ${expiresIn}`,
  };
}
```

---

## 2. Arrow Functions (Deep Dive)

Arrow functions are not just shorter syntax — they have fundamentally different behavior around `this`, `arguments`, and constructors.

### The `this` Difference

Regular functions create their own `this` binding. Arrow functions inherit `this` from their **lexical (enclosing) scope**.

```js
// Problem with regular function in class/object method
class Timer {
  constructor() {
    this.count = 0;
  }

  start() {
    setInterval(function() {
      this.count++; // `this` here is NOT the Timer instance
      // In strict mode: TypeError (this is undefined)
      // In sloppy mode: `this` is the global object
    }, 1000);
  }
}

// Fix with arrow function
class Timer {
  constructor() {
    this.count = 0;
  }

  start() {
    setInterval(() => {
      this.count++; // `this` = Timer instance (from enclosing method scope) ✓
    }, 1000);
  }
}
```

### Arrow Functions Have No `arguments` Object

```js
function regularFn() {
  console.log(arguments); // Arguments [1, 2, 3]
}
regularFn(1, 2, 3);

const arrowFn = () => {
  console.log(arguments); // ReferenceError or outer function's arguments
};
arrowFn(1, 2, 3);

// Use rest params instead
const arrowFn = (...args) => {
  console.log(args); // [1, 2, 3] ✓
};
```

### Arrow Functions Cannot Be Constructors

```js
const Person = (name) => {
  this.name = name;
};
new Person("Alice"); // TypeError: Person is not a constructor
```

### Arrow Functions Have No `prototype`

```js
const fn = () => {};
fn.prototype; // undefined
```

### When to Use Arrow vs Regular

|Scenario|Use|
|---|---|
|Array methods (map, filter, reduce)|Arrow ✓|
|setTimeout/setInterval callbacks|Arrow ✓|
|Callbacks that need lexical `this`|Arrow ✓|
|Object method (needs own `this`)|Regular ✓|
|Constructor function|Regular ✓|
|Generator function|Regular ✓|
|Method in prototype/class body|Regular ✓|

---

## 3. Enhanced Object Literals

ES6 gave us a cleaner syntax for defining objects.

### Property Shorthand

```js
const name = "Alice";
const email = "alice@example.com";
const role = "admin";

// Old
const user = { name: name, email: email, role: role };

// ES6 shorthand — when variable name matches key name
const user = { name, email, role };
```

### Method Shorthand

```js
// Old
const api = {
  fetchUser: function(id) { return fetch(`/users/${id}`); },
};

// ES6 shorthand (also enables super keyword in class-like patterns)
const api = {
  fetchUser(id) { return fetch(`/users/${id}`); },
  async fetchUsers() { return (await fetch("/users")).json(); },
  *generateIds() { /* generator */ },
};
```

### Computed Property Names

```js
const prefix = "auth";

const handlers = {
  [`${prefix}Login`]: handleLogin,    // authLogin
  [`${prefix}Logout`]: handleLogout,  // authLogout
  [`${prefix}Refresh`]: handleRefresh // authRefresh
};

// Real-world: building a Redux reducer
const ACTION_TYPES = { INCREMENT: "INCREMENT", DECREMENT: "DECREMENT" };

const reducer = {
  [ACTION_TYPES.INCREMENT]: (state) => ({ ...state, count: state.count + 1 }),
  [ACTION_TYPES.DECREMENT]: (state) => ({ ...state, count: state.count - 1 }),
};

function handleAction(state, action) {
  const handler = reducer[action.type];
  return handler ? handler(state) : state;
}
```

### Getters and Setters in Object Literals

```js
const userProfile = {
  _firstName: "Alice",
  _lastName: "Chen",

  // Getter — accessed like a property, not a function call
  get fullName() {
    return `${this._firstName} ${this._lastName}`;
  },

  // Setter — called when you assign to the property
  set fullName(value) {
    const [first, ...rest] = value.split(" ");
    this._firstName = first;
    this._lastName = rest.join(" ");
  },
};

userProfile.fullName;           // "Alice Chen"
userProfile.fullName = "Bob Smith";
userProfile._firstName;         // "Bob"
```

---

## 4. Default Parameters (Deep Dive)

```js
// Default can be any expression, including function calls
function createEndpoint(
  path,
  method = "GET",
  timeout = getDefaultTimeout(),
  headers = { "Content-Type": "application/json" }
) {
  return { path, method, timeout, headers };
}

// Defaults can reference earlier parameters
function makeRange(start = 0, end = start + 10) {
  return { start, end };
}
makeRange();      // { start: 0, end: 10 }
makeRange(5);     // { start: 5, end: 15 }
makeRange(5, 20); // { start: 5, end: 20 }

// null does NOT trigger defaults — only undefined does
function greet(name = "stranger") {
  return `Hello, ${name}`;
}
greet(undefined); // "Hello, stranger" — default triggered
greet(null);      // "Hello, null" — no default, null is explicit
greet("");        // "Hello, " — empty string is not undefined
```

---

## 5. Spread Operator

The spread operator (`...`) **expands** an iterable (array, string, object) into individual elements.

### Spread in Arrays

```js
const a = [1, 2, 3];
const b = [4, 5, 6];

[...a, ...b]         // [1, 2, 3, 4, 5, 6] — combine
[0, ...a, ...b, 7]   // [0, 1, 2, 3, 4, 5, 6, 7] — insert anywhere
[...a]               // [1, 2, 3] — shallow copy

// Pass array as function arguments
Math.max(...a);      // same as Math.max(1, 2, 3)
console.log(...a);   // same as console.log(1, 2, 3)

// Spread strings into characters
[..."hello"] // ["h", "e", "l", "l", "o"]
```

### Spread in Objects

```js
const defaults = { theme: "light", lang: "en", fontSize: 14 };
const overrides = { theme: "dark", fontSize: 16 };

// Merge (later keys overwrite earlier ones)
const settings = { ...defaults, ...overrides };
// { theme: "dark", lang: "en", fontSize: 16 }

// Shallow clone
const original = { id: 1, name: "Alice" };
const copy = { ...original }; // independent primitive values

// Add/override one property cleanly
const updated = { ...user, email: "new@email.com", updatedAt: new Date() };
```

### Real-World: Immutable State Updates (Redux pattern)

```js
// Never mutate state directly — always create a new object
const state = {
  user: { id: 1, name: "Alice", prefs: { theme: "dark" } },
  cart: { items: [], total: 0 },
  loading: false,
};

// Update nested user preference immutably
const newState = {
  ...state,
  user: {
    ...state.user,
    prefs: {
      ...state.user.prefs,
      theme: "light", // only change this one value
    },
  },
};
// state is unchanged, newState has the update

// Add item to cart
const newState2 = {
  ...state,
  cart: {
    ...state.cart,
    items: [...state.cart.items, newItem],
    total: state.cart.total + newItem.price,
  },
};
```

### Spread with Function Calls

```js
function updateRecord(id, field, value, options) { /* ... */ }

const args = [recordId, "email", "new@email.com"];
const opts = { validate: true, notify: true };

updateRecord(...args, opts); // same as updateRecord(recordId, "email", "new@email.com", opts)
```

---

## 6. Rest Operator

Rest collects multiple elements into a single array. It looks the same as spread (`...`) but does the opposite — it **gathers**instead of expanding.

```js
// In function parameters: gather remaining args
function createLog(level, timestamp, ...messages) {
  console.log(`[${level}] ${timestamp}:`, ...messages);
}

createLog("INFO", "12:00", "User logged in", "IP: 1.2.3.4");
// messages = ["User logged in", "IP: 1.2.3.4"]

// In destructuring: gather remaining elements/properties
const [first, second, ...rest] = [1, 2, 3, 4, 5];
// first = 1, second = 2, rest = [3, 4, 5]

const { password, token, ...safeUser } = user;
// safeUser = user without password and token fields — great for API responses!
```

### Real-World: Omitting Sensitive Fields

```js
// Strip private fields before sending to client
async function getUserProfile(userId) {
  const user = await db.users.findById(userId);

  // Destructure out the private stuff, keep the rest
  const { passwordHash, internalId, twoFactorSecret, ...publicProfile } = user;

  return publicProfile;
}

// Or as a reusable function
function omit(obj, ...keys) {
  const omitSet = new Set(keys);
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => !omitSet.has(key))
  );
}

omit(user, "password", "ssn", "creditCard");
```

---

## 7. Array Destructuring

Destructuring lets you unpack values from arrays and objects into distinct variables.

### Basic Array Destructuring

```js
const coords = [40.7128, -74.0060]; // [lat, lng]

// Without destructuring
const lat = coords[0];
const lng = coords[1];

// With destructuring
const [lat, lng] = coords;
const [lat, lng, altitude = 0] = coords; // default for missing values

// Skipping elements
const [, second, , fourth] = [1, 2, 3, 4];
// second = 2, fourth = 4
```

### Swapping Variables

```js
let a = 1;
let b = 2;

// Old way needed a temp variable
// temp = a; a = b; b = temp;

// Destructuring swap
[a, b] = [b, a]; // a = 2, b = 1
```

### Returning Multiple Values from Functions

```js
// A function returning an array of [result, error] (Golang-style error handling)
async function trySafe(promise) {
  try {
    const result = await promise;
    return [result, null];
  } catch (error) {
    return [null, error];
  }
}

const [users, error] = await trySafe(fetchUsers());
if (error) {
  console.error("Failed:", error.message);
  return;
}
renderUsers(users);
```

### Destructuring with Rest

```js
const [head, ...tail] = [1, 2, 3, 4, 5];
// head = 1, tail = [2, 3, 4, 5]

// Real use: processing command-line arguments
const [node, script, command, ...args] = process.argv;
// command = "start", args = any extra arguments
```

---

## 8. Object Destructuring

### Basic Object Destructuring

```js
const user = {
  id: 1,
  name: "Alice",
  email: "alice@example.com",
  role: "admin",
  isActive: true,
};

const { name, email } = user;
// name = "Alice", email = "alice@example.com"
// (user is unchanged)
```

### Rename During Destructuring

```js
const { name: userName, email: userEmail } = user;
// userName = "Alice", userEmail = "alice@example.com"
// `name` and `email` are no longer defined as variables
```

### Default Values

```js
const { role = "user", avatar = "/default.png" } = user;
// role = "admin" (exists in user, default ignored)
// avatar = "/default.png" (doesn't exist in user, default used)
```

### Function Parameter Destructuring

The most powerful use — destructuring directly in the function signature:

```js
// Without destructuring
function createUser(options) {
  const name = options.name;
  const email = options.email;
  const role = options.role || "user";
  // ...
}

// With destructuring
function createUser({ name, email, role = "user", isActive = true }) {
  return { id: generateId(), name, email, role, isActive, createdAt: new Date() };
}

// Calling is still clean — pass an object
createUser({ name: "Alice", email: "alice@example.com" });
createUser({ name: "Bob", email: "bob@example.com", role: "admin" });
```

---

## 9. Advanced & Nested Destructuring

### Nested Object Destructuring

```js
const apiResponse = {
  status: 200,
  data: {
    user: {
      id: 1,
      name: "Alice",
      address: {
        city: "New York",
        country: "US",
      },
    },
    meta: {
      totalCount: 100,
      page: 1,
    },
  },
};

// Deeply nested destructuring
const {
  status,
  data: {
    user: {
      name,
      address: { city, country },
    },
    meta: { totalCount, page },
  },
} = apiResponse;
```

### Mixed Array/Object Destructuring

```js
// API returns: { users: [...], pagination: {...} }
const {
  data: { users: [firstUser, ...otherUsers], pagination: { currentPage, totalPages } },
} = apiResponse;
```

### Destructuring in for...of

```js
const products = [
  { id: 1, name: "Laptop", price: 999 },
  { id: 2, name: "Mouse",  price: 49  },
];

for (const { id, name, price } of products) {
  console.log(`${id}: ${name} — $${price}`);
}

// With Object.entries
const config = { host: "localhost", port: 5432, db: "paideon" };
for (const [key, value] of Object.entries(config)) {
  console.log(`${key}: ${value}`);
}
```

### Destructuring "Craziness" — Real Patterns

```js
// Normalize API data in one shot
const {
  user_id: id,
  full_name: name,
  email_address: email,
  account_status: rawStatus,
  roles: [primaryRole = "user"] = [], // first role, default "user" if array empty
} = rawApiResponse;

const isActive = rawStatus === "ACTIVE";
```

```js
// React: destructuring props with rename + defaults
function ProductCard({
  product: {
    id,
    name,
    price,
    images: [primaryImage = "/placeholder.jpg"] = [],
    tags = [],
  },
  onAddToCart,
  isLoading = false,
}) {
  return (/* JSX */);
}
```

---

## 10. Ternary Operator (Advanced)

The ternary `condition ? a : b` is an expression — it returns a value.

```js
// Basic
const label = isLoggedIn ? "Logout" : "Login";

// Nested (avoid more than 2 levels — use if/else or lookup instead)
const badge = role === "admin"
  ? "Admin"
  : role === "moderator"
  ? "Mod"
  : "User";

// In JSX (React rendering)
return (
  <div>
    {isLoading
      ? <Spinner />
      : error
      ? <ErrorMessage error={error} />
      : <DataTable data={data} />
    }
  </div>
);

// In object/array literals
const config = {
  timeout: isProduction ? 30_000 : 5_000,
  retries: isProduction ? 3 : 1,
  logLevel: isProduction ? "error" : "debug",
};
```

---

## 11. for...in Loop

`for...in` iterates over **all enumerable string keys** of an object, including inherited ones.

```js
const user = { name: "Alice", role: "admin", age: 30 };

for (const key in user) {
  console.log(key, user[key]);
}
// "name" "Alice"
// "role" "admin"
// "age"  30

// IMPORTANT: Always guard against inherited properties
for (const key in obj) {
  if (Object.hasOwn(obj, key)) {
    // safe — only own properties
  }
}
```

**Avoid `for...in` for arrays** — it can include non-index keys if any were added to the array or its prototype:

```js
Array.prototype.customMethod = function() {};
const arr = [1, 2, 3];

for (const i in arr) {
  console.log(i); // "0", "1", "2", "customMethod" — unexpected!
}

// Use for...of or forEach for arrays
```

---

## 12. for...of Loop

`for...of` iterates over **values** of any **iterable** — arrays, strings, Maps, Sets, generators, NodeLists, etc.

```js
// Arrays
for (const user of users) {
  console.log(user.name);
}

// Strings (iterates over characters, handles Unicode properly)
for (const char of "hello") {
  console.log(char); // "h", "e", "l", "l", "o"
}

// With index using entries()
for (const [index, user] of users.entries()) {
  console.log(index, user.name);
}

// Maps
const roleMap = new Map([["alice", "admin"], ["bob", "user"]]);
for (const [username, role] of roleMap) {
  console.log(`${username} is ${role}`);
}

// Sets
const uniqueTags = new Set(["js", "node", "js"]);
for (const tag of uniqueTags) {
  console.log(tag); // "js", "node" — no duplicate
}

// NodeList (DOM)
for (const element of document.querySelectorAll(".card")) {
  element.classList.add("visible");
}
```

### `for...of` vs `for...in`

||`for...in`|`for...of`|
|---|---|---|
|Iterates over|Keys (strings)|Values|
|Works on|Objects (and arrays, but avoid)|Any iterable|
|Includes inherited|Yes (use hasOwn guard)|No|
|Works on plain objects|Yes|No (not iterable)|

---

## 13. forEach

`forEach` calls a function for each element. It's cleaner than a for loop but has limitations: **no break, no continue, can't be awaited (async)**.

```js
const users = [
  { id: 1, name: "Alice", email: "alice@example.com" },
  { id: 2, name: "Bob",   email: "bob@example.com"   },
];

users.forEach((user, index, array) => {
  console.log(`${index + 1}/${array.length}: ${user.name}`);
});

// forEach always returns undefined — don't assign its result
const result = users.forEach(u => u.name); // undefined — not an array!

// forEach with async — PROBLEM
users.forEach(async (user) => {
  await sendEmail(user.email); // forEach doesn't wait for promises!
});

// Fix: use for...of for async operations
for (const user of users) {
  await sendEmail(user.email); // ✓ properly awaited
}

// Or if parallel: Promise.all with map
await Promise.all(users.map(user => sendEmail(user.email)));
```

---

## 14. map

`map` creates a **new array** by transforming each element. It's one of the most used array methods in modern JavaScript.

```js
// Basic transform
const prices = [10, 20, 30];
const withTax = prices.map(price => price * 1.08); // [10.8, 21.6, 32.4]

// Transform object shape
const users = [
  { id: 1, firstName: "Alice", lastName: "Chen", email: "alice@example.com" },
  { id: 2, firstName: "Bob",   lastName: "Smith", email: "bob@example.com"  },
];

// Create display-friendly version
const displayUsers = users.map(({ id, firstName, lastName, email }) => ({
  id,
  fullName: `${firstName} ${lastName}`,
  email,
  initials: `${firstName[0]}${lastName[0]}`,
}));

// Get just one field (pluck pattern)
const emailList = users.map(u => u.email);
// ["alice@example.com", "bob@example.com"]

// Create a lookup map (ID → user)
const usersById = users.map(u => [u.id, u]);
const userMap = new Map(users.map(u => [u.id, u]));
userMap.get(1); // { id: 1, firstName: "Alice", ... }
```

### Real-World: Transform API Response for UI

```js
async function loadDashboardMetrics() {
  const raw = await fetchMetrics();

  return raw.map(metric => ({
    id: metric.metric_id,
    label: metric.display_name,
    value: formatNumber(metric.raw_value),
    unit: metric.unit_type,
    change: {
      value: metric.pct_change,
      direction: metric.pct_change >= 0 ? "up" : "down",
      formatted: `${metric.pct_change >= 0 ? "+" : ""}${metric.pct_change.toFixed(1)}%`,
    },
    sparkline: metric.historical_values.map(v => ({
      date: v.date,
      value: v.value,
    })),
  }));
}
```

---

## 15. filter

`filter` creates a **new array** containing only elements that pass a test.

```js
const products = [
  { id: 1, name: "Laptop", price: 999,  category: "electronics", inStock: true  },
  { id: 2, name: "Shirt",  price: 29,   category: "clothing",    inStock: true  },
  { id: 3, name: "Phone",  price: 699,  category: "electronics", inStock: false },
  { id: 4, name: "Shoes",  price: 89,   category: "clothing",    inStock: true  },
];

// Single condition
const inStockItems = products.filter(p => p.inStock);

// Multiple conditions
const affordableElectronics = products.filter(
  p => p.category === "electronics" && p.price < 800 && p.inStock
);

// Chaining with map (very common pattern)
const productNames = products
  .filter(p => p.inStock)
  .filter(p => p.price < 500)
  .map(p => p.name);
// ["Shirt", "Shoes"]
```

### Real-World: Multi-Filter Dashboard

```js
function filterOrders(orders, filters) {
  return orders.filter(order => {
    // Each filter is optional — skip if not specified
    if (filters.status && order.status !== filters.status) return false;
    if (filters.minAmount && order.amount < filters.minAmount) return false;
    if (filters.maxAmount && order.amount > filters.maxAmount) return false;
    if (filters.dateFrom && new Date(order.createdAt) < new Date(filters.dateFrom)) return false;
    if (filters.dateTo && new Date(order.createdAt) > new Date(filters.dateTo)) return false;
    if (filters.customerId && order.customerId !== filters.customerId) return false;
    if (filters.search) {
      const search = filters.search.toLowerCase();
      const matches = order.customerName.toLowerCase().includes(search) ||
                      order.id.toString().includes(search);
      if (!matches) return false;
    }
    return true;
  });
}
```

---

## 16. find & findIndex

`find` returns the **first element** that matches, or `undefined`. `findIndex` returns the **index** or `-1`.

```js
const users = [
  { id: 1, name: "Alice", role: "user"  },
  { id: 2, name: "Bob",   role: "admin" },
  { id: 3, name: "Carol", role: "admin" },
];

users.find(u => u.role === "admin");      // { id: 2, name: "Bob", ... }
users.findIndex(u => u.role === "admin"); // 1
users.find(u => u.id === 99);            // undefined
users.findIndex(u => u.id === 99);       // -1

// findLast / findLastIndex (ES2023) — searches from end
users.findLast(u => u.role === "admin");  // { id: 3, name: "Carol", ... }

// Real-world: update one item in an array
const index = cart.items.findIndex(item => item.productId === targetId);
if (index !== -1) {
  cart.items[index] = { ...cart.items[index], quantity: newQty };
}
```

---

## 17. every & some

`every` returns `true` if **all** elements pass. `some` returns `true` if **at least one** passes. Both short-circuit.

```js
const permissions = ["read", "write", "delete", "admin"];
const required = ["read", "write"];

// Does the user have ALL required permissions?
const hasAll = required.every(perm => permissions.includes(perm)); // true

// Does the user have ANY of these permissions?
const hasAny = ["admin", "superadmin"].some(perm => permissions.includes(perm)); // true

// Validate all form fields
const fields = [
  { name: "email", value: "alice@example.com", valid: true },
  { name: "password", value: "pass123", valid: true },
  { name: "username", value: "", valid: false },
];

const isFormValid = fields.every(field => field.valid); // false
const hasErrors = fields.some(field => !field.valid);   // true

// Are all items in cart in stock?
const canCheckout = cart.items.every(item => item.inStock && item.quantity > 0);
```

---

## 18. reduce

`reduce` is the most powerful array method — it accumulates all elements into a single value. It can do everything map and filter can, plus much more.

```js
array.reduce(callback, initialValue)
// callback(accumulator, currentValue, currentIndex, array) => newAccumulator
```

### Summing Values

```js
const orders = [
  { id: 1, amount: 100 },
  { id: 2, amount: 250 },
  { id: 3, amount: 75  },
];

const total = orders.reduce((sum, order) => sum + order.amount, 0);
// 425
```

### Building Objects from Arrays

```js
const users = [
  { id: "u1", name: "Alice" },
  { id: "u2", name: "Bob"   },
];

// Create a lookup dictionary: { u1: { ...alice }, u2: { ...bob } }
const userById = users.reduce((acc, user) => {
  acc[user.id] = user;
  return acc;
}, {});

userById["u1"]; // { id: "u1", name: "Alice" }
```

### Grouping

```js
const orders = [
  { id: 1, status: "pending",   amount: 100 },
  { id: 2, status: "shipped",   amount: 200 },
  { id: 3, status: "pending",   amount: 150 },
];

const byStatus = orders.reduce((groups, order) => {
  (groups[order.status] ??= []).push(order);
  return groups;
}, {});

byStatus.pending; // [{ id: 1 }, { id: 3 }]
```

### Flattening

```js
const nested = [[1, 2], [3, 4], [5]];
const flat = nested.reduce((acc, arr) => [...acc, ...arr], []);
// [1, 2, 3, 4, 5] — same as nested.flat()
```

### Real-World: Cart Totals with reduce

```js
const cart = {
  items: [
    { productId: "p1", name: "Laptop",  price: 999.99, quantity: 1, taxRate: 0.08 },
    { productId: "p2", name: "Mouse",   price: 49.99,  quantity: 2, taxRate: 0.08 },
    { productId: "p3", name: "Monitor", price: 399.99, quantity: 1, taxRate: 0.08 },
  ],
};

const summary = cart.items.reduce(
  (acc, item) => {
    const subtotal = item.price * item.quantity;
    const tax = subtotal * item.taxRate;

    acc.itemCount += item.quantity;
    acc.subtotal += subtotal;
    acc.tax += tax;
    acc.total += subtotal + tax;

    return acc;
  },
  { itemCount: 0, subtotal: 0, tax: 0, total: 0 }
);

// { itemCount: 4, subtotal: 1499.96, tax: 119.997, total: 1619.957 }
```

---

## 19. Map Data Structure

`Map` is a key-value store like a plain object, but with important differences:

- Keys can be **any type** (not just strings)
- Maintains **insertion order**
- Has a `size` property
- Better performance for frequent additions/deletions

```js
const map = new Map();

// Setting values
map.set("key", "value");
map.set(1, "number key");
map.set({ id: 1 }, "object key");
map.set(true, "boolean key");

// Getting values
map.get("key");  // "value"
map.get(1);      // "number key"
map.size;        // 4

// Checking
map.has("key");  // true
map.has("nope"); // false

// Deleting
map.delete("key");
map.clear(); // removes all entries

// Iterating
for (const [key, value] of map) { /* ... */ }
map.forEach((value, key) => { /* ... */ });
[...map.keys()]
[...map.values()]
[...map.entries()]
```

### When to Use Map vs Object

|Criteria|Object|Map|
|---|---|---|
|Key types|Only strings/symbols|Any type|
|Key order|Not guaranteed|Insertion order|
|Size|`Object.keys().length`|`.size`|
|Prototypal keys|Has inherited keys|None|
|JSON serialization|`JSON.stringify`|Must convert|
|Performance (large data)|Worse|Better|

### Real-World: Caching and Rate Limiting

```js
// Cache API responses by URL
const cache = new Map();

async function cachedFetch(url, maxAgeMs = 60_000) {
  if (cache.has(url)) {
    const { data, timestamp } = cache.get(url);
    if (Date.now() - timestamp < maxAgeMs) {
      return data; // return cached result
    }
  }

  const data = await fetch(url).then(r => r.json());
  cache.set(url, { data, timestamp: Date.now() });
  return data;
}

// Rate limiter: track requests per IP (using object as key concept)
const requestCounts = new Map(); // Map<string, { count: number, resetAt: number }>

function checkRateLimit(ip, limit = 100, windowMs = 60_000) {
  const now = Date.now();
  const record = requestCounts.get(ip);

  if (!record || now > record.resetAt) {
    requestCounts.set(ip, { count: 1, resetAt: now + windowMs });
    return { allowed: true, remaining: limit - 1 };
  }

  if (record.count >= limit) {
    return { allowed: false, retryAfter: record.resetAt - now };
  }

  record.count++;
  return { allowed: true, remaining: limit - record.count };
}
```

---

## 20. Set Data Structure

A `Set` is a collection of **unique values** — it automatically deduplicates.

```js
const set = new Set([1, 2, 3, 2, 1]); // Set {1, 2, 3}
set.size; // 3

set.add(4);    // Set {1, 2, 3, 4}
set.add(1);    // Set {1, 2, 3, 4} — no duplicate added
set.has(2);    // true
set.delete(2); // removes 2
set.clear();   // removes all

// Convert to array
[...set]
Array.from(set)
```

### Real-World Uses

```js
// 1. Deduplication
const tags = ["js", "node", "js", "react", "node", "ts"];
const uniqueTags = [...new Set(tags)]; // ["js", "node", "react", "ts"]

// 2. Checking existence (faster than Array.includes for large collections)
const bannedEmails = new Set(["spam@example.com", "blocked@test.com"]);
function isEmailBanned(email) {
  return bannedEmails.has(email); // O(1) lookup vs O(n) for array.includes
}

// 3. Set operations (union, intersection, difference)
const adminIds = new Set([1, 2, 3, 4]);
const activeIds = new Set([2, 3, 5, 6]);

// Union: all from both
const union = new Set([...adminIds, ...activeIds]); // {1,2,3,4,5,6}

// Intersection: only in both
const intersection = new Set([...adminIds].filter(id => activeIds.has(id))); // {2,3}

// Difference: in adminIds but NOT in activeIds
const difference = new Set([...adminIds].filter(id => !activeIds.has(id))); // {1,4}

// 4. Track visited items
const processedOrderIds = new Set();

async function processOrders(orders) {
  for (const order of orders) {
    if (processedOrderIds.has(order.id)) continue; // skip duplicates
    await processOrder(order);
    processedOrderIds.add(order.id);
  }
}
```

---

## 21. Symbols

A `Symbol` is a **unique, immutable primitive value**. Every `Symbol()` call creates a new unique symbol, even with the same description.

```js
const sym1 = Symbol("id");
const sym2 = Symbol("id");
sym1 === sym2; // false! Every symbol is unique

// Description (just for debugging)
sym1.toString(); // "Symbol(id)"
sym1.description; // "id"
```

### Primary Uses

**1. Unique object keys (no collisions)**

```js
// Two different libraries can add their own metadata to the same object
// without overwriting each other
const AUTH_META = Symbol("auth");
const LOGGER_META = Symbol("logger");

const user = { id: 1, name: "Alice" };
user[AUTH_META] = { tokenHash: "abc", issuedAt: Date.now() };
user[LOGGER_META] = { trackingId: "xyz" };

// Symbol keys are invisible to normal enumeration
Object.keys(user);        // ["id", "name"] — Symbols not included
JSON.stringify(user);     // '{"id":1,"name":"Alice"}' — Symbols excluded
for (const k in user) {}  // only id and name

// You CAN get them if you know how
Object.getOwnPropertySymbols(user); // [Symbol(auth), Symbol(logger)]
```

**2. Well-Known Symbols (protocol/interface customization)**

```js
// Symbol.iterator — make any object iterable
class Range {
  constructor(start, end) {
    this.start = start;
    this.end = end;
  }

  [Symbol.iterator]() {
    let current = this.start;
    const end = this.end;
    return {
      next() {
        return current <= end
          ? { value: current++, done: false }
          : { done: true };
      },
    };
  }
}

const range = new Range(1, 5);
for (const n of range) {
  console.log(n); // 1, 2, 3, 4, 5
}
[...range]; // [1, 2, 3, 4, 5]

// Symbol.toPrimitive — customize type conversion
class Money {
  constructor(amount, currency) {
    this.amount = amount;
    this.currency = currency;
  }

  [Symbol.toPrimitive](hint) {
    if (hint === "number") return this.amount;
    if (hint === "string") return `${this.amount} ${this.currency}`;
    return this.amount; // default
  }
}

const price = new Money(99.99, "USD");
+price;        // 99.99 (number hint)
`${price}`;    // "99.99 USD" (string hint)
price + 10;    // 109.99 (default hint)
```

---

## Summary Cheat Sheet

```
Template literals: `${expr}` — interpolation, multiline, tagged templates
Arrow functions:   no own `this`, no `arguments`, no `prototype`, concise
Object literals:   shorthand props, shorthand methods, computed keys, get/set
Spread:            ...arr expands; { ...obj } clones; { ...a, ...b } merges
Rest:              ...args collects; { prop, ...rest } extracts remainder
Destructuring:     [a, b] = arr; { x, y } = obj; rename, default, nested
Ternary:           cond ? a : b — expression, not statement

Array helpers (all return new value, none mutate):
  forEach — iterate (no return value, avoid with async)
  map     — transform each element → new array
  filter  — keep matching elements → new array
  find    — first match or undefined
  findIndex — index of first match or -1
  every   — all pass? → boolean (short-circuits)
  some    — any pass? → boolean (short-circuits)
  reduce  — fold array into single value (most powerful)

Map:  any key type, ordered, .get/.set/.has/.delete, better perf
Set:  unique values, .add/.has/.delete, O(1) lookup, great for dedup
Symbol: unique keys, not enumerable, well-known symbols for protocols
```

---

_Next: [Part 5 — DOM Manipulation & Events](./part-5-dom-and-events.md)_