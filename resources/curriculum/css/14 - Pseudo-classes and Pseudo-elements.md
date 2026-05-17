Pseudo-classes select elements based on their state or position in the document. Pseudo-elements create virtual elements or target specific parts of an element's content. Together they handle a huge range of UI patterns without adding extra HTML.

---

## State Pseudo-classes

```css
:hover        { }   /* pointer is over the element */
:focus        { }   /* element has keyboard focus */
:focus-within { }   /* element or any descendant has focus */
:focus-visible{ }   /* focus via keyboard only — not from mouse click */
:active       { }   /* element is being clicked/pressed */
:visited      { }   /* link that the user has already visited */
:target       { }   /* element whose id matches the current URL fragment */
```

### `:focus` vs `:focus-visible`

```css
/* BAD — shows focus ring on mouse clicks too, looks messy */
button:focus {
  outline: 2px solid royalblue;
}

/* GOOD — ring only appears for keyboard navigation */
button:focus-visible {
  outline: 2px solid royalblue;
  outline-offset: 2px;
}

/* Remove default ring, provide your own via :focus-visible */
button:focus:not(:focus-visible) {
  outline: none;
}
```

### `:focus-within`

Styles a container when any element inside it has focus — great for form styling:

```css
.form-field {
  border: 1px solid #ccc;
  transition: border-color 0.15s ease;
}

/* Highlight the whole field wrapper when its input is focused */
.form-field:focus-within {
  border-color: royalblue;
  box-shadow: 0 0 0 3px hsl(220 90% 55% / 0.2);
}
```

### `:target`

Matches an element when its `id` equals the URL fragment (`#section-name`):

```css
/* Highlight a section when jumped to via anchor link */
:target {
  scroll-margin-top: 5rem;
  background: hsl(220 90% 55% / 0.08);
  border-left: 3px solid royalblue;
}
```

---

## Form State Pseudo-classes

```css
:checked          { }  /* checked checkbox or radio */
:disabled         { }  /* element has the disabled attribute */
:enabled          { }  /* element does NOT have disabled */
:required         { }  /* input with required attribute */
:optional         { }  /* input without required */
:valid            { }  /* input currently passing validation */
:invalid          { }  /* input currently failing validation */
:in-range         { }  /* number/range input within min/max */
:out-of-range     { }  /* number/range input outside min/max */
:placeholder-shown{ }  /* input currently showing its placeholder */
:read-only        { }  /* input with readonly attribute */
:read-write       { }  /* input without readonly (editable) */
:indeterminate    { }  /* checkbox in indeterminate state */
```

```css
/* Style valid/invalid only after user interaction — not on first load */
input:not(:placeholder-shown):valid {
  border-color: hsl(142, 71%, 45%);
}

input:not(:placeholder-shown):invalid {
  border-color: hsl(0, 84%, 60%);
}

/* Dim disabled inputs */
input:disabled,
button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

/* Custom checkbox */
input[type="checkbox"]:checked + label {
  font-weight: 700;
  color: royalblue;
}
```

---

## Structural Pseudo-classes

### Child-based

```css
:first-child          { }  /* first child among all siblings */
:last-child           { }  /* last child */
:nth-child(n)         { }  /* nth child (1-indexed) */
:nth-last-child(n)    { }  /* nth from the end */
:only-child           { }  /* the only child of its parent */
```

### Type-based

```css
:first-of-type        { }  /* first sibling of this tag type */
:last-of-type         { }  /* last sibling of this tag type */
:nth-of-type(n)       { }  /* nth sibling of this type */
:nth-last-of-type(n)  { }  /* nth from the end of this type */
:only-of-type         { }  /* only sibling of this type */
```

### `:nth-child()` Formula — `An + B`

```css
:nth-child(2)    { }   /* only the 2nd item */
:nth-child(odd)  { }   /* 1, 3, 5, 7... */
:nth-child(even) { }   /* 2, 4, 6, 8... */
:nth-child(3n)   { }   /* every 3rd: 3, 6, 9, 12... */
:nth-child(3n+1) { }   /* 1, 4, 7, 10... */
:nth-child(n+4)  { }   /* 4th item and all after it */
:nth-child(-n+3) { }   /* only the first 3 items */
```

```css
/* Zebra-stripe a table */
tr:nth-child(even) {
  background: hsl(220, 20%, 97%);
}

/* Remove bottom border from the last item in a list */
li:last-child {
  border-bottom: none;
}

/* Style every 3rd card differently */
.card:nth-child(3n) {
  background: hsl(220, 90%, 97%);
}
```

### `:empty`

```css
/* Hide empty elements entirely */
p:empty { display: none; }

/* Show a placeholder message when a list is empty */
ul:empty::before {
  content: "No items yet.";
  color: #999;
}
```

---

## Logical Pseudo-classes

### `:not()` — Exclusion

