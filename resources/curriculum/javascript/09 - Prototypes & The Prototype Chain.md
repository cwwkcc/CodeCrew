> JavaScript's class syntax is beautiful and familiar. But it's a thin layer over something completely different from Java or C# classes. Understanding prototypes is understanding what JavaScript _actually_ does — and why it behaves the way it does when classes seem to break.

---

## Table of Contents

1. [The Core Idea: Objects Linked to Objects](#1-the-core-idea-objects-linked-to-objects)
2. [The Prototype Chain](#2-the-prototype-chain)
3. [**proto** vs prototype](#3-__proto__-vs-prototype)
4. [How Property Lookup Works](#4-how-property-lookup-works)
5. [Object.create — Explicit Prototype Setting](#5-objectcreate--explicit-prototype-setting)
6. [Constructor Functions — Pre-Class Pattern](#6-constructor-functions--pre-class-pattern)
7. [How ES6 Classes Work Under the Hood](#7-how-es6-classes-work-under-the-hood)
8. [Inheritance via the Prototype Chain](#8-inheritance-via-the-prototype-chain)
9. [hasOwnProperty vs in Operator](#9-hasownproperty-vs-in-operator)
10. [Common Prototype Mistakes](#10-common-prototype-mistakes)
11. [Practical Implications](#11-practical-implications)

---

## 1. The Core Idea: Objects Linked to Objects

In class-based languages (Java, C#), classes are blueprints and objects are instances of those blueprints. The class and the object are separate things.

In JavaScript, **there are no classes at the language level** (the `class` keyword is syntax sugar — more on that later). There are only objects. Objects can be linked to other objects. When you look up a property on an object, JavaScript walks the chain of linked objects until it finds it.

This linking mechanism is called the **prototype chain**.

```javascript
const animal = {
  breathe() {
    return "inhale... exhale...";
  },
};

const dog = {
  bark() {
    return "woof!";
  },
};

// Link dog to animal — dog's prototype is animal
Object.setPrototypeOf(dog, animal);

dog.bark(); // "woof!" — found on dog directly
dog.breathe(); // "inhale... exhale..." — not on dog, found on animal via prototype chain
dog.fly(); // TypeError: dog.fly is not a function — not found anywhere in the chain
```

---

## 2. The Prototype Chain

Every object in JavaScript has an internal slot called `[[Prototype]]` which points to another object (or `null` for the root). This forms the chain.

```
dog ──[[Prototype]]──▶ animal ──[[Prototype]]──▶ Object.prototype ──[[Prototype]]──▶ null

When you access dog.breathe():
  1. Look on dog itself           → not found
  2. Look on dog's prototype (animal) → found! return "inhale... exhale..."

When you access dog.toString():
  1. Look on dog               → not found
  2. Look on animal            → not found
  3. Look on Object.prototype  → found! (toString is defined here)

When you access dog.fly():
  1. Look on dog               → not found
  2. Look on animal            → not found
  3. Look on Object.prototype  → not found
  4. null — end of chain       → TypeError
```

**`Object.prototype`** is the root of almost every prototype chain. It provides the methods that all objects have:

```javascript
const obj = {};
obj.toString(); // "[object Object]"
obj.hasOwnProperty; // function — from Object.prototype
obj.valueOf(); // the object itself — from Object.prototype

// Proving the chain:
Object.getPrototypeOf(obj) === Object.prototype; // true
Object.getPrototypeOf(Object.prototype) === null; // true — end of chain
```

---

## 3. `__proto__` vs `prototype`

These two look similar and are related but are completely different things. This is one of the most confusing parts of JavaScript.

### `__proto__` (dunder proto)

This is the **instance's link** to its prototype. Every object has it. It's the actual `[[Prototype]]` internal slot, exposed as a property.

```javascript
const dog = {};
dog.__proto__ === Object.prototype; // true

// Modern equivalent (prefer these over __proto__):
Object.getPrototypeOf(dog) === Object.prototype; // true
Object.setPrototypeOf(dog, animal); // sets the prototype
```

`__proto__` is a legacy accessor property. Use `Object.getPrototypeOf` and `Object.setPrototypeOf` in production code.

### `prototype`

This is a property on **constructor functions and classes**. It's the object that will become the `__proto__` of instances created with `new`.

```javascript
function Dog(name) {
  this.name = name;
}

Dog.prototype.bark = function () {
  return `${this.name} says woof!`;
};

const rex = new Dog("Rex");

// What `new` does:
// 1. Creates an empty object: {}
// 2. Sets its __proto__ to Dog.prototype
// 3. Calls Dog() with `this` = the new object
// 4. Returns the new object

rex.__proto__ === Dog.prototype; // true
rex.bark(); // "Rex says woof!" — found on Dog.prototype
```

```
Visualising:

Dog (constructor function)
  .prototype ──────────────────────▶ { bark: fn, constructor: Dog }
                                              ↑
rex (instance)                               │
  .name = "Rex"                              │
  .__proto__ ────────────────────────────────┘
```

---

## 4. How Property Lookup Works

```javascript
const base = {
  type: "base",
  greet() {
    return "Hello from base";
  },
};

const child = Object.create(base);
child.type = "child"; // shadows base.type

const grandchild = Object.create(child);

// Lookups on grandchild:
grandchild.type; // "child" — found on child (shadows base.type)
grandchild.greet(); // "Hello from base" — walks up: grandchild → child → base ✓

// Setting a property always sets it on the object DIRECTLY
grandchild.type = "grandchild"; // does NOT modify child.type
grandchild.type; // "grandchild" — own property
child.type; // "child" — unchanged
```

### Shadowing

When an object defines a property with the same name as one on its prototype chain, the object's own property **shadows** the prototype's property.

```javascript
function Animal(name) {
  this.name = name;
}

Animal.prototype.toString = function () {
  return `Animal: ${this.name}`;
};

const cat = new Animal("Whiskers");
cat.toString(); // "Animal: Whiskers" — shadows Object.prototype.toString

// Prototype chain lookup order:
// 1. cat's own properties
// 2. Animal.prototype
// 3. Object.prototype
// → Animal.prototype.toString found at step 2 — never reaches Object.prototype.toString
```

---

## 5. `Object.create` — Explicit Prototype Setting

`Object.create(proto)` creates a new object with `proto` as its `[[Prototype]]`. It's the most explicit way to set up prototype relationships.

```javascript
const vehicleProto = {
  start() {
    return `${this.make} ${this.model} starting...`;
  },
  stop() {
    return `${this.make} ${this.model} stopping`;
  },
};

const carProto = Object.create(vehicleProto);
carProto.honk = function () {
  return "beep!";
};

// Creating instances
const myCar = Object.create(carProto);
myCar.make = "Toyota";
myCar.model = "Corolla";

myCar.start(); // "Toyota Corolla starting..."  — from vehicleProto
myCar.honk(); // "beep!"                       — from carProto
```

```javascript
// Object.create(null) — no prototype at all
// Creates an object with NO prototype chain
const pureMap = Object.create(null);
pureMap.key = "value";
pureMap.toString; // undefined — no Object.prototype methods!

// Useful as a clean key-value store with no inherited properties
// (avoids issues where "constructor", "toString" etc. are valid keys)
// Libraries like Lodash use this for internal caches
```

---

## 6. Constructor Functions — Pre-Class Pattern

Before ES6 classes, constructors + prototypes was the standard pattern. You'll see this in older codebases and it's still valid JS.

```javascript
function Person(name, email) {
  // `this` is the new object being constructed
  this.name = name;
  this.email = email;
}

// Methods are added to .prototype — shared by all instances
// NOT added to `this` — that would create a new function per instance
Person.prototype.greet = function () {
  return `Hi, I'm ${this.name}`;
};

Person.prototype.toJSON = function () {
  return { name: this.name, email: this.email };
};

const alice = new Person("Alice", "alice@example.com");
const bob = new Person("Bob", "bob@example.com");

alice.greet(); // "Hi, I'm Alice"
bob.greet(); // "Hi, I'm Bob"

// Both instances share the SAME greet function from Person.prototype
alice.greet === bob.greet; // true — same function reference
```

### What `new` Actually Does

```javascript
// What `new Person("Alice", "alice@example.com")` does:

function new_simulation(Constructor, ...args) {
  // 1. Create a new empty object
  const obj = {};

  // 2. Set its prototype to Constructor.prototype
  Object.setPrototypeOf(obj, Constructor.prototype);

  // 3. Call the constructor with `this` = the new object
  const result = Constructor.apply(obj, args);

  // 4. Return the new object (or the constructor's return value if it's an object)
  return result instanceof Object ? result : obj;
}
```

```javascript
// If you forget `new`:
const alice = Person("Alice", "alice@example.com"); // No `new`
// `this` inside Person becomes the global object (or undefined in strict mode)
// `alice` is undefined — Person() returns nothing
// window.name = "Alice"  ← global pollution (in sloppy mode)!

// Defensive pattern for forgetting new (older code):
function Person(name) {
  if (!(this instanceof Person)) {
    return new Person(name); // called without new — fix it
  }
  this.name = name;
}
```

---

## 7. How ES6 Classes Work Under the Hood

ES6 `class` is **syntactic sugar** over constructor functions and prototypes. It compiles down to the same prototype-based code. Understanding this explains all the "quirks" of classes.

```javascript
// ES6 class syntax:
class Animal {
  constructor(name) {
    this.name = name;
  }

  speak() {
    return `${this.name} makes a sound`;
  }

  static create(name) {
    return new Animal(name);
  }
}

// What JavaScript actually does (roughly):
function Animal(name) {
  this.name = name;
}

Animal.prototype.speak = function () {
  return `${this.name} makes a sound`;
};

Animal.create = function (name) {
  // static → on the constructor, not prototype
  return new Animal(name);
};
```

```javascript
// Proving they're the same:
class Dog {
  constructor(name) {
    this.name = name;
  }
  bark() {
    return "woof!";
  }
}

typeof Dog; // "function" — class IS a function
Dog.prototype.bark; // function — method is on prototype
Object.getPrototypeOf(Dog.prototype); // Object.prototype
```

### Class Fields (ES2022)

```javascript
class Counter {
  // Public class field — added to each instance (like this.count = 0 in constructor)
  count = 0;

  // Private class field — truly private, not on prototype
  #secret = "hidden";

  increment() {
    this.count++;
    this.#secret = "still hidden";
  }

  getSecret() {
    return this.#secret; // accessible inside the class
  }
}

const c = new Counter();
c.count; // 0 — accessible
c.#secret; // SyntaxError — genuinely private, not just convention
```

---

## 8. Inheritance via the Prototype Chain

```javascript
class Animal {
  constructor(name) {
    this.name = name;
  }

  speak() {
    return `${this.name} makes a sound`;
  }
}

class Dog extends Animal {
  constructor(name, breed) {
    super(name); // MUST call super() before using `this`
    this.breed = breed;
  }

  // Overrides Animal.speak
  speak() {
    return `${this.name} barks`;
  }

  // Calls parent method
  describe() {
    return `${super.speak()} and is a ${this.breed}`;
  }
}
```

What `extends` does to the prototype chain:

```
Dog.prototype ──[[Prototype]]──▶ Animal.prototype ──[[Prototype]]──▶ Object.prototype

rex (Dog instance)
  .__proto__ = Dog.prototype
  Dog.prototype has: speak (overridden), describe
  Animal.prototype has: speak (original — shadowed by Dog's)
  Object.prototype has: toString, hasOwnProperty, etc.
```

```javascript
const rex = new Dog("Rex", "Labrador");

rex.speak(); // "Rex barks"            — Dog.prototype.speak
rex.describe(); // "Rex makes a sound and is a Labrador" — super.speak() → Animal.prototype.speak
rex.toString(); // "[object Object]"       — Object.prototype.toString

rex instanceof Dog; // true
rex instanceof Animal; // true — checks the entire prototype chain
rex instanceof Object; // true — always true for objects
```

---

## 9. `hasOwnProperty` vs `in` Operator

```javascript
const parent = { inherited: true };
const child = Object.create(parent);
child.own = true;

// `in` operator — checks the ENTIRE prototype chain
"own" in child; // true — own property
"inherited" in child; // true — found on parent
"missing" in child; // false — not found anywhere

// hasOwnProperty — checks ONLY own properties (not the chain)
child.hasOwnProperty("own"); // true
child.hasOwnProperty("inherited"); // false — it's on parent, not child
child.hasOwnProperty("missing"); // false

// Modern equivalent: Object.hasOwn (ES2022 — avoids edge cases)
Object.hasOwn(child, "own"); // true
Object.hasOwn(child, "inherited"); // false

// Why Object.hasOwn is safer:
const obj = Object.create(null); // no prototype — no hasOwnProperty!
obj.key = "value";
obj.hasOwnProperty("key"); // TypeError — hasOwnProperty doesn't exist
Object.hasOwn(obj, "key"); // true — works on null-prototype objects
```

### Iterating: for...in and Own Properties

```javascript
const parent = { a: 1 };
const child = Object.create(parent);
child.b = 2;
child.c = 3;

// for...in iterates ALL enumerable properties, including inherited
for (const key in child) {
  console.log(key); // b, c, a (order may vary; `a` is from parent)
}

// To iterate only own properties:
for (const key in child) {
  if (Object.hasOwn(child, key)) {
    console.log(key); // b, c (only own)
  }
}

// Or use Object.keys — always only own enumerable properties
Object.keys(child); // ["b", "c"]
Object.values(child); // [2, 3]
Object.entries(child); // [["b", 2], ["c", 3]]
```

---

## 10. Common Prototype Mistakes

### Mistake 1: Shared mutable state on prototype

```javascript
// BUG: array on prototype is SHARED between all instances
function Team() {}
Team.prototype.members = []; // ← this is shared!

const team1 = new Team();
const team2 = new Team();

team1.members.push("Alice");
console.log(team2.members); // ["Alice"] — SAME array!

// FIX: initialise in constructor
function Team() {
  this.members = []; // each instance gets its OWN array
}
```

### Mistake 2: Losing `this` context

```javascript
class Timer {
  constructor() {
    this.count = 0;
  }

  tick() {
    this.count++;
    console.log(this.count);
  }
}

const timer = new Timer();

// BUG: `this` is lost when method is passed as a callback
setTimeout(timer.tick, 1000); // `this` is undefined (strict) or global

// FIX 1: bind
setTimeout(timer.tick.bind(timer), 1000);

// FIX 2: arrow function wrapper
setTimeout(() => timer.tick(), 1000);

// FIX 3: class field with arrow function (bound at construction)
class TimerFixed {
  count = 0;
  tick = () => {
    // arrow function — `this` is lexically bound
    this.count++;
    console.log(this.count);
  };
}

// FIX 3 is most common in React (event handlers)
```

### Mistake 3: Extending built-ins

```javascript
// Extending Array — usually works but has edge cases
class MyArray extends Array {
  sum() {
    return this.reduce((a, b) => a + b, 0);
  }
}

const arr = new MyArray(1, 2, 3);
arr.sum(); // 6 ✓

// But methods that return new arrays return MyArray, not Array:
const mapped = arr.map((x) => x * 2);
mapped instanceof MyArray; // true — this is often surprising

// Most experienced developers avoid extending built-ins
// Instead, use composition or standalone utility functions:
function sum(arr) {
  return arr.reduce((a, b) => a + b, 0);
}
```

---

## 11. Practical Implications

Understanding prototypes helps you:

**Debug `this` binding issues** — when a method loses `this`, it's because `this` isn't stored on the prototype chain — it's determined by how the function is called.

**Understand performance** — methods on the prototype are shared (one copy). Methods in the constructor body create a new function per instance (expensive for 1000s of objects).

```javascript
// Inefficient: new function per instance
class Bad {
  constructor() {
    this.greet = function () {
      return "hello";
    }; // new fn for each instance
  }
}

// Efficient: shared via prototype
class Good {
  greet() {
    return "hello";
  } // one function, on prototype
}

// 1000 instances of Bad = 1000 greet functions in memory
// 1000 instances of Good = 1 greet function shared
```

**Read library source code** — React, Lodash, Express all use prototypes under the hood. Once you can read prototype-based code, you can read anything.

**Understand `instanceof`** — it walks the prototype chain, not class names. This is why `[] instanceof Array` is true, and why it fails across iframes (different `Array.prototype`).

```javascript
// instanceof checks the prototype chain
function isArray(value) {
  // More reliable than instanceof across iframes/realms:
  return Object.prototype.toString.call(value) === "[object Array]";
  // Or simply:
  return Array.isArray(value); // ← use this in practice
}
```

---

## Summary

```
Prototypes: objects linked to objects
  Every object has [[Prototype]] pointing to another object (or null)
  Property lookup walks the chain until found or null reached

__proto__ vs prototype:
  __proto__ (or Object.getPrototypeOf) — the instance's link to its prototype
  .prototype — property of constructor functions/classes;
               becomes __proto__ of instances created with `new`

Object.create(proto):
  Creates new object with proto as its [[Prototype]]
  Most explicit way to set up prototype relationships

Constructor functions:
  function Foo() { this.x = 1; }  — adds per-instance properties
  Foo.prototype.method = fn        — shared method (one copy)
  new Foo() — creates object, sets __proto__, runs constructor, returns object

ES6 class:
  Syntactic sugar over constructor + prototype
  typeof MyClass === "function"  ← it's still a function
  Methods go on .prototype
  Static methods go on the constructor itself
  Private fields (#) are truly private

Inheritance:
  extends sets up prototype chain: Child.prototype → Parent.prototype
  super() must come before `this` in constructor
  super.method() calls the parent's version

Key methods:
  Object.hasOwn(obj, key)    — own property only (not chain)
  "key" in obj               — checks entire chain
  Object.keys/values/entries — own enumerable properties only
  for...in                   — all enumerable including inherited (filter with hasOwn)

Common mistakes:
  Shared mutable objects on prototype → initialise in constructor
  Losing `this` in callbacks → bind, arrow wrapper, or class field arrow fn
```

---

_Next: [10 — Error Handling](./10%20-%20Error%20Handling.md)_
