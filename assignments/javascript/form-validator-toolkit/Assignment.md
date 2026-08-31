## Assignment Overview

Build a small, reusable form-validation module — the kind of utility you'd actually import into a real project. This assignment brings together regular expressions (the actual validation rules), ES modules (organizing the code properly instead of one giant file), a browser API (persisting the last-used form values), and a lightweight design pattern (structuring the validator itself).

---

## Covers

`javascript/11 - JavaScript design Patterns`, `javascript/12 - Regular Expressions`, `javascript/13 -  Browser APIs`, `javascript/14 - Modules in Depth`

---

## Project Structure

```
project/
├── index.html
├── main.js
└── validators.js
```

---

## Part 1 — `validators.js` (ES Module, Regex)

Write and **export** the following validation functions:

```js
export function isValidEmail(value) {
  // returns true/false
}

export function isValidSriLankanPhone(value) {
  // accepts formats like: 0771234567, +94771234567, 077-123-4567
  // returns true/false
}

export function isStrongPassword(value) {
  // at least 8 characters, at least one number, at least one letter
  // returns true/false
}

export function isNotEmpty(value) {
  // true if the trimmed value has at least one character
}
```

Each one should use a **regular expression** as its core logic (Module 12) — not a chain of unrelated string methods pretending to be a regex.

---

## Part 2 — The Validator "Registry" (a Simple Design Pattern)

Rather than writing one big `if/else` chain to validate a whole form, build a small, reusable structure that maps field names to their validation rules — this is a lightweight version of the **Strategy pattern** (Module 11): the validation _algorithm_ for each field is swapped in as data, instead of being hardcoded into one large function.

```js
// validators.js — add this to the same file
export const fieldRules = {
  email: { validate: isValidEmail, message: "Enter a valid email address" },
  phone: {
    validate: isValidSriLankanPhone,
    message: "Enter a valid Sri Lankan phone number",
  },
  password: {
    validate: isStrongPassword,
    message: "Password must be 8+ characters with a letter and a number",
  },
};

export function validateForm(formData, rules) {
  // formData: { email: "...", phone: "...", password: "..." }
  // rules: fieldRules (or a subset of it)
  //
  // returns: { valid: boolean, errors: { fieldName: message, ... } }
  // (errors object only contains entries for FAILING fields)
}
```

---

## Part 3 — `main.js` — Wiring It Up, Plus a Browser API

```html
<form id="signup-form">
  <input type="email" name="email" placeholder="Email" />
  <input type="tel" name="phone" placeholder="Phone" />
  <input type="password" name="password" placeholder="Password" />
  <button type="submit">Sign Up</button>
</form>
<div id="errors"></div>
```

- On submit, run `validateForm()` against the entered values using `fieldRules`
- If invalid, prevent submission and display each error message next to (or below) the relevant field
- If valid, show a success message (no real server — this is a front-end-only exercise)
- **Browser API requirement**: use `localStorage` to remember the last **valid** email entered, and pre-fill the email field with it on page load — so a returning visitor doesn't have to retype it

```js
import { fieldRules, validateForm } from "./validators.js";

// your wiring code here
```

```html
<!-- main.js must be loaded as a module for import/export to work -->
<script type="module" src="main.js"></script>
```

---

## Constraints

- `validators.js` must use `export`/`import` (ES modules) — not attaching functions to the global `window` object.
- Every validation rule's core check must be a regular expression, not string-method-only logic (`.includes()`, `.length` alone, etc.) — the point of this assignment is practicing regex specifically.
- `localStorage` values are always strings — if you store anything beyond a plain string, remember you'll need `JSON.stringify`/`JSON.parse` to round-trip it correctly (a callback to `javascript/02`'s `JSON` coverage).

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — A reasonable email regex for this exercise (not a fully RFC-compliant one — those are notoriously complex): `/^[^\s@]+@[^\s@]+\.[^\s@]+$/`.

> 💡 **Hint 2** — For the Sri Lankan phone format, think about what's actually _optional_ (a `+94` or `0` prefix, dashes) versus what's fixed (9 digits after the prefix) — regex alternation (`|`) and optional groups (`?`) both apply here.

> 💡 **Hint 3** — `<script type="module">` is required for `import`/`export` to work in a browser at all — a plain `<script src="main.js">` (no `type="module"`) throws a syntax error the moment it hits an `import` statement.

---

## Submission Guidelines

1. `index.html`
2. `validators.js`
3. `main.js`
4. A short note listing your final regex for each of the four validators, and one sentence per regex explaining what each part matches

---

## Bonus Challenge 🏆

- 🔁 Add real-time validation — show/clear an error message as the user types (on the `input` event) rather than only on submit.
- 🌐 Use the browser's `Intl.NumberFormat` or `matchMedia` API somewhere in the UI (e.g. a dark-mode toggle that respects `prefers-color-scheme` on first load) as a second Browser API touchpoint beyond `localStorage`.
- 🧩 Add a second "strategy" for password strength — a `isVeryStrongPassword` requiring a special character too — and let the registry swap between them via a configuration flag, without touching `validateForm()` itself.

---

## Learning Objectives

- ✓ Write real, working regular expressions for practical validation tasks.
- ✓ Organize code into genuine ES modules with `import`/`export`, instead of one large script.
- ✓ Recognize the Strategy pattern as "swap the algorithm via data, not via an if/else chain."
- ✓ Use `localStorage` correctly, including its string-only storage limitation.

**Good Luck! 🚀**
