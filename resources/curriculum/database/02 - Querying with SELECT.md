`SELECT` is the query you'll write more than every other kind combined. This module covers filtering, sorting, and aggregating — the three things almost every real query needs to do.

---

## Basic Shape

```sql
SELECT full_name, grade
FROM students
WHERE grade = 10
ORDER BY full_name ASC
LIMIT 20;
```

| Clause     | Job                                                                                                  |
| ---------- | ---------------------------------------------------------------------------------------------------- |
| `SELECT`   | Which columns to return (`*` means all columns — fine for exploring, avoid in real application code) |
| `FROM`     | Which table to read from                                                                             |
| `WHERE`    | Which rows to include                                                                                |
| `ORDER BY` | What order to return them in                                                                         |
| `LIMIT`    | How many rows, maximum                                                                               |

SQL clauses have a fixed order in the query text (`SELECT` ... `FROM` ... `WHERE` ... `ORDER BY` ... `LIMIT`), but the database actually _executes_ them in a different logical order underneath (roughly: `FROM` → `WHERE` → `SELECT` → `ORDER BY` → `LIMIT`). You don't need to memorize that execution order, but it explains why you can't reference a column alias from `SELECT` inside the same query's `WHERE` clause — `WHERE` runs before `SELECT` does.

---

## Filtering with `WHERE`

```sql
-- Equality
WHERE grade = 10

-- Comparison
WHERE grade >= 10 AND grade <= 12

-- Text matching (case-sensitive)
WHERE full_name = 'Ashan Perera'

-- Text pattern matching — % is a wildcard for any characters
WHERE full_name LIKE 'A%'          -- starts with A
WHERE email LIKE '%@school.lk'     -- ends with @school.lk

-- Case-insensitive pattern matching
WHERE full_name ILIKE 'ashan%'

-- Checking against a list of values
WHERE grade IN (10, 11, 12)

-- Checking a range
WHERE created_at BETWEEN '2026-01-01' AND '2026-06-30'

-- Checking for missing values — NULL needs its own operators
WHERE email IS NULL
WHERE email IS NOT NULL

-- Combining conditions
WHERE grade = 10 AND (department_id = 3 OR department_id = 5)
```

**`NULL` is not a value — it's the absence of one.** `WHERE email = NULL` never matches anything, even rows where `email` genuinely has no value, because `NULL` isn't equal to anything, including itself. `IS NULL` / `IS NOT NULL` are the only correct ways to test for it. This trips up almost everyone the first time.

---

## Sorting with `ORDER BY`

```sql
ORDER BY grade ASC              -- lowest first (default if unspecified)
ORDER BY grade DESC             -- highest first
ORDER BY grade ASC, full_name ASC  -- sort by grade, then alphabetically within each grade
```

---

## Aggregating — Turning Many Rows into a Summary

Aggregate functions collapse multiple rows into a single computed value:

```sql
SELECT COUNT(*) FROM students;                    -- how many students total
SELECT COUNT(*) FROM students WHERE grade = 10;   -- how many in grade 10
SELECT AVG(grade) FROM enrollments WHERE subject_id = 3;  -- average grade in one subject
SELECT MAX(created_at) FROM enrollments;          -- most recent enrollment
SELECT MIN(grade), MAX(grade) FROM enrollments;   -- lowest and highest grade recorded
SELECT SUM(amount) FROM payments WHERE status = 'completed';
```

### `GROUP BY` — Aggregating Per Category

```sql
-- Count students in EACH grade, not the whole table at once
SELECT grade, COUNT(*) AS student_count
FROM students
GROUP BY grade
ORDER BY grade;
```

```
grade | student_count
------+--------------
10    | 42
11    | 38
12    | 35
```

The rule: **every column in `SELECT` that isn't wrapped in an aggregate function must also appear in `GROUP BY`.** `SELECT grade, full_name, COUNT(*) ... GROUP BY grade` is invalid — the database has no way to know which single `full_name` to show for a group of 42 students.

### `HAVING` — Filtering _After_ Aggregation

```sql
-- WHERE filters rows before grouping; HAVING filters groups after
SELECT grade, COUNT(*) AS student_count
FROM students
GROUP BY grade
HAVING COUNT(*) > 40;   -- only grades with more than 40 students
```

`WHERE` can't reference `COUNT(*)` because `WHERE` runs before the grouping happens — that's exactly what `HAVING` is for.

---

## A Realistic Query

```sql
-- Top 5 subjects by average grade, among grade 11 students only,
-- excluding subjects with fewer than 10 enrollments (too small a sample)
SELECT
  subjects.name,
  COUNT(*) AS enrollment_count,
  ROUND(AVG(enrollments.grade), 1) AS average_grade
FROM enrollments
JOIN students ON students.id = enrollments.student_id
JOIN subjects ON subjects.id = enrollments.subject_id
WHERE students.grade = 11
GROUP BY subjects.name
HAVING COUNT(*) >= 10
ORDER BY average_grade DESC
LIMIT 5;
```

This uses `JOIN` (Module 03) to pull in data from related tables — almost every query beyond the simplest ones needs at least one.

---

## Common Mistakes

- **`SELECT *` in real application code.** It's convenient while exploring in a SQL client, but in an actual query path it fetches columns you don't need (wasted bandwidth) and silently breaks if someone renames a column your code was implicitly depending on by position.
- **Forgetting `LIMIT` on an exploratory query against a large table.** `SELECT * FROM enrollments;` with no limit on a table with a million rows can lock up a client or take down a connection pool. Get in the habit of always adding `LIMIT 100` while poking around.
- **Comparing against `NULL` with `=` instead of `IS NULL`.** Covered above, but worth repeating — this is the single most common "why isn't my WHERE clause matching rows I can see are missing that value" bug.
- **Using `HAVING` where `WHERE` would do, or vice versa.** If the condition is about raw row values, it's `WHERE` (runs first, cheaper). If it's about an aggregate result, it's `HAVING` (runs after grouping).
