## Why Semantics Matter

Before HTML5, developers wrapped everything in `<div>` tags and used CSS class names for meaning (`<div class="header">`, `<div class="nav">`). HTML5 introduced landmark and sectioning elements so the _markup itself_ communicates structure — not just class names.

Semantic elements benefit: accessibility (screen readers navigate by landmarks), SEO (search engines understand page structure), maintainability (code is self-documenting).

---

## Page-Level Landmark Elements

```
<body>
  <header>         Site header / article header
  <nav>            Navigation links
  <main>           Primary page content — ONE per page
    <article>      Self-contained content (blog post, news item)
      <header>     Article header
      <section>    Thematic grouping within article/body
      <aside>      Tangentially related content (pull quote, sidebar)
      <footer>     Article footer
    </article>
    <aside>        Page-level sidebar
  </main>
  <footer>         Site footer
</body>
```

### `<header>`

Introductory content for its nearest sectioning ancestor. May contain a logo, heading, navigation, search form. Can appear multiple times (once for `<body>`, once per `<article>`).

### `<nav>`

A major block of navigation links. Not every group of links — only primary navigation structures. A page may have several `<nav>` elements (main nav, breadcrumbs, pagination).

### `<main>`

The dominant content of the `<body>`. Must be unique — exactly one visible `<main>` per page. Excludes content repeated across pages (header, nav, footer).

### `<article>`

Self-contained, independently distributable content. Ask: "could this be syndicated in an RSS feed or published elsewhere and still make sense?" If yes → `<article>`. Examples: blog posts, news stories, forum posts, product cards.

### `<section>`

A thematic grouping of content, typically with a heading. Use when content belongs together but is not independently distributable. If you find yourself writing `<section>` with no heading, reconsider — a `<div>` may be more appropriate.

### `<aside>`

Content tangentially related to the surrounding content. In context of an `<article>`: pull quotes, related links, author bio boxes. At the page level: sidebars, advertising, supplementary information.

### `<footer>`

Closing content for its sectioning ancestor. At the page level: copyright, legal links, site map, social links. Within an `<article>`: author, publication date, tags, related links.

---

## Disclosure Widget: `<details>` and `<summary>`

A native HTML accordion with no JavaScript required:

```html
<details>
  <summary>What is the difference between TCP and UDP?</summary>
  <p>TCP provides reliable, ordered, error-checked delivery of a stream of bytes.
     UDP is connectionless and provides no delivery guarantees, but has lower latency.</p>
</details>
```

`<summary>` is the visible toggle. Everything else inside `<details>` is shown/hidden. Add the `open` attribute to have it expanded by default.

---

## `<dialog>`

A native modal or non-modal dialog:

```html
<dialog id="confirm-dialog">
  <p>Are you sure you want to delete this item?</p>
  <button onclick="this.closest('dialog').close('confirm')">Yes</button>
  <button onclick="this.closest('dialog').close()">No</button>
</dialog>
```

Open with `dialog.showModal()` (modal) or `dialog.show()` (non-modal) in JS. The `open` attribute can be set in HTML for non-modal use without JS.

---

## `<time>`

Machine-readable date and time:

```html
<time datetime="2025-08-15">Independence Day</time>
<time datetime="2025-08-15T09:00:00+05:30">9 AM IST</time>
<time datetime="PT2H30M">two and a half hours</time>  <!-- duration -->
```

The `datetime` attribute uses ISO 8601 format. Search engines and calendar apps can parse this to extract dates.

---

## Text-Level Semantic Elements

|Element|Meaning|
|---|---|
|`<address>`|Contact info for nearest article or body|
|`<cite>`|Title of a work being referenced|
|`<blockquote cite="URL">`|Extended quotation|
|`<ins datetime>`|Inserted content (tracked change)|
|`<del datetime>`|Deleted content (tracked change)|
|`<ruby>`, `<rt>`, `<rp>`|Ruby annotation (East Asian typography)|
|`<bdi>`|Bi-directional isolation (for user-generated text)|
|`<bdo dir>`|Override text directionality|