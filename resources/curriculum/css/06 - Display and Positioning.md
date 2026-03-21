## The `display` Property

`display` controls how an element participates in layout — both how it sits among its siblings, and how its children are arranged inside it.

### Outer Display Values

|Value|Behaviour|
|---|---|
|`block`|Takes full width, stacks vertically|
|`inline`|Flows with text, width/height ignored|
|`inline-block`|Flows with text, but respects width/height and all margins|
|`none`|Removed from layout entirely (not just invisible — takes no space)|

```css
span    { display: block; }         /* make inline elements stack */
div     { display: inline; }        /* make block elements flow in text */
button  { display: inline-block; }  /* flow inline but control size */
.hidden { display: none; }          /* remove from page */
```

### Inner Display Values

These turn the element into a container with a special layout algorithm for children:

|Value|What children do|
|---|---|
|`flex`|Flexible one-dimensional layout (Module 07)|
|`grid`|Two-dimensional grid layout (Module 08)|
|`flow-root`|Creates a new block formatting context (clears floats)|

### `visibility: hidden` vs `display: none`

```css
.invisible { visibility: hidden; } /* hidden but still takes up space */
.removed   { display: none; }      /* hidden and takes no space */
```

---

## Normal Flow

By default, elements follow the document's **normal flow**:

- **Block elements** stack vertically, each on its own line
- **Inline elements** flow horizontally left to right, wrapping when the line is full

The `position` property moves elements out of normal flow (partially or entirely).

---

## The `position` Property

### `position: static` (default)

The element is in normal flow. `top`, `right`, `bottom`, `left`, and `z-index` have no effect.

### `position: relative`

The element stays in normal flow but can be shifted with `top/right/bottom/left` relative to where it would have been. The space it originally occupied is preserved.

```css
.shifted {
  position: relative;
  top: 10px;    /* move down 10px from its normal position */
  left: 20px;   /* move right 20px from its normal position */
}
```

Most importantly, `position: relative` makes an element the **containing block** for absolutely positioned descendants.

### `position: absolute`

Removed from normal flow entirely — other elements act as if it does not exist. It is positioned relative to its nearest **positioned ancestor** (any ancestor with `position` other than `static`). If none exists, it is positioned relative to the `<html>` element.

```css
.parent {
  position: relative;  /* makes this the containing block */
}

.badge {
  position: absolute;
  top: 8px;
  right: 8px;
  /* positioned 8px from top-right corner of .parent */
}
```

### `position: fixed`

Removed from normal flow. Positioned relative to the **viewport** — stays in the same place even when the page scrolls. Used for sticky headers, floating buttons, modals.

```css
.navbar {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
}
```

### `position: sticky`

A hybrid: behaves like `relative` until it hits a scroll threshold, then behaves like `fixed`. The most common use case is table headers and navigation that stick when scrolled past.

```css
th {
  position: sticky;
  top: 0;  /* sticks when it reaches the top of the scroll container */
  background: white;
}
```

---

## `top`, `right`, `bottom`, `left`

These properties only work on positioned elements (anything except `static`). They set the offset from the corresponding edge of the containing block:

```css
.overlay {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  /* equivalent to: width: 100%; height: 100%; */
  /* covers the entire parent */
}
```

---

## `z-index`

Controls stacking order of positioned elements. Higher value = on top:

```css
.modal   { position: fixed; z-index: 1000; }
.tooltip { position: absolute; z-index: 100; }
.navbar  { position: sticky; z-index: 50; }
```

`z-index` only works on positioned elements. Default is `auto` (stacks in document order).

---

## `overflow`

Controls what happens when content is too large for its container:

```css
.box { overflow: visible; }  /* default — content spills out */
.box { overflow: hidden; }   /* content is clipped */
.box { overflow: scroll; }   /* always show scrollbars */
.box { overflow: auto; }     /* only show scrollbars when needed */

/* Control axes independently */
.box { overflow-x: hidden; overflow-y: auto; }
```

---

## Float (Legacy — Know It, Avoid It for Layout)

`float` was the original CSS layout tool, now replaced by Flexbox and Grid. You will encounter it in older codebases:

```css
img { float: left; }   /* text wraps around the image */
img { float: right; }
div { clear: both; }   /* next element does not wrap around floated elements */
```

Do not use `float` for layout. Use it only for its original purpose — wrapping text around an image or figure.