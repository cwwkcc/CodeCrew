## Assignment Overview

Rather than a new project, this assignment has you write real tests for **code you've already built** — the `gradebook.js` functions from the JavaScript curriculum's `student-grade-calculator` assignment, and the DOM logic from `interactive-todo-list`. Testing your own code (instead of a toy example built just for the test) is deliberately the point: it's a very different feeling than testing something written specifically to be easy to test.

If you haven't completed those two assignments yet, do (or at least skim) them first — this one assumes that code exists.

---

## Covers

`testing/00 - Why Automated Testing & The Testing Pyramid`, `testing/01 - Unit Testing Fundamentals`, `testing/02 - Mocking & Test Doubles`, `testing/03 - Integration Testing`

---

## Part 1 — Unit Test the Gradebook (Pure Functions)

Using Vitest, write a test file `gradebook.test.js` covering the functions from `student-grade-calculator`:

```js
import { describe, test, expect } from "vitest";
import {
  average,
  letterGrade,
  classReport,
  topStudent,
  classAverage,
} from "./gradebook.js";

describe("average", () => {
  test("calculates the correct average", () => {
    expect(average([78, 82, 91, 68])).toBe(79.8);
  });

  test("handles a single grade", () => {
    expect(average([100])).toBe(100);
  });

  // what happens with an EMPTY array? Does your function crash,
  // return NaN, or handle it gracefully? Write a test that finds out —
  // then, if it reveals a problem, go back and fix gradebook.js
});

describe("letterGrade", () => {
  // test all five bands (A/B/C/D/F), AND the exact boundary values
  // (89 vs 90, 79 vs 80, etc.) — boundary values are exactly where
  // off-by-one mistakes in an if/else chain actually show up
});
```

**Requirements:**

- At least 3 tests for `average` (including the empty-array edge case)
- At least 6 tests for `letterGrade` — one per grade band, PLUS explicit boundary tests (e.g. both `89` and `90` need their own test, not just "somewhere in the 80s")
- At least 2 tests for `classReport` — including one confirming it does **not mutate** the original `students` array (this is exactly the constraint the original assignment required — now you're proving it, rather than just claiming it)
- At least 1 test for `topStudent` covering a tie (two students with the identical highest average)

If any test you write reveals an actual bug in your `gradebook.js` from the earlier assignment — that's not a problem, that's the entire point of testing. Fix the bug, and note what you found.

---

## Part 2 — Mock `Date` for the Report Date Function

Your `formatReportDate()` function depends on the current date — which makes it non-deterministic and hard to test directly without controlling time (Module 02's exact topic).

```js
import { vi, test, expect, beforeEach, afterEach } from "vitest";

beforeEach(() => {
  vi.useFakeTimers();
  vi.setSystemTime(new Date("2026-03-15T10:00:00Z"));
});

afterEach(() => {
  vi.useRealTimers();
});

test("formats today's date as DD/MM/YYYY", () => {
  expect(formatReportDate()).toBe("15/03/2026");
});
```

Write this test, confirming your function produces the exact expected string for a frozen, known date.

---

## Part 3 — Test the Todo List's Logic (Separating Logic from DOM)

The `interactive-todo-list` assignment mixed DOM manipulation directly into the todo logic — which makes it hard to unit test, since a unit test shouldn't need a real browser DOM. Before testing it, **refactor** it slightly: pull the pure logic (adding a task to an array, marking one done, deleting one) out into standalone functions that take and return plain data, separate from the DOM-updating code.

```js
// todo-logic.js — pulled out, no DOM code in here at all
export function addTask(tasks, text) {
  // returns a NEW array with the new task appended
}

export function toggleTask(tasks, taskId) {
  // returns a NEW array with that task's `done` flipped
}

export function deleteTask(tasks, taskId) {
  // returns a NEW array without that task
}
```

Then write unit tests for all three — no DOM, no browser needed, just plain functions in, plain arrays out. Your original `todo.js` can still import and use these functions to actually update the page; the tests just don't need to know that part exists.

**This refactor is itself the lesson**: code that's hard to test is very often a sign that two responsibilities (here: "manage the data" and "update the DOM") got tangled together. Separating them made the logic testable _and_ more reusable — that's not a coincidence.

---

## Constraints

- Every test must have a genuinely descriptive name — reading just the test names (without the code) should tell you roughly what's being verified.
- Use `toEqual`, not `toBe`, when asserting against arrays/objects (Module 01's most common mistake) — several of these tests compare arrays of student/task objects.
- No test should depend on another test's side effects or run order — each one should set up whatever it needs and be independently runnable.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — For the empty-array `average()` case: decide what the _correct_ behavior actually should be first (return `0`? Throw an error?), write the test for that decision, then make the code match — don't just observe whatever it currently does and write a test that agrees with it.

> 💡 **Hint 2** — Boundary testing pattern: for a `letterGrade` cutoff at 90, you want `expect(letterGrade(90)).toBe("A")` AND `expect(letterGrade(89)).toBe("B")` as two separate, explicit tests — this is exactly where a `>=` vs `>` typo would get caught, and a single "somewhere in the middle" test would miss it entirely.

> 💡 **Hint 3** — When refactoring the todo list, `todo.js` (the DOM-handling file) should end up calling `addTask`/`toggleTask`/`deleteTask` from `todo-logic.js`, then re-rendering the DOM based on the _returned_ new array — rather than manipulating `<li>` elements as its source of truth.

---

## Submission Guidelines

1. `gradebook.test.js`
2. `todo-logic.js` (the refactored pure functions) and `todo-logic.test.js`
3. A short note listing any real bugs your tests found in the original `gradebook.js` or `todo.js`, and how you fixed them

---

## Bonus Challenge 🏆

- 📊 Run `npx vitest run --coverage` and look at the report. Pick one line that's _not_ covered — is it actually worth testing, or is it dead/unreachable code? Write a sentence explaining your judgment call either way (Module 05's core lesson: coverage is a signal, not a target).
- 🔗 If you also completed the `fetch-and-render-directory` assignment, write one integration-style test (Module 03) that mocks `fetch` itself (not the whole `loadStudents` function) and confirms your code handles a `response.ok === false` case correctly.

---

## Learning Objectives

- ✓ Write real unit tests for code you already wrote, including deliberately testing edge cases and boundaries.
- ✓ Mock time (`Date`) to make a non-deterministic function testable.
- ✓ Recognize when tangled logic-and-DOM code needs to be separated to become genuinely testable — and that this separation is usually a real quality improvement, not just a testing convenience.

**Good Luck! 🚀**
