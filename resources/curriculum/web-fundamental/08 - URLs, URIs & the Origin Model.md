> The URL is the most fundamental primitive of the web. Everything — links, API calls, cookies, same-origin policy, CORS — derives from the URL's structure. Understanding exactly what a URL is and how the browser thinks about "origins" unlocks a whole category of web concepts that are otherwise confusing.

---

## Table of Contents

1. [URI vs URL vs URN — The Distinction](#1-uri-vs-url-vs-urn--the-distinction)
2. [URL Structure — Every Part Named and Explained](#2-url-structure--every-part-named-and-explained)
3. [Percent Encoding — Allowed Characters in URLs](#3-percent-encoding--allowed-characters-in-urls)
4. [The URL API — Working with URLs in JavaScript](#4-the-url-api--working-with-urls-in-javascript)
5. [Origin — The Security Boundary](#5-origin--the-security-boundary)
6. [The Same-Origin Policy — What It Restricts](#6-the-same-origin-policy--what-it-restricts)
7. [Site vs Origin — The Difference](#7-site-vs-origin--the-difference)
8. [Relative URLs — How They Resolve](#8-relative-urls--how-they-resolve)
9. [Data URLs](#9-data-urls)
10. [Blob URLs](#10-blob-urls)
11. [URL Best Practices for Paideon API](#11-url-best-practices-for-paideon-api)

---

## 1. URI vs URL vs URN — The Distinction

These terms are used interchangeably in everyday speech, but they have specific meanings.

```
URI (Uniform Resource Identifier)
  The umbrella term. Identifies a resource.
  
  Two types of URI:
    URL → identifies a resource AND tells you how to locate it
    URN → identifies a resource by name, without location info

URL (Uniform Resource Locator)
  A URI that includes the mechanism to retrieve the resource.
  Scheme tells you HOW to get it (http, https, ftp, etc.)
  
  https://paideon.lk/api/students → URL (and also a URI)
  How to get it: HTTPS → DNS lookup → TCP → TLS → HTTP GET

URN (Uniform Resource Name)
  A URI that names a resource without describing how to find it.
  
  isbn:978-0-13-468599-1  → identifies a book by ISBN (no location)
  urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66  → UUID format URN
  
In practice:
  Web developers use "URL" for almost everything.
  "URI" appears in specs and HTTP headers.
  They are often interchangeable in conversation.
```

---

## 2. URL Structure — Every Part Named and Explained

```
https://ashan:secret@api.paideon.lk:8443/v1/students?class=10A&sort=name#results
│       │     │      │               │    │           │                   │
│       │     │      │               │    │           │                   └─ Fragment
│       │     │      │               │    │           └─────────────────── Query
│       │     │      │               │    └─────────────────────────────── Path
│       │     │      │               └──────────────────────────────────── Port
│       │     │      └──────────────────────────────────────────────────── Host
│       │     └─────────────────────────────────────────────────────────── Password
│       └───────────────────────────────────────────────────────────────── Username
└───────────────────────────────────────────────────────────────────────── Scheme
```

### Scheme

```
https://    HyperText Transfer Protocol Secure
http://     HyperText Transfer Protocol (insecure)
ftp://      File Transfer Protocol
mailto:     Email address
tel:        Telephone number (no // — no authority section)
file:///    Local filesystem
ws://       WebSocket
wss://      WebSocket Secure

The scheme determines everything: how to interpret the rest, which protocol to use.
```

### Authority

```
ashan:secret@api.paideon.lk:8443
│     │      │               │
│     │      │               └── Port (optional — defaults per scheme)
│     │      └────────────────── Host (domain or IP address)
│     └───────────────────────── Password
└─────────────────────────────── Username

Username:password — Basic Auth credentials in the URL.
  Deprecated and dangerous. Credentials appear in browser history, logs, etc.
  Never use. Use Authorization header instead.

Host:
  Domain name: api.paideon.lk
  IPv4:        192.168.1.1
  IPv6:        [::1]  (brackets required for IPv6 in URLs)

Port:
  Default ports per scheme (omitted if default):
    http  → 80
    https → 443
    ftp   → 21
    ws    → 80
    wss   → 443
  
  Non-default ports must be explicit:
    http://localhost:3001
    https://api.paideon.lk:8443
```

### Path

```
/v1/students/cuid-123/grades

Segments separated by forward slash.
Case-sensitive on most servers (Linux is case-sensitive).
Trailing slash may or may not be significant:
  /students  ≠  /students/  on some servers
  → pick one and redirect the other.

Dot segments:
  .   → current directory (usually removed by browser before sending)
  ..  → parent directory
  
  /v1/students/../courses  → resolves to /v1/courses
  Browsers normalize URLs before sending. But be aware of path traversal:
  If server code constructs file paths from URL without sanitizing,
  ../../etc/passwd could be dangerous.
```

### Query String

```
?class=10A&sort=name&order=desc&page=2

? marks the start of the query.
& separates key-value pairs.
= separates key from value.

Parsing:
  class=10A → key: "class", value: "10A"
  sort=name → key: "sort",  value: "name"

Ordering: generally not significant (API should handle any order).
Encoding: keys and values must be percent-encoded.
  Spaces → %20 or + (in application/x-www-form-urlencoded only)
  & inside a value → %26 (otherwise parsed as separator)
  = inside a value → %3D

Arrays in query strings (no standard, many conventions):
  ?ids=1&ids=2&ids=3         → same key repeated
  ?ids[]=1&ids[]=2           → PHP style
  ?ids[0]=1&ids[1]=2         → indexed
  ?ids=1,2,3                 → comma-separated (simplest)
  
  URLSearchParams handles "same key repeated":
  new URLSearchParams('ids=1&ids=2').getAll('ids') → ['1', '2']
```

### Fragment

```
#results

The fragment identifier. Also called the "hash."

NEVER sent to the server. The browser strips it before sending the HTTP request.
It is client-side only.

Traditional use: link to a specific section of a page.
  https://paideon.lk/docs#authentication
  → browser scrolls to <element id="authentication">

Single-Page App routing (legacy):
  Before the History API, SPAs used the fragment for routing.
  https://paideon.lk/#/students/123
  → Server always sees / → React Router reads the fragment → renders student
  This is "hash routing" — works without server configuration.

Modern SPAs use the History API instead (clean URLs without #).
```

---

## 3. Percent Encoding — Allowed Characters in URLs

URLs can only contain a specific set of ASCII characters. Everything else must be percent-encoded.

```
Unreserved characters (never need encoding):
  A-Z  a-z  0-9  -  _  .  ~

Reserved characters (have special meaning — encode if used as data, not as syntax):
  :  /  ?  #  [  ]  @  !  $  &  '  (  )  *  +  ,  ;  =

Everything else must be encoded as %HH where HH is the hex byte value.

Examples:
  Space    → %20
  @        → %40  (when in a value, not as user@host separator)
  &        → %26  (when in a value, not as query separator)
  =        → %3D  (when in a value, not as key=value separator)
  /        → %2F  (when in a value, not as path separator)
  ?        → %3F  (when in a value, not as query start)
  +        → %2B  (+ in URLs means space in form data; encode literal + as %2B)
  #        → %23  (when in a value, not as fragment start)

Non-ASCII characters (Unicode):
  First encode as UTF-8 bytes, then percent-encode each byte.
  
  Sinhala: "ශිෂ්ය" (student)
  UTF-8 bytes: E0 B6 B3 E0 B7 8A E0 B6 B7 E0 B7 8A E0 B6 BA
  URL encoded: %E0%B6%B3%E0%B7%8A%E0%B6%B7%E0%B7%8A%E0%B6%BA

Encoding in JavaScript:
  encodeURIComponent('hello world&name=x')
  → 'hello%20world%26name%3Dx'  (encodes almost everything, safe for values)
  
  encodeURI('https://paideon.lk/path?q=hello world')
  → 'https://paideon.lk/path?q=hello%20world'  (doesn't encode URL syntax chars)
  
  Use encodeURIComponent for individual values.
  Use encodeURI for full URLs if you must, but prefer URL API instead.

Decoding:
  decodeURIComponent('%E0%B6%B3') → "ශ"
```

---

## 4. The URL API — Working with URLs in JavaScript

Never manually parse or construct URLs with string concatenation. Use the `URL` class.

```javascript
// Parsing a URL
const url = new URL('https://api.paideon.lk:8443/v1/students?class=10A&sort=name#top');

url.protocol   // "https:"
url.hostname   // "api.paideon.lk"
url.port       // "8443"
url.host       // "api.paideon.lk:8443"
url.pathname   // "/v1/students"
url.search     // "?class=10A&sort=name"
url.hash       // "#top"
url.origin     // "https://api.paideon.lk:8443"
url.href       // Full URL string

// Query parameters
url.searchParams.get('class')      // "10A"
url.searchParams.get('sort')       // "name"
url.searchParams.has('order')      // false
url.searchParams.getAll('ids')     // [] (or array if multiple)

// Modifying
url.searchParams.set('page', '2');
url.searchParams.append('ids', '123');
url.searchParams.append('ids', '456');
url.searchParams.delete('sort');

url.href  // "https://api.paideon.lk:8443/v1/students?class=10A&page=2&ids=123&ids=456#top"

// Constructing from parts
const apiUrl = new URL('/v1/students', 'https://api.paideon.lk');
apiUrl.href  // "https://api.paideon.lk/v1/students"

// Building search params separately
const params = new URLSearchParams({
  class: '10A',
  sort: 'name',
  order: 'asc',
  page: '1',
});
params.toString()  // "class=10A&sort=name&order=asc&page=1"

// In NestJS, use the URL module:
import { URL } from 'url';  // Node.js built-in
// Same API as browser URL
```

---

## 5. Origin — The Security Boundary

The **origin** is the browser's fundamental security boundary. It is defined as the combination of three parts:

```
Origin = Scheme + Host + Port

https://paideon.lk:443           → scheme: https, host: paideon.lk, port: 443
https://api.paideon.lk:443       → DIFFERENT (different host)
http://paideon.lk:443            → DIFFERENT (different scheme)
https://paideon.lk:8443          → DIFFERENT (different port)
https://paideon.lk               → SAME as https://paideon.lk:443 (default port)

All of these are different origins:
  https://paideon.lk
  http://paideon.lk           ← different scheme
  https://www.paideon.lk      ← different host (subdomain!)
  https://api.paideon.lk      ← different host (subdomain!)
  https://paideon.lk:3000     ← different port
  https://paideon.com         ← different host entirely

SAME origin (scheme + host + port all identical):
  https://paideon.lk/login          same origin as
  https://paideon.lk/dashboard      ← same scheme, host, port (path doesn't matter)
  https://paideon.lk/api/students   ← same origin
```

Port 443 is the default HTTPS port. `https://paideon.lk` and `https://paideon.lk:443` are the same origin. Port 80 is the default HTTP port.

---

## 6. The Same-Origin Policy — What It Restricts

The Same-Origin Policy (SOP) is the browser's most fundamental security mechanism. It prevents malicious websites from reading data from other websites.

```
Without SOP:

You're logged into paideon.lk in one tab.
You visit evil.com in another tab.

evil.com runs:
  fetch('https://paideon.lk/api/students')
  // browser sends your paideon.lk cookie automatically
  // response contains all student data
  // evil.com reads and exfiltrates it

With SOP:

evil.com is origin: https://evil.com
paideon.lk is origin: https://paideon.lk
Different origins → SOP kicks in.

fetch('https://paideon.lk/api/students') from evil.com:
  Browser SENDS the request (cookies included — this is why CSRF exists).
  Browser RECEIVES the response.
  Browser BLOCKS evil.com's JavaScript from READING the response.
  
The key insight:
  SOP does not prevent requests from being made.
  SOP prevents the response from being read by a different origin.
  This is why CSRF attacks work (request is made, changes happen).
  This is why CORS is needed for legitimate cross-origin reads.
```

### What the SOP Restricts

```
Reading cross-origin responses (unless CORS allows it):
  fetch(), XHR, .responseText, .json() — blocked

Accessing cross-origin iframe's DOM:
  const iframe = document.querySelector('iframe[src="https://bank.com"]');
  iframe.contentDocument  → SecurityError

Accessing cross-origin window properties:
  window.open('https://google.com').document  → blocked
  But: window.postMessage() works across origins (intentionally)

What the SOP does NOT restrict:
  Making requests (fetch, form submit) — just can't read the response
  Loading resources via HTML tags:
    <script src="https://cdn.com/lib.js">  → loads fine
    <img src="https://other.com/img.png">  → loads fine
    <link rel="stylesheet" href="https://cdn.com/style.css"> → loads fine
  These are "simple" cross-origin loads, necessary for the web to function.
```

---

## 7. Site vs Origin — The Difference

"Same-site" and "same-origin" are often confused. They have different definitions.

```
ORIGIN = scheme + host + port  (exact match required)
SITE   = scheme + registrable domain  (looser match)

Examples:

https://paideon.lk      vs  https://api.paideon.lk
  Same origin?  NO   (different host: paideon.lk vs api.paideon.lk)
  Same site?    YES  (same registrable domain: paideon.lk, same scheme: https)

https://paideon.lk      vs  http://paideon.lk
  Same origin?  NO   (different scheme)
  Same site?    NO   (different scheme — "schemeful same-site")

https://paideon.lk      vs  https://evil.paideon.lk
  Same origin?  NO
  Same site?    YES  (evil.paideon.lk is a subdomain of paideon.lk)
  
  WAIT — this means evil.paideon.lk is "same site" as paideon.lk?
  Yes. The site concept is broader.
  This is why SameSite=Lax cookies flow between subdomains.
  If evil.paideon.lk is compromised, same-site cookies are at risk.

https://paideon.github.io  vs  https://other.github.io
  Same origin?  NO
  Same site?    NO  (github.io is a Public Suffix — not a registrable domain)
  Registrable domain: paideon.github.io (not just github.io)
  
  The Public Suffix List (PSL) defines these boundaries.
  Browsers use the PSL to determine registrable domains.

Why this distinction matters:
  CORS uses origin (strict)
  SameSite cookies use site (loose)
  Document.domain (deprecated) used site
  Cross-site request attacks use site boundaries
```

---

## 8. Relative URLs — How They Resolve

When a relative URL is encountered, the browser resolves it against a base URL.

```
Base URL (the current page): https://paideon.lk/app/students/list.html

Relative URL                 Resolves To
─────────────────────────────────────────────────────────────────────────────
./detail.html                https://paideon.lk/app/students/detail.html
detail.html                  https://paideon.lk/app/students/detail.html  (same)
../courses/list.html         https://paideon.lk/app/courses/list.html
/api/students                https://paideon.lk/api/students  (absolute path)
//cdn.paideon.lk/img.jpg     https://cdn.paideon.lk/img.jpg  (protocol-relative)
https://other.com/page       https://other.com/page  (absolute — unchanged)

Rules:
  Starts with scheme (https:)  → absolute URL, use as-is
  Starts with //               → protocol-relative, inherit scheme from base
  Starts with /                → absolute path, inherit scheme+host from base
  Starts with . or no prefix   → relative path, resolve from current directory
  Starts with ..               → go up one directory, then resolve

Base URL is determined by:
  1. <base href="..."> element in <head> (overrides everything)
  2. Current page URL
  3. In HTML emails/embedded content: the embedding context

In React / Next.js:
  <a href="/students"> → absolute path → https://paideon.lk/students
  <Link href="/students"> → same, but Next.js handles routing
  
  fetch('/api/students') → in browser: resolves from current origin → https://paideon.lk/api/students
  fetch('/api/students') → in Node.js (server component): relative URLs are invalid
                           → must use absolute URL: fetch('http://localhost:3001/api/students')
  
  This is a common mistake: relative URLs work in browser, fail in SSR.
```

---

## 9. Data URLs

Data URLs embed content directly in the URL. No separate HTTP request is made.

```
Format:
  data:[mediatype][;base64],data

Examples:
  data:text/plain;base64,SGVsbG8gV29ybGQ=
  → "Hello World" as plain text

  data:image/png;base64,iVBORw0KGgoAAAANSUhEUg...
  → A PNG image, base64-encoded, embedded directly

  data:text/html,<h1>Hello</h1>
  → HTML content (no base64, just URL-encoded)

Use cases:
  Inline tiny images (icon, placeholder) in HTML/CSS:
    background-image: url('data:image/png;base64,...')
  
  No HTTP request needed → saves one network round trip.
  Only worthwhile for very small resources (< 1-2 KB).
  Large data URLs increase HTML/CSS size → may be slower overall.

Security:
  Browsers restrict what data URLs can do.
  In modern browsers: data URLs cannot access cookies, localStorage, parent context.
  data: is a separate origin from any website.
  JavaScript in a data URL iframe cannot communicate with the parent page.

In Paideon:
  Student profile photo loading state: data URL placeholder SVG (tiny, inline)
  Small icons might use data URLs if they're truly tiny.
  Profile photos themselves → stored on server, served via normal URL.
```

---

## 10. Blob URLs

Blob URLs (also called Object URLs) are temporary URLs that reference binary data held in browser memory.

```javascript
// Create a blob (in-memory binary data)
const imageData = await fetch('/api/students/cuid-123/photo').then(r => r.blob());

// Create a temporary URL for it
const blobUrl = URL.createObjectURL(imageData);
// blobUrl looks like: blob:https://paideon.lk/550e8400-e29b-41d4-a716-446655440000

// Use the blob URL
const img = document.createElement('img');
img.src = blobUrl;
document.body.appendChild(img);

// When done — MUST revoke or memory leaks
URL.revokeObjectURL(blobUrl);
```

### Use Cases

```
File downloads (without server round-trip):
  const reportData = generateReport();  // client-side generated CSV
  const blob = new Blob([reportData], { type: 'text/csv' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'grades.csv';
  a.click();
  URL.revokeObjectURL(url);

Preview uploaded files before sending:
  input.addEventListener('change', (e) => {
    const file = e.target.files[0];
    const url = URL.createObjectURL(file);
    preview.src = url;  // show preview without uploading
  });

Video/audio streaming from ArrayBuffer.

Blob URLs:
  Are scoped to the origin that created them.
  Only work in the same browsing session (tab).
  Are revoked when:
    → URL.revokeObjectURL() is called
    → The document is unloaded (if not revoked manually)
  Must be revoked manually in long-lived apps to prevent memory leaks.
```

---

## 11. URL Best Practices for Paideon API

```
Be consistent with trailing slashes:
  Choose: /api/v1/students  OR  /api/v1/students/
  Never: sometimes with, sometimes without.
  Nginx: redirect one to the other.

Always use HTTPS:
  http:// URLs should 301 redirect to https://.
  Never serve API responses over HTTP.

Use lowercase:
  /api/students       ← correct
  /api/Students       ← inconsistent, confusing
  
  Nginx is case-sensitive on Linux. /api/Students ≠ /api/students.
  Always lowercase. Always.

URL-encode special characters in path and query:
  Never construct URLs with string concatenation:
    WRONG: `/api/students/${name}`   // breaks if name contains /
    RIGHT: new URL(`/api/students/${encodeURIComponent(name)}`, baseUrl)

Don't put sensitive data in URLs:
  Tokens, passwords, session IDs → NEVER in URLs.
  URLs appear in: browser history, server logs, referrer headers, CDN logs.
  They are not private.

Keep URLs stable:
  A URL is a resource identifier. It should be permanent.
  If a student's ID is cuid-123, /api/students/cuid-123 should work forever.
  If a URL must change: 301 redirect old to new.
  
  Don't expose auto-incrementing IDs:
    /api/students/1  → reveals count, easy to enumerate, fragile
    /api/students/clwq4rz9k0000qk08h4j7g1p2  → CUID, opaque, safe

The URL is the API contract:
  Other code (mobile apps, third-party integrations) depends on your URLs.
  Breaking a URL breaks those integrations.
  Version the API (/v1/, /v2/) to allow breaking changes without breaking clients.
```

---

_Next: [Part 10 — CORS](09%20-%20CORS.md)_