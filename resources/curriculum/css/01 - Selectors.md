A selector is the part of a CSS rule that tells the browser which elements to style. Knowing selectors well means you can target any element on the page without adding extra `class` attributes everywhere.

---

## Basic Selectors

### Element Selector

Targets every instance of an HTML element:

```css
p {
  color: navy;
} /* all <p> elements */
h1 {
  font-size: 2rem;
} /* all <h1> elements */
```

### Class Selector

Targets elements with a specific `class` attribute. Starts with `.`:

```css
.card {
  border: 1px solid #ccc;
}
.highlight {
  background: yellow;
}
```

```html
<div class="card">...</div>
<p class="highlight">Important</p>
```

An element can have multiple classes separated by spaces:

```html
<div class="card highlight large">...</div>
```

### ID Selector

Targets a single element with a specific `id`. Starts with `#`. IDs must be unique per page:

```css
#main-header {
  background: navy;
}
```

**In practice:** ID selectors have very high specificity, which makes them hard to override later. Most developers prefer class selectors for styling and reserve `id` for JavaScript and anchor links.

### Universal Selector

Targets every element on the page. Used mostly in resets:

```css
* {
  box-sizing: border-box;
}
```

---

## Attribute Selectors

Target elements based on their HTML attributes:

```css
/* Has the attribute at all */
[disabled] {
  opacity: 0.5;
}

/* Attribute equals a value */
[type="text"] {
  border: 1px solid blue;
}

/* Attribute starts with */
[href^="https"] {
  color: green;
}

/* Attribute ends with */
[href$=".pdf"] {
  color: red;
}

/* Attribute contains */
[class*="btn"] {
  cursor: pointer;
}
```

---

## Combinators

Combinators describe the relationship between selectors.

### Descendant Combinator (space)

Targets elements that are anywhere inside another element:

```css
nav a {
  color: white;
} /* any <a> anywhere inside <nav> */
```

### Child Combinator (`>`)

Targets elements that are direct children only — not grandchildren:

```css
ul > li {
  list-style: square;
} /* only <li> directly inside <ul> */
```

### Adjacent Sibling (`+`)

Targets an element immediately after another:

```css
h2 + p {
  font-size: 1.1rem;
} /* the <p> that comes right after an <h2> */
```

### General Sibling (`~`)

Targets all matching siblings that come after:

```css
h2 ~ p {
  color: grey;
} /* all <p> elements after an <h2> at the same level */
```

---

## Grouping

Apply the same styles to multiple selectors by separating them with commas:

```css
h1,
h2,
h3 {
  font-family: Georgia, serif;
}

.card,
.panel,
.box {
  border-radius: 8px;
  padding: 1rem;
}
```

---

## Pseudo-classes

A pseudo-class targets an element based on its **state** or **position**. Starts with `:`.

### User Interaction States

```css
a:hover {
  color: red;
} /* mouse is over the element */
a:focus {
  outline: 2px solid;
} /* element has keyboard focus */
a:active {
  color: darkred;
} /* element is being clicked */

input:checked {
  accent-color: green;
}
input:disabled {
  opacity: 0.4;
}
input:required {
  border-color: red;
}
input:valid {
  border-color: green;
}
input:invalid {
  border-color: red;
}
```

### Structural Pseudo-classes

```css
li:first-child {
  font-weight: bold;
} /* first sibling */
li:last-child {
  border-bottom: none;
} /* last sibling */
li:nth-child(2) {
  color: red;
} /* exactly the 2nd */
li:nth-child(odd) {
  background: #f5f5f5;
} /* 1st, 3rd, 5th... */
li:nth-child(even) {
  background: white;
} /* 2nd, 4th, 6th... */
li:nth-child(3n) {
  color: blue;
} /* every 3rd */
li:only-child {
  color: green;
} /* only child of parent */
p:not(.intro) {
  color: grey;
} /* every <p> without class intro */
```

### `:is()` and `:where()`

Shorthand for grouping selectors. `:is()` keeps specificity; `:where()` has zero specificity (useful for resets):

```css
:is(h1, h2, h3) {
  line-height: 1.2;
}

/* same as */
h1,
h2,
h3 {
  line-height: 1.2;
}
```

---

## Pseudo-elements

A pseudo-element targets a **part** of an element, or inserts content around it. Starts with `::`.

```css
p::first-line {
  font-weight: bold;
} /* first line of the paragraph */
p::first-letter {
  font-size: 2em;
} /* drop cap */

::selection {
  background: yellow;
} /* text the user has highlighted */

input::placeholder {
  color: #aaa;
} /* placeholder text in inputs */
```

### `::before` and `::after`

Insert generated content before or after an element's content. Require `content: ""`:

```css
.required::after {
  content: " *";
  color: red;
}

blockquote::before {
  content: "\201C"; /* opening quote character */
  font-size: 3em;
}
```

The inserted content is not part of the DOM — it cannot be selected, copied, or read by screen readers. Do not put meaningful text in `content`.

---

## Specificity Quick Reference

| Selector                | Score (ID, Class, Element) |
| ----------------------- | -------------------------- |
| `p`                     | 0, 0, 1                    |
| `.card`                 | 0, 1, 0                    |
| `p.card`                | 0, 1, 1                    |
| `nav > ul > li`         | 0, 0, 3                    |
| `#header`               | 1, 0, 0                    |
| `#header .nav li:hover` | 1, 1, 1                    |

When two rules conflict, the one with the higher specificity score wins. If scores are equal, the later rule in the file wins.
---

## Common Mistakes

```css
/* WRONG: ID selectors for styling — specificity so high that any later
   override needs an even higher-specificity (or !important) selector */
#card {
  border-radius: 8px;
}

/* CORRECT: class selectors are the default for styling; reserve IDs
   for JavaScript hooks and same-page anchor links */
.card {
  border-radius: 8px;
}

/* WRONG: over-qualified selectors — brittle, breaks the moment the
   markup changes from a <div> to something else */
div.card.highlighted {
  border-color: gold;
}

/* CORRECT: let the class do the work */
.card.highlighted {
  border-color: gold;
}
```

A useful habit: keep selector specificity as flat as possible across a whole project — mostly single classes, rarely IDs, almost never `!important`. A codebase where every selector has roughly the same, low specificity is one where "add one more specific rule" always works predictably, instead of turning into a guessing game about what currently outranks what.
