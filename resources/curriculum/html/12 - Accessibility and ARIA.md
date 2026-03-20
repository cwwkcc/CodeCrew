## Why Accessibility

An accessible web site works for users who use: screen readers (blind/low-vision users), keyboard-only navigation (motor impairments), switch devices, voice control software, and users with cognitive disabilities who benefit from clear structure.

In many countries (and increasingly under WCAG 2.1 AA), web accessibility is a legal requirement.

---

## The Foundation: Semantic HTML

The most powerful accessibility tool is correct semantic HTML. Use:

- `<button>` for buttons (not `<div onclick>`)
- `<a href>` for links (not `<span onclick>`)
- `<nav>` for navigation
- `<main>` for the main content
- `<h1>`–`<h6>` in correct order
- `<label>` for every form input
- `<table>` only for tabular data

Semantic HTML provides accessible names, roles, and states for free.

---

## ARIA: Accessible Rich Internet Applications

ARIA attributes _supplement_ HTML semantics when native elements are insufficient. The first rule of ARIA: **don't use ARIA if a native HTML element works.**

### Roles

```html
<div role="alert">Your session will expire in 5 minutes.</div>
<div role="dialog" aria-labelledby="dialog-title" aria-modal="true">...</div>
<nav role="navigation" aria-label="Breadcrumbs">...</nav>
```

Landmark roles: `banner`, `navigation`, `main`, `complementary`, `contentinfo`, `search`, `form`, `region`.

### States and Properties

```html
<!-- Labelling -->
<button aria-label="Close dialog">×</button>
<nav aria-labelledby="nav-heading">
  <h2 id="nav-heading">Site sections</h2>
  ...
</nav>

<!-- Describing -->
<input type="password" aria-describedby="pwd-hint">
<p id="pwd-hint">Must be 8+ characters, with one uppercase letter and one number.</p>

<!-- States -->
<button aria-expanded="false" aria-controls="menu">Menu</button>
<ul id="menu" hidden>...</ul>

<input type="checkbox" aria-checked="mixed">  <!-- indeterminate state -->

<!-- Live regions (announce dynamic content to screen readers) -->
<div aria-live="polite">Loading complete. 24 results found.</div>
<div aria-live="assertive">Error: network connection lost.</div>

<!-- Hide decorative content -->
<img src="divider.svg" alt="" aria-hidden="true">
<span aria-hidden="true">→</span>

<!-- Mark required fields -->
<input type="email" required aria-required="true">

<!-- Mark invalid fields -->
<input type="email" aria-invalid="true" aria-describedby="email-error">
<p id="email-error" role="alert">Please enter a valid email address.</p>
```

---

## Focus Management

All interactive elements must be keyboard-focusable in a logical order:

```html
<!-- Add to non-interactive elements that need programmatic focus -->
<div tabindex="-1" id="main-content">...</div>

<!-- AVOID: positive tabindex values disrupt natural order -->
<button tabindex="3">Don't do this</button>
```

`tabindex="0"` adds an element to the natural tab order. `tabindex="-1"` removes from tab order but allows programmatic `.focus()`. Positive `tabindex` values are an anti-pattern — they break the expected flow.

---

## Skip Links

```html
<body>
  <a href="#main-content" class="skip-link">Skip to main content</a>
  <header>...</header>
  <nav>...</nav>
  <main id="main-content">...</main>
</body>
```

A skip link lets keyboard users jump over repeated navigation to reach the main content. Usually visually hidden until focused.

---