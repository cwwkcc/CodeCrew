> Your application is only as secure as every package it depends on. The npm ecosystem has thousands of packages, each a potential vector. This covers dependency vulnerabilities, supply chain attacks, how to audit and monitor, and how attackers compromise packages.

---

## Table of Contents

1. [The Supply Chain Threat](#1-the-supply-chain-threat)
2. [Dependency Confusion Attacks](#2-dependency-confusion-attacks)
3. [Typosquatting](#3-typosquatting)
4. [Malicious Package Updates](#4-malicious-package-updates)
5. [Known Vulnerability Exploitation](#5-known-vulnerability-exploitation)
6. [npm audit](#6-npm-audit)
7. [Snyk](#7-snyk)
8. [Dependabot](#8-dependabot)
9. [Lockfiles](#9-lockfiles)
10. [Minimizing Dependency Risk](#10-minimizing-dependency-risk)

---

## 1. The Supply Chain Threat

Modern applications depend on hundreds of packages. The average NestJS project has 500-1000 transitive dependencies (packages your packages depend on).

```
Your package.json: 30 direct dependencies
Each of those: 20-50 dependencies
Transitive total: 500-1000 packages

You reviewed 30. You're trusting 470-970 packages you've never read.
Each one is a potential backdoor.
```

**Real examples:**

- **event-stream (2018):** A maintained npm package had a new maintainer added who injected code to steal bitcoin wallets. Affected millions of downloads.
- **ua-parser-js (2021):** Attacker compromised the maintainer's npm account, published a version that installed cryptominer and password stealer.
- **node-ipc (2022):** Maintainer intentionally introduced malware that wiped files on Russian/Belarusian systems (protestware). Affected thousands of projects via Vue CLI.
- **colors/faker (2022):** Maintainer deliberately corrupted their own widely-used packages in protest.
- **xz utils (2024):** 2-year social engineering attack to introduce a backdoor into a core Linux compression library.

---

## 2. Dependency Confusion Attacks

```
Your company has internal packages published to a private registry:
  @mycompany/internal-auth (version 2.1.0)

Attack:
  Attacker publishes a MALICIOUS package to the PUBLIC npm registry
  with the SAME name: @mycompany/internal-auth (version 9.9.0)

  npm's default behavior:
    If the public registry has a higher version → install from public registry
    → Attacker's malicious package installed instead of your internal one

Affected: Apple, Microsoft, PayPal, Tesla, Uber, and many others (2021 research by Alex Birsan)
```

**Defenses:**

- Use a private registry with scope reservations (npm, Verdaccio, Artifactory)
- Configure npm to always use private registry for scoped packages
- Add `@mycompany` scope to the public npm registry (even if unused) so no one else can claim it
- Pin exact versions in package-lock.json

---

## 3. Typosquatting

Attacker publishes packages with names similar to popular ones, hoping developers mistype:

```
Real:      express      →  Typo: expres, expresss, expres-validator
Real:      lodash       →  Typo: loadash, lodahs
Real:      cross-env    →  Typo: crossenv, cross_env
Real:      jsonwebtoken →  Typo: json-web-token, jsonweb-token
```

These packages often contain legitimate-looking code with an added payload that exfiltrates environment variables (including secrets) to an external server.

**Defense:**

- Double-check package names before `npm install`
- Use organization-scoped packages where possible
- Audit `package.json` after `npm install` — did the right packages get added?

---

## 4. Malicious Package Updates

Legitimate packages can be compromised via:

- Maintainer account takeover (weak password, no 2FA on npm account)
- Malicious new maintainer added (social engineering)
- Protestware (maintainer intentionally introduces malware)
- Supply chain compromise of the maintainer's development machine

**Defense:**

- Pin exact versions in `package-lock.json` (lockfile integrity)
- Review changelogs before upgrading major/minor versions
- Monitor security advisories for packages you use
- Avoid auto-merging dependency PRs without review for critical dependencies

---

## 5. Known Vulnerability Exploitation

Older versions of packages contain known CVEs (Common Vulnerabilities and Exposures). Attackers scan for and exploit these.

```
CVE-2021-23369: handlebars prototype pollution
CVE-2021-44228: Log4Shell (Java — but illustrates the class)
CVE-2022-37601: loader-utils prototype pollution (affected webpack, Next.js)
CVE-2023-44270: postcss line return parsing — affected many build tools

Running outdated dependencies → known exploits → attacker has documented paths in
```

---

## 6. npm audit

Built into npm. Checks your dependencies against the npm advisory database.

```bash
npm audit
# Lists known vulnerabilities in your dependencies
# Output:
#   high     1
#   moderate  3
#   low       8

npm audit --json
# Machine-readable output for CI integration

npm audit fix
# Automatically upgrades affected packages to fixed versions
# (only where semver-compatible — won't break your API)

npm audit fix --force
# Upgrades even with breaking changes — review carefully
```

### Reading audit output

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│               moderate severity vulnerability               │
│                                                             │
│   Path Traversal in some-package                           │
│   Package:         some-package                            │
│   Dependency of:   express-fileupload                      │
│   Path:            express > express-fileupload > some-pkg │
│   More info:       https://github.com/advisories/GHSA-...  │
│                                                             │
└─────────────────────────────────────────────────────────────┘

Severity levels:
  critical  → fix immediately — remote code execution, data breach possible
  high      → fix urgently — significant impact
  moderate  → fix in next update cycle
  low       → fix when convenient
```

---

## 7. Snyk

Snyk provides deeper vulnerability scanning than npm audit:

```bash
npm install -g snyk
snyk auth        # authenticate with Snyk account
snyk test        # scan current project
snyk monitor     # continuous monitoring (re-scans when new advisories published)

# Also scans:
# - Docker images: snyk container test myimage:latest
# - Infrastructure as Code: snyk iac test
```

Snyk has a larger vulnerability database than npm audit and often finds issues earlier. The free tier is sufficient for open-source projects.

---

## 8. Dependabot

GitHub's built-in dependency update automation.

```yaml
# .github/dependabot.yml
version: 2
updates:
  - package-ecosystem: "npm"
    directory: "/"
    schedule:
      interval: "weekly" # or "daily", "monthly"
    open-pull-requests-limit: 10
    # Automatically opens PRs when dependencies have security updates
    # or when newer versions are available
```

Dependabot:

1. Monitors your `package.json` against known vulnerabilities
2. Opens PRs to upgrade vulnerable packages
3. Includes changelogs and release notes in the PR description

**For security updates:** merge these quickly. For non-security updates: review changelogs, run tests, then merge.

---

## 9. Lockfiles

`package-lock.json` (npm) or `yarn.lock` records the exact version and integrity hash of every installed package — including all transitive dependencies.

```json
// package-lock.json excerpt
{
  "express": {
    "version": "4.18.2",
    "resolved": "https://registry.npmjs.org/express/-/express-4.18.2.tgz",
    "integrity": "sha512-5/PsL6iGOs..."
    // SHA-512 hash of the package tarball
  }
}
```

**Integrity hash:** Before installing, npm verifies the package hash. If the package was tampered with on the registry (a compromised maintainer changed the package contents), the hash won't match → installation fails.

### Lockfile Rules

```
✅ Always commit package-lock.json to version control
   → Ensures everyone installs the same versions
   → CI/CD uses exact versions

✅ Use npm ci in CI/CD (not npm install)
   npm ci: installs exactly from lockfile, fails if lockfile doesn't match package.json
   npm install: may update lockfile on install

✅ Review lockfile changes in PRs
   Unexpected changes to lockfile = possible supply chain attack

❌ Never commit node_modules
   → Not needed if lockfile is committed
   → node_modules is 100x larger than the lockfile
```

---

## 10. Minimizing Dependency Risk

### Fewer Dependencies = Smaller Attack Surface

```
Before adding a package, ask:
  → Can I implement this with stdlib (built-in Node.js modules)?
     - crypto, fs, path, url, http — cover a lot
  → How many transitive dependencies does this add?
     npx package-phobia <package-name>  → shows install size and dep count
  → When was it last updated? Does it have active maintenance?
  → How many downloads? (Popular packages get more security scrutiny)
  → Does it have a security policy / response history?
```

```bash
# Check a package's dependencies before installing
npx npm-why some-package    # why is this package in your tree?
npx depcheck                # find unused dependencies
npm ls --depth=0            # list direct dependencies
npm ls some-package         # find which package requires this
```

### Vendoring Critical Dependencies

For truly critical dependencies (auth libraries, crypto), consider vendoring — copying the source into your repository and updating manually. You have full control over changes but must track security advisories yourself.

### Runtime Protection

```ts
// Limit what a compromised package can do
// Use separate processes for untrusted code
// Limit network access (firewall rules, no outbound except known endpoints)

// Environment variables to protect:
// Compromise of any package could exfiltrate process.env
// → Use secrets management (see file 21) instead of env vars for most sensitive keys
// → Rotate secrets regularly (compromise detected → rotate → attacker loses access)
```

---

## Summary

```
Supply chain attacks:
  Dependency confusion:  attacker publishes higher-version package to public registry
  Typosquatting:         similar names to popular packages
  Account compromise:    maintainer's npm account hacked → malicious version published
  Intentional malware:   maintainer or contributor introduces backdoor

Scanning tools:
  npm audit:    built-in, checks npm advisory database
  Snyk:         larger database, continuous monitoring, Docker/IaC scanning
  Dependabot:   GitHub integration, automatic security PRs

Lockfiles:
  package-lock.json records exact versions + integrity hashes
  Integrity check prevents tampered packages from installing
  Always commit lockfile; use npm ci in CI/CD

Minimizing risk:
  Use stdlib where possible
  Fewer dependencies = smaller attack surface
  Check maintenance status before adding packages
  Review lockfile changes in PRs
  Audit package names for typos before installing
```
