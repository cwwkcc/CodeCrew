The `<head>` element contains metadata about the document — information the browser, search engines, and social media platforms need, but which is not displayed directly on the page. Nothing in `<head>` is visible to the user except the `<title>`.

---

## Full `<head>` Reference

```html
<head>
  <!-- 1. Character encoding — must be first, before anything else -->
  <meta charset="UTF-8">

  <!-- 2. Viewport — required for responsive design -->
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <!-- 3. Page title — shown in browser tab, bookmarks, search results -->
  <title>Closures — JavaScript | Paideon</title>

  <!-- 4. SEO meta tags -->
  <meta name="description" content="A 150–160 character summary of the page for search results.">
  <meta name="author" content="Ashan Kavinda">
  <meta name="robots" content="index, follow">

  <!-- 5. Canonical URL — tells search engines which URL is the authoritative version -->
  <link rel="canonical" href="https://paideon.lk/courses/javascript/closures">

  <!-- 6. Open Graph — controls appearance when shared on Facebook, LinkedIn, etc. -->
  <meta property="og:title"       content="Closures — JavaScript">
  <meta property="og:description" content="Learn how closures work in JavaScript.">
  <meta property="og:image"       content="https://paideon.lk/og/closures.jpg">
  <meta property="og:url"         content="https://paideon.lk/courses/javascript/closures">
  <meta property="og:type"        content="article">
  <meta property="og:site_name"   content="Paideon">

  <!-- 7. Twitter Card — controls appearance when shared on Twitter/X -->
  <meta name="twitter:card"        content="summary_large_image">
  <meta name="twitter:title"       content="Closures — JavaScript">
  <meta name="twitter:description" content="Learn how closures work in JavaScript.">
  <meta name="twitter:image"       content="https://paideon.lk/og/closures.jpg">

  <!-- 8. Favicons -->
  <link rel="icon"             href="/favicon.ico"          sizes="any">
  <link rel="icon"             href="/favicon.svg"          type="image/svg+xml">
  <link rel="apple-touch-icon" href="/apple-touch-icon.png">
  <link rel="manifest"         href="/site.webmanifest">

  <!-- 9. Stylesheets -->
  <link rel="stylesheet" href="/css/main.css">

  <!-- 10. Font preloading — fetch critical fonts early -->
  <link rel="preload" href="/fonts/inter.woff2" as="font" type="font/woff2" crossorigin>

  <!-- 11. DNS prefetch / Preconnect — speed up third-party connections -->
  <link rel="preconnect"    href="https://fonts.googleapis.com">
  <link rel="dns-prefetch"  href="https://cdn.example.com">

  <!-- 12. Scripts -->
  <script src="/js/app.js" defer></script>
</head>
```

---

## `<meta charset>`

```html
<meta charset="UTF-8">
```

Must be the very first element inside `<head>`, before the `<title>`. Tells the browser how to decode the bytes of the file. UTF-8 supports every language and emoji — always use it.

---

## `<meta name="viewport">`

```html
<meta name="viewport" content="width=device-width, initial-scale=1.0">
```

Without this, mobile browsers zoom out and render the page at ~980px wide (desktop width), then scale it down. This makes text tiny and forces users to pinch-zoom.

`width=device-width` sets the viewport to the device's actual screen width. `initial-scale=1.0` prevents default zoom.

---

## `<title>`

```html
<title>Page Name — Site Name</title>
```

- Shown in the browser tab
- Shown as the clickable headline in search results
- Used as the default bookmark name
- Read aloud by screen readers when the page loads

Good title format: `Specific Page Name — Site Name`. Keep it 50–60 characters.

---

## SEO Meta Tags

### Description

```html
<meta name="description" content="Learn how closures work in JavaScript — what they are, why they exist, and how to use them in real code.">
```

- Shown as the snippet below the title in search results
- Does not directly affect ranking, but affects click-through rate
- Keep it 150–160 characters

### Robots

```html
<!-- Default — allow indexing and following links -->
<meta name="robots" content="index, follow">

<!-- Prevent indexing (e.g. admin pages, staging environments) -->
<meta name="robots" content="noindex, nofollow">

<!-- Index the page but don't follow its links -->
<meta name="robots" content="index, nofollow">
```

### Canonical URL

```html
<link rel="canonical" href="https://paideon.lk/courses/javascript/closures">
```

