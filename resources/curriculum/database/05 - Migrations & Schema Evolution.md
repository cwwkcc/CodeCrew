A schema is never finished — new features need new tables, new columns, new constraints. A **migration** is how those changes get made safely, tracked, and repeatably, instead of by hand-editing a production database and hoping everyone remembers what changed.

---

## Why Not Just Edit the Database Directly?

```sql
-- Someone SSHs into the production database and runs this by hand:
ALTER TABLE students ADD COLUMN phone VARCHAR(20);
```

This works, once. It also means: nobody else's local database has this column, there's no record of _when_ or _why_ it was added, and if the production server is ever rebuilt from scratch, this change is gone unless someone remembers to redo it. Migrations solve all three problems by turning schema changes into version-controlled files that run in a defined order.

---

## Prisma Migrations in Practice

Nexus already uses this workflow — this is what actually runs behind `prisma migrate dev`:

```prisma
// schema.prisma — the source of truth for what the schema SHOULD look like
model Student {
  id        Int      @id @default(autoincrement())
  fullName  String
  grade     Int
  phone     String?  // ← adding this line is the change
  createdAt DateTime @default(now())
}
```

```bash
# Prisma compares schema.prisma against the database's current state,
# generates the SQL needed to close the gap, and saves it as a migration file
npx prisma migrate dev --name add_student_phone
```

```sql
-- prisma/migrations/20260315120000_add_student_phone/migration.sql
-- This file is generated automatically — and it's exactly the kind of
-- statement you'd write by hand, just tracked and repeatable
ALTER TABLE "Student" ADD COLUMN "phone" TEXT;
```

That generated `.sql` file gets committed to git alongside the code that needs it. Anyone who pulls the branch and runs `prisma migrate dev` gets the exact same schema change applied to their own local database, in the exact same order as everyone else.

---

## The Insert / Update / Delete Statements Underneath

Migrations mostly change _structure_ (`ALTER TABLE`, `CREATE TABLE`). Actually writing and modifying _data_ uses the other three CRUD statements from Module 00:

```sql
-- INSERT — add a new row
INSERT INTO students (full_name, grade, email)
VALUES ('Thanuga Fernando', 10, 'thanuga@school.lk');

-- UPDATE — modify existing rows (ALWAYS include a WHERE clause)
UPDATE students
SET grade = 11
WHERE id = 4;

-- DELETE — remove rows (ALWAYS include a WHERE clause)
DELETE FROM students
WHERE id = 4;
```

**An `UPDATE` or `DELETE` with no `WHERE` clause applies to every row in the table.** This is one of the most common — and most damaging — mistakes in all of SQL. Before running either statement against a real database, run the equivalent `SELECT` with the same `WHERE` clause first, confirm it returns exactly the rows you expect, and only then swap `SELECT *` for `UPDATE`/`DELETE`.

```ts
// The Prisma equivalents, which you're already using in Nexus
await prisma.student.create({
  data: { fullName: "Thanuga Fernando", grade: 10 },
});
await prisma.student.update({ where: { id: 4 }, data: { grade: 11 } });
await prisma.student.delete({ where: { id: 4 } });
```

---

## Migrations in a Team / CI Context

```bash
# Local development — creates AND applies a new migration, safe to
# re-run since it tracks what's already been applied
npx prisma migrate dev --name description_of_change

# Production / CI — applies any pending migrations that haven't run yet,
# but never generates new ones or prompts for input
npx prisma migrate deploy
```

`migrate dev` is interactive and meant for your own machine while building a feature. `migrate deploy` is the non-interactive version meant for a CI/CD pipeline or a production deploy step — it applies whatever migrations exist in the `prisma/migrations` folder and does nothing else. Mixing these up (running `migrate dev` against production) is a common and risky mistake, since `dev` can, in some cases, offer to reset the database if it detects drift.

---

## Handling a Breaking Schema Change Safely

Some changes are risky to ship in one step — most commonly, adding a `NOT NULL` column to a table that already has rows (existing rows have no value to put there). The safe pattern is to split it into stages:

```sql
-- Step 1: add the column as nullable, deploy
ALTER TABLE students ADD COLUMN phone TEXT;

-- Step 2: backfill existing rows with a real or placeholder value
UPDATE students SET phone = 'unknown' WHERE phone IS NULL;

-- Step 3: once every row has a value, THEN enforce it going forward
ALTER TABLE students ALTER COLUMN phone SET NOT NULL;
```

Doing all three in a single migration against a table that already has data fails outright — the `NOT NULL` constraint has no way to satisfy itself for rows that predate the column.

---

## Common Mistakes

- **Editing a database schema by hand instead of through a migration**, even "just this once." It immediately breaks the guarantee that every environment (your machine, a teammate's machine, production) is running the same schema.
- **Running `UPDATE`/`DELETE` without a `WHERE` clause**, or with a `WHERE` clause that's broader than intended. Always run the equivalent `SELECT` first to confirm exactly which rows will be affected.
- **Adding a `NOT NULL` column directly to a table with existing data**, without a backfill step. The migration will simply fail against any table that already has rows.
- **Committing a migration file that was manually edited after generation.** Prisma's migration files are meant to be generated, applied once, and left alone — hand-editing a migration that's already been applied elsewhere creates drift between environments that Prisma has no way to detect or reconcile.
