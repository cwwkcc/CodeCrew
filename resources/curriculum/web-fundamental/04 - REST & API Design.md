> REST is an architectural style, not a protocol. Most "REST" APIs are actually just HTTP APIs — and that's fine. This part explains what REST actually means, how to design URLs and responses that will feel natural to any developer, versioning, pagination, error shapes, and the conventions used throughout Paideon's NestJS API.

---

## Table of Contents

1. [What REST Actually Is](#1-what-rest-actually-is)
2. [REST Constraints — The Six Rules](#2-rest-constraints--the-six-rules)
3. [Resource Naming — The Most Important Skill](#3-resource-naming--the-most-important-skill)
4. [HTTP Methods Mapped to CRUD](#4-http-methods-mapped-to-crud)
5. [Nested Resources and Relationships](#5-nested-resources-and-relationships)
6. [Query Parameters — Filtering, Sorting, Searching](#6-query-parameters--filtering-sorting-searching)
7. [API Versioning](#7-api-versioning)
8. [Response Shapes — Consistency Is Everything](#8-response-shapes--consistency-is-everything)
9. [Error Responses — How to Communicate Failure](#9-error-responses--how-to-communicate-failure)
10. [Pagination — Handling Large Collections](#10-pagination--handling-large-collections)
11. [Idempotency Keys — Safe Retries](#11-idempotency-keys--safe-retries)
12. [HATEOAS — The Part Nobody Implements](#12-hateoas--the-part-nobody-implements)
13. [REST vs GraphQL vs RPC](#13-rest-vs-graphql-vs-rpc)
14. [Paideon API Design Patterns](#14-paideon-api-design-patterns)

---

## 1. What REST Actually Is

REST (Representational State Transfer) was defined by Roy Fielding in his 2000 PhD dissertation. It is not a standard, not a spec, not a protocol. It is an **architectural style** — a set of constraints that, when followed, produce a scalable, interoperable system.

```
Most APIs described as "REST" or "RESTful" are not actually REST.
They are HTTP APIs with resource-based URLs.
This is fine. True REST (with HATEOAS) is rarely implemented.
"REST API" in the industry means "HTTP API with predictable URLs and JSON."

Fielding himself has complained publicly about this.
Understanding the ideal helps you make better practical decisions.
```

---

## 2. REST Constraints — The Six Rules

### 1. Client-Server

```
Client and server are separated. They evolve independently.
Client doesn't know how data is stored.
Server doesn't know how the UI is built.

In Paideon:
  Next.js (client) and NestJS (server) are separate processes.
  NestJS doesn't know Next.js exists — it just responds to HTTP requests.
  Next.js could be replaced with a mobile app. NestJS wouldn't change.
```

### 2. Stateless

```
Every request must contain ALL information the server needs.
The server stores no session state between requests.

Correct REST:
  GET /api/students HTTP/1.1
  Authorization: Bearer eyJhbGc...     ← who you are, in every request

Wrong (stateful):
  "Server, remember I'm an admin from my last request."
  The server should NOT need to remember anything.

Why stateless?
  Scale: any server instance can handle any request.
         No need to route a user to "their" server.
  Reliability: if a server restarts, no session is lost.
  Visibility: each request is self-describing (easier to debug, log, trace).

JWT tokens are stateless auth — the token itself contains the user's identity.
Sessions (server-side storage of who you are) are stateful — not strictly REST.
```

### 3. Cacheable

```
Responses must declare whether they can be cached.
Cache-Control, ETag, Last-Modified headers fulfill this.

If a response is cacheable:
  Client (or proxy) can reuse it for future identical requests.
  Reduces load on the server.
  Improves performance for the client.

GET requests should generally be cacheable.
POST/PUT/DELETE responses should not be cached (they modify state).
```

### 4. Uniform Interface

```
The interface between client and server is standardized.
Four sub-constraints:

a) Resource identification
   Resources are identified by URIs.
   /api/students/cuid-123 identifies one specific student.

b) Manipulation through representations
   Client manipulates resources through representations.
   The server's internal database model is irrelevant.
   Client sends JSON → server understands and applies it.

c) Self-descriptive messages
   Each message includes enough information to describe how to process it.
   Content-Type tells the receiver how to parse the body.

d) HATEOAS
   Responses include links to related actions/resources.
   (See section 12)
```

### 5. Layered System

```
Client doesn't know if it's talking to the final server or an intermediary.

In Paideon:
  Browser → Cloudflare → Nginx → NestJS

  The browser has no idea Cloudflare and Nginx exist.
  It just talks to paideon.lk:443.
  This is the layered system constraint.

  Intermediaries can add caching, security, load balancing.
  Client and server don't need to know about them.
```

### 6. Code on Demand (Optional)

```
Servers can send executable code to clients.
JavaScript served from a server is the canonical example.
The only optional constraint.
```

---

## 3. Resource Naming — The Most Important Skill

URLs should identify resources (nouns), not actions (verbs).

### Rules

```
Use nouns, not verbs:
  WRONG:  GET /api/getStudents
  WRONG:  POST /api/createStudent
  WRONG:  DELETE /api/deleteStudent?id=123

  RIGHT:  GET /api/students           ← list all students
  RIGHT:  POST /api/students          ← create a student
  RIGHT:  DELETE /api/students/123    ← delete student 123

Use plural for collections:
  RIGHT:  /api/students       (not /api/student)
  RIGHT:  /api/courses        (not /api/course)
  RIGHT:  /api/assignments    (not /api/assignment)

Use lowercase, hyphen-separated:
  RIGHT:  /api/exam-results
  WRONG:  /api/examResults    (camelCase in URLs is unusual)
  WRONG:  /api/exam_results   (underscores are valid but less common)
  WRONG:  /api/ExamResults    (PascalCase in URLs — never)

Use IDs (not names) to identify specific resources:
  RIGHT:  /api/students/cuid-123
  WRONG:  /api/students/ashan-silva  (names change, not unique)

Resource identifiers should be opaque:
  Don't expose auto-incrementing integers: /api/students/1, /2, /3
  Reasons:
    → Reveals total count (competitor intelligence)
    → Insecure direct object reference: easy to enumerate
    → CUID or UUID is better: /api/students/clwq4rz9k0000qk08h4j7g1p2
```

### URL Hierarchy Reflects Resource Hierarchy

```
/api/schools                          All schools
/api/schools/school-cuid              One school
/api/schools/school-cuid/students     Students in that school
/api/schools/school-cuid/students/student-cuid   One student in that school
/api/schools/school-cuid/classes      Classes in that school
/api/schools/school-cuid/classes/class-cuid      One class
/api/schools/school-cuid/classes/class-cuid/students   Students in that class
```

Don't nest too deeply. More than 3 levels is unwieldy.

```
Too deep:  /api/schools/s1/classes/c1/assignments/a1/submissions/sub1/grades/g1

Alternative: flatten with query parameters where nesting gets deep:
  /api/submissions?class=c1&assignment=a1&student=s1

Or: some resources have their own top-level endpoint:
  /api/grades/g1  (grade is a resource in its own right, not buried in submission)
```

---

## 4. HTTP Methods Mapped to CRUD

```
Resource: /api/students
Action                    HTTP Method    URL                    Status
─────────────────────────────────────────────────────────────────────────────
List all students         GET            /api/students          200 OK
Get one student           GET            /api/students/:id      200 OK
Create a student          POST           /api/students          201 Created
Replace a student         PUT            /api/students/:id      200 OK
Update a student          PATCH          /api/students/:id      200 OK
Delete a student          DELETE         /api/students/:id      204 No Content

Additional patterns:
─────────────────────────────────────────────────────────────────────────────
Check if exists           HEAD           /api/students/:id      200 or 404
List supported methods    OPTIONS        /api/students          204

Non-CRUD actions — use a sub-resource or action noun:
  POST /api/students/:id/suspend      → suspend (action as sub-resource)
  POST /api/students/:id/transfer     → transfer to another class
  POST /api/auth/logout               → action, not a resource
  POST /api/reports/generate          → trigger report generation
```

---

## 5. Nested Resources and Relationships

```
Scenario: Get all assignments for class 10A in school "abc-school".

Option A: Nested URL
  GET /api/schools/abc-school/classes/class-10a/assignments

Option B: Query parameters on a flat URL
  GET /api/assignments?school=abc-school&class=class-10a

When to use nesting:
  The child resource cannot exist without the parent.
  The relationship is primary (not optional).
  The nesting depth is at most 2-3.

  /api/schools/:schoolId/students   → students belong to a school
  /api/students/:id/grades          → grades belong to a student

When to use query parameters:
  You're filtering, not navigating to a child.
  The resource has multiple optional parent contexts.
  The nesting would be too deep.

Both options can be valid for the same resource.
Pick the one that makes more semantic sense.
Be consistent across your API.
```

---

## 6. Query Parameters — Filtering, Sorting, Searching

```
GET /api/students?class=10A&status=active&sort=name&order=asc&page=2&limit=20

Filtering:
  ?class=10A           → students in class 10A
  ?status=active       → only active students
  ?status=active,suspended  → multiple values for same field

Sorting:
  ?sort=name           → sort by name
  ?sort=name,createdAt → multi-field sort
  ?order=asc           → ascending (default)
  ?order=desc          → descending

  Alternative: ?sort=+name,-createdAt (+ for asc, - for desc)
  Pick one convention and stick to it.

Searching:
  ?q=ashan             → search across relevant fields
  ?search=ashan        → same concept, different name

  Be clear in documentation what fields are searched.

Pagination:
  ?page=2&limit=20     → offset-based: page 2, 20 per page
  ?cursor=xxx&limit=20 → cursor-based: after record xxx, 20 records
  (See section 10 for full pagination coverage)

Field selection (sparse fieldsets):
  ?fields=id,name,email   → return only these fields
  Reduces payload size when the client doesn't need all fields.
  Useful for list views vs detail views.

Date filtering:
  ?createdAfter=2026-01-01
  ?createdBefore=2026-03-13
  ISO 8601 format: YYYY-MM-DD or YYYY-MM-DDTHH:mm:ssZ
```

---

## 7. API Versioning

APIs evolve. Breaking changes must not break existing clients. Versioning is how you manage this.

### URL Path Versioning (Most Common)

```
/api/v1/students
/api/v2/students

Pros:
  Explicit — you can see the version in the URL.
  Easy to test in browser/curl.
  Proxies and CDNs can route by path.

Cons:
  "REST purists" argue the URL should identify the resource, not the version.
  Resources feel duplicated (/v1/ and /v2/ for same concept).

For Paideon: use URL versioning. Simple, clear, widely understood.
```

### Header Versioning

```
GET /api/students HTTP/1.1
API-Version: 2

Pros:
  URL stays clean.
  Resource URL is the same regardless of version.

Cons:
  Not visible in browser address bar.
  Harder to test without tools.
  Caching can be tricky (need Vary: API-Version header).
```

### Accept Header Versioning

```
GET /api/students HTTP/1.1
Accept: application/vnd.paideon.v2+json

Strict REST approach. Rarely used in practice.
```

### When to Version

```
Breaking changes that require a new version:
  → Removing a field from a response
  → Changing a field's type (string → number)
  → Changing a URL structure
  → Removing an endpoint
  → Changing required/optional status of a field

Non-breaking changes (no new version needed):
  → Adding a new optional field to a response
  → Adding a new optional query parameter
  → Adding a new endpoint
  → Making an optional field required with a backward-compatible default

NestJS versioning:
  app.enableVersioning({ type: VersioningType.URI });

  @Controller({ path: 'students', version: '1' })
  @Controller({ path: 'students', version: '2' })
```

---

## 8. Response Shapes — Consistency Is Everything

Pick a response shape and use it everywhere. Inconsistency is the number one API frustration.

### Single Resource

```json
{
  "data": {
    "id": "clwq4rz9k0000qk08h4j7g1p2",
    "name": "Ashan Silva",
    "email": "ashan@school.lk",
    "class": "10A",
    "createdAt": "2026-01-15T10:30:00Z",
    "updatedAt": "2026-03-01T14:22:00Z"
  }
}
```

### Collection

```json
{
  "data": [
    { "id": "...", "name": "Ashan Silva", ... },
    { "id": "...", "name": "Nimal Perera", ... }
  ],
  "meta": {
    "total": 147,
    "page": 1,
    "limit": 20,
    "totalPages": 8
  }
}
```

### Why Wrap in `data`?

```
{ "data": {...} }  vs  {...}

Wrapping allows you to add metadata alongside data:
  { "data": {...}, "meta": {...}, "links": {...} }

Without wrapper:
  Adding pagination metadata means changing the top-level shape → breaking change.

With wrapper:
  Adding meta is backwards-compatible.
  Clients reading data["data"] are unaffected by new top-level keys.

Industry divided on this. Pick one and be consistent.
NestJS apps typically return raw objects. Wrapping is a deliberate choice.
```

### Timestamps

```
Always use ISO 8601 UTC:
  "createdAt": "2026-03-13T08:23:14.123Z"

  Z means UTC (zero offset).
  Clients convert to local time if needed.
  Never return timestamps as epoch integers (harder to read, debug).
  Never return timezone-aware timestamps without UTC offset.
```

---

## 9. Error Responses — How to Communicate Failure

Error responses need as much design as success responses.

### Shape

```json
{
  "error": {
    "code": "STUDENT_NOT_FOUND",
    "message": "No student found with ID clwq4rz9k0000qk08h4j7g1p2",
    "details": null,
    "requestId": "req_01hw4rz9kf"
  }
}
```

### Validation Errors (422)

```json
{
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Request validation failed",
    "details": [
      {
        "field": "email",
        "message": "must be a valid email address",
        "received": "not-an-email"
      },
      {
        "field": "class",
        "message": "must be one of: 10A, 10B, 10C, 11A, 11B",
        "received": "12Z"
      }
    ],
    "requestId": "req_01hw4rz9kf"
  }
}
```

### Error Code Design

```
Use machine-readable codes (all caps, underscore-separated):
  STUDENT_NOT_FOUND
  INVALID_CREDENTIALS
  TOKEN_EXPIRED
  INSUFFICIENT_PERMISSIONS
  DUPLICATE_EMAIL
  RATE_LIMIT_EXCEEDED
  VALIDATION_ERROR
  INTERNAL_ERROR

Why machine-readable codes?
  Frontend can switch on the code:
    if (error.code === 'TOKEN_EXPIRED') { await refreshToken(); retry(); }
    if (error.code === 'RATE_LIMIT_EXCEEDED') { showRateLimitMessage(); }

  Human-readable messages can change without breaking the frontend.
  Error codes should be stable across API versions.

HTTP status tells you the category of error.
Error code tells you exactly what happened.
Message tells the human what happened.
Use all three.
```

### NestJS Error Handling

```typescript
// Custom exceptions
export class StudentNotFoundException extends NotFoundException {
  constructor(id: string) {
    super({
      error: {
        code: "STUDENT_NOT_FOUND",
        message: `No student found with ID ${id}`,
        details: null,
      },
    });
  }
}

// Global exception filter for consistent shape
@Catch()
export class GlobalExceptionFilter implements ExceptionFilter {
  catch(exception: unknown, host: ArgumentsHost) {
    const ctx = host.switchToHttp();
    const response = ctx.getResponse<FastifyReply>();
    const request = ctx.getRequest<FastifyRequest>();

    const status =
      exception instanceof HttpException ? exception.getStatus() : 500;

    const body =
      exception instanceof HttpException
        ? exception.getResponse()
        : {
            error: {
              code: "INTERNAL_ERROR",
              message: "An unexpected error occurred",
            },
          };

    response.status(status).send({
      ...body,
      // Ensure requestId is always present
      requestId: request.id,
    });
  }
}
```

---

## 10. Pagination — Handling Large Collections

Never return unbounded results. A school with 2,000 students should never respond to `GET /api/students` with 2,000 objects.

### Offset-Based Pagination

```
GET /api/students?page=3&limit=20

Page 1: records 1–20   (OFFSET 0, LIMIT 20)
Page 2: records 21–40  (OFFSET 20, LIMIT 20)
Page 3: records 41–60  (OFFSET 40, LIMIT 20)

SQL: SELECT * FROM students LIMIT 20 OFFSET 40

Response:
{
  "data": [...],
  "meta": {
    "total": 147,     ← requires a separate COUNT(*) query
    "page": 3,
    "limit": 20,
    "totalPages": 8,
    "hasNext": true,
    "hasPrev": true
  }
}

Pros:
  User can jump to arbitrary page (page=7).
  Total count lets you render "Page 3 of 8" or a page selector.
  Simple to implement.

Cons:
  Unstable: if a record is inserted between page 1 and page 2 fetches,
  some records appear twice (duplicate) or are skipped.
  Performance: OFFSET 10000 LIMIT 20 makes PostgreSQL scan 10,020 rows
  then discard the first 10,000. Slow on large datasets.
```

### Cursor-Based Pagination

```
GET /api/students?cursor=clwq4rz9k0000qk08h4j7g1p2&limit=20

First request: no cursor → get first 20 records
Response includes cursor for next page:
{
  "data": [...],
  "meta": {
    "limit": 20,
    "nextCursor": "clwq4rz9k9999qk08h4j7g1p2",
    "hasNext": true
  }
}

Next request: GET /api/students?cursor=clwq4rz9k9999...&limit=20
SQL: SELECT * FROM students WHERE id > 'clwq4rz9k9999...' ORDER BY id LIMIT 20

Pros:
  Stable: insertions/deletions don't cause duplicates or skips.
  Fast: uses index scan, no OFFSET overhead.
  Correct: real-time feeds (activity streams, notifications).

Cons:
  Cannot jump to arbitrary page.
  No total count (expensive to compute).
  Cursor must be opaque (base64-encoded position).

For Paideon:
  Student lists → offset pagination (stable dataset, user may want page N)
  Activity feeds, notifications → cursor pagination (real-time, append-only)
```

---

## 11. Idempotency Keys — Safe Retries

POST requests are not idempotent. If a network timeout occurs after the server processes the request but before the response reaches the client, retrying creates a duplicate.

```
Client sends: POST /api/payments { amount: 5000, ... }
Server: processes payment, deducts money, sends response
Network: drops the response packet
Client: sees timeout → retries
Server: processes payment AGAIN → double charge

Solution: Idempotency keys

Client sends:
  POST /api/payments HTTP/1.1
  Idempotency-Key: 01hw4rz9kf-payment-2026-03-13
  { amount: 5000, ... }

Server:
  Check if Idempotency-Key is in cache.
  Not found → process payment → cache result with this key (TTL: 24 hours).

Client retries (same key):
  POST /api/payments HTTP/1.1
  Idempotency-Key: 01hw4rz9kf-payment-2026-03-13  ← same key

Server:
  Finds key in cache → returns cached result immediately.
  Payment NOT processed again.

For Paideon:
  Relevant for: enrollment creation, grade submission, any create-once operation.
  Client generates UUID: const idempotencyKey = crypto.randomUUID()
  Store in localStorage before sending, clear after confirmed.
```

---

## 12. HATEOAS — The Part Nobody Implements

HATEOAS (Hypermedia as the Engine of Application State) is the final REST constraint that makes an API truly RESTful.

```
A HATEOAS response includes links to related actions:

GET /api/students/cuid-123

{
  "data": {
    "id": "cuid-123",
    "name": "Ashan Silva",
    "status": "active"
  },
  "links": {
    "self":    "/api/students/cuid-123",
    "classes": "/api/students/cuid-123/classes",
    "grades":  "/api/students/cuid-123/grades",
    "suspend": "/api/students/cuid-123/suspend",
    "school":  "/api/schools/school-abc"
  }
}

The client discovers what it can do next from the response itself.
It doesn't need to hardcode URLs.

In theory:
  A HATEOAS client needs to know only the entry point (/api).
  From there, it navigates by following links.
  API URLs can change without breaking clients (they just follow links).

In practice:
  Almost nobody implements this.
  Frontend teams hardcode API URLs.
  The discovery mechanism is documentation, not hypermedia.

HATEOAS is conceptually important for understanding REST's intent,
but you don't need it for a working Paideon API.
```

---

## 13. REST vs GraphQL vs RPC

```
REST:
  Resource-based. Multiple endpoints. Client asks for a resource.
  Client gets whatever the server decides to return.

  Problem: Over-fetching (get full student object when you need just the name).
  Problem: Under-fetching (need student + grades → 2 requests).

GraphQL:
  Query-based. One endpoint (/graphql). Client specifies exactly what it wants.

  query {
    student(id: "cuid-123") {
      name
      grades { subject score }
    }
  }

  Returns exactly: name and grades. Nothing more.

  Pros: No over/under-fetching. Strong typing. Self-documenting schema.
  Cons: Complex caching, complex security (DDoS via deeply nested queries),
        learning curve, requires schema management.

  Good for: Complex frontends with many views, mobile apps (minimize data).

RPC (gRPC, JSON-RPC, tRPC):
  Procedure-based. Client calls a function on the server.

  POST /rpc { method: "getStudent", params: { id: "cuid-123" } }

  gRPC: binary (protobuf), streaming, efficient for service-to-service.
  tRPC: TypeScript-first, end-to-end type safety, great for monorepos.

  Good for: Internal microservice communication, TypeScript monorepos.

For Paideon:
  REST for the public API (clear, well-understood, easy to document).
  If Paideon grows to microservices: gRPC for inter-service communication.
  tRPC is an interesting option given the NestJS/Next.js TS monorepo.
```

---

## 14. Paideon API Design Patterns

```
Base URL: https://paideon.lk/api/v1

Auth endpoints (no auth required):
  POST   /api/v1/auth/login
  POST   /api/v1/auth/refresh
  POST   /api/v1/auth/logout
  POST   /api/v1/auth/forgot-password
  POST   /api/v1/auth/reset-password

School management (admin only):
  GET    /api/v1/schools
  POST   /api/v1/schools
  GET    /api/v1/schools/:id
  PATCH  /api/v1/schools/:id

Students:
  GET    /api/v1/students                          → list (with filters: ?class=10A)
  POST   /api/v1/students                          → create
  GET    /api/v1/students/:id                      → get one
  PATCH  /api/v1/students/:id                      → update
  DELETE /api/v1/students/:id                      → soft delete (status=deleted)
  POST   /api/v1/students/:id/transfer             → transfer to different class
  POST   /api/v1/students/:id/suspend              → suspend
  GET    /api/v1/students/:id/grades               → student's grades
  GET    /api/v1/students/:id/attendance           → student's attendance

Classes:
  GET    /api/v1/classes
  POST   /api/v1/classes
  GET    /api/v1/classes/:id
  PATCH  /api/v1/classes/:id
  GET    /api/v1/classes/:id/students              → students in class
  GET    /api/v1/classes/:id/timetable             → class timetable

Assignments:
  GET    /api/v1/assignments?class=:id&subject=:id
  POST   /api/v1/assignments
  GET    /api/v1/assignments/:id
  PATCH  /api/v1/assignments/:id
  DELETE /api/v1/assignments/:id
  GET    /api/v1/assignments/:id/submissions       → all submissions

Grades:
  POST   /api/v1/grades                            → submit grades (teacher)
  GET    /api/v1/grades?student=:id&term=:term
  PATCH  /api/v1/grades/:id                        → correct a grade

Tenant isolation:
  Every endpoint internally filters by the authenticated user's schoolId.
  Students from school A can never see data from school B.
  schoolId comes from JWT payload, not from request body/params.
  (Never trust: schoolId from client. Always use: JWT claims.)
```

---

_Next: [Part 06 — Authentication Flows on the Web](05%20-%20Authentication%20Flows%20on%20the%20Web.md)_
