# Assignment 2 — Solution Guide

## How to Use This File

Read this **after** you have attempted the assignment on your own.
Each error is explained with three things: what is wrong, why it matters, and how to fix it.

There are **15 errors** in the broken file. The bonus section covers 2 additional issues.

---

## Error 1 — Missing `lang` Attribute on `<html>`

**Location**: Opening `<html>` tag

**Wrong code**:

```html
<html></html>
```

**What is wrong**:
The `<html>` tag has no `lang` attribute.

**Why it matters**:
Screen readers use `lang` to select the correct pronunciation engine.
Search engines use it for language targeting.
Omitting it is an accessibility defect.

**Fixed code**:

```html
<html lang="en"></html>
```

---

## Error 2 — Missing `<meta charset="UTF-8">`

**Location**: `<head>` section

**Wrong code**:

```html
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
</head>
```

**What is wrong**:
The `<meta charset="UTF-8">` declaration is missing entirely.

**Why it matters**:
Without it the browser guesses the character encoding and can guess wrong —
causing garbled characters, especially for non-ASCII text like Sinhala or Tamil.
It must be the **first** element inside `<head>`.

**Fixed code**:

```html
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
</head>
```

---

## Error 3 — Empty `<title>`

**Location**: `<head>` section

**Wrong code**:

```html
<title></title>
```

**What is wrong**:
The title is empty.

**Why it matters**:
The title appears in the browser tab, bookmarks, search engine results, and screen
reader announcements. An empty title gives the user and search engines no information
about the page.

**Fixed code**:

```html
<title>CodeCrew Student Hub</title>
```

---

## Error 4 — Heading Level Skipped from `<h1>` to `<h3>`

**Location**: Below the `<h1>` tag

**Wrong code**:

```html
<h1>CodeCrew Student Hub</h1>
<h3>Your guide to everything HTML</h3>
```

**What is wrong**:
The heading jumps from `<h1>` directly to `<h3>`, skipping `<h2>`.

**Why it matters**:
Headings define the document outline. Skipping levels breaks the hierarchy.
Screen readers and search engines rely on a clean heading structure to understand
the page. If `h2` is the subtitle of the page, use `<h2>` — not `<h3>`.

**Fixed code**:

```html
<h1>CodeCrew Student Hub</h1>
<h2>Your guide to everything HTML</h2>
```

---

## Error 5 — Navigation in a `<div>` Instead of `<nav>`

**Location**: Navigation section

**Wrong code**:

```html
<div>
  <a href="#">Home</a>
  ...
</div>
```

**What is wrong**:
The navigation is wrapped in a `<div>` instead of a `<nav>` element.

**Why it matters**:
`<nav>` is a landmark element. Screen readers announce it as a navigation region,
allowing users to jump directly to navigation. A `<div>` carries no meaning —
it is invisible to assistive technology.

**Fixed code**:

```html
<nav aria-label="Main navigation">...</nav>
```

---

## Error 6 — Navigation Links Not Wrapped in a List

**Location**: Inside the navigation

**Wrong code**:

```html
<div>
  <a href="#">Home</a>
  <a href="#">Resources</a>
  <a href="#">Schedule</a>
  <a href="#">Contact</a>
</div>
```

**What is wrong**:
The navigation links are bare `<a>` tags with no list structure around them.

**Why it matters**:
A set of navigation items is literally a list of links. The correct pattern is
`<nav>` → `<ul>` → `<li>` → `<a>`. This gives screen readers the item count
("list of 4 items") and proper navigation behaviour.

**Fixed code**:

```html
<nav aria-label="Main navigation">
  <ul>
    <li><a href="#resources">Resources</a></li>
    <li><a href="#setup">How to Set Up VS Code</a></li>
    <li><a href="#schedule">Class Schedule</a></li>
    <li><a href="#contact">Contact</a></li>
  </ul>
</nav>
```

---

## Error 7 — External Links Missing `rel="noopener noreferrer"`

**Location**: Useful Resources section

**Wrong code**:

```html
<a href="https://developer.mozilla.org" target="_blank">MDN Web Docs</a>
<a href="https://www.w3schools.com" target="_blank">W3Schools</a>
```

