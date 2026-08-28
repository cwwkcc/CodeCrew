---
tags: [MOC, database, sql, curriculum]
---

# 🐘 Database & SQL Curriculum

Relational databases from first principles through to the Prisma patterns already used in Nexus: schema design, querying, joins, indexing, migrations. 7 modules.

> **Location:** `resources/curriculum/database/`
>
> **Why this track exists:** the Next.js curriculum (`nextjs/03` and `nextjs/05`) already uses Prisma directly, assuming database fundamentals that were previously never taught anywhere in the vault. This track is meant to sit _before_ those modules in a learning path — everything here uses PostgreSQL syntax, the same engine Nexus runs.

---

## Module Index

| #   | Topic                              | File                                         |
| --- | ---------------------------------- | -------------------------------------------- |
| 00  | Relational Databases & Why SQL     | `00 - Relational Databases & Why SQL.md`     |
| 01  | Schema Design & Data Types         | `01 - Schema Design & Data Types.md`         |
| 02  | Querying with SELECT               | `02 - Querying with SELECT.md`               |
| 03  | Joins & Relationships              | `03 - Joins & Relationships.md`              |
| 04  | Indexes & Query Performance        | `04 - Indexes & Query Performance.md`        |
| 05  | Migrations & Schema Evolution      | `05 - Migrations & Schema Evolution.md`      |
| 06  | Prisma ORM Patterns for This Stack | `06 - Prisma ORM Patterns for This Stack.md` |

---

## Suggested Order Relative to Other Tracks

```
nodejs/  →  database/  →  nextjs/03 (Server Components & Data Fetching)
                       →  nextjs/05 (Server Actions & Mutations)
```

Modules 00–05 are pure SQL/relational theory (engine-agnostic in concept, PostgreSQL in syntax). Module 06 is the deliberate bridge into Prisma — it assumes 00–05 and maps every pattern back to the SQL it generates.

---

## 🔗 Cross‑Curriculum Links

| Module                               | Related Topics                                                                                        |
| ------------------------------------ | ----------------------------------------------------------------------------------------------------- |
| `00 - Relational Databases`          | → [[../nextjs/03 - Server Components & Data Fetching]]                                                |
| `03 - Joins & Relationships`         | → [[../nextjs/03 - Server Components & Data Fetching]] (the `include` pattern)                        |
| `04 - Indexes & Query Performance`   | → [[../nextjs/03 - Server Components & Data Fetching]] (caching Prisma queries)                       |
| `05 - Migrations & Schema Evolution` | → [[../nextjs/05 - Server Actions & Mutations]]                                                       |
| `06 - Prisma ORM Patterns`           | → [[../nextjs/03 - Server Components & Data Fetching]], [[../nextjs/05 - Server Actions & Mutations]] |
