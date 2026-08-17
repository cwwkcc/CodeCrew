An accessible website works for users who navigate with a keyboard, use screen readers, rely on voice control software, or have cognitive disabilities. Accessibility is not a feature you add at the end — it is a quality of correct HTML from the start.

---

## The Foundation: Semantic HTML

The most powerful accessibility tool is simply using the right HTML element. Semantic elements carry built-in roles, states, and keyboard behaviour that assistive technologies understand:

```html
<!-- WRONG — a div has no role, no keyboard behaviour, no meaning -->
<div onclick="submitForm()">Submit</div>

<!-- RIGHT — a button is focusable, activatable via Enter/Space, announced as "button" -->
<button type="submit">Submit</button>
```

```html
<!-- WRONG — span has no link semantics -->
<span onclick="goTo('/about')">About</span>

<!-- RIGHT — <a> is keyboard-navigable, announced as a link, right-clickable -->
<a href="/about">About</a>
```

Use the correct element and you get accessibility for free. Use a `<div>` and you must rebuild all of that manually with ARIA.

---

## Landmark Elements

Landmark elements divide the page into regions. Screen reader users can jump directly between them:

```html
<header>
  <!-- Site header / banner -->
  <nav>
    <!-- Navigation links -->
    <main>
      <!-- Primary page content — one per page -->
      <aside>
        <!-- Supplementary content (sidebar, related links) -->
        <footer>
          <!-- Footer -->
          <section>
            <!-- Thematic grouping — needs an accessible name -->
            <article><!-- Self-contained content (blog post, card) --></article>
          </section>
        </footer>
      </aside>
    </main>
  </nav>
</header>
```

```html
<!-- Full page structure -->
<body>
  <header>
    <nav aria-label="Main navigation">...</nav>
  </header>

  <main id="main-content">
    <h1>Page Title</h1>
    <article>...</article>
    <aside aria-label="Related resources">...</aside>
  </main>

  <footer>
    <nav aria-label="Footer navigation">...</nav>
  </footer>
</body>
```

---

## Heading Hierarchy

Headings create a document outline. Screen readers use them as navigation shortcuts:

```html
<!-- RIGHT — logical hierarchy, no levels skipped -->
<h1>JavaScript</h1>
<h2>Functions</h2>
<h3>Arrow Functions</h3>
<h3>Closures</h3>
<h2>Objects</h2>
<h3>Prototypes</h3>

<!-- WRONG — skipping from h1 to h3 breaks the outline -->
<h1>JavaScript</h1>
<h3>Functions</h3>
<!-- skipped h2 -->
```

Rules:

- One `<h1>` per page — the page title
- Never skip levels (no jumping from `<h2>` to `<h4>`)
- Use headings for document structure, not for font size — use CSS for that

---

## Images

Every `<img>` needs an `alt` attribute:

```html
<!-- Informative image — describe what it shows -->
<img
  src="diagram.png"
  alt="Flowchart showing how a request passes through middleware"
/>

<!-- Decorative image — empty alt, screen reader skips it -->
<img src="divider.svg" alt="" />

<!-- Image as a link — describe the destination, not the image -->
<a href="/home">
  <img src="logo.png" alt="Paideon — home" />
</a>

<!-- Complex image — long description via aria-describedby -->
<img
  src="chart.png"
  alt="Bar chart showing monthly signups"
  aria-describedby="chart-desc"
/>
<p id="chart-desc">In January there were 120 signups, February had 145...</p>
```

Writing bad alt text is worse than no alt — "image.png" or "photo" tells a screen reader user nothing. Describe the purpose of the image, not its appearance.

---

## Forms

Every input needs a label, and the label must be programmatically associated:

