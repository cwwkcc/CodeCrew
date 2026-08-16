## The `<img>` Element

```html
<img
  src="photo.jpg"
  alt="A golden retriever sitting on grass"
  width="800"
  height="600"
/>
```

`<img>` is a void element — no closing tag. Its four core attributes:

| Attribute | Required    | Purpose                      |
| --------- | ----------- | ---------------------------- |
| `src`     | Yes         | URL of the image file        |
| `alt`     | Yes*        | Alternative text description |
| `width`   | Recommended | Intrinsic width in pixels    |
| `height`  | Recommended | Intrinsic height in pixels   |

*`alt` is technically optional in markup but omitting it is almost always an accessibility defect. The only exception is a decorative image where `alt=""` (empty string) tells screen readers to skip it entirely.

---

## Writing Good `alt` Text

The `alt` attribute must convey the _meaning_ the image provides, not describe it literally:

```html
<!-- BAD: describes file, not meaning -->
<img src="chart.png" alt="chart.png" />

<!-- BAD: redundant prefix -->
<img src="chart.png" alt="Image of a bar chart" />

<!-- GOOD: conveys the information the image communicates -->
<img
  src="chart.png"
  alt="Bar chart showing monthly sales. August peaked at 4,200 units."
/>

<!-- GOOD: decorative image (ornamental divider) -->
<img src="divider.png" alt="" />
```

---

## `width` and `height`: Why They Matter

Specifying dimensions lets the browser reserve layout space before the image loads. Without them, the page _reflows_ (jumps around) as images load — harming Cumulative Layout Shift (CLS), a Core Web Vital. Always specify intrinsic dimensions.

---

## Responsive Images: `srcset` and `sizes`

For different screen densities or viewport sizes:

```html
<!-- Density-switching: serve 2× image on high-DPI screens -->
<img
  src="photo-800w.jpg"
  srcset="photo-800w.jpg 1x, photo-1600w.jpg 2x"
  alt="Mountain landscape"
/>

<!-- Width-based: serve different sizes at different viewport widths -->
<img
  src="photo-800w.jpg"
  srcset="photo-400w.jpg 400w, photo-800w.jpg 800w, photo-1200w.jpg 1200w"
  sizes="(max-width: 600px) 400px, (max-width: 1000px) 800px, 1200px"
  alt="Mountain landscape"
/>
```

---

## Art Direction: `<picture>`

Use `<picture>` when you need to serve a _completely different image crop_ at different viewport widths (not just a different resolution):

```html
<picture>
  <source media="(max-width: 600px)" srcset="hero-mobile.jpg" />
  <source media="(max-width: 1200px)" srcset="hero-tablet.jpg" />
  <img src="hero-desktop.jpg" alt="Aerial view of Colombo city" />
</picture>
```

The browser uses the first `<source>` whose `media` condition matches, and falls back to the `<img>` if none match. The `<img>` is mandatory — it provides the `alt` text and the ultimate fallback.

---

## Format Selection with `<picture>`

```html
<picture>
  <source type="image/avif" srcset="photo.avif" />
  <source type="image/webp" srcset="photo.webp" />
  <img src="photo.jpg" alt="Description" />
</picture>
```

Serves the most modern, efficient format the browser supports, with JPEG as fallback.

---

## `<figure>` and `<figcaption>`

Groups an image (or any self-contained content) with its caption:

```html
<figure>
  <img src="diagram.png" alt="OSI model layer diagram" />
  <figcaption>
    Figure 1. The seven layers of the OSI network model. Image adapted from
    <cite>Computer Networks</cite> by Tanenbaum.
  </figcaption>
</figure>
```

`<figure>` is not exclusively for images. It can contain code blocks, quotes, tables, or any content that is _referenced from the main content but could be moved without breaking the document's flow_.

---

## Image Formats Reference

| Format | Extension | Best for                                                      |
| ------ | --------- | ------------------------------------------------------------- |
| JPEG   | `.jpg`    | Photographs, complex gradients                                |
| PNG    | `.png`    | Images requiring transparency, screenshots, line art          |
| WebP   | `.webp`   | General-purpose: better compression than JPEG and PNG         |
| AVIF   | `.avif`   | Best compression (2024 recommendation), newer browser support |
| SVG    | `.svg`    | Icons, logos, illustrations — vector, scales infinitely       |
| GIF    | `.gif`    | Avoid; use WebP or `<video>` for animation                    |

---

## Common Mistakes

```html
<!-- WRONG: missing alt entirely — screen readers announce the whole file name -->
<img src="staff-photo-2026-final-v3.jpg" />

<!-- CORRECT: alt describes the content or purpose, not the filename -->
<img
  src="staff-photo-2026-final-v3.jpg"
  alt="ICT teacher demonstrating a robotics kit"
/>

<!-- CORRECT: purely decorative image gets an empty alt, not a missing one —
     this tells assistive tech to skip it silently rather than guess -->
<img src="divider-swirl.svg" alt="" />

<!-- WRONG: no width/height — the browser doesn't know the image's aspect
     ratio until it downloads, so the layout jumps as it loads in -->
<img src="hero.jpg" alt="Students at the science fair" />

<!-- CORRECT: intrinsic size reserved up front, prevents layout shift -->
<img
  src="hero.jpg"
  alt="Students at the science fair"
  width="1200"
  height="600"
/>
```

- **Using a CSS `background-image` for content that matters.** A photo of the principal in the "About" page is content — it belongs in `<img>` with real `alt` text. Background images are for decoration only; they're invisible to screen readers and search engines by design.
- **Wrapping every image in `<figure>`.** Reserve `<figure>`/`<figcaption>` for images that need an actual caption or are referenced from the surrounding text — not as a generic image container.
