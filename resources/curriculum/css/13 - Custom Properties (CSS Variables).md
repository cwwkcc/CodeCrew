Custom properties (commonly called CSS variables) store values you reuse across your stylesheet. Change one variable and every place that uses it updates. They cascade, inherit through the DOM, and can be scoped to components — unlike preprocessor variables which are compiled away before reaching the browser.

---

## Declaring and Using

Custom properties start with `--` and are declared inside a selector:

```css
:root {
  --color-primary:  hsl(220, 90%, 55%);
  --color-text:     #1a1a2e;
  --color-bg:       #f8f9fa;
  --spacing-sm:     0.5rem;
  --spacing-md:     1rem;
  --spacing-lg:     2rem;
  --border-radius:  8px;
  --shadow-md:      0 4px 12px rgba(0, 0, 0, 0.1);
}
```

`:root` targets the `<html>` element — declaring variables there makes them globally available. Read them with `var()`:

```css
.button {
  background:    var(--color-primary);
  padding:       var(--spacing-sm) var(--spacing-md);
  border-radius: var(--border-radius);
  box-shadow:    var(--shadow-md);
}

p {
  color: var(--color-text);
}
```

---

## Fallback Values

`var()` accepts a fallback as the second argument, used when the variable is not defined:

```css
color:     var(--color-accent, navy);      /* navy if --color-accent not set */
padding:   var(--spacing-custom, 1rem);
font-size: var(--size, var(--base-size, 1rem));  /* fallback chain */
```

---

## Scope and Inheritance

Custom properties cascade like any other CSS property. A declaration on a descendant overrides the ancestor's value for that element and all its children:

```css
:root {
  --color-primary: royalblue;
}

.danger-zone {
  --color-primary: crimson;   /* override for this element and its descendants */
}

.button {
  background: var(--color-primary);
  /* royalblue everywhere */
  /* crimson inside .danger-zone */
}
```

This makes component-level theming natural without needing extra class names on every element.

---

## Dark Mode

```css
:root {
  --bg:         #ffffff;
  --bg-surface: #f8f9fa;
  --text:       #1a1a2e;
  --text-muted: #6b7280;
  --border:     #e2e8f0;
  --primary:    hsl(220, 90%, 55%);
}

@media (prefers-color-scheme: dark) {
  :root {
    --bg:         #0f172a;
    --bg-surface: #1e293b;
    --text:       #f1f5f9;
    --text-muted: #94a3b8;
    --border:     #334155;
    --primary:    hsl(220, 90%, 65%);
  }
}

body {
  background: var(--bg);
  color:      var(--text);
}

.card {
  background: var(--bg-surface);
  border:     1px solid var(--border);
}
```

Swapping an entire theme is just redefining the variables — all the component styles update automatically.

### Class-Based Theme Toggle

Instead of relying on the OS preference, toggle a class on `<html>` for manual dark/light switching:

```css
:root      { --bg: #fff;    --text: #111; }
:root.dark { --bg: #0f172a; --text: #f1f5f9; }
```

---

## Using Custom Properties in `calc()`

```css
:root {
  --spacing-unit: 4px;
  --columns: 12;
}

.grid-item {
  padding: calc(var(--spacing-unit) * 4);           /* 16px */
  width:   calc(100% / var(--columns) * 3);         /* 3-column span */
  margin:  calc(var(--spacing-unit) * 6 + 2px);
}
```

---

## `@property` — Typed Custom Properties

`@property` registers a custom property with a type. This enables things that are otherwise impossible — like transitioning a colour stored in a variable:

```css
/* Without @property — variables cannot be transitioned */
:root { --accent: royalblue; }
.btn  { background: var(--accent); transition: --accent 0.3s; }  /* does NOT work */

/* With @property — CSS knows it is a <color> and can interpolate it */
@property --accent {
  syntax:        '<color>';
  inherits:      false;
  initial-value: royalblue;
}

.btn {
  background: var(--accent);
  transition: --accent 0.3s ease;
}
.btn:hover {
  --accent: crimson;   /* now transitions smoothly */
}
```

```css
/* Animating a gradient angle */
@property --angle {
  syntax:        '<angle>';
  inherits:      false;
  initial-value: 0deg;
}

@keyframes rotate {
  to { --angle: 360deg; }
}

.gradient-card {
  background: linear-gradient(var(--angle), royalblue, crimson);
  animation:  rotate 4s linear infinite;
}
```

---

## Component Token Pattern

Define a component's variables with defaults, then override them per variant:

```css
.button {
  --btn-bg:     var(--color-primary);
  --btn-text:   white;
  --btn-radius: var(--border-radius);
  --btn-pad:    var(--spacing-sm) var(--spacing-md);

  background:    var(--btn-bg);
  color:         var(--btn-text);
  border-radius: var(--btn-radius);
  padding:       var(--btn-pad);
}

/* Variant just swaps the token */
.button--danger  { --btn-bg: var(--color-danger); }
.button--success { --btn-bg: var(--color-success); }
.button--large   { --btn-pad: var(--spacing-md) var(--spacing-lg); }
```

---

## Naming Conventions

```css
/* Scale-based — for spacing */
--spacing-1: 0.25rem;
--spacing-2: 0.5rem;
--spacing-4: 1rem;
--spacing-8: 2rem;

/* Semantic — for colour roles */
--color-primary:    hsl(220, 90%, 55%);
--color-success:    hsl(142, 71%, 45%);
--color-warning:    hsl(38,  92%, 50%);
--color-danger:     hsl(0,   84%, 60%);
--color-text:       hsl(222, 47%, 11%);
--color-text-muted: hsl(215, 16%, 47%);
--color-bg:         hsl(0,   0%,  100%);
--color-border:     hsl(214, 32%, 91%);
```

---

## Common Mistakes

```css
/* WRONG — custom properties are case-sensitive */
:root { --ColorPrimary: blue; }
.btn  { color: var(--colorprimary); }    /* undefined — different casing */

/* WRONG — cannot use a variable as a property name */
:root { --prop: color; }
.text { var(--prop): red; }              /* invalid syntax */

/* WRONG — space before the comma in var() */
color: var(--color-primary ,red);        /* invalid */
color: var(--color-primary, red);        /* correct */

/* WRONG — quotes around a non-string fallback */
line-height: var(--lh, "1.5");           /* invalid — remove quotes */
line-height: var(--lh, 1.5);            /* correct */
```

---

