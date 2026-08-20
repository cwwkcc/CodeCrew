> React's rendering model is what makes it fast and predictable. Understanding when React renders, what it renders, and how it decides what to update in the DOM explains most performance issues and a lot of seemingly strange behaviour.

---

## Table of Contents

1. [What "Rendering" Means in React](#1-what-rendering-means-in-react)
2. [The Render Cycle](#2-the-render-cycle)
3. [The Virtual DOM](#3-the-virtual-dom)
4. [Reconciliation and Diffing](#4-reconciliation-and-diffing)
5. [When React Re-renders](#5-when-react-re-renders)
6. [The key Prop and Reconciliation](#6-the-key-prop-and-reconciliation)
7. [Batching](#7-batching)
8. [StrictMode and Double Rendering](#8-strictmode-and-double-rendering)
9. [Commit Phase — When the DOM Actually Changes](#9-commit-phase--when-the-dom-actually-changes)

---

## 1. What "Rendering" Means in React

"Rendering" in React has a specific meaning that's different from what it might sound like:

**Rendering** = React calling your component function to get JSX.

**Painting** = the browser actually drawing pixels to the screen.

React rendering does NOT immediately change the DOM. It creates a description of what the UI should look like. React then decides if and how to update the real DOM — that happens separately.

```
State change triggers:
  1. Render phase    → React calls component functions, produces JSX
  2. Reconciliation  → React compares new JSX with previous
  3. Commit phase    → React makes minimal DOM changes
  4. Browser paint   → Browser draws the pixels
```

---

## 2. The Render Cycle

When a state update occurs, React goes through this process:

```jsx
function Counter() {
  const [count, setCount] = useState(0);

  // React calls this function on every render
  // During render, this code runs top to bottom
  const doubled = count * 2; // computed during render

  return (
    // return value is the "render output"
    <div>
      <p>Count: {count}</p>
      <p>Doubled: {doubled}</p>
      <button onClick={() => setCount((c) => c + 1)}>+</button>
    </div>
  );
}
```

Step by step on first render:

1. React calls `Counter()`
2. `useState(0)` returns `[0, setCount]`
3. `doubled` = 0
4. Returns JSX describing a div with two paragraphs and a button
5. React creates real DOM nodes and adds to the page

On click (second render):

1. `setCount(c => c + 1)` schedules state update (count will be 1)
2. React calls `Counter()` again
3. `useState(0)` returns `[1, setCount]` — React remembers 1
4. `doubled` = 2
5. Returns new JSX with updated values
6. React diffs: what changed from the previous output?
7. Updates only the text content of the two paragraphs

---

## 3. The Virtual DOM

The **virtual DOM** is a lightweight JavaScript representation of the DOM. React maintains two copies:

- The current virtual DOM (what's on screen)
- The new virtual DOM (result of the latest render)

```javascript
// What React creates from JSX — a plain JavaScript object tree
{
  type: "div",
  props: {
    children: [
      {
        type: "p",
        props: { children: "Count: 1" }
      },
      {
        type: "p",
        props: { children: "Doubled: 2" }
      },
      {
        type: "button",
        props: {
          onClick: [function],
          children: "+"
        }
      }
    ]
  }
}
```

This is much cheaper to create and compare than real DOM nodes. DOM nodes have hundreds of properties. React elements have a handful.

---

## 4. Reconciliation and Diffing

**Reconciliation** is how React figures out what changed between two renders. React's diffing algorithm compares the new virtual DOM with the previous one.

### Rule 1: Different type = destroy and rebuild

If the element type changes, React destroys the old subtree and builds a new one from scratch — including all child component state.

```jsx
// If condition switches from true to false:
{
  isAdmin ? (
    <AdminDashboard /> // type: AdminDashboard
  ) : (
    <UserDashboard />
  ); // type: UserDashboard
}

// React sees different types → destroys AdminDashboard (all its state lost)
// → mounts a fresh UserDashboard
```

### Rule 2: Same type = update in place

If the type is the same, React updates the existing DOM node/component with new props.

```jsx
// Before: <p className="text" style={{ color: "red" }}>Hello</p>
// After:  <p className="text" style={{ color: "blue" }}>Hello</p>
// React updates: only the color style changes — everything else stays
```

### Rule 3: Lists use keys

Without keys, React matches list items by position. With keys, React matches by key value.

```jsx
// Without keys — matching by position
// Before: [A, B, C]
// After:  [X, A, B, C]  (X inserted at start)
// React thinks: position 0 changed A→X, position 1 changed B→A, etc.
// Updates all four items — expensive

// With keys — matching by key value
// Before: [{key:"a", A}, {key:"b", B}, {key:"c", C}]
// After:  [{key:"x", X}, {key:"a", A}, {key:"b", B}, {key:"c", C}]
// React: "a", "b", "c" already exist — just move them. "x" is new — insert it.
// Updates: only one new insert — efficient
```

---

## 5. When React Re-renders

A component re-renders when:

1. **Its own state changes** (`useState` setter called)
2. **Its parent re-renders** (even if props haven't changed)
3. **A context it consumes changes** (File 10)

```jsx
function Parent() {
  const [count, setCount] = useState(0);

  return (
    <div>
      <button onClick={() => setCount((c) => c + 1)}>+</button>
      <Child /> {/* re-renders every time Parent re-renders */}
      <ExpensiveList /> {/* also re-renders — even though no props passed! */}
    </div>
  );
}

function Child() {
  console.log("Child rendered"); // logs on every Parent re-render
  return <p>I am a child</p>;
}
```

**React re-renders children by default.** This is usually fine — React is fast. Premature optimisation with `memo`everywhere is a real problem in React codebases. Optimise when you measure a problem. (Covered in File 20.)

### What does NOT cause a re-render

```jsx
function Component() {
  const [count, setCount] = useState(0);

  // Calling setCount with the SAME value → no re-render
  // React compares with Object.is() — if same, skips the re-render
  setCount(0); // count is already 0 → React skips

  // Regular variables changing → no re-render
  let localVar = 0;
  localVar = 5; // React doesn't watch this

  // Refs changing → no re-render (File 09)
  const ref = useRef(0);
  ref.current = 5; // silently updates, no re-render
}
```

---

## 6. The `key` Prop and Reconciliation

The `key` prop has two uses — both are about controlling reconciliation.

### Use 1: Lists (already covered in File 01)

```jsx
students.map((s) => <StudentCard key={s.id} student={s} />);
```

### Use 2: Force reset — tell React an element is "different"

```jsx
// When userId changes, we want to reset the form completely
// Changing the key tells React: this is a brand new component
function UserEditor({ userId }) {
  return <UserForm key={userId} userId={userId} />;
}

// UserForm's internal state (field values, validation) resets whenever
// userId changes, because a new key = new component instance
```

---

## 7. Batching

React **batches** multiple state updates that happen in the same event handler, running only one re-render.

```jsx
function Component() {
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [role, setRole] = useState("");

  function handleReset() {
    setName(""); // } React batches these
    setEmail(""); // } three updates
    setRole(""); // } into ONE re-render
  }

  // Only ONE re-render, not three
}
```

**React 18 automatic batching** extended this to async operations too:

```jsx
// Before React 18 — each caused a separate re-render
setTimeout(() => {
  setCount((c) => c + 1); // re-render 1
  setFlag((f) => !f); // re-render 2
}, 1000);

// React 18+ — batched even in setTimeout, fetch, promises
setTimeout(() => {
  setCount((c) => c + 1); // }
  setFlag((f) => !f); // } ONE re-render
}, 1000);
```

If you ever need to force synchronous state application (rare), use `flushSync`:

```jsx
import { flushSync } from "react-dom";

flushSync(() => {
  setCount((c) => c + 1);
});
// DOM is updated synchronously here
// Only use when you need to read updated DOM layout immediately
```

---

## 8. StrictMode and Double Rendering

In development, `React.StrictMode` calls component functions **twice** on every render. This is intentional.

```jsx
function Component() {
  console.log("rendering"); // logs TWICE in development with StrictMode

  return <div />;
}
```

**Why?** To help you find **impure renders** — renders that produce side effects. If your component is pure (same input → same output, no side effects), running it twice produces the same result. If it's not pure, the double render will expose the bug.

```jsx
// StrictMode will catch this impurity:
let rendered = 0;
function ImpureComponent() {
  rendered++; // side effect during render
  return <p>Rendered {rendered} times</p>;

  // Without StrictMode: renders "Rendered 1 times", "Rendered 2 times"...
  // With StrictMode: renders "Rendered 2 times", "Rendered 4 times"...
  // The doubling exposes that this component is impure
}
```

StrictMode double-rendering **only happens in development**. Production renders once. Don't disable StrictMode to "fix" double renders — fix the impurity.

---

## 9. Commit Phase — When the DOM Actually Changes

After reconciliation, React enters the **commit phase**: making the actual DOM changes.

```
Render phase (can be interrupted, may run multiple times):
  → React calls component functions
  → Produces React elements (virtual DOM)
  → Runs reconciliation (diffing)

Commit phase (synchronous, cannot be interrupted):
  → Applies DOM changes
  → Runs useLayoutEffect cleanup + setup (File 08)
  → Browser paints the screen
  → Runs useEffect cleanup + setup (File 08)
```

The render phase can be paused, aborted, and restarted (this is how React Concurrent features work — React 18). The commit phase always runs to completion.

**Practical implication:** Don't do heavy work in the render phase. Component functions should be fast and pure. Side effects (data fetching, subscriptions, DOM manipulation) belong in `useEffect` — which runs after commit, not during render.

---

## Summary

```
Rendering = calling your component function to get JSX.
  Not the same as painting pixels — that's the browser's job later.

The render cycle:
  State update → React calls component → produces JSX →
  reconciliation → minimal DOM updates → browser paints

Virtual DOM:
  Lightweight JS object representing the UI
  React maintains current + new copies and diffs them
  Cheaper than working with real DOM nodes directly

Reconciliation rules:
  Different type → destroy old tree, build new one (state is lost)
  Same type      → update in place with new props
  Lists          → use key for efficient matching by identity, not position

When React re-renders:
  1. State changes (useState setter)
  2. Parent re-renders (even if props unchanged)
  3. Consumed context changes
  Calling setState with same value → skipped
  Changing regular variables / refs → not a re-render

Batching:
  Multiple setState in same event → one re-render
  React 18: batches in async contexts too (setTimeout, promises)

StrictMode double rendering:
  Development only — intentional
  Exposes impure renders
  Never disable to fix — fix the impurity instead

Commit phase:
  Synchronous, cannot be interrupted
  DOM changes happen here
  useLayoutEffect → paint → useEffect
```

---

_Next: [05 — Conditional Rendering & Lists](./05%20-%20Conditional%20Rendering%20%26%20Lists.md)_
