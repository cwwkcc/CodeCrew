> **Part 3 of 5.** TypeScript's object-oriented programming model — classes with typed properties, access modifiers (`public`, `private`, `protected`, `readonly`), static members, abstract classes, and the complete interfaces system.

---

## Table of Contents

1. [OOP in TypeScript — Overview](#1-oop-in-typescript--overview)
2. [Class Basics & Typed Properties](#2-class-basics--typed-properties)
3. [Access Modifiers: public, private, protected](#3-access-modifiers-public-private-protected)
4. [Parameter Properties (Shorthand)](#4-parameter-properties-shorthand)
5. [Readonly in Classes](#5-readonly-in-classes)
6. [Static Members](#6-static-members)
7. [Getters and Setters](#7-getters-and-setters)
8. [Abstract Classes](#8-abstract-classes)
9. [Class Inheritance](#9-class-inheritance)
10. [Interfaces — Introduction](#10-interfaces--introduction)
11. [Interfaces vs Type Aliases](#11-interfaces-vs-type-aliases)
12. [Interface Extension](#12-interface-extension)
13. [Implementing Interfaces](#13-implementing-interfaces)
14. [Function Interfaces](#14-function-interfaces)
15. [Indexable Interfaces](#15-indexable-interfaces)
16. [Declaration Merging](#16-declaration-merging)
17. [Real-World OOP Patterns](#17-real-world-oop-patterns)

---

## 1. OOP in TypeScript — Overview

TypeScript greatly enhances JavaScript's class system with:

- **Typed properties** — all properties must be declared and typed
- **Access modifiers** — `public`, `private`, `protected`
- **True private fields** — `#field` (hard private) or `private field` (soft)
- **`readonly`** — immutable after construction
- **Abstract classes** — enforce contracts without providing implementation
- **Interfaces** — structural contracts that classes can implement
- **Generics on classes** — type parameters for reusable data structures

```ts
// Full TypeScript class — everything used at once
class UserService {
  private readonly db: Database;
  private cache = new Map<string, User>();
  public readonly name = "UserService";
  protected logger: Logger;

  constructor(db: Database, logger: Logger) {
    this.db = db;
    this.logger = logger;
  }

  async findById(id: string): Promise<User | null> {
    if (this.cache.has(id)) return this.cache.get(id)!;
    const user = await this.db.users.findOne({ id });
    if (user) this.cache.set(id, user);
    return user;
  }
}
```

---

## 2. Class Basics & Typed Properties

In TypeScript, all class properties must be **declared** — you can't assign `this.x = 5` in the constructor without first declaring `x` as a property.

```ts
class User {
  // All properties must be declared here with their types
  id: number;
  name: string;
  email: string;
  role: "admin" | "user";
  isActive: boolean;
  createdAt: Date;

  constructor(id: number, name: string, email: string) {
    this.id = id;
    this.name = name;
    this.email = email;
    this.role = "user"; // assigned in constructor
    this.isActive = true; // assigned in constructor
    this.createdAt = new Date(); // assigned in constructor
  }
}

const alice = new User(1, "Alice", "alice@example.com");
alice.name; // string
alice.role; // "admin" | "user"
alice.createdAt; // Date
alice.salary; // TS Error: Property 'salary' does not exist on type 'User'
```

### Property Initialization Options

```ts
class Config {
  // Option 1: Initialized directly (no constructor needed)
  maxRetries: number = 3;
  debug: boolean = false;
  timeout: number = 10_000;

  // Option 2: Initialized in constructor (see above)
  port: number;
  constructor(port: number) {
    this.port = port;
  }

  // Option 3: Definite assignment assertion — tell TS "I'll assign this, trust me"
  dbConnection!: DatabaseConnection; // The `!` means "definitely assigned before use"
  // Use sparingly — it bypasses type safety

  // Option 4: Optional — might be undefined
  cacheClient?: RedisClient;
}
```

### `strictPropertyInitialization`

With `"strict": true`, TypeScript requires that every class property is initialized in the constructor (or directly). This catches a common class of bugs:

```ts
class BadService {
  db: Database; // TS Error: Property 'db' has no initializer and is not definitely assigned in the constructor
}

class GoodService {
  db: Database;
  constructor(db: Database) {
    this.db = db; // ✓
  }
}
```

---

## 3. Access Modifiers: public, private, protected

TypeScript has three access modifiers that control visibility of class members.

### `public` — Default

```ts
class User {
  public name: string; // visible everywhere — this is the default
  public greet(): string {
    return `Hello, I'm ${this.name}`;
  }
}

const user = new User("Alice");
user.name; // ✓ — accessible outside the class
user.greet(); // ✓
```

### `private` — Class-Only

`private` members are only accessible within the class body. TypeScript enforces this at compile time.

```ts
class UserService {
  private db: Database;
  private cache: Map<string, User> = new Map();

  constructor(db: Database) {
    this.db = db;
  }

  private async loadFromDb(id: string): Promise<User | null> {
    return this.db.users.findOne({ id });
  }

  async getUser(id: string): Promise<User | null> {
    if (this.cache.has(id)) return this.cache.get(id)!;
    const user = await this.loadFromDb(id); // ✓ — called from within class
    if (user) this.cache.set(id, user);
    return user;
  }
}

const service = new UserService(db);
service.getUser("u1"); // ✓ — public method
service.db; // TS Error: Property 'db' is private
service.loadFromDb("u1"); // TS Error: Property 'loadFromDb' is private
service.cache; // TS Error: Property 'cache' is private
```

### TypeScript `private` vs JavaScript `#`

There's an important distinction:

```ts
class Foo {
  private tsPrivate = "ts private"; // TypeScript private — only compile-time check
  #jsPrivate = "js private"; // JavaScript private — true runtime private
}

const foo = new Foo();
foo.tsPrivate; // TS Error at compile time, BUT accessible via JS at runtime!
(foo as any).tsPrivate; // "ts private" — bypasses TS, still works in JS

foo.#jsPrivate; // TS Error AND runtime error — truly private
(foo as any)["#jsPrivate"]; // SyntaxError — can't even access via property name
```

**Rule:** Use `#` for true encapsulation (runtime privacy). Use `private` when TypeScript compile-time checks are sufficient.

### `protected` — Class and Subclasses

`protected` members are accessible within the class AND all subclasses, but not from outside.

```ts
class BaseRepository<T> {
  protected db: Database; // subclasses need this
  protected tableName: string;

  constructor(db: Database, tableName: string) {
    this.db = db;
    this.tableName = tableName;
  }

  protected async executeQuery<R>(sql: string, params: unknown[]): Promise<R> {
    return this.db.query(sql, params);
  }

  async findAll(): Promise<T[]> {
    return this.executeQuery(`SELECT * FROM ${this.tableName}`, []);
  }
}

class UserRepository extends BaseRepository<User> {
  constructor(db: Database) {
    super(db, "users");
  }

  async findByEmail(email: string): Promise<User | null> {
    const result = await this.executeQuery<User[]>(
      // ✓ — protected method accessible in subclass
      `SELECT * FROM ${this.tableName} WHERE email = $1`,
      [email],
    );
    return result[0] ?? null;
  }
}

const repo = new UserRepository(db);
repo.findByEmail("alice@example.com"); // ✓ — public method
repo.db; // TS Error: protected, not accessible outside
repo.executeQuery("...", []); // TS Error: protected
```

---

## 4. Parameter Properties (Shorthand)

TypeScript provides a shorthand that declares AND initializes a class property in one step inside the constructor:

```ts
// Without parameter properties:
class UserService {
  private readonly db: Database;
  private readonly logger: Logger;
  public readonly name: string;

  constructor(db: Database, logger: Logger, name: string) {
    this.db = db;
    this.logger = logger;
    this.name = name;
  }
}

// With parameter properties (access modifier in constructor param):
class UserService {
  constructor(
    private readonly db: Database, // declares + assigns this.db
    private readonly logger: Logger, // declares + assigns this.logger
    public readonly name: string, // declares + assigns this.name
  ) {}
  // Equivalent! No body code needed.
}
```

### Parameter Properties in Practice

```ts
class AuthService {
  constructor(
    private readonly userRepo: UserRepository,
    private readonly jwtSecret: string,
    private readonly tokenExpiry: string = "15m",
    private readonly refreshExpiry: string = "7d",
  ) {}

  async login(email: string, password: string): Promise<TokenPair> {
    const user = await this.userRepo.findByEmail(email);
    if (!user) throw new UnauthorizedError("Invalid credentials");

    const valid = await bcrypt.compare(password, user.passwordHash);
    if (!valid) throw new UnauthorizedError("Invalid credentials");

    return this.generateTokens(user);
  }

  private generateTokens(user: User): TokenPair {
    const access = jwt.sign({ sub: user.id, role: user.role }, this.jwtSecret, {
      expiresIn: this.tokenExpiry,
    });
    const refresh = jwt.sign({ sub: user.id }, this.jwtSecret, {
      expiresIn: this.refreshExpiry,
    });
    return { access, refresh };
  }
}
```

---

## 5. Readonly in Classes

`readonly` class properties can only be set in the constructor (or directly in the declaration):

```ts
class Order {
  readonly id: string;
  readonly createdAt: Date;
  readonly customerId: string;
  status: OrderStatus; // mutable
  items: OrderItem[]; // mutable

  constructor(customerId: string, items: OrderItem[]) {
    this.id = generateOrderId(); // ✓ — set in constructor
    this.createdAt = new Date(); // ✓ — set in constructor
    this.customerId = customerId; // ✓ — set in constructor
    this.status = OrderStatus.Draft;
    this.items = items;
  }

  addItem(item: OrderItem): void {
    this.items.push(item); // ✓ — items is not readonly
    this.id = "new_id"; // TS Error — readonly after constructor
    this.createdAt = new Date(); // TS Error
  }
}
```

---

## 6. Static Members

`static` members belong to the class itself, not instances.

```ts
class IdGenerator {
  private static nextId: number = 1;
  private static prefix: string = "ID";

  static generate(): string {
    return `${IdGenerator.prefix}_${String(IdGenerator.nextId++).padStart(6, "0")}`;
  }

  static reset(): void {
    IdGenerator.nextId = 1;
  }

  static configure(prefix: string): void {
    IdGenerator.prefix = prefix;
  }
}

IdGenerator.generate(); // "ID_000001"
IdGenerator.generate(); // "ID_000002"
IdGenerator.configure("USR");
IdGenerator.generate(); // "USR_000003"

// Can't access on instance
const gen = new IdGenerator();
gen.generate(); // TS Error: Property 'generate' does not exist on type 'IdGenerator'
```

### Static Singleton Pattern

```ts
class DatabaseConnection {
  private static instance: DatabaseConnection | null = null;
  private isConnected: boolean = false;

  private constructor(private readonly connectionString: string) {}

  static getInstance(connectionString: string): DatabaseConnection {
    if (!DatabaseConnection.instance) {
      DatabaseConnection.instance = new DatabaseConnection(connectionString);
    }
    return DatabaseConnection.instance;
  }

  async connect(): Promise<void> {
    if (this.isConnected) return;
    // ... connect to DB
    this.isConnected = true;
  }

  async query<T>(sql: string, params: unknown[]): Promise<T[]> {
    if (!this.isConnected) throw new Error("Not connected");
    // ... execute query
    return [];
  }
}

// Usage — always returns the same instance
const db = DatabaseConnection.getInstance(process.env.DATABASE_URL!);
await db.connect();
```

---

## 7. Getters and Setters

```ts
class User {
  private _firstName: string;
  private _lastName: string;
  private _email: string;

  constructor(firstName: string, lastName: string, email: string) {
    this._firstName = firstName;
    this._lastName = lastName;
    this._email = email;
  }

  // Getter — accessed as property, not method call
  get fullName(): string {
    return `${this._firstName} ${this._lastName}`;
  }

  // Getter and setter pair
  get email(): string {
    return this._email;
  }

  set email(value: string) {
    if (!value.includes("@")) throw new Error("Invalid email format");
    this._email = value.toLowerCase().trim();
  }

  // Computed getter
  get initials(): string {
    return `${this._firstName[0]}${this._lastName[0]}`.toUpperCase();
  }
}

const user = new User("Alice", "Chen", "ALICE@EXAMPLE.COM");
user.fullName; // "Alice Chen"
user.initials; // "AC"
user.email; // "alice@example.com" (normalized)
user.email = "alice@company.com"; // ✓ — setter validates + normalizes
user.email = "notanemail"; // throws Error
```

---

## 8. Abstract Classes

An **abstract class** is a class that cannot be instantiated directly — it must be subclassed. It can define abstract methods that subclasses MUST implement, and concrete methods that subclasses inherit.

```ts
abstract class BaseRepository<T, ID = string> {
  constructor(protected readonly db: Database) {}

  // Abstract methods — subclasses MUST implement these
  abstract findById(id: ID): Promise<T | null>;
  abstract findAll(filters?: Partial<T>): Promise<T[]>;
  abstract create(data: Omit<T, "id" | "createdAt" | "updatedAt">): Promise<T>;
  abstract update(id: ID, data: Partial<T>): Promise<T | null>;
  abstract delete(id: ID): Promise<boolean>;

  // Concrete methods — inherited by all subclasses
  async findOrFail(id: ID): Promise<T> {
    const entity = await this.findById(id);
    if (!entity) throw new NotFoundError(`Entity ${id} not found`);
    return entity;
  }

  async exists(id: ID): Promise<boolean> {
    const entity = await this.findById(id);
    return entity !== null;
  }

  async createMany(items: Array<Omit<T, "id" | "createdAt" | "updatedAt">>): Promise<T[]> {
    return Promise.all(items.map(item => this.create(item)));
  }
}

// Cannot instantiate abstract class directly
const repo = new BaseRepository(db); // TS Error: Cannot create an instance of an abstract class

// Must subclass and implement all abstract methods
class UserRepository extends BaseRepository<User> {
  async findById(id: string): Promise<User | null> {
    return this.db.query<User>("SELECT * FROM users WHERE id = $1", [id])
      .then(rows => rows[0] ?? null);
  }

  async findAll(filters?: Partial<User>): Promise<User[]> {
    const conditions = buildWhereClause(filters);
    return this.db.query<User>(`SELECT * FROM users ${conditions.sql}`, conditions.params);
  }

  async create(data: Omit<User, "id" | "createdAt" | "updatedAt">): Promise<User> {
    const id = generateId();
    const now = new Date();
    return this.db.queryOne<User>(
      "INSERT INTO users (id, name, email, ..., created_at, updated_at) VALUES ($1,...) RETURNING *",
      [id, data.name, data.email, ..., now, now]
    );
  }

  async update(id: string, data: Partial<User>): Promise<User | null> {
    const sets = buildSetClause(data);
    return this.db.queryOne<User>(
      `UPDATE users SET ${sets.sql} WHERE id = $${sets.params.length + 1} RETURNING *`,
      [...sets.params, id]
    );
  }

  async delete(id: string): Promise<boolean> {
    const result = await this.db.query(
      "DELETE FROM users WHERE id = $1", [id]
    );
    return result.rowCount > 0;
  }

  // Additional methods specific to UserRepository
  async findByEmail(email: string): Promise<User | null> {
    return this.db.queryOne<User>(
      "SELECT * FROM users WHERE email = $1", [email]
    );
  }
}

const userRepo = new UserRepository(db);
await userRepo.findOrFail("u1");   // inherited from BaseRepository
await userRepo.findByEmail("alice@example.com"); // UserRepository-specific
```

---

## 9. Class Inheritance

```ts
class BaseEntity {
  readonly id: string;
  readonly createdAt: Date;
  updatedAt: Date;

  constructor(id?: string) {
    this.id = id ?? generateId();
    this.createdAt = new Date();
    this.updatedAt = new Date();
  }

  touch(): void {
    this.updatedAt = new Date();
  }

  toJSON(): Record<string, unknown> {
    return {
      id: this.id,
      createdAt: this.createdAt.toISOString(),
      updatedAt: this.updatedAt.toISOString(),
    };
  }
}

class User extends BaseEntity {
  name: string;
  email: string;
  role: UserRole;

  constructor(name: string, email: string, role: UserRole = UserRole.User) {
    super(); // MUST call super() — calls BaseEntity constructor
    this.name = name;
    this.email = email;
    this.role = role;
  }

  // Override parent method
  override toJSON(): Record<string, unknown> {
    return {
      ...super.toJSON(), // include parent fields
      name: this.name,
      email: this.email,
      role: this.role,
    };
  }
}

class AdminUser extends User {
  permissions: string[];

  constructor(name: string, email: string, permissions: string[] = []) {
    super(name, email, UserRole.Admin); // call User constructor
    this.permissions = permissions;
  }

  can(permission: string): boolean {
    return (
      this.permissions.includes(permission) || this.permissions.includes("*")
    );
  }

  override toJSON(): Record<string, unknown> {
    return {
      ...super.toJSON(),
      permissions: this.permissions,
    };
  }
}

const admin = new AdminUser("Alice", "alice@example.com", ["read", "write"]);
admin.id; // ✓ — from BaseEntity
admin.name; // ✓ — from User
admin.can("read"); // ✓ — from AdminUser
admin instanceof AdminUser; // true
admin instanceof User; // true
admin instanceof BaseEntity; // true
```

### The `override` Keyword

TypeScript 4.3+ added `override` to make subclass method overrides explicit and safe:

```ts
class Animal {
  move(): void {
    console.log("Moving");
  }
  speak(): void {
    console.log("...");
  }
}

class Dog extends Animal {
  override move(): void {
    // ✓ — explicit override
    console.log("Running");
  }

  run(): void {
    // new method, not an override
    console.log("Running fast!");
  }

  override fly(): void {
    // TS Error! 'fly' doesn't exist in Animal
    console.log("Dogs can't fly");
  }
}
```

---

## 10. Interfaces — Introduction

An **interface** in TypeScript is a structural contract — it defines the shape (property names and types, method signatures) that an object must conform to.

```ts
interface User {
  id: string;
  name: string;
  email: string;
  role: "admin" | "user";
  isActive: boolean;
  createdAt: Date;
}

// Any object that has all these properties at the right types satisfies the interface
const user: User = {
  id: "u1",
  name: "Alice",
  email: "alice@example.com",
  role: "admin",
  isActive: true,
  createdAt: new Date(),
};
```

### Interface with Methods

```ts
interface Logger {
  info(message: string, meta?: Record<string, unknown>): void;
  warn(message: string, meta?: Record<string, unknown>): void;
  error(message: string, error?: Error, meta?: Record<string, unknown>): void;
  debug(message: string, meta?: Record<string, unknown>): void;
}

interface Cache<T> {
  get(key: string): T | undefined;
  set(key: string, value: T, ttlMs?: number): void;
  delete(key: string): boolean;
  clear(): void;
  has(key: string): boolean;
  size(): number;
}
```

---

## 11. Interfaces vs Type Aliases

```ts
// Interface — object shape
interface UserInterface {
  id: string;
  name: string;
}

// Type alias — same object shape
type UserType = {
  id: string;
  name: string;
};

// Both work the same way for object shapes:
const u1: UserInterface = { id: "1", name: "Alice" };
const u2: UserType = { id: "1", name: "Alice" };
```

### What Only `type` Can Do

```ts
// Primitive aliases
type ID = string;
type Timestamp = number;

// Union types
type Result<T> = { success: true; data: T } | { success: false; error: string };

// Tuples
type Pair<T> = [T, T];
type AsyncResult<T> = [T | null, Error | null];

// Conditional types
type NonNullable<T> = T extends null | undefined ? never : T;

// Mapped types
type Optional<T> = { [K in keyof T]?: T[K] };
```

### What Only `interface` Can Do

```ts
// Declaration merging (same interface declared twice — they merge)
interface Window {
  myCustomProperty: string; // extends the global Window interface
}

// Classes can `implement` interfaces (also works with type, but interface is conventional)
interface Serializable {
  serialize(): string;
}
class User implements Serializable { ... }
```

### The Recommendation

- **`interface`** for: class contracts, object shapes that will be extended or implemented, library public APIs
- **`type`** for: unions, primitives, tuples, mapped/conditional types, utilities, computed types

Many teams use `type` for everything except `implements` and when declaration merging is needed.

---

## 12. Interface Extension

Interfaces can extend one or multiple other interfaces:

```ts
interface HasId {
  id: string;
}

interface HasTimestamps {
  createdAt: Date;
  updatedAt: Date;
}

interface HasSoftDelete {
  deletedAt: Date | null;
}

// Extends multiple interfaces
interface BaseEntity extends HasId, HasTimestamps, HasSoftDelete {}

interface User extends BaseEntity {
  name: string;
  email: string;
  role: UserRole;
}

// User has: id, createdAt, updatedAt, deletedAt, name, email, role
```

### Deep Interface Hierarchy

```ts
interface Entity {
  id: string;
}

interface TimestampedEntity extends Entity {
  createdAt: Date;
  updatedAt: Date;
}

interface SoftDeletableEntity extends TimestampedEntity {
  deletedAt: Date | null;
  isDeleted: boolean;
}

interface AuditableEntity extends SoftDeletableEntity {
  createdBy: string;
  updatedBy: string;
}

// A fully auditable entity
interface Product extends AuditableEntity {
  name: string;
  sku: string;
  price: number;
  inventory: number;
}
```

---

## 13. Implementing Interfaces

A class that `implements` an interface must provide all the interface's members:

```ts
interface PaymentProcessor {
  name: string;
  process(
    amount: number,
    currency: string,
    metadata: PaymentMetadata,
  ): Promise<PaymentResult>;
  refund(transactionId: string, amount?: number): Promise<RefundResult>;
  getTransaction(transactionId: string): Promise<Transaction>;
}

// Stripe implementation
class StripePaymentProcessor implements PaymentProcessor {
  readonly name = "Stripe";

  constructor(private readonly apiKey: string) {}

  async process(
    amount: number,
    currency: string,
    metadata: PaymentMetadata,
  ): Promise<PaymentResult> {
    const stripe = new Stripe(this.apiKey);
    const intent = await stripe.paymentIntents.create({
      amount: Math.round(amount * 100),
      currency: currency.toLowerCase(),
      metadata,
    });
    return {
      success: intent.status === "succeeded",
      transactionId: intent.id,
      amount,
      currency,
    };
  }

  async refund(transactionId: string, amount?: number): Promise<RefundResult> {
    const stripe = new Stripe(this.apiKey);
    const refund = await stripe.refunds.create({
      payment_intent: transactionId,
      ...(amount ? { amount: Math.round(amount * 100) } : {}),
    });
    return { success: refund.status === "succeeded", refundId: refund.id };
  }

  async getTransaction(transactionId: string): Promise<Transaction> {
    const stripe = new Stripe(this.apiKey);
    const intent = await stripe.paymentIntents.retrieve(transactionId);
    return mapStripeIntentToTransaction(intent);
  }
}

// PayPal implementation — same interface, different implementation
class PayPalPaymentProcessor implements PaymentProcessor {
  readonly name = "PayPal";
  // ... must implement ALL interface methods
}

// Code that uses any PaymentProcessor — doesn't care which one
async function processCheckout(
  cart: Cart,
  processor: PaymentProcessor, // ← program to the interface!
): Promise<Order> {
  const result = await processor.process(cart.total, cart.currency, {
    orderId: cart.id,
    customerId: cart.customerId,
  });

  if (!result.success) throw new PaymentError("Payment failed");

  return createOrder(cart, result.transactionId);
}

// Works with either
await processCheckout(cart, new StripePaymentProcessor(STRIPE_KEY));
await processCheckout(cart, new PayPalPaymentProcessor(PAYPAL_KEY));
```

### Multiple Interface Implementation

```ts
interface Serializable {
  serialize(): string;
}

interface Validatable {
  validate(): ValidationResult;
}

interface Loggable {
  toLogEntry(): Record<string, unknown>;
}

// A class can implement multiple interfaces
class UserRegistrationForm implements Serializable, Validatable, Loggable {
  constructor(
    public name: string,
    public email: string,
    public password: string,
  ) {}

  serialize(): string {
    return JSON.stringify({ name: this.name, email: this.email });
  }

  validate(): ValidationResult {
    const errors: string[] = [];
    if (!this.name.trim()) errors.push("Name is required");
    if (!this.email.includes("@")) errors.push("Invalid email");
    if (this.password.length < 8) errors.push("Password too short");
    return { valid: errors.length === 0, errors };
  }

  toLogEntry(): Record<string, unknown> {
    return { email: this.email, name: this.name, timestamp: Date.now() };
    // password intentionally excluded
  }
}
```

---

## 14. Function Interfaces

Interfaces can describe functions:

```ts
// Interface for a function
interface Comparator<T> {
  (a: T, b: T): number; // call signature
}

// Usage
const byName: Comparator<User> = (a, b) => a.name.localeCompare(b.name);
const byPrice: Comparator<Product> = (a, b) => a.price - b.price;

users.sort(byName);
products.sort(byPrice);

// Interface with both call signature AND properties
interface ValidationFn {
  (value: unknown): boolean;
  errorMessage: string;
  fieldName: string;
}

function createValidator(
  fn: (v: unknown) => boolean,
  field: string,
  message: string,
): ValidationFn {
  const validator = fn as ValidationFn;
  validator.errorMessage = message;
  validator.fieldName = field;
  return validator;
}

const emailValidator = createValidator(
  (v) => typeof v === "string" && v.includes("@"),
  "email",
  "Must be a valid email",
);

emailValidator("alice@example.com"); // true
emailValidator.errorMessage; // "Must be a valid email"
```

---

## 15. Indexable Interfaces

Interfaces with index signatures for dictionary-like structures:

```ts
// String index
interface StringMap {
  [key: string]: string;
}

const headers: StringMap = {
  "Content-Type": "application/json",
  Authorization: "Bearer ...",
};

// Number index (like an array)
interface NumberIndexed {
  [index: number]: string;
}

// Mixed — named properties with index signature
interface FlexibleConfig {
  version: string; // specific named property
  debug: boolean; // specific named property
  [key: string]: unknown; // any additional keys (must be compatible supertype)
}
```

---

## 16. Declaration Merging

Interfaces support **declaration merging** — you can declare the same interface multiple times and TypeScript merges them:

```ts
// Original interface (e.g., from a library)
interface Request {
  url: string;
  method: string;
  headers: Record<string, string>;
}

// Your extension — merged with the original
interface Request {
  user?: AuthenticatedUser; // added by auth middleware
  requestId: string; // added by logging middleware
  startTime: number; // added for performance tracking
}

// Now Request has ALL properties from both declarations
function handleRequest(req: Request): void {
  req.url; // ✓ — from first declaration
  req.user?.id; // ✓ — from second declaration (optional)
  req.requestId; // ✓ — from second declaration
}
```

### Extending Global Types

```ts
// Extend Express's Request type globally
declare global {
  namespace Express {
    interface Request {
      user?: { id: string; role: string };
    }
  }
}

// Now in all Express route handlers:
app.get("/profile", (req, res) => {
  req.user?.id; // ✓ — TypeScript knows about this
});
```

---

## 17. Real-World OOP Patterns

### Repository Pattern — Full Example

```ts
// Interface first
interface IUserRepository {
  findById(id: string): Promise<User | null>;
  findByEmail(email: string): Promise<User | null>;
  findAll(params: UserQueryParams): Promise<PaginatedResult<User>>;
  create(data: CreateUserDTO): Promise<User>;
  update(id: string, data: UpdateUserDTO): Promise<User | null>;
  softDelete(id: string): Promise<boolean>;
  restore(id: string): Promise<boolean>;
}

// Implementation
class PostgresUserRepository implements IUserRepository {
  constructor(private readonly db: Pool) {}

  async findById(id: string): Promise<User | null> {
    const { rows } = await this.db.query(
      "SELECT * FROM users WHERE id = $1 AND deleted_at IS NULL",
      [id],
    );
    return rows[0] ? mapRowToUser(rows[0]) : null;
  }

  async findByEmail(email: string): Promise<User | null> {
    const { rows } = await this.db.query(
      "SELECT * FROM users WHERE email = $1 AND deleted_at IS NULL",
      [email.toLowerCase()],
    );
    return rows[0] ? mapRowToUser(rows[0]) : null;
  }

  async findAll(params: UserQueryParams): Promise<PaginatedResult<User>> {
    const { sql, countSql, queryParams } = buildUserQuery(params);
    const [dataResult, countResult] = await Promise.all([
      this.db.query(sql, queryParams),
      this.db.query(countSql, queryParams.slice(0, -2)), // exclude LIMIT/OFFSET params
    ]);
    return {
      data: dataResult.rows.map(mapRowToUser),
      total: parseInt(countResult.rows[0].count),
      page: params.page ?? 1,
      limit: params.limit ?? 20,
    };
  }

  async create(data: CreateUserDTO): Promise<User> {
    const id = generateId();
    const passwordHash = await bcrypt.hash(data.password, 12);
    const { rows } = await this.db.query(
      `INSERT INTO users (id, name, email, password_hash, role, is_active, created_at, updated_at)
       VALUES ($1,$2,$3,$4,$5,$6,NOW(),NOW()) RETURNING *`,
      [
        id,
        data.name,
        data.email.toLowerCase(),
        passwordHash,
        data.role ?? "user",
        true,
      ],
    );
    return mapRowToUser(rows[0]);
  }

  async update(id: string, data: UpdateUserDTO): Promise<User | null> {
    const sets = buildSetClause(data);
    if (sets.sql === "") return this.findById(id);
    const { rows } = await this.db.query(
      `UPDATE users SET ${sets.sql}, updated_at = NOW()
       WHERE id = $${sets.params.length + 1} AND deleted_at IS NULL RETURNING *`,
      [...sets.params, id],
    );
    return rows[0] ? mapRowToUser(rows[0]) : null;
  }

  async softDelete(id: string): Promise<boolean> {
    const { rowCount } = await this.db.query(
      "UPDATE users SET deleted_at = NOW() WHERE id = $1 AND deleted_at IS NULL",
      [id],
    );
    return rowCount > 0;
  }

  async restore(id: string): Promise<boolean> {
    const { rowCount } = await this.db.query(
      "UPDATE users SET deleted_at = NULL WHERE id = $1 AND deleted_at IS NOT NULL",
      [id],
    );
    return rowCount > 0;
  }
}

// Service layer — depends on interface, not implementation
class UserService {
  constructor(
    private readonly userRepo: IUserRepository, // ← depends on abstraction
    private readonly emailService: IEmailService,
    private readonly eventBus: IEventBus,
  ) {}

  async createUser(data: CreateUserDTO): Promise<UserResponseDTO> {
    const existing = await this.userRepo.findByEmail(data.email);
    if (existing) throw new ConflictError("Email already registered");

    const user = await this.userRepo.create(data);

    // Fire and forget — non-critical
    this.emailService
      .sendWelcomeEmail(user)
      .catch((err) => logger.error("Welcome email failed", err));
    this.eventBus.emit("user.created", { userId: user.id }).catch(() => {});

    return omit(user, "passwordHash");
  }
}
```

---

## Summary Cheat Sheet

```
Access modifiers:
  public    — default, accessible everywhere
  private   — only within the class body (compile-time check)
  #field    — true JavaScript private (runtime check too)
  protected — class + subclasses only

Class features:
  Parameter properties: constructor(private readonly db: DB) {}
  readonly: set in constructor only
  static: belongs to class, not instances
  abstract: must be overridden by subclasses
  override keyword: explicit method override (TS 4.3+)
  get/set: computed properties with access control

Interfaces:
  Define structural contracts — shape, not implementation
  Can extend multiple interfaces with `extends`
  Classes implement with `implements`
  Support declaration merging (same interface declared twice)
  Function signatures, index signatures supported

Type vs Interface:
  type:      unions, primitives, tuples, mapped/conditional types
  interface: class contracts, extendable shapes, declaration merging

Design patterns:
  Repository: interface IRepo → concrete PostgresRepo
  Service:    depends on IRepo (interface), not implementation
  Singleton:  private static instance + static getInstance()
  Abstract:   BaseRepository<T> with abstract findById(), findAll(), etc.
```

---

_Next: [Part 4 — Generics & Type Narrowing](./ts-part-4-generics-narrowing.md)_
