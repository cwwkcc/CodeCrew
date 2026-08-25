> Sessions and cookies are how HTTP — a stateless protocol — remembers who you are between requests. Understanding exactly how they work, what the flags mean, and how they're attacked is foundational to building secure authentication.

---

## Table of Contents

1. [HTTP Is Stateless — The Problem](#1-http-is-stateless--the-problem)
2. [Sessions — Server-Side State](#2-sessions--server-side-state)
3. [Cookies — The Transport Layer](#3-cookies--the-transport-layer)
4. [Cookie Attributes](#4-cookie-attributes)
5. [Session Fixation](#5-session-fixation)
6. [Session Hijacking](#6-session-hijacking)
7. [Token-Based Sessions vs Cookie Sessions](#7-token-based-sessions-vs-cookie-sessions)
8. [Refresh Token in Cookie Pattern](#8-refresh-token-in-cookie-pattern)
9. [Session Expiry — Absolute vs Idle](#9-session-expiry--absolute-vs-idle)
10. [Secure Logout](#10-secure-logout)

---

## 1. HTTP Is Stateless — The Problem

Every HTTP request is independent. The server has no memory of previous requests by default.

```
Request 1:  GET /login  → "who are you?" → you log in
Request 2:  GET /dashboard → server has no idea you just logged in → "who are you?"

Without sessions, every request would require re-authentication.
```

Session management is the mechanism that persists identity across requests. It's one of the most attack-rich areas of web security.

---

## 2. Sessions — Server-Side State

Traditional session management:

```
1. User logs in with correct credentials
2. Server creates a session:
   sessionId = crypto.randomBytes(32).toString("hex")  // 64 hex chars, 256 bits
   sessionStore[sessionId] = { userId, role, createdAt, expiresAt }
3. Server sends sessionId to browser in a cookie:
   Set-Cookie: sessionId=abc123...; HttpOnly; Secure; SameSite=Strict
4. Browser stores the cookie, sends it with every subsequent request
5. Server looks up sessionStore[sessionId] → knows who the user is
```

The session ID is just a random pointer to server-side data. It carries no information itself — the server holds all the state.

**Session store options:**

```
In-memory (single server):   Map() or simple object — lost on restart
Redis:                        Fast, persistent, supports expiry, shared across servers
Database:                     Slower, persistent, easy to query for "all sessions by user"
```

---

## 3. Cookies — The Transport Layer

A cookie is a key-value pair the browser stores and sends with every request to the matching domain.

```
Server → Browser:
  Set-Cookie: name=value; options

Browser → Server (every subsequent request to domain):
  Cookie: name=value
```

Cookies are the standard transport for session IDs and tokens in web applications. They have security attributes that control their behavior.

---

## 4. Cookie Attributes

### HttpOnly

```
Set-Cookie: sessionId=abc123; HttpOnly

Without HttpOnly:
  JavaScript can read: document.cookie  → "sessionId=abc123"
  XSS attack steals the session ID → attacker takes over session

With HttpOnly:
  JavaScript cannot read the cookie — only the browser sees it
  document.cookie does NOT include HttpOnly cookies
  XSS can't steal session IDs stored in HttpOnly cookies
```

**Always set HttpOnly on auth cookies** (session IDs, refresh tokens). Any cookie that shouldn't be read by JavaScript should be HttpOnly.

Access tokens (if in a cookie) are sometimes NOT HttpOnly — because the frontend needs to read them to send as a Bearer header. The refresh token should always be HttpOnly.

### Secure

```
Set-Cookie: sessionId=abc123; Secure

Without Secure:
  Cookie sent over both HTTP and HTTPS
  On HTTP (even by accident) → cookie visible in plaintext on the wire

With Secure:
  Cookie only sent over HTTPS connections
  Never sent over plain HTTP
```

**Always set Secure in production.** Combine with HSTS to ensure no HTTP requests ever happen.

### SameSite

```
Set-Cookie: sessionId=abc123; SameSite=Strict

SameSite=Strict:
  Cookie NOT sent with cross-site requests at all
  Visiting evil.com → clicking link to bank.com → cookie not sent
  User appears logged out until they directly navigate to bank.com
  Best for security; can cause UX friction (OAuth redirects lose session)

SameSite=Lax (browser default since ~2020):
  Cookie sent with same-site requests
  Cookie sent with top-level navigation GET (clicking a link to the site)
  Cookie NOT sent with: POST, iframe, fetch, XHR from other sites
  Protects against most CSRF; allows normal link navigation

SameSite=None:
  Cookie always sent cross-site
  Must be combined with Secure
  Only needed for: embedded widgets, OAuth flows that need cross-site cookies
```

### Domain and Path

```
Set-Cookie: sessionId=abc123; Domain=example.com; Path=/

Domain=example.com:
  Cookie sent to example.com AND all subdomains (api.example.com, etc.)
  Without Domain attribute: only sent to exact domain that set it

  Warning: if you set Domain=example.com from api.example.com,
           the cookie is also sent to mail.example.com, etc.

Path=/:
  Cookie sent for all paths
  Path=/api: cookie only sent for /api/* paths
  Mostly a scoping mechanism, not a security control
  (different paths on same origin can still read each other's data)
```

### Max-Age and Expires

```
Session cookie (no Max-Age/Expires):
  Deleted when browser closes (tab, not just window in modern browsers)

Persistent cookie:
  Set-Cookie: sessionId=abc123; Max-Age=86400  (1 day)
  Set-Cookie: sessionId=abc123; Expires=Thu, 01 Jan 2026 00:00:00 GMT

  Stored on disk — survives browser close
  Deleted at the specified time

For refresh tokens: persistent (days to weeks)
For access tokens in cookies: short max-age or session-only
```

### The Ideal Auth Cookie

```
Set-Cookie: refreshToken=<value>;
  HttpOnly;          ← JS can't read it
  Secure;            ← HTTPS only
  SameSite=Strict;   ← CSRF protection
  Max-Age=604800;    ← 7 days
  Path=/auth;        ← only sent to /auth/* endpoints (reduce exposure)
```

---

## 5. Session Fixation

An attacker sets the session ID before the user logs in, then waits for the user to authenticate with that session ID.

```
Attack:
1. Attacker visits site → receives session ID: "attacker_session_123"
2. Attacker tricks user into using that session ID:
   → Sends link: https://app.com?sessionId=attacker_session_123
   → User's browser stores this session ID
3. User logs in with their credentials
4. Vulnerable server: authenticates user against the existing session
   → session "attacker_session_123" is now authenticated as the user
5. Attacker uses "attacker_session_123" → is now logged in as the user

The attacker knew the session ID before authentication — they "fixed" it.
```

**Defense:** Always generate a NEW session ID after successful authentication.

```ts
// On login:
// 1. Verify credentials
// 2. Destroy the pre-login session
// 3. Create a brand new session with the authenticated user's data

async login(credentials: LoginDto, req: Request): Promise<LoginResponse> {
  const user = await this.verifyCredentials(credentials);

  // Regenerate session ID to prevent session fixation
  await new Promise<void>((resolve, reject) => {
    req.session.regenerate(err => err ? reject(err) : resolve());
  });

  // Now set user data on the new session
  req.session.userId = user.id;
  req.session.role = user.role;

  return { success: true };
}
```

---

## 6. Session Hijacking

An attacker obtains a valid session ID and uses it to impersonate the user.

**How session IDs are stolen:**

```
Network sniffing: session in HTTP (not HTTPS) → visible in plaintext
  Defense: HTTPS everywhere + Secure cookie flag

XSS:           script reads document.cookie
  Defense: HttpOnly flag

Predictable IDs: sequential or weakly random session IDs
  Defense: crypto.randomBytes(32) — 256 bits of entropy

Log exposure:   session ID appears in server access logs (from URL parameter)
  Defense: never put session IDs in URLs — use cookies only

Referrer leakage: session ID in URL → sent in Referer header to external sites
  Defense: Referrer-Policy + never put session IDs in URLs
```

**Defense: session fingerprinting**

Bind the session to client characteristics that shouldn't change:

```ts
// Store at session creation
session.ipAddress = req.ip;
session.userAgent = req.headers["user-agent"];

// Verify on each request
if (
  session.ipAddress !== req.ip ||
  session.userAgent !== req.headers["user-agent"]
) {
  // Session may be hijacked — invalidate and force re-login
  req.session.destroy();
  throw new UnauthorizedException("Session invalidated");
}
```

This isn't foolproof (IPs change on mobile, VPNs, etc.) but raises the bar for attackers.

---

## 7. Token-Based Sessions vs Cookie Sessions

```
Cookie-based (traditional):
  Session ID stored server-side
  Cookie contains only a pointer
  Server must look up session on every request (DB or Redis round-trip)
  Easy to revoke (delete from store)
  Statefulness: session store is shared state — all servers must access it

Token-based (JWT):
  All session data encoded in the token itself
  Server validates signature — no DB lookup
  Stateless — any server can validate any token
  Hard to revoke before expiry
  Token contains data visible to holder (base64 decoded)

Hybrid (this codebase):
  Short-lived JWT access token in memory
  Long-lived refresh token as httpOnly cookie
  Access: stateless JWT validation
  Refresh: DB lookup on /auth/refresh (acceptable — less frequent)
  Revocation: invalidate refresh token in DB → stops issuing new access tokens
```

---

## 8. Refresh Token in Cookie Pattern

The standard pattern for SPAs and mobile apps using JWT:

```
Login response:
  Body:    { accessToken: "eyJ..." }         ← short-lived, 15 min, stored in JS memory
  Cookie:  refreshToken=<value>; HttpOnly; Secure; SameSite=Strict; Path=/auth

Access token usage:
  Authorization: Bearer <accessToken>       ← in header, from JS memory
  → Immune to CSRF (not auto-attached)
  → At risk from XSS (in JS memory) but 15-min lifetime limits damage

Refresh:
  POST /auth/refresh
  Cookie: refreshToken=<value>              ← automatically sent (httpOnly cookie)
  Response: { accessToken: "new eyJ..." }  ← new 15-min access token

Logout:
  POST /auth/logout
  Server: invalidate refresh token in DB, clear the cookie
  Client: clear access token from memory
```

**Why not just a long-lived JWT?**

```
Long-lived JWT (bad):
  24-hour expiry
  Stolen via XSS → attacker has 24 hours
  No way to revoke without DB check (defeats statelessness)

Short JWT + httpOnly refresh token (good):
  15-minute access token → XSS steals it → 15 minutes of damage
  Refresh token in httpOnly cookie → XSS can't steal it
  Server can revoke refresh token immediately → ends all sessions
```

---

## 9. Session Expiry — Absolute vs Idle

```
Absolute expiry:
  Session created at T → expires at T + 8 hours regardless of activity
  User actively using the app for 9 hours → logged out mid-session
  Pro: strict upper bound on session lifetime
  Con: frustrating if user is actively working

Idle timeout:
  Session expires if no activity for X minutes (e.g., 30 min)
  User active → session keeps renewing
  User walks away → session expires after 30 min
  Pro: natural expiry for inactive sessions
  Con: an attacker who steals a session can keep it alive by staying active

Best practice: BOTH
  Idle: 30 minutes (or role-dependent)
  Absolute: 8 hours (or role-dependent)

  Privileged roles → shorter times:
    Admin:   idle 15 min, absolute 4 hours
    Teacher: idle 30 min, absolute 8 hours
    Student: idle 60 min, absolute 12 hours
```

```ts
// Implementing idle timeout with JWT access tokens:
// Each access token has iat (issued at) + exp (expires at, 15 min)
// Refresh token rotation extends the session with each refresh
// If user stops refreshing → access token expires after 15 min → session ends

// Implementing idle detection on frontend:
let lastActivity = Date.now();
window.addEventListener("mousemove", () => (lastActivity = Date.now()));
window.addEventListener("keypress", () => (lastActivity = Date.now()));

setInterval(() => {
  const idle = (Date.now() - lastActivity) / 1000 / 60; // minutes
  if (idle > 30) {
    // Force logout
    authStore.logout();
  }
}, 60_000); // check every minute
```

---

## 10. Secure Logout

Logout must invalidate the server-side state — not just clear client-side storage.

```ts
// WRONG — just clears client storage, server session still valid
function logout() {
  localStorage.removeItem("accessToken");
  // The refresh token cookie is still valid → attacker can still use it!
}

// CORRECT — server-side invalidation
async function logout() {
  await fetch("/auth/logout", {
    method: "POST",
    headers: { Authorization: `Bearer ${accessToken}` },
  });
  // Server: marks refresh token as revoked in DB, clears cookie
  // Client: clear access token from memory
  accessToken = null;
  router.push("/login");
}
```

```ts
// Server-side logout handler
@Post("logout")
@UseGuards(JwtAuthGuard)
async logout(@Req() req: Request, @Res() res: Response, @CurrentUser() user: JwtPayload) {
  // Revoke refresh token in DB
  const refreshToken = req.cookies["refreshToken"];
  if (refreshToken) {
    const hash = crypto.createHash("sha256").update(refreshToken).digest("hex");
    await this.db.refreshToken.updateMany({
      where: { tokenHash: hash, userId: user.sub },
      data: { revokedAt: new Date() },
    });
  }

  // Clear the cookie
  res.clearCookie("refreshToken", {
    httpOnly: true,
    secure: true,
    sameSite: "strict",
    path: "/auth",
  });

  res.json({ success: true });
}
```

**Logout-all-devices:** For "sign out everywhere" functionality, revoke all refresh tokens for the user:

```ts
await this.db.refreshToken.updateMany({
  where: { userId: user.sub, revokedAt: null },
  data: { revokedAt: new Date() },
});
```

---

## Summary

```
Sessions: server-side state, identified by a random session ID (or JWT)
Cookies: the transport — Set-Cookie header, sent automatically by browser

Cookie attributes (all must be set on auth cookies):
  HttpOnly:        JS can't read → XSS can't steal session IDs
  Secure:          HTTPS only → no plaintext transmission
  SameSite=Strict: not sent cross-site → CSRF protection
  Max-Age:         controlled lifetime

Session fixation:
  Attacker pre-sets session ID → user logs in → attacker uses it
  Defense: regenerate session ID after login

Session hijacking:
  Session ID stolen via network, XSS, logs, referrer
  Defense: HTTPS + HttpOnly + Secure + high-entropy IDs

Hybrid pattern (recommended):
  Short-lived JWT access token: in JS memory, Bearer header, CSRF-immune
  Long-lived refresh token: httpOnly cookie, not readable by JS

Session expiry:
  Both idle timeout (inactivity) and absolute timeout (regardless of activity)
  Role-dependent: admins get shorter timeouts

Secure logout:
  Server must revoke refresh token — clearing client storage is not enough
  "Sign out everywhere" → revoke all refresh tokens for the user
```

---

_Next: [23 — Passwords and Hashing](./23-passwords-and-hashing.md)_
