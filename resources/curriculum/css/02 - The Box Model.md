Every element in CSS is a rectangular box. The box model defines what makes up that rectangle and how its dimensions are calculated. Understanding the box model is the single most important concept in CSS layout.

---

## The Four Layers

```
┌──────────────────────────────────┐
│            MARGIN                │  ← space outside the border (transparent)
│  ┌────────────────────────────┐  │
│  │          BORDER            │  │  ← the visible edge
│  │  ┌──────────────────────┐  │  │
│  │  │       PADDING        │  │  │  ← space inside the border
│  │  │  ┌────────────────┐  │  │  │
│  │  │  │    CONTENT     │  │  │  │  ← text, images, child elements
│  │  │  └────────────────┘  │  │  │
│  │  └──────────────────────┘  │  │
│  └────────────────────────────┘  │
└──────────────────────────────────┘
```

- **Content** — the actual content of the element (text, image, etc.)
- **Padding** — space between the content and the border. Same background colour as the content.
- **Border** — a line around the padding. Can have width, style, and colour.
- **Margin** — space outside the border. Always transparent.

---

## `box-sizing`

This is the most important CSS property to understand before anything else.

By default, `width` and `height` set the size of the **content area only**. Padding and border are added on top, making the element larger than you specified. This is `box-sizing: content-box`.

```css
.box {
  width: 200px;
  padding: 20px;
  border: 5px solid black;
  /* actual rendered width = 200 + 20 + 20 + 5 + 5 = 250px */
}
```

`box-sizing: border-box` changes this so that `width` and `height` include padding and border. The content area shrinks to accommodate them. This is what almost every developer uses:

```css
.box {
  box-sizing: border-box;
  width: 200px;
  padding: 20px;
  border: 5px solid black;
  /* actual rendered width = 200px — padding and border are included */
}
```

Apply it globally at the start of every stylesheet:

```css
*,
*::before,
*::after {
  box-sizing: border-box;
}
```

---

## Width and Height

```css
.box {
  width: 300px;
  height: 150px;
  min-width: 100px;
  max-width: 600px;
  min-height: 50px;
  max-height: 400px;
}
```

- `width` / `height` — fixed size
- `min-width` / `min-height` — never smaller than this
- `max-width` / `max-height` — never larger than this

Block elements (`<div>`, `<p>`, `<h1>`) take up the full available width by default. Inline elements (`<span>`, `<a>`, `<strong>`) are only as wide as their content.

---

## Padding

```css
/* All sides */
padding: 20px;

/* Top/bottom  Left/right */
padding: 10px 20px;

/* Top  Left/right  Bottom */
padding: 10px 20px 15px;

/* Top  Right  Bottom  Left  (clockwise from top) */
padding: 10px 20px 15px 5px;

/* Individual sides */
padding-top: 10px;
padding-right: 20px;
padding-bottom: 15px;
padding-left: 5px;
```

Padding cannot be negative.

---

## Border

```css
/* Shorthand: width style colour */
border: 2px solid navy;

/* Individual sides */
border-top: 1px dashed red;
border-bottom: 3px double green;

/* Individual properties */
border-width: 2px;
border-style: solid; /* solid, dashed, dotted, double, none */
border-color: navy;

/* Rounded corners */
border-radius: 8px; /* all corners */
border-radius: 8px 0 8px 0; /* top-left, top-right, bottom-right, bottom-left */
border-radius: 50%; /* circle (on a square element) */
```

---

## Margin

```css
/* Same shorthand rules as padding */
margin: 20px;
margin: 10px 20px;
margin: 10px auto; /* auto centres a block element horizontally */

/* Individual sides */
margin-top: 10px;
margin-right: auto;
margin-bottom: 10px;
margin-left: auto;
```

Margin can be negative — pulling an element toward its neighbour.

### Margin Collapse

When two block elements are stacked vertically, their margins merge (collapse) into a single margin equal to the **larger** of the two. This does not happen with padding.

```css
.first {
  margin-bottom: 30px;
}
.second {
  margin-top: 20px;
}
/* gap between them = 30px, not 50px */
```

Margin collapse only happens vertically (top/bottom), not horizontally (left/right). It does not happen inside flex or grid containers.

---

## Outline

Similar to border but does not take up space in the layout — it is drawn on top of surrounding elements without affecting their position:

```css
button:focus {
  outline: 2px solid blue;
  outline-offset: 4px; /* space between element edge and outline */
}
```

**Never remove `outline` on focusable elements without providing an alternative** — keyboard users rely on it to know which element is focused.

---

## The `display` Property Preview

How an element participates in layout is controlled by `display`. Covered in full in Module 06, but the two you need to know now:

| Value          | Behaviour                                                      |
| -------------- | -------------------------------------------------------------- |
| `block`        | Full width, stacks vertically, respects width/height/margin    |
| `inline`       | Flows with text, ignores width/height, horizontal margins only |
| `inline-block` | Flows with text but respects width/height and all margins      |

---

## DevTools: Your Best Friend

Every browser has developer tools (F12 or right-click → Inspect). The Elements panel shows the box model of any selected element visually — margins in orange, borders in yellow, padding in green, content in blue. You will use this constantly.

Always open DevTools when debugging layout issues.
---

## Common Mistakes

```css
/* WRONG: content-box (the default) — padding and border ADD to the
   width you set, so a 300px box with 20px padding renders at 340px */
.card {
  width: 300px;
  padding: 20px;
  border: 1px solid #ccc;
  /* actual rendered width: 300 + 20*2 + 1*2 = 342px */
}

/* CORRECT: border-box makes width the FINAL rendered width, padding
   and border are subtracted from the content area instead of added on */
*,
*::before,
*::after {
  box-sizing: border-box;
}

.card {
  width: 300px; /* this is now the true, final width — 300px, period */
  padding: 20px;
  border: 1px solid #ccc;
}
```

- **Not setting `box-sizing: border-box` globally.** This single line prevents the majority of "why is this 40px wider than I set it to" bugs. It's so close to universally correct that most modern resets include it as the very first rule.
- **Forgetting margin collapse.** Vertical margins between adjacent block siblings (and between a parent and its first/last child, under some conditions) collapse to the _larger_ of the two values, not their sum. Two stacked elements with `margin-bottom: 20px` and `margin-top: 20px` end up with 20px between them, not 40px — surprising the first time you hit it.
