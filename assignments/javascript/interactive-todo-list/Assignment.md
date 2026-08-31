## Assignment Overview

Build a working todo list in the browser — add items, mark them done, delete them — using only vanilla JavaScript DOM manipulation. No frameworks (that's what the React curriculum is for). This assignment is about two things: writing modern ES6+ syntax fluently, and manipulating the real DOM correctly and efficiently.

---

## Covers

`javascript/03 - Modern JavaScript (ES6+)`, `javascript/04 - DOM Manipulation & Events`

---

## The Starting HTML

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <title>Todo List</title>
  </head>
  <body>
    <form id="todo-form">
      <input type="text" id="todo-input" placeholder="Add a task..." required />
      <button type="submit">Add</button>
    </form>

    <ul id="todo-list"></ul>

    <p id="todo-count"></p>

    <script src="todo.js"></script>
  </body>
</html>
```

---

## Your Task

Write `todo.js` so that:

### 1. Adding a Task

- Submitting the form (typing text and pressing "Add" or Enter) creates a new `<li>` inside `#todo-list`
- Each `<li>` should contain: the task text, a "Done" checkbox/button, and a "Delete" button
- The input field should clear itself after adding
- **The form submission must not reload the page** — this requires `event.preventDefault()`

### 2. Marking a Task Done

- Clicking the "Done" control on a task toggles a `done` visual state (e.g. strikethrough text via a CSS class) — clicking again un-marks it
- This must work correctly for **every** task, including ones added after the page first loaded — a common DOM mistake is only wiring up event listeners for elements that existed at page-load time

### 3. Deleting a Task

- Clicking "Delete" removes that specific `<li>` from the list — and only that one, not others

### 4. Live Count

- `#todo-count` should always show `"3 tasks, 1 done"` (updating live as tasks are added, completed, or deleted) using a template literal, not string concatenation

---

## Constraints — Modern JavaScript Requirements

Your code must demonstrate genuine use of at least the following ES6+ features (not just "old JavaScript that happens to run in a modern browser"):

- **`const`/`let`** — no `var` anywhere
- **Arrow functions** for at least your event handler callbacks
- **Template literals** for building any string that includes a variable (task counts, generated HTML)
- **Destructuring** — at least one place where you destructure a value out of an object or array
- **`querySelector`/`querySelectorAll`** rather than the older `getElementById`-only style, at least where selecting multiple or complex elements

---

## Constraints — DOM Requirements

- **Use event delegation for the Done/Delete buttons** — attach _one_ listener to `#todo-list` itself (using `event.target` to figure out what was clicked), not one listener per task. This is the correct pattern for handling events on elements that don't exist yet at page load, and it's covered directly in Module 04.
- No inline `onclick=""` attributes in your generated HTML — attach all listeners in JavaScript.
- No page reloads or navigations — everything happens by manipulating the existing DOM.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — Event delegation: `todoList.addEventListener("click", (e) => { if (e.target.matches(".delete-btn")) { ... } })` — the listener lives on the parent, but `e.target` tells you exactly which child was actually clicked, even for children added long after the listener was attached.

> 💡 **Hint 2** — To find "which task" a clicked button belongs to, give each `<li>` a `data-id` attribute when you create it, then use `e.target.closest("li").dataset.id` to trace back from the button to its containing task.

> 💡 **Hint 3** — For the live count, recalculate it from the actual DOM (`document.querySelectorAll(".done").length`) or from a JavaScript array you're keeping in sync — either works, but pick one source of truth and stick to it, rather than trying to keep two things in sync manually.

---

## Submission Guidelines

1. `index.html` (as given, or with minor styling additions)
2. `todo.js` — your complete implementation
3. A comment at the top of `todo.js` noting where you used event delegation and why

---

## Bonus Challenge 🏆

- 🔤 Add input validation: don't allow adding a task that's empty or only whitespace (`.trim()`).
- ✏️ Add an "Edit" button that turns the task text into an editable `<input>` in place, saving on Enter or blur.
- 🎯 Add a filter (`All` / `Active` / `Done`) that shows/hides tasks without deleting them from the underlying data.

---

## Learning Objectives

- ✓ Write idiomatic ES6+ JavaScript (arrow functions, template literals, destructuring, `const`/`let`).
- ✓ Use event delegation correctly instead of attaching listeners to individual dynamic elements.
- ✓ Correctly prevent default form behavior and manage DOM state without a framework.

**Good Luck! 🚀**
