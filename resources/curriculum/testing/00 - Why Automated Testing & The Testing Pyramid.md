Every project so far has been tested by hand — run the app, click around, check it looks right. That works until the project is big enough that "click around and check" can't realistically cover everything that could break. This module covers why automated tests exist and how to think about what kind of test to write for a given piece of code.

---

## What a Test Actually Is

An automated test is a small program that runs another piece of your program and checks the result is what you expect — no browser, no manual clicking, no human required:

```ts
function add(a: number, b: number): number {
  return a + b;
}

// This is a test — it calls the function and checks the result
test("add sums two numbers", () => {
  expect(add(2, 3)).toBe(5);
});
```

Run it, and either it passes silently or it fails with a clear message telling you exactly what was expected versus what actually happened. Do this for the parts of your code that matter, and you get something manual testing can't give you: the same check, run automatically, every single time the code changes — including changes to code you didn't even touch, in a part of the codebase you forgot could be affected.

---

## Why Bother — What Manual Testing Doesn't Catch

- **Regressions.** You fix bug A. Two weeks later, an unrelated change reintroduces bug A because nobody was manually re-checking it. A test for bug A catches this the moment it happens, not weeks later when a user reports it again.
- **Refactoring confidence.** Rewriting how a function works internally is safe if a test suite confirms the _output_ hasn't changed — without tests, every refactor is a leap of faith.
- **Documentation that can't go stale.** A test literally shows a real, working example of how a function is meant to be called — unlike a comment, it fails loudly the moment it stops being true.
- **Coverage of edge cases you'd forget to check by hand.** Empty input, a negative number, a network failure mid-request — these are exactly the cases manual "click around" testing tends to skip, and exactly the cases that break in production.

None of this means test everything, always. It means: know what kind of confidence each type of test buys you, and spend that effort where a bug would actually hurt.

---

## The Testing Pyramid

Different tests check different amounts of your system, at very different costs to write and run:

```
        /\
       /  \      E2E Tests (few)
      /----\     Slow, expensive, test the whole real system
     /      \
    /--------\   Integration Tests (some)
   /          \  Medium speed, test several pieces working together
  /------------\
 /              \ Unit Tests (many)
/________________\ Fast, cheap, test one function/component in isolation
```

| Level                | Tests                                                                        | Speed              | What it catches                                                                                                                           |
| -------------------- | ---------------------------------------------------------------------------- | ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------- |
| **Unit**             | One function or component, in isolation                                      | Milliseconds       | Logic errors in a single piece                                                                                                            |
| **Integration**      | Several pieces working together (a tRPC router hitting a real test database) | Seconds            | Wiring problems between pieces that each pass their own unit tests                                                                        |
| **End-to-end (E2E)** | A real browser driving the real, fully running app                           | Seconds to minutes | Problems that only show up when everything is actually connected — including the parts no unit test touches, like actual browser behavior |

The shape matters: **many** fast unit tests, a **moderate** number of integration tests, and **few**, carefully chosen E2E tests for the critical paths (login, checkout, submitting a form) that would be genuinely bad to have broken in production. A project with the pyramid inverted — mostly E2E tests, few unit tests — ends up with a test suite that's slow to run and painful to maintain, because a single small bug can break dozens of end-to-end flows at once instead of one focused unit test.

Modules 01–02 cover unit testing and mocking; Module 03 covers integration testing; Module 04 covers E2E. React's own component-testing patterns (React Testing Library, user interaction testing) are covered in `react/23 - Testing React Components` rather than repeated here — this track covers the parts of testing that apply regardless of framework.

---

## What's Worth Testing (and What Isn't)

- **Do test:** business logic, data transformations, validation rules, anything with edge cases (empty arrays, zero, negative numbers, malformed input), anything that's broken before and could break again.
- **Don't bother testing:** trivial one-line getters, third-party libraries (they have their own tests), or implementation details that would make the test break on every harmless refactor even when behavior hasn't actually changed.

A test suite's value comes from the bugs it actually catches, not from a coverage percentage — Module 05 covers coverage metrics and why chasing 100% is usually the wrong goal.

---

## Common Mistakes

- **Writing zero tests until something breaks in production, then trying to test everything at once.** Testing is far cheaper to introduce gradually, as code is written, than to retrofit onto a large existing codebase all at once.
- **Testing implementation details instead of behavior.** A test that checks _how_ a function works internally (which private variable changed) breaks on every refactor even when the actual output is still correct — testing the _input → output_ contract instead is what actually gives refactoring confidence.
- **An inverted pyramid** — reaching for a full E2E test to check something a two-line unit test could verify in milliseconds. E2E tests are valuable specifically because they're rare and reserved for what actually needs them.
