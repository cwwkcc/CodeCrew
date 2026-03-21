This module collects the full reference for pseudo-classes and pseudo-elements, expanding on what was introduced in Module 01.

## State Pseudo-classes

```css
:hover      { }  /* pointer over element */
:focus      { }  /* keyboard focus */
:focus-within { }  /* element or any descendant has focus — useful for forms */
:focus-visible { }  /* focus via keyboard only, not mouse click */
:active     { }  /* element being clicked */
:visited    { }  /* visited link */
:target     { }  /* element whose id matches the URL fragment (#section-id) */
```

## Form State Pseudo-classes

```css
:checked        { }  /* checked checkbox or radio */
:disabled       { }  /* disabled form element */
:enabled        { }  /* enabled form element */
:required       { }  /* input with required attribute */
:optional       { }  /* input without required attribute */
:valid          { }  /* input passing validation */
:invalid        { }  /* input failing validation */
:in-range       { }  /* number input within min/max range */
:out-of-range   { }  /* number input outside min/max range */
:placeholder-shown { }  /* input showing its placeholder */
:read-only      { }
:read-write     { }
```

## Structural Pseudo-classes

```css
:first-child     { }  /* first sibling among all types */
:last-child      { }
:nth-child(n)    { }  /* by position (1-indexed) */
:nth-last-child(n) { }  /* counted from the end */
:only-child      { }  /* sole child of parent */

:first-of-type   { }  /* first sibling of same tag type */
:last-of-type    { }
:nth-of-type(n)  { }
:only-of-type    { }

:empty           { }  /* element with no children (not even whitespace) */
:root            { }  /* <html> element */
```

### `nth-child` Formula

`nth-child(An + B)` where A is the step and B is the offset:

```css
:nth-child(2)    { }  /* only the 2nd */
:nth-child(odd)  { }  /* 1, 3, 5... */
:nth-child(even) { }  /* 2, 4, 6... */
:nth-child(3n)   { }  /* 3, 6, 9... */
:nth-child(3n+1) { }  /* 1, 4, 7... */
:nth-child(n+4)  { }  /* 4th and beyond */
:nth-child(-n+3) { }  /* first 3 only */
```

## Logical Pseudo-classes

```css
:not(.active)           { }  /* any element that does not match */
:not(p, .card)          { }  /* not a <p> and not .card (multiple selectors) */
:is(h1, h2, h3)         { }  /* matches any in the list — keeps highest specificity */
:where(h1, h2, h3)      { }  /* same but zero specificity — good for resets */
:has(img)               { }  /* parent selector — elements that contain an img */
```

`:has()` is the long-awaited "parent selector". It lets you style an element based on what it contains:

```css
.card:has(img) { padding: 0; }           /* cards containing an image — remove padding */
li:has(a:hover) { background: #f5f5f5; } /* list item when its link is hovered */
form:has(:invalid) { border-color: red; } /* form while any field is invalid */
```

## Pseudo-elements Reference

```css
::before        { content: ""; }  /* insert content before element's content */
::after         { content: ""; }  /* insert content after element's content */
::first-line    { }  /* first line of a block element */
::first-letter  { }  /* first character */
::selection     { }  /* user-selected text */
::placeholder   { }  /* placeholder text in inputs */
::marker        { }  /* list item bullet or number */
::backdrop      { }  /* full-screen element backdrop */
```

### `::marker` — Styling list bullets

```css
li::marker {
  color: navy;
  font-size: 1.2em;
}

li::marker { content: "→ "; }  /* custom marker character */
```

### Best Practices

- Use `:focus-visible` instead of `:focus` for focus rings — it only shows the ring for keyboard navigation, not mouse clicks, giving a cleaner UI without sacrificing accessibility
- Never use `outline: none` on `:focus` without providing an alternative visible indicator
- Use `:is()` to simplify repetitive selector groups
- Use `:has()` to replace JavaScript-based parent class toggling where possible