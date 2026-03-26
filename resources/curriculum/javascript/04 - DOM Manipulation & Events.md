> **Part 5 of 7.** Everything about the Document Object Model — selecting, creating, modifying, removing elements; managing styles and classes; and the full events system including bubbling, delegation, and keyboard events.

---

## Table of Contents

1. [What Is the DOM?](#1-what-is-the-dom)
2. [DOM Setup](#2-dom-setup)
3. [Accessing DOM Elements](#3-accessing-dom-elements)
4. [Manipulating Text Content](#4-manipulating-text-content)
5. [Managing Classes](#5-managing-classes)
6. [Working with Attributes](#6-working-with-attributes)
7. [Navigating the DOM Tree](#7-navigating-the-dom-tree)
8. [Modifying Styles](#8-modifying-styles)
9. [Creating DOM Elements](#9-creating-dom-elements)
10. [Removing DOM Elements](#10-removing-dom-elements)
11. [DOM Events Introduction](#11-dom-events-introduction)
12. [Event Handling Deep Dive](#12-event-handling-deep-dive)
13. [Keyboard Events](#13-keyboard-events)
14. [Working with DOM Events (Advanced)](#14-working-with-dom-events-advanced)
15. [DOM Projects: Real Patterns](#15-dom-projects-real-patterns)

---

## 1. What Is the DOM?

The **Document Object Model** is a tree-shaped, in-memory representation of your HTML document. When a browser loads an HTML file, it parses the markup and builds this tree. JavaScript then interacts with the tree to make pages dynamic.

```
document
└── html
    ├── head
    │   ├── title
    │   └── link
    └── body
        ├── header
        │   └── nav
        ├── main
        │   ├── section#hero
        │   └── section#products
        │       └── div.product-card (×N)
        └── footer
```

Every box in that tree is a **node**. Most importantly:

- **Element nodes** — `<div>`, `<p>`, `<input>`, etc.
- **Text nodes** — the text inside elements
- **Comment nodes** — HTML comments

JavaScript accesses the DOM via the global `document` object.

### The DOM Is Not HTML

The DOM is a live representation. JavaScript can change the DOM without changing the source HTML file. What the browser _renders_ is the current DOM, not the original HTML.

---

## 2. DOM Setup

### Waiting for the DOM to be Ready

Never try to access DOM elements before they exist. Three approaches:

```html
<!-- Approach 1: defer attribute (recommended) -->
<head>
  <script src="main.js" defer></script>
</head>
<body>
  <div id="app"></div>
</body>
```

```js
// Approach 2: DOMContentLoaded event (if you must put script in head without defer)
document.addEventListener("DOMContentLoaded", () => {
  // DOM is fully parsed, safe to query elements
  initApp();
});

// Approach 3: window.onload — waits for DOM + all resources (images, CSS)
// Usually overkill — DOMContentLoaded is faster and usually sufficient
window.addEventListener("load", () => {
  initApp();
});
```

**DOMContentLoaded** fires when HTML is parsed and DOM is built (but images/CSS may not be loaded). **load** fires when everything is fully loaded — images, stylesheets, iframes.

---

## 3. Accessing DOM Elements

### Single Element Selectors

```js
// getElementById — fastest single-element selector
const appRoot = document.getElementById("app");
const modal = document.getElementById("auth-modal");

// querySelector — CSS selector, returns first match
const heading = document.querySelector("h1");
const primaryBtn = document.querySelector(".btn-primary");
const emailInput = document.querySelector("input[type='email']");
const navLinks = document.querySelector("nav > ul > li:first-child a");

// These return null if not found — always guard against null!
const element = document.querySelector("#missing-id");
if (element) {
  element.textContent = "Found!";
}
```

### Multiple Element Selectors

```js
// querySelectorAll — returns a STATIC NodeList (snapshot)
const allCards = document.querySelectorAll(".product-card");
const allInputs = document.querySelectorAll("form input, form textarea");

// Convert NodeList to array to use array methods
const cardsArray = Array.from(allCards);
const cardsArray2 = [...allCards];

cardsArray.filter(card => card.dataset.price > 100);
cardsArray.map(card => card.dataset.id);

// getElementsByClassName — returns LIVE HTMLCollection (updates with DOM)
const activeItems = document.getElementsByClassName("active");
// If you add class "active" to an element, it auto-appears in activeItems

// getElementsByTagName — also live
const paragraphs = document.getElementsByTagName("p");
```

### Static vs Live Collections

```js
const static_list = document.querySelectorAll(".item");    // static snapshot
const live_list = document.getElementsByClassName("item"); // live collection

// Adding a new .item element
const newItem = document.createElement("div");
newItem.className = "item";
document.body.appendChild(newItem);

static_list.length; // same as before — doesn't update!
live_list.length;   // +1 — auto-updated!
```

In practice, `querySelectorAll` is preferred for predictability.

### Scoped Queries — Query Within an Element

```js
// Search only within a specific container
const form = document.getElementById("login-form");
const emailInput = form.querySelector("input[name='email']"); // only searches inside form
const allInputs = form.querySelectorAll("input");
```

---

## 4. Manipulating Text Content

There are three main ways to get/set element content — each with different behavior and security implications.

### `textContent` — Raw Text

```js
const heading = document.querySelector("h1");

// Getting
heading.textContent;
// Returns ALL text inside, including hidden elements, without HTML parsing

// Setting
heading.textContent = "Welcome back, Alice!";
// Treats string as plain text — HTML is ESCAPED, so <b>bold</b> shows literally
// SAFE from XSS attacks — use this for user-generated content

// Security: textContent CANNOT cause XSS
element.textContent = userProvidedText; // even "<script>alert(1)</script>" is safe
```

### `innerText` — Rendered Text

```js
// Similar to textContent but:
// - Respects CSS (hidden elements excluded)
// - Triggers reflow (performance cost)
// - Aware of line breaks from CSS

element.innerText;     // only visible text
element.textContent;   // ALL text including hidden
```

### `innerHTML` — HTML Markup

```js
// Getting — returns full HTML markup of element's contents
const card = document.querySelector(".card");
card.innerHTML; // "<h2>Product Name</h2><p class="price">$99</p>"

// Setting — parses as HTML and renders it
card.innerHTML = `
  <h2>${product.name}</h2>
  <p class="price">$${product.price}</p>
  <button class="btn-add-cart" data-id="${product.id}">Add to Cart</button>
`;

// ⚠️ DANGER: NEVER use innerHTML with unsanitized user input!
// This causes XSS (Cross-Site Scripting):
element.innerHTML = `Hello, ${userInput}`; // if userInput = "<img onerror=alert(1) src=x>"
// → runs attacker's JavaScript!

// Safe alternative for inserting HTML with user data:
// 1. Use textContent for text
// 2. Use createElement + setAttribute for structures
// 3. Sanitize with DOMPurify library if you must use innerHTML with user data
import DOMPurify from "dompurify";
element.innerHTML = DOMPurify.sanitize(untrustedHtml);
```

### `insertAdjacentHTML` — Insert Without Replacing

```js
// Better than innerHTML = because it doesn't destroy existing event listeners
element.insertAdjacentHTML("beforebegin", "<p>Before element</p>");
element.insertAdjacentHTML("afterbegin", "<p>First inside element</p>");
element.insertAdjacentHTML("beforeend", "<p>Last inside element</p>");
element.insertAdjacentHTML("afterend", "<p>After element</p>");

// ⚠️ Same XSS risk as innerHTML — sanitize user input!
```

---

## 5. Managing Classes

`classList` is the modern API for working with CSS classes.

```js
const button = document.querySelector("#submit-btn");

// Add one or more classes
button.classList.add("loading");
button.classList.add("disabled", "processing");

// Remove
button.classList.remove("loading");
button.classList.remove("disabled", "processing");

// Toggle — adds if absent, removes if present; returns boolean (new state)
button.classList.toggle("expanded");
button.classList.toggle("active", condition); // force add/remove based on condition
// button.classList.toggle("active", true) — always adds
// button.classList.toggle("active", false) — always removes

// Check
button.classList.contains("loading"); // true/false

// Replace
button.classList.replace("btn-secondary", "btn-primary");

// All classes as array
[...button.classList]; // ["btn", "btn-primary", "loading"]
```

### Real-World: Button Loading State

```js
async function handleFormSubmit(event) {
  event.preventDefault();

  const btn = event.target.querySelector("[type='submit']");
  const form = event.target;

  // Set loading state
  btn.classList.add("loading");
  btn.disabled = true;
  btn.textContent = "Saving...";

  try {
    const data = getFormData(form);
    await submitFormData(data);

    btn.classList.remove("loading");
    btn.classList.add("success");
    btn.textContent = "Saved!";

    setTimeout(() => {
      btn.classList.remove("success");
      btn.textContent = "Save Changes";
      btn.disabled = false;
    }, 2000);

  } catch (error) {
    btn.classList.remove("loading");
    btn.classList.add("error");
    btn.textContent = "Try Again";
    btn.disabled = false;
    showErrorMessage(error.message);
  }
}
```

### Real-World: Tabs

```js
function initTabs(containerSelector) {
  const container = document.querySelector(containerSelector);
  const tabs = container.querySelectorAll("[data-tab]");
  const panels = container.querySelectorAll("[data-panel]");

  tabs.forEach(tab => {
    tab.addEventListener("click", () => {
      const targetPanel = tab.dataset.tab;

      // Remove active from all
      tabs.forEach(t => t.classList.remove("active"));
      panels.forEach(p => p.classList.remove("active"));

      // Add active to selected
      tab.classList.add("active");
      container.querySelector(`[data-panel="${targetPanel}"]`)
               .classList.add("active");
    });
  });
}

initTabs(".dashboard-tabs");
```

---

## 6. Working with Attributes

### Standard HTML Attributes

```js
const link = document.querySelector("a.nav-link");
const input = document.querySelector("input#username");

// Getting
input.getAttribute("type");      // "text"
input.getAttribute("placeholder"); // "Enter username"
input.id;                         // "username" (direct property for common attrs)
input.type;                       // "text"
link.href;                        // full URL (resolved)
link.getAttribute("href");        // as written in HTML (relative path)

// Setting
input.setAttribute("placeholder", "Username or email");
input.setAttribute("disabled", "");   // boolean attributes need empty string
input.disabled = true;                // or direct property for common attrs
link.setAttribute("href", "/dashboard");

// Removing
input.removeAttribute("disabled");
input.disabled = false;

// Checking
input.hasAttribute("required"); // true/false
```

### Data Attributes (`data-*`)

Custom HTML5 attributes for storing data in HTML elements. Accessible via `dataset`.

```html
<div class="product-card"
     data-id="prod_123"
     data-price="99.99"
     data-category="electronics"
     data-in-stock="true">
</div>
```

```js
const card = document.querySelector(".product-card");

// camelCase conversion: data-in-stock → card.dataset.inStock
card.dataset.id;        // "prod_123" (always string!)
card.dataset.price;     // "99.99" (convert with Number())
card.dataset.category;  // "electronics"
card.dataset.inStock;   // "true" (string, not boolean!)

// Setting data attributes
card.dataset.discount = "10"; // adds data-discount="10" to HTML
card.dataset.lastViewed = Date.now();

// Remove
delete card.dataset.discount;

// Reading and converting types properly
const price = parseFloat(card.dataset.price);
const isInStock = card.dataset.inStock === "true"; // string → boolean
const id = card.dataset.id;
```

### Real-World: Event Delegation with Data Attributes

```js
// Instead of adding event listeners to each button,
// use ONE listener on the parent + data attributes

const productsGrid = document.querySelector("#products-grid");

productsGrid.addEventListener("click", (event) => {
  const btn = event.target.closest("[data-action]");
  if (!btn) return;

  const action = btn.dataset.action;
  const productId = btn.closest("[data-product-id]")?.dataset.productId;

  switch (action) {
    case "add-to-cart":
      addToCart(productId);
      break;
    case "add-to-wishlist":
      addToWishlist(productId);
      break;
    case "view-details":
      openProductModal(productId);
      break;
  }
});
```

---

## 7. Navigating the DOM Tree

### Parent, Children, Siblings

```js
const item = document.querySelector(".nav-item.active");

// Parent
item.parentElement;           // immediate parent
item.parentNode;              // parent node (could be non-element)
item.closest(".nav-wrapper"); // nearest ancestor matching selector (walks up)

// Children
item.children;                // HTMLCollection of child ELEMENTS
item.childNodes;              // NodeList including text nodes, comments
item.firstElementChild;       // first child element
item.lastElementChild;        // last child element
item.childElementCount;       // number of child elements

// Siblings
item.nextElementSibling;      // next sibling element
item.previousElementSibling;  // previous sibling element

// Check if element contains another
nav.contains(item);           // true if nav has item as descendant
```

### `closest()` — The Most Useful Navigation Method

`closest()` walks UP the DOM tree from the element and returns the first ancestor that matches the selector. It checks the element itself first.

```js
// HTML:
// <table>
//   <tr data-user-id="123">
//     <td>Alice</td>
//     <td><button class="delete-btn">Delete</button></td>
//   </tr>
// </table>

document.querySelector("table").addEventListener("click", (e) => {
  const btn = e.target.closest(".delete-btn");
  if (!btn) return;

  const row = btn.closest("tr");          // walk up to find the row
  const userId = row.dataset.userId;      // "123"
  deleteUser(userId);
  row.remove();
});
```

---

## 8. Modifying Styles

### Inline Styles via `style`

```js
const element = document.querySelector(".card");

// Get inline styles only (NOT computed styles from CSS)
element.style.color;            // "" if not set inline
element.style.backgroundColor;  // camelCase! (not background-color)

// Set inline styles
element.style.color = "red";
element.style.backgroundColor = "#f0f0f0";
element.style.fontSize = "16px";
element.style.display = "none";
element.style.transform = "translateX(100px)";

// Remove inline style
element.style.color = "";

// Set multiple styles at once
Object.assign(element.style, {
  position: "fixed",
  top: "0",
  left: "0",
  width: "100%",
  zIndex: "1000",
});
```

### `getComputedStyle` — Read Actual Applied Styles

```js
// CSS: .card { padding: 16px; border-radius: 8px; }
const card = document.querySelector(".card");

// element.style only shows INLINE styles
card.style.padding;          // "" — not set inline

// getComputedStyle shows the ACTUAL rendered style
const styles = window.getComputedStyle(card);
styles.padding;              // "16px"
styles.borderRadius;         // "8px"
styles.display;              // "block"
styles.width;                // "320px"
parseFloat(styles.width);    // 320 (number)
```

### CSS Variables via JavaScript

```js
// Getting a CSS custom property
const root = document.documentElement; // :root element
const primaryColor = getComputedStyle(root).getPropertyValue("--color-primary").trim();

// Setting a CSS custom property (affects all elements using that variable!)
root.style.setProperty("--color-primary", "#6366f1");
root.style.setProperty("--sidebar-width", "280px");

// Real-world: theme switcher
function setTheme(theme) {
  const themes = {
    light: { "--bg": "#ffffff", "--text": "#111827", "--border": "#e5e7eb" },
    dark:  { "--bg": "#111827", "--text": "#f9fafb", "--border": "#374151" },
  };

  const vars = themes[theme];
  if (!vars) return;

  const root = document.documentElement;
  Object.entries(vars).forEach(([prop, value]) => {
    root.style.setProperty(prop, value);
  });

  localStorage.setItem("theme", theme);
  document.documentElement.dataset.theme = theme;
}
```

### Prefer Classes Over Inline Styles

In real projects, toggle CSS classes rather than setting inline styles — it keeps styling in CSS where it belongs:

```js
// BAD: mixing concerns, hard to maintain
element.style.display = "none";
element.style.opacity = "0";
element.style.transform = "translateY(-10px)";

// GOOD: CSS handles the styling
element.classList.add("hidden"); // CSS: .hidden { display: none; }
element.classList.add("fade-out"); // CSS: .fade-out { opacity: 0; ... }
```

---

## 9. Creating DOM Elements

### `createElement`

```js
// Create an element
const card = document.createElement("div");

// Set its properties
card.className = "product-card";
card.id = "card-123";
card.dataset.productId = "123";

// Set content
card.innerHTML = `
  <img src="/product-123.jpg" alt="Product">
  <h3>Product Name</h3>
  <p>$99.99</p>
`;
// Or use textContent / append children for safety

// Insert into DOM
document.getElementById("products-grid").appendChild(card);
```

### `append` vs `appendChild` vs `prepend`

```js
const list = document.querySelector("ul");

// appendChild — single node, returns the appended node
const li = document.createElement("li");
list.appendChild(li); // adds at end

// append — multiple nodes OR strings, returns undefined
list.append(li1, li2, "Text node");

// prepend — adds at beginning
list.prepend(li);

// insertBefore
list.insertBefore(newItem, referenceItem); // inserts before referenceItem

// after / before (on the element itself)
referenceItem.before(newItem);  // inserts newItem before referenceItem (sibling)
referenceItem.after(newItem);   // inserts newItem after referenceItem

// replaceWith
oldElement.replaceWith(newElement);
```

### Building DOM from Data (Real Pattern)

```js
function renderProductCard(product) {
  const card = document.createElement("article");
  card.className = "product-card";
  card.dataset.productId = product.id;

  // Create child elements safely (no innerHTML with user data)
  const img = document.createElement("img");
  img.src = product.imageUrl;
  img.alt = product.name; // ← user data goes into attributes, not innerHTML
  img.loading = "lazy";

  const nameEl = document.createElement("h3");
  nameEl.textContent = product.name; // ← textContent for user text — XSS safe!

  const priceEl = document.createElement("p");
  priceEl.className = "price";
  priceEl.textContent = formatCurrency(product.price);

  const addBtn = document.createElement("button");
  addBtn.className = "btn btn-primary";
  addBtn.textContent = "Add to Cart";
  addBtn.dataset.action = "add-to-cart";

  // Assemble the card
  card.append(img, nameEl, priceEl, addBtn);
  return card;
}

// Render many products efficiently
function renderProductGrid(products, container) {
  // Use DocumentFragment to batch DOM operations — only ONE reflow
  const fragment = document.createDocumentFragment();

  products.forEach(product => {
    fragment.appendChild(renderProductCard(product));
  });

  container.innerHTML = ""; // clear existing
  container.appendChild(fragment); // single DOM insertion
}
```

### Why `DocumentFragment` Matters for Performance

Every time you append an element directly to the DOM, the browser can trigger a **reflow** (recalculate layout) and **repaint**. If you're adding 100 items, that's 100 potential reflows.

`DocumentFragment` is a lightweight, in-memory container. You build everything in it, then add the fragment to the DOM in one operation — just one reflow.

```js
// SLOW: 1000 DOM insertions = potentially 1000 reflows
for (let i = 0; i < 1000; i++) {
  list.appendChild(createItem(i));
}

// FAST: 1 DOM insertion = 1 reflow
const fragment = document.createDocumentFragment();
for (let i = 0; i < 1000; i++) {
  fragment.appendChild(createItem(i));
}
list.appendChild(fragment);
```

---

## 10. Removing DOM Elements

```js
// Remove the element itself
const card = document.querySelector(".product-card");
card.remove(); // modern, cleanest way

// Remove a child
const parent = document.querySelector("#products-grid");
const child = document.querySelector(".product-card");
parent.removeChild(child); // older, more verbose

// Clear all children
parent.innerHTML = ""; // quick but destroys event listeners on children
while (parent.firstChild) {
  parent.removeChild(parent.firstChild); // preserves cleanup (event listeners?)
}
// Best for large lists: parent.replaceChildren() (modern)
parent.replaceChildren(); // removes all children
parent.replaceChildren(newChild1, newChild2); // replaces with new children

// Soft remove: hide without removing (preserves state)
element.style.display = "none";
element.hidden = true;
element.classList.add("hidden"); // with CSS .hidden { display: none }
```

---

## 11. DOM Events Introduction

An **event** is a signal that something happened — a user clicked, typed, the page loaded, etc. JavaScript can listen for events and respond.

### Event Flow: Capturing and Bubbling

When you click a button inside a div inside a section inside body, THREE phases happen:

1. **Capture phase** — event travels DOWN from `window` → `document` → `body` → `section` → `div` → `button`
2. **Target phase** — event reaches the actual target (`button`)
3. **Bubble phase** — event travels BACK UP `button` → `div` → `section` → `body` → `document` → `window`

```
window
  document
    body
      section        ← fires in capture (going down)
        div          ← fires in capture
          button     ← fires first (target)
        div          ← fires in bubble (going up)
      section        ← fires in bubble
    body             ← fires in bubble
  document           ← fires in bubble
window               ← fires in bubble
```

By default, event listeners fire in the **bubble phase**. Most real code uses bubbling.

---

## 12. Event Handling Deep Dive

### `addEventListener` — The Right Way

```js
const btn = document.querySelector("#submit-btn");

// addEventListener(event, callback, options)
btn.addEventListener("click", handleClick);

function handleClick(event) {
  console.log(event.target);    // the element that was clicked
  console.log(event.currentTarget); // the element the listener is on
  console.log(event.type);      // "click"
  console.log(event.timeStamp); // when event fired
}

// Options object
btn.addEventListener("click", handleClick, {
  once: true,      // auto-removes after first fire
  passive: true,   // hint that we won't call preventDefault (scroll performance!)
  capture: true,   // listen in capture phase instead of bubble
});

// Remove a listener (must use same function reference!)
btn.removeEventListener("click", handleClick);
// Note: anonymous functions can't be removed!
btn.addEventListener("click", () => {}); // this can NEVER be removed
```

### The Event Object

```js
document.addEventListener("click", (event) => {
  // Target info
  event.target           // element actually clicked
  event.currentTarget    // element the listener is on

  // Mouse position
  event.clientX          // X relative to viewport
  event.clientY          // Y relative to viewport
  event.pageX            // X relative to document (includes scroll)
  event.pageY            // Y relative to document
  event.offsetX          // X relative to the target element

  // Modifier keys held during click
  event.shiftKey         // boolean
  event.ctrlKey          // boolean
  event.altKey           // boolean
  event.metaKey          // Cmd on Mac, Windows key on PC

  // Default behavior and propagation
  event.preventDefault() // stops default action (link navigation, form submit, etc.)
  event.stopPropagation() // stops bubbling up to parent elements
  event.stopImmediatePropagation() // stops other listeners on same element too
});
```

### `preventDefault` vs `stopPropagation`

```js
// preventDefault — stops the browser's default behavior
// e.g.: form submit, link navigation, checkbox check, right-click menu
const form = document.querySelector("form");
form.addEventListener("submit", (e) => {
  e.preventDefault(); // stops page reload
  validateAndSubmit(form);
});

const link = document.querySelector("a.ajax-link");
link.addEventListener("click", (e) => {
  e.preventDefault(); // stops browser from navigating
  loadPageViaSPA(link.href);
});

// stopPropagation — stops the event from bubbling up
const modal = document.querySelector(".modal-content");
modal.addEventListener("click", (e) => {
  e.stopPropagation(); // prevent click from reaching the backdrop
});

const backdrop = document.querySelector(".modal-backdrop");
backdrop.addEventListener("click", () => {
  closeModal(); // only fires if user clicks backdrop, not the content
});
```

### Event Delegation

Instead of attaching listeners to each element, attach one listener to a parent and let bubbling work for you.

```js
// ❌ BAD — creates hundreds of event listeners
document.querySelectorAll(".delete-btn").forEach(btn => {
  btn.addEventListener("click", handleDelete);
});
// Problem: need to re-attach for dynamically added items!

// ✅ GOOD — one listener, handles all current and future items
const table = document.querySelector("#users-table");
table.addEventListener("click", (event) => {
  const btn = event.target.closest("button[data-action]");
  if (!btn) return; // click was not on a button

  const action = btn.dataset.action;
  const userId = btn.closest("tr").dataset.userId;

  if (action === "edit")   openEditModal(userId);
  if (action === "delete") confirmDelete(userId);
  if (action === "block")  blockUser(userId);
});
```

---

## 13. Keyboard Events

### The Three Keyboard Events

- `keydown` — fires when key is pressed (fires repeatedly if held)
- `keypress` — deprecated, don't use
- `keyup` — fires when key is released

```js
document.addEventListener("keydown", (event) => {
  event.key;      // "a", "Enter", "ArrowUp", "Escape", "Tab", " " (space)
  event.code;     // "KeyA", "Enter", "ArrowUp" — physical key, layout-independent
  event.keyCode;  // deprecated number — don't use
  event.shiftKey; // true if Shift was held
  event.ctrlKey;  // true if Ctrl was held
  event.altKey;   // true if Alt was held
  event.metaKey;  // true if Cmd/Win was held
  event.repeat;   // true if key is being held down and event is repeating
});
```

### Key Values Cheat Sheet

```js
event.key values for special keys:
"Enter"      "Escape"      "Tab"         "Backspace"    "Delete"
"ArrowUp"    "ArrowDown"   "ArrowLeft"   "ArrowRight"
"Home"       "End"         "PageUp"      "PageDown"
"F1"–"F12"   " " (space)  "Shift"       "Control"      "Alt"
"a"–"z"      "A"–"Z"      "0"–"9"
```

### Real-World: Keyboard Shortcuts

```js
// Global keyboard shortcut handler
const shortcuts = new Map([
  ["ctrl+k", openCommandPalette],
  ["ctrl+s", saveDocument],
  ["ctrl+z", undo],
  ["ctrl+shift+z", redo],
  ["escape", closeModal],
  ["f1", openHelp],
]);

document.addEventListener("keydown", (event) => {
  // Build a key identifier string
  const parts = [];
  if (event.ctrlKey || event.metaKey) parts.push("ctrl"); // treat Cmd same as Ctrl
  if (event.shiftKey) parts.push("shift");
  if (event.altKey) parts.push("alt");
  parts.push(event.key.toLowerCase());

  const shortcut = parts.join("+");
  const handler = shortcuts.get(shortcut);

  if (handler) {
    event.preventDefault();
    handler(event);
  }
});

// Escape to close modal
document.addEventListener("keydown", (e) => {
  if (e.key === "Escape") {
    const openModal = document.querySelector(".modal.open");
    if (openModal) closeModal(openModal);
  }
});
```

### Real-World: Search Input with Keyboard UX

```js
function initSearchUI() {
  const searchInput = document.querySelector("#search-input");
  const dropdown = document.querySelector("#search-dropdown");
  const results = [];
  let selectedIndex = -1;

  searchInput.addEventListener("keydown", (e) => {
    const items = dropdown.querySelectorAll(".result-item");

    switch (e.key) {
      case "ArrowDown":
        e.preventDefault(); // don't scroll page
        selectedIndex = Math.min(selectedIndex + 1, items.length - 1);
        updateHighlight(items, selectedIndex);
        break;

      case "ArrowUp":
        e.preventDefault();
        selectedIndex = Math.max(selectedIndex - 1, -1);
        updateHighlight(items, selectedIndex);
        break;

      case "Enter":
        if (selectedIndex >= 0) {
          navigateTo(results[selectedIndex].url);
        }
        break;

      case "Escape":
        closeDropdown();
        searchInput.blur();
        break;
    }
  });
}

function updateHighlight(items, selectedIndex) {
  items.forEach((item, i) => {
    item.classList.toggle("highlighted", i === selectedIndex);
  });
  if (selectedIndex >= 0) {
    items[selectedIndex].scrollIntoView({ block: "nearest" });
  }
}
```

---

## 14. Working with DOM Events (Advanced)

### Common Event Types

```js
// Mouse events
"click"        // single left click
"dblclick"     // double click
"mousedown"    // button pressed
"mouseup"      // button released
"mousemove"    // cursor moved over element
"mouseenter"   // cursor enters element (doesn't bubble)
"mouseleave"   // cursor leaves element (doesn't bubble)
"mouseover"    // cursor enters element or child (DOES bubble)
"mouseout"     // cursor leaves element or child (DOES bubble)
"contextmenu"  // right click

// Form events
"submit"       // form submitted
"change"       // value changed + focus lost (select, checkbox, radio, input)
"input"        // every keystroke in text input/textarea
"focus"        // element gains focus (doesn't bubble)
"blur"         // element loses focus (doesn't bubble)
"focusin"      // focus gained (DOES bubble)
"focusout"     // focus lost (DOES bubble)
"reset"        // form reset

// Drag events
"dragstart"    "drag"    "dragend"
"dragenter"    "dragover"  "dragleave"    "drop"

// Window/document events
"resize"       // window resized
"scroll"       // element or window scrolled
"load"         // resource loaded
"unload"       // page about to unload
"beforeunload" // warn before leaving
"hashchange"   // URL hash changed
"popstate"     // history navigation (browser back/forward)

// Clipboard
"copy"    "cut"    "paste"

// Media
"play"    "pause"    "ended"    "timeupdate"    "volumechange"

// Touch (mobile)
"touchstart"    "touchmove"    "touchend"
```

### Form Events in Depth

```js
const loginForm = document.getElementById("login-form");

// Handling form submit
loginForm.addEventListener("submit", async (event) => {
  event.preventDefault(); // CRITICAL — stops page reload

  // Get all form data as object
  const formData = new FormData(event.target);
  const data = Object.fromEntries(formData);
  // { email: "alice@example.com", password: "..." }

  // Validate
  const errors = validateLoginForm(data);
  if (errors.length) {
    showFormErrors(errors);
    return;
  }

  await submitLogin(data);
});

// Real-time validation on input
const emailInput = loginForm.querySelector("[name='email']");
emailInput.addEventListener("input", debounce((e) => {
  const isValid = /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(e.target.value);
  emailInput.classList.toggle("invalid", !isValid);
  emailInput.nextElementSibling?.classList.toggle("hidden", isValid);
}, 300));

// Getting all form values
function getFormData(form) {
  return Object.fromEntries(new FormData(form));
}

// Programmatically set/get values
emailInput.value = "alice@example.com";
emailInput.value; // "alice@example.com"

const checkbox = document.querySelector("#remember-me");
checkbox.checked; // boolean
checkbox.checked = true;

const select = document.querySelector("#role-select");
select.value; // currently selected option value
select.value = "admin"; // programmatically select
```

### `IntersectionObserver` — Lazy Loading / Infinite Scroll

```js
// Lazy load images as they scroll into viewport
const imageObserver = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const img = entry.target;
      img.src = img.dataset.src; // swap data-src to actual src
      img.classList.remove("lazy");
      imageObserver.unobserve(img); // stop watching once loaded
    }
  });
}, {
  rootMargin: "200px 0px", // start loading 200px before entering viewport
  threshold: 0,
});

document.querySelectorAll("img[data-src]").forEach(img => {
  imageObserver.observe(img);
});

// Infinite scroll: load more data when bottom sentinel is visible
const sentinel = document.querySelector("#load-more-sentinel");
const scrollObserver = new IntersectionObserver(async (entries) => {
  if (entries[0].isIntersecting && !isLoading) {
    await loadMoreItems();
  }
});
scrollObserver.observe(sentinel);
```

### `MutationObserver` — Watch DOM Changes

```js
// Watch for any changes to an element
const observer = new MutationObserver((mutations) => {
  mutations.forEach(mutation => {
    if (mutation.type === "childList") {
      console.log("Children added/removed:", mutation.addedNodes, mutation.removedNodes);
    }
    if (mutation.type === "attributes") {
      console.log(`Attribute ${mutation.attributeName} changed`);
    }
  });
});

observer.observe(document.querySelector("#dynamic-content"), {
  childList: true,   // watch for children added/removed
  attributes: true,  // watch for attribute changes
  subtree: true,     // watch all descendants, not just direct children
});

// Stop observing
observer.disconnect();
```

---

## 15. DOM Projects: Real Patterns

### Project 1: Live Search Filter

```js
function initLiveSearch(inputSelector, itemsSelector, searchableAttr = "text") {
  const input = document.querySelector(inputSelector);
  const items = [...document.querySelectorAll(itemsSelector)];

  input.addEventListener("input", debounce(() => {
    const query = input.value.toLowerCase().trim();

    let visibleCount = 0;
    items.forEach(item => {
      const text = (searchableAttr === "text"
        ? item.textContent
        : item.dataset[searchableAttr]
      ).toLowerCase();

      const matches = !query || text.includes(query);
      item.classList.toggle("hidden", !matches);
      if (matches) visibleCount++;
    });

    // Show empty state
    const emptyState = document.querySelector(".empty-state");
    if (emptyState) emptyState.classList.toggle("hidden", visibleCount > 0);
  }, 200));
}

initLiveSearch("#user-search", ".user-row");
```

### Project 2: Modal System

```js
class Modal {
  constructor(id) {
    this.modal = document.getElementById(id);
    this.backdrop = this.modal.querySelector(".modal-backdrop");
    this.content = this.modal.querySelector(".modal-content");
    this.closeButtons = this.modal.querySelectorAll("[data-action='close']");

    this.bindEvents();
  }

  open(data = {}) {
    // Populate modal with data if provided
    if (data.title) {
      this.modal.querySelector(".modal-title").textContent = data.title;
    }

    this.modal.classList.add("open");
    document.body.classList.add("modal-open"); // prevent scroll
    this.modal.setAttribute("aria-hidden", "false");

    // Focus first focusable element (accessibility)
    const firstFocusable = this.content.querySelector(
      "button, input, select, textarea, [tabindex]:not([tabindex='-1'])"
    );
    firstFocusable?.focus();

    return this; // chainable
  }

  close() {
    this.modal.classList.remove("open");
    document.body.classList.remove("modal-open");
    this.modal.setAttribute("aria-hidden", "true");
    return this;
  }

  bindEvents() {
    // Close on backdrop click
    this.backdrop.addEventListener("click", () => this.close());

    // Close on close button
    this.closeButtons.forEach(btn => {
      btn.addEventListener("click", () => this.close());
    });

    // Close on Escape
    document.addEventListener("keydown", (e) => {
      if (e.key === "Escape" && this.modal.classList.contains("open")) {
        this.close();
      }
    });

    // Trap focus inside modal (accessibility)
    this.modal.addEventListener("keydown", (e) => {
      if (e.key !== "Tab") return;
      const focusable = [...this.content.querySelectorAll(
        "button, input, select, textarea, a[href], [tabindex]:not([tabindex='-1'])"
      )];
      const first = focusable[0];
      const last = focusable[focusable.length - 1];

      if (e.shiftKey && document.activeElement === first) {
        e.preventDefault();
        last.focus();
      } else if (!e.shiftKey && document.activeElement === last) {
        e.preventDefault();
        first.focus();
      }
    });
  }
}

// Usage
const deleteModal = new Modal("delete-confirm-modal");
document.querySelectorAll(".delete-btn").forEach(btn => {
  btn.addEventListener("click", () => {
    deleteModal.open({ title: `Delete "${btn.dataset.name}"?` });
  });
});
```

### Project 3: Toast Notification System

```js
class ToastManager {
  constructor() {
    this.container = this.createContainer();
  }

  createContainer() {
    const container = document.createElement("div");
    container.id = "toast-container";
    container.setAttribute("aria-live", "polite");
    document.body.appendChild(container);
    return container;
  }

  show(message, type = "info", duration = 4000) {
    const toast = document.createElement("div");
    toast.className = `toast toast-${type}`;
    toast.setAttribute("role", "alert");

    const icons = { success: "✓", error: "✕", warning: "⚠", info: "ℹ" };
    toast.innerHTML = `
      <span class="toast-icon">${icons[type] ?? icons.info}</span>
      <span class="toast-message"></span>
      <button class="toast-close" aria-label="Dismiss">✕</button>
    `;
    toast.querySelector(".toast-message").textContent = message; // XSS safe

    this.container.appendChild(toast);

    // Animate in
    requestAnimationFrame(() => toast.classList.add("visible"));

    // Auto dismiss
    const timeout = setTimeout(() => this.dismiss(toast), duration);

    // Manual dismiss
    toast.querySelector(".toast-close").addEventListener("click", () => {
      clearTimeout(timeout);
      this.dismiss(toast);
    });
  }

  dismiss(toast) {
    toast.classList.remove("visible");
    toast.addEventListener("transitionend", () => toast.remove(), { once: true });
  }

  success(msg) { this.show(msg, "success"); }
  error(msg)   { this.show(msg, "error", 6000); }
  warning(msg) { this.show(msg, "warning"); }
  info(msg)    { this.show(msg, "info"); }
}

const toast = new ToastManager();

// Usage anywhere in your app
toast.success("User created successfully!");
toast.error("Failed to save. Please try again.");
```

### Project 4: Drag-and-Drop List

```js
function initDragAndDrop(listSelector) {
  const list = document.querySelector(listSelector);
  let dragging = null;

  list.addEventListener("dragstart", (e) => {
    dragging = e.target.closest("[draggable]");
    dragging.classList.add("dragging");
    e.dataTransfer.effectAllowed = "move";
  });

  list.addEventListener("dragend", () => {
    dragging?.classList.remove("dragging");
    document.querySelectorAll(".drag-over").forEach(el => {
      el.classList.remove("drag-over");
    });
    dragging = null;
  });

  list.addEventListener("dragover", (e) => {
    e.preventDefault(); // REQUIRED to allow drop
    const afterEl = getDragAfterElement(list, e.clientY);
    const overEl = e.target.closest("[draggable]");

    list.querySelectorAll(".drag-over").forEach(el => el.classList.remove("drag-over"));
    if (overEl && overEl !== dragging) overEl.classList.add("drag-over");

    if (!afterEl) {
      list.appendChild(dragging);
    } else if (afterEl !== dragging) {
      list.insertBefore(dragging, afterEl);
    }
  });

  function getDragAfterElement(container, y) {
    const siblings = [...container.querySelectorAll("[draggable]:not(.dragging)")];
    return siblings.reduce((closest, child) => {
      const box = child.getBoundingClientRect();
      const offset = y - box.top - box.height / 2;
      if (offset < 0 && offset > closest.offset) {
        return { offset, element: child };
      }
      return closest;
    }, { offset: Number.NEGATIVE_INFINITY }).element;
  }
}

initDragAndDrop("#task-list");
```

---

## Summary Cheat Sheet

```
Access:       getElementById, querySelector, querySelectorAll
              .closest() — walks up, .contains() — check descendant

Content:      textContent — safe plain text (use for user data)
              innerHTML — renders HTML (NEVER use with user data unescaped)
              insertAdjacentHTML — insert without replacing

Classes:      classList.add/remove/toggle/contains/replace

Attributes:   getAttribute/setAttribute/removeAttribute/hasAttribute
              dataset — for data-* attributes (always strings, convert types)

Styles:       element.style.propName — inline styles
              getComputedStyle(el) — actual applied style
              CSS vars via setProperty/getPropertyValue

Create:       createElement + append/prepend/insertBefore
              DocumentFragment for batch insertions (performance)

Events:       addEventListener(type, fn, options)
              event.target — clicked element
              event.currentTarget — listener element
              preventDefault() — stop browser default
              stopPropagation() — stop bubbling
              Delegation: listen on parent, check event.target.closest()

Keyboard:     keydown/keyup, event.key, event.code, modifier keys

Forms:        new FormData(form), Object.fromEntries(formData)
              input event — every keystroke; change — after blur
```

---

_Next: [Part 6 — Object-Oriented Programming](./part-6-oop.md)_