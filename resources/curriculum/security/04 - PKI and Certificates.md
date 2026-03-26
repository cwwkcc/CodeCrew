> When your browser shows a padlock next to a URL, it has verified that you're talking to the real server — not an impostor. That verification is powered by a system called Public Key Infrastructure (PKI). This covers how certificates work, how trust is established, and what the CA system actually is.

---

## Table of Contents

1. [The Problem PKI Solves](#1-the-problem-pki-solves)
2. [Digital Certificates — What They Are](#2-digital-certificates--what-they-are)
3. [Certificate Contents — X.509](#3-certificate-contents--x509)
4. [Certificate Authorities](#4-certificate-authorities)
5. [Certificate Chains](#5-certificate-chains)
6. [The Trust Store](#6-the-trust-store)
7. [How Your Browser Verifies a Certificate](#7-how-your-browser-verifies-a-certificate)
8. [Certificate Revocation](#8-certificate-revocation)
9. [Let's Encrypt and Automated Certificates](#9-lets-encrypt-and-automated-certificates)
10. [Certificate Types](#10-certificate-types)
11. [Certificate Pinning](#11-certificate-pinning)
12. [What Can Go Wrong](#12-what-can-go-wrong)

---

## 1. The Problem PKI Solves

Asymmetric encryption requires you to have someone's public key to communicate securely with them. But how do you know a public key actually belongs to who you think it does?

```
The MITM (Man-in-the-Middle) Problem:

Without verification:
  You want to connect to bank.com
  Attacker intercepts your connection
  Attacker sends you their own public key, claiming to be bank.com
  You encrypt with attacker's key → attacker decrypts, reads, re-encrypts, forwards
  Everything looks normal — you're "talking to bank.com" — but attacker sees everything

The question: how do you know the public key you receive is REALLY bank.com's?
```

PKI solves this by introducing a trusted third party — a **Certificate Authority (CA)** — that vouches for the ownership of public keys.

---

## 2. Digital Certificates — What They Are

A **digital certificate** is a document that binds a public key to an identity (a domain name, organization, person) and is signed by a Certificate Authority.

```
Certificate:
  "The public key [abc...] belongs to bank.com"
  Signed by: TrustableCA

You verify:
  1. Is the certificate for bank.com? → yes
  2. Is the CA's signature valid? → yes
  3. Do I trust TrustableCA? → yes (it's in my trust store)
  → Therefore: the public key [abc...] really belongs to bank.com
```

The CA's signature is what you actually verify. You don't independently verify that bank.com owns the key — you trust that the CA did that verification before signing the certificate.

---

## 3. Certificate Contents — X.509

The standard format for certificates is X.509. Every TLS certificate is an X.509 certificate.

```
Subject:
  Common Name (CN):     bank.com
  Organization (O):     Bank Corporation
  Country (C):          US
  State (ST):           California

Subject Alternative Names (SANs):
  DNS: bank.com
  DNS: www.bank.com
  DNS: api.bank.com

Public Key:
  Algorithm:  ECDSA with P-256  (or RSA-2048)
  Key:        [the actual public key bytes]

Validity:
  Not Before: 2025-01-01
  Not After:  2025-04-01  (Let's Encrypt certificates are 90 days)
              or 2027-01-01 (commercially issued — up to 2 years)

Issuer:
  CN: Let's Encrypt R11
  O:  Let's Encrypt

Serial Number: 04:a1:b2:c3:...  (unique per CA)

Signature Algorithm: SHA-256 with ECDSA
Signature: [the CA's signature over all the above]

Extensions:
  Key Usage:             Digital Signature, Key Encipherment
  Extended Key Usage:    TLS Web Server Authentication
  Basic Constraints:     CA: FALSE  (this is not itself a CA)
  CRL Distribution:      http://crl.letsencrypt.org/...
  OCSP:                  http://r11.o.letsencrypt.org
```

### Subject Alternative Names

The CN (Common Name) is legacy. Modern certificates use **SANs** for domain matching. A certificate can cover multiple domains:

```
SANs:
  DNS: example.com
  DNS: www.example.com
  DNS: api.example.com
  DNS: *.example.com     ← wildcard (covers one level of subdomains)

Wildcard *.example.com matches:
  → anything.example.com   ✅
  → mail.example.com       ✅
  → sub.anything.example.com  ❌ (only one level)
  → example.com             ❌ (wildcard doesn't match the apex domain)
```

---

## 4. Certificate Authorities

A **Certificate Authority (CA)** is an organization that:

1. Verifies the identity of certificate requesters
2. Signs certificates for verified requesters
3. Maintains revocation lists for compromised certificates

### Levels of Verification

```
Domain Validation (DV):
  → CA verifies you control the domain (via DNS record or file challenge)
  → Fast (minutes to hours), automated
  → What Let's Encrypt issues
  → Browser shows: 🔒 (padlock)
  → Sufficient for most websites

Organization Validation (OV):
  → CA also verifies the organization exists (business records, etc.)
  → Manual process, days to weeks
  → Browser shows: 🔒 + organization name in cert details
  → Used by businesses wanting to show their name

Extended Validation (EV):
  → Most thorough — legal entity verification, physical address, etc.
  → Was once shown with a green bar and company name in browsers
  → Modern browsers removed the visual distinction (EV = no more visible benefit)
  → Rarely worth the extra cost and process today
```

### Major CAs

```
Let's Encrypt     → Free, automated, DV only, 90-day certs, huge adoption
DigiCert          → Commercial, all types, widely trusted
Sectigo (Comodo)  → Commercial, widely trusted
GlobalSign        → Commercial, enterprise focus
Google Trust      → Google's CA, used for Google properties
ISRG (Let's Encrypt parent) → Non-profit, free TLS for the web
```

There are about 50-100 CAs whose root certificates are trusted by browsers and operating systems. Each one is a potential trust anchor.

---

## 5. Certificate Chains

Individual server certificates are not signed directly by the CA's root certificate. They're signed by an **intermediate certificate**, which is signed by the root.

```
Root CA Certificate
  └── Intermediate CA Certificate
       └── Server Certificate (your website)
```

Why intermediates?

```
Root CA private key:
  → Stored offline in a hardware security module (HSM)
  → Air-gapped — no network connection
  → If compromised: every certificate it ever signed is untrusted
                    rebuilding trust takes years
  → Used only to sign intermediate CAs (rare, ceremonial)

Intermediate CA private key:
  → Online, used for day-to-day certificate signing
  → If compromised: revoke the intermediate, issue new one
                    limited blast radius
  → Root still trusted → rebuild quickly
```

### Chain Verification

When your server presents a certificate, it should also send the intermediate certificates. Browsers need the full chain to verify back to a trusted root.

```
Browser receives:
  [Server Cert] ← signed by →  [Intermediate CA]
  [Intermediate CA] ← signed by → [Root CA]

Browser has [Root CA] in its trust store.

Verification:
  1. Verify server cert signature using intermediate CA's public key → valid
  2. Verify intermediate CA signature using root CA's public key → valid
  3. Root CA is in trust store → trusted
  → Connection is trusted
```

If the server doesn't send the intermediate certificate, browsers may fail to verify the chain ("incomplete certificate chain" errors), even though the server cert itself is valid.

---

## 6. The Trust Store

A **trust store** is a list of trusted root CA certificates maintained by:

- Operating systems (Windows, macOS, Linux/Mozilla NSS)
- Browsers (Chrome uses Chrome Root Store, Firefox uses its own)
- Application runtimes (Node.js bundles a list)

```
When you install Windows or macOS, it ships with ~150-200 trusted root certificates.
Your browser trusts any certificate signed (directly or via intermediates) by any of these roots.

This is the foundation of web trust:
  → You trust Microsoft/Apple/Mozilla to curate the root list
  → CAs in the list have agreed to audits (WebTrust, ETSI)
  → Bad CAs get removed (DigiNotar was removed after being hacked in 2011)
```

### Node.js Trust Store

Node.js bundles Mozilla's NSS root store. When making HTTPS requests:

```js
// Node.js uses the bundled cert store automatically
const https = require("https");
https.get("https://example.com", (res) => { ... });
// This verifies the cert chain against Mozilla's root store

// Add custom CA (e.g., internal CA for development)
const agent = new https.Agent({
  ca: fs.readFileSync("path/to/internal-ca.crt"),
});
```

---

## 7. How Your Browser Verifies a Certificate

Full flow of what happens when you visit `https://example.com`:

```
1. Browser connects to example.com:443 (TCP)

2. TLS handshake begins
   Browser sends: "Hello, I support TLS 1.3, here are my cipher suites"
   Server sends:  "Hello, let's use TLS_AES_256_GCM_SHA384"
   Server sends:  [Server Certificate] + [Intermediate Certificate]

3. Browser receives the certificate chain

4. Certificate validation:
   a. Check: is the hostname in the cert's SANs?
      → cert covers example.com? → yes
   b. Check: is the current date between NotBefore and NotAfter?
      → yes
   c. Check: is the issuer's signature valid?
      → verify server cert signature using intermediate CA's public key → valid
   d. Check: is the intermediate's signature valid?
      → verify intermediate signature using root CA's public key → valid
   e. Check: is the root CA in the trust store?
      → yes
   f. Check: has any cert in the chain been revoked? (OCSP/CRL)
      → checking...

5. All checks pass → TLS handshake continues → connection is encrypted

6. Browser shows 🔒 padlock
```

### What the Padlock Means (And Doesn't Mean)

```
Padlock means:
  ✅ Connection is encrypted (attacker can't read traffic)
  ✅ Certificate is valid and signed by a trusted CA
  ✅ Certificate is for this domain

Padlock does NOT mean:
  ❌ The website is safe or trustworthy
  ❌ The website won't try to scam you
  ❌ The organization behind the website is legitimate
  
Phishers use HTTPS too. Let's Encrypt issues certs to anyone who controls a domain.
"bank0f-america.com" can have a valid padlock.
The padlock only guarantees you're talking to the owner of that specific domain.
```

---

## 8. Certificate Revocation

A certificate may need to be revoked before it expires — if the private key was stolen, if the organization changed, or if the cert was issued incorrectly.

### CRL — Certificate Revocation List

A list of revoked certificate serial numbers, published by the CA and signed. Browsers download periodically.

```
Problems:
  → Lists can be large (MBs for major CAs)
  → Cached — may be hours or days out of date
  → If the CRL server is down, browser may "fail open" (accept the cert anyway)
```

### OCSP — Online Certificate Status Protocol

Real-time query to the CA: "Is certificate serial #abc123 still valid?"

```
OCSP request:  → CA's OCSP server
OCSP response: { status: "good" / "revoked" / "unknown" }

Problems:
  → Privacy: CA learns every site you visit (request reveals the cert you're checking)
  → Performance: adds a round-trip to every TLS connection
  → Availability: if OCSP server is down → fail open → revocation not checked
```

### OCSP Stapling

The **server** queries its own cert's OCSP status and includes the signed response in the TLS handshake. Browser gets revocation status without contacting the CA.

```
Benefits:
  → No privacy leak (browser doesn't contact CA)
  → Faster (no extra round-trip)
  → Cached OCSP response pre-fetched by server (usually valid 24 hours)
```

This is configured in Nginx:

```nginx
ssl_stapling on;
ssl_stapling_verify on;
ssl_trusted_certificate /path/to/chain.pem;
resolver 1.1.1.1;  # DNS resolver for fetching OCSP responses
```

### The Hard Problem

Revocation is genuinely unsolved in PKI. If a CA is compromised and certificates are revoked, there's no guarantee clients will check revocation before the cert naturally expires. This is why:

- Certificate lifetimes have shortened (90 days with Let's Encrypt)
- Certificate Transparency (CT) logs help detect mis-issued certs quickly
- Browsers are moving toward shorter validity periods

---

## 9. Let's Encrypt and Automated Certificates

Let's Encrypt (2015) made HTTPS free and automatic. Before it, certificates cost $50-$300/year and required manual processes.

### The ACME Protocol

ACME (Automatic Certificate Management Environment) is the protocol Let's Encrypt uses for automated certificate issuance.

**How domain control verification works:**

```
HTTP-01 challenge:
  1. You request a cert for example.com
  2. Let's Encrypt gives you a token: "xK8Jz2..."
  3. You serve it at: http://example.com/.well-known/acme-challenge/xK8Jz2...
  4. Let's Encrypt fetches that URL
  5. Token matches → you control the domain → cert issued

DNS-01 challenge:
  1. You request a cert for example.com
  2. Let's Encrypt gives you a token
  3. You create DNS TXT record: _acme-challenge.example.com = <token>
  4. Let's Encrypt queries DNS
  5. Record matches → cert issued
  → Required for wildcard certs (*.example.com)
  → Required if port 80 is blocked
```

### Certbot / Automated Renewal

```bash
# Install certbot and get a cert
certbot --nginx -d example.com -d www.example.com

# Certbot modifies Nginx config automatically
# Cron job runs twice daily to check expiry
# If < 30 days remaining → auto-renew

# Verify cron
systemctl list-timers | grep certbot
```

Let's Encrypt certificates are 90 days. The short lifetime:

1. Limits damage from key compromise
2. Forces automation (manual renewal of 90-day certs is unsustainable)
3. Acts as a forcing function for keeping infrastructure maintained

---

## 10. Certificate Types

### Single-Domain Certificates

```
Covers: example.com (and usually www.example.com via SAN)
Use: Standard — most websites
```

### Wildcard Certificates

```
Covers: *.example.com
Matches: api.example.com, mail.example.com, etc.
Does NOT match: example.com (apex) or sub.sub.example.com
Use: When you have many subdomains
Requires DNS-01 challenge for Let's Encrypt
```

### Multi-Domain (SAN) Certificates

```
Covers: example.com, example.org, api.example.com, anotherdomain.com
Use: When you have multiple unrelated domains on one server
```

### Self-Signed Certificates

```
Signed by yourself (no CA)
Not trusted by browsers (red warning page)
Uses: Internal development, internal services where you control the trust store
      Docker containers talking to each other internally
```

```bash
# Generate self-signed cert for development
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem \
  -days 365 -nodes -subj "/CN=localhost" \
  -addext "subjectAltName=DNS:localhost,IP:127.0.0.1"
```

---

## 11. Certificate Pinning

Certificate pinning means hardcoding the expected certificate (or its public key hash) into the client application. Even if an attacker has a CA-signed certificate for your domain, the client rejects it if it doesn't match the pinned value.

```
Without pinning:
  Attacker compromises a CA → gets cert for api.yourapp.com
  MITM attack → client trusts it (it's CA-signed) → traffic intercepted

With pinning:
  Client expects: SHA-256(public key) = "abc123..."
  Attacker cert: SHA-256(different public key) = "xyz789..."
  → Client rejects connection even though cert is CA-signed
```

### Where Pinning Is Used

```
Mobile apps → hard-code server cert hash in app binary
              → protects against network-level MITM
              → even corporate HTTPS inspection proxies are rejected

Browsers → HPKP (HTTP Public Key Pinning) header
           → Deprecated in 2018 (too easy to permanently brick a site)
           → Chrome removed support

Alternative → Certificate Transparency (CT) logs
              → All certs must be logged publicly
              → Clients can detect rogue certs for their domain
```

### Pinning in Node.js (For Internal Service Communication)

```js
import https from "https";
import fs from "fs";
import crypto from "crypto";

const expectedPin = "sha256//AAAA..."; // SHA-256 of cert's public key, base64

const agent = new https.Agent({
  checkServerIdentity: (host, cert) => {
    const publicKey = cert.pubkey;
    const pin = "sha256//" + crypto
      .createHash("sha256")
      .update(publicKey)
      .digest("base64");
    
    if (pin !== expectedPin) {
      throw new Error(`Certificate pin mismatch for ${host}`);
    }
    // Return undefined = no error
  },
});

https.get({ hostname: "internal-service", agent }, (res) => { ... });
```

---

## 12. What Can Go Wrong

### CA Compromise

DigiNotar (2011): Dutch CA completely compromised. Attackers issued certs for google.com and others. Used for Iranian government surveillance. DigiNotar removed from all trust stores → company went bankrupt.

**Lesson:** The entire web PKI depends on every CA in the trust store being uncompromised. That's ~100+ organizations globally. The weakest link determines security.

### Certificate Transparency (CT) Logs

Response to CA compromise risk: **Certificate Transparency** requires CAs to log every issued certificate to public, append-only, auditable logs.

```
When a cert is issued:
  1. CA submits cert to CT logs (Merkle tree structure)
  2. Log returns a signed timestamp (SCT)
  3. SCT embedded in the cert

When browser connects:
  1. Checks that cert has valid SCTs
  2. Means the cert is publicly logged
  3. Domain owners can monitor CT logs for unauthorized certs
```

This doesn't prevent mis-issuance, but it makes it detectable quickly.

### Key Compromise

If your private key leaks:

1. Immediately request certificate revocation from your CA
2. Generate new key pair
3. Request new certificate
4. Deploy new cert
5. Investigate how the key leaked

Private keys should never be:

- Committed to version control
- Transmitted over unencrypted channels
- Stored in world-readable files

```bash
# Check private key permissions
ls -la private.key
# Should be: -rw------- (600) — owner read only
chmod 600 private.key
```

### Expired Certificates

Expired certificates cause browser warnings and connection failures. Solution: automated renewal (Certbot or Cloudflare handles this).

```bash
# Check expiry
openssl x509 -in cert.pem -noout -dates
# notBefore=Jan  1 00:00:00 2025 GMT
# notAfter=Apr  1 00:00:00 2025 GMT

# Check expiry of a live domain
echo | openssl s_client -connect example.com:443 2>/dev/null | openssl x509 -noout -dates
```

Set up monitoring: alert when cert expires in < 30 days.

---

## Summary

```
PKI solves the public key distribution problem:
  Without PKI: no way to know if a public key is genuine
  With PKI: CA signs a certificate binding a public key to a domain
  You trust the CA → you trust the binding

X.509 certificate contains:
  Subject (domain), SANs, public key, validity period, CA signature

Certificate chain:
  Server cert ← signed by intermediate ← signed by root
  Root is in your trust store
  Intermediates protect the root key (offline, air-gapped)

Trust store:
  ~150 root CAs trusted by OSes/browsers
  Every one is a potential trust anchor
  Compromise of one = compromise of global web trust for that CA

Verification steps (every HTTPS connection):
  1. Domain matches SANs
  2. Date within validity period
  3. Signature chain valid up to trusted root
  4. Certificate not revoked (OCSP/CRL)

Let's Encrypt:
  Free, automated, 90-day certs
  ACME protocol (HTTP-01 or DNS-01 challenge)
  Auto-renewal via certbot/ACME client

Certificate pinning:
  Hardcode expected cert/pubkey hash in client
  Defeats rogue CA attacks
  Complex to manage — use CT logs as alternative

Certificate Transparency:
  All certs logged publicly (append-only)
  Makes mis-issuance detectable
  Required by Chrome since 2018
```

---

_Next: [05 — TLS Deep Dive](./05-tls-deep-dive.md)_