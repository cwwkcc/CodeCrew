> Security testing integrates security verification into the development lifecycle — not as a final gate, but as a continuous process. This covers the types of automated security testing (SAST, DAST, dependency scanning), how to set them up, and a comprehensive pre-launch security checklist.

---

## Table of Contents

1. [Security Testing Types](#1-security-testing-types)
2. [SAST — Static Analysis](#2-sast--static-analysis)
3. [DAST — Dynamic Analysis](#3-dast--dynamic-analysis)
4. [Dependency Vulnerability Scanning](#4-dependency-vulnerability-scanning)
5. [Secret Scanning](#5-secret-scanning)
6. [Security in CI/CD](#6-security-in-cicd)
7. [Manual Security Testing](#7-manual-security-testing)
8. [Security Unit Tests](#8-security-unit-tests)
9. [Pre-Launch Security Checklist](#9-pre-launch-security-checklist)

---

## 1. Security Testing Types

```
SAST (Static Application Security Testing):
  Analyzes source code without running it
  Finds: hardcoded secrets, SQL concatenation, dangerous function use
  Fast, runs in CI, finds issues early
  False positives are common — needs tuning

DAST (Dynamic Application Security Testing):
  Tests a running application by sending requests
  Finds: injection vulnerabilities, auth bypasses, misconfigurations
  Closer to real attacks — fewer false positives
  Runs against a deployed (staging) environment

SCA (Software Composition Analysis):
  Scans dependencies for known vulnerabilities
  Finds: CVEs in npm packages, outdated library versions
  Fast, automated, integrated with package managers
  
IAST (Interactive Application Security Testing):
  Agents instrument the running app from inside
  Monitors execution during tests
  Very low false positives, deeper insight
  Complex to set up — relevant at scale

Penetration testing:
  Manual or tool-assisted adversarial testing
  Finds: business logic flaws, complex attack chains
  Covered in file 24
```

---

## 2. SAST — Static Analysis

### ESLint with Security Rules

```bash
npm install --save-dev eslint-plugin-security eslint-plugin-no-secrets

# .eslintrc.js
module.exports = {
  plugins: ["security", "no-secrets"],
  extends: ["plugin:security/recommended"],
  rules: {
    "no-secrets/no-secrets": ["error", { tolerance: 4.5 }],
    "security/detect-object-injection": "warn",
    "security/detect-non-literal-regexp": "warn",
    "security/detect-unsafe-regex": "error",
    "security/detect-buffer-noassert": "error",
    "security/detect-child-process": "warn",
    "security/detect-disable-mustache-escape": "error",
    "security/detect-eval-with-expression": "error",
    "security/detect-new-buffer": "error",
    "security/detect-no-csrf-before-method-override": "warn",
    "security/detect-possible-timing-attacks": "warn",
    "security/detect-pseudoRandomBytes": "error",  // flag Math.random()
    "security/detect-sql-literal-injection": "error",
  },
};
```

What eslint-plugin-security catches:

```ts
// FLAGGED: potential SQL injection
const query = "SELECT * FROM users WHERE id = " + userId;

// FLAGGED: eval with dynamic string
eval("function " + userInput + "(){}");

// FLAGGED: Math.random() for security
const token = Math.random().toString(36);

// FLAGGED: unsafe regex (ReDoS vulnerability)
const regex = new RegExp("(a+)+");  // exponential backtracking possible

// FLAGGED: object injection
const data = obj[userInput];  // accessing object with user-controlled key
```

### CodeQL — GitHub's SAST Engine

```yaml
# .github/workflows/codeql.yml
name: CodeQL
on:
  push:
    branches: [main, "feature/**"]
  pull_request:
    branches: [main]

jobs:
  analyze:
    runs-on: ubuntu-latest
    permissions:
      security-events: write
    
    steps:
      - uses: actions/checkout@v4
      
      - name: Initialize CodeQL
        uses: github/codeql-action/init@v3
        with:
          languages: javascript-typescript
          queries: security-and-quality
      
      - name: Perform CodeQL Analysis
        uses: github/codeql-action/analyze@v3
```

CodeQL finds:

- Injection vulnerabilities (SQL, command, path traversal)
- XSS
- Prototype pollution
- Unsafe deserialization
- Hardcoded credentials

Results appear in GitHub Security tab → Code Scanning Alerts.

### Semgrep — Rule-Based SAST

```yaml
# .semgrep.yml — custom rules
rules:
  - id: no-console-log-sensitive
    patterns:
      - pattern: console.log($X)
      - metavariable-pattern:
          metavariable: $X
          patterns:
            - pattern-either:
              - pattern: $Y.password
              - pattern: $Y.token
              - pattern: $Y.secret
    message: "Possible sensitive data in console.log"
    severity: WARNING

  - id: jwt-hardcoded-secret
    pattern: jwt.sign($P, "...")  # hardcoded string literal
    message: "JWT signed with hardcoded secret"
    severity: ERROR
```

```bash
# Run Semgrep
semgrep --config auto .         # use auto-detected rules
semgrep --config p/nodejs .     # Node.js-specific rules
semgrep --config p/jwt .        # JWT-specific rules
```

---

## 3. DAST — Dynamic Analysis

### OWASP ZAP — Automated Scanning

ZAP scans a running application. Run against staging environment only.

```yaml
# .github/workflows/dast.yml
name: DAST Security Scan
on:
  push:
    branches: [staging]

jobs:
  zap-scan:
    runs-on: ubuntu-latest
    
    steps:
      - name: ZAP Baseline Scan (passive only)
        uses: zaproxy/action-baseline@v0.10.0
        with:
          target: "https://staging.yourschool.lk"
          rules_file_name: ".zap/rules.tsv"
          fail_action: false
          artifact_name: "zap-report"
      
      - name: Upload ZAP Report
        uses: actions/upload-artifact@v4
        with:
          name: zap-report
          path: report_html.html
```

### ZAP Rules Configuration

```tsv
# .zap/rules.tsv
# Format: rule_id\talert_type\t(IGNORE|WARN|FAIL)
# Alert types: IGNORE = don't report, WARN = report but don't fail build, FAIL = fail build

10015	WARN		# Incomplete or No Cache-control Header
10016	WARN		# Web Browser XSS Protection Not Enabled
10017	FAIL		# Cross-Domain JavaScript Source File Inclusion
10019	FAIL		# Content-Type Header Missing
10020	FAIL		# X-Frame-Options Header Not Set
10021	WARN		# X-Content-Type-Options Header Missing
10023	WARN		# Information Disclosure - Debug Error Messages
10024	FAIL		# Information Disclosure - Sensitive Information in URL
10025	FAIL		# Information Disclosure - Sensitive Information in HTTP Referrer Header
```

### Nuclei — Template-Based DAST

```bash
# Run Nuclei security templates against your app
nuclei -u https://staging.yourschool.lk \
  -t ~/nuclei-templates/http/vulnerabilities/ \
  -t ~/nuclei-templates/http/misconfigurations/ \
  -severity medium,high,critical \
  -o nuclei-report.txt
```

Nuclei has thousands of community templates covering specific CVEs, misconfigurations, and vulnerability patterns.

---

## 4. Dependency Vulnerability Scanning

### npm audit — Built-in

```bash
# Check for known vulnerabilities
npm audit

# Example output:
# found 2 vulnerabilities (1 moderate, 1 high)
#
# high: CVSS 7.5 — ReDoS in validator
# Package: validator
# Patched in: >=13.7.0
# Dependency of: nestjs → class-validator → validator
# Fix: npm audit fix

# Auto-fix non-breaking updates
npm audit fix

# Check if there are fixable issues
npm audit --audit-level=high  # exit code 1 if high/critical found
```

### Dependabot — Automated PRs

```yaml
# .github/dependabot.yml
version: 2
updates:
  - package-ecosystem: "npm"
    directory: "/"
    schedule:
      interval: "weekly"
    open-pull-requests-limit: 10
    ignore:
      # Only ignore minor version bumps for major frameworks
      # (breaking changes need manual review)
      - dependency-name: "typescript"
        update-types: ["version-update:semver-major"]

  - package-ecosystem: "docker"
    directory: "/"
    schedule:
      interval: "weekly"
```

Dependabot automatically opens PRs when:

- A dependency has a newer version
- A dependency has a security vulnerability

### Snyk — More Comprehensive

```yaml
# .github/workflows/snyk.yml
name: Snyk Security Scan
on: [push]

jobs:
  snyk:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: snyk/actions/node@master
        env:
          SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
        with:
          args: --severity-threshold=high
```

Snyk's advantages over npm audit:

- Larger vulnerability database
- Detects transitive dependency issues
- Container image scanning
- IaC scanning (Docker, Kubernetes, Terraform)

---

## 5. Secret Scanning

### Detecting Committed Secrets

```bash
# gitleaks — scans git history for secrets
gitleaks detect --source . --report-path gitleaks-report.json

# truffleHog — deep git history scan
trufflehog git file://. --only-verified

# GitHub's built-in secret scanning
# Automatically enabled for public repos
# Scans for: AWS keys, GitHub tokens, Stripe keys, etc.
# Settings → Security → Secret scanning
```

### Pre-Commit Hook to Block Secret Commits

```bash
# Install pre-commit framework
pip install pre-commit

# .pre-commit-config.yaml
repos:
  - repo: https://github.com/gitleaks/gitleaks
    rev: v8.18.2
    hooks:
      - id: gitleaks

  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.5.0
    hooks:
      - id: detect-private-key
      - id: check-json
      - id: check-yaml

# Install hooks
pre-commit install
```

Now `git commit` will fail if you try to commit a private key or hardcoded secret.

---

## 6. Security in CI/CD

Full security pipeline:

```yaml
# .github/workflows/security.yml
name: Security Pipeline
on:
  push:
    branches: [main, "feature/**"]
  pull_request:
    branches: [main]

jobs:
  sast:
    name: Static Analysis
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Run ESLint security rules
        run: npx eslint . --ext .ts --max-warnings 0
      - name: Run Semgrep
        uses: semgrep/semgrep-action@v1
        with:
          config: p/nodejs p/jwt p/secrets

  sca:
    name: Dependency Scan
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
      - run: npm ci
      - name: npm audit
        run: npm audit --audit-level=high

  secrets:
    name: Secret Scan
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0  # full history for secret scanning
      - name: gitleaks
        uses: gitleaks/gitleaks-action@v2
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

  codeql:
    name: CodeQL Analysis
    runs-on: ubuntu-latest
    permissions:
      security-events: write
    steps:
      - uses: actions/checkout@v4
      - uses: github/codeql-action/init@v3
        with:
          languages: javascript-typescript
      - uses: github/codeql-action/analyze@v3
```

---

## 7. Manual Security Testing

Automated tools miss business logic issues. Manual testing is required.

### Auth Flow Testing

```
□ Try accessing any authenticated endpoint without a token → must get 401
□ Try with an expired token → must get 401
□ Try with a token from a deleted user → must get 401
□ Try with a token from a different school → resource not found (not 403)
□ Try changing JWT algorithm to "none" → must be rejected
□ Logout → try using the old refresh token → must be rejected
□ Generate a token with a modified role (e.g., student → admin) → must fail sig verification
```

### Authorization Testing

```
□ Create user A and user B in the same school
□ User A: GET /api/users/[user_B_id] → must get 403 or 404 (not user B's data)
□ User A: PUT /api/users/[user_B_id] → must get 403
□ Create user C in a different school
□ User A: GET /api/users/[user_C_id] → must get 404 (not even know it exists)
□ As student: GET /api/admin/audit-logs → must get 403
□ As teacher: DELETE /api/users → must get 403
□ As admin: GET /api/users of different school → must get 404
```

### Rate Limiting Testing

```bash
# Test login rate limit
for i in {1..15}; do
  curl -s -o /dev/null -w "%{http_code}\n" \
    -X POST https://api.yourschool.lk/auth/login \
    -H "Content-Type: application/json" \
    -d '{"email":"test@test.com","password":"wrong"}'
done
# Should see: 200 200 200 ... 429 429 429 after 10 attempts
```

---

## 8. Security Unit Tests

Security properties should have explicit tests, not just be tested incidentally.

```ts
// auth.security.spec.ts
describe("Authentication Security", () => {
  describe("JWT alg:none attack", () => {
    it("should reject tokens with alg: none", async () => {
      // Forge a token with algorithm set to none
      const header = Buffer.from('{"alg":"none","typ":"JWT"}').toString("base64url");
      const payload = Buffer.from(JSON.stringify({ sub: adminUser.id, role: "admin" })).toString("base64url");
      const forgedToken = `${header}.${payload}.`; // no signature

      const response = await request(app.getHttpServer())
        .get("/api/admin/users")
        .set("Authorization", `Bearer ${forgedToken}`);

      expect(response.status).toBe(401);
    });
  });

  describe("Cross-tenant isolation", () => {
    it("should not expose school B data to school A admin", async () => {
      const tokenA = await loginAsAdmin(schoolA);
      const studentB = await createStudent(schoolB);

      const response = await request(app.getHttpServer())
        .get(`/api/users/${studentB.id}`)
        .set("Authorization", `Bearer ${tokenA}`);

      expect(response.status).toBe(404);
      expect(response.body).not.toHaveProperty("email", studentB.email);
    });
  });

  describe("Privilege escalation", () => {
    it("student cannot access admin endpoints", async () => {
      const studentToken = await loginAsStudent(schoolA);

      const response = await request(app.getHttpServer())
        .get("/api/admin/audit-log")
        .set("Authorization", `Bearer ${studentToken}`);

      expect(response.status).toBe(403);
    });
  });

  describe("Rate limiting", () => {
    it("should rate limit login attempts", async () => {
      const attempts = Array.from({ length: 15 }, () =>
        request(app.getHttpServer())
          .post("/auth/login")
          .send({ email: "test@test.com", password: "wrong" })
      );
      const responses = await Promise.all(attempts);
      const rateLimited = responses.filter(r => r.status === 429);
      expect(rateLimited.length).toBeGreaterThan(0);
    });
  });
});
```

---

## 9. Pre-Launch Security Checklist

### Authentication and Session Management

```
□ Passwords hashed with bcrypt (cost 12+) or Argon2id
□ JWT secrets are 256+ bit random values from CSPRNG
□ JWT expiry: access token ≤15 min, refresh token ≤30 days
□ JWT alg:none attack rejected
□ Refresh token rotation on use
□ Refresh token invalidated on logout
□ "Logout all devices" functionality works
□ Password reset tokens: random, expiring, single-use, stored as hash
□ Account lockout after repeated failed logins
□ Rate limiting on /auth/login, /auth/refresh, /auth/register
```

### Authorization

```
□ Every HTTP method on every endpoint has authorization guard
□ RBAC roles checked server-side (not from JWT without sig verification)
□ Ownership/school check on every resource access (not just route-level)
□ findFirst with schoolId instead of findUnique by ID
□ IDOR tested: user A cannot read user B's data
□ Cross-school tested: school A admin cannot read school B data
□ Admin endpoints unreachable by teacher/student/parent
```

### Input and Output

```
□ All inputs validated (type, length, format) via class-validator
□ ValidationPipe whitelist: true (strip unlisted properties)
□ SQL injection attempts return 400 (parameterized queries via Prisma)
□ XSS: user-generated content escaped on output (React default)
□ File uploads: type restricted, size limited, stored outside web root
□ No stack traces in API error responses (NODE_ENV=production)
□ Verbose error messages only in server logs, not HTTP responses
□ No sensitive data in error messages ("User not found" not "Alice not found")
```

### Transport and Headers

```
□ HTTPS everywhere (HTTP redirects to HTTPS)
□ HSTS header: max-age=31536000; includeSubDomains
□ TLS 1.2/1.3 only (tested with ssllabs.com — score A)
□ All security headers set (tested with securityheaders.com — score A)
□ Cookies: HttpOnly, Secure, SameSite=Strict
□ CORS origin restricted to known domains
□ Server and X-Powered-By headers removed/genericized
```

### Configuration and Infrastructure

```
□ NODE_ENV=production (enables security defaults, disables dev tools)
□ All secrets in environment variables (not in code or git)
□ .env files in .gitignore
□ Secret scan on git history (gitleaks) — clean
□ npm audit — no critical or high vulnerabilities
□ Database not accessible from public internet
□ Database user: least-privilege (no DROP, no CREATE TABLE in production)
□ Redis password set, not publicly accessible
□ Admin panel (if any) behind additional auth layer or IP restriction
□ Rate limiting on all API endpoints (not just auth)
□ No debug endpoints (/dev, /test, /health exposing internal state)
```

### Logging and Monitoring

```
□ Authentication events logged (login success/fail, logout, password reset)
□ Authorization failures logged
□ Sensitive data NOT logged (passwords, tokens, SSNs)
□ Log timestamps in UTC with timezone
□ Logs shipped to persistent storage (not only local disk)
□ Alert configured for: unusual login patterns, mass auth failures
```

---

## Summary

```
SAST (static analysis):
  eslint-plugin-security: flag dangerous patterns in code
  CodeQL: GitHub native, finds injection, XSS, unsafe patterns
  Semgrep: custom rules for your codebase patterns

DAST (dynamic analysis):
  OWASP ZAP: baseline scan against staging environment
  Nuclei: template-based scanning for specific CVEs/misconfigs
  Runs against deployed app — finds runtime issues

SCA (dependency scanning):
  npm audit: built-in, runs on npm install
  Dependabot: automated PRs for vulnerability fixes
  Snyk: larger database, container scanning

Secret scanning:
  gitleaks: scans git history
  Pre-commit hook: blocks secrets before they're committed
  GitHub secret scanning: automatic on push

Security unit tests:
  Test JWT alg:none attack explicitly
  Test cross-tenant isolation: user A cannot read user B's data
  Test privilege escalation: student cannot reach admin endpoints

Pre-launch checklist:
  Auth/session, authorization, input/output,
  transport/headers, configuration, logging
```