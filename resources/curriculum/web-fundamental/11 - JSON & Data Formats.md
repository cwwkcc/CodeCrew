> JSON is the lingua franca of web APIs. But most developers use it superficially — they serialize and parse and that's it. This part covers JSON fully: the spec, the edge cases, `JSON.stringify`/`JSON.parse` options most people don't know exist, JSON Schema for validation, and when to reach for something other than JSON entirely.

---

## Table of Contents

1. [JSON — The Specification](#1-json--the-specification)
2. [JSON Data Types and Their Limits](#2-json-data-types-and-their-limits)
3. [JSON.stringify — Complete Reference](#3-jsonstringify--complete-reference)
4. [JSON.parse — Complete Reference](#4-jsonparse--complete-reference)
5. [JSON Edge Cases That Break Things](#5-json-edge-cases-that-break-things)
6. [JSON Schema — Validating Structure](#6-json-schema--validating-structure)
7. [JSON in HTTP — Content Negotiation](#7-json-in-http--content-negotiation)
8. [YAML — Human-Friendly Configuration](#8-yaml--human-friendly-configuration)
9. [MessagePack — Binary JSON](#9-messagepack--binary-json)
10. [Protocol Buffers — Typed Binary Serialization](#10-protocol-buffers--typed-binary-serialization)
11. [Format Comparison — When to Use What](#11-format-comparison--when-to-use-what)
12. [JSON Best Practices for Paideon API](#12-json-best-practices-for-paideon-api)

---

## 1. JSON — The Specification

JSON (JavaScript Object Notation) was defined by Douglas Crockford and standardized in RFC 8259 and ECMA-404. It is a text format for representing structured data.

```
JSON is NOT JavaScript.
JSON is a separate format that happens to be a subset of JavaScript literal syntax.

Valid JSON:
  {"name": "Ashan", "age": 17}

Valid JavaScript object literal (not valid JSON):
  {name: "Ashan", age: 17}         // unquoted keys
  {"name": 'Ashan', "age": 17}     // single-quoted strings
  {"name": "Ashan", /* comment */} // comments not allowed in JSON

JSON has exactly 6 value types:
  string, number, boolean, null, array, object

That's it. No functions, no undefined, no Date, no Symbol, no BigInt.
```

---

## 2. JSON Data Types and Their Limits

### String

```
"hello world"      → must use double quotes, single quotes are invalid
"she said \"hi\""  → double quote inside must be escaped with \"
"line1\nline2"     → \n is newline, \t is tab, \r is carriage return
"\u0041"           → Unicode escape: U+0041 = 'A'
"ශිෂ්ය"           → UTF-8 Unicode is valid directly in JSON strings
""                 → empty string is valid

Escape sequences:
  \"   double quote
  \\   backslash
  \/   forward slash (optional escape)
  \b   backspace
  \f   form feed
  \n   newline
  \r   carriage return
  \t   tab
  \uXXXX Unicode code point (4 hex digits)
```

### Number

```
JSON numbers are concept-only — no int vs float distinction.
42          → integer
3.14        → decimal
-17         → negative
1.5e10      → scientific notation
1.5E-3      → also valid

NOT valid in JSON:
  NaN           → not a number — use null instead
  Infinity      → use null or a string representation
  -Infinity     → same
  0x1F          → hex literals not allowed
  017           → octal not allowed

CRITICAL ISSUE: JavaScript numbers are IEEE 754 64-bit floats.
They can represent integers exactly up to 2^53 - 1 = 9,007,199,254,740,991.

If your ID or value exceeds this, JavaScript loses precision:
  JSON.parse('{"id": 9007199254740993}')
  → { id: 9007199254740992 }   // WRONG! lost last digit

This affects:
  Database IDs from systems using 64-bit integers (PostgreSQL BIGINT)
  Twitter IDs (53+ bits)
  Any large integer counter

Solution: send large integers as strings in JSON:
  { "id": "9007199254740993" }   // string → preserved exactly
  
PostgreSQL BIGINT → Prisma returns it as a BigInt in JS.
JSON.stringify({ id: BigInt(9007199254740993) }) → TypeError: BigInt is not serializable
Workaround: (BigInt).toString() before serializing.
```

### Boolean and Null

```
true    → lowercase only. True, TRUE are invalid JSON.
false   → lowercase only.
null    → lowercase only. Null, NULL are invalid JSON.

null semantics:
  "field": null    → field exists but has no value
  (field absent)   → field does not exist
  
  These are different in many APIs.
  A PATCH request: {"email": null} might mean "clear email"
                   (no email field) might mean "don't change email"
  Document this distinction in your API.
```

### Array

```json
[]                              → empty array
[1, 2, 3]                       → numbers
["a", "b", "c"]                 → strings
[{"id": 1}, {"id": 2}]          → objects
[1, "two", true, null, [3, 4]]  → mixed types (valid JSON, unusual in APIs)
```

### Object

```json
{}                    → empty object
{"key": "value"}      → key MUST be a string (quoted)
{"a": 1, "b": 2}      → comma-separated key-value pairs
                         no trailing comma — {"a": 1, "b": 2,} is INVALID

Key ordering: not guaranteed by the JSON spec.
JSON.stringify may produce different key order than the input object.
Do not depend on JSON key order for parsing logic.

Duplicate keys: technically invalid per RFC 8259.
Behavior is implementation-defined:
  Most parsers use the last value: {"a": 1, "a": 2} → {a: 2}
  Never produce duplicate keys.
```

---

## 3. JSON.stringify — Complete Reference

```javascript
JSON.stringify(value, replacer?, space?)
```

### Basic Usage

```javascript
JSON.stringify({ name: 'Ashan', age: 17 })
// '{"name":"Ashan","age":17}'

// With pretty-printing (space argument)
JSON.stringify({ name: 'Ashan', age: 17 }, null, 2)
// '{
//   "name": "Ashan",
//   "age": 17
// }'

JSON.stringify({ name: 'Ashan', age: 17 }, null, '\t')
// Tab-indented output
```

### What Gets Dropped

```javascript
// Values that cannot be represented in JSON are dropped or replaced:
JSON.stringify({
  fn: function() {},          // → undefined → key dropped
  sym: Symbol('x'),           // → undefined → key dropped
  undef: undefined,           // → undefined → key dropped
  inf: Infinity,              // → null
  nan: NaN,                   // → null
  date: new Date('2026-03-13'),// → "2026-03-13T00:00:00.000Z" (string!)
  bigint: BigInt(123),        // → TypeError thrown!
  regex: /pattern/,           // → {} (empty object)
})
// '{"inf":null,"nan":null,"date":"2026-03-13T00:00:00.000Z","regex":{}}'

// Dropped keys: fn, sym, undef (undefined values silently removed from objects)
// Array undefined becomes null:
JSON.stringify([1, undefined, 3])  // '[1,null,3]'
```

### The Replacer Function

```javascript
// replacer(key, value) — called for every key-value pair.
// Return the value to include, or undefined to exclude.

const data = {
  id: 'cuid-123',
  name: 'Ashan',
  passwordHash: '$2b$12$...',   // should NOT be in API response
  internalNote: 'flagged',      // should NOT be in API response
  email: 'ashan@school.lk',
};

JSON.stringify(data, (key, value) => {
  if (key === 'passwordHash') return undefined;  // exclude
  if (key === 'internalNote') return undefined;  // exclude
  return value;  // include everything else
});
// '{"id":"cuid-123","name":"Ashan","email":"ashan@school.lk"}'

// Array replacer — allowlist of keys to include:
JSON.stringify(data, ['id', 'name', 'email'])
// '{"id":"cuid-123","name":"Ashan","email":"ashan@school.lk"}'
```

### toJSON Method

```javascript
// If an object has a toJSON() method, stringify calls it:
class Student {
  constructor(public id: string, public name: string, public passwordHash: string) {}
  
  toJSON() {
    return {
      id: this.id,
      name: this.name,
      // passwordHash excluded
    };
  }
}

const student = new Student('cuid-123', 'Ashan', '$2b$12$...');
JSON.stringify(student)
// '{"id":"cuid-123","name":"Ashan"}'

// Date.prototype.toJSON() is how Date serializes to ISO string:
new Date().toJSON()  // "2026-03-13T08:23:14.000Z"
```

### Handling BigInt

```javascript
// BigInt throws TypeError by default
JSON.stringify({ id: BigInt(9007199254740993) })
// TypeError: Do not know how to serialize a BigInt

// Solution: replacer
JSON.stringify({ id: BigInt(9007199254740993) }, (key, value) => {
  if (typeof value === 'bigint') return value.toString();
  return value;
})
// '{"id":"9007199254740993"}'
```

---

## 4. JSON.parse — Complete Reference

```javascript
JSON.parse(text, reviver?)
```

### Basic Usage

```javascript
JSON.parse('{"name":"Ashan","age":17}')
// { name: 'Ashan', age: 17 }

JSON.parse('[1, 2, 3]')
// [1, 2, 3]

JSON.parse('"hello"')
// 'hello'

JSON.parse('null')
// null

JSON.parse('true')
// true
```

### Error Handling

```javascript
// JSON.parse throws SyntaxError for invalid JSON.
// ALWAYS wrap in try/catch when parsing external input.

try {
  const data = JSON.parse(untrustedInput);
  // use data
} catch (error) {
  if (error instanceof SyntaxError) {
    // invalid JSON — handle gracefully
    console.error('Invalid JSON:', error.message);
  }
}

// Common parse errors:
JSON.parse("{'key': 'value'}")   // SyntaxError: single quotes
JSON.parse("{key: 'value'}")     // SyntaxError: unquoted key
JSON.parse('{"a": 1,}')          // SyntaxError: trailing comma
JSON.parse('')                    // SyntaxError: unexpected end
JSON.parse(undefined)            // SyntaxError
```

### The Reviver Function

```javascript
// reviver(key, value) — called bottom-up for every key-value pair.
// Return the value to use (transform it) or undefined to delete.

const json = '{"createdAt":"2026-03-13T08:23:14.000Z","name":"Ashan"}';

JSON.parse(json, (key, value) => {
  // Revive ISO date strings back into Date objects
  if (typeof value === 'string' && /^\d{4}-\d{2}-\d{2}T/.test(value)) {
    return new Date(value);
  }
  return value;
});
// { createdAt: Date object, name: 'Ashan' }

// Without reviver: dates come back as strings.
// JSON.parse always returns strings for date fields — it has no date type.
// Reviver is the clean solution for consistent Date handling.
```

---

## 5. JSON Edge Cases That Break Things

### Dates Are Always Strings

```javascript
const obj = { createdAt: new Date('2026-03-13') };
const json = JSON.stringify(obj);   // '{"createdAt":"2026-03-13T00:00:00.000Z"}'
const parsed = JSON.parse(json);    // { createdAt: "2026-03-13T00:00:00.000Z" }

typeof parsed.createdAt === 'string'  // true!

parsed.createdAt instanceof Date  // false!
parsed.createdAt.getFullYear()    // TypeError: getFullYear is not a function

Always: when parsing JSON from an API, convert date strings to Date objects.
Or: keep them as ISO strings and only convert when formatting for display.
```

### NaN and Infinity Become null

```javascript
JSON.stringify({ value: NaN })       // '{"value":null}'
JSON.stringify({ value: Infinity })  // '{"value":null}'
JSON.stringify({ value: -Infinity }) // '{"value":null}'

// Caller receives null and doesn't know if the original was NaN, Infinity, or actually null.
// Design your data model to avoid NaN/Infinity in API responses.
```

### The Empty String Key

```javascript
JSON.stringify({ '': 'empty key is valid' })
// '{"":"empty key is valid"}'

// Valid JSON. Just surprising.
// Object shorthand: obj[''] to access it.
```

### Deep Cloning with JSON (common but limited)

```javascript
// Common pattern for "deep clone":
const original = { a: { b: { c: 42 } } };
const clone = JSON.parse(JSON.stringify(original));
clone.a.b.c = 99;
console.log(original.a.b.c);  // 42 (original unchanged) ✓

// Limitations:
// Drops: functions, undefined values, symbols, RegExps, BigInt
// Converts: Dates to strings, Maps/Sets to {} or []
// Doesn't handle circular references (throws RangeError)

// Modern alternative: structuredClone() (Node 17+, modern browsers)
const clone2 = structuredClone(original);
// Handles: Dates, Maps, Sets, ArrayBuffers, circular refs
// Still doesn't clone: functions, DOM nodes
```

### Circular References

```javascript
const obj = { name: 'Ashan' };
obj.self = obj;  // circular reference

JSON.stringify(obj);
// TypeError: Converting circular structure to JSON

// Detection:
function safeStringify(obj: unknown) {
  const seen = new WeakSet();
  return JSON.stringify(obj, (key, value) => {
    if (typeof value === 'object' && value !== null) {
      if (seen.has(value)) return '[Circular]';
      seen.add(value);
    }
    return value;
  });
}
```

---

## 6. JSON Schema — Validating Structure

JSON Schema is a vocabulary for describing the structure of JSON data. It lets you validate that incoming JSON matches an expected shape.

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "required": ["email", "password"],
  "additionalProperties": false,
  "properties": {
    "email": {
      "type": "string",
      "format": "email",
      "maxLength": 255
    },
    "password": {
      "type": "string",
      "minLength": 8,
      "maxLength": 128
    }
  }
}
```

### Common Keywords

```
type:         "string", "number", "integer", "boolean", "null", "array", "object"
required:     Array of required property names
properties:   Schema for each property
additionalProperties: false → reject unknown properties
                      true  → allow unknown (default)
                      {schema} → unknown properties must match schema

String constraints:
  minLength, maxLength
  pattern: "^[a-zA-Z0-9]+$"  (regex)
  format: "email", "uri", "date", "date-time", "uuid"
  enum: ["admin", "teacher", "student", "parent"]

Number constraints:
  minimum, maximum
  exclusiveMinimum, exclusiveMaximum
  multipleOf: 5  (must be divisible by 5)

Array constraints:
  items: schema for array elements
  minItems, maxItems
  uniqueItems: true  (no duplicates)

Composition:
  allOf: [schema1, schema2]   must match all
  anyOf: [schema1, schema2]   must match at least one
  oneOf: [schema1, schema2]   must match exactly one
  not: schema                 must NOT match
```

### JSON Schema in NestJS (class-validator)

NestJS doesn't use JSON Schema directly — it uses class-validator decorators which compile to similar validation logic.

```typescript
import { IsEmail, IsString, MinLength, MaxLength, IsEnum } from 'class-validator';

export class LoginDto {
  @IsEmail()
  @MaxLength(255)
  email: string;

  @IsString()
  @MinLength(8)
  @MaxLength(128)
  password: string;
}

export class CreateStudentDto {
  @IsString()
  @MinLength(2)
  @MaxLength(100)
  name: string;

  @IsEmail()
  email: string;

  @IsEnum(['10A', '10B', '10C', '11A', '11B'])
  class: string;
}

// In main.ts:
app.useGlobalPipes(new ValidationPipe({
  whitelist: true,        // strip unknown properties (like additionalProperties: false)
  forbidNonWhitelisted: true,  // throw on unknown properties
  transform: true,        // transform string "17" to number 17 if type is number
}));
```

---

## 7. JSON in HTTP — Content Negotiation

```
Sending JSON:
  POST /api/students HTTP/1.1
  Content-Type: application/json       ← tells server what format the body is in
  {"name": "Ashan", "class": "10A"}

Requesting JSON:
  GET /api/students HTTP/1.1
  Accept: application/json             ← tells server what format you want back

Server responding with JSON:
  HTTP/1.1 200 OK
  Content-Type: application/json; charset=utf-8
  {"data": [...]}

Content negotiation:
  A server might support multiple response formats (JSON, XML, CSV).
  Client says: Accept: application/json, application/xml;q=0.9
  Server picks the best match and responds with the chosen format in Content-Type.
  
  In practice: NestJS APIs only return JSON.
  Accept header is often ignored. Not worth implementing multi-format support
  unless required.

Line ending and encoding:
  Content-Type: application/json; charset=utf-8
  UTF-8 is the standard and recommended encoding for JSON.
  No BOM (Byte Order Mark) in JSON.
  
  In practice: charset=utf-8 is often omitted.
  RFC 8259 specifies UTF-8 as the default and only standard encoding.
```

---

## 8. YAML — Human-Friendly Configuration

YAML (YAML Ain't Markup Language) is a superset of JSON optimized for human readability.

```yaml
# YAML uses indentation for structure, no quotes usually needed
name: Ashan Silva
age: 17
class: "10A"      # quotes optional for strings without special chars
active: true
grades:
  math: 85
  science: 92
  english: 78
contacts:
  - type: email
    value: ashan@school.lk
  - type: phone
    value: "0771234567"    # quoted to prevent number parsing

# Comments are supported (JSON has no comments)
# YAML is JSON-compatible: any valid JSON is valid YAML

# Multi-line string
bio: |
  Ashan is a diligent student
  who excels in mathematics.

# Inline (flow) style — looks like JSON
student: {name: Ashan, age: 17}
scores: [85, 92, 78]
```

### Where YAML Is Used

```
Configuration files:
  docker-compose.yml
  kubernetes manifests (.yaml)
  GitHub Actions workflows (.yml)
  NestJS config files

NestJS config with YAML:
  import * as yaml from 'js-yaml';
  import { readFileSync } from 'fs';
  
  ConfigModule.forRoot({
    load: [() => yaml.load(readFileSync('config.yaml', 'utf8'))],
  });

YAML gotchas:
  Indentation matters (like Python).
  Tabs not allowed — spaces only.
  
  Norway problem:
    countries:
      - NO     → parsed as false (NO is truthy 'no' = false in YAML 1.1)
      - SE     → "SE" (fine)
    
    YAML 1.1 interprets: yes/no/on/off/true/false as boolean.
    Always quote strings that could be misinterpreted.
    
  Octal numbers (YAML 1.1):
    value: 0777  → parsed as 511 (octal 777) in YAML 1.1
    Quote it: value: "0777"
    
  YAML 1.2 fixed most of these. js-yaml uses YAML 1.2 by default.
```

---

## 9. MessagePack — Binary JSON

MessagePack is a binary serialization format that is semantically equivalent to JSON but more compact and faster to parse.

```
JSON:
  {"name":"Ashan","age":17,"active":true}
  38 bytes

MessagePack equivalent:
  83 a4 6e 61 6d 65 a5 41 73 68 61 6e a3 61 67 65 11 a6 61 63 74 69 76 65 c3
  25 bytes (34% smaller)

For larger objects, savings are more dramatic.
Numbers encoded as binary (not text digits) → more compact.
Strings with short lengths have 1-byte headers.

Advantages:
  ~20-40% smaller than equivalent JSON
  Faster to parse (binary, not text)
  Same data model as JSON (same types)

Disadvantages:
  Not human-readable (binary blob)
  Cannot be debugged with text tools
  Less universal tooling

When to use:
  High-frequency messages over WebSocket (position updates, sensor data)
  Internal service-to-service communication
  Mobile apps on slow/expensive connections
  IoT devices with bandwidth constraints

Not worth it for:
  Normal REST APIs (JSON is fine, compression is more impactful)
  Any endpoint humans might inspect with browser DevTools

In Node.js:
  import { pack, unpack } from 'msgpackr';
  const bytes = pack({ name: 'Ashan', age: 17 });
  const obj = unpack(bytes);
```

---

## 10. Protocol Buffers — Typed Binary Serialization

Protocol Buffers (Protobuf) is Google's binary serialization format. Unlike MessagePack, it requires a schema definition.

```protobuf
// student.proto
syntax = "proto3";

message Student {
  string id = 1;
  string name = 2;
  int32 age = 3;
  bool active = 4;
  repeated string subjects = 5;
}
```

```
Generate code from schema:
  protoc --ts_out=. student.proto
  → generates TypeScript types and serialization code

Serialize:
  const student = { id: 'cuid-123', name: 'Ashan', age: 17, active: true };
  const bytes = Student.toBinary(student);  // 20 bytes

Deserialize:
  const student = Student.fromBinary(bytes);
  // typed as Student — full TypeScript type safety

Compared to JSON:
  Size:    ~50-80% smaller than JSON
  Speed:   ~3-10× faster to serialize/deserialize
  Types:   schema is the source of truth, generated types are always correct
  
Disadvantages:
  Requires schema file management
  Schema changes need to be handled carefully (field number compatibility)
  Not human-readable
  More complex tooling setup

Use cases:
  gRPC (uses Protobuf exclusively)
  Google's internal microservices
  High-performance service-to-service calls
  Mobile apps (bandwidth-sensitive)

For Paideon:
  Not needed now. JSON + compression is sufficient.
  If Paideon becomes a large distributed system with many services:
  gRPC + Protobuf for inter-service communication.
  JSON + REST for the public-facing API (developer familiarity).
```

---

## 11. Format Comparison — When to Use What

```
                JSON        YAML        MessagePack  Protobuf
─────────────────────────────────────────────────────────────────────────
Human-readable  Yes         Yes         No           No
Comments        No          Yes         No           Yes (in .proto)
Schema          No          No          No           Yes (required)
Browser native  Yes         No          No           No
Compression     With gzip   With gzip   Built-in     Built-in
Size (raw)      Baseline    +10-20%     -30-40%      -50-80%
Parse speed     Baseline    Slower      Faster       Fastest
Type safety     No          No          No           Yes (via codegen)
Use case        APIs, data  Config      WS, IoT      gRPC, microservices

Decision:
  Web API responses → JSON (universal, human-readable, browser native)
  Configuration files → YAML (comments, readability, industry standard)
  High-frequency WebSocket messages → MessagePack (if bandwidth matters)
  Internal microservice calls → Protobuf/gRPC (type safety + performance)
  
  Always use gzip/brotli compression with JSON.
  Compressed JSON is often smaller than uncompressed MessagePack.
  The readability benefit of JSON outweighs the size disadvantage once compressed.
```

---

## 12. JSON Best Practices for Paideon API

### Consistent Field Naming

```
Choose one convention and apply it everywhere:
  camelCase:     { "studentId": "...", "createdAt": "..." }   ← preferred for JSON APIs
  snake_case:    { "student_id": "...", "created_at": "..." } ← common in Python APIs
  PascalCase:    { "StudentId": "...", "CreatedAt": "..." }   ← avoid for data

Paideon: camelCase.
Matches JavaScript conventions. Prisma returns camelCase. No transformation needed.
```

### Timestamps

```
Always ISO 8601 UTC:
  "createdAt": "2026-03-13T08:23:14.123Z"
  
  Z = UTC (zero offset). Always UTC in the API.
  Clients convert to local time for display.
  
  Milliseconds included (123 in .123Z) or not — pick one.
  Prisma returns milliseconds. Keep them.

Never:
  Unix epoch integers:  "createdAt": 1741855394   (hard to read, debug)
  Non-UTC offsets:      "createdAt": "2026-03-13T13:53:14+05:30"
                        (correct but requires careful handling)
```

### Null vs Absent Fields

```
Define and document your convention clearly:

Convention A: Null = field exists but has no value
  { "email": null }       → email field is cleared/empty
  (no email field)        → email field is not applicable or unknown

Convention B: Always include all fields, use null for empty
  GET /api/students/123
  → { "id": "...", "name": "Ashan", "middleName": null, "phone": null }
  All defined fields always present. null means empty.
  
Paideon recommendation:
  GET responses: include all fields, null for empty (Option B).
    Frontend can always access student.phone without checking if field exists.
  
  PATCH requests: absent field = don't change, null field = clear
    { "phone": null }  → clear phone
    (no phone key)     → don't touch phone
    { "phone": "0771234567" } → set phone
```

### Error Response Consistency

```json
{
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Request validation failed",
    "details": [
      { "field": "email", "message": "must be a valid email" },
      { "field": "class", "message": "must be one of: 10A, 10B, 10C" }
    ],
    "requestId": "req_01hw4rz9kf"
  }
}
```

Always the same shape. Frontend can rely on `response.error.code` to know what happened.

### Serialization Security

```typescript
// Exclude sensitive fields from API responses.
// Never return passwordHash, internal flags, or audit data to clients.

// NestJS: use @Exclude() from class-transformer
import { Exclude, Expose } from 'class-transformer';

export class UserResponseDto {
  @Expose()
  id: string;

  @Expose()
  name: string;

  @Expose()
  email: string;

  @Exclude()    // ← never serialized to JSON
  passwordHash: string;

  @Exclude()
  refreshTokenHash: string;
}

// In controller:
@UseInterceptors(ClassSerializerInterceptor)
@Get(':id')
async findOne(@Param('id') id: string): Promise<UserResponseDto> {
  const user = await this.usersService.findOne(id);
  return plainToInstance(UserResponseDto, user);
}

// Or: use Prisma select to only fetch what you need (simpler):
const user = await prisma.user.findUnique({
  where: { id },
  select: {
    id: true,
    name: true,
    email: true,
    role: true,
    // passwordHash: NOT selected → never in memory, never returned
  },
});
```

---

_End of Web Fundamentals curriculum._

_Previous parts:_

- _[Part 01 — The Internet & Networking Fundamentals](./Part%2001%20-%20The%20Internet%20%26%20Networking%20Fundamentals.md)_
- _[Part 02 — HTTP Deep Dive](01%20-%20HTTP%20Deep%20Dive.md)_
- _[Part 03 — TLS & HTTPS](./Part%2003%20-%20TLS%20%26%20HTTPS.md)_
- _[Part 04 — Cookies, Sessions & Browser Storage](./Part%2004%20-%20Cookies%2C%20Sessions%20%26%20Browser%20Storage.md)_
- _[Part 05 — REST & API Design](./Part%2005%20-%20REST%20%26%20API%20Design.md)_
- _[Part 06 — Authentication Flows on the Web](05%20-%20Authentication%20Flows%20on%20the%20Web.md)_
- _[Part 07 — Web Performance](06%20-%20Web%20Performance.md)_
- _[Part 08 — Browser Internals](07%20-%20Browser%20Internals.md)_
- _[Part 09 — URLs, URIs & the Origin Model](./Part%2009%20-%20URLs%2C%20URIs%20%26%20the%20Origin%20Model.md)_
- _[Part 10 — CORS](09%20-%20CORS.md)_
- _[Part 11 — WebSockets & Real-Time Web](./Part%2011%20-%20WebSockets%20%26%20Real-Time%20Web.md)_