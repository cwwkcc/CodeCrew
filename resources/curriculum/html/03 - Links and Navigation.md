The `<a>` element is the hyperlink — the mechanism that connects the web. Every navigation pattern, every button that routes to another page, every file download, and every in-page jump is built on `<a>`.

---

## Basic Syntax

```html
<a href="https://example.com">Visit Example</a>
```

`<a>` is an inline element. The `href` attribute is what creates the link — without it, `<a>` is just inert text with no navigation behaviour.

---

## `href` Values

|Type|Syntax|Use|
|---|---|---|
|Absolute URL|Full URL with scheme|External sites|
|Root-relative|Starts with `/`|Same site, from root|
|Relative|No leading `/`|Same site, relative to current file|
|Fragment|`#id`|Jump to element on same page|
|`mailto:`|`mailto:user@example.com`|Open email client|
|`tel:`|`tel:+94712345678`|Dial a phone number|
|`download`|Any URL + `download` attr|Trigger file download|

```html
<!-- Absolute — full URL -->
<a href="https://developer.mozilla.org">MDN</a>

<!-- Root-relative — works from any page depth on the same site -->
<a href="/about">About</a>

<!-- Relative — resolves from the current page's location -->
<a href="../images/photo.jpg">Photo</a>

<!-- Same-page anchor — jumps to element with id="installation" -->
<a href="#installation">Jump to Installation</a>

<!-- Email — opens the user's email client -->
<a href="mailto:hello@paideon.lk">Email us</a>

<!-- Phone — on mobile, offers to dial the number -->
<a href="tel:+94712345678">Call us</a>

<!-- Download — triggers a file download with a custom filename -->
<a href="/reports/q3.pdf" download="Q3_Report_2025.pdf">Download Report</a>
```

The `download` attribute works only for same-origin URLs (files on the same website).

---

## `target` — Where to Open the Link

```html
<!-- Default — opens in the same tab -->
<a href="/about">About</a>

<!-- Opens in a new tab -->
<a href="https://external.com" target="_blank" rel="noopener noreferrer">
  External link
</a>
```

|Value|Behaviour|
|---|---|
|`_self`|Same tab (default)|
|`_blank`|New tab or window|
|`_parent`|Parent frame (used with iframes)|
|`_top`|Top-level frame, breaks out of all iframes|

**Always add `rel="noopener noreferrer"` when using `target="_blank"`** on external links. Without it, the opened page can access and manipulate the original tab — a security risk. This is important to know even if you don't yet understand exactly how it works.

---

## Fragment Identifiers — In-Page Navigation

Any element with an `id` attribute becomes a linkable anchor:

```html
<!-- The target element -->
<h2 id="installation">Installation</h2>

<!-- A link to it — from the same page -->
<a href="#installation">Jump to Installation</a>

<!-- A link to it — from another page -->
<a href="/docs#installation">Docs: Installation</a>
```

Clicking the link scrolls the browser to that element and adds `#installation` to the URL.

`id` values must be unique within a document and cannot contain spaces.

---

## Navigation Landmark: `<nav>`

Wrap navigation links in a `<nav>` element. It tells the browser (and screen readers) that this group of links is the site's navigation:

```html
<nav>
  <ul>
    <li><a href="/">Home</a></li>
    <li><a href="/courses">Courses</a></li>
    <li><a href="/about">About</a></li>
    <li><a href="/contact">Contact</a></li>
  </ul>
</nav>
```

A page can have more than one `<nav>`. Use `aria-label` to distinguish them:

```html
<nav aria-label="Main navigation">
  <!-- primary nav links -->
</nav>

<nav aria-label="Footer navigation">
  <!-- footer links -->
</nav>
```

---

## Breadcrumb Navigation

A breadcrumb shows the path from home to the current page. Use `<ol>` (ordered — it's a path), and mark the current page with `aria-current="page"`:

```html
<nav aria-label="Breadcrumb">
  <ol>
    <li><a href="/">Home</a></li>
    <li><a href="/courses">Courses</a></li>
    <li><a href="/courses/javascript">JavaScript</a></li>
    <li><a href="/courses/javascript/closures" aria-current="page">Closures</a></li>
  </ol>
</nav>
```

`aria-current="page"` tells screen readers this is the currently active page in the list.

---

## `<a>` vs `<button>` — When to Use Which

This is one of the most common mistakes in HTML:

|Element|Use for|
|---|---|
|`<a href>`|**Navigation** — takes the user to a URL|
|`<button>`|**Actions** — does something on the page (submit form, open modal, toggle menu)|

```html
<!-- RIGHT — link navigates somewhere -->
<a href="/dashboard">Go to Dashboard</a>

<!-- WRONG — link used to trigger an action -->
<a href="#" onclick="openModal()">Open Modal</a>

<!-- RIGHT — button for in-page actions -->
<button type="button" onclick="openModal()">Open Modal</button>
```

Using `<a href="#">` as a button breaks keyboard navigation and screen reader announcements.

---

## `<link>` in `<head>` vs `<a>` in `<body>`

These are two completely different elements that share nothing in common except the word "link":

|Element|Where|Purpose|
|---|---|---|
|`<a href>`|`<body>`|A clickable link for users|
|`<link rel href>`|`<head>`|Connects resources (stylesheets, icons) — not visible to users|

```html
<head>
  <!-- These are <link> elements — not clickable, not for users -->
  <link rel="stylesheet" href="/styles.css">
  <link rel="icon" href="/favicon.ico">
</head>

<body>
  <!-- This is an <a> element — a clickable link for users -->
  <a href="/about">About</a>
</body>
```

---
