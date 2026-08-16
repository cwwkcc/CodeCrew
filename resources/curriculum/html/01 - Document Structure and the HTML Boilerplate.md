## The Minimal Valid HTML Document

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Page Title</title>
  </head>
  <body>
    <p>Content goes here.</p>
  </body>
</html>
```

Every single line above has a reason for existing. None of them are optional in a production document. This module explains each one.

---

## `<!DOCTYPE html>`

This is not an HTML tag — it is a _document type declaration_. It tells the browser to use **standards mode** rather than _quirks mode_.

Quirks mode exists for backward compatibility with pages written in the late 1990s before browsers agreed on standards. In quirks mode, browsers emulate old, incorrect behaviour. You never want quirks mode. `<!DOCTYPE html>` is the switch that prevents it.

The `html` in `<!DOCTYPE html>` refers to the HTML Living Standard. Older doctypes (HTML 4, XHTML) were long strings referencing DTD files. The modern doctype is intentionally minimal.

**Rule:** `<!DOCTYPE html>` must be the very first thing in any HTML file, before any whitespace.

---

## `<html lang="en">`

The root element of every HTML document. There is exactly one, and every other element is a descendant of it.

The `lang` attribute declares the document's primary language using a [BCP 47 language tag](https://www.ietf.org/rfc/bcp/bcp47.txt):

| Language             | Tag       |
| -------------------- | --------- |
| English              | `en`      |
| Sinhala              | `si`      |
| Tamil                | `ta`      |
| French (Canada)      | `fr-CA`   |
| Chinese (Simplified) | `zh-Hans` |

**Why it matters:** Screen readers use `lang` to select the correct pronunciation engine. Search engines use it for language targeting. CSS `hyphens: auto` uses it for hyphenation dictionaries. Omitting it is an accessibility defect.

---

## The `<head>` Element

The `<head>` contains _metadata_ about the document — information that is not displayed as content but is consumed by the browser, search engines, and other tools.

### `<meta charset="UTF-8">`

Declares the character encoding. **UTF-8** can represent every character in every human language plus emoji. Without this declaration the browser guesses, and it can guess wrong — causing mojibake (garbled characters).

This must be the **first child of `<head>`**, before `<title>`. The browser needs the encoding declaration before it reads the title (which may contain non-ASCII characters).

### `<meta name="viewport" content="width=device-width, initial-scale=1.0">`

Controls how the page is scaled on mobile devices. Without it, mobile browsers render the page at a virtual ~980px width and then scale it down — making everything tiny.

- `width=device-width` — use the device's actual screen width as the viewport width
- `initial-scale=1.0` — do not zoom in or out on initial load

This is a mandatory line for any document that will be viewed on a phone.

### `<title>`

The document title. Appears in:

- The browser tab
- Browser history and bookmarks
- Search engine result pages (as the clickable heading)
- Screen reader announcements when navigating between tabs

Rules for a good title:

- Be specific and descriptive
- Unique per page — not the same title on every page of a site
- Typically formatted as: `Page Name — Site Name`
- 50–60 characters is the practical SEO limit before truncation in search results

---

## The `<body>` Element

Contains everything that is rendered and displayed to the user. There is exactly one.

---

## Tree Structure and Nesting Rules

HTML is a _tree_. Elements nest inside each other. The nesting must be valid:

```html
<!-- CORRECT: properly nested -->
<p>This is <strong>very important</strong> text.</p>

<!-- WRONG: overlapping tags -->
<p>This is <strong>very important</p> text.</strong>
```

Every browser has an error-recovery algorithm for malformed HTML, but you should never rely on it. Always close tags in the reverse order you opened them.

### Content Model

Every element has a defined _content model_ — what kinds of children it is allowed to have. The two most important categories:

| Category                       | Description                            | Examples                                         |
| ------------------------------ | -------------------------------------- | ------------------------------------------------ |
| **Block-level** (flow content) | Takes up full width, stacks vertically | `<div>`, `<p>`, `<h1>`–`<h6>`, `<ul>`, `<table>` |
| **Inline** (phrasing content)  | Flows within text                      | `<span>`, `<a>`, `<strong>`, `<em>`, `<img>`     |

Critical rule: **block elements may not be nested inside inline elements.**

```html
<!-- WRONG: block inside inline -->
<span><p>Text</p></span>

<!-- CORRECT -->
<p><span>Text</span></p>
```

---

## Whitespace and Comments

The browser collapses consecutive whitespace (spaces, tabs, newlines) in HTML content down to a single space. You can use indentation freely for readability without affecting output.

```html
<!-- This is an HTML comment. It is invisible in the browser. -->
```

Comments are useful for section markers and TODO notes. They are sent to the client and visible in View Source — never put passwords or secrets in HTML comments.

---

## Void Elements

Some elements have no content and no closing tag:

```html
<br />
<!-- line break -->
<hr />
<!-- thematic break / horizontal rule -->
<img />
<!-- image -->
<input />
<!-- form field -->
<meta />
<!-- metadata -->
<link />
<!-- external resource link -->
```

In HTML5 these are written without a slash. The XHTML-style self-closing slash (`<br />`) is valid but unnecessary in HTML.

---

## Summary

| Line                         | Purpose                                         |
| ---------------------------- | ----------------------------------------------- |
| `<!DOCTYPE html>`            | Activates standards mode                        |
| `<html lang="...">`          | Root element; declares document language        |
| `<meta charset="UTF-8">`     | Declares UTF-8 encoding — must be first in head |
| `<meta name="viewport" ...>` | Correct scaling on mobile devices               |
| `<title>`                    | Names the document everywhere it appears        |
| `<body>`                     | Contains all visible content                    |

---

## Common Mistakes

```html
<!-- WRONG: charset after title — browser may have already
     guessed an encoding and misread the title's non-ASCII characters -->
<head>
  <title>Café Menu</title>
  <meta charset="UTF-8" />
</head>

<!-- CORRECT: charset first, before anything with text content -->
<head>
  <meta charset="UTF-8" />
  <title>Café Menu</title>
</head>
```

- **Forgetting the viewport meta tag.** This is the single most common reason a page "looks fine on my laptop" and is unreadably tiny on a phone during testing.
- **Closing tags out of order.** `<div><p>Text</div></p>` will often _look_ fine because browsers silently repair broken nesting — but you're relying on error-recovery, not on your markup being correct. Always close in reverse of the order you opened.
- **Block elements inside inline elements**, most often `<div>` or `<p>` nested inside `<a>` or `<span>`. The browser will move things around to make it valid, and the result is rarely what you intended.
- **Skipping `<!DOCTYPE html>` or adding whitespace/comments before it.** Either one silently drops the page into quirks mode, and everything downstream (CSS box model, `vh` units, `<table>` layout) starts behaving like it's 1999.
