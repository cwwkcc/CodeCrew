# Security Curriculum

A structured, progressive curriculum covering the security concepts every web developer needs — from cryptographic foundations through application-layer vulnerabilities, identity systems, and infrastructure hardening. All 30 modules are numbered `00–29` — read them in order.

> **Location:** `resources/curriculum/security/`

---

## Module Index

### Part 1 — Cryptographic Foundations

|#|Topic|File|
|---|---|---|
|00|Cryptography Fundamentals — Principles, Goals, and Threat Models|`00 - Cryptography Fundamentals.md`|
|01|Encryption — Symmetric and Asymmetric|`01 - Encryption.md`|
|02|Hashing — SHA Family, HMAC, and Key Derivation Functions|`02 - Hashing.md`|
|03|Digital Signatures & MACs — Authentication and Non-Repudiation|`03 - Digital Signatures & MACs.md`|

### Part 2 — Transport Security

|#|Topic|File|
|---|---|---|
|04|PKI and Certificates — CAs, X.509, and the Trust Chain|`04 - PKI and Certificates.md`|
|05|TLS Deep Dive — Handshake, Cipher Suites, and Certificate Verification|`05 - TLS Deep Dive.md`|
|06|DNS Security — DNSSEC, DoH, DoT|`06 - DNS Security.md`|
|07|HTTP Security Headers — CSP, HSTS, and More|`07 - HTTP Security Headers.md`|

### Part 3 — Identity & Authentication

|#|Topic|File|
|---|---|---|
|08|MFA and TOTP — How Authenticator Apps Work|`08 - MFA and TOTP.md`|
|09|OAuth 2.0 and OpenID Connect|`09 - OAuth 2.0 and OpenID Connect.md`|
|10|RBAC and ABAC — Role and Attribute-Based Access Control|`10 - RBAC and ABAC.md`|
|11|Multi-Tenancy Security — Isolation and Data Segregation|`11 - Multi-Tenancy Security.md`|
|12|API Authorization — Scopes, Keys, and Rate Limiting|`12 - API Authorization.md`|
|21|JWT and Tokens — Structure, Signing, and Common Attacks|`21 - JWT and Tokens.md`|
|22|Sessions and Cookies — Secure Storage and Session Hijacking|`22 - Sessions and Cookies.md`|

### Part 4 — Application Vulnerabilities (OWASP)

|#|Topic|File|
|---|---|---|
|13|Injection Attacks — SQL, Command, and NoSQL Injection|`13 - Injection Attacks.md`|
|14|XSS and CSRF — Cross-Site Scripting and Request Forgery|`14 - XSS and CSRF.md`|
|15|Insecure Design — Design-Level Flaws and Threat Modeling|`15 - Insecure Design.md`|
|17|File Upload Security|`17 - File Upload Security.md`|
|26|WebSocket Security|`26 - WebSocket Security.md`|

### Part 5 — Infrastructure & Supply Chain

|#|Topic|File|
|---|---|---|
|16|Supply Chain and Dependency Security|`16 - Supply Chain and Dependency Security.md`|
|18|Secrets Management — Environment Variables, Vaults, and Rotation|`18 - Secrets Management.md`|
|19|Database Security — Permissions, Encryption at Rest, and Auditing|`19 - Database Security.md`|
|20|Docker Security — Images, Containers, and Runtime Hardening|`20 - Docker Security.md`|
|25|Email Security — SPF, DKIM, DMARC|`25 - Email Security.md`|

### Part 6 — Testing & Operations

|#|Topic|File|
|---|---|---|
|23|Threat Modeling — STRIDE, Attack Trees, and Risk Assessment|`23 - Threat Modeling.md`|
|24|Penetration Testing Basics|`24 - Penetration Testing Basics.md`|
|27|Mobile Security|`27 - Mobile Security.md`|
|28|Security Testing — SAST, DAST, and Fuzzing|`28 - Security Testing.md`|
|29|Logging and Monitoring — Audit Trails and Anomaly Detection|`29 - Logging and Monitoring.md`|

---

## Notes

- Modules 00–03 (cryptography) are foundational — all later modules assume this knowledge.
- The numbering is sequential by topic group. Read in order for the full picture, or jump to a specific group if you need a targeted reference.