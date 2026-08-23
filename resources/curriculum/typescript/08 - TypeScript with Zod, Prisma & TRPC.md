> The Project Nexus stack is built entirely on TypeScript type safety flowing end-to-end — from the database schema (Prisma) through validation (Zod) to the API layer (tRPC) and into the React frontend. This file shows how these three libraries fit together and how TypeScript makes the entire stack type-safe without duplication.

---

## Table of Contents

1. [The Type Safety Problem This Stack Solves](#1-the-type-safety-problem-this-stack-solves)
2. [Zod — Runtime Validation with Static Types](#2-zod--runtime-validation-with-static-types)
3. [Zod Schema Patterns](#3-zod-schema-patterns)
4. [Prisma — Database Types](#4-prisma--database-types)
5. [Prisma Type Utilities](#5-prisma-type-utilities)
6. [tRPC — End-to-End Type Safety](#6-trpc--end-to-end-type-safety)
7. [Connecting All Three](#7-connecting-all-three)
8. [Type-Safe Environment Variables](#8-type-safe-environment-variables)

---

## 1. The Type Safety Problem This Stack Solves

In a traditional API, types break at every boundary:

```
Database schema (SQL)         → no types
    ↓ manual mapping
API response (JSON)           → any
    ↓ no validation
Server code                   → type assertions
    ↓ fetch()
Client code                   → any again
```

With Prisma + Zod + tRPC:

```
Database schema (schema.prisma)    → Prisma generates TypeScript types
    ↓ automatic
Server service code                → fully typed (Student, Course, etc.)
    ↓ Zod validates input
tRPC procedure                     → input typed, output typed
    ↓ automatic
React component                    → useQuery returns typed data, no casting
```

One source of truth. No type drift. Runtime errors at boundaries are caught at compile time.

---

## 2. Zod — Runtime Validation with Static Types

TypeScript types exist only at compile time — they're erased when code runs. External data (HTTP bodies, query params, environment variables) has no guarantee of matching your types at runtime.

Zod bridges this gap: you define a schema once, get both the runtime validator and the TypeScript type from it.

```typescript
import { z } from "zod";

// Define the schema
const createStudentSchema = z.object({
  name: z.string().min(2, "Name must be at least 2 characters"),
  email: z.string().email("Invalid email format"),
  grade: z.number().int().min(1).max(13),
  dateOfBirth: z.string().datetime(),
  parentEmail: z.string().email().optional(),
  subjects: z.array(z.string()).min(1, "At least one subject required"),
});

// TypeScript type — inferred automatically, zero duplication
type CreateStudentDTO = z.infer<typeof createStudentSchema>;
// {
//   name:        string;
//   email:       string;
//   grade:       number;
//   dateOfBirth: string;
//   parentEmail?: string | undefined;
//   subjects:    string[];
// }

// Runtime validation
const result = createStudentSchema.safeParse(requestBody);

if (!result.success) {
  // result.error.issues contains detailed error messages
  const errors = result.error.flatten();
  // errors.fieldErrors: { name?: string[], email?: string[], ... }
  return res.status(422).json({ errors: errors.fieldErrors });
}

// result.data is typed as CreateStudentDTO — safe to use
const student = await createStudent(result.data);
```

### `parse` vs `safeParse`

```typescript
// parse — throws ZodError if invalid
const data = createStudentSchema.parse(requestBody);
// data is CreateStudentDTO — or an exception was thrown

// safeParse — returns { success, data } | { success: false, error }
const result = createStudentSchema.safeParse(requestBody);
if (result.success) {
  result.data; // CreateStudentDTO
} else {
  result.error; // ZodError with detailed issues
}

// parseAsync / safeParseAsync — for schemas with async transforms/refinements
const result = await createStudentSchema.safeParseAsync(requestBody);
```

---

## 3. Zod Schema Patterns

### String validations

```typescript
z.string();
z.string().min(2);
z.string().max(100);
z.string().length(10);
z.string().email();
z.string().url();
z.string().uuid();
z.string().cuid();
z.string().regex(/^[A-Z]{3}-\d{4}$/, "Must match format ABC-1234");
z.string().startsWith("https://");
z.string().endsWith(".lk");
z.string().includes("@");
z.string().trim(); // transform: remove whitespace before validation
z.string().toLowerCase(); // transform: convert to lowercase
z.string().datetime(); // ISO 8601 datetime string
z.string().ip();
```

### Number validations

```typescript
z.number();
z.number().int(); // must be integer (no decimals)
z.number().positive(); // > 0
z.number().nonnegative(); // >= 0
z.number().negative(); // < 0
z.number().min(0);
z.number().max(100);
z.number().multipleOf(5);
z.number().finite();
z.number().safe(); // within Number.MAX_SAFE_INTEGER
```

### Optional, nullable, default

```typescript
z.string().optional(); // string | undefined
z.string().nullable(); // string | null
z.string().nullish(); // string | null | undefined
z.string().default("anonymous"); // uses default if undefined
z.string().optional().default(""); // always has a value

// Coerce — convert incoming type before validating
z.coerce.number(); // "42" → 42 (useful for query params which are always strings)
z.coerce.boolean(); // "true" → true
z.coerce.date(); // "2026-01-15" → Date object
```

### Object schemas

```typescript
const schema = z
  .object({
    id: z.string().uuid(),
    name: z.string(),
  })
  .strict() // reject unknown keys (throws on extra fields)
  .passthrough() // OR: allow unknown keys through
  .strip(); // OR: silently remove unknown keys (default)

// Nested objects
const addressSchema = z.object({
  street: z.string(),
  city: z.string(),
  district: z.string(),
  province: z.enum(["Western", "Central", "Southern", "Northern"]),
});

const studentSchema = z.object({
  name: z.string(),
  address: addressSchema, // nested — fully typed
});

// Partial and required
const updateSchema = studentSchema.partial(); // all fields optional
const strictSchema = studentSchema.required(); // all fields required

// Pick and omit
const publicSchema = studentSchema.omit({ address: true });
const miniSchema = studentSchema.pick({ name: true });

// Extend
const extendedSchema = studentSchema.extend({
  phone: z.string().optional(),
});

// Merge two schemas
const mergedSchema = schema1.merge(schema2);
```

### Discriminated unions and refinements

```typescript
// Discriminated union — Zod validates based on the discriminant
const paymentSchema = z.discriminatedUnion("method", [
  z.object({ method: z.literal("card"), cardToken: z.string() }),
  z.object({
    method: z.literal("bank"),
    accountNumber: z.string(),
    routingNumber: z.string(),
  }),
  z.object({ method: z.literal("paypal"), email: z.string().email() }),
]);

// Cross-field validation with .refine()
const passwordSchema = z
  .object({
    password: z.string().min(8),
    confirmPassword: z.string(),
  })
  .refine((data) => data.password === data.confirmPassword, {
    message: "Passwords don't match",
    path: ["confirmPassword"],
  });

// superRefine — for complex multi-error validation
const dateRangeSchema = z
  .object({
    startDate: z.string().datetime(),
    endDate: z.string().datetime(),
  })
  .superRefine((data, ctx) => {
    if (new Date(data.endDate) <= new Date(data.startDate)) {
      ctx.addIssue({
        code: z.ZodIssueCode.custom,
        message: "End date must be after start date",
        path: ["endDate"],
      });
    }
  });
```

---

## 4. Prisma — Database Types

Prisma generates TypeScript types directly from your schema. Every model, every relation, every operation is fully typed.

```prisma
// schema.prisma
model Student {
  id          String    @id @default(cuid())
  name        String
  email       String    @unique
  grade       Int
  createdAt   DateTime  @default(now())
  updatedAt   DateTime  @updatedAt
  enrollments Enrollment[]
  scores      Score[]
}

model Course {
  id          String    @id @default(cuid())
  title       String
  teacherId   String
  teacher     Teacher   @relation(fields: [teacherId], references: [id])
  enrollments Enrollment[]
}

model Enrollment {
  id        String  @id @default(cuid())
  studentId String
  courseId  String
  student   Student @relation(fields: [studentId], references: [id])
  course    Course  @relation(fields: [courseId], references: [id])
}
```

```typescript
// Prisma generates these types automatically:
import { PrismaClient, Student, Course, Enrollment } from "@prisma/client";

const prisma = new PrismaClient();

// All operations are fully typed — TypeScript knows what fields exist
const student = await prisma.student.findUnique({
  where: { id: "s-123" },
});
// student: Student | null

// With relations — include adds the relation to the type
const studentWithEnrollments = await prisma.student.findUnique({
  where: { id: "s-123" },
  include: { enrollments: { include: { course: true } } },
});
// studentWithEnrollments: (Student & { enrollments: (Enrollment & { course: Course })[] }) | null

// TypeScript catches errors:
await prisma.student.findUnique({ where: { nonExistentField: "x" } });
// TS Error: Object literal may only specify known properties
```

---

## 5. Prisma Type Utilities

Prisma generates utility types for every model and operation.

```typescript
import type { Prisma, Student, Course, Enrollment } from "@prisma/client";

// Get the type for a specific select/include combination
type StudentWithCourses = Prisma.StudentGetPayload<{
  include: {
    enrollments: { include: { course: true } };
  };
}>;
// Exactly typed — has student fields + enrollments array + course on each enrollment

// Get the type for a create input
type CreateStudentInput = Prisma.StudentCreateInput;
// { name: string; email: string; grade: number; ... }

// Get the type for a where clause
type StudentWhereInput = Prisma.StudentWhereInput;
// { id?: string; name?: StringFilter; email?: string; ... }

// Get the type for an update input
type UpdateStudentInput = Prisma.StudentUpdateInput;
// { name?: string | StringFieldUpdateOperationsInput; email?: string | ...; ... }

// Building typed query helpers
function buildStudentSelect<T extends Prisma.StudentSelect>(select: T) {
  return select;
}

const publicFields = buildStudentSelect({
  id: true,
  name: true,
  email: true,
  grade: true,
  // TS Error if you include a non-existent field
});

type PublicStudent = Prisma.StudentGetPayload<{ select: typeof publicFields }>;
// { id: string; name: string; email: string; grade: number }
```

### Repository pattern with Prisma types

```typescript
interface StudentRepository {
  findById(id: string): Promise<Student | null>;
  findByEmail(email: string): Promise<Student | null>;
  findAll(params: {
    where?: Prisma.StudentWhereInput;
    orderBy?: Prisma.StudentOrderByWithRelationInput;
    skip?: number;
    take?: number;
  }): Promise<Student[]>;
  create(data: Prisma.StudentCreateInput): Promise<Student>;
  update(id: string, data: Prisma.StudentUpdateInput): Promise<Student>;
  delete(id: string): Promise<Student>;
  count(where?: Prisma.StudentWhereInput): Promise<number>;
}

class PrismaStudentRepository implements StudentRepository {
  constructor(private readonly prisma: PrismaClient) {}

  findById(id: string) {
    return this.prisma.student.findUnique({ where: { id } });
  }

  findAll({ where, orderBy, skip, take } = {}) {
    return this.prisma.student.findMany({ where, orderBy, skip, take });
  }

  create(data: Prisma.StudentCreateInput) {
    return this.prisma.student.create({ data });
  }

  update(id: string, data: Prisma.StudentUpdateInput) {
    return this.prisma.student.update({ where: { id }, data });
  }

  delete(id: string) {
    return this.prisma.student.delete({ where: { id } });
  }

  count(where?: Prisma.StudentWhereInput) {
    return this.prisma.student.count({ where });
  }
}
```

---

## 6. tRPC — End-to-End Type Safety

tRPC exposes TypeScript types from your server procedures directly to your client — no code generation, no OpenAPI specs, no manual type duplication.

```typescript
// server/routers/student.ts
import { z } from "zod";
import { router, publicProcedure, protectedProcedure } from "../trpc";
import { TRPCError } from "@trpc/server";

export const studentRouter = router({
  // Query — read data
  list: protectedProcedure
    .input(
      z.object({
        grade: z.number().int().min(1).max(13).optional(),
        query: z.string().optional(),
        page: z.number().int().min(1).default(1),
        limit: z.number().int().min(1).max(100).default(20),
      }),
    )
    .query(async ({ ctx, input }) => {
      // ctx.user is typed (from protectedProcedure)
      // input is { grade?: number; query?: string; page: number; limit: number }
      const { grade, query, page, limit } = input;

      const [students, total] = await Promise.all([
        ctx.prisma.student.findMany({
          where: {
            grade,
            name: query ? { contains: query, mode: "insensitive" } : undefined,
          },
          skip: (page - 1) * limit,
          take: limit,
        }),
        ctx.prisma.student.count({
          where: { grade, name: query ? { contains: query } : undefined },
        }),
      ]);

      return {
        students,
        total,
        page,
        limit,
        totalPages: Math.ceil(total / limit),
      };
    }),

  // Mutation — modify data
  create: protectedProcedure
    .input(createStudentSchema) // reuse your Zod schema
    .mutation(async ({ ctx, input }) => {
      // Only admins can create students
      if (!ctx.user.roles.includes("admin")) {
        throw new TRPCError({
          code: "FORBIDDEN",
          message: "Admin access required",
        });
      }

      const existing = await ctx.prisma.student.findUnique({
        where: { email: input.email },
      });
      if (existing) {
        throw new TRPCError({
          code: "CONFLICT",
          message: "Email already registered",
        });
      }

      return ctx.prisma.student.create({ data: input });
      // Return type: Student — inferred automatically
    }),

  delete: protectedProcedure
    .input(z.object({ id: z.string().cuid() }))
    .mutation(async ({ ctx, input }) => {
      const student = await ctx.prisma.student.findUnique({
        where: { id: input.id },
      });
      if (!student) {
        throw new TRPCError({
          code: "NOT_FOUND",
          message: `Student ${input.id} not found`,
        });
      }

      return ctx.prisma.student.delete({ where: { id: input.id } });
    }),
});
```

```typescript
// client/components/StudentList.tsx — ZERO type duplication
import { trpc } from "../utils/trpc";

function StudentList() {
  // input is typed: { grade?: number; query?: string; page: number; limit: number }
  // data is typed: { students: Student[]; total: number; page: number; ... }
  const { data, isLoading } = trpc.student.list.useQuery({
    grade: 11,
    page:  1,
  });

  if (isLoading) return <Spinner />;

  return (
    <div>
      {data?.students.map(student => (
        // student is typed as Student — IDE autocomplete works
        <div key={student.id}>{student.name} — Grade {student.grade}</div>
      ))}
    </div>
  );
}
```

---

## 7. Connecting All Three

The full data flow for creating a student:

```typescript
// 1. Zod schema — single source of truth for shape + validation
const createStudentSchema = z.object({
  name:        z.string().min(2).max(100).trim(),
  email:       z.string().email().toLowerCase(),
  grade:       z.coerce.number().int().min(1).max(13),
  dateOfBirth: z.string().datetime().optional(),
  parentEmail: z.string().email().optional(),
});

type CreateStudentDTO = z.infer<typeof createStudentSchema>;

// 2. tRPC procedure — uses the schema for input validation
const studentRouter = router({
  create: protectedProcedure
    .input(createStudentSchema)
    .mutation(async ({ ctx, input }) => {
      // input is CreateStudentDTO — typed and validated
      return ctx.prisma.student.create({ data: input });
      // Return type: Student — from Prisma
    }),
});

// 3. Client — React component with full type safety
function CreateStudentForm() {
  const utils = trpc.useUtils();

  const create = trpc.student.create.useMutation({
    onSuccess: () => utils.student.list.invalidate(),
  });

  const form = useForm<CreateStudentDTO>({
    resolver: zodResolver(createStudentSchema),  // same schema validates the form
  });

  function onSubmit(data: CreateStudentDTO) {
    create.mutate(data);  // data is typed, tRPC validates again on server
  }

  return (
    <Form {...form}>
      <form onSubmit={form.handleSubmit(onSubmit)}>
        <FormField
          control={form.control}
          name="email"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Email</FormLabel>
              <FormControl><Input type="email" {...field} /></FormControl>
              <FormMessage />
            </FormItem>
          )}
        />
        <Button type="submit" disabled={create.isPending}>
          {create.isPending ? "Creating..." : "Create Student"}
        </Button>
      </form>
    </Form>
  );
}
```

The schema is defined once. Types flow through: form validation → tRPC input → Prisma → tRPC output → React component. Change the schema and TypeScript surfaces every place that needs to update.

---

## 8. Type-Safe Environment Variables

Environment variables are strings at runtime but need to be typed. Zod is the cleanest solution.

```typescript
// src/env.ts — validate and type all env vars at startup
import { z } from "zod";

const envSchema = z.object({
  // Server
  NODE_ENV: z.enum(["development", "staging", "production", "test"]),
  PORT: z.coerce.number().default(3000),

  // Database
  DATABASE_URL: z.string().url(),

  // Auth
  JWT_SECRET: z.string().min(32, "JWT secret must be at least 32 chars"),
  JWT_EXPIRES_IN: z.string().default("15m"),
  REFRESH_TOKEN_SECRET: z.string().min(32),
  REFRESH_TOKEN_EXPIRES: z.string().default("7d"),

  // External services (optional)
  RESEND_API_KEY: z.string().optional(),
  CLOUDFLARE_R2_URL: z.string().url().optional(),

  // App
  APP_URL: z.string().url(),
  CORS_ORIGINS: z.string().transform((s) => s.split(",").map((u) => u.trim())),
});

// Throws at startup if any required env var is missing or invalid
// Much better than a runtime crash 3 hours after deployment
export const env = envSchema.parse(process.env);

// Type-safe access everywhere
env.DATABASE_URL; // string
env.PORT; // number (coerced from string)
env.NODE_ENV; // "development" | "staging" | "production" | "test"
env.CORS_ORIGINS; // string[] (transformed from comma-separated)
```

```typescript
// Optional: extend process.env types for autocomplete
declare global {
  namespace NodeJS {
    interface ProcessEnv extends z.infer<typeof envSchema> {}
  }
}
// Now process.env.DATABASE_URL is typed as string (not string | undefined)
```

---

## Summary

```
The stack:
  Prisma → generates types from DB schema
  Zod    → validates external data, infers types from schema
  tRPC   → shares server types with client automatically

Zod core pattern:
  const schema = z.object({ field: z.string().min(2) });
  type Type = z.infer<typeof schema>;
  const result = schema.safeParse(data);
  if (result.success) { result.data } else { result.error }

Key Zod features:
  .optional() / .nullable() / .nullish()
  .default(value) — use if undefined
  z.coerce.number() — convert string → number (query params)
  .refine() — cross-field validation
  .discriminatedUnion("type", [...]) — typed union

Prisma types:
  Student, Course, etc. — model types from schema
  Prisma.StudentGetPayload<{ include: ... }> — with relations
  Prisma.StudentCreateInput — create operation input
  Prisma.StudentWhereInput — where clause type

tRPC:
  .input(zodSchema) — validates and types the input
  .query(async ({ ctx, input }) => {}) — typed handler
  .mutation(async ({ ctx, input }) => {}) — typed handler
  TRPCError({ code: "NOT_FOUND" }) — typed error responses
  Client: trpc.router.procedure.useQuery(input) — types flow through

Single source of truth:
  Define Zod schema once
  Reuse: form validation (zodResolver), tRPC input, type inference
  Change schema → TypeScript surfaces all affected code

Environment variables:
  z.coerce.number() for numeric vars (PORT)
  z.enum([...]) for NODE_ENV
  .transform() to parse lists ("a,b,c" → ["a","b","c"])
  envSchema.parse(process.env) — throws at startup if invalid
```
