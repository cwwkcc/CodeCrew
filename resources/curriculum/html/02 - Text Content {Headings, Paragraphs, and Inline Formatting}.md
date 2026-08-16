## Headings: `<h1>` – `<h6>`

Headings create an _outline_ of the document. They are not font-size controls — that is CSS's job. They are semantic markers of section hierarchy.

```html
<h1>The History of Computing</h1>
<h2>Early Machines</h2>
<h3>The Difference Engine</h3>
<h3>The Analytical Engine</h3>
<h2>The Electronic Era</h2>
<h3>ENIAC</h3>
```

### Rules

- There should be exactly **one `<h1>`** per page — the document's main topic.
- Do not skip heading levels (e.g. jump from `<h2>` to `<h4>`). Screen readers navigate documents by heading outline, and gaps break that structure.
- Headings are for _document structure_, not for making text large. Never use `<h3>` just because you want medium-sized text.

---

## Paragraphs: `<p>`

The default container for prose text. The browser adds vertical margin above and below by default (via the browser stylesheet).

```html
<p>
  The first programmable electronic computer, ENIAC, was completed in 1945. It
  weighed 30 tons and occupied 1,800 square feet.
</p>
```

A common mistake: using `<br>` to create spacing between blocks of text. If the content is a new paragraph, use a new `<p>` element. `<br>` is only for line breaks _within_ a block — poetry, addresses, code fragments inside prose.

---

## Thematic Break: `<hr>`

A _thematic break_ — a shift in topic within a section. Rendered as a horizontal rule by default, but its meaning is semantic, not decorative.

```html
<p>Chapter content here.</p>
<hr />
<p>A different topic begins here.</p>
```

---

## Inline Formatting Elements

### Semantic inline elements (use these)

| Element                 | Meaning                                        | Default rendering          |
| ----------------------- | ---------------------------------------------- | -------------------------- |
| `<strong>`              | Strong importance, seriousness, or urgency     | Bold                       |
| `<em>`                  | Stress emphasis (changes meaning of sentence)  | Italic                     |
| `<mark>`                | Highlighted / relevant to current context      | Yellow background          |
| `<abbr title="...">`    | Abbreviation or acronym                        | Dotted underline           |
| `<cite>`                | Title of a creative work                       | Italic                     |
| `<code>`                | Inline code fragment                           | Monospace font             |
| `<kbd>`                 | Keyboard input                                 | Monospace, sometimes boxed |
| `<samp>`                | Sample output from a program                   | Monospace                  |
| `<var>`                 | Mathematical or programming variable           | Italic                     |
| `<time datetime="...">` | Machine-readable date/time                     | Plain text                 |
| `<dfn>`                 | Term being defined                             | Italic                     |
| `<q>`                   | Inline quotation                               | Adds quotation marks       |
| `<sup>`                 | Superscript                                    | Raised small text          |
| `<sub>`                 | Subscript                                      | Lowered small text         |
| `<s>`                   | No longer accurate or relevant (strikethrough) | Strikethrough              |
| `<u>`                   | Unarticulated annotation (e.g. misspelling)    | Underline                  |
| `<small>`               | Side comments, legal fine print                | Smaller text               |
| `<wbr>`                 | Word break opportunity (hint to browser)       | Nothing visible            |

### Presentational elements (avoid in most cases)

| Element | Issue                                                                                                             |
| ------- | ----------------------------------------------------------------------------------------------------------------- |
| `<b>`   | Bold without semantic meaning. Use `<strong>` unless you genuinely mean "stylistically offset without importance" |
| `<i>`   | Italic without semantic meaning. Use `<em>` unless you mean "technical term, foreign phrase, thought"             |

The distinction: `<strong>Warning: do not press.</strong>` — genuinely important. `<b>Ingredients</b>` in a recipe — a style choice, no semantic urgency.

### The neutral containers

| Element  | Type   | Purpose                                                      |
| -------- | ------ | ------------------------------------------------------------ |
| `<div>`  | Block  | Generic block container. Use when no semantic element fits.  |
| `<span>` | Inline | Generic inline container. Use when no semantic element fits. |

These carry no meaning on their own. They exist to be hooks for CSS and JavaScript. Over-use of `<div>` and `<span>` in place of semantic elements is called "div soup" — a code smell in HTML.

---

## Block Quotation: `<blockquote>`

For extended quotations from another source. The `cite` attribute can hold the source URL.

```html
<blockquote cite="https://example.com/source">
  <p>
    We shall fight on the beaches, we shall fight on the landing grounds, we
    shall fight in the fields and in the streets.
  </p>
</blockquote>
```

---

## Preformatted Text: `<pre>`

Preserves whitespace exactly as written — spaces, tabs, and newlines are not collapsed. Used for code blocks, ASCII art, and any content where exact spacing is meaningful. Usually combined with `<code>`:

```html
<pre><code>
int main() {
    return 0;
}
</code></pre>
```

---

## The `<address>` Element

Contact information for its nearest `<article>` or `<body>` ancestor. Commonly misused — it is for contact info, not for postal addresses in general.

```html
<address>
  Written by <a href="mailto:cinderax@example.com">Cinderax</a>.
</address>
```

---

## Key Principles

1. **Choose elements for meaning, not appearance.** The browser stylesheet is a hint, not the spec.
2. **One `<h1>` per page.** Build a proper heading outline.
3. **Don't use `<br>` as a paragraph separator.** Use `<p>`.
4. **Prefer semantic elements over `<b>` and `<i>`.** Use `<strong>` and `<em>`.
5. **Use `<div>` and `<span>` only when nothing semantic fits.**

---

## Common Mistakes

```html
<!-- WRONG: <br> used to fake paragraph spacing -->
Line one<br /><br /><br />Line two

<!-- CORRECT: two separate paragraphs, spaced with CSS margin -->
<p>Line one</p>
<p>Line two</p>

<!-- WRONG: heading levels chosen by font size wanted, not document outline -->
<h1>Page Title</h1>
<h3>Section Title</h3>
<!-- skipped h2 because "h3 looked the right size" -->

<!-- CORRECT: pick the level by outline position, size it with CSS -->
<h1>Page Title</h1>
<h2>Section Title</h2>

<!-- WRONG: <b> and <i> used for meaning -->
<p>Warning: <b>do not unplug the device</b>.</p>

<!-- CORRECT: <strong> and <em> carry meaning; <b>/<i> are purely visual and rarely what you want -->
<p>Warning: <strong>do not unplug the device</strong>.</p>
```

Heading levels describe a document **outline**, not a font-size ladder — a screen reader user often jumps between pages by heading level alone, so skipping from `<h1>` to `<h3>` genuinely breaks navigation for them, even though sighted users never notice.
