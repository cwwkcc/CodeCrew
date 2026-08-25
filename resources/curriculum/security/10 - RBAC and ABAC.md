> Access control decides who can do what. Role-Based Access Control (RBAC) is the most widely used model. Attribute-Based Access Control (ABAC) is more powerful. This covers both in depth — how they work, how to implement them, and how to combine them.

---

## Table of Contents

1. [Access Control Models Overview](#1-access-control-models-overview)
2. [RBAC — Role-Based Access Control](#2-rbac--role-based-access-control)
3. [Hierarchical RBAC](#3-hierarchical-rbac)
4. [RBAC Implementation in NestJS](#4-rbac-implementation-in-nestjs)
5. [ABAC — Attribute-Based Access Control](#5-abac--attribute-based-access-control)
6. [Policy Languages (ReBAC, PBAC)](#6-policy-languages-rebac-pbac)
7. [Hybrid RBAC + ABAC](#7-hybrid-rbac--abac)
8. [Row-Level Security](#8-row-level-security)
9. [Common Authorization Mistakes](#9-common-authorization-mistakes)

---

## 1. Access Control Models Overview

```
DAC — Discretionary Access Control
  Resource owners control access to their own resources
  Like a file system: you own a file, you set read/write/execute permissions
  Example: Google Drive folder sharing

MAC — Mandatory Access Control
  System enforces access based on classification labels
  Users can't override — even if you "own" a file, security policy controls access
  Military systems: Confidential, Secret, Top Secret
  Not common in web apps

RBAC — Role-Based Access Control
  Permissions attached to roles; users assigned roles
  Most common in web applications
  Simple to understand and manage at scale

ABAC — Attribute-Based Access Control
  Permissions based on attributes of user, resource, and environment
  "A user with role=admin AND department=HR can view salary data IF on internal network"
  More powerful than RBAC, more complex to implement

ReBAC — Relationship-Based Access Control
  Access based on relationships between entities
  "You can view a document if you are the owner OR are in a group that was shared with"
  Google Zanzibar (Google's authorization system) is a ReBAC system
```

---

## 2. RBAC — Role-Based Access Control

### Core Components

```
Users          → have roles
Roles          → have permissions
Permissions    → allow operations on resources

User: Alice
Roles: [TEACHER, LIBRARIAN]
Permissions: read:students (via TEACHER) + manage:books (via LIBRARIAN)
```

### Flat RBAC

Simplest form: a user has one or more roles, each role has a fixed set of permissions.

```ts
enum Permission {
  READ_OWN_PROFILE = "read:own_profile",
  READ_ANY_PROFILE = "read:any_profile",
  WRITE_OWN_PROFILE = "write:own_profile",
  WRITE_ANY_PROFILE = "write:any_profile",
  MANAGE_USERS = "manage:users",
  VIEW_AUDIT_LOG = "view:audit_log",
  MANAGE_LIBRARY = "manage:library",
  VIEW_HEALTH_DATA = "view:health_data",
}

const ROLE_PERMISSIONS: Record<UserRole, Permission[]> = {
  [UserRole.STUDENT]: [
    Permission.READ_OWN_PROFILE,
    Permission.WRITE_OWN_PROFILE,
  ],
  [UserRole.TEACHER]: [
    Permission.READ_OWN_PROFILE,
    Permission.WRITE_OWN_PROFILE,
    Permission.READ_ANY_PROFILE, // own class students
  ],
  [UserRole.ADMIN]: [
    Permission.READ_OWN_PROFILE,
    Permission.WRITE_OWN_PROFILE,
    Permission.READ_ANY_PROFILE,
    Permission.WRITE_ANY_PROFILE,
    Permission.MANAGE_USERS,
    Permission.VIEW_AUDIT_LOG,
    Permission.VIEW_HEALTH_DATA, // + step-up required
  ],
  // ...
};

// Guard
function hasPermission(userRole: UserRole, required: Permission): boolean {
  return ROLE_PERMISSIONS[userRole].includes(required);
}
```

---

## 3. Hierarchical RBAC

Roles inherit permissions from other roles (role inheritance).

```
ADMIN inherits from STAFF
STAFF inherits from USER

USER:      read:self, write:self
STAFF:     + read:school_data, access:intranet
ADMIN:     + manage:users, manage:settings, view:audit_log

ADMIN has all USER + STAFF + ADMIN permissions automatically
```

```ts
const ROLE_HIERARCHY: Record<UserRole, UserRole[]> = {
  [UserRole.STUDENT]: [],
  [UserRole.TEACHER]: [UserRole.STUDENT], // teacher inherits student permissions
  [UserRole.LIBRARIAN]: [UserRole.STAFF],
  [UserRole.ADMIN]: [UserRole.LIBRARIAN, UserRole.TEACHER],
};

function getEffectivePermissions(role: UserRole): Set<Permission> {
  const permissions = new Set<Permission>(ROLE_PERMISSIONS[role] ?? []);
  for (const parentRole of ROLE_HIERARCHY[role] ?? []) {
    for (const perm of getEffectivePermissions(parentRole)) {
      permissions.add(perm);
    }
  }
  return permissions;
}
```

---

## 4. RBAC Implementation in NestJS

```ts
// roles.decorator.ts
export const ROLES_KEY = "roles";
export const Roles = (...roles: UserRole[]) => SetMetadata(ROLES_KEY, roles);

// permissions.decorator.ts
export const PERMISSIONS_KEY = "permissions";
export const RequirePermissions = (...permissions: Permission[]) =>
  SetMetadata(PERMISSIONS_KEY, permissions);

// rbac.guard.ts
@Injectable()
export class RbacGuard implements CanActivate {
  constructor(private reflector: Reflector) {}

  canActivate(context: ExecutionContext): boolean {
    const requiredPermissions = this.reflector.getAllAndOverride<Permission[]>(
      PERMISSIONS_KEY,
      [context.getHandler(), context.getClass()]
    );

    if (!requiredPermissions?.length) return true;

    const { user } = context.switchToHttp().getRequest();
    const effectivePermissions = getEffectivePermissions(user.role);

    return requiredPermissions.every(p => effectivePermissions.has(p));
  }
}

// Usage
@Get("audit-log")
@RequirePermissions(Permission.VIEW_AUDIT_LOG)
getAuditLog() { ... }
```

---

## 5. ABAC — Attribute-Based Access Control

ABAC evaluates policies based on attributes of the **subject** (user), **resource**, **action**, and **environment**.

```
Policy example:
  "A user can view a student's health record IF:
    user.role = ADMIN
    AND user.schoolId = student.schoolId
    AND user.hasStepUpAuth = true
    AND environment.time is within school hours"

Attributes:
  Subject:     user.role, user.schoolId, user.department, user.stepUpExpiry
  Resource:    student.schoolId, student.grade, record.classification
  Action:      read, write, delete
  Environment: current time, IP address, request origin
```

```ts
interface AuthContext {
  user: JwtPayload & { stepUpExpiry?: Date };
  resource: Record<string, unknown>;
  action: "read" | "write" | "delete";
  environment: {
    time: Date;
    ipAddress: string;
  };
}

type Policy = (ctx: AuthContext) => boolean;

// Policy definitions
const policies: Record<string, Policy> = {
  "view:health_data": (ctx) =>
    ctx.user.role === UserRole.ADMIN &&
    ctx.user.schoolId === ctx.resource["schoolId"] &&
    ctx.user.stepUpExpiry !== undefined &&
    ctx.user.stepUpExpiry > new Date(),

  "view:own_grades": (ctx) =>
    ctx.user.role === UserRole.STUDENT &&
    ctx.user.sub === ctx.resource["studentId"],

  "view:class_grades": (ctx) =>
    ctx.user.role === UserRole.TEACHER &&
    ctx.user.schoolId === ctx.resource["schoolId"] &&
    ctx.user.sub === ctx.resource["teacherId"],
};

function canAccess(policyName: string, ctx: AuthContext): boolean {
  const policy = policies[policyName];
  if (!policy) return false;
  return policy(ctx);
}
```

---

## 6. Policy Languages (ReBAC, PBAC)

### ReBAC — Relationship-Based Access Control

Access based on relationships between entities in a graph.

```
"User A can view Document D if:
  A is the owner of D
  OR A is a member of Group G which has viewer access to D
  OR A has been directly shared D with viewer access"

Google Zanzibar model:
  Tuple: (object, relation, user)
  e.g.: (doc:123, viewer, user:alice)
        (doc:123, viewer, group:team-a#member)  ← everyone in group

Check: "Can alice view doc:123?"
  → Check tuples
  → alice is a member of team-a
  → team-a has viewer access to doc:123
  → Yes
```

Open-source implementations: OpenFGA, Oso, SpiceDB, Casbin.

### PBAC — Policy-Based Access Control

Explicit policy rules in a policy language (like OPA — Open Policy Agent).

```rego
# OPA Rego policy
package app.authz

default allow = false

allow {
    input.user.role == "admin"
    input.user.school_id == input.resource.school_id
    input.action == "read"
}

allow {
    input.user.role == "student"
    input.user.id == input.resource.student_id
    input.action == "read"
}
```

OPA policies are evaluated externally — your app asks OPA "can user X do Y to resource Z?" OPA evaluates the policy and returns allow/deny. Decouples authorization logic from application code.

---

## 7. Hybrid RBAC + ABAC

Most real systems use RBAC for coarse-grained access (role gates at endpoints) and ABAC for fine-grained access (row-level, attribute-level checks within the service).

```
Layer 1 (RBAC — route guard):
  GET /api/students/:id
  Allowed roles: ADMIN, TEACHER, STUDENT, PARENT
  Block: LIBRARIAN, STAFF

Layer 2 (ABAC — service layer):
  ADMIN:   can view any student in their school
  TEACHER: can view students in their classes only
  STUDENT: can view only themselves
  PARENT:  can view only their children

The route guard is fast and coarse.
The service check is precise and enforces the actual policy.
Both are necessary — the guard alone isn't granular enough; the service check alone misses the route-level protection.
```

---

## 8. Row-Level Security

Row-level security (RLS) enforces data access at the database query level.

### Application-Level RLS (Most Common)

```ts
// Every query automatically includes the school's context
async findStudents(user: JwtPayload, filters: StudentFilters) {
  const where: Prisma.UserWhereInput = {
    schoolId: user.schoolId,   // ALWAYS filter by school — never omit
    role: UserRole.STUDENT,
    ...buildRoleBasedFilter(user, filters),
  };

  return this.prisma.user.findMany({ where });
}

function buildRoleBasedFilter(user: JwtPayload, filters: StudentFilters) {
  if (user.role === UserRole.ADMIN) {
    return {}; // admin sees all students in their school
  }
  if (user.role === UserRole.TEACHER) {
    return {
      enrollments: {
        some: { class: { teacherId: user.sub } },
      },
    };
  }
  if (user.role === UserRole.STUDENT) {
    return { id: user.sub };
  }
  throw new ForbiddenException();
}
```

### Database-Level RLS (PostgreSQL)

PostgreSQL supports RLS natively — policies defined in the database ensure records are filtered even if application code forgets.

```sql
-- Enable RLS on users table
ALTER TABLE users ENABLE ROW LEVEL SECURITY;

-- Policy: users can only see records from their school
CREATE POLICY school_isolation ON users
  USING (school_id = current_setting('app.current_school_id')::uuid);

-- In application: set the school context before queries
-- (via a custom Prisma middleware or direct SQL)
SET app.current_school_id = '...';
SELECT * FROM users; -- automatically filtered by school_id
```

Database RLS is an additional safety net — it enforces isolation even if application code has a bug. Defense in depth.

---

## 9. Common Authorization Mistakes

### Enforcing Only at the Route Level

```ts
// WRONG — only checking role at the controller, not verifying resource ownership
@Get(":id")
@Roles(UserRole.STUDENT)  // any student can call this...
async getProfile(@Param("id") id: string) {
  return this.userService.findById(id);  // ...and get ANY user's profile!
}

// CORRECT — check at service layer too
@Get(":id")
@Roles(UserRole.STUDENT, UserRole.ADMIN)
async getProfile(@Param("id") id: string, @CurrentUser() user: JwtPayload) {
  return this.userService.findByIdWithOwnerCheck(id, user);
}
```

### Trusting Client-Provided IDs for Filtering

```ts
// WRONG — schoolId comes from request body
async getStudents(@Body("schoolId") schoolId: string) {
  return this.db.findMany({ where: { schoolId } }); // attacker can request any school
}

// CORRECT — schoolId always from verified JWT
async getStudents(@CurrentUser() user: JwtPayload) {
  return this.db.findMany({ where: { schoolId: user.schoolId } }); // from signed token
}
```

### Forgetting Authorization on Write Operations

```ts
// WRONG — only protecting reads
@Get(":id")           @Roles(UserRole.ADMIN)  // ✅ protected
@Post()               @Roles(UserRole.ADMIN)  // ✅ protected
@Put(":id")           // ← unprotected! anyone can update!
@Delete(":id")        @Roles(UserRole.ADMIN)  // ✅ protected

// Always protect PUT, PATCH, DELETE — not just GET and POST
```

---

## Summary

```
DAC: owner controls access (files)
MAC: system controls by classification (military)
RBAC: role controls access (most web apps)
ABAC: attributes control access (complex policies)
ReBAC: relationships control access (Google Docs model)

RBAC:
  Users → Roles → Permissions
  Simple, scalable, easy to understand
  Hierarchical RBAC: role inheritance

ABAC:
  Policies over subject + resource + action + environment attributes
  More flexible, more complex
  Good for: time-based access, department-based, step-up auth conditions

Hybrid RBAC + ABAC:
  RBAC at route level (coarse-grained, fast)
  ABAC at service level (fine-grained, precise)

Row-level security:
  Application layer: always include schoolId/userId in WHERE clauses
  Database layer: PostgreSQL RLS as defense in depth

Common mistakes:
  Authorization only at route level (not service level)
  Trusting client-provided IDs for resource scoping
  Forgetting to protect write operations
```

---
