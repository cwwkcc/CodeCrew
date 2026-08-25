> Cryptography is the mathematical foundation every security mechanism is built on. Before you can understand encryption, hashing, or signatures, you need to understand what problems cryptography actually solves and what "secure" means mathematically.

---

## Table of Contents

1. [What Cryptography Actually Is](#1-what-cryptography-actually-is)
2. [The Core Problems Cryptography Solves](#2-the-core-problems-cryptography-solves)
3. [Entropy and Randomness](#3-entropy-and-randomness)
4. [Kerckhoffs's Principle](#4-kerckhoffss-principle)
5. [Security Through Obscurity — Why It Fails](#5-security-through-obscurity--why-it-fails)
6. [Computational Security — What "Unbreakable" Actually Means](#6-computational-security--what-unbreakable-actually-means)
7. [The Attacker Model](#7-the-attacker-model)
8. [Cryptographic Primitives Overview](#8-cryptographic-primitives-overview)
9. [Common Mistakes That Break Cryptography](#9-common-mistakes-that-break-cryptography)

---

## 1. What Cryptography Actually Is

Cryptography is the study of techniques for securing communication and data in the presence of adversaries. The word comes from Greek: _kryptos_ (hidden) + _graphia_ (writing).

Modern cryptography is not about hiding algorithms — it's about mathematical problems that are easy to do in one direction and practically impossible to reverse.

```
Easy direction:   multiply two 1024-bit prime numbers → instant
Hard direction:   factor the result back into primes → billions of years

This asymmetry is the foundation of most public-key cryptography.
```

The three branches relevant to application security:

```
Symmetric cryptography   → same key to encrypt and decrypt (AES)
Asymmetric cryptography  → different keys to encrypt and decrypt (RSA, ECC)
Cryptographic hashing    → one-way transformation, no decryption (SHA-256, bcrypt)
```

---

## 2. The Core Problems Cryptography Solves

Any secure communication system needs to solve four distinct problems. Confusing them leads to using the wrong tool.

### Confidentiality — Keeping Secrets

Only the intended recipient can read the message.

```
Alice sends Bob a message.
Eve is listening on the network.
Eve intercepts the message but cannot read it.

Tool: Encryption (symmetric or asymmetric)
```

### Integrity — Detecting Tampering

The recipient can verify the message was not modified in transit.

```
Alice sends Bob: "Transfer $100 to account 12345"
Eve intercepts, changes to: "Transfer $100 to account 99999"
Bob receives it — how does he know it was modified?

Without integrity protection: he can't.
With a MAC or digital signature: the modification is detected.

Tool: Hash functions, MACs, digital signatures
```

### Authentication — Verifying Identity

The recipient can verify who sent the message.

```
Bob receives a message claiming to be from Alice.
How does Bob know it's actually from Alice and not Eve impersonating her?

Tool: Digital signatures, MACs with shared secrets
```

### Non-repudiation — Preventing Denial

The sender cannot later deny having sent the message.

```
Alice sends a signed contract.
Later Alice claims she never sent it.
The digital signature proves she did — only she has her private key.

Tool: Digital signatures (asymmetric only — symmetric MACs don't provide this)
```

### The Table

```
Property          Encryption   Hash/MAC   Digital Signature
─────────────────────────────────────────────────────────────
Confidentiality   ✅           ❌          ❌
Integrity         ❌           ✅          ✅
Authentication    ❌           ✅*         ✅
Non-repudiation   ❌           ❌          ✅

* MACs authenticate but require a shared secret — both parties can generate them,
  so either party could have created the MAC. No non-repudiation.
```

---

## 3. Entropy and Randomness

**Entropy** in cryptography means unpredictability — how many bits of information an attacker needs to guess a value.

```
8-character password, lowercase only (26 chars):
  26^8 = 208 billion possible values
  → ~37.6 bits of entropy

8-character password, mixed case + numbers + symbols (95 chars):
  95^8 = 6.6 quadrillion possible values
  → ~52.6 bits of entropy

Randomly generated 256-bit key:
  2^256 possible values
  → 256 bits of entropy
  → ~115 quattuordecillion — effectively impossible to brute force
```

### Why `Math.random()` Is Catastrophically Wrong for Security

```js
// NEVER use this for security
const token = Math.random().toString(36).slice(2);

// Math.random() is a pseudorandom number generator (PRNG)
// It's seeded with a predictable value (often system time)
// Given enough outputs, the internal state can be reconstructed
// An attacker who observes a few tokens can predict all future ones
```

`Math.random()` is designed for games and simulations where speed matters and predictability doesn't. It has roughly 32–64 bits of internal state — an attacker can reconstruct it with modest effort.

### Cryptographically Secure Pseudorandom Number Generators (CSPRNG)

```js
// Node.js — correct
import crypto from "crypto";

// Random bytes — for tokens, keys, salts
const token = crypto.randomBytes(32).toString("hex"); // 256 bits of entropy
const salt = crypto.randomBytes(16); // 128 bits

// Random integer in range [0, max)
const randomInt = crypto.randomInt(0, 100);

// Web Crypto API (browser + Node.js 19+)
const array = new Uint8Array(32);
crypto.getRandomValues(array);
```

A CSPRNG draws from the operating system's entropy pool — hardware noise, timing events, I/O interrupts — sources of true randomness. Even if you observe every output, you cannot predict future values without access to the OS entropy pool.

### Entropy Requirements by Use Case

```
Use case                   Minimum entropy   What to use
───────────────────────────────────────────────────────────────
Session tokens             128 bits          crypto.randomBytes(16)
Password reset tokens      128 bits          crypto.randomBytes(16)
Refresh tokens             256 bits          crypto.randomBytes(32)
Encryption keys (AES-128)  128 bits          crypto.randomBytes(16)
Encryption keys (AES-256)  256 bits          crypto.randomBytes(32)
JWT secrets (HS256)        256 bits          crypto.randomBytes(32)
TOTP secrets               160 bits          crypto.randomBytes(20)
Password salts (bcrypt)    128 bits          generated internally by bcrypt
```

---

## 4. Kerckhoffs's Principle

Formulated by Auguste Kerckhoffs in 1883:

> **A cryptosystem should be secure even if everything about the system, except the key, is public knowledge.**

This seems counterintuitive. Wouldn't hiding the algorithm add security?

No. Here's why:

**The key can be changed. The algorithm can't.**

If your security depends on the algorithm being secret:

- A single leak of the algorithm = all security gone, permanently
- Every person who knows the algorithm is a liability
- You can't open-source your code, can't hire consultants, can't audit

If your security depends only on the key:

- Leak a key → rotate the key → security restored
- Algorithm can be public, peer-reviewed, and battle-tested by the entire world

**Real world implication:**

AES is fully public. Every detail of how it works is published. Thousands of cryptographers have tried to break it for 25 years. It's still secure. The key is what you protect.

A home-grown secret algorithm has been reviewed by zero experts. History is littered with "proprietary encryption" schemes that were broken trivially.

```
Good:  AES-256 with a secret 256-bit key
Bad:   Secret algorithm with a key
Worse: Secret algorithm, no key (the algorithm IS the key)
```

---

## 5. Security Through Obscurity — Why It Fails

Security through obscurity means relying on attackers not knowing how the system works.

```
Examples of security through obscurity:
  → "Our API endpoint is /xKj29Api — attackers won't find it"
  → "We use a proprietary cipher no one knows about"
  → "Our admin panel is at /backdoor123 — hidden"
  → "We obfuscate our JavaScript — no one can reverse engineer it"
```

**Why it always fails eventually:**

```
1. Endpoints get discovered via scanning, logs, referrer headers, sitemap leaks
2. Proprietary ciphers get reverse engineered from the binary
3. Hidden admin panels show up in Google, web archives, or breach disclosures
4. Obfuscated JS is just compressed JS — deobfuscation tools exist
```

Security through obscurity is not zero value — hiding information raises the cost of attack. But it is never a primary defense. It's a supplement, not a substitute.

```
Right approach:
  Primary defense:    Cryptography + correct implementation
  Supplement:         Don't advertise your tech stack in headers
  Also fine:          Don't put /admin in your sitemap

Wrong approach:
  Sole defense:       "Attackers won't find the endpoint"
```

---

## 6. Computational Security — What "Unbreakable" Actually Means

"Unbreakable" in cryptography doesn't mean theoretically impossible to break. It means computationally infeasible — breaking it would take longer than the universe has existed, or more energy than the sun produces.

### The Cost of Brute Force

```
AES-256 key space: 2^256 possible keys

If an attacker could try 1 trillion (10^12) keys per second:
  Time to exhaust: 2^256 / 10^12 seconds
                 = 10^65 seconds
                 = 10^57 years

The universe is 1.4 × 10^10 years old.
Breaking AES-256 by brute force is not possible, ever.
Not with all computers on Earth.
Not with all computers that could ever be built.
```

### Security Levels

Cryptographers measure security in bits:

```
Security level   Meaning                           Example
──────────────────────────────────────────────────────────────────
80 bits          Borderline — avoid for new systems
128 bits         Secure for the foreseeable future  AES-128, SHA-256 output truncated
192 bits         Very secure                        AES-192
256 bits         Maximum practical security         AES-256, SHA-256, Ed25519
```

128 bits means an attacker needs 2^128 operations to break it by brute force. On any hardware currently conceivable, this is impossible.

### Why Key Length Matters More Than Algorithm Name

```
RSA-512    → broken (factored in hours with modern hardware)
RSA-1024   → borderline, avoid
RSA-2048   → secure today
RSA-4096   → very secure, slower
RSA-512 "sounds" like a strong algorithm — the number is the key size in bits
Larger key = larger numbers to factor = harder to break
```

### Quantum Computing Consideration

Quantum computers threaten asymmetric cryptography (RSA, ECC) but not symmetric (AES) or hash functions (SHA-256). Grover's algorithm can search 2^256 possibilities in 2^128 operations — which halves the effective security of symmetric keys. AES-256 → effectively 128-bit security against quantum. Still fine.

RSA and ECC are vulnerable to Shor's algorithm — a sufficiently powerful quantum computer breaks them entirely. Post-quantum cryptography (NIST standardized in 2024: ML-KEM, ML-DSA) is the replacement. Not urgent for Paideon-scale applications today, but worth knowing.

---

## 7. The Attacker Model

Good security design starts by defining who you're defending against. Different attackers have different capabilities.

### Attacker Capabilities

```
Script kiddie
  → Uses existing tools (Metasploit, SQLMap, LOIC)
  → No custom exploit development
  → Opportunistic — hits easy targets

Motivated individual
  → Can write custom scripts
  → Understands vulnerability classes
  → Has patience — will spend days on a target

Organized criminal group
  → Has infrastructure (botnets, rented servers)
  → Buys 0-days on black markets
  → Financial motivation — ROI focused

Nation-state actor
  → Unlimited resources
  → Custom malware and 0-days
  → Long-term persistent access
  → Political/intelligence motivation
```

### The Threat Model for a School Management System

```
High probability, lower sophistication:
  → Students trying to view other students' grades
  → Students trying to change their own attendance records
  → Credential stuffing from leaked passwords
  → Opportunistic scanning (automated bots hitting everything)

Medium probability, medium sophistication:
  → A teacher accessing students' health/behavioral data out of curiosity
  → A disgruntled staff member trying to access admin functions
  → A parent trying to access more than their own children's data

Low probability, high impact:
  → Targeted attack on health/behavioral data (sensitive, regulated)
  → Admin credential compromise
  → Full database breach
```

Designing against nation-state attackers for a school system is over-engineering. Designing against script kiddies and curious insiders is correct. But "correct" doesn't mean "weak" — it means proportionate.

---

## 8. Cryptographic Primitives Overview

A **primitive** is a basic cryptographic building block. Complex protocols are built by combining primitives.

```
Block ciphers           AES
  → Encrypt fixed-size blocks of data
  → Foundation of most symmetric encryption

Stream ciphers          ChaCha20
  → Encrypt arbitrary-length data bit by bit
  → Faster than block ciphers on hardware without AES instructions

Hash functions          SHA-256, SHA-3, BLAKE2
  → One-way compression
  → Fixed output, any input size

MACs                    HMAC-SHA256, Poly1305
  → Hash + secret key = authenticated hash
  → Detects tampering, verifies sender knows the key

Key derivation (KDF)    PBKDF2, bcrypt, scrypt, Argon2
  → Derives cryptographic keys from passwords
  → Intentionally slow to resist brute force

Asymmetric encryption   RSA-OAEP, ECIES
  → Encrypt with public key, decrypt with private key

Digital signatures      RSA-PSS, ECDSA, Ed25519
  → Sign with private key, verify with public key

Key exchange            Diffie-Hellman, ECDH
  → Two parties derive a shared secret over a public channel
  → Neither party sends the secret — they compute it independently

Authenticated encryption  AES-GCM, ChaCha20-Poly1305
  → Combines confidentiality + integrity in one primitive
  → Encrypt + MAC in one operation
  → Always prefer over "encrypt then MAC separately"
```

These are covered in depth in the following files. This is just the map.

---

## 9. Common Mistakes That Break Cryptography

Cryptographic algorithms are hard to break. Implementations of them are routinely broken. The algorithm is almost never the weak point.

### Using the Right Algorithm Wrong

```js
// WRONG — ECB mode leaks patterns
const cipher = crypto.createCipheriv("aes-256-ecb", key, null);

// Correct — GCM provides authenticated encryption
const cipher = crypto.createCipheriv("aes-256-gcm", key, iv);
```

ECB (Electronic Codebook) mode encrypts each block independently. Identical plaintext blocks produce identical ciphertext blocks. An attacker can detect patterns without knowing the key.

### Reusing IVs

```js
// WRONG — same IV every time
const iv = Buffer.alloc(16, 0); // all zeros, always
const cipher = crypto.createCipheriv("aes-256-cbc", key, iv);

// Correct — random IV per encryption
const iv = crypto.randomBytes(16);
const cipher = crypto.createCipheriv("aes-256-cbc", key, iv);
// Store IV alongside ciphertext — it's not secret, just must be unique
```

An initialization vector (IV) must be random and unique per encryption. Reusing an IV with the same key leaks information about the relationship between plaintexts.

### Rolling Your Own Crypto

```js
// WRONG — home-grown "encryption"
function myEncrypt(text, key) {
  return text
    .split("")
    .map((c, i) =>
      String.fromCharCode(c.charCodeAt(0) ^ key.charCodeAt(i % key.length)),
    )
    .join("");
}
// This is a basic XOR cipher. Broken trivially.
```

Use established libraries. In Node.js: the built-in `crypto` module, or `libsodium` (via `tweetnacl` or `libsodium-wrappers`). Never implement cryptographic algorithms yourself.

### Incorrect Comparison

```js
// WRONG — early exit comparison leaks timing information
if (providedToken === storedToken) { ... }

// Correct — constant-time comparison
import crypto from "crypto";
if (crypto.timingSafeEqual(
  Buffer.from(providedToken),
  Buffer.from(storedToken)
)) { ... }
```

### Weak Key Generation

```js
// WRONG
const key = "mysecretkey"; // human-chosen, low entropy

// Correct — cryptographically random
const key = crypto.randomBytes(32); // 256 bits of entropy
```

### Storing Keys with the Data

```
Database contains:
  id | ciphertext | encryption_key
  1  | xKj29...   | aes-key-here

Breaking the encryption = just reading the key column.
Keys must be stored separately from the data they protect.
In environment variables (for symmetric keys) or HSM (for production).
```

---

## Summary

```
Cryptography solves four problems:
  Confidentiality (encryption), Integrity (hashing/MAC),
  Authentication (signatures/MAC), Non-repudiation (signatures only)

Entropy = unpredictability
  Math.random() = predictable, never use for security
  crypto.randomBytes() = cryptographically secure, always use

Kerckhoffs's principle:
  Security must depend on the key, not algorithm secrecy
  Public, peer-reviewed algorithms > secret algorithms

Security through obscurity:
  Never a primary defense — supplement only
  Hidden endpoints are found, proprietary ciphers are reversed

Computational security:
  "Secure" = breaking takes longer than the universe exists
  128-bit security is the practical minimum

Attacker model:
  Define who you're defending against
  Proportionate defense — don't over-engineer, don't under-engineer

Common implementation mistakes:
  ECB mode, IV reuse, home-grown crypto, non-constant-time comparison,
  weak key generation, storing keys with data
```

---

_Next: [02 — Encryption](./02-encryption.md)_