```html
<!-- RIGHT — for/id association -->
<label for="email">Email address</label>
<input type="email" id="email" name="email" required autocomplete="email" />

<!-- RIGHT — wrapping label (implicit association) -->
<label>
  Email address
  <input type="email" name="email" required />
</label>

<!-- WRONG — placeholder is not a label -->
<input type="email" placeholder="Email address" />
<!-- no label at all -->

<!-- WRONG — visually nearby but not associated -->
<p>Email address</p>
<input type="email" />
```

### Grouping Related Inputs

```html
<!-- Radio buttons and checkboxes need fieldset + legend -->
<fieldset>
  <legend>Preferred contact method</legend>
  <label><input type="radio" name="contact" value="email" /> Email</label>
  <label><input type="radio" name="contact" value="phone" /> Phone</label>
  <label><input type="radio" name="contact" value="sms" /> SMS</label>
</fieldset>
```

### Error Messages

```html
<label for="username">Username</label>
<input
  type="text"
  id="username"
  aria-describedby="username-error"
  aria-invalid="true"
/>
<p id="username-error" role="alert">Username must be at least 3 characters.</p>
```

`aria-describedby` links the error message to the input. `aria-invalid="true"` signals the field is in an error state. `role="alert"` causes screen readers to announce the error immediately.

---

## ARIA — Accessible Rich Internet Applications

ARIA adds semantic meaning to elements that HTML alone cannot express. **The first rule of ARIA: don't use ARIA if a native HTML element already works.**

```html
<!-- Don't do this -->
<div role="button" tabindex="0" onclick="handleClick()">Click me</div>

<!-- Do this — <button> already has role="button", tabindex, keyboard handling -->
<button onclick="handleClick()">Click me</button>
```

ARIA is for situations where no native HTML element fits — custom dropdowns, modals, tabs, sliders, etc.

### `aria-label` and `aria-labelledby`

Provide accessible names to elements that have no visible text label:

```html
<!-- aria-label — direct string -->
<button aria-label="Close dialog">×</button>
<nav aria-label="Breadcrumb">...</nav>
<input type="search" aria-label="Search courses" />

<!-- aria-labelledby — points to another element's id -->
<section aria-labelledby="section-heading">
  <h2 id="section-heading">Recent Courses</h2>
  ...
</section>

<dialog aria-labelledby="dialog-title">
  <h2 id="dialog-title">Confirm deletion</h2>
  ...
</dialog>
```

### `aria-describedby`

Links a longer description to an element — read after the label:

```html
<input type="password" aria-describedby="password-requirements" />
<p id="password-requirements">
  Must be 8+ characters with one uppercase letter and one number.
</p>
```

### `aria-expanded` and `aria-controls`

For toggle patterns — menus, accordions, drawers:

```html
<button aria-expanded="false" aria-controls="menu-list">Menu</button>
<ul id="menu-list" hidden>
  <li><a href="/">Home</a></li>
  <li><a href="/about">About</a></li>
</ul>
```

When the menu opens, update `aria-expanded="true"` and remove the `hidden` attribute.

### `aria-hidden`

Hides content from assistive technology without hiding it visually:

```html
<!-- Decorative icon next to a labelled button — icon is redundant -->
<button>
  <svg aria-hidden="true" focusable="false">...</svg>
  Save document
</button>

<!-- Decorative separator -->
<span aria-hidden="true">|</span>
```

Do not use `aria-hidden="true"` on a focusable element — keyboard users would still reach it but screen readers would not announce it.

### `aria-live` — Dynamic Content

Announces content changes to screen readers without requiring focus:

```html
<!-- polite — waits for the user to finish what they are doing before announcing -->
<div aria-live="polite" aria-atomic="true">Search returned 24 results.</div>

<!-- assertive — interrupts immediately — use only for urgent errors -->
<div aria-live="assertive" role="alert">Error: Your session has expired.</div>
```

`aria-atomic="true"` means the entire region is announced as a unit rather than just the changed portion.

---

## Keyboard Navigation

All interactive elements must be reachable and operable with the keyboard alone:

