> Injection attacks happen when untrusted data is sent to an interpreter as part of a command. SQL injection, NoSQL injection, command injection — all share one root cause: mixing code and data. This covers every injection type, how each works, and exactly how to prevent them.

---

## Table of Contents

1. [Why Injection Happens](#1-why-injection-happens)
2. [SQL Injection](#2-sql-injection)
3. [Blind SQL Injection](#3-blind-sql-injection)
4. [NoSQL Injection](#4-nosql-injection)
5. [Command Injection](#5-command-injection)
6. [Path Traversal](#6-path-traversal)
7. [Template Injection](#7-template-injection)
8. [XXE — XML External Entity](#8-xxe--xml-external-entity)
9. [Defense: Parameterization](#9-defense-parameterization)
10. [Defense: Input Validation](#10-defense-input-validation)

---

## 1. Why Injection Happens

One root cause: **user input is interpreted as code rather than data**.

```
Safe query:
  "SELECT * FROM users WHERE email = $1"
  Parameter: "alice@school.lk"
  Database: looks up email equal to the literal string

Unsafe query:
  `SELECT * FROM users WHERE email = '${email}'`
  Input: "' OR '1'='1"
  Becomes: SELECT * FROM users WHERE email = '' OR '1'='1'
  Database: evaluates the injected condition — returns all users
```

The database can't distinguish a legitimate string from injected SQL — because the developer merged them into one string. The solution is always the same: never merge user data into commands. Always pass data as separate parameters.

---

## 2. SQL Injection

### Authentication Bypass

```
Login query:
  `SELECT * FROM users WHERE email = '${email}' AND password = '${password}'`

Input:
  email:    ' OR '1'='1' --
  password: anything

Query becomes:
  SELECT * FROM users WHERE email = '' OR '1'='1' --' AND password = 'anything'

'1'='1' is always true. -- comments out the rest.
Returns the first user in the table — usually admin.
Attacker is logged in without a valid password.
```

### Data Exfiltration via UNION

```
URL: /books?id=1

Vulnerable: SELECT title, author FROM books WHERE id = ${id}

Injection: id = 1 UNION SELECT email, password_hash FROM users --

Full query:
  SELECT title, author FROM books WHERE id = 1
  UNION SELECT email, password_hash FROM users --

Returns: book data + all usernames and hashes from the users table.
Attacker now has all credentials.
```

### Error-Based Injection

Force the database to include data in error messages:

```sql
' AND EXTRACTVALUE(1, CONCAT(0x7e, (SELECT version()), 0x7e)) --
-- Error message contains: "XPATH syntax error: '~8.0.32~'"
-- Database version revealed in the error
```

---

## 3. Blind SQL Injection

When the application doesn't return query results or errors — infer information from behavior.

### Boolean-Based

```
/user?id=1 AND 1=1   → normal response (true condition)
/user?id=1 AND 1=2   → empty/different response (false condition)

Extract data one bit at a time:
/user?id=1 AND SUBSTRING(password,1,1)='a'  → true if first char is 'a'

Automated tools (sqlmap) extract entire databases this way in minutes.
```

### Time-Based

```
/user?id=1 AND SLEEP(5)
→ Response takes 5 seconds → SQL was executed → injection confirmed

Extract data:
/user?id=1 AND IF(SUBSTRING(password,1,1)='a', SLEEP(5), 0)
→ 5 second delay → first character of password is 'a'
→ No delay → try next character
```

---

## 4. NoSQL Injection

MongoDB operator injection — instead of SQL syntax, abuse MongoDB query operators.

```js
// VULNERABLE — user input goes directly into query object
const user = await db.users.findOne({
  email: req.body.email,
  password: req.body.password,
});

// Attack — send JSON body:
// { "email": { "$ne": null }, "password": { "$ne": null } }

// Query becomes:
{ email: { $ne: null }, password: { $ne: null } }
// "where email is not null AND password is not null"
// Returns the first user in the database → authentication bypassed

// SAFE — validate type before query
if (typeof req.body.email !== "string") throw new BadRequestException();
if (typeof req.body.password !== "string") throw new BadRequestException();
```

With NestJS class-validator, `@IsString()` on DTO fields rejects objects automatically — the `$ne: null` object fails validation before reaching the service.

---

## 5. Command Injection

User input passed to OS shell commands:

```js
// VULNERABLE
const filename = req.query.file;
exec(`cat /uploads/${filename}`, callback);

// Input: "report.txt; rm -rf /"
// Command: cat /uploads/report.txt; rm -rf /
// Destroys the entire filesystem.
```

```js
// SAFE — use execFile (no shell) or bypass shell entirely
import { execFile } from "child_process";
import { readFile } from "fs/promises";
import path from "path";

// Best: don't use shell at all
const uploadDir = "/uploads";
const resolved = path.resolve(uploadDir, filename);
if (!resolved.startsWith(uploadDir)) throw new ForbiddenException();
const content = await readFile(resolved, "utf8");

// If you must spawn a process:
execFile("convert", ["input.jpg", "-resize", "200x200", "output.jpg"]);
// execFile passes args as array — no shell interpolation
```

**Never:** `exec(user_input)`, `eval(user_input)`, `spawn("sh", ["-c", user_input])`.

---

## 6. Path Traversal

User input in file paths can escape the intended directory:

```js
// VULNERABLE
const file = req.query.filename; // "../../etc/passwd"
const content = fs.readFileSync(`/uploads/${file}`);
// Reads /uploads/../../etc/passwd = /etc/passwd

// SAFE
const uploadDir = path.resolve("/uploads");
const requested = path.resolve(uploadDir, file); // resolves ../ sequences

if (!requested.startsWith(uploadDir + path.sep)) {
  throw new ForbiddenException("Invalid path");
}
const content = fs.readFileSync(requested);
```

Also validate filename characters — reject null bytes (`\0`), which can truncate strings in some languages.

---

## 7. Template Injection

User input rendered inside a server-side template engine:

```
Handlebars:
  Template:   Hello {{name}}!       ← safe, auto-escaped
  Template:   Hello {{{name}}}!     ← UNSAFE, raw HTML — allows injection

Pug/Jade:
  - const title = req.query.title
  h1= title                          ← safe
  h1!= title                         ← UNSAFE, raw

Python Jinja2:
  {{ name | e }}                     ← safe, escaped
  {{ name }}                         ← safe by default (Jinja2 auto-escapes in HTML mode)

Server-side template injection (SSTI):
  Template: "Hello " + req.body.name + "!"   ← eval-like
  Input: "{{7*7}}"
  Output: "Hello 49!"  ← template evaluated the expression
  Input: "{{''.__class__.__mro__[1].__subclasses__()[132].__init__.__globals__['sys'].modules['os'].popen('id').read()}}"
  → Remote code execution
```

**Defense:** Never concatenate user input into template strings. Use template variables (`{{name}}` not `"Hello " + name + "!"`). Use auto-escaping template engines.

---

## 8. XXE — XML External Entity

XML parsers can reference external entities — local files or URLs — which attackers exploit:

```xml
<?xml version="1.0"?>
<!DOCTYPE foo [
  <!ENTITY xxe SYSTEM "file:///etc/passwd">
]>
<request><name>&xxe;</name></request>

<!-- When parsed, &xxe; expands to contents of /etc/passwd -->
<!-- Application processes and may return file contents -->
```

```js
// SAFE — disable external entities
import { XMLParser } from "fast-xml-parser";
const parser = new XMLParser({
  allowBooleanAttributes: true,
  // fast-xml-parser doesn't support external entities by default — safe
});

// If using libxml-based parsers — explicitly disable:
// libxmljs2: parseXmlString(xml, { nonet: true, noent: true })
```

Most Node.js JSON-based APIs never parse XML — this vulnerability mainly affects apps that accept XML input (SOAP, some file formats). Prefer JSON.

---

## 9. Defense: Parameterization

The universal defense against query injection. The query template and the data are sent separately to the interpreter — user input can never become part of the command.

```ts
// Prisma — parameterized by design, impossible to inject
const users = await prisma.user.findMany({
  where: {
    email: userEmail, // becomes $1 parameter
    schoolId: schoolId, // becomes $2 parameter
  },
});

// Prisma raw query — still parameterized via tagged template
const result = await prisma.$queryRaw`
  SELECT * FROM users WHERE email = ${userEmail} AND school_id = ${schoolId}
`;
// This is NOT regular string interpolation — Prisma's tagged template parameterizes values

// Raw pg driver
const { rows } = await client.query(
  "SELECT * FROM users WHERE email = $1 AND school_id = $2",
  [userEmail, schoolId],
);
```

---

## 10. Defense: Input Validation

Not a substitute for parameterization — an additional layer. Reject bad input before it reaches the database or shell.

```ts
class SearchDto {
  @IsString()
  @MaxLength(100)
  @Matches(/^[a-zA-Z0-9 .'\-]+$/) // only safe characters
  query: string;

  @IsEnum(UserRole)
  role: UserRole;

  @IsUUID("4") // validates UUID format
  schoolId: string;
}

// Global ValidationPipe
app.useGlobalPipes(
  new ValidationPipe({
    whitelist: true, // strip undeclared properties
    forbidNonWhitelisted: true, // reject requests with extra properties
    transform: true, // auto-transform types
  }),
);
```

`whitelist: true` also prevents mass assignment — properties not in the DTO are stripped before reaching the controller.

---

## Summary

```
Root cause: mixing user data into commands/queries

SQL injection:   ' OR '1'='1, UNION SELECT, SLEEP() for blind
  Defense: always parameterized queries (Prisma, $queryRaw tagged template)

NoSQL injection: { $ne: null } bypasses authentication
  Defense: IsString() validator rejects objects

Command injection: ; rm -rf / appended to exec() arguments
  Defense: execFile (no shell), fs.readFile (no shell at all)

Path traversal: ../../etc/passwd escapes upload directory
  Defense: path.resolve() + startsWith() check

Template injection: user input evaluated as template code
  Defense: never concatenate user input into template strings

XXE: external entities in XML read local files
  Defense: disable external entities, prefer JSON

Parameterization: query + data separate → injection impossible
Input validation: whitelist, type check, length limit, regex — defense in depth
```

---
