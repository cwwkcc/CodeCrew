> Encryption transforms readable data into unreadable data. Only someone with the key can reverse it. This covers symmetric encryption (AES), asymmetric encryption (RSA, ECC), key exchange (Diffie-Hellman), and when to use each.

---

## Table of Contents

1. [Symmetric Encryption](#1-symmetric-encryption)
2. [AES — The Standard](#2-aes--the-standard)
3. [Block Cipher Modes](#3-block-cipher-modes)
4. [Authenticated Encryption (AES-GCM)](#4-authenticated-encryption-aes-gcm)
5. [Asymmetric Encryption](#5-asymmetric-encryption)
6. [RSA](#6-rsa)
7. [Elliptic Curve Cryptography (ECC)](#7-elliptic-curve-cryptography-ecc)
8. [Key Exchange — Diffie-Hellman](#8-key-exchange--diffie-hellman)
9. [Hybrid Encryption](#9-hybrid-encryption)
10. [Digital Signatures](#10-digital-signatures)
11. [When to Use What](#11-when-to-use-what)

---

## 1. Symmetric Encryption

**Same key to encrypt and decrypt.**

```
Alice and Bob share secret key K.

Alice: plaintext → [encrypt with K] → ciphertext → send to Bob
Bob:   ciphertext → [decrypt with K] → plaintext

Eve intercepts ciphertext → cannot read it without K
```

**Properties:**

- Fast — designed for bulk data encryption
- Key must be shared securely before communication can begin
- If the key is compromised, all past and future messages are exposed

**The key distribution problem:** How do Alice and Bob securely share the key in the first place? If they're communicating over an insecure channel, they can't send the key over that same channel. This is what asymmetric cryptography and key exchange protocols solve.

---

## 2. AES — The Standard

**Advanced Encryption Standard** is the symmetric cipher used everywhere. Adopted as the US federal standard in 2001 after a public competition. Still unbroken in 2026.

### Key Sizes

```
AES-128 → 128-bit key → 2^128 possible keys → secure
AES-192 → 192-bit key → 2^192 possible keys → very secure
AES-256 → 256-bit key → 2^256 possible keys → maximum practical security
```

AES-128 is secure for virtually all purposes. AES-256 is used when you want a safety margin against future advances or classified-data requirements.

### How AES Works (Conceptually)

AES is a **block cipher** — it encrypts fixed-size blocks of data (128 bits = 16 bytes at a time).

```
Input: 16-byte plaintext block
Key:   16/24/32 bytes (AES-128/192/256)

Process:
  1. Key expansion — derive round keys from the main key
  2. Initial round: XOR plaintext with first round key
  3. 10/12/14 main rounds (depends on key size), each doing:
     → SubBytes    — substitute each byte using an S-box lookup table
     → ShiftRows   — cyclically shift rows of the state
     → MixColumns  — linear transformation of each column
     → AddRoundKey — XOR with the round key
  4. Final round (no MixColumns)

Output: 16-byte ciphertext block
```

The math behind each step (Galois Field arithmetic) is out of scope. The important thing is: each round introduces confusion (non-linearity via SubBytes) and diffusion (spreading one bit's effect across many bits via ShiftRows/MixColumns). After 10+ rounds, any relationship between plaintext and ciphertext is completely obscured.

**You never implement AES yourself.** Use `crypto.createCipheriv()` in Node.js or a well-audited library.

---

## 3. Block Cipher Modes

AES encrypts one 16-byte block at a time. Real data is longer than 16 bytes. A **mode of operation** defines how to apply AES to arbitrary-length data.

### ECB — Electronic Codebook (Never Use)

```
Block 1 → AES(key) → Ciphertext Block 1
Block 2 → AES(key) → Ciphertext Block 2
Block 3 → AES(key) → Ciphertext Block 3
```

Each block encrypted independently. **Fatal flaw: identical plaintext blocks produce identical ciphertext blocks.**Patterns in the data survive encryption.

```
The classic example: encrypt a bitmap image with ECB.
The shape of the image is still clearly visible in the ciphertext
because the same pixel colors (same bytes) produce the same ciphertext.
```

Never use ECB. It's not actually encryption in any meaningful security sense.

### CBC — Cipher Block Chaining

```
Each plaintext block is XOR'd with the previous ciphertext block before encryption.
The first block is XOR'd with a random IV (initialization vector).

C0 = AES(P0 XOR IV, key)
C1 = AES(P1 XOR C0, key)
C2 = AES(P2 XOR C1, key)
```

Identical plaintext blocks now produce different ciphertext (because of chaining). Better than ECB.

**Problem:** CBC only provides confidentiality, not integrity. An attacker can flip bits in the ciphertext and predictably affect the decrypted plaintext (CBC bit-flipping attacks). Must be combined with a MAC.

**Also:** CBC decryption is sequential — can't parallelize. And padding oracle attacks exist against naive CBC implementations.

### CTR — Counter Mode

```
Generates a keystream by encrypting a counter value, then XOR's with plaintext.

keystream_0 = AES(IV || 0, key)
keystream_1 = AES(IV || 1, key)
...
ciphertext_n = plaintext_n XOR keystream_n
```

Turns AES into a stream cipher. Parallelizable. No padding needed. Same issue as CBC: no authentication built in.

### GCM — Galois/Counter Mode (Use This)

GCM combines CTR mode with a Galois field MAC (GHASH). It provides authenticated encryption — both confidentiality AND integrity in one operation.

This is covered in depth in the next section.

---

## 4. Authenticated Encryption (AES-GCM)

**Authenticated Encryption with Associated Data (AEAD)** is the modern standard. It provides:

```
Confidentiality  → attacker cannot read the plaintext
Integrity        → any modification of ciphertext is detected
Authenticity     → only someone with the key could have created it
```

AES-GCM is the most widely deployed AEAD cipher. Used in TLS 1.3, HTTPS, storage encryption everywhere.

### Components

```
Inputs:
  plaintext      → data to encrypt
  key            → 16/24/32 bytes (AES-128/192/256-GCM)
  IV/nonce       → 12 bytes, unique per encryption (NEVER reuse with same key)
  AAD            → optional "associated data" — authenticated but NOT encrypted
                   (e.g., headers, metadata you want to verify but not hide)

Outputs:
  ciphertext     → same length as plaintext
  auth tag       → 16 bytes — the authentication tag (MAC)
```

The auth tag proves the ciphertext was produced by someone with the key and has not been modified. Verification of the tag happens before decryption — if the tag is invalid, you never even attempt to decrypt.

### Why the IV Must Never Be Reused

If you encrypt two different messages with the same key AND same IV in GCM:

```
C1 = P1 XOR keystream
C2 = P2 XOR keystream   (same keystream — same key + IV)

C1 XOR C2 = P1 XOR P2   (keystreams cancel out!)
```

An attacker who sees both ciphertexts can XOR them to get P1 XOR P2. From there, with knowledge of common plaintext patterns, both plaintexts can often be recovered. IV reuse in GCM completely breaks confidentiality.

**Solution:** Use a random 12-byte IV per encryption. Prepend it to the ciphertext — it's not secret, just unique.

### Node.js Implementation

```js
import crypto from "crypto";

function encrypt(plaintext: string, key: Buffer): { ciphertext: string; iv: string; tag: string } {
  // Generate a unique 12-byte IV for this encryption
  const iv = crypto.randomBytes(12);

  const cipher = crypto.createCipheriv("aes-256-gcm", key, iv);

  // Optional: set additional authenticated data
  // cipher.setAAD(Buffer.from("some-context-string"));

  let ciphertext = cipher.update(plaintext, "utf8", "hex");
  ciphertext += cipher.final("hex");

  // Get the 16-byte authentication tag
  const tag = cipher.getAuthTag();

  return {
    ciphertext,
    iv: iv.toString("hex"),
    tag: tag.toString("hex"),
  };
}

function decrypt(ciphertext: string, key: Buffer, iv: string, tag: string): string {
  const decipher = crypto.createDecipheriv(
    "aes-256-gcm",
    key,
    Buffer.from(iv, "hex")
  );

  // Set the authentication tag BEFORE decrypting
  decipher.setAuthTag(Buffer.from(tag, "hex"));

  let plaintext = decipher.update(ciphertext, "hex", "utf8");
  plaintext += decipher.final("utf8");
  // If tag verification fails, final() throws — ciphertext was tampered

  return plaintext;
}
```

### What to Store

```
Store alongside the ciphertext:
  → IV (not secret — must be unique per encryption)
  → Auth tag (not secret — needed to verify on decryption)
  → Algorithm identifier (for future algorithm rotation)

Never store:
  → The key (store separately — env variable, secrets manager)
```

---

## 5. Asymmetric Encryption

**Different keys to encrypt and decrypt.**

```
Key pair: public key (share freely) + private key (never share)

Encrypt with public key  → only the private key can decrypt
Sign with private key    → anyone with the public key can verify
```

```
Bob publishes his public key. Anyone can have it.

Alice wants to send Bob a secret:
  → Alice encrypts with Bob's public key
  → Only Bob's private key can decrypt it
  → Eve has the public key but can't decrypt

Bob wants to sign a document:
  → Bob signs with his private key
  → Anyone with Bob's public key can verify the signature
  → Only Bob could have produced it (only he has the private key)
```

**Properties:**

- Solves the key distribution problem — public keys are, by definition, public
- Much slower than symmetric encryption — not suitable for bulk data
- Used to encrypt small things (keys, signatures) — not gigabytes of data

---

## 6. RSA

**Rivest-Shamir-Adleman** — the most widely known asymmetric algorithm. Security based on the difficulty of factoring large numbers.

### Conceptual Basis

```
Pick two large prime numbers: p and q
Compute n = p × q                         (easy)
Compute φ(n) = (p-1)(q-1)
Choose e: public exponent (usually 65537)
Compute d: private exponent such that e × d ≡ 1 (mod φ(n))

Public key:  (n, e)
Private key: (n, d)

Encrypt: c = m^e mod n
Decrypt: m = c^d mod n

Breaking RSA = factoring n back into p and q
For n = 2048-bit, this is computationally infeasible
```

### RSA Key Sizes

```
RSA-512    → Broken (factorable in hours)
RSA-1024   → Borderline — avoid
RSA-2048   → Secure today — minimum recommendation
RSA-4096   → Very secure — slower operations
```

### RSA-OAEP — Correct Padding

Raw RSA (textbook RSA) is deterministic and malleable — you must use padding.

```
OAEP (Optimal Asymmetric Encryption Padding) — correct for encryption
PSS  (Probabilistic Signature Scheme)        — correct for signatures
PKCS#1 v1.5                                  — legacy, has known vulnerabilities
```

```js
import crypto from "crypto";

// Generate RSA key pair
const { publicKey, privateKey } = crypto.generateKeyPairSync("rsa", {
  modulusLength: 2048,
  publicKeyEncoding: { type: "spki", format: "pem" },
  privateKeyEncoding: { type: "pkcs8", format: "pem" },
});

// Encrypt with public key (OAEP padding)
const ciphertext = crypto.publicEncrypt(
  { key: publicKey, padding: crypto.constants.RSA_PKCS1_OAEP_PADDING },
  Buffer.from("hello world")
);

// Decrypt with private key
const plaintext = crypto.privateDecrypt(
  { key: privateKey, padding: crypto.constants.RSA_PKCS1_OAEP_PADDING },
  ciphertext
);
```

**Limitation:** RSA can only encrypt data smaller than the key size (minus padding overhead). RSA-2048 can encrypt at most ~214 bytes. This is why RSA is used to encrypt keys, not data (hybrid encryption — see below).

---

## 7. Elliptic Curve Cryptography (ECC)

ECC provides the same security as RSA with much smaller key sizes. Security based on the elliptic curve discrete logarithm problem.

```
Same security level:
  RSA-2048  ≈  ECC-224
  RSA-3072  ≈  ECC-256
  RSA-7680  ≈  ECC-384

ECC-256 gives RSA-3072-equivalent security with:
  → Smaller keys (256 bits vs 3072 bits)
  → Faster operations
  → Less bandwidth
```

### Why ECC Is Preferred Today

```
TLS 1.3 default: ECDHE (Elliptic Curve Diffie-Hellman Ephemeral)
SSH modern keys: Ed25519 (an ECC variant)
JWT RS256 alternative: ES256 (ECDSA with P-256)

RSA is still widely supported and secure, but ECC is the modern choice
for new systems where performance matters.
```

### Common Curves

```
P-256 (secp256r1)    → NIST standard, widely supported, TLS default
P-384 (secp384r1)    → Higher security margin
X25519               → Key exchange, modern, fast, no patent concerns
Ed25519              → Signatures, modern, very fast
secp256k1            → Bitcoin's curve (not recommended for general use)
```

```js
// Generate Ed25519 key pair (modern — for signatures)
const { publicKey, privateKey } = crypto.generateKeyPairSync("ed25519");

// Generate X25519 key pair (for key exchange)
const { publicKey: dhPublic, privateKey: dhPrivate } =
  crypto.generateKeyPairSync("x25519");
```

---

## 8. Key Exchange — Diffie-Hellman

**The key distribution problem:** How do two parties establish a shared secret over a public channel without ever sending the secret?

Diffie-Hellman solves this with a mathematical trick:

### The Colour Analogy

```
1. Alice and Bob agree on a public starting colour: YELLOW (public)
2. Alice picks a secret colour: RED (never shared)
3. Bob picks a secret colour: BLUE (never shared)
4. Alice mixes: YELLOW + RED = ORANGE → sends to Bob
5. Bob mixes:   YELLOW + BLUE = GREEN → sends to Alice
6. Alice mixes: GREEN + RED = BROWN
7. Bob mixes:   ORANGE + BLUE = BROWN
8. Both arrive at BROWN — without ever sending it

Eve sees: YELLOW, ORANGE, GREEN — cannot recover BROWN without the secret colours
```

The mathematical equivalent uses modular exponentiation or elliptic curves instead of colour mixing. The "mixing" is easy; the "unmixing" (discrete logarithm) is hard.

### ECDHE — The Modern Standard

**Elliptic Curve Diffie-Hellman Ephemeral** — used in TLS 1.3 for every connection.

"Ephemeral" means new key pairs are generated for every connection. This provides **forward secrecy**: even if the server's long-term private key is compromised later, past sessions cannot be decrypted because those ephemeral keys no longer exist.

```
TLS 1.3 handshake:
  1. Client generates ephemeral ECDH key pair
  2. Server generates ephemeral ECDH key pair
  3. They exchange public keys
  4. Each computes the shared secret independently
  5. Session keys derived from shared secret
  6. Connection encrypted with AES-GCM using those session keys
  7. After session: ephemeral private keys deleted
```

```js
// ECDH key exchange in Node.js
const alice = crypto.createECDH("prime256v1");
const alicePublicKey = alice.generateKeys();

const bob = crypto.createECDH("prime256v1");
const bobPublicKey = bob.generateKeys();

// Alice computes shared secret using her private key + Bob's public key
const aliceSecret = alice.computeSecret(bobPublicKey);

// Bob computes shared secret using his private key + Alice's public key
const bobSecret = bob.computeSecret(alicePublicKey);

// aliceSecret === bobSecret — they arrived at the same value
// Neither ever sent the secret over the wire
```

---

## 9. Hybrid Encryption

In practice, asymmetric encryption is never used alone for data. It's too slow for anything but tiny payloads.

**Hybrid encryption** combines both:

```
1. Generate a random symmetric key (AES-256, 32 bytes) — the "session key"
2. Encrypt the actual data with AES-GCM using the session key (fast, any size)
3. Encrypt the session key with RSA/ECC public key (slow, but only 32 bytes)
4. Send: [RSA-encrypted session key] + [AES-encrypted data] + [GCM auth tag]

Receiver:
1. Decrypt session key with their private key
2. Decrypt data with session key
```

This is exactly what TLS does, and what PGP does for email encryption.

```
Asymmetric: solves key distribution (public key is public)
Symmetric:  solves performance (fast bulk encryption)
Hybrid:     combines both advantages
```

---

## 10. Digital Signatures

Signatures prove:

1. The message was created by someone with the private key (authentication)
2. The message has not been modified (integrity)
3. The signer cannot deny signing it (non-repudiation)

```
Sign:
  hash = SHA-256(message)           // hash the message
  signature = RSA_sign(hash, privateKey)  // encrypt hash with private key
  send: message + signature

Verify:
  hash1 = SHA-256(received_message)      // hash what was received
  hash2 = RSA_verify(signature, publicKey) // decrypt signature with public key
  if hash1 === hash2 → signature is valid
```

Why hash first? RSA can only sign small data (smaller than key size). Hashing gives a fixed 32-byte fingerprint of any-size document.

### Algorithms

```
RSA-PSS    → RSA with proper padding for signatures
ECDSA      → ECC-based signatures (used in TLS, JWTs as ES256)
Ed25519    → Modern, fast, secure, simple — preferred for new systems

JWT signing algorithms:
  RS256  → RSA-PSS with SHA-256
  ES256  → ECDSA with P-256
  EdDSA  → Ed25519 (newer JWTs)
```

```js
// Ed25519 signing
const { publicKey, privateKey } = crypto.generateKeyPairSync("ed25519");

const message = Buffer.from("important document content");
const signature = crypto.sign(null, message, privateKey);
// null = algorithm is determined by key type (Ed25519 uses SHA-512 internally)

const isValid = crypto.verify(null, message, publicKey, signature);
// isValid → true

// If message is modified:
const tamperedMessage = Buffer.from("important MODIFIED document content");
const stillValid = crypto.verify(null, tamperedMessage, publicKey, signature);
// stillValid → false
```

---

## 11. When to Use What

```
Scenario                                    Use
──────────────────────────────────────────────────────────────────────────────
Encrypting data at rest (database fields)   AES-256-GCM
Encrypting files                            AES-256-GCM
Password storage                            bcrypt / Argon2 (not AES — see file 03)
Transmitting data securely                  TLS (handles all of this for you)
Sending a secret to a specific person       RSA-OAEP or ECIES (hybrid encryption)
Proving you sent something (non-repudiation) Ed25519 or RSA-PSS digital signature
JWT signing (single server)                 HS256 (HMAC — symmetric, simpler)
JWT signing (multiple services verify)      RS256 or ES256 (asymmetric)
Key exchange                                ECDHE (X25519 or P-256)
Verifying file integrity                    SHA-256 hash (see file 03)
Secure random token generation              crypto.randomBytes()
```

### What Not to Use

```
Don't use:
  MD5           → cryptographically broken (collision attacks)
  SHA-1         → cryptographically broken
  DES / 3DES    → too slow and small keys, broken
  RC4           → broken
  AES-ECB       → leaks patterns
  RSA-PKCS#1v1.5 for encryption → padding oracle vulnerable
  Math.random() → not cryptographically secure
```

---

## Summary

```
Symmetric encryption (AES):
  Same key to encrypt/decrypt
  Fast — for bulk data
  AES-256-GCM: authenticated encryption — confidentiality + integrity
  IV must be unique per encryption (never reuse with same key)

Asymmetric encryption (RSA, ECC):
  Public key encrypts, private key decrypts
  Slow — only for small data (keys, signatures)
  ECC preferred over RSA: same security, smaller keys, faster
  RSA-2048 minimum; prefer ECC-256 (P-256 or X25519)

Key exchange (Diffie-Hellman):
  Two parties compute shared secret without ever transmitting it
  ECDHE + "ephemeral" = forward secrecy
  TLS 1.3 uses ECDHE for every connection

Hybrid encryption:
  Random symmetric key encrypts data (fast)
  Asymmetric key encrypts the symmetric key (solves distribution)
  Used in TLS, PGP, any real-world protocol

Digital signatures:
  Private key signs, public key verifies
  Provides: integrity + authentication + non-repudiation
  Ed25519 preferred for new systems
  JWT: RS256 (RSA) or ES256 (ECC) for asymmetric; HS256 for symmetric
```

---

_Next: [03 — Hashing](./03-hashing.md)_