**What is wrong**:
Both external links use `target="_blank"` without `rel="noopener noreferrer"`.

**Why it matters**:
Without `rel="noopener"`, the page that opens in the new tab can access the
original window via `window.opener` in JavaScript — a security vulnerability.
`rel="noreferrer"` also suppresses the Referer header for privacy.
These two attributes must always accompany `target="_blank"` on external links.

**Fixed code**:

```html
<a
  href="https://developer.mozilla.org"
  target="_blank"
  rel="noopener noreferrer"
  >MDN Web Docs</a
>
<a href="https://www.w3schools.com" target="_blank" rel="noopener noreferrer"
  >W3Schools</a
>
```

---

## Error 8 — `<a>` Tag Missing `href`

**Location**: Useful Resources section

**Wrong code**:

```html
<a>HTML Validator</a>
```

**What is wrong**:
The anchor tag has no `href` attribute.

**Why it matters**:
Without `href`, `<a>` is not a link — it is just styled text. It is not
keyboard-focusable, not clickable for navigation, and not announced as a link
by screen readers. Every `<a>` that is meant to navigate must have an `href`.

**Fixed code**:

```html
<a href="https://validator.w3.org">HTML Validator</a>
```

---

## Error 9 — `target="_blank"` on a `mailto:` Link

**Location**: Useful Resources section

**Wrong code**:

```html
<a href="mailto:hello@codecrew.lk" target="_blank">Email Us</a>
```

**What is wrong**:
`target="_blank"` is used on a `mailto:` link.

**Why it matters**:
A `mailto:` link opens the user's email client — not a web page. `target="_blank"`
has no effect on this and is meaningless here. Adding it is a sign of
copy-pasting attributes without understanding what they do.

**Fixed code**:

```html
<a href="mailto:hello@codecrew.lk">Email Us</a>
```

---

## Error 10 — Download Link Missing the `download` Attribute

**Location**: Useful Resources section

**Wrong code**:

```html
<a href="/notes.pdf">Download Class Notes</a>
```

**What is wrong**:
The link says "Download" but has no `download` attribute.

**Why it matters**:
Without the `download` attribute, clicking this link navigates to the PDF
in the browser instead of downloading it. The `download` attribute tells the
browser to prompt a file download and sets the suggested filename.

**Fixed code**:

```html
<a href="/notes.pdf" download="CodeCrew-Class-Notes.pdf"
  >Download Class Notes</a
>
```

---

## Error 11 — `<ul>` Used for an Ordered Sequence of Steps

**Location**: How to Set Up VS Code section

**Wrong code**:

```html
<ul>
  <li>Go to code.visualstudio.com</li>
  <li>Click the Download button</li>
  <li>Run the installer</li>
  <li>Open VS Code after installation</li>
  <li>Install the Live Server extension</li>
</ul>
```

**What is wrong**:
An unordered list is used for a sequence of steps that must be followed in order.

**Why it matters**:
`<ul>` communicates "order does not matter here." But installation steps have
a strict sequence — you cannot install Live Server before opening VS Code.
When order matters, use `<ol>`. The numbered output also helps the user follow
along step by step.

**Fixed code**:

```html
<ol>
  <li>Go to code.visualstudio.com</li>
  <li>Click the Download button</li>
  <li>Run the installer</li>
  <li>Open VS Code after installation</li>
  <li>Install the Live Server extension</li>
</ol>
```

---

## Error 12 — `<ol>` Used for an Unordered Collection

**Location**: Technologies We Teach section

**Wrong code**:

```html
<ol>
  <li>HTML</li>
  <li>CSS</li>
  <li>JavaScript</li>
  <li>React</li>
  <li>Node.js</li>
</ol>
```

**What is wrong**:
An ordered list is used for a collection of technologies where order carries
no meaning.

**Why it matters**:
`<ol>` implies a ranked or sequential relationship. HTML is not "more important
than CSS" or "must come before React" in an absolute sense. When items are simply
a group with no sequence, `<ul>` is the correct choice.

**Fixed code**:

```html
<ul>
  <li>HTML</li>
  <li>CSS</li>
  <li>JavaScript</li>
  <li>React</li>
  <li>Node.js</li>
</ul>
```

---

