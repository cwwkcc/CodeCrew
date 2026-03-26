> The database is the most valuable target. Every other security control exists to protect what's in there. This covers database user permissions, encryption at rest, connection security, injection prevention, and backup security.

---

## Table of Contents

1. [Principle of Least Privilege for DB Users](#1-principle-of-least-privilege-for-db-users)
2. [Network Isolation](#2-network-isolation)
3. [Encryption at Rest](#3-encryption-at-rest)
4. [Encryption at the Field Level](#4-encryption-at-the-field-level)
5. [Connection Security](#5-connection-security)
6. [SQL Injection at the DB Layer](#6-sql-injection-at-the-db-layer)
7. [Audit Logging in PostgreSQL](#7-audit-logging-in-postgresql)
8. [Backup Security](#8-backup-security)
9. [Soft Deletes and Data Retention](#9-soft-deletes-and-data-retention)

---

## 1. Principle of Least Privilege for DB Users

```sql
-- Create a dedicated application user (not postgres/root)
CREATE USER paideon_app WITH PASSWORD 'strong_random_password_here';

-- Grant only what the application needs
GRANT CONNECT ON DATABASE paideon TO paideon_app;
GRANT USAGE ON SCHEMA public TO paideon_app;
GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA public TO paideon_app;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO paideon_app;

-- DO NOT GRANT:
-- SUPERUSER       → full database admin
-- CREATEDB        → can create new databases
-- CREATEROLE      → can create new users
-- DROP TABLE      → can destroy tables
-- TRUNCATE        → can wipe tables without audit trail

-- For future tables (when migrations run):
ALTER DEFAULT PRIVILEGES IN SCHEMA public
  GRANT SELECT, INSERT, UPDATE, DELETE ON TABLES TO paideon_app;
ALTER DEFAULT PRIVILEGES IN SCHEMA public
  GRANT USAGE, SELECT ON SEQUENCES TO paideon_app;
```

If the application is compromised via SQL injection, the attacker is limited to SELECT/INSERT/UPDATE/DELETE — they can't DROP tables, CREATE users, or execute OS commands.

### Separate Users for Different Access Levels

```sql
-- Read-only user for reporting queries / analytics
CREATE USER paideon_reader WITH PASSWORD '...';
GRANT CONNECT ON DATABASE paideon TO paideon_reader;
GRANT USAGE ON SCHEMA public TO paideon_reader;
GRANT SELECT ON ALL TABLES IN SCHEMA public TO paideon_reader;

-- Migration user — used only by Prisma migrate (CI/CD pipeline)
CREATE USER paideon_migrator WITH PASSWORD '...';
GRANT ALL PRIVILEGES ON DATABASE paideon TO paideon_migrator;
-- This user can ALTER TABLE, CREATE TABLE, DROP TABLE
-- But it's only used during deployments, not at runtime
```

---

## 2. Network Isolation

```
PostgreSQL default: listens on all interfaces (0.0.0.0)
This means it's accessible from the public internet if firewall isn't configured.

# postgresql.conf
listen_addresses = 'localhost'       # only localhost (single server)
# or:
listen_addresses = '10.0.0.5'       # specific internal IP

# pg_hba.conf — Client authentication
# TYPE  DATABASE  USER         ADDRESS       METHOD
local   all       postgres                   peer        # local unix socket, postgres OS user
host    paideon   paideon_app  10.0.0.0/24  scram-sha-256  # app server on internal network only
host    all       all          0.0.0.0/0   reject         # reject everything else
```

```
Firewall rules (Ubuntu ufw):
  ufw allow from 10.0.0.5 to any port 5432  # allow only from app server
  ufw deny 5432                              # deny everything else
  ufw enable
```

If using a managed database (AWS RDS, Supabase, Neon):

- Set VPC to only allow connections from your application's IP/security group
- Disable public accessibility
- SSL required

---

## 3. Encryption at Rest

**Full-disk encryption** protects against physical theft of server hardware.

```
Cloud providers: typically enabled by default
  AWS RDS: encrypted at rest using KMS (enable during creation)
  Google Cloud SQL: encrypted by default
  Supabase: encrypted at rest
  
VPS (bare metal server):
  LUKS (Linux Unified Key Setup) — encrypt the disk
  Data is encrypted when the server is off — useless to physical theft
  Application accesses data normally when server is running (transparent)
```

Full-disk encryption does NOT protect against:

- A running server being compromised
- SQL injection (database is accessible when running)
- Insider threats (DBA can read all data)

For those: use field-level encryption for the most sensitive data.

---

## 4. Encryption at the Field Level

The most sensitive fields (TOTP secrets, health data, confidential documents) can be encrypted at the application level — the database stores only ciphertext.

```ts
import { createCipheriv, createDecipheriv, randomBytes } from "crypto";

const FIELD_ENCRYPTION_KEY = Buffer.from(
  process.env.FIELD_ENCRYPTION_KEY, "hex"  // 32-byte key, stored in secrets manager
);

function encryptField(plaintext: string): string {
  const iv = randomBytes(12);
  const cipher = createCipheriv("aes-256-gcm", FIELD_ENCRYPTION_KEY, iv);
  
  let encrypted = cipher.update(plaintext, "utf8", "hex");
  encrypted += cipher.final("hex");
  const tag = cipher.getAuthTag().toString("hex");
  
  // Store: iv + tag + ciphertext (all needed for decryption)
  return `${iv.toString("hex")}:${tag}:${encrypted}`;
}

function decryptField(stored: string): string {
  const [ivHex, tagHex, encrypted] = stored.split(":");
  const iv = Buffer.from(ivHex, "hex");
  const tag = Buffer.from(tagHex, "hex");
  
  const decipher = createDecipheriv("aes-256-gcm", FIELD_ENCRYPTION_KEY, iv);
  decipher.setAuthTag(tag);
  
  let decrypted = decipher.update(encrypted, "hex", "utf8");
  decrypted += decipher.final("utf8");
  return decrypted;
}

// Prisma middleware — transparent encryption/decryption
const encryptedFields = {
  User: ["totpSecret"],
  StudentHealth: ["bloodType", "conditions", "medications"],
};
```

**Tradeoff:** Encrypted fields can't be queried/indexed efficiently. You can search by exact match (hash the value, store alongside ciphertext) but can't do LIKE/range queries. Design your schema accordingly.

---

## 5. Connection Security

```
DATABASE_URL must require SSL:
  postgresql://user:password@host:5432/db?sslmode=require

sslmode options:
  disable      → no SSL (never use in production)
  require      → require SSL, but don't verify the certificate
  verify-ca    → verify the CA signed the server certificate
  verify-full  → verify CA + hostname matches certificate (most secure)
```

```ts
// Prisma — SSL configuration
// prisma/schema.prisma
datasource db {
  provider = "postgresql"
  url      = env("DATABASE_URL")
}

// DATABASE_URL: postgresql://user:pass@host/db?sslmode=verify-full&sslrootcert=/path/to/ca.pem
// For managed DBs (Supabase, Railway): sslmode=require is typically sufficient
```

---

## 6. SQL Injection at the DB Layer

Even with parameterized queries in the application, some operations could be vulnerable if using dynamic SQL.

```sql
-- VULNERABLE — dynamic SQL in a stored procedure
CREATE FUNCTION get_user(search_term TEXT)
RETURNS TABLE(id TEXT, email TEXT) AS $$
BEGIN
  RETURN QUERY EXECUTE 'SELECT id, email FROM users WHERE email = ''' || search_term || '''';
END;
$$ LANGUAGE plpgsql;

-- SAFE — parameterized in stored procedure
CREATE FUNCTION get_user(search_term TEXT)
RETURNS TABLE(id TEXT, email TEXT) AS $$
BEGIN
  RETURN QUERY EXECUTE 'SELECT id, email FROM users WHERE email = $1'
  USING search_term;  -- parameterized
END;
$$ LANGUAGE plpgsql;
```

For Prisma: always use tagged template literals for raw queries:

```ts
// SAFE — tagged template parameterizes automatically
await prisma.$queryRaw`SELECT * FROM users WHERE email = ${email}`;

// DANGEROUS — regular template string concatenation
await prisma.$queryRawUnsafe(`SELECT * FROM users WHERE email = '${email}'`);
// $queryRawUnsafe is there for dynamic column/table names — validate carefully
```

---

## 7. Audit Logging in PostgreSQL

```sql
-- Enable pgaudit extension
CREATE EXTENSION pgaudit;

-- postgresql.conf
pgaudit.log = 'write, ddl'  -- log writes (INSERT/UPDATE/DELETE) and schema changes
pgaudit.log_client = on
pgaudit.log_level = log

-- This logs:
-- Every INSERT, UPDATE, DELETE with the full query and parameters
-- Every DDL statement (CREATE TABLE, DROP TABLE, ALTER TABLE)
-- Useful for: forensics after a breach, compliance requirements
```

Application-level audit log (AuditLog table in Prisma) covers business events. PostgreSQL-level audit covers raw database operations — useful if application audit logging is bypassed.

---

## 8. Backup Security

```
Backup strategy:
  Daily automated backups
  Stored in a DIFFERENT geographic region (not the same server)
  Encrypted at rest
  Tested monthly (restore to staging → verify data integrity)
  Retention: 30 days rolling (GDPR: don't keep data longer than needed)

What good backups protect against:
  → Accidental deletion (oops, dropped the table)
  → Ransomware (encrypted production data — restore from backup)
  → Hardware failure
  → Accidental data corruption

What backups don't protect against:
  → Data breach (backup is still a copy of the breached data)
  → Corruption that went undetected for 30+ days (past retention)
```

```bash
# PostgreSQL backup
pg_dump -U postgres -h localhost paideon | gzip > paideon_$(date +%Y%m%d).sql.gz

# Encrypt the backup before storing offsite
gpg --symmetric --cipher-algo AES256 paideon_20260311.sql.gz
# Move encrypted backup to separate storage (S3, R2, different cloud)

# Automated with pg_basebackup for larger DBs
# Or use managed DB service's built-in backup (Supabase, Railway, RDS)
```

---

## 9. Soft Deletes and Data Retention

```prisma
// Every model has deletedAt — no hard deletes
model User {
  id        String    @id @default(cuid())
  email     String    @unique
  deletedAt DateTime? // null = active; set = soft-deleted
}
```

```ts
// Soft delete — never DELETE, always UPDATE
async softDelete(id: string, schoolId: string): Promise<void> {
  await this.prisma.user.update({
    where: { id, schoolId },
    data: { deletedAt: new Date() },
  });
  // Record still exists — in audit trail, can be recovered
}

// All queries must exclude soft-deleted records
async findAll(schoolId: string) {
  return this.prisma.user.findMany({
    where: {
      schoolId,
      deletedAt: null,  // ALWAYS include this
    },
  });
}
```

Benefits:

- Accidental deletions are recoverable without a DB restore
- Full audit trail (who deleted it, when)
- Referential integrity maintained (foreign keys still valid)

---

## Summary

```
Principle of least privilege:
  Dedicated app user: SELECT, INSERT, UPDATE, DELETE only
  No SUPERUSER, no DROP, no TRUNCATE
  Separate read-only user for analytics
  Separate migration user (only used during deployments)

Network isolation:
  PostgreSQL listens on internal IP only
  Firewall blocks all external access to port 5432

Encryption:
  At rest: full-disk encryption (cloud default or LUKS)
  In transit: sslmode=require minimum, verify-full ideal
  Field-level: AES-256-GCM for most sensitive fields (TOTP secrets, health data)

Backups:
  Daily, separate region, encrypted, tested monthly
  30-day retention

Soft deletes:
  deletedAt field — never hard DELETE
  All queries filter deletedAt: null
  Recoverable, auditable
```

---
