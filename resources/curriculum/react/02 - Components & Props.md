> Components are the building blocks of React. Props are how they communicate. Getting these right — how to design a component's API, when to split components, how to think about prop flow — determines whether your codebase stays maintainable as it grows.

---

## Table of Contents

1. [Component Anatomy](#1-component-anatomy)
2. [Props In Depth](#2-props-in-depth)
3. [Default Props](#3-default-props)
4. [Prop Types and Validation](#4-prop-types-and-validation)
5. [Component Composition Patterns](#5-component-composition-patterns)
6. [When to Split a Component](#6-when-to-split-a-component)
7. [Lifting State Up](#7-lifting-state-up)
8. [Pure Components](#8-pure-components)
9. [Component Design Guidelines](#9-component-design-guidelines)

---

## 1. Component Anatomy

A React component is a JavaScript function that:

- Has a name starting with a **capital letter**
- Accepts a single **props object** as its argument
- Returns **JSX** (or null)

```jsx
// Minimal component
function Greeting() {
  return <h1>Hello</h1>;
}

// Component with props
function Greeting({ name, isLoud = false }) {
  const text = isLoud ? name.toUpperCase() : name;
  return <h1>Hello, {text}!</h1>;
}

// Arrow function component (identical behaviour)
const Greeting = ({ name, isLoud = false }) => {
  const text = isLoud ? name.toUpperCase() : name;
  return <h1>Hello, {text}!</h1>;
};

// Usage
<Greeting name="Ashan" />
<Greeting name="Dineth" isLoud />       // isLoud={true}
<Greeting name="Kavya" isLoud={false} />
```

---

## 2. Props In Depth

Props (short for properties) are the inputs to a component. They flow **down** — from parent to child, never the other way.

```jsx
// Parent defines what to pass
function App() {
  const student = {
    id: "s123",
    name: "Ashan Perera",
    grade: 11,
    score: 91,
    avatarUrl: "/avatars/ashan.jpg",
  };

  return <StudentCard student={student} onEdit={handleEdit} />;
}

// Child receives via props
function StudentCard({ student, onEdit }) {
  return (
    <div className="card">
      <img src={student.avatarUrl} alt={student.name} />
      <h2>{student.name}</h2>
      <p>
        Grade {student.grade} — Score: {student.score}
      </p>
      <button onClick={() => onEdit(student.id)}>Edit</button>
    </div>
  );
}
```

### Props are read-only

A component must never modify its own props. Props are immutable from the component's perspective.

```jsx
function BadComponent({ count }) {
  count = count + 1; // ✗ Never mutate props
  return <p>{count}</p>;
}

function GoodComponent({ count }) {
  const displayCount = count + 1; // ✓ Create new value
  return <p>{displayCount}</p>;
}
```

### Any JavaScript value can be a prop

```jsx
<Component
  // Primitives
  name="Ashan"
  age={17}
  isActive={true}
  score={null}

  // Objects and arrays
  student={{ name: "Ashan", grade: 11 }}
  scores={[82, 91, 78]}

  // Functions (callbacks)
  onClick={handleClick}
  onSubmit={(data) => submitForm(data)}
  formatValue={(n) => n.toFixed(2)}

  // JSX (React elements)
  icon={<StarIcon />}
  header={<h1>Title</h1>}

  // Components (render props pattern)
  renderItem={(item) => <Item data={item} />}
/>
```

---

## 3. Default Props

Set default values using JavaScript default parameter syntax — the modern, preferred approach.

```jsx
// Default values in destructuring
function Button({
  children,
  variant = "primary",
  size = "md",
  disabled = false,
  onClick,
}) {
  return (
    <button
      className={`btn btn-${variant} btn-${size}`}
      disabled={disabled}
      onClick={onClick}
    >
      {children}
    </button>
  );
}

// Usage — any omitted prop gets its default
<Button onClick={handleSave}>Save</Button>
// variant="primary", size="md", disabled=false

<Button variant="danger" size="lg" onClick={handleDelete}>
  Delete
</Button>

// Nested defaults
function Avatar({
  src,
  alt = "User avatar",
  size = 40,
  shape = "circle",
}) {
  return (
    <img
      src={src ?? "/default-avatar.png"}
      alt={alt}
      width={size}
      height={size}
      className={`avatar avatar-${shape}`}
    />
  );
}
```

---

## 4. Prop Types and Validation

Without TypeScript, use the `prop-types` library for runtime validation. With TypeScript (Project Nexus), define types directly. Full coverage in File 21 — here's the pattern:

```tsx
// With TypeScript (what you'll use in Project Nexus)
interface StudentCardProps {
  student: {
    id: string;
    name: string;
    grade: number;
    score: number;
    avatarUrl?: string; // optional
  };
  onEdit: (id: string) => void;
  isHighlighted?: boolean;
}

function StudentCard({
  student,
  onEdit,
  isHighlighted = false,
}: StudentCardProps) {
  return (
    <div className={isHighlighted ? "card card-highlighted" : "card"}>
      <h2>{student.name}</h2>
      <button onClick={() => onEdit(student.id)}>Edit</button>
    </div>
  );
}
```

---

## 5. Component Composition Patterns

### Containment — components that wrap other components

```jsx
// A generic panel that accepts any children
function Panel({ title, children, footer }) {
  return (
    <div className="panel">
      <div className="panel-header">
        <h2>{title}</h2>
      </div>
      <div className="panel-body">{children}</div>
      {footer && <div className="panel-footer">{footer}</div>}
    </div>
  );
}

// Usage — Panel doesn't need to know what it contains
<Panel
  title="Student Results"
  footer={<Button onClick={exportResults}>Export</Button>}
>
  <ResultsTable students={students} />
  <Pagination page={page} total={total} />
</Panel>;
```

### Specialisation — specific version of a general component

```jsx
// General
function Dialog({ title, children, onClose, variant = "default" }) {
  return (
    <div className={`dialog dialog-${variant}`}>
      <button onClick={onClose}>×</button>
      <h2>{title}</h2>
      {children}
    </div>
  );
}

// Specialised
function ConfirmDialog({ message, onConfirm, onCancel }) {
  return (
    <Dialog title="Confirm" onClose={onCancel} variant="warning">
      <p>{message}</p>
      <Button onClick={onCancel} variant="ghost">
        Cancel
      </Button>
      <Button onClick={onConfirm} variant="danger">
        Confirm
      </Button>
    </Dialog>
  );
}

function AlertDialog({ message, onClose }) {
  return (
    <Dialog title="Alert" onClose={onClose}>
      <p>{message}</p>
      <Button onClick={onClose}>OK</Button>
    </Dialog>
  );
}
```

### Slot pattern — named content areas via props

```jsx
// Instead of a single children prop, accept multiple named sections
function PageLayout({ header, sidebar, main, footer }) {
  return (
    <div className="layout">
      <header className="layout-header">{header}</header>
      <div className="layout-body">
        <aside className="layout-sidebar">{sidebar}</aside>
        <main className="layout-main">{main}</main>
      </div>
      <footer className="layout-footer">{footer}</footer>
    </div>
  );
}

<PageLayout
  header={<Navbar />}
  sidebar={<FilterPanel filters={filters} onChange={setFilters} />}
  main={<StudentGrid students={students} />}
  footer={<Pagination page={page} total={total} />}
/>;
```

---

## 6. When to Split a Component

There's no perfect rule, but these signals tell you it's time to extract a component:

**Split when:**

- A piece of the JSX is visually distinct and reused elsewhere
- A section has its own logic that's unrelated to the rest
- The component file is getting long and hard to scan
- You find yourself scrolling to understand one part of a component

**Don't split when:**

- It's used in exactly one place and the logic is simple
- Splitting would require passing many props through an intermediate layer
- It would make the code harder to follow by adding indirection

```jsx
// BEFORE — one big component
function StudentPage({ student }) {
  return (
    <div>
      {/* Header — reused in many places */}
      <div className="page-header">
        <img src={student.avatarUrl} className="avatar" />
        <div>
          <h1>{student.name}</h1>
          <span className="badge">{student.role}</span>
        </div>
      </div>

      {/* Stats — self-contained section */}
      <div className="stats-grid">
        <div className="stat">
          <p className="stat-label">Average Score</p>
          <p className="stat-value">{student.averageScore}%</p>
        </div>
        <div className="stat">
          <p className="stat-label">Attendance</p>
          <p className="stat-value">{student.attendance}%</p>
        </div>
        <div className="stat">
          <p className="stat-label">Grade</p>
          <p className="stat-value">{student.grade}</p>
        </div>
      </div>
    </div>
  );
}

// AFTER — extracted components
function StudentAvatar({ src, name, role }) {
  return (
    <div className="page-header">
      <img src={src} className="avatar" alt={name} />
      <div>
        <h1>{name}</h1>
        <span className="badge">{role}</span>
      </div>
    </div>
  );
}

function StatCard({ label, value }) {
  return (
    <div className="stat">
      <p className="stat-label">{label}</p>
      <p className="stat-value">{value}</p>
    </div>
  );
}

function StudentPage({ student }) {
  return (
    <div>
      <StudentAvatar
        src={student.avatarUrl}
        name={student.name}
        role={student.role}
      />
      <div className="stats-grid">
        <StatCard label="Average Score" value={`${student.averageScore}%`} />
        <StatCard label="Attendance" value={`${student.attendance}%`} />
        <StatCard label="Grade" value={student.grade} />
      </div>
    </div>
  );
}
```

---

## 7. Lifting State Up

When two components need to share the same state, move the state to their closest common ancestor and pass it down as props.

```jsx
// PROBLEM: two sibling components that need shared state
function FilterBar() {
  const [query, setQuery] = useState(""); // local state
  return <input value={query} onChange={(e) => setQuery(e.target.value)} />;
}

function StudentList() {
  // Can't access FilterBar's query — it's trapped in FilterBar
  return <ul>...</ul>;
}

function StudentsPage() {
  return (
    <>
      <FilterBar />
      <StudentList /> {/* can't filter because it doesn't have query */}
    </>
  );
}

// SOLUTION: lift state to the common parent
function StudentsPage() {
  const [query, setQuery] = useState(""); // state lives here

  const filtered = students.filter((s) =>
    s.name.toLowerCase().includes(query.toLowerCase()),
  );

  return (
    <>
      <FilterBar query={query} onQueryChange={setQuery} />
      <StudentList students={filtered} />
    </>
  );
}

// Both children receive what they need via props
function FilterBar({ query, onQueryChange }) {
  return (
    <input
      value={query}
      onChange={(e) => onQueryChange(e.target.value)}
      placeholder="Search students..."
    />
  );
}

function StudentList({ students }) {
  return (
    <ul>
      {students.map((s) => (
        <li key={s.id}>{s.name}</li>
      ))}
    </ul>
  );
}
```

---

## 8. Pure Components

A React component should be **pure** with respect to its props and state:

- Same inputs → same output
- No side effects during rendering
- Doesn't modify external state

```jsx
// ✓ Pure — same props always produce same JSX
function PriceDisplay({ amount, currency }) {
  const formatted = new Intl.NumberFormat("en-LK", {
    style: "currency",
    currency,
  }).format(amount);

  return <span className="price">{formatted}</span>;
}

// ✗ Impure — reads external mutable state during render
let taxRate = 0.08;
function PriceDisplay({ amount }) {
  const withTax = amount * (1 + taxRate); // depends on mutable external var
  return <span>{withTax}</span>;
}

// ✗ Impure — causes side effects during render
function StudentCard({ student }) {
  console.log("Rendered:", student.name); // side effect in render
  analytics.track("student_viewed", student.id); // side effect in render
  return <div>{student.name}</div>;
}

// ✓ Side effects belong in useEffect (covered in File 08)
function StudentCard({ student }) {
  useEffect(() => {
    analytics.track("student_viewed", student.id); // ✓ in effect
  }, [student.id]);

  return <div>{student.name}</div>;
}
```

---

## 9. Component Design Guidelines

### Design props like a public API

```jsx
// BAD — caller must know internal implementation details
<Button cssClass="btn-primary btn-lg" htmlType="submit">Save</Button>

// GOOD — caller describes intent, component handles details
<Button variant="primary" size="lg" type="submit">Save</Button>
```

### Accept `className` and `style` for extensibility

```jsx
// Let callers customise layout without forking the component
function Card({ children, className, style, ...props }) {
  return (
    <div className={`card ${className ?? ""}`} style={style} {...props}>
      {children}
    </div>
  );
}

// Callers can add margin, width, etc. without changing Card itself
<Card className="mt-8 max-w-md">...</Card>;
```

### Prefer specific props over passing an entire object

```jsx
// OK — passes the whole object
<StudentCard student={student} />

// Better when only some fields are needed — clearer dependencies
<StudentCard
  name={student.name}
  grade={student.grade}
  avatarUrl={student.avatarUrl}
/>
// Now it's obvious exactly what StudentCard uses
```

### Name event handlers `onX` in props, `handleX` in implementations

```jsx
// Props: onX (describes the event)
<SearchBar onSearch={handleSearch} onClear={handleClear} />;

// Implementation: handleX (describes the handler)
function SearchBar({ onSearch, onClear }) {
  function handleInputChange(e) {
    onSearch(e.target.value);
  }

  return <input onChange={handleInputChange} />;
}
```

---

## Summary

```
Component rules:
  Capital letter name
  Accept one props object as argument
  Return JSX (or null)
  Must be pure — no side effects during render, same props → same output

Props:
  Any JS value: primitives, objects, arrays, functions, JSX
  Read-only — never mutate props
  Flow down only — parent to child

Default props:
  Use JS default parameter syntax: function Comp({ prop = "default" }) {}

Composition patterns:
  Containment:     accept children (generic wrappers)
  Specialisation:  specific version of general component
  Slot pattern:    named content via multiple props (header, sidebar, main)

When to split:
  Visually distinct and reusable
  Has its own self-contained logic
  Component is getting long/hard to scan

Lifting state up:
  When siblings need shared state → move it to their common parent
  Parent holds state, passes it down to children as props + callbacks

Design guidelines:
  Props as public API — describe intent, not implementation
  Accept className for extensibility
  Name callbacks onX in props, handleX in implementation
  Prefer specific props over passing entire objects
```

---

_Next: [03 — State & useState](./03%20-%20State%20%26%20useState.md)_
