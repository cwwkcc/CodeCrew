> Performance is not an afterthought. A slow school management system means teachers wait while class time passes, students can't submit assignments on slow connections, and administrators abandon workflows. This part covers what actually makes pages fast — from the first byte to the last painted pixel.

---

## Table of Contents

1. [How the Browser Loads a Page — The Critical Rendering Path](#1-how-the-browser-loads-a-page--the-critical-rendering-path)
2. [What Makes Pages Slow](#2-what-makes-pages-slow)
3. [Caching — The Fastest Request Is the One Never Made](#3-caching--the-fastest-request-is-the-one-never-made)
4. [HTTP Caching Headers — Complete Reference](#4-http-caching-headers--complete-reference)
5. [Compression — Gzip and Brotli](#5-compression--gzip-and-brotli)
6. [Resource Hints — Telling the Browser What's Coming](#6-resource-hints--telling-the-browser-whats-coming)
7. [Images — The Biggest Opportunity](#7-images--the-biggest-opportunity)
8. [JavaScript Performance](#8-javascript-performance)
9. [CDNs — Serving From the Edge](#9-cdns--serving-from-the-edge)
10. [Performance Metrics — What to Measure](#10-performance-metrics--what-to-measure)
11. [Next.js Performance Features](#11-nextjs-performance-features)
12. [Paideon Performance Checklist](#12-paideon-performance-checklist)

---

## 1. How the Browser Loads a Page — The Critical Rendering Path

The critical rendering path is the sequence of steps from receiving HTML to displaying the first pixel. Every step is a potential bottleneck.

```
1. DNS Lookup         paideon.lk → 104.21.45.67             ~20ms (cached: 0ms)
2. TCP Handshake      SYN → SYN-ACK → ACK                   ~1 RTT (~20ms)
3. TLS Handshake      ClientHello → ServerHello → Finished   ~1 RTT (~20ms, TLS 1.3)
4. HTTP Request       GET /login → waiting for response
5. Time to First Byte (TTFB)  server processing + network   ~50-200ms
6. Download HTML              browser receives HTML           ~10-50ms
7. Parse HTML         build DOM tree
   → encounter <link rel="stylesheet"> → block, fetch CSS
   → encounter <script src="...">      → block, fetch+run JS
   → encounter <img src="...">         → fetch (non-blocking)
8. Download CSS       browser fetches all stylesheets
9. Build CSSOM        CSS Object Model (parallel to DOM)
10. Build Render Tree DOM + CSSOM → what's visible and how
11. Layout            calculate position/size of every element
12. Paint             fill in pixels for each element
13. Composite         layer GPU-composited elements
14. First Contentful Paint (FCP) → user sees something

Then: fetch JavaScript bundles → parse → execute → React hydration
→ Time to Interactive (TTI) → page responds to user input
```

The critical path is everything that BLOCKS step 11 (layout). CSS and JavaScript in `<head>` are render-blocking by default.

---

## 2. What Makes Pages Slow

### Render-Blocking Resources

```html
<!-- These block rendering until downloaded and parsed -->
<link rel="stylesheet" href="styles.css" />
<!-- render-blocking -->
<script src="app.js"></script>
<!-- render-blocking AND parse-blocking -->

<!-- These do not block rendering -->
<script src="analytics.js" defer></script>
<!-- downloaded parallel, executes after HTML parsed -->
<script src="widget.js" async></script>
<!-- downloaded + executes as soon as ready -->
<link rel="stylesheet" href="print.css" media="print" />
<!-- non-blocking (print only) -->
```

**`defer` vs `async`:**

```
defer:
  Download: parallel with HTML parsing (non-blocking)
  Execute: after HTML is fully parsed, in order of appearance
  Use for: scripts that depend on the DOM, scripts that depend on each other

async:
  Download: parallel with HTML parsing (non-blocking)
  Execute: immediately when downloaded (may interrupt HTML parsing)
  Order: not guaranteed
  Use for: independent scripts (analytics, ads, chat widgets)

For Next.js: the framework handles this automatically.
Your <Script> component with strategy="lazyOnload" or "afterInteractive"
uses defer/async under the hood.
```

### Too Much JavaScript

```
JavaScript is the most expensive resource per byte.
1MB of JavaScript ≠ 1MB of an image.

Image: download → decompress → display. Done.
JavaScript: download → parse → compile → execute → may block main thread during all steps.

Parse + compile: ~1ms per KB on a mid-range device.
1MB of JS = ~1 second of CPU time just to parse and compile.
Before any code even runs.

Paideon serves schools. School computers may be old.
Mobile devices on 3G are common in Sri Lanka.
Test on slow connections and slow devices.

Strategies:
  Code splitting: only load JS for the current page.
  Tree shaking: bundler removes unused code.
  Dynamic imports: load heavy features on demand.
```

### Unoptimized Images

```
Images typically account for 50-80% of total page weight.

A DSLR photo uploaded without compression:
  Raw: 5,000 × 3,000 px = 15 megapixels
  JPEG at 100% quality: ~8MB
  Displayed at: 300 × 200px on the page

You sent 8MB to display 300px wide. That's 40× more data than needed.

Optimized:
  Resize to 600px (2× for retina) × quality 75% JPEG: ~30KB
  Convert to WebP: ~20KB
  Convert to AVIF: ~15KB
  Savings: 99.8%
```

### No Caching

```
Without caching:
  Every visit: download all JS, CSS, images
  User visits 10 pages: downloads the same 300KB bundle 10 times

With caching:
  First visit: download once
  Subsequent visits: instant (from disk cache)
  Returning visitor: instant

JS and CSS bundles should be cached indefinitely.
The filename contains a content hash — if the file changes, the filename changes.
New filename → cache miss → fresh download.
```

---

## 3. Caching — The Fastest Request Is the One Never Made

There are multiple layers of caching between your server and the user's browser.

```
Browser cache      → per user, per browser. Fastest.
Service worker     → programmable cache, used in PWAs.
CDN edge cache     → shared by all users near that edge node. Fast.
Reverse proxy cache → Nginx can cache responses. Near your app.
Application cache  → Redis/Memcached. Your NestJS app caches DB results.
Database query cache → PostgreSQL's own buffer pool.

Request flow:
  Browser → [browser cache hit? serve] → CDN → [CDN cache hit? serve] → Your server

Each layer that hits = a request that never reaches the next layer.
```

### What to Cache and For How Long

```
Cache forever (use content hashing to bust):
  JS bundles:      /static/js/main.a1b2c3d4.js   → max-age=31536000, immutable
  CSS bundles:     /static/css/main.e5f6g7h8.css  → max-age=31536000, immutable
  Images with hash /static/img/logo.i9j0k1.webp   → max-age=31536000, immutable

  immutable: tells the browser "don't even check if this changed."
  Content hash in filename means if content changes, URL changes → fresh download.

Cache briefly:
  HTML pages:      max-age=0, must-revalidate  (always revalidate HTML)
                   OR: no-cache (always check with server)
  API responses for public data: max-age=60 (1 minute)

Never cache:
  Auth tokens, user data, personalized content
  POST/PUT/DELETE responses
  Anything with private user information
  no-store
```

---

## 4. HTTP Caching Headers — Complete Reference

```
Cache-Control directives:

public           Anyone can cache this (CDNs, proxies, browsers).
private          Only the user's browser can cache this. Not CDNs.
no-cache         Cache it, but ALWAYS revalidate with server before serving.
no-store         Don't cache at all. Don't write it to disk.
max-age=N        Cache for N seconds. After expiry, revalidate.
s-maxage=N       Like max-age, but for shared caches (CDNs) only. Overrides max-age for CDNs.
must-revalidate  After max-age expires, don't serve stale. Must check with server.
immutable        Don't revalidate ever. Only use with content-hashed filenames.
stale-while-revalidate=N  Serve stale while fetching fresh in background.

Example configurations:
─────────────────────────────────────────────────────
Static JS bundle (content-hashed filename):
  Cache-Control: public, max-age=31536000, immutable
  → Cached for 1 year. Never revalidated. Name change = new file.

HTML page:
  Cache-Control: no-cache
  → Cached but revalidated on every visit. If not changed → 304 (fast).

API response (public, frequently updated):
  Cache-Control: public, max-age=60, stale-while-revalidate=30
  → Serve from cache for 60 seconds.
  → Up to 30s after expiry, serve stale while fetching fresh.

Auth API response:
  Cache-Control: no-store
  → Never cache. Period.

CDN-cached public content, private browser cache:
  Cache-Control: private, max-age=3600
  → Browser caches 1 hour. CDN doesn't cache.

  Cache-Control: public, s-maxage=86400, max-age=3600
  → CDN caches 24 hours. Browser caches 1 hour.
```

### ETag and Conditional Requests

```
First response:
  HTTP/1.1 200 OK
  ETag: "abc123def456"
  Cache-Control: no-cache
  [body]

Second request:
  GET /api/students HTTP/1.1
  If-None-Match: "abc123def456"

Server checks:
  Current ETag matches → nothing changed
  HTTP/1.1 304 Not Modified
  [no body — saves bandwidth]

  Current ETag different → data changed
  HTTP/1.1 200 OK
  ETag: "xyz789abc012"
  [new body]

ETag can be:
  Hash of the content: MD5/SHA-256 of the response body.
  Version counter: "v42"
  Last modified timestamp: "Mon, 13 Mar 2026 10:00:00 GMT"

Prisma: every record has updatedAt. Use it as ETag:
  ETag: "2026-03-13T10:00:00.000Z"
```

---

## 5. Compression — Gzip and Brotli

Text responses (HTML, CSS, JS, JSON) compress dramatically.

```
Compression ratios (typical):
  JavaScript bundle: 300KB uncompressed → 90KB gzip → 75KB brotli (75% smaller)
  JSON API response:  50KB uncompressed → 10KB gzip → 8KB brotli (84% smaller)
  HTML:              20KB uncompressed → 7KB gzip → 6KB brotli (70% smaller)

Binary files (images, video, fonts) are already compressed.
Compressing them again adds CPU cost but doesn't reduce size.

Algorithms:
  gzip:   Widely supported. Fast compression. Good ratios. Default.
  brotli: Better compression (10-25% smaller than gzip). Slower compression.
          Only over HTTPS (required by spec). Modern browsers support it.
  zstd:   Newer. Fast + good ratios. Limited browser support currently.

Nginx configuration:
─────────────────────────────────────────────────────────────
# Gzip
gzip on;
gzip_vary on;             # Vary: Accept-Encoding (for caches)
gzip_min_length 256;      # Don't compress tiny responses (overhead > benefit)
gzip_comp_level 6;        # 1-9, higher = smaller but slower. 6 is good balance.
gzip_types text/plain text/css application/json application/javascript
            text/xml application/xml image/svg+xml;

# Brotli (requires ngx_brotli module)
brotli on;
brotli_comp_level 4;
brotli_types text/plain text/css application/json application/javascript
             text/xml application/xml image/svg+xml;
─────────────────────────────────────────────────────────────

Client indicates support:
  Accept-Encoding: gzip, deflate, br
  (br = brotli)

Server responds:
  Content-Encoding: br
  (the response body is brotli-compressed)

Client decompresses transparently.
```

---

## 6. Resource Hints — Telling the Browser What's Coming

Resource hints let you instruct the browser to prepare for upcoming resources before it discovers them in the HTML.

```html
<!-- preconnect: establish TCP+TLS to an origin before we need it -->
<link rel="preconnect" href="https://fonts.googleapis.com" />
<link rel="preconnect" href="https://api.paideon.lk" crossorigin />

<!-- dns-prefetch: resolve DNS for an origin (lighter than preconnect) -->
<link rel="dns-prefetch" href="https://cdn.paideon.lk" />

<!-- preload: download a specific resource needed for current page -->
<link rel="preload" href="/fonts/inter.woff2" as="font" crossorigin />
<link rel="preload" href="/critical.css" as="style" />

<!-- prefetch: download a resource likely needed for NEXT page (low priority) -->
<link rel="prefetch" href="/dashboard.js" />

<!-- modulepreload: preload + parse a JavaScript module -->
<link rel="modulepreload" href="/app.js" />
```

### When to Use Each

```
preconnect:
  Use when you know you'll fetch from a specific origin but don't know the URL yet.
  Example: Google Fonts CDN, your API domain.
  Establishes DNS + TCP + TLS. Expensive. Use for max 2-3 origins.

dns-prefetch:
  Use for origins where you're less certain, or as fallback for preconnect.
  DNS only. Cheap. OK to use for more origins.

preload:
  Use for critical resources on the CURRENT page.
  Fonts, hero images, critical CSS/JS.
  The browser will NOT preload these on its own (it discovers them late in parsing).
  as= attribute is required and must be correct (font, style, script, image).

prefetch:
  Use for resources needed on the NEXT page.
  Browser fetches at low priority during idle time.
  Example: prefetch dashboard.js on the login page (user will likely go there next).

  Next.js <Link> does this automatically for visible links.

Don't overuse preload:
  Every preloaded resource competes for bandwidth.
  Preloading things you don't need is worse than not preloading.
```

---

## 7. Images — The Biggest Opportunity

### Formats

```
JPEG:   Photos. Lossy. Good quality at small sizes.
        .jpg/.jpeg
        Use for: student photos, report charts

PNG:    Graphics with transparency. Lossless.
        Use for: logos, icons with transparency

WebP:   Modern. Better than JPEG and PNG for most uses.
        ~30% smaller than JPEG at same quality.
        ~25% smaller than PNG for lossless.
        Supported by all modern browsers.
        Use for: everything (fallback to JPEG/PNG for old browsers)

AVIF:   Even better than WebP. ~50% smaller than JPEG.
        Slower to encode. Growing browser support.
        Use for: where supported (with WebP fallback)

SVG:    Vector. Text-based. Scales to any size without quality loss.
        Use for: logos, icons, illustrations

GIF:    Animated. Terrible compression. Replace with video or WebP animated.
```

### Responsive Images

```html
<!-- srcset: browser picks the right size for the device -->
<img
  src="student-photo-400.jpg"
  srcset="
    student-photo-400.jpg   400w,
    student-photo-800.jpg   800w,
    student-photo-1200.jpg 1200w
  "
  sizes="
    (max-width: 600px) 100vw,
    (max-width: 1200px) 50vw,
    400px
  "
  alt="Student photo"
/>

<!-- picture: serve different formats by browser support -->
<picture>
  <source srcset="student.avif" type="image/avif" />
  <source srcset="student.webp" type="image/webp" />
  <img src="student.jpg" alt="Student photo" />
</picture>
```

### Lazy Loading

```html
<!-- Native lazy loading: browser only loads when approaching viewport -->
<img src="class-photo.jpg" loading="lazy" alt="Class photo" />

<!-- Don't lazy load above-the-fold images -->
<img src="hero.jpg" loading="eager" alt="School banner" />
<!-- or just omit loading="" — eager is default -->
```

### Next.js `<Image>` Component

```typescript
import Image from 'next/image';

<Image
  src="/students/ashan.jpg"
  alt="Ashan Silva"
  width={300}
  height={300}
  // Automatically:
  // → Resizes and optimizes
  // → Converts to WebP/AVIF if supported
  // → Lazy loads by default
  // → Prevents layout shift (reserves space)
  // → Serves correct size for device
/>

// For priority (above the fold, LCP image):
<Image src="/hero.jpg" priority alt="School" width={1200} height={400} />
```

---

## 8. JavaScript Performance

### Code Splitting

```typescript
// Without code splitting: one giant bundle.
// Every page loads ALL the JavaScript for ALL pages.

// With code splitting: separate bundles per page/feature.
// Next.js does this automatically per page.

// Dynamic import: load on demand
const HeavyChart = dynamic(() => import('../components/HeavyChart'), {
  loading: () => <Spinner />,
  ssr: false  // don't render on server (chart libraries often need browser APIs)
});

// Only loads HeavyChart.js when the component is actually rendered.
// The rest of the page loads without it.
```

### Tree Shaking

```javascript
// WRONG: imports entire lodash library (~70KB gzipped)
import _ from "lodash";
_.debounce(fn, 300);

// RIGHT: imports only debounce (~1KB)
import debounce from "lodash/debounce";

// Even better: use native alternatives
const debounce = (fn, delay) => {
  let timer;
  return (...args) => {
    clearTimeout(timer);
    timer = setTimeout(() => fn(...args), delay);
  };
};
```

### Avoiding Main Thread Blocking

```typescript
// Heavy computation blocks the main thread → UI freezes

// WRONG: synchronous heavy computation
function processGrades(grades: Grade[]) {
  return grades.map((g) => expensiveCalculation(g)); // might take 500ms
}

// BETTER: break into chunks with setTimeout
async function processGradesChunked(grades: Grade[]) {
  const results = [];
  for (let i = 0; i < grades.length; i += 100) {
    const chunk = grades.slice(i, i + 100);
    results.push(...chunk.map((g) => expensiveCalculation(g)));
    await new Promise((resolve) => setTimeout(resolve, 0)); // yield to browser
  }
  return results;
}

// BEST: use Web Workers for true off-main-thread computation
const worker = new Worker("/workers/grade-processor.js");
worker.postMessage({ grades });
worker.onmessage = (e) => setProcessedGrades(e.data);
```

---

## 9. CDNs — Serving From the Edge

A CDN (Content Delivery Network) is a globally distributed network of servers that cache and serve your content from the server closest to the user.

```
Without CDN:
  Student in Colombo → paideon.lk server in Frankfurt → ~150ms per request

With CDN (Cloudflare has edge in Singapore, ~30ms from Colombo):
  Student in Colombo → Cloudflare edge Singapore → ~30ms
  (For cached content, the Frankfurt server is not involved at all)

What CDNs cache:
  Static assets:   JS, CSS, images, fonts (indefinitely with content hashing)
  API responses:   If Cache-Control: public is set (rarely appropriate for private data)
  HTML pages:      If not personalized (public pages only)

What CDNs cannot (should not) cache:
  Authenticated user data
  Real-time data
  POST/PUT/DELETE responses

Cloudflare setup for Paideon:
  All traffic proxied through Cloudflare (orange cloud in DNS settings).
  Static assets (/_next/static/*): cached at edge.
  /api/*: bypassed or short TTL (user-specific data).

  Cache rules in Cloudflare:
    URL pattern: /_next/static/*
    → Cache everything, max age 1 year

    URL pattern: /api/*
    → Bypass cache (always forward to origin)
```

---

## 10. Performance Metrics — What to Measure

### Core Web Vitals (Google's metrics)

```
LCP — Largest Contentful Paint
  When does the largest visible element finish loading?
  Measures: perceived load speed
  Target: < 2.5 seconds

  Usually: hero image, large text block, video thumbnail
  Fix slow LCP: preload LCP image, optimize image, fast TTFB, no render-blocking

FID — First Input Delay (being replaced by INP)
INP — Interaction to Next Paint
  How long from user interaction to visual response?
  Measures: responsiveness
  Target: < 200ms

  Usually blocked by: heavy JavaScript on main thread
  Fix: reduce JS, defer non-critical scripts, code split

CLS — Cumulative Layout Shift
  How much does the page shift unexpectedly during load?
  Measures: visual stability
  Target: < 0.1

  Cause: images without dimensions, dynamically injected content, fonts loading
  Fix: always specify width/height on images, reserve space for dynamic content
```

### Other Key Metrics

```
TTFB — Time to First Byte
  Time from request to first byte of response.
  Measures: server response time + network.
  Target: < 800ms (ideally < 200ms)

  Fix slow TTFB: optimize database queries, add caching, move server closer to users.

FCP — First Contentful Paint
  When does the browser render the first text or image?
  Target: < 1.8 seconds

TTI — Time to Interactive
  When can the user actually interact with the page?
  This is often much later than FCP.
  Heavy JS can show the page but keep it unresponsive for seconds.
```

### Measuring Performance

```
Chrome DevTools:
  Network tab → disable cache, throttle to Slow 3G → observe waterfall
  Performance tab → record → analyze flame chart
  Lighthouse → automated audit, scores 0-100 per metric

Online tools:
  PageSpeed Insights: https://pagespeed.web.dev (uses real-world data)
  WebPageTest:        https://www.webpagetest.org (detailed waterfall)

Real user monitoring (RUM):
  Measure actual user experience, not just lab conditions.
  Cloudflare has built-in RUM.
  Or: web-vitals npm package → send metrics to your own analytics.
```

---

## 11. Next.js Performance Features

Next.js provides performance features that you get automatically or with minimal configuration.

```
Automatic:
  Code splitting per page
  Automatic static optimization (static pages when no data fetching)
  Image optimization via <Image> component
  Font optimization via next/font (zero layout shift)
  Script optimization via next/script

Server-side rendering (SSR):
  Page is rendered on server for each request.
  User gets complete HTML immediately (no blank screen).
  React hydrates after (attaches event listeners).

Static site generation (SSG):
  Page is pre-built at build time.
  No server rendering on each request — just serves a file.
  Fastest for pages that don't change per request (landing pages, docs).

Incremental Static Regeneration (ISR):
  Static page, but regenerated in background every N seconds.
  Combines SSG speed with fresh data.

  export const revalidate = 60;  // regenerate every 60 seconds

React Server Components (App Router):
  Components that run on the server only.
  Can query database directly.
  No JavaScript shipped to client for server components.

  // This component has ZERO JS on the client:
  async function StudentList() {
    const students = await prisma.student.findMany();
    return <ul>{students.map(s => <li>{s.name}</li>)}</ul>;
  }

next/font:
  // Loads font with zero layout shift (CLS = 0)
  // Self-hosts the font file (no Google Fonts round trip)
  import { Inter } from 'next/font/google';
  const inter = Inter({ subsets: ['latin'] });
```

---

## 12. Paideon Performance Checklist

```
Network:
  ✓ HTTPS everywhere (TLS 1.3)
  ✓ HTTP/2 or HTTP/3 enabled in Nginx
  ✓ Cloudflare CDN in front of origin
  ✓ Cloudflare edge caches static assets
  ✓ Brotli compression enabled in Nginx

Caching:
  ✓ JS/CSS bundles: max-age=31536000, immutable (content-hashed names)
  ✓ Images: max-age=86400 or longer
  ✓ HTML: no-cache (always fresh)
  ✓ API responses: no-store (user data, never cache)
  ✓ ETag on API list endpoints (304 for unchanged data)

JavaScript:
  ✓ No unnecessary dependencies (audit with bundlephobia.com)
  ✓ Dynamic import for heavy components (chart libraries, PDF viewers)
  ✓ defer/async on third-party scripts
  ✓ Tree shaking (Vite/Next.js handles this)

Images:
  ✓ Use next/image for all images
  ✓ WebP/AVIF format
  ✓ Correct sizing (don't serve 2000px image at 300px display width)
  ✓ loading="lazy" for below-the-fold images
  ✓ Explicit width/height (prevents CLS)

Fonts:
  ✓ Use next/font (zero layout shift, self-hosted)
  ✓ Subset fonts to only needed characters (Latin for English, Sinhala if needed)
  ✓ woff2 format

Server-side:
  ✓ Database query optimization (indexes on frequently queried columns)
  ✓ N+1 query prevention (Prisma select with include, not nested findMany)
  ✓ Redis for caching expensive, frequently-read data
  ✓ Connection pooling (Prisma default: pool of 10)
  ✓ TTFB < 200ms for API endpoints

Monitoring:
  ✓ Core Web Vitals tracked
  ✓ TTFB monitored for key pages
  ✓ Lighthouse score checked on every major release
  ✓ Performance regression CI check
```

---

_Next: [Part 08 — Browser Internals](07%20-%20Browser%20Internals.md)_
