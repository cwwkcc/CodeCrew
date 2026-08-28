Module 01 introduced foreign keys — a column in one table pointing at a row in another. A **join** is how you actually combine data across that link in a single query. This is the module that makes relational databases click.

---

## Why Joins Exist

```
students:                       departments:
| id | name    | department_id |     | id | name    |
|----|---------|----------------|     |----|---------|
| 1  | Ashan   | 3              |     | 1  | Science |
| 2  | Nadeesha| 1              |     | 3  | ICT     |
```

`students.department_id` is just a number — `3`. On its own it tells you nothing readable. A join looks that number up in `departments` and pulls in the matching row, so a query can return `"ICT"` instead of `3`.

---

## `INNER JOIN` — Only Matching Rows

```sql
SELECT students.name, departments.name AS department_name
FROM students
INNER JOIN departments ON students.department_id = departments.id;
```

```
name    | department_name
--------|------------------
Ashan   | ICT
Nadeesha| Science
```

`INNER JOIN` (usually just written `JOIN`) returns only rows where the match succeeds on **both** sides. A student with a `department_id` that doesn't exist in `departments` (data integrity problem — this is exactly what the `REFERENCES` foreign key constraint from Module 01 prevents) simply wouldn't appear in the result at all.

---

## `LEFT JOIN` — Keep Everything on One Side

```sql
-- Every student, even ones with no department assigned yet (department_id IS NULL)
SELECT students.name, departments.name AS department_name
FROM students
LEFT JOIN departments ON students.department_id = departments.id;
```

```
name    | department_name
--------|------------------
Ashan   | ICT
Nadeesha| Science
Vindi   | NULL              ← kept, even though there's no matching department
```

`LEFT JOIN` keeps every row from the "left" (first-named) table regardless of whether a match exists on the right — filling in `NULL` where there's nothing to join. This is the join you reach for whenever "show me everything, plus related data if it exists" is the actual requirement — e.g. "every student, plus their most recent enrollment if they have one."

| Join type    | Keeps unmatched rows from...                                                                              |
| ------------ | --------------------------------------------------------------------------------------------------------- |
| `INNER JOIN` | Neither side — only rows matched on both                                                                  |
| `LEFT JOIN`  | The left (first) table                                                                                    |
| `RIGHT JOIN` | The right (second) table — rare in practice; usually written as a `LEFT JOIN` with tables swapped instead |
| `FULL JOIN`  | Both sides                                                                                                |

---

## The Three Relationship Shapes

### One-to-Many

The most common relationship: one department has many students, but each student belongs to exactly one department. The foreign key lives on the "many" side:

```sql
CREATE TABLE departments (id SERIAL PRIMARY KEY, name VARCHAR(100));
CREATE TABLE students (
  id SERIAL PRIMARY KEY,
  name VARCHAR(120),
  department_id INTEGER REFERENCES departments(id)
);
```

### Many-to-Many

A student can enroll in many subjects, and each subject has many students. Neither table can hold a single foreign key for this — it needs a **join table** in between:

```sql
CREATE TABLE subjects (id SERIAL PRIMARY KEY, name VARCHAR(100));

-- The join table — each row represents ONE student-subject pairing
CREATE TABLE enrollments (
  student_id INTEGER REFERENCES students(id),
  subject_id INTEGER REFERENCES subjects(id),
  grade      SMALLINT,
  PRIMARY KEY (student_id, subject_id)   -- a composite primary key
);
```

```sql
-- Getting from one side to the other now needs TWO joins
SELECT students.name, subjects.name AS subject_name, enrollments.grade
FROM enrollments
JOIN students ON students.id = enrollments.student_id
JOIN subjects ON subjects.id = enrollments.subject_id;
```

### One-to-One

Rare, but occasionally the right call — splitting a table in two for a genuinely separate concern (e.g. `students` and `student_profiles`, where the profile holds large or sensitive fields that most queries don't need):

```sql
CREATE TABLE student_profiles (
  student_id INTEGER PRIMARY KEY REFERENCES students(id),
  bio TEXT,
  emergency_contact VARCHAR(255)
);
```

The `student_id` being _both_ the primary key and a foreign key is what enforces "exactly one profile per student" — a plain foreign key alone would allow a student to have several profile rows.

---

## How This Maps to Prisma

Every relationship above has a direct Prisma equivalent that generates the same underlying SQL:

```prisma
model Department {
  id       Int       @id @default(autoincrement())
  name     String
  students Student[]              // "many" side — no column, just a reverse relation
}

model Student {
  id           Int         @id @default(autoincrement())
  name         String
  departmentId Int?
  department   Department? @relation(fields: [departmentId], references: [id])
  enrollments  Enrollment[]
}

model Subject {
  id          Int          @id @default(autoincrement())
  name        String
  enrollments Enrollment[]
}

// The join table for the many-to-many relationship
model Enrollment {
  studentId Int
  subjectId Int
  grade     Int?
  student   Student @relation(fields: [studentId], references: [id])
  subject   Subject @relation(fields: [subjectId], references: [id])

  @@id([studentId, subjectId])
}
```

`prisma.student.findMany({ include: { department: true } })` is Prisma generating the `LEFT JOIN` from earlier, automatically. Understanding the SQL underneath is what lets you recognize when a slow Prisma query is doing something expensive (like an N+1 query pattern — covered in Module 04) instead of just accepting whatever the ORM produces.

---

## Common Mistakes

- **Using `INNER JOIN` where `LEFT JOIN` was needed.** If "students with no department yet" silently disappear from a report, this is almost always the cause — `INNER JOIN` drops any row that doesn't match on both sides.
- **Forgetting the `ON` condition, or joining on the wrong columns.** A join with no condition (or the wrong one) produces a **cross join** — every row from the first table paired with every row from the second, which explodes into millions of nonsense rows on tables of any real size.
- **Trying to model a many-to-many relationship with two foreign keys on one table.** It doesn't work — a "many-to-many" always needs a separate join table (or Prisma's implicit many-to-many, which creates one for you behind the scenes) because neither original table can hold a _list_ of foreign keys in a single column.
