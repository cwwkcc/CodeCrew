> **Part 4 of 5.** Two of the most powerful and nuanced parts of TypeScript — generics from first principles to advanced patterns, and every type narrowing technique including type guards, assertion functions, and discriminated unions.

---

## Table of Contents

1. [Why Generics Exist](#1-why-generics-exist)
2. [Generic Functions](#2-generic-functions)
3. [Generic Interfaces](#3-generic-interfaces)
4. [Generic Classes](#4-generic-classes)
5. [Generic Type Aliases](#5-generic-type-aliases)
6. [Generic Constraints](#6-generic-constraints)
7. [Default Generic Parameters](#7-default-generic-parameters)
8. [Conditional Types](#8-conditional-types)
9. [Mapped Types](#9-mapped-types)
10. [Template Literal Types](#10-template-literal-types)
11. [Utility Types (Built-In Generics)](#11-utility-types-built-in-generics)
12. [Type Narrowing Overview](#12-type-narrowing-overview)
13. [`typeof` Guard](#13-typeof-guard)
14. [`instanceof` Guard](#14-instanceof-guard)
15. [Truthiness Narrowing](#15-truthiness-narrowing)
16. [Equality Narrowing](#16-equality-narrowing)
17. [`in` Operator Narrowing](#17-in-operator-narrowing)
18. [Discriminated Union Narrowing](#18-discriminated-union-narrowing)
19. [User-Defined Type Guards](#19-user-defined-type-guards)
20. [Assertion Functions](#20-assertion-functions)

---

## 1. Why Generics Exist

Generics solve the problem of writing code that works with **multiple types** while still being **type-safe**.

### The Problem Without Generics

```ts
// You'd need a separate function for each type
function firstNumber(arr: number[]): number | undefined {
  return arr[0];
}
function firstString(arr: string[]): string | undefined {
  return arr[0];
}
function firstUser(arr: User[]): User | undefined {
  return arr[0];
}
// This scales terribly — you'd need N functions for N types

// The `any` "solution" — loses all type information
function first(arr: any[]): any {
  return arr[0];
}
const result = first([1, 2, 3]);
result.toUpperCase(); // TypeScript allows this — but crashes at runtime!
```

### The Generic Solution

```ts
// ONE function that works for any type, while staying fully typed
function first<T>(arr: T[]): T | undefined {
  return arr[0];
}

const n = first([1, 2, 3]); // T is inferred as number → n: number | undefined
const s = first(["a", "b"]); // T is inferred as string → s: string | undefined
const u = first(users); // T is inferred as User   → u: User | undefined

n?.toFixed(2); // ✓ — TypeScript knows n is number | undefined
n?.toUpperCase(); // TS Error — number doesn't have toUpperCase
```

Think of `T` as a **type variable** — a placeholder that gets filled in with a real type when the function is called.

---

## 2. Generic Functions

### Syntax

```ts
// <T> is the "type parameter" — declared in angle brackets
function identity<T>(value: T): T {
  return value;
}

// Multiple type parameters
function pair<T, U>(first: T, second: U): [T, U] {
  return [first, second];
}

// Arrow function with generic
const identity = <T>(value: T): T => value;
// In TSX files, use <T,> to avoid ambiguity with JSX:
const identity = <T>(value: T): T => value;
```

### Type Inference with Generics

TypeScript infers `T` from the argument — you rarely need to specify it explicitly:

```ts
function wrap<T>(value: T): { value: T } {
  return { value };
}

wrap(42); // TypeScript infers T = number → { value: number }
wrap("hello"); // TypeScript infers T = string → { value: string }
wrap({ id: 1 }); // TypeScript infers T = { id: number } → { value: { id: number } }

// Explicit type argument (when inference fails or you want to be explicit)
wrap<string>("hello");
```

### Real-World Generic Functions

```ts
// Type-safe array manipulation
function last<T>(arr: T[]): T | undefined {
  return arr[arr.length - 1];
}

function chunk<T>(arr: T[], size: number): T[][] {
  const chunks: T[][] = [];
  for (let i = 0; i < arr.length; i += size) {
    chunks.push(arr.slice(i, i + size));
  }
  return chunks;
}

function groupBy<T, K extends string | number>(
  arr: T[],
  keyFn: (item: T) => K
): Record<K, T[]> {
  return arr.reduce((acc, item) => {
    const key = keyFn(item);
    (acc[key] ??= []).push(item);
    return acc;
  }, {} as Record<K, T[]>);
}

// Usage — fully typed
const orders: Order[] = [...];
const byStatus = groupBy(orders, o => o.status);
// byStatus: Record<OrderStatus, Order[]>
byStatus.pending;   // Order[]
byStatus.shipped;   // Order[]

const users: User[] = [...];
const byRole = groupBy(users, u => u.role);
// byRole: Record<"admin" | "user" | "moderator", User[]>
byRole.admin;  // User[]
```

### Generic Factory / Builder Functions

```ts
// Creates a type-safe event emitter
function createEventEmitter<Events extends Record<string, unknown>>() {
  const listeners = new Map<keyof Events, Set<Function>>();

  return {
    on<E extends keyof Events>(
      event: E,
      handler: (data: Events[E]) => void,
    ): void {
      if (!listeners.has(event)) listeners.set(event, new Set());
      listeners.get(event)!.add(handler);
    },

    emit<E extends keyof Events>(event: E, data: Events[E]): void {
      listeners.get(event)?.forEach((handler) => handler(data));
    },

    off<E extends keyof Events>(
      event: E,
      handler: (data: Events[E]) => void,
    ): void {
      listeners.get(event)?.delete(handler);
    },
  };
}

// Define the event map — type safety flows from here
type AppEvents = {
  "user.created": { userId: string; email: string };
  "user.deleted": { userId: string };
  "order.placed": { orderId: string; total: number; customerId: string };
  "payment.failed": { orderId: string; error: string };
};

const emitter = createEventEmitter<AppEvents>();

emitter.on("user.created", (data) => {
  // data is { userId: string; email: string } — fully typed!
  sendWelcomeEmail(data.email);
});

emitter.emit("order.placed", {
  orderId: "o1",
  total: 99.99,
  customerId: "c1",
});

emitter.emit("order.placed", { orderId: "o1" });
// TS Error: Property 'total' is missing in type '{ orderId: string }'
```

---

## 3. Generic Interfaces

```ts
// Generic interface with one type parameter
interface ApiResponse<T> {
  success: boolean;
  data: T;
  message: string;
  timestamp: string;
}

// Usage
type UserResponse = ApiResponse<User>;
// { success: boolean; data: User; message: string; timestamp: string }

type UsersListResponse = ApiResponse<User[]>;
// { success: boolean; data: User[]; message: string; timestamp: string }

// Discriminated union with generic
interface SuccessResponse<T> {
  success: true;
  data: T;
  statusCode: number;
}

interface ErrorResponse {
  success: false;
  error: string;
  code: string;
  statusCode: number;
  details?: unknown;
}

type ApiResult<T> = SuccessResponse<T> | ErrorResponse;
```

### Generic Interfaces for Data Structures

```ts
// Generic pagination
interface PaginatedResult<T> {
  data: T[];
  total: number;
  page: number;
  limit: number;
  totalPages: number;
  hasNextPage: boolean;
  hasPrevPage: boolean;
}

// Generic repository interface
interface IRepository<T, ID = string> {
  findById(id: ID): Promise<T | null>;
  findAll(params?: QueryParams): Promise<PaginatedResult<T>>;
  create(data: Omit<T, "id" | "createdAt" | "updatedAt">): Promise<T>;
  update(id: ID, data: Partial<T>): Promise<T | null>;
  delete(id: ID): Promise<boolean>;
}

// Strongly typed implementations
class UserRepository implements IRepository<User> {
  async findById(id: string): Promise<User | null> { ... }
  // ... all methods must match IRepository<User>
}

class ProductRepository implements IRepository<Product, number> {
  async findById(id: number): Promise<Product | null> { ... }
  // ID is number here
}
```

---

## 4. Generic Classes

```ts
class DataStore<T> {
  private items: Map<string, T> = new Map();

  set(key: string, value: T): void {
    this.items.set(key, value);
  }

  get(key: string): T | undefined {
    return this.items.get(key);
  }

  getOrThrow(key: string): T {
    const value = this.items.get(key);
    if (value === undefined) throw new Error(`Key "${key}" not found`);
    return value;
  }

  delete(key: string): boolean {
    return this.items.delete(key);
  }

  has(key: string): boolean {
    return this.items.has(key);
  }

  entries(): [string, T][] {
    return [...this.items.entries()];
  }

  size(): number {
    return this.items.size;
  }
}

const userStore = new DataStore<User>();
userStore.set("u1", {
  id: "u1",
  name: "Alice",
  email: "alice@example.com",
  role: "admin",
});
const user = userStore.get("u1"); // User | undefined — fully typed

const configStore = new DataStore<string>();
configStore.set("apiUrl", "https://api.example.com");
const url = configStore.get("apiUrl"); // string | undefined
```

### Generic Stack (Classic Example)

```ts
class Stack<T> {
  private items: T[] = [];

  push(item: T): void {
    this.items.push(item);
  }

  pop(): T | undefined {
    return this.items.pop();
  }

  peek(): T | undefined {
    return this.items[this.items.length - 1];
  }

  isEmpty(): boolean {
    return this.items.length === 0;
  }

  size(): number {
    return this.items.length;
  }
}

const numberStack = new Stack<number>();
numberStack.push(1);
numberStack.push(2);
numberStack.pop(); // number | undefined — TypeScript knows it's a number

const requestStack = new Stack<HttpRequest>();
requestStack.push(req);
requestStack.peek(); // HttpRequest | undefined
```

---

## 5. Generic Type Aliases

```ts
// Generic type alias
type Maybe<T> = T | null | undefined;
type Nullable<T> = T | null;
type Optional<T> = T | undefined;

// Result type (like Rust's Result<T, E>)
type Result<T, E extends Error = Error> =
  { ok: true; value: T } | { ok: false; error: E };

// Usage
async function fetchUser(id: string): Promise<Result<User>> {
  try {
    const user = await db.users.findById(id);
    if (!user) return { ok: false, error: new NotFoundError("User not found") };
    return { ok: true, value: user };
  } catch (err) {
    return { ok: false, error: err as Error };
  }
}

const result = await fetchUser("u1");
if (result.ok) {
  console.log(result.value.name); // ✓ — TypeScript knows value is User
} else {
  console.error(result.error.message); // ✓ — TypeScript knows error is Error
}
```

---

## 6. Generic Constraints

Constraints limit which types can be used as a type parameter, letting you safely access properties on `T`.

### `extends` Constraint

```ts
// Without constraint — can't access .length (not all types have it)
function logLength<T>(value: T): void {
  console.log(value.length); // TS Error: Property 'length' does not exist on type 'T'
}

// With constraint — T must have a .length property
function logLength<T extends { length: number }>(value: T): void {
  console.log(value.length); // ✓ — T is guaranteed to have length
}

logLength("hello"); // ✓ — string has length
logLength([1, 2, 3]); // ✓ — array has length
logLength({ length: 5 }); // ✓ — has length property
logLength(42); // TS Error — number has no length
```

### `keyof` Constraint

```ts
// T is an object type, K must be a key of T
function getProperty<T, K extends keyof T>(obj: T, key: K): T[K] {
  return obj[key];
}

const user = { id: 1, name: "Alice", email: "alice@example.com" };

getProperty(user, "name"); // string — TypeScript infers return type!
getProperty(user, "id"); // number
getProperty(user, "missing"); // TS Error: "missing" is not a key of user
```

### Constraints in Real Code

```ts
// Merge two objects, ensuring no key conflicts
function merge<T extends object, U extends object>(obj1: T, obj2: U): T & U {
  return { ...obj1, ...obj2 };
}

// Type-safe object transformation
function mapValues<T extends object, V>(
  obj: T,
  fn: (value: T[keyof T], key: keyof T) => V,
): Record<keyof T, V> {
  return Object.fromEntries(
    Object.entries(obj).map(([k, v]) => [k, fn(v, k as keyof T)]),
  ) as Record<keyof T, V>;
}

// Omit with type safety
function omit<T extends object, K extends keyof T>(
  obj: T,
  ...keys: K[]
): Omit<T, K> {
  const keysToOmit = new Set(keys);
  return Object.fromEntries(
    Object.entries(obj).filter(([k]) => !keysToOmit.has(k as K)),
  ) as Omit<T, K>;
}

const user = {
  id: 1,
  name: "Alice",
  password: "secret",
  email: "alice@example.com",
};
const safe = omit(user, "password");
// safe: { id: number; name: string; email: string }
safe.password; // TS Error — correctly removed from type!
```

---

## 7. Default Generic Parameters

```ts
// T defaults to `unknown` if not specified
interface ApiResponse<T = unknown> {
  data: T;
  statusCode: number;
  message: string;
}

// Can use without specifying T
const response: ApiResponse = {
  data: "anything",
  statusCode: 200,
  message: "OK",
};

// Or specify T for full type safety
const userResponse: ApiResponse<User> = {
  data: { id: "1", name: "Alice", email: "a@a.com", role: "user" },
  statusCode: 200,
  message: "OK",
};

// Default to array
interface PaginatedResponse<T, Meta = { page: number; total: number }> {
  items: T[];
  meta: Meta;
}

type UserPage = PaginatedResponse<User>;
// { items: User[]; meta: { page: number; total: number } }
```

---

## 8. Conditional Types

Conditional types choose between types based on a condition:

```ts
// T extends U ? TrueType : FalseType
type IsString<T> = T extends string ? true : false;

type A = IsString<string>; // true
type B = IsString<number>; // false

// NonNullable — built into TS but let's see how it works
type NonNullable<T> = T extends null | undefined ? never : T;

type C = NonNullable<string | null | undefined>;
// string | null | undefined extends null → never
// string | null | undefined extends undefined → never
// string | null | undefined extends string → string
// = string

// Extract types from a union
type Extract<T, U> = T extends U ? T : never;
type Exclude<T, U> = T extends U ? never : T;

type Strings = Extract<string | number | boolean, string>; // string
type NoStrings = Exclude<string | number | boolean, string>; // number | boolean

// Infer — extract type from within another type
type UnpackPromise<T> = T extends Promise<infer U> ? U : T;

type A = UnpackPromise<Promise<User>>; // User
type B = UnpackPromise<Promise<string>>; // string
type C = UnpackPromise<number>; // number (not a Promise, returns as-is)

// Unpack array
type UnpackArray<T> = T extends Array<infer U> ? U : T;
type D = UnpackArray<User[]>; // User
```

---

## 9. Mapped Types

Mapped types create new types by transforming each property in an existing type:

```ts
// Syntax: { [K in keyof T]: TransformedType }

// Make all properties optional (like Partial<T>)
type MyPartial<T> = {
  [K in keyof T]?: T[K];
};

// Make all properties required (like Required<T>)
type MyRequired<T> = {
  [K in keyof T]-?: T[K]; // `-?` removes the optional modifier
};

// Make all properties readonly
type MyReadonly<T> = {
  readonly [K in keyof T]: T[K];
};

// Transform all values to a different type
type Stringify<T> = {
  [K in keyof T]: string;
};

type StringUser = Stringify<User>;
// { id: string; name: string; email: string; role: string; ... }
```

### Real-World Mapped Types

```ts
// Validation schema — for each property, define a validator
type ValidationSchema<T> = {
  [K in keyof T]: {
    required?: boolean;
    validate?: (value: T[K]) => boolean;
    errorMessage?: string;
  };
};

type UserValidation = ValidationSchema<CreateUserDTO>;
// {
//   name: { required?: boolean; validate?: (v: string) => boolean; ... }
//   email: { required?: boolean; validate?: (v: string) => boolean; ... }
//   ...
// }

const userValidation: UserValidation = {
  name: { required: true, validate: (v) => v.length >= 2 },
  email: { required: true, validate: (v) => v.includes("@") },
  password: { required: true, validate: (v) => v.length >= 8 },
};

// Form field metadata — for each property, define UI config
type FormFields<T> = {
  [K in keyof T]: {
    label: string;
    type: "text" | "email" | "password" | "number" | "select";
    placeholder?: string;
    options?: string[];
  };
};
```

---

## 10. Template Literal Types

TypeScript can create types from string patterns:

```ts
type EventName = `on${string}`;
type ClickEvent = `on${"Click" | "Tap" | "Press"}`; // "onClick" | "onTap" | "onPress"

// Generate CRUD method names for any resource
type Resource = "user" | "product" | "order";
type CrudMethod = "get" | "create" | "update" | "delete" | "list";
type ApiMethod = `${CrudMethod}${Capitalize<Resource>}`;
// "getUser" | "createUser" | "updateUser" | "deleteUser" | "listUser" |
// "getProduct" | ... | "listOrder"

// CSS property types
type CSSProperty = `${string}-${string}`;
type ThemeVariable = `--${string}`;

// Event listener naming convention
type EventListenerMap<T extends string> = {
  [K in T as `on${Capitalize<K>}`]: (event: Event) => void;
};
type ClickHandlers = EventListenerMap<"click" | "hover" | "focus">;
// { onClick: ...; onHover: ...; onFocus: ... }
```

---

## 11. Utility Types (Built-In Generics)

TypeScript ships with a full library of utility types. Here are all the important ones with real examples:

```ts
type User = {
  id: string;
  name: string;
  email: string;
  password: string;
  role: "admin" | "user";
  isActive: boolean;
  createdAt: Date;
  updatedAt: Date;
};

// --- OBJECT TRANSFORMATION ---

// Partial<T> — all props optional
type UpdateUserDTO = Partial<User>;
// All fields optional — use for PATCH requests

// Required<T> — all props required
type FullUser = Required<Partial<User>>;
// Back to all required

// Pick<T, Keys> — select specific keys
type UserListItem = Pick<User, "id" | "name" | "email" | "role">;

// Omit<T, Keys> — remove specific keys
type PublicUser = Omit<User, "password">;
type CreateUserDTO = Omit<User, "id" | "createdAt" | "updatedAt"> & {
  password: string;
};

// Readonly<T> — make all props readonly
type ImmutableUser = Readonly<User>;

// Record<K, V> — dictionary type
type PermissionMap = Record<User["role"], string[]>;
// { admin: string[]; user: string[] }

// --- SET OPERATIONS ---

// Extract<T, U> — types in T that are assignable to U
type NumberTypes = Extract<string | number | boolean, number>; // number

// Exclude<T, U> — types in T that are NOT assignable to U
type NonNull<T> = Exclude<T, null | undefined>;

// NonNullable<T> — removes null and undefined
type SureString = NonNullable<string | null | undefined>; // string

// --- FUNCTION TYPES ---

// ReturnType<T> — return type of a function
type GetUserReturn = ReturnType<typeof getUser>; // Promise<User | null>

// Parameters<T> — parameter types as tuple
type GetUserParams = Parameters<typeof getUser>; // [id: string]

// ConstructorParameters<T> — constructor param types
type UserCtorParams = ConstructorParameters<typeof User>; // [name: string, email: string]

// InstanceType<T> — instance type of a constructor
type UserInstance = InstanceType<typeof User>; // User

// Awaited<T> — unwraps Promise recursively
type ResolvedUser = Awaited<Promise<Promise<User>>>; // User

// --- STRING MANIPULATION ---
type T1 = Uppercase<"hello">; // "HELLO"
type T2 = Lowercase<"HELLO">; // "hello"
type T3 = Capitalize<"hello">; // "Hello"
type T4 = Uncapitalize<"Hello">; // "hello"
```

---

## 12. Type Narrowing Overview

**Type narrowing** is TypeScript's ability to refine a broad type to a more specific one based on runtime checks. TypeScript analyzes your code's control flow and adjusts what it knows about a variable.

```ts
function process(value: string | number | null | undefined) {
  // Here: value is string | number | null | undefined

  if (value == null) {
    // Here: value is null | undefined (== checks both)
    return;
  }

  // Here: value is string | number (null/undefined eliminated)

  if (typeof value === "string") {
    // Here: value is string
    console.log(value.toUpperCase()); // ✓
    return;
  }

  // Here: value is number (string was handled above)
  console.log(value.toFixed(2)); // ✓
}
```

---

## 13. `typeof` Guard

```ts
// typeof can narrow: "string" | "number" | "boolean" | "bigint" | "symbol" | "undefined" | "function" | "object"

function formatValue(value: string | number | boolean | null): string {
  if (typeof value === "string") {
    return value.toUpperCase(); // value: string
  }
  if (typeof value === "number") {
    return value.toFixed(2); // value: number
  }
  if (typeof value === "boolean") {
    return value ? "Yes" : "No"; // value: boolean
  }
  // value: null (all other types handled)
  return "N/A";
}

// Note: typeof null === "object" — classic JS gotcha
// Always check for null separately before checking for "object"
function processObject(value: unknown): void {
  if (typeof value === "object" && value !== null) {
    // value: object (not null)
    console.log(Object.keys(value));
  }
}
```

---

## 14. `instanceof` Guard

`instanceof` narrows to a specific class type:

```ts
class ApiError extends Error {
  constructor(
    message: string,
    public statusCode: number,
  ) {
    super(message);
    this.name = "ApiError";
  }
}

class NetworkError extends Error {
  constructor(
    message: string,
    public url: string,
  ) {
    super(message);
    this.name = "NetworkError";
  }
}

function handleError(err: unknown): void {
  if (err instanceof ApiError) {
    // err: ApiError — can access err.statusCode
    console.error(`API Error ${err.statusCode}: ${err.message}`);
    if (err.statusCode === 401) redirectToLogin();
  } else if (err instanceof NetworkError) {
    // err: NetworkError — can access err.url
    console.error(`Network error fetching ${err.url}: ${err.message}`);
    showOfflineMessage();
  } else if (err instanceof Error) {
    // err: Error — generic error
    console.error(`Unexpected error: ${err.message}`);
  } else {
    // err: unknown — not even an Error object
    console.error("Unknown error:", err);
  }
}
```

---

## 15. Truthiness Narrowing

TypeScript narrows based on whether a value is truthy or falsy:

```ts
function greet(name: string | null | undefined): string {
  if (name) {
    // name: string (null and undefined are falsy, filtered out)
    return `Hello, ${name}`;
  }
  return "Hello, stranger";
}

// Be careful: empty string "" is also falsy
function processName(name: string | null): string {
  if (name) {
    // name: string BUT "" would never reach here
    return name.toUpperCase();
  }
  return "Anonymous";
}

// Safer — explicitly check for null/undefined
function processName(name: string | null): string {
  if (name !== null) {
    return name.toUpperCase(); // name: string (including "")
  }
  return "Anonymous";
}

// Practical use: optional callback
function doWork(callback?: () => void): void {
  // ...work
  if (callback) {
    callback(); // callback: () => void (narrowed from (() => void) | undefined)
  }
}
```

---

## 16. Equality Narrowing

TypeScript narrows on `===`, `!==`, `==`, and `!=`:

```ts
function processStatus(status: "active" | "inactive" | "banned" | null): void {
  if (status === null) {
    // status: null
    return;
  }
  // status: "active" | "inactive" | "banned"

  if (status === "banned") {
    // status: "banned"
    showBanMessage();
    return;
  }
  // status: "active" | "inactive"

  if (status === "active") {
    // status: "active"
    showDashboard();
  } else {
    // status: "inactive" (must be, all others handled)
    showInactiveMessage();
  }
}

// Loose equality check: == null catches both null AND undefined
function handleValue(value: string | null | undefined): void {
  if (value == null) {
    // value: null | undefined
    return;
  }
  // value: string
  console.log(value.toUpperCase());
}
```

---

## 17. `in` Operator Narrowing

The `in` operator checks if a property exists on an object:

```ts
type Cat = { meow(): void; purr(): void };
type Dog = { bark(): void; fetch(): void };
type Pet = Cat | Dog;

function makePetSound(pet: Pet): void {
  if ("meow" in pet) {
    // pet: Cat (has meow property)
    pet.meow();
    pet.purr();
  } else {
    // pet: Dog (doesn't have meow)
    pet.bark();
  }
}

// Real-world: different API response shapes
type SuccessResponse = { data: User; requestId: string };
type ErrorResponse = { error: string; code: string; statusCode: number };
type ApiResponse = SuccessResponse | ErrorResponse;

function handleResponse(response: ApiResponse): void {
  if ("data" in response) {
    // response: SuccessResponse
    renderUser(response.data);
  } else {
    // response: ErrorResponse
    showError(response.error, response.statusCode);
  }
}
```

---

## 18. Discriminated Union Narrowing

The most powerful and clean narrowing pattern — a shared literal property ("discriminant") identifies each variant:

```ts
// Each variant has a unique `kind` (or `type`, `status`, etc.) field
type CircleShape = { kind: "circle"; radius: number };
type RectangleShape = { kind: "rectangle"; width: number; height: number };
type TriangleShape = { kind: "triangle"; base: number; height: number };

type Shape = CircleShape | RectangleShape | TriangleShape;

function calculateArea(shape: Shape): number {
  switch (shape.kind) {
    case "circle":
      // shape: CircleShape — TypeScript knows radius exists
      return Math.PI * shape.radius ** 2;

    case "rectangle":
      // shape: RectangleShape — TypeScript knows width and height exist
      return shape.width * shape.height;

    case "triangle":
      // shape: TriangleShape — TypeScript knows base and height exist
      return 0.5 * shape.base * shape.height;

    default:
      const _exhaustive: never = shape; // catches missing cases
      throw new Error("Unknown shape");
  }
}
```

### Real-World: Async State Machine

```ts
type ApiState<T> =
  | { status: "idle" }
  | { status: "loading" }
  | { status: "success"; data: T; fetchedAt: Date }
  | { status: "error"; error: Error; retryCount: number };

function renderComponent<T>(state: ApiState<T>): void {
  switch (state.status) {
    case "idle":
      return renderEmpty();

    case "loading":
      return renderSpinner();
    // state.data — TS Error: doesn't exist on loading state

    case "success":
      return renderData(state.data); // ✓ — data exists here
    // state.fetchedAt              // ✓ — fetchedAt exists here

    case "error":
      return renderError(state.error.message, state.retryCount); // ✓

    default:
      const _: never = state;
      throw new Error("Unhandled state");
  }
}
```

---

## 19. User-Defined Type Guards

A **type guard** is a function that returns a **type predicate** — `value is SomeType`. When it returns `true`, TypeScript narrows the type.

```ts
// Syntax: parameter is Type
function isString(value: unknown): value is string {
  return typeof value === "string";
}

function isUser(value: unknown): value is User {
  return (
    typeof value === "object" &&
    value !== null &&
    "id" in value &&
    "name" in value &&
    "email" in value
  );
}

// Usage
function process(data: unknown): void {
  if (isUser(data)) {
    // data: User — fully typed!
    console.log(data.name.toUpperCase());
    data.nonExistentProp; // TS Error — TypeScript knows the User shape
  }
}
```

### Type Guards for Arrays

```ts
// Filter array and narrow type at the same time
function isNonNull<T>(value: T | null | undefined): value is T {
  return value !== null && value !== undefined;
}

const maybeUsers: (User | null | undefined)[] = [user1, null, user2, undefined];
const users: User[] = maybeUsers.filter(isNonNull);
// TypeScript knows users is User[] — no null/undefined!

// Without a type guard, TypeScript would give (User | null | undefined)[]
const also: (User | null | undefined)[] = maybeUsers.filter((u) => u !== null);
// filter doesn't narrow automatically — you need a type predicate function
```

### Complex Type Guards

```ts
// Validate and narrow an API response
interface CreateUserResponse {
  id: string;
  name: string;
  email: string;
  role: "admin" | "user";
  createdAt: string;
}

function isCreateUserResponse(data: unknown): data is CreateUserResponse {
  if (typeof data !== "object" || data === null) return false;
  const d = data as Record<string, unknown>;
  return (
    typeof d.id === "string" &&
    typeof d.name === "string" &&
    typeof d.email === "string" &&
    (d.role === "admin" || d.role === "user") &&
    typeof d.createdAt === "string"
  );
}

async function createUser(dto: CreateUserDTO): Promise<CreateUserResponse> {
  const response = await fetch("/api/users", {
    method: "POST",
    body: JSON.stringify(dto),
  });
  const data: unknown = await response.json();

  if (!isCreateUserResponse(data)) {
    throw new Error("Invalid API response shape");
  }

  return data; // CreateUserResponse — fully typed
}
```

---

## 20. Assertion Functions

Assertion functions throw if a condition is false and narrow the type if they don't throw:

```ts
// `asserts condition` — after this call, condition is guaranteed true
function assert(condition: boolean, message: string): asserts condition {
  if (!condition) throw new Error(`Assertion failed: ${message}`);
}

// `asserts value is T` — after this call, value is guaranteed to be T
function assertDefined<T>(
  value: T | null | undefined,
  message: string,
): asserts value is T {
  if (value == null) throw new Error(message);
}

function assertUser(value: unknown): asserts value is User {
  if (!isUser(value)) throw new TypeError("Expected a User object");
}

// Usage
const userId = req.params.id;
assert(userId.length > 0, "userId must not be empty");
// TypeScript now knows: userId is string (still) AND length > 0

const user = await userRepo.findById(userId);
assertDefined(user, `User "${userId}" not found`);
// TypeScript now knows: user is User (not null/undefined)

user.name; // ✓ — no need for null check, assert already handles it

// Real-world: NestJS-style guard in service
async function getAuthenticatedUser(token: string): Promise<User> {
  const payload = verifyToken(token);
  assertDefined(payload, "Invalid token");

  const user = await userRepo.findById(payload.sub);
  assertDefined(user, "Token user not found");

  assert(user.isActive, "User account is deactivated");

  return user; // TypeScript knows: User (not null/undefined)
}
```

---

## Summary Cheat Sheet

```
Generics:
  <T>                         — type parameter (placeholder)
  fn<T>(arg: T): T            — type inferred from argument
  fn<T extends object>(...)   — constraint: T must be an object
  fn<T, K extends keyof T>    — K must be a key of T
  fn<T = string>(...)         — default type parameter

Generic utilities:
  Partial<T>      — all props optional
  Required<T>     — all props required
  Pick<T, Keys>   — select keys
  Omit<T, Keys>   — remove keys
  Readonly<T>     — immutable
  Record<K, V>    — dictionary
  ReturnType<F>   — function return type
  Parameters<F>   — function param types
  Awaited<T>      — unwrap Promise

Advanced types:
  Conditional:    T extends U ? A : B
  Mapped:         { [K in keyof T]: Transform }
  Template:       `on${Capitalize<string>}`
  infer:          T extends Promise<infer U> ? U : T

Type Narrowing — TypeScript refines types in branches:
  typeof x === "string"          — primitive type check
  x instanceof MyClass           — class instance check
  if (x)                         — truthiness (removes null/undefined/0/"")
  x === "literal"                — equality narrows to that literal
  "prop" in x                    — property existence
  switch(x.kind)                 — discriminated union (best pattern)

Type guards:
  function isUser(v): v is User  — user-defined, returns boolean
  Use with .filter(isNonNull) to narrow array element types

Assertion functions:
  asserts condition              — throws if false, narrows after call
  asserts value is T             — throws if not T, narrows to T after call
```

---

_Next: [Part 5 — Real-World TypeScript: Config, Declaration Files, Axios & Express](./ts-part-5-real-world.md)_
