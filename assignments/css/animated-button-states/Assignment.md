## Assignment Overview

Build a small set of button variants with real interactive states — hover, focus, active, and disabled — using transitions, transforms, and pseudo-classes properly. This is the assignment where CSS stops being static and starts responding to the user.

---

## Covers

`css/10 - Transitions`, `css/11 - Animations`, `css/12 - Transforms`, `css/13 - Custom Properties`, `css/14 - Pseudo-classes and Pseudo-elements`

---

## The Starting HTML

```html
<div class="button-row">
  <button class="btn btn--primary">Primary</button>
  <button class="btn btn--secondary">Secondary</button>
  <button class="btn btn--danger">Delete</button>
  <button class="btn btn--primary" disabled>Disabled</button>
</div>

<div class="loading-row">
  <div class="spinner"></div>
  <span>Loading your dashboard...</span>
</div>
```

---

## Your Task

### Part 1 — Button States

Style `.btn` (shared base styles) and its three variants so that:

- **Default state**: padding, rounded corners, no default browser button border/background
- **`:hover`**: the background colour shifts to a slightly darker shade, and the button lifts very slightly (a small `translateY` — a couple of pixels is enough) — **this must animate smoothly**, not snap instantly (Module 10)
- **`:focus-visible`**: a clearly visible outline or ring — **do not remove focus outlines without replacing them with something equally visible** (this is a real accessibility requirement, not a style suggestion — a keyboard user needs to see which button is focused)
- **`:active`** (while actually being clicked): the lift from `:hover` should reverse — the button should appear to press down (`translateY` in the opposite direction, or a `scale` slightly below 1)
- **`:disabled`**: reduced opacity, and the cursor should show as `not-allowed` — the disabled button should visibly look non-interactive, not just behave that way

### Part 2 — Colour via Custom Properties

- Define your three variant colours (`--color-primary`, `--color-secondary`, `--color-danger`) once, on `:root`
- Every button variant's background, hover shade, and border should reference these variables with `var()` — **no variant should have its colour hardcoded directly in the `.btn--*` rule**
- The hover shade for each variant should be a **darker version** of its base colour — you can either define a second `--color-primary-hover` variable per colour, or use `color-mix()` to darken the base colour on the fly (either approach is acceptable)

### Part 3 — A Loading Spinner

- Build `.spinner` as a small circle that **rotates continuously** using `@keyframes` and `animation` — a classic loading-spinner effect
- The spinner should **not** rotate at a jarring speed — somewhere around 0.8–1.2 seconds per full rotation looks right
- Respect motion preferences: wrap the spinning animation so it's disabled (or replaced with something calmer, like a simple opacity pulse) when the user has `prefers-reduced-motion` set (Module 11's Common Mistakes covers exactly why this matters)

---

## Constraints

- No JavaScript — every state above (`:hover`, `:focus-visible`, `:active`, `:disabled`) is a CSS pseudo-class doing real work, not a class toggled by a script.
- `transition` must name specific properties (`transform`, `background-color`, etc.) — not `transition: all` (Module 10's Common Mistakes explains the performance reasoning).
- The lift/press effect must use `transform`, not `top`/`margin-top` — for the same performance reason.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — `:focus-visible` (not plain `:focus`) shows the ring only for keyboard navigation, not every mouse click — this is the modern, correct choice for exactly this kind of button.

> 💡 **Hint 2** — The rotating spinner needs a `@keyframes` rule defining `from { transform: rotate(0deg); }` and `to { transform: rotate(360deg); }`, then an `animation` property on `.spinner` referencing it with `infinite linear`.

> 💡 **Hint 3** — For the reduced-motion spinner, wrap the `animation` declaration in `@media (prefers-reduced-motion: no-preference) { }` — outside that block, give `.spinner` a sensible static or gently-pulsing fallback instead of nothing at all.

---

## Submission Guidelines

1. `index.html`
2. `style.css`
3. A short note (as a comment at the top of your CSS) listing your three chosen colours and confirming you tested the disabled button and the reduced-motion spinner behaviour

---

## Bonus Challenge 🏆

- ✨ Add a subtle `::before` pseudo-element ripple or shine effect that appears only on hover.
- 🌗 Make the whole button set respond correctly if `.dark-theme` is added to a parent element, by redefining the custom properties inside that scope (Module 13's Common Mistakes covers exactly this scoping pattern).
- ⏱️ Add a second spinner variant using `animation-direction: alternate` for a back-and-forth pulsing dot instead of a full rotation.

---

## Learning Objectives

- ✓ Use `transition` correctly and performantly (`transform`/`opacity`, named properties, not `all`).
- ✓ Build a real, infinite `@keyframes` animation and respect `prefers-reduced-motion`.
- ✓ Centralize a colour palette in custom properties instead of repeating hex values.
- ✓ Style every real interactive state a button can be in — not just `:hover`.

**Good Luck! 🚀**
