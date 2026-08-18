> Design patterns are reusable solutions to common problems. JavaScript's patterns differ from classical OOP patterns because of closures, prototypes, and the module system. These are the ones you'll actually encounter — in React source code, Express middleware, Node.js libraries, and your own projects.

---

## Table of Contents

1. [Why Patterns Matter](#1-why-patterns-matter)
2. [Module Pattern](#2-module-pattern)
3. [Singleton Pattern](#3-singleton-pattern)
4. [Observer / Event Emitter Pattern](#4-observer--event-emitter-pattern)
5. [Factory Pattern](#5-factory-pattern)
6. [Strategy Pattern](#6-strategy-pattern)
7. [Middleware Pattern](#7-middleware-pattern)
8. [Proxy Pattern](#8-proxy-pattern)
9. [Command Pattern](#9-command-pattern)
10. [Patterns in React](#10-patterns-in-react)

---

## 1. Why Patterns Matter

Patterns aren't things you impose on code. They're names for structures that appear naturally when solving recurring problems well. Knowing their names means:

- You can communicate intent clearly ("this is a strategy pattern") without explaining the whole structure
- You recognise them when reading others' code (Express middleware _is_ the middleware pattern)
- You reach for the right tool instead of reinventing a poor version of it

---

## 2. Module Pattern

**Problem:** How do you create private state in JavaScript? How do you expose a public API while hiding implementation details?

**Solution:** Use a closure to create private scope, return an object with the public API.

```javascript
// The module pattern (pre-ESM, still used today)
const CartService = (() => {
  // PRIVATE
  let items = [];
  let discountCode = null;

  function calculateSubtotal() {
    return items.reduce((sum, item) => sum + item.price * item.quantity, 0);
  }

  function applyDiscount(subtotal) {
    if (discountCode === "KITS10") return subtotal * 0.9;
    return subtotal;
  }

  // PUBLIC API
  return {
    addItem(product, quantity = 1) {
      const existing = items.find((i) => i.id === product.id);
      if (existing) {
        existing.quantity += quantity;
      } else {
        items.push({ ...product, quantity });
      }
    },

    removeItem(productId) {
      items = items.filter((i) => i.id !== productId);
    },

    applyDiscountCode(code) {
      discountCode = code;
    },

    getTotal() {
      const subtotal = calculateSubtotal();
      return applyDiscount(subtotal);
    },

    getItems() {
      return [...items]; // return a copy — can't mutate internal state directly
    },

    clear() {
      items = [];
      discountCode = null;
    },
  };
})();

// Usage
CartService.addItem({ id: "p1", name: "Book", price: 1500 }, 2);
CartService.applyDiscountCode("KITS10");
CartService.getTotal(); // 2700 (3000 * 0.9)

CartService.items; // undefined — private
CartService.calculateSubtotal; // undefined — private
```

With ES modules (the modern approach), files are modules by default:

```javascript
// cart.js — everything not exported is private
let items = [];

function calculateSubtotal() {
  return items.reduce((sum, item) => sum + item.price * item.quantity, 0);
}

export function addItem(product, quantity = 1) { ... }
export function getTotal() { ... }

// calculateSubtotal is private — not exported, not accessible from outside
```

---

## 3. Singleton Pattern

**Problem:** How do you ensure only one instance of something exists — a database connection, a logger, a configuration object?

**Solution:** Return the same instance on every call.

```javascript
// Database connection singleton
class Database {
  static #instance = null;
  #connection = null;

  constructor() {
    if (Database.#instance) {
      return Database.#instance; // return existing instance
    }
    Database.#instance = this;
  }

  async connect(url) {
    if (this.#connection) return; // already connected
    this.#connection = await createConnection(url);
  }

  async query(sql, params) {
    if (!this.#connection) throw new Error("Not connected");
    return this.#connection.query(sql, params);
  }

  static getInstance() {
    if (!Database.#instance) {
      Database.#instance = new Database();
    }
    return Database.#instance;
  }
}

// Usage — always the same instance
const db1 = Database.getInstance();
const db2 = Database.getInstance();
db1 === db2; // true
```

```javascript
// Simpler singleton using module scope
// In Node.js, modules are cached after the first require/import
// So a module-level variable IS a singleton

// logger.js
class Logger {
  constructor(service) {
    this.service = service;
    this.entries = [];
  }

  log(level, message, meta = {}) {
    const entry = {
      timestamp: new Date().toISOString(),
      level,
      service: this.service,
      message,
      ...meta,
    };
    this.entries.push(entry);
    console.log(JSON.stringify(entry));
  }

  info(message, meta) {
    this.log("INFO", message, meta);
  }
  error(message, meta) {
    this.log("ERROR", message, meta);
  }
  warn(message, meta) {
    this.log("WARN", message, meta);
  }
}

// This is created once when the module is first imported
// Every subsequent import gets the same instance
export const logger = new Logger("app");
```

---

## 4. Observer / Event Emitter Pattern

**Problem:** How do you let different parts of your system react to events without tightly coupling them to each other?

**Solution:** Objects publish events; other objects subscribe to them. Publisher doesn't know who's listening.

```javascript
class EventEmitter {
  #listeners = new Map();

  on(event, listener) {
    if (!this.#listeners.has(event)) {
      this.#listeners.set(event, new Set());
    }
    this.#listeners.get(event).add(listener);
    return () => this.off(event, listener); // return unsubscribe function
  }

  once(event, listener) {
    const wrapper = (...args) => {
      listener(...args);
      this.off(event, wrapper);
    };
    return this.on(event, wrapper);
  }

  off(event, listener) {
    this.#listeners.get(event)?.delete(listener);
  }

  emit(event, ...args) {
    this.#listeners.get(event)?.forEach((listener) => {
      try {
        listener(...args);
      } catch (err) {
        console.error(`Error in listener for ${event}:`, err);
        // Don't let one bad listener stop others from running
      }
    });
  }

  removeAllListeners(event) {
    if (event) {
      this.#listeners.delete(event);
    } else {
      this.#listeners.clear();
    }
  }
}

// Real-world usage
class OrderService extends EventEmitter {
  async placeOrder(orderData) {
    const order = await db.order.create(orderData);

    // Emit events — OrderService doesn't know who cares
    this.emit("order:placed", order);

    return order;
  }

  async cancelOrder(orderId) {
    const order = await db.order.update(orderId, { status: "cancelled" });
    this.emit("order:cancelled", order);
    return order;
  }
}

const orderService = new OrderService();

// Different parts of the system react independently
orderService.on("order:placed", async (order) => {
  await emailService.sendConfirmation(order.email, order);
});

orderService.on("order:placed", async (order) => {
  await inventoryService.decrementStock(order.items);
});

orderService.on("order:placed", (order) => {
  analytics.track("order_placed", { orderId: order.id, total: order.total });
});

// Clean up when done
const unsubscribe = orderService.on("order:cancelled", (order) => {
  refundService.initiateRefund(order);
});

// Later:
unsubscribe(); // remove the listener
```

The Observer pattern is the foundation of the DOM event system (`addEventListener`), Node.js's `EventEmitter`, and React's synthetic event system.

---

## 5. Factory Pattern

**Problem:** How do you create objects without knowing their exact class? How do you centralise complex object construction?

**Solution:** A function (factory) handles the decision of which object to create and how.

```javascript
// Notification factory — creates the right notification type
function createNotification(type, data) {
  const base = {
    id: generateId(),
    createdAt: new Date(),
    read: false,
  };

  switch (type) {
    case "email":
      return {
        ...base,
        type: "email",
        to: data.to,
        subject: data.subject,
        body: data.body,
        send() {
          return emailProvider.send(this.to, this.subject, this.body);
        },
      };

    case "sms":
      return {
        ...base,
        type: "sms",
        phone: data.phone,
        message: data.message,
        send() {
          return smsProvider.send(this.phone, this.message);
        },
      };

    case "push":
      return {
        ...base,
        type: "push",
        userId: data.userId,
        title: data.title,
        body: data.body,
        send() {
          return pushProvider.notify(this.userId, this.title, this.body);
        },
      };

    default:
      throw new Error(`Unknown notification type: ${type}`);
  }
}

// Caller doesn't need to know about EmailNotification, SMSNotification, etc.
const notification = createNotification("email", {
  to: "dineth@cwwkcc.lk",
  subject: "Your results",
  body: "...",
});

await notification.send();
```

```javascript
// Abstract factory — a factory of factories
// Creates families of related objects

function createStorageProvider(type) {
  if (type === "s3") {
    return {
      upload: (key, data) =>
        s3.putObject({ Bucket: "my-bucket", Key: key, Body: data }),
      download: (key) => s3.getObject({ Bucket: "my-bucket", Key: key }),
      delete: (key) => s3.deleteObject({ Bucket: "my-bucket", Key: key }),
      url: (key) => `https://my-bucket.s3.amazonaws.com/${key}`,
    };
  }

  if (type === "minio") {
    return {
      upload: (key, data) => minioClient.putObject("my-bucket", key, data),
      download: (key) => minioClient.getObject("my-bucket", key),
      delete: (key) => minioClient.removeObject("my-bucket", key),
      url: (key) => `https://minio.cwwkcc.lk/my-bucket/${key}`,
    };
  }

  if (type === "local") {
    return {
      upload: (key, data) => fs.writeFile(path.join("uploads", key), data),
      download: (key) => fs.readFile(path.join("uploads", key)),
      delete: (key) => fs.unlink(path.join("uploads", key)),
      url: (key) => `/uploads/${key}`,
    };
  }

  throw new Error(`Unknown storage type: ${type}`);
}

// Swap implementations without changing calling code
const storage = createStorageProvider(process.env.STORAGE_TYPE || "local");
await storage.upload("profile/u123.jpg", imageBuffer);
```

---

## 6. Strategy Pattern

**Problem:** How do you choose between different algorithms or behaviours at runtime without a mess of if/else chains?

**Solution:** Define a family of algorithms, encapsulate each one, and make them interchangeable.

```javascript
// Payment strategies
const paymentStrategies = {
  creditCard: {
    name: "Credit Card",
    validate(data) {
      if (!data.cardNumber || data.cardNumber.length !== 16) {
        throw new ValidationError("Invalid card number");
      }
      if (!data.cvv || data.cvv.length < 3) {
        throw new ValidationError("Invalid CVV");
      }
    },
    async process(amount, data) {
      return stripeClient.charge({ amount, source: data.token });
    },
  },

  paypal: {
    name: "PayPal",
    validate(data) {
      if (!data.email || !data.email.includes("@")) {
        throw new ValidationError("Invalid PayPal email");
      }
    },
    async process(amount, data) {
      return paypalClient.createPayment({ amount, payerEmail: data.email });
    },
  },

  bankTransfer: {
    name: "Bank Transfer",
    validate(data) {
      if (!data.accountNumber || !data.routingNumber) {
        throw new ValidationError("Bank account details required");
      }
    },
    async process(amount, data) {
      return bankClient.initiateTransfer({ amount, ...data });
    },
  },
};

class PaymentService {
  async processPayment(method, amount, data) {
    const strategy = paymentStrategies[method];
    if (!strategy) throw new Error(`Unsupported payment method: ${method}`);

    strategy.validate(data);
    const result = await strategy.process(amount, data);

    await db.payment.create({
      method,
      amount,
      status: "completed",
      transactionId: result.id,
    });

    return result;
  }
}

// Adding a new payment method: just add a new strategy object
// No changes to PaymentService
paymentStrategies.crypto = {
  name: "Cryptocurrency",
  validate(data) {
    /* ... */
  },
  async process(amount, data) {
    /* ... */
  },
};
```

---

## 7. Middleware Pattern

**Problem:** How do you compose a chain of processing steps, where each step can inspect, modify, or short-circuit the flow?

**Solution:** Each middleware is a function that receives the data, does its work, then calls `next()` to pass control to the next middleware.

You already know this pattern — it's Express.

```javascript
// Implementing middleware from scratch
function createMiddlewareChain() {
  const middlewares = [];

  return {
    use(fn) {
      middlewares.push(fn);
      return this; // chainable
    },

    async run(context) {
      let index = 0;

      const next = async () => {
        if (index >= middlewares.length) return;
        const middleware = middlewares[index++];
        await middleware(context, next);
      };

      await next();
      return context;
    },
  };
}

// Middleware functions
async function authenticate(ctx, next) {
  const token = ctx.headers.authorization?.replace("Bearer ", "");
  if (!token) {
    ctx.status = 401;
    ctx.body = { error: "Unauthorised" };
    return; // don't call next() — short-circuit the chain
  }

  try {
    ctx.user = verifyToken(token);
    await next(); // continue chain
  } catch {
    ctx.status = 401;
    ctx.body = { error: "Invalid token" };
  }
}

async function rateLimit(ctx, next) {
  const key = `rate:${ctx.user?.id ?? ctx.ip}`;
  const requests = await redis.incr(key);
  await redis.expire(key, 60); // 1-minute window

  if (requests > 100) {
    ctx.status = 429;
    ctx.body = { error: "Too many requests" };
    return; // short-circuit
  }

  await next();
}

async function logger(ctx, next) {
  const start = Date.now();
  await next();
  const duration = Date.now() - start;
  console.log(`${ctx.method} ${ctx.path} ${ctx.status} ${duration}ms`);
}

// Composing the chain
const pipeline = createMiddlewareChain()
  .use(logger)
  .use(authenticate)
  .use(rateLimit);

// Running it
const context = { method: "GET", path: "/api/users", headers: req.headers };
await pipeline.run(context);
```

---

## 8. Proxy Pattern

**Problem:** How do you intercept and control access to an object — for validation, logging, caching, or lazy loading?

**Solution:** JavaScript's built-in `Proxy` object wraps another object and intercepts operations on it.

```javascript
// Validation proxy — enforce types and constraints
function createValidatedObject(target, validators) {
  return new Proxy(target, {
    set(obj, prop, value) {
      const validate = validators[prop];
      if (validate) {
        const error = validate(value);
        if (error) throw new TypeError(`${prop}: ${error}`);
      }
      obj[prop] = value;
      return true; // must return true to indicate success
    },
  });
}

const user = createValidatedObject(
  { name: "", age: 0, email: "" },
  {
    name: (v) =>
      typeof v !== "string"
        ? "must be a string"
        : v.length < 2
          ? "too short"
          : null,
    age: (v) =>
      typeof v !== "number"
        ? "must be a number"
        : v < 0
          ? "must be positive"
          : null,
    email: (v) => (!v.includes("@") ? "must be a valid email" : null),
  },
);

user.name = "Alice"; // ✓
user.age = 20; // ✓
user.email = "not-an-email"; // TypeError: email: must be a valid email
user.name = "A"; // TypeError: name: too short
```

```javascript
// Caching proxy — cache expensive method calls
function createCachingProxy(target) {
  const cache = new Map();

  return new Proxy(target, {
    get(obj, prop) {
      const value = obj[prop];

      if (typeof value !== "function") return value;

      return function (...args) {
        const key = `${prop}:${JSON.stringify(args)}`;

        if (cache.has(key)) {
          console.log(`Cache hit: ${prop}`);
          return cache.get(key);
        }

        const result = value.apply(obj, args);

        if (result instanceof Promise) {
          return result.then((resolved) => {
            cache.set(key, Promise.resolve(resolved));
            return resolved;
          });
        }

        cache.set(key, result);
        return result;
      };
    },
  });
}

const cachedDB = createCachingProxy(db);
await cachedDB.query("SELECT * FROM students WHERE grade = 12"); // hits DB
await cachedDB.query("SELECT * FROM students WHERE grade = 12"); // Cache hit
```

---

## 9. Command Pattern

**Problem:** How do you encapsulate an action as an object — so you can queue it, log it, undo it, or retry it?

**Solution:** Each action is an object with an `execute()` method (and optionally `undo()`).

```javascript
// Command interface
class Command {
  async execute() {
    throw new Error("execute() not implemented");
  }
  async undo() {
    throw new Error("undo() not implemented");
  }
}

// Concrete commands
class CreatePostCommand extends Command {
  constructor(data, postService) {
    super();
    this.data = data;
    this.postService = postService;
    this.createdId = null;
  }

  async execute() {
    const post = await this.postService.create(this.data);
    this.createdId = post.id; // remember for undo
    return post;
  }

  async undo() {
    if (this.createdId) {
      await this.postService.delete(this.createdId);
      this.createdId = null;
    }
  }
}

class UpdatePostCommand extends Command {
  constructor(id, updates, postService) {
    super();
    this.id = id;
    this.updates = updates;
    this.postService = postService;
    this.previousState = null;
  }

  async execute() {
    this.previousState = await this.postService.findById(this.id); // save for undo
    return this.postService.update(this.id, this.updates);
  }

  async undo() {
    if (this.previousState) {
      await this.postService.update(this.id, this.previousState);
    }
  }
}

// Command executor with undo history
class CommandExecutor {
  #history = [];
  #redoStack = [];

  async execute(command) {
    const result = await command.execute();
    this.#history.push(command);
    this.#redoStack = []; // executing a new command clears redo stack
    return result;
  }

  async undo() {
    const command = this.#history.pop();
    if (!command) return;
    await command.undo();
    this.#redoStack.push(command);
  }

  async redo() {
    const command = this.#redoStack.pop();
    if (!command) return;
    const result = await command.execute();
    this.#history.push(command);
    return result;
  }
}

// Usage — every action is undoable
const executor = new CommandExecutor();

const post = await executor.execute(
  new CreatePostCommand({ title: "Hello" }, postService),
);
await executor.execute(
  new UpdatePostCommand(post.id, { title: "Hello World" }, postService),
);

await executor.undo(); // reverts title back to "Hello"
await executor.undo(); // deletes the post
await executor.redo(); // re-creates the post
```

---

## 10. Patterns in React

React codebases use patterns constantly — often without naming them. Recognising them makes reading and writing React code cleaner.

### Higher-Order Component (HOC) — Factory Pattern applied to components

```javascript
// A HOC is a function that takes a component and returns an enhanced component
function withAuth(WrappedComponent) {
  return function AuthenticatedComponent(props) {
    const { user, isLoading } = useAuth();

    if (isLoading) return <Spinner />;
    if (!user) return <Navigate to="/login" />;

    return <WrappedComponent {...props} user={user} />;
  };
}

const ProtectedDashboard = withAuth(Dashboard);
// ProtectedDashboard is a new component that includes auth checking
```

### Render Props — Strategy Pattern applied to React

```javascript
// The "strategy" for rendering is passed as a prop
function DataFetcher({ url, renderLoading, renderError, renderData }) {
  const { data, error, isLoading } = useFetch(url);

  if (isLoading) return renderLoading();
  if (error) return renderError(error);
  return renderData(data);
}

// Caller chooses how to render each state
<DataFetcher
  url="/api/students"
  renderLoading={() => <Spinner />}
  renderError={(err) => <ErrorBanner message={err.message} />}
  renderData={(students) => <StudentList students={students} />}
/>;
```

### Custom Hooks — Module Pattern for logic reuse

```javascript
// Encapsulates logic with private state, exposes only what's needed
function useCart() {
  const [items, setItems] = useState([]);
  const [discountCode, setDiscountCode] = useState(null);

  // Private helper — not returned
  const calculateSubtotal = () =>
    items.reduce((sum, item) => sum + item.price * item.quantity, 0);

  // Public API — what the component sees
  const addItem = (product, quantity = 1) => {
    setItems((prev) => {
      const existing = prev.find((i) => i.id === product.id);
      if (existing) {
        return prev.map((i) =>
          i.id === product.id ? { ...i, quantity: i.quantity + quantity } : i,
        );
      }
      return [...prev, { ...product, quantity }];
    });
  };

  const total =
    discountCode === "KITS10" ? calculateSubtotal() * 0.9 : calculateSubtotal();

  return { items, total, addItem, setDiscountCode };
}
```

---

## Summary

```
Module:       Closure-based private state with a public API
              → Used everywhere: ES modules, hooks, services

Singleton:    One instance per process
              → Database connections, loggers, config

Observer:     Publish events; subscribers react without coupling
              → DOM events, Node EventEmitter, React state updates

Factory:      Centralise object creation, abstract over implementation
              → createNotification, createStorageProvider

Strategy:     Swap algorithms/behaviours at runtime
              → Payment methods, sorting strategies, storage backends

Middleware:   Chain of processing steps, each can short-circuit
              → Express, Koa, Redux middleware, HTTP pipelines

Proxy:        Intercept property access/mutation on an object
              → Validation, caching, logging, reactive systems (Vue 3)

Command:      Encapsulate actions as objects with execute/undo
              → Undo/redo, queuing, transaction logs

In React:
  HOC         → Factory pattern for components
  Render props → Strategy pattern for rendering
  Custom hooks → Module pattern for logic
```

---

_Next: [12 — The Web APIs: Storage, Workers, and More](./12%20-%20Web%20APIs.md)_
