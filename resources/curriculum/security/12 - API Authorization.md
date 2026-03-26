
> Beyond user-facing auth, APIs need their own authorization layer — API keys for machine-to-machine access, scopes to limit what each key can do, and rate limiting as an authorization mechanism. This covers the full picture of API-level security.

---

## Table of Contents

1. [API Keys](#1-api-keys)
2. [API Key Security](#2-api-key-security)
3. [Scopes and Permissions for API Keys](#3-scopes-and-permissions-for-api-keys)
4. [Rate Limiting as Authorization](#4-rate-limiting-as-authorization)
5. [Rate Limiting Strategies](#5-rate-limiting-strategies)
6. [API Versioning Security](#6-api-versioning-security)
7. [Webhook Security](#7-webhook-security)

---

## 1. API Keys

API keys are long-lived credentials for machine-to-machine (M2M) communication — scripts, CI/CD pipelines, integrations, background services.

```
vs User tokens (JWT):
  User tokens: authenticated by a human, short-lived, represent a user
  API keys:    authenticated by a service, long-lived, represent an application

When to use API keys:
  → Server-to-server communication
  → CLI tools authenticating to your API
  → Third-party integrations
  → Webhooks from external services
  → Mobile app initial authentication (before user login)
```

### API Key Format

```
Format: <prefix>_<random_bytes>

Example: sk_live_a3b4c5d6e7f8...  (Stripe-style)
         pk_test_xyz123...
         
Benefits of prefix:
  → Immediately identifiable as an API key (helps in secret scanning)
  → Distinguishes test vs live, read vs write
  → GitHub's secret scanning looks for patterns like these to warn about leaked keys
```

### Generating API Keys

```ts
import crypto from "crypto";

function generateApiKey(prefix: string = "key"): { raw: string; hash: string } {
  const rawKey = `${prefix}_${crypto.randomBytes(32).toString("hex")}`;
  // sha256 hash for storage (like refresh tokens — never store the raw key)
  const hash = crypto.createHash("sha256").update(rawKey).digest("hex");
  return { raw: rawKey, hash };
}

// Show the raw key ONCE to the user on creation
// Store only the hash
// User must copy it immediately — it can never be shown again
```

---

## 2. API Key Security

### Never Store the Raw Key

```ts
// WRONG
await db.apiKey.create({ data: { key: rawKey, userId } });

// CORRECT — store only hash (like password reset tokens)
await db.apiKey.create({ data: { keyHash: hash, keyPrefix: rawKey.slice(0, 12), userId } });
// Store a prefix (e.g., "sk_live_a3b4") so users can identify which key it is
// Without being able to recover the full key
```

### Lookup on Each Request

```ts
// API key authentication middleware
@Injectable()
export class ApiKeyGuard implements CanActivate {
  async canActivate(context: ExecutionContext): Promise<boolean> {
    const request = context.switchToHttp().getRequest();
    const apiKey = request.headers["x-api-key"];
    
    if (!apiKey) return false;
    
    const hash = crypto.createHash("sha256").update(apiKey).digest("hex");
    
    const key = await this.db.apiKey.findUnique({
      where: { keyHash: hash },
      include: { user: true },
    });
    
    if (!key || key.revokedAt) return false;
    if (key.expiresAt && key.expiresAt < new Date()) return false;
    
    // Log usage
    await this.db.apiKey.update({
      where: { id: key.id },
      data: { lastUsedAt: new Date(), usageCount: { increment: 1 } },
    });
    
    request.apiKey = key;
    request.user = key.user;
    return true;
  }
}
```

### API Key Rotation

API keys should be rotatable without downtime:

```
1. User creates new API key
2. Both old and new key work simultaneously (grace period)
3. User updates their systems to use new key
4. User revokes old key

Never force immediate rotation — it causes downtime.
```

---

## 3. Scopes and Permissions for API Keys

Different API keys should have different access levels — the principle of least privilege for machine clients.

```ts
enum ApiKeyScope {
  READ_USERS = "users:read",
  WRITE_USERS = "users:write",
  READ_LIBRARY = "library:read",
  WRITE_LIBRARY = "library:write",
  ADMIN = "admin",            // full access
}

// API key in DB
model ApiKey {
  id          String   @id @default(cuid())
  keyHash     String   @unique
  keyPrefix   String   // first 12 chars for display
  userId      String
  scopes      String[] // e.g., ["users:read", "library:read"]
  name        String   // "CI Pipeline", "Mobile App v2"
  expiresAt   DateTime?
  revokedAt   DateTime?
  lastUsedAt  DateTime?
  usageCount  Int      @default(0)
  createdAt   DateTime @default(now())
}

// Scope check in guard
function hasScope(apiKey: ApiKey, required: ApiKeyScope): boolean {
  return apiKey.scopes.includes(required) || apiKey.scopes.includes(ApiKeyScope.ADMIN);
}
```

---

## 4. Rate Limiting as Authorization

Rate limiting is usually thought of as DDoS protection, but it's also an authorization mechanism — it controls **how much** a client can access, not just **whether** they can access.

```
Different authorization levels → different rate limits:
  Unauthenticated:          100 requests/hour
  Authenticated (user):     1,000 requests/hour
  API key (standard):       10,000 requests/hour
  API key (premium):        100,000 requests/hour
  Internal service:         unlimited (trusted network only)
```

Rate limits by scope:

```
Rate limits can be per-endpoint:
  GET /api/students    → 100/min (read, cheaper)
  POST /api/students   → 10/min (write, more expensive)
  
  POST /auth/login     → 10/hour/IP (sensitive — anti-brute-force)
  GET /api/health      → unlimited (doesn't touch DB)
```

---

## 5. Rate Limiting Strategies

### Fixed Window

```
100 requests per minute
Window resets at :00 every minute

Problem: burst attacks at window boundaries
  User sends 100 requests at 0:59 → resets at 1:00 → sends 100 more
  200 requests in 2 seconds — defeats the purpose
```

### Sliding Window

```
At any given moment, count requests in the last 60 seconds.
More accurate than fixed window.
More memory-intensive (must store timestamps of each request).
```

### Token Bucket

```
Bucket has capacity C (e.g., 100 tokens)
Tokens refill at rate R (e.g., 10/second)
Each request consumes 1 token
If bucket empty → reject

Allows bursting up to capacity C
Steady-state limit is R/second
Most flexible — used by most production systems
```

### Leaky Bucket

```
Requests enter a queue
Queue drains at a fixed rate
Queue full → new requests rejected
Smooths out bursty traffic — output is always at a steady rate
Good for protecting downstream services
```

### Implementation with NestJS Throttler

```ts
// app.module.ts
@Module({
  imports: [
    ThrottlerModule.forRootAsync({
      imports: [ConfigModule],
      inject: [ConfigService],
      useFactory: (config: ConfigService) => ({
        throttlers: [
          { name: "short", ttl: seconds(10), limit: 30 },  // burst
          { name: "long", ttl: minutes(1), limit: 100 },   // sustained
        ],
      }),
    }),
  ],
})

// Custom throttler for sensitive endpoints
@Post("login")
@Throttle({ default: { ttl: hours(1), limit: 10 } })
async login() { ... }

// Rate limiting by API key (not just IP)
@Injectable()
export class ApiKeyThrottlerGuard extends ThrottlerGuard {
  protected async getTracker(req: Record<string, any>): Promise<string> {
    // Use API key ID as the rate limit key (not IP address)
    return req.apiKey?.id ?? req.ip;
  }
}
```

---

## 6. API Versioning Security

Old API versions accumulate security debt. Versioning strategy affects how you patch vulnerabilities.

```
URL versioning: /api/v1/users, /api/v2/users
Header versioning: API-Version: 2
Query versioning: /api/users?version=2

Security considerations:
  → Old versions must be deprecated and removed
  → Vulnerabilities in v1 must be patched even if v2 is available (some clients won't migrate)
  → Breaking security changes are justified if the old behavior is insecure
  
Deprecation timeline:
  1. Announce deprecation (6-12 months notice)
  2. Log warnings on deprecated version usage (identify clients that haven't migrated)
  3. Set hard deadline
  4. Remove old version
```

---

## 7. Webhook Security

Webhooks are HTTP callbacks — a service calls YOUR endpoint when an event occurs. Securing them is the reverse of normal API auth: you're the server, verifying the caller.

```
Stripe sends:
  POST https://yourapp.com/webhooks/stripe
  Headers:
    Stripe-Signature: t=1741691200,v1=abc123...
  Body: { event data }

Without verification: anyone can POST to your webhook endpoint claiming to be Stripe
```

### HMAC Signature Verification

```ts
// Stripe-style webhook verification
function verifyWebhookSignature(
  rawBody: Buffer,     // MUST be raw bytes, not parsed JSON
  signature: string,
  secret: string,
  toleranceSeconds = 300  // 5 minute clock skew tolerance
): boolean {
  const [timestampPart, signaturePart] = signature.split(",");
  const timestamp = parseInt(timestampPart.replace("t=", ""));

  // Check timestamp — prevent replay attacks
  const now = Math.floor(Date.now() / 1000);
  if (Math.abs(now - timestamp) > toleranceSeconds) {
    return false;  // Too old or in the future
  }

  // Compute expected signature
  const payload = `${timestamp}.${rawBody.toString()}`;
  const expected = crypto
    .createHmac("sha256", secret)
    .update(payload)
    .digest("hex");

  const provided = signaturePart.replace("v1=", "");

  return crypto.timingSafeEqual(
    Buffer.from(expected),
    Buffer.from(provided)
  );
}

// NestJS webhook controller
@Post("stripe")
async stripeWebhook(
  @Req() req: RawRequest,    // raw body required
  @Headers("stripe-signature") sig: string
) {
  const isValid = verifyWebhookSignature(req.rawBody, sig, process.env.STRIPE_WEBHOOK_SECRET);
  if (!isValid) throw new UnauthorizedException();
  
  // Process event
}
```

### Key Points

```
Always use raw body for signature verification
  → Parsing JSON changes whitespace/ordering → hash mismatch
  → Use express.raw() or NestJS raw body middleware for webhook routes

Replay protection via timestamp:
  → Include timestamp in the signed payload
  → Reject signatures older than 5 minutes
  → Without this: attacker can capture a valid webhook, replay it later

Use timingSafeEqual for comparison
  → Prevents timing attacks on the signature comparison
```

---

## Summary

```
API Keys:
  For M2M auth — server scripts, integrations
  Format: prefix_random64hex
  Store SHA-256 hash only — never the raw key
  Show raw key once on creation
  Support rotation (grace period overlap)
  Per-key scopes: principle of least privilege

Rate limiting:
  Also an authorization mechanism — controls how much access
  Token bucket algorithm: bursting + steady-state
  Different limits for: unauthenticated, user, API key tiers
  Sensitive endpoints get tighter limits (login: 10/hour/IP)

Webhooks:
  Verify HMAC signature using raw request body
  Include timestamp in signature payload (replay protection)
  Reject signatures older than 5 minutes
  Use timingSafeEqual for comparison
```