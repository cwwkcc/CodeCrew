> TLS is what makes HTTPS secure. Every time you see the padlock in the browser, TLS is the reason. This part goes deep — the handshake, certificate verification, the mathematics of key exchange, and the practical things that matter when building Paideon.

---

## Table of Contents

1. [What TLS Is — And What It Is Not](#1-what-tls-is--and-what-it-is-not)
2. [The TLS Handshake — TLS 1.3 Step by Step](#2-the-tls-handshake--tls-13-step-by-step)
3. [Key Exchange — The Mathematics](#3-key-exchange--the-mathematics)
4. [Certificates — What They Are and How They Work](#4-certificates--what-they-are-and-how-they-work)
5. [Certificate Authorities and the Chain of Trust](#5-certificate-authorities-and-the-chain-of-trust)
6. [Cipher Suites — What Gets Negotiated](#6-cipher-suites--what-gets-negotiated)
7. [TLS 1.2 vs TLS 1.3](#7-tls-12-vs-tls-13)
8. [SNI — How One Server Serves Many Domains](#8-sni--how-one-server-serves-many-domains)
9. [HSTS — Forcing HTTPS](#9-hsts--forcing-https)
10. [Certificate Pinning](#10-certificate-pinning)
11. [Mixed Content — Why One HTTP Resource Breaks Everything](#11-mixed-content--why-one-http-resource-breaks-everything)
12. [Let's Encrypt and Automatic Certificates](#12-lets-encrypt-and-automatic-certificates)
13. [TLS in the Paideon Stack](#13-tls-in-the-paideon-stack)

---

## 1. What TLS Is — And What It Is Not

TLS (Transport Layer Security) is the protocol that provides:

```
Confidentiality    Data is encrypted. An eavesdropper cannot read it.
Integrity          Data cannot be modified in transit without detection.
Authentication     You are actually talking to the real server, not an impostor.
```

What TLS is NOT:

```
TLS does not hide WHO you connect to.
  The IP address is visible. The domain name leaks via SNI.
  Your ISP knows you connected to paideon.lk.
  They cannot see which page or what data.

TLS does not protect against the server itself.
  Once data reaches the server, TLS has done its job.
  If your server is compromised, TLS provides no protection.

TLS does not protect against flawed application code.
  SQL injection, auth bypass, etc. — these happen AFTER TLS decryption.
  TLS is a transport-layer guarantee. App-layer security is separate.

TLS does not guarantee the identity of the person you're talking to.
  It guarantees the identity of the SERVER (via certificates).
  The person operating the server might still be malicious.
  "The connection is secure" ≠ "The website is trustworthy"
```

---

## 2. The TLS Handshake — TLS 1.3 Step by Step

TLS 1.3 completes the handshake in one round trip (1-RTT). TLS 1.2 required two.

```
Client                                    Server
  │                                         │
  │── TCP SYN ─────────────────────────── ▶ │
  │◀── TCP SYN-ACK ─────────────────────── │   TCP established
  │── TCP ACK ─────────────────────────── ▶ │
  │                                         │
  │── ClientHello ─────────────────────── ▶ │
  │   TLS version: 1.3                      │
  │   Client random: [32 random bytes]      │
  │   Supported cipher suites:              │
  │     TLS_AES_256_GCM_SHA384              │
  │     TLS_CHACHA20_POLY1305_SHA256        │
  │   Key share: [X25519 public key]        │
  │   SNI: "paideon.lk"                     │
  │                                         │
  │◀── ServerHello ────────────────────── │
  │    Selected cipher: AES_256_GCM_SHA384  │
  │    Server key share: [X25519 public key]│
  │    [Encrypted from here on]             │
  │                                         │
  │◀── Certificate ────────────────────── │ (encrypted)
  │◀── CertificateVerify ──────────────── │ (encrypted)
  │◀── Finished ───────────────────────── │ (encrypted)
  │                                         │
  │── Finished ───────────────────────── ▶ │
  │                                         │
  │   [ Application data flows, encrypted ] │
  │── GET /api/students HTTP/1.1 ──────── ▶│
  │◀── HTTP/1.1 200 OK ─────────────────── │
```

### What Each Message Contains

**ClientHello:**

```
TLS version: 1.3
Client random: 32 cryptographically random bytes generated right now
               This randomness is critical — it ensures each session
               produces different keys even with the same server.
Cipher suites: List of crypto algorithms client supports, in preference order
Key share: Client's public key for Diffie-Hellman key exchange
           Client picks a key exchange method (X25519) and computes its
           DH public value — sent speculatively to save a round trip
SNI: Server Name Indication — the hostname being requested
     Sent in plaintext. Required so server knows which cert to use.
```

**ServerHello:**

```
Selected cipher suite: The best cipher both sides support
Server random: 32 cryptographically random bytes from the server
Server key share: Server's DH public value (in response to client's)
```

At this point, both sides have enough information to independently derive the same session keys. No key was ever sent over the network.

**Certificate:**

```
The server's TLS certificate.
Contains: server's public key, domain name, issuer, validity dates.
Encrypted in TLS 1.3 (unencrypted in TLS 1.2).
```

**CertificateVerify:**

```
The server signs a hash of the entire handshake transcript
using its private key.

This proves: "I actually possess the private key for this certificate."
Without this, anyone who obtained a copy of the certificate could impersonate the server.
```

**Finished (server then client):**

```
A MAC (Message Authentication Code) of the entire handshake.
Derived from the session keys.

Both sides verify these match.
If anyone tampered with the handshake (changed cipher suites, injected data),
these MACs won't match → abort.
```

---

## 3. Key Exchange — The Mathematics

The fundamental problem: two parties want to agree on a secret key over a public channel. How?

### Diffie-Hellman (conceptual version)

```
Both parties agree on two public numbers: g = 5, p = 23
(Anyone can know these. They're sent in plaintext.)

Client secretly picks: a = 6
Server secretly picks: b = 15

Client computes: A = g^a mod p = 5^6 mod 23 = 15,625 mod 23 = 8
                 Sends A=8 to server. Anyone can see A=8.

Server computes: B = g^b mod p = 5^15 mod 23 = 30,517,578,125 mod 23 = 19
                 Sends B=19 to client. Anyone can see B=19.

Client computes: shared = B^a mod p = 19^6 mod 23 = 47,045,881 mod 23 = 2
Server computes: shared = A^b mod p = 8^15 mod 23 = 35,184,372,088,832 mod 23 = 2

BOTH GET THE SAME NUMBER: 2
The shared secret is 2.

What an eavesdropper sees: g=5, p=23, A=8, B=19
To find the secret: must solve the Discrete Logarithm Problem
  "Given 5^x mod 23 = 8, find x"
  With real parameters (2048-bit numbers), this is computationally infeasible.
```

### X25519 — What TLS 1.3 Actually Uses

X25519 is an elliptic curve Diffie-Hellman protocol using Curve25519.

```
The mathematics are more complex but the concept is identical:
  → Both parties pick private random values
  → Compute public values using elliptic curve point multiplication
  → Exchange public values
  → Both independently compute the same shared secret
  → Private values never cross the network

Why X25519 over classic DH:
  → Smaller keys: 32 bytes vs 256+ bytes (RSA-2048)
  → Faster computation
  → No known weak parameter choices (Curve25519 is designed to be safe)
  → Designed by Daniel Bernstein (trusted curve)
```

### Forward Secrecy

This is why TLS 1.3 uses ephemeral key exchange (fresh keys every session):

```
TLS 1.2 (without forward secrecy):
  Server has a static private key.
  An attacker records all encrypted traffic for years.
  Years later, they steal the server's private key.
  Now they can decrypt ALL historical traffic.

TLS 1.3 (with forward secrecy — mandatory):
  Each session uses a fresh ephemeral key pair.
  Keys are discarded after the session.
  An attacker who steals the private key today:
  → Can decrypt future connections (until you replace the cert)
  → CANNOT decrypt past connections (those session keys are gone)

Forward secrecy = past sessions are protected even if long-term keys are compromised.
```

---

## 4. Certificates — What They Are and How They Work

A TLS certificate is a signed data structure that binds a public key to a domain name.

```
Structure of a certificate (X.509 format):

Subject:
  Common Name (CN):     paideon.lk
  Subject Alt Names:    paideon.lk, www.paideon.lk

Issuer:
  Common Name:          R11                    ← Let's Encrypt intermediate CA
  Organization:         Let's Encrypt

Validity:
  Not Before:           2026-01-01 00:00:00 UTC
  Not After:            2026-04-01 00:00:00 UTC  ← 90 days (Let's Encrypt)

Public Key:
  Algorithm:            id-ecPublicKey (P-256)
  Public Key:           [65 bytes of elliptic curve point]

Signature:
  Algorithm:            ecdsa-with-SHA384
  Signature:            [Let's Encrypt's signature over everything above]
```

### What the Browser Verifies

```
1. Does the domain match?
   Certificate says: paideon.lk
   We're connecting to: paideon.lk ✓
   
   Wildcard: *.paideon.lk matches api.paideon.lk ✓
   But NOT: paideon.lk (wildcard doesn't match the apex)
   And NOT: sub.api.paideon.lk (wildcard only goes one level deep)

2. Is today within the validity period?
   Certificate expires 2026-04-01.
   Today is 2026-03-13.  ✓
   If expired → NET::ERR_CERT_DATE_INVALID

3. Is the issuer trusted?
   Check the issuer's certificate. Is IT trusted?
   Walk up the chain until you reach a root CA.
   Is that root CA in the browser's built-in trust store?
   If yes → ✓
   If not → NET::ERR_CERT_AUTHORITY_INVALID

4. Is the signature valid?
   The Let's Encrypt intermediate CA signed the certificate.
   Browser verifies the signature using Let's Encrypt's public key.
   If signature is valid → nobody tampered with the certificate.

5. Has the certificate been revoked?
   CRL (Certificate Revocation List): download a list of revoked certs
   OCSP (Online Certificate Status Protocol): query issuer in real-time
   OCSP Stapling: server includes a fresh OCSP response in the TLS handshake
   (This is the modern approach — avoids extra round trips)

All 5 checks must pass. One failure → the browser refuses to connect.
```

---

## 5. Certificate Authorities and the Chain of Trust

No one is born trusted. Trust is delegated.

```
ROOT CA (self-signed, ultimate trust anchor)
  e.g., ISRG Root X1 (Let's Encrypt's root)
  
  Stored in browser's/OS's built-in trust store.
  Root CAs are offline — they never directly sign end-entity certs.
  (If their private key leaked, all trust would be broken.)

INTERMEDIATE CA (signed by root)
  e.g., Let's Encrypt R11
  
  Intermediate CAs sign the actual certificates.
  They're online but their key is rotated every few years.
  If compromised, only certificates from that intermediate are affected.

END-ENTITY CERTIFICATE (signed by intermediate)
  e.g., paideon.lk's certificate
  
  This is what your server presents.
  Signed by Let's Encrypt R11.

Chain of trust:
  paideon.lk cert  ← signed by → Let's Encrypt R11 ← signed by → ISRG Root X1
  
  Browser: "Is ISRG Root X1 in my trust store? Yes. Chain is valid."
```

### Certificate Transparency

```
Since 2018, all publicly trusted certificates must be logged
in public Certificate Transparency (CT) logs.

When Let's Encrypt issues your certificate, it's logged.
Anyone can query https://crt.sh/?q=paideon.lk to see all certs
ever issued for your domain.

Why this matters:
  If a CA issues a rogue certificate for your domain,
  it will appear in CT logs.
  Monitoring CT logs can detect certificate mis-issuance.
  
  CAA DNS records tell CAs which ones are allowed to issue
  certs for your domain, adding another layer of protection.
```

---

## 6. Cipher Suites — What Gets Negotiated

A cipher suite specifies exactly which algorithms are used for a TLS session.

```
TLS_AES_256_GCM_SHA384
│   │   │   │   │
│   │   │   │   └── Hash for HKDF (key derivation)
│   │   │   └── Authentication mode (GCM = Galois/Counter Mode)
│   │   └── Key size (256 bits)
│   └── Symmetric cipher (AES)
└── Protocol

In TLS 1.3, key exchange is always ECDHE (ephemeral DH).
Authentication is always via the certificate.
The cipher suite only specifies symmetric crypto + hash.

TLS 1.3 cipher suites:
  TLS_AES_128_GCM_SHA256        → AES-128-GCM for data, SHA-256 for HKDF
  TLS_AES_256_GCM_SHA384        → AES-256-GCM for data, SHA-384 for HKDF
  TLS_CHACHA20_POLY1305_SHA256  → ChaCha20-Poly1305 for data, SHA-256 for HKDF

ChaCha20-Poly1305:
  Alternative to AES. Faster on devices without hardware AES acceleration.
  Mobile devices and IoT hardware often use this.
  Same security level as AES-256-GCM.
```

---

## 7. TLS 1.2 vs TLS 1.3

```
                    TLS 1.2                 TLS 1.3
──────────────────────────────────────────────────────────────
Handshake RTTs      2                       1
Forward secrecy     Optional                Mandatory
0-RTT resumption    No                      Yes (with caveats)
Cipher suites       Many (some weak)        3 (all strong)
RSA key exchange    Supported (no FS)       Removed
Certificate         Unencrypted             Encrypted
Hello message       Unencrypted             Partially encrypted

TLS 1.2 is still common. TLS 1.3 is the current standard.
TLS 1.0 and 1.1 are deprecated (disabled in all modern browsers).

Your Nginx should specify:
  ssl_protocols TLSv1.2 TLSv1.3;
  (drop TLSv1.0 and TLSv1.1)
```

### 0-RTT Resumption in TLS 1.3

TLS 1.3 supports 0-RTT (zero round trip time resumption) for returning clients.

```
Normal TLS 1.3: 1 RTT before data
0-RTT: client sends encrypted data in the FIRST packet (before handshake completes)

How:
  Server gives client a "session ticket" after a connection.
  Next connection: client sends data encrypted with ticket key immediately.
  Server can decrypt and process before handshake completes.

Tradeoff:
  0-RTT data is not forward secret (tied to ticket key, not fresh ephemeral)
  0-RTT data is replay-vulnerable:
    An attacker could record the first packet and replay it.
    The server would process the request again.
  
  Safe for: GET requests (idempotent)
  Dangerous for: POST/PUT/DELETE (not idempotent — replay = duplicate action)
  
  NestJS should have 0-RTT disabled for state-changing API endpoints,
  or use replay protection (anti-replay nonces).
```

---

## 8. SNI — How One Server Serves Many Domains

A single IP can host many HTTPS sites. But TLS needs to pick the right certificate before the HTTP request is parsed (which would reveal the hostname). This is the chicken-and-egg problem SNI solves.

```
Problem:
  Your VPS has IP 157.245.23.14
  It hosts: paideon.lk, api.paideon.lk, admin.paideon.lk
  Each needs its own certificate (or a wildcard cert).

  Normally: TLS handshake must start with the certificate.
  But: the server doesn't know which certificate to use yet
       (it doesn't know which domain the client wants).
  But: the HTTP Host header comes AFTER TLS is established.

SNI (Server Name Indication):
  Client includes the hostname in the ClientHello, IN PLAINTEXT.
  Before encryption begins.
  
  ClientHello includes: server_name: "paideon.lk"
  
  Nginx sees this → picks paideon.lk certificate → handshake proceeds.

Privacy implication:
  SNI is visible to anyone who can observe the TLS handshake.
  Your ISP sees which hostnames you connect to.
  
  Solution: Encrypted Client Hello (ECH) — TLS extension being rolled out.
  Encrypts the entire ClientHello (including SNI) using a public key
  published in DNS. Your ISP only sees the CDN domain, not the real hostname.
  Not yet universally supported (Cloudflare supports it).
```

---

## 9. HSTS — Forcing HTTPS

HSTS (HTTP Strict Transport Security) tells browsers to always use HTTPS, even if the user types `http://`.

```
Server sends:
  Strict-Transport-Security: max-age=31536000; includeSubDomains; preload

Browser stores this for max-age seconds (1 year = 31,536,000 seconds).
While stored:
  User types http://paideon.lk → browser converts to https:// BEFORE request
  No HTTP request is ever made.
  No opportunity for SSL stripping attack.
```

### HSTS Preloading

```
Even with HSTS, the FIRST visit over HTTP is vulnerable.
An attacker can strip HTTPS on that first visit.

HSTS Preload:
  Submit your domain to https://hstspreload.org
  Your domain gets added to a list baked into Chrome, Firefox, Safari, Edge
  From that point, browsers NEVER make HTTP requests to your domain
  Even on the absolute first visit to a fresh browser installation
  
  Requirements:
    → max-age ≥ 31,536,000 (1 year)
    → includeSubDomains
    → preload directive
    → All subdomains must also serve HTTPS
  
  Warning: Hard to reverse. Once preloaded, removing takes months.
  Only add Paideon to preload when you're confident all subdomains use HTTPS.
```

---

## 10. Certificate Pinning

Certificate pinning is the practice of hardcoding expected certificate information into a client application.

```
Normal HTTPS:
  Client trusts any certificate signed by any trusted CA.
  ~150 CAs are trusted. If any one of them is compromised, they could
  issue a fake certificate for paideon.lk.

Certificate pinning:
  Client says: "I will ONLY accept a certificate with this specific
               public key hash for paideon.lk."
  Even if an attacker gets a valid certificate from a CA,
  the pinned client rejects it.

Example (HTTP Public Key Pinning — deprecated):
  Public-Key-Pins: pin-sha256="base64=="; max-age=2592000

Modern approach — certificate transparency monitoring + CAA records:
  CAA record: "Only Let's Encrypt can issue certs for paideon.lk"
  Monitor CT logs for unexpected certificates.

For Paideon:
  Mobile apps: pin the certificate public key in the app binary.
  Web browsers: cannot pin (user must be able to install their own certs).
  
  Pinning in mobile apps prevents MITM attacks even with
  corporate proxies that install their own CA.

Danger:
  If you pin a certificate and the certificate expires or is replaced:
  → all pinned clients break until they're updated
  → pin a backup key too, always
```

---

## 11. Mixed Content — Why One HTTP Resource Breaks Everything

A page loaded over HTTPS with any resource (image, script, CSS) over HTTP is "mixed content."

```
https://paideon.lk/dashboard
  <script src="http://cdn.example.com/analytics.js">   ← ACTIVE mixed content
  <img src="http://legacy.cdn.com/logo.png">           ← PASSIVE mixed content

Active mixed content (scripts, stylesheets, iframes):
  Browser BLOCKS these entirely.
  A script over HTTP could be modified by a MITM to execute arbitrary code
  on your HTTPS page. Your HTTPS guarantees mean nothing.

Passive mixed content (images, audio, video):
  Browser LOADS these but shows a warning.
  An image can't execute code, but can be modified (injected with tracking pixels).
  Most browsers are moving toward blocking these too.

Fix:
  Use https:// or // for all resource URLs.
  Protocol-relative URL: //cdn.example.com/script.js
  → Uses http when page is http, https when page is https.
  Since pages should always be https now, just use https://.

Content-Security-Policy can upgrade all HTTP requests to HTTPS:
  Content-Security-Policy: upgrade-insecure-requests
  → Browser converts http:// to https:// for all subresource requests.
```

---

## 12. Let's Encrypt and Automatic Certificates

Let's Encrypt is a free, automated, open CA. It issues 90-day certificates for free, automated via ACME protocol.

```
How it works:

1. You run certbot (or Nginx plugin) on your server
2. certbot requests a certificate for paideon.lk from Let's Encrypt
3. Let's Encrypt challenges you to prove you control the domain:
   
   HTTP-01 challenge:
   Let's Encrypt: "Place this file at http://paideon.lk/.well-known/acme-challenge/TOKEN"
   certbot: creates the file
   Let's Encrypt: fetches it → verifies → issues certificate
   
   DNS-01 challenge:
   Let's Encrypt: "Create a TXT record: _acme-challenge.paideon.lk = VALUE"
   certbot: creates the DNS record via Cloudflare API
   Let's Encrypt: checks DNS → issues certificate
   (Required for wildcard certificates)

4. Certificate issued. 90-day validity.
5. certbot sets up a cron job to renew at 60 days (before expiry).

Nginx config:
  ssl_certificate     /etc/letsencrypt/live/paideon.lk/fullchain.pem;
  ssl_certificate_key /etc/letsencrypt/live/paideon.lk/privkey.pem;

fullchain.pem = your cert + intermediate cert (full chain)
privkey.pem   = your private key (keep this secret — never share)

Why 90 days?
  Short validity limits damage if a cert is mis-issued.
  Forces automation — you can't manually manage 90-day certs at scale.
  Incentivises automation, which is more reliable than human renewal anyway.
```

---

## 13. TLS in the Paideon Stack

```
Browser ──HTTPS──▶ Cloudflare ──HTTP or HTTPS──▶ Nginx ──HTTP──▶ NestJS/Next.js
                      │                             │
                    (TLS                          (TLS
                  terminated)               terminated here
                                            if not using CF)

Option A: Cloudflare handles TLS (recommended for Paideon)
  Browser ↔ Cloudflare: TLS (certificate from Cloudflare)
  Cloudflare ↔ VPS Nginx: HTTPS (Cloudflare's Origin certificate, or Let's Encrypt)
  Nginx ↔ NestJS: plain HTTP on localhost (private network, safe)
  
  Advantages:
    → DDoS protection, CDN, WAF
    → Free SSL certificates (Cloudflare manages them)
    → Easy configuration in Cloudflare dashboard

Option B: Direct TLS on Nginx (no Cloudflare, or Cloudflare passthrough)
  Browser ↔ Nginx: TLS (Let's Encrypt certificate via certbot)
  Nginx ↔ NestJS: plain HTTP on localhost (never exposed)
  
  Nginx TLS config:
  ──────────────────────────────────────────────
  server {
      listen 443 ssl http2;
      server_name paideon.lk www.paideon.lk;
      
      ssl_certificate     /etc/letsencrypt/live/paideon.lk/fullchain.pem;
      ssl_certificate_key /etc/letsencrypt/live/paideon.lk/privkey.pem;
      
      ssl_protocols TLSv1.2 TLSv1.3;
      ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:
                  ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:
                  ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305;
      ssl_prefer_server_ciphers off;
      
      # HSTS
      add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;
      
      # OCSP Stapling
      ssl_stapling on;
      ssl_stapling_verify on;
  }
  
  server {
      listen 80;
      server_name paideon.lk www.paideon.lk;
      return 301 https://$host$request_uri;  # Redirect HTTP → HTTPS
  }
  ──────────────────────────────────────────────

NestJS doesn't need to handle TLS.
Nginx does it. NestJS sees plain HTTP on port 3001 (localhost only).
```

---

_Next: [Part 04 — Cookies, Sessions & Browser Storage](./Part%2004%20-%20Cookies%2C%20Sessions%20%26%20Browser%20Storage.md)_