Before writing a single query, someone has to decide what tables exist and what columns each one has. That decision is the **schema** — and a bad schema makes every query afterward harder than it needs to be.

---

## Designing a Table

A table definition in SQL looks like this:

```sql
CREATE TABLE students (
  id         SERIAL PRIMARY KEY,
  full_name  VARCHAR(120) NOT NULL,
  grade      SMALLINT NOT NULL,
  email      VARCHAR(255) UNIQUE,
  created_at TIMESTAMP NOT NULL DEFAULT now()
);
```

Reading each part:

| Piece           | Meaning                                                                      |
| --------------- | ---------------------------------------------------------------------------- |
| `SERIAL`        | Auto-incrementing integer — Postgres assigns 1, 2, 3... automatically        |
| `PRIMARY KEY`   | The column that uniquely identifies each row — every table needs exactly one |
| `VARCHAR(120)`  | Variable-length text, capped at 120 characters                               |
| `NOT NULL`      | This column can never be empty                                               |
| `UNIQUE`        | No two rows can share the same value in this column                          |
| `DEFAULT now()` | If no value is given, use the current timestamp                              |

---

## Common Data Types

| Type                        | Use for                                                                        |
| --------------------------- | ------------------------------------------------------------------------------ |
| `INTEGER` / `SERIAL`        | Whole numbers, auto-incrementing IDs                                           |
| `VARCHAR(n)`                | Short text with a known max length (names, emails)                             |
| `TEXT`                      | Long text with no practical length limit (article bodies, descriptions)        |
| `BOOLEAN`                   | True/false flags (`is_published`, `is_active`)                                 |
| `TIMESTAMP` / `TIMESTAMPTZ` | Date and time — prefer `TIMESTAMPTZ` (timezone-aware) for anything user-facing |
| `DATE`                      | A calendar date with no time component (a birthday, a due date)                |
| `NUMERIC(p, s)`             | Exact decimal numbers — always use this for money, never `FLOAT`               |
| `JSONB`                     | Structured but flexible data that doesn't need its own table (see below)       |

**Never use `FLOAT`/`REAL` for money.** Floating-point numbers can't represent most decimal fractions exactly — `0.1 + 0.2` famously doesn't equal `0.3` in floating-point math. `NUMERIC(10, 2)` stores exact decimal values and is what every real financial system uses.

---

## Choosing Primary Keys

| Strategy                                 | How it works                            | Trade-off                                                                                      |
| ---------------------------------------- | --------------------------------------- | ---------------------------------------------------------------------------------------------- |
| **Auto-incrementing integer** (`SERIAL`) | Database assigns 1, 2, 3...             | Simple, compact, but predictable/guessable (`/students/2` implies `/students/1` exists)        |
| **UUID**                                 | A randomly generated 128-bit identifier | Not guessable, safe to expose in URLs, but larger and slightly slower to index than an integer |

A common real-world pattern: auto-incrementing integer as the _internal_ primary key (fast joins), plus a separate UUID column exposed in URLs and APIs (not guessable). Prisma supports both natively — `@id @default(autoincrement())` for the former, `@id @default(uuid())` or `@default(cuid())` for the latter.

---

## Normalization — Avoiding Duplicate Data

**Normalization** is the process of structuring tables so each fact is stored exactly once.

```
-- WRONG: department name duplicated on every student row
students:
| id | name    | department_name |
|----|---------|------------------|
| 1  | Ashan   | Information & Communication Technology |
| 2  | Nadeesha| Information & Communication Technology |

-- Rename the department once and you have to update every single row,
-- and any typo produces two "different" departments that are really one.
```

```
-- CORRECT: department stored once, students reference it by id
departments:
| id | name                                    |
|----|------------------------------------------|
| 1  | Information & Communication Technology  |

students:
| id | name    | department_id |
|----|---------|----------------|
| 1  | Ashan   | 1              |
| 2  | Nadeesha| 1              |
```

Rename the department once, in one row, and every student's department updates implicitly — because they're not storing the name at all, just a reference to it. This reference is a **foreign key**, covered fully in Module 03.

Normalization isn't an absolute rule to chase for its own sake — a small amount of intentional duplication (called **denormalization**) is sometimes the right call for read performance in a table that's queried far more than it's written to. But that's a deliberate trade-off you make _after_ understanding the normalized form, not a default.

---

## Constraints — Letting the Database Enforce Rules

Constraints stop bad data from ever being written, rather than trusting every piece of application code to check it correctly:

```sql
CREATE TABLE enrollments (
  id          SERIAL PRIMARY KEY,
  student_id  INTEGER NOT NULL REFERENCES students(id),
  subject_id  INTEGER NOT NULL REFERENCES subjects(id),
  grade       SMALLINT CHECK (grade >= 0 AND grade <= 100),
  UNIQUE (student_id, subject_id)  -- a student can't enroll in the same subject twice
);
```

| Constraint                 | Prevents                                        |
| -------------------------- | ----------------------------------------------- |
| `NOT NULL`                 | A required field being left empty               |
| `UNIQUE`                   | Duplicate values where there should only be one |
| `CHECK`                    | Values outside a valid range or set             |
| `REFERENCES` (foreign key) | A row pointing at something that doesn't exist  |

The `UNIQUE (student_id, subject_id)` line is a **composite unique constraint** — it's not that either column alone must be unique, it's that the _combination_ must be. This single line does the enforcement work that would otherwise require an application-level check on every single write path — and unlike an application check, it can't be bypassed by a bug, a script, or a second app writing to the same database.

---

## Common Mistakes

- **No primary key at all**, or a primary key on a column that isn't actually guaranteed unique (like `email` before verifying no duplicates can slip in through a race condition). Every table needs an explicit, database-enforced primary key.
- **Using `VARCHAR(255)` for everything out of habit.** It's a harmless historical default, but `TEXT` is simpler when there's no real length limit, and a genuinely short field (a two-letter language code) should have a genuinely short type.
- **Putting business rules only in application code, never in the schema.** A `CHECK` constraint or `NOT NULL` is enforced no matter what writes to the table — a future script, a different app, or a bug in this one. Application-level validation (Zod schemas, form validation) is still valuable for good error messages, but the database should be the last line of defense, not the only one.
