> `useMemo` and `useCallback` are performance optimisation hooks. The most important thing to understand about them is also the most commonly ignored: **they are not free**. They have a cost — memory overhead and comparison work on every render. Use them only when you have measured a performance problem, not as a precaution.

---

## Table of Contents

1. [What Problem These Hooks Solve](#1-what-problem-these-hooks-solve)
2. [useMemo — Memoising Computed Values](#2-usememo--memoising-computed-values)
3. [useCallback — Memoising Functions](#3-usecallback--memoising-functions)
4. [React.memo — Skipping Re-renders](#4-reactmemo--skipping-re-renders)
5. [When to Actually Use These](#5-when-to-actually-use-these)
6. [When NOT to Use These](#6-when-not-to-use-these)
7. [The Dependency Array Rules](#7-the-dependency-array-rules)
8. [Real-World Examples](#8-real-world-examples)

---

## 1. What Problem These Hooks Solve

On every render, React re-runs your component function. Every value is recomputed. Every function is recreated.

```jsx
function StudentList({ students, filters }) {
  // This runs on EVERY render — even if students and filters haven't changed
  const filtered = students
    .filter((s) => s.grade === filters.grade)
    .sort((a, b) => a.name.localeCompare(b.name));

  // This function is a NEW function reference on every render
  const handleDelete = (id) => deleteStudent(id);

  return filtered.map((s) => (
    <StudentCard key={s.id} student={s} onDelete={handleDelete} />
  ));
}
```

Usually this is fine — React is fast. But sometimes:

1. A computation is expensive (filtering/sorting 10,000 items)
2. A child component is wrapped in `React.memo` and gets a new function reference every render — causing it to re-render unnecessarily

That's when `useMemo` and `useCallback` help.

---

## 2. `useMemo` — Memoising Computed Values

`useMemo` caches the return value of a function. It only recomputes when dependencies change.

```jsx
import { useMemo } from "react";

const memoisedValue = useMemo(() => expensiveComputation(a, b), [a, b]);
```

```jsx
function StudentReport({ students, filters }) {
  // Only recomputes when students or filters changes
  const processedStudents = useMemo(() => {
    return students
      .filter((s) => {
        if (filters.grade && s.grade !== filters.grade) return false;
        if (filters.query && !s.name.toLowerCase().includes(filters.query))
          return false;
        return true;
      })
      .sort((a, b) => {
        if (filters.sortBy === "name") return a.name.localeCompare(b.name);
        if (filters.sortBy === "score") return b.score - a.score;
        return 0;
      });
  }, [students, filters]);

  // Also memoised — only recalculates when processedStudents changes
  const stats = useMemo(
    () => ({
      count: processedStudents.length,
      average:
        processedStudents.reduce((sum, s) => sum + s.score, 0) /
        processedStudents.length,
      top: processedStudents[0],
    }),
    [processedStudents],
  );

  return (
    <div>
      <ReportStats stats={stats} />
      <StudentTable students={processedStudents} />
    </div>
  );
}
```

---

## 3. `useCallback` — Memoising Functions

`useCallback` caches the function itself. It returns the same function reference as long as dependencies haven't changed.

```jsx
import { useCallback } from "react";

const memoisedFn = useCallback(() => {
  doSomething(a, b);
}, [a, b]);
```

```jsx
function StudentList({ onRefresh }) {
  const [students, setStudents] = useState([]);

  // Without useCallback: new function on every render
  // With useCallback: same reference until id changes
  const handleDelete = useCallback(
    async (id) => {
      await deleteStudent(id);
      setStudents((prev) => prev.filter((s) => s.id !== id));
      onRefresh();
    },
    [onRefresh],
  ); // stable as long as onRefresh is stable

  return students.map((s) => (
    <StudentCard
      key={s.id}
      student={s}
      onDelete={handleDelete} // same reference = StudentCard can use React.memo
    />
  ));
}
```

---

## 4. `React.memo` — Skipping Re-renders

`React.memo` wraps a component so it only re-renders when its props change. Without it, `useMemo`/`useCallback` have no effect on child re-renders.

```jsx
import { memo } from "react";

// StudentCard will only re-render if its props change
const StudentCard = memo(function StudentCard({ student, onDelete }) {
  console.log("StudentCard rendered:", student.name);

  return (
    <div className="card">
      <h3>{student.name}</h3>
      <p>Score: {student.score}</p>
      <button onClick={() => onDelete(student.id)}>Delete</button>
    </div>
  );
});
```

**The trinity — all three must work together:**

```
React.memo   →  skips re-render if props are reference-equal
useCallback  →  keeps function reference stable
useMemo      →  keeps object/array reference stable

If StudentCard is wrapped in memo BUT onDelete is a new function every render:
  React.memo comparison fails (new function ≠ same function)
  → StudentCard re-renders anyway
  → useCallback not used → memo is wasted

All three work together or none of them do.
```

```jsx
// Complete example — all three working together
const StudentCard = memo(function StudentCard({ student, onDelete, onEdit }) {
  return (
    <div>
      <p>{student.name}</p>
      <button onClick={() => onDelete(student.id)}>Delete</button>
      <button onClick={() => onEdit(student)}>Edit</button>
    </div>
  );
});

function StudentPage({ courseId }) {
  const [students, setStudents] = useState([]);

  // Stable reference — only recreates if setStudents changes (it never does)
  const handleDelete = useCallback((id) => {
    setStudents((prev) => prev.filter((s) => s.id !== id));
  }, []);

  const handleEdit = useCallback((student) => {
    openEditModal(student);
  }, []);

  return students.map((s) => (
    <StudentCard
      key={s.id}
      student={s}
      onDelete={handleDelete} // stable reference
      onEdit={handleEdit} // stable reference
    />
  ));
}
// Now StudentCard only re-renders when its specific student object changes
```

---

## 5. When to Actually Use These

### `useMemo` — use when:

```jsx
// ✓ Genuinely expensive computation (> ~1ms, called on every render)
const sortedData = useMemo(
  () => hugeDataset.sort(complexCompareFn),
  [hugeDataset],
);

// ✓ Stable object reference needed by a memoised child or useEffect
const queryOptions = useMemo(
  () => ({
    page,
    limit: 20,
    sortBy,
    filters,
  }),
  [page, sortBy, filters],
);

// ✓ Referential equality for useEffect deps
useEffect(() => {
  fetchData(queryOptions);
}, [queryOptions]); // without memo, new object every render → infinite loop
```

### `useCallback` — use when:

```jsx
// ✓ Passed to a component wrapped in React.memo
<MemoizedChild onAction={useCallback(() => doThing(), [])} />;

// ✓ Listed as a useEffect dependency
const fetchData = useCallback(() => {
  fetch(url).then(setData);
}, [url]);

useEffect(() => {
  fetchData();
}, [fetchData]); // stable reference = effect doesn't run on every render
```

---

## 6. When NOT to Use These

### Don't wrap everything

```jsx
// ✗ Useless — simple computation doesn't need memoisation
const doubled = useMemo(() => count * 2, [count]);
// Just do: const doubled = count * 2

// ✗ Useless — component isn't wrapped in React.memo
<RegularChild onClick={useCallback(() => setOpen(true), [])} />;
// The component re-renders on every parent render anyway

// ✗ Useless — primitive value comparison is already cheap
const isValid = useMemo(() => email.includes("@"), [email]);
// Just do: const isValid = email.includes("@")
```

### The cost of memoisation

```
Every useMemo/useCallback:
  - Stores the memoised value in memory
  - Runs dependency comparison on every render
  - Adds cognitive overhead (more code to read)

If the computation is cheap: memoisation costs more than the computation saves.
```

### When the benefit is zero

```jsx
// React.memo with no useCallback = useless memo
const Child = memo(function Child({ onClick }) {
  return <button onClick={onClick}>Click</button>;
});

function Parent() {
  // New function every render — memo comparison ALWAYS fails
  return <Child onClick={() => doThing()} />;
  // Child re-renders on every Parent render — same as without memo
}
```

---

## 7. The Dependency Array Rules

Same rules as `useEffect` — all values from component scope that are used inside must be listed.

```jsx
// ✗ Missing dependency — stale computation
const result = useMemo(() => {
  return processData(data, config); // config is used
}, [data]); // missing config!

// ✓ Complete dependencies
const result = useMemo(() => {
  return processData(data, config);
}, [data, config]);

// ✗ Missing callback dependency
const handler = useCallback(() => {
  onComplete(value); // value and onComplete used
}, []); // both missing!

// ✓ Complete
const handler = useCallback(() => {
  onComplete(value);
}, [value, onComplete]);
```

**Empty dependency array `[]` means**: memoised once, never recalculated. Only valid if the computation truly has no dependencies.

```jsx
// ✓ Valid empty array — genuinely no dependencies
const constants = useMemo(
  () => ({
    MAX_ITEMS: 100,
    API_VERSION: "v2",
  }),
  [],
);

// ✓ Valid empty array — only uses setters (which are stable)
const handleReset = useCallback(() => {
  setCount(0);
  setName("");
}, []); // setCount and setName are stable — don't need to be listed
// (React guarantees setter functions from useState are stable)
```

---

## 8. Real-World Examples

### Expensive search/filter

```jsx
function StudentSearch({ students }) {
  const [query, setQuery] = useState("");
  const [grade, setGrade] = useState(null);

  const results = useMemo(() => {
    // This runs on every render without memo
    // With 5000 students and debounced search, this is measurably slow
    return students.filter((student) => {
      const matchesQuery =
        !query ||
        student.name.toLowerCase().includes(query.toLowerCase()) ||
        student.email.toLowerCase().includes(query.toLowerCase());

      const matchesGrade = grade === null || student.grade === grade;

      return matchesQuery && matchesGrade;
    });
  }, [students, query, grade]);

  return (
    <>
      <SearchControls
        query={query}
        onQueryChange={setQuery}
        grade={grade}
        onGradeChange={setGrade}
      />
      <p>{results.length} results</p>
      <StudentGrid students={results} />
    </>
  );
}
```

### Stable callback for external subscription

```jsx
function useEventSource(url, onMessage) {
  // If onMessage changed reference on every render, this effect would
  // reconnect to the EventSource on every render — catastrophic
  const stableOnMessage = useCallback(onMessage, []); // ← careful: only if onMessage is truly stable

  useEffect(() => {
    const es = new EventSource(url);
    es.onmessage = (e) => stableOnMessage(JSON.parse(e.data));
    return () => es.close();
  }, [url, stableOnMessage]);
}
```

### Context value memoisation

```jsx
function AppProvider({ children }) {
  const [user, setUser] = useState(null);
  const [settings, setSettings] = useState(defaultSettings);

  // Without memo: new object on every render → every context consumer re-renders
  const value = useMemo(
    () => ({
      user,
      settings,
      updateSettings: (changes) =>
        setSettings((prev) => ({ ...prev, ...changes })),
      logout: () => {
        setUser(null);
        clearToken();
      },
    }),
    [user, settings],
  );

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
}
```

---

## Summary

```
The problem:
  On every render, values are recomputed and functions are recreated.
  Usually fine. Only matters when: expensive computation, or memoised children.

useMemo:
  useMemo(() => computation, [deps])
  Caches the computed VALUE
  Recomputes only when deps change
  Use for: expensive filtering/sorting, stable objects for useEffect deps

useCallback:
  useCallback(fn, [deps])
  Caches the FUNCTION REFERENCE
  Returns same function object as long as deps don't change
  Use for: passed to React.memo children, listed as useEffect deps

React.memo:
  memo(Component) — skip re-render if props are reference-equal
  Works only when combined with useCallback/useMemo for object/function props

The trinity:
  memo + useCallback + useMemo must work together
  memo alone won't help if props change every render (new function/object refs)

CRITICAL — performance anti-patterns:
  Don't wrap everything — memoisation has a cost
  Only use when: computation is actually slow, or component re-renders too often
  Measure first — don't optimise what isn't a problem

Dependency array:
  Same rules as useEffect — list everything used from component scope
  Empty [] = memoised once forever (only valid if truly no deps)
  useState setters are stable — don't need to be listed
```

---

_Next: [13 — Custom Hooks](./13%20-%20Custom%20Hooks.md)_
