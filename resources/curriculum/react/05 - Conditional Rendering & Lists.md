> Showing different UI based on conditions, and turning arrays of data into arrays of components — these are the two most common tasks in any React application. This file goes beyond the basics to cover the patterns, the gotchas, and how to handle real-world data.

---

## Table of Contents

1. [Conditional Rendering Patterns](#1-conditional-rendering-patterns)
2. [Guard Clauses in Components](#2-guard-clauses-in-components)
3. [Loading, Error, and Empty States](#3-loading-error-and-empty-states)
4. [Rendering Lists — In Depth](#4-rendering-lists--in-depth)
5. [Keys — The Full Story](#5-keys--the-full-story)
6. [Nested Lists](#6-nested-lists)
7. [Dynamic Component Selection](#7-dynamic-component-selection)
8. [Real-World Patterns](#8-real-world-patterns)

---

## 1. Conditional Rendering Patterns

React has no template syntax for conditionals — you use JavaScript directly.

### Ternary — two outcomes

```jsx
// Simple two-way condition
<div>{isLoggedIn ? <Dashboard /> : <LoginPage />}</div>

// When one side is null (still valid JSX)
<div>{hasError ? <ErrorBanner message={error} /> : null}</div>
```

### `&&` — render or nothing

```jsx
// Render only when true
{isAdmin && <AdminPanel />}
{notifications.length > 0 && <NotificationBadge count={notifications.length} />}

// ⚠️ The 0 gotcha — numbers are rendered, not skipped
{count && <Badge />}         // renders "0" when count is 0!
{count > 0 && <Badge />}     // ✓ — safe
{!!count && <Badge />}       // ✓ — coerce to boolean
{count !== 0 && <Badge />}   // ✓ — explicit
```

### Early returns — for complex conditions

```jsx
// When a component has fundamentally different outputs based on a condition
function UserGreeting({ user }) {
  if (!user) {
    return <p>Please log in.</p>;
  }

  if (user.isBanned) {
    return <BannedMessage reason={user.banReason} />;
  }

  if (!user.isVerified) {
    return <VerifyEmailPrompt email={user.email} />;
  }

  // Default happy path
  return <p>Welcome back, {user.name}!</p>;
}
```

### Variables — compute before rendering

```jsx
function StatusBadge({ status, count }) {
  // Compute the rendering data before the return
  const badgeConfig = {
    active:   { label: "Active",   className: "badge-green" },
    pending:  { label: "Pending",  className: "badge-yellow" },
    inactive: { label: "Inactive", className: "badge-grey" },
  }[status] ?? { label: "Unknown", className: "badge-grey" };

  const showCount = count > 0;

  return (
    <span className={badgeConfig.className}>
      {badgeConfig.label}
      {showCount && <sup>{count}</sup>}
    </span>
  );
}
```

### `switch` in a helper function

```jsx
function renderPaymentMethod(method) {
  switch (method) {
    case "card":   return <CardPaymentForm />;
    case "bank":   return <BankTransferForm />;
    case "crypto": return <CryptoPaymentForm />;
    default:       return <p>Select a payment method</p>;
  }
}

function CheckoutPage({ paymentMethod }) {
  return (
    <div>
      <h2>Payment</h2>
      {renderPaymentMethod(paymentMethod)}
    </div>
  );
}
```

---

## 2. Guard Clauses in Components

Guard clauses return early when conditions aren't met, keeping the main logic clean.

```jsx
function StudentReport({ studentId, data, isLoading, error }) {
  // Guards — handle edge cases first
  if (!studentId) return null;
  if (isLoading) return <Spinner />;
  if (error) return <ErrorMessage message={error} />;
  if (!data) return <EmptyState message="No data available" />;

  // Happy path — all the complex UI
  return (
    <div className="report">
      <ReportHeader student={data.student} />
      <ScoreChart scores={data.scores} />
      <SubjectBreakdown subjects={data.subjects} />
    </div>
  );
}
```

---

## 3. Loading, Error, and Empty States

Every component that fetches data has three states beyond "success". Handling all of them is what separates production-quality UIs from prototypes.

```jsx
function StudentList() {
  const [students, setStudents] = useState([]);
  const [status, setStatus] = useState("loading");
  const [error, setError] = useState(null);

  useEffect(() => {
    fetchStudents()
      .then(data => {
        setStudents(data);
        setStatus("success");
      })
      .catch(err => {
        setError(err.message);
        setStatus("error");
      });
  }, []);

  // Loading state
  if (status === "loading") {
    return (
      <div className="loading-container">
        <Spinner size="lg" />
        <p>Loading students...</p>
      </div>
    );
  }

  // Error state
  if (status === "error") {
    return (
      <div className="error-container">
        <AlertCircleIcon />
        <p>Failed to load students: {error}</p>
        <button onClick={() => window.location.reload()}>Try again</button>
      </div>
    );
  }

  // Empty state — not an error, but no data
  if (students.length === 0) {
    return (
      <div className="empty-container">
        <UsersIcon />
        <p>No students found</p>
        <button onClick={openAddStudentModal}>Add first student</button>
      </div>
    );
  }

  // Success state
  return (
    <ul>
      {students.map(student => (
        <StudentCard key={student.id} student={student} />
      ))}
    </ul>
  );
}
```

### Skeleton loading — better UX than spinners

```jsx
function StudentCardSkeleton() {
  return (
    <div className="card skeleton">
      <div className="skeleton-avatar" />
      <div className="skeleton-line skeleton-line-lg" />
      <div className="skeleton-line skeleton-line-sm" />
    </div>
  );
}

function StudentGrid({ isLoading, students }) {
  if (isLoading) {
    return (
      <div className="grid">
        {Array.from({ length: 6 }, (_, i) => (
          <StudentCardSkeleton key={i} />
        ))}
      </div>
    );
  }

  return (
    <div className="grid">
      {students.map(s => <StudentCard key={s.id} student={s} />)}
    </div>
  );
}
```

---

## 4. Rendering Lists — In Depth

`.map()` is the primary tool. Everything flows from it.

```jsx
function CourseList({ courses }) {
  return (
    <div className="course-grid">
      {courses.map(course => (
        <CourseCard
          key={course.id}
          title={course.title}
          teacher={course.teacher}
          studentCount={course.studentCount}
          onEnroll={() => handleEnroll(course.id)}
        />
      ))}
    </div>
  );
}
```

### Filtering before rendering

```jsx
function ActiveStudentList({ students }) {
  return (
    <ul>
      {students
        .filter(s => s.isActive)
        .map(s => (
          <li key={s.id}>{s.name}</li>
        ))
      }
    </ul>
  );
}
```

### Sorting before rendering

```jsx
function LeaderBoard({ students }) {
  // Sort a copy — never mutate props
  const sorted = [...students].sort((a, b) => b.score - a.score);

  return (
    <ol>
      {sorted.map((student, index) => (
        <li key={student.id}>
          <span className="rank">#{index + 1}</span>
          <span>{student.name}</span>
          <span className="score">{student.score}</span>
        </li>
      ))}
    </ol>
  );
}
```

### Grouping before rendering

```jsx
function StudentsByGrade({ students }) {
  // Group students by grade
  const byGrade = Object.groupBy(students, s => s.grade);
  // Or manually:
  // const byGrade = students.reduce((groups, s) => {
  //   (groups[s.grade] ??= []).push(s);
  //   return groups;
  // }, {});

  const grades = Object.keys(byGrade).sort((a, b) => Number(a) - Number(b));

  return (
    <div>
      {grades.map(grade => (
        <section key={grade}>
          <h2>Grade {grade}</h2>
          <ul>
            {byGrade[grade].map(student => (
              <li key={student.id}>{student.name}</li>
            ))}
          </ul>
        </section>
      ))}
    </div>
  );
}
```

---

## 5. Keys — The Full Story

Keys tell React which item in a list corresponds to which DOM element between renders.

```jsx
// ✓ Best: use stable, unique IDs from your data
students.map(s => <StudentCard key={s.id} />)

// ✓ Acceptable: stable natural unique values
countries.map(c => <Option key={c.code} value={c.code}>{c.name}</Option>)
navItems.map(item => <NavLink key={item.path} to={item.path}>{item.label}</NavLink>)

// ✗ Never: index when list can change
students.map((s, i) => <StudentCard key={i} />)
// If you insert at position 0, all keys shift
// React matches "new key 0" with "old key 0" — wrong student!
// Component state (like expanded/collapsed) will attach to wrong items

// ✓ Index is ok when:
// - List is static (never reordered, never items inserted/removed at non-end)
// - Items have no unique identifier
// - The component has no internal state to preserve
const staticSteps = ["Select grade", "Enter details", "Review"];
staticSteps.map((step, i) => <Step key={i} label={step} />)  // fine

// ✗ Never: random values or timestamps
students.map(s => <StudentCard key={Math.random()} />)  // new key every render → remount
students.map(s => <StudentCard key={Date.now()} />)     // same problem
```

### What happens with wrong keys

```jsx
// State gets attached to the wrong item:
function TodoItem() {
  const [isDone, setIsDone] = useState(false);  // internal state
  return (
    <li>
      <input type="checkbox" checked={isDone} onChange={() => setIsDone(d => !d)} />
    </li>
  );
}

// If using index as key and you insert a todo at position 0:
// Old: [A(key=0, isDone=true), B(key=1), C(key=2)]
// New: [X(key=0), A(key=1), B(key=2), C(key=3)]
// React matches key=0 → X gets A's isDone=true state (BUG)
// Use unique IDs to avoid this entirely
```

---

## 6. Nested Lists

Nested lists need keys at each level.

```jsx
function SubjectList({ classes }) {
  return (
    <div>
      {classes.map(cls => (
        <div key={cls.id}>
          <h2>{cls.name}</h2>
          <ul>
            {cls.subjects.map(subject => (
              // subject.id is unique within the full dataset
              <li key={subject.id}>
                {subject.name} — {subject.teacher}
              </li>
            ))}
          </ul>
        </div>
      ))}
    </div>
  );
}
```

---

## 7. Dynamic Component Selection

Sometimes you need to choose which component to render based on a value.

```jsx
// Component map — clean and type-safe
const CARD_COMPONENTS = {
  student:   StudentCard,
  teacher:   TeacherCard,
  admin:     AdminCard,
  parent:    ParentCard,
};

function UserCard({ user }) {
  const Component = CARD_COMPONENTS[user.role] ?? DefaultCard;
  return <Component user={user} />;
}

// Usage
<UserCard user={{ role: "teacher", name: "Mr. Silva" }} />
// → renders <TeacherCard user={...} />
```

```jsx
// Dynamic import for code splitting — load component only when needed
import { lazy, Suspense } from "react";

const HeavyChart = lazy(() => import("./HeavyChart"));

function ReportPage({ showChart }) {
  return (
    <div>
      <ReportTable />
      {showChart && (
        <Suspense fallback={<Spinner />}>
          <HeavyChart />    {/* loaded only when showChart is true */}
        </Suspense>
      )}
    </div>
  );
}
```

---

## 8. Real-World Patterns

### Search + filter + sort in one component

```jsx
function StudentsPage({ students }) {
  const [query, setQuery] = useState("");
  const [gradeFilter, setGradeFilter] = useState("all");
  const [sortBy, setSortBy] = useState("name");

  // All derived — no extra state needed
  const processed = students
    .filter(s => gradeFilter === "all" || s.grade === Number(gradeFilter))
    .filter(s => s.name.toLowerCase().includes(query.toLowerCase()))
    .sort((a, b) => {
      if (sortBy === "name")  return a.name.localeCompare(b.name);
      if (sortBy === "score") return b.score - a.score;
      if (sortBy === "grade") return a.grade - b.grade;
      return 0;
    });

  return (
    <div>
      <div className="controls">
        <input
          value={query}
          onChange={e => setQuery(e.target.value)}
          placeholder="Search by name..."
        />
        <select value={gradeFilter} onChange={e => setGradeFilter(e.target.value)}>
          <option value="all">All Grades</option>
          {[6,7,8,9,10,11,12,13].map(g => (
            <option key={g} value={g}>Grade {g}</option>
          ))}
        </select>
        <select value={sortBy} onChange={e => setSortBy(e.target.value)}>
          <option value="name">Name</option>
          <option value="score">Score</option>
          <option value="grade">Grade</option>
        </select>
      </div>

      {processed.length === 0 ? (
        <EmptyState message="No students match your filters" />
      ) : (
        <div className="grid">
          {processed.map(s => <StudentCard key={s.id} student={s} />)}
        </div>
      )}

      <p className="results-count">
        Showing {processed.length} of {students.length} students
      </p>
    </div>
  );
}
```

### Virtualised lists — when you have thousands of items

For very long lists (1000+ items), rendering all items causes performance problems. Use a virtualisation library that only renders visible items.

```jsx
// Using @tanstack/react-virtual
import { useVirtualizer } from "@tanstack/react-virtual";

function VirtualStudentList({ students }) {
  const parentRef = useRef(null);

  const virtualizer = useVirtualizer({
    count: students.length,
    getScrollElement: () => parentRef.current,
    estimateSize: () => 64,  // estimated height of each row
  });

  return (
    <div ref={parentRef} className="list-container" style={{ height: "600px", overflow: "auto" }}>
      <div style={{ height: virtualizer.getTotalSize() }}>
        {virtualizer.getVirtualItems().map(virtualItem => (
          <div
            key={virtualItem.key}
            style={{
              position: "absolute",
              top: 0,
              transform: `translateY(${virtualItem.start}px)`,
            }}
          >
            <StudentRow student={students[virtualItem.index]} />
          </div>
        ))}
      </div>
    </div>
  );
}
// Only renders ~10-20 rows, regardless of how many students there are
```

---

## Summary

```
Conditional rendering patterns:
  Ternary:         condition ? <A /> : <B />
  &&:              condition && <A />  — careful: 0 renders as "0"
  Early return:    return null / <Loading /> / <Error /> before happy path
  Variables:       compute before return, use in JSX
  Helper function: extract complex conditions to a function

Guard clauses:
  Check and return early for null, loading, error, empty
  Keeps the happy path at the bottom, clean and readable

Three states beyond success:
  Loading  → Spinner or skeleton
  Error    → Error message + retry
  Empty    → Empty state with call to action
  All three must be handled for production-quality UI

Lists:
  .map() to transform data → JSX elements
  Always provide a key prop on the outermost element
  Filter, sort, group before mapping — all derived from state, not extra state
  Nested lists: key at each nesting level

Keys:
  Unique stable identifier from the data (database ID is best)
  Never index (when list can change) — state attaches to wrong item
  Never Math.random() or Date.now() — remounts on every render
  Index is fine for truly static lists with no internal state

Dynamic component selection:
  { role: ComponentMap }[role] — clean way to pick component by type

Virtualisation:
  For 500+ items: @tanstack/react-virtual
  Renders only visible items — massive performance gain
```

---

_Next: [06 — Event Handling](./06%20-%20Event%20Handling.md)_