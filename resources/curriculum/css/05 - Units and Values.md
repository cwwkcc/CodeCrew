CSS has many different units. Choosing the right unit is one of the most important decisions in writing maintainable CSS — it determines how your layout responds to different screen sizes, zoom levels, and user preferences.

---

## Absolute Units

Fixed sizes that never change regardless of context:

| Unit             | What it is               | Use for                        |
| ---------------- | ------------------------ | ------------------------------ |
| `px`             | Pixels (device-adjusted) | Borders, shadows, fine details |
| `pt`             | Points (1pt = 1/72 inch) | Print stylesheets only         |
| `cm`, `mm`, `in` | Physical measurements    | Print stylesheets only         |

`px` is the main absolute unit for screens. Use it for things that should have a fixed physical size — borders, outlines, box shadows. Avoid it for font sizes and layout dimensions that should scale.

---

## Relative Units

Scale relative to something else — the most important units for responsive design:

### Relative to Font Size

| Unit  | Relative to                               |
| ----- | ----------------------------------------- |
| `em`  | The element's own `font-size`             |
| `rem` | The root element's (`<html>`) `font-size` |

```css
html {
  font-size: 16px;
} /* browser default */

h1 {
  font-size: 2rem; /* 2 × 16px = 32px */
  padding: 1rem; /* 1 × 16px = 16px — always relative to root */
}

p {
  font-size: 1rem; /* 16px */
  padding: 1em; /* 1 × 16px = 16px — relative to this element */
}

.nested {
  font-size: 0.75em; /* 0.75 × parent's font-size */
  /* em compounds in nesting! rem does not. */
}
```

**Rule of thumb:**

- Use `rem` for font sizes and spacing — consistent, predictable
- Use `em` for spacing that should scale with the element's own font size (e.g. padding on a button that comes in multiple sizes)
- Never use `px` for `font-size` on body text — it ignores the user's browser preference

### Relative to Viewport

| Unit   | Relative to                                                            |
| ------ | ---------------------------------------------------------------------- |
| `vw`   | 1% of the viewport width                                               |
| `vh`   | 1% of the viewport height                                              |
| `vmin` | 1% of the smaller viewport dimension                                   |
| `vmax` | 1% of the larger viewport dimension                                    |
| `dvh`  | 1% of the dynamic viewport height (accounts for mobile browser chrome) |

```css
.hero {
  height: 100vh; /* full screen height */
  width: 100vw; /* full screen width */
}

h1 {
  font-size: clamp(
    1.5rem,
    4vw,
    3rem
  ); /* scales with viewport, but has min/max */
}
```

---

## Percentage

Relative to the parent element's corresponding dimension:

```css
.container {
  width: 80%;
} /* 80% of parent's width */
.half {
  width: 50%;
} /* 50% of parent's width */
p {
  line-height: 150%;
} /* 150% of the element's font-size */
```

---

## The `clamp()` Function

Sets a value with a minimum, preferred, and maximum:

```css
/* clamp(min, preferred, max) */
font-size: clamp(1rem, 2.5vw, 2rem);
width: clamp(300px, 50%, 700px);
```

This is the modern way to write fluid typography and layout — the value scales with the viewport between the min and max bounds.

---

## `min()` and `max()`

```css
width: min(500px, 100%); /* whichever is smaller — never overflows */
width: max(300px, 50%); /* whichever is larger — never smaller than 300px */
```

---

## `calc()`

Performs calculations mixing different units:

```css
width: calc(100% - 2rem); /* full width minus padding */
height: calc(100vh - 60px); /* full viewport minus navbar */
margin-top: calc(1rem + 5px);
font-size: calc(1rem + 0.5vw); /* mix of fixed and fluid */
```

---

## Colors as Values

Covered in Module 03 but summarized here:

| Format | Example                      |
| ------ | ---------------------------- |
| Named  | `red`, `navy`, `transparent` |
| Hex    | `#3a86ff`, `#fff`            |
| RGB    | `rgb(58, 134, 255)`          |
| RGBA   | `rgba(58, 134, 255, 0.5)`    |
| HSL    | `hsl(217, 100%, 61%)`        |
| HSLA   | `hsla(217, 100%, 61%, 0.5)`  |

---

## Keywords

CSS properties also accept keyword values that are not units:

| Keyword   | Meaning                                                     |
| --------- | ----------------------------------------------------------- |
| `auto`    | Browser calculates the value (varies by property)           |
| `inherit` | Use the parent's value                                      |
| `initial` | Use the browser's default for this property                 |
| `unset`   | `inherit` if the property is inherited, otherwise `initial` |
| `none`    | No value / disable                                          |
| `normal`  | The standard value for this property                        |

---

## Which Unit to Use — Quick Guide

| Situation                        | Unit                       |
| -------------------------------- | -------------------------- |
| Font size (body, headings)       | `rem`                      |
| Padding and margin               | `rem` or `em`              |
| Borders, shadows, outlines       | `px`                       |
| Layout widths                    | `%`, `rem`, or `fr` (Grid) |
| Viewport-filling sections        | `vh`, `vw`, or `dvh`       |
| Fluid typography                 | `clamp()` with `vw`        |
| Anything that should never scale | `px`                       |

---

## Common Mistakes

```css
/* WRONG: em compounds with nested elements — a component with
   padding in em set inside another component with a different
   font-size ends up an unpredictable, inconsistent size */
.card {
  font-size: 1.2em;
}
.card .badge {
  padding: 0.5em;
} /* 0.5 × 1.2 × (parent's font-size) — not
                                       what most people expect at a glance */

/* CORRECT: rem is always relative to the ROOT font-size, never compounds */
.card {
  font-size: 1.2rem;
}
.card .badge {
  padding: 0.5rem;
} /* always a predictable, fixed size */
```

- **Confusing `rem` and `em`.** `rem` = relative to the root (`<html>`) — predictable everywhere. `em` = relative to the _current_ element's own font-size — compounds through nested elements, which is exactly what you want for some things (icon sizing that should scale with its own text) and exactly what you don't want for others (consistent spacing).
- **Using `vh` for full-height mobile layouts.** On mobile Safari and Chrome, `100vh` includes the space the address bar _would_ occupy even when it's hidden, causing content to overflow or leave a gap as the bar shows/hides on scroll. The modern fix is `100dvh` (dynamic viewport height), which tracks the actual visible area.
- **Treating `%` widths as relative to the viewport.** A percentage width is relative to the _parent element_, not the screen — a very common source of "why is this only half the width I expected" when the parent itself isn't full-width.
