CSS Grid is a two-dimensional layout system. While Flexbox handles one direction at a time, Grid handles rows and columns simultaneously. It is the right tool for overall page structure, image galleries, dashboards, and any layout where items need to align on both axes at once.

---

## Turning on Grid

Apply `display: grid` to the **parent** (the grid container). Its direct children become **grid items**:

```css
.container {
  display: grid;
}
```

By default this does nothing visible — you need to define columns and/or rows.

---

## Defining Columns and Rows

### `grid-template-columns`

```css
/* Three equal columns of 200px each */
grid-template-columns: 200px 200px 200px;

/* Two columns: one fixed, one flexible */
grid-template-columns: 250px 1fr;

/* Three equal flexible columns */
grid-template-columns: 1fr 1fr 1fr;

/* Repeat shorthand */
grid-template-columns: repeat(3, 1fr);

/* Mixed */
grid-template-columns: 250px repeat(2, 1fr);
```

The `fr` unit means "fraction of remaining space". `1fr 1fr 1fr` divides the available width into three equal parts.

### `grid-template-rows`

```css
grid-template-rows: auto 1fr auto;  /* header | content | footer */
grid-template-rows: 60px 1fr 40px;
```

### `gap`

```css
gap: 1rem;           /* same gap between all rows and columns */
gap: 1rem 2rem;      /* row-gap  column-gap */
row-gap: 1rem;
column-gap: 2rem;
```

---

## The `fr` Unit

`fr` represents a fraction of the leftover space in the grid container, after fixed sizes are taken out:

```css
grid-template-columns: 200px 1fr 1fr;
/* 200px is taken out, the remaining space is split equally between the two 1fr columns */

grid-template-columns: 1fr 2fr;
/* 2fr column is twice as wide as 1fr column */
```

---

## `repeat()` and `auto-fill` / `auto-fit`

```css
/* 4 equal columns */
grid-template-columns: repeat(4, 1fr);

/* As many 200px columns as fit — no media queries needed */
grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));

/* Same but columns stretch to fill if there are fewer than fit */
grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
```

`auto-fill` creates as many tracks as possible, even if empty. `auto-fit` collapses empty tracks. For most responsive grids, `auto-fit` is what you want.

---

## Placing Items

By default, grid items are placed in document order, filling the grid left to right, top to bottom. You can override this explicitly.

### Grid Lines

Grid lines are numbered from 1. A 3-column grid has 4 vertical lines (1, 2, 3, 4).

```css
.item {
  grid-column: 1 / 3;   /* from line 1 to line 3 — spans 2 columns */
  grid-row: 2 / 4;      /* from line 2 to line 4 — spans 2 rows */
}
```

### `span` Keyword

```css
.item {
  grid-column: 1 / span 2;  /* start at line 1, span 2 columns */
  grid-row: span 3;          /* span 3 rows, wherever the auto-placement puts it */
}

/* Full width */
.item {
  grid-column: 1 / -1;  /* -1 means the last line */
}
```

---

## Named Template Areas

A visual way to define the layout:

```css
.container {
  display: grid;
  grid-template-columns: 250px 1fr;
  grid-template-rows: 60px 1fr 40px;
  grid-template-areas:
    "header  header"
    "sidebar main"
    "footer  footer";
}

header  { grid-area: header; }
.sidebar { grid-area: sidebar; }
main    { grid-area: main; }
footer  { grid-area: footer; }
```

Each string in `grid-template-areas` represents a row. Each word names a cell. Use `.` for an empty cell. All rows must have the same number of cells.

---

## Alignment

Grid has the same alignment properties as Flexbox, but they apply to both axes.

### Container-level

```css
/* Align items along the inline axis (horizontal by default) */
justify-items: start | end | center | stretch;

/* Align items along the block axis (vertical by default) */
align-items: start | end | center | stretch;

/* Align the whole grid inside the container */
justify-content: start | end | center | space-between | space-around | space-evenly;
align-content: start | end | center | space-between | space-around | space-evenly;

/* Shorthand */
place-items: center;            /* align-items: center + justify-items: center */
place-content: center;
```

### Item-level

```css
.item {
  justify-self: end;    /* override horizontal alignment for this item */
  align-self: center;   /* override vertical alignment for this item */
  place-self: center;   /* shorthand */
}
```

---

## `minmax()`

Sets a minimum and maximum size for a track:

```css
grid-template-columns: minmax(150px, 1fr) minmax(150px, 2fr);
/* first column: at least 150px, at most 1fr */
/* second column: at least 150px, at most 2fr */
```

---

## When to Use Grid vs Flexbox

|Scenario|Use|
|---|---|
|Overall page layout|Grid|
|Items in a row or column that need to be flexible|Flexbox|
|Two-dimensional alignment (rows AND columns together)|Grid|
|A single row of items (nav, buttons)|Flexbox|
|Cards that should wrap responsively|Either — Grid with `auto-fit` is cleaner|
|Centering one element|Flexbox (`justify-content: center + align-items: center`)|

They are not competing tools — use both. A grid layout can contain flex containers inside its cells.

---

## Common Grid Patterns

### Responsive card grid (no media queries)

```css
.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 1.5rem;
}
```

### Classic page layout

```css
body {
  display: grid;
  grid-template-areas:
    "header"
    "main"
    "footer";
  grid-template-rows: auto 1fr auto;
  min-height: 100vh;
}
```

### Sidebar layout

```css
.layout {
  display: grid;
  grid-template-columns: 260px 1fr;
  grid-template-areas: "sidebar main";
  gap: 2rem;
}
```