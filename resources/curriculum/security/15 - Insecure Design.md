> Some vulnerabilities live at the design level — not in a specific line of code, but in how the system was conceived. Mass assignment, IDOR, broken function-level authorization, SSRF, and security misconfiguration are all design failures.

---

## Table of Contents

1. [Mass Assignment](#1-mass-assignment)
2. [IDOR — Insecure Direct Object Reference](#2-idor--insecure-direct-object-reference)
3. [Broken Function Level Authorization](#3-broken-function-level-authorization)
4. [Security Misconfiguration](#4-security-misconfiguration)
5. [SSRF — Server-Side Request Forgery](#5-ssrf--server-side-request-forgery)
6. [Insecure Deserialization](#6-insecure-deserialization)
7. [Sensitive Data Exposure](#7-sensitive-data-exposure)

---

## 1. Mass Assignment

The application allows clients to set fields they shouldn't be able to set, by directly assigning request body properties to a model.

```ts
// VULNERABLE — raw body directly into create
@Post("users")
async create(@Body() body: any) {
  return this.db.user.create({ data: body });
}

// Normal request: { name: "Alice", email: "...", password: "..." }
// Attack request:  { name: "Alice", email: "...", password: "...", role: "admin", isVerified: true }
// User is created as an admin with a verified email → instant privilege escalation
```

### Defense: Strict DTOs with Whitelist

```ts
// DTO explicitly declares only the fields the client is allowed to set
class CreateUserDto {
  @IsString() @MaxLength(100) name: string;
  @IsEmail() email: string;
  @IsString() @MinLength(8) password: string;
  // role, isVerified, schoolId → NOT in DTO → cannot be set by client
}

// Controller — DTO automatically strips undeclared fields (whitelist: true)
@Post("users")
async create(@Body() dto: CreateUserDto) {
  return this.userService.create(dto);
  // dto only has { name, email, password } — nothing else possible
}

// Service — sets sensitive fields explicitly
async create(dto: CreateUserDto) {
  const hash = await bcrypt.hash(dto.password, 12);
  return this.db.user.create({
    data: {
      name: dto.name,
      email: dto.email,
      passwordHash: hash,
      role: UserRole.STUDENT,   // always set server-side, never from client
      isVerified: false,        // always set server-side
      schoolId: this.currentSchoolId,  // always from context, never from client
    },
  });
}
```

Global `ValidationPipe({ whitelist: true })` ensures any property not in the DTO is stripped. `forbidNonWhitelisted: true` rejects the request entirely if extra properties are present.

---

## 2. IDOR — Insecure Direct Object Reference

A user accesses a resource by directly referencing its ID — and the server doesn't verify the user is allowed to access that specific resource.

```
Authenticated student Alice:
  GET /api/grades/550  → her own grades → 200 OK

Alice modifies the ID:
  GET /api/grades/551  → another student's grades

Without ownership check: returns Bob's grades to Alice.
This is IDOR — the only "attack" is changing a number in a URL.
```

### Why It Happens

```ts
// VULNERABLE — validates auth, but not ownership
@Get("grades/:id")
@UseGuards(JwtAuthGuard)    // ✅ must be logged in
async getGrades(@Param("id") id: string) {
  return this.db.gradeRecord.findUnique({ where: { id } });
  // No check that this grade record belongs to the requesting user!
}
```

### Defense: Always Verify Ownership

```ts
// CORRECT — verify the resource belongs to the requesting user
@Get("grades/:id")
@UseGuards(JwtAuthGuard)
async getGrades(@Param("id") id: string, @CurrentUser() user: JwtPayload) {
  const record = await this.db.gradeRecord.findFirst({
    where: {
      id,
      studentId: user.sub,      // must belong to this student
      schoolId: user.schoolId,  // must belong to this school
    },
  });

  if (!record) throw new NotFoundException(); // not found OR not authorized — same response
  return record;
}
```

Return `404 Not Found`, not `403 Forbidden` — don't confirm the resource exists but the user can't access it. That leaks information about other users' data.

### CUID IDs — Not a Defense Alone

Using non-sequential IDs (CUIDs, UUIDs) makes IDOR harder to exploit by guessing:

```
Sequential: 550, 551, 552 — trivial to enumerate
CUID:       clxyz1234567890abcd — 2^120 possibilities — guessing is impractical
```

But this is not a substitute for proper ownership checks. An attacker with one valid ID (perhaps from an API response) can directly access that resource without ownership verification.

---

## 3. Broken Function Level Authorization

Attackers invoke functions (endpoints) they don't have permission for — typically admin functions.

```
Application has:
  GET  /api/users          → public endpoint, lists users
  POST /api/admin/users    → admin endpoint, creates users
  DELETE /api/admin/users/:id  → admin endpoint, deletes users

Broken:
  POST /api/admin/users has no role check
  Any authenticated user can create users → privilege escalation
```

```ts
// VULNERABLE — missing guard on admin endpoint
@Controller("admin/users")
export class AdminUserController {
  @Post()
  // ← No guard here! Any authenticated user can create admin users
  async create(@Body() dto: CreateUserDto) {
    return this.userService.createAdmin(dto);
  }

  @Delete(":id")
  @UseGuards(JwtAuthGuard, RolesGuard)
  @Roles(UserRole.ADMIN)  // ✅ protected
  async delete(@Param("id") id: string) { ... }
}

// CORRECT — apply guard at controller level (protects all routes)
@Controller("admin/users")
@UseGuards(JwtAuthGuard, RolesGuard)
@Roles(UserRole.ADMIN)
export class AdminUserController {
  // All routes in this controller automatically require ADMIN role
  @Post() create() { ... }         // ✅ inherits controller-level guard
  @Delete(":id") delete() { ... }  // ✅ inherits controller-level guard
}
```

Apply guards at the **controller level** for admin controllers — all routes are automatically protected. Individual route `@Roles()` decorators are easy to forget; controller-level is not.

---

## 4. Security Misconfiguration

Insecure defaults, unnecessary features enabled, unpatched systems, exposed debug endpoints.

### Common Misconfigurations

```
Debug mode in production:
  Stack traces in error responses reveal code structure, file paths, library versions
  NODE_ENV=production → suppress detailed errors

Default credentials:
  Admin:admin, postgres:postgres, root: (empty)
  Always change defaults on first setup

Unnecessary features enabled:
  MongoDB with no authentication
  PostgreSQL listening on 0.0.0.0 (public internet)
  Redis with no password
  All should be internal-network only

Exposed management endpoints:
  /actuator (Spring Boot health/metrics)
  /__debug
  /swagger-ui in production (reveals full API surface)

Directory listing enabled:
  Nginx: autoindex on; → lists all files in a directory
  Should be: autoindex off;

Missing security headers:
  X-Frame-Options, CSP, HSTS not set
  → Clickjacking, XSS, SSL stripping attacks possible

Verbose error messages:
  "User not found" vs "Invalid credentials"
  Database error messages exposed to client
  File path in stack traces
```

### Hardened Production Config

```ts
// NestJS — suppress internal errors from client
@Catch()
export class GlobalExceptionFilter implements ExceptionFilter {
  catch(exception: unknown, host: ArgumentsHost) {
    const ctx = host.switchToHttp();
    const response = ctx.getResponse<Response>();

    if (exception instanceof HttpException) {
      response.status(exception.getStatus()).json(exception.getResponse());
    } else {
      // Don't expose internal errors to client
      this.logger.error(exception);
      response
        .status(500)
        .json({ statusCode: 500, message: "Internal server error" });
    }
  }
}
```

```nginx
# Hide Nginx version from Server header
server_tokens off;

# Disable directory listing
autoindex off;

# Only expose necessary ports (via firewall)
# PostgreSQL (5432), Redis (6379) — only accessible from app server, not public
```

---

## 5. SSRF — Server-Side Request Forgery

The application fetches a URL provided by the user — and the attacker provides an internal URL the server can reach but the attacker normally can't.

```
Vulnerability:
  POST /api/fetch-preview
  { "url": "https://user-provided-url.com/image.jpg" }

  Server fetches the URL and returns content.

Attack:
  { "url": "http://169.254.169.254/latest/meta-data/iam/security-credentials/role" }
  → This is the AWS EC2 metadata endpoint — only accessible from the server itself
  → Returns the server's AWS credentials
  → Attacker now has full AWS access

  { "url": "http://localhost:5432" }      → probes internal PostgreSQL
  { "url": "http://10.0.0.1/admin" }     → accesses internal admin panel
  { "url": "file:///etc/passwd" }        → reads local files (some libraries)
```

### Defense

```ts
import { URL } from "url";
import { isIP } from "net";

function isSafeUrl(urlString: string): boolean {
  let url: URL;
  try {
    url = new URL(urlString);
  } catch {
    return false; // invalid URL
  }

  // Only allow HTTPS
  if (url.protocol !== "https:") return false;

  // Block private/loopback/link-local IP ranges
  const hostname = url.hostname;
  const privateRanges = [
    /^localhost$/i,
    /^127\./,
    /^10\./,
    /^172\.(1[6-9]|2[0-9]|3[01])\./,
    /^192\.168\./,
    /^169\.254\./, // link-local (AWS metadata)
    /^::1$/, // IPv6 loopback
    /^fd[0-9a-f]{2}:/i, // IPv6 private
  ];

  if (privateRanges.some((r) => r.test(hostname))) return false;

  // Only allow expected domains if possible (allowlist is better than blocklist)
  const allowedDomains = ["images.example.com", "cdn.trusted.com"];
  if (
    !allowedDomains.some((d) => hostname === d || hostname.endsWith(`.${d}`))
  ) {
    return false;
  }

  return true;
}
```

**Allowlist is better than blocklist** — blocklists can be bypassed with IP encoding tricks (`0x7f000001` = 127.0.0.1, IPv6 formats, DNS rebinding). If possible, only fetch from a known list of safe domains.

---

## 6. Insecure Deserialization

Deserializing untrusted data can lead to object injection, remote code execution, or privilege escalation.

```
PHP Object Injection:
  Attacker sends a serialized PHP object with a modified class
  PHP deserializes it → creates the object → calls __wakeup() → executes attacker code

Java Deserialization:
  Libraries like Apache Commons Collections had gadget chains
  Crafted serialized objects → execute arbitrary code on deserialization

Node.js:
  JSON.parse() is safe — plain data, no code execution
  node-serialize (old library) was vulnerable — could execute JS on deserialize

  Safer: only deserialize JSON. Never eval() serialized data.
  Validate structure after parse:
```

```ts
// After parsing JSON — validate it matches expected structure
const data = JSON.parse(rawInput);
// data is now a plain object — JSON.parse can't execute code

// But: validate the structure before using it
if (!isValidStructure(data)) throw new BadRequestException();

// Use class-transformer to safely deserialize into typed objects
import { plainToClass } from "class-transformer";
import { validateOrReject } from "class-validator";

const dto = plainToClass(CreateUserDto, data);
await validateOrReject(dto); // validates types and constraints
```

---

## 7. Sensitive Data Exposure

Returning more data than necessary — API responses with internal fields, hidden fields, or data the client doesn't need.

```ts
// VULNERABLE — returns entire user object including password hash
async getUser(id: string) {
  return this.db.user.findUnique({ where: { id } });
  // Returns: { id, email, passwordHash, totpSecret, ... }
}

// CORRECT — explicit field selection
async getUser(id: string) {
  return this.db.user.findUnique({
    where: { id },
    select: {
      id: true,
      name: true,
      email: true,
      role: true,
      // passwordHash: false  ← excluded by not including it
      // totpSecret: false
    },
  });
}

// Or use a response DTO
class UserResponseDto {
  id: string;
  name: string;
  email: string;
  role: UserRole;
  // No password, no totpSecret, no internal fields
}
```

Also applies to:

- Error messages containing stack traces, file paths, DB queries
- Log entries containing passwords or tokens
- API responses with fields intended for internal use
- Debug endpoints left enabled in production

---

## Summary

```
Mass assignment:
  Client sets fields they shouldn't → privilege escalation
  Defense: strict DTOs with whitelist:true

IDOR:
  Changing an ID in a URL accesses another user's resource
  Defense: always verify ownership in service layer
  Return 404 (not 403) to avoid leaking resource existence

Broken function-level auth:
  Admin endpoints without role guards
  Defense: apply guards at controller level, not just route level

Security misconfiguration:
  Debug mode in prod, default credentials, exposed internals
  Defense: security review checklist, separate prod config

SSRF:
  Server fetches attacker-controlled URL → reaches internal services
  Defense: allowlist of safe domains, block private IP ranges

Insecure deserialization:
  Crafted serialized objects execute code on deserialization
  Defense: only JSON (safe), validate structure after parsing

Sensitive data exposure:
  API returns more than needed (password hashes, internal fields)
  Defense: explicit field selection in queries, response DTOs
```

---
