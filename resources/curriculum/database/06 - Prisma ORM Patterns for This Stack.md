Everything in Modules 00–05 was pure SQL and relational theory — deliberately, because an ORM makes far more sense once you know what it's generating underneath. This module is the bridge: how Prisma's specific patterns map to the SQL concepts already covered, and the handful of Prisma-specific gotchas that catch people who learned the ORM without the SQL underneath it.

---

## What an ORM Actually Is

An **ORM** (Object-Relational Mapper) translates between rows/columns (relational) and objects (what your application code actually works with). Prisma specifically is a **type-safe query builder** — it doesn't just translate, it generates TypeScript types directly from your schema, so `prisma.student.findMany()` is fully typed without writing a single interface by hand.

```prisma
// schema.prisma
model Student {
  id       Int    @id @default(autoincrement())
  fullName String
  grade    Int
}
```

```ts
// Prisma generates this type automatically — you never hand-write it
type Student = {
  id: number;
  fullName: string;
  grade: number;
};

// And every query is checked against it at compile time
const students = await prisma.student.findMany();
students[0].fullName; // ✓ typed as string
students[0].fulName; // ✗ TypeScript error — caught before it ever runs
```

---

## Mapping Prisma Calls to SQL

Seeing the actual generated SQL is the fastest way to stop treating Prisma as a black box:

| Prisma                                        | Generates roughly                              |
| --------------------------------------------- | ---------------------------------------------- |
| `findMany()`                                  | `SELECT * FROM "Student"`                      |
| `findUnique({ where: { id: 4 } })`            | `SELECT * FROM "Student" WHERE id = 4 LIMIT 1` |
| `findMany({ where: { grade: 10 } })`          | `SELECT * FROM "Student" WHERE grade = 10`     |
| `findMany({ include: { department: true } })` | A `LEFT JOIN` against `Department` (Module 03) |
| `findMany({ orderBy: { fullName: 'asc' } })`  | `... ORDER BY "fullName" ASC`                  |
| `create({ data: {...} })`                     | `INSERT INTO "Student" (...) VALUES (...)`     |
| `update({ where, data })`                     | `UPDATE "Student" SET ... WHERE id = ...`      |
| `delete({ where })`                           | `DELETE FROM "Student" WHERE id = ...`         |

You can see the real generated SQL directly during development:

```ts
// prisma/client.ts
const prisma = new PrismaClient({
  log: ["query"], // prints every generated SQL statement to the console
});
```

---

## `select` vs `include`

Both narrow what a query returns, but for different purposes — mixing them up, or using neither, is one of the most common Prisma performance mistakes:

```ts
// No select/include — fetches EVERY column on Student, and no relations.
// Fine for a quick script; wasteful in a real API route serving a list view.
const students = await prisma.student.findMany();

// select — return ONLY the specific fields you actually need
const students = await prisma.student.findMany({
  select: { id: true, fullName: true },
});

// include — fetch all of Student's own columns, PLUS a related table
// (this is the JOIN from Module 03)
const students = await prisma.student.findMany({
  include: { department: true },
});

// select can also pull in specific fields from a relation, combining both
const students = await prisma.student.findMany({
  select: {
    fullName: true,
    department: { select: { name: true } },
  },
});
```

Rule of thumb: default to `select` with exactly the fields the page/response actually needs. `findMany()` with no `select` on a table with many columns (or large `TEXT`/`JSONB` fields) fetches data over the wire that gets thrown away immediately — invisible in development with a handful of rows, measurable in production.

---

## Transactions — Multiple Writes That Must All Succeed Together

Some operations need several writes to happen as a single, all-or-nothing unit — e.g. enrolling a student in a subject _and_ incrementing that subject's enrollment count. If the second write fails after the first succeeds, the data is now inconsistent.

```ts
// WRONG: if the second call throws, the first one has already
// committed — the database is left in a half-finished state
await prisma.enrollment.create({ data: { studentId, subjectId } });
await prisma.subject.update({
  where: { id: subjectId },
  data: { enrollmentCount: { increment: 1 } },
});

// CORRECT: $transaction ensures both succeed or both roll back
await prisma.$transaction([
  prisma.enrollment.create({ data: { studentId, subjectId } }),
  prisma.subject.update({
    where: { id: subjectId },
    data: { enrollmentCount: { increment: 1 } },
  }),
]);
```

Underneath, this is SQL's `BEGIN`, the two statements, then `COMMIT` (or `ROLLBACK` if anything fails) — Prisma's `$transaction` is a thin wrapper around exactly that.

---

## Raw SQL When You Need It

Prisma covers the vast majority of real queries, but some things — complex reporting queries, database-specific functions, performance-critical paths — are genuinely easier or faster written as raw SQL:

```ts
// $queryRaw — for SELECTs, returns typed results
const result = await prisma.$queryRaw`
  SELECT grade, COUNT(*) as student_count
  FROM "Student"
  GROUP BY grade
  ORDER BY grade
`;

// $executeRaw — for INSERT/UPDATE/DELETE, returns affected row count
await prisma.$executeRaw`UPDATE "Student" SET grade = grade + 1 WHERE grade < 12`;
```

This is exactly why Modules 00–05 matter even in a Prisma-first project: the moment a query needs something the query builder doesn't expose cleanly, you're writing the SQL directly, with no ORM abstraction to lean on.

---

## Common Mistakes

- **Treating Prisma as a replacement for understanding the database, rather than a layer on top of it.** The N+1 problem (Module 04) happens just as easily with Prisma as with raw SQL — the ORM makes it _easier to write by accident_, not immune to it.
- **`findMany()` with no `select` on a list endpoint.** Fetching every column (including large text fields, timestamps you don't display, etc.) for a table you're about to render as a compact list is wasted bandwidth that scales badly.
- **Multiple related writes with no `$transaction`.** If any operation depends on another succeeding for the data to stay consistent, wrap them together — otherwise a crash or error between the two calls leaves the database in a state that's hard to detect and harder to fix after the fact.
- **Reaching for `$queryRaw` before trying the query builder.** Raw SQL loses type safety and Prisma's protections against SQL injection unless parameterized carefully (`$queryRaw` with template literals handles this correctly — string concatenation into a raw query does not). Use it when the query builder genuinely can't express what's needed, not as a default habit.
