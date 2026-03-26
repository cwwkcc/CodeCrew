> JWTs (JSON Web Tokens) are everywhere in modern authentication. They are also widely misunderstood and frequently misimplemented. This covers the full structure, signing, verification, what to put in them, what not to, and every common implementation mistake.

---

## Table of Contents

1. [What a JWT Is](#1-what-a-jwt-is)
2. [The Three Parts](#2-the-three-parts)
3. [Base64URL Is Not Encryption](#3-base64url-is-not-encryption)
4. [Signing — How the Signature Works](#4-signing--how-the-signature-works)
5. [Verification Flow](#5-verification-flow)
6. [Signing Algorithms — HS256 vs RS256 vs ES256](#6-signing-algorithms--hs256-vs-rs256-vs-es256)
7. [Standard Claims](#7-standard-claims)
8. [What to Put in the Payload (and What Not To)](#8-what-to-put-in-the-payload-and-what-not-to)
9. [JWT Storage — The localStorage vs httpOnly Cookie Debate](#9-jwt-storage--the-localstorage-vs-httonly-cookie-debate)
10. [Access Token + Refresh Token Pattern](#10-access-token--refresh-token-pattern)
11. [Token Revocation Problem](#11-token-revocation-problem)
12. [Common JWT Vulnerabilities](#12-common-jwt-vulnerabilities)
13. [Implementing JWT in NestJS](#13-implementing-jwt-in-nestjs)

---

## 1. What a JWT Is

A JWT is a compact, self-contained token that encodes a JSON payload and proves the payload hasn't been tampered with via a cryptographic signature.

"Self-contained" means the token carries everything needed to verify it — no database lookup required on the receiving end (in the stateless case).

```
Use cases:
  Authentication tokens    → "this user is logged in as alice, role: admin"
  Authorization tokens     → "this service is allowed to call billing API"
  ID tokens (OIDC)         → "this person's identity is alice@gmail.com"
  Short-lived capabilities → "this link can download file XYZ for the next 10 minutes"
```

---

## 2. The Three Parts

A JWT is three base64url-encoded strings joined by dots:

```
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9
.
eyJzdWIiOiJ1c2VyXzEyMyIsImVtYWlsIjoiYWxpY2VAc2Nob29sLmxrIiwicm9sZSI6InRlYWNoZXIiLCJzY2hvb2xJZCI6InNjaG9vbF9hYmMiLCJpYXQiOjE3NDE2OTEyMDAsImV4cCI6MTc0MTY5NDgwMH0
.
SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c
```

### Part 1: Header

```json
{
  "alg": "HS256",
  "typ": "JWT"
}
```

- `alg`: the signing algorithm
- `typ`: token type ("JWT")

### Part 2: Payload (Claims)

```json
{
  "sub": "user_123",
  "email": "alice@school.lk",
  "role": "teacher",
  "schoolId": "school_abc",
  "iat": 1741691200,
  "exp": 1741694800
}
```

This is the actual data. Anyone who has the token can read this. It is not encrypted.

### Part 3: Signature

```
HMAC-SHA256(
  base64url(header) + "." + base64url(payload),
  secret
)
```

The signature proves the header and payload haven't been changed since the server issued the token.

---

## 3. Base64URL Is Not Encryption

This is the most common JWT misconception.

```
Base64URL encoding:
  eyJzdWIiOiJ1c2VyXzEyMyJ9
  ↓ decode ↓
  {"sub":"user_123"}

Base64URL is an encoding, not encryption.
Anyone with the token can decode the payload with zero knowledge of any key.
```

```js
// You can decode any JWT payload without the secret
const token = "eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJ1c2VyXzEyMyIsInJvbGUiOiJhZG1pbiJ9.xxx";
const payload = JSON.parse(atob(token.split(".")[1]));
// { sub: "user_123", role: "admin" }
// No secret needed. The payload is public.
```

**Practical implication:** Never put sensitive data in a JWT payload unless the JWT is encrypted (JWE — JSON Web Encryption, which is different from the standard signed JWT you use everywhere). Putting a password, SSN, or credit card number in a JWT payload exposes it to anyone who touches the token.

---

## 4. Signing — How the Signature Works

```
Server signs:
  data = base64url(header) + "." + base64url(payload)
  signature = HMAC-SHA256(data, secret)

Token = data + "." + base64url(signature)
```

The signature is computed over the header AND payload together. Changing either one byte of either part produces a completely different signature.

```
Original payload: { "role": "teacher" }
Signature over:   eyJhbGciOiJIUzI1NiJ9.eyJyb2xlIjoidGVhY2hlciJ9

Attacker modifies: { "role": "admin" }
New base64:        eyJyb2xlIjoiYWRtaW4ifQ
New signature needed: HMAC-SHA256("eyJhbGciOiJIUzI1NiJ9.eyJyb2xlIjoiYWRtaW4ifQ", secret)

Attacker doesn't know the secret → can't compute the valid signature → forgery fails.
Server verification: expected signature ≠ provided signature → token rejected.
```

**The signature doesn't hide the payload.** It proves the payload is authentic — that it was created by someone with the secret, and hasn't been changed.

---

## 5. Verification Flow

Every request that uses a JWT must go through this verification:

```
1. Extract token from request
   Header: Authorization: Bearer eyJhbGci...

2. Split into three parts: header.payload.signature

3. Decode header → get algorithm
   { "alg": "HS256" }

4. Recompute signature:
   expected = HMAC-SHA256(header + "." + payload, secret)

5. Compare signatures (constant-time):
   timingSafeEqual(expected, provided)
   If mismatch → reject (token forged or corrupted)

6. Decode payload → parse claims

7. Check exp (expiry):
   if (now > exp) → reject (token expired)

8. Check iat (issued at) — optional:
   if iat is suspiciously far in the past → could be stale

9. Check iss (issuer) — if used:
   if iss !== expected_issuer → reject

10. Check aud (audience) — if used:
    if aud doesn't include your service → reject

11. Token is valid → use the claims
```

Steps 4–10 must ALL be performed. Libraries like `jsonwebtoken` or `@nestjs/jwt` handle this when you call `verify()`.

---

## 6. Signing Algorithms — HS256 vs RS256 vs ES256

### HS256 — HMAC-SHA256 (Symmetric)

```
Same secret to sign and verify.
Server signs → Server verifies (using the same key)

Pros:  Fast, simple, one key to manage
Cons:  Every service that needs to verify tokens must share the secret
       If the secret leaks → attackers can forge tokens

When to use:
  Single server or tightly-coupled services that all share the secret
  Internal microservices where you control all verifiers
```

### RS256 — RSA-SHA256 (Asymmetric)

```
Private key to sign → Public key to verify (different keys)

Server signs with private key (secret).
Any service can verify using the public key (shareable).

Pros:  Public key can be published (JWKS endpoint) without compromising security
       Multiple services can verify without needing the signing key
       Non-repudiation: only the private key holder could have signed it
Cons:  Slower than HS256. Larger tokens. More key management.

When to use:
  Multiple independent services need to verify tokens
  Public-facing authentication (OIDC providers publish JWKS)
  When you want to publish your public keys for external verification
```

### ES256 — ECDSA with P-256 (Asymmetric, ECC)

```
Same concept as RS256 but using elliptic curve cryptography.
Smaller keys and signatures than RSA for same security level.

Pros:  Smaller than RS256, same security, modern
Cons:  Less universally supported than RS256 (though support is good now)

When to use:
  Prefer over RS256 for new systems
  When token size matters (mobile, bandwidth-constrained)
```

### The `alg: "none"` Vulnerability

Some early JWT libraries accepted `alg: "none"` in the header, which meant "no signature — trust the payload." Attackers could forge tokens by:

```json
// Header
{ "alg": "none", "typ": "JWT" }

// Payload
{ "role": "admin" }

// Signature: (empty)
```

**Always explicitly specify the expected algorithm in `verify()`:**

```ts
// WRONG — accepts whatever algorithm the token claims
jwt.verify(token, secret);

// CORRECT — only accepts HS256
jwt.verify(token, secret, { algorithms: ["HS256"] });
```

Modern libraries have fixed this by default, but always be explicit.

---

## 7. Standard Claims

JWT defines reserved claim names (RFC 7519):

```
iss (issuer)          → who created the token
                        e.g., "https://api.yourapp.com"
                        used to reject tokens from other issuers

sub (subject)         → who the token is about (usually user ID)
                        e.g., "user_cuid123"
                        the stable identifier for the user

aud (audience)        → who the token is intended for
                        e.g., "api.yourapp.com" or ["api", "billing"]
                        prevents a token for Service A from being used on Service B

exp (expiration time) → Unix timestamp after which the token is invalid
                        e.g., 1741694800
                        ALWAYS set this — a JWT without expiry is valid forever

iat (issued at)       → Unix timestamp when the token was issued
                        useful for detecting old tokens, audit logging

nbf (not before)      → Unix timestamp before which the token is not valid
                        rarely needed — use for future-dated tokens

jti (JWT ID)          → unique identifier for this token
                        used for one-time tokens or token revocation tracking
```

---

## 8. What to Put in the Payload (and What Not To)

### Include (Low-sensitivity, needed on every request)

```json
{
  "sub": "user_abc123",        ← stable user ID (not email — emails change)
  "role": "teacher",           ← needed for every authorization check
  "schoolId": "school_xyz",    ← needed for every tenant isolation check
  "iat": 1741691200,
  "exp": 1741694800
}
```

### Don't Include

```
Passwords (obviously)          → attacker reads from token
Password hashes                → never — hashes can be used for offline cracking
SSNs / national IDs            → too sensitive to be in a token
Health data                    → sensitive, not needed per-request
Full email for display         → not needed in every request (fetch from DB when needed)
Mutable data (balance, count)  → becomes stale — token doesn't update mid-session
Large objects                  → JWT is sent on every request — keep it small
```

### The Stale Data Problem

Anything you put in the JWT is a snapshot at token issuance time. If the user's role changes after token issuance:

```
Token issued at 10:00: { role: "teacher" }
Admin demotes user at 10:05.
User makes request at 10:30: token says role: "teacher" → still has teacher access!

The token doesn't know the role changed.
The token is valid (signature checks out, not expired).
```

Solutions:

1. **Short access token lifetime** (15 minutes) — role change takes effect within 15 minutes
2. **Token blacklisting** — add the specific token's `jti` to a revocation list when role changes
3. **Fetch role from DB on each request** — defeats the point of stateless JWT for role checks
4. **Keep mutable data out of token** — only put the user ID, fetch current role from cache

---

## 9. JWT Storage — The localStorage vs httpOnly Cookie Debate

This is one of the most discussed topics in web auth.

### Option A: localStorage (or sessionStorage, or in-memory)

```
Store: localStorage.setItem("accessToken", token);
Send:  headers: { Authorization: "Bearer " + localStorage.getItem("accessToken") }
```

**XSS risk:** Any JavaScript running on your page can read localStorage. A successful XSS attack steals the token.

**CSRF-safe:** localStorage is not automatically attached to requests. Attacker can't make your browser send the token cross-site (Bearer headers require JavaScript to set).

### Option B: httpOnly Cookie

```
Server sets: Set-Cookie: accessToken=...; HttpOnly; Secure; SameSite=Strict
Browser automatically attaches cookie to all requests to the domain.
JavaScript cannot read the cookie.
```

**XSS-resistant:** httpOnly cookies are inaccessible to JavaScript. XSS can't steal the token.

**CSRF risk:** Cookies are automatically attached to cross-site requests. Mitigated by `SameSite=Strict` + CSRF tokens.

### The Real Answer

```
In-memory (JavaScript variable, not localStorage):
  → Cleared on page refresh (requires re-login or silent refresh)
  → XSS can still steal it via window hooks, but harder than localStorage
  → CSRF-safe (must set Authorization header explicitly)

Best practice for access tokens:
  → Store in memory (not localStorage, not sessionStorage)
  → Refresh tokens in httpOnly cookie (longer-lived, needs XSS protection)

Why this combination:
  Access token in memory:
    → Short-lived (15 min) — loss not catastrophic
    → Never touches localStorage — harder for XSS to steal
    → Sent as Bearer header — immune to CSRF

  Refresh token in httpOnly cookie:
    → Longer-lived — needs better protection than memory (survives tab close)
    → httpOnly — XSS can't steal it
    → SameSite=Strict — CSRF can't use it
    → Rotation on each use — if stolen, detected quickly
```

---

## 10. Access Token + Refresh Token Pattern

```
Access token:
  Contains: user claims (sub, role, schoolId)
  Expiry: short (15 minutes)
  Storage: JavaScript memory
  Sent on: every API request (Authorization: Bearer header)

Refresh token:
  Contains: only a reference to the session (jti or opaque ID)
  Expiry: long (7–30 days)
  Storage: httpOnly cookie
  Sent on: only to /auth/refresh endpoint
```

```
User logs in:
  Server issues both tokens.
  Access token → sent in response body → stored in memory by client.
  Refresh token → Set-Cookie: refreshToken=...; HttpOnly; Secure; SameSite=Strict; Path=/auth/refresh

15 minutes later, access token expires:
  Client calls POST /auth/refresh
  Refresh token cookie is automatically sent (httpOnly cookie)
  Server validates refresh token → issues new access token
  Client stores new access token in memory

User closes browser:
  Memory cleared → access token gone
  But refresh token cookie persists (if not sessionStorage-backed)
  Next visit → silent refresh → new access token → user is "still logged in"

User clicks logout:
  Client clears memory (access token gone)
  Client calls POST /auth/logout
  Server invalidates refresh token in database
  Server clears the cookie: Set-Cookie: refreshToken=; Max-Age=0
```

---

## 11. Token Revocation Problem

Stateless JWTs cannot be revoked before expiry. If you issue a 15-minute access token, you cannot "cancel" it — it's valid until the expiry time.

```
Scenario: admin immediately revokes a teacher's access due to misconduct.
Access token: still valid for up to 15 minutes.
Teacher can still make API calls during that window.

Solutions:

1. Accept the window (most practical)
   With 15-minute access tokens: worst case is 15 minutes of residual access.
   Usually acceptable in non-critical systems.

2. Token blacklisting
   Maintain a Redis set of revoked token JTIs.
   On every request: check if jti is in the blacklist.
   Adds a Redis lookup to every request — but Redis is fast (<1ms).
   Blacklist entries expire at the token's original exp time.

3. Version field in token
   Add tokenVersion: 5 to the JWT.
   Store current version in user record.
   On request: if token.tokenVersion < user.tokenVersion → reject.
   Admin increments user's tokenVersion → all existing tokens invalid.
   Requires a DB lookup per request — effectively stateful.

4. Very short access token lifetime (1–5 minutes)
   Limits the window to acceptable range.
   More refresh traffic but still practical.
```

For most applications: short-lived tokens + accept the window. Add blacklisting if you need immediate revocation for high-security scenarios (admin role removal, account suspension).

---

## 12. Common JWT Vulnerabilities

### `alg: "none"` (Covered Above)

Always specify `algorithms: ["HS256"]` in verify options.

### Algorithm Confusion (RS256 → HS256 Switch)

```
Server uses RS256. Publishes public key.
Attacker knows public key (it's public).

Attack: attacker creates a token, signs it with HS256 using the PUBLIC key as the HS256 secret.
If server verifies with just: jwt.verify(token, publicKey) without specifying algorithm:
  → Server might use the algorithm from the token header
  → Token claims alg: HS256
  → Server uses HMAC-SHA256(payload, publicKey)
  → Attacker also did HMAC-SHA256(payload, publicKey) — signatures match!
  → Forged token accepted

Defense: always specify expected algorithms in verify():
  jwt.verify(token, publicKey, { algorithms: ["RS256"] });
```

### Weak Secret

```
HS256 signature:
  HMAC-SHA256(data, secret)

If secret = "secret" or "password" or "jwt_secret":
  Attackers can brute-force the secret using JWT cracking tools
  Once the secret is known → forge any token

Secret requirements:
  → At minimum 256 bits of entropy (32 random bytes)
  → Generated with CSPRNG (crypto.randomBytes)
  → Stored in environment variable, not in code
  → Different secrets for access tokens and refresh tokens
```

```ts
// Generate a proper secret
import crypto from "crypto";
console.log(crypto.randomBytes(32).toString("hex")); // 64 hex chars = 256 bits
// Use this output as your JWT_SECRET env variable
```

### Token in URLs / Logs

```
WRONG — token in URL
GET /api/download?token=eyJhbGci...

The token:
  → Appears in browser history
  → Appears in server access logs
  → Sent in Referer header to external resources
  → Cached by proxies

CORRECT — token in Authorization header
Authorization: Bearer eyJhbGci...

Headers are not logged by default in most systems.
Never put tokens in URLs except for single-use short-lived download links.
```

---

## 13. Implementing JWT in NestJS

```ts
// auth.module.ts
@Module({
  imports: [
    JwtModule.registerAsync({
      imports: [ConfigModule],
      inject: [ConfigService],
      useFactory: (config: ConfigService) => ({
        secret: config.getOrThrow("JWT_ACCESS_SECRET"),
        signOptions: {
          expiresIn: "15m",
          issuer: "api.yourapp.com",
          audience: "api.yourapp.com",
        },
      }),
    }),
  ],
})

// auth.service.ts
@Injectable()
export class AuthService {
  constructor(
    private jwtService: JwtService,
    private config: ConfigService,
  ) {}

  issueAccessToken(user: User): string {
    const payload: JwtPayload = {
      sub: user.id,
      role: user.role,
      schoolId: user.schoolId,
    };
    return this.jwtService.sign(payload);
    // Uses the secret + options from JwtModule registration
  }

  verifyAccessToken(token: string): JwtPayload {
    return this.jwtService.verify<JwtPayload>(token, {
      algorithms: ["HS256"],  // explicitly specify — never omit
      issuer: "api.yourapp.com",
      audience: "api.yourapp.com",
    });
  }
}

// jwt.strategy.ts
@Injectable()
export class JwtStrategy extends PassportStrategy(Strategy) {
  constructor(config: ConfigService) {
    super({
      jwtFromRequest: ExtractJwt.fromAuthHeaderAsBearerToken(),
      ignoreExpiration: false,
      secretOrKey: config.getOrThrow("JWT_ACCESS_SECRET"),
      algorithms: ["HS256"],
      issuer: "api.yourapp.com",
      audience: "api.yourapp.com",
    });
  }

  async validate(payload: JwtPayload): Promise<JwtPayload> {
    // payload is already verified (signature + exp + iss + aud)
    // Additional checks go here (e.g., check blacklist)
    return payload;
  }
}
```

---

## Summary

```
JWT structure: header.payload.signature (all base64url-encoded)
  Payload is READABLE by anyone with the token — not encrypted
  Signature proves authenticity and integrity — does not hide data

Signing:
  HS256: symmetric — same secret for sign and verify
  RS256/ES256: asymmetric — private key signs, public key verifies
  Always specify algorithms in verify() — prevent alg:none and confusion attacks

Standard claims:
  sub (user ID), role, schoolId: in payload
  exp: ALWAYS set — no expiry = valid forever
  iss + aud: set if multiple services or issuers

What not to put in JWT:
  Sensitive data (readable by anyone with the token)
  Mutable data (goes stale — role changes not reflected until new token)

Storage:
  Access token: JavaScript memory (not localStorage)
  Refresh token: httpOnly cookie with SameSite=Strict

Revocation:
  Stateless JWTs can't be revoked before expiry
  Short lifetime (15 min) limits the damage window
  Redis blacklist for immediate revocation if needed

Secret:
  32+ random bytes from CSPRNG
  Never committed to code
  Different secrets for access vs refresh tokens
```