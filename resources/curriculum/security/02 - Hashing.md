> Hash functions are one of the most versatile tools in cryptography. They power password storage, data integrity verification, digital signatures, MACs, and more. This covers hash properties, the SHA family, HMAC, and key derivation functions — the specialized hashing used for passwords.

---

## Table of Contents

1. [What a Hash Function Is](#1-what-a-hash-function-is)
2. [Cryptographic Hash Properties](#2-cryptographic-hash-properties)
3. [The SHA Family](#3-the-sha-family)
4. [Hash Function Applications](#4-hash-function-applications)
5. [HMAC — Keyed Hashing](#5-hmac--keyed-hashing)
6. [Why Password Hashing Is Different](#6-why-password-hashing-is-different)
7. [Key Derivation Functions (KDFs)](#7-key-derivation-functions-kdfs)
8. [bcrypt In Depth](#8-bcrypt-in-depth)
9. [scrypt](#9-scrypt)
10. [Argon2 — The Modern Standard](#10-argon2--the-modern-standard)
11. [Comparing KDFs](#11-comparing-kdfs)
12. [Salting — Why and How](#12-salting--why-and-how)

---

## 1. What a Hash Function Is

A hash function takes input of any size and produces a fixed-size output (the hash, digest, or checksum). It is deterministic — same input always produces the same output.

```
SHA-256("hello")   = 2cf24dba5fb0a3...  (64 hex chars = 256 bits)
SHA-256("hello!")  = ce06092fb4d6...     (completely different — "avalanche effect")
SHA-256("")        = e3b0c44298fc1...    (empty string has a hash too)

Input can be 1 byte or 1 terabyte.
Output is always 256 bits (for SHA-256).
```

A cryptographic hash function has additional mathematical properties that make it useful for security.

---

## 2. Cryptographic Hash Properties

### Pre-image Resistance (One-way)

Given a hash `h`, it is computationally infeasible to find any input `m` such that `hash(m) = h`.

```
You see: 2cf24dba5fb0a3...
You cannot reverse it to find "hello" (except by brute force)
```

This is why hashed passwords are safe to store — you can't work backwards from the hash to the password.

### Second Pre-image Resistance

Given an input `m1`, it is computationally infeasible to find a different input `m2` such that `hash(m1) = hash(m2)`.

```
You have document D1 with hash H.
You cannot craft document D2 (with different content) that also has hash H.
```

This is what makes hash-based integrity verification reliable.

### Collision Resistance

It is computationally infeasible to find ANY two inputs `m1` and `m2` such that `hash(m1) = hash(m2)`.

This is stronger than second pre-image resistance — here the attacker can choose both inputs freely.

```
Birthday problem: If you hash N random messages, you'll find a collision after ~2^(n/2) attempts
  SHA-256: collision after ~2^128 attempts → still infeasible
  MD5:     collision after ~2^64 attempts  → feasible with modern hardware
            → MD5 is broken for collision resistance
```

### Avalanche Effect

Changing one bit of input completely changes the output.

```
SHA-256("hello")    = 2cf24dba5fb0a30e...
SHA-256("Hello")    = 185f8db32921bd46...   (capital H — completely different)
SHA-256("hellp")    = 6c62272...             (one character off — completely different)
```

This means you can't learn anything about the input by comparing hashes.

---

## 3. The SHA Family

### MD5 — Broken, Never Use for Security

128-bit output. Collision attacks are trivial. Two files can be crafted to have the same MD5 hash. Still used for non-security checksums (detecting accidental corruption), but never for security purposes.

### SHA-1 — Broken, Avoid

160-bit output. Collision attacks are practical (SHAttered attack, 2017, cost ~$100,000). Deprecated in TLS certificates, Git is migrating away from it. Don't use for security.

### SHA-2 Family — Current Standard

```
SHA-224  → 224-bit output (truncated SHA-256 with different initial values)
SHA-256  → 256-bit output — most widely used
SHA-384  → 384-bit output (truncated SHA-512)
SHA-512  → 512-bit output — larger output, faster on 64-bit hardware
SHA-512/256 → 256-bit output computed via SHA-512 internals (faster on 64-bit)
```

SHA-256 is the default choice for most applications. No known practical attacks.

### SHA-3 — The Backup Standard

Completely different construction from SHA-2 (Keccak sponge construction vs Merkle-Damgård). Standardized in 2015 as an alternative in case weaknesses are found in SHA-2.

SHA-3 is not widely deployed because SHA-2 remains secure. But it's a good alternative if you need defense in depth.

### BLAKE2 / BLAKE3

Faster than SHA-2 on software implementations, with similar security guarantees. BLAKE3 is especially fast (parallel by design). Not yet as widely standardized as SHA-2 but growing in adoption.

```
When to use which:
  SHA-256  → Default. Interoperability, standards compliance, JWTs
  SHA-512  → When you need larger output or are on 64-bit and want speed
  SHA-3    → When you explicitly want a SHA-2 alternative
  BLAKE3   → When raw performance matters and you control both ends
  MD5/SHA-1 → Never, except non-security checksums (and even then, prefer SHA-256)
```

---

## 4. Hash Function Applications

### Data Integrity Verification

```
File download:
  Publisher: SHA-256(ubuntu.iso) = a3b4c5...  (published alongside the file)
  Downloader: SHA-256(downloaded_file) = ?
  → Match: file arrived intact
  → Mismatch: corrupted or tampered

Git uses SHA-1 (migrating to SHA-256) for every commit, tree, and blob.
Change one character in a file → different hash → different commit ID → chain broken
```

### Storing Derived Values Safely

```
Password reset tokens:
  Generate: token = crypto.randomBytes(32).toString("hex")  // 64 chars
  Store:    tokenHash = SHA-256(token)
  Send:     token (in the email link)

  When user clicks link:
    Compute: SHA-256(provided_token)
    Compare: against stored tokenHash

  Database breach: attacker gets tokenHash
  → Cannot reverse SHA-256 to get token
  → Token is useless to attacker (it's 256 bits of randomness — not brute-forceable)
```

This is different from passwords — you hash the token before storing, not the token to derive a key.

### Deduplication

```
File storage system:
  hash = SHA-256(file_contents)
  if hash exists in storage → don't store again, just reference it

  Two users upload the same file → stored once, referenced twice
  Hash is the identity of the content (content-addressable storage)
```

### Fingerprinting / Content IDs

```
API request signing:
  canonical_request = METHOD + "\n" + PATH + "\n" + body
  signature = HMAC-SHA256(canonical_request, secret_key)

  Request ID in logs:
  request_id = SHA-256(ip + timestamp + random)

  Certificate fingerprint (what browsers show in cert details):
  fingerprint = SHA-256(raw_certificate_bytes)
```

---

## 5. HMAC — Keyed Hashing

A plain hash doesn't authenticate — anyone can compute `SHA-256(message)`. An **HMAC** (Hash-based Message Authentication Code) adds a secret key.

```
HMAC-SHA256(message, key) = SHA-256((key XOR opad) || SHA-256((key XOR ipad) || message))

Simplified: it's SHA-256 applied twice with the key mixed in each time
```

Properties:

- Same key → same HMAC (deterministic)
- Different key → completely different HMAC
- Without the key, cannot forge a valid HMAC
- Verifying an HMAC proves the sender knows the key AND the message hasn't been modified

### Where HMACs Are Used

```
JWT signatures (HS256):
  signature = HMAC-SHA256(header + "." + payload, JWT_SECRET)

API request signing (AWS Signature V4, Stripe webhooks):
  signature = HMAC-SHA256(canonical_request, API_SECRET)

Cookie integrity:
  value = data + "." + HMAC-SHA256(data, COOKIE_SECRET)

Refresh token storage:
  storedHash = SHA-256(rawToken)   ← this is just a hash, not HMAC
  (no key needed here — token itself has sufficient entropy)
```

### HMAC vs Digital Signatures

```
HMAC:
  → Symmetric — both parties need the same key
  → Both parties can GENERATE valid HMACs
  → No non-repudiation (Alice can't prove Bob sent it, since Alice could have made it too)
  → Fast

Digital signature (RSA/ECDSA):
  → Asymmetric — private key signs, public key verifies
  → Only the private key holder can generate valid signatures
  → Non-repudiation (only Alice's private key could have signed it)
  → Slower
```

Use HMAC when you control both ends (server signs and verifies its own JWTs). Use digital signatures when you need non-repudiation or multiple parties need to verify.

### HMAC in Node.js

```js
import crypto from "crypto";

const key = crypto.randomBytes(32); // 256-bit key
const message = "important message";

// Create HMAC
const hmac = crypto.createHmac("sha256", key);
hmac.update(message);
const mac = hmac.digest("hex");

// Verify HMAC — use timingSafeEqual to prevent timing attacks
function verifyHmac(message: string, mac: string, key: Buffer): boolean {
  const expected = crypto.createHmac("sha256", key)
    .update(message)
    .digest();

  const provided = Buffer.from(mac, "hex");

  if (expected.length !== provided.length) return false;

  return crypto.timingSafeEqual(expected, provided);
}
```

---

## 6. Why Password Hashing Is Different

Regular hash functions (SHA-256, BLAKE3) are designed to be **as fast as possible**. For integrity checks, file fingerprinting, and signatures, speed is a feature.

For passwords, speed is a vulnerability.

```
SHA-256 on modern GPU: ~20 billion hashes/second
bcrypt at cost 12:     ~10 hashes/second

Password "P@ssw0rd123":
  SHA-256: crack in milliseconds (in common password lists)
  bcrypt:  crack in days to months (depends on cost factor)
```

Password hashing needs to be:

1. **Deliberately slow** — makes brute force expensive
2. **Memory-hard** (for modern KDFs) — can't be parallelized cheaply on GPUs/ASICs
3. **Salted** — prevents rainbow tables and batch cracking

---

## 7. Key Derivation Functions (KDFs)

A KDF takes a password (low entropy, human-chosen) and derives a cryptographic key (high entropy, fixed size). For passwords, this involves deliberate slowness.

The four main password KDFs:

```
PBKDF2   → RFC 2898, FIPS-approved, widely supported, CPU-bound
bcrypt   → 1999, widely deployed, CPU-bound, 72-char limit
scrypt   → 2009, memory-hard, CPU-bound
Argon2   → 2015, memory-hard, parallelism-tunable, NIST recommended
```

---

## 8. bcrypt In Depth

bcrypt uses the Eksblowfish key schedule — a modified version of the Blowfish cipher's key setup that is intentionally expensive.

### The Output Format

```
$2b$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/LewFX8e3P1bFLp2eO
  │  │  │                     │
  │  │  └─ 22 chars: base64-encoded 128-bit salt
  │  └──── cost factor (12 = 2^12 = 4096 iterations)
  └─────── version ($2b = current standard version)
           (remaining chars = 31-char hash)
```

Everything needed to verify a password is in this single string.

### The Cost Factor

```
Cost  Iterations  Approx time (modern server)
  10    1,024      ~5ms
  11    2,048      ~10ms
  12    4,096      ~100ms   ← current recommendation
  13    8,192      ~200ms
  14   16,384      ~400ms

Every +1 doubles the time. Increase as hardware gets faster.
OWASP recommends: cost 12 minimum (2024)
```

### The 72-Character Limit

bcrypt silently truncates input at 72 bytes. A password longer than 72 bytes is equivalent to its first 72 bytes.

```
password: "aaaaaaaaa..." (100 chars)
bcrypt sees: "aaaaaaaaa..." (first 72 chars only)

Two users with passwords:
  "A" × 72 + "B" × 28   →  treated as "A" × 72
  "A" × 72 + "C" × 28   →  treated as "A" × 72  ← same hash! different passwords!
```

For most real passwords (under 72 chars), this is fine. For systems that allow very long passwords, pre-hash with SHA-256 before bcrypt:

```js
import bcrypt from "bcrypt";
import crypto from "crypto";

async function hashPassword(password: string): Promise<string> {
  // Pre-hash to handle passwords > 72 chars
  // Use base64 to avoid null bytes that could truncate even earlier
  const prehash = crypto.createHash("sha256").update(password).digest("base64");
  return bcrypt.hash(prehash, 12);
}

async function verifyPassword(password: string, hash: string): Promise<boolean> {
  const prehash = crypto.createHash("sha256").update(password).digest("base64");
  return bcrypt.compare(prehash, hash);
}
```

### bcrypt in Node.js

```js
import bcrypt from "bcrypt";

// Hash a password
const hash = await bcrypt.hash("userPassword", 12);
// Takes ~100ms at cost 12
// Returns: "$2b$12$..."

// Verify
const isValid = await bcrypt.compare("userPassword", hash); // true
const isWrong = await bcrypt.compare("wrongPassword", hash); // false
// compare() is constant-time — same duration whether right or wrong
// Takes ~100ms regardless of result

// Check if rehashing is needed (e.g., you raised the cost factor)
const needsRehash = bcrypt.getRounds(hash) < 12;
if (needsRehash) {
  const newHash = await bcrypt.hash(password, 12);
  // Update in database
}
```

---

## 9. scrypt

scrypt (2009) was designed to be both CPU-hard and **memory-hard** — filling large amounts of RAM during computation. This specifically targets GPU and ASIC attacks:

```
SHA-256 on GPU:  thousands of parallel threads, each computing a hash
bcrypt on GPU:   thousands of parallel threads (bcrypt is only CPU-hard)

scrypt on GPU:   each thread needs N × r × 128 bytes of RAM
                 GPU has limited RAM → can't run as many parallel threads
                 → GPU advantage dramatically reduced
```

### Parameters

```
N    → CPU/memory cost (work factor) — must be a power of 2
r    → block size (affects memory and CPU)
p    → parallelization factor
       Minimum memory: N × r × 128 bytes

OWASP recommended: N=32768 (2^15), r=8, p=1 → 32MB RAM per hash
High security:     N=65536 (2^16), r=8, p=1 → 64MB RAM per hash
```

```js
import crypto from "crypto";

// scrypt is built into Node.js crypto
const password = "userPassword";
const salt = crypto.randomBytes(32);

crypto.scrypt(
  password,
  salt,
  64,
  { N: 32768, r: 8, p: 1 },
  (err, derivedKey) => {
    // derivedKey is 64 bytes
    // Store: salt + derivedKey (must store salt separately — not built into output like bcrypt)
  },
);

// Promisified version
const { promisify } = require("util");
const scryptAsync = promisify(crypto.scrypt);

const key = await scryptAsync(password, salt, 64, { N: 32768, r: 8, p: 1 });
```

**bcrypt vs scrypt:** scrypt is harder to break with specialized hardware (GPUs/ASICs) but is less battle-tested than bcrypt and more complex to configure. For a web application, either is fine. bcrypt is more widely deployed and understood.

---

## 10. Argon2 — The Modern Standard

Argon2 won the Password Hashing Competition in 2015. It's the NIST-recommended password hashing function as of 2024.

Three variants:

```
Argon2d    → Memory-hard, fastest, best for resistance to GPU attacks
             Not recommended for password hashing (vulnerable to side-channel attacks)

Argon2i    → Memory-hard with data-independent memory access
             Resistant to side-channel attacks
             Recommended when timing attacks are a concern

Argon2id   → Hybrid of Argon2i and Argon2d
             OWASP recommended for password hashing
             Best of both worlds
```

### Parameters

```
memory   → RAM to use in KiB (higher = harder to parallelize)
time     → iterations (higher = slower, more CPU)
parallelism → threads to use

OWASP recommended (2024):
  Argon2id, memory=19456 (19MB), time=2, parallelism=1
  → ~100ms on modern hardware

  OR

  Argon2id, memory=12288 (12MB), time=3, parallelism=1
```

```js
import argon2 from "argon2";

// Hash
const hash = await argon2.hash("userPassword", {
  type: argon2.argon2id,
  memoryCost: 19456,   // 19 MB
  timeCost: 2,
  parallelism: 1,
});
// hash includes encoded parameters + salt (like bcrypt's self-contained format)
// "$argon2id$v=19$m=19456,t=2,p=1$..."

// Verify
const isValid = await argon2.verify(hash, "userPassword"); // true

// Check if rehash needed (parameters changed)
if (argon2.needsRehash(hash, { memoryCost: 19456, timeCost: 2 })) {
  const newHash = await argon2.hash(password, { ... });
}
```

---

## 11. Comparing KDFs

```
Feature              PBKDF2     bcrypt     scrypt     Argon2id
────────────────────────────────────────────────────────────────────
Memory-hard          ❌          ❌          ✅          ✅
GPU resistance       Poor        Moderate   Good        Best
FIPS approved        ✅          ❌          ❌          ❌ (in progress)
OWASP recommended    ✅          ✅          ✅          ✅ (preferred)
Node.js built-in     ✅          ❌(npm)     ✅          ❌(npm)
Self-contained hash  ❌          ✅          ❌          ✅
72-char limit        ❌          ✅          ❌          ❌
Age / battle-tested  Oldest      25yr        15yr       ~9yr

Best for:
  New projects       Argon2id or bcrypt
  FIPS compliance    PBKDF2
  No npm deps        bcrypt (widely available) or Node.js scrypt
  Max GPU resistance Argon2id
```

**Recommendation:**

- New projects: **Argon2id** (modern, best GPU resistance, OWASP preferred)
- Existing projects / team familiarity: **bcrypt** (still excellent, widely understood)
- FIPS requirement (government/compliance): **PBKDF2** (only FIPS-approved option)

---

## 12. Salting — Why and How

A **salt** is a random value added to the password before hashing. It is stored alongside the hash (not secret — purpose is uniqueness, not secrecy).

### What Salting Prevents

**Rainbow tables:** Pre-computed lookup tables of `hash → password`. Without salt, SHA-256("password") is always the same value. The attacker just looks it up.

With salt: `SHA-256("password" + "a7f3c2d9...")` is unique per user. No precomputed table can contain it — the attacker must compute it fresh.

**Batch cracking:** Without salt, if 1000 users have "password123", they all have the same hash. Crack it once, get 1000 accounts. With per-user salts, each hash is unique — crack each one individually.

### How Salting Works

```
Registration:
  salt = crypto.randomBytes(32)         // unique per user
  hash = bcrypt.hash(password + salt)   // or bcrypt handles it internally
  store: { hash, salt }                 // salt is NOT secret

Login:
  retrieve: { hash, salt } from database
  verify: bcrypt.compare(provided_password + salt, hash)
```

**bcrypt and Argon2 handle salting automatically** — you never manage the salt manually. They generate a random salt, incorporate it into the computation, and embed it in the output string.

For raw SHA-256 or scrypt, you manage the salt yourself:

```js
// scrypt — manual salt management
const salt = crypto.randomBytes(32);
const hash = await scryptAsync(password, salt, 64, { N: 32768, r: 8, p: 1 });

// Store both:
const stored = {
  salt: salt.toString("hex"),
  hash: hash.toString("hex"),
};

// Verify:
const saltBuf = Buffer.from(stored.salt, "hex");
const newHash = await scryptAsync(providedPassword, saltBuf, 64, {
  N: 32768,
  r: 8,
  p: 1,
});
const isValid = crypto.timingSafeEqual(
  newHash,
  Buffer.from(stored.hash, "hex"),
);
```

---

## Summary

```
Hash function properties:
  Pre-image resistance   → can't reverse hash to find input
  Second pre-image       → can't find different input with same hash
  Collision resistance   → can't find any two inputs with same hash
  Avalanche effect       → 1-bit change → completely different output

SHA family:
  MD5, SHA-1  → broken, never use for security
  SHA-256     → current standard, use by default
  SHA-512     → larger output, faster on 64-bit hardware
  SHA-3       → alternative construction, backup if SHA-2 falls

HMAC:
  HMAC-SHA256(message, key) → authenticated hash
  Only keyholder can produce valid HMAC
  Use timingSafeEqual to verify (prevent timing attacks)

Password hashing:
  Regular hashes are too fast — use KDFs
  KDFs are deliberately slow and optionally memory-hard

bcrypt:     cost 12, ~100ms, 72-char limit, self-contained output, 25yr battle-tested
scrypt:     memory-hard, more GPU-resistant than bcrypt, manual salt management
Argon2id:   best GPU resistance, OWASP preferred, self-contained, newest

Salting:
  Random per-user value added before hashing
  Stored with hash (not secret)
  bcrypt/Argon2 handle salting automatically
  Prevents rainbow tables and batch cracking
```

---

_Next: [04 — PKI and Certificates](./04-pki-and-certificates.md)_
