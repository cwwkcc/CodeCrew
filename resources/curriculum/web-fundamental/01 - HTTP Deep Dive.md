> HTTP is the language of the web. Every request your browser makes, every API call your NestJS server receives — it's all HTTP. This part covers everything: what HTTP actually is at the wire level, every method, every status code, every important header, and how the protocol evolved from 1991 to today.

---

## Table of Contents

1. [What HTTP Is — At the Wire Level](#1-what-http-is--at-the-wire-level)
2. [HTTP Versions — The Evolution](#2-http-versions--the-evolution)
3. [The Request — Every Part Explained](#3-the-request--every-part-explained)
4. [The Response — Every Part Explained](#4-the-response--every-part-explained)
5. [HTTP Methods — Deep Level](#5-http-methods--deep-level)
6. [Status Codes — Complete Reference](#6-status-codes--complete-reference)
7. [Headers — Complete Reference](#7-headers--complete-reference)
8. [Content-Type — Complete Reference](#8-content-type--complete-reference)
9. [HTTP vs HTTPS — The Difference at the Wire](#9-http-vs-https--the-difference-at-the-wire)
10. [Keep-Alive and Connection Reuse](#10-keep-alive-and-connection-reuse)
11. [HTTP in NestJS — What Actually Happens](#11-http-in-nestjs--what-actually-happens)

---

## 1. What HTTP Is — At the Wire Level

HTTP (HyperText Transfer Protocol) is a text-based request-response protocol. It runs on top of TCP (or QUIC for HTTP/3). It has no memory — every request is completely independent.

```
HTTP is just text.

A request is literally a text message:
────────────────────────────────────────────
GET /api/students HTTP/1.1\r\n
Host: paideon.lk\r\n
Authorization: Bearer eyJhbGc...\r\n
Accept: application/json\r\n
\r\n
────────────────────────────────────────────

A response is literally a text message:
────────────────────────────────────────────
HTTP/1.1 200 OK\r\n
Content-Type: application/json\r\n
Content-Length: 342\r\n
\r\n
{"students": [...]}
────────────────────────────────────────────

\r\n is CRLF (Carriage Return + Line Feed)
The blank line (\r\n\r\n) separates headers from body.
```

HTTP is **stateless**. The server remembers nothing between requests. If you make two requests in a row, the server does not know they came from the same person unless you tell it (via cookies, tokens, or session IDs). This is by design — statelessness makes servers easy to scale horizontally.

---

## 2. HTTP Versions — The Evolution

### HTTP/0.9 (1991)

```
The original. One method: GET. No headers. No status codes.
Request:  GET /page.html
Response: [raw HTML, connection closes]
```

### HTTP/1.0 (1996)

```
Added: headers, status codes, POST method, Content-Type
Problem: one TCP connection per request.

Timeline for a page with 3 images:
  TCP handshake → GET HTML → close
  TCP handshake → GET img1 → close
  TCP handshake → GET img2 → close
  TCP handshake → GET img3 → close

4 round trips just for TCP. Slow.
```

### HTTP/1.1 (1997) — Still the Baseline

```
Added: persistent connections (keep-alive), pipelining,
       Host header (mandatory), chunked transfer encoding,
       more methods (PUT, DELETE, OPTIONS, PATCH, HEAD)

Persistent connections: TCP stays open after a request.
  GET HTML → GET img1 → GET img2 → GET img3
  All on the same TCP connection.

Problem: Head-of-line blocking.
  Pipelining lets you send multiple requests without waiting,
  but responses must come back IN ORDER.
  If response #2 is slow, responses #3, #4, #5 wait.

Still the most widely used version today.
When you write HTTP/1.1 in NestJS — this is what you're using.
```

### HTTP/2 (2015)

```
Problem it solves: head-of-line blocking and multiplexing

HTTP/2 is binary, not text.
Introduces frames and streams.

Multiple requests can be in-flight simultaneously on ONE TCP connection.
Each request gets a stream ID. Responses can arrive out of order.

Stream 1: GET /api/students  → response arrives 3rd
Stream 2: GET /api/courses   → response arrives 1st
Stream 3: GET /api/grades    → response arrives 2nd

No blocking. True parallelism over one connection.

Also adds:
  → Header compression (HPACK) — headers often repeat between requests
  → Server push — server can send resources before client asks
    (e.g., when browser requests HTML, server immediately pushes CSS/JS)

Remaining problem: TCP head-of-line blocking.
  If one TCP packet is lost, ALL streams wait for retransmission.
  This is a limitation of TCP, not HTTP.
```

### HTTP/3 (2022)

```
Problem it solves: TCP head-of-line blocking

HTTP/3 runs on QUIC instead of TCP.
QUIC is built on UDP with reliability implemented per-stream.

If one packet is lost:
  TCP: all streams stall
  QUIC: only the stream that lost the packet stalls
        other streams continue flowing

Also:
  → 0-RTT connection establishment (no separate TCP handshake)
  → Built-in encryption (TLS 1.3 is mandatory, part of QUIC)
  → Connection migration (switch from WiFi to cellular without reconnecting)

Not universally supported yet but major CDNs (Cloudflare) support it.
When using Cloudflare in front of Paideon, clients may use HTTP/3
to reach Cloudflare, even if your Nginx uses HTTP/1.1 internally.
```

---

## 3. The Request — Every Part Explained

```
POST /api/auth/login HTTP/1.1\r\n
Host: paideon.lk\r\n
Content-Type: application/json\r\n
Content-Length: 52\r\n
Accept: application/json\r\n
Accept-Language: en-US,en;q=0.9,si;q=0.8\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)...\r\n
Connection: keep-alive\r\n
\r\n
{"email":"ashan@school.lk","password":"my-password"}
```

### Request Line

```
POST /api/auth/login HTTP/1.1
│    │               │
│    │               └── Protocol version
│    └── Request target (path + query string)
└── Method
```

The request target can take several forms:

```
/api/students?school=abc       Origin form (most common — used with proxies)
https://paideon.lk/api/login   Absolute form (used with CONNECT for proxies)
*                              Asterisk form (used with OPTIONS for server-wide)
```

### Headers

```
Host: paideon.lk
│     └── The domain being requested
└── The only MANDATORY header in HTTP/1.1
    Without it, a server hosting multiple domains can't know which one.

Content-Type: application/json
  What format the BODY is in.
  Only relevant when there is a body (POST, PUT, PATCH).

Content-Length: 52
  How many bytes the body is.
  Server reads exactly this many bytes.
  Without this, server doesn't know where the body ends.
  Alternative: Transfer-Encoding: chunked (for streaming bodies)

Accept: application/json
  What formats the CLIENT can handle in the response.
  Server should respond with one of these.
  application/json, text/html, */* (anything)

Authorization: Bearer eyJhbGc...
  Credentials. "Bearer" means it's a JWT or OAuth token.
  Other schemes: Basic (base64 username:password), Digest, API-Key

User-Agent: Mozilla/5.0 ...
  Identifies the client software.
  Browsers send their browser/OS info here.
  Server-to-server API calls often send library names.
  Can be faked — never trust it for security decisions.

Cookie: session_id=abc; theme=dark
  All cookies for this domain, automatically sent by browser.

Referer: https://paideon.lk/dashboard
  The page the user was on before this request.
  Misspelling is historical (correct: "Referrer").
  Set to empty for privacy in some cases.
```

### Body

```
The body is separated from headers by a blank line (\r\n\r\n).

{"email":"ashan@school.lk","password":"my-password"}

Body can be:
  JSON        Content-Type: application/json
  Form data   Content-Type: application/x-www-form-urlencoded
  File upload Content-Type: multipart/form-data
  Plain text  Content-Type: text/plain
  Binary      Content-Type: application/octet-stream

GET and HEAD requests should not have a body.
POST, PUT, PATCH typically have a body.
DELETE can have a body but rarely does.
```

---

## 4. The Response — Every Part Explained

```
HTTP/1.1 200 OK\r\n
Content-Type: application/json; charset=utf-8\r\n
Content-Length: 284\r\n
Cache-Control: no-store\r\n
X-Request-Id: req_01hw4rz9kf\r\n
Date: Fri, 13 Mar 2026 08:23:14 GMT\r\n
\r\n
{"accessToken":"eyJhbGc...","user":{"id":"cuid","email":"ashan@school.lk"}}
```

### Status Line

```
HTTP/1.1 200 OK
│        │   │
│        │   └── Reason phrase (human-readable, ignored by machines)
│        └── Status code (3-digit number, machine-readable)
└── Protocol version
```

### Response Headers

```
Content-Type: application/json; charset=utf-8
  What format the body is in.
  Browser uses this to decide how to render the response.
  charset=utf-8 tells it the text encoding.

Content-Length: 284
  Size of the body in bytes.
  Client reads exactly this many bytes.

Cache-Control: no-store
  Tells the browser and any proxies how to cache this response.
  no-store = do not cache at all (appropriate for auth responses)

Set-Cookie: refresh_token=eyJ...; HttpOnly; Secure; SameSite=Strict; Max-Age=604800
  Instructs the browser to store a cookie.
  Multiple Set-Cookie headers for multiple cookies.

Location: https://paideon.lk/dashboard
  Used with 3xx redirects. Browser makes a new request to this URL.

WWW-Authenticate: Bearer realm="paideon"
  Used with 401. Tells client what authentication method to use.

X-Request-Id: req_01hw4rz9kf
  Custom header. Not standard. Common convention for tracing.
  Prefix X- was historically used for custom headers.

Date: Fri, 13 Mar 2026 08:23:14 GMT
  When the response was generated. Useful for debugging.
```

---

## 5. HTTP Methods — Deep Level

### GET

```
Purpose: Retrieve a resource.
Has body: No (technically allowed, but ignored by most servers)
Safe: Yes (should not modify state)
Idempotent: Yes (calling it N times = same as calling it once)

GET /api/students?class=10A&school=school-cuid HTTP/1.1

Parameters go in the query string, NOT the body.
URLs are logged by servers, proxies, browsers.
Never put sensitive data (passwords, tokens) in a query string.
```

### POST

```
Purpose: Create a resource, or trigger an action.
Has body: Yes
Safe: No
Idempotent: No (two identical POSTs may create two records)

POST /api/auth/login HTTP/1.1
{"email":"...","password":"..."}

POST /api/students HTTP/1.1
{"name":"Ashan","class":"10A"}
→ Creates a new student. Response: 201 Created + the new student.

Retry-safe? No. If you POST an order and the request times out,
retrying might create a duplicate order. Idempotency keys solve this.
```

### PUT

```
Purpose: Replace a resource entirely.
Has body: Yes
Idempotent: Yes (replacing with the same data N times = same result)

PUT /api/students/cuid-123 HTTP/1.1
{"name":"Ashan Silva","class":"10B","phone":"0771234567"}

Replaces the ENTIRE student record with this body.
Fields omitted from the body are deleted/nulled.
If the resource doesn't exist, some implementations create it.
```

### PATCH

```
Purpose: Partially update a resource.
Has body: Yes
Idempotent: Technically not guaranteed (depends on operation)

PATCH /api/students/cuid-123 HTTP/1.1
{"class":"10B"}

Only updates the class field. Everything else stays the same.
This is what most "update" endpoints actually use.

Difference from PUT:
  PUT  → send the entire object (replace)
  PATCH → send only changed fields (merge)
```

### DELETE

```
Purpose: Delete a resource.
Has body: Rare
Idempotent: Yes (deleting an already-deleted resource = same end state)

DELETE /api/students/cuid-123 HTTP/1.1

Response: 204 No Content (success, no body)
       or 200 OK with deleted record in body
       or 404 if not found (but idempotently could return 204)
```

### HEAD

```
Purpose: Same as GET, but returns ONLY headers, no body.
Use case: Check if a resource exists, check its size/type without downloading.

HEAD /api/exports/report.pdf HTTP/1.1

Response headers identical to GET response.
Body is empty.
Content-Length tells you how big the body WOULD be.

Use case: Before downloading a 100MB file, check its size first.
```

### OPTIONS

```
Purpose: Ask what methods/headers are supported for a resource.
Critical for: CORS preflight requests (explained in Part 10).

OPTIONS /api/students HTTP/1.1
Origin: https://paideon.lk

Response:
HTTP/1.1 204 No Content
Allow: GET, POST, OPTIONS
Access-Control-Allow-Methods: GET, POST
Access-Control-Allow-Headers: Authorization, Content-Type
```

### Idempotency Summary

```
Method    Safe    Idempotent    Has Body
GET       Yes     Yes           No
HEAD      Yes     Yes           No
OPTIONS   Yes     Yes           No
POST      No      No            Yes
PUT       No      Yes           Yes
PATCH     No      No*           Yes
DELETE    No      Yes           Rarely

Safe = does not modify server state
Idempotent = N calls have same effect as 1 call
```

---

## 6. Status Codes — Complete Reference

Status codes are 3-digit numbers. The first digit is the class.

### 1xx — Informational

```
100 Continue
  Server has received the request headers.
  Client should send the body.
  Used when client asks "I'm about to send a large body, is that OK?"
  with Expect: 100-continue header.

101 Switching Protocols
  Server agrees to switch protocols.
  Used in WebSocket upgrade: client requests HTTP→WebSocket switch,
  server responds with 101, connection is now WebSocket.
```

### 2xx — Success

```
200 OK
  Standard success. Response body contains the result.
  GET → body is the resource
  POST action → body is the result

201 Created
  Resource was created successfully.
  Should include Location header pointing to the new resource.
  POST /api/students → 201 + Location: /api/students/new-cuid

202 Accepted
  Request accepted but not yet processed.
  Used for async operations: "We'll process this in the background."
  Response body often includes a job ID to poll status.

204 No Content
  Success, but no body to return.
  DELETE → 204 (resource deleted, nothing to return)
  PUT/PATCH updates where you don't need the updated resource back.
  
  Important: 204 must NOT have a body. Browser ignores any body.

206 Partial Content
  Response is a range of the requested content.
  Used with Range header for resumable downloads, video streaming.
  Browser requests video: Range: bytes=0-1023
  Server returns: 206 + Content-Range: bytes 0-1023/45678901
```

### 3xx — Redirection

```
301 Moved Permanently
  Resource has a new permanent URL.
  Browser caches this redirect indefinitely.
  Future requests go directly to new URL.
  Use for: HTTP → HTTPS redirect, domain changes.

302 Found  (historically misused as "temporary redirect")
  Resource is temporarily at a different URL.
  Not cached.
  Browser changes method to GET on redirect (misuse of spec).

303 See Other
  Redirect to a different URL with GET.
  Used after POST to redirect to a "result" page.
  Post/Redirect/Get pattern: prevents form resubmission on refresh.

304 Not Modified
  Client sent a conditional request (If-None-Match, If-Modified-Since).
  Server checked: the resource hasn't changed.
  Client should use its cached version.
  Body is empty. Saves bandwidth.

307 Temporary Redirect
  Like 302, but browser MUST use the same method.
  POST → 307 → POST (not GET).

308 Permanent Redirect
  Like 301, but browser MUST use the same method.
  POST → 308 → POST (not GET).
```

### 4xx — Client Error

```
400 Bad Request
  The request is malformed. Server cannot parse it.
  Wrong JSON syntax, missing required field, invalid value.
  Response body should explain what's wrong.

401 Unauthorized  (name is misleading — means "not authenticated")
  You need to authenticate but haven't.
  Or your credentials are invalid/expired.
  Response includes WWW-Authenticate header telling client how to auth.
  
  In Paideon: no JWT token sent, or token is expired.

403 Forbidden  (means "not authorized")
  You are authenticated, but you don't have permission.
  
  In Paideon: authenticated as a student, trying to access admin endpoint.

404 Not Found
  Resource doesn't exist at this URL.
  Used for: URL doesn't match any route, or resource not found in DB.
  
  Security note: 404 vs 403. Sometimes you want to return 404
  even when a resource exists but the user lacks permission.
  This prevents leaking that the resource exists at all.

405 Method Not Allowed
  The URL exists but not for this HTTP method.
  GET /api/students → 200
  DELETE /api/students → 405 (can't delete the entire collection)
  Response includes Allow: GET, POST header.

409 Conflict
  Request conflicts with current state.
  POST /api/students with an email that already exists → 409
  Optimistic locking: you sent version=5 but current version is 6 → 409

410 Gone
  Like 404, but confirms the resource existed and was permanently deleted.
  Use when you want to tell crawlers to remove the URL from indexes.

422 Unprocessable Entity
  Request is syntactically valid JSON but fails validation.
  Valid JSON, but email field is not a valid email format → 422
  NestJS class-validator throws 422 by default.

429 Too Many Requests
  Rate limit exceeded.
  Response should include:
    Retry-After: 60  (seconds until the client can retry)
    X-RateLimit-Limit: 100
    X-RateLimit-Remaining: 0
    X-RateLimit-Reset: 1741855200
```

### 5xx — Server Error

```
500 Internal Server Error
  Something went wrong on the server that wasn't the client's fault.
  Should NEVER expose stack traces or internal details in production.
  Log the details internally, return a generic message.

501 Not Implemented
  Server doesn't support the requested method.
  Rare — mostly academic.

502 Bad Gateway
  Nginx received an invalid response from your NestJS app.
  Usually means NestJS crashed or isn't running.
  Classic Paideon debugging scenario: NestJS down → Nginx returns 502.

503 Service Unavailable
  Server is temporarily down.
  Overloaded, or in maintenance mode.
  Typically includes Retry-After header.

504 Gateway Timeout
  Nginx waited too long for NestJS to respond.
  NestJS is running but too slow (infinite loop, database deadlock).
  Default Nginx proxy timeout: 60 seconds.
```

---

## 7. Headers — Complete Reference

### Request Headers

```
Authorization
  Carries credentials.
  Bearer eyJhbGc...   → JWT or OAuth token
  Basic dXNlcjpwYXNz  → Base64(username:password) — ONLY over HTTPS
  
  NestJS extracts this in guards:
    const token = request.headers['authorization']?.split(' ')[1]

Accept
  Formats the client will accept in response.
  application/json    → JSON only
  text/html           → HTML only
  */*                 → Anything
  
  Multiple with priority:
  Accept: text/html, application/json;q=0.9, */*;q=0.8
  q= values are quality weights. Default is 1.0.

Accept-Encoding
  Compression algorithms client supports.
  Accept-Encoding: gzip, deflate, br
  Server compresses response with one of these.
  br = Brotli (better compression than gzip)

Accept-Language
  Preferred languages.
  Accept-Language: en-US, en;q=0.9, si;q=0.8
  Sinhala at quality 0.8 if English unavailable.

Cache-Control (request)
  no-cache  → validate with server even if cached copy exists
  no-store  → don't cache at all
  max-age=0 → treat cached copy as stale

Content-Type
  Format of the REQUEST body.
  application/json                    → JSON body
  application/x-www-form-urlencoded   → HTML form data (key=value&key2=value2)
  multipart/form-data                 → File uploads
  text/plain                          → Raw text

If-None-Match
  Conditional request. Send me the resource only if it has changed.
  If-None-Match: "abc123"   ← ETag from previous response
  Server checks: if current ETag matches → 304, else → 200 + new content

If-Modified-Since
  Conditional request by date.
  If-Modified-Since: Mon, 09 Mar 2026 00:00:00 GMT
  Server checks last-modified time → 304 or 200.

Range
  Request a portion of a resource.
  Range: bytes=0-1023       → first 1024 bytes
  Range: bytes=1024-2047    → second 1024 bytes
  Range: bytes=-1024        → last 1024 bytes
  Server responds with 206 Partial Content.
```

### Response Headers

```
Cache-Control (response)
  Controls how clients and proxies cache the response.
  
  no-store             → Never cache. Auth responses, sensitive data.
  no-cache             → Cache but revalidate with server every time.
  private              → Browser can cache, CDNs cannot.
  public               → Everyone (CDNs, proxies) can cache.
  max-age=3600         → Cache for 3600 seconds (1 hour).
  s-maxage=86400       → CDN cache for 1 day (overrides max-age for CDNs).
  immutable            → Never revalidate. Use with content-hashed filenames.
  must-revalidate      → Don't serve stale even if server is down.
  
  Paideon examples:
  API responses:         Cache-Control: no-store
  Static JS/CSS bundle:  Cache-Control: public, max-age=31536000, immutable
  (The filename contains a hash — if content changes, filename changes)

ETag
  A fingerprint of the response content.
  ETag: "d41d8cd98f00b204e9800998ecf8427e"
  Client stores this with the cached response.
  Next request: If-None-Match: "d41d8cd..." → 304 if unchanged.

Last-Modified
  When the resource was last changed.
  Last-Modified: Mon, 09 Mar 2026 14:22:00 GMT
  Client sends: If-Modified-Since: Mon, 09 Mar 2026 14:22:00 GMT → 304 if not changed.

Location
  URL to redirect to (3xx) or URL of newly created resource (201).

Set-Cookie
  Sets a cookie in the browser. Full reference in Part 04.
  Set-Cookie: refresh_token=eyJ...; HttpOnly; Secure; SameSite=Strict; Max-Age=604800

Strict-Transport-Security
  HSTS — tells browser to always use HTTPS for this domain.
  Strict-Transport-Security: max-age=31536000; includeSubDomains; preload

X-Content-Type-Options
  X-Content-Type-Options: nosniff
  Tells browser: don't try to guess the content type.
  Use exactly what Content-Type says.
  Prevents MIME sniffing attacks.

X-Frame-Options
  X-Frame-Options: DENY
  Prevents this page from being loaded in an iframe.
  Prevents clickjacking attacks.
  Superseded by Content-Security-Policy frame-ancestors.

Retry-After
  Used with 429 or 503. When can the client retry?
  Retry-After: 60        → retry after 60 seconds
  Retry-After: Fri, 13 Mar 2026 09:00:00 GMT → retry after this datetime
```

---

## 8. Content-Type — Complete Reference

```
text/html                         → HTML documents
text/css                          → CSS stylesheets
text/javascript                   → JavaScript (legacy)
application/javascript            → JavaScript (current standard)
application/json                  → JSON data
application/xml                   → XML data
application/pdf                   → PDF documents
application/zip                   → ZIP archives
application/octet-stream          → Generic binary (download prompt)
application/x-www-form-urlencoded → HTML form data
multipart/form-data               → File uploads (mixed content)
image/jpeg                        → JPEG image
image/png                         → PNG image
image/webp                        → WebP image (better compression)
image/svg+xml                     → SVG vector image
image/gif                         → GIF image
audio/mpeg                        → MP3 audio
video/mp4                         → MP4 video
font/woff2                        → Web font (WOFF2 format)
```

### Form data encoding formats

```
application/x-www-form-urlencoded:
  name=Ashan+Silva&email=ashan%40school.lk&class=10A
  
  Spaces encoded as +
  Special chars percent-encoded (%40 = @)
  & separates fields
  Used by HTML forms without file upload

multipart/form-data:
  --boundary-abc123
  Content-Disposition: form-data; name="name"
  
  Ashan Silva
  --boundary-abc123
  Content-Disposition: form-data; name="avatar"; filename="photo.jpg"
  Content-Type: image/jpeg
  
  [binary file data]
  --boundary-abc123--
  
  Each field is separated by the boundary string.
  Required when uploading files — binary data can't be percent-encoded.
```

---

## 9. HTTP vs HTTPS — The Difference at the Wire

```
HTTP (no encryption):

TCP connection established
→ Request sent as plain text
→ Anyone on the network can read it

Example: your WiFi router sees:
POST /api/auth/login HTTP/1.1
{"email":"ashan@school.lk","password":"my-password-in-plain-text"}


HTTPS:

TCP connection established
TLS handshake (see Part 03)
→ Request is encrypted before leaving your device

Example: your WiFi router sees:
[completely random bytes — looks like noise]
17 03 03 01 2F 8A 4F E3 B2 91...

The URL path is also encrypted.
Your ISP sees: you connected to 104.21.45.67:443
They cannot see: which page you're on, what data you sent

Exception: the Host header is sent in SNI before encryption begins.
So your ISP can see: you connected to paideon.lk
But not: /api/auth/login, or any headers/body.
```

---

## 10. Keep-Alive and Connection Reuse

HTTP/1.1 defaults to persistent connections. This is critical for performance.

```
Without keep-alive (HTTP/1.0 behavior):
  TCP handshake → request → response → TCP teardown
  TCP handshake → request → response → TCP teardown
  TCP handshake → request → response → TCP teardown
  
  3 requests = 3 TCP handshakes = 3× round trips wasted

With keep-alive (HTTP/1.1 default):
  TCP handshake
  → request → response
  → request → response
  → request → response
  → [connection closes after idle timeout or explicit Connection: close]
  
  3 requests = 1 TCP handshake

Headers:
  Request:  Connection: keep-alive
  Response: Connection: keep-alive
            Keep-Alive: timeout=5, max=100
            (close after 5 seconds idle OR 100 requests)

To close:
  Connection: close
  → after this response, connection closes
```

---

## 11. HTTP in NestJS — What Actually Happens

When you write a NestJS controller, here's what happens at the HTTP level:

```typescript
@Controller('api/students')
export class StudentsController {
  @Get(':id')
  findOne(@Param('id') id: string) {
    return this.studentsService.findOne(id);
  }
}
```

```
Incoming HTTP request:
  GET /api/students/cuid-123 HTTP/1.1
  Host: paideon.lk
  Authorization: Bearer eyJ...

NestJS processes:
1. HTTP adapter (Fastify or Express) parses the raw TCP bytes
2. Routing: matches GET /api/students/:id → StudentsController.findOne
3. Guards run (JwtAuthGuard checks Authorization header)
4. Interceptors run (logging, transformation)
5. Pipes run (validate/transform @Param('id'))
6. Handler runs → calls service → returns data

NestJS builds response:
7. Return value → serialized to JSON (if no explicit response set)
8. HTTP status: 200 by default (201 if you use @HttpCode(201))
9. Headers added: Content-Type: application/json, Content-Length, etc.
10. Response sent over TCP

Outgoing HTTP response:
  HTTP/1.1 200 OK
  Content-Type: application/json; charset=utf-8
  Content-Length: 184
  
  {"id":"cuid-123","name":"Ashan Silva","class":"10A"}
```

The `@Param`, `@Body`, `@Query`, `@Headers` decorators are just reading from the parsed HTTP request object that Fastify/Express creates from the raw TCP bytes.

---

_Next: [Part 03 — TLS & HTTPS](./Part%2003%20-%20TLS%20%26%20HTTPS.md)_