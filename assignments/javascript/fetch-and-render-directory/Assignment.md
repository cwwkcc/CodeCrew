## Assignment Overview

Fetch real data from a public API, model it with a class, and render it to the page — with proper loading and error states. This assignment ties together three modules that are each hard alone and genuinely difficult to use correctly together: classes, `async`/`await`, and understanding _why_ your UI needs to handle "still loading" and "something went wrong" as real, first-class states rather than afterthoughts.

---

## Covers

`javascript/05 - Object-Oriented Programming`, `javascript/06 - Asynchronous JavaScript & APIs`, `javascript/07 - The JavaScript Engine, Runtime & Memory`

---

## The API

Use the free [JSONPlaceholder](https://jsonplaceholder.typicode.com/users) API — no key required:

```
GET https://jsonplaceholder.typicode.com/users
```

Returns an array of 10 fake users, each shaped like:

```json
{
  "id": 1,
  "name": "Leanne Graham",
  "email": "Sincere@april.biz",
  "phone": "1-770-736-8031 x56442",
  "company": { "name": "Romaguera-Crona" }
}
```

---

## Part 1 — Model the Data with a Class

```js
class Student {
  constructor(data) {
    // your job: pull the fields you need off `data` and store them
  }

  get initials() {
    // returns e.g. "LG" for "Leanne Graham" — a computed property, not a stored one
  }

  get contactCard() {
    // returns a formatted string: "Leanne Graham — Sincere@april.biz — Romaguera-Crona"
  }
}
```

Build the `Student` class with a constructor that takes the raw API object, plus the two **getters** above (Module 05 covers getters — a property that's computed on access, not stored).

---

## Part 2 — Fetch and Handle the Three States

Write an `async` function `loadStudents()` that:

1. Sets a loading state and renders a "Loading students..." message
2. `fetch()`s the API, converts the response to JSON, and constructs an array of `Student` instances from it
3. Renders each student's `contactCard` into the page
4. **If the fetch fails** (try changing the URL to something invalid to test this), catches the error and renders a clear error message instead of leaving the page stuck on "Loading..." forever or silently showing nothing

```html
<div id="status"></div>
<ul id="student-list"></ul>
```

```js
async function loadStudents() {
  const statusEl = document.getElementById("status");
  const listEl = document.getElementById("student-list");

  // your implementation:
  // - show loading state
  // - try the fetch + JSON parsing + rendering
  // - catch and display errors
  // - clear the loading state either way
}
```

---

## Constraints

- **Use `async`/`await` syntax, not raw `.then()` chains.** Both work, but this assignment is specifically testing `async`/`await` fluency.
- **A real `try`/`catch` around the fetch, not just a "happy path."** Test this by temporarily breaking the URL and confirming your error state actually shows up.
- **Check `response.ok` before trying to parse the JSON.** `fetch()` does **not** reject on HTTP error statuses (404, 500) — only on network failure. A 404 response still "succeeds" as far as `fetch()` is concerned; you have to check `response.ok` yourself and throw an error if it's false. This is one of the most commonly-missed details in real `fetch()` code.
- No external libraries — plain `fetch()`.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — The `response.ok` check: `if (!response.ok) throw new Error(\`HTTP error: ${response.status}\`);`right after the`fetch()`call, before calling`.json()`.

> 💡 **Hint 2** — Class getters use the `get` keyword with no parentheses when _called_: define `get initials() { ... }`, then access it as `student.initials`, **not** `student.initials()`.

> 💡 **Hint 3** — To test your error state without changing your actual fetch logic, try fetching a URL that doesn't exist (`https://jsonplaceholder.typicode.com/does-not-exist`) temporarily, confirm the error message shows correctly, then switch the URL back.

---

## Submission Guidelines

1. `index.html`
2. `student-directory.js` — the `Student` class and `loadStudents()` function
3. A short note describing what you saw when you deliberately broke the fetch URL to test your error handling

---

## Bonus Challenge 🏆

- ⏱️ Add a visible delay before rendering (using `setTimeout` wrapped in a promise) to make the loading state actually observable during normal testing — otherwise a fast local network makes it flash by too quickly to verify it works.
- 🔍 Add a search input that filters the rendered student list by name, without re-fetching.
- 🧠 In a comment, explain in your own words why `for (var i = 0; i < students.length; i++) { setTimeout(() => console.log(students[i]), 100); }` would log the wrong data with `var` but work correctly with `let` — this connects directly to Module 07's coverage of scope and the event loop.

---

## Learning Objectives

- ✓ Model API data with a class, including computed getters.
- ✓ Use `async`/`await` correctly, including proper error handling with `try`/`catch`.
- ✓ Recognize that `fetch()` doesn't reject on HTTP error responses — `response.ok` must be checked explicitly.
- ✓ Treat loading and error states as real UI states, not edge cases to skip.

**Good Luck! 🚀**
