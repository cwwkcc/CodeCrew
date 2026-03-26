> TLS (Transport Layer Security) is the protocol that powers HTTPS. Every time a padlock appears in your browser, TLS is running underneath. This covers the full handshake, what actually happens when you type a URL, cipher suites, forward secrecy, and the attacks TLS protects against.

---

## Table of Contents

1. [TLS vs SSL — History](#1-tls-vs-ssl--history)
2. [What TLS Provides](#2-what-tls-provides)
3. [TLS 1.3 Handshake — Full Detail](#3-tls-13-handshake--full-detail)
4. [TLS 1.2 Handshake — For Comparison](#4-tls-12-handshake--for-comparison)
5. [Cipher Suites](#5-cipher-suites)
6. [Forward Secrecy](#6-forward-secrecy)
7. [Session Resumption](#7-session-resumption)
8. [TLS Record Protocol](#8-tls-record-protocol)
9. [MITM Attacks Against TLS](#9-mitm-attacks-against-tls)
10. [Historical TLS Attacks](#10-historical-tls-attacks)
11. [Configuring TLS Correctly](#11-configuring-tls-correctly)
12. [TLS Inspection / Corporate Proxies](#12-tls-inspection--corporate-proxies)

---

## 1. TLS vs SSL — History

```
SSL 1.0  → 1994, Netscape, never released (too flawed)
SSL 2.0  → 1995, Netscape, released — broken, disabled everywhere
SSL 3.0  → 1996, Netscape — POODLE attack (2014) → deprecated
TLS 1.0  → 1999, IETF RFC 2246 — SSL 3.0 with fixes — deprecated 2021
TLS 1.1  → 2006, IETF RFC 4346 — minor improvements — deprecated 2021
TLS 1.2  → 2008, IETF RFC 5246 — still widely used, still supported
TLS 1.3  → 2018, IETF RFC 8446 — major redesign, current standard
```

When people say "SSL" colloquially, they mean TLS. The term "SSL certificate" is historical — all modern certificates are TLS certificates.

**TLS 1.0 and 1.1 are disabled** by modern browsers, servers, and CDNs. TLS 1.2 is still widely supported. TLS 1.3 is the goal.

---

## 2. What TLS Provides

```
Confidentiality:
  All data encrypted — attacker intercepting traffic sees encrypted bytes
  Even the SNI (Server Name Indication — the hostname) is encrypted in TLS 1.3
  (with Encrypted Client Hello, or ECH — optional extension, growing in deployment)

Integrity:
  Every TLS record has an authentication tag (AEAD — AES-GCM or ChaCha20-Poly1305)
  Any modification of a single byte is detected and connection terminated

Authentication:
  Server presents a certificate proving it is who it claims to be
  Client verifies the certificate chain (see file 04)
  Mutual TLS (mTLS) → client also presents a certificate (used in service meshes)

Replay protection:
  Each record has a sequence number
  Replaying old TLS records is detected and rejected
```

---

## 3. TLS 1.3 Handshake — Full Detail

TLS 1.3 reduced the handshake from 2 round trips to 1 (with 0-RTT resumption possible).

```
Client                                    Server
  │                                          │
  │──── ClientHello ─────────────────────►  │
  │     Version: TLS 1.3                     │
  │     Supported cipher suites             │
  │     Supported key exchange groups        │
  │     Key Share: ECDH public key           │  ← client sends key share upfront
  │     SNI: api.example.com               │  ← which hostname it wants
  │     Session ticket (for resumption)     │
  │                                          │
  │  ◄──── ServerHello ────────────────────  │
  │        Chosen cipher suite              │
  │        Key Share: server's ECDH pubkey  │  ← server responds with its key share
  │        [All subsequent records encrypted]│
  │                                          │  Server now computes shared secret
  │                                          │  Derives session keys
  │                                          │
  │  ◄──── {EncryptedExtensions} ──────────  │  ← encrypted from here
  │  ◄──── {Certificate} ──────────────────  │  server's cert (encrypted)
  │  ◄──── {CertificateVerify} ────────────  │  signature proving server has private key
  │  ◄──── {Finished} ─────────────────────  │  MAC over full handshake transcript
  │                                          │
  │  Client verifies certificate             │
  │  Client verifies CertificateVerify sig   │
  │  Client computes shared secret (same)    │
  │  Client verifies Finished MAC            │
  │                                          │
  │──── {Finished} ──────────────────────►  │
  │                                          │
  │  ←──── Application Data ─────────────►  │  encrypted communication begins
```

### Key Points

**1-RTT handshake:** In TLS 1.2, the handshake takes 2 round trips. TLS 1.3 achieves 1 round trip because the client sends its key share in the ClientHello (it guesses which group the server will choose).

**All certificates are encrypted:** In TLS 1.2, certificates were sent in plaintext. In TLS 1.3, everything after ServerHello is encrypted. An attacker watching the network can't see which certificate was presented.

**No negotiation of legacy options:** TLS 1.3 removed all weak cipher suites, export crypto, RSA key exchange, and CBC mode. Only AEAD ciphers allowed.

**CertificateVerify:** The server signs the handshake transcript with its private key. This proves the server actually possesses the private key corresponding to the certificate (not just a copy of someone else's cert).

---

## 4. TLS 1.2 Handshake — For Comparison

```
Client                              Server

── ClientHello ──────────────────►
   TLS version, cipher suites,
   random nonce

◄── ServerHello ─────────────────
    Chosen cipher suite, random nonce

◄── Certificate ─────────────────   (plaintext — visible to network)
◄── ServerKeyExchange ───────────   (DH params, if DHE/ECDHE)
◄── ServerHelloDone ─────────────

── ClientKeyExchange ────────────►  (premaster secret encrypted with server pubkey)
── ChangeCipherSpec ─────────────►  (switching to encrypted)
── Finished ─────────────────────►

◄── ChangeCipherSpec ─────────────
◄── Finished ─────────────────────

──── Application Data ───────────►
◄─── Application Data ────────────
```

**2 round trips** before application data can flow.

**Certificates in plaintext:** An observer can see which certificate the server presented, revealing the domain. (TLS 1.3 fixed this.)

**RSA key exchange option:** In TLS 1.2 with RSA key exchange (not DHE/ECDHE), the client encrypts a secret with the server's RSA public key. If the server's private key is later compromised, ALL past recorded traffic can be decrypted. TLS 1.3 eliminated this — all key exchange is ephemeral.

---

## 5. Cipher Suites

A **cipher suite** is a set of algorithms that defines exactly how a TLS connection is protected.

### TLS 1.2 Format

```
TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384

TLS_           → protocol
ECDHE_         → key exchange algorithm (Elliptic Curve Diffie-Hellman Ephemeral)
RSA_           → authentication algorithm (how server cert is signed)
WITH_          → separator
AES_256_GCM_   → bulk encryption (AES-256 in GCM mode)
SHA384         → MAC/PRF algorithm
```

### TLS 1.3 Cipher Suites (Simplified)

TLS 1.3 separates key exchange, authentication, and encryption:

```
Key exchange:      Always ECDHE or DHE (ephemeral — forward secrecy guaranteed)
Authentication:    Determined by the certificate type (RSA or ECDSA)
Encryption suites: Only 5 allowed:
  TLS_AES_128_GCM_SHA256        ← most common
  TLS_AES_256_GCM_SHA384
  TLS_CHACHA20_POLY1305_SHA256  ← for devices without AES hardware acceleration
  TLS_AES_128_CCM_SHA256
  TLS_AES_128_CCM_8_SHA256
```

All five are AEAD (authenticated encryption) — no separate MAC step.

### Choosing Cipher Suites (TLS 1.2 Server Config)

```nginx
# Nginx — recommended modern config
ssl_protocols TLSv1.2 TLSv1.3;
ssl_ciphers ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256;
ssl_prefer_server_ciphers on;

# What this does:
# - Only TLS 1.2+ (no 1.0, 1.1)
# - Only ECDHE (forward secrecy)
# - Only GCM or ChaCha20-Poly1305 (AEAD — authenticated encryption)
# - No CBC, no RC4, no export ciphers, no 3DES
```

---

## 6. Forward Secrecy

**Forward secrecy** (or **perfect forward secrecy, PFS**) means that compromising the server's long-term private key does not allow decryption of past recorded traffic.

### Without Forward Secrecy (TLS 1.2 with RSA key exchange)

```
1. Client generates premaster secret
2. Client encrypts it with server's RSA public key
3. Server decrypts with its RSA private key
4. Both derive session keys from premaster secret

If attacker:
  → Records all encrypted traffic
  → Later (years from now) obtains server's private key
  → Can decrypt the premaster secret from recorded handshake
  → Derives the same session keys
  → Decrypts ALL recorded traffic
```

### With Forward Secrecy (ECDHE)

```
1. Server generates EPHEMERAL ECDH key pair (fresh for this connection)
2. Client generates EPHEMERAL ECDH key pair
3. They exchange public keys
4. Both compute shared secret (Diffie-Hellman math)
5. Session keys derived from shared secret
6. Ephemeral private keys DELETED after handshake

If attacker:
  → Records all encrypted traffic
  → Later obtains server's LONG-TERM private key
  → This key was not involved in key exchange!
  → Cannot recover the ephemeral keys (deleted)
  → Cannot decrypt past traffic
```

The long-term private key is only used to authenticate (sign the CertificateVerify). The actual encryption keys are derived from ephemeral ECDH. Each session gets fresh, independent keys.

**TLS 1.3 mandates forward secrecy** — RSA key exchange was removed. All TLS 1.3 connections have forward secrecy.

---

## 7. Session Resumption

TLS handshakes add latency. For a user visiting multiple pages on the same site, doing a full handshake on every connection is wasteful. Session resumption allows skipping most of the handshake.

### TLS 1.3 Session Tickets

```
After initial handshake:
  Server sends NewSessionTicket message
  Contains: encrypted session state (keys, parameters)
  Encrypted with a server-only key

On next connection:
  Client sends the ticket in ClientHello
  Server decrypts ticket → recovers session state
  Skips certificate exchange, key derivation
  
  0-RTT data:
    Client can even send application data in the first message (0-RTT)
    Before server has responded
    Trade-off: 0-RTT data has no forward secrecy and is replay-vulnerable
    Only safe for idempotent requests (GET, not POST with side effects)
```

### Session Ticket Key Rotation

Session ticket encryption keys must be rotated. If the ticket key leaks:

- Past sessions can be decrypted (breaks forward secrecy)
- Old tickets can be used to resume sessions

```nginx
# Nginx — rotate session ticket keys
ssl_session_tickets on;
ssl_session_ticket_key /path/to/ticket.key;
# Rotate this key periodically (e.g., every 24 hours)
# Old tickets with old key become invalid — users do full handshake
```

---

## 8. TLS Record Protocol

Once the handshake is complete, all data is sent in **TLS records**.

```
TLS Record structure:
  ┌──────────────────────────────────────────────────────┐
  │ Content Type  (1 byte) — application data, alert, etc│
  │ Legacy Version (2 bytes) — always 0x0303 in TLS 1.3  │
  │ Length        (2 bytes) — length of the payload      │
  │ Payload       (variable) — encrypted data + auth tag │
  └──────────────────────────────────────────────────────┘
```

Each record is encrypted independently with AES-GCM or ChaCha20-Poly1305. The sequence number is included in the AAD (Additional Authenticated Data) of the AEAD cipher — this prevents replay attacks (an attacker can't take record #5 and insert it earlier in the stream).

Maximum record size: 16KB. Large payloads are split across multiple records.

---

## 9. MITM Attacks Against TLS

### Classic MITM

```
Without TLS:
  Client → [packets in clear] → Attacker → [packets in clear] → Server
  Attacker reads everything

With TLS (correctly implemented):
  Client → [TLS to server cert] → Server
  Attacker intercepts TCP connection but can't decrypt (no private key)
```

### MITM via Rogue Certificate

```
Attacker obtains a CA-signed certificate for the target domain:
  → Compromises a CA
  → Social engineers a CA into issuing the cert
  → Uses a subdomain cert with wildcard (if the cert is for *.example.com)

Then positions themselves between client and server.
Client verifies the certificate → valid! (it's CA-signed) → proceeds.
Attacker decrypts, reads, re-encrypts, forwards.

Defense: Certificate Transparency (CT) logs — unauthorized certs are detectable.
         Certificate pinning — client only accepts specific cert/pubkey hash.
```

### SSL Stripping

```
Browser visits http://bank.com (HTTP, not HTTPS)
Attacker intercepts → serves the page over HTTP to the victim
But connects to bank.com via HTTPS for the actual content
Victim sees http:// (no padlock) but trusts it
Attacker reads everything in plaintext

Defense: HSTS — browser never makes HTTP requests to HSTS-enabled domains
         HSTS Preload — even on first visit, browser uses HTTPS
```

---

## 10. Historical TLS Attacks

Understanding past attacks explains why TLS 1.3 removed so much.

### POODLE (2014) — Killed SSL 3.0

CBC mode in SSL 3.0 had padding oracle vulnerability. Downgrade attacks forced browsers to use SSL 3.0, then exploited POODLE. Fixed by: removing SSL 3.0 support.

### BEAST (2011) — Against TLS 1.0 CBC

Browser Exploit Against SSL/TLS. Exploited predictable IVs in TLS 1.0 CBC mode. Partly mitigated, fully fixed by: TLS 1.1+ or not using CBC (GCM).

### CRIME / BREACH — Compression Oracles

Exploited TLS compression to recover secrets (like CSRF tokens) by observing ciphertext length changes when request included attacker-chosen content. Fixed by: disabling TLS compression (already off by default in TLS 1.3).

### HEARTBLEED (2014) — OpenSSL Bug

Buffer over-read in OpenSSL's heartbeat extension. Allowed reading up to 64KB of server memory per request — including private keys, session data, passwords. Affected ~17% of HTTPS servers at the time. Fixed by: patching OpenSSL.

Lesson: Even correct protocol design can't protect against implementation bugs. Keep libraries updated.

### LOGJAM / FREAK — Export Crypto Downgrade

Forced servers to use intentionally weakened "export" cipher suites from 1990s US export restrictions. Fixed by: removing export cipher support from clients and servers. TLS 1.3 banned all weak ciphers.

### Lucky 13 — CBC Timing

Timing attack against CBC padding verification in TLS 1.2. Fixed by: careful constant-time implementation, and by using GCM (no padding). TLS 1.3 has no CBC ciphers.

### Pattern: Why TLS 1.3 Removed Everything

TLS 1.3 removed:

- RSA key exchange (no forward secrecy)
- CBC cipher modes (padding oracle attacks)
- RC4 (biases in keystream)
- DH with static params (Logjam)
- SHA-1 and MD5 in PRF
- Compression
- Renegotiation
- Export crypto
- All cipher suites below 128-bit security

Every removal was motivated by a real attack. The simplification reduces attack surface.

---

## 11. Configuring TLS Correctly

### Nginx — Production Config

```nginx
server {
  listen 443 ssl;
  listen [::]:443 ssl;
  http2 on;  # HTTP/2 requires HTTPS — significant performance improvement

  server_name api.example.com;

  # Certificate and key
  ssl_certificate /etc/letsencrypt/live/api.example.com/fullchain.pem;
  ssl_certificate_key /etc/letsencrypt/live/api.example.com/privkey.pem;

  # TLS versions — 1.2 and 1.3 only
  ssl_protocols TLSv1.2 TLSv1.3;

  # Cipher suites for TLS 1.2 (TLS 1.3 handles its own)
  ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305;
  ssl_prefer_server_ciphers off;  # Let client choose (client usually has better hardware knowledge)

  # ECDH curve
  ssl_ecdh_curve X25519:prime256v1:secp384r1;

  # Session cache (for TLS 1.2)
  ssl_session_timeout 1d;
  ssl_session_cache shared:MozSSL:10m;
  ssl_session_tickets off;  # Or rotate keys regularly

  # OCSP stapling
  ssl_stapling on;
  ssl_stapling_verify on;
  ssl_trusted_certificate /etc/letsencrypt/live/api.example.com/chain.pem;
  resolver 1.1.1.1 8.8.8.8 valid=300s;

  # HSTS
  add_header Strict-Transport-Security "max-age=63072000; includeSubDomains; preload" always;

  # Other security headers (covered in file 07)
  add_header X-Frame-Options DENY always;
  add_header X-Content-Type-Options nosniff always;
}

# HTTP → HTTPS redirect
server {
  listen 80;
  listen [::]:80;
  server_name api.example.com;
  return 301 https://$host$request_uri;
}
```

### Testing TLS Configuration

```bash
# SSL Labs (web tool — check public site)
# https://www.ssllabs.com/ssltest/ → should score A or A+

# testssl.sh (command line)
./testssl.sh api.example.com

# Check TLS version support
openssl s_client -connect api.example.com:443 -tls1_1
# Should fail (TLS 1.1 disabled)
openssl s_client -connect api.example.com:443 -tls1_2
# Should succeed

# Check certificate chain
openssl s_client -connect api.example.com:443 -showcerts
```

---

## 12. TLS Inspection / Corporate Proxies

Many corporate networks perform **TLS inspection** (also called SSL inspection or SSL interception). The proxy terminates your TLS connection, reads the plaintext, re-encrypts to the destination.

```
Normal TLS:
  Client → [TLS to server's cert] → Server

With corporate proxy:
  Client → [TLS to proxy's cert] → Proxy → [TLS to server's cert] → Server
  
  Proxy has the corp's CA certificate installed on your device.
  Your browser trusts the corp CA.
  Proxy generates a cert for example.com, signed by the corp CA.
  Your browser trusts it → no warning.
  Proxy reads everything.
```

This is a legitimate use case (corporate security monitoring, data loss prevention). The corp IT team installs their CA certificate on company-managed devices. BYOD or personal devices would see an untrusted cert warning.

**Implications for application developers:**

- Certificate pinning defeats corporate proxies (the pinned hash won't match)
- This is often intentional — mobile apps pin to prevent interception
- This can cause problems in managed environments — consider carefully before pinning in enterprise software

---

## Summary

```
TLS history:
  SSL is dead. TLS 1.0/1.1 deprecated. TLS 1.2 still used. TLS 1.3 is current.

TLS provides:
  Confidentiality (AEAD encryption), Integrity (auth tag per record),
  Authentication (certificate chain), Replay protection (sequence numbers)

TLS 1.3 handshake:
  1 round trip (vs TLS 1.2's 2)
  Client sends key share in ClientHello
  Certificates encrypted
  Mandates forward secrecy (ECDHE only)

Cipher suites:
  Only AEAD in TLS 1.3: AES-GCM, ChaCha20-Poly1305
  ECDHE always (ephemeral = forward secrecy)
  No CBC, no RSA key exchange, no weak ciphers

Forward secrecy:
  Ephemeral ECDH keys — deleted after handshake
  Long-term key compromise doesn't expose past sessions
  TLS 1.3 mandates it; TLS 1.2 with ECDHE has it

Historical attacks drove TLS 1.3 simplification:
  POODLE, BEAST, CRIME, LOGJAM, FREAK — all attack features now removed

Configuration:
  TLS 1.2 + 1.3 only
  ECDHE cipher suites only
  OCSP stapling
  HSTS
  Session ticket key rotation
```

---

_Next: [06 — DNS Security](./06-dns-security.md)_