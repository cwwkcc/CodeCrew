> **Part 2 of 5.** The full TypeScript type system: arrays, objects, type aliases, optional & readonly properties, intersection types, union types, literal types, tuples, and enums — all with deep real-world examples.

---

## Table of Contents

1. [Array Types In Depth](#1-array-types-in-depth)
2. [Multi-Dimensional Arrays](#2-multi-dimensional-arrays)
3. [Objects In Depth](#3-objects-in-depth)
4. [Type Aliases](#4-type-aliases)
5. [Optional Properties](#5-optional-properties)
6. [Readonly Property](#6-readonly-property)
7. [Intersection Types](#7-intersection-types)
8. [Union Types](#8-union-types)
9. [Literal Types](#9-literal-types)
10. [Tuples](#10-tuples)
11. [Enums](#11-enums)

---

## 1. Array Types In Depth

TypeScript has two equivalent syntaxes for array types:

```ts
// Syntax 1: Type[] — most common, preferred for simple types
let scores: number[] = [95, 87, 92];
let names: string[] = ["Alice", "Bob", "Carol"];
let flags: boolean[] = [true, false, true];

// Syntax 2: Array<Type> — generic syntax, needed for complex types sometimes
let scores: Array<number> = [95, 87, 92];
let users: Array<{ id: number; name: string }> = [];
```

### Array Type Inference

```ts
// TypeScript infers array type from the initial values
const numbers = [1, 2, 3]; // number[]
const mixed = [1, "hello", true]; // (number | string | boolean)[]
const empty = []; // never[] — will widen as you push values

// Best practice: annotate empty arrays explicitly
const users: User[] = [];
const errors: string[] = [];
```

### Working with Typed Arrays

```ts
const scores: number[] = [88, 92, 76, 95, 84];

// All array methods are fully typed
scores.push(90); // OK — number
scores.push("90"); // TS Error: string not assignable to number

const highest = scores.reduce((max, s) => Math.max(max, s), 0); // number
const passed = scores.filter((s) => s >= 75); // number[]
const letter = scores.map((s) => (s >= 90 ? "A" : "B")); // string[]
const total = scores.reduce((sum, s) => sum + s, 0); // number

// TypeScript knows the element type
scores.forEach((score) => {
  score.toFixed(2); // ✓ — TypeScript knows score is number
  score.toUpperCase(); // TS Error — number has no toUpperCase
});
```

### Array of Objects

```ts
type User = {
  id: number;
  name: string;
  email: string;
  role: "admin" | "user";
};

const users: User[] = [
  { id: 1, name: "Alice", email: "alice@example.com", role: "admin" },
  { id: 2, name: "Bob",   email: "bob@example.com",   role: "user"  },
];

// TypeScript fully understands the element shape
const admin = users.find(u => u.role === "admin"); // User | undefined
const emails = users.map(u => u.email);            // string[]
const admins = users.filter(u => u.role === "admin"); // User[]

// Autocomplete works perfectly inside callbacks
users.forEach(u => {
  u.    // IDE suggests: id, name, email, role
});
```

### Readonly Arrays

```ts
// Array whose contents cannot be mutated
const ALLOWED_ROLES: readonly string[] = ["admin", "user", "moderator"];
// OR: ReadonlyArray<string>
const ALLOWED_ROLES: ReadonlyArray<string> = ["admin", "user", "moderator"];

ALLOWED_ROLES.push("superadmin"); // TS Error: push does not exist on readonly array
ALLOWED_ROLES[0] = "root"; // TS Error: Index signature in type 'readonly string[]' only permits reading

// Real-world: configuration arrays that should never change
const HTTP_METHODS = ["GET", "POST", "PUT", "PATCH", "DELETE"] as const;
// type: readonly ["GET", "POST", "PUT", "PATCH", "DELETE"]
type HttpMethod = (typeof HTTP_METHODS)[number]; // "GET" | "POST" | "PUT" | "PATCH" | "DELETE"
```

### Typed Array Methods Return Types

```ts
const users: User[] = [...];

users.find(u => u.id === 1);         // User | undefined
users.findIndex(u => u.id === 1);    // number
users.filter(u => u.role === "admin"); // User[]
users.map(u => u.name);              // string[]
users.some(u => u.role === "admin"); // boolean
users.every(u => u.isActive);        // boolean
users.reduce<Record<number, User>>((acc, u) => {
  acc[u.id] = u;
  return acc;
}, {}); // Record<number, User>

// Sorting — comparator is typed
users.sort((a, b) => a.name.localeCompare(b.name)); // User[]
```

---

## 2. Multi-Dimensional Arrays

A multi-dimensional array is simply an array of arrays.

```ts
// 2D array — matrix of numbers
const matrix: number[][] = [
  [1, 2, 3],
  [4, 5, 6],
  [7, 8, 9],
];

matrix[0][1]; // 2 — TypeScript knows this is a number

// 2D array with Array<> syntax
const grid: Array<Array<number>> = [
  [0, 1],
  [1, 0],
];

// Typed 2D array operations
const flattened: number[] = matrix.flat(); // number[]
const transposed: number[][] = matrix[0].map((_, colIndex) =>
  matrix.map((row) => row[colIndex]),
); // number[][]
```

### Real-World: Weekly Schedule Data

```ts
type TimeSlot = {
  hour: number;
  isBooked: boolean;
  bookingId?: string;
};

type DaySchedule = TimeSlot[];
type WeeklySchedule = DaySchedule[]; // 7 days × N time slots

const schedule: WeeklySchedule = Array.from({ length: 7 }, () =>
  Array.from({ length: 24 }, (_, hour) => ({
    hour,
    isBooked: false,
  })),
);

// Access a specific time slot — fully typed
const mondayAt9am: TimeSlot = schedule[0][9];
mondayAt9am.isBooked; // boolean
mondayAt9am.bookingId; // string | undefined
```

### 3D Arrays

```ts
// Rarely needed, but TypeScript handles them fine
type Voxel = { density: number; material: string };
const voxelGrid: Voxel[][][] = []; // 3D space of voxels

// Dashboard data: [week][day][hour] = sales
const salesData: number[][][] = Array.from({ length: 4 }, () =>
  Array.from({ length: 7 }, () => Array.from({ length: 24 }, () => 0)),
);
salesData[0][1][14]; // Week 1, Tuesday, 2pm — number ✓
```

---

## 3. Objects In Depth

TypeScript allows you to describe the exact shape of an object with all property types.

### Inline Object Type

```ts
// Annotating a variable with an inline object type
let user: {
  id: number;
  name: string;
  email: string;
  isActive: boolean;
} = {
  id: 1,
  name: "Alice",
  email: "alice@example.com",
  isActive: true,
};

user.name; // string
user.isActive; // boolean
user.role; // TS Error: Property 'role' does not exist on type '...'
user.name = 42; // TS Error: number not assignable to string
```

### Excess Property Checking

TypeScript checks for extra properties when you assign object literals directly:

```ts
type Point = { x: number; y: number };

const p: Point = { x: 1, y: 2, z: 3 };
// TS Error: Object literal may only specify known properties,
// and 'z' does not exist in type 'Point'

// But NOT when assigned through a variable (structural typing)
const point3D = { x: 1, y: 2, z: 3 };
const p: Point = point3D; // ✓ — point3D has AT LEAST the required properties
```

### Nested Objects

```ts
type Address = {
  street: string;
  city: string;
  country: string;
  postalCode: string;
};

type UserProfile = {
  id: number;
  name: string;
  email: string;
  address: Address; // nested object type
  preferences: {
    // inline nested type
    theme: "light" | "dark";
    language: string;
    notifications: boolean;
  };
};

const profile: UserProfile = {
  id: 1,
  name: "Alice",
  email: "alice@example.com",
  address: {
    street: "123 Main St",
    city: "New York",
    country: "US",
    postalCode: "10001",
  },
  preferences: {
    theme: "dark",
    language: "en",
    notifications: true,
  },
};

profile.address.city; // string
profile.preferences.theme; // "light" | "dark"
profile.preferences.theme = "dark"; // ✓
profile.preferences.theme = "blue"; // TS Error — "blue" not in union
```

### Index Signatures

When the keys aren't known in advance but the value types are consistent:

```ts
// All keys are strings, all values are numbers
type ScoreMap = {
  [subject: string]: number;
};

const grades: ScoreMap = {
  math: 95,
  english: 87,
  science: 92,
};

grades["history"] = 88; // ✓
grades["math"] = "A"; // TS Error: string not assignable to number

// Mixed: some known keys + index signature
type Config = {
  debug: boolean; // specific known key
  version: string; // specific known key
  [key: string]: unknown; // any additional keys must be unknown
  // Note: `unknown` because specific props (debug: boolean) must be
  // assignable to the index signature value type
};

// Real-world: API response with dynamic field names
type MetricsResponse = {
  timestamp: string;
  [metricName: string]: string | number;
};
```

### `Record<Keys, Values>` — Typed Dictionary

A cleaner way to write index signatures when keys come from a specific set:

```ts
// Record<KeyType, ValueType>
type RolePermissions = Record<string, boolean>;
const perms: RolePermissions = {
  read: true,
  write: false,
  delete: false,
};

// Better: restrict keys with union
type Permission = "read" | "write" | "delete" | "admin";
type RolePermissions = Record<Permission, boolean>;

const adminPerms: RolePermissions = {
  read: true,
  write: true,
  delete: true,
  admin: true,
  // Must include ALL keys — TypeScript errors if any are missing!
};

// Real-world: mapping status codes to messages
const HTTP_MESSAGES: Record<number, string> = {
  200: "OK",
  201: "Created",
  400: "Bad Request",
  401: "Unauthorized",
  404: "Not Found",
  500: "Internal Server Error",
};
```

---

## 4. Type Aliases

A **type alias** gives a name to any type. This is the primary tool for creating reusable, named types in TypeScript.

```ts
// Syntax: type AliasName = TypeExpression
type UserID = number;
type Username = string;
type IsActive = boolean;

// Much more useful for complex types
type User = {
  id: UserID;
  name: Username;
  email: string;
  role: "admin" | "user" | "moderator";
  isActive: IsActive;
  createdAt: Date;
  address?: Address; // optional (see next section)
};

// Arrays
type UserList = User[];
type IDList = UserID[];

// Function types
type EventHandler<T = Event> = (event: T) => void;
type AsyncHandler<TInput, TOutput> = (input: TInput) => Promise<TOutput>;

// Usage
const createUser: AsyncHandler<CreateUserDTO, User> = async (dto) => {
  // dto is CreateUserDTO, return type is Promise<User>
};
```

### Type Aliases vs Interfaces

Both can describe object shapes. Key differences:

|                     | Type Alias             | Interface                           |
| ------------------- | ---------------------- | ----------------------------------- |
| Object shapes       | ✓                      | ✓                                   |
| Primitives          | ✓ (`type ID = string`) | ✗                                   |
| Unions              | ✓                      | ✗                                   |
| Tuples              | ✓                      | ✗                                   |
| Extends/implements  | ✓ (intersection `&`)   | ✓ (`extends`)                       |
| Declaration merging | ✗                      | ✓ (can define same interface twice) |
| Error messages      | Less clear sometimes   | Cleaner in errors                   |

**Recommendation:**

- Use `interface` for object shapes that will be extended (classes, OOP hierarchies)
- Use `type` for unions, primitives, tuples, mapped types, and computed types
- Covered in full in Part 3 (Interfaces)

### Utility Type Aliases

TypeScript has built-in generic utility types that build on type aliases:

```ts
type User = {
  id: number;
  name: string;
  email: string;
  password: string;
  role: "admin" | "user";
  createdAt: Date;
};

// Partial<T> — make all properties optional
type UpdateUserDTO = Partial<User>;
// { id?: number; name?: string; email?: string; ... }

// Required<T> — make all properties required
type FullUser = Required<UpdateUserDTO>; // back to original

// Pick<T, Keys> — select specific properties
type PublicUser = Pick<User, "id" | "name" | "email" | "role">;
// { id: number; name: string; email: string; role: "admin" | "user" }

// Omit<T, Keys> — remove specific properties
type SafeUser = Omit<User, "password">;
// User without `password` — use this for API responses!

// Readonly<T> — make all properties readonly
type ImmutableUser = Readonly<User>;

// ReturnType<T> — extract return type from a function type
type UserFinderReturn = ReturnType<typeof findUser>; // User | null

// Parameters<T> — extract parameter types as tuple
type FindUserParams = Parameters<typeof findUser>; // [id: number]

// Awaited<T> — unwrap Promise type
type ResolvedUser = Awaited<Promise<User>>; // User
```

### Real-World DTO Patterns

```ts
type User = {
  id: string;
  name: string;
  email: string;
  passwordHash: string;
  role: "admin" | "user";
  isActive: boolean;
  createdAt: Date;
  updatedAt: Date;
};

// Create — omit server-generated fields
type CreateUserDTO = Omit<
  User,
  "id" | "passwordHash" | "createdAt" | "updatedAt"
> & {
  password: string; // plain text — will be hashed
};

// Update — all fields optional, can't change id/email
type UpdateUserDTO = Partial<
  Omit<User, "id" | "email" | "createdAt" | "updatedAt" | "passwordHash">
>;

// Response — never expose password hash
type UserResponseDTO = Omit<User, "passwordHash">;

// List item — lighter version for list views
type UserListItemDTO = Pick<
  User,
  "id" | "name" | "email" | "role" | "isActive"
>;
```

---

## 5. Optional Properties

A property marked with `?` is optional — its type becomes `T | undefined`.

```ts
type Product = {
  id: string;
  name: string;
  price: number;
  description?: string; // string | undefined
  discount?: number; // number | undefined
  imageUrl?: string; // string | undefined
  tags?: string[]; // string[] | undefined
};

const product: Product = {
  id: "p1",
  name: "Laptop",
  price: 999,
  // description, discount, imageUrl, tags all omitted — valid!
};

// TypeScript forces you to handle the undefined case
product.description.toUpperCase();
// TS Error: Object is possibly 'undefined'

// Fix with optional chaining
product.description?.toUpperCase(); // string | undefined

// Fix with nullish coalescing
const desc = product.description ?? "No description available"; // string

// Fix with type narrowing
if (product.description) {
  product.description.toUpperCase(); // ✓ — narrowed to string
}
```

### Optional vs `undefined` in Union

These look similar but behave differently:

```ts
type A = { x?: number }; // x can be omitted entirely
type B = { x: number | undefined }; // x must be present, but can be undefined

const a: A = {}; // ✓ — x not required
const b: B = {}; // TS Error — x is required (even if value is undefined)
const b: B = { x: undefined }; // ✓
```

### Optional Parameters in Functions

```ts
// Optional function params work the same way
function createUser(
  name: string,
  email: string,
  role?: "admin" | "user", // role?: "admin" | "user" means role is ("admin" | "user") | undefined
  metadata?: Record<string, unknown>,
): User {
  return {
    id: generateId(),
    name,
    email,
    role: role ?? "user", // default if not provided
    metadata: metadata ?? {},
    createdAt: new Date(),
  };
}
```

### Real-World: API Query Parameters

```ts
type UserQueryParams = {
  page?: number;
  limit?: number;
  search?: string;
  role?: "admin" | "user" | "moderator";
  isActive?: boolean;
  sortBy?: "name" | "email" | "createdAt";
  sortOrder?: "asc" | "desc";
  createdAfter?: Date;
  createdBefore?: Date;
};

async function getUsers(
  params: UserQueryParams = {},
): Promise<PaginatedResponse<User>> {
  const {
    page = 1,
    limit = 20,
    sortBy = "createdAt",
    sortOrder = "desc",
    ...filters
  } = params;

  const offset = (page - 1) * limit;

  return db.users.findMany({
    where: buildWhereClause(filters),
    orderBy: { [sortBy]: sortOrder },
    take: limit,
    skip: offset,
  });
}
```

---

## 6. Readonly Property

The `readonly` modifier prevents a property from being reassigned after initialization.

```ts
type User = {
  readonly id: string; // can never be changed
  readonly createdAt: Date; // can never be changed
  name: string; // can be changed
  email: string; // can be changed
};

const user: User = {
  id: "user_123",
  createdAt: new Date(),
  name: "Alice",
  email: "alice@example.com",
};

user.name = "Alice Chen"; // ✓ — not readonly
user.id = "user_456"; // TS Error: Cannot assign to 'id' because it is a read-only property
user.createdAt = new Date(); // TS Error: Cannot assign to 'createdAt'...
```

### Important: `readonly` Is Shallow

Like `Object.freeze`, `readonly` only prevents reassignment of the property itself — not mutation of the object it points to:

```ts
type Order = {
  readonly id: string;
  readonly items: OrderItem[]; // readonly array reference
};

const order: Order = { id: "o1", items: [] };

order.id = "o2"; // TS Error — can't reassign the reference
order.items = []; // TS Error — can't reassign the reference
order.items.push(item); // ✓ — mutating the ARRAY is allowed (shallow!)

// For deep immutability, use `as const` or deep readonly:
type DeepReadonly<T> = {
  readonly [K in keyof T]: T[K] extends object ? DeepReadonly<T[K]> : T[K];
};
```

### `readonly` in Classes

```ts
class User {
  readonly id: string;
  readonly createdAt: Date;
  name: string;
  email: string;

  constructor(id: string, name: string, email: string) {
    this.id = id; // ✓ — readonly can be set in constructor
    this.createdAt = new Date();
    this.name = name;
    this.email = email;
  }

  updateEmail(newEmail: string): void {
    this.email = newEmail; // ✓
    this.id = "new_id"; // TS Error — readonly after construction
  }
}
```

### `Readonly<T>` Utility Type

```ts
// Makes ALL properties readonly at once
const config: Readonly<ServerConfig> = {
  port: 3000,
  host: "localhost",
  debug: false,
};

config.port = 4000; // TS Error
```

---

## 7. Intersection Types

An **intersection type** combines multiple types into one — the result must satisfy ALL constituent types.

```ts
// Syntax: TypeA & TypeB
type Combined = TypeA & TypeB;
// A value of type Combined has ALL properties of TypeA AND ALL properties of TypeB
```

### Basic Intersection

```ts
type HasId = { id: string };
type HasTimestamps = { createdAt: Date; updatedAt: Date };
type HasSoftDelete = { deletedAt: Date | null };

// Every model gets these base fields
type BaseModel = HasId & HasTimestamps & HasSoftDelete;

type User = BaseModel & {
  name: string;
  email: string;
  role: "admin" | "user";
};

// User must have: id, createdAt, updatedAt, deletedAt, name, email, role
const user: User = {
  id: "u1",
  createdAt: new Date(),
  updatedAt: new Date(),
  deletedAt: null,
  name: "Alice",
  email: "alice@example.com",
  role: "admin",
};
```

### Intersection for Mixins / Capabilities

```ts
type Serializable = {
  serialize(): string;
  deserialize(data: string): void;
};

type Validatable = {
  validate(): boolean;
  getErrors(): string[];
};

type Loggable = {
  getLogEntry(): Record<string, unknown>;
};

// A form that has ALL three capabilities
type UserForm = {
  name: string;
  email: string;
  password: string;
} & Serializable &
  Validatable;

function processForm(form: UserForm): void {
  if (!form.validate()) {
    console.error(form.getErrors());
    return;
  }
  const serialized = form.serialize();
  saveToStorage(serialized);
}
```

### Intersection in Function Parameters

```ts
// Request augmented by middlewares
type Request = {
  url: string;
  method: string;
  headers: Record<string, string>;
  body: unknown;
};

type AuthenticatedRequest = Request & {
  user: { id: string; role: string };
};

type LoggedRequest = Request & {
  requestId: string;
  startTime: number;
};

// After both middlewares run
type ProcessedRequest = AuthenticatedRequest & LoggedRequest;

function handleApiRoute(req: ProcessedRequest): void {
  req.user.id; // ✓ — from AuthenticatedRequest
  req.requestId; // ✓ — from LoggedRequest
  req.method; // ✓ — from base Request
}
```

### Conflict in Intersections

If two types in an intersection have the same key with incompatible types, the result is `never`:

```ts
type A = { value: string };
type B = { value: number };
type C = A & B;
// C.value is `string & number` = `never`
// No value can be both string AND number

const c: C = { value: "hello" }; // TS Error — value is never
```

---

## 8. Union Types

A **union type** means a value can be **one of several types**. It's the most frequently used compound type in TypeScript.

```ts
// Syntax: TypeA | TypeB
type StringOrNumber = string | number;

let id: StringOrNumber = 1;
id = "user_abc"; // also valid

function formatId(id: string | number): string {
  return String(id);
}
```

### Discriminated Unions — The Killer Feature

A discriminated union is a union of object types where each type has a **common literal property** (the "discriminant") that uniquely identifies it:

```ts
type LoadingState = {
  status: "loading"; // ← discriminant
};

type SuccessState<T> = {
  status: "success"; // ← discriminant
  data: T;
};

type ErrorState = {
  status: "error"; // ← discriminant
  error: string;
  code: number;
};

type AsyncState<T> = LoadingState | SuccessState<T> | ErrorState;

// TypeScript narrows based on the discriminant
function renderUserState(state: AsyncState<User>): string {
  switch (state.status) {
    case "loading":
      return "Loading...";
    case "success":
      return `Hello, ${state.data.name}`; // TypeScript knows data exists here
    case "error":
      return `Error ${state.code}: ${state.error}`; // TypeScript knows error and code exist
    // With `never` exhaustiveness check:
    default:
      const _: never = state; // ensure all cases handled
      throw new Error("Unknown state");
  }
}
```

### Union with Narrowing

TypeScript narrows union types based on checks:

```ts
function processInput(input: string | number | null): string {
  // input is `string | number | null` here

  if (input === null) {
    return "No input";
    // Below this: input is still `string | number` (TypeScript knows null branch returned)
  }

  if (typeof input === "string") {
    return input.toUpperCase(); // input is `string` here
  }

  // TypeScript knows input must be `number` now
  return input.toFixed(2); // ✓
}
```

### Union for API Response Typing

```ts
// Every API response is either success or failure
type ApiResponse<T> =
  | { success: true; data: T; statusCode: number }
  | { success: false; error: string; statusCode: number; code: string };

async function createOrder(dto: CreateOrderDTO): Promise<ApiResponse<Order>> {
  try {
    const order = await orderService.create(dto);
    return { success: true, data: order, statusCode: 201 };
  } catch (err) {
    return {
      success: false,
      error: err.message,
      statusCode: err.statusCode ?? 500,
      code: err.code ?? "INTERNAL_ERROR",
    };
  }
}

// Consuming it
const result = await createOrder(dto);

if (result.success) {
  console.log(result.data.id); // ✓ — TypeScript knows data exists
} else {
  console.error(result.error); // ✓ — TypeScript knows error exists
  // result.data                  — TS Error! data doesn't exist on the error type
}
```

---

## 9. Literal Types

A **literal type** is a type that represents a single specific value, not just a general kind.

```ts
// Type is not just "string" — it's specifically the string "north"
type Direction = "north" | "south" | "east" | "west";

// Type is not just "number" — it's specifically 1, 2, or 3
type DiceRoll = 1 | 2 | 3 | 4 | 5 | 6;

// Boolean literal — only one possible value
type AlwaysTrue = true;

// Combining primitives
type StatusCode = 200 | 201 | 400 | 401 | 403 | 404 | 500;
```

### Why Literal Types Matter

```ts
// Without literal types
function move(direction: string): void { ... }
move("north"); // ✓
move("nroth"); // ✓ — typo goes undetected!

// With literal types
function move(direction: "north" | "south" | "east" | "west"): void { ... }
move("north"); // ✓
move("nroth"); // TS Error: Argument of type '"nroth"' is not assignable to...
```

### Real-World Literal Type Patterns

```ts
// HTTP method type
type HttpMethod =
  "GET" | "POST" | "PUT" | "PATCH" | "DELETE" | "HEAD" | "OPTIONS";

// User roles
type UserRole = "superadmin" | "admin" | "moderator" | "user" | "guest";

// Order status — often mirrors DB enum
type OrderStatus =
  | "draft"
  | "pending_payment"
  | "payment_processing"
  | "paid"
  | "fulfilling"
  | "shipped"
  | "delivered"
  | "cancelled"
  | "refunded";

// Environment
type Environment = "development" | "staging" | "production";
const env: Environment = process.env.NODE_ENV as Environment;

// Sort direction
type SortOrder = "asc" | "desc";

// Theme
type Theme = "light" | "dark" | "system";
```

### Literal Types with `as const`

```ts
// Without `as const`: type is string
const status = "active"; // type: string (widened)

// With `as const`: type is the literal "active"
const status = "active" as const; // type: "active"

// Object with `as const` — all string/number/boolean props become literals
const CONFIG = {
  env: "production",
  maxRetries: 3,
  features: {
    darkMode: true,
    beta: false,
  },
} as const;

CONFIG.env; // type: "production"
CONFIG.maxRetries; // type: 3
CONFIG.features.darkMode; // type: true

// Array with `as const`
const ALLOWED_SORTS = ["name", "date", "price"] as const;
// type: readonly ["name", "date", "price"]

type AllowedSort = (typeof ALLOWED_SORTS)[number];
// type: "name" | "date" | "price"
```

---

## 10. Tuples

A **tuple** is a typed array with a fixed number of elements where each position has a known type.

```ts
// Array: number of elements unknown, all same type
const scores: number[] = [95, 87, 92, 88]; // 0 to N numbers

// Tuple: exact number of elements, each with its own type
const userEntry: [number, string] = [1, "Alice"]; // exactly [number, string]

// Destructuring tuples (very natural)
const [id, name] = userEntry;
// id: number, name: string — fully typed!

// Accessing by index
userEntry[0]; // number
userEntry[1]; // string
userEntry[2]; // TS Error: Tuple type '[number, string]' has no element at index 2
```

### Labeled Tuples

Labels make tuples more readable (labels don't affect behavior):

```ts
type Coordinate = [x: number, y: number, z?: number];
type UserTuple = [
  id: number,
  name: string,
  email: string,
  role: "admin" | "user",
];

const coord: Coordinate = [10, 20]; // z is optional
const coord3d: Coordinate = [10, 20, 5];

function createUserEntry(id: number, name: string, email: string): UserTuple {
  return [id, name, email, "user"];
}
```

### Tuple as Function Return Value

The most common real use of tuples — returning multiple values with different types:

```ts
// [result, error] pattern (like Go)
type Result<T> = [T, null] | [null, Error];

async function safeAsync<T>(promise: Promise<T>): Promise<Result<T>> {
  try {
    return [await promise, null];
  } catch (err) {
    return [null, err instanceof Error ? err : new Error(String(err))];
  }
}

// Usage — no try/catch at call site
const [user, err] = await safeAsync(fetchUser(id));
if (err) {
  console.error(err.message);
  return;
}
// TypeScript knows: user is T (not null), err is null
console.log(user.name); // ✓
```

### React-style `useState` Tuple

```ts
// Returns [value, setter] — a classic tuple pattern
function createState<T>(initial: T): [T, (value: T) => void] {
  let state = initial;
  const setState = (value: T): void => {
    state = value;
  };
  return [state, setState];
}

const [count, setCount] = createState(0);
// count: number, setCount: (value: number) => void
setCount(1); // ✓
setCount("a"); // TS Error: string not assignable to number
```

### Rest Elements in Tuples

```ts
// Tuple with variable-length tail
type AtLeastTwo = [number, number, ...number[]];

const pair: AtLeastTwo = [1, 2]; // ✓
const triple: AtLeastTwo = [1, 2, 3]; // ✓
const single: AtLeastTwo = [1]; // TS Error — need at least 2 elements

// Spread tuples in function signatures
function first<T extends unknown[]>(arr: [T[0], ...T]): T[0] {
  return arr[0];
}
```

---

## 11. Enums

Enums define a set of named constants. TypeScript has two kinds: **numeric enums** and **string enums**.

### Numeric Enums

```ts
enum Direction {
  North, // 0
  South, // 1
  East, // 2
  West, // 3
}

Direction.North; // 0
Direction.South; // 1

// You can also go from value to name (reverse mapping)
Direction[0]; // "North"
Direction[3]; // "West"

// Starting from a custom number
enum Priority {
  Low = 1,
  Medium = 2,
  High = 3,
  Critical = 10,
}

// Continued from previous member
enum StatusCode {
  OK = 200,
  Created, // 201 — continues from 200
  BadRequest = 400,
  Unauthorized, // 401
  Forbidden, // 402 — wait, should be 403! This is a common trap
}
```

### String Enums — Preferred in Practice

```ts
enum UserRole {
  Admin = "admin",
  User = "user",
  Moderator = "moderator",
  Guest = "guest",
}

// String enums are much better for:
// 1. Debugging — "admin" is readable in logs, 0 is not
// 2. Serialization — JSON contains "admin", not 0
// 3. Safety — no accidental reverse mapping

const role: UserRole = UserRole.Admin;
role; // "admin"

// Serialize to JSON correctly
JSON.stringify({ role: UserRole.Admin }); // '{"role":"admin"}'
```

### Enums in Real Code

```ts
// Order status
enum OrderStatus {
  Draft = "draft",
  PendingPayment = "pending_payment",
  Processing = "processing",
  Paid = "paid",
  Shipped = "shipped",
  Delivered = "delivered",
  Cancelled = "cancelled",
  Refunded = "refunded",
}

// HTTP status codes
enum HttpStatus {
  OK = 200,
  Created = 201,
  NoContent = 204,
  BadRequest = 400,
  Unauthorized = 401,
  Forbidden = 403,
  NotFound = 404,
  Conflict = 409,
  UnprocessableEntity = 422,
  TooManyRequests = 429,
  InternalServerError = 500,
  ServiceUnavailable = 503,
}

// Using enum in switch for exhaustiveness
function describeStatus(status: OrderStatus): string {
  switch (status) {
    case OrderStatus.Draft:
      return "Not yet submitted";
    case OrderStatus.PendingPayment:
      return "Awaiting payment";
    case OrderStatus.Processing:
      return "Processing your order";
    case OrderStatus.Paid:
      return "Payment received";
    case OrderStatus.Shipped:
      return "On its way!";
    case OrderStatus.Delivered:
      return "Delivered";
    case OrderStatus.Cancelled:
      return "Cancelled";
    case OrderStatus.Refunded:
      return "Refunded";
    default:
      const _: never = status; // exhaustiveness check
      return "Unknown status";
  }
}
```

### `const` Enums — For Performance

```ts
// `const enum` — completely erased at compile time, values are inlined
const enum Direction {
  North = "NORTH",
  South = "SOUTH",
  East = "EAST",
  West = "WEST",
}

const dir = Direction.North;
// Compiled to: const dir = "NORTH"; — no object created, no runtime cost

// Limitation: can't iterate over const enum values at runtime
// Use regular enum if you need to loop over values
```

### Enums vs Literal Type Unions — The Debate

```ts
// Enum approach
enum UserRole {
  Admin = "admin",
  User = "user",
}

// Literal union approach
type UserRole = "admin" | "user";

// Enum pros:
//   - Grouped under a namespace (UserRole.Admin vs "admin")
//   - Can be used as value (passed to functions expecting the enum)
//   - IDE grouping

// Literal union pros:
//   - No runtime object generated (unless const enum)
//   - Simpler type error messages
//   - Easier to compose with other types
//   - More idiomatic TypeScript

// In modern TS projects, string literal unions are often preferred
// But enums are still great for large, stable, well-defined sets
```

---

## Summary Cheat Sheet

```
Arrays:
  number[]  or  Array<number>
  readonly number[]  or  ReadonlyArray<number>
  Infer from values; annotate empty arrays explicitly

Objects:
  { key: Type; key2: Type }
  Index signature: { [key: string]: value }
  Record<KeyType, ValueType>  — typed dictionary

Type Aliases:
  type Name = TypeExpression
  Utility types: Partial, Required, Pick, Omit, Readonly, Record, ReturnType

Optional:
  prop?: Type  →  prop is Type | undefined
  Must narrow before use: if(x), x?.method(), x ?? default

Readonly:
  readonly prop: Type  →  cannot reassign after construction
  Shallow — object contents can still mutate
  Readonly<T>  utility makes all props readonly

Intersection (&):
  A & B  →  value must have ALL properties of A AND B
  Use for: composing types, mixins, extending base types

Union (|):
  A | B  →  value is either A or B
  Discriminated unions: common literal `status` field for narrowing
  Always narrow before accessing type-specific properties

Literal types:
  "north" | "south"  — specific string values
  1 | 2 | 3          — specific number values
  as const           — forces literal type inference

Tuples:
  [Type1, Type2]     — fixed-length, each position typed
  [T, null] | [null, Error]  — result pattern
  Labeled: [x: number, y: number]

Enums:
  Numeric: auto-increments from 0
  String: explicit values — preferred (readable in logs/JSON)
  const enum: inlined at compile time (no runtime object)
  Exhaustiveness: switch covers all enum values
```

---

_Next: [Part 3 — OOP & Interfaces](./ts-part-3-oop.md)_
