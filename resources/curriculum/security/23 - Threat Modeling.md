> Threat modeling is a structured way of thinking about what can go wrong before you build, rather than discovering it after a breach. It forces you to think like an attacker — systematically — so you know where to invest your security effort.

---

## Table of Contents

1. [What Threat Modeling Is](#1-what-threat-modeling-is)
2. [When to Do It](#2-when-to-do-it)
3. [The STRIDE Framework](#3-the-stride-framework)
4. [Attack Trees](#4-attack-trees)
5. [Data Flow Diagrams](#5-data-flow-diagrams)
6. [Trust Boundaries](#6-trust-boundaries)
7. [PASTA — Another Approach](#7-pasta--another-approach)
8. [Prioritizing Threats — DREAD](#8-prioritizing-threats--dread)
9. [Threat Modeling in Practice](#9-threat-modeling-in-practice)

---

## 1. What Threat Modeling Is

Threat modeling answers four questions:

1. **What are we building?** (the system and its components)
2. **What can go wrong?** (threats)
3. **What are we going to do about it?** (mitigations)
4. **Did we do a good job?** (validation)

```
Without threat modeling:
  Build system → ship it → breach happens → patch → repeat
  Reactive, expensive, damage already done

With threat modeling:
  Think about attacks before building
  Design mitigations in from the start
  Cheaper, proactive, better coverage
```

Threat modeling doesn't eliminate all vulnerabilities — it systematically reduces the ones you know about before they're exploitable.

---

## 2. When to Do It

```
New feature design:
  Before writing code for a new authentication flow, file upload, or payment integration
  Cheapest time to change the design

System architecture:
  When introducing a new service, database, or external integration
  When moving from monolith to microservices

Before launch:
  Final review of the full system attack surface
  Check: does the implementation match the threat model?

After incidents:
  What did the threat model miss?
  Update it to prevent recurrence
```

A threat model doesn't have to be a long document. A one-hour whiteboard session asking "what could an attacker do here?" is better than nothing.

---

## 3. The STRIDE Framework

STRIDE is a mnemonic for six categories of threats. Developed at Microsoft in the late 1990s. Apply each category to every component in your system.

```
S — Spoofing
T — Tampering
R — Repudiation
I — Information Disclosure
D — Denial of Service
E — Elevation of Privilege
```

### S — Spoofing Identity

Pretending to be someone or something else.

```
Threats:
  User spoofing:      attacker logs in as another user (credential theft, token theft)
  Service spoofing:   attacker's server impersonates your backend
  IP spoofing:        attacker fakes source IP in logs

Mitigations:
  Strong authentication (MFA, short-lived tokens)
  TLS certificate validation (prevents service spoofing)
  Don't rely on IP as identity (easily spoofed)
```

### T — Tampering

Modifying data without authorization.

```
Threats:
  Database: attacker modifies records directly (SQL injection, compromised DB)
  In transit: MITM modifies HTTP responses or API requests
  JWT: attacker modifies the payload (without valid signature)
  File: uploaded file is replaced with malicious version

Mitigations:
  Input validation and parameterized queries
  TLS for all data in transit (AEAD — integrity built in)
  JWT signature verification
  File integrity checks (hash on upload, verify on serve)
```

### R — Repudiation

Denying having performed an action.

```
Threats:
  User denies modifying a record ("I never changed that grade")
  Admin denies granting a role ("I never made that account an admin")
  No audit trail to prove otherwise

Mitigations:
  Audit log: who did what, when, from where
  Immutable logs (append-only, not editable by the logged user)
  Digital signatures for critical operations
  Non-repudiation requires: who (authenticated identity) + what (action) + when (timestamp)
```

### I — Information Disclosure

Exposing data to unauthorized parties.

```
Threats:
  Data breach: attacker reads database (SQL injection, compromised credentials)
  Verbose errors: stack traces reveal server structure, DB schema
  API over-exposure: endpoint returns more fields than needed
  Log leakage: passwords or tokens logged accidentally

Mitigations:
  Principle of least privilege (users only see their data)
  Generic error messages to users; detailed logs server-side only
  DTO response shaping (strip sensitive fields)
  Never log passwords, tokens, or PII
```

### D — Denial of Service

Making the system unavailable.

```
Threats:
  Network DDoS: volumetric attack floods bandwidth
  Application DoS: computationally expensive requests (regex, hashing)
  Resource exhaustion: too many open connections, file handles, memory
  Login flooding: fills rate limit, blocks legitimate users

Mitigations:
  Rate limiting (all endpoints, especially auth)
  CDN + DDoS protection (Cloudflare, AWS Shield)
  Input size limits (max payload size)
  Timeout on slow requests
  Efficient algorithms (avoid O(n²) on user-controlled input sizes)
```

### E — Elevation of Privilege

Gaining permissions beyond what was granted.

```
Threats:
  Horizontal escalation: student A views student B's data (IDOR)
  Vertical escalation: student gains teacher/admin permissions
  Privilege abuse: admin uses permissions for unauthorized purposes

Mitigations:
  RBAC/ABAC enforced server-side
  Ownership checks on every resource access
  Principle of least privilege for roles and API keys
  Step-up authentication for sensitive operations (health data)
```

---

## 4. Attack Trees

An attack tree represents the structure of an attack: the root is the attacker's goal, branches are methods to achieve it.

```
Goal: Access student health records without authorization
│
├── Compromise an admin account
│   ├── Credential stuffing (reused passwords from breaches)
│   ├── Phishing attack on admin
│   └── Brute force admin password
│
├── Exploit a vulnerability in the health records endpoint
│   ├── SQL injection → direct DB access
│   ├── IDOR → access another student's record by ID
│   └── Bypass authorization check (missing guard)
│
├── Compromise the database directly
│   ├── Credential theft (DB password in code repository)
│   ├── Exposed DB port (misconfiguration)
│   └── Supply chain attack (compromised dependency)
│
└── Insider threat
    ├── Admin views records outside their role
    └── Unauthorized export to external storage
```

Attack trees help you see which paths are most likely and which mitigations cut off the most branches.

**Where to focus:** Branches that require the least attacker skill/resources are highest priority. Credential stuffing on admin accounts requires no technical skill — mitigate first with MFA and breach-password checking.

---

## 5. Data Flow Diagrams

A Data Flow Diagram (DFD) maps how data moves through your system. It's the foundation for identifying where threats apply.

```
Components:
  External entities:  users, external services (browser, mobile app, third-party APIs)
  Processes:          your application code that transforms data
  Data stores:        databases, caches, file systems
  Data flows:         arrows showing data movement

Example (simplified school system DFD):

  [Browser] ──HTTP──► [NestJS API] ──Prisma──► [PostgreSQL]
                             │                      │
                             └──────────────► [Redis Cache]
                             │
                      [Mobile App] ──HTTPS──► [NestJS API]

  [Admin Browser] ──HTTPS──► [NestJS API] ──► [Audit Log DB]

  [Email Service] ◄──SMTP/API── [NestJS API]

  [File Storage R2] ◄──S3 API── [NestJS API] ◄── [Browser uploads]
```

Apply STRIDE to each component and each data flow arrow.

---

## 6. Trust Boundaries

A trust boundary separates zones of different trust levels. Data crossing a trust boundary must be validated and authenticated.

```
Trust boundaries in a web app:

  [Internet] ←─── trust boundary ───► [Your infrastructure]
                                         │
                              [DMZ / reverse proxy (Nginx)]
                                         │
                              [Application tier (NestJS)]
                                         │
                              [Data tier (PostgreSQL, Redis)]

  Crossing internet → infrastructure:
    → All input must be validated
    → Authentication required
    → TLS required

  Crossing application → data tier:
    → Parameterized queries
    → Least-privilege DB user
    → No direct internet access to DB
```

Data from outside a trust boundary is **untrusted** until validated. Data created inside the trust boundary by your own code is trusted.

```ts
// Everything from req.body, req.params, req.query, req.headers
// is from OUTSIDE the trust boundary → validate everything

// Data from your own DB, config files, internal services
// is from INSIDE → trust it (but still validate schema)

// JWT from Authorization header: crosses boundary → verify signature
// Data INSIDE a verified JWT: created by your system → trusted
```

---

## 7. PASTA — Another Approach

**Process for Attack Simulation and Threat Analysis** — a risk-centric, 7-stage methodology.

```
Stage 1: Define business objectives
  → What does the system do? What data is most sensitive?
  → What regulations apply (GDPR, health data)?

Stage 2: Define technical scope
  → Application components, infrastructure, integrations

Stage 3: Decompose the application
  → DFDs, trust boundaries, entry/exit points

Stage 4: Threat analysis
  → What threats exist in this environment?
  → CVE data, threat intelligence feeds

Stage 5: Vulnerability analysis
  → What weaknesses exist that threats can exploit?
  → Code review, dependency scan

Stage 6: Attack modeling
  → Build attack trees for the highest-risk scenarios

Stage 7: Risk and impact analysis
  → Quantify impact of each threat
  → Prioritize mitigations by risk score

PASTA is more comprehensive than STRIDE — used for full system security reviews.
STRIDE is faster — good for feature-level reviews.
```

---

## 8. Prioritizing Threats — DREAD

After listing threats, you need to prioritize them. DREAD provides a scoring framework:

```
D — Damage potential    (1-10): how bad is the impact if exploited?
R — Reproducibility     (1-10): how easy to reproduce the attack?
E — Exploitability      (1-10): how much skill/effort to exploit?
A — Affected users      (1-10): how many users are impacted?
D — Discoverability     (1-10): how easily discovered?

Score = (D + R + E + A + D) / 5
High (7-10):    fix immediately
Medium (4-6):   fix in next sprint
Low (1-3):      fix when convenient
```

Example:

```
Threat: Unauthenticated access to student list endpoint
  Damage:         8 (PII exposure for all students)
  Reproducibility:10 (just make the request)
  Exploitability: 10 (no skill required)
  Affected:       9 (all students in school)
  Discoverability: 7 (endpoint in API documentation)
  Score: 8.8 → CRITICAL — fix immediately

Threat: XSS in rarely-used admin notes field
  Damage:         7 (admin account takeover)
  Reproducibility: 6 (requires specific conditions)
  Exploitability:  6 (requires some knowledge)
  Affected:        2 (only admins who view that field)
  Discoverability: 4 (not obvious)
  Score: 5.0 → MEDIUM — fix in next sprint
```

---

## 9. Threat Modeling in Practice

A practical lightweight process for a small team building a feature:

```
1. Draw it (15 minutes)
   Whiteboard the feature: what components are involved?
   Where does user data flow? What gets stored? What APIs are called?

2. STRIDE it (20 minutes)
   For each component: what's the STRIDE threat?
   "What if someone spoofs this input?"
   "What sensitive data flows through here?"
   "Can someone do this without authorization?"

3. Attack tree the top 3 risks (10 minutes)
   What are the scariest 3 things that could go wrong?
   Draw the attack tree for each — what paths lead there?

4. Write mitigations (15 minutes)
   For each identified threat: what's the mitigation?
   Add them to the implementation plan

5. Add tests (ongoing)
   Write a test for each threat:
   "I should not be able to read another student's data"
   "SQL injection attempt should return 400, not data"
```

---

## Summary

```
Threat modeling answers: what can go wrong, and what do we do about it?
Cheapest when done before building; do it for every significant feature

STRIDE categories:
  Spoofing:              impersonating identity
  Tampering:             unauthorized data modification
  Repudiation:           denying actions
  Information Disclosure: exposing data to unauthorized parties
  Denial of Service:     making the system unavailable
  Elevation of Privilege: gaining unauthorized permissions

Attack trees:
  Goal at root, methods as branches
  Identify most likely/cheapest attack paths

Data Flow Diagrams:
  Map where data moves through the system
  Apply STRIDE to each component and data flow

Trust boundaries:
  Separate zones of trust — data crossing boundaries must be validated
  Internet → your app: validate everything
  App → DB: parameterized queries, least privilege

DREAD scoring:
  Damage × Reproducibility × Exploitability × Affected users × Discoverability
  Prioritizes which threats to fix first

Lightweight practice:
  15 min: draw the system
  20 min: STRIDE each component
  10 min: attack tree top 3 risks
  15 min: mitigations
  = 60 minutes → dramatically better security awareness
```
