# JavaScript Assignment 4 — Solution Guide

## How to Use This File

Read this **after** attempting the assignment yourself. Each bug is explained in full, with the corrected code.

---

## Bug 1 — The Loop That Logs the Wrong Numbers

**What's actually happening**: `var` is function-scoped, not block-scoped — there is only **one** `i` variable for the entire loop, not a new one per iteration. The loop runs to completion (incrementing `i` to `3` and exiting the loop condition) _before_ any of the three `setTimeout` callbacks actually fire, 100ms later. By the time they run, they all look up the same, single `i`, which is now `3`.

**The fix**:

```js
function createButtons() {
  for (let i = 0; i < 3; i++) {
    // ← var changed to let
    setTimeout(() => {
      console.log(`Button ${i} clicked`);
    }, 100);
  }
}
```

**Why the fix works**: `let` is block-scoped, and — specifically for `for` loops — the JavaScript spec creates a **fresh binding of `i` for every iteration**. Each of the three arrow functions now closes over its own independent `i` (`0`, `1`, and `2` respectively), rather than all three sharing the single `var i` that keeps counting after the loop ends.

---

## Bug 2 — The Counter Factory That Shares State It Shouldn't

**What's actually happening**: A closure captures a _reference_ to a variable in whatever scope it was declared in — not a private copy made just for that function. In the **working** version, `let count = 0;` lives inside `createCounter()`, so every call to the factory creates a brand-new scope with its own `count`. In the **buggy** version, `count` was moved outside the factory entirely, to the module/top level — now there is only **one** `count` in the whole program, and every counter object returned by `createCounterBuggy()` closes over that exact same variable. `buggyA.increment()` and `buggyB.increment()` are incrementing the identical variable, just accessed through two different objects.

**The fix**: keep the variable declaration _inside_ the factory function, exactly as in the first, working example — the "bug" was moving it out, so the fix is simply moving it back in:

```js
function createCounterFixed() {
  let count = 0; // ← lives inside the factory again — one fresh count per call
  return {
    increment: function () {
      count++;
      return count;
    },
    reset: function () {
      count = 0;
    },
  };
}
```

**Why the fix works**: each call to `createCounterFixed()` creates a new function execution context with its own `count` variable. The two returned objects' `increment`/`reset` methods close over _their own_ factory call's `count` — completely independent of each other, which is the entire point of using a closure-based factory instead of a single shared variable.

---

## Bug 3 — The Fetch That Fails Silently

**What's actually happening**: `fetch()`'s returned promise only **rejects** on a genuine network-level failure (DNS failure, no connection, CORS block). An HTTP error response — like a 404 for a user ID that doesn't exist — is still a "successful" fetch as far as the promise is concerned; `response.ok` will simply be `false` and `response.status` will be `404`. The code never checks either of those, so it proceeds to call `.json()` on the (likely empty or error-shaped) body, gets back something that doesn't have a `.name` property, and silently sets `textContent` to `undefined` — no exception is ever thrown, so the `async` functions complete "successfully" with nothing visibly wrong.

**The fix**:

```js
async function loadUserProfile(userId) {
  const response = await fetch(
    `https://jsonplaceholder.typicode.com/users/${userId}`,
  );

  if (!response.ok) {
    throw new Error(`User ${userId} not found (status ${response.status})`);
  }

  return response.json();
}

async function displayProfile(userId) {
  try {
    const profile = await loadUserProfile(userId);
    document.getElementById("profile-name").textContent = profile.name;
  } catch (error) {
    console.error(error);
    document.getElementById("profile-name").textContent =
      "Could not load profile.";
  }
}
```

**Why the fix works**: explicitly checking `response.ok` and throwing turns a silent, invisible failure into a real JavaScript error — one that a `try`/`catch` can actually catch. The `catch` block then does two things a robust error handler always should: logs the real error for debugging (`console.error`), and shows the _user_ something understandable, rather than leaving a blank field with no explanation.

---

## The Thread Connecting All Three

Every one of these bugs shares the same underlying shape: **something silently does the wrong thing instead of failing loudly.** `var` silently shares a variable across iterations. Moving a `let` out of a factory function silently shares state across "independent" objects. Skipping `response.ok` silently swallows an HTTP error. Recognizing this pattern — code that runs without complaint but produces a subtly wrong result — is most of what real-world JavaScript debugging actually is, far more often than syntax errors or crashes.
