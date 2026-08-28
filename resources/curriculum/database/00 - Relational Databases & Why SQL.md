Every app you've built so far has kept its data in memory (a JavaScript array, a React state object) — which means it disappears the moment the program stops running. A database is what makes data outlive the program. This module covers what a relational database actually is, before writing a single query.

---

## What a Database Is

A **database** is organized, persistent storage that a program can read from and write to reliably, even across restarts, crashes, and multiple simultaneous users. "Persistent" is the key word — it survives the life of any single program.

| Where data lives    | Survives restart? | Survives crash?                       | Multiple users at once? |
| ------------------- | ----------------- | ------------------------------------- | ----------------------- |
| A JS variable       | No                | No                                    | N/A (single process)    |
| A JSON file on disk | Yes               | Sometimes (partial writes corrupt it) | No (no locking)         |
| A real database     | Yes               | Yes                                   | Yes                     |

---

## What "Relational" Means

A **relational database** stores data in **tables** — rows and columns, like a spreadsheet with strict rules. Each table represents one kind of thing:

```
students table:
| id | name          | grade | department_id |
|----|---------------|-------|---------------|
| 1  | Ashan Perera   | 10    | 3             |
| 2  | Nadeesha Silva | 11    | 1             |

departments table:
| id | name        |
|----|-------------|
| 1  | Science     |
| 3  | ICT         |
```

The word "relational" refers to how tables **relate** to each other — `students.department_id` points at a row in `departments`. This is the single most important idea in the whole track: data isn't duplicated across tables, it's _linked_. Module 03 (Joins & Relationships) is entirely about how those links work in practice.

---

## Why Not Just Use JSON Files?

```json
// students.json — looks fine at first...
[
  { "id": 1, "name": "Ashan Perera", "grade": 10, "department": "ICT" },
  { "id": 2, "name": "Nadeesha Silva", "grade": 11, "department": "Science" }
]
```

This works for a school project. It breaks down for a real application:

- **No enforced structure.** Nothing stops a bug from writing `{ "grade": "ten" }` into one entry and `10` into another.
- **No safe concurrent writes.** Two requests updating the file at the same time can corrupt it or silently lose one of the writes.
- **Duplicated, driftable data.** If `"department": "ICT"` is spelled `"I.C.T"` in one record, you now have two departments that are really one.
- **No efficient search.** Finding "every student in grade 10" means reading and scanning the _entire_ file, every time, even with a million records.

A relational database solves all four: enforced column types, safe concurrent access (via **transactions** — see Module 04), foreign keys that prevent orphaned/misspelled references, and **indexes** that make lookups fast regardless of table size.

---

## SQL — The Language, Not a Product

**SQL** (Structured Query Language) is the language used to talk to a relational database — not a specific piece of software. Many different database _engines_ all speak (mostly) the same SQL:

| Engine              | Common use case                                                                                        |
| ------------------- | ------------------------------------------------------------------------------------------------------ |
| **PostgreSQL**      | General-purpose, feature-rich, the default choice for most new projects (what Nexus runs)              |
| **MySQL / MariaDB** | Widely used, especially in older or WordPress-adjacent stacks                                          |
| **SQLite**          | A database that's just a single file — no server needed, great for local dev, mobile apps, small tools |
| **SQL Server**      | Common in enterprise/.NET environments                                                                 |

This curriculum uses **PostgreSQL** syntax throughout, since that's the industry default for new projects and what you're already running for Nexus. The differences between engines are mostly small syntax quirks — the concepts (tables, joins, indexes, transactions) transfer everywhere.

---

## The Four Basic Operations — CRUD

Almost everything a database does falls into one of four categories, usually abbreviated **CRUD**:

| Operation  | SQL keyword | Meaning              |
| ---------- | ----------- | -------------------- |
| **C**reate | `INSERT`    | Add a new row        |
| **R**ead   | `SELECT`    | Fetch existing rows  |
| **U**pdate | `UPDATE`    | Modify existing rows |
| **D**elete | `DELETE`    | Remove rows          |

Module 02 covers `SELECT` in depth (it's the one you'll write the most of, by far); the others are covered briefly in Module 05 alongside migrations, since in practice most write operations in a real app go through an ORM rather than hand-written SQL — but understanding what the ORM generates underneath is what separates knowing an ORM's API from actually understanding your database.

---

## Where This Fits with What You Already Know

You've already used a database without necessarily thinking of it that way — every time a Next.js Server Component in the `nextjs/` curriculum calls `prisma.student.findMany()`, that call becomes a `SELECT` statement sent to PostgreSQL. This track builds the layer underneath that call: what the database actually does when it receives that query, why it's fast (or slow), and how to design the tables Prisma is reading from in the first place.

---

## Common Mistakes

- **Treating a database like a bigger JSON file.** The value of a relational database comes from _structure and constraints_, not just capacity. A table with no foreign keys and no unique constraints has given up most of what makes SQL worth using.
- **Assuming all SQL databases are interchangeable in every detail.** Core `SELECT`/`INSERT`/`JOIN` syntax is nearly universal; things like auto-incrementing IDs, date functions, and JSON column support vary by engine. When in doubt, check the PostgreSQL docs specifically, since that's the engine this track (and Nexus) uses.
