## Assignment Overview

A capstone-style assignment pulling together global state (outside React's own component tree), animation, and a real performance-optimization exercise — plus handling a slow or failing component gracefully with Suspense and an Error Boundary. Build a small "Favorites" feature for the student directory from earlier assignments.

---

## Covers

`react/18 - Global State (Zustand)`, `react/19 - Animations (Framer Motion)`, `react/20 - Performance Optimization`, `react/25 - Suspense & Error Boundaries`

> **Modules 21, 22, 24, and 26** (React + TypeScript, Design Patterns in React, Shadcn/Radix, `useImperativeHandle`/`forwardRef`) aren't covered by a dedicated assignment — they're best absorbed by reading, and by noticing them show up naturally in the assignments above (e.g. `forwardRef` if you ever needed to expose a child's DOM node to a parent in the hooks assignment). If you want deliberate practice with any of them specifically, ask your instructor for a focused follow-up exercise.

---

## Part 1 — Global State with Zustand

```jsx
import { create } from "zustand";

const useFavoritesStore = create((set) => ({
  favoriteIds: [],
  toggleFavorite: (studentId) =>
    set((state) => ({
      favoriteIds: state.favoriteIds.includes(studentId)
        ? state.favoriteIds.filter((id) => id !== studentId)
        : [...state.favoriteIds, studentId],
    })),
}));
```

- Build a `useFavoritesStore` holding an array of favorited student IDs, with a `toggleFavorite` action
- Add a ★/☆ button to each student card (reuse the `StudentCard` component from Assignment 1 if you have it) that toggles that student's favorite status
- Add a separate `FavoritesPanel` component, rendered _elsewhere_ in the component tree (not a parent/child of the student list), that shows the count and names of currently-favorited students — proving the state is genuinely global and not passed down through props

---

## Part 2 — Animate the Favoriting Action

Using Framer Motion:

- When a student is added to favorites, animate their card with a brief scale/highlight effect
- Animate the `FavoritesPanel`'s list so items smoothly appear/disappear as they're added/removed, rather than snapping instantly
- Respect `prefers-reduced-motion` (carried over from the CSS curriculum's Animations module) — Framer Motion has a built-in way to detect and respect this system preference; look up `useReducedMotion` in its docs

---

## Part 3 — A Real Performance Measurement

- Render a **large** list — at least 200 students (generate fake data if needed) — each as a `StudentCard`
- Wrap `StudentCard` in `React.memo`, and add a `console.log` inside it
- Toggle one student's favorite status, and check the console: **only that one card should log a re-render**, not all 200
- If all 200 log on every toggle, something's wrong — find and fix it (a common cause: passing a brand-new inline function or object as a prop on every parent render, which defeats `React.memo`'s comparison — this is exactly where `useCallback`, from the previous assignment, becomes genuinely necessary rather than optional)

---

## Part 4 — Suspense & an Error Boundary

- Wrap the student list section in a `<Suspense fallback={<p>Loading students...</p>}>` boundary around whatever data-fetching component supplies the list (reuse the TanStack Query setup from the previous assignment if convenient — TanStack Query has a Suspense-compatible mode)
- Wrap the same section in an Error Boundary component, so a failed fetch (or a deliberately-thrown error, for testing) shows a friendly fallback UI instead of a blank white screen or an unhandled error crashing the whole app
- Test this by deliberately throwing an error inside `StudentCard` (e.g. `if (student.id === 3) throw new Error("Test error");`) and confirming the Error Boundary catches it — then remove the test throw once confirmed

---

## Constraints

- Zustand state must be the single source of truth for favorites — no duplicate `useState` also tracking favorite status somewhere else that could drift out of sync.
- `React.memo` alone isn't enough if you're still passing new function/object references as props every render — Part 3 specifically requires demonstrating this interaction, not just wrapping components in `memo` and assuming it worked.
- The Error Boundary must be a real class component (or a library-provided one) — Suspense and error boundaries are the one place in modern React where you still reach for a class, since there's no hook equivalent for catching render errors.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — If `React.memo` isn't preventing re-renders, check the function you're passing as `onToggleFavorite` — if it's defined inline in the parent (`onClick={() => toggleFavorite(id)}`), a new function is created every render, which `React.memo`'s shallow prop comparison sees as "different props," triggering a re-render anyway. Wrap it in `useCallback` at the point where it's defined.

> 💡 **Hint 2** — A minimal Error Boundary class:
>
> ```jsx
> class ErrorBoundary extends React.Component {
>   state = { hasError: false };
>   static getDerivedStateFromError() {
>     return { hasError: true };
>   }
>   render() {
>     if (this.state.hasError) return <p>Something went wrong.</p>;
>     return this.props.children;
>   }
> }
> ```

> 💡 **Hint 3** — `useReducedMotion` from Framer Motion returns `true`/`false` based on the user's OS setting — wrap your animation config in a check against it, falling back to an instant (non-animated) state change when it's `true`.

---

## Submission Guidelines

1. All component/store files
2. A short note on what you found in Part 3 before and after the `useCallback` fix — the actual console output difference is the evidence this worked

---

## Bonus Challenge 🏆

- 🌗 Persist the Zustand favorites store to `localStorage` using Zustand's `persist` middleware.
- 🎬 Add a Framer Motion `layout` animation so the list smoothly reflows when a favorited item moves to the top.
- 🧪 Deliberately break `React.memo`'s effectiveness a second, different way (passing a new object literal as a prop: `style={{ color: "red" }}` created fresh every render) and confirm you can now explain, from first principles, why object/array/function literals defined inline in JSX are a common hidden performance cost.

---

## Learning Objectives

- ✓ Use global state (Zustand) for state that doesn't belong to a single component tree.
- ✓ Animate real UI changes with Framer Motion, respecting reduced-motion preferences.
- ✓ Measure — not assume — a `React.memo` optimization's effectiveness, and recognize what actually defeats it.
- ✓ Handle async loading and render errors gracefully with Suspense and an Error Boundary.

**Good Luck! 🚀**
