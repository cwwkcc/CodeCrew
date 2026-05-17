Embedded content brings external media — video, audio, maps, other documents — into the page. The main elements are `<video>`, `<audio>`, `<iframe>`, and `<picture>` for responsive images with multiple sources.

---

## `<video>`

```html
<video
  controls
  width="800"
  height="450"
  poster="thumbnail.jpg"
  preload="metadata"
>
  <source src="lecture.webm" type="video/webm">
  <source src="lecture.mp4"  type="video/mp4">
  <track kind="subtitles" src="captions-en.vtt" srclang="en" label="English" default>
  <track kind="subtitles" src="captions-si.vtt" srclang="si" label="Sinhala">
  <p>Your browser does not support HTML video.
     <a href="lecture.mp4">Download the video instead.</a>
  </p>
</video>
```

The browser tries each `<source>` in order and uses the first format it supports. The paragraph fallback only shows on very old browsers.

### Video Attributes

|Attribute|Purpose|
|---|---|
|`controls`|Show the browser's default playback controls|
|`autoplay`|Play immediately on load — requires `muted` on most browsers|
|`muted`|Start without audio (required for `autoplay` in Chrome/Firefox)|
|`loop`|Replay from the start when it ends|
|`poster`|Image shown before playback starts|
|`preload`|`none` / `metadata` / `auto` — how much to load before play|
|`width` / `height`|Reserve layout space — prevents the page jumping when the video loads|
|`playsinline`|On iOS, play inline instead of going fullscreen automatically|

### `<track>` — Subtitles and Captions

```html
<track kind="subtitles"    src="en.vtt"    srclang="en" label="English" default>
<track kind="captions"     src="en-cc.vtt" srclang="en" label="English (CC)">
<track kind="chapters"     src="chapters.vtt">
<track kind="descriptions" src="descriptions.vtt">
```

|`kind`|Purpose|
|---|---|
|`subtitles`|Translation of spoken dialogue|
|`captions`|Dialogue + sound effects + speaker labels (for deaf users)|
|`chapters`|Chapter markers for navigation|
|`descriptions`|Text descriptions of visual content (for blind users)|

`default` on a `<track>` enables it automatically if the user has not expressed a preference.

---

## `<audio>`

Identical pattern to `<video>`, without the visual dimensions:

```html
<audio controls preload="metadata">
  <source src="podcast.ogg" type="audio/ogg">
  <source src="podcast.mp3" type="audio/mpeg">
  <source src="podcast.aac" type="audio/aac">
  Your browser does not support HTML audio.
</audio>
```

Supports the same attributes — `controls`, `autoplay`, `muted`, `loop`, `preload`.

---

## `<picture>` — Responsive Images with Multiple Sources

`<picture>` lets the browser choose the most appropriate image file based on screen size or format support:

```html
<picture>
  <!-- Modern format — used if the browser supports AVIF -->
  <source
    type="image/avif"
    srcset="hero-400.avif 400w, hero-800.avif 800w, hero-1200.avif 1200w"
    sizes="(max-width: 600px) 400px, (max-width: 1024px) 800px, 1200px"
  >
  <!-- Fallback modern format -->
  <source
    type="image/webp"
    srcset="hero-400.webp 400w, hero-800.webp 800w, hero-1200.webp 1200w"
    sizes="(max-width: 600px) 400px, (max-width: 1024px) 800px, 1200px"
  >
  <!-- Final fallback — always required, carries the alt text -->
  <img src="hero-800.jpg" alt="Students in a classroom" width="800" height="450">
</picture>
```

The browser tries `<source>` elements in order and uses the first one it supports. The `<img>` at the bottom is mandatory — it is the fallback and it always carries the `alt` attribute.

### Art Direction — Different Crops per Screen Size

```html
<picture>
  <!-- Portrait crop for narrow screens -->
  <source media="(max-width: 599px)" srcset="hero-portrait.jpg">
  <!-- Landscape crop for wide screens -->
  <source media="(min-width: 600px)" srcset="hero-landscape.jpg">
  <img src="hero-landscape.jpg" alt="Team photo">
</picture>
```

---

## `<iframe>`

Embeds a completely separate HTML document inside the current page. Used for maps, video embeds, and third-party widgets:

```html
<iframe
  src="https://www.openstreetmap.org/export/embed.html?bbox=79.8,6.9,80.0,7.0"
  width="600"
  height="400"
  title="Map of Colombo"
  loading="lazy"
>
</iframe>
```

### iframe Attributes

|Attribute|Purpose|
|---|---|
|`title`|Accessible label — required. Screen readers announce this.|
|`loading="lazy"`|Delays loading until the iframe is near the viewport|
|`sandbox`|Restricts what the embedded content is allowed to do|
|`allowfullscreen`|Allows the iframe to go fullscreen (needed for YouTube/Vimeo)|
|`width` / `height`|Sets the dimensions|

### `sandbox` — Restricting iframes

An empty `sandbox` applies maximum restrictions. Add permissions back individually as needed:

```html
<!-- Maximum restriction — no scripts, no forms, nothing -->
<iframe src="untrusted.html" sandbox></iframe>

<!-- Allow scripts only -->
<iframe src="widget.html" sandbox="allow-scripts"></iframe>

<!-- Allow scripts and treat as same origin -->
<iframe src="widget.html" sandbox="allow-scripts allow-same-origin"></iframe>
```

|Permission|What it allows|
|---|---|
|`allow-scripts`|JavaScript execution|
|`allow-same-origin`|Access cookies and storage as if same origin|
|`allow-forms`|Form submission|
|`allow-popups`|Opening new tabs/windows|
|`allow-downloads`|Triggering file downloads|

### Embedding YouTube

```html
<iframe
  width="560"
  height="315"
  src="https://www.youtube.com/embed/dQw4w9WgXcQ"
  title="Video title"
  allowfullscreen
  loading="lazy"
>
</iframe>
```

---

## `<embed>` and `<object>`

Legacy elements. Their only remaining common use is embedding PDFs:

```html
<object data="document.pdf" type="application/pdf" width="100%" height="600">
  <p>PDF cannot be displayed. <a href="document.pdf">Download instead.</a></p>
</object>
```

Even this is inconsistent across browsers. For modern projects, link to the PDF with `<a>` and let the browser or OS handle opening it.

---

## Important Notes

- Always set `width` and `height` on `<video>` and `<img>` — this reserves space so the page does not jump when media loads.
- Always include `title` on `<iframe>` — screen readers need it to describe what is embedded.
- Always include `alt` on `<img>` inside `<picture>` — it is the accessible description of the image.
- Use `loading="lazy"` on iframes and images that are not visible when the page first loads.

---

_Next: [HTML Entities and Character References](https://claude.ai/chat/10%20-%20HTML%20Entities%20and%20Character%20References.md)_