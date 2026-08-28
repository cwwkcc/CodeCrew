A query that works fine on 200 rows of test data can grind to a halt on 2 million real rows. This module covers why, and the two most important tools for fixing it: indexes, and recognizing the N+1 pattern.

---

## Why Queries Get Slow

Without help, finding a row means the database checks every single row in the table — a **sequential scan**. On a table with a few hundred rows this is instant. On a table with millions of rows, it's the difference between a query that returns in 2ms and one that takes 4 seconds.

```sql
-- Without an index: scans every row in the table to find matches
SELECT * FROM students WHERE email = 'ashan@school.lk';
```

---

## Indexes — A Shortcut for Lookups

An **index** is a separate data structure the database maintains alongside a table, built specifically to make lookups on one or more columns fast — conceptually similar to the index at the back of a textbook, which lets you jump straight to a page instead of reading the whole book to find a topic.

```sql
CREATE INDEX idx_students_email ON students(email);

-- Now this query uses the index instead of scanning every row
SELECT * FROM students WHERE email = 'ashan@school.lk';
```

**Primary keys and columns with a `UNIQUE` constraint are indexed automatically** — you don't need to add these yourself. The columns that actually need a manual index are the ones you filter or join on _often_ that aren't already a primary/unique key — most commonly, foreign key columns.

```sql
-- students.department_id is queried constantly via joins, but foreign
-- keys are NOT indexed automatically in Postgres — this is a very
-- common thing to forget
CREATE INDEX idx_students_department_id ON students(department_id);
```

### The Trade-off

Indexes aren't free — every index speeds up reads but slows down writes slightly (the index has to be updated on every `INSERT`/`UPDATE`/`DELETE` too) and takes up disk space. The rule of thumb: index columns you filter, sort, or join on frequently; don't index every column "just in case."

---

## Reading a Query Plan

Postgres can show you exactly how it plans to execute a query, before running it:

```sql
EXPLAIN ANALYZE
SELECT * FROM students WHERE email = 'ashan@school.lk';
```

```
-- Without an index:
Seq Scan on students  (cost=0.00..458.00 rows=1 width=64) (actual time=3.912 ms)
  Filter: (email = 'ashan@school.lk'::text)

-- With an index:
Index Scan using idx_students_email on students  (cost=0.29..8.31 rows=1 width=64) (actual time=0.021 ms)
  Index Cond: (email = 'ashan@school.lk'::text)
```

`Seq Scan` means "read every row." `Index Scan` means "jump straight to it." The `actual time` line is the concrete number worth watching — in this example, roughly 180x faster with the index. You don't need to become an expert at reading query plans to get value from this — even just knowing to check for `Seq Scan` on a slow query, on a table with more than a few thousand rows, catches most real problems.

---

## The N+1 Query Problem

This is the single most common real-world database performance bug, and it comes from ORMs (Prisma included) making it _easy_ to accidentally write:

```ts
// Looks innocent — but watch what it actually does
const students = await prisma.student.findMany(); // 1 query

for (const student of students) {
  const department = await prisma.department.findUnique({
    where: { id: student.departmentId },
  });
  console.log(department.name);
}
// If there are 200 students, this is 1 + 200 = 201 separate database
// round-trips, when the same result could be one single query.
```

```ts
// Fixed — Prisma's `include` generates one JOIN query instead
const students = await prisma.student.findMany({
  include: { department: true },
});
// 1 query total, regardless of how many students there are
```

The name comes from the shape of the bug: **1** query to get the list, plus **N** more queries (one per item in that list). It's easy to write by accident because each individual line of code looks completely reasonable — the problem only shows up as "why is this page slow" once there's enough data for it to matter, which is often well after the code was written and reviewed.

---

## When to Actually Worry About This

Not every query needs `EXPLAIN ANALYZE` and a hand-tuned index. A sensible order of operations:

1. Write the obviously correct query first (Modules 02–03). Don't pre-optimize.
2. If a specific page or endpoint is measurably slow, check for N+1 patterns first — they're the most common cause and the easiest to fix (usually just adding `include` or `select` in the same Prisma call).
3. If it's still slow, check `EXPLAIN ANALYZE` for `Seq Scan` on your large tables and add indexes on the columns actually being filtered.
4. Only reach for caching, denormalization, or read replicas once indexing and query shape are both already right — those are real tools, but they hide problems if used before the underlying query is sound.

---

## Common Mistakes

- **Adding an index to every column "to be safe."** This bloats write performance and disk usage for indexes that are never actually used by a query planner. Index based on real, observed query patterns.
- **Not indexing foreign key columns.** Unlike primary keys, Postgres does not auto-index foreign keys — and foreign key columns are exactly the ones most often used in `JOIN`s and `WHERE` clauses.
- **Writing an N+1 loop without realizing it**, especially inside `.map()` or a `for` loop that calls an async database function per item. If you're calling `await prisma.something.find...` inside a loop over another query's results, stop and check whether `include` or a single batched query (`findMany({ where: { id: { in: [...] } } })`) can replace it.
