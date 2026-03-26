> **Part 6 of 7.** JavaScript's OOP model — `this` in full depth, factory functions, constructor functions, `Object.create`, the prototype chain, prototypal inheritance, classes, and the four pillars: encapsulation, abstraction, inheritance, polymorphism.

---

## Table of Contents

1. [OOP Introduction](#1-oop-introduction)
2. [The `this` Keyword — Full Deep Dive](#2-the-this-keyword--full-deep-dive)
3. [Factory Functions](#3-factory-functions)
4. [Constructor Functions](#4-constructor-functions)
5. [Built-In Constructors](#5-built-in-constructors)
6. [Object.create Method](#6-objectcreate-method)
7. [The Prototype Chain](#7-the-prototype-chain)
8. [Prototypal Inheritance](#8-prototypal-inheritance)
9. [Introduction to Classes](#9-introduction-to-classes)
10. [Access Modifiers — Private Fields & Methods](#10-access-modifiers--private-fields--methods)
11. [Encapsulation](#11-encapsulation)
12. [Abstraction](#12-abstraction)
13. [Inheritance](#13-inheritance)
14. [Polymorphism](#14-polymorphism)

---

## 1. OOP Introduction

**Object-Oriented Programming** is a paradigm where you model your application as a collection of objects that have **state (data)** and **behavior (methods)**, and that interact with each other.

### The Four Pillars of OOP

1. **Encapsulation** — bundle related data and behavior, hide internal details
2. **Abstraction** — expose a clean interface, hide implementation complexity
3. **Inheritance** — derive new types from existing ones (share and extend behavior)
4. **Polymorphism** — different objects respond to the same interface differently

### JavaScript's OOP Model

JavaScript uses **prototypal inheritance**, not classical inheritance like Java or C++. Objects inherit directly from other objects — there is no distinction between "class" and "instance" at the engine level.

ES6 **classes** are syntactic sugar over prototypal inheritance — they compile down to constructor functions and prototype chains. Understanding the prototype model is essential for understanding why classes behave the way they do.

---

## 2. The `this` Keyword — Full Deep Dive

`this` is the **execution context** — it refers to the object that "owns" the current call. Unlike most languages, `this` in JavaScript is not determined when a function is defined — it's determined **when the function is called**.

### Rule 1: Global Context

```js
// In a browser, global `this` is `window`
// In Node.js module, global `this` is `module.exports` (initially {})
// In Node.js global scope (non-module), `this` is `global`
console.log(this); // window (browser), {} (Node module)

// In strict mode, global `this` inside a function is undefined
"use strict";
function fn() {
  console.log(this); // undefined
}
fn();
```

### Rule 2: Implicit Binding (Method Calls)

When a function is called as a method of an object, `this` is the object:

```js
const user = {
  name: "Alice",
  greet() {
    return `Hello, I'm ${this.name}`; // this = user
  },
};

user.greet(); // "Hello, I'm Alice" ✓

// The key is HOW you call it, not WHERE the function lives
const greetFn = user.greet;
greetFn(); // "Hello, I'm undefined" — this = undefined (strict) or window!
```

### Rule 3: Explicit Binding (call, apply, bind)

```js
function introduce(greeting, punctuation) {
  return `${greeting}, I'm ${this.name}${punctuation}`;
}

const alice = { name: "Alice" };
const bob   = { name: "Bob"   };

// .call(thisArg, arg1, arg2, ...)
introduce.call(alice, "Hello", "!");  // "Hello, I'm Alice!"
introduce.call(bob, "Hi", ".");       // "Hi, I'm Bob."

// .apply(thisArg, [args]) — useful when args are already in an array
introduce.apply(alice, ["Hey", "?"]);

// .bind(thisArg, arg1, ...) — returns a new function, doesn't call it
const introduceAlice = introduce.bind(alice, "Hello");
introduceAlice("!"); // "Hello, I'm Alice!"
introduceAlice("."); // "Hello, I'm Alice."
```

### Rule 4: `new` Binding

When called with `new`, `this` is the newly created object:

```js
function User(name, email) {
  // 1. A new empty object is created: {}
  // 2. this is set to that new object
  this.name = name;
  this.email = email;
  // 3. The new object is returned automatically (if no explicit return)
}

const alice = new User("Alice", "alice@example.com");
alice.name; // "Alice"
```

### Rule 5: Arrow Function — No Own `this`

Arrow functions do not have their own `this`. They inherit `this` from the enclosing lexical scope at definition time:

```js
const user = {
  name: "Alice",

  // Regular function: `this` depends on how it's called
  regularGreet: function() {
    return `I'm ${this.name}`; // this = user when called as user.regularGreet()
  },

  // Arrow function: `this` is from lexical scope (not user!)
  arrowGreet: () => {
    return `I'm ${this.name}`; // this = global/undefined — NOT user!
  },

  // Arrow inside a regular method: `this` = user ✓
  startTimer: function() {
    setInterval(() => {
      console.log(`${this.name} is still here`); // this = user ✓
    }, 1000);
  },
};
```

### `this` Priority (Highest to Lowest)

```
1. new binding        — new Fn() → this is new object
2. Explicit binding   — call/apply/bind → this is specified arg
3. Implicit binding   — obj.method() → this is obj
4. Default binding    — fn() → this is undefined (strict) or global
```

Arrow functions do not participate in this hierarchy — they use the enclosing scope's `this`.

### Real-World: Class Context Issues

```js
class ApiClient {
  constructor(baseUrl) {
    this.baseUrl = baseUrl;
    this.pendingRequests = 0;
  }

  // Problem: method used as callback loses `this`
  async fetchUser(id) {
    this.pendingRequests++;
    const data = await fetch(`${this.baseUrl}/users/${id}`).then(r => r.json());
    this.pendingRequests--;
    return data;
  }
}

const client = new ApiClient("https://api.example.com");

// ❌ Loses `this` — fetchUser is passed as a value, not called on client
button.addEventListener("click", client.fetchUser); // this.baseUrl = undefined

// ✅ Fixes:
button.addEventListener("click", () => client.fetchUser(userId)); // Arrow wrapper
button.addEventListener("click", client.fetchUser.bind(client));  // bind
```

---

## 3. Factory Functions

A **factory function** is a regular function that creates and returns an object. It's one pattern for creating multiple instances of similar objects without `new`.

```js
function createUser(name, email, role = "user") {
  // Private via closure
  let loginAttempts = 0;
  const createdAt = new Date();

  return {
    // Public interface
    name,
    email,
    role,

    login(password) {
      if (loginAttempts >= 5) throw new Error("Account locked");
      const success = verifyPassword(email, password);
      if (!success) loginAttempts++;
      return success;
    },

    resetLoginAttempts() {
      loginAttempts = 0;
    },

    getInfo() {
      return {
        name: this.name,
        email: this.email,
        role: this.role,
        memberSince: createdAt,
        // loginAttempts is NOT exposed — private via closure
      };
    },
  };
}

const alice = createUser("Alice", "alice@example.com", "admin");
const bob   = createUser("Bob",   "bob@example.com");

alice.role;           // "admin"
alice.loginAttempts;  // undefined — private!
alice.getInfo();      // { name, email, role, memberSince }
```

### Pros and Cons of Factory Functions

**Pros:**

- Simple, no `new` required
- True private state via closures
- Each instance has its own copy of methods (memory consideration)
- Works well with destructuring: `const { login, getInfo } = createUser(...)` (no `this` context issues)

**Cons:**

- Each instance gets its own copy of all methods (uses more memory than prototype)
- Can't use `instanceof`
- No prototype chain — can't do inheritance as cleanly

---

## 4. Constructor Functions

A constructor function is called with `new`. By convention, constructor functions are PascalCase.

```js
function User(name, email, role = "user") {
  // `this` is the new object being created
  this.name = name;
  this.email = email;
  this.role = role;
  this.createdAt = new Date();
}

// Methods go on the PROTOTYPE — shared by all instances (memory efficient)
User.prototype.getInfo = function() {
  return {
    name: this.name,
    email: this.email,
    role: this.role,
  };
};

User.prototype.toString = function() {
  return `User(${this.name}, ${this.email})`;
};

const alice = new User("Alice", "alice@example.com", "admin");
const bob   = new User("Bob",   "bob@example.com");

alice.getInfo();             // { name: "Alice", ... }
alice instanceof User;       // true
alice.constructor === User;  // true

// alice and bob SHARE the same getInfo function — it's on User.prototype
alice.getInfo === bob.getInfo; // true
```

### What `new` Does Step by Step

```js
// When you write: const alice = new User("Alice", "alice@example.com")
// JavaScript does:

// 1. Create a new empty object
const obj = {};

// 2. Set its prototype to Constructor.prototype
Object.setPrototypeOf(obj, User.prototype);
// or: obj.__proto__ = User.prototype;

// 3. Call the constructor with `this` = obj
User.call(obj, "Alice", "alice@example.com");

// 4. Return obj (unless constructor explicitly returns a non-primitive)
const alice = obj;
```

### Constructor Without `new` — A Common Bug

```js
// Without new: `this` is undefined (strict) or global
const oops = User("Alice", "alice@example.com");
// In strict mode: TypeError (cannot set property of undefined)
// In sloppy mode: pollutes the global scope!

// Protection pattern
function User(name) {
  if (!(this instanceof User)) {
    return new User(name); // auto-correct if called without new
  }
  this.name = name;
}
```

---

## 5. Built-In Constructors

JavaScript has several built-in constructors. Understanding them clarifies how everything in JS is an object (except primitives).

```js
// These all create wrapper objects — usually just use literals instead
const str = new String("hello");  // String object (not primitive)
const num = new Number(42);       // Number object
const bool = new Boolean(true);   // Boolean object

typeof str;          // "object" — not "string"!
str === "hello";     // false! Object !== primitive
"hello" === "hello"; // true

// Autoboxing: JS temporarily wraps primitives as objects when you call methods
"hello".toUpperCase(); // JS temporarily creates a String object for this call
(42).toString();       // same for Number

// These constructors ARE useful:
const arr  = new Array(3);     // empty array of length 3 (but use [] instead)
const obj  = new Object();     // {} is cleaner
const date = new Date();       // no literal syntax for Date — must use constructor
const re   = new RegExp("\\d+", "g"); // or use /\d+/g literal
const map  = new Map();
const set  = new Set();
const err  = new Error("Something went wrong");
```

### Custom Error Classes

A very common real-world use of classes extending built-ins:

```js
class AppError extends Error {
  constructor(message, statusCode = 500, code = "INTERNAL_ERROR") {
    super(message);
    this.name = this.constructor.name;
    this.statusCode = statusCode;
    this.code = code;
    this.isOperational = true; // vs programming errors
    Error.captureStackTrace(this, this.constructor);
  }
}

class NotFoundError extends AppError {
  constructor(resource, id) {
    super(`${resource} with id "${id}" not found`, 404, "NOT_FOUND");
    this.resource = resource;
    this.resourceId = id;
  }
}

class UnauthorizedError extends AppError {
  constructor(message = "Authentication required") {
    super(message, 401, "UNAUTHORIZED");
  }
}

class ForbiddenError extends AppError {
  constructor(action, resource) {
    super(`You don't have permission to ${action} ${resource}`, 403, "FORBIDDEN");
  }
}

class ValidationError extends AppError {
  constructor(fields) {
    super("Validation failed", 400, "VALIDATION_ERROR");
    this.fields = fields; // [{ field: "email", message: "Invalid format" }]
  }
}

// Usage in NestJS-like service
async function getUserById(id) {
  const user = await db.users.findById(id);
  if (!user) throw new NotFoundError("User", id);
  if (!user.isActive) throw new ForbiddenError("access", "inactive account");
  return user;
}

// Global error handler
function handleError(err, req, res) {
  if (err instanceof AppError && err.isOperational) {
    return res.status(err.statusCode).json({
      error: { code: err.code, message: err.message, fields: err.fields },
    });
  }
  // Programming error — log and return generic message
  console.error("UNHANDLED ERROR:", err);
  return res.status(500).json({ error: { code: "INTERNAL_ERROR", message: "Server error" } });
}
```

---

## 6. Object.create Method

`Object.create(proto)` creates a new object with its prototype explicitly set to `proto`.

```js
const animalProto = {
  breathe() { return `${this.name} is breathing`; },
  eat(food) { return `${this.name} eats ${food}`; },
};

const dog = Object.create(animalProto);
dog.name = "Rex";
dog.breed = "Labrador";

dog.breathe(); // "Rex is breathing" — inherited from animalProto
dog.eat("kibble"); // "Rex eats kibble"

dog.__proto__ === animalProto; // true
Object.getPrototypeOf(dog) === animalProto; // true (preferred over __proto__)

// Object.create(null) — creates object with NO prototype
const pureMap = Object.create(null);
pureMap.key = "value";
"toString" in pureMap; // false — no inherited methods
// Useful as a safe hash map — no prototype pollution risk
```

### Using Object.create for Inheritance

```js
function Animal(name) {
  this.name = name;
}
Animal.prototype.speak = function() {
  return `${this.name} makes a sound`;
};

function Dog(name, breed) {
  Animal.call(this, name); // call parent constructor
  this.breed = breed;
}

// Set up prototype chain: Dog.prototype → Animal.prototype
Dog.prototype = Object.create(Animal.prototype);
Dog.prototype.constructor = Dog; // restore constructor

Dog.prototype.speak = function() {
  return `${this.name} barks`;
};

const rex = new Dog("Rex", "Labrador");
rex.speak();           // "Rex barks" — overrides Animal's version
rex instanceof Dog;    // true
rex instanceof Animal; // true — prototype chain!
```

---

## 7. The Prototype Chain

Every object in JavaScript has a hidden `[[Prototype]]` reference to another object. This forms a chain that ends at `Object.prototype`, whose `[[Prototype]]` is `null`.

```
rex
  .__proto__ → Dog.prototype
                  .speak()
                  .__proto__ → Animal.prototype
                                  .breathe()
                                  .__proto__ → Object.prototype
                                                  .toString()
                                                  .hasOwnProperty()
                                                  .__proto__ → null
```

### Property Lookup

When you access a property on an object, JavaScript:

1. Checks the object itself (own properties)
2. Checks `[[Prototype]]` (one level up)
3. Continues up the chain until found or `null` is reached (→ `undefined`)

```js
const alice = new User("Alice", "alice@example.com");

// Own property lookup
alice.name;         // found on alice directly — done

// Prototype lookup
alice.getInfo();    // not on alice → look in User.prototype → found!

// Object.prototype lookup
alice.toString();   // not on alice → User.prototype → Object.prototype → found!

// Not found anywhere
alice.flyToMoon;    // null chain reached → undefined
```

### Prototype Performance Note

This lookup happens for EVERY property access on EVERY property that isn't own. Modern JS engines optimize this heavily (hidden classes, inline caches), but avoiding unnecessarily deep prototype chains is good practice.

### `hasOwnProperty` vs `in`

```js
"name" in alice;             // true — own property
"getInfo" in alice;          // true — inherited from prototype
"flyToMoon" in alice;        // false — not anywhere in chain

Object.hasOwn(alice, "name");    // true — only own
Object.hasOwn(alice, "getInfo"); // false — it's on the prototype
```

---

## 8. Prototypal Inheritance

Building inheritance chains using constructor functions and Object.create.

```js
// Base "class" — Permission Entity
function PermissionEntity(id, permissions = []) {
  this.id = id;
  this._permissions = new Set(permissions);
}

PermissionEntity.prototype.can = function(permission) {
  return this._permissions.has(permission) || this._permissions.has("*");
};

PermissionEntity.prototype.addPermission = function(perm) {
  this._permissions.add(perm);
  return this;
};

// "Subclass" — User
function User(id, name, email, permissions) {
  PermissionEntity.call(this, id, permissions); // super()
  this.name = name;
  this.email = email;
  this.createdAt = new Date();
}
User.prototype = Object.create(PermissionEntity.prototype);
User.prototype.constructor = User;

User.prototype.getProfile = function() {
  return { id: this.id, name: this.name, email: this.email };
};

// "Subclass" — AdminUser
function AdminUser(id, name, email) {
  User.call(this, id, name, email, ["*"]); // admin gets all permissions
  this.isAdmin = true;
}
AdminUser.prototype = Object.create(User.prototype);
AdminUser.prototype.constructor = AdminUser;

AdminUser.prototype.deleteUser = function(userId) {
  if (!this.can("*")) throw new Error("Insufficient permissions");
  return db.users.delete(userId);
};

const alice = new AdminUser("u1", "Alice", "alice@example.com");
alice.can("read");    // true — inherited through PermissionEntity
alice.getProfile();   // inherited through User
alice.deleteUser("u2"); // AdminUser's own method
alice instanceof AdminUser;       // true
alice instanceof User;            // true
alice instanceof PermissionEntity; // true
```

---

## 9. Introduction to Classes

ES6 classes are syntactic sugar over prototype-based OOP. They make the pattern cleaner and more readable, but do NOT change how inheritance works under the hood.

```js
class User {
  // Class fields (ES2022) — declared at class level
  role = "user";             // public field, initialized per instance
  createdAt = new Date();    // same

  constructor(name, email) {
    this.name = name;
    this.email = email;
  }

  // Instance method — goes on User.prototype
  getProfile() {
    return {
      name: this.name,
      email: this.email,
      role: this.role,
    };
  }

  // Static method — called on the class, not instances
  static create(name, email) {
    return new User(name, email);
  }

  // Getter
  get displayName() {
    return `${this.name} (${this.email})`;
  }

  // Setter
  set displayName(value) {
    const [name, rest] = value.split(" (");
    this.name = name;
    // etc.
  }

  // toString — called by string coercion
  toString() {
    return `User(${this.name})`;
  }
}

const alice = User.create("Alice", "alice@example.com");
alice.displayName;    // "Alice (alice@example.com)"
`${alice}`;           // "User(Alice)"
alice instanceof User; // true
```

### Class Inheritance with `extends` and `super`

```js
class Animal {
  constructor(name, sound) {
    this.name = name;
    this.sound = sound;
  }

  speak() {
    return `${this.name} says ${this.sound}`;
  }

  toString() {
    return `Animal(${this.name})`;
  }
}

class Dog extends Animal {
  constructor(name, breed) {
    super(name, "woof"); // MUST call super() before using `this`
    this.breed = breed;
  }

  // Override parent method
  speak() {
    const base = super.speak(); // call parent method
    return `${base}! (${this.breed})`;
  }

  fetch(item) {
    return `${this.name} fetches the ${item}`;
  }
}

const rex = new Dog("Rex", "Labrador");
rex.speak();          // "Rex says woof! (Labrador)"
rex.fetch("ball");    // "Rex fetches the ball"
rex instanceof Dog;   // true
rex instanceof Animal; // true
```

### Static Members

```js
class MathUtils {
  static PI = 3.14159265358979;

  static circleArea(radius) {
    return MathUtils.PI * radius ** 2;
  }

  static degreesToRadians(degrees) {
    return (degrees * Math.PI) / 180;
  }
}

MathUtils.circleArea(5); // 78.53...
// new MathUtils() is pointless — all methods are static

// Real-world: Repository pattern
class UserRepository {
  static #instance = null;
  #db;

  constructor(db) {
    this.#db = db;
  }

  // Singleton pattern
  static getInstance(db) {
    if (!UserRepository.#instance) {
      UserRepository.#instance = new UserRepository(db);
    }
    return UserRepository.#instance;
  }

  async findById(id) {
    return this.#db.query("SELECT * FROM users WHERE id = $1", [id]);
  }

  async findByEmail(email) {
    return this.#db.query("SELECT * FROM users WHERE email = $1", [email]);
  }
}
```

---

## 10. Access Modifiers — Private Fields & Methods

ES2022 introduced true private class fields using the `#` prefix.

```js
class BankAccount {
  // Private fields — accessible ONLY within the class body
  #balance;
  #transactionHistory = [];
  #pin;

  // Public field
  owner;

  constructor(owner, initialBalance, pin) {
    this.owner = owner;
    this.#balance = initialBalance;
    this.#pin = pin;
  }

  // Private method
  #validatePin(pin) {
    return pin === this.#pin;
  }

  #recordTransaction(type, amount) {
    this.#transactionHistory.push({
      type,
      amount,
      balance: this.#balance,
      timestamp: new Date(),
    });
  }

  // Public methods
  deposit(amount) {
    if (amount <= 0) throw new Error("Deposit must be positive");
    this.#balance += amount;
    this.#recordTransaction("deposit", amount);
    return this;
  }

  withdraw(amount, pin) {
    if (!this.#validatePin(pin)) throw new Error("Invalid PIN");
    if (amount > this.#balance) throw new Error("Insufficient funds");
    this.#balance -= amount;
    this.#recordTransaction("withdrawal", amount);
    return this;
  }

  get balance() {
    return this.#balance;
  }

  getStatement() {
    return [...this.#transactionHistory]; // copy — don't expose mutable internal
  }
}

const account = new BankAccount("Alice", 1000, "1234");
account.balance;               // 1000 (via getter)
account.#balance;              // SyntaxError — private field!
account.deposit(500).withdraw(200, "1234");
account.balance;               // 1300
```

---

## 11. Encapsulation

**Encapsulation** = bundling data + behavior, and restricting direct access to internal state. The outside world interacts only through the public interface.

```js
class ShoppingCart {
  #items = new Map(); // productId → { product, quantity }
  #couponCode = null;
  #discountPercent = 0;

  addItem(product, quantity = 1) {
    if (quantity <= 0) throw new Error("Quantity must be positive");

    const existing = this.#items.get(product.id);
    if (existing) {
      existing.quantity += quantity;
    } else {
      this.#items.set(product.id, { product, quantity });
    }

    return this; // chainable
  }

  removeItem(productId) {
    if (!this.#items.has(productId)) throw new Error("Item not in cart");
    this.#items.delete(productId);
    return this;
  }

  updateQuantity(productId, quantity) {
    if (quantity <= 0) return this.removeItem(productId);
    const item = this.#items.get(productId);
    if (!item) throw new Error("Item not in cart");
    item.quantity = quantity;
    return this;
  }

  applyCoupon(code) {
    const discounts = { SAVE10: 10, SAVE20: 20, HALFOFF: 50 };
    if (!discounts[code]) throw new Error("Invalid coupon code");
    this.#couponCode = code;
    this.#discountPercent = discounts[code];
    return this;
  }

  // Private calculation — implementation detail
  #calculateSubtotal() {
    let subtotal = 0;
    for (const { product, quantity } of this.#items.values()) {
      subtotal += product.price * quantity;
    }
    return subtotal;
  }

  // Public computed properties
  get itemCount() {
    let count = 0;
    for (const { quantity } of this.#items.values()) count += quantity;
    return count;
  }

  get subtotal() { return this.#calculateSubtotal(); }

  get discount() {
    return this.#calculateSubtotal() * (this.#discountPercent / 100);
  }

  get total() { return this.subtotal - this.discount; }

  getSummary() {
    return {
      items: [...this.#items.values()].map(({ product, quantity }) => ({
        id: product.id,
        name: product.name,
        price: product.price,
        quantity,
        lineTotal: product.price * quantity,
      })),
      couponCode: this.#couponCode,
      subtotal: this.subtotal,
      discount: this.discount,
      total: this.total,
    };
  }
}

const cart = new ShoppingCart();
cart
  .addItem({ id: "p1", name: "Laptop", price: 999 })
  .addItem({ id: "p2", name: "Mouse",  price: 49 }, 2)
  .applyCoupon("SAVE10");

cart.total;       // 979.20
cart.#items;      // SyntaxError — private!
```

---

## 12. Abstraction

**Abstraction** = hide complex implementation, expose a simple interface. Users of your class don't need to know HOW it works — just WHAT it does.

```js
class AuthService {
  #tokenStore;
  #refreshTimer = null;

  constructor(tokenStore) {
    this.#tokenStore = tokenStore;
  }

  // Simple public interface — hide all the JWT/crypto complexity
  async login(email, password) {
    const tokens = await this.#authenticate(email, password);
    this.#storeTokens(tokens);
    this.#scheduleRefresh(tokens.expiresIn);
    return { success: true, user: tokens.user };
  }

  async logout() {
    const token = this.#tokenStore.get("refreshToken");
    if (token) await this.#revokeToken(token);
    this.#clearTokens();
    if (this.#refreshTimer) clearTimeout(this.#refreshTimer);
  }

  async getAuthHeader() {
    const token = this.#tokenStore.get("accessToken");
    const expiry = this.#tokenStore.get("tokenExpiry");

    if (Date.now() > Number(expiry) - 60_000) {
      await this.#refreshTokens(); // auto-refresh before expiry
    }

    return `Bearer ${this.#tokenStore.get("accessToken")}`;
  }

  get isAuthenticated() {
    const token = this.#tokenStore.get("accessToken");
    const expiry = this.#tokenStore.get("tokenExpiry");
    return !!token && Date.now() < Number(expiry);
  }

  // Private — complex internals hidden from users of this class
  async #authenticate(email, password) {
    const response = await fetch("/api/auth/login", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ email, password }),
    });
    if (!response.ok) throw new Error("Authentication failed");
    return response.json();
  }

  async #refreshTokens() {
    const refresh = this.#tokenStore.get("refreshToken");
    const response = await fetch("/api/auth/refresh", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ refreshToken: refresh }),
    });
    if (!response.ok) { await this.logout(); throw new Error("Session expired"); }
    this.#storeTokens(await response.json());
  }

  async #revokeToken(token) {
    await fetch("/api/auth/logout", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ refreshToken: token }),
    }).catch(() => {}); // best-effort
  }

  #storeTokens({ accessToken, refreshToken, expiresIn }) {
    this.#tokenStore.set("accessToken", accessToken);
    this.#tokenStore.set("refreshToken", refreshToken);
    this.#tokenStore.set("tokenExpiry", Date.now() + expiresIn * 1000);
  }

  #clearTokens() {
    this.#tokenStore.delete("accessToken");
    this.#tokenStore.delete("refreshToken");
    this.#tokenStore.delete("tokenExpiry");
  }

  #scheduleRefresh(expiresIn) {
    if (this.#refreshTimer) clearTimeout(this.#refreshTimer);
    const refreshAt = (expiresIn - 60) * 1000;
    this.#refreshTimer = setTimeout(() => this.#refreshTokens(), refreshAt);
  }
}
```

---

## 13. Inheritance

**Inheritance** allows a class to acquire properties and methods from a parent class, and extend or override them.

```js
// Base entity with common fields for database models
class BaseEntity {
  #id;
  #createdAt;
  #updatedAt;

