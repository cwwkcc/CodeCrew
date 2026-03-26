> **Part 2 of 7.** Covers for/while/do-while loops, logical operators, arrays (with all built-in techniques), and JavaScript objects.

---

## Table of Contents

1. [Logical Operators](#1-logical-operators)
2. [For Loop](#2-for-loop)
3. [While Loop](#3-while-loop)
4. [Do-While Loop](#4-do-while-loop)
5. [Loop Control: break & continue](#5-loop-control-break--continue)
6. [Arrays: Introduction](#6-arrays-introduction)
7. [Array Techniques](#7-array-techniques)
8. [JavaScript Objects](#8-javascript-objects)

---

## 1. Logical Operators

Logical operators combine or invert boolean expressions. In JavaScript, they're more powerful than just `true`/`false` — they actually return **one of their operands**.

### `&&` — Logical AND

Returns the **first falsy** value it encounters, or the **last value** if all are truthy.

```js
true && true       // true
true && false      // false
"hello" && 42      // 42      (both truthy, returns last)
null && "hello"    // null    (null is falsy, short-circuits)
0 && "never runs"  // 0       (0 is falsy, short-circuits)
```

**Real use — guard conditions:**

```js
// Only call the function if the user exists AND has a permission
user && user.permissions && user.permissions.includes("write") && saveDocument();

// In React: conditional render
{isAuthenticated && <UserMenu />}
```

### `||` — Logical OR

Returns the **first truthy** value, or the **last value** if all are falsy.

```js
false || "default"    // "default"
null || undefined     // undefined (both falsy, returns last)
"value" || "default"  // "value"
0 || 42               // 42 (0 is falsy)
```

**Real use — fallback values:**

```js
const port = process.env.PORT || 3000;
const displayName = user.nickname || user.username || user.email;
```

**Trap:** `0` and `""` are falsy. If they're valid values, `||` will incorrectly fall back:

```js
const timeout = userConfig.timeout || 5000;
// If userConfig.timeout is 0 (immediate), this WRONGLY gives 5000!
```

### `??` — Nullish Coalescing

Returns the right side only if the left is **`null` or `undefined`** (not just any falsy value).

```js
0 ?? "default"          // 0     (0 is not null/undefined)
"" ?? "default"         // ""    (empty string is not null/undefined)
null ?? "default"       // "default"
undefined ?? "default"  // "default"

// Fix for the timeout problem above:
const timeout = userConfig.timeout ?? 5000; // 0 is preserved!
```

### `!` — Logical NOT

Inverts a boolean. `!!` converts any value to its boolean equivalent.

```js
!true       // false
!false      // true
!null       // true  (null is falsy, so NOT gives true)
!0          // true
!"hello"    // false (truthy string, NOT gives false)

// Double NOT — common idiom
!!null      // false
!!"hello"   // true
!!0         // false

// Real use: converting to boolean for a flag
const hasPermission = !!(user.roles && user.roles.length > 0);
```

### Logical Assignment Operators (ES2021)

```js
// ||= — assign only if left side is falsy
user.role ||= "guest"; // same as: user.role = user.role || "guest"

// &&= — assign only if left side is truthy
user.profile &&= cleanProfile(user.profile); // only clean if profile exists

// ??= — assign only if left side is null/undefined
config.timeout ??= 5000;
```

---

## 2. For Loop

The classic `for` loop gives you full control over iteration: initialization, condition, and update all in one line.

### Anatomy

```js
for (initialization; condition; update) {
  // body
}

// Step by step:
for (let i = 0; i < 5; i++) {
  // 1. let i = 0       — runs once before loop starts
  // 2. i < 5           — checked before every iteration
  // 3. body executes   — if condition is true
  // 4. i++             — runs after every iteration
  // 5. go back to 2
}
```

### Iterating Over an Array

```js
const users = ["alice", "bob", "charlie"];

for (let i = 0; i < users.length; i++) {
  console.log(i, users[i]); // 0 "alice", 1 "bob", 2 "charlie"
}
```

**Tip:** Cache `array.length` if the array could change or the loop is performance-critical:

```js
const len = users.length;
for (let i = 0; i < len; i++) { ... }
```

### Reverse Iteration

```js
for (let i = users.length - 1; i >= 0; i--) {
  console.log(users[i]); // "charlie", "bob", "alice"
}
```

### Nested For Loops

```js
// Building a multiplication table
for (let row = 1; row <= 3; row++) {
  for (let col = 1; col <= 3; col++) {
    process.stdout.write(`${row * col}\t`);
  }
  console.log(); // newline
}
// 1  2  3
// 2  4  6
// 3  6  9
```

**Real-world nested loop — processing a 2D data structure:**

```js
// Dashboard: weekly sales data — rows are weeks, cols are days
const weeklySales = [
  [120, 200, 150, 300, 250, 180, 90],  // week 1
  [140, 220, 160, 280, 270, 200, 110], // week 2
];

let grandTotal = 0;
for (let week = 0; week < weeklySales.length; week++) {
  let weekTotal = 0;
  for (let day = 0; day < weeklySales[week].length; day++) {
    weekTotal += weeklySales[week][day];
  }
  console.log(`Week ${week + 1} total: $${weekTotal}`);
  grandTotal += weekTotal;
}
console.log(`Grand total: $${grandTotal}`);
```

---

## 3. While Loop

A `while` loop runs as long as a condition is true. Use it when you don't know in advance how many iterations you'll need.

```js
while (condition) {
  // body
  // IMPORTANT: something inside must eventually make condition false
  // or you get an infinite loop
}
```

### Example: Polling for Data

```js
// Real-world: retry a failed API call up to N times
async function fetchWithRetry(url, maxRetries = 3) {
  let attempts = 0;
  let lastError;

  while (attempts < maxRetries) {
    try {
      const response = await fetch(url);
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      return await response.json();
    } catch (error) {
      lastError = error;
      attempts++;
      console.warn(`Attempt ${attempts} failed: ${error.message}`);
      // Exponential backoff: wait 1s, 2s, 4s between retries
      await sleep(1000 * 2 ** (attempts - 1));
    }
  }

  throw new Error(`Failed after ${maxRetries} attempts: ${lastError.message}`);
}

const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));
```

### Pagination with While

```js
// Fetching all pages of a paginated API
async function fetchAllUsers() {
  const allUsers = [];
  let page = 1;
  let hasMore = true;

  while (hasMore) {
    const response = await fetch(`/api/users?page=${page}&limit=100`);
    const { users, total, limit } = await response.json();

    allUsers.push(...users);
    hasMore = allUsers.length < total;
    page++;
  }

  return allUsers;
}
```

### Infinite Loop Guard

Always ensure your loop can terminate:

```js
// INFINITE LOOP — never terminates!
let x = 0;
while (x < 5) {
  console.log(x);
  // forgot x++ — x stays 0 forever
}

// Safe version
let x = 0;
while (x < 5) {
  console.log(x);
  x++; // termination condition progresses
}
```

---

## 4. Do-While Loop

Like a while loop, but the body runs **at least once** before the condition is checked.

```js
do {
  // body (runs at least once)
} while (condition);
```

### Example: User Input Validation

```js
// Keep asking until valid input is received
let userInput;
do {
  userInput = prompt("Enter a number between 1 and 10:");
  userInput = Number(userInput);
} while (isNaN(userInput) || userInput < 1 || userInput > 10);

console.log(`You entered: ${userInput}`);
```

### Real Use: Game Loop / Menu Loop

```js
// Simulating a terminal menu
let choice;
do {
  choice = showMenu(); // display and get option
  switch (choice) {
    case "1": viewProfile(); break;
    case "2": editSettings(); break;
    case "3": logout(); break;
  }
} while (choice !== "3");
```

**In practice:** `do-while` is rare. It's mainly useful when you need at least one execution guaranteed. Most loops use `for` or `while`.

---

## 5. Loop Control: `break` & `continue`

### `break` — Exit the Loop Immediately

```js
// Find the first admin user, stop searching once found
let adminUser = null;
for (let i = 0; i < users.length; i++) {
  if (users[i].role === "admin") {
    adminUser = users[i];
    break; // no point checking further
  }
}
```

### `continue` — Skip This Iteration

```js
// Process only active products (skip inactive ones)
for (let i = 0; i < products.length; i++) {
  if (!products[i].isActive) continue; // skip to next iteration

  // This only runs for active products
  renderProductCard(products[i]);
}
```

### Labeled Breaks (Nested Loops)

```js
// Break out of OUTER loop from inside INNER loop
outerLoop: for (let i = 0; i < matrix.length; i++) {
  for (let j = 0; j < matrix[i].length; j++) {
    if (matrix[i][j] === targetValue) {
      console.log(`Found at [${i}][${j}]`);
      break outerLoop; // breaks the outer loop, not just the inner one
    }
  }
}
```

---

## 6. Arrays: Introduction

Arrays are **ordered, indexed, dynamically-sized** collections of values. In JavaScript, arrays are objects — they can hold values of any type, including other arrays.

### Creating Arrays

```js
// Array literal (preferred)
const fruits = ["apple", "banana", "cherry"];

// Array constructor (avoid — ambiguous)
const arr = new Array(3);      // [empty × 3] — not [3]!
const arr2 = new Array(1, 2);  // [1, 2]

// Array.from — creates array from array-like or iterable
Array.from("hello")            // ["h", "e", "l", "l", "o"]
Array.from({ length: 5 }, (_, i) => i + 1) // [1, 2, 3, 4, 5]
Array.from(new Set([1, 2, 2, 3])) // [1, 2, 3] — deduplication

// Array.of
Array.of(7)   // [7] — unlike new Array(7) which creates empty array of length 7
```

### Accessing and Modifying Elements

```js
const items = ["a", "b", "c", "d"];

items[0]          // "a" — first element
items[items.length - 1] // "d" — last element
items.at(-1)      // "d" — ES2022: negative index from end
items.at(-2)      // "c"

items[1] = "B";   // ["a", "B", "c", "d"] — direct mutation
items[10] = "x";  // creates sparse array — items[4] through [9] are `empty`
```

### Core Mutation Methods (Modify the Original Array)

```js
const arr = [1, 2, 3];

// Adding elements
arr.push(4);         // [1, 2, 3, 4] — adds to END, returns new length
arr.unshift(0);      // [0, 1, 2, 3, 4] — adds to BEGINNING, returns new length

// Removing elements
arr.pop();           // removes from END, returns removed element
arr.shift();         // removes from BEGINNING, returns removed element

// Removing/inserting at any position: splice(start, deleteCount, ...items)
arr.splice(1, 2);           // removes 2 elements starting at index 1
arr.splice(1, 0, "a", "b"); // inserts "a", "b" at index 1 (nothing deleted)
arr.splice(2, 1, "x");      // replaces element at index 2 with "x"

// Sorting
const nums = [3, 1, 4, 1, 5];
nums.sort();                  // lexicographic sort — "10" < "9"!
nums.sort((a, b) => a - b);   // numeric ascending
nums.sort((a, b) => b - a);   // numeric descending

// Reversing
arr.reverse(); // mutates and returns the array
```

### Non-Mutating Methods (Return New Array)

```js
const arr = [1, 2, 3];

arr.concat([4, 5])        // [1, 2, 3, 4, 5] — original untouched
arr.slice(1, 3)           // [2, 3] — extracts portion (end exclusive)
arr.slice(-2)             // [2, 3] — last 2 elements

// Flat (nested arrays)
[1, [2, 3], [4, [5]]].flat()    // [1, 2, 3, 4, [5]] — 1 level
[1, [2, [3, [4]]]].flat(Infinity) // [1, 2, 3, 4] — fully flatten

// FlatMap — map + flat(1) in one step
[[1, 2], [3, 4]].flatMap(x => x) // [1, 2, 3, 4]
```

### Searching in Arrays

```js
const users = [
  { id: 1, name: "Alice", active: true },
  { id: 2, name: "Bob",   active: false },
  { id: 3, name: "Alice", active: true },
];

users.indexOf(2)            // only works for primitives, not objects
users.includes(2)           // same — primitives only

// For objects, use find/findIndex (covered in Part 4)
users.find(u => u.name === "Bob")         // { id: 2, name: "Bob", ... }
users.findIndex(u => u.id === 3)          // 2
users.findLast(u => u.name === "Alice")   // { id: 3 ... } — searches from end
```

### Checking / Introspecting Arrays

```js
Array.isArray([])      // true — the ONLY reliable way to check for arrays
Array.isArray({})      // false
[].length              // 0
[1, 2, 3].length       // 3
```

### Spread with Arrays

```js
const a = [1, 2, 3];
const b = [4, 5, 6];

const combined = [...a, ...b];       // [1, 2, 3, 4, 5, 6]
const copy = [...a];                 // shallow copy — not same reference

// Passing array items as function arguments
Math.max(...a); // same as Math.max(1, 2, 3)
```

---

## 7. Array Techniques

This section covers the power methods: the functional iteration helpers that are used constantly in real projects. (In-depth treatment is in Part 4 — here we cover the core mutating techniques and utility patterns.)

### Joining and Converting

```js
[1, 2, 3].join(", ")          // "1, 2, 3"
[1, 2, 3].join("")            // "123"
[1, 2, 3].toString()          // "1,2,3"

// Real use: building a SQL IN clause (conceptually)
const ids = [1, 2, 3, 4];
const placeholder = ids.map((_, i) => `$${i + 1}`).join(", ");
// "$1, $2, $3, $4"
```

### Copying Arrays (Shallow vs Deep)

```js
const original = [{ id: 1 }, { id: 2 }];

// Shallow copy — new array, but SAME object references inside
const shallow1 = [...original];
const shallow2 = original.slice();
const shallow3 = Array.from(original);
const shallow4 = original.concat();

// Mutating a nested object affects ALL shallow copies
shallow1[0].id = 99;
console.log(original[0].id); // 99 — changed!

// Deep copy — truly independent
const deep = JSON.parse(JSON.stringify(original)); // simple, loses functions/dates
const deep2 = structuredClone(original);           // modern, handles more types
```

### Removing Duplicates

```js
const tags = ["js", "node", "js", "react", "node"];

// Using Set (preserves insertion order)
const unique = [...new Set(tags)]; // ["js", "node", "react"]

// For arrays of objects (by a specific key)
function uniqueById(arr) {
  const seen = new Set();
  return arr.filter(item => {
    if (seen.has(item.id)) return false;
    seen.add(item.id);
    return true;
  });
}
```

### Grouping (ES2024 / Polyfill Pattern)

```js
const orders = [
  { id: 1, status: "pending",   amount: 100 },
  { id: 2, status: "shipped",   amount: 200 },
  { id: 3, status: "pending",   amount: 150 },
  { id: 4, status: "delivered", amount: 300 },
];

// Group by status
const grouped = orders.reduce((acc, order) => {
  const key = order.status;
  acc[key] = acc[key] || [];
  acc[key].push(order);
  return acc;
}, {});

/*
{
  pending:   [{ id: 1 }, { id: 3 }],
  shipped:   [{ id: 2 }],
  delivered: [{ id: 4 }],
}
*/

// ES2024: Object.groupBy
const grouped2 = Object.groupBy(orders, order => order.status);
```

### Chunking an Array

```js
// Split array into chunks of size n — useful for batch API calls
function chunk(arr, size) {
  const chunks = [];
  for (let i = 0; i < arr.length; i += size) {
    chunks.push(arr.slice(i, i + size));
  }
  return chunks;
}

const userIds = [1, 2, 3, 4, 5, 6, 7];
chunk(userIds, 3); // [[1, 2, 3], [4, 5, 6], [7]]

// Real use: send emails in batches of 50 to avoid API rate limits
const emailBatches = chunk(emailList, 50);
for (const batch of emailBatches) {
  await sendBatchEmail(batch);
  await sleep(1000); // rate limit gap
}
```

### Flattening Nested Menu Structure

```js
// Real-world: flatten a nested navigation config
const nav = [
  { label: "Home", href: "/" },
  {
    label: "Products",
    children: [
      { label: "Electronics", href: "/electronics" },
      { label: "Clothing",    href: "/clothing" },
    ],
  },
];

// Get all hrefs (including nested)
const allLinks = nav.flatMap(item =>
  item.children ? item.children.map(c => c.href) : [item.href]
);
// ["/", "/electronics", "/clothing"]
```

---

## 8. JavaScript Objects

Objects are **collections of key-value pairs** (properties). They're the backbone of JavaScript — nearly everything is an object.

### Creating Objects

```js
// Object literal (most common)
const user = {
  id: 1,
  name: "Alice",
  email: "alice@example.com",
  role: "admin",
  isActive: true,
  address: {         // nested object
    city: "New York",
    country: "US",
  },
  permissions: ["read", "write", "delete"], // array as value
};
```

### Accessing Properties

```js
// Dot notation (preferred when key is a valid identifier)
user.name          // "Alice"
user.address.city  // "New York"

// Bracket notation (required when key is dynamic or has special chars)
user["name"]       // "Alice"

const key = "email";
user[key]          // "alice@example.com" — dynamic access

const data = { "first-name": "Alice", "2024_revenue": 50000 };
data["first-name"]   // "Alice" — can't use dot notation for hyphenated keys
data["2024_revenue"] // 50000
```

### Adding, Updating, Deleting Properties

```js
const product = { id: 1, name: "Laptop" };

// Add
product.price = 999.99;
product["inStock"] = true;

// Update
product.name = "Gaming Laptop";

// Delete
delete product.inStock;

// Check if a property exists
"name" in product              // true
product.hasOwnProperty("name") // true — only own properties, not inherited
Object.hasOwn(product, "name") // true — modern, preferred over hasOwnProperty
```

### Iterating Over Objects

```js
const config = { host: "localhost", port: 5432, db: "paideon" };

// for...in — iterates over ALL enumerable keys (including inherited!)
for (const key in config) {
  if (Object.hasOwn(config, key)) { // guard against inherited props
    console.log(`${key}: ${config[key]}`);
  }
}

// Object.keys() — own enumerable property NAMES
Object.keys(config)   // ["host", "port", "db"]

// Object.values() — own enumerable property VALUES
Object.values(config) // ["localhost", 5432, "paideon"]

// Object.entries() — [key, value] pairs — most useful
Object.entries(config)
// [["host", "localhost"], ["port", 5432], ["db", "paideon"]]

// Loop with entries (most common in real code)
for (const [key, value] of Object.entries(config)) {
  console.log(`${key} = ${value}`);
}

// Practical: transform values
const uppercased = Object.fromEntries(
  Object.entries(config).map(([k, v]) => [k, String(v).toUpperCase()])
);
```

### Object Methods

```js
// Object.assign — shallow merge / copy
const defaults = { theme: "light", lang: "en", timeout: 5000 };
const userPrefs = { theme: "dark", lang: "fr" };
const settings = Object.assign({}, defaults, userPrefs);
// { theme: "dark", lang: "fr", timeout: 5000 }
// NOTE: Object.assign mutates the first argument — pass {} as target to avoid

// Spread (preferred modern way)
const settings2 = { ...defaults, ...userPrefs };
// Same result, cleaner syntax

// Object.freeze — makes object immutable (shallow)
const ROLES = Object.freeze({ ADMIN: "admin", USER: "user", MOD: "mod" });
ROLES.ADMIN = "superadmin"; // silently fails (or throws in strict mode)
ROLES.NEW = "new";          // also silently fails

// Object.keys / values / entries — see above

// Object.fromEntries — converts [key, value] pairs back to an object
const entries = [["a", 1], ["b", 2]];
Object.fromEntries(entries); // { a: 1, b: 2 }

// Real use: filter an object's keys
function pickFields(obj, fields) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => fields.includes(key))
  );
}
pickFields(user, ["name", "email"]); // { name: "Alice", email: "..." }

function omitFields(obj, fields) {
  return Object.fromEntries(
    Object.entries(obj).filter(([key]) => !fields.includes(key))
  );
}
omitFields(user, ["password", "internalId"]); // user without sensitive fields
```

### Destructuring Objects (Preview of Part 4)

```js
const { name, email, role = "user" } = user;
// Extracts name, email, role from user
// role defaults to "user" if undefined

// Rename during destructure
const { name: userName, email: userEmail } = user;

// Nested destructure
const { address: { city, country } } = user;

// Function parameter destructure (very common in real code)
function renderUserCard({ name, email, role, isActive }) {
  // use them directly, no user.name etc.
}
```

### Computed Property Names

```js
// Build an object with dynamic keys
const field = "username";
const value = "alice";

const update = { [field]: value }; // { username: "alice" }

// Real use: building a dynamic filter object
function buildFilter(filters) {
  return filters.reduce((acc, { field, value }) => {
    acc[field] = value;
    return acc;
  }, {});
}

buildFilter([
  { field: "status", value: "active" },
  { field: "role",   value: "admin" },
]);
// { status: "active", role: "admin" }
```

### Shorthand Property Notation

```js
const name = "Alice";
const email = "alice@example.com";
const role = "admin";

// Old way:
const user = { name: name, email: email, role: role };

// Shorthand (ES6):
const user = { name, email, role }; // when key and variable have same name
```

### Object Cloning Patterns

```js
const original = {
  id: 1,
  name: "Alice",
  address: { city: "NY" },
  tags: ["admin"],
};

// SHALLOW CLONE — nested objects are still shared references
const shallow = { ...original };
const shallow2 = Object.assign({}, original);

shallow.address.city = "LA"; // ALSO changes original.address.city!

// DEEP CLONE — fully independent
const deep1 = JSON.parse(JSON.stringify(original));
// Limitations: loses Date, Function, undefined, circular refs

const deep2 = structuredClone(original);
// Modern (Node 17+, all modern browsers): handles Date, RegExp, etc.
```

### Real-World Example: Building a User Profile Object

```js
// Incoming data from API — may be messy or partial
const rawApiUser = {
  user_id: "u_123",
  full_name: "Alice Chen",
  email_address: "alice@example.com",
  account_status: "ACTIVE",
  created_at: "2024-01-15T08:30:00Z",
  last_login: null,
  roles: ["admin", "editor"],
  profile: {
    bio: "Full-stack developer",
    avatar_url: "https://cdn.example.com/avatars/u_123.jpg",
  },
};

// Normalize to our app's format
function normalizeUser(raw) {
  return {
    id: raw.user_id,
    name: raw.full_name,
    email: raw.email_address,
    isActive: raw.account_status === "ACTIVE",
    createdAt: new Date(raw.created_at),
    lastLoginAt: raw.last_login ? new Date(raw.last_login) : null,
    roles: raw.roles ?? [],
    isAdmin: raw.roles?.includes("admin") ?? false,
    avatar: raw.profile?.avatar_url ?? "/default-avatar.png",
    bio: raw.profile?.bio ?? "",
  };
}

const user = normalizeUser(rawApiUser);
```

### Symbol as Object Keys

Symbols create unique property keys that won't clash with string keys:

```js
const SECRET_KEY = Symbol("secretKey");
const obj = {
  name: "Alice",
  [SECRET_KEY]: "top-secret-value",
};

obj[SECRET_KEY]    // "top-secret-value"
Object.keys(obj)   // ["name"] — Symbol is NOT enumerable!
JSON.stringify(obj) // '{"name":"Alice"}' — Symbol is NOT serialized

// Real use: framework internals, preventing key collisions
const TYPE = Symbol("type");
function createAction(type, payload) {
  return { [TYPE]: type, payload };
}
```

---

## Summary Cheat Sheet

```
Logical:       && (first falsy), || (first truthy), ?? (first non-null/undefined)
Assignment:    ||=  &&=  ??=
Loop choice:   for — known iterations; while — unknown; do-while — at least 1 run
Control:       break (exit), continue (skip), labeled break (nested loops)

Arrays:
  Mutating:    push/pop (end), shift/unshift (start), splice (anywhere), sort, reverse
  Non-mutating: slice, concat, flat, flatMap, spread
  Searching:   find, findIndex, includes, indexOf
  Utility:     Array.from, Array.isArray, structuredClone

Objects:
  Access:      dot (static key), bracket (dynamic key)
  Iterate:     Object.keys/values/entries + for...of
  Clone:       spread (shallow), structuredClone (deep)
  Merge:       { ...obj1, ...obj2 }
  Check key:   Object.hasOwn(obj, key) or 'key' in obj
```

---

_Next: [Part 3 — Functions, Scope, Closures, Object Methods, JSON, Dates & Timers](./part-3-functions.md)_