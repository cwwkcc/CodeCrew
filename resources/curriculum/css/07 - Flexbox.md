Flexbox is a one-dimensional layout system. "One-dimensional" means it handles layout in one direction at a time — either a row or a column. It is the go-to tool for aligning and distributing items in a line, navigation bars, card rows, centring elements, and any layout where you need items to flex to fill available space.

---

## Turning on Flexbox

Apply `display: flex` to the **parent** (the flex container). Its direct children become **flex items**:

```css
.container {
  display: flex;
}
```

```html
<div class="container">
  <!-- flex container -->
  <div>Item 1</div>
  <!-- flex item -->
  <div>Item 2</div>
  <!-- flex item -->
  <div>Item 3</div>
  <!-- flex item -->
</div>
```

Flex only affects direct children — not grandchildren.

---

## The Main Axis and Cross Axis

Flexbox has two axes:

- **Main axis** — the direction items are laid out (`row` = horizontal, `column` = vertical)
- **Cross axis** — perpendicular to the main axis

All alignment properties refer to one of these two axes. Understanding which axis you are working on is essential.

---

## Container Properties

### `flex-direction`

Sets the main axis direction:

```css
flex-direction: row; /* default — left to right */
flex-direction: row-reverse; /* right to left */
flex-direction: column; /* top to bottom */
flex-direction: column-reverse; /* bottom to top */
```

### `flex-wrap`

Controls whether items wrap to a new line when they overflow:

```css
flex-wrap: nowrap; /* default — items shrink to fit, may overflow */
flex-wrap: wrap; /* items wrap to next line */
flex-wrap: wrap-reverse;
```

### `flex-flow` Shorthand

```css
flex-flow: row wrap; /* direction + wrap */
```

### `gap`

Space between flex items (does not add space at the outer edges):

```css
gap: 1rem; /* same gap in both directions */
gap: 1rem 2rem; /* row-gap  column-gap */
```

---

## Alignment Properties (Container)

### `justify-content` — alignment along the main axis

```css
justify-content: flex-start; /* items at start (default) */
justify-content: flex-end; /* items at end */
justify-content: center; /* items in centre */
justify-content: space-between; /* first and last touch edges, equal gaps between */
justify-content: space-around; /* equal space around each item */
justify-content: space-evenly; /* equal space between all gaps including edges */
```

### `align-items` — alignment along the cross axis (for one line)

```css
align-items: stretch; /* items fill the cross axis (default) */
align-items: flex-start; /* items at start of cross axis */
align-items: flex-end; /* items at end of cross axis */
align-items: center; /* items centred on cross axis */
align-items: baseline; /* items aligned by text baseline */
```

### `align-content` — alignment of multiple lines (when `flex-wrap: wrap`)

Same values as `justify-content`, but for the cross axis when there are multiple rows:

```css
align-content: flex-start;
align-content: center;
align-content: space-between;
```

### Centering an element perfectly (the classic trick)

```css
.container {
  display: flex;
  justify-content: center; /* centre on main axis */
  align-items: center; /* centre on cross axis */
}
```

---

## Item Properties

These are applied to individual flex items, not the container.

### `flex-grow`

How much an item should grow relative to others when there is extra space:

```css
.item {
  flex-grow: 0;
} /* default — do not grow */
.item {
  flex-grow: 1;
} /* grow equally with other flex-grow: 1 items */
.wide {
  flex-grow: 2;
} /* takes twice as much extra space */
```

### `flex-shrink`

How much an item should shrink when there is not enough space:

```css
.item {
  flex-shrink: 1;
} /* default — shrink proportionally */
.fixed {
  flex-shrink: 0;
} /* do not shrink */
```

### `flex-basis`

The initial size of the item before growing or shrinking:

```css
.item {
  flex-basis: auto;
} /* default — use the item's content width */
.item {
  flex-basis: 200px;
}
.item {
  flex-basis: 30%;
}
.item {
  flex-basis: 0;
} /* all extra space is distributed by flex-grow */
```

### `flex` Shorthand

```css
/* grow  shrink  basis */
.item {
  flex: 1 1 auto;
} /* default */
.item {
  flex: 1;
} /* shorthand for flex: 1 1 0 — grow and shrink equally */
.item {
  flex: 0 0 200px;
} /* fixed 200px, no grow, no shrink */
.item {
  flex: auto;
} /* flex: 1 1 auto */
.item {
  flex: none;
} /* flex: 0 0 auto — rigid */
```

`flex: 1` on all items gives them equal width, sharing available space.

### `align-self`

Override `align-items` for a single item:

```css
.item {
  align-self: flex-end;
}
.item {
  align-self: center;
}
```

### `order`

Control the visual order without changing the HTML order (affects accessibility — use with caution):

```css
.first {
  order: -1;
} /* appears before items with order: 0 */
.last {
  order: 1;
}
```

---

## Common Patterns

### Navigation bar

```css
nav {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 1rem;
}
```

### Equal-width columns

```css
.container {
  display: flex;
  gap: 1rem;
}
.column {
  flex: 1;
} /* each column takes equal share */
```

### Sticky footer (push footer to bottom of page)

```css
body {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
}

main {
  flex: 1;
} /* main grows to fill available space */
```

### Card with content pushed to bottom

```css
.card {
  display: flex;
  flex-direction: column;
}

.card-footer {
  margin-top: auto;
}
```

---

## Common Mistakes

```css
/* WRONG: text inside a flex child doesn't truncate with an ellipsis —
   flex items default to a min-width based on their content, which
   silently overrides overflow/text-overflow */
.flex-item {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

/* CORRECT: min-width: 0 lets the item actually shrink below its content size */
.flex-item {
  min-width: 0;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
```

- **Mixing up `justify-content` and `align-items`.** `justify-content` controls the **main axis** (horizontal, in a default row layout); `align-items` controls the **cross axis** (vertical). Switch `flex-direction: column` and the axes swap — the same two properties now do the opposite of what they did a moment ago, which trips people up constantly.
- **Not accounting for `flex-shrink`'s default of `1`.** Flex items shrink to fit their container by default, even ones you didn't intend to shrink (like a fixed-width logo). `flex-shrink: 0` on anything that must keep its exact size is a common, easy-to-forget fix.
