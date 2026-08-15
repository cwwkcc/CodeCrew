## What Responsive Design Is

A responsive website adapts its layout and appearance to fit the screen it is viewed on — a phone, a tablet, a laptop, or a wide desktop monitor. The goal is that the same HTML works everywhere with only CSS adjustments.

---

## The Viewport Meta Tag (Required)

Before any CSS, make sure the HTML has this in `<head>`. Without it, mobile browsers zoom out and render the page at ~980px:

```html
<meta name="viewport" content="width=device-width, initial-scale=1.0" />
```

---

## Media Queries

A media query applies a block of CSS only when a condition is true:

```css
/* Basic syntax */
@media (condition) {
  /* CSS applied only when condition is met */
}

/* Minimum width — styles apply from this width upward */
@media (min-width: 768px) {
  .container {
    display: grid;
    grid-template-columns: 1fr 1fr;
  }
}

/* Maximum width — styles apply up to this width */
@media (max-width: 767px) {
  nav {
    flex-direction: column;
  }
}

/* Range (modern syntax) */
@media (768px <= width <= 1200px) {
  /* tablet-specific styles */
}

/* Combining conditions */
@media (min-width: 768px) and (max-width: 1199px) {
  /* only for tablets */
}

/* Screen vs print */
@media print {
  .no-print {
    display: none;
  }
}

/* Dark mode */
@media (prefers-color-scheme: dark) {
  body {
    background: #111;
    color: #f0f0f0;
  }
}

/* Reduced motion (accessibility) */
@media (prefers-reduced-motion: reduce) {
  * {
    animation: none !important;
    transition: none !important;
  }
}
```

---

## Mobile-First vs Desktop-First

**Mobile-first:** Write your base CSS for small screens. Add media queries with `min-width` to add complexity for larger screens. This is the recommended approach.

```css
/* Base styles — mobile */
.grid {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

/* Tablet and up */
@media (min-width: 768px) {
  .grid {
    flex-direction: row;
  }
}

/* Desktop and up */
@media (min-width: 1200px) {
  .grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
  }
}
```

**Desktop-first:** Write base CSS for large screens, use `max-width` to simplify for smaller screens. Leads to more overriding and is generally harder to maintain.

---

## Common Breakpoints

These are conventions, not rules. Choose breakpoints based on your content, not device sizes:

| Name    | Width          | Typical use                           |
| ------- | -------------- | ------------------------------------- |
| Mobile  | < 640px        | Single column, stacked layout         |
| Tablet  | 640px – 1023px | Two columns, simplified nav           |
| Desktop | ≥ 1024px       | Multi-column, full navigation         |
| Wide    | ≥ 1280px       | Constrain max-width, add side margins |

---

## Fluid Layout Techniques

Beyond breakpoints, make elements naturally fluid:

```css
/* Constrained container that centres itself */
.container {
  width: min(1200px, 90%);
  margin-inline: auto;
}

/* Fluid typography */
h1 {
  font-size: clamp(1.75rem, 5vw, 3.5rem);
}

/* Responsive grid without media queries */
.card-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 1.5rem;
}
```

---

---

## Common Mistakes

```css
/* WRONG: desktop-first — write the complex layout, then override it
   downward for mobile; the mobile styles end up fighting the desktop ones */
.card {
  display: flex;
  width: 33%;
}
@media (max-width: 768px) {
  .card {
    display: block;
    width: 100%;
  }
}

/* CORRECT: mobile-first — the simple, single-column layout is the
   default; complexity is added as screen space becomes available */
.card {
  display: block;
  width: 100%;
}
@media (min-width: 768px) {
  .card {
    display: flex;
    width: 33%;
  }
}
```

- **Choosing breakpoints based on specific devices** (`@media (max-width: 375px)` for "iPhone SE") **instead of where your own content breaks.** Device sizes change every year; resize the browser window yourself and add a breakpoint exactly where the layout starts to look cramped or awkward, regardless of what device that corresponds to.
- **Only testing by resizing a desktop browser window.** Desktop resize simulates viewport width but not touch targets, real font rendering, or actual mobile Safari/Chrome quirks (see Module 05's `100vh` note). Use browser DevTools' device emulation _and_ a real phone before calling a layout "responsive."
