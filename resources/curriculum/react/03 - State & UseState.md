> State is what makes a UI interactive. It's data that changes over time — and when it changes, React re-renders the component to reflect the new reality. `useState` is the hook that gives components their own memory. Getting state right is the most important skill in React.

---

## Table of Contents

1. [What State Is](#1-what-state-is)
2. [useState — The Hook](#2-usestate--the-hook)
3. [State Updates Are Asynchronous](#3-state-updates-are-asynchronous)
4. [Functional Updates](#4-functional-updates)
5. [State with Objects and Arrays](#5-state-with-objects-and-arrays)
6. [Multiple State Variables](#6-multiple-state-variables)
7. [Derived State — Don't Store What You Can Compute](#7-derived-state--dont-store-what-you-can-compute)
8. [Initialising State Lazily](#8-initialising-state-lazily)
9. [Resetting State with Key](#9-resetting-state-with-key)
10. [State Design Patterns](#10-state-design-patterns)

---

## 1. What State Is

**State** is data that:

1. Changes over time
2. When it changes, causes the component to re-render

Compare with props (data from the parent, read-only) and regular variables (don't trigger re-renders):

```jsx
function Counter() {
  // ✗ Regular variable — changing it won't update the UI
  let count = 0;

  function handleClick() {
    count++;  // changes the variable but React doesn't know
    console.log(count);  // increments, but UI never updates
  }

  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={handleClick}>Increment</button>
    </div>
  );
}
```

React doesn't watch your variables. It only re-renders when **state** changes — state that you declare with `useState`.

---

## 2. `useState` — The Hook

```jsx
import { useState } from "react";

function Counter() {
  // Declare a state variable
  // useState returns [currentValue, setterFunction]
  const [count, setCount] = useState(0);  // 0 is the initial value

  function handleIncrement() {
    setCount(count + 1);  // schedule a state update → triggers re-render
  }

  return (
    <div>
      <p>Count: {count}</p>
      <button onClick={handleIncrement}>+</button>
      <button onClick={() => setCount(count - 1)}>-</button>
      <button onClick={() => setCount(0)}>Reset</button>
    </div>
  );
}
```

### What happens on `setCount(count + 1)`:

1. React schedules a re-render of `Counter`
2. React calls `Counter()` again
3. `useState(0)` returns the new count (not 0 — React remembers the updated value)
4. The component returns new JSX
5. React diffs old and new JSX, updates the DOM

The `0` in `useState(0)` is **only used on the very first render**. After that, React ignores the initial value argument.

### Rules of Hooks

Hooks have two non-negotiable rules:

```jsx
// ✓ Call hooks at the top level of a component
function Component() {
  const [a, setA] = useState(0);
  const [b, setB] = useState("");
  // ...
}

// ✗ Never call hooks inside conditions
function BadComponent({ showExtra }) {
  if (showExtra) {
    const [x, setX] = useState(0);  // Error: conditional hook
  }
}

// ✗ Never call hooks inside loops
function BadComponent({ items }) {
  return items.map(item => {
    const [selected, setSelected] = useState(false);  // Error: hook in loop
    return <div onClick={() => setSelected(true)}>{item.name}</div>;
  });
}

// ✗ Never call hooks inside nested functions
function BadComponent() {
  function innerFunction() {
    const [x, setX] = useState(0);  // Error: hook in nested function
  }
}
```

The reason: React tracks hooks by the **order they're called**. If the order changes between renders (due to conditions or loops), React can't match the hook calls to the right state values.

---

## 3. State Updates Are Asynchronous

`setState` doesn't update the value immediately. It **schedules** an update. React batches multiple state updates and applies them together, then re-renders once.

```jsx
function Counter() {
  const [count, setCount] = useState(0);

  function handleClick() {
    setCount(count + 1);
    setCount(count + 1);
    setCount(count + 1);
    // You might expect count to become 3 — it doesn't
    // All three calls read the same `count` value (say, 0)
    // They all schedule: set count to 0 + 1 = 1
    // Result: count becomes 1, not 3
  }

  function handleClickWrong() {
    setCount(count + 1);
    console.log(count);  // still the OLD count — update hasn't happened yet
  }

  return <button onClick={handleClick}>+</button>;
}
```

---

## 4. Functional Updates

When new state depends on the previous state, use the **functional update form**. This gives you a callback that receives the most recent state value — even if multiple updates are batched.

```jsx
function Counter() {
  const [count, setCount] = useState(0);

  function handleTripleIncrement() {
    // ✗ Wrong — all read the same stale `count`
    setCount(count + 1);  // uses count = 0 → sets to 1
    setCount(count + 1);  // uses count = 0 → sets to 1
    setCount(count + 1);  // uses count = 0 → sets to 1
    // Result: 1

    // ✓ Correct — each callback receives the latest queued value
    setCount(prev => prev + 1);  // prev = 0 → sets to 1
    setCount(prev => prev + 1);  // prev = 1 → sets to 2
    setCount(prev => prev + 1);  // prev = 2 → sets to 3
    // Result: 3
  }
}
```

**Use functional updates when:**

- New state depends on the previous state
- Multiple updates happen in the same event
- Updates happen inside `setTimeout`, `setInterval`, or async callbacks

```jsx
// Toggle — always use functional form
setIsOpen(prev => !prev);  // ✓ safe
setIsOpen(!isOpen);         // ✗ may be stale in async contexts

// Counter in a timer — functional form is essential
useEffect(() => {
  const id = setInterval(() => {
    setCount(prev => prev + 1);  // ✓ always uses current value
    // setCount(count + 1);      // ✗ stale closure — count is always 0
  }, 1000);
  return () => clearInterval(id);
}, []);
```

---

## 5. State with Objects and Arrays

React state updates must be **immutable** — you never mutate state directly. You always create a new value.

This is because React checks if state changed by reference equality. If you mutate the existing object, the reference is the same, and React won't see a change.

### Objects

```jsx
const [user, setUser] = useState({ name: "Ashan", grade: 11, score: 82 });

// ✗ Mutating state — React won't re-render
user.score = 91;
setUser(user);  // same reference → no re-render

// ✓ Create new object with spread
setUser({ ...user, score: 91 });  // new reference → re-render

// ✓ Nested objects — spread at every level
const [profile, setProfile] = useState({
  user: { name: "Ashan" },
  settings: { theme: "dark", language: "en" },
});

setProfile({
  ...profile,
  settings: {
    ...profile.settings,
    language: "si",
  },
});
```

### Arrays

```jsx
const [students, setStudents] = useState([
  { id: 1, name: "Ashan",  score: 82 },
  { id: 2, name: "Dineth", score: 91 },
]);

// ADD — spread + new item
setStudents([...students, { id: 3, name: "Kavya", score: 78 }]);

// REMOVE — filter out the item
setStudents(students.filter(s => s.id !== 2));

// UPDATE — map and replace the matching item
setStudents(students.map(s =>
  s.id === 1 ? { ...s, score: 88 } : s
));

// SORT — sort returns a new array only if you spread first
setStudents([...students].sort((a, b) => b.score - a.score));

// ✗ NEVER mutate array state methods
students.push({ id: 3, name: "Kavya" });  // mutates — won't re-render
students.sort((a, b) => a.score - b.score);  // mutates — won't re-render

// Reference: what to use instead
// Instead of push   → [...arr, newItem]
// Instead of unshift → [newItem, ...arr]
// Instead of pop    → arr.slice(0, -1)
// Instead of splice → arr.filter() or arr.map()
// Instead of sort   → [...arr].sort()
// Instead of reverse → [...arr].reverse()
```

---

## 6. Multiple State Variables

Use multiple `useState` calls for independent pieces of state. Group related state together.

```jsx
// ✓ Independent state — separate variables
function SearchPage() {
  const [query, setQuery] = useState("");
  const [page, setPage] = useState(1);
  const [sortBy, setSortBy] = useState("name");
  const [isLoading, setIsLoading] = useState(false);

  // Each can update independently without affecting others
}

// ✓ Related state — grouped in one object
function Form() {
  const [formData, setFormData] = useState({
    name: "",
    email: "",
    grade: "",
    message: "",
  });

  // Update one field at a time
  function handleChange(e) {
    setFormData(prev => ({
      ...prev,
      [e.target.name]: e.target.value,
    }));
  }
}

// When to group vs separate:
// Separate: when updates happen independently
// Group:    when multiple values always update together (like form fields)
//           or when you need to reset them all at once
```

---

## 7. Derived State — Don't Store What You Can Compute

If a value can be computed from existing state or props, don't store it in state. Compute it during render.

```jsx
// ✗ BAD — fullName is redundant state
function UserForm() {
  const [firstName, setFirstName] = useState("");
  const [lastName, setLastName] = useState("");
  const [fullName, setFullName] = useState("");  // unnecessary!

  function handleFirstNameChange(e) {
    setFirstName(e.target.value);
    setFullName(e.target.value + " " + lastName);  // tedious to keep in sync
  }
}

// ✓ GOOD — derive fullName during render
function UserForm() {
  const [firstName, setFirstName] = useState("");
  const [lastName, setLastName] = useState("");

  const fullName = firstName + " " + lastName;  // computed, not stored
  // fullName is always in sync — impossible for it not to be
}

// More examples of derivable state:
function StudentList({ students }) {
  const [query, setQuery] = useState("");
  const [sortBy, setSortBy] = useState("name");

  // ✗ Don't store filteredStudents in state
  // ✓ Compute it
  const filteredStudents = students
    .filter(s => s.name.toLowerCase().includes(query.toLowerCase()))
    .sort((a, b) => a[sortBy] > b[sortBy] ? 1 : -1);

  // selectedCount, averageScore, topStudent — all derived, not stored
  const averageScore = filteredStudents.reduce((sum, s) => sum + s.score, 0) / filteredStudents.length;
}
```

---

## 8. Initialising State Lazily

If computing the initial state is expensive, pass a **function** to `useState`. React calls it only once on the first render.

```jsx
// ✗ Expensive computation runs on EVERY render
function Component() {
  const [data, setData] = useState(parseHugeJsonFile());
  // parseHugeJsonFile() runs every time component re-renders!
}

// ✓ Pass a function — only called once
function Component() {
  const [data, setData] = useState(() => parseHugeJsonFile());
  // parseHugeJsonFile() called only on first render
}

// Common use: reading from localStorage
function Settings() {
  const [theme, setTheme] = useState(() => {
    return localStorage.getItem("theme") ?? "dark";
    // Only reads localStorage on first render
  });
}

// Common use: expensive initial calculation
function DataGrid({ rawData }) {
  const [processedData, setProcessedData] = useState(() =>
    rawData.map(processExpensively)
  );
}
```

---

## 9. Resetting State with `key`

When you need to completely reset a component's state, change its `key` prop. React treats a component with a new key as a brand new component — it unmounts the old one and mounts a fresh one.

```jsx
function EditForm({ studentId }) {
  // This form has internal state (field values, validation errors, etc.)
  // When studentId changes, we want to completely reset the form
  return <StudentForm key={studentId} studentId={studentId} />;
  // Each studentId gets a fresh StudentForm with clean state
}

// Without key — form state persists when studentId changes
function EditForm({ studentId }) {
  return <StudentForm studentId={studentId} />;
  // Fields still have previous student's values when navigating to a new student
}

// Use case: resetting a counter
const [version, setVersion] = useState(0);
<Counter key={version} />
<button onClick={() => setVersion(v => v + 1)}>Reset Counter</button>
// Clicking Reset mounts a brand-new Counter with count = 0
```

---

## 10. State Design Patterns

### Async state — loading, error, data

```jsx
function StudentProfile({ id }) {
  const [status, setStatus] = useState("idle");  // idle | loading | success | error
  const [student, setStudent] = useState(null);
  const [error, setError] = useState(null);

  useEffect(() => {
    setStatus("loading");
    fetchStudent(id)
      .then(data => {
        setStudent(data);
        setStatus("success");
      })
      .catch(err => {
        setError(err.message);
        setStatus("error");
      });
  }, [id]);

  if (status === "loading") return <Spinner />;
  if (status === "error")   return <ErrorMessage message={error} />;
  if (status === "idle")    return null;

  return <div>{student.name}</div>;
}
```

### UI state — modals, tabs, accordions

```jsx
function FAQ({ items }) {
  const [openIndex, setOpenIndex] = useState(null);

  return (
    <div>
      {items.map((item, index) => (
        <div key={item.id}>
          <button onClick={() => setOpenIndex(index === openIndex ? null : index)}>
            {item.question}
          </button>
          {openIndex === index && <p>{item.answer}</p>}
        </div>
      ))}
    </div>
  );
}

// Multi-select
function TagPicker({ tags }) {
  const [selected, setSelected] = useState(new Set());

  function toggle(tag) {
    setSelected(prev => {
      const next = new Set(prev);
      if (next.has(tag)) next.delete(tag);
      else next.add(tag);
      return next;
    });
  }

  return tags.map(tag => (
    <button
      key={tag}
      onClick={() => toggle(tag)}
      className={selected.has(tag) ? "selected" : ""}
    >
      {tag}
    </button>
  ));
}
```

---

## Summary

```
What state is:
  Data that changes over time AND causes re-renders when it does
  Different from props (read-only, from parent) and regular variables (no re-renders)

useState:
  const [value, setValue] = useState(initialValue)
  Initial value only used on first render
  Calling setValue schedules a re-render

Rules of Hooks:
  Only call at the top level — never in conditions, loops, or nested functions
  Only call inside React function components or custom hooks

State updates are async:
  setValue doesn't update immediately — it schedules
  Reading value right after setValue gives the old value

Functional updates:
  setValue(prev => prev + 1) — use when new state depends on previous
  Essential for timers, multiple batched updates, async contexts

Immutability:
  Objects: { ...prev, changed: newValue }
  Arrays: [...prev, newItem], prev.filter(), prev.map()
  Never mutate state directly — React won't see the change

Derived state:
  Don't store what you can compute
  Compute values during render from existing state/props
  Keeps state minimal and always in sync

Lazy initialisation:
  useState(() => expensiveComputation())
  Pass a function — only called once on first render

Reset with key:
  <Component key={uniqueId} /> — changing key unmounts and remounts

State location:
  Local: only one component needs it
  Lifted: multiple siblings need it → move to common parent
  Global: many unrelated components → Context or Zustand (Files 10, 18)
```

---

_Next: [04 — Rendering & the Virtual DOM](./04%20-%20Rendering%20%26%20the%20Virtual%20DOM.md)_