```css
/* Everything except .active */
li:not(.active) { opacity: 0.6; }

/* Multiple exclusions */
li:not(.active):not(:last-child) { border-bottom: 1px solid #eee; }

/* Modern — comma-separated list */
li:not(.active, .disabled) { opacity: 0.6; }
```

### `:is()` — Grouping

Matches any selector in the list. Uses the **highest specificity** of any item in the list:

```css
/* Instead of writing this: */
h1 a, h2 a, h3 a, h4 a { color: inherit; }

/* Write this: */
:is(h1, h2, h3, h4) a { color: inherit; }

/* Complex selectors */
:is(article, section, aside) :is(h2, h3) {
  font-size: 1.25rem;
}
```

### `:where()` — Grouping with Zero Specificity

Same as `:is()` but contributes **zero specificity** — easy to override:

```css
/* Good for resets and base styles that should never win specificity battles */
:where(h1, h2, h3, h4, h5, h6) {
  font-weight: 700;
  line-height: 1.2;
}
```

### `:has()` — Parent Selector

Style an element based on what it **contains**. This was the most requested CSS feature for years:

```css
/* Card that contains an image — remove padding */
.card:has(img) {
  padding: 0;
}

/* Form that contains an invalid input — red border */
form:has(:invalid) {
  border-color: crimson;
}

/* List item when its link is hovered */
li:has(a:hover) {
  background: hsl(220, 90%, 97%);
}

/* Section that does NOT have an h2 */
section:not(:has(h2)) {
  padding-top: 0;
}

/* Figure that contains a video — wider layout */
figure:has(video) {
  max-width: 100%;
}
```

---

## Pseudo-elements

Pseudo-elements target a specific part of an element or insert virtual content. They use `::` (two colons):

```css
::before        /* virtual element before the content */
::after         /* virtual element after the content */
::first-line    /* first rendered line of a block */
::first-letter  /* first character */
::selection     /* text selected by the user */
::placeholder   /* placeholder text in inputs */
::marker        /* list item bullet or number */
::backdrop      /* full-page overlay behind a <dialog> */
```

### `::before` and `::after`

These create virtual child elements at the start or end of an element's content. They require a `content` property — even if empty:

```css
/* Decorative quote marks */
blockquote::before { content: '"'; font-size: 4em; color: #ccc; }
blockquote::after  { content: '"'; font-size: 4em; color: #ccc; }

/* Required field asterisk */
label.required::after {
  content: ' *';
  color: crimson;
}

/* Clearfix (less needed now with flexbox/grid) */
.clearfix::after {
  content: '';
  display: block;
  clear: both;
}

/* Badge / counter */
.notification::after {
  content: attr(data-count);  /* reads from HTML attribute */
  background: crimson;
  color: white;
  border-radius: 50%;
  padding: 2px 6px;
  font-size: 0.75rem;
}
```

### `::first-line` and `::first-letter`

```css
/* Drop cap */
p::first-letter {
  font-size: 3em;
  font-weight: 700;
  float: left;
  line-height: 1;
  margin-right: 0.1em;
  color: royalblue;
}

/* Style opening line differently */
article p:first-of-type::first-line {
  font-weight: 600;
  font-variant: small-caps;
}
```

### `::selection`

```css
::selection {
  background: hsl(220, 90%, 55%);
  color: white;
}

/* Different selection colour per section */
.code-block::selection {
  background: hsl(280, 70%, 60%);
}
```

### `::placeholder`

```css
input::placeholder {
  color: #aaa;
  font-style: italic;
  font-size: 0.9em;
}
```

### `::marker`

```css
/* Style the bullet or number without removing it */
li::marker {
  color: royalblue;
  font-size: 1.2em;
}

/* Custom marker character */
ul li::marker {
  content: '→ ';
}

ol li::marker {
  font-weight: 700;
  color: crimson;
}
```

### `::backdrop`

```css
/* Style the backdrop behind a <dialog> element */
dialog::backdrop {
  background: rgba(0, 0, 0, 0.5);
  backdrop-filter: blur(4px);
}
```

---

## Common Patterns

```css
/* Tooltip via ::before */
[data-tooltip] {
  position: relative;
}
[data-tooltip]::before {
  content: attr(data-tooltip);
  position: absolute;
  bottom: 100%;
  left: 50%;
  transform: translateX(-50%);
  background: #333;
  color: white;
  padding: 4px 8px;
  border-radius: 4px;
  font-size: 0.8rem;
  white-space: nowrap;
  opacity: 0;
  pointer-events: none;
  transition: opacity 0.2s ease;
}
[data-tooltip]:hover::before {
  opacity: 1;
}

/* Active nav link indicator */
nav a.active::after {
  content: '';
  display: block;
  height: 2px;
  background: royalblue;
  border-radius: 1px;
}

/* External link icon */
a[href^="https://"]::after {
  content: ' ↗';
  font-size: 0.75em;
  opacity: 0.6;
}
```
