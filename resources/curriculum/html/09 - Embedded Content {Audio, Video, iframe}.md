## `<video>`

```html
<video controls width="800" height="450" poster="thumbnail.jpg" preload="metadata">
  <source src="lecture.webm" type="video/webm">
  <source src="lecture.mp4"  type="video/mp4">
  <track kind="subtitles" src="captions-en.vtt" srclang="en" label="English" default>
  <track kind="subtitles" src="captions-si.vtt" srclang="si" label="Sinhala">
  <p>Your browser does not support HTML video.
     <a href="lecture.mp4">Download the video.</a>
  </p>
</video>
```

|Attribute|Purpose|
|---|---|
|`controls`|Show browser's playback controls|
|`autoplay`|Start playing immediately (requires `muted` on most browsers)|
|`muted`|Start muted|
|`loop`|Repeat playback|
|`poster`|Image shown before playback starts|
|`preload`|`none` / `metadata` / `auto` — how much to buffer on load|
|`width` / `height`|Reserve layout space|

Multiple `<source>` elements provide codec fallbacks. The `<track>` element adds subtitles and captions (WebVTT format). The fallback paragraph shows for very old browsers.

---

## `<audio>`

```html
<audio controls>
  <source src="podcast.ogg" type="audio/ogg">
  <source src="podcast.mp3" type="audio/mpeg">
  Your browser does not support audio.
</audio>
```

Same pattern as `<video>` but without `width`, `height`, or `poster`.

---

## `<iframe>`

Embeds another HTML document inside the current page:

```html
<iframe
  src="https://www.openstreetmap.org/export/embed.html?bbox=79.8,6.9,80.0,7.0"
  width="600"
  height="400"
  title="Map of Colombo"
  loading="lazy"
  sandbox="allow-scripts allow-same-origin">
</iframe>
```

|Attribute|Purpose|
|---|---|
|`title`|Accessible label — required|
|`loading="lazy"`|Defers loading until near viewport|
|`sandbox`|Restricts what embedded content can do|
|`allow`|Feature policy (camera, fullscreen, etc.)|
|`referrerpolicy`|Controls Referer header|

### `sandbox` values

An empty `sandbox=""` applies all restrictions. Add permissions selectively:

|Permission|Allows|
|---|---|
|`allow-scripts`|JavaScript execution|
|`allow-same-origin`|Treat content as same origin|
|`allow-forms`|Form submission|
|`allow-popups`|Opening new windows|
|`allow-top-navigation`|Navigating the top browsing context|

---

## `<embed>` and `<object>`

Legacy elements for plugins (Flash, Java applets). Avoid. Their only remaining practical use is embedding PDFs natively:

```html
<object data="document.pdf" type="application/pdf" width="100%" height="600">
  <p><a href="document.pdf">Download PDF</a></p>
</object>
```

---