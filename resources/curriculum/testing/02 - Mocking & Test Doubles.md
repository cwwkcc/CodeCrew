A unit test should test **one** thing in isolation. But most real code calls other things — a database, an API, the system clock. Mocking is how you test a function without actually running everything it depends on.

---

## The Problem Mocking Solves

```ts
async function sendWelcomeEmail(student: Student) {
  const emailBody = buildEmailBody(student); // pure logic — easy to test
  await emailProvider.send(student.email, emailBody); // a REAL network call
  await prisma.student.update({
    // a REAL database write
    where: { id: student.id },
    data: { welcomeEmailSentAt: new Date() },
  });
}
```

Testing this function "for real" would mean actually sending an email and writing to a real database every time the test suite runs — slow, unreliable (what if the network is down?), and it would spam a real inbox on every test run. A **mock** replaces the real email provider and database with a fake stand-in that records what was called, without doing the real thing.

---

## Test Doubles — The Vocabulary

"Mock" is often used loosely for all of these, but they're technically distinct:

| Type     | What it does                                                                                      |
| -------- | ------------------------------------------------------------------------------------------------- |
| **Stub** | Returns a canned, fixed value — doesn't check how it was called                                   |
| **Mock** | Returns a value AND records calls, so you can assert it was called correctly                      |
| **Spy**  | Wraps a _real_ function, letting it still run, while also recording calls to it                   |
| **Fake** | A working but simplified implementation (e.g. an in-memory array standing in for a real database) |

In practice, day-to-day, people say "mock" for all four — this table matters mostly so you recognize the right tool for a given job, not as strict terminology to enforce.

---

## Mocking Functions with Vitest

```ts
import { describe, test, expect, vi } from "vitest";

test("sendWelcomeEmail calls the email provider with the right address", async () => {
  // vi.fn() creates a mock function — it records every call, with what arguments
  const mockSend = vi.fn().mockResolvedValue(undefined);

  await sendWelcomeEmail(
    { id: 1, email: "ashan@school.lk" },
    { send: mockSend }, // inject the mock instead of the real provider
  );

  expect(mockSend).toHaveBeenCalledWith(
    "ashan@school.lk",
    expect.stringContaining("Welcome"),
  );
  expect(mockSend).toHaveBeenCalledTimes(1);
});
```

| Mock configuration        | Behavior                                               |
| ------------------------- | ------------------------------------------------------ |
| `vi.fn()`                 | A bare mock — returns `undefined` unless configured    |
| `.mockReturnValue(x)`     | Always returns `x` (synchronous)                       |
| `.mockResolvedValue(x)`   | Always resolves to `x` (for async functions)           |
| `.mockRejectedValue(err)` | Always rejects with `err` — for testing error handling |
| `.mockImplementation(fn)` | Runs custom logic instead of a fixed value             |

---

## Mocking Entire Modules

Sometimes the dependency isn't passed in as a parameter (like above) — it's imported directly, which is common with things like a Prisma client singleton:

```ts
// __mocks__ approach, or inline with vi.mock
import { vi, test, expect } from "vitest";
import { prisma } from "../lib/prisma";
import { getStudentReport } from "./getStudentReport";

vi.mock("../lib/prisma", () => ({
  prisma: {
    student: {
      findUnique: vi.fn(),
    },
  },
}));

test("getStudentReport returns null for a missing student", async () => {
  vi.mocked(prisma.student.findUnique).mockResolvedValue(null);

  const report = await getStudentReport(999);

  expect(report).toBeNull();
});
```

`vi.mock()` replaces the entire module for the whole test file — every import of `../lib/prisma` anywhere in the code under test gets the mocked version instead of the real one, without needing to change the actual application code to accept an injected dependency.

---

## Mocking Time

Code that depends on `Date.now()` or `setTimeout` is otherwise non-deterministic — the same test could behave differently depending on exactly when it happens to run:

```ts
import { vi, test, expect, beforeEach, afterEach } from "vitest";

beforeEach(() => {
  vi.useFakeTimers();
  vi.setSystemTime(new Date("2026-03-15T10:00:00Z"));
});

afterEach(() => {
  vi.useRealTimers();
});

test("marks an assignment as overdue after the due date", () => {
  const assignment = { dueDate: new Date("2026-03-14T23:59:59Z") };
  expect(isOverdue(assignment)).toBe(true); // "now" is frozen at March 15
});
```

Without freezing time, a test like this would pass today and silently start failing (or passing for the wrong reason) on a different date — a classic source of tests that mysteriously break months after they were written and never touched again.

---

## When NOT to Mock

Mocking is a tool for isolating the unit under test, not a default to reach for everywhere:

- **Don't mock what you're actually trying to test.** Mocking the function under test itself defeats the entire point of the test.
- **Don't mock pure functions with no side effects.** If a function just transforms input to output with no I/O, call the real thing — mocking it adds complexity for zero benefit.
- **Prefer real (or fake, in-memory) implementations for integration tests.** Module 03 covers integration tests, which deliberately use a real test database instead of mocks — mocking too aggressively at that level defeats the purpose of an integration test, which is to check that the real pieces actually work together.

---

## Common Mistakes

- **Over-mocking**, to the point that a test only verifies "the mock was called," not that the actual logic does anything correct. If every dependency is mocked, ask whether the test is still checking real behavior.
- **Forgetting to reset mocks between tests.** `vi.fn()` created outside a `beforeEach` keeps accumulating call history across tests unless explicitly cleared with `vi.clearAllMocks()` in an `afterEach` — leading to `toHaveBeenCalledTimes(1)` failing because a previous test's call is still counted.
- **Mocking `Date`/time inconsistently or not at all**, for logic that depends on "now." This is exactly the kind of bug that passes in CI for months and then fails on one specific date (leap years, month-end edge cases) with no code having changed.
