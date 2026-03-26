> DNS translates domain names into IP addresses. It's the phone book of the internet. And historically, it was designed with zero security. This covers DNS attacks, DNSSEC, and the domain-level defenses that protect your application before a packet even reaches your server.

---

## Table of Contents

1. [How DNS Works](#1-how-dns-works)
2. [DNS Attack Surface](#2-dns-attack-surface)
3. [DNS Cache Poisoning](#3-dns-cache-poisoning)
4. [DNS Hijacking](#4-dns-hijacking)
5. [DNS Spoofing / MITM](#5-dns-spoofing--mitm)
6. [BGP Hijacking](#6-bgp-hijacking)
7. [DNSSEC](#7-dnssec)
8. [DNS over HTTPS (DoH) and DNS over TLS (DoT)](#8-dns-over-https-doh-and-dns-over-tls-dot)
9. [Domain Security Practices](#9-domain-security-practices)
10. [Subdomain Takeover](#10-subdomain-takeover)

---

## 1. How DNS Works

```
You type: api.example.com

1. Check local DNS cache → not found

2. Ask recursive resolver (your ISP's or 8.8.8.8 or 1.1.1.1)
   "What is the IP for api.example.com?"

3. Resolver doesn't know → asks root nameservers
   Root: "I don't know api.example.com, but for .com, ask these servers"

4. Resolver asks .com TLD nameservers
   TLD: "I don't know api.example.com, but for example.com, ask these servers"

5. Resolver asks example.com's authoritative nameserver
   Auth NS: "api.example.com is at IP 203.0.113.10, TTL 300"

6. Resolver caches the answer (for TTL seconds = 300 = 5 minutes)
   Returns 203.0.113.10 to your computer

7. Your browser connects to 203.0.113.10:443

All of this happens in milliseconds. This entire chain was historically unencrypted and unauthenticated.
```

### DNS Record Types

```
A       → domain → IPv4 address
AAAA    → domain → IPv6 address
CNAME   → domain → another domain (alias)
MX      → domain → mail server
TXT     → arbitrary text (used for: SPF, DKIM, DMARC, domain verification)
NS      → nameserver for a domain
CAA     → which CAs are allowed to issue certs for this domain
PTR     → IP → domain (reverse lookup)
SOA     → Start of Authority — zone metadata
SRV     → service location records
```

---

## 2. DNS Attack Surface

DNS is a rich attack surface because:

1. Historically no authentication — any resolver could return any answer
2. UDP-based — easy to spoof source IPs
3. Caching — poisoned answers persist for the TTL duration
4. Hierarchical — compromise any level → control responses for everything below

---

## 3. DNS Cache Poisoning

**Goal:** Inject a fake DNS record into a resolver's cache, causing users to be directed to attacker-controlled IPs.

```
Attack (Kaminsky attack, 2008):
  1. Attacker sends forged DNS response to a resolver
  2. Response claims: api.example.com → 1.2.3.4 (attacker's server)
  3. If the forged response arrives before the legitimate one → cached
  4. All users using that resolver → directed to attacker's server for TTL seconds

The trick: DNS queries include a 16-bit transaction ID.
           Attacker floods responses with all 65,536 possible IDs.
           One of them matches → cache poisoned.
```

**Mitigation:**

- **Source port randomization:** Resolver uses a random source port per query (adds ~16 bits of entropy on top of the 16-bit transaction ID → 32 bits total)
- **DNSSEC:** Cryptographic signatures on DNS records — forged records can't pass verification
- **0x20 encoding:** Randomize case in query (dNs.ExAmPlE.CoM) — legitimate resolver echoes the case back; attacker's forged response can't predict the case

---

## 4. DNS Hijacking

DNS hijacking means changing the legitimate DNS records — through your domain registrar or DNS host.

```
Attack vectors:
  → Compromise the DNS registrar account (weak password, no 2FA)
  → Compromise the DNS hosting provider account
  → Social engineering the registrar's support team
  → Exploit vulnerabilities in the registrar's web interface

Effect:
  Attacker changes A records → all users sent to attacker's server
  Attacker changes MX records → all incoming email routed to attacker
  Attacker adds TXT records → passes SPF/DKIM verification for phishing

Real examples:
  → Malaysian Airlines website hijacked (2015)
  → Brazilian bank DNS hijacked (2017) — all bank traffic redirected for hours
```

**Defenses:**

- Enable 2FA on domain registrar and DNS hosting accounts
- Use registrar lock (also called domain lock or transfer lock) — prevents unauthorized transfers
- Enable DNSSEC — changing DNS records without the signing key won't be trusted
- Monitor DNS records for unauthorized changes
- Use a reputable, security-focused registrar

---

## 5. DNS Spoofing / MITM

```
On a local network (coffee shop WiFi):
  1. Attacker is on the same network
  2. User's DNS queries are unencrypted (plain UDP)
  3. Attacker intercepts queries, responds with fake answers
  4. User connects to attacker's server instead of intended destination

Combined with SSL stripping:
  Attacker intercepts HTTP requests → serves fake site
  If user types HTTP, attacker never lets them reach HTTPS
```

**Defenses:**

- DNS over HTTPS (DoH) or DNS over TLS (DoT) — encrypts DNS queries
- HTTPS everywhere — even with spoofed DNS, TLS cert verification catches it
- Certificate Transparency — attacker needs a valid cert to complete the MITM, and any issued cert is logged

---

## 6. BGP Hijacking

BGP (Border Gateway Protocol) is how internet routers know which networks are reachable via which paths. It was designed in the 1980s with no authentication.

```
Attack:
  1. Attacker announces BGP routes claiming to own IP blocks they don't own
  2. Other routers believe them (BGP trusts announcements)
  3. Traffic destined for the victim's IPs is routed to the attacker
  4. Attacker sees all that traffic — including TLS handshakes

Notable examples:
  → Amazon Route53 BGP hijack (2018) — cryptocurrency theft
  → Pakistan Telecom knocked YouTube offline (2008) by accidentally announcing YouTube's IP block
  → Russian ISP redirected traffic from major financial institutions through Russia (2017)
```

BGP hijacking combined with a compromised CA = MITM on HTTPS.

**Mitigation:**

- RPKI (Resource Public Key Infrastructure) — cryptographic validation of BGP route origins
- Not something application developers control — this is ISP/network level
- As an application developer: DNSSEC + CT logs are your defenses

---

## 7. DNSSEC

DNSSEC (DNS Security Extensions) adds cryptographic signatures to DNS records. A resolver can verify that a DNS response is authentic and hasn't been tampered with.

```
How it works:
  1. Zone owner generates a key pair (Zone Signing Key)
  2. All DNS records are signed with the private key
  3. The public key is published as a DNSKEY record
  4. Parent zone signs the child's DNSKEY (chain of trust)
  5. Root zone is the ultimate trust anchor (managed by IANA)

Verification:
  Resolver receives DNS response
  → Checks signature on the record using published DNSKEY
  → Checks that DNSKEY is signed by parent zone
  → Chains up to root zone
  → Valid chain → response is authentic

Forged response:
  → Signature won't validate (attacker doesn't have private key)
  → Resolver rejects the response → SERVFAIL
```

### DNSSEC Limitations

- Complex to implement and manage
- Only ~30% of domains are signed (2024)
- Amplification attack risk (DNSSEC responses are large — can be used in DDoS)
- Does not encrypt DNS queries — still visible to network observers
- Requires resolver support

### Enabling DNSSEC

Done at the registrar level (DS record delegated to parent zone) and DNS hosting provider (zone signing). Most modern managed DNS providers (Cloudflare, Route53) support it with one click.

---

## 8. DNS over HTTPS (DoH) and DNS over TLS (DoT)

These protocols encrypt DNS queries, preventing network observers from seeing which domains you're querying.

```
Traditional DNS:
  Query:    UDP port 53, plaintext
  Visible:  Which domains you're looking up — your ISP sees everything

DNS over TLS (DoT):
  TCP port 853, TLS encrypted
  Resolver must support DoT (e.g., 1.1.1.1, 8.8.8.8)
  ISP can see you're making DNS queries (TLS handshake) but not the content

DNS over HTTPS (DoH):
  HTTPS port 443, TLS encrypted
  Looks like regular HTTPS traffic — ISP can't distinguish from web browsing
  Harder to block
  Supported by major browsers (Firefox, Chrome) built-in
```

Neither prevents a compromised DNS resolver from lying to you. They only prevent network-level eavesdropping on your queries. DNSSEC is still needed for record authenticity.

---

## 9. Domain Security Practices

### CAA Records — Restrict Certificate Issuance

```
; Only Let's Encrypt can issue certs for example.com
example.com. CAA 0 issue "letsencrypt.org"
example.com. CAA 0 issuewild ";"  ; disallow wildcard certs from anyone
example.com. CAA 0 iodef "mailto:security@example.com"  ; notify on violation attempts
```

Even if an attacker social-engineers a CA into issuing a cert for your domain, a CA that respects CAA records will check first and refuse if you're not in the list.

### Registrar Lock

Enables a lock on your domain at the registry level. Prevents unauthorized transfers or NS changes. Must be unlocked (with authentication) before any changes.

```
Registrar Lock (also: ClientTransferProhibited, ClientUpdateProhibited):
  → Prevents: domain transfer to another registrar without your explicit action
  → Prevents: NS record changes without unlocking
  → Standard protection — enable for all production domains
```

### Domain Monitoring

```
Monitor for:
  → Unauthorized DNS record changes
  → New certificates issued for your domain (CT log alerts)
  → Domains similar to yours (typosquatting) that could be used for phishing
    (example.corn, examp1e.com, exаmple.com with Cyrillic 'a')

Tools:
  → Facebook's CT monitoring: developers.facebook.com/tools/ct
  → certspotter.com (CT log monitoring)
  → DNStwist (typosquatting detection)
```

---

## 10. Subdomain Takeover

A subdomain takeover occurs when a DNS record points to a service you no longer control, and an attacker claims that service.

```
Scenario:
  Your DNS: staging.example.com CNAME → myapp.herokuapp.com
  You delete the Heroku app (staging.myapp.herokuapp.com no longer exists)
  But forget to delete the DNS record

  Attacker:
    → Creates a Heroku app with the name "myapp"
    → Now staging.example.com → attacker's Heroku app
    → Attacker controls staging.example.com

Attack impact:
  → Serve malicious content under your domain (phishing users of your domain)
  → Get TLS certificate for staging.example.com (it's a valid Let's Encrypt domain)
  → Access cookies scoped to *.example.com
  → Bypass CORS if staging.example.com is whitelisted
```

### Services Commonly Vulnerable to Subdomain Takeover

```
Heroku, GitHub Pages, Azure, AWS S3/CloudFront, Fastly, Shopify, Surge.sh, etc.
Any service where you get a subdomain that you can "claim" by creating an account
```

### Prevention

```
1. When decommissioning a service → delete DNS records FIRST, then the service
   (not the other way around)

2. Audit DNS records regularly:
   → Does every CNAME point to a still-active service?
   → Does every A record point to a still-owned IP?

3. Use tools like:
   → can-i-take-over-xyz (GitHub list of vulnerable services)
   → subjack, subfinder + nuclei templates for subdomain takeover detection
```

---

## Summary

```
DNS translates domains to IPs — historically no security

Cache poisoning: inject fake records into resolver cache
  → Mitigated by: source port randomization, DNSSEC

DNS hijacking: compromise registrar/DNS host account
  → Mitigated by: 2FA on registrar, domain lock, DNSSEC

DNS spoofing: network-level interception
  → Mitigated by: DoH/DoT (encrypts queries), HTTPS + TLS cert verification

BGP hijacking: announce someone else's IP block
  → Application-level mitigation: DNSSEC + CT logs

DNSSEC: cryptographic signatures on DNS records
  → Detects tampered/forged responses
  → ~30% adoption — not universal

DoH/DoT: encrypts DNS queries from observers
  → Doesn't authenticate records (still need DNSSEC)

Domain hardening:
  → CAA records: restrict which CAs can issue certs
  → Registrar lock: prevent unauthorized transfers
  → Monitor CT logs for unauthorized certificate issuance

Subdomain takeover:
  → DNS points to service you no longer control
  → Attacker claims the service → controls your subdomain
  → Always delete DNS records BEFORE decommissioning services
```

---

_Next: [07 — HTTP Security Headers](./07-http-security-headers.md)_