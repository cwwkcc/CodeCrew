## Assignment Overview

Build an attendance tracker: a list of students where you can mark each one Present/Absent/Late, add a note, and see a live summary. This assignment is entirely about handling real user interaction correctly — conditional rendering, list updates, event handling, and controlled form inputs working together.

---

## Covers

`react/04 - Rendering & The Virtual DOM`, `react/05 - Conditional Rendering & Lists`, `react/06 - Event Handling`, `react/07 - Forms & Controlled Components`

---

## Starting Data

```jsx
const initialStudents = [
  { id: 1, name: "Ashan Perera", status: "unmarked", note: "" },
  { id: 2, name: "Nadeesha Silva", status: "unmarked", note: "" },
  { id: 3, name: "Thanuga Fernando", status: "unmarked", note: "" },
];
```

---

## Your Task

### 1. Status Buttons Per Student

For each student, render three buttons: **Present**, **Absent**, **Late**. Clicking one sets that student's `status` — and the currently-selected status should be visually distinct (e.g. a highlighted background on whichever button matches the current status).

```jsx
// The KEY requirement: clicking a button for ONE student must not affect
// any other student's status. This means your state update needs to
// target a single item inside the array, not the whole array.
```

### 2. A Note Field — Controlled Input

Each student also gets a small text input for an optional note ("left early", "called in sick", etc.). This must be a **controlled component** (Module 07) — its value comes from state, and typing updates that state via `onChange`, rather than letting the DOM manage the input's value independently.

### 3. Conditional Rendering — the Summary

Above the list, render a live summary:

```
Present: 1   Absent: 1   Late: 0   Unmarked: 1
```

- If **every** student has been marked (no one is `"unmarked"`), replace the summary with: **"✅ Attendance complete!"**
- This requires genuine conditional rendering — not just always showing both possibilities with CSS `display: none` on one of them.

### 4. Filtering the List (Conditional Rendering + Lists Together)

Add a small set of filter buttons above the list: **All / Present / Absent / Late / Unmarked**. Clicking one shows only students matching that status — Module 05's list-rendering and conditional-rendering topics used together.

---

## Constraints

- **State must live in the parent component** (holding the full array of students), passed down to each student row as props, with an update function passed down too — not separate, independent state inside each row component. This is the "lifting state up" pattern that makes the cross-student summary and filtering possible at all.
- **Never mutate the students array or a student object directly.** Every status/note change must produce a new array (and a new object for the changed student) — `array.map()` returning new objects is the standard pattern here.
- Every list render still needs correct, stable `key` props (carried over from the previous assignment).

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — Updating one student's status without touching the others:
>
> ```jsx
> setStudents(
>   students.map((s) => (s.id === studentId ? { ...s, status: newStatus } : s)),
> );
> ```
>
> This creates a new array where only the matching student is a new object — everyone else is untouched.

> 💡 **Hint 2** — The "all marked" check: `students.every(s => s.status !== "unmarked")` — `.every()` is the right tool for "is this true for every item," as opposed to `.some()`, which checks "is this true for at least one."

> 💡 **Hint 3** — For the note input, remember the controlled-component shape: `<input value={student.note} onChange={(e) => updateNote(student.id, e.target.value)} />` — a controlled input with no `onChange` handler will appear "frozen" and refuse to accept typed input, which is a very common first mistake.

---

## Submission Guidelines

1. `App.jsx` (or split into components — your choice)
2. A short note on how you decided where state should "live" (which component owns the `students` array) and why

---

## Bonus Challenge 🏆

- 💾 Add a "Reset All" button that sets every student back to `"unmarked"` with an empty note, in one click.
- 📈 Add a percentage: `"Attendance rate: 67%"` (present + late, out of total marked).
- ⌨️ Make the note input show a small character counter (`12/100`), and prevent typing beyond a max length.

---

## Learning Objectives

- ✓ Lift state up to the appropriate parent component instead of duplicating it.
- ✓ Update a single item within an array of state correctly, without mutation.
- ✓ Build genuinely controlled form inputs.
- ✓ Combine conditional rendering and list rendering in one realistic feature.

**Good Luck! 🚀**
