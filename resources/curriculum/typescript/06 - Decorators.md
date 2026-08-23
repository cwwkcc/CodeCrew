> Decorators are the syntax that makes NestJS possible. They're the `@Controller()`, `@Injectable()`, `@Get()`, `@Body()`annotations you see in every NestJS file. Understanding what decorators actually do turns NestJS from magic into understandable code.

---

## Table of Contents

1. [What Decorators Are](#1-what-decorators-are)
2. [Enabling Decorators](#2-enabling-decorators)
3. [Class Decorators](#3-class-decorators)
4. [Method Decorators](#4-method-decorators)
5. [Property Decorators](#5-property-decorators)
6. [Parameter Decorators](#6-parameter-decorators)
7. [Decorator Factories](#7-decorator-factories)
8. [Decorator Composition](#8-decorator-composition)
9. [Reflect Metadata](#9-reflect-metadata)
10. [NestJS Decorators — What They Actually Do](#10-nestjs-decorators--what-they-actually-do)
11. [Writing Useful Decorators](#11-writing-useful-decorators)

---

## 1. What Decorators Are

A decorator is a function that wraps a class, method, property, or parameter to modify or annotate it. The `@` syntax is just sugar for calling that function.

```typescript
// This:
@Controller("students")
class StudentController {}

// Is exactly this:
class StudentController {}
StudentController = Controller("students")(StudentController);
// The decorator function receives the class and returns a (possibly modified) class
```

Decorators are a **meta-programming** feature — they let you write code that operates on other code at class definition time (not runtime).

```typescript
// The simplest possible decorator:
function log(target: Function) {
  console.log(`Class defined: ${target.name}`);
}

@log
class MyService {}
// Console: "Class defined: MyService"
// Runs when the file is loaded — not when MyService is instantiated
```

---

## 2. Enabling Decorators

Decorators are a Stage 3 TC39 proposal. Two versions exist in TypeScript:

```json
// tsconfig.json

// Legacy decorators (TypeScript 4.x style, what NestJS uses):
{
  "compilerOptions": {
    "experimentalDecorators": true,
    "emitDecoratorMetadata": true // needed for NestJS DI
  }
}

// Modern decorators (TypeScript 5.0+, new standard):
// No flag needed — just use them
// Syntax is slightly different and not yet used by NestJS
```

For NestJS projects, always use `experimentalDecorators: true` and `emitDecoratorMetadata: true`. This is already in every NestJS project's `tsconfig.json`.

---

## 3. Class Decorators

A class decorator receives the constructor function and can return a new constructor (replacing the class) or undefined (modifying in place).

```typescript
// Signature: (constructor: Function) => Function | void

// Example: add metadata to a class
function Entity(tableName: string) {
  return function (constructor: Function) {
    constructor.prototype.__tableName = tableName;
    // Adds __tableName to every instance
  };
}

@Entity("students")
class Student {
  id: string;
  name: string;
}

const s = new Student();
(s as any).__tableName; // "students"
```

```typescript
// Example: seal the class (prevent extending and property addition)
function sealed(constructor: Function) {
  Object.seal(constructor);
  Object.seal(constructor.prototype);
}

@sealed
class BankAccount {
  balance: number = 0;
}

// BankAccount cannot be extended, prototype cannot be modified
```

```typescript
// Example: replace the class entirely
function singleton<T extends { new (...args: any[]): {} }>(constructor: T) {
  let instance: InstanceType<T>;

  return class extends constructor {
    constructor(...args: any[]) {
      if (instance) return instance;
      super(...args);
      instance = this as InstanceType<T>;
    }
  };
}

@singleton
class Database {
  connection: string;
  constructor(url: string) {
    this.connection = url;
  }
}

const db1 = new Database("postgres://...");
const db2 = new Database("postgres://...");
db1 === db2; // true — same instance
```

---

## 4. Method Decorators

A method decorator receives the class prototype, the method name, and the property descriptor.

```typescript
// Signature:
// (
//   target: Object,           — class prototype (or constructor for static methods)
//   propertyKey: string,      — method name
//   descriptor: PropertyDescriptor  — method descriptor
// ) => PropertyDescriptor | void

function log(target: Object, key: string, descriptor: PropertyDescriptor) {
  const originalMethod = descriptor.value;

  descriptor.value = function (...args: any[]) {
    console.log(`→ Calling ${key} with`, args);
    const result = originalMethod.apply(this, args);
    console.log(`← ${key} returned`, result);
    return result;
  };

  return descriptor;
}

class MathService {
  @log
  add(a: number, b: number): number {
    return a + b;
  }
}

const math = new MathService();
math.add(2, 3);
// Console:
// → Calling add with [2, 3]
// ← add returned 5
```

```typescript
// Useful: measure execution time
function measureTime(
  target: Object,
  key: string,
  descriptor: PropertyDescriptor,
): PropertyDescriptor {
  const original = descriptor.value;

  descriptor.value = async function (...args: any[]) {
    const start = performance.now();
    const result = await original.apply(this, args);
    const duration = performance.now() - start;
    console.log(`${key} took ${duration.toFixed(2)}ms`);
    return result;
  };

  return descriptor;
}

class StudentService {
  @measureTime
  async findAll(): Promise<Student[]> {
    return this.db.query("SELECT * FROM students");
  }
}
```

```typescript
// Useful: retry on failure
function retry(attempts: number, delayMs = 1000) {
  return function (
    target: Object,
    key: string,
    descriptor: PropertyDescriptor,
  ) {
    const original = descriptor.value;

    descriptor.value = async function (...args: any[]) {
      for (let i = 0; i < attempts; i++) {
        try {
          return await original.apply(this, args);
        } catch (err) {
          if (i === attempts - 1) throw err;
          console.warn(`${key} attempt ${i + 1} failed, retrying...`);
          await new Promise((r) => setTimeout(r, delayMs * (i + 1)));
        }
      }
    };

    return descriptor;
  };
}

class PaymentService {
  @retry(3, 500)
  async chargeCard(amount: number): Promise<string> {
    return paymentGateway.charge(amount);
  }
}
```

---

## 5. Property Decorators

Property decorators receive the class prototype and the property name. They cannot access the property value directly (the property doesn't exist yet at decoration time) — they work with metadata.

```typescript
// Signature: (target: Object, propertyKey: string) => void

function validate(min: number, max: number) {
  return function (target: Object, propertyKey: string) {
    let value: number;

    Object.defineProperty(target, propertyKey, {
      get() {
        return value;
      },
      set(newValue: number) {
        if (newValue < min || newValue > max) {
          throw new RangeError(
            `${propertyKey} must be between ${min} and ${max}, got ${newValue}`,
          );
        }
        value = newValue;
      },
      enumerable: true,
      configurable: true,
    });
  };
}

class StudentScore {
  @validate(0, 100)
  score: number = 0;
}

const s = new StudentScore();
s.score = 85; // ✓
s.score = 105; // RangeError: score must be between 0 and 100, got 105
```

---

## 6. Parameter Decorators

Parameter decorators mark which parameters should receive what. They're primarily used for dependency injection (NestJS's `@Body()`, `@Param()`, `@Query()`).

```typescript
// Signature:
// (
//   target: Object,
//   propertyKey: string,
//   parameterIndex: number
// ) => void

// The decorator itself can't inject anything — it marks the parameter
// A framework (NestJS) then reads the metadata and does the injection

function Body(target: Object, key: string, index: number) {
  // Store metadata: "parameter at index `index` of method `key` wants the request body"
  const existing = Reflect.getMetadata("body-params", target, key) || [];
  existing.push(index);
  Reflect.defineMetadata("body-params", existing, target, key);
}

class UserController {
  async create(@Body() createUserDto: CreateUserDTO) {
    // NestJS reads the metadata, extracts req.body, passes it as the first argument
  }
}
```

---

## 7. Decorator Factories

A decorator factory is a function that returns a decorator. This allows passing configuration to the decorator.

```typescript
// Without factory — no configuration possible
@log
class Foo {}

// With factory — accepts configuration
function log(level: "info" | "debug" | "warn") {
  // Returns the actual decorator
  return function (target: Function) {
    console.log(`[${level.toUpperCase()}] Class ${target.name} loaded`);
  };
}

@log("info")
class UserController {}

@log("debug")
class InternalService {}
```

```typescript
// Factory with method decorator — common pattern
function cache(ttlSeconds: number) {
  return function (
    target: Object,
    key: string,
    descriptor: PropertyDescriptor,
  ) {
    const original = descriptor.value;
    const cacheMap = new Map<string, { value: any; expiresAt: number }>();

    descriptor.value = async function (...args: any[]) {
      const cacheKey = JSON.stringify(args);
      const cached = cacheMap.get(cacheKey);

      if (cached && Date.now() < cached.expiresAt) {
        return cached.value;
      }

      const result = await original.apply(this, args);
      cacheMap.set(cacheKey, {
        value: result,
        expiresAt: Date.now() + ttlSeconds * 1000,
      });
      return result;
    };

    return descriptor;
  };
}

class StudentService {
  @cache(300) // cache for 5 minutes
  async findAll(): Promise<Student[]> {
    return this.db.query("SELECT * FROM students");
  }

  @cache(60) // cache for 1 minute
  async findById(id: string): Promise<Student | null> {
    return this.db.query("SELECT * FROM students WHERE id = $1", [id]);
  }
}
```

---

## 8. Decorator Composition

Multiple decorators apply bottom-up (closest to the class/method first).

```typescript
// Applied in order: @first, @second, @third
// But evaluated (called) bottom-up: third → second → first

@first
@second
@third
class Foo {}

// Equivalent to: first(second(third(Foo)))
```

```typescript
function A(target: Function) {
  console.log("A: applied");
  return class extends (target as any) {
    label = "A";
  };
}

function B(target: Function) {
  console.log("B: applied");
  return class extends (target as any) {
    label = (this as any).label + "B";
  };
}

@A
@B
class Example {}

// Console:
// B: applied   ← inner first
// A: applied   ← outer second

const e = new (Example as any)();
e.label; // "AB"  (B ran first, set "B", then A prepended "A")
```

---

## 9. Reflect Metadata

`reflect-metadata` is a polyfill for the Reflect.metadata API. It's what allows NestJS to know the TypeScript type of a constructor parameter at runtime — which is how dependency injection works.

```bash
npm install reflect-metadata
```

```typescript
// At the top of your entry file:
import "reflect-metadata";
```

```typescript
// emitDecoratorMetadata: true makes TypeScript emit type information
// that reflect-metadata can read at runtime

class UserService {
  constructor(
    private readonly userRepository: UserRepository,
    private readonly cacheService: CacheService,
  ) {}
}

// With emitDecoratorMetadata, TypeScript emits:
// Reflect.metadata("design:paramtypes", [UserRepository, CacheService])
// on UserService's constructor

// NestJS reads this to know: "I need to inject UserRepository and CacheService"
const paramTypes = Reflect.getMetadata("design:paramtypes", UserService);
// [UserRepository, CacheService] — at runtime!
```

```typescript
// The three metadata keys TypeScript emits:
// "design:type"       — the type of a property
// "design:paramtypes" — the types of constructor/method parameters
// "design:returntype" — the return type of a method

function Injectable(target: Function) {
  // Read the constructor parameter types at runtime
  const deps = Reflect.getMetadata("design:paramtypes", target) || [];
  console.log(
    "Dependencies:",
    deps.map((d: Function) => d.name),
  );
}

@Injectable
class StudentService {
  constructor(
    private repo: StudentRepository,
    private logger: LoggerService,
  ) {}
}
// Console: Dependencies: ["StudentRepository", "LoggerService"]
```

---

## 10. NestJS Decorators — What They Actually Do

NestJS is decorator-first. Every feature — routing, validation, auth, dependency injection — is expressed through decorators.

```typescript
// @Controller — marks a class as a route handler, stores route prefix metadata
@Controller("students")
class StudentController {
  // Equivalent to:
  // Reflect.defineMetadata("path", "students", StudentController)
  // Reflect.defineMetadata("isController", true, StudentController)
}

// @Get, @Post, @Put, @Delete — store HTTP method + path metadata on the method
@Get(":id")
findOne(@Param("id") id: string) { }
// Stores: { method: "GET", path: ":id" } on findOne
// NestJS reads this when building the route table

// @Injectable — marks a class for the DI container
@Injectable()
class StudentService { }
// Marks the class as providable/injectable
// DI container will manage its lifecycle

// @Param, @Body, @Query — mark parameters for extraction from the request
async create(
  @Body() dto: CreateStudentDTO,       // extract req.body
  @Param("id") id: string,             // extract req.params.id
  @Query("page") page: string,         // extract req.query.page
) { }

// @UseGuards — attaches auth guards to a controller or method
@UseGuards(JwtAuthGuard)
@Get("profile")
getProfile(@Request() req) { }

// @UseInterceptors — attaches interceptors (logging, transformation, caching)
@UseInterceptors(CacheInterceptor)
@Get()
findAll() { }

// @Roles — custom metadata for role-based access
@Roles("admin", "teacher")
@Delete(":id")
remove(@Param("id") id: string) { }
// Roles guard reads this metadata to check permissions
```

```typescript
// Custom decorator — combining multiple decorators
import { applyDecorators, UseGuards, SetMetadata } from "@nestjs/common";

export function Auth(...roles: string[]) {
  return applyDecorators(
    SetMetadata("roles", roles),
    UseGuards(JwtAuthGuard, RolesGuard),
  );
}

// Usage — clean call site
@Auth("admin")
@Delete(":id")
remove(@Param("id") id: string) { }
```

---

## 11. Writing Useful Decorators

### Validate class instance

```typescript
// Validates that all required fields are present on an object
function RequiredFields(...fields: string[]) {
  return function <T extends { new (...args: any[]): {} }>(constructor: T) {
    return class extends constructor {
      constructor(...args: any[]) {
        super(...args);
        for (const field of fields) {
          if (
            (this as any)[field] === undefined ||
            (this as any)[field] === null
          ) {
            throw new Error(
              `${constructor.name}: field "${field}" is required`,
            );
          }
        }
      }
    };
  };
}

@RequiredFields("name", "email", "grade")
class Student {
  name: string;
  email: string;
  grade: number;

  constructor(data: Partial<Student>) {
    Object.assign(this, data);
  }
}

new Student({ name: "Ashan", email: "a@school.lk", grade: 11 }); // ✓
new Student({ name: "Ashan" }); // Error: Student: field "email" is required
```

### Throttle method calls

```typescript
function throttle(limitMs: number) {
  return function (
    target: Object,
    key: string,
    descriptor: PropertyDescriptor,
  ) {
    const original = descriptor.value;
    let lastRun = 0;

    descriptor.value = function (...args: any[]) {
      const now = Date.now();
      if (now - lastRun < limitMs) return;
      lastRun = now;
      return original.apply(this, args);
    };

    return descriptor;
  };
}

class ScrollHandler {
  @throttle(100)
  onScroll(event: Event) {
    this.updatePosition();
  }
}
```

---

## Summary

```
What decorators are:
  Functions applied to classes, methods, properties, or parameters
  @Decorator syntax is sugar for: Decorator(target)

Types:
  Class decorator:    (constructor) => constructor | void
  Method decorator:   (target, key, descriptor) => descriptor | void
  Property decorator: (target, key) => void
  Parameter decorator:(target, key, index) => void

Decorator factories:
  Function that returns a decorator — allows configuration
  @log("info") — factory; @log — bare decorator

Composition:
  Multiple decorators on same target — apply bottom-up (inner first)
  @A @B class Foo → A(B(Foo))

Reflect Metadata:
  import "reflect-metadata"
  emitDecoratorMetadata: true in tsconfig
  Lets TypeScript emit type info readable at runtime
  Foundation of NestJS dependency injection

NestJS usage:
  @Controller("path")  — route prefix + marks as controller
  @Get/Post/Put/:path  — HTTP method + route path
  @Injectable()        — register with DI container
  @Body/@Param/@Query  — extract from request
  @UseGuards/@UseInterceptors — attach middleware
  Custom: applyDecorators() — compose multiple into one

Write your own:
  Method: wrap descriptor.value with new function
  Class:  return class extends constructor {}
  Factory: return function(target) {} pattern
```

---

_Next: [07 — Error Handling Patterns in TypeScript](./07%20-%20Error%20Handling%20Patterns.md)_
