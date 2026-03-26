> **Part 5 of 5.** Production TypeScript — the ideal `tsconfig.json` setup explained line-by-line, declaration files (`.d.ts`) and the `@types` ecosystem, TypeScript with Axios (fully typed HTTP client), and TypeScript with Express (typed routes, middleware, request augmentation).

---

## Table of Contents

1. [Advanced TypeScript Setup](#1-advanced-typescript-setup)
2. [tsconfig.json — Every Option Explained](#2-tsconfigjson--every-option-explained)
3. [Declaration Files](#3-declaration-files)
4. [The `@types` Ecosystem](#4-the-types-ecosystem)
5. [Writing Your Own Declaration Files](#5-writing-your-own-declaration-files)
6. [TypeScript with Axios](#6-typescript-with-axios)
7. [TypeScript with Express](#7-typescript-with-express)

---

## 1. Advanced TypeScript Setup

### Recommended Project Structure

```
my-api/
├── src/
│   ├── types/
│   │   ├── index.ts          // re-exports all types
│   │   ├── models.ts         // domain model types
│   │   ├── dtos.ts           // Data Transfer Object types
│   │   ├── api.ts            // API request/response types
│   │   └── env.d.ts          // environment variable types
│   ├── config/
│   │   └── index.ts
│   ├── middleware/
│   │   ├── auth.ts
│   │   └── errorHandler.ts
│   ├── controllers/
│   │   └── user.controller.ts
│   ├── services/
│   │   └── user.service.ts
│   ├── repositories/
│   │   └── user.repository.ts
│   └── index.ts
├── @types/                   // custom declaration files
│   └── express/
│       └── index.d.ts        // Express Request augmentation
├── dist/                     // compiled output
├── tsconfig.json
├── tsconfig.build.json       // tsconfig for production build
└── package.json
```

### package.json Scripts

```json
{
  "scripts": {
    "dev": "tsx watch src/index.ts",
    "build": "tsc --project tsconfig.build.json",
    "start": "node dist/index.js",
    "typecheck": "tsc --noEmit",
    "typecheck:watch": "tsc --noEmit --watch",
    "lint": "eslint src --ext .ts"
  },
  "devDependencies": {
    "typescript": "^5.3.0",
    "tsx": "^4.6.0",
    "@types/node": "^20.10.0",
    "@types/express": "^4.17.21",
    "ts-node": "^10.9.0"
  }
}
```

---

## 2. tsconfig.json — Every Option Explained

### Development Config

```json
{
  "compilerOptions": {

    // ───────────────────────────────────────────
    // TARGET & MODULE
    // ───────────────────────────────────────────

    "target": "ES2022",
    // Which JavaScript version to emit.
    // ES2022: has async/await, optional chaining, nullish coalescing, class fields.
    // Use "ES2017" minimum for Node.js 12+, "ES2022" for Node.js 16+.

    "module": "CommonJS",
    // Module system for output.
    // "CommonJS" for Node.js (require/module.exports)
    // "ESNext" or "ES2022" for bundlers (Vite, webpack, esbuild)
    // "NodeNext" for modern Node.js ESM projects

    "lib": ["ES2022"],
    // Built-in type definitions to include.
    // "ES2022" — all modern JS APIs
    // "DOM" — browser APIs (window, document, fetch)
    // "DOM.Iterable" — makes DOM collections iterable
    // Don't include "DOM" for Node.js-only projects

    // ───────────────────────────────────────────
    // OUTPUT
    // ───────────────────────────────────────────

    "outDir": "./dist",
    // Where compiled .js files go.
    // Always add /dist to .gitignore

    "rootDir": "./src",
    // Root of your TypeScript source files.
    // Ensures folder structure is replicated in outDir.

    "declaration": true,
    // Generate .d.ts declaration files alongside .js files.
    // Required if publishing a library.

    "declarationMap": true,
    // Generate .d.ts.map files — lets IDEs navigate from .d.ts back to .ts source.

    "sourceMap": true,
    // Generate .js.map files — maps compiled JS back to TS source for debugging.
    // Essential for meaningful stack traces in production.

    "removeComments": false,
    // Whether to strip comments from output. Set true for production.

    // ───────────────────────────────────────────
    // STRICT TYPE CHECKING (ALWAYS ENABLE ALL)
    // ───────────────────────────────────────────

    "strict": true,
    // Enables all strict checks at once. Equivalent to enabling all below:

    "noImplicitAny": true,
    // Error when TypeScript would infer `any`.
    // function foo(x) {} — TS Error: x implicitly has 'any' type

    "strictNullChecks": true,
    // null and undefined are NOT assignable to other types unless explicitly included.
    // Without this: string and string | null are treated the same — dangerous!
    // With this: you must handle null/undefined explicitly.

    "strictFunctionTypes": true,
    // Stricter checking of function parameter types (contravariance).
    // Prevents subtle type safety holes in callbacks.

    "strictBindCallApply": true,
    // Strict type checking for .bind(), .call(), .apply().

    "strictPropertyInitialization": true,
    // Class properties must be initialized in the constructor or have a definite assignment assertion.

    "noImplicitThis": true,
    // Error when `this` would be implicitly `any`.

    "useUnknownInCatchVariables": true,
    // In catch blocks, the caught variable is `unknown` instead of `any`.
    // Forces you to check error type before using it.
    // try { } catch (err) { err.message } — TS Error: err is unknown

    "alwaysStrict": true,
    // Emit "use strict" in all output files.

    // ───────────────────────────────────────────
    // ADDITIONAL STRICTNESS (HIGHLY RECOMMENDED)
    // ───────────────────────────────────────────

    "noUnusedLocals": true,
    // Error on declared but unused local variables.
    // const x = 5; — TS Error if x is never read

    "noUnusedParameters": true,
    // Error on declared but unused function parameters.
    // function fn(a: string, b: number) { return a; } — Error: b unused

    "noImplicitReturns": true,
    // All code paths in a function must return a value.
    // function fn(x: boolean): string { if (x) return "yes"; } — Error: missing return

    "noFallthroughCasesInSwitch": true,
    // Error on switch cases that fall through (missing break/return).

    "exactOptionalPropertyTypes": true,
    // Makes optional properties stricter.
    // type A = { x?: string }
    // Without this: you can set `obj.x = undefined`
    // With this: you can only set `obj.x = "value"` or omit it entirely.
    // This is more correct but more strict — enable carefully.

    // ───────────────────────────────────────────
    // MODULE RESOLUTION
    // ───────────────────────────────────────────

    "moduleResolution": "node",
    // How TypeScript resolves imports.
    // "node" — traditional Node.js resolution (most common)
    // "node16" / "nodenext" — for native ESM Node.js projects
    // "bundler" — for use with Vite/webpack/esbuild

    "esModuleInterop": true,
    // Allows `import express from 'express'` instead of `import * as express from 'express'`
    // Makes working with CommonJS modules much nicer.
    // Required for most popular Node.js libraries.

    "allowSyntheticDefaultImports": true,
    // Allow default imports from modules with no default export.
    // Automatically enabled when esModuleInterop is true.

    "resolveJsonModule": true,
    // Allows importing .json files directly.
    // import config from './config.json'; — config is typed based on the JSON structure!

    "baseUrl": "./src",
    // Base directory for non-relative module names.
    // Enables: import { UserService } from "services/user"
    // Instead of: import { UserService } from "../../services/user"

    "paths": {
      "@/*": ["./*"],
      "@types/*": ["types/*"],
      "@services/*": ["services/*"],
      "@repositories/*": ["repositories/*"],
      "@controllers/*": ["controllers/*"],
      "@middleware/*": ["middleware/*"]
    },
    // Path aliases for cleaner imports.
    // Note: tsc doesn't rewrite these — you need tsconfig-paths or a bundler.

    // ───────────────────────────────────────────
    // TYPE DEFINITIONS
    // ───────────────────────────────────────────

    "types": ["node"],
    // Limit which @types packages are globally included.
    // Without this: ALL installed @types are included.
    // Be explicit: ["node"] for Node.js, add "jest" for tests, etc.

    "typeRoots": ["./node_modules/@types", "./@types"],
    // Where to look for type definitions.
    // ./@types — your custom declaration files

    // ───────────────────────────────────────────
    // ADVANCED
    // ───────────────────────────────────────────

    "skipLibCheck": true,
    // Skip type checking of declaration files in node_modules.
    // Dramatically speeds up compilation.
    // Safe because library authors are responsible for their .d.ts correctness.

    "forceConsistentCasingInFileNames": true,
    // Error if a file is imported with different casing than its actual name.
    // Prevents bugs on case-sensitive Linux servers when developed on macOS/Windows.

    "incremental": true,
    // Store information from the last compilation to speed up subsequent ones.
    // Creates a .tsbuildinfo file.

    "experimentalDecorators": true,
    // Enable decorators (@Injectable, @Controller, etc.)
    // Required for NestJS, TypeORM, and similar frameworks.

    "emitDecoratorMetadata": true
    // Emit type metadata for decorators.
    // Required for NestJS (dependency injection) and TypeORM.
  },

  "include": ["src/**/*", "@types/**/*"],
  "exclude": ["node_modules", "dist", "**/*.test.ts", "**/*.spec.ts"]
}
```

### Separate Build Config

```json
// tsconfig.build.json — for production builds (excludes test files, enables optimizations)
{
  "extends": "./tsconfig.json",
  "compilerOptions": {
    "removeComments": true,
    "incremental": false
  },
  "exclude": ["node_modules", "dist", "**/*.test.ts", "**/*.spec.ts", "**/*.e2e-spec.ts"]
}
```

### Typing Environment Variables

```ts
// src/types/env.d.ts
// Augments the Node.js `process.env` type to include your environment variables

declare global {
  namespace NodeJS {
    interface ProcessEnv {
      NODE_ENV: "development" | "staging" | "production" | "test";
      PORT?: string;
      DATABASE_URL: string;
      JWT_SECRET: string;
      JWT_EXPIRES_IN: string;
      REFRESH_TOKEN_SECRET: string;
      REDIS_URL?: string;
      SMTP_HOST?: string;
      SMTP_PORT?: string;
      SMTP_USER?: string;
      SMTP_PASS?: string;
      AWS_ACCESS_KEY_ID?: string;
      AWS_SECRET_ACCESS_KEY?: string;
      AWS_REGION?: string;
      AWS_S3_BUCKET?: string;
    }
  }
}

export {}; // Makes this a module (required for global augmentation)
```

```ts
// src/config/index.ts — validated config with proper types
function requireEnv(key: keyof NodeJS.ProcessEnv): string {
  const value = process.env[key];
  if (!value) throw new Error(`Required environment variable "${key}" is not set`);
  return value;
}

export const config = {
  env: (process.env.NODE_ENV ?? "development") as "development" | "staging" | "production",
  port: parseInt(process.env.PORT ?? "3000", 10),
  database: {
    url: requireEnv("DATABASE_URL"),
  },
  jwt: {
    secret: requireEnv("JWT_SECRET"),
    expiresIn: requireEnv("JWT_EXPIRES_IN"),
    refreshSecret: requireEnv("REFRESH_TOKEN_SECRET"),
  },
  redis: {
    url: process.env.REDIS_URL,
  },
} as const;

// Now TypeScript knows the exact shape of your config
config.jwt.secret;   // string
config.port;         // number
config.redis.url;    // string | undefined
```

---

## 3. Declaration Files

A **declaration file** (`.d.ts`) describes the **types** of a JavaScript module without containing any implementation. It's how TypeScript gets type information for libraries written in plain JavaScript.

### What's Inside a Declaration File

```ts
// myLibrary.d.ts — describes the types of myLibrary.js

// Export a function
export declare function formatDate(date: Date, format: string): string;

// Export a class
export declare class EventEmitter<Events extends Record<string, unknown>> {
  on<E extends keyof Events>(event: E, handler: (data: Events[E]) => void): this;
  emit<E extends keyof Events>(event: E, data: Events[E]): boolean;
  off<E extends keyof Events>(event: E, handler: (data: Events[E]) => void): this;
}

// Export an interface
export declare interface Config {
  apiUrl: string;
  timeout: number;
  retries?: number;
}

// Export a const
export declare const VERSION: string;

// Export a type
export declare type Callback<T> = (error: Error | null, result: T | null) => void;

// Default export
export default declare function createServer(config: Config): Server;
```

### Ambient Declarations

For globals injected at runtime (not from an import):

```ts
// globals.d.ts
// Declare a global variable
declare const __APP_VERSION__: string;
declare const __IS_PRODUCTION__: boolean;

// Declare a global function
declare function requireAuth(): void;

// Extend a global interface
declare interface Window {
  analytics: {
    track(event: string, properties?: Record<string, unknown>): void;
    identify(userId: string, traits?: Record<string, unknown>): void;
  };
  dataLayer: unknown[];
}

// Declare a module for non-JS assets
declare module "*.svg" {
  const content: string;
  export default content;
}

declare module "*.png" {
  const content: string;
  export default content;
}

declare module "*.json" {
  const content: Record<string, unknown>;
  export default content;
}

// Declare a module with no types (last resort)
declare module "some-untyped-library" {
  const value: unknown;
  export default value;
}
```

---

## 4. The `@types` Ecosystem

**DefinitelyTyped** is a massive community repository of `.d.ts` files for popular JavaScript libraries. These are published as `@types/package-name` on npm.

### Installing Type Definitions

```bash
# Express
npm install express
npm install --save-dev @types/express

# Lodash
npm install lodash
npm install --save-dev @types/lodash

# Node.js built-ins
npm install --save-dev @types/node

# JWT
npm install jsonwebtoken
npm install --save-dev @types/jsonwebtoken

# bcrypt
npm install bcryptjs
npm install --save-dev @types/bcryptjs

# multer
npm install multer
npm install --save-dev @types/multer

# CORS
npm install cors
npm install --save-dev @types/cors
```

### Libraries with Built-in Types

Many modern libraries ship their own `.d.ts` files and don't need `@types`:

```bash
# These have built-in TypeScript support:
npm install axios          # axios/index.d.ts included
npm install zod            # zod types included
npm install typescript     # obviously
npm install prisma         # generates types
npm install @nestjs/core   # NestJS is written in TypeScript
npm install date-fns       # includes types
```

### Checking for Types

```bash
# Check if @types/package-name exists
npm info @types/express

# Or check the TypeSearch registry
# https://www.typescriptlang.org/dt/search
```

---

## 5. Writing Your Own Declaration Files

### Scenario 1: Typing a Legacy JS Module in Your Project

```ts
// src/legacy/utils.js (plain JS — not TypeScript)
function calculateDiscount(price, percent, maxDiscount) {
  const discount = price * (percent / 100);
  return Math.min(discount, maxDiscount ?? Infinity);
}
module.exports = { calculateDiscount };

// src/legacy/utils.d.ts — declaration file alongside it
export declare function calculateDiscount(
  price: number,
  percent: number,
  maxDiscount?: number
): number;
```

### Scenario 2: Augmenting Express Request (Most Common)

```ts
// @types/express/index.d.ts
// This file augments the express module's Request type globally

import { UserRole } from "../../src/types/models";

declare global {
  namespace Express {
    // Augment the Request interface
    interface Request {
      user?: {
        id: string;
        email: string;
        role: UserRole;
        iat: number;
        exp: number;
      };
      requestId: string;
      startTime: number;
    }
  }
}

export {};
```

### Scenario 3: Library Without Types

```ts
// @types/some-legacy-lib/index.d.ts
declare module "some-legacy-lib" {
  // Best effort typing of the library

  export interface Connection {
    connect(host: string, port: number): Promise<void>;
    disconnect(): void;
    query<T = unknown>(sql: string, params?: unknown[]): Promise<T[]>;
    isConnected: boolean;
  }

  export function createConnection(config: {
    host: string;
    port: number;
    database: string;
    user: string;
    password: string;
  }): Connection;

  export class SomeLegacyError extends Error {
    code: string;
    sqlState: string;
  }

  // If you can't type everything, use any for the rest
  export function complexLegacyFunction(...args: unknown[]): unknown;

  const lib: {
    createConnection: typeof createConnection;
    SomeLegacyError: typeof SomeLegacyError;
  };
  export default lib;
}
```

---

## 6. TypeScript with Axios

Axios has excellent built-in TypeScript support. The key is using its generic type parameters correctly.

### Installation

```bash
npm install axios
# No @types/axios needed — axios ships its own .d.ts
```

### The Axios Generic: `axios.get<T>(url)`

```ts
import axios, { AxiosResponse, AxiosError } from "axios";

// T is the type of response.data
const response = await axios.get<User>("/api/users/1");
// response.data: User — fully typed!

const listResponse = await axios.get<User[]>("/api/users");
// listResponse.data: User[] — fully typed!
```

### Building a Typed API Client with Axios

```ts
import axios, {
  AxiosInstance,
  AxiosRequestConfig,
  AxiosError,
  InternalAxiosRequestConfig,
} from "axios";

// Types for all API responses
interface ApiSuccessResponse<T> {
  success: true;
  data: T;
  message: string;
  timestamp: string;
}

interface ApiErrorResponse {
  success: false;
  error: string;
  code: string;
  statusCode: number;
  details?: Record<string, string[]>;
}

type ApiResponse<T> = ApiSuccessResponse<T> | ApiErrorResponse;

// Create a typed API client
class ApiClient {
  private readonly client: AxiosInstance;

  constructor(baseURL: string, private getToken?: () => string | null) {
    this.client = axios.create({
      baseURL,
      timeout: 10_000,
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
      },
    });

    this.setupInterceptors();
  }

  private setupInterceptors(): void {
    // Request interceptor — attach auth token
    this.client.interceptors.request.use(
      (config: InternalAxiosRequestConfig): InternalAxiosRequestConfig => {
        const token = this.getToken?.();
        if (token) {
          config.headers.Authorization = `Bearer ${token}`;
        }
        config.headers["X-Request-ID"] = crypto.randomUUID();
        return config;
      },
      (error: AxiosError) => Promise.reject(error)
    );

    // Response interceptor — unwrap data or handle errors
    this.client.interceptors.response.use(
      (response: AxiosResponse<ApiResponse<unknown>>) => response,
      (error: AxiosError<ApiErrorResponse>) => {
        // Handle specific HTTP status codes globally
        if (error.response?.status === 401) {
          this.handleUnauthorized();
        }
        return Promise.reject(this.transformError(error));
      }
    );
  }

  private handleUnauthorized(): void {
    // Dispatch event, redirect to login, clear tokens, etc.
    window.dispatchEvent(new CustomEvent("auth:expired"));
  }

  private transformError(error: AxiosError<ApiErrorResponse>): Error {
    const apiError = error.response?.data;
    if (apiError && !apiError.success) {
      const err = new Error(apiError.error) as Error & {
        code: string;
        statusCode: number;
        details?: Record<string, string[]>;
      };
      err.code = apiError.code;
      err.statusCode = apiError.statusCode;
      err.details = apiError.details;
      return err;
    }
    if (error.code === "ECONNABORTED") return new Error("Request timed out");
    if (!error.response) return new Error("Network error — check your connection");
    return new Error(`HTTP ${error.response.status}: ${error.message}`);
  }

  // Generic GET — T is the type of the actual data payload
  async get<T>(
    url: string,
    config?: AxiosRequestConfig
  ): Promise<T> {
    const response = await this.client.get<ApiSuccessResponse<T>>(url, config);
    return response.data.data;
  }

  async post<TData, TResponse>(
    url: string,
    data: TData,
    config?: AxiosRequestConfig
  ): Promise<TResponse> {
    const response = await this.client.post<ApiSuccessResponse<TResponse>>(url, data, config);
    return response.data.data;
  }

  async put<TData, TResponse>(
    url: string,
    data: TData,
    config?: AxiosRequestConfig
  ): Promise<TResponse> {
    const response = await this.client.put<ApiSuccessResponse<TResponse>>(url, data, config);
    return response.data.data;
  }

  async patch<TData, TResponse>(
    url: string,
    data: TData,
    config?: AxiosRequestConfig
  ): Promise<TResponse> {
    const response = await this.client.patch<ApiSuccessResponse<TResponse>>(url, data, config);
    return response.data.data;
  }

  async delete<TResponse = void>(
    url: string,
    config?: AxiosRequestConfig
  ): Promise<TResponse> {
    const response = await this.client.delete<ApiSuccessResponse<TResponse>>(url, config);
    return response.data.data;
  }
}

// Domain-specific API modules
const apiClient = new ApiClient(
  process.env.REACT_APP_API_URL!,
  () => localStorage.getItem("accessToken")
);

// Typed API modules
const userApi = {
  getById: (id: string) =>
    apiClient.get<User>(`/users/${id}`),

  list: (params?: UserQueryParams) =>
    apiClient.get<PaginatedResult<User>>("/users", { params }),

  create: (data: CreateUserDTO) =>
    apiClient.post<CreateUserDTO, User>("/users", data),

  update: (id: string, data: UpdateUserDTO) =>
    apiClient.patch<UpdateUserDTO, User>(`/users/${id}`, data),

  delete: (id: string) =>
    apiClient.delete(`/users/${id}`),

  changePassword: (id: string, data: { currentPassword: string; newPassword: string }) =>
    apiClient.post<typeof data, void>(`/users/${id}/change-password`, data),
};

const authApi = {
  login: (credentials: { email: string; password: string }) =>
    apiClient.post<typeof credentials, { accessToken: string; refreshToken: string }>(
      "/auth/login",
      credentials
    ),

  logout: () => apiClient.post<void, void>("/auth/logout", undefined),

  refresh: (refreshToken: string) =>
    apiClient.post<{ refreshToken: string }, { accessToken: string }>(
      "/auth/refresh",
      { refreshToken }
    ),
};

// Usage — completely type-safe
const user = await userApi.getById("u1");
user.name;           // string ✓
user.unknownField;   // TS Error ✓

const newUser = await userApi.create({
  name: "Alice",
  email: "alice@example.com",
  password: "securepass123",
  role: "user",
});
newUser.id;          // string ✓ (server-generated, in response)
```

### Typing Axios Errors

```ts
import { isAxiosError } from "axios";

async function handleApiCall(): Promise<void> {
  try {
    await userApi.create(data);
  } catch (err: unknown) {
    // With useUnknownInCatchVariables: true, err is `unknown`
    // Use isAxiosError type guard to narrow

    if (isAxiosError<ApiErrorResponse>(err)) {
      // err.response?.data is typed as ApiErrorResponse
      const statusCode = err.response?.status;
      const apiError = err.response?.data;

      if (statusCode === 422 && apiError?.details) {
        // Validation errors
        showFieldErrors(apiError.details);
      } else if (statusCode === 409) {
        showToast("error", "That email is already registered");
      } else {
        showToast("error", apiError?.error ?? "Request failed");
      }
    } else if (err instanceof Error) {
      showToast("error", err.message);
    }
  }
}
```

---

## 7. TypeScript with Express

### Installation

```bash
npm install express
npm install --save-dev @types/express @types/node
```

### Basic Typed Express App

```ts
import express, { Application, Request, Response, NextFunction } from "express";

const app: Application = express();
app.use(express.json());

// Route handler with typed request/response
app.get("/", (req: Request, res: Response): void => {
  res.json({ message: "Hello World" });
});

app.listen(3000, () => console.log("Server running"));
```

### Typing Route Parameters, Query, and Body

```ts
import { Request, Response, NextFunction } from "express";

// Generic type parameters on Request:
// Request<Params, ResBody, ReqBody, Query>

// GET /users/:id
app.get(
  "/users/:id",
  async (
    req: Request<{ id: string }>,  // Params
    res: Response<User | ErrorBody>  // Response body
  ): Promise<void> => {
    const { id } = req.params;  // id: string ✓

    const user = await userRepo.findById(id);
    if (!user) {
      res.status(404).json({ error: "User not found" });
      return;
    }
    res.json(user);
  }
);

// GET /users?page=1&limit=20&search=alice
interface UserQueryParams {
  page?: string;    // query params are always strings
  limit?: string;
  search?: string;
  role?: string;
}

app.get(
  "/users",
  async (
    req: Request<{}, {}, {}, UserQueryParams>,  // empty params, empty body, typed query
    res: Response<PaginatedResult<User>>
  ): Promise<void> => {
    const {
      page = "1",
      limit = "20",
      search,
      role,
    } = req.query;  // UserQueryParams ✓

    const result = await userRepo.findAll({
      page: parseInt(page, 10),
      limit: parseInt(limit, 10),
      search,
      role: role as UserRole | undefined,
    });

    res.json(result);
  }
);

// POST /users
app.post(
  "/users",
  async (
    req: Request<{}, {}, CreateUserDTO>,  // typed request body
    res: Response<User | ErrorBody>
  ): Promise<void> => {
    const dto: CreateUserDTO = req.body;  // ✓ typed as CreateUserDTO

    // But: Express doesn't validate at runtime!
    // Use Zod, Joi, or class-validator to validate dto at runtime
    const validation = CreateUserSchema.safeParse(dto);
    if (!validation.success) {
      res.status(400).json({ error: "Validation failed", details: validation.error.issues });
      return;
    }

    const user = await userService.create(validation.data);
    res.status(201).json(user);
  }
);
```

### Typed Middleware

```ts
import { Request, Response, NextFunction, RequestHandler } from "express";

// Auth middleware — attaches user to request
export const authMiddleware: RequestHandler = async (
  req: Request,
  res: Response,
  next: NextFunction
): Promise<void> => {
  const authHeader = req.headers.authorization;

  if (!authHeader?.startsWith("Bearer ")) {
    res.status(401).json({ error: "No authorization header" });
    return; // important: return after sending response to prevent next() being called
  }

  const token = authHeader.slice(7);

  try {
    const payload = verifyToken(token);
    req.user = payload; // TypeScript Error without augmentation!
    next();
  } catch {
    res.status(401).json({ error: "Invalid or expired token" });
  }
};
```

### Augmenting the Request Type

```ts
// @types/express/index.d.ts
// This file extends Express's built-in types globally

declare global {
  namespace Express {
    interface Request {
      user?: {
        id: string;
        email: string;
        role: "admin" | "user" | "moderator";
        iat: number;
        exp: number;
      };
      requestId: string;
      startTime: number;
    }
  }
}

export {};
```

Now `req.user` is typed everywhere in your Express app.

### Typed Router

```ts
import { Router, Request, Response, RequestHandler } from "express";

const router = Router();

// Type-safe route builder helper
function asyncHandler(
  fn: (req: Request, res: Response, next: NextFunction) => Promise<void>
): RequestHandler {
  return (req, res, next) => fn(req, res, next).catch(next);
}

// Auth-required routes
router.use(authMiddleware);

router.get(
  "/profile",
  asyncHandler(async (req, res) => {
    // req.user is typed here because of our augmentation
    const user = await userRepo.findById(req.user!.id);
    if (!user) {
      res.status(404).json({ error: "User not found" });
      return;
    }
    res.json(omit(user, "passwordHash"));
  })
);

router.put(
  "/profile",
  asyncHandler(async (req: Request<{}, {}, UpdateProfileDTO>, res) => {
    const updated = await userRepo.update(req.user!.id, req.body);
    res.json(updated);
  })
);
```

### Error Handling Middleware

```ts
import { Request, Response, NextFunction } from "express";

// TypeScript needs 4 parameters for error middleware — even if you don't use `next`
function errorHandler(
  err: unknown,  // `unknown` because useUnknownInCatchVariables is on
  req: Request,
  res: Response,
  next: NextFunction  // Required for Express to recognize it as error middleware
): void {
  // Log the error
  console.error({
    requestId: req.requestId,
    url: req.url,
    method: req.method,
    error: err instanceof Error ? err.message : err,
    stack: err instanceof Error ? err.stack : undefined,
  });

  // Handle known error types
  if (err instanceof ZodError) {
    res.status(400).json({
      success: false,
      error: "Validation failed",
      code: "VALIDATION_ERROR",
      details: err.flatten().fieldErrors,
    });
    return;
  }

  if (err instanceof NotFoundError) {
    res.status(404).json({
      success: false,
      error: err.message,
      code: "NOT_FOUND",
    });
    return;
  }

  if (err instanceof UnauthorizedError) {
    res.status(401).json({
      success: false,
      error: err.message,
      code: "UNAUTHORIZED",
    });
    return;
  }

  if (err instanceof ForbiddenError) {
    res.status(403).json({
      success: false,
      error: err.message,
      code: "FORBIDDEN",
    });
    return;
  }

  // Unknown errors — don't leak details
  res.status(500).json({
    success: false,
    error: "An unexpected error occurred",
    code: "INTERNAL_ERROR",
  });
}

app.use(errorHandler);
```

### Full Typed Express App Example

```ts
// src/index.ts
import express, { Application } from "express";
import cors from "cors";
import helmet from "helmet";
import { config } from "./config";
import { userRouter } from "./routes/user.routes";
import { authRouter } from "./routes/auth.routes";
import { requestLogger } from "./middleware/requestLogger";
import { errorHandler } from "./middleware/errorHandler";
import { notFoundHandler } from "./middleware/notFoundHandler";

function createApp(): Application {
  const app = express();

  // Middleware
  app.use(helmet());
  app.use(cors({ origin: config.corsOrigin, credentials: true }));
  app.use(express.json({ limit: "10mb" }));
  app.use(express.urlencoded({ extended: true }));
  app.use(requestLogger);

  // Health check
  app.get("/health", (_, res) => {
    res.json({ status: "ok", timestamp: new Date().toISOString(), version: config.version });
  });

  // Routes
  app.use("/api/v1/auth", authRouter);
  app.use("/api/v1/users", userRouter);

  // Error handling (must be last)
  app.use(notFoundHandler);
  app.use(errorHandler);

  return app;
}

async function bootstrap(): Promise<void> {
  const app = createApp();

  app.listen(config.port, () => {
    console.log(`Server running on port ${config.port} [${config.env}]`);
  });
}

bootstrap().catch(console.error);
```

---

## Final Notes & Best Practices

### TypeScript Patterns Summary

```ts
// ✅ Use strict mode — always
{ "strict": true }

// ✅ Type your environment variables
declare namespace NodeJS { interface ProcessEnv { DATABASE_URL: string; } }

// ✅ Use unknown over any in catch blocks
try { } catch (err: unknown) { }

// ✅ Use type guards before accessing unknown values
if (isAxiosError(err)) { err.response?.status; }

// ✅ Prefer interfaces for class contracts
interface IUserRepository { findById(id: string): Promise<User | null>; }

// ✅ Use generic utility types
type CreateDTO = Omit<User, "id" | "createdAt">;
type UpdateDTO = Partial<Pick<User, "name" | "email">>;

// ✅ Type your API responses
const res = await axios.get<User>("/users/1");
res.data.name; // string ✓

// ✅ Augment Express Request for custom middleware properties
declare global { namespace Express { interface Request { user?: AuthUser; } } }

// ✅ Use discriminated unions for state management
type State<T> = { status: "loading" } | { status: "success"; data: T } | { status: "error"; error: Error };

// ✅ Use exhaustiveness checking in switches
default: { const _: never = value; throw new Error("Unhandled case"); }

// ✅ Prefer readonly for immutable data
const ROLES: readonly string[] = ["admin", "user"];
const config = { ... } as const;
```

### Common Gotchas

```ts
// ❌ Don't use any
function process(data: any): void {}  // loses all safety

// ✅ Use unknown instead
function process(data: unknown): void {
  if (typeof data === "string") { data.toUpperCase(); }
}

// ❌ Don't assert types you're not sure about
const user = response as User;  // might crash at runtime if shape is wrong

// ✅ Validate first, then cast
if (isUser(response)) { const user = response; }

// ❌ Don't forget query params are strings
const page = req.query.page; // string | undefined — NOT number!

// ✅ Always convert
const pageNum = parseInt(String(req.query.page ?? "1"), 10);

// ❌ Don't use optional chaining to silence errors you should handle
const name = user?.profile?.name; // if this can be undefined, handle it!

// ✅ Handle the undefined case explicitly
const name = user?.profile?.name ?? "Anonymous";
```

---

## Summary Cheat Sheet

```
tsconfig.json essentials:
  "strict": true                    — always, enables all strict checks
  "target": "ES2022"                — modern JS output
  "outDir": "./dist"                — compiled output location
  "sourceMap": true                 — debugging support
  "esModuleInterop": true           — cleaner imports for CJS modules
  "resolveJsonModule": true         — import .json files
  "skipLibCheck": true              — faster builds
  "forceConsistentCasingInFileNames": true — cross-platform safety

Declaration files (.d.ts):
  Describe types of JS modules without implementation
  Installed via @types/package-name (DefinitelyTyped)
  Write your own for untyped libraries or global augmentation
  Augment Express Request via declare global { namespace Express { interface Request {} } }
  Augment process.env via declare namespace NodeJS { interface ProcessEnv {} }

Axios with TypeScript:
  axios.get<T>(url)           → response.data is T
  axios.post<T>(url, data)    → response.data is T
  Use AxiosInstance for a shared client with interceptors
  Use isAxiosError(err) type guard in catch blocks
  Type the request/response shape with generic parameters

Express with TypeScript:
  Request<Params, ResBody, ReqBody, Query>
  Response<ResBody>
  RequestHandler — type for middleware functions
  Augment Express.Request via @types/express/index.d.ts
  asyncHandler wrapper — catches async errors and passes to next()
  Error handler needs 4 params: (err, req, res, next)
```

---

_This is the final part of the 5-part TypeScript Deep Documentation series._

```
Part 1: Basics            — what is TS, setup, annotations, inference, any, functions, void, never
Part 2: Type System       — arrays, objects, aliases, optional/readonly, intersections, unions, literals, tuples, enums
Part 3: OOP & Interfaces  — classes, access modifiers, abstract, interfaces, implements, declaration merging
Part 4: Generics          — generic functions/classes/interfaces, constraints, conditional/mapped types, all narrowing patterns
Part 5: Real World        — tsconfig, declaration files, @types, Axios typed client, Express typed server
```