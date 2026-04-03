> Object-Oriented Programming is a way of organising code around objects that combine data (properties) and behaviour (methods). JavaScript supports OOP through its class syntax (introduced in ES6) which sits on top of the prototype system. This file teaches you to model real-world problems with classes — the full prototype story comes in File 09.

---

## Table of Contents

1. [Why OOP?](#1-why-oop)
2. [Classes and Constructors](#2-classes-and-constructors)
3. [Instance Properties and Methods](#3-instance-properties-and-methods)
4. [Private Fields and Methods](#4-private-fields-and-methods)
5. [Static Properties and Methods](#5-static-properties-and-methods)
6. [Getters and Setters](#6-getters-and-setters)
7. [Inheritance and extends](#7-inheritance-and-extends)
8. [Mixins — Multiple Behaviours](#8-mixins--multiple-behaviours)
9. [Design Principles](#9-design-principles)

---

## 1. Why OOP?

As programs grow, organising code into functions and loose variables becomes difficult. OOP groups related data and behaviour into objects, making code:

- **Easier to reason about** — a `Student` object knows everything about a student
- **Reusable** — create many `Student` objects from one class
- **Maintainable** — change how students work in one place
- **Encapsulated** — hide internal details, expose only what's needed

---

## 2. Classes and Constructors

A **class** is a blueprint. An **instance** is an object created from that blueprint.

```javascript
class Student {
  constructor(name, grade, scores) {
    // `this` refers to the new object being created
    this.name = name;
    this.grade = grade;
    this.scores = scores;
  }
}

// Create instances with `new`
const ashan  = new Student("Ashan",  11, [82, 91, 78]);
const dineth = new Student("Dineth", 12, [91, 88, 95]);

ashan.name;    // "Ashan"
dineth.grade;  // 12

// Without new: TypeError
const bad = Student("Ashan", 11, []);  // TypeError: Class constructor must be called with new
```

---

## 3. Instance Properties and Methods

**Instance properties** are unique to each object. **Instance methods** are shared by all instances (they live on the prototype).

```javascript
class Student {
  // Class fields — defined per-instance (ES2022)
  // (same as assigning in constructor, but more explicit)
  school = "CWWKCC";

  constructor(name, grade, scores = []) {
    this.name = name;
    this.grade = grade;
    this.scores = scores;
  }

  // Instance method — shared via prototype (one copy for all instances)
  average() {
    if (this.scores.length === 0) return 0;
    return this.scores.reduce((sum, s) => sum + s, 0) / this.scores.length;
  }

  letterGrade() {
    const avg = this.average();
    if (avg >= 75) return "A";
    if (avg >= 65) return "B";
    if (avg >= 50) return "C";
    return "F";
  }

  addScore(score) {
    this.scores.push(score);
    return this;  // return `this` to enable method chaining
  }

  toString() {
    return `${this.name} (Grade ${this.grade}): ${this.letterGrade()}`;
  }
}

const ashan = new Student("Ashan", 11, [82, 91, 78]);
ashan.average();     // 83.67
ashan.letterGrade(); // "A"
ashan.school;        // "CWWKCC"

// Method chaining
ashan.addScore(88).addScore(95);
ashan.average();  // now includes 88 and 95

`${ashan}`;  // "Ashan (Grade 11): A" — toString() is called automatically
```

---

## 4. Private Fields and Methods

Private fields (prefixed with `#`) are only accessible inside the class. They don't appear in `Object.keys()`, JSON, or any external access.

```javascript
class BankAccount {
  #balance;       // private field
  #transactions;  // private field

  constructor(owner, initialBalance = 0) {
    this.owner = owner;              // public
    this.#balance = initialBalance;  // private
    this.#transactions = [];         // private
  }

  // Private method
  #recordTransaction(type, amount) {
    this.#transactions.push({
      type,
      amount,
      balance: this.#balance,
      date: new Date(),
    });
  }

  deposit(amount) {
    if (amount <= 0) throw new Error("Deposit amount must be positive");
    this.#balance += amount;
    this.#recordTransaction("deposit", amount);
    return this;
  }

  withdraw(amount) {
    if (amount <= 0) throw new Error("Withdrawal amount must be positive");
    if (amount > this.#balance) throw new Error("Insufficient funds");
    this.#balance -= amount;
    this.#recordTransaction("withdrawal", amount);
    return this;
  }

  get balance() {
    return this.#balance;  // read-only access via getter
  }

  getStatement() {
    return this.#transactions.map(t =>
      `${t.type}: ${t.amount} LKR (Balance: ${t.balance} LKR)`
    ).join("\n");
  }
}

const account = new BankAccount("Ashan", 10000);
account.deposit(5000).withdraw(2000);
account.balance;           // 13000
account.getStatement();    // "deposit: 5000 LKR..."

account.#balance;          // SyntaxError — private, genuinely inaccessible
account.#recordTransaction; // SyntaxError
```

---

## 5. Static Properties and Methods

**Static** members belong to the class itself, not instances. They're accessed as `ClassName.method()`, not `instance.method()`.

```javascript
class Student {
  static #count = 0;       // private static — tracks how many students created
  static school = "CWWKCC"; // public static

  constructor(name, grade) {
    this.name = name;
    this.grade = grade;
    this.id = ++Student.#count;
  }

  // Static factory methods — alternative constructors
  static fromJSON(json) {
    const data = JSON.parse(json);
    return new Student(data.name, data.grade);
  }

  static fromObject({ name, grade }) {
    return new Student(name, grade);
  }

  // Static utility methods
  static compareByGrade(a, b) {
    return a.grade - b.grade;
  }

  static getCount() {
    return Student.#count;
  }
}

const s1 = new Student("Ashan", 11);
const s2 = new Student("Dineth", 12);

Student.school;       // "CWWKCC"
Student.getCount();   // 2
s1.getCount();        // TypeError — static methods aren't on instances

// Factory methods
const s3 = Student.fromObject({ name: "Kavya", grade: 11 });
const s4 = Student.fromJSON('{"name":"Saman","grade":12}');

// Using static comparator
const students = [s2, s1, s3, s4];
students.sort(Student.compareByGrade);
// sorted by grade: s1, s3, s2, s4
```

---

## 6. Getters and Setters

Getters and setters let you define computed properties that look like regular properties.

```javascript
class Temperature {
  #celsius;

  constructor(celsius) {
    this.#celsius = celsius;
  }

  // Getter — accessed like a property, no ()
  get fahrenheit() {
    return (this.#celsius * 9/5) + 32;
  }

  get kelvin() {
    return this.#celsius + 273.15;
  }

  // Setter — validate before setting
  set celsius(value) {
    if (value < -273.15) throw new RangeError("Below absolute zero!");
    this.#celsius = value;
  }

  get celsius() {
    return this.#celsius;
  }
}

const temp = new Temperature(100);
temp.fahrenheit;    // 212 — computed on the fly
temp.kelvin;        // 373.15
temp.celsius = 0;   // triggers setter
temp.celsius;       // 0
temp.celsius = -300;  // RangeError!
```

```javascript
// Practical: derived/computed properties
class Cart {
  #items = [];

  addItem(item) {
    this.#items.push(item);
    return this;
  }

  get total() {
    return this.#items.reduce((sum, item) => sum + item.price * item.quantity, 0);
  }

  get isEmpty() {
    return this.#items.length === 0;
  }

  get count() {
    return this.#items.reduce((sum, item) => sum + item.quantity, 0);
  }
}

const cart = new Cart();
cart.addItem({ name: "Book", price: 500, quantity: 2 });
cart.total;    // 1000
cart.count;    // 2
cart.isEmpty;  // false
```

---

## 7. Inheritance and `extends`

Inheritance lets one class build on another — inheriting all its properties and methods, then extending or overriding them.

```javascript
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
  #breed;

  constructor(name, breed) {
    super(name, "woof");  // call parent constructor — REQUIRED before `this`
    this.#breed = breed;
  }

  // Override parent method
  speak() {
    return `${this.name} barks: ${this.sound}!`;
  }

  // Extend parent method
  describe() {
    return `${super.speak()} — a ${this.#breed}`;
    //       ↑ calls Animal.speak() explicitly
  }

  fetch(item) {
    return `${this.name} fetches the ${item}`;
  }
}

class GuideDog extends Dog {
  #owner;

  constructor(name, breed, owner) {
    super(name, breed);  // calls Dog constructor
    this.#owner = owner;
  }

  guide() {
    return `${this.name} guides ${this.#owner}`;
  }
}

const rex   = new Dog("Rex", "Labrador");
const buddy = new GuideDog("Buddy", "Golden Retriever", "Ashan");

rex.speak();          // "Rex barks: woof!"
rex.describe();       // "Rex says woof — a Labrador"
buddy.speak();        // "Buddy barks: woof!" (inherited from Dog)
buddy.guide();        // "Buddy guides Ashan"

// instanceof checks the prototype chain
buddy instanceof GuideDog;  // true
buddy instanceof Dog;       // true
buddy instanceof Animal;    // true
buddy instanceof Object;    // always true
```

### When `super` is required

```javascript
class Base {
  constructor(value) {
    this.value = value;
  }
}

class Child extends Base {
  constructor(value, extra) {
    // Must call super() BEFORE using `this`
    super(value);         // sets this.value = value via Base constructor
    this.extra = extra;   // then you can use `this`
  }
}

// If a subclass has no constructor, the parent constructor is used automatically:
class SimpleChild extends Base {
  // No constructor needed — Base's constructor is called with the same args
  double() {
    return this.value * 2;
  }
}
```

---

## 8. Mixins — Multiple Behaviours

JavaScript classes can only extend one parent. Mixins let you compose behaviour from multiple sources.

```javascript
// Mixins are functions that take a class and return an enhanced class

const Serializable = (Base) => class extends Base {
  toJSON() {
    return JSON.stringify(this);
  }

  static fromJSON(json) {
    return Object.assign(new this(), JSON.parse(json));
  }
};

const Timestamped = (Base) => class extends Base {
  constructor(...args) {
    super(...args);
    this.createdAt = new Date();
    this.updatedAt = new Date();
  }

  touch() {
    this.updatedAt = new Date();
    return this;
  }
};

const Validatable = (Base) => class extends Base {
  validate() {
    const errors = [];
    if (!this.name?.trim()) errors.push("Name is required");
    if (!this.email?.includes("@")) errors.push("Email is invalid");
    return { valid: errors.length === 0, errors };
  }
};

// Compose multiple mixins
class User extends Serializable(Timestamped(Validatable(class {}))) {
  constructor(name, email) {
    super();
    this.name = name;
    this.email = email;
  }
}

const user = new User("Ashan", "ashan@cwwkcc.lk");
user.validate();   // { valid: true, errors: [] }
user.toJSON();     // JSON string with all properties
user.createdAt;    // Date when created
user.touch();      // updates updatedAt
```

---

## 9. Design Principles

### Single Responsibility

Each class should have one reason to change.

```javascript
// BAD — Student does too many things
class Student {
  constructor(name) { this.name = name; }
  save() { db.save(this); }           // database concern
  sendEmail() { emailer.send(this); } // email concern
  formatReport() { /* ... */ }        // reporting concern
}

// GOOD — separate concerns
class Student {
  constructor(name) { this.name = name; }
  getDisplayName() { return this.name; }
}

class StudentRepository {
  async save(student) { return db.save(student); }
  async findById(id) { return db.find(id); }
}

class StudentEmailer {
  async sendWelcome(student) { /* ... */ }
}
```

### Encapsulation

Hide internal details. Expose only what consumers need.

```javascript
class Timer {
  #startTime = null;
  #elapsed = 0;
  #running = false;

  start() {
    if (this.#running) return;
    this.#startTime = Date.now();
    this.#running = true;
  }

  stop() {
    if (!this.#running) return;
    this.#elapsed += Date.now() - this.#startTime;
    this.#running = false;
  }

  reset() {
    this.#startTime = null;
    this.#elapsed = 0;
    this.#running = false;
  }

  // Read-only computed property — consumers get what they need
  get elapsed() {
    if (this.#running) {
      return this.#elapsed + (Date.now() - this.#startTime);
    }
    return this.#elapsed;
  }
}
```

### Favour Composition Over Inheritance

Deep inheritance hierarchies are fragile. Composing behaviour is more flexible.

```javascript
// Inheritance hierarchy (fragile)
Animal → Mammal → Dog → TrainedDog → GuideDog

// Composition — attach behaviours as needed
function createDog({ name, breed }) {
  const base = { name, breed };
  const speak = { speak: () => `${name} barks!` };
  const fetch = { fetch: (item) => `${name} fetches ${item}` };

  return { ...base, ...speak, ...fetch };
}

function withGuiding(dog, owner) {
  return { ...dog, guide: () => `${dog.name} guides ${owner}` };
}

const rex   = createDog({ name: "Rex",   breed: "Labrador" });
const buddy = withGuiding(createDog({ name: "Buddy", breed: "Golden" }), "Ashan");
```

---

## Summary

```
Class basics:
  class Name { constructor(...) { this.prop = value; } }
  new Name(...args) — creates an instance

Instance members:
  Properties: set in constructor (this.x = ...) or class fields
  Methods: defined in class body, shared via prototype

Private (#):
  #field, #method() — only accessible inside the class
  Not in Object.keys(), JSON, or external access
  Enforced by the JS engine (not just convention like _private)

Static:
  static method() — belongs to class, not instances
  Accessed as ClassName.method(), not instance.method()
  Use for: factory methods, utilities, counters

Getters/Setters:
  get prop() { ... } — called like a property (no ())
  set prop(value) { ... } — intercept assignment, validate

Inheritance:
  class Child extends Parent { ... }
  Must call super() before using `this` in a constructor
  super.method() — call parent's version of overridden method

Mixins:
  const Mixed = (Base) => class extends Base { ... }
  Compose multiple behaviours without deep hierarchies

Principles:
  Single responsibility — one class, one purpose
  Encapsulation — hide internals with private fields
  Prefer composition — attach behaviour rather than deep inheritance
```

---

_Next: [06 — Asynchronous JavaScript & APIs](./06%20-%20Asynchronous%20JavaScript%20%26%20APIs.md)_