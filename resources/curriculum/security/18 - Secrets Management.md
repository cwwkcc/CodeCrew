> Secrets are API keys, database passwords, JWT secrets, encryption keys — anything that grants access. Where and how you store them determines whether a breach exposes your entire infrastructure or just a limited blast radius.

---

## Table of Contents

1. [What Are Secrets?](#1-what-are-secrets)
2. [What Happens When Secrets Leak](#2-what-happens-when-secrets-leak)
3. [The .env File](#3-the-env-file)
4. [Environment Variables in Production](#4-environment-variables-in-production)
5. [Secrets Managers](#5-secrets-managers)
6. [Secret Scanning](#6-secret-scanning)
7. [Secret Rotation](#7-secret-rotation)
8. [JWT Secret Best Practices](#8-jwt-secret-best-practices)
9. [Database Credentials](#9-database-credentials)

---

## 1. What Are Secrets?

```
Secrets = anything that grants access or proves identity to a system

Database credentials:    DATABASE_URL, DB_PASSWORD
API keys:                STRIPE_SECRET_KEY, SENDGRID_API_KEY, R2_SECRET_KEY
Encryption keys:         FIELD_ENCRYPTION_KEY, FILE_ENCRYPTION_KEY
JWT secrets:             JWT_SECRET, REFRESH_TOKEN_SECRET
OAuth credentials:       GOOGLE_CLIENT_SECRET, GITHUB_CLIENT_SECRET
TOTP encryption key:     TOTP_ENCRYPTION_KEY
Third-party webhooks:    STRIPE_WEBHOOK_SECRET
SSH keys:                private keys for server access
TLS private keys:        certificate private keys
```

---

## 2. What Happens When Secrets Leak

```
Leaked DATABASE_URL:
  → Direct database access
  → Dump all student data, health records, credentials
  → GDPR breach notification required
  → School liability

Leaked JWT_SECRET:
  → Forge any JWT, impersonate any user including admins
  → Until rotated, every account in the system is compromised

Leaked STRIPE_SECRET_KEY:
  → Create charges, issue refunds, access customer data
  → Financial liability

Leaked AWS_ACCESS_KEY_ID + AWS_SECRET_ACCESS_KEY:
  → Full AWS account access
  → Spin up thousands of servers (cryptomining at your expense)
  → Access all data in S3/RDS
  → One breach cost a developer $50,000 AWS bill in one weekend
```

Secrets that are leaked via a git commit to a public repo are typically discovered within minutes by bots that scan GitHub continuously.

---

## 3. The .env File

For local development, `.env` files store secrets as environment variables.

```bash
# .env — NEVER commit this file
DATABASE_URL=postgresql://user:password@localhost:5432/paideon_dev
JWT_SECRET=k9X2m#pL8qR5nW3tYvZxB7cJhD4eA6fG
JWT_REFRESH_SECRET=another_random_32_char_string_here
TOTP_ENCRYPTION_KEY=yet_another_32_char_string_here
R2_ACCESS_KEY_ID=your_key_here
R2_SECRET_ACCESS_KEY=your_secret_here
```

```bash
# .gitignore — MUST include
.env
.env.local
.env.*.local
.env.production
*.pem
*.key
```

### .env.example — What to Commit

Commit a template with variable names but no values:

```bash
# .env.example — safe to commit
DATABASE_URL=postgresql://user:password@localhost:5432/paideon_dev
JWT_SECRET=<generate with: node -e "require('crypto').randomBytes(32).toString('hex')">
JWT_REFRESH_SECRET=<generate with: node -e "require('crypto').randomBytes(32).toString('hex')">
TOTP_ENCRYPTION_KEY=<generate with: node -e "require('crypto').randomBytes(32).toString('hex')">
R2_ACCESS_KEY_ID=<from R2 dashboard>
R2_SECRET_ACCESS_KEY=<from R2 dashboard>
```

New developers: copy `.env.example` → `.env`, fill in values. They know what's needed without having any actual secrets.

---

## 4. Environment Variables in Production

In production, secrets should come from the environment — not `.env` files.

```
Options for production:
  → Host platform env vars (Railway, Heroku, Vercel, Render)
    Set in the platform's dashboard → injected into process.env at startup
    
  → System environment (VPS/bare metal)
    /etc/environment or systemd service file
    Encrypted at rest if the host is configured correctly
    
  → Docker: docker run -e JWT_SECRET=... or docker-compose env_file
  
  → Kubernetes: Secrets objects (base64 encoded, optionally encrypted at rest)
```

```ts
// NestJS — ConfigModule validates required variables at startup
import Joi from "joi";

@Module({
  imports: [
    ConfigModule.forRoot({
      validationSchema: Joi.object({
        NODE_ENV: Joi.string().valid("development", "production", "test").required(),
        DATABASE_URL: Joi.string().required(),
        JWT_SECRET: Joi.string().min(32).required(),
        JWT_REFRESH_SECRET: Joi.string().min(32).required(),
        TOTP_ENCRYPTION_KEY: Joi.string().length(64).required(),  // 32 bytes hex = 64 chars
        PORT: Joi.number().default(3000),
      }),
    }),
  ],
})
```

If a required variable is missing at startup, the app refuses to start and logs which variable is missing. This catches misconfigured deployments immediately.

---

## 5. Secrets Managers

For production systems, a dedicated secrets manager is more secure than environment variables alone:

```
AWS Secrets Manager:
  → Stores secrets encrypted (KMS)
  → Automatic rotation built-in (Lambda function rotates DB passwords)
  → IAM-based access control (only the app's IAM role can retrieve secrets)
  → Audit log of every secret access

HashiCorp Vault:
  → Self-hosted or HCP (cloud)
  → Dynamic secrets (generates DB credentials on demand, revokes after TTL)
  → Policies for fine-grained access control
  → Many auth backends (Kubernetes, AWS IAM, etc.)

Infisical (open source):
  → Self-hostable Vault alternative
  → Native NestJS/Node.js SDK
  → Simpler than Vault for smaller teams

Doppler:
  → Managed secrets platform
  → CLI, SDK, auto-sync to Heroku/Railway/AWS
  → Simple UI for managing secrets across environments
```

### Fetching Secrets at Runtime

```ts
import { SecretsManagerClient, GetSecretValueCommand } from "@aws-sdk/client-secrets-manager";

const client = new SecretsManagerClient({ region: "ap-south-1" });

async function getSecret(secretName: string): Promise<Record<string, string>> {
  const response = await client.send(
    new GetSecretValueCommand({ SecretId: secretName })
  );
  return JSON.parse(response.SecretString);
}

// At app startup:
const secrets = await getSecret("paideon/production");
process.env.JWT_SECRET = secrets.JWT_SECRET;
process.env.DATABASE_URL = secrets.DATABASE_URL;
```

---

## 6. Secret Scanning

Detect secrets accidentally committed to version control.

### git-secrets / gitleaks (Pre-commit)

```bash
# Install gitleaks
brew install gitleaks  # macOS
# or download from: github.com/gitleaks/gitleaks

# Scan entire git history for leaked secrets
gitleaks detect --source . --verbose

# Run as pre-commit hook (prevents committing secrets)
cat .git/hooks/pre-commit
#!/bin/sh
gitleaks protect --staged
```

### GitHub Secret Scanning

GitHub scans all public repositories for known secret patterns:

- AWS access keys
- Stripe keys
- npm tokens
- GitHub tokens
- Many more

For private repos: Secret Scanning is available on GitHub Advanced Security. Found secrets are flagged and optionally reported to the service provider (who can revoke them).

### If a Secret Is Committed

```
Immediate response:
  1. Revoke/rotate the secret IMMEDIATELY
     (bots scan GitHub in real time — rotate before they find it)
  2. Assume it was seen and exploited
  3. Check for unauthorized access
  4. Clean the git history (git filter-branch or BFG Repo Cleaner)
  5. Force-push (if public) or delete and recreate the repository

git filter-repo --path .env --invert-paths
# Removes .env from entire git history
```

---

## 7. Secret Rotation

Regularly rotating secrets limits the damage from a secret that was silently compromised.

```
Rotation schedule:
  JWT_SECRET:         Rotate every 90 days (or immediately after security incident)
  Database passwords: Rotate every 90 days (with zero-downtime rollover)
  API keys:           Rotate annually or on personnel change
  TLS certificates:   Automated (Let's Encrypt 90-day auto-renewal)

Zero-downtime rotation for JWT_SECRET:
  1. Generate new secret: JWT_SECRET_NEW
  2. Update app to accept tokens signed with EITHER old or new secret
  3. Deploy
  4. Update JWT_SECRET to new value, remove JWT_SECRET_OLD acceptance
  5. All new tokens use new secret; old tokens expire within 15 minutes
  6. After 15 minutes: remove old secret acceptance entirely
```

---

## 8. JWT Secret Best Practices

```ts
// Generate a strong secret (do this once, store it securely)
import crypto from "crypto";
const secret = crypto.randomBytes(32).toString("hex");
// Output: "a3b4c5d6e7f8..."  (64-char hex = 256 bits)
// Never use a human-readable passphrase as the JWT secret

// ConfigService usage
@Injectable()
export class JwtConfigService {
  constructor(private config: ConfigService) {}

  get accessTokenSecret(): string {
    return this.config.getOrThrow("JWT_SECRET");
    // getOrThrow: throws if variable is missing — better than undefined
  }

  get accessTokenExpiry(): string {
    return "15m";
  }

  get refreshTokenSecret(): string {
    return this.config.getOrThrow("JWT_REFRESH_SECRET");
    // Separate secret for refresh tokens
    // If access secret leaks, refresh tokens are still safe
  }
}
```

Use separate secrets for access tokens and refresh tokens — different secrets = different blast radius if one leaks.

---

## 9. Database Credentials

```
Development: use a local DB with a simple password
  DATABASE_URL=postgresql://paideon:devpassword@localhost:5432/paideon_dev

Production: follow these rules:
  → Dedicated database user for the application (not the postgres superuser)
  → Application user has only the permissions it needs:
    GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES TO paideon_app;
    -- NOT: GRANT ALL PRIVILEGES, NOT: SUPERUSER
  → Database not accessible from the public internet
    (only accessible from application server on internal network)
  → Strong random password (32+ characters)
  → Password stored in secrets manager, not in env file on disk
  → SSL required for connection (sslmode=require in connection string)
```

```
DATABASE_URL=postgresql://paideon_app:STRONG_RANDOM_PASSWORD@db.internal:5432/paideon
  ?sslmode=require
  &connect_timeout=10
```

---

## Summary

```
Secrets: DB passwords, API keys, JWT secrets, encryption keys — treat all as critical

Never:
  Commit .env to version control
  Use weak passwords/secrets (human-readable phrases)
  Share secrets via Slack/email/tickets
  Log secrets (console.log(process.env) is dangerous)
  Use the same secret across environments

Always:
  .env in .gitignore
  .env.example with variable names (no values) committed
  Strong random secrets (crypto.randomBytes(32).toString("hex"))
  Separate secrets per environment (dev/staging/prod)
  Rotate on schedule and immediately after incidents

Production:
  Platform env vars or secrets manager (AWS Secrets Manager, Vault, Doppler)
  ConfigModule validates required vars at startup
  Pre-commit hooks with gitleaks prevent accidental commits
  GitHub secret scanning on all repositories
```

---
