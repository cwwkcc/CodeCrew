Typography in CSS controls how text looks — the font, size, weight, spacing, and alignment. Good typography is the foundation of readable design.

---

## Font Family

```css
body {
  font-family: "Inter", "Helvetica Neue", Arial, sans-serif;
}

h1 {
  font-family: "Georgia", "Times New Roman", serif;
}

code {
  font-family: "Fira Code", "Consolas", "Courier New", monospace;
}
```

The value is a **font stack** — a comma-separated list of fonts tried left to right. If the first font is not available, the browser tries the next, and so on. The last value is always a generic family: `serif`, `sans-serif`, `monospace`, `cursive`, `fantasy`.

Font names with spaces must be quoted.

### Loading Web Fonts

```html
<!-- In <head> — load from Google Fonts -->
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;600;700&display=swap"
      rel="stylesheet">
```

```css
/* Or load a local font file with @font-face */
@font-face {
  font-family: "MyFont";
  src: url("/fonts/myfont.woff2") format("woff2");
  font-weight: 400;
  font-display: swap;  /* show fallback font while loading */
}
```

---

## Font Size

```css
h1 { font-size: 2.5rem; }
p  { font-size: 1rem; }    /* 1rem = root element font size, usually 16px */
small { font-size: 0.875rem; }
```

Prefer `rem` for font sizes — it scales with the user's browser font size preference, making your site more accessible. Avoid `px` for body text.

---

## Font Weight

```css
p      { font-weight: 400; }   /* normal */
strong { font-weight: 700; }   /* bold */
h1     { font-weight: 800; }   /* extra bold */

/* Named keywords */
p { font-weight: normal; }   /* 400 */
b { font-weight: bold; }     /* 700 */
```

Common weights: 100 (Thin), 300 (Light), 400 (Regular), 500 (Medium), 600 (SemiBold), 700 (Bold), 800 (ExtraBold), 900 (Black). Only weights the font file actually includes will render correctly — others are simulated.

---

## Font Style

```css
em     { font-style: italic; }
normal { font-style: normal; }
```

---

## Line Height

The vertical space between lines of text. One of the most impactful properties for readability:

```css
body { line-height: 1.6; }   /* unitless — relative to font-size. Preferred. */
h1   { line-height: 1.2; }   /* tighter for large headings */
p    { line-height: 1.7; }   /* comfortable for body text */
```

Use a unitless value (no px, no rem) — it scales proportionally with the font size of that element and its children.

---

## Letter and Word Spacing

```css
h1      { letter-spacing: 0.05em; }   /* slightly spread out */
.mono   { letter-spacing: -0.02em; }  /* slightly tighter */
p       { word-spacing: 0.1em; }
```

---

## Text Alignment

```css
p    { text-align: left; }    /* default for LTR languages */
h1   { text-align: center; }
.num { text-align: right; }   /* numbers in tables */
p    { text-align: justify; } /* spread to fill the line — use carefully */
```

---

## Text Decoration

```css
a         { text-decoration: none; }              /* remove underline */
a:hover   { text-decoration: underline; }
del       { text-decoration: line-through; }
.dotted   { text-decoration: underline dotted; }
.coloured { text-decoration: underline wavy red; }
```

---

## Text Transform

```css
h2       { text-transform: uppercase; }
.sub     { text-transform: lowercase; }
.name    { text-transform: capitalize; }  /* first letter of each word */
```

---

## Text Overflow

Controls what happens when text is too long for its container:

```css
.truncate {
  white-space: nowrap;      /* prevent line breaks */
  overflow: hidden;          /* hide overflow */
  text-overflow: ellipsis;   /* show … at the end */
}
```

---

## White Space

```css
p    { white-space: normal; }     /* default — collapses whitespace, wraps */
pre  { white-space: pre; }        /* preserves whitespace and newlines, no wrap */
code { white-space: pre-wrap; }   /* preserves whitespace and newlines, wraps */
.nowrap { white-space: nowrap; }  /* no wrapping */
```

---

## Text Shadow

```css
h1 {
  /* offset-x  offset-y  blur-radius  colour */
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
}

/* Multiple shadows */
h1 {
  text-shadow:
    1px 1px 0 #000,
    -1px -1px 0 #000;
}
```

---

## Shorthand: `font`

```css
/* style  variant  weight  size/line-height  family */
body {
  font: 400 1rem/1.6 "Inter", sans-serif;
}
```

The shorthand requires at minimum `font-size` and `font-family`. It resets all other font properties to their defaults — use it carefully.