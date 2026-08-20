> Before writing a single line of React, you need the right mental model. React has opinions about how UIs should be built — and those opinions are different from the DOM manipulation you learned in JavaScript. Understanding _why_ React exists and _how_ it thinks about UIs will make everything else make sense.

---

## Table of Contents

1. [The Problem React Solves](#1-the-problem-react-solves)
2. [The Core Idea: UI as a Function of State](#2-the-core-idea-ui-as-a-function-of-state)
3. [Declarative vs Imperative](#3-declarative-vs-imperative)
4. [Components — The Unit of UI](#4-components--the-unit-of-ui)
5. [The Component Tree](#5-the-component-tree)
6. [React vs the DOM](#6-react-vs-the-dom)
7. [The React Ecosystem](#7-the-react-ecosystem)
8. [Your First React App](#8-your-first-react-app)

---

## 1. The Problem React Solves

Before React, building dynamic UIs meant manually manipulating the DOM:

```javascript
// Traditional DOM manipulation — what you learned in JS File 04
function updateUserProfile(user) {
  document.getElementById("username").textContent = user.name;
  document.getElementById("avatar").src = user.avatarUrl;
  document.getElementById("email").textContent = user.email;

  if (user.isAdmin) {
    document.getElementById("admin-badge").style.display = "block";
  } else {
    document.getElementById("admin-badge").style.display = "none";
  }

  if (user.notifications.length > 0) {
    document.getElementById("notification-count").textContent =
      user.notifications.length;
    document.getElementById("notification-bell").classList.add("active");
  } else {
    document.getElementById("notification-bell").classList.remove("active");
  }
}
```

This works for one function. But as an app grows:

- You have dozens of pieces of state (user, posts, notifications, theme, sidebar, modals...)
- Each piece of state affects multiple parts of the UI
- Changes to one thing need to update other things
- You have to track what the DOM currently looks like to know what to change

You end up writing a lot of code that asks: "what is the current state of the UI, and what do I need to change to make it match the new state?" This is **synchronising state with UI** — and it gets exponentially harder as the app grows.

React's answer: **don't synchronise. Re-describe.**

---

## 2. The Core Idea: UI as a Function of State

React's fundamental insight is this:

```
UI = f(state)
```

Your UI is a pure function of your application state. Given a specific state, the UI should always look a specific way. When state changes, React re-runs the function and figures out what changed.

```javascript
// Instead of: "change this element because the user logged in"
// You say:    "here's what the UI should look like given this user"

function UserProfile({ user }) {
  return (
    <div>
      <img src={user.avatarUrl} />
      <h2>{user.name}</h2>
      <p>{user.email}</p>
      {user.isAdmin && <span className="badge">Admin</span>}
      {user.notifications.length > 0 && (
        <span className="bell active">{user.notifications.length}</span>
      )}
    </div>
  );
}
```

When `user` changes, React re-runs `UserProfile` with the new user and figures out what DOM changes are needed. You never touch the DOM directly. You just describe what the UI should look like.

This is the shift. Everything else in React flows from this idea.

---

## 3. Declarative vs Imperative

**Imperative** code says _how_ to do something, step by step:

```javascript
// Imperative — you tell the DOM what to do
const list = document.getElementById("student-list");
list.innerHTML = ""; // clear it

students.forEach((student) => {
  const li = document.createElement("li");
  li.className = "student-item";
  li.textContent = student.name;
  if (student.score > 90) li.classList.add("top-student");
  list.appendChild(li);
});
```

**Declarative** code says _what_ you want, not how to achieve it:

```javascript
// Declarative — you describe the desired output
function StudentList({ students }) {
  return (
    <ul>
      {students.map((student) => (
        <li
          key={student.id}
          className={
            student.score > 90 ? "student-item top-student" : "student-item"
          }
        >
          {student.name}
        </li>
      ))}
    </ul>
  );
}
```

React handles the _how_. You handle the _what_. This is why React code is easier to read and reason about — you can look at a component and immediately understand what it renders, without tracing through a sequence of mutations.

---

## 4. Components — The Unit of UI

A React **component** is a function that:

- Accepts **props** (inputs, like function parameters)
- Returns **JSX** (a description of the UI to render)

```javascript
// The simplest possible component
function Greeting() {
  return <h1>Hello, world!</h1>;
}

// A component with props
function Greeting({ name }) {
  return <h1>Hello, {name}!</h1>;
}

// Using the component
<Greeting name="Ashan" />
<Greeting name="Dineth" />
```

Components are composable — you build complex UIs by combining simple components:

```javascript
function Avatar({ src, alt }) {
  return <img src={src} alt={alt} className="avatar" />;
}

function UserName({ name, role }) {
  return (
    <div>
      <strong>{name}</strong>
      <span className="role">{role}</span>
    </div>
  );
}

function UserCard({ user }) {
  return (
    <div className="card">
      <Avatar src={user.avatarUrl} alt={user.name} />
      <UserName name={user.name} role={user.role} />
    </div>
  );
}
```

**Three rules for component names:**

1. Must start with a capital letter — `UserCard`, not `userCard`
2. Must return something renderable (JSX, string, number, null, or an array)
3. Must be a pure function with respect to its props — same props → same output

---

## 5. The Component Tree

React UIs are trees of components. There's one root component (usually `App`), and everything else is nested inside it.

```
App
├── Header
│     ├── Logo
│     ├── Nav
│     │     ├── NavLink (×5)
│     │     └── MobileMenuButton
│     └── AuthButtons
│           ├── LoginButton
│           └── SignupButton
├── Main
│     ├── HeroSection
│     │     ├── HeroHeading
│     │     └── HeroImage
│     └── NewsSection
│           └── NewsCard (×6)
└── Footer
      ├── FooterLinks
      └── Copyright
```

Data flows **down** through this tree via props. Events flow **up** via callback props. This one-way data flow makes it easy to trace where data comes from and who changes it.

---

## 6. React vs the DOM

React sits between your code and the DOM. You never touch the DOM directly in React — you describe what you want, and React handles the DOM.

```
Your Components (JSX)
        ↓
    React (library)
        ↓
  Virtual DOM (React's internal representation)
        ↓
  Reconciliation (what changed?)
        ↓
  Real DOM (browser)
```

**The Virtual DOM** is a lightweight JavaScript representation of the DOM tree. When state changes:

1. React re-runs the component function to get the new JSX
2. Compares new virtual DOM with previous virtual DOM (diffing)
3. Calculates the minimal set of real DOM changes needed
4. Applies only those changes

This is called **reconciliation**. It's what makes React fast — instead of rebuilding the entire DOM, it makes surgical updates. Full detail in [[04 - Rendering & The Virtual DOM]].

---

## 7. The React Ecosystem

React itself is intentionally minimal — it only handles the view layer. A real application needs more:

```
React (core)          — components, state, rendering
React DOM             — renders React to the browser DOM
React Router          — navigation between pages/views
TanStack Query        — server state, data fetching, caching
Zustand               — global client state
React Hook Form       — form state and validation
Framer Motion         — animations
shadcn/ui + Radix     — accessible component primitives
```

React doesn't prescribe how you do routing, state management, or data fetching. That's both its strength (flexibility) and its challenge (you have to choose).

---

## 8. Your First React App

```bash
# Create a new React app (Vite — fast, modern)
npm create vite@latest my-app -- --template react
cd my-app
npm install
npm run dev

# For Next.js
npx create-next-app@latest my-app
```

The entry point:

```javascript
// main.jsx (Vite) or app/layout.tsx (Next.js App Router)
import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App";

// Mount React to the DOM
ReactDOM.createRoot(document.getElementById("root")).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
);
```

```javascript
// App.jsx — the root component
export default function App() {
  return (
    <div>
      <h1>CWWKCC</h1>
      <p>C.W.W. Kannangara Central College</p>
    </div>
  );
}
```

**`React.StrictMode`** — a development-only wrapper that:

- Runs component functions twice to detect side effects in wrong places
- Warns about deprecated APIs
- Helps you find bugs before production

You'll notice effects running twice in development — this is intentional and expected. It's StrictMode checking your effects are properly cleaned up.

---

## Summary

```
The problem React solves:
  Manual DOM synchronisation doesn't scale.
  As apps grow, tracking "what changed and what to update" becomes unmanageable.

The core idea:
  UI = f(state)
  Describe what the UI should look like for a given state.
  React figures out how to make the DOM match.

Declarative vs Imperative:
  Imperative: "do this, then this, then that"
  Declarative: "here's what I want" — React handles the how

Components:
  Functions that take props and return JSX
  Must start with capital letter
  Must be pure — same props → same output
  Compose to build complex UIs from simple pieces

Component tree:
  One root, everything nested inside
  Data flows DOWN (props), events flow UP (callbacks)

React vs DOM:
  You never touch the DOM directly
  React maintains a virtual DOM, diffs it, applies minimal real DOM changes

The ecosystem:
  React is just the view layer
  Router, state management, data fetching — all separate libraries
```

---

_Next: [[01 - JSX in Depth]]_
