> Hashing proves data hasn't changed — but it doesn't prove _who_ produced it. This part covers the two mechanisms that add **authentication** to integrity: MACs (symmetric, shared-secret) and Digital Signatures (asymmetric, public-key). Understanding both is required before PKI and certificates make sense.

---

## Table of Contents

1. [The Problem: Integrity vs Authentication](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#1-the-problem-integrity-vs-authentication)
2. [MAC — Message Authentication Code](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#2-mac--message-authentication-code)
3. [HMAC — The Standard MAC Construction](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#3-hmac--the-standard-mac-construction)
4. [HMAC in Practice](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#4-hmac-in-practice)
5. [Digital Signatures — The Asymmetric Alternative](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#5-digital-signatures--the-asymmetric-alternative)
6. [How RSA Signatures Work](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#6-how-rsa-signatures-work)
7. [How ECDSA Signatures Work](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#7-how-ecdsa-signatures-work)
8. [Signature Verification Flow](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#8-signature-verification-flow)
9. [MAC vs Digital Signature — When to Use Which](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#9-mac-vs-digital-signature--when-to-use-which)
10. [Digital Signatures in the Real World](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#10-digital-signatures-in-the-real-world)
11. [Common Pitfalls](https://claude.ai/chat/9b30bfc4-b3a4-400d-9fe0-56059bf69f76#11-common-pitfalls)

---

## 1. The Problem: Integrity vs Authentication

A plain hash tells you whether data was corrupted. It does not tell you who produced the data.

```
Alice sends Bob: message + SHA-256(message)

Problem: An attacker (Mallory) can intercept, modify the message,
         recompute the hash, and send the new (message + hash) to Bob.

Bob receives a valid hash → thinks the message is intact → it is not.

The hash proves the message wasn't accidentally corrupted in transit.
It does NOT prove Alice sent it.
```

To prove both integrity AND origin, you need a secret — either a shared secret (MAC) or a private key (digital signature).

---

## 2. MAC — Message Authentication Code

A MAC is a short tag produced from a message and a **shared secret key**. Only someone with the key can produce or verify the tag.

```
MAC(key, message) → tag

Send: message + tag

Receiver:
  Recompute: MAC(key, message) → expected_tag
  Compare:   tag == expected_tag?
    → yes: message is intact AND was produced by someone with the key
    → no:  message was tampered with, or tag is forged
```

The key distinction from a plain hash:

```
Hash:  SHA-256(message)         → anyone can recompute
MAC:   HMAC(key, message)       → only key-holders can produce or verify
```

### MAC Security Properties

**Unforgeability** — Without the key, an attacker cannot produce a valid tag for any message, even after seeing many (message, tag) pairs.

**Authentication** — If the tag is valid, the message was produced by someone who holds the key.

**Integrity** — Any modification to the message invalidates the tag.

What MACs do NOT provide:

- **Non-repudiation** — Since both Alice and Bob share the same key, either could have produced the tag. Alice cannot later prove to a third party that she sent a message, and Bob cannot prove Alice sent it (Bob could have faked it himself).
- **Confidentiality** — The message is still in plaintext. MAC only authenticates; it doesn't encrypt.

---

## 3. HMAC — The Standard MAC Construction

HMAC (Hash-based Message Authentication Code) is the standard way to build a MAC using a cryptographic hash function.

### Naive Approach (Broken)

You might think: just prepend the key to the message and hash it.

```
tag = SHA-256(key || message)

This is broken. Length-extension attacks:
  SHA-256 has an internal state that's exposed in the output.
  An attacker can append data to the message and compute a valid hash
  without knowing the key.
```

### HMAC Construction (Correct)

HMAC uses two nested hash operations with derived keys to prevent length-extension attacks.

```
HMAC(key, message):
  1. Derive two subkeys:
       ipad = key XOR 0x36363636...   (inner padding)
       opad = key XOR 0x5C5C5C5C...   (outer padding)

  2. Inner hash:
       inner = SHA-256(ipad || message)

  3. Outer hash:
       tag = SHA-256(opad || inner)
```

You don't implement this yourself — every standard library has HMAC. The construction is what you need to understand.

```
HMAC-SHA256("secret-key", "Transfer $100 to Alice")
→ a4b2c9d1e8...  (32 bytes = 256 bits)

HMAC-SHA256("secret-key", "Transfer $200 to Alice")
→ f91a3d72bb...  (completely different — avalanche effect)
```

### Choosing the Hash for HMAC

```
HMAC-SHA256   → standard choice, 32-byte tag
HMAC-SHA384   → higher security margin, 48-byte tag
HMAC-SHA512   → 64-byte tag, faster on 64-bit hardware for large messages
HMAC-SHA1     → legacy, avoid in new systems
HMAC-MD5      → broken, never use
```

---

## 4. HMAC in Practice

### Timing-Safe Comparison

Never compare MAC tags with a regular string equality check. Regular equality short-circuits on the first mismatch — this leaks timing information that can be used to forge tags bit by bit.

```javascript
// WRONG — leaks timing
if (receivedTag === computedTag) { ... }

// CORRECT — constant-time comparison
import { timingSafeEqual } from 'crypto';
const isValid = timingSafeEqual(
  Buffer.from(receivedTag, 'hex'),
  Buffer.from(computedTag, 'hex')
);
```

### HMAC for API Request Signing

A common pattern for securing webhook deliveries or API calls:

```
Sender:
  timestamp = now()
  payload   = JSON.stringify(body)
  message   = timestamp + "." + payload
  tag       = HMAC-SHA256(shared_secret, message)
  Header:   X-Signature: sha256=<tag>
  Header:   X-Timestamp: <timestamp>

Receiver:
  1. Extract timestamp — reject if older than 5 minutes (replay attack prevention)
  2. Recompute: HMAC-SHA256(shared_secret, timestamp + "." + body)
  3. Compare with received tag (timing-safe)
  4. Accept only if match
```

GitHub webhooks, Stripe webhooks, and most payment providers use exactly this pattern.

---

## 5. Digital Signatures — The Asymmetric Alternative

MACs require both parties to share the same secret key. This creates a distribution problem — how do you securely share the key in the first place? And it rules out non-repudiation.

Digital signatures solve both problems using asymmetric cryptography.

```
Key pair:
  Private key  → kept secret by the signer (only Alice has this)
  Public key   → distributed freely (anyone can have this)

Signing:
  signature = Sign(private_key, message)

Verification:
  valid = Verify(public_key, message, signature)
```

The mathematical relationship between the keys means:

- Only the private key can produce a valid signature for a message
- Anyone with the public key can verify the signature
- The signature is bound to the specific message — it cannot be reused

```
Alice signs a document:
  signature = Sign(alice_private_key, document)
  Sends: document + signature

Bob verifies:
  Verify(alice_public_key, document, signature)
  → valid: document was signed by the holder of alice_private_key
           and has not been modified since signing
```

### Non-Repudiation

Unlike MACs, digital signatures provide **non-repudiation**:

```
Alice cannot later deny signing a document:
  The signature was produced with her private key.
  Only she has her private key (by definition).
  A valid signature is mathematical proof of intent.

Compare to MAC:
  Both Alice and Bob have the shared key.
  Either could have produced the tag.
  Alice can claim Bob forged it — there's no way to disprove this.
```

Non-repudiation is why contracts, code signing, and certificates use digital signatures, not MACs.

---

## 6. How RSA Signatures Work

RSA signing uses the same RSA key pair as RSA encryption, but the operation is reversed.

```
RSA Encryption:   encrypt with public key,  decrypt with private key
RSA Signature:    sign    with private key,  verify  with public key
```

### Sign

```
1. Hash the message:
   h = SHA-256(message)

2. Apply the private key operation:
   signature = h^d mod n
   (where d and n are the RSA private key components)
```

The raw message is never signed directly — always a hash of it. This is important for two reasons: RSA can only operate on values smaller than the key size, and hashing ensures the signature is the same size regardless of message length.

### Verify

```
1. Hash the received message:
   h = SHA-256(message)

2. Apply the public key operation to the signature:
   h' = signature^e mod n
   (where e and n are the RSA public key components)

3. Compare:
   h == h' → valid signature
   h != h' → invalid (message tampered or signature forged)
```

### RSA Key Sizes

```
1024 bits → broken, never use
2048 bits → minimum acceptable (most common today)
3072 bits → NIST recommended for new systems
4096 bits → higher security margin, slower performance
```

---

## 7. How ECDSA Signatures Work

ECDSA (Elliptic Curve Digital Signature Algorithm) is the modern alternative to RSA. It produces the same security with much smaller keys.

```
RSA-3072  ≈  ECDSA P-256  (256-bit key)
RSA-15360 ≈  ECDSA P-521  (521-bit key)
```

ECDSA is based on the **elliptic curve discrete logarithm problem** — given a point on a curve and the result of multiplying it by a private scalar, it's computationally infeasible to recover the scalar.

### Common Curves

```
P-256  (secp256r1)   → NIST curve, most widely supported, used in TLS, JWT
P-384  (secp384r1)   → higher security margin
P-521  (secp521r1)   → maximum NIST security
secp256k1            → Bitcoin's curve (not NIST)
Ed25519              → Edwards curve, faster and simpler than ECDSA, no random nonce needed
```

### Why Ed25519 Is Gaining Ground

Standard ECDSA requires a random nonce `k` per signature. If `k` is ever reused or predictable, the private key can be recovered. This is a real vulnerability — Sony's PS3 was hacked because ECDSA was implemented with a constant `k`.

Ed25519 (EdDSA on Curve25519) is deterministic — the nonce is derived from the message and private key, so it is never random and never reused. This eliminates an entire class of implementation errors.

```
ECDSA (P-256):   requires secure random nonce per signature
Ed25519:         deterministic — same (key, message) always produces same signature
                 faster signing and verification
                 smaller signatures (64 bytes vs 72 bytes)
```

---

## 8. Signature Verification Flow

End-to-end flow with the full picture:

```
SIGNING (Alice):
  ┌──────────────────────────────────────────────┐
  │  message = "Transfer $500 to Bob"            │
  │  hash    = SHA-256(message)                  │
  │  sig     = Sign(alice_private_key, hash)     │
  │  send → (message, sig)                       │
  └──────────────────────────────────────────────┘

VERIFICATION (Bob):
  ┌──────────────────────────────────────────────┐
  │  Receive (message, sig)                      │
  │  hash'  = SHA-256(message)                   │
  │  valid  = Verify(alice_public_key, hash', sig)│
  │                                              │
  │  If valid:                                   │
  │    → message was produced by Alice's key     │
  │    → message has not been modified           │
  │                                              │
  │  If invalid:                                 │
  │    → message was tampered after signing, or  │
  │    → signature was not made with Alice's key │
  └──────────────────────────────────────────────┘
```

Note: verification only proves the signature was made by the holder of the private key. Whether that private key actually belongs to "Alice" is a separate question — that's what certificates and PKI answer.

---

## 9. MAC vs Digital Signature — When to Use Which

```
                          MAC              Digital Signature
──────────────────────────────────────────────────────────────
Key type               Shared secret       Key pair
Who can verify         Anyone with key     Anyone with public key
Non-repudiation        No                  Yes
Performance            Very fast           Slower (especially RSA)
Key distribution       Hard                Easy (public key is public)
Use case               Internal systems    Public-facing, legal proof
Examples               Webhook signing     JWTs, TLS, code signing
                       Session tokens      Certificate signing
                       API auth (same org) Software releases
```

Choose a MAC when:

- Both parties already share a secret (same system, internal service-to-service)
- You need maximum performance
- Non-repudiation is not required

Choose a digital signature when:

- The verifier is a third party who cannot share a secret
- You need non-repudiation (contracts, transactions, audit logs)
- The public key needs to be distributed widely (code signing, TLS)

---

## 10. Digital Signatures in the Real World

### JWT (JSON Web Tokens)

JWTs are signed using HMAC-SHA256 (symmetric) or RS256/ES256 (asymmetric).

```
Header:    { "alg": "ES256", "typ": "JWT" }
Payload:   { "sub": "user_123", "role": "admin", "exp": 1234567890 }
Signature: ECDSA-Sign(private_key, base64(header) + "." + base64(payload))

Token: base64(header).base64(payload).base64(signature)
```

Anyone with the public key can verify the token — the server that issued it doesn't need to be contacted again.

### Code Signing

Software publishers sign their releases with their private key. Operating systems verify the signature with the publisher's public key before installing:

```
Developer:
  hash = SHA-256(installer.exe)
  sig  = Sign(dev_private_key, hash)
  distribute: installer.exe + sig + dev_certificate

User's OS:
  1. Verify the certificate is issued by a trusted CA
  2. Verify sig against hash of installer.exe using public key from certificate
  → "This installer was published by Verified Developer Inc. and has not been modified."
```

### Git Commit Signing

Git supports signing commits with GPG or SSH keys:

```bash
git config --global commit.gpgsign true
git commit -m "feat: add payment module"
# Commit is signed with your private key

git log --show-signature
# Shows: "Good signature from user@example.com"
```

### TLS Certificates

Every TLS certificate you encounter is a Certificate Authority's digital signature over a domain's public key. This is the bridge from this part to Part 04 (PKI).

```
CA signs:
  "The public key [xyz...] belongs to example.com"
  signature = Sign(CA_private_key, certificate_data)

Browser verifies:
  Verify(CA_public_key, certificate_data, signature)
  → trusted: the public key really belongs to example.com
```

---

## 11. Common Pitfalls

### Signing the Wrong Thing

Always sign a hash of the data, never raw data. And be precise about what is in scope.

```
Wrong: Sign only the amount in a payment request
       → attacker changes the recipient name (unsigned) while keeping valid signature

Right: Sign the entire canonical form of the request
       amount + recipient + timestamp + nonce
```

### Algorithm Confusion

In systems that accept multiple algorithms, ensure the verifier enforces which algorithm is expected.

```
JWT "alg: none" attack:
  Some early JWT libraries accepted alg: none and skipped verification entirely.
  Always explicitly check the algorithm before verifying.

RSA/HMAC confusion:
  If a system accepts both RS256 (asymmetric) and HS256 (symmetric),
  an attacker could send a token signed with HS256 using the server's
  public key as the HMAC secret — if the server uses its public key as
  the HS256 verification key, the token passes.
  Fix: enforce a single algorithm per token type.
```

### Weak Keys

```
RSA-1024 → broken
RSA-2048 → minimum
RSA-4096 → preferred for long-lived keys

ECDSA P-192 → avoid
ECDSA P-256 → standard minimum
Ed25519     → preferred for new systems
```

### Reusing Nonces in ECDSA

As noted earlier: if the random nonce `k` in ECDSA is ever reused for two different messages, the private key can be algebraically recovered. Use Ed25519 to eliminate this risk entirely.

### Not Binding Signatures to Context

A signature proves the data was signed — it doesn't prove it was signed for your specific protocol. If you sign the same data for different purposes, a signature from one context can be replayed in another.

```
Mitigation: Include a domain separator in what you sign.
  Sign: "myapp_v1_payment" + canonical_payload
  Not:  canonical_payload alone
```