When the same content is accessible at multiple URLs (e.g. with and without trailing slash, with query parameters), the canonical tag tells search engines which version is the "real" one and consolidates their ranking signals.

---

## Open Graph Tags

Open Graph controls how a page appears when shared on social media — Facebook, LinkedIn, WhatsApp, Slack, and others all read these tags:

```html
<meta property="og:title"       content="Page title — shown in the card">
<meta property="og:description" content="Description shown below the title.">
<meta property="og:image"       content="https://example.com/og-image.jpg">
<meta property="og:url"         content="https://example.com/this-page">
<meta property="og:type"        content="website">  <!-- or: article, product -->
<meta property="og:site_name"   content="Your Site Name">
```

The image should be at least 1200×630px. Without an `og:image`, social platforms show a generic preview or nothing.

---

## Twitter Card Tags

```html
<meta name="twitter:card"        content="summary_large_image">
<meta name="twitter:title"       content="Page title">
<meta name="twitter:description" content="Description">
<meta name="twitter:image"       content="https://example.com/twitter-image.jpg">
<meta name="twitter:site"        content="@YourHandle">
```

|`twitter:card` value|Layout|
|---|---|
|`summary`|Small thumbnail, title, description|
|`summary_large_image`|Large image across the full width of the card|
|`player`|Embedded video/audio player|

---

## Favicons

The browser icon shown in the tab and bookmarks:

```html
<!-- .ico file — supported by all browsers including very old ones -->
<link rel="icon" href="/favicon.ico" sizes="any">

<!-- SVG — scales perfectly, supports dark mode -->
<link rel="icon" href="/favicon.svg" type="image/svg+xml">

<!-- Apple devices — used when saving to home screen -->
<link rel="apple-touch-icon" href="/apple-touch-icon.png">  <!-- 180×180px -->

<!-- Web App Manifest — for PWA install prompts -->
<link rel="manifest" href="/site.webmanifest">
```

The browser picks the most appropriate format. SVG is preferred where supported.

---

## Resource Hints

These tell the browser to prepare connections or downloads before they are explicitly needed:

```html
<!-- preload — fetch this resource NOW, at high priority -->
<link rel="preload" href="/fonts/inter.woff2" as="font" type="font/woff2" crossorigin>
<link rel="preload" href="/images/hero.jpg"   as="image">
<link rel="preload" href="/css/critical.css"  as="style">

<!-- preconnect — open TCP + TLS connection to this origin early -->
<link rel="preconnect" href="https://fonts.googleapis.com">

<!-- dns-prefetch — resolve the DNS for this host early (lighter than preconnect) -->
<link rel="dns-prefetch" href="https://cdn.example.com">

<!-- prefetch — low priority fetch of a resource likely needed on the NEXT page -->
<link rel="prefetch" href="/js/dashboard-bundle.js">
```

Use `preload` for critical resources on the current page (fonts, hero images). Use `preconnect` for third-party domains you will fetch from. Use `prefetch` for the next likely page.

---

## Script Loading Strategies

Where and how you load scripts determines whether they block the page from rendering:

```html
<!-- No attribute — blocks parsing entirely. Bad for body scripts. -->
<script src="app.js"></script>

<!-- defer — downloads in parallel, executes after HTML is fully parsed -->
<!-- Order is preserved. Best for most scripts. -->
<script src="app.js" defer></script>

<!-- async — downloads in parallel, executes immediately when ready -->
<!-- Order NOT guaranteed. Good for independent scripts like analytics. -->
<script src="analytics.js" async></script>

<!-- type="module" — ES module. Deferred by default. -->
<script type="module" src="app.js"></script>
```

|Strategy|Download|Execution|Order|
|---|---|---|---|
|_(none)_|Blocks parsing|Immediately|In order|
|`defer`|Parallel|After parsing|In order|
|`async`|Parallel|When ready|Not guaranteed|
|`type="module"`|Parallel|After parsing|In order|

The `<script>` tag is typically placed just before `</body>` or in `<head>` with `defer`.

---

## `<meta http-equiv>`

Used rarely. The main remaining use case is meta-refresh:

```html
<!-- Redirect to a new URL after 5 seconds -->
<meta http-equiv="refresh" content="5; url=https://new-location.com">

<!-- Immediate redirect -->
<meta http-equiv="refresh" content="0; url=https://new-location.com">
```

Prefer server-side redirects (HTTP 301/302) over meta-refresh where possible.

---
