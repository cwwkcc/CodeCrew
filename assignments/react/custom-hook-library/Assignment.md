## Assignment Overview

Build a small library of reusable custom hooks — the pattern that lets you extract stateful logic out of a component and reuse it anywhere. This is also where `useMemo`/`useCallback` earn their place: not as a habit to apply everywhere, but as a specific fix for a specific, demonstrable performance problem.

---

## Covers

`react/12 - UseMemo & UseCallBack`, `react/13 - Custom Hooks`, `react/14 - React 19 New Features`

---

## Part 1 — Three Custom Hooks

### `useLocalStorage`

```jsx
function useLocalStorage(key, initialValue) {
  // behaves like useState, but persists to (and reads from) localStorage
  // usage: const [name, setName] = useLocalStorage("username", "");
}
```

### `useDebounce`

```jsx
function useDebounce(value, delayMs) {
  // returns a "delayed" version of `value` that only updates once `value`
  // has stopped changing for `delayMs` — the classic search-input pattern
}
```

### `useToggle`

```jsx
function useToggle(initialValue = false) {
  // returns [value, toggle] — toggle() flips the boolean each time it's called
}
```

Build all three as genuinely reusable hooks (files named `useLocalStorage.js`, `useDebounce.js`, `useToggle.js`, each starting with `use` — Module 13 covers why that naming convention isn't just a style preference, it's how React's linter rules and the Rules of Hooks actually detect hook usage).

---

## Part 2 — Use Them in a Real Component

Build a `SearchableStudentList` component that:

- Uses `useLocalStorage` to remember the last search term across page refreshes
- Uses `useDebounce` so the actual filtering only runs 300ms after the user stops typing — not on every keystroke
- Uses `useToggle` to show/hide an "advanced filters" section

```jsx
function SearchableStudentList({ students }) {
  const [searchTerm, setSearchTerm] = useLocalStorage("lastSearch", "");
  const debouncedSearch = useDebounce(searchTerm, 300);
  const [showFilters, toggleFilters] = useToggle(false);

  // filtered list computed from debouncedSearch, not searchTerm directly
}
```

---

## Part 3 — Demonstrate a Real `useMemo`/`useCallback` Fix

Add a genuinely expensive computation to the list — something artificially slow enough to visibly matter:

```jsx
function expensiveSort(students) {
  // deliberately wasteful, to make the performance difference observable
  let result = [...students];
  for (let i = 0; i < 1_000_000; i++) {
    /* busy work */
  }
  return result.sort((a, b) => a.name.localeCompare(b.name));
}
```

- **First**, call `expensiveSort` directly in the component body on every render, and add a `console.log("Sorting ran")` inside it. Type in an unrelated input elsewhere on the page (something that triggers a re-render but has nothing to do with the sort) and observe the log firing every single time, even though the actual student list hasn't changed.
- **Then**, wrap it in `useMemo` with the correct dependency array, and confirm the log now only fires when the students array (or search term) actually changes — not on unrelated re-renders.

```jsx
const sortedStudents = useMemo(
  () => expensiveSort(filteredStudents),
  [filteredStudents],
);
```

---

## Constraints

- Every custom hook must start with `use` and be a genuinely separate, reusable function — not just a chunk of logic copy-pasted between components.
- `useMemo` should be justified by an actual, demonstrated performance difference (Part 3's console log test) — not applied as a reflexive habit. Blindly wrapping cheap computations in `useMemo` adds complexity for no measurable benefit, and this assignment is specifically testing that you can tell the difference.
- `useDebounce`'s internal implementation needs its own `useEffect` with proper cleanup (a `setTimeout` that gets cleared if `value` changes again before the delay finishes) — this connects directly back to the previous assignment's cleanup-function requirement.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — `useLocalStorage`'s initial value needs to check `localStorage` first (falling back to `initialValue` if nothing's stored yet), and its setter needs to write to `localStorage` every time it's called, in addition to updating React state.

> 💡 **Hint 2** — `useDebounce` structure: hold the debounced value in its own `useState`, and inside a `useEffect` that depends on `value`, set a `setTimeout` to update it after `delayMs` — with a cleanup function that clears that timeout if `value` changes again first (this is what actually creates the "debounce" — a change that happens _before_ the delay finishes cancels the pending update).

> 💡 **Hint 3** — For Part 3's demonstration to actually be visible, make sure the "unrelated input" genuinely re-renders the _same_ component tree that contains `expensiveSort` — a completely separate component won't demonstrate the problem, since React only re-renders what's actually affected.

---

## Submission Guidelines

1. Three hook files: `useLocalStorage.js`, `useDebounce.js`, `useToggle.js`
2. `SearchableStudentList.jsx`
3. A short note describing what you actually observed in the console before and after adding `useMemo` in Part 3 — this is the real evidence that the optimization did something

---

## Bonus Challenge 🏆

- 🪝 Build a fourth hook, `useMediaQuery(query)`, wrapping the browser's `matchMedia` API, returning a boolean that updates live as the viewport crosses the breakpoint.
- ⚡ Read about React 19's compiler-based automatic memoization (Module 14) and write a short paragraph on what it changes about when you'd still reach for `useMemo` manually versus when the compiler handles it for you.
- 🔁 Wrap an event handler passed to a memoized child component in `useCallback`, and demonstrate (with a `console.log` in the child, wrapped in `React.memo`) that the child stops needlessly re-rendering.

---

## Learning Objectives

- ✓ Extract reusable stateful logic into genuine custom hooks.
- ✓ Understand debouncing as a real-world `useEffect` + cleanup pattern, not just a library you import.
- ✓ Use `useMemo` because you've _measured_ a real performance problem, not by default.

**Good Luck! 🚀**
