## Full Head Reference

```html
<head>
  <!-- 1. Character encoding — must be first -->
  <meta charset="UTF-8">

  <!-- 2. Viewport for responsive design -->
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <!-- 3. Document title -->
  <title>Page Title — Site Name</title>

  <!-- 4. Standard SEO meta tags -->
  <meta name="description" content="A 150–160 character summary of the page.">
  <meta name="author" content="Cinderax">
  <meta name="keywords" content="c++, algorithms, competitive programming">
  <meta name="robots" content="index, follow">

  <!-- 5. Canonical URL (prevents duplicate content issues) -->
  <link rel="canonical" href="https://example.com/this-page">

  <!-- 6. Open Graph (controls how the page looks when shared on social media) -->
  <meta property="og:title"       content="Page Title">
  <meta property="og:description" content="Page description">
  <meta property="og:image"       content="https://example.com/og-image.jpg">
  <meta property="og:url"         content="https://example.com/this-page">
  <meta property="og:type"        content="website">

  <!-- 7. Twitter Card -->
  <meta name="twitter:card"        content="summary_large_image">
  <meta name="twitter:title"       content="Page Title">
  <meta name="twitter:description" content="Page description">
  <meta name="twitter:image"       content="https://example.com/twitter-image.jpg">

  <!-- 8. Favicons -->
  <link rel="icon"             href="/favicon.ico" sizes="any">
  <link rel="icon"             href="/icon.svg"    type="image/svg+xml">
  <link rel="apple-touch-icon" href="/apple-touch-icon.png">
  <link rel="manifest"         href="/manifest.webmanifest">

  <!-- 9. Stylesheets -->
  <link rel="stylesheet" href="/css/main.css">

  <!-- 10. Preloading critical resources -->
  <link rel="preload" href="/fonts/inter.woff2" as="font" type="font/woff2" crossorigin>
  <link rel="preconnect" href="https://fonts.googleapis.com">

  <!-- 11. Scripts (usually at end of body, but here with defer) -->
  <script src="/js/app.js" defer></script>
</head>
```

## `<meta http-equiv>`

Older mechanism; mostly replaced by HTTP headers. Still used for:

```html
<!-- Redirect after 5 seconds -->
<meta http-equiv="refresh" content="5; url=https://new-location.com">

<!-- Content Security Policy (better set as HTTP header) -->
<meta http-equiv="Content-Security-Policy" content="default-src 'self'">
```

## Script Loading Strategies

|Attribute|Behaviour|
|---|---|
|_(none)_|Blocks parsing; executes immediately|
|`defer`|Downloads in parallel; executes after parsing|
|`async`|Downloads in parallel; executes immediately when ready (order not guaranteed)|
|`type="module"`|ES module; deferred by default|

---