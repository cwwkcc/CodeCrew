Unit tests are the base of the pyramid — the most common test you'll write, and the fastest to run. This module covers Vitest, the test runner already used in `react/23`, but applied here to plain functions and logic rather than components.

---

## Why Vitest

Vitest is a fast test runner built on Vite, with a Jest-compatible API — if you've seen Jest syntax anywhere, Vitest will look nearly identical. It's the standard choice for a Vite/Next.js-based stack like Nexus.

```bash
npm install --save-dev vitest
```

```json
// package.json
{
  "scripts": {
    "test": "vitest",
    "test:run": "vitest run"
  }
}
```

`vitest` (no args) runs in **watch mode** — it re-runs affected tests automatically every time you save a file, which is how you'll use it day-to-day while writing code. `vitest run` runs once and exits — what CI uses (Module 05).

---

## Anatomy of a Test File

```ts
// utils/formatCurrency.ts
export function formatCurrency(amount: number): string {
  return `Rs. ${amount.toFixed(2)}`;
}
```

```ts
// utils/formatCurrency.test.ts
import { describe, test, expect } from "vitest";
import { formatCurrency } from "./formatCurrency";

describe("formatCurrency", () => {
  test("formats a whole number with two decimal places", () => {
    expect(formatCurrency(100)).toBe("Rs. 100.00");
  });

  test("rounds to two decimal places", () => {
    expect(formatCurrency(99.999)).toBe("Rs. 100.00");
  });

  test("handles zero", () => {
    expect(formatCurrency(0)).toBe("Rs. 0.00");
  });

  test("handles negative numbers", () => {
    expect(formatCurrency(-50)).toBe("Rs. -50.00");
  });
});
```

| Function                       | Job                                                                            |
| ------------------------------ | ------------------------------------------------------------------------------ |
| `describe(name, fn)`           | Groups related tests together — purely organizational, doesn't affect behavior |
| `test(name, fn)` (alias: `it`) | A single test case                                                             |
| `expect(value)`                | Wraps a value so you can assert something about it                             |

Notice the four tests above aren't redundant — each checks a genuinely different behavior (basic case, rounding, zero, negative). This is the actual skill in unit testing: not writing _many_ tests, but identifying the _distinct cases_ worth checking.

---

## Common Matchers

```ts
expect(value).toBe(5); // strict equality (===) — primitives
expect(value).toEqual({ a: 1 }); // deep equality — objects/arrays
expect(value).not.toBe(5); // negation
expect(array).toContain("item"); // array includes a value
expect(array).toHaveLength(3); // array/string length
expect(value).toBeNull();
expect(value).toBeUndefined();
expect(value).toBeTruthy();
expect(value).toBeFalsy();
expect(fn).toThrow(); // function throws any error
expect(fn).toThrow("specific message"); // throws with a specific message
expect(number).toBeGreaterThan(10);
expect(number).toBeCloseTo(0.3, 2); // for floating-point comparisons
```

**`toBe` vs `toEqual` is the single most common beginner mistake.** `toBe` uses `===`, which for objects and arrays checks _reference_ equality, not content:

```ts
// WRONG matcher for objects — this FAILS even though the content is identical,
// because {} === {} is false in JavaScript (different references in memory)
expect({ name: "Ashan" }).toBe({ name: "Ashan" });

// CORRECT — toEqual checks the VALUES inside, recursively
expect({ name: "Ashan" }).toEqual({ name: "Ashan" });
```

---

## Setup and Teardown

For tests that need shared preparation (a fresh object, a reset counter) before each test runs:

```ts
import { describe, test, expect, beforeEach, afterEach } from "vitest";

describe("ShoppingCart", () => {
  let cart: ShoppingCart;

  beforeEach(() => {
    cart = new ShoppingCart(); // fresh cart before EVERY test — no shared state leaking between tests
  });

  afterEach(() => {
    cart.clear(); // cleanup, if needed
  });

  test("starts empty", () => {
    expect(cart.items).toHaveLength(0);
  });

  test("adding an item increases the count", () => {
    cart.add({ id: 1, price: 500 });
    expect(cart.items).toHaveLength(1);
  });
});
```

`beforeEach` runs before every single `test` in its `describe` block — this is what prevents one test's leftover state from silently affecting the next test's result, which is a common and confusing source of "this test fails only when run after that other one" bugs.

---

## Testing Async Code

```ts
// utils/fetchStudent.ts
export async function fetchStudent(id: number) {
  const res = await fetch(`/api/students/${id}`);
  if (!res.ok) throw new Error("Student not found");
  return res.json();
}
```

```ts
test("throws when the student is not found", async () => {
  // async test functions, and await the actual assertion for rejected promises
  await expect(fetchStudent(9999)).rejects.toThrow("Student not found");
});
```

Forgetting `await` on an assertion against a promise is a common mistake — without it, the test function returns before the assertion actually runs, and a failing assertion can silently pass because Vitest never waited around to see it fail.

---

## What Makes a Good Unit Test

- **Tests one thing.** If a test's name needs "and" in it ("formats currency and handles negative numbers"), it's probably two tests.
- **Is deterministic.** The same input always produces the same result — no reliance on the current date/time, random numbers, or network calls without controlling them (Module 02 covers mocking these away).
- **Fails with a clear message.** A good test name plus a specific matcher (`toBe(5)` rather than `toBeTruthy()`) means a failure tells you exactly what broke, without needing to open a debugger.

---

## Common Mistakes

- **Using `toBe` for objects/arrays instead of `toEqual`.** Covered above — this is the single most common Vitest mistake for anyone coming from testing only primitive values.
- **Forgetting `await` on async assertions**, especially `.rejects` / `.resolves`. The test can report a false pass if the promise hasn't settled before the test function returns.
- **Shared mutable state between tests with no `beforeEach` reset.** A test suite that only passes when run in a specific order is a sign that state is leaking between tests.
- **One giant test covering many unrelated behaviors.** When it fails, you have to read through everything it does to figure out which part actually broke — smaller, focused tests fail with much more specific, useful messages.
