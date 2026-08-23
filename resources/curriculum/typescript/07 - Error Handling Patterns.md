> TypeScript doesn't make errors go away — it makes them visible and typed. This file covers how to type errors correctly, how to build custom error hierarchies, and the patterns that separate code that fails safely from code that corrupts state silently.

---

## Table of Contents

1. [The Problem with Untyped Errors](#1-the-problem-with-untyped-errors)
2. [Typing the catch Clause](#2-typing-the-catch-clause)
3. [Custom Error Classes](#3-custom-error-classes)
4. [Error Hierarchies](#4-error-hierarchies)
5. [The Result Pattern](#5-the-result-pattern)
6. [Discriminated Union Errors](#6-discriminated-union-errors)
7. [Never Throw Strings](#7-never-throw-strings)
8. [Type Guards for Errors](#8-type-guards-for-errors)
9. [Async Error Handling](#9-async-error-handling)
10. [Error Handling in NestJS / Express](#10-error-handling-in-nestjs--express)

---

## 1. The Problem with Untyped Errors

TypeScript cannot type what's thrown. The `throw` statement accepts `any` value. This means `catch` blocks receive an `unknown`(in strict mode) or `any`.

```typescript
try {
  doSomethingRisky();
} catch (err) {
  // Without strict mode: err is `any` — no safety
  err.message.toUpperCase(); // TypeScript doesn't catch this if err isn't an Error

  // With useUnknownInCatchVariables: true (default in strict mode)
  // err is `unknown` — must narrow before using
  err.message; // TS Error: Object is of type 'unknown'
}
```

The rule: **`catch` variables are always `unknown` in strict TypeScript**. You must narrow them before use.

---

## 2. Typing the `catch` Clause

```typescript
// Always type err as unknown and narrow it
try {
  await fetchData();
} catch (err: unknown) {
  if (err instanceof Error) {
    console.error(err.message); // ✓ — narrowed to Error
    console.error(err.stack); // ✓
  } else {
    console.error("Unknown error:", err);
  }
}

// Utility: always get a string message from anything thrown
function getErrorMessage(error: unknown): string {
  if (error instanceof Error) return error.message;
  if (typeof error === "string") return error;
  if (
    error !== null &&
    typeof error === "object" &&
    "message" in error &&
    typeof (error as { message: unknown }).message === "string"
  ) {
    return (error as { message: string }).message;
  }
  return String(error);
}

try {
  riskyOperation();
} catch (err) {
  logger.error(getErrorMessage(err)); // always works
}
```

---

## 3. Custom Error Classes

Custom errors let you identify and handle errors by type, not by parsing message strings.

```typescript
// Base application error — always extend Error
class AppError extends Error {
  readonly name: string;

  constructor(message: string) {
    super(message);
    this.name = this.constructor.name;
    // Fix prototype chain (needed for instanceof in some environments)
    Object.setPrototypeOf(this, new.target.prototype);
  }
}

// Domain errors
class NotFoundError extends AppError {
  readonly resource: string;
  readonly id: string;

  constructor(resource: string, id: string) {
    super(`${resource} with id "${id}" not found`);
    this.resource = resource;
    this.id = id;
  }
}

class ValidationError extends AppError {
  readonly fields: Record<string, string[]>;

  constructor(message: string, fields: Record<string, string[]>) {
    super(message);
    this.fields = fields;
  }
}

class UnauthorizedError extends AppError {
  constructor(message = "Authentication required") {
    super(message);
  }
}

class ForbiddenError extends AppError {
  readonly requiredRole: string;

  constructor(requiredRole: string) {
    super(`This action requires the "${requiredRole}" role`);
    this.requiredRole = requiredRole;
  }
}

// Usage
throw new NotFoundError("Student", "s-123");
throw new ValidationError("Invalid form data", { email: ["Invalid format"] });
throw new ForbiddenError("admin");
```

---

## 4. Error Hierarchies

Structure errors in a tree that mirrors your domain. This makes `instanceof` checks at every level possible.

```typescript
// Base
class AppError extends Error {
  constructor(message: string) {
    super(message);
    this.name = this.constructor.name;
    Object.setPrototypeOf(this, new.target.prototype);
  }
}

// HTTP layer
class HttpError extends AppError {
  constructor(
    message: string,
    readonly statusCode: number,
  ) {
    super(message);
  }
}

class BadRequestError extends HttpError {
  constructor(msg: string) {
    super(msg, 400);
  }
}
class UnauthorizedError extends HttpError {
  constructor(msg = "Auth required") {
    super(msg, 401);
  }
}
class ForbiddenError extends HttpError {
  constructor(msg = "Forbidden") {
    super(msg, 403);
  }
}
class NotFoundError extends HttpError {
  constructor(msg: string) {
    super(msg, 404);
  }
}
class ConflictError extends HttpError {
  constructor(msg: string) {
    super(msg, 409);
  }
}
class UnprocessableError extends HttpError {
  constructor(
    msg: string,
    readonly fields?: Record<string, string[]>,
  ) {
    super(msg, 422);
  }
}
class TooManyRequestsError extends HttpError {
  constructor(msg = "Rate limit exceeded") {
    super(msg, 429);
  }
}

// Domain layer
class DomainError extends AppError {}

class InsufficientFundsError extends DomainError {
  constructor(
    readonly available: number,
    readonly required: number,
  ) {
    super(`Insufficient funds: have ${available}, need ${required}`);
  }
}

class DuplicateEmailError extends DomainError {
  constructor(readonly email: string) {
    super(`Email "${email}" is already registered`);
  }
}

// Infrastructure layer
class DatabaseError extends AppError {
  constructor(
    message: string,
    readonly originalError?: Error,
  ) {
    super(message);
  }
}

// Check at any level
try {
  await processRequest();
} catch (err) {
  if (err instanceof HttpError) {
    // any HTTP error — has statusCode
    res.status(err.statusCode).json({ error: err.message });
  } else if (err instanceof DomainError) {
    // domain logic error — always 400-ish
    res.status(400).json({ error: err.message });
  } else if (err instanceof AppError) {
    // any known application error
    res.status(500).json({ error: err.message });
  } else {
    // completely unknown — log with full detail
    logger.error("Unexpected error", err);
    res.status(500).json({ error: "Internal server error" });
  }
}
```

---

## 5. The Result Pattern

Instead of throwing, return a discriminated union of success or failure. The caller is forced to handle both.

```typescript
// Result type — success or failure, never throws
type Ok<T> = { ok: true; value: T };
type Err<E> = { ok: false; error: E };
type Result<T, E = Error> = Ok<T> | Err<E>;

// Constructors
const ok = <T>(value: T): Ok<T> => ({ ok: true, value });
const err = <E>(error: E): Err<E> => ({ ok: false, error });

// Usage in a service function
async function findStudent(
  id: string,
): Promise<Result<Student, NotFoundError | DatabaseError>> {
  try {
    const student = await db.student.findUnique({ where: { id } });
    if (!student) return err(new NotFoundError("Student", id));
    return ok(student);
  } catch (e) {
    return err(
      new DatabaseError("Query failed", e instanceof Error ? e : undefined),
    );
  }
}

// Caller MUST handle both cases — TypeScript enforces it
async function handleGetStudent(id: string) {
  const result = await findStudent(id);

  if (!result.ok) {
    if (result.error instanceof NotFoundError) {
      return res.status(404).json({ error: result.error.message });
    }
    return res.status(500).json({ error: "Database error" });
  }

  // result.value is Student — TypeScript knows
  return res.json(result.value);
}
```

### Chaining Results

```typescript
// Chain operations, short-circuit on first error
function divide(a: number, b: number): Result<number, string> {
  if (b === 0) return err("Division by zero");
  return ok(a / b);
}

function sqrt(n: number): Result<number, string> {
  if (n < 0) return err("Cannot sqrt negative number");
  return ok(Math.sqrt(n));
}

// Chain without nesting
function calculate(a: number, b: number): Result<number, string> {
  const divResult = divide(a, b);
  if (!divResult.ok) return divResult;

  const sqrtResult = sqrt(divResult.value);
  if (!sqrtResult.ok) return sqrtResult;

  return ok(Math.round(sqrtResult.value * 100) / 100);
}
```

---

## 6. Discriminated Union Errors

For domain errors with structured data, use a discriminated union instead of class hierarchies.

```typescript
type StudentError =
  | { type: "NOT_FOUND"; id: string }
  | { type: "DUPLICATE_EMAIL"; email: string }
  | { type: "INVALID_GRADE"; grade: number; min: number; max: number }
  | { type: "ENROLLMENT_FULL"; courseId: string; capacity: number };

async function enrollStudent(
  studentId: string,
  courseId: string,
): Promise<Result<Enrollment, StudentError>> {
  const student = await db.student.findUnique({ where: { id: studentId } });
  if (!student) return err({ type: "NOT_FOUND", id: studentId });

  const course = await db.course.findUnique({ where: { id: courseId } });
  if (!course) return err({ type: "NOT_FOUND", id: courseId });

  const enrollmentCount = await db.enrollment.count({ where: { courseId } });
  if (enrollmentCount >= course.capacity) {
    return err({
      type: "ENROLLMENT_FULL",
      courseId,
      capacity: course.capacity,
    });
  }

  const enrollment = await db.enrollment.create({
    data: { studentId, courseId },
  });
  return ok(enrollment);
}

// Exhaustive handling — TypeScript warns if a case is missed
const result = await enrollStudent(studentId, courseId);
if (!result.ok) {
  switch (result.error.type) {
    case "NOT_FOUND":
      return res
        .status(404)
        .json({ error: `Resource ${result.error.id} not found` });
    case "ENROLLMENT_FULL":
      return res.status(409).json({
        error: `Course is full (capacity: ${result.error.capacity})`,
      });
    case "DUPLICATE_EMAIL":
    case "INVALID_GRADE":
      return res.status(422).json({ error: result.error });
    default:
      const _exhaustive: never = result.error; // TypeScript warns if uncovered
  }
}
```

---

## 7. Never Throw Strings

A common mistake — never throw a raw string, number, or object. Always throw an Error instance.

```typescript
// ✗ Never do this
throw "Something went wrong";
throw 404;
throw { code: "NOT_FOUND", message: "Student not found" };

// Why it's bad:
try {
  throw "Failure";
} catch (err) {
  err instanceof Error; // false — plain string
  err.stack; // undefined — no stack trace
  err.message; // undefined — strings don't have .message
}

// ✓ Always throw an Error or Error subclass
throw new Error("Something went wrong");
throw new NotFoundError("Student", id);
throw new ValidationError("Invalid input", fields);

// ✓ The catch block then works correctly
try {
  throw new NotFoundError("Student", id);
} catch (err) {
  err instanceof Error; // true
  err instanceof AppError; // true
  err instanceof NotFoundError; // true
  err.stack; // full stack trace
  err.message; // "Student with id "..." not found"
}
```

---

## 8. Type Guards for Errors

```typescript
// Type guard for AppError
function isAppError(value: unknown): value is AppError {
  return value instanceof AppError;
}

// Type guard for specific error types
function isNotFoundError(value: unknown): value is NotFoundError {
  return value instanceof NotFoundError;
}

function isHttpError(value: unknown): value is HttpError {
  return value instanceof HttpError;
}

// Type guard for error-like objects (for external errors)
function isErrorLike(
  value: unknown,
): value is { message: string; stack?: string } {
  return (
    value !== null &&
    typeof value === "object" &&
    "message" in value &&
    typeof (value as any).message === "string"
  );
}

// Comprehensive error handler
function handleError(err: unknown): { statusCode: number; message: string } {
  if (err instanceof HttpError) {
    return { statusCode: err.statusCode, message: err.message };
  }
  if (err instanceof DomainError) {
    return { statusCode: 400, message: err.message };
  }
  if (err instanceof AppError) {
    return { statusCode: 500, message: err.message };
  }
  if (isErrorLike(err)) {
    return { statusCode: 500, message: err.message };
  }
  return { statusCode: 500, message: "An unexpected error occurred" };
}
```

---

## 9. Async Error Handling

```typescript
// Typed async wrapper — no try/catch boilerplate at call sites
async function tryCatch<T>(
  promise: Promise<T>,
): Promise<[T, null] | [null, Error]> {
  try {
    return [await promise, null];
  } catch (err) {
    return [null, err instanceof Error ? err : new Error(String(err))];
  }
}

// Clean call sites
async function getStudentProfile(id: string) {
  const [student, err] = await tryCatch(fetchStudent(id));
  if (err) {
    logger.error("fetchStudent failed", { id, error: err.message });
    return null;
  }
  return student;
}

// Promise.allSettled with typed errors
async function loadDashboard(userId: string) {
  const results = await Promise.allSettled([
    fetchUser(userId),
    fetchPosts(userId),
    fetchStats(userId),
  ]);

  const [userResult, postsResult, statsResult] = results;

  if (userResult.status === "rejected") {
    // Critical — can't render without user
    throw userResult.reason instanceof Error
      ? userResult.reason
      : new Error("Failed to load user");
  }

  return {
    user: userResult.value,
    posts: postsResult.status === "fulfilled" ? postsResult.value : [],
    stats: statsResult.status === "fulfilled" ? statsResult.value : null,
  };
}
```

---

## 10. Error Handling in NestJS / Express

### NestJS — HTTP Exceptions

```typescript
import {
  BadRequestException,
  NotFoundException,
  UnauthorizedException,
  ForbiddenException,
  ConflictException,
  UnprocessableEntityException,
  InternalServerErrorException,
} from "@nestjs/common";

@Injectable()
class StudentService {
  async findById(id: string): Promise<Student> {
    const student = await this.repo.findById(id);
    if (!student) {
      throw new NotFoundException(`Student ${id} not found`);
      // NestJS catches this and sends 404 with the message
    }
    return student;
  }

  async create(dto: CreateStudentDto): Promise<Student> {
    const existing = await this.repo.findByEmail(dto.email);
    if (existing) {
      throw new ConflictException(`Email ${dto.email} is already registered`);
    }
    return this.repo.create(dto);
  }
}
```

### NestJS — Global Exception Filter

```typescript
import {
  ExceptionFilter,
  Catch,
  ArgumentsHost,
  HttpException,
  HttpStatus,
  Logger,
} from "@nestjs/common";
import { Request, Response } from "express";

@Catch()
export class GlobalExceptionFilter implements ExceptionFilter {
  private readonly logger = new Logger(GlobalExceptionFilter.name);

  catch(exception: unknown, host: ArgumentsHost) {
    const ctx = host.switchToHttp();
    const response = ctx.getResponse<Response>();
    const request = ctx.getRequest<Request>();

    let statusCode = HttpStatus.INTERNAL_SERVER_ERROR;
    let message = "Internal server error";
    let details: unknown;

    if (exception instanceof HttpException) {
      statusCode = exception.getStatus();
      const body = exception.getResponse();
      message = typeof body === "string" ? body : (body as any).message;
      details = typeof body === "object" ? (body as any).details : undefined;
    } else if (exception instanceof DomainError) {
      statusCode = HttpStatus.BAD_REQUEST;
      message = exception.message;
    } else if (exception instanceof Error) {
      this.logger.error(exception.message, exception.stack);
    }

    response.status(statusCode).json({
      statusCode,
      message,
      details,
      timestamp: new Date().toISOString(),
      path: request.url,
    });
  }
}
```

---

## Summary

```
catch variables:
  Always `unknown` in strict mode — must narrow before use
  if (err instanceof Error) — standard narrowing
  getErrorMessage(err) — utility to always get a string

Custom errors:
  Extend Error, call super(message), set this.name
  Fix prototype chain: Object.setPrototypeOf(this, new.target.prototype)
  Add structured fields: statusCode, resource, fields, etc.

Error hierarchy:
  AppError → HttpError → NotFoundException etc.
  AppError → DomainError → business-specific errors
  instanceof checks at any level

Result pattern:
  { ok: true; value: T } | { ok: false; error: E }
  Forces callers to handle both outcomes
  No exceptions thrown — return the error as a value

Discriminated union errors:
  { type: "NOT_FOUND"; id: string } | { type: "ENROLLMENT_FULL"; ... }
  switch on type — TypeScript warns if a case is uncovered

Never throw:
  Strings, numbers, plain objects
  Always throw Error or a subclass
  Stack traces only work on Error instances

Type guards:
  value instanceof AppError
  Custom isErrorLike() for external/unknown errors

Async patterns:
  tryCatch() wrapper — go-style [result, error]
  Promise.allSettled — check .status on each result
```

---

_Next: [08 — TypeScript with Zod, Prisma & tRPC](./08%20-%20TypeScript%20with%20Zod%2C%20Prisma%20%26%20tRPC.md)_
