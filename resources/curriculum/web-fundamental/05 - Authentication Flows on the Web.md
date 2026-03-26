> Authentication is a well-understood problem with well-established patterns. The mistakes are almost always in the implementation, not the concept. This part covers how authentication actually works at the HTTP level — token mechanics, refresh flows, logout, multi-device sessions — and maps it all to Paideon's specific implementation.

---

## Table of Contents

1. [Authentication vs Authorization](#1-authentication-vs-authorization)
2. [The Two Fundamental Auth Approaches](#2-the-two-fundamental-auth-approaches)
3. [JWT — Anatomy and Mechanics](#3-jwt--anatomy-and-mechanics)
4. [The Access Token + Refresh Token Pattern](#4-the-access-token--refresh-token-pattern)
5. [The Full Login Flow — HTTP Level](#5-the-full-login-flow--http-level)
6. [Silent Token Refresh — How It Works](#6-silent-token-refresh--how-it-works)
7. [Logout — What It Actually Means](#7-logout--what-it-actually-means)
8. [Multi-Device Sessions](#8-multi-device-sessions)
9. [Token Storage — Where and Why](#9-token-storage--where-and-why)
10. [Common Auth Vulnerabilities](#10-common-auth-vulnerabilities)
11. [Paideon Auth Flow — Complete Reference](#11-paideon-auth-flow--complete-reference)

---

## 1. Authentication vs Authorization

These two words are often conflated. They are distinct steps.

```
Authentication (AuthN):
  "Who are you?"
  Verifying identity.
  
  You claim to be Ashan Silva.
  You prove it with a password, biometric, MFA code.
  
  Result: a verified identity (usually a user ID and role).

Authorization (AuthZ):
  "What are you allowed to do?"
  Verifying permissions.
  
  Ashan is verified as a student.
  Can he access /api/grades/modify? No — that's a teacher action.
  Can he access /api/students/his-own-id? Yes.
  
  Result: allow or deny the specific action.

Order:
  Authentication ALWAYS comes first.
  You cannot make authorization decisions about an unknown identity.

HTTP status codes:
  401 Unauthorized → actually means "not authenticated"
  403 Forbidden    → means "authenticated but not authorized"
```

---

## 2. The Two Fundamental Auth Approaches

### Session-Based (Stateful)

```
LOGIN:
  Client sends credentials.
  Server verifies, creates a session record in the database:
    sessions table: { id: "sess_abc", userId: "cuid-123", createdAt, expiresAt }
  Server sets a cookie: Set-Cookie: session_id=sess_abc; HttpOnly; Secure

SUBSEQUENT REQUESTS:
  Browser sends: Cookie: session_id=sess_abc (automatic)
  Server: looks up sess_abc in database → finds userId cuid-123 → authenticated

LOGOUT:
  DELETE sessions record from database.
  Cookie is now useless — server won't find the session.

PROS:
  Instant revocation — delete session row → user is logged out everywhere.
  Tokens are opaque → cannot be decoded to reveal user data.
  
CONS:
  Stateful — every request requires a database lookup.
  Hard to scale across multiple servers without shared session storage.
  (Redis is commonly used as a session store for distributed systems.)
```

### Token-Based (Stateless) with JWT

```
LOGIN:
  Client sends credentials.
  Server verifies, generates a JWT (signed with server's secret).
  Server sends JWT to client.
  Server stores NOTHING.

SUBSEQUENT REQUESTS:
  Client sends: Authorization: Bearer eyJhbGc...
  Server: verifies JWT signature → reads claims → authenticated.
  NO database lookup required.

LOGOUT:
  Client deletes the token locally.
  Server cannot invalidate a valid token (no server state).
  Token remains valid until it expires.

PROS:
  Stateless — any server can verify any token.
  Scalable — no shared session storage needed.
  Works well for APIs consumed by multiple clients (web, mobile, 3rd party).
  
CONS:
  No instant revocation — if a token is stolen, it's valid until expiry.
  Mitigation: short expiry (15 min) + refresh token rotation.
  JWT payload is base64-encoded (not encrypted) — anyone can decode it.
  Don't store sensitive data in JWT claims.
```

---

## 3. JWT — Anatomy and Mechanics

JWT (JSON Web Token) is a compact, self-contained token format. Format: `header.payload.signature`.

```
Full JWT:
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJjdWlkLTEyMyIsImVtYWlsIjoiYXNoYW5Ac2Nob29sLmxrIiwicm9sZSI6InN0dWRlbnQiLCJzY2hvb2xJZCI6InNjaG9vbC1hYmMiLCJpYXQiOjE3NDE4NTUyMDAsImV4cCI6MTc0MTg1NjEwMH0.xyz_signature_here

Each part is base64url-encoded. Separated by dots.
```

### Header

```json
{
  "alg": "HS256",
  "typ": "JWT"
}

alg: the signing algorithm
  HS256: HMAC-SHA256 — symmetric (one secret key for signing AND verifying)
  RS256: RSA-SHA256  — asymmetric (private key signs, public key verifies)
  ES256: ECDSA-SHA256 — asymmetric, more efficient than RSA

For Paideon (single server): HS256 is fine.
For distributed systems: RS256 lets other services verify tokens with
  just the public key, never needing the signing secret.
```

### Payload (Claims)

```json
{
  "sub":      "cuid-123",
  "email":    "ashan@school.lk",
  "role":     "student",
  "schoolId": "school-abc",
  "iat":      1741855200,
  "exp":      1741856100
}

Standard claims (iana-registered):
  sub:  Subject — who the token is about (user ID)
  iat:  Issued At — unix timestamp when token was created
  exp:  Expiration — unix timestamp when token expires
  iss:  Issuer — who created this token ("paideon-api")
  aud:  Audience — who the token is intended for ("paideon-web")
  jti:  JWT ID — unique ID for this token (used for revocation tracking)

Custom claims:
  role, schoolId, email — anything your application needs.

CRITICAL: the payload is base64url ENCODED, NOT encrypted.
Anyone can decode it:
  atob("eyJzdWIiOiJjdWlkLTEyMyI...") → readable JSON

Never put in JWT payload:
  Passwords
  Private keys
  Sensitive personal data (medical records, payment info)
  Anything you wouldn't want visible on a public billboard

Safe in JWT payload:
  User ID, email, role, schoolId, permissions
```

### Signature

```
HMAC-SHA256(
  base64url(header) + "." + base64url(payload),
  secret_key
)

The signature cryptographically binds the header and payload to the secret.

If anyone modifies the payload (e.g., changes "role":"student" to "role":"admin"):
  The signature no longer matches → server rejects it.

The secret MUST be kept secret.
If the secret leaks → anyone can forge tokens → complete auth bypass.

Store in environment variable, never in code or git:
  JWT_SECRET=a-very-long-random-string-at-least-256-bits
  
Generate: node -e "console.log(require('crypto').randomBytes(64).toString('hex'))"
```

---

## 4. The Access Token + Refresh Token Pattern

Short-lived access tokens + long-lived refresh tokens solve the statelessness/revocation tradeoff.

```
Access Token:
  Short-lived: 15 minutes
  Stateless: server verifies signature only, no DB lookup
  Stored: JavaScript memory (most secure for XSS)
  Sent with: every API request (Authorization: Bearer ...)
  
Refresh Token:
  Long-lived: 7 days
  Stateful: stored in database (can be revoked)
  Stored: HttpOnly, Secure, SameSite=Strict cookie
  Sent with: only to /api/auth/refresh endpoint

Why two tokens?
  If access token is stolen: attacker has 15 minutes.
  Token expires → they're locked out. No action required from user.
  
  If refresh token is stolen: this is serious.
  But: it's in HttpOnly cookie → JS cannot read it → XSS cannot steal it.
  Network theft requires breaking HTTPS → very difficult.
  DB revocation: you can still invalidate it.

Refresh token rotation:
  Every time you use a refresh token, it's invalidated and replaced.
  
  Client: POST /api/auth/refresh (sends refresh token cookie)
  Server: 
    1. Verify refresh token exists in DB and is not revoked
    2. Delete old refresh token
    3. Create new refresh token, store in DB
    4. Issue new access token
    5. Set new refresh token in cookie
  
  If attacker steals refresh token and uses it:
    → New token issued to attacker
    → Original token invalidated
    → Next time legitimate user tries to refresh: their token is gone → logged out
    → User notices, contacts support → all tokens revoked
  
  This is "refresh token rotation with reuse detection."
```

---

## 5. The Full Login Flow — HTTP Level

```
STEP 1: User submits login form
─────────────────────────────────────────────────────────────
POST /api/v1/auth/login HTTP/1.1
Host: paideon.lk
Content-Type: application/json

{"email":"ashan@school.lk","password":"correct-horse-battery-staple"}


STEP 2: Server validates credentials
─────────────────────────────────────────────────────────────
1. Find user by email in database
2. If not found → 401 (but say "invalid credentials", not "email not found")
   (revealing that email exists = user enumeration vulnerability)
3. Compare password with bcrypt hash:
   bcrypt.compare(plaintext, hash) → true/false
4. If false → 401 "Invalid credentials"
5. Check if account is suspended/deleted → 403 "Account suspended"
6. Generate access token (15min expiry)
7. Generate refresh token (7 days), store in DB


STEP 3: Success response
─────────────────────────────────────────────────────────────
HTTP/1.1 200 OK
Content-Type: application/json
Set-Cookie: refresh_token=eyJhbGc...; HttpOnly; Secure; SameSite=Strict; Max-Age=604800; Path=/api/v1/auth

{
  "accessToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": "cuid-123",
    "name": "Ashan Silva",
    "email": "ashan@school.lk",
    "role": "student",
    "schoolId": "school-abc"
  }
}


STEP 4: Client stores tokens
─────────────────────────────────────────────────────────────
// In Next.js / React:
const { accessToken, user } = response.data;
// accessToken stored in JavaScript variable / Zustand/Redux state
// refresh_token cookie is stored automatically by browser
// user object stored in app state

// Access token is now in memory.
// On page refresh, it's gone → will be refreshed silently.
```

---

## 6. Silent Token Refresh — How It Works

When the access token expires (or on page load after a refresh), the client fetches a new one without the user noticing.

```
CLIENT STARTUP (page load or app init):
─────────────────────────────────────────────────────────────
1. Check if access token in memory: null (page was refreshed)
2. POST /api/v1/auth/refresh (refresh token cookie sent automatically)
3. If 200: store new access token in memory, proceed normally
4. If 401: no valid refresh token → redirect to login


ACCESS TOKEN EXPIRY DURING SESSION:
─────────────────────────────────────────────────────────────
Approach A: Proactive refresh (recommended)
  Set a timer for 14 minutes after login.
  At 14 min: silently call /api/v1/auth/refresh before token expires.
  User never hits a 401.
  
  setTimeout(() => {
    silentRefresh();
  }, 14 * 60 * 1000);  // 14 minutes in ms

Approach B: Reactive refresh
  Make API call → receive 401 TOKEN_EXPIRED.
  Pause the original request.
  Call /api/v1/auth/refresh.
  Retry the original request with new token.
  
  // Axios interceptor example:
  axios.interceptors.response.use(
    response => response,
    async error => {
      if (error.response?.status === 401 && 
          error.response?.data?.error?.code === 'TOKEN_EXPIRED') {
        await refreshAccessToken();
        return axios.request(error.config);  // retry original
      }
      return Promise.reject(error);
    }
  );


TOKEN REFRESH HTTP:
─────────────────────────────────────────────────────────────
POST /api/v1/auth/refresh HTTP/1.1
Host: paideon.lk
Cookie: refresh_token=eyJhbGc...  (automatic — browser sends this)

HTTP/1.1 200 OK
Set-Cookie: refresh_token=eyJhbGc_NEW...; HttpOnly; Secure; ...  (new cookie)

{"accessToken": "eyJhbGciOiJIUzI1NiJ9_NEW..."}

Client: updates in-memory access token.
Browser: updates cookie automatically.
User: notices nothing.
```

---

## 7. Logout — What It Actually Means

Logout has two parts: client-side cleanup and server-side revocation.

```
LOGOUT HTTP:
─────────────────────────────────────────────────────────────
POST /api/v1/auth/logout HTTP/1.1
Authorization: Bearer eyJhbGc...
Cookie: refresh_token=eyJhbGc...

Server:
  1. Extract refresh token from cookie
  2. Delete it from database (revoked)
  3. Optionally: add access token's jti to a revocation cache
     (blacklist, stored in Redis with TTL = token's remaining life)

HTTP/1.1 200 OK
Set-Cookie: refresh_token=; Max-Age=0; HttpOnly; Secure; SameSite=Strict; Path=/api/v1/auth

{}

Max-Age=0 instructs browser to delete the cookie immediately.

CLIENT:
  Clear access token from memory.
  Clear user state.
  Redirect to /login.


WHAT HAPPENS IF LOGOUT REQUEST FAILS?
─────────────────────────────────────────────────────────────
Client-side: clear token from memory regardless.
  The user is "logged out" from the client's perspective.
  But the refresh token cookie may still be in the browser.
  And the refresh token may still be valid in the DB.

If the request truly failed (network error):
  Retry the logout request.
  Or: clear the cookie from client side manually (set Max-Age=0 from JS).
  Note: HttpOnly cookies CANNOT be deleted by JavaScript directly.
        Only the server can clear HttpOnly cookies.
        This is a subtle limitation.

Best practice:
  Server revokes the token.
  Server clears the cookie.
  Client clears its state.
  All three are necessary for complete logout.
```

---

## 8. Multi-Device Sessions

A user can be logged in on multiple devices simultaneously. Each device has its own refresh token.

```
Database: refresh_tokens table

id          | userId      | token (hashed) | deviceInfo        | createdAt | expiresAt | revoked
──────────────────────────────────────────────────────────────────────────────────────────────────
rt-cuid-1   | cuid-123    | hash-abc       | Chrome, Windows   | 2026-03-01 | 2026-03-08 | false
rt-cuid-2   | cuid-123    | hash-def       | Safari, iPhone    | 2026-03-05 | 2026-03-12 | false
rt-cuid-3   | cuid-123    | hash-xyz       | Firefox, Ubuntu   | 2026-03-10 | 2026-03-17 | false

Ashan is logged in on 3 devices.
Each has its own refresh token.
Each gets its own access tokens.

"Log out all devices" = revoke all refresh tokens for this userId:
  UPDATE refresh_tokens SET revoked = true WHERE userId = 'cuid-123'

"Log out this device" = revoke just the current refresh token:
  UPDATE refresh_tokens SET revoked = true WHERE id = 'rt-cuid-2'

"View active sessions":
  SELECT deviceInfo, createdAt FROM refresh_tokens 
  WHERE userId = 'cuid-123' AND revoked = false AND expiresAt > NOW()

Don't store the raw refresh token in the database.
Store a hash: refreshTokenHash = bcrypt.hash(token, 10)
Or: SHA-256(token) → faster than bcrypt, appropriate for tokens (which are already random).
When verifying: hash the incoming token, compare to stored hash.
```

---

## 9. Token Storage — Where and Why

This topic has strong opinions. Here is the full analysis.

```
OPTION A: HttpOnly Cookie (for access token)
─────────────────────────────────────────────────────────────
Storage: HttpOnly, Secure, SameSite=Strict cookie
Sent with: every request automatically (to matching domain/path)

XSS: cannot steal — JS cannot read HttpOnly cookie
CSRF: protected by SameSite=Strict
Network: protected by Secure (HTTPS only)

Limitation: automatically sent on ALL requests to matching domain.
  Must configure SameSite and Path precisely.
  Cross-origin API calls: cookie won't be sent (SameSite=Strict blocks cross-origin).


OPTION B: Memory (for access token)
─────────────────────────────────────────────────────────────
Storage: JavaScript variable / Zustand store / Redux state
Sent with: manually added to each request

XSS: CAN be stolen if attacker has XSS during token's lifetime.
  But: 15-minute TTL limits window of opportunity.
  Best-effort XSS mitigation: Content-Security-Policy.
CSRF: not vulnerable (token not sent automatically).
Network: protected by HTTPS.

Limitation: lost on page refresh (requires silent refresh).


OPTION C: localStorage / sessionStorage (WRONG for tokens)
─────────────────────────────────────────────────────────────
XSS: IMMEDIATELY steals token. Any XSS attack reads it.
     Attacker has unlimited time to use it (no page close required).
     
DO NOT store auth tokens (access or refresh) in localStorage or sessionStorage.
This is one of the most common security mistakes in web development.


PAIDEON RECOMMENDED APPROACH:
  Refresh token: HttpOnly, Secure, SameSite=Strict cookie
  Access token: JavaScript memory
  
  Best of both worlds:
  - Refresh token (the valuable one) is fully protected from XSS
  - Access token in memory: loses it on refresh, needs silent refresh
  - Access token has 15min TTL: even if stolen via XSS, short window
```

---

## 10. Common Auth Vulnerabilities

### User Enumeration

```
WRONG — tells attacker if email exists:
  POST /api/auth/login { email: "unknown@x.com", password: "wrong" }
  → 404 Not Found ("user not found")
  
  POST /api/auth/login { email: "real@user.com", password: "wrong" }
  → 401 Unauthorized ("invalid password")

CORRECT — same message regardless:
  POST /api/auth/login { email: "anything", password: "anything" }
  → 401 "Invalid credentials"    (always the same)

Also applies to:
  Password reset: "If that email exists, we've sent a reset link" (never confirm/deny)
  Registration: don't say "email already registered" — say "check your email"
```

### Timing Attacks

```
WRONG:
  const user = await db.findByEmail(email);
  if (!user) return 401;         // returns instantly
  const valid = await bcrypt.compare(password, user.hash);
  if (!valid) return 401;        // returns after bcrypt (slow)

Attacker can measure response time:
  Fast response → user doesn't exist
  Slow response → user exists, wrong password

CORRECT:
  const user = await db.findByEmail(email);
  const dummyHash = '$2b$12$invalidhashfortimingnormalization';
  const hash = user?.passwordHash ?? dummyHash;
  const valid = await bcrypt.compare(password, hash);  // always runs bcrypt
  if (!user || !valid) return 401;
  // Response time is the same whether user exists or not.
```

### JWT Algorithm Confusion

```
WRONG — accepting "none" algorithm:
  Header: { "alg": "none" }
  
  Some JWT libraries accept alg=none → signature is not verified.
  Attacker can forge any payload with no signature.

CORRECT:
  Always specify accepted algorithms explicitly:
  jwt.verify(token, secret, { algorithms: ['HS256'] })
  
  Never accept "none" or RS256 when you expect HS256.
  
Also wrong: accepting RS256 when configured for HS256.
  In RS256, the server's public key is used to verify.
  Attacker could set alg=HS256 and sign with the PUBLIC key.
  If the library doesn't enforce algorithm → accepted.
```

---

## 11. Paideon Auth Flow — Complete Reference

```
ROLES:
  super_admin  → Paideon platform administrators
  school_admin → School administrators (principals, registrars)
  teacher      → Teachers
  student      → Students
  parent       → Parents (read-only, their children's data)

JWT payload:
  {
    "sub": "cuid-123",           User ID
    "role": "student",            Role
    "schoolId": "school-abc",     Multi-tenant isolation
    "email": "ashan@school.lk",   
    "iat": 1741855200,
    "exp": 1741856100             15 minutes from iat
  }

GUARD HIERARCHY in NestJS:
  JwtAuthGuard     → verifies JWT signature, checks expiry
                     runs first on every protected route
  
  RolesGuard       → checks role claim against @Roles() decorator
                     @Roles('teacher', 'school_admin')
  
  SchoolGuard      → verifies schoolId in token matches
                     the school of the resource being accessed
                     prevents cross-school data access

PROTECTED ROUTE FLOW:
  Request arrives
    ↓
  JwtAuthGuard: extract token from Authorization header
    → jwt.verify(token, JWT_SECRET, { algorithms: ['HS256'] })
    → invalid/expired → 401
    → valid → req.user = { sub, role, schoolId, email }
    ↓
  RolesGuard: check req.user.role against allowed roles
    → not allowed → 403
    ↓
  SchoolGuard: check req.user.schoolId matches resource's schoolId
    → mismatch → 403
    ↓
  Controller handler executes

RATE LIMITING on auth endpoints (prevent brute force):
  POST /api/auth/login:      5 attempts per 15 minutes per IP
  POST /api/auth/refresh:    20 attempts per minute per IP
  POST /api/auth/reset:      3 attempts per hour per email
  
  After 5 failed logins: temporary account lock (5 minutes).
  Log all failed attempts with IP for security audit.
```

---

_Next: [Part 07 — Web Performance](06%20-%20Web%20Performance.md)_