```html
<!-- Natural tab order — all natively focusable -->
<a href="/about">About</a>
<button>Submit</button>
<input type="text" />
<select>
  <option>Option</option>
</select>
<textarea></textarea>

<!-- Add to non-interactive elements that need programmatic focus -->
<div tabindex="-1" id="modal-content">...</div>

<!-- Add to custom interactive widgets -->
<div role="button" tabindex="0">Custom button</div>
```

| `tabindex` value         | Effect                                               |
| ------------------------ | ---------------------------------------------------- |
| Not present              | Element uses its natural focusability                |
| `0`                      | Added to tab order in DOM order                      |
| `-1`                     | Removed from tab order, but focusable via JavaScript |
| Positive (e.g. `1`, `2`) | Avoid — breaks expected tab order                    |

Never use positive `tabindex` values. They override the natural order and create confusing navigation.

---

## Skip Link

The first focusable element on the page should be a "skip to content" link so keyboard users can bypass repeated navigation:

```html
<body>
  <a href="#main-content" class="skip-link">Skip to main content</a>

  <header>
    <nav>...</nav>
  </header>

  <main id="main-content">...</main>
</body>
```

The skip link is visually hidden until focused. When a keyboard user presses Tab on the page, it appears first. Clicking it jumps focus to `<main>`.

---

## Language Attribute

Always declare the document language on `<html>`:

```html
<html lang="en">
  <html lang="si">
    <!-- Sinhala -->
    <html lang="ta">
      <!-- Tamil -->
    </html>
  </html>
</html>
```

When a section of the page is in a different language, mark it:

```html
<p>
  The French phrase
  <span lang="fr">c'est la vie</span>
  means "that's life".
</p>
```

Screen readers use the `lang` attribute to select the correct pronunciation engine. Without it, text may be pronounced incorrectly.

---

## Checklist

```
Semantic HTML
  [ ] Native elements used (button, a, input) — no div/span pretending to be interactive
  [ ] Heading hierarchy is logical and unbroken
  [ ] Landmark regions present (header, nav, main, footer)

Images
  [ ] Every <img> has alt text
  [ ] Decorative images have alt=""
  [ ] Complex images have detailed descriptions

Forms
  [ ] Every input has an associated <label>
  [ ] Radio/checkbox groups use <fieldset> + <legend>
  [ ] Error messages linked via aria-describedby

Keyboard
  [ ] All interactive elements are reachable via Tab
  [ ] No positive tabindex values
  [ ] Skip link present
  [ ] Focus is never trapped (except in modals where it should be)

ARIA
  [ ] aria-label on icon-only buttons
  [ ] aria-expanded on toggleable elements
  [ ] aria-live on dynamic regions
  [ ] aria-hidden on decorative content

Language
  [ ] <html lang="..."> is set
```

---

---

## Common Mistakes

```html
<!-- WRONG: ARIA bolted onto a div that could've just been a real button —
     now you have to hand-implement everything the browser gives <button> for free -->
<div role="button" onclick="submit()">Submit</div>

<!-- CORRECT: the native element already does all of this — keyboard focus,
     Enter/Space activation, correct screen reader announcement -->
<button type="button" onclick="submit()">Submit</button>

<!-- WRONG: redundant ARIA on an element that already announces itself correctly -->
<button aria-label="Submit">Submit</button>

<!-- CORRECT: aria-label is for when there's no visible text, not a repeat of it -->
<button aria-label="Close dialog">✕</button>
```

**The first rule of ARIA is: don't use ARIA if a native HTML element already does the job.** `<nav>`, `<button>`, `<a>`, and `<input>` all come with built-in roles, keyboard behavior, and states — reimplementing that with `<div role="...">` and JavaScript is more code and more ways to get it wrong. Bad ARIA (wrong role, stale `aria-expanded`, a label that contradicts the visible text) is often worse than no ARIA at all, because screen reader users trust it and get actively misled.
