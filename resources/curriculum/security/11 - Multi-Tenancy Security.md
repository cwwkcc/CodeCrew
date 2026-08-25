> Multi-tenancy means one application serves multiple isolated customers (tenants). The biggest risk: one tenant accessing another's data. This covers isolation models, the patterns that prevent cross-tenant access, and what makes a multi-tenant security breach so severe.

---

## Table of Contents

1. [Multi-Tenancy Models](#1-multi-tenancy-models)
2. [The Cross-Tenant Threat](#2-the-cross-tenant-threat)
3. [Database Isolation Strategies](#3-database-isolation-strategies)
4. [Application-Level Tenant Isolation](#4-application-level-tenant-isolation)
5. [Tenant Context Propagation](#5-tenant-context-propagation)
6. [Testing Tenant Isolation](#6-testing-tenant-isolation)
7. [Multi-Tenant File Storage](#7-multi-tenant-file-storage)

---

## 1. Multi-Tenancy Models

```
Silo model (database-per-tenant):
  Each tenant has their own database
  Complete isolation — SQL queries physically can't cross tenants
  Expensive — database overhead per tenant
  Easier compliance (tenant data physically separate)
  Example: Large enterprise SaaS

Shared schema model (row-level isolation):
  All tenants share tables
  Every row has a tenantId column
  Application must filter by tenantId on every query
  Cheap — one database for all tenants
  Isolation is entirely application-enforced
  Risk: forgetting tenantId in a query → data leak
  Most common for web applications

Hybrid (schema-per-tenant):
  One database, separate schema per tenant
  Moderate isolation, moderate cost
  PostgreSQL supports this natively
```

---

## 2. The Cross-Tenant Threat

```
Scenario:
  Tenant A: School A
  Tenant B: School B
  Both have users with role ADMIN

  Admin from School A is authenticated:
  JWT: { sub: "userA", schoolId: "school_A", role: "admin" }

  Admin A manipulates a request:
  GET /api/students?schoolId=school_B   (putting school_B's ID in the query)

  Without tenant isolation:
    Query: SELECT * FROM users WHERE schoolId = 'school_B' → returns School B's students
    Admin A just read School B's private student data

  This is cross-tenant data leakage — a critical security failure.
```

Real consequences:

- GDPR violations — data processors must ensure tenant data isolation
- Loss of customer trust
- Regulatory fines
- Possible legal action

---

## 3. Database Isolation Strategies

### Row-Level — schoolId on Every Table

```sql
CREATE TABLE users (
  id         TEXT PRIMARY KEY,
  school_id  TEXT NOT NULL REFERENCES schools(id),  -- EVERY table has this
  email      TEXT NOT NULL,
  role       TEXT NOT NULL,
  ...
);

CREATE INDEX idx_users_school_id ON users(school_id);
-- Every query filters by school_id → performance matters → index it
```

### Prisma Schema with Tenant Isolation

```prisma
model User {
  id        String @id @default(cuid())
  schoolId  String                         // never nullable
  school    School @relation(fields: [schoolId], references: [id])
  email     String
  role      UserRole
  // ...
}

model Book {
  id        String @id @default(cuid())
  schoolId  String                         // library is per-school
  // ...
}

// EVERY entity that belongs to a school has schoolId
// The only entities without schoolId: School itself, global config
```

---

## 4. Application-Level Tenant Isolation

### The SchoolGuard Pattern

```ts
// school.guard.ts
@Injectable()
export class SchoolGuard implements CanActivate {
  constructor(private reflector: Reflector) {}

  canActivate(context: ExecutionContext): boolean {
    const req = context.switchToHttp().getRequest();
    const user = req.user as JwtPayload;

    // Attach schoolId from JWT to request for use in services
    // This prevents any controller/service from using a schoolId from the request body
    req.tenantId = user.schoolId;

    return true;
  }
}

// Apply globally so it ALWAYS runs
// main.ts
app.useGlobalGuards(new JwtAuthGuard(), new SchoolGuard());
```

### Enforcing in Services

```ts
// base.service.ts — every service extends this
@Injectable()
export abstract class TenantAwareService {
  constructor(protected readonly prisma: PrismaService) {}

  protected getTenantFilter(schoolId: string) {
    if (!schoolId)
      throw new InternalServerErrorException("Tenant context missing");
    return { schoolId };
  }
}

// user.service.ts
@Injectable()
export class UserService extends TenantAwareService {
  async findAll(schoolId: string): Promise<User[]> {
    return this.prisma.user.findMany({
      where: {
        ...this.getTenantFilter(schoolId), // ALWAYS called
        deletedAt: null,
      },
    });
  }

  async findById(id: string, schoolId: string): Promise<User | null> {
    return this.prisma.user.findFirst({
      where: {
        id,
        ...this.getTenantFilter(schoolId),
        // Note: findFirst, not findUnique
        // findUnique only uses the primary key (id) without schoolId constraint
        // findFirst can use compound conditions including schoolId
      },
    });
  }
}
```

### The Critical Difference: findFirst vs findUnique

```ts
// WRONG — findUnique only looks up by primary key
// schoolId is NOT used in the WHERE clause
const user = await prisma.user.findUnique({
  where: { id: targetId }, // finds user regardless of school
});
// An admin from School A can get School B's user if they know the ID!

// CORRECT — findFirst uses compound conditions
const user = await prisma.user.findFirst({
  where: {
    id: targetId,
    schoolId: requestingUser.schoolId, // must match requesting user's school
  },
});
// Returns null if user doesn't belong to the requesting user's school
```

---

## 5. Tenant Context Propagation

The tenant context (schoolId) must flow from the JWT through every layer of the application without the application code being able to "forget" it.

```ts
// Controller — extracts schoolId from JWT user
@Get(":id")
async getUser(
  @Param("id") id: string,
  @CurrentUser() user: JwtPayload  // schoolId is in here
) {
  return this.userService.findById(id, user.schoolId);
  // schoolId is explicitly passed down
}

// Service — uses it in query
async findById(id: string, schoolId: string) {
  return this.prisma.user.findFirst({
    where: { id, schoolId },  // both conditions enforced
  });
}
```

Alternative: use AsyncLocalStorage to propagate context implicitly (similar to how request-scoped context works). More ergonomic but hides the dependency.

---

## 6. Testing Tenant Isolation

Tenant isolation MUST be tested with cross-tenant scenarios:

```ts
describe("UserService - tenant isolation", () => {
  let schoolA: School;
  let schoolB: School;
  let adminA: User;

  beforeEach(async () => {
    schoolA = await createSchool("School A");
    schoolB = await createSchool("School B");
    adminA = await createUser({ schoolId: schoolA.id, role: "admin" });
    await createUser({
      schoolId: schoolB.id,
      role: "student",
      email: "bob@b.lk",
    });
  });

  it("should not return users from another school", async () => {
    const users = await userService.findAll(schoolA.id);
    const schoolBUser = users.find((u) => u.schoolId === schoolB.id);
    expect(schoolBUser).toBeUndefined();
  });

  it("should return null when fetching cross-school user by ID", async () => {
    const schoolBUser = await db.user.findFirst({
      where: { schoolId: schoolB.id },
    });
    const result = await userService.findById(schoolBUser.id, schoolA.id);
    expect(result).toBeNull(); // Not 403 — just null, don't reveal existence
  });

  it("should not leak school B users in search", async () => {
    const results = await userService.search("bob", schoolA.id);
    expect(results).toHaveLength(0); // bob is in school B, invisible to school A
  });
});
```

---

## 7. Multi-Tenant File Storage

Files must also be tenant-isolated.

```ts
// File naming strategy — never let tenants share paths
function getFilePath(schoolId: string, userId: string, filename: string): string {
  const sanitized = path.basename(filename); // strip directory traversal
  return `schools/${schoolId}/users/${userId}/${Date.now()}-${sanitized}`;
  // schools/school_A/users/user_123/1741691200-photo.jpg
}

// Cloudflare R2 or AWS S3 — use bucket policies or path-based isolation
// Ensure each school's files are under a school-scoped prefix

// Signed URL generation — always verify school ownership before signing
async getSignedUrl(fileId: string, requestingSchoolId: string): Promise<string> {
  const file = await this.prisma.mediaFile.findFirst({
    where: {
      id: fileId,
      schoolId: requestingSchoolId,  // tenant check before signing URL
    },
  });
  if (!file) throw new NotFoundException();

  return this.r2.getSignedUrl(file.path);
}
```

---

## Summary

```
Multi-tenancy models:
  Silo (DB per tenant): strong isolation, expensive
  Shared schema (row-level): cheap, risk of query errors, most common
  Hybrid (schema per tenant): middle ground

Cross-tenant breach consequences:
  Data leak between competing customers, GDPR violations, legal liability

Isolation patterns:
  schoolId on every tenant-scoped table
  Always use findFirst with schoolId condition (not findUnique)
  schoolId always from JWT — never from request body
  SchoolGuard attaches tenantId to request context

Defense in depth:
  Application layer: explicit schoolId in all queries
  Database layer: PostgreSQL RLS as backup
  Testing: dedicated cross-tenant tests

File storage:
  Tenant-scoped path prefix: schools/{schoolId}/...
  Verify school ownership before generating signed URLs
```

---
