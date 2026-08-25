> OAuth 2.0 is the standard for delegated authorization — "sign in with Google", "allow this app to read your GitHub repos". OpenID Connect layers identity on top of OAuth 2.0. Even if your app doesn't use them, you need to understand them to reason about third-party integrations and potential vulnerabilities.

---

## Table of Contents

1. [The Problem OAuth Solves](#1-the-problem-oauth-solves)
2. [OAuth 2.0 Core Concepts](#2-oauth-20-core-concepts)
3. [Authorization Code Flow (PKCE)](#3-authorization-code-flow-pkce)
4. [Other Grant Types](#4-other-grant-types)
5. [Access Tokens and Refresh Tokens in OAuth](#5-access-tokens-and-refresh-tokens-in-oauth)
6. [Scopes](#6-scopes)
7. [OpenID Connect](#7-openid-connect)
8. [ID Tokens](#8-id-tokens)
9. [Common OAuth Vulnerabilities](#9-common-oauth-vulnerabilities)
10. [When to Use OAuth vs Sessions](#10-when-to-use-oauth-vs-sessions)

---

## 1. The Problem OAuth Solves

Before OAuth, "sign in with Google" would mean giving a third-party app your Google username and password. The app would log in as you — full access to everything, indefinitely, with no way to revoke it without changing your password.

```
Old way (dangerous):
  "Please give us your Gmail password so we can read your contacts"
  → App has your password → full access to your entire Google account
  → If app is compromised → attacker has your Google password

OAuth way:
  "Allow this app to read only your contacts" (specific scope)
  → You authenticate WITH Google on Google's site
  → Google gives the app a limited access token
  → App can read contacts — nothing else
  → You can revoke access at any time on Google's account settings
  → App never sees your password
```

---

## 2. OAuth 2.0 Core Concepts

```
Resource Owner:     The user — you
Client:             The third-party application
Authorization Server: The service that authenticates (Google, GitHub, etc.)
Resource Server:    The API being accessed (Google Contacts API, GitHub API)

In simple cases, Authorization Server and Resource Server are the same service.
```

```
OAuth 2.0 Tokens:
  Access Token:     Short-lived, used to call the Resource Server API
  Refresh Token:    Long-lived, used to get new access tokens
  Authorization Code: Short-lived, one-time, exchanged for access token
```

---

## 3. Authorization Code Flow (PKCE)

The most secure and most widely used flow. Use this for web apps and mobile apps.

```
1. User clicks "Sign in with Google"

2. App redirects to Google:
   GET https://accounts.google.com/oauth2/auth?
     response_type=code
     &client_id=YOUR_CLIENT_ID
     &redirect_uri=https://yourapp.com/callback
     &scope=openid email profile contacts.readonly
     &state=random_csrf_token
     &code_challenge=BASE64URL(SHA256(code_verifier))   ← PKCE
     &code_challenge_method=S256

3. User sees Google's login/consent screen
   User authenticates with Google
   User clicks "Allow"

4. Google redirects to:
   https://yourapp.com/callback?
     code=AUTHORIZATION_CODE
     &state=random_csrf_token

5. App verifies state matches (CSRF protection)
   App exchanges code for tokens:
   POST https://accounts.google.com/oauth2/token
     grant_type=authorization_code
     code=AUTHORIZATION_CODE
     redirect_uri=https://yourapp.com/callback
     client_id=YOUR_CLIENT_ID
     client_secret=YOUR_CLIENT_SECRET
     code_verifier=ORIGINAL_RANDOM_STRING    ← PKCE

6. Google responds:
   {
     access_token: "ya29.xxx",
     expires_in: 3600,
     refresh_token: "1//xxx",
     scope: "openid email profile contacts.readonly",
     token_type: "Bearer",
     id_token: "eyJ..."   ← OpenID Connect
   }

7. App uses access_token to call APIs
   App uses id_token to identify the user
```

### PKCE — Proof Key for Code Exchange

PKCE protects against authorization code interception attacks (especially in mobile apps where redirect URIs are less secure).

```
Before redirect:
  app generates: code_verifier = crypto.randomBytes(32).toString("base64url")
  app computes:  code_challenge = base64url(sha256(code_verifier))
  sends code_challenge in authorization request

When exchanging code for token:
  sends code_verifier
  server verifies: sha256(code_verifier) === stored code_challenge

  If someone intercepts the authorization code, they can't exchange it
  because they don't have the original code_verifier.
```

PKCE is now recommended for ALL OAuth flows, including server-side apps (not just mobile).

---

## 4. Other Grant Types

### Client Credentials — Machine to Machine

```
No user involved. Your server authenticates directly.

POST /oauth/token
  grant_type=client_credentials
  client_id=YOUR_CLIENT_ID
  client_secret=YOUR_CLIENT_SECRET
  scope=read:data

Use case: background jobs, microservices calling each other, cron tasks
```

### Implicit Flow — Deprecated

Returned tokens directly in URL fragment (no code exchange). Removed from OAuth 2.1 because:

- Tokens in URLs appear in browser history, logs, Referer headers
- No PKCE support

Never use implicit flow for new implementations.

### Resource Owner Password Credentials — Deprecated

```
User gives their username + password directly to the client app.
App sends them to the authorization server.

POST /oauth/token
  grant_type=password
  username=alice@example.com
  password=secret123

Defeats the entire purpose of OAuth (user still gives password to third party).
Only existed for legacy migration. Removed from OAuth 2.1.
```

---

## 5. Access Tokens and Refresh Tokens in OAuth

OAuth access tokens can be:

**Opaque tokens:** Random strings. Resource Server must call Authorization Server to validate them (token introspection). Stateful.

**JWT access tokens:** Self-contained. Resource Server validates signature locally. Stateless. Common in modern OAuth implementations.

OAuth refresh tokens in the context of a user-facing app work the same way as described in the auth notes — they're long-lived, stored securely, rotated on use, and revocable.

---

## 6. Scopes

Scopes limit what an access token can do.

```
Google OAuth scopes:
  openid              → verify identity (required for OpenID Connect)
  email               → read email address
  profile             → read name and profile picture
  https://www.googleapis.com/auth/contacts.readonly  → read contacts
  https://www.googleapis.com/auth/calendar           → full calendar access

Your app should request only the scopes it needs.
Users see what you're requesting — requesting too much → users decline.
Principle of least privilege applies to OAuth scopes too.
```

Designing scopes for your own OAuth server:

```
read:users       → read user data
write:users      → create/update users
admin:users      → delete users, change roles
read:library     → read library data
write:library    → manage books
```

---

## 7. OpenID Connect

**OpenID Connect (OIDC)** is an identity layer on top of OAuth 2.0. OAuth answers "is this user authorized?" — OIDC answers "who is this user?"

```
OAuth 2.0:
  → Gives you an access token to call APIs
  → Tells you what the user CAN DO
  → Does NOT tell you who the user is

OIDC adds:
  → id_token (a JWT) containing user identity claims
  → /userinfo endpoint to fetch additional user data
  → Standard claims: sub (user ID), email, name, picture, etc.
```

OIDC is what "Sign in with Google/GitHub/Apple" actually uses. OAuth 2.0 is the authorization layer; OIDC is the identity layer.

---

## 8. ID Tokens

An ID Token is a JWT issued by the authorization server that contains identity claims about the user.

```json
{
  "iss": "https://accounts.google.com", // issuer
  "sub": "1234567890", // Google's internal user ID (stable)
  "aud": "YOUR_CLIENT_ID", // your application
  "exp": 1741694800, // expiry
  "iat": 1741691200, // issued at
  "email": "alice@gmail.com",
  "email_verified": true,
  "name": "Alice Smith",
  "picture": "https://lh3.googleusercontent.com/...",
  "locale": "en",
  "nonce": "abc123" // must match what you sent
}
```

### Validating an ID Token

```ts
import { OAuth2Client } from "google-auth-library";

const client = new OAuth2Client(CLIENT_ID);

async function verifyIdToken(idToken: string) {
  const ticket = await client.verifyIdToken({
    idToken,
    audience: CLIENT_ID,
  });
  const payload = ticket.getPayload();

  // Must verify:
  // 1. Signature (done by verifyIdToken)
  // 2. iss: must be "https://accounts.google.com"
  // 3. aud: must match your client ID
  // 4. exp: must not be expired
  // 5. nonce: must match what you sent (prevents replay)

  return payload;
}
```

---

## 9. Common OAuth Vulnerabilities

### Open Redirect via redirect_uri

```
Malicious redirect_uri:
  https://accounts.google.com/oauth2/auth?
    ...&redirect_uri=https://attacker.com/steal

Google should validate that redirect_uri is pre-registered.
But some implementations allow partial matching:
  Registered: https://yourapp.com/callback
  Attacker uses: https://yourapp.com/callback/../../../attacker.com

Authorization code (or token) sent to attacker.
Defense: Exact match validation on redirect_uri registration.
```

### CSRF on Redirect

Without the `state` parameter, an attacker can initiate an OAuth flow and trick a user into completing it (linking the attacker's Google account to the victim's app account).

```
state = crypto.randomBytes(32).toString("hex")  // store in session
Verify: received state === stored state → before exchanging code

Always use and verify the state parameter.
```

### Token Leakage in Logs/Referrer

Access tokens in URLs (implicit flow) appear in:

- Browser history
- Server logs
- Referer header when navigating to another site

This is why implicit flow was deprecated. Authorization Code + PKCE keeps tokens out of URLs.

### Token Substitution

Accepting an access token meant for Service A as authentication for Service B.

```
JWTs have aud (audience) claim.
Always validate that aud matches your service.
A token issued for your dev environment should not work in production.
```

---

## 10. When to Use OAuth vs Sessions

```
Use sessions (your own auth):
  → You control all users (school system, company intranet)
  → You don't want external dependencies
  → User identity is managed by your system
  → School management system: users are school-defined → own auth

Use OAuth / OIDC (federated identity):
  → Users already have accounts with identity providers (Google, GitHub)
  → You want to avoid managing passwords
  → Public-facing consumer app where friction must be minimized
  → Enterprise SSO (SAML or OIDC)

Use both (hybrid):
  → Primary: own auth (email + password)
  → Secondary: "Sign in with Google" as an alternative
  → Link external OAuth identity to internal user account on first use
```

---

## Summary

```
OAuth 2.0:
  Delegated authorization — limited access without sharing passwords
  Authorization Code + PKCE = secure flow for all apps
  Client Credentials = machine-to-machine
  Implicit and ROPC = deprecated, don't use

Scopes: limit what a token can do — request only what you need

OIDC: identity layer on OAuth 2.0
  id_token = JWT with who the user is
  access_token = what the user can do

Security:
  Always use and verify state parameter (CSRF)
  Exact match on redirect_uri
  Validate aud in ID tokens
  Use PKCE for all public clients
  Keep tokens out of URLs (no implicit flow)
```
