## Assignment Overview

Below are three small, broken pieces of code. Each one looks reasonable at a glance, runs without throwing a syntax error, and produces the **wrong result** — for a specific, conceptual reason rooted in closures, prototypes, or error handling. Your job is to find each bug, explain _why_ it happens, and fix it.

This is a debug-style assignment (same format as `html/fix-the-html` and `css/fix-the-responsive-layout`) — you're not building something new, you're developing the skill of reading code and knowing exactly where and why it's wrong.

---

## Covers

`javascript/08 - Closures In Depth`, `javascript/09 - Prototypes & The Prototype Chain`, `javascript/10 - Error Handling`

---

## Bug 1 — The Loop That Logs the Wrong Numbers

```js
function createButtons() {
  for (var i = 0; i < 3; i++) {
    setTimeout(() => {
      console.log(`Button ${i} clicked`);
    }, 100);
  }
}

createButtons();
// Expected output (one per line, after ~100ms):
//   Button 0 clicked
//   Button 1 clicked
//   Button 2 clicked
//
// Actual output:
//   Button 3 clicked
//   Button 3 clicked
//   Button 3 clicked
```

**Your task**: explain why every callback logs `3` instead of `0`, `1`, `2`, and fix it with a **one-word change**.

---

## Bug 2 — The Counter Factory That Shares State It Shouldn't

```js
function createCounter() {
  let count = 0;
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

const counterA = createCounter();
const counterB = createCounter();

counterA.increment();
counterA.increment();
counterB.increment();

console.log(counterA.increment()); // expected: 3
console.log(counterB.increment()); // expected: 2

// Actual: this part is already correct! Read the code carefully —
// the bug is in what happens next:

counterA.reset();
console.log(counterB.increment()); // expected: 3 (counterB's OWN count, unaffected by counterA.reset())
// actual: also affected — figure out why, by reading
// the version below, which is subtly different from
// the one above
```

```js
// This is the ACTUAL buggy version — spot the difference from the working
// version above
let count = 0; // <-- moved outside the factory function

function createCounterBuggy() {
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

const buggyA = createCounterBuggy();
const buggyB = createCounterBuggy();
buggyA.increment();
buggyB.increment();
console.log(buggyA.increment()); // expected 2, but what does it actually log?
```

**Your task**: explain exactly why moving `let count = 0;` outside the factory function breaks independence between counters, in terms of closures and where each function's variable scope actually points.

---

## Bug 3 — The Fetch That Fails Silently

```js
async function loadUserProfile(userId) {
  const response = await fetch(
    `https://jsonplaceholder.typicode.com/users/${userId}`,
  );
  const data = await response.json();
  return data;
}

async function displayProfile(userId) {
  const profile = await loadUserProfile(userId);
  document.getElementById("profile-name").textContent = profile.name;
}

displayProfile(9999); // an ID that doesn't exist
// Expected: some kind of visible error message on the page
// Actual: nothing happens — no error in the console, no message on the page,
//         profile-name just stays empty forever
```

**Your task**: explain why this fails **silently** — with no thrown error and no console message — even though `userId: 9999` doesn't correspond to a real user, and rewrite it so a real error is surfaced (both logged AND shown to the user).

---

## Submission Guidelines

Create a file `fixes.md` with three sections (`## Bug 1`, `## Bug 2`, `## Bug 3`), each containing:

1. **What's actually happening** — a plain-English explanation of the bug's real cause
2. **The fix** — your corrected code
3. **Why the fix works** — one or two sentences connecting it back to the underlying concept (closures / prototype scope / error handling)

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1 (Bug 1)** — `var` is function-scoped, not block-scoped. All three `setTimeout` callbacks close over the _same_ `i` variable, which has already finished looping (reaching `3`) by the time any of them actually runs 100ms later. What declaration keyword creates a **new** binding of `i` on every loop iteration instead?

> 💡 **Hint 2 (Bug 2)** — A closure captures a **reference** to a variable, not a snapshot of its value at creation time. When `count` lives _inside_ `createCounter()`, each call to the factory creates a brand-new `count` in a brand-new scope. When `count` lives _outside_, at the module level, every counter object created by the factory closes over the exact same single variable — there's only one `count` to share, no matter how many "counters" you create.

> 💡 **Hint 3 (Bug 3)** — Recall from the "Fetch and Render" assignment: `fetch()` only rejects on a genuine network failure, not on an HTTP error status. A request for a non-existent user ID still returns a "successful" (200 or sometimes 404, depending on the API) response as far as `fetch()`'s promise is concerned — check what `response.ok` and `response.status` actually are for this specific request, and handle it explicitly.

---

## Bonus Challenge 🏆

- 🔒 Rewrite Bug 2's factory function using a **class with a private field** (`#count`) instead of a closure, and explain in a sentence why a private class field gives you the same encapsulation guarantee as the closure version.
- 🐛 Write one more "trap" of your own — a short snippet with a closure, prototype, or async bug — and swap it with a classmate to see if they can diagnose it.

---

## Learning Objectives

- ✓ Understand exactly why `var` in a loop with an async callback captures the wrong value, and how `let` fixes it.
- ✓ Understand that closures capture variable _references_, and that where a variable is declared determines whether it's shared or independent.
- ✓ Recognize that `fetch()` requires an explicit `response.ok` check — silent failures are a direct consequence of skipping it.

**Good Luck! 🚀**