## Error 13 — Nested List Placed Outside the Parent `<li>`

**Location**: Frontend vs Backend section

**Wrong code**:

```html
<ul>
  <li>Frontend</li>
  <ul>
    <li>HTML</li>
    <li>CSS</li>
  </ul>
  <li>Backend</li>
  <ul>
    <li>Node.js</li>
    <li>Databases</li>
  </ul>
</ul>
```

**What is wrong**:
The inner `<ul>` elements are placed after the `<li>` closing tags, making them
direct children of the outer `<ul>` instead of children of the `<li>`.

**Why it matters**:
A `<ul>` can only have `<li>` as direct children. Placing another `<ul>` directly
inside a `<ul>` is invalid HTML. The nested list must go **inside** the parent
`<li>`, before its closing tag.

**Fixed code**:

```html
<ul>
  <li>
    Frontend
    <ul>
      <li>HTML</li>
      <li>CSS</li>
      <li>JavaScript</li>
    </ul>
  </li>
  <li>
    Backend
    <ul>
      <li>Node.js</li>
      <li>Databases</li>
    </ul>
  </li>
</ul>
```

---

## Error 14 — `<ul>` Used for Term-Definition Pairs Instead of `<dl>`

**Location**: Key Terms section

**Wrong code**:

```html
<ul>
  <li>HTML: HyperText Markup Language</li>
  <li>CSS: Cascading Style Sheets</li>
  <li>DOM: Document Object Model</li>
  <li>HTTP: The protocol used to transfer web documents</li>
</ul>
```

**What is wrong**:
A glossary of terms and their definitions is represented as an unordered list.

**Why it matters**:
HTML has a dedicated element for term-definition pairs: `<dl>` (description list),
with `<dt>` for the term and `<dd>` for the definition. Using `<ul>` loses the
semantic relationship between the term and its meaning. Screen readers and search
engines cannot tell which part is the term and which is the definition.

**Fixed code**:

```html
<dl>
  <dt>HTML</dt>
  <dd>
    HyperText Markup Language — the standard language for structuring web
    content.
  </dd>

  <dt>CSS</dt>
  <dd>
    Cascading Style Sheets — controls the visual presentation of HTML elements.
  </dd>

  <dt>DOM</dt>
  <dd>
    Document Object Model — the in-memory tree the browser builds from parsed
    HTML.
  </dd>

  <dt>HTTP</dt>
  <dd>The application-layer protocol used to transfer web documents.</dd>
</dl>
```

---

## Error 15 — Table Missing `<caption>`, `<thead>`, `<tbody>`, `<th>`, and `scope`

**Location**: Class Schedule section

**Wrong code**:

```html
<table border="1">
  <tr>
    <td>Day</td>
    <td>Subject</td>
    <td>Time</td>
    <td>Room</td>
  </tr>
  <tr>
    <td>Monday</td>
    ...
  </tr>
</table>
```

**What is wrong**:
The table is missing five things at once:

1. No `<caption>` — no title for the table
2. No `<thead>` — header row is not marked as a header group
3. No `<tbody>` — data rows are not wrapped in a body group
4. Header cells use `<td>` instead of `<th>` — no semantic weight
5. No `scope` attribute on header cells — screen readers cannot associate headers with data cells

**Why it matters**:
A table without these elements is just a visual grid. A screen reader navigating
this table cannot tell the user what each column means because there are no real
header cells. `<th scope="col">` is what links each column header to its data.

**Fixed code**:

```html
<table border="1">
  <caption>
    CodeCrew Weekly Class Schedule
  </caption>
  <thead>
    <tr>
      <th scope="col">Day</th>
      <th scope="col">Subject</th>
      <th scope="col">Time</th>
      <th scope="col">Room</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th scope="row">Monday</th>
      <td>HTML</td>
      <td>09:00</td>
      <td>Lab 1</td>
    </tr>
    <tr>
      <th scope="row">Tuesday</th>
      <td>CSS</td>
      <td>09:00</td>
      <td>Lab 1</td>
    </tr>
    <tr>
      <th scope="row">Wednesday</th>
      <td>JavaScript</td>
      <td>10:00</td>
      <td>Lab 2</td>
    </tr>
    <tr>
      <th scope="row">Thursday</th>
      <td>React</td>
      <td>10:00</td>
      <td>Lab 2</td>
    </tr>
  </tbody>
  <tfoot>
    <tr>
      <th scope="row" colspan="4">4 sessions per week</th>
    </tr>
  </tfoot>
</table>
```

