Valid HTML is consistently parsed by every browser. Best practices go further — they ensure the document is semantic, accessible, performant, and maintainable. Validation catches syntax errors; best practices catch everything else.

---

## HTML Validation

### The W3C Nu HTML Checker

The official validator checks your markup against the HTML Living Standard:

- **Online:** [validator.w3.org](https://validator.w3.org/) — paste a URL, upload a file, or paste code directly
- **VS Code:** install the **HTMLHint** or **Markuplint** extension for inline warnings
- **Command line:** `npm install -g html-validate` then `html-validate index.html`

A valid document is not necessarily a good document. Validation catches syntax errors — unclosed tags, invalid attribute values, duplicate IDs. It does not catch semantic mistakes (using `<div>` where `<article>` belongs) or accessibility issues.

Run both:

- W3C validator for syntax
- Lighthouse (browser DevTools → Lighthouse tab) for accessibility, performance, and SEO

---

## The HTML Living Standard

HTML is maintained by WHATWG, not by version numbers. There is no "HTML5" — there is HTML, which is continuously updated. The spec lives at [html.spec.whatwg.org](https://html.spec.whatwg.org/).

The best day-to-day reference is **MDN Web Docs** at [developer.mozilla.org](https://developer.mozilla.org/). It is accurate, up-to-date, and includes browser compatibility tables for every element and attribute.

---

## Common Validation Errors and Fixes

```html
<!-- ERROR: Unclosed element -->
<div>
  <p>Text

<!-- FIX -->
<div>
  <p>Text</p>
</div>


<!-- ERROR: Duplicate id -->
<div id="header">...</div>
<div id="header">...</div>   <!-- two elements with the same id -->

<!-- FIX: ids must be unique -->
<div id="site-header">...</div>
<div id="page-header">...</div>


<!-- ERROR: <li> not inside <ul>, <ol>, or <menu> -->
<div>
  <li>Item</li>
</div>

<!-- FIX -->
<ul>
  <li>Item</li>
</ul>


<!-- ERROR: Block element inside inline element -->
<a href="/page">
  <div>Click here</div>   <!-- <div> is block, cannot be inside <a> normally -->
</a>

<!-- FIX: use a block-level link or CSS -->
<a href="/page" style="display: block">Click here</a>


<!-- ERROR: Obsolete attributes -->
<table border="1" cellpadding="5">    <!-- deprecated attributes -->

<!-- FIX: use CSS instead -->
<table class="data-table">


<!-- ERROR: Missing required attribute -->
<img src="photo.jpg">    <!-- missing alt -->

<!-- FIX -->
<img src="photo.jpg" alt="Description of photo">
```

---

## Best Practices Checklist

### Document Level

```html
<!-- Every HTML file should start with these four lines -->
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Descriptive Page Title — Site Name</title>
```

- [ ] `<!DOCTYPE html>` is the very first line — no whitespace before it
- [ ] `<html lang="...">` has the correct language code
- [ ] `<meta charset="UTF-8">` is the first child of `<head>`
- [ ] `<meta name="viewport">` is present
- [ ] `<title>` is unique per page, descriptive, 50–60 characters

### Document Structure

- [ ] One `<h1>` per page — the primary page title
- [ ] Heading hierarchy is never skipped (no `<h1>` → `<h3>`)
- [ ] One `<main>` per page
- [ ] `<nav>` wraps all primary navigation
- [ ] `<header>`, `<footer>`, `<article>`, `<section>`, `<aside>` used correctly
- [ ] No layout tables — use CSS Flexbox or Grid
- [ ] No `<div>` or `<span>` used where a semantic element fits

### Images

- [ ] Every `<img>` has an `alt` attribute
- [ ] Decorative images use `alt=""`
- [ ] `width` and `height` attributes present on images — prevents layout shift
- [ ] `loading="lazy"` on below-the-fold images
- [ ] `<figure>` and `<figcaption>` for images that need a caption

### Links

- [ ] All `target="_blank"` links have `rel="noopener noreferrer"`
- [ ] Link text is descriptive — not "click here" or "read more"
- [ ] `<a>` used for navigation, `<button>` used for actions

### Forms

- [ ] Every `<input>` has an associated `<label>` (via `for`/`id` or wrapping)
- [ ] Radio and checkbox groups use `<fieldset>` + `<legend>`
- [ ] `type`, `required`, `autocomplete` attributes set appropriately
- [ ] Error messages linked to inputs via `aria-describedby`
- [ ] Submit button uses `<button type="submit">` or `<input type="submit">`

### Accessibility

- [ ] Skip link present as first focusable element
- [ ] All interactive elements are keyboard-focusable
- [ ] No positive `tabindex` values
- [ ] `aria-label` on icon-only buttons
- [ ] `lang` declared on `<html>` and overridden where content switches language

### Performance

- [ ] `<link rel="preload">` for critical fonts and hero images
- [ ] `loading="lazy"` on below-the-fold images and iframes
- [ ] Scripts use `defer` or `async` where appropriate
- [ ] Images have `width` and `height` to prevent Cumulative Layout Shift (CLS)

### Meta and SEO

- [ ] `<meta name="description">` present and 150–160 characters
- [ ] Open Graph tags present (`og:title`, `og:description`, `og:image`, `og:url`)
- [ ] `<link rel="canonical">` present
- [ ] Favicon defined

---

## Things to Stop Doing

```html
<!-- Don't use presentational HTML attributes -->
<font size="5" color="red">Big red text</font>   <!-- obsolete -->
<b>Bold</b>           <!-- use <strong> for important text, or CSS font-weight -->
<i>Italic</i>         <!-- use <em> for emphasis, or CSS font-style -->
<br><br><br>          <!-- don't use line breaks for spacing — use CSS margin/padding -->
<hr>                  <!-- only use <hr> for thematic breaks, not visual dividers -->
<center>Content</center>  <!-- obsolete — use CSS text-align or flexbox -->

<!-- Don't write inline styles for layout -->
<div style="float: left; margin: 10px; font-size: 14px;">...</div>  <!-- hard to maintain -->

<!-- Don't use tables for layout -->
<table>
  <tr>
    <td>Sidebar</td>
    <td>Main content</td>
  </tr>
</table>

<!-- Don't use meaningless IDs -->
<div id="div1">...</div>
<div id="container2">...</div>
```

---

## Useful Tools

|Tool|Use|
|---|---|
|[validator.w3.org](https://validator.w3.org/)|Validate HTML syntax|
|[developer.mozilla.org](https://developer.mozilla.org/)|Element and attribute reference|
|Browser DevTools → Lighthouse|Accessibility, performance, SEO audit|
|Browser DevTools → Accessibility tree|See what screen readers see|
|[wave.webaim.org](https://wave.webaim.org/)|Accessibility evaluation|
|HTMLHint (VS Code extension)|Inline HTML linting|

---
