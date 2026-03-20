## HTML Validation

The W3C Nu HTML Checker validates markup against the HTML Living Standard:

- Online: [validator.w3.org](https://validator.w3.org/)
- CLI: `npx vnu-jar` or `html-validate` npm package
- IDE: most editors have plugins (VSCode: HTMLHint, Markuplint)

**A valid document is not necessarily a good document.** Validation catches syntax errors; it does not catch semantic mistakes (using `<div>` where `<article>` belongs) or accessibility issues. Use both a validator _and_ an accessibility checker ([axe DevTools](https://www.deque.com/axe/), browser dev tools Lighthouse audit).

---

## The Living Standard

HTML is maintained by WHATWG at [html.spec.whatwg.org](https://html.spec.whatwg.org/). The MDN Web Docs ([developer.mozilla.org](https://developer.mozilla.org/)) is the best practical reference for day-to-day lookup — accurate, up-to-date, with browser compatibility tables.

---

## Best Practices Checklist

### Document level

- [ ] `<!DOCTYPE html>` is the very first line
- [ ] `<html lang="...">` has the correct language
- [ ] `<meta charset="UTF-8">` is the first child of `<head>`
- [ ] `<meta name="viewport" ...>` is present
- [ ] `<title>` is unique, descriptive, 50–60 characters

### Structure

- [ ] One `<h1>` per page; heading hierarchy is not skipped
- [ ] One `<main>` per page
- [ ] `<nav>` wraps all primary navigation
- [ ] `<header>` and `<footer>` used appropriately
- [ ] `<article>` and `<section>` used semantically
- [ ] No layout tables
- [ ] No `<div>` soup where semantic elements fit

### Images

- [ ] Every `<img>` has `alt` (empty string for decorative)
- [ ] `width` and `height` specified on all images
- [ ] `<figure>` + `<figcaption>` for captioned images
- [ ] Modern formats (WebP/AVIF) with JPEG fallback via `<picture>`

### Forms

- [ ] Every input has an associated `<label>`
- [ ] `<fieldset>` + `<legend>` for radio/checkbox groups
- [ ] `required`, `type`, and `autocomplete` attributes set correctly
- [ ] Error messages linked via `aria-describedby`

### Links

- [ ] `target="_blank"` links have `rel="noopener noreferrer"`
- [ ] Link text is descriptive (not "click here")

### Accessibility

- [ ] Skip link present
- [ ] All interactive elements are keyboard-focusable
- [ ] ARIA used only where native HTML is insufficient
- [ ] `lang` set on content in a different language from the document

### Performance

- [ ] `<link rel="preload">` for critical fonts/images
- [ ] `loading="lazy"` on below-the-fold images and iframes
- [ ] `defer` or `async` on non-critical scripts

### Validation

- [ ] Passes W3C Nu HTML Checker with zero errors
- [ ] Passes Lighthouse accessibility audit at 90+