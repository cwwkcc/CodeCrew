## Assignment Overview

Build your first real React feature: a list of student cards, rendered from data, with a working "search" input that filters the list by typing. This is the assignment where the React mental model — components, props, and state — stops being theory and becomes something you've actually built.

---

## Covers

`react/00 - What React is & The Mental Model`, `react/01 - JSX in Depth`, `react/02 - Components & Props`, `react/03 - State & UseState`

---

## Starting Data

```jsx
const students = [
  { id: 1, name: "Ashan Perera", grade: 10, club: "ICT Society" },
  { id: 2, name: "Nadeesha Silva", grade: 11, club: "Debate Club" },
  { id: 3, name: "Thanuga Fernando", grade: 10, club: "ICT Society" },
  { id: 4, name: "Vindi Jayasuriya", grade: 12, club: "Robotics Club" },
];
```

---

## Your Task

### 1. Build a `StudentCard` Component

```jsx
function StudentCard({ name, grade, club }) {
  // renders one student's details
}
```

- Takes `name`, `grade`, and `club` as **props** (not by reaching into a global `students` array from inside the component)
- Renders them in some reasonable layout (a `<div>` with a heading and two lines of text is enough — no styling requirements for this assignment)

### 2. Build a `StudentList` Component

```jsx
function StudentList({ students }) {
  // renders one <StudentCard> per student
}
```

- Takes an array of students as a prop, and renders one `StudentCard` per entry
- **Every rendered item needs a `key` prop** — and it must be the student's `id`, not the array index (Module 02/05 covers exactly why array index is the wrong choice once a list can be filtered, reordered, or have items removed)

### 3. Add a Search Input with State

```jsx
function App() {
  // your state and filtering logic here
}
```

- Add a text `<input>` above the list
- Typing into it should filter the rendered list in real time to only students whose `name` includes the typed text (case-insensitive)
- The filtering itself should use **state** to hold the current search text — typing triggers a re-render with a narrower list, not direct DOM manipulation

---

## Constraints

- **No direct DOM manipulation** (`document.querySelector`, `.innerHTML`, etc.) anywhere — everything must go through JSX, props, and state. This is the core discipline the React mental model is built on: you describe _what_ the UI should look like for a given state, and React handles updating the actual DOM.
- **`StudentCard` must not know about the full `students` array or the search text.** It only knows what it's given through its own props — this is what makes it reusable and testable in isolation.
- Filtering must happen on every keystroke (no submit button) — this is what actually exercises `useState` correctly.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — The search state: `const [searchText, setSearchText] = useState("");` — then the input's `onChange` calls `setSearchText(e.target.value)`.

> 💡 **Hint 2** — Filtering: `students.filter(s => s.name.toLowerCase().includes(searchText.toLowerCase()))` — computed fresh on every render, not stored as its own separate state.

> 💡 **Hint 3** — If your list "flickers" or behaves oddly when filtering, double check you're using `student.id` as the `key`, not the array index from `.map((student, index) => ...)`.

---

## Submission Guidelines

1. `App.jsx` (or split into `StudentCard.jsx` / `StudentList.jsx` / `App.jsx` if you prefer multiple files)
2. A short note explaining, in your own words, why `key={index}` would have been the wrong choice here even though it "works" when the list order never changes

---

## Bonus Challenge 🏆

- 🎯 Add a second filter — a dropdown to filter by `grade` — combined with the existing text search (both filters active at once).
- 📊 Show a live count above the list: `"Showing 2 of 4 students"`, updating as the filters change.
- 🃏 Add a `selected` state so clicking a card highlights it (only one card selected at a time).

---

## Learning Objectives

- ✓ Understand props as read-only data passed down, and state as data a component owns and can change.
- ✓ Render a list correctly with proper, stable `key` props.
- ✓ Build a component hierarchy where each piece only knows what it's explicitly given.

**Good Luck! 🚀**
