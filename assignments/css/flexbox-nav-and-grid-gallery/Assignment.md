## Assignment Overview

Build a **navigation bar** with Flexbox and a **photo gallery** with Grid — two of the most common real-world layout patterns, and the two layout systems you now know. This assignment is entirely about `css/06`, `css/07`, and `css/08` — no colours or typography requirements beyond "looks reasonable."

---

## Covers

`css/06 - Display and Positioning`, `css/07 - Flexbox`, `css/08 - Grid`

---

## Part 1 — The Navigation Bar (Flexbox)

### The Starting HTML

```html
<nav class="navbar">
  <div class="navbar__logo">KITS</div>
  <ul class="navbar__links">
    <li><a href="#">Home</a></li>
    <li><a href="#">About</a></li>
    <li><a href="#">Projects</a></li>
    <li><a href="#">Contact</a></li>
  </ul>
  <button class="navbar__cta">Join Us</button>
</nav>
```

### Requirements

- The logo sits on the far left, the nav links are centred (or clustered together), and the "Join Us" button sits on the far right — **all three in a single row**, regardless of viewport width
- `<ul class="navbar__links">` should have its default list bullets and default padding removed, and its `<li>` items laid out **horizontally** with even spacing between them (not manual margins — use Flexbox's `gap`)
- The whole navbar should vertically centre all three children, even if they end up different heights
- **Constraint: you may only use Flexbox for this part — no Grid.**

---

## Part 2 — The Photo Gallery (Grid)

### The Starting HTML

```html
<div class="gallery">
  <img src="https://picsum.photos/400/300?1" alt="" />
  <img src="https://picsum.photos/400/300?2" alt="" />
  <img src="https://picsum.photos/400/300?3" alt="" />
  <img src="https://picsum.photos/400/300?4" alt="" />
  <img src="https://picsum.photos/400/300?5" alt="" />
  <img src="https://picsum.photos/400/300?6" alt="" />
  <img src="https://picsum.photos/400/300?7" alt="" />
</div>
```

### Requirements

- A **3-column grid** on wide screens, with a consistent `gap` between images
- Every image should fill its grid cell completely with no gaps or distortion (`object-fit` will matter here, alongside a fixed height per row)
- The **first image should span 2 columns and 2 rows** — a "featured" image bigger than the rest (this is exactly what `grid-column` / `grid-row` spanning is for)
- **Constraint: you may only use Grid for this part — no Flexbox.**

---

## Constraints (Both Parts)

- No JavaScript.
- No fixed pixel widths on the flex/grid _items themselves_ — let the layout systems do the sizing.
- Every image needs a real (even if placeholder) `alt=""` per the HTML accessibility rules you already know — these are decorative in this exercise, so empty `alt=""` is correct here.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — "Logo left, links centre, button right" in one row is a classic three-part Flexbox layout. Look at `justify-content: space-between` combined with wrapping the links in their own flex container if they need internal spacing too.

> 💡 **Hint 2** — Removing list bullets: `list-style: none;` plus `padding: 0;` on the `<ul>` — both are needed, since padding is what's actually creating the indent you're seeing, not the bullets themselves.

> 💡 **Hint 3** — For the featured image spanning cells, the properties are `grid-column: span 2;` and `grid-row: span 2;` on that one image specifically (you'll need an `nth-child` selector, from the HTML curriculum's CSS selectors module).

---

## Submission Guidelines

1. `index.html` — both sections in one file (or two separate files — your choice)
2. `style.css` — clearly separated into a Navbar section and a Gallery section (use comments)

---

## Bonus Challenge 🏆

- 📱 Make the navbar collapse the links into a simple stacked column below 600px width using a media query — a small preview of the next assignment.
- 🖼️ Make the gallery grid go from 3 columns down to 2, then 1, as the viewport shrinks — without touching HTML, only CSS.
- 🎯 Add a hover effect that slightly enlarges a gallery image without it overlapping/breaking the grid (hint: `transform: scale()` doesn't affect layout — from the CSS Transforms module).

---

## Learning Objectives

- ✓ Choose Flexbox for one-dimensional layouts and Grid for two-dimensional ones — deliberately, not by guessing.
- ✓ Use `justify-content`, `align-items`, and `gap` correctly for a real navigation pattern.
- ✓ Use explicit grid placement (`grid-column`/`grid-row` spanning) for a non-uniform layout.

**Good Luck! 🚀**