---

## Bonus Errors

### Bonus Error A — `<b>` and `<i>` Used Instead of Semantic Elements

**Location**: Contact section

**Wrong code**:

```html
<p>
  <b>Important: The next class is on Friday 18 April at 9AM. Do not be late.</b>
</p>
<p>Have questions? <i>We are always happy to help.</i> Reach out any time.</p>
```

**What is wrong**:
`<b>` is used for a critically important warning. `<i>` is used for
stressed emphasis.

**Why it matters**:
`<b>` means "bold without importance." The notice here is genuinely urgent —
it should be `<strong>`. Similarly, `<i>` means "italic without emphasis."
The phrase "We are always happy to help" carries emotional stress in context —
it should be `<em>`.

**Fixed code**:

```html
<p>
  <strong
    >Important: The next class is on Friday 18 April at 9AM. Do not be
    late.</strong
  >
</p>
<p>Have questions? <em>We are always happy to help.</em> Reach out any time.</p>
```

---

### Bonus Error B — `<br><br>` Used to Create Paragraph Spacing

**Location**: Contact section

**Wrong code**:

```html
<p>Have questions? <i>We are always happy to help.</i> Reach out any time.</p>
<br />
<br />
<p>You can also find us on...</p>
```

**What is wrong**:
Two `<br>` tags are used between two paragraphs to add visual spacing.

**Why it matters**:
`<br>` is for line breaks _within_ a block of content — not for spacing between
separate blocks. If content belongs in separate paragraphs, use separate `<p>`
tags. The browser already adds appropriate spacing between paragraphs. Using
`<br>` as a spacer is a layout hack that breaks semantic meaning.

**Fixed code**:

```html
<p>Have questions? <em>We are always happy to help.</em> Reach out any time.</p>
<p>You can also find us on...</p>
```

---

## Error Summary Table

| #   | Location         | Error                                                      | Fix                                   |
| --- | ---------------- | ---------------------------------------------------------- | ------------------------------------- |
| 1   | `<html>`         | Missing `lang` attribute                                   | Add `lang="en"`                       |
| 2   | `<head>`         | Missing `<meta charset="UTF-8">`                           | Add as first child of `<head>`        |
| 3   | `<head>`         | Empty `<title>`                                            | Add meaningful page title             |
| 4   | Below `<h1>`     | `<h3>` used after `<h1>` — level skipped                   | Change to `<h2>`                      |
| 5   | Navigation       | `<div>` used instead of `<nav>`                            | Replace with `<nav aria-label="...">` |
| 6   | Navigation       | Links are bare `<a>` tags — no list                        | Wrap in `<ul><li>`                    |
| 7   | Resources        | External links missing `rel="noopener noreferrer"`         | Add the attribute to both links       |
| 8   | Resources        | `<a>` missing `href`                                       | Add a valid `href` value              |
| 9   | Resources        | `target="_blank"` on `mailto:` link                        | Remove `target="_blank"`              |
| 10  | Resources        | Download link missing `download` attribute                 | Add `download="filename.pdf"`         |
| 11  | VS Code setup    | `<ul>` used for ordered steps                              | Change to `<ol>`                      |
| 12  | Technologies     | `<ol>` used for unordered items                            | Change to `<ul>`                      |
| 13  | Frontend/Backend | Nested `<ul>` placed after `</li>`                         | Move inside parent `<li>`             |
| 14  | Key Terms        | `<ul>` used for term-definition pairs                      | Replace with `<dl>`, `<dt>`, `<dd>`   |
| 15  | Table            | Missing `<caption>`, `<thead>`, `<tbody>`, `<th>`, `scope` | Full table restructure                |
| B-A | Contact          | `<b>` and `<i>` used instead of `<strong>` and `<em>`      | Replace with semantic elements        |
| B-B | Contact          | `<br><br>` used between paragraphs                         | Remove — `<p>` spacing is enough      |
