> **Part 4 of 5.** TypeScript ships with a library of built-in utility types that transform existing types into new ones. This part covers every utility type in depth, then goes further into mapped types, conditional types, template literal types, and infer — the building blocks TypeScript's utility types are themselves written with.

---

## Table of Contents

1. [What Utility Types Are](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#1-what-utility-types-are)
2. [Partial, Required, and Readonly](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#2-partial-required-and-readonly)
3. [Pick and Omit](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#3-pick-and-omit)
4. [Record](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#4-record)
5. [Extract and Exclude](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#5-extract-and-exclude)
6. [NonNullable](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#6-nonnullable)
7. [Function Type Utilities](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#7-function-type-utilities)
8. [Awaited](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#8-awaited)
9. [Mapped Types](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#9-mapped-types)
10. [Conditional Types](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#10-conditional-types)
11. [infer — Extracting Types from Structure](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#11-infer--extracting-types-from-structure)
12. [Template Literal Types](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#12-template-literal-types)
13. [Combining Techniques — Real Patterns](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#13-combining-techniques--real-patterns)

---

## 1. What Utility Types Are

TypeScript's utility types are generic types built into the standard library. They take one or more types as parameters and produce a new transformed type.

```typescript
// Original type
type User = {
  id: number;
  name: string;
  email: string;
  password: string;
};

// Transformed types — no duplication, derived from User
type PartialUser   = Partial<User>;        // all fields optional
type PublicUser    = Omit<User, 'password'>; // password removed
type UserUpdate    = Partial<Pick<User, 'name' | 'email'>>; // optional subset
```

The key value: you define a type once and derive all variants from it. When the base type changes, all derived types update automatically.

---

## 2. Partial, Required, and Readonly

### Partial<T>

Makes all properties of `T` optional.

```typescript
type User = {
  id: number;
  name: string;
  email: string;
};

type UserUpdate = Partial<User>;
// Equivalent to:
// {
//   id?: number;
//   name?: string;
//   email?: string;
// }

function updateUser(id: number, changes: Partial<User>): User {
  // changes can have any subset of User's fields
}

updateUser(1, { name: "Alice" });           // valid
updateUser(1, { name: "Alice", email: "" }); // valid
updateUser(1, {});                           // valid — no changes
```

Common use: PATCH endpoints where the body contains only the fields being changed.

### Required<T>

The opposite of `Partial` — makes all properties required, removing `?` from every field.

```typescript
type Config = {
  host?: string;
  port?: number;
  timeout?: number;
};

type ResolvedConfig = Required<Config>;
// {
//   host: string;
//   port: number;
//   timeout: number;
// }

// After loading config from env/file, you've guaranteed all fields exist
function resolveConfig(input: Config): ResolvedConfig {
  return {
    host:    input.host    ?? 'localhost',
    port:    input.port    ?? 3000,
    timeout: input.timeout ?? 5000,
  };
}
```

### Readonly<T>

Makes all properties readonly — they can be set once but not mutated.

```typescript
type Config = {
  host: string;
  port: number;
};

const config: Readonly<Config> = {
  host: 'localhost',
  port: 3000,
};

config.port = 4000; // Error: Cannot assign to 'port' because it is a read-only property

// Useful for configuration objects, Redux state, and immutable data
type ImmutableUser = Readonly<User>;
```

`Readonly` is shallow — nested objects are not made readonly. For deep immutability you need a recursive mapped type (covered in section 9).

---

## 3. Pick and Omit

### Pick<T, K>

Constructs a type by picking a subset of properties `K` from `T`.

```typescript
type User = {
  id: number;
  name: string;
  email: string;
  password: string;
  createdAt: Date;
};

// What you send back in an API response — no password
type PublicUser = Pick<User, 'id' | 'name' | 'email'>;
// {
//   id: number;
//   name: string;
//   email: string;
// }

// What you use for a preview card
type UserCard = Pick<User, 'id' | 'name'>;
```

### Omit<T, K>

Constructs a type by removing properties `K` from `T`. The inverse of `Pick`.

```typescript
// Remove sensitive fields
type SafeUser = Omit<User, 'password'>;
// {
//   id: number;
//   name: string;
//   email: string;
//   createdAt: Date;
// }

// Remove auto-generated fields before inserting
type CreateUserInput = Omit<User, 'id' | 'createdAt'>;
// {
//   name: string;
//   email: string;
//   password: string;
// }
```

### Pick vs Omit — Which to Use

```typescript
// Use Pick when you want a FEW fields from a large type
type UserCard = Pick<User, 'id' | 'name'>;

// Use Omit when you want MOST fields, removing a few
type SafeUser = Omit<User, 'password'>;

// Rule of thumb:
//   Selecting 1–3 fields from 10+ → Pick
//   Removing 1–3 fields from 10+  → Omit
```

---

## 4. Record<K, V>

Constructs an object type with keys of type `K` and values of type `V`.

```typescript
// Simple: string keys, number values
type ScoreMap = Record<string, number>;
const scores: ScoreMap = { alice: 95, bob: 87 };

// Union keys — enforces all keys are present
type Role = 'admin' | 'editor' | 'viewer';
type RolePermissions = Record<Role, string[]>;

const permissions: RolePermissions = {
  admin:  ['read', 'write', 'delete'],
  editor: ['read', 'write'],
  viewer: ['read'],
  // TypeScript error if any Role key is missing
};

// Mapping from one type to another
type UserCache = Record<number, User>;   // userId → User
type Config    = Record<string, unknown>; // arbitrary config object
```

`Record<string, V>` is equivalent to `{ [key: string]: V }` — an index signature. But `Record<UnionType, V>` is more powerful because it enforces that every member of the union is present.

---

## 5. Extract and Exclude

These operate on union types rather than object types.

### Exclude<T, U>

Removes from union `T` all members that are assignable to `U`.

```typescript
type Status = 'pending' | 'active' | 'suspended' | 'deleted';

type ActiveStatus = Exclude<Status, 'deleted' | 'suspended'>;
// 'pending' | 'active'

type NoStrings = Exclude<string | number | boolean, string>;
// number | boolean

type NoNull = Exclude<string | null | undefined, null | undefined>;
// string
```

### Extract<T, U>

The opposite — keeps only members of `T` that are assignable to `U`.

```typescript
type Status = 'pending' | 'active' | 'suspended' | 'deleted';

type DangerousStatus = Extract<Status, 'suspended' | 'deleted' | 'archived'>;
// 'suspended' | 'deleted'  (archived not in Status, so excluded)

type OnlyStrings = Extract<string | number | boolean, string>;
// string
```

---

## 6. NonNullable<T>

Removes `null` and `undefined` from a type. Equivalent to `Exclude<T, null | undefined>`.

```typescript
type MaybeString = string | null | undefined;

type DefiniteString = NonNullable<MaybeString>;
// string

// Useful after null-checks
function processValue(value: string | null): void {
  if (!value) return;
  // TypeScript still thinks value is string | null here in older patterns
  // but NonNullable is useful for type-level transformations:
}

type NonNullableFields<T> = {
  [K in keyof T]: NonNullable<T[K]>;
};
```

---

## 7. Function Type Utilities

### ReturnType<T>

Extracts the return type of a function type.

```typescript
function getUser() {
  return { id: 1, name: 'Alice', email: 'alice@example.com' };
}

type User = ReturnType<typeof getUser>;
// { id: number; name: string; email: string; }

// Useful when you don't want to define the return type separately
async function fetchConfig() {
  return { host: 'localhost', port: 3000, debug: false };
}

type Config = Awaited<ReturnType<typeof fetchConfig>>;
// { host: string; port: number; debug: boolean; }
```

### Parameters<T>

Extracts the parameter types of a function as a tuple.

```typescript
function createUser(name: string, email: string, age: number): void {}

type CreateUserParams = Parameters<typeof createUser>;
// [name: string, email: string, age: number]

// Access individual parameters
type FirstParam  = Parameters<typeof createUser>[0]; // string
type SecondParam = Parameters<typeof createUser>[1]; // string

// Useful for wrapping functions
function withLogging<T extends (...args: any[]) => any>(fn: T) {
  return (...args: Parameters<T>): ReturnType<T> => {
    console.log('Calling with:', args);
    return fn(...args);
  };
}
```

### ConstructorParameters<T>

Like `Parameters`, but for class constructors.

```typescript
class Server {
  constructor(host: string, port: number, debug: boolean) {}
}

type ServerArgs = ConstructorParameters<typeof Server>;
// [host: string, port: number, debug: boolean]
```

### InstanceType<T>

Extracts the instance type of a constructor.

```typescript
class DatabaseConnection {
  query(sql: string): Promise<unknown[]> { return Promise.resolve([]); }
}

type DBConn = InstanceType<typeof DatabaseConnection>;
// DatabaseConnection

// Useful with factory functions or DI containers
function createInstance<T extends new (...args: any[]) => any>(
  Constructor: T
): InstanceType<T> {
  return new Constructor();
}
```

---

## 8. Awaited<T>

Unwraps the type inside a `Promise`, recursively.

```typescript
type A = Awaited<Promise<string>>;
// string

type B = Awaited<Promise<Promise<number>>>;
// number  (unwraps nested Promises)

type C = Awaited<string>;
// string  (non-Promise types pass through unchanged)

// Practical use: get the resolved type of an async function
async function getUsers(): Promise<User[]> { ... }

type Users = Awaited<ReturnType<typeof getUsers>>;
// User[]
```

---

## 9. Mapped Types

Mapped types let you create new types by iterating over the keys of an existing type and transforming each property.

### Basic Syntax

```typescript
type Mapped<T> = {
  [K in keyof T]: T[K];  // identity — same as T
};
```

`keyof T` produces a union of all keys. `T[K]` is the type of property `K` in `T`. Together, `[K in keyof T]: T[K]` iterates over all properties.

### Transforming Property Types

```typescript
type User = {
  id: number;
  name: string;
  email: string;
};

// Make every value nullable
type Nullable<T> = {
  [K in keyof T]: T[K] | null;
};

type NullableUser = Nullable<User>;
// {
//   id: number | null;
//   name: string | null;
//   email: string | null;
// }

// Wrap every value in a Promise
type Async<T> = {
  [K in keyof T]: Promise<T[K]>;
};
```

### Adding and Removing Modifiers

```typescript
// Add readonly and optional to all properties
type StrictReadonly<T> = {
  readonly [K in keyof T]?: T[K];
};

// Remove readonly from all properties
type Mutable<T> = {
  -readonly [K in keyof T]: T[K];
};

// Remove optional from all properties (same as Required<T>)
type Concrete<T> = {
  [K in keyof T]-?: T[K];
};
```

The `-` modifier removes `readonly` or `?`. The `+` modifier (default) adds them.

### Filtering Keys with `as`

```typescript
// Keep only string-valued properties
type StringFields<T> = {
  [K in keyof T as T[K] extends string ? K : never]: T[K];
};

type User = { id: number; name: string; email: string; age: number };
type StringUser = StringFields<User>;
// { name: string; email: string; }

// Rename keys
type Getters<T> = {
  [K in keyof T as `get${Capitalize<string & K>}`]: () => T[K];
};

type UserGetters = Getters<User>;
// {
//   getId:    () => number;
//   getName:  () => string;
//   getEmail: () => string;
// }
```

### Deep Readonly

The built-in `Readonly<T>` is shallow — nested objects aren't affected. A recursive mapped type fixes this:

```typescript
type DeepReadonly<T> = {
  readonly [K in keyof T]: T[K] extends object ? DeepReadonly<T[K]> : T[K];
};

type Config = { database: { host: string; port: number }; debug: boolean };

type FrozenConfig = DeepReadonly<Config>;
// { readonly database: { readonly host: string; readonly port: number }; readonly debug: boolean }
```

---

## 10. Conditional Types

Conditional types select a type based on a condition, using the same `extends` syntax used for constraints.

```typescript
type IsString<T> = T extends string ? 'yes' : 'no';

type A = IsString<string>;  // 'yes'
type B = IsString<number>;  // 'no'
type C = IsString<'hello'>; // 'yes' — 'hello' extends string
```

### Distributive Conditional Types

When the type being checked (`T`) is a bare type parameter and you pass a union, the conditional distributes over each member:

```typescript
type IsString<T> = T extends string ? 'yes' : 'no';

type D = IsString<string | number | boolean>;
// 'yes' | 'no' | 'no'
// Equivalent to: IsString<string> | IsString<number> | IsString<boolean>
```

This is how `Exclude` and `Extract` are implemented:

```typescript
// Exclude<T, U> = T extends U ? never : T
// When T is a union, this distributes:
// Exclude<'a' | 'b' | 'c', 'a'> 
//   = ('a' extends 'a' ? never : 'a') | ('b' extends 'a' ? never : 'b') | ('c' extends 'a' ? never : 'c')
//   = never | 'b' | 'c'
//   = 'b' | 'c'
```

To prevent distribution (treat the union as a whole), wrap in a tuple:

```typescript
type IsUnion<T> = [T] extends [T] ? true : false;
// [T] prevents distribution — T is treated as one type
```

---

## 11. infer — Extracting Types from Structure

`infer` declares a type variable inside a conditional type. TypeScript infers what that variable should be from the structure being matched.

```typescript
// Extract the return type of any function
type ReturnType<T> = T extends (...args: any[]) => infer R ? R : never;
//                                                 ^^^^^^^ infer R captures the return type

type A = ReturnType<() => string>;         // string
type B = ReturnType<(x: number) => boolean>; // boolean
```

### Extracting from Arrays

```typescript
type ElementType<T> = T extends (infer E)[] ? E : never;

type A = ElementType<string[]>;          // string
type B = ElementType<number[]>;          // number
type C = ElementType<Array<User>>;       // User
type D = ElementType<string>;            // never — string is not an array
```

### Extracting from Promises

```typescript
type UnwrapPromise<T> = T extends Promise<infer U> ? U : T;

type A = UnwrapPromise<Promise<string>>; // string
type B = UnwrapPromise<Promise<User>>;   // User
type C = UnwrapPromise<string>;          // string — passes through
```

### Extracting Function Parameter Types

```typescript
type FirstParam<T> = T extends (first: infer F, ...rest: any[]) => any ? F : never;

type A = FirstParam<(name: string, age: number) => void>; // string
type B = FirstParam<() => void>;                          // never
```

### Infer in Recursive Types

```typescript
// Flatten a nested array to its deepest element type
type DeepElement<T> = T extends (infer E)[]
  ? DeepElement<E>
  : T;

type A = DeepElement<string[][][]>; // string
type B = DeepElement<number[]>;     // number
type C = DeepElement<string>;       // string
```

---

## 12. Template Literal Types

TypeScript can construct string literal types using template literal syntax.

```typescript
type Greeting = `Hello, ${string}`;
// Any string starting with "Hello, "

type EventName = `on${Capitalize<string>}`;
// "onClick", "onChange", "onSubmit", etc.

// Combining literal unions
type Direction = 'top' | 'right' | 'bottom' | 'left';
type Edge = `border-${Direction}`;
// 'border-top' | 'border-right' | 'border-bottom' | 'border-left'

type Size = 'sm' | 'md' | 'lg';
type Color = 'red' | 'blue' | 'green';
type ButtonClass = `btn-${Size}-${Color}`;
// 'btn-sm-red' | 'btn-sm-blue' | 'btn-sm-green' | 'btn-md-red' | ...
```

### Built-in String Manipulation Types

```typescript
type U = Uppercase<'hello'>;         // 'HELLO'
type L = Lowercase<'WORLD'>;         // 'world'
type C = Capitalize<'hello'>;        // 'Hello'
type UC = Uncapitalize<'Hello'>;     // 'hello'
```

### Generating Getter/Setter Names

```typescript
type Fields = 'name' | 'email' | 'age';

type Getters = `get${Capitalize<Fields>}`;
// 'getName' | 'getEmail' | 'getAge'

type Setters = `set${Capitalize<Fields>}`;
// 'setName' | 'setEmail' | 'setAge'
```

### Type-Safe Event Systems

```typescript
type EventMap = {
  click:  { x: number; y: number };
  focus:  { target: HTMLElement };
  change: { value: string };
};

type EventHandlers = {
  [K in keyof EventMap as `on${Capitalize<string & K>}`]: (event: EventMap[K]) => void;
};

// {
//   onClick:  (event: { x: number; y: number }) => void;
//   onFocus:  (event: { target: HTMLElement }) => void;
//   onChange: (event: { value: string }) => void;
// }
```

---

## 13. Combining Techniques — Real Patterns

### Type-Safe API Response Wrapper

```typescript
type ApiSuccess<T> = { status: 'success'; data: T };
type ApiError      = { status: 'error';   message: string };
type ApiResponse<T> = ApiSuccess<T> | ApiError;

// Extract the data type from a success response
type ApiData<T> = T extends ApiSuccess<infer D> ? D : never;

type UserResponse = ApiResponse<User>;
type UserData     = ApiData<ApiSuccess<User>>;  // User
```

### Deep Partial

Built-in `Partial<T>` is shallow. For deeply nested config objects:

```typescript
type DeepPartial<T> = {
  [K in keyof T]?: T[K] extends object ? DeepPartial<T[K]> : T[K];
};

type Config = {
  server: { host: string; port: number };
  database: { url: string; pool: number };
};

type PartialConfig = DeepPartial<Config>;
// {
//   server?:   { host?: string; port?: number };
//   database?: { url?: string; pool?: number };
// }
```

### Builder Pattern with Conditional Types

```typescript
type BuilderState = {
  hasName:  boolean;
  hasEmail: boolean;
};

type UserBuilder<S extends BuilderState> = {
  setName(name: string):  UserBuilder<S & { hasName: true }>;
  setEmail(email: string): UserBuilder<S & { hasEmail: true }>;
} & (S extends { hasName: true; hasEmail: true }
  ? { build(): User }
  : { build: never });

// build() only appears when both setName and setEmail have been called
```

### Discriminated Union Helpers

```typescript
type Shape =
  | { kind: 'circle';    radius: number }
  | { kind: 'rectangle'; width: number; height: number }
  | { kind: 'triangle';  base: number;  height: number };

// Extract a specific variant by discriminant
type ExtractVariant<T, K extends string> = Extract<T, { kind: K }>;

type Circle = ExtractVariant<Shape, 'circle'>;
// { kind: 'circle'; radius: number }

// Ensure a switch is exhaustive
function assertNever(x: never): never {
  throw new Error('Unexpected value: ' + x);
}

function area(shape: Shape): number {
  switch (shape.kind) {
    case 'circle':    return Math.PI * shape.radius ** 2;
    case 'rectangle': return shape.width * shape.height;
    case 'triangle':  return 0.5 * shape.base * shape.height;
    default:          return assertNever(shape); // compile error if a case is missing
  }
}
```

### Validated Environment Variables

```typescript
type EnvSchema = {
  DATABASE_URL: string;
  PORT:         number;
  DEBUG:        boolean;
};

type EnvKeys = keyof EnvSchema;

function loadEnv(): EnvSchema {
  const raw = process.env;

  return {
    DATABASE_URL: raw.DATABASE_URL ?? (() => { throw new Error('DATABASE_URL missing') })(),
    PORT:         Number(raw.PORT ?? 3000),
    DEBUG:        raw.DEBUG === 'true',
  };
}

// The return type is fully typed — no string everywhere
const env = loadEnv();
env.PORT;  // number — not string
```