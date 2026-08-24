> CORS (Cross-Origin Resource Sharing) is one of the most misunderstood topics in web development. Developers see the error, don't understand why it happens, and blindly add `Access-Control-Allow-Origin: *`. This part explains exactly what CORS is, why it exists, how the browser and server both participate, and how to configure it correctly in NestJS.

---

## Table of Contents

1. [Why CORS Exists — The Problem It Solves](#1-why-cors-exists--the-problem-it-solves)
2. [What CORS Actually Is](#2-what-cors-actually-is)
3. [Simple Requests — No Preflight](#3-simple-requests--no-preflight)
4. [Preflighted Requests — The OPTIONS Request](#4-preflighted-requests--the-options-request)
5. [CORS Headers — Complete Reference](#5-cors-headers--complete-reference)
6. [Credentials and CORS](#6-credentials-and-cors)
7. [What the Browser Does vs What the Server Does](#7-what-the-browser-does-vs-what-the-server-does)
8. [Common CORS Errors and What They Mean](#8-common-cors-errors-and-what-they-mean)
9. [CORS in NestJS — Correct Configuration](#9-cors-in-nestjs--correct-configuration)
10. [CORS Is Not a Security Mechanism for the Server](#10-cors-is-not-a-security-mechanism-for-the-server)
11. [Paideon CORS Configuration](#11-paideon-cors-configuration)

---

## 1. Why CORS Exists — The Problem It Solves

The Same-Origin Policy (Part 09) prevents scripts from reading cross-origin responses. But legitimate cross-origin reads are necessary.

```
THE LEGITIMATE NEED:

Frontend:   https://paideon.lk         (Next.js)
API:        https://api.paideon.lk     (NestJS)

These are different origins (different subdomains).
The SOP, by default, blocks paideon.lk from reading api.paideon.lk responses.

But we WANT this to work — paideon.lk IS the intended client for api.paideon.lk.

THE CORS SOLUTION:
The server explicitly tells the browser:
  "Yes, I allow cross-origin requests from https://paideon.lk."

Browser: "OK, the server consented. I'll let the JavaScript read the response."

CORS is a mechanism to relax the SOP — with the server's explicit permission.
```

---

## 2. What CORS Actually Is

CORS is a set of HTTP headers that the **server** adds to its responses, instructing the **browser** about which cross-origin requests are allowed.

```
Key facts:

1. CORS is enforced by the BROWSER, not the server.
   The server just adds headers.
   The browser decides whether to let JavaScript read the response.

2. CORS applies to browser JavaScript (fetch, XHR).
   It does NOT apply to: curl, Postman, server-to-server requests.
   If Postman works but the browser doesn't → it's a CORS issue.
   CORS errors ONLY appear in browser JavaScript.

3. CORS is the server granting permission.
   The browser asks: "Server, can origin X read your response?"
   The server answers via headers.
   If no permission: browser blocks JavaScript from reading the response.

4. The request still goes through (usually).
   CORS does not prevent the request from being sent.
   It prevents JavaScript from reading the response.
   (Exception: preflighted requests check permission first.)
```

---

## 3. Simple Requests — No Preflight

Some cross-origin requests are considered "simple." The browser sends them directly without a preliminary check.

### What Makes a Request Simple

```
A request is simple if ALL of the following are true:

Method is one of:
  GET, HEAD, POST

Headers are ONLY from this safe set:
  Accept
  Accept-Language
  Content-Language
  Content-Type (ONLY with these values):
    application/x-www-form-urlencoded
    multipart/form-data
    text/plain

Any other header → not simple → preflight required.
  Authorization: Bearer ...      → NOT simple → preflight
  Content-Type: application/json → NOT simple → preflight
  X-Requested-With: ...          → NOT simple → preflight
```

### Simple Request Flow

```
Browser (paideon.lk) makes:
GET https://api.paideon.lk/api/public-stats HTTP/1.1
Origin: https://paideon.lk     ← browser always adds this for cross-origin

API server responds:
HTTP/1.1 200 OK
Access-Control-Allow-Origin: https://paideon.lk
[body]

Browser checks:
  Is the requesting origin in Access-Control-Allow-Origin? YES.
  → JavaScript can read the response. ✓

If server responds without CORS header:
HTTP/1.1 200 OK
[no Access-Control-Allow-Origin]
[body]

Browser:
  BLOCKS JavaScript from reading the response.
  The HTTP request completed, the response was received — JS just can't see it.
  Console: "Access to fetch at '...' from origin '...' has been blocked by CORS policy"
```

In practice, almost no real API requests are "simple." `Content-Type: application/json` is not on the safe list. `Authorization: Bearer`is not on the safe list. Every authenticated JSON API call triggers a preflight.

---

## 4. Preflighted Requests — The OPTIONS Request

For non-simple requests, the browser sends a preliminary OPTIONS request to ask for permission before making the actual request.

```
Browser wants to send:
  POST https://api.paideon.lk/api/students
  Authorization: Bearer eyJ...
  Content-Type: application/json
  {"name": "Ashan", ...}

Authorization header → not simple → preflight required.

─────────────────────────────────────────────────────────────────
STEP 1: Preflight (browser sends automatically, before the real request)
─────────────────────────────────────────────────────────────────
OPTIONS /api/students HTTP/1.1
Host: api.paideon.lk
Origin: https://paideon.lk
Access-Control-Request-Method: POST
Access-Control-Request-Headers: Authorization, Content-Type

"Before I send POST with these headers, will you allow it?"

─────────────────────────────────────────────────────────────────
STEP 2: Preflight Response (server must reply correctly)
─────────────────────────────────────────────────────────────────
HTTP/1.1 204 No Content
Access-Control-Allow-Origin: https://paideon.lk
Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS
Access-Control-Allow-Headers: Authorization, Content-Type
Access-Control-Max-Age: 86400

"Yes, that origin, method, and headers are allowed. Cache this answer for 1 day."

─────────────────────────────────────────────────────────────────
STEP 3: Actual Request (browser sends the real request)
─────────────────────────────────────────────────────────────────
POST /api/students HTTP/1.1
Host: api.paideon.lk
Origin: https://paideon.lk
Authorization: Bearer eyJ...
Content-Type: application/json
{"name": "Ashan", ...}

─────────────────────────────────────────────────────────────────
STEP 4: Actual Response
─────────────────────────────────────────────────────────────────
HTTP/1.1 201 Created
Access-Control-Allow-Origin: https://paideon.lk
{"id": "cuid-123", "name": "Ashan", ...}

Browser: ACAO header present and matches. JavaScript reads response. ✓
```

### Preflight Caching — Access-Control-Max-Age

```
Without caching:
  Every CORS request = 2 HTTP requests (OPTIONS + actual).
  High-traffic SPA: 2× the requests.

With Max-Age:
  Access-Control-Max-Age: 86400   (cache preflight result for 1 day)

  Browser caches the permission.
  Next request to same origin + same method + same headers:
  → No OPTIONS request. Just the actual request.

  Browser limit: Chrome caps at 7200 seconds (2 hours). Firefox at 86400 (1 day).
  Set to 7200 (Chrome's max) for practical caching.
```

---

## 5. CORS Headers — Complete Reference

### Response Headers (Server → Browser)

```
Access-Control-Allow-Origin
  Which origin is allowed to read this response.

  Access-Control-Allow-Origin: https://paideon.lk     → only this origin
  Access-Control-Allow-Origin: *                       → any origin (no credentials)

  Only ONE origin can be specified per response.
  To allow multiple origins: check the request's Origin header,
  if it's in your allowed list → echo it back.

Access-Control-Allow-Methods
  Used in preflight response. Which methods are allowed.

  Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS

  OPTIONS itself should usually be included.

Access-Control-Allow-Headers
  Used in preflight response. Which request headers are allowed.

  Access-Control-Allow-Headers: Authorization, Content-Type, X-Request-Id

Access-Control-Allow-Credentials
  Whether cookies and Authorization headers can be included in cross-origin requests.

  Access-Control-Allow-Credentials: true

  When true: Access-Control-Allow-Origin CANNOT be *.
  Must be a specific origin. (See section 6.)

Access-Control-Expose-Headers
  By default, JavaScript can only read "safe" response headers:
    Cache-Control, Content-Language, Content-Length, Content-Type,
    Expires, Last-Modified, Pragma

  To expose custom headers to JavaScript:
  Access-Control-Expose-Headers: X-Request-Id, X-RateLimit-Remaining

  Without this: response.headers.get('X-Request-Id') returns null.

Access-Control-Max-Age
  How long (seconds) to cache the preflight result.
  Access-Control-Max-Age: 7200    (2 hours — Chrome's practical max)
```

### Request Headers (Browser → Server, automatic)

```
Origin
  The origin making the request. Added automatically by browser.
  Origin: https://paideon.lk

  Note: Origin is also sent for same-origin requests in some cases.
  It's only absent for: navigation requests, <img> loads, etc.

Access-Control-Request-Method
  In preflight only. The method the browser wants to use.
  Access-Control-Request-Method: POST

Access-Control-Request-Headers
  In preflight only. The non-simple headers the browser wants to send.
  Access-Control-Request-Headers: Authorization, Content-Type
```

---

## 6. Credentials and CORS

"Credentials" in CORS means: cookies, Authorization headers, and TLS client certificates.

```
By default, cross-origin requests do NOT include credentials.

fetch('https://api.paideon.lk/api/students')
  → No cookies sent. No Authorization if using cookie-based auth.

To include credentials:
fetch('https://api.paideon.lk/api/students', {
  credentials: 'include'    // send cookies, Authorization
})

For this to work, the server must respond with:
  Access-Control-Allow-Credentials: true
  Access-Control-Allow-Origin: https://paideon.lk  ← MUST be specific, not *

If server has:
  Access-Control-Allow-Origin: *
  Access-Control-Allow-Credentials: true

Browser: IGNORES the response. Treats it as a CORS failure.
Reason: * with credentials would allow any site to read your authenticated data.

The combination "specific origin + allow credentials" is the only safe way
to allow cross-origin credentialed requests.
```

### When credentials: 'include' is needed

```
Scenario A: access token in Authorization header (Paideon's approach)
  fetch(url, {
    headers: { Authorization: `Bearer ${accessToken}` }
    // credentials: 'include' NOT needed for headers — they're explicit
  })

  The Authorization header is sent explicitly.
  credentials: 'include' is for COOKIES, not explicit headers.

Scenario B: refresh token in HttpOnly cookie (Paideon's approach)
  fetch('https://api.paideon.lk/api/auth/refresh', {
    method: 'POST',
    credentials: 'include'   // REQUIRED — sends the HttpOnly cookie
  })

  The refresh_token cookie won't be sent without credentials: 'include'
  because api.paideon.lk is a different origin from paideon.lk.

  Server must have:
    Access-Control-Allow-Origin: https://paideon.lk
    Access-Control-Allow-Credentials: true
```

---

## 7. What the Browser Does vs What the Server Does

This distinction resolves most CORS confusion.

```
BROWSER responsibilities:
  → Adds Origin header to cross-origin requests
  → Decides if request needs a preflight (based on method + headers)
  → Sends OPTIONS preflight request
  → Reads server's CORS headers from preflight response
  → Decides if actual request is allowed
  → Sends actual request (if allowed)
  → After receiving response: reads ACAO header
  → Allows or blocks JavaScript from reading the response body

SERVER responsibilities:
  → Reads incoming Origin header
  → Decides if that origin should be allowed
  → Adds Access-Control-* headers to the response
  → Must respond to OPTIONS requests (preferably with 204)
  → Nothing else. The server does NOT enforce CORS.

What the server cannot do:
  → The server cannot prevent a request from being made.
    (The browser may not send the actual request, but it can always send OPTIONS.)
  → The server cannot stop a non-browser (curl, Postman) from reading responses.
    CORS is purely a browser enforcement mechanism.
  → The server cannot "block" an origin via CORS.
    It can refuse to add ACAO headers, but the browser is the one that blocks.
```

---

## 8. Common CORS Errors and What They Mean

```
Error 1:
"Access to fetch at 'https://api.paideon.lk/api/students' from origin
'https://paideon.lk' has been blocked by CORS policy:
No 'Access-Control-Allow-Origin' header is present on the requested resource."

Cause:   Server is not sending ACAO header at all.
Fix:     Enable CORS on the server for this origin.
Not:     A server error. The server may have returned 200 with data —
         but the browser blocked JS from reading it.

─────────────────────────────────────────────────────────────────

Error 2:
"...has been blocked by CORS policy:
The value of the 'Access-Control-Allow-Origin' header in the response must not
be the wildcard '*' when the request's credentials mode is 'include'."

Cause:   Server has ACAO: * but client uses credentials: 'include'.
Fix:     Server must use specific origin: Access-Control-Allow-Origin: https://paideon.lk
         AND: Access-Control-Allow-Credentials: true

─────────────────────────────────────────────────────────────────

Error 3:
"...has been blocked by CORS policy:
Response to preflight request doesn't pass access control check:
It does not have HTTP ok status."

Cause:   OPTIONS request returned non-2xx status (404, 500, etc.).
Fix:     Ensure your server handles OPTIONS requests and returns 204.
         Check that the route doesn't require auth for OPTIONS.
         (Preflight requests never send credentials.)

─────────────────────────────────────────────────────────────────

Error 4:
"...has been blocked by CORS policy:
Request header field x-custom-header is not allowed by
Access-Control-Allow-Headers in preflight response."

Cause:   Client sends a custom header not listed in ACAH.
Fix:     Add the header to Access-Control-Allow-Headers on the server.

─────────────────────────────────────────────────────────────────

Error 5 (misleading — not actually a CORS error):
Network error or ERR_CONNECTION_REFUSED, but the console shows a CORS error.

Cause:   Server is not running, or Nginx is blocking the request entirely.
         The browser gets no response → reports it as CORS failure.
Fix:     Check if the server is actually running and accepting connections.
         curl the URL directly to verify. If curl fails → it's not CORS.
```

---

## 9. CORS in NestJS — Correct Configuration

```typescript
// main.ts

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  // Option A: Simple — single origin
  app.enableCors({
    origin: "https://paideon.lk",
    methods: ["GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS"],
    allowedHeaders: ["Authorization", "Content-Type", "X-Request-Id"],
    credentials: true,
    maxAge: 7200,
  });

  // Option B: Multiple origins (dev + prod)
  const allowedOrigins = [
    "https://paideon.lk",
    "https://www.paideon.lk",
    process.env.NODE_ENV === "development" ? "http://localhost:3000" : null,
  ].filter(Boolean);

  app.enableCors({
    origin: (requestOrigin, callback) => {
      // Allow requests with no origin (curl, Postman, server-to-server)
      if (!requestOrigin) return callback(null, true);

      if (allowedOrigins.includes(requestOrigin)) {
        callback(null, true);
      } else {
        callback(new Error(`CORS: origin ${requestOrigin} not allowed`));
      }
    },
    methods: ["GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS"],
    allowedHeaders: ["Authorization", "Content-Type", "X-Request-Id"],
    credentials: true,
    maxAge: 7200,
  });

  await app.listen(3001);
}
```

### What NestJS's enableCors Does

```
It registers a middleware that runs on every request:

1. Reads the incoming Origin header.
2. If origin matches allowed list:
   → Adds Access-Control-Allow-Origin: [that origin]
   → Adds Vary: Origin (important for caching — see below)
   → Adds other ACAO headers

3. If the request method is OPTIONS (preflight):
   → Adds Access-Control-Allow-Methods
   → Adds Access-Control-Allow-Headers
   → Adds Access-Control-Max-Age
   → Returns 204 immediately (does NOT continue to route handler)

4. If origin doesn't match:
   → Returns 403 or simply omits CORS headers (browser then blocks)
```

### The Vary: Origin Header

```
When you serve different ACAO headers based on the Origin,
you must add:
  Vary: Origin

This tells CDNs and proxies:
"The response may differ based on the Origin header.
Cache it separately per origin."

Without Vary: Origin:
  User A (from paideon.lk) → gets response with ACAO: https://paideon.lk
  CDN caches that response.
  User B (from other-site.com) → CDN serves cached response with ACAO: https://paideon.lk
  → User B's cross-origin request appears to be allowed for paideon.lk
  → (Or vice versa: legitimate user gets wrong origin in ACAO → CORS error)

NestJS's enableCors adds Vary: Origin automatically.
```

---

## 10. CORS Is Not a Security Mechanism for the Server

This is the most important thing to understand about CORS.

```
CORS protects USERS from malicious websites reading their data.
CORS does NOT protect your SERVER from malicious requests.

Why:

1. CORS is enforced by browsers.
   Non-browser clients (curl, Postman, malicious server scripts) ignore CORS.
   An attacker's Node.js script can call your API directly, bypassing CORS entirely.

2. CORS does not prevent requests from being made.
   Even from a browser, the request is often sent — only the response is blocked.
   For non-preflighted requests, the server processes the request fully.
   If it's a DELETE request that causes damage, the damage is done before CORS blocks.

CORS only answers: "Can THIS browser's JavaScript read the response FROM origin X?"

For actual server security:
  → Authentication: JWT token required for protected endpoints
  → Authorization: role checks, schoolId checks
  → Rate limiting: prevent brute force
  → Input validation: prevent injection attacks
  → IP allowlisting: for admin endpoints, if applicable

Setting ACAO: * does NOT make your API insecure for authenticated endpoints.
  Authenticated endpoint: requires Authorization header → requires user's token.
  ACAO: * means "any JavaScript can read the response IF they have the token."
  But anyone with a valid token is authenticated anyway.

  The danger of ACAO: * is with endpoints that rely on COOKIES for auth
  and use credentials: 'include' — that's why * is blocked with credentials.

  For explicit token auth (Authorization: Bearer): ACAO: * is often fine.
  For cookie auth: ACAO must be specific + credentials: true.
```

---

## 11. Paideon CORS Configuration

```
Paideon architecture:
  Frontend: https://paideon.lk        (Next.js — port 3000)
  API:      https://api.paideon.lk    (NestJS — port 3001)

  These are different origins.
  Frontend makes cross-origin requests to API.
  CORS is required.

Auth approach:
  Access token: in Authorization header (explicit — not a CORS credential issue)
  Refresh token: in HttpOnly cookie → cross-origin cookie requires credentials: 'include'
                 → ACAO must be specific origin + ACAC: true

Complete NestJS config:

  app.enableCors({
    origin: [
      'https://paideon.lk',
      'https://www.paideon.lk',
      ...(isDev ? ['http://localhost:3000'] : []),
    ],
    methods: ['GET', 'HEAD', 'POST', 'PUT', 'PATCH', 'DELETE', 'OPTIONS'],
    allowedHeaders: [
      'Authorization',
      'Content-Type',
      'Accept',
      'X-Request-Id',
    ],
    exposedHeaders: [
      'X-Request-Id',
      'X-RateLimit-Limit',
      'X-RateLimit-Remaining',
    ],
    credentials: true,   // required: refresh token is an HttpOnly cookie
    maxAge: 7200,        // Chrome's practical max
  });

Frontend fetch calls:

  // Regular API call (access token in header)
  fetch('https://api.paideon.lk/api/students', {
    headers: {
      'Authorization': `Bearer ${accessToken}`,
      'Content-Type': 'application/json',
    }
    // credentials: 'include' NOT needed here (no cookies required for this call)
  });

  // Refresh token call (needs cookie)
  fetch('https://api.paideon.lk/api/auth/refresh', {
    method: 'POST',
    credentials: 'include',   // REQUIRED — sends the HttpOnly refresh_token cookie
  });

  // Logout (needs cookie to revoke)
  fetch('https://api.paideon.lk/api/auth/logout', {
    method: 'POST',
    credentials: 'include',
    headers: { 'Authorization': `Bearer ${accessToken}` },
  });

What happens if running both services on localhost (development):
  Frontend: http://localhost:3000   (Next.js dev server)
  API:      http://localhost:3001   (NestJS)

  These are STILL different origins (different ports).
  CORS still applies.
  Add http://localhost:3000 to allowed origins in dev.

  Alternative: use Next.js rewrites to proxy API calls:
    // next.config.ts
    rewrites: async () => [{
      source: '/api/:path*',
      destination: 'http://localhost:3001/api/:path*',
    }]

  With rewrites: browser calls https://paideon.lk/api/... (same origin).
  No CORS needed. Next.js server proxies to NestJS server-to-server.
  This is cleaner for production too (single domain, no CORS at all).
```

---

_Next: [Part 11 — WebSockets & Real-Time Web](./Part%2011%20-%20WebSockets%20%26%20Real-Time%20Web.md)_
