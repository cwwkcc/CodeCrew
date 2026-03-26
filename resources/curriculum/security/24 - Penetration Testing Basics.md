> Penetration testing is structured adversarial testing — attacking your own system the way a real attacker would. This covers the methodology (not how to attack others), reconnaissance, vulnerability scanning, reading CVEs, and what you should test before launching.

---

## Table of Contents

1. [What Penetration Testing Is](#1-what-penetration-testing-is)
2. [Types of Penetration Tests](#2-types-of-penetration-tests)
3. [The Penetration Testing Methodology](#3-the-penetration-testing-methodology)
4. [Reconnaissance](#4-reconnaissance)
5. [Scanning and Enumeration](#5-scanning-and-enumeration)
6. [Exploitation Basics](#6-exploitation-basics)
7. [Reading and Applying CVEs](#7-reading-and-applying-cves)
8. [OWASP ZAP — Automated Scanning](#8-owasp-zap--automated-scanning)
9. [What to Test Before Launching](#9-what-to-test-before-launching)

---

## 1. What Penetration Testing Is

Penetration testing (pen testing) is authorized security testing where you attempt to find and exploit vulnerabilities in your own systems — before attackers do.

```
Bug bounty:   external researchers find bugs → paid reward → you fix
Pen test:     hired professionals (or yourself) systematically test the system
Code review:  developers review code for security issues

Different from:
  Vulnerability scan:  automated tools identify potential weaknesses (no exploitation)
  Pen test:            actually attempts to exploit those weaknesses to confirm impact
```

**Scope and authorization are critical.** A penetration test must be:

- Authorized by the system owner in writing
- Scoped to specific systems and methods
- Time-bounded

Testing systems you don't own, or without authorization, is illegal in virtually every jurisdiction.

---

## 2. Types of Penetration Tests

```
Black box:
  Tester has no knowledge of internal systems
  Simulates an external attacker starting from scratch
  Realistic but time-consuming — lots of reconnaissance

White box:
  Tester has full access: source code, architecture diagrams, credentials
  Most thorough — can find issues in business logic and code
  Simulates insider threat or a very patient attacker

Grey box:
  Tester has some information: API documentation, a test account
  Balances realism and thoroughness
  Most common for web application testing

Scope dimensions:
  Network:       test network infrastructure, exposed services
  Web app:       test the web application (HTTP endpoints, auth, business logic)
  API:           test the REST/GraphQL API specifically
  Mobile:        test mobile app + its API communication
  Social eng:    phishing, phone pretexting (requires separate authorization)
```

---

## 3. The Penetration Testing Methodology

```
Phase 1: Planning
  Define scope: what systems? what methods? what's off-limits?
  Rules of engagement: can you DoS? during what hours?
  Authorization in writing
  
Phase 2: Reconnaissance
  Gather information about the target without directly interacting with it
  OSINT (open source intelligence)

Phase 3: Scanning and Enumeration
  Directly interact with the target to discover services, ports, tech stack
  
Phase 4: Exploitation
  Attempt to exploit identified vulnerabilities
  Confirm impact
  
Phase 5: Post-exploitation
  What can you access from the compromised foothold?
  Escalate privileges, move laterally, access data
  
Phase 6: Reporting
  Document: vulnerabilities found, steps to reproduce, impact, recommendation
  Severity rating for each finding
  
Phase 7: Remediation verification
  After fixes are deployed: verify the vulnerabilities are actually fixed
```

---

## 4. Reconnaissance

Reconnaissance is gathering information about the target using publicly available sources — before touching the system.

### Passive Reconnaissance

```
DNS records:
  → What subdomains exist? (api.example.com, admin.example.com, staging.example.com)
  → MX records reveal email provider
  → TXT records reveal tech (SPF, DKIM, domain verification codes for Google/GitHub)
  
  Tools: dig, nslookup, dnsx, subfinder, amass

  dig ANY example.com
  dig txt example.com
  subfinder -d example.com  # enumerate subdomains

WHOIS:
  → Who registered the domain? Contact details?
  → Registration date, expiry (expiring domains can be hijacked)
  
  whois example.com

Certificate Transparency logs:
  → All certificates ever issued for *.example.com
  → Reveals subdomains you might not know about
  
  curl "https://crt.sh/?q=%.example.com&output=json"

Tech stack fingerprinting:
  → Response headers reveal server (Nginx/Apache), framework (Express, NestJS)
  → Wappalyzer (browser extension) detects JavaScript libraries
  → Job listings often reveal tech stack ("We use NestJS, PostgreSQL, Redis")
  
  Security implication: headers can reveal versions → look up CVEs for those versions
  Defense: set "Server: " header to empty or generic value

GitHub/GitLab:
  → Public repositories with configuration files
  → Accidentally committed secrets (API keys, passwords)
  → Historical commits even after deletion (git log)
  
  Tools: trufflehog, gitleaks (search for secrets in repos)

Google dorking:
  site:example.com filetype:env     → look for exposed .env files
  site:example.com inurl:admin      → find admin panels
  site:example.com "DB_PASSWORD"    → accidentally indexed secrets
```

### What to Protect Against

```
Hide server version in headers:
  Nginx: server_tokens off;
  Express: app.disable("x-powered-by");
  Helmet.js does this automatically

Minimize public subdomains
  Don't name subdomains "admin", "staging", "dev" — they invite targeting

Keep secrets out of git
  .gitignore for .env files
  Secret scanning (GitHub's built-in, trufflehog pre-commit hook)

WHOIS privacy
  Register domains with WHOIS privacy protection
  Contact email is a target for phishing
```

---

## 5. Scanning and Enumeration

Direct interaction with the target to discover attack surface.

### Port Scanning

```bash
# nmap — the standard port scanner
nmap -sV -sC example.com         # service version detection + default scripts
nmap -p 1-65535 example.com      # all ports (slow)
nmap -F example.com              # fast scan — top 100 ports

# What you're looking for:
# Port 22 (SSH):   exposed to internet? passwordauth enabled?
# Port 3306 (MySQL): database directly accessible?
# Port 6379 (Redis): Redis without auth?
# Port 27017 (MongoDB): MongoDB without auth?
# Port 8080/3000/4000: dev servers accidentally exposed?
```

**For Paideon:** Your VPS should have only 22 (SSH), 80 (HTTP, redirects to HTTPS), 443 (HTTPS) exposed to the internet. Database ports must be closed to the internet.

### Web Application Enumeration

```bash
# Directory brute-forcing — discover hidden endpoints
gobuster dir -u https://api.example.com -w /usr/share/wordlists/common.txt
# or: ffuf, dirb, feroxbuster

# What you're looking for:
# /admin, /dashboard, /backup, /.git, /.env, /config.json
# /api/v1/users (admin endpoint accidentally exposed)
# /phpinfo.php (reveals server config)

# API endpoint discovery
# Review API documentation, JavaScript bundles, mobile app traffic
# Tools: Swagger UI often exposed at /api, /docs, /swagger-ui
```

### Web Technology Fingerprinting

```bash
# WhatWeb — identifies technologies
whatweb https://example.com

# Response headers analysis
curl -I https://api.example.com
# Look for: X-Powered-By, Server, X-AspNet-Version, X-Generator
# These reveal technology stack → look up known vulnerabilities for those versions
```

---

## 6. Exploitation Basics

Exploitation confirms that a vulnerability is exploitable and quantifies impact.

### Manual Testing Checklist

```
Authentication:
  Can you bypass login? (SQLi, default credentials, logic flaws)
  Does password reset work securely? (token predictable? link reusable? no expiry?)
  Is MFA bypassable? (backup codes brute-forceable? TOTP accepted indefinitely?)

Authorization:
  Can user A access user B's data? (change ID in URL/body)
  Can a student access teacher endpoints? (change role in JWT — won't work if signed properly)
  Can you access /admin/* without admin role?

Input handling:
  SQL injection: ' OR '1'='1 in login fields
  XSS: <script>alert(1)</script> in all input fields
  Path traversal: ../../etc/passwd in file paths
  Open redirect: /redirect?to=https://evil.com

Session management:
  Is the session ID predictable?
  Can you reuse a refresh token after rotation? (should get 401)
  Does logout actually invalidate the token?
  Are cookies HttpOnly, Secure, SameSite?

Business logic:
  Can you skip steps in multi-step processes?
  Can you access features before payment (if applicable)?
  Are rate limits enforced on auth endpoints?
```

### Using Burp Suite

Burp Suite is the standard web app pen testing tool. It acts as an HTTP proxy between your browser and the server, letting you intercept and modify requests.

```
Basic workflow:
  1. Configure browser to use Burp as proxy (127.0.0.1:8080)
  2. Browse the application normally
  3. Burp captures all requests
  4. Right-click request → "Send to Repeater"
  5. In Repeater: modify request, resend, observe response
  6. Test: modify IDs, inject payloads, change roles

Burp Scanner (Pro):
  Automated scanning for common vulnerabilities
  XSS, SQLi, CSRF, open redirects, etc.

Burp Intruder:
  Automated attack: try a list of payloads on a parameter
  Useful for: brute force, parameter fuzzing, wordlist attacks
```

---

## 7. Reading and Applying CVEs

A CVE (Common Vulnerabilities and Exposures) is a standardized identifier for a publicly known vulnerability.

### CVE Anatomy

```
CVE-2021-44228

CVE:   Common Vulnerabilities and Exposures
2021:  Year the CVE was assigned
44228: Sequential number within that year

This specific CVE: Log4Shell (Apache Log4j RCE vulnerability)
```

### CVSS — Severity Scoring

```
CVSS v3 score: 0.0 to 10.0

0.0 - 3.9:   Low
4.0 - 6.9:   Medium
7.0 - 8.9:   High
9.0 - 10.0:  Critical

CVSS considers:
  Attack Vector (network/adjacent/local/physical)
  Attack Complexity (low/high)
  Privileges Required (none/low/high)
  User Interaction (none/required)
  Confidentiality/Integrity/Availability impact (none/low/high)
```

### Finding CVEs for Your Dependencies

```bash
# npm audit — check for known vulnerabilities
npm audit

# Example output:
# crypto-js  <=3.3.0
# Severity: critical
# PBKDF2 1,000 iterations by default is 1,000x weaker than spec
# Fix Available: npm audit fix
# CVE-2023-46133

# Snyk — more comprehensive
npx snyk test

# Check a specific package
# https://snyk.io/vuln/npm:package-name
# https://nvd.nist.gov/vuln/search
```

### Applying CVE Information

```
1. Read the CVE description:
   "Does this vulnerability affect my usage of the package?"
   "Am I using the vulnerable code path?"

2. Check the fix version:
   Is there a patched version? Update to it.
   Is there no patch? Is there a workaround?

3. Assess risk:
   CVSS 9.8 Critical in a rarely-used package: medium priority
   CVSS 7.0 High in your auth library: high priority
   Context matters more than raw score

4. Update and test:
   npm update package-name
   Run tests to ensure nothing broke
   Deploy
```

---

## 8. OWASP ZAP — Automated Scanning

OWASP ZAP (Zed Attack Proxy) is a free, open-source automated security scanner. Good for finding common vulnerabilities.

```bash
# Run ZAP as a Docker container
docker run -t owasp/zap2docker-stable zap-baseline.py \
  -t https://api.example.com \
  -r zap-report.html

# Types of scans:
# Baseline:  passive only — no actual attacks
# API scan:  scan an API using OpenAPI/Swagger spec
# Full scan: active — actually sends attacks (use only against test env)
```

### What ZAP Finds

```
Automatically detects:
  → Missing security headers (no HSTS, no CSP)
  → SSL/TLS issues (weak ciphers, expired cert)
  → Information leakage (server version in headers, verbose errors)
  → Basic XSS and injection (not all — manual testing still needed)
  → CSRF vulnerabilities
  → Open redirects
  → Path traversal

ZAP does NOT find:
  → Business logic flaws
  → Authorization bypasses (needs authenticated testing)
  → Complex multi-step attack chains
  → IDOR (needs knowledge of valid object IDs)
```

### Integrating into CI/CD

```yaml
# GitHub Actions example
- name: ZAP baseline scan
  uses: zaproxy/action-baseline@v0.9.0
  with:
    target: "https://staging.example.com"
    rules_file_name: ".zap/rules.tsv"
    fail_action: false  # don't fail build, just report
```

---

## 9. What to Test Before Launching

Pre-launch security checklist — these are the minimum checks before going live:

### Authentication and Authorization

```
□ Brute force protection on login (rate limiting, account lockout)
□ Password reset tokens expire and are one-time use
□ Session IDs are high-entropy and random
□ Session invalidated on logout (server-side)
□ MFA can't be bypassed by replaying old codes
□ Authorization checked on EVERY endpoint (not just GET)
□ IDOR: user A cannot access user B's resources
□ Admin endpoints require admin role
□ JWT signature verified with correct algorithm
□ JWT alg: none attack doesn't work (test: change alg to "none")
```

### Input and Data

```
□ All inputs validated (type, length, format)
□ SQL injection attempts return 400, not data
□ XSS payloads in text fields are stored escaped, rendered as text
□ File uploads: type restricted, size limited, stored outside web root
□ Path traversal: ../../etc/passwd returns 400/403
□ Large payload (10MB+ body): server handles gracefully, doesn't OOM
```

### Transport and Headers

```
□ HTTP → HTTPS redirect working
□ HSTS header present
□ All security headers set (run securityheaders.com)
□ TLS 1.0/1.1 disabled (test with SSL Labs)
□ Cookies: HttpOnly, Secure, SameSite
□ Debug mode off in production
□ Stack traces not shown in error responses
```

### Configuration

```
□ No default credentials (admin/admin, root/root, etc.)
□ Database not accessible from the internet
□ No .env, .git, or config files exposed via HTTP
□ Development endpoints removed (/test, /debug)
□ Rate limiting on all auth endpoints
□ Dependencies audited (npm audit — no critical/high)
□ Secrets in environment variables, not in code
□ Server version headers removed
```

---

## Summary

```
Penetration testing: authorized, structured adversarial testing of your own systems

Types: black box (no info), white box (full info), grey box (partial info)

Methodology:
  Plan → Recon → Scan → Exploit → Report → Verify fixes

Reconnaissance (passive):
  DNS, certificate transparency, WHOIS, GitHub, Google dorking
  Defense: hide tech stack headers, secrets out of git

Scanning (active):
  Port scanning (nmap), directory brute-forcing (gobuster), fingerprinting
  Defense: close unused ports, remove hidden admin paths, generic headers

CVEs:
  Identifier for known vulnerabilities
  CVSS score: 0-10 (critical: 9.0+)
  npm audit / Snyk to find CVEs in your dependencies
  Update affected packages; assess contextual risk

OWASP ZAP:
  Automated scanner — finds headers, basic XSS/SQLi, SSL issues
  Doesn't replace manual testing for business logic
  Run baseline scan before launch

Pre-launch checklist:
  Auth/authz, input validation, transport/headers, configuration
  Test JWT alg:none attack specifically
  Verify database not internet-accessible
```