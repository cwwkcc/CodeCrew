## What CSS Is

CSS (Cascading Style Sheets) is the language that controls the visual presentation of HTML. HTML defines what content is — CSS defines how it looks.

The three technologies of the web have a clear division of responsibility:

- **HTML** — structure and meaning
- **CSS** — presentation and layout
- **JavaScript** — behaviour and interactivity

CSS is not a programming language. It is a set of rules. Each rule tells the browser: "find these elements, and apply these styles to them."

---

## Three Ways to Add CSS

### 1. External Stylesheet (correct for almost everything)

```html
<head>
  <link rel="stylesheet" href="/css/main.css">
</head>
```

The CSS lives in a separate file. The browser downloads it once and caches it — subsequent pages that link the same file get it for free. This is the standard approach.

### 2. Internal `<style>` Block (use for quick tests only)

```html
<head>
  <style>
    p { color: red; }
  </style>
</head>
```

CSS written inside a `<style>` tag in the HTML file. Fine for experimenting, but does not benefit from caching and mixes content with presentation.

### 3. Inline `style` Attribute (avoid)

```html
<p style="color: red;">Text</p>
```

CSS written directly on an element. Overrides everything else, impossible to maintain at scale, and mixes structure with presentation. Only acceptable in very specific situations (e.g. programmatically generated styles via JavaScript).

---

## CSS Rule Syntax

```css
selector {
  property: value;
  property: value;
}
```

- **Selector** — which elements to target
- **Property** — which aspect of the element to change
- **Value** — what to change it to
- **Declaration** — a single `property: value` pair
- **Declaration block** — the `{ }` containing all declarations for that selector
- **Rule** — the selector plus its declaration block

```css
/* This is a comment in CSS */

h1 {
  color: navy;
  font-size: 2rem;
  font-weight: bold;
}
```

---

## The Cascade

CSS stands for _Cascading_ Style Sheets. The cascade is the algorithm the browser uses to decide which rule wins when multiple rules target the same element and the same property.

The cascade considers three factors in this order:

### 1. Origin and Importance

|Origin|Priority|
|---|---|
|Browser default stylesheet|Lowest|
|Author stylesheet (your CSS)|Middle|
|`!important` declarations|Highest|

`!important` overrides everything. It is a last resort, not a tool — overusing it is a sign of poorly structured CSS.

```css
p { color: red !important; }  /* wins against everything else */
```

### 2. Specificity

When two rules from the same origin target the same element, the more _specific_ selector wins.

Specificity is calculated as a score with three components: **(ID, Class, Element)**

|Selector type|Specificity score|
|---|---|
|Element selector (`p`, `h1`, `div`)|0, 0, 1|
|Class selector (`.card`)|0, 1, 0|
|Attribute selector (`[type="text"]`)|0, 1, 0|
|Pseudo-class (`:hover`, `:first-child`)|0, 1, 0|
|ID selector (`#header`)|1, 0, 0|
|Inline `style=""`|1, 0, 0, 0 (always wins)|

```css
p { color: blue; }           /* 0,0,1 — loses */
.intro { color: green; }     /* 0,1,0 — wins */
#main p { color: red; }      /* 1,0,1 — wins over both above */
```

### 3. Source Order

If two rules have equal specificity, the one that appears **later** in the stylesheet wins.

```css
p { color: blue; }
p { color: red; }  /* wins — comes later */
```

---

## Inheritance

Some CSS properties are _inherited_ — child elements automatically receive the value from their parent unless they have their own rule.

**Inherited by default:** `color`, `font-family`, `font-size`, `font-weight`, `line-height`, `text-align`, `list-style`, `cursor`, `visibility`

**Not inherited by default:** `margin`, `padding`, `border`, `background`, `width`, `height`, `display`, `position`

```html
<div style="color: navy;">
  <p>This paragraph is navy</p>        <!-- inherits color from div -->
  <p style="color: red;">This is red</p>  <!-- overrides with own value -->
</div>
```

### Explicit Inheritance

You can force inheritance on a non-inherited property using the keyword `inherit`:

```css
.child {
  border: inherit;  /* takes the border value from its parent */
}
```

Or reset to the browser default using `initial`, or use `unset` which means `inherit` for inherited properties and `initial` for non-inherited ones.

---

## The Browser Default Stylesheet

Every browser ships with a built-in stylesheet that gives HTML elements their default appearance — `<h1>` is big and bold, `<a>` is blue and underlined, `<p>` has vertical margin. This is why a plain HTML file without any CSS still looks somewhat structured.

Your CSS always runs on top of these defaults. You are not building from nothing — you are overriding the browser's starting point.

---

## CSS Resets and Normalizers

Because different browsers have slightly different default stylesheets, many developers start with a **CSS reset** to flatten all defaults to a consistent baseline:

```css
/* Minimal modern reset */
*, *::before, *::after {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}
```

A more complete option is [modern-normalize](https://github.com/sindresorhus/modern-normalize), which preserves useful defaults while removing cross-browser inconsistencies.

---

## Summary

|Concept|What it means|
|---|---|
|External stylesheet|CSS in a separate `.css` file, linked via `<link>`|
|Rule|A selector + declaration block|
|Cascade|The algorithm for deciding which rule wins|
|Specificity|How precisely a selector targets elements — more specific wins|
|Inheritance|Child elements automatically receive some property values from parents|
|Browser defaults|The starting stylesheet every browser applies before yours|