  constructor(id = crypto.randomUUID()) {
    this.#id = id;
    this.#createdAt = new Date();
    this.#updatedAt = new Date();
  }

  get id()        { return this.#id; }
  get createdAt() { return this.#createdAt; }
  get updatedAt() { return this.#updatedAt; }

  touch() {
    this.#updatedAt = new Date();
    return this;
  }

  toJSON() {
    return {
      id: this.#id,
      createdAt: this.#createdAt.toISOString(),
      updatedAt: this.#updatedAt.toISOString(),
    };
  }
}

class User extends BaseEntity {
  #passwordHash;

  constructor({ name, email, passwordHash, id }) {
    super(id);
    this.name = name;
    this.email = email;
    this.role = "user";
    this.isActive = true;
    this.#passwordHash = passwordHash;
  }

  verifyPassword(hash) {
    return this.#passwordHash === hash; // simplified
  }

  deactivate() {
    this.isActive = false;
    this.touch();
    return this;
  }

  toJSON() {
    return {
      ...super.toJSON(), // include base fields
      name: this.name,
      email: this.email,
      role: this.role,
      isActive: this.isActive,
      // passwordHash intentionally NOT included
    };
  }
}

class AdminUser extends User {
  #managedDepartments = [];

  constructor(data) {
    super(data);
    this.role = "admin";
  }

  addManagedDepartment(dept) {
    this.#managedDepartments.push(dept);
    this.touch();
    return this;
  }

  toJSON() {
    return {
      ...super.toJSON(),
      managedDepartments: [...this.#managedDepartments],
    };
  }
}

const admin = new AdminUser({
  name: "Alice",
  email: "alice@example.com",
  passwordHash: "hashed_pw",
});

admin.id;                    // UUID from BaseEntity
admin.role;                  // "admin"
admin.addManagedDepartment("Engineering");
JSON.stringify(admin.toJSON(), null, 2);
// Includes base fields + user fields + admin fields, no password
```

---

## 14. Polymorphism

**Polymorphism** = many forms. Different classes can implement the same interface differently — code that uses the interface doesn't need to know the specific type.

### Method Overriding

```js
// Payment processor abstraction
class PaymentProcessor {
  constructor(config) {
    if (new.target === PaymentProcessor) {
      throw new Error("PaymentProcessor is abstract — instantiate a subclass");
    }
    this.config = config;
  }

  // Template method — defines the algorithm, delegates steps to subclasses
  async processPayment(order) {
    await this.validateOrder(order);
    const paymentData = this.buildPaymentPayload(order); // subclass implements
    const result = await this.executeCharge(paymentData); // subclass implements
    return this.normalizeResult(result); // subclass implements
  }

  async validateOrder(order) {
    if (!order.amount || order.amount <= 0) throw new Error("Invalid amount");
    if (!order.currency) throw new Error("Currency required");
  }

  // "Abstract" methods — must be overridden
  buildPaymentPayload(order) { throw new Error("buildPaymentPayload() not implemented"); }
  async executeCharge(payload) { throw new Error("executeCharge() not implemented"); }
  normalizeResult(result) { throw new Error("normalizeResult() not implemented"); }
}

class StripeProcessor extends PaymentProcessor {
  buildPaymentPayload(order) {
    return {
      amount: Math.round(order.amount * 100), // Stripe uses cents
      currency: order.currency.toLowerCase(),
      payment_method: order.paymentMethodId,
      description: order.description,
    };
  }

  async executeCharge(payload) {
    const stripe = require("stripe")(this.config.secretKey);
    return stripe.paymentIntents.create(payload);
  }

  normalizeResult(result) {
    return {
      success: result.status === "succeeded",
      transactionId: result.id,
      amount: result.amount / 100,
      currency: result.currency.toUpperCase(),
    };
  }
}

class PayPalProcessor extends PaymentProcessor {
  buildPaymentPayload(order) {
    return {
      intent: "CAPTURE",
      purchase_units: [{
        amount: { value: order.amount.toFixed(2), currency_code: order.currency },
        description: order.description,
      }],
    };
  }

  async executeCharge(payload) {
    // PayPal API call
    return paypalClient.orders.create(payload);
  }

  normalizeResult(result) {
    return {
      success: result.status === "COMPLETED",
      transactionId: result.id,
      amount: parseFloat(result.purchase_units[0].amount.value),
      currency: result.purchase_units[0].amount.currency_code,
    };
  }
}

// POLYMORPHISM IN ACTION:
// This function doesn't care if it's Stripe, PayPal, or any future processor
// It just calls the same interface and gets the same shape back
async function checkout(order, processor) {
  const result = await processor.processPayment(order);

  if (result.success) {
    await markOrderPaid(order.id, result.transactionId);
    return { success: true, transactionId: result.transactionId };
  } else {
    throw new Error("Payment failed");
  }
}

// Works with any PaymentProcessor subclass
const stripe = new StripeProcessor({ secretKey: process.env.STRIPE_KEY });
const paypal = new PayPalProcessor({ clientId: process.env.PAYPAL_ID });

await checkout(order, stripe);  // same function
await checkout(order, paypal);  // same function, different behavior
```

### Duck Typing Polymorphism

JavaScript doesn't require formal inheritance for polymorphism — any object with the right methods will work ("if it quacks like a duck"):

```js
// Different "renderers" with the same interface
const htmlRenderer = {
  render(data) { return `<table>${data.map(r => `<tr>${r.join("<td>")}</tr>`).join("")}</table>`; }
};

const csvRenderer = {
  render(data) { return data.map(row => row.join(",")).join("\n"); }
};

const jsonRenderer = {
  render(data) { return JSON.stringify(data, null, 2); }
};

// Works with any object that has a .render() method
function exportReport(data, renderer) {
  return renderer.render(data);
}

exportReport(reportData, htmlRenderer);
exportReport(reportData, csvRenderer);
exportReport(reportData, jsonRenderer);
```

---

## Summary Cheat Sheet

```
this:
  Default binding     fn() → undefined (strict) / global
  Implicit binding    obj.method() → this = obj
  Explicit binding    .call/.apply/.bind → specified object
  new binding         new Fn() → new empty object
  Arrow function      no own this — uses enclosing scope

Creation patterns:
  Factory function    no new, closure privacy, memory cost per instance
  Constructor fn      new, prototype-shared methods, memory efficient
  class               syntactic sugar, ES6+, extends/super, private fields (#)

Prototype chain:
  obj.__proto__ → Constructor.prototype → Object.prototype → null
  Property lookup walks chain upward
  hasOwnProperty / Object.hasOwn — check only own props

OOP Pillars:
  Encapsulation:  # private fields + public interface; getters/setters
  Abstraction:    hide complex internals, expose simple API
  Inheritance:    extends, super(), override methods
  Polymorphism:   same interface, different implementations

Patterns:
  Template Method: base class defines algorithm, subclasses implement steps
  Factory Method:  static create() for controlled instantiation
  Singleton:       static #instance for one shared instance
  Abstract class:  throw in constructor if new.target === BaseClass
```

---

_Next: [Part 7 — Asynchronous JavaScript & APIs](./part-7-async-and-apis.md)_