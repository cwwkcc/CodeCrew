> Logging is how you know something happened. Monitoring is how you know something is wrong right now. Together they form the only way to detect breaches, investigate incidents, and prove what occurred. This covers what to log, what never to log, how to structure logs, and how to build meaningful alerts.

---

## Table of Contents

1. [Why Logging Matters for Security](#1-why-logging-matters-for-security)
2. [What to Log](#2-what-to-log)
3. [What Never to Log](#3-what-never-to-log)
4. [Log Structure and Format](#4-log-structure-and-format)
5. [Audit Logs vs Application Logs](#5-audit-logs-vs-application-logs)
6. [Log Integrity and Tampering](#6-log-integrity-and-tampering)
7. [Sensitive Data in Logs](#7-sensitive-data-in-logs)
8. [Alerting — What to Alert On](#8-alerting--what-to-alert-on)
9. [Log Retention](#9-log-retention)
10. [Implementing Logging in NestJS](#10-implementing-logging-in-nestjs)
11. [Common Logging Mistakes](#11-common-logging-mistakes)

---

## 1. Why Logging Matters for Security

Without logs, a breach is invisible until the damage is done. With logs, you can:

```
Detect:      "There have been 500 failed login attempts for alice@school.lk in the last 10 minutes"
Investigate: "The attacker logged in at 14:32, accessed the student records table at 14:34,
              and exported 2,400 rows at 14:37. They came from IP 1.2.3.4."
Prove:       "These are the exact records accessed, by whom, and when" — for GDPR compliance
Recover:     "The attack started at 14:32 — restore from the 14:00 backup"
```

OWASP lists "Insufficient Logging and Monitoring" as one of the top 10 web application security risks — not because logging is hard, but because most teams log too little or log the wrong things.

The average time to detect a breach is 207 days (IBM Cost of a Data Breach Report). Proper monitoring can collapse that to minutes.

---

## 2. What to Log

### Authentication Events

```
Every login attempt (success and failure):
  timestamp, userId or email, IP address, user agent, result (success/failure/locked)

Why: brute force detection, credential stuffing detection, impossible travel detection

Logout:
  timestamp, userId, sessionId, reason (user-initiated, timeout, forced by admin)

Password changes:
  timestamp, userId, IP, whether it was self-service or admin-initiated

MFA events:
  TOTP verification success/failure
  Backup code usage (always alert on this)
  MFA setup, MFA disabled

Account lockout:
  timestamp, userId, triggering IP, lockout duration
```

### Authorization Events

```
Access denied (403):
  timestamp, userId, requested resource, their role, required permission

  Why: pattern of 403s from one user = they're probing for access beyond their role

Privilege escalation attempts:
  Any time a user tries to access a resource above their role

Admin actions:
  Every action an admin takes (role changes, account creation, deletion, resets)
  Admins have the most power — their actions need the most scrutiny
```

### Data Access Events (Audit Log)

```
For sensitive data, log every access:
  timestamp, userId, role, action (read/write/delete/export), resourceType, resourceId, schoolId

Examples:
  Teacher viewed student health record: { action: "read", resource: "health_record", studentId: "..." }
  Admin exported student list: { action: "export", resource: "students", count: 450, schoolId: "..." }
  User changed another user's password: { action: "password_reset", targetUserId: "...", initiatedBy: "..." }
```

### Application Events

```
Errors and exceptions:
  All 5xx errors with full stack trace
  All unhandled promise rejections
  Database connection failures

Rate limit hits:
  Which endpoint, which IP/user, how far over the limit

Suspicious patterns:
  Same IP hitting login for many different accounts
  Requests with malformed tokens (could be probing)
  Requests to non-existent endpoints (scanning)

Configuration changes:
  Environment variable changes (if detectable)
  Feature flag changes
  Deployment events (new version deployed — timestamps matter for incident correlation)
```

---

## 3. What Never to Log

Just as important as what to log.

### Credentials and Secrets

```
NEVER log:
  Passwords (plaintext or hashed)
  API keys
  JWT tokens (full token — contains the user's identity and can be replayed)
  Refresh tokens
  Session IDs
  Encryption keys
  Private keys

Why this matters:
  Logs are often stored in less secure locations than application databases
  Logs are shipped to third-party services (Datadog, Logtail, Papertrail)
  Logs are often readable by more people than the database
  A leaked log file containing JWT tokens = every active session compromised
```

### Sensitive Personal Data

```
NEVER log (or log only partially/masked):
  Full credit card numbers   → log only last 4 digits
  Social Security / NIC numbers → don't log at all
  Passwords (again — worth repeating)
  Health/medical data
  Full email addresses in high-volume logs → can be useful in audit logs, not app logs
  IP addresses in some jurisdictions are PII (GDPR) → consider hashing or truncating
```

### Request/Response Bodies (Unless Carefully Redacted)

```
Logging full request bodies is tempting for debugging.
Request bodies often contain:
  → Login credentials (POST /auth/login body has the password)
  → Personal information (registration forms)
  → Payment data
  → Sensitive form fields

If you log request bodies:
  Redact known sensitive fields before logging:
  const safeBody = redact(body, ["password", "token", "creditCard", "ssn"]);

Better: log only the shape of the request (which fields exist, not their values)
        for debugging purposes.
```

---

## 4. Log Structure and Format

### Structured Logging (JSON)

Plain text logs are difficult to query and parse at scale. Structured JSON logs are machine-readable and can be indexed, filtered, and aggregated.

```json
{
  "timestamp": "2026-03-11T10:34:52.123Z",
  "level": "warn",
  "event": "auth.login.failed",
  "userId": null,
  "email": "alice@school.lk",
  "ip": "203.0.113.42",
  "userAgent": "Mozilla/5.0 ...",
  "reason": "invalid_password",
  "attemptCount": 3,
  "schoolId": null,
  "requestId": "req_abc123",
  "service": "auth-service",
  "environment": "production"
}
```

This log entry can be queried: "Show me all failed logins from IP 203.0.113.42 in the last hour."

### Fields to Include on Every Log Entry

```
timestamp    → ISO 8601 UTC (never local time — log aggregators use UTC)
level        → error, warn, info, debug
event        → dot-namespaced event name: auth.login.success, student.record.accessed
requestId    → unique per HTTP request (for correlating logs across services)
userId       → if authenticated (null if not)
schoolId     → tenant context (critical for multi-tenant isolation in logs too)
ip           → client IP
service      → which service emitted this (for microservices)
environment  → production, staging, development
```

### Log Levels

```
error   → something failed that shouldn't have
          5xx errors, database failures, unhandled exceptions
          These require immediate attention

warn    → something concerning but not broken
          Failed login attempts, rate limit hits, deprecated API usage
          Review these regularly

info    → normal significant events
          Login success, logout, password change, data export
          Audit log entries

debug   → detailed diagnostic information
          Query parameters, function entry/exit, cache hits/misses
          Should be DISABLED in production (too verbose, too much PII risk)
          Enable temporarily for debugging specific issues

verbose → even more granular than debug
          Definitely off in production
```

---

## 5. Audit Logs vs Application Logs

These serve different purposes and should be stored separately.

### Application Logs

```
Purpose: debugging, performance monitoring, error tracking
Audience: developers, ops team
Retention: 30–90 days
Storage: log aggregator (Datadog, Logtail, Loki)
Content: errors, warnings, request traces, timing data
Sensitivity: medium (redact PII but debug info can be verbose)
```

### Audit Logs

```
Purpose: compliance, security investigation, non-repudiation
Audience: security team, auditors, compliance officers, (never developers by default)
Retention: 1–7 years (GDPR minimum 1 year; financial regulations up to 7)
Storage: append-only, tamper-evident store (separate from app DB or write-once S3)
Content: who did what to which resource and when
Sensitivity: high — contains PII (who accessed whose record)
```

```ts
// Separate audit log table in the database
model AuditLog {
  id          String   @id @default(cuid())
  timestamp   DateTime @default(now())
  actorId     String   // who performed the action
  actorRole   String
  actorIp     String
  schoolId    String   // tenant context
  action      String   // "read", "write", "delete", "export", "login", "logout"
  resource    String   // "student_record", "health_data", "user_account"
  resourceId  String?  // the specific record affected
  metadata    Json?    // additional context (e.g., { exportedCount: 450 })
  // No updatedAt — audit logs are append-only, never modified
}
```

Audit logs should be **append-only** — no UPDATE or DELETE operations are ever performed on them. If an audit log can be deleted, it loses its value as evidence.

---

## 6. Log Integrity and Tampering

An attacker who has compromised a system may try to erase evidence from logs.

### Defenses

```
Separate log storage:
  Ship logs to an external service immediately
  Attacker would need to compromise both your server AND the log service
  Services: Datadog, Logtail, AWS CloudWatch, Loki + Grafana

Write-once storage:
  AWS S3 with Object Lock (WORM — Write Once Read Many)
  Logs written to S3 cannot be deleted or modified for the retention period
  Even if an attacker gets AWS credentials, WORM prevents deletion

Append-only log table:
  Database user for the application has INSERT permission but NOT UPDATE or DELETE on audit_logs
  CREATE ROLE app_logger;
  GRANT INSERT ON audit_logs TO app_logger;
  -- No GRANT UPDATE or DELETE

Centralized log aggregation:
  Log aggregation service receives and indexes logs
  Application fires and forgets — the log is shipped even if the app is immediately killed
```

### Log Chaining (Advanced)

Each log entry includes a hash of the previous entry (like a blockchain). Any deletion or modification breaks the chain and is immediately detectable.

```
entry_n.hash = SHA256(entry_n.content + entry_{n-1}.hash)
```

Complex to implement but provides strong tamper-evidence for high-security audit logs.

---

## 7. Sensitive Data in Logs

### Redaction

```ts
const SENSITIVE_FIELDS = [
  "password",
  "token",
  "refreshToken",
  "apiKey",
  "secret",
  "authorization",
  "cookie",
  "ssn",
  "creditCard",
];

function redactSensitive(
  obj: Record<string, unknown>,
): Record<string, unknown> {
  const result: Record<string, unknown> = {};
  for (const [key, value] of Object.entries(obj)) {
    if (SENSITIVE_FIELDS.some((f) => key.toLowerCase().includes(f))) {
      result[key] = "[REDACTED]";
    } else if (typeof value === "object" && value !== null) {
      result[key] = redactSensitive(value as Record<string, unknown>);
    } else {
      result[key] = value;
    }
  }
  return result;
}

// Usage in request logging middleware
logger.info({
  event: "http.request",
  method: req.method,
  path: req.path,
  body: redactSensitive(req.body), // safe to log
  headers: redactSensitive(req.headers), // Authorization header → [REDACTED]
});
```

### Partial Masking

For data that is useful to log partially:

```ts
// Email: log domain only for high-volume logs
function maskEmail(email: string): string {
  const [local, domain] = email.split("@");
  return `${local[0]}***@${domain}`; // "a***@school.lk"
}

// IP: log /24 prefix for approximate location without exact identity
function maskIp(ip: string): string {
  const parts = ip.split(".");
  return `${parts[0]}.${parts[1]}.${parts[2]}.xxx`; // "203.0.113.xxx"
}

// Token: log prefix only for identification without enabling replay
function maskToken(token: string): string {
  return `${token.slice(0, 12)}...`; // "eyJhbGciOiJI..."
}
```

---

## 8. Alerting — What to Alert On

Monitoring without alerting is just history — useful for investigation but not prevention.

### Immediate Alerts (PagerDuty / SMS)

```
Authentication:
  Account locked after repeated failures (n=10 in 10 minutes)
  Successful login from new country/impossible location
    (user logged in from Sri Lanka, then from Russia 5 minutes later → impossible travel)
  Admin account login outside business hours
  Backup code used (rare — potential account recovery or attack)
  Multiple accounts locked from same IP (credential stuffing)

Data:
  Bulk data export (> N records in single request)
  Admin accessed health/sensitive data records
  Database dump / unusual large SELECT

Infrastructure:
  5xx error rate spikes (> 5% of requests in 5 minutes)
  Database connection failures
  Secret rotation failure
  SSL certificate expiring in < 14 days
```

### Daily Review Alerts (Email Digest)

```
Failed login summary: which accounts, which IPs, volumes
403 Forbidden patterns: which users hit authorization limits
New admin accounts created
Permissions changed
Dependency vulnerabilities from automated scanning
```

### Threshold Alerting Example

```ts
// Redis-backed threshold tracking
async function trackFailedLogin(ip: string, email: string): Promise<void> {
  const ipKey = `failed_login:ip:${ip}`;
  const emailKey = `failed_login:email:${email}`;

  const [ipCount, emailCount] = await Promise.all([
    redis.incr(ipKey),
    redis.incr(emailKey),
  ]);

  // Set expiry on first increment
  if (ipCount === 1) await redis.expire(ipKey, 600); // 10 min window
  if (emailCount === 1) await redis.expire(emailKey, 600);

  // Alert thresholds
  if (ipCount === 50) {
    await alerting.send({
      severity: "high",
      title: "Possible credential stuffing",
      message: `IP ${ip} has failed login 50 times in 10 minutes`,
    });
  }

  if (emailCount === 10) {
    await alerting.send({
      severity: "medium",
      title: "Account brute force",
      message: `Account ${email} has 10 failed login attempts in 10 minutes`,
    });
    // Also: lock the account
  }
}
```

---

## 9. Log Retention

```
Application logs:    30–90 days
  (debugging value drops off quickly; storage costs add up)

Security logs:       1 year minimum
  (breach investigation may start months after the event)

Audit logs:          1–7 years depending on regulation
  GDPR:              at minimum as long as the personal data exists
  Financial records: 7 years in most jurisdictions
  School records:    check local education regulations

Access logs (HTTP):  90 days
  Enough for security investigation, not so long it becomes a PII liability
```

Storage costs: compress old logs. Text logs compress 10:1 with gzip. S3 Glacier for long-term archive.

---

## 10. Implementing Logging in NestJS

### Logger Setup with Winston

```ts
// logger.module.ts
import winston from "winston";

export const logger = winston.createLogger({
  level: process.env.NODE_ENV === "production" ? "info" : "debug",
  format: winston.format.combine(
    winston.format.timestamp(),
    winston.format.errors({ stack: true }),
    winston.format.json(), // structured JSON output
  ),
  transports: [
    new winston.transports.Console({
      silent: process.env.NODE_ENV === "test",
    }),
    // In production: also ship to external service
    // new DatadogTransport({ apiKey: process.env.DD_API_KEY }),
    // new winston.transports.Http({ host: "logs.logtail.com", ... }),
  ],
});
```

### Request Logging Middleware

```ts
// logging.middleware.ts
@Injectable()
export class LoggingMiddleware implements NestMiddleware {
  use(req: Request, res: Response, next: NextFunction) {
    const requestId = crypto.randomUUID();
    req["requestId"] = requestId;

    const start = Date.now();

    res.on("finish", () => {
      const duration = Date.now() - start;
      const user = req["user"] as JwtPayload | undefined;

      logger.info({
        event: "http.request",
        requestId,
        method: req.method,
        path: req.path,
        statusCode: res.statusCode,
        duration,
        ip: req.ip,
        userId: user?.sub ?? null,
        schoolId: user?.schoolId ?? null,
        userAgent: req.headers["user-agent"],
      });
    });

    next();
  }
}
```

### Audit Log Service

```ts
// audit.service.ts
@Injectable()
export class AuditService {
  constructor(private readonly prisma: PrismaService) {}

  async log(params: {
    actor: JwtPayload;
    action: string;
    resource: string;
    resourceId?: string;
    ip: string;
    metadata?: Record<string, unknown>;
  }): Promise<void> {
    await this.prisma.auditLog.create({
      data: {
        actorId: params.actor.sub,
        actorRole: params.actor.role,
        actorIp: params.ip,
        schoolId: params.actor.schoolId,
        action: params.action,
        resource: params.resource,
        resourceId: params.resourceId,
        metadata: params.metadata,
      },
    });
  }
}

// Usage in a service
async getHealthRecord(studentId: string, actor: JwtPayload, ip: string) {
  const record = await this.findHealthRecord(studentId, actor.schoolId);

  await this.auditService.log({
    actor,
    action: "read",
    resource: "health_record",
    resourceId: studentId,
    ip,
  });

  return record;
}
```

### Exception Logging

```ts
// all-exceptions.filter.ts
@Catch()
export class AllExceptionsFilter implements ExceptionFilter {
  catch(exception: unknown, host: ArgumentsHost) {
    const ctx = host.switchToHttp();
    const req = ctx.getRequest<Request>();
    const res = ctx.getResponse<Response>();

    const status =
      exception instanceof HttpException ? exception.getStatus() : 500;

    if (status >= 500) {
      logger.error({
        event: "http.error",
        requestId: req["requestId"],
        statusCode: status,
        path: req.path,
        userId: req["user"]?.sub ?? null,
        error:
          exception instanceof Error
            ? {
                message: exception.message,
                stack: exception.stack,
                name: exception.name,
              }
            : String(exception),
      });
    }

    res
      .status(status)
      .json({ statusCode: status, message: "Internal server error" });
  }
}
```

---

## 11. Common Logging Mistakes

### Logging Passwords on Failed Login

```ts
// WRONG
logger.warn(`Login failed for ${email} with password ${password}`);

// CORRECT — never log the password
logger.warn({ event: "auth.login.failed", email, reason: "invalid_password" });
```

### String Concatenation in Logs (Log Injection)

```ts
// WRONG — attacker can inject fake log entries
// Input: "alice\nINFO 2026-03-11 auth.login.success userId=admin"
logger.info("Login failed for: " + userInput);
// Produces a fake "success" log line in your logs

// CORRECT — structured logging prevents injection
logger.warn({ event: "auth.login.failed", email: userInput });
// userInput goes into the "email" field — it cannot create new log entries
```

### Logging Inside Tight Loops

```ts
// WRONG — logs 10,000 entries for a batch operation
for (const student of students) {
  logger.info(`Processing student ${student.id}`);
  await processStudent(student);
}

// CORRECT — log summary
logger.info({ event: "batch.start", count: students.length });
await Promise.all(students.map(processStudent));
logger.info({
  event: "batch.complete",
  count: students.length,
  duration: elapsed,
});
```

### Not Logging 4xx Errors

```ts
// Many teams only log 5xx. But 4xx patterns reveal attacks:
// Many 401s = credential stuffing
// Many 403s = privilege probing
// Many 404s to API paths = endpoint scanning

// Log all 4xx too, at lower severity
if (status >= 400 && status < 500) {
  logger.info({ event: "http.client_error", statusCode: status, path: req.path, ... });
}
```

### Ignoring Log Volume

Debug-level logging in production can generate gigabytes per day, making it:

- Expensive to store
- Slow to search
- Full of noise that hides real signals

Set production log level to `info` minimum. Enable `debug` only temporarily with a feature flag.

---

## Summary

```
Why logging matters:
  Detection (see attacks happening), investigation (what happened),
  compliance (prove what was accessed), recovery (when did it start?)

What to log:
  All auth events (login, logout, failures, MFA, lockouts)
  All admin actions
  All access to sensitive data (audit log)
  All 4xx and 5xx errors
  Rate limit hits, suspicious patterns

What NEVER to log:
  Passwords, tokens, API keys, session IDs, encryption keys
  Full SSNs, full credit card numbers
  Request bodies without redaction

Log structure:
  JSON format — machine-readable, queryable
  Every entry: timestamp (UTC), level, event name, requestId, userId, schoolId

Audit logs vs application logs:
  Application: debugging, 30-90 days, log aggregator
  Audit: compliance/security, 1-7 years, append-only, tamper-evident

Alerting:
  Immediate: account lockout, impossible travel, bulk export, admin after-hours login
  Daily digest: failed login summary, 403 patterns, new admins

NestJS implementation:
  Winston for structured JSON logging
  Middleware for request/response logging (redact sensitive fields)
  AuditService for append-only audit records
  AllExceptionsFilter for 5xx error logging

Mistakes to avoid:
  Logging credentials (check every logger.* call near auth code)
  String concatenation (enables log injection)
  Logging inside loops (use summaries)
  Ignoring 4xx errors (they reveal attack patterns)
```
