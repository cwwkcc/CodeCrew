Unit tests check one function in isolation, with everything else mocked away. **Integration tests** check that several real pieces — a tRPC router, the database, an authentication check — actually work correctly _together_. This is the middle layer of the pyramid, and for a full-stack project like Nexus, often the layer that catches the most real bugs per test written.

---

## Why Integration Tests Matter Here Specifically

A unit test for a tRPC procedure with the database mocked can pass while the actual Prisma query is subtly wrong (a typo'd column name, a broken join, a query that returns the wrong shape) — because the mock was configured to return exactly what the test expected, regardless of whether the real query would ever produce that. An integration test runs the real procedure against a real (test) database, catching exactly this class of bug.

```
Unit test:         "does this function transform X into Y, given these inputs?"
Integration test:  "does calling this API endpoint actually read/write the
                     correct rows in a real database?"
```

---

## Setting Up a Test Database

Integration tests need a real database to run against — never the actual development or production one, since tests create, modify, and delete data:

```bash
# .env.test — a completely separate database from development
DATABASE_URL="postgresql://user:password@localhost:5432/nexus_test"
```

```ts
// vitest.setup.ts — runs before the test suite
import { execSync } from "child_process";

// Apply all migrations to the test database before tests run
execSync("npx prisma migrate deploy", {
  env: { ...process.env, DATABASE_URL: process.env.DATABASE_URL_TEST },
});
```

```ts
// vitest.config.ts
export default defineConfig({
  test: {
    setupFiles: ["./vitest.setup.ts"],
    environment: "node",
  },
});
```

---

## Resetting State Between Tests

The single most important rule for integration tests: **one test's leftover data must never affect another test.**

```ts
import { beforeEach, afterAll } from "vitest";
import { prisma } from "../lib/prisma";

beforeEach(async () => {
  // Clear tables in an order that respects foreign key constraints
  // (Module 03 of the database curriculum) — children before parents
  await prisma.enrollment.deleteMany();
  await prisma.student.deleteMany();
  await prisma.department.deleteMany();
});

afterAll(async () => {
  await prisma.$disconnect();
});
```

An alternative to manual cleanup is wrapping each test in a database transaction that's rolled back at the end — faster for large suites, but more setup than most small projects need. For a project the size of Nexus, `deleteMany()` in `beforeEach` is simple, correct, and fast enough.

---

## Testing a tRPC Procedure End-to-End (Server Side)

```ts
// server/routers/student.test.ts
import { describe, test, expect, beforeEach } from "vitest";
import { appRouter } from "./_app";
import { prisma } from "../lib/prisma";

describe("student.create", () => {
  beforeEach(async () => {
    await prisma.student.deleteMany();
  });

  test("creates a student and persists it to the database", async () => {
    const caller = appRouter.createCaller({
      prisma,
      session: mockAdminSession,
    });

    const result = await caller.student.create({
      fullName: "Nadeesha Silva",
      grade: 10,
    });

    expect(result.fullName).toBe("Nadeesha Silva");

    // The real check — did it actually land in the database?
    const inDb = await prisma.student.findUnique({ where: { id: result.id } });
    expect(inDb).not.toBeNull();
    expect(inDb?.grade).toBe(10);
  });

  test("rejects creation without an authenticated session", async () => {
    const caller = appRouter.createCaller({ prisma, session: null });

    await expect(
      caller.student.create({ fullName: "Test", grade: 10 }),
    ).rejects.toThrow(/unauthorized/i);
  });
});
```

This test uses the **real** Prisma client against the test database, and the **real** tRPC router logic — nothing about the actual create-a-student flow is mocked. The only thing that's test-specific is which database it's pointed at.

---

## What to Cover with Integration Tests (and What to Leave to Unit Tests)

- **Authorization/permission checks** — does an unauthenticated or wrong-role request actually get rejected by the real middleware, not just by a mocked check?
- **Data actually persisting correctly**, including relations — did the join table row get created, did a foreign key constraint correctly reject bad data?
- **Multi-step flows** — create a student, then enroll them, then check the enrollment count updated (Module 06 of the database curriculum's transaction example) — this is exactly the kind of interaction a unit test with everything mocked can't meaningfully verify.

Leave pure logic (validation rules, formatting, calculations) to fast unit tests — running a database round-trip to test that `formatCurrency(100) === "Rs. 100.00"` is unnecessary weight for something that has no actual dependency on the database.

---

## Common Mistakes

- **Running integration tests against the development database.** A test suite that deletes rows is not something you want anywhere near real or even realistic development data — always a dedicated test database.
- **Not resetting data between tests**, leading to tests that pass individually but fail when run together (or in a different order) because one test's leftover row satisfies — or breaks — another test's assumptions.
- **Mocking so much of the stack that the integration test isn't actually integrating anything.** If the database, the auth check, and the router logic are all mocked, it's really just a unit test wearing an integration test's file name.
- **Slow test suites from unnecessary integration tests.** If a unit test (with things mocked) can verify the same logic in milliseconds instead of the integration test's real database round-trip, prefer the unit test and reserve integration tests for what actually needs real wiring.
