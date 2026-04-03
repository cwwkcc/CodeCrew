> The DOM is the browser's live representation of your HTML. JavaScript manipulates it to make pages dynamic — showing and hiding elements, updating content, responding to user actions. This is what JavaScript was originally built for, and understanding it deeply makes you a better React developer too, because React is just a layer on top of these same mechanisms.

---

## Table of Contents

1. [What the DOM Is](#1-what-the-dom-is)
2. [Selecting Elements](#2-selecting-elements)
3. [Reading and Writing Content](#3-reading-and-writing-content)
4. [Attributes and Classes](#4-attributes-and-classes)
5. [Styles](#5-styles)
6. [Creating and Modifying the DOM](#6-creating-and-modifying-the-dom)
7. [Events — Fundamentals](#7-events--fundamentals)
8. [The Event Object](#8-the-event-object)
9. [Event Bubbling, Capturing, and Delegation](#9-event-bubbling-capturing-and-delegation)
10. [Forms](#10-forms)
11. [Observers and Mutations](#11-observers-and-mutations)

---

## 1. What the DOM Is

When the browser parses HTML, it creates a tree of objects called the **Document Object Model**. Each HTML element becomes a **node** in this tree.

```
HTML:
<body>
  <header>
    <h1>CWWKCC</h1>
  </header>
  <main>
    <p class="intro">Welcome</p>
  </main>
</body>

DOM Tree:
document
  └── html
        ├── head
        └── body
              ├── header
              │     └── h1 → "CWWKCC"
              └── main
                    └── p.intro → "Welcome"
```

JavaScript can read, modify, add, or remove any node in this tree. Every change immediately reflects in what the user sees.

`document` is the entry point — the root of the DOM tree.

---

## 2. Selecting Elements

```javascript
// querySelector — returns the FIRST match (CSS selector)
const heading = document.querySelector("h1");
const intro = document.querySelector(".intro");          // class
const form = document.querySelector("#login-form");     // id
const input = document.querySelector("input[type='email']");  // attribute
const nested = document.querySelector(".card .title");  // descendant

// Returns null if not found — always check before using
if (heading) {
  heading.textContent = "New Title";
}

// querySelectorAll — returns ALL matches as a NodeList
const allButtons = document.querySelectorAll("button");
const navLinks = document.querySelectorAll("nav a");
const checked = document.querySelectorAll("input[type='checkbox']:checked");

// NodeList is array-like but NOT a real array
// Convert to array to use array methods
const buttons = [...document.querySelectorAll("button")];
buttons.map(btn => btn.textContent);  // array methods work now

// Scoped to an element — search within a specific element
const nav = document.querySelector("nav");
const navLinks2 = nav.querySelectorAll("a");  // only anchors inside nav

// Other selectors (older but still used):
document.getElementById("main-title");         // by ID (fastest)
document.getElementsByClassName("card");       // live HTMLCollection
document.getElementsByTagName("p");            // live HTMLCollection

// Traversal
element.parentElement          // the direct parent
element.children               // live HTMLCollection of child ELEMENTS
element.firstElementChild      // first child element
element.lastElementChild       // last child element
element.nextElementSibling     // next sibling element
element.previousElementSibling // previous sibling element
element.closest(".card")       // walks UP the tree to find matching ancestor
```

---

## 3. Reading and Writing Content

```javascript
const el = document.querySelector(".intro");

// textContent — raw text (safe, no HTML parsing)
el.textContent;              // "Welcome"
el.textContent = "Updated";  // sets text — any HTML is escaped (safe)
el.textContent = "<b>Bold</b>";  // renders as literal text, not bold

// innerHTML — HTML string (can parse HTML — XSS risk if user input!)
el.innerHTML;              // "<span>Welcome</span>"
el.innerHTML = "<strong>Hello</strong>";  // actually renders as bold

// NEVER put user input in innerHTML without sanitisation:
// el.innerHTML = userInput;  ← XSS vulnerability!

// innerText — like textContent but respects CSS visibility
// (textContent returns all text including hidden elements)
el.innerText;  // only visible text

// value — for form inputs
const input = document.querySelector("input");
input.value;             // current value of the input
input.value = "Ashan";  // set the value programmatically
```

---

## 4. Attributes and Classes

```javascript
const link = document.querySelector("a");

// getAttribute / setAttribute / removeAttribute
link.getAttribute("href");              // "/about"
link.setAttribute("href", "/contact");  // change the href
link.setAttribute("target", "_blank");  // add a new attribute
link.removeAttribute("target");         // remove it
link.hasAttribute("href");              // true

// Direct property access (for standard attributes)
link.href;       // "https://..." (resolved absolute URL)
link.id;
link.type;       // for inputs
link.disabled;   // boolean for form elements
link.checked;    // boolean for checkboxes

// Data attributes — custom data stored on elements
// HTML: <div data-user-id="123" data-role="admin">
const div = document.querySelector("[data-user-id]");
div.dataset.userId;   // "123" (camelCase from kebab-case)
div.dataset.role;     // "admin"
div.dataset.score = 95;  // sets data-score="95"

// classList — the right way to manage classes
const card = document.querySelector(".card");

card.classList.add("active");
card.classList.remove("loading");
card.classList.toggle("expanded");          // add if absent, remove if present
card.classList.toggle("visible", true);    // force add (second arg is boolean)
card.classList.toggle("hidden", false);    // force remove
card.classList.contains("active");         // true/false
card.classList.replace("old-class", "new-class");

// All classes as string
card.className;  // "card active expanded"
```

---

## 5. Styles

```javascript
const el = document.querySelector(".box");

// Inline styles — via style property
el.style.color = "red";
el.style.fontSize = "18px";      // camelCase for CSS properties
el.style.backgroundColor = "#1a1a1a";
el.style.display = "none";       // hide element

// Read computed styles (including from stylesheets)
const computed = window.getComputedStyle(el);
computed.color;          // "rgb(255, 0, 0)"
computed.fontSize;       // "18px"
computed.display;        // "block"

// Prefer toggling classes over inline styles
// (keep styles in CSS, toggle behaviour in JS)
el.classList.toggle("hidden");  // .hidden { display: none }
el.classList.toggle("active");  // .active { color: red; font-weight: bold }

// CSS custom properties (variables)
document.documentElement.style.setProperty("--primary-color", "#c9a84c");
getComputedStyle(el).getPropertyValue("--primary-color");  // "#c9a84c"
```

---

## 6. Creating and Modifying the DOM

```javascript
// createElement — create a new element
const div = document.createElement("div");
div.className = "card";
div.textContent = "Hello";

// Append to the DOM
document.body.appendChild(div);   // add as last child of body
document.body.prepend(div);       // add as first child
document.querySelector(".container").append(div);  // add to specific element

// Modern insertion methods (more flexible)
const existing = document.querySelector(".existing");
existing.before(div);    // insert before existing
existing.after(div);     // insert after existing
existing.replaceWith(div);  // replace existing with div

// append accepts multiple nodes and strings
container.append("Text node", span, anotherDiv);

// Remove
div.remove();  // remove from DOM (modern, clean)

// Clone
const clone = div.cloneNode(true);   // true = deep clone (with children)
const shallow = div.cloneNode(false); // false = element only, no children

// insertAdjacentHTML — insert HTML at specific position relative to element
existing.insertAdjacentHTML("beforebegin", "<p>Before the element</p>");
existing.insertAdjacentHTML("afterbegin",  "<p>First child</p>");
existing.insertAdjacentHTML("beforeend",   "<p>Last child</p>");
existing.insertAdjacentHTML("afterend",    "<p>After the element</p>");
// ⚠️ Same XSS risk as innerHTML — don't use with unsanitised user input

// DocumentFragment — batch DOM insertions for performance
const fragment = document.createDocumentFragment();
students.forEach(student => {
  const li = document.createElement("li");
  li.textContent = student.name;
  fragment.appendChild(li);
});
ul.appendChild(fragment);  // ONE DOM update instead of N
```

---

## 7. Events — Fundamentals

Events are signals that something happened — a click, a keystroke, a page load, a network response.

```javascript
const button = document.querySelector("button");

// addEventListener — the correct way to attach event handlers
button.addEventListener("click", function handler(event) {
  console.log("Clicked!", event);
});

// Arrow function handler
button.addEventListener("click", (e) => {
  e.preventDefault();   // stop default browser behaviour
  handleClick(e);
});

// Remove a listener — MUST pass the same function reference
function handleClick(e) { console.log("clicked"); }
button.addEventListener("click", handleClick);
button.removeEventListener("click", handleClick);
// Note: you can only remove named functions — anonymous functions can't be removed

// Once — fires once, then removes itself automatically
button.addEventListener("click", handleOnce, { once: true });

// Common events
"click"         // mouse click / touch tap
"dblclick"      // double click
"mousedown"     // mouse button pressed
"mouseup"       // mouse button released
"mousemove"     // mouse moved
"mouseenter"    // mouse entered element (no bubbling)
"mouseleave"    // mouse left element (no bubbling)
"mouseover"     // mouse over element or child (bubbles)
"mouseout"      // mouse out of element or child (bubbles)
"contextmenu"   // right click

"keydown"       // key pressed (fires repeatedly if held)
"keyup"         // key released
"keypress"      // deprecated — use keydown/keyup

"input"         // input value changed (real-time)
"change"        // input value changed and element loses focus
"submit"        // form submitted
"focus"         // element gained focus
"blur"          // element lost focus
"focusin"       // focus (bubbles — unlike focus)
"focusout"      // blur (bubbles)

"load"          // resource (page, image, script) finished loading
"DOMContentLoaded"  // DOM ready (before external resources)
"resize"        // window resized
"scroll"        // element or page scrolled
"visibilitychange"  // tab became active/inactive

"touchstart"    // finger touched screen
"touchend"      // finger lifted
"touchmove"     // finger moved

"dragstart"     // drag began
"dragover"      // dragging over a drop target
"drop"          // dropped on a target
```

---

## 8. The Event Object

Every event handler receives an `event` object with information about what happened.

```javascript
document.addEventListener("click", (e) => {
  e.type;           // "click"
  e.target;         // the ELEMENT that was clicked
  e.currentTarget;  // the ELEMENT the handler is attached to
  e.timeStamp;      // when the event occurred (ms from page load)

  e.preventDefault();    // stop default behaviour (e.g. form submit, link navigation)
  e.stopPropagation();   // stop event from bubbling up
  e.stopImmediatePropagation();  // stop bubbling AND prevent other handlers on same element
});

// Mouse event properties
document.addEventListener("click", (e) => {
  e.clientX; e.clientY;   // position relative to viewport
  e.pageX;   e.pageY;     // position relative to document
  e.offsetX; e.offsetY;   // position relative to element
  e.button;               // which mouse button (0=left, 1=middle, 2=right)
  e.ctrlKey;              // was Ctrl held?
  e.shiftKey;             // was Shift held?
  e.altKey;               // was Alt held?
  e.metaKey;              // was Cmd/Win held?
});

// Keyboard event properties
document.addEventListener("keydown", (e) => {
  e.key;      // "Enter", "ArrowUp", "a", "A", " " (spacebar)
  e.code;     // "KeyA", "Space", "Enter" — physical key (layout-independent)
  e.repeat;   // true if key is being held down

  if (e.key === "Escape") closeModal();
  if (e.key === "Enter" && e.ctrlKey) submitForm();
});

// Input event
input.addEventListener("input", (e) => {
  e.target.value;  // current value of the input
});
```

---

## 9. Event Bubbling, Capturing, and Delegation

When an event fires on an element, it doesn't just fire there — it travels through the DOM.

```javascript
// Event bubbling — event travels FROM the target UP to document
// <div id="parent">
//   <button id="child">Click me</button>
// </div>

document.querySelector("#parent").addEventListener("click", (e) => {
  console.log("parent handler");  // fires SECOND
});

document.querySelector("#child").addEventListener("click", (e) => {
  console.log("child handler");   // fires FIRST
  // e.stopPropagation();         // would prevent "parent handler" from running
});

// Clicking the button: "child handler" then "parent handler"

// e.target vs e.currentTarget
parent.addEventListener("click", (e) => {
  e.target;         // the actual element clicked (the button)
  e.currentTarget;  // the element this handler is on (the parent)
});
```

### Event Delegation — attach ONE handler to a parent

```javascript
// INEFFICIENT: 100 items = 100 event listeners
document.querySelectorAll(".student-row").forEach(row => {
  row.addEventListener("click", handleRowClick);
});

// EFFICIENT: 1 event listener on the parent (delegation)
document.querySelector(".students-table").addEventListener("click", (e) => {
  // e.target is the actual element clicked
  const row = e.target.closest(".student-row");  // walk up to find row
  if (!row) return;  // click wasn't on a row

  const studentId = row.dataset.studentId;
  handleRowClick(studentId);
});

// Delegation also works for dynamically added elements:
// New rows added later will automatically work — no need to re-attach listeners
```

---

## 10. Forms

```javascript
const form = document.querySelector("#login-form");

// Prevent default form submission (page reload)
form.addEventListener("submit", (e) => {
  e.preventDefault();

  // Read all form values
  const formData = new FormData(form);
  const email    = formData.get("email");
  const password = formData.get("password");

  // Or read individual inputs
  const emailInput = form.querySelector("[name='email']");
  emailInput.value;

  // Object from FormData
  const data = Object.fromEntries(formData);
  // { email: "ashan@school.lk", password: "..." }
});

// Input validation
const emailInput = document.querySelector("#email");

emailInput.addEventListener("blur", (e) => {
  const value = e.target.value;
  if (!value.includes("@")) {
    showError(e.target, "Invalid email");
  } else {
    clearError(e.target);
  }
});

// Programmatic form control
form.reset();                 // reset all fields
emailInput.focus();           // focus an input
emailInput.select();          // select all text in input
emailInput.setCustomValidity("Email is taken");  // set custom validation message
emailInput.reportValidity();  // show validation UI

// Checkbox and radio
const checkbox = document.querySelector("input[type='checkbox']");
checkbox.checked;        // true/false
checkbox.indeterminate;  // true for "some but not all" state

const radios = document.querySelectorAll("input[name='grade']");
const selectedGrade = [...radios].find(r => r.checked)?.value;
```

---

## 11. Observers and Mutations

### IntersectionObserver — element visibility

```javascript
// Track when elements enter/leave the viewport
const observer = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      entry.target.classList.add("visible");  // animate in
      observer.unobserve(entry.target);       // stop watching once visible
    }
  });
}, {
  threshold: 0.1,    // fire when 10% of element is visible
  rootMargin: "0px 0px -50px 0px",  // shrink viewport bottom by 50px
});

document.querySelectorAll(".animate-on-scroll").forEach(el => {
  observer.observe(el);
});

// Lazy loading images
const imageObserver = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const img = entry.target;
      img.src = img.dataset.src;   // load the real image
      imageObserver.unobserve(img);
    }
  });
});

document.querySelectorAll("img[data-src]").forEach(img => {
  imageObserver.observe(img);
});
```

### ResizeObserver — element size changes

```javascript
const resizeObserver = new ResizeObserver((entries) => {
  entries.forEach(entry => {
    const { width, height } = entry.contentRect;
    console.log(`Element resized: ${width}×${height}`);
    adjustLayout(entry.target, width);
  });
});

resizeObserver.observe(document.querySelector(".responsive-chart"));
```

### MutationObserver — DOM changes

```javascript
// Watch for changes to the DOM
const mutationObserver = new MutationObserver((mutations) => {
  mutations.forEach(mutation => {
    if (mutation.type === "childList") {
      mutation.addedNodes.forEach(node => {
        console.log("Node added:", node);
      });
    }
    if (mutation.type === "attributes") {
      console.log(`Attribute ${mutation.attributeName} changed`);
    }
  });
});

mutationObserver.observe(document.querySelector(".container"), {
  childList: true,      // watch for added/removed children
  subtree: true,        // watch all descendants, not just direct children
  attributes: true,     // watch attribute changes
  characterData: true,  // watch text content changes
});

mutationObserver.disconnect();  // stop observing
```

---

## Summary

```
Selecting:
  querySelector / querySelectorAll — CSS selectors, most flexible
  getElementById — fastest for ID lookup
  .closest() — walk UP the DOM to find an ancestor

Content:
  textContent — safe text, no HTML parsing
  innerHTML — renders HTML, XSS risk with user input
  value — for input elements

Attributes:
  getAttribute/setAttribute/removeAttribute
  dataset — for data-* attributes
  classList.add/remove/toggle/contains — manage classes

Creating:
  createElement → set properties → append to DOM
  DocumentFragment — batch multiple insertions
  insertAdjacentHTML — insert at specific position

Events:
  addEventListener(type, handler, options)
  removeEventListener — must pass same function reference
  { once: true } — auto-removes after first fire
  e.preventDefault() — stop default (form submit, link nav)
  e.stopPropagation() — stop bubbling

Delegation:
  Attach ONE handler to parent, check e.target.closest()
  Works for dynamically added elements

Forms:
  e.preventDefault() on submit
  new FormData(form) — get all values at once
  Object.fromEntries(formData) — convert to plain object

Observers:
  IntersectionObserver — visibility, lazy loading, scroll animations
  ResizeObserver — element dimensions
  MutationObserver — DOM changes
```

---

_Next: [05 — Object-Oriented Programming](./05%20-%20Object-Oriented%20Programming.md)_