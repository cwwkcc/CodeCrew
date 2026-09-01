## Assignment Overview

Build a small dashboard combining four hooks that are each individually confusing the first time, and genuinely tricky to combine correctly: `useEffect` for a running timer, `useRef` for a focus trick, `useContext` for app-wide theming, and `useReducer` for a small quiz score tracker. Four separate mini-features, one page.

---

## Covers

`react/08 - UseEffect & Side Effects`, `react/09 - UseRef`, `react/10 - UseContext`, `react/11 - UseReducer`

---

## Part 1 — A Live Timer (`useEffect`)

```jsx
function ElapsedTimer() {
  // shows "Elapsed: 0s", "Elapsed: 1s", "Elapsed: 2s"... counting up every second
}
```

- Use `setInterval` inside a `useEffect` to increment a `seconds` state value once per second
- **The interval must be cleared when the component unmounts** — this is a cleanup function, and it's the single most commonly-skipped part of `useEffect` (Module 08's biggest real-world source of bugs: intervals/listeners that keep running after a component is gone)
- Add a button elsewhere on the page that conditionally renders/unmounts `<ElapsedTimer />`, and confirm (via a `console.log` in your cleanup function) that the interval actually stops when it unmounts — an interval that keeps silently running (and logging, or worse, updating state) on an unmounted component is exactly the bug this exercise is designed to catch

---

## Part 2 — Auto-Focus on Mount (`useRef`)

```jsx
function QuickSearchBox() {
  // a text input that is automatically focused the moment the component appears
}
```

- Use `useRef` to get a reference to the actual `<input>` DOM node
- In a `useEffect` that runs once on mount, call `.focus()` on that ref
- This is the classic, correct use case for `useRef`: reaching into the real DOM for something React's declarative model doesn't handle on its own (focus is an imperative action, not a piece of state)

---

## Part 3 — App-Wide Theme (`useContext`)

```jsx
const ThemeContext = createContext();

function ThemeProvider({ children }) {
  // holds the current theme ("light" / "dark") and a toggle function
}
```

- Build a `ThemeContext` with a provider holding the current theme and a function to toggle it
- Consume it in **at least two different, separately-nested components** (not just at the top level) to prove it's actually solving the "prop drilling" problem — e.g. a header component and a footer component, several levels apart, both reading and correctly reflecting the current theme without either one receiving it as a prop
- Toggling the theme anywhere should update it everywhere it's consumed

---

## Part 4 — A Quiz Score Tracker (`useReducer`)

```jsx
function quizReducer(state, action) {
  // handles: { type: "CORRECT" }, { type: "INCORRECT" }, { type: "RESET" }
}

function QuizScoreTracker() {
  // uses useReducer with the reducer above
}
```

- State shape: `{ correct: number, incorrect: number }`
- Three buttons: "Mark Correct", "Mark Incorrect", "Reset" — each dispatches the corresponding action
- Display: `"Score: 3 correct, 1 incorrect (75%)"` — the percentage should handle the zero-answers-yet case without producing `NaN` or a crash

---

## Constraints

- **`useReducer`, not multiple `useState` calls, for the quiz tracker.** The whole point of this part is practicing reducer logic (a pure function computing new state from old state + an action) for state that has more than one related piece of data changing together.
- **Every `useEffect` with a subscription/timer/listener must have a cleanup function.** This is checked directly in Part 1's unmount test.
- **The theme must genuinely flow through Context, not props threaded down manually** — if you find yourself passing `theme` as a prop through a component that doesn't itself use it, just to hand it to a grandchild, that's exactly the problem Context solves, and you haven't actually used it yet.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — `useEffect` cleanup shape: `useEffect(() => { const id = setInterval(...); return () => clearInterval(id); }, []);` — the function _returned_ from the effect is the cleanup, called automatically when the component unmounts (or before the effect re-runs, if its dependencies change).

> 💡 **Hint 2** — `useRef` for focus: `const inputRef = useRef(null); <input ref={inputRef} />`, then `inputRef.current.focus()` inside a `useEffect(() => { ... }, [])` that runs once on mount.

> 💡 **Hint 3** — For the percentage calculation avoiding `NaN`: `const total = correct + incorrect; const percentage = total === 0 ? 0 : Math.round((correct / total) * 100);`

---

## Submission Guidelines

1. `App.jsx` plus separate component files for each part
2. A short note describing what you actually observed in the console when testing the timer's cleanup (Part 1) — confirm the interval really stopped

---

## Bonus Challenge 🏆

- ⏸️ Add a Pause/Resume button to the timer, without breaking the cleanup behavior when it unmounts mid-pause.
- 🎨 Persist the theme choice with `localStorage` so it survives a page refresh (this connects back to the JavaScript curriculum's Browser APIs module).
- 🧮 Add an `"UNDO"` action to the quiz reducer that reverts the most recent correct/incorrect mark (hint: you'll need to track a small history in the reducer's state).

---

## Learning Objectives

- ✓ Write `useEffect` cleanup functions correctly for anything that subscribes, listens, or runs on an interval.
- ✓ Recognize `useRef` as the tool for imperative DOM access, not general-purpose state storage.
- ✓ Use Context to avoid prop drilling across multiple component levels.
- ✓ Model related state transitions with a reducer instead of several separate `useState` calls.

**Good Luck! 🚀**
