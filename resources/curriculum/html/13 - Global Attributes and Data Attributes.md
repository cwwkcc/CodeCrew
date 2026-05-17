Global attributes are available on every HTML element regardless of type. Data attributes let you attach custom information to any element for use by CSS or JavaScript.

---

## Global Attributes

|Attribute|Purpose|
|---|---|
|`id`|Unique identifier within the document|
|`class`|Space-separated CSS class names|
|`lang`|Language of this element's content|
|`dir`|Text direction: `ltr`, `rtl`, `auto`|
|`title`|Advisory tooltip text|
|`hidden`|Hides the element from rendering and assistive technology|
|`tabindex`|Keyboard tab order|
|`contenteditable`|Makes the element editable by the user|
|`draggable`|Marks the element as draggable|
|`spellcheck`|Enable or disable spell checking|
|`translate`|Whether to translate this content: `yes` / `no`|
|`accesskey`|Keyboard shortcut (use carefully — conflicts with OS shortcuts)|
|`style`|Inline CSS (prefer external stylesheets)|
|`slot`|Used with Web Components|

---

## `id`

Unique identifier for an element — no two elements in the same document should share an `id`:

```html
<section id="about">
  <h2 id="about-heading">About Paideon</h2>
</section>
```

Uses:

- Fragment links: `<a href="#about">` scrolls to the element
- CSS selectors: `#about { ... }`
- Label association: `<label for="email">` + `<input id="email">`
- ARIA references: `aria-labelledby="about-heading"`
- JavaScript targeting: `document.getElementById('about')`

`id` values are case-sensitive and cannot contain spaces. Letters, digits, hyphens, and underscores are safe.

---

## `class`

One or more space-separated class names:

```html
<button class="btn btn-primary btn-large">Submit</button>
<p class="text-muted small italic">Helper text</p>
```

An element can have as many classes as needed. Classes are not unique — many elements can share the same class. Used heavily for CSS targeting and JavaScript selection.

---

## `lang`

Declares the language of an element's content. Screen readers use it to select the correct pronunciation engine:

```html
<!-- Document language on <html> -->
<html lang="en">

<!-- Override for a specific phrase -->
<p>
  The phrase <span lang="fr">c'est la vie</span> means "that's life".
</p>

<!-- Sinhala content -->
<p lang="si">ආයුබෝවන්</p>
```

---

## `dir`

Text direction:

```html
<p dir="ltr">Left-to-right text (English, Sinhala)</p>
<p dir="rtl">Right-to-left text (Arabic, Hebrew)</p>
<p dir="auto">Let the browser detect direction</p>

<!-- Override for a specific word in a bidirectional paragraph -->
<p>
  The Arabic word <span dir="rtl" lang="ar">مرحبا</span> means "hello".
</p>
```

---

## `title`

Advisory tooltip text, shown on hover in most browsers:

```html
<abbr title="HyperText Markup Language">HTML</abbr>
<button title="Save your work (Ctrl+S)">Save</button>
<td title="Score for week 3">87</td>
```

`title` is **not** a substitute for `alt` on images or `<label>` on inputs. Screen readers handle it inconsistently — do not rely on it as the only accessible name. Use it only for supplementary, non-essential information.

---

## `hidden`

Hides an element completely — visually and from assistive technology:

```html
<!-- Hidden — not rendered, not announced by screen readers -->
<div hidden>This is not visible.</div>

<!-- Revealed by removing the attribute -->
<div id="panel" hidden>Panel content</div>
```

`hidden` is equivalent to `display: none`. It is different from CSS `visibility: hidden` (which hides visually but preserves space) or `opacity: 0` (which hides visually but the element is still accessible).

---

## `tabindex`

Controls whether and where an element appears in the keyboard Tab sequence:

```html
<!-- Add to an element that should receive programmatic focus but not tab to -->
<div tabindex="-1" id="main-content">...</div>

<!-- Add a non-interactive element to the tab order -->
<div role="button" tabindex="0" onclick="handleClick()">Custom widget</div>
```

|Value|Effect|
|---|---|
|Not present|Uses natural focusability (buttons, links, inputs are natively focusable)|
|`0`|Adds element to tab order at its natural DOM position|
|`-1`|Removed from tab order; can be focused via JavaScript|
|Positive (`1`, `2`, ...)|Avoid — overrides natural order and causes confusing navigation|

---

## `contenteditable`

Makes an element's content editable in the browser, like a simple text area:

```html
<div contenteditable="true">
  Click here to edit this text.
</div>

<!-- Prevent editing -->
<div contenteditable="false">Read only.</div>
```

Used for rich text editors and inline editing UIs. The element does not submit with a form — JavaScript is needed to capture its content.

---

## `draggable`

Marks an element as draggable via the HTML Drag and Drop API:

```html
<div draggable="true" id="item-1">Drag me</div>
<div draggable="false">Cannot drag</div>
```

Images and links are draggable by default. Other elements are not.

---

## `spellcheck`

```html
<textarea spellcheck="true">Check my spelling.</textarea>
<textarea spellcheck="false">No spell checking here — e.g. code editor.</textarea>
```

Useful to disable in elements that contain code, passwords, or structured data where spell check would flag valid content as errors.

---

## `translate`

Tells translation tools (browser translate, Google Translate) whether to translate this content:

```html
<!-- Translate this normally -->
<p translate="yes">Welcome to our website.</p>

<!-- Do not translate — brand name, code, proper noun -->
<span translate="no">Paideon</span>
<code translate="no">npm install react</code>
```

---

## Data Attributes: `data-*`

Data attributes store custom, application-specific data directly on HTML elements, without inventing non-standard attributes or hidden inputs:

```html
<li
  data-user-id="cuid-abc123"
  data-role="admin"
  data-score="92"
  data-last-login="2025-11-01"
>
  Ashan Kavinda
</li>
```

### Rules

- Must start with `data-`
- Followed by at least one character
- No uppercase letters in the attribute name
- No semicolons

### Reading Data Attributes in HTML

Data attributes are primarily read by CSS and JavaScript, but they also encode meaningful state in the HTML for anyone reading the source:

```html
<!-- State encoded in data attributes -->
<button data-state="loading" disabled>Saving...</button>
<button data-state="idle">Save</button>

<!-- Configuration for a component -->
<div
  data-carousel
  data-autoplay="true"
  data-interval="3000"
  data-loop="true"
>
  ...
</div>
```

### Targeting with CSS Attribute Selectors

```css
/* Style based on data attribute value */
[data-role="admin"]  { font-weight: 700; color: crimson; }
[data-state="loading"] { opacity: 0.6; cursor: wait; }
[data-state="error"]   { border-color: crimson; }

/* Any element that has this attribute, regardless of value */
[data-tooltip] { cursor: help; }
```

### Naming Convention

Use lowercase kebab-case. Camel-case names are automatically converted:

```html
<div data-user-id="123" data-first-name="Ashan"></div>
```

```
HTML:        data-user-id      → dataset.userId
HTML:        data-first-name   → dataset.firstName
HTML:        data-max-value    → dataset.maxValue
```

The `dataset` API converts kebab-case to camelCase automatically.

---

