## The Anchor Element: `<a>`

```html
<a href="https://example.com">Visit Example</a>
```

`<a>` is an inline element. The `href` attribute is what makes it a link; without it, `<a>` is just a placeholder anchor with no navigation behaviour.

---

## `href` Values

|Type|Syntax|Example|
|---|---|---|
|Absolute URL|Full URL with scheme|`href="https://example.com/page"`|
|Root-relative|From site root|`href="/about"`|
|Relative|From current file|`href="../images/photo.jpg"`|
|Same-page anchor|Fragment identifier|`href="#section-2"`|
|Email|`mailto:` scheme|`href="mailto:user@example.com"`|
|Phone|`tel:` scheme|`href="tel:+94712345678"`|
|Download|Any URL + `download` attr|`href="/report.pdf" download`|

---

## Target and Rel

```html
<!-- Open in new tab -->
<a href="https://example.com" target="_blank" rel="noopener noreferrer">
  External link
</a>
```

- `target="_blank"` — opens in a new tab/window.
- `rel="noopener"` — prevents the new page from accessing `window.opener` (security).
- `rel="noreferrer"` — also suppresses the `Referer` header (privacy + implies noopener).

**Always pair `target="_blank"` with `rel="noopener noreferrer"`.** Omitting these on external links is a security and privacy defect.

---

## Anchor Targets (Fragment IDs)

Any element with an `id` attribute becomes a linkable target:

```html
<h2 id="installation">Installation</h2>

<!-- elsewhere on the page -->
<a href="#installation">Jump to Installation</a>
```

`id` values must be unique within the document and must not contain spaces.

---

## Navigation Landmark: `<nav>`

```html
<nav aria-label="Main navigation">
  <ul>
    <li><a href="/">Home</a></li>
    <li><a href="/about">About</a></li>
    <li><a href="/contact">Contact</a></li>
  </ul>
</nav>
```

`<nav>` is a _landmark element_ — screen readers expose it as a navigation region. A page may have multiple `<nav>` elements; use `aria-label` to distinguish them.

Navigation links are almost always a list of links (`<ul>` + `<li>` + `<a>`). This is semantically correct: a list of navigation items is literally a list.

---

## The `download` Attribute

```html
<a href="/files/report.pdf" download="Annual_Report_2025.pdf">
  Download Report
</a>
```

Prompts a file download instead of navigation. The attribute value sets the suggested filename. Works only for same-origin URLs (or with CORS headers on cross-origin).

---

## `<link>` in `<head>` vs `<a>` in `<body>`

These are different elements for different purposes:

|Element|Where|Purpose|
|---|---|---|
|`<a href>`|`<body>`|User-navigable hyperlink|
|`<link rel href>`|`<head>`|Machine-readable resource relationship (stylesheet, icon, canonical, etc.)|