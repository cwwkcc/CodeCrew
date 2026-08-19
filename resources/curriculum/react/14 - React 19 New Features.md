> React 19 (released December 2024) introduced a set of new primitives that address patterns every application needs: form submissions, async transitions, optimistic updates, and reading async resources directly in components. These aren't replacements for what came before — they're additions that make specific common patterns cleaner.

---

## Table of Contents

1. [Overview of What's New](#1-overview-of-whats-new)
2. [Actions — Async Transitions](#2-actions--async-transitions)
3. [useTransition with Async Functions](#3-usetransition-with-async-functions)
4. [useActionState](#4-useactionstate)
5. [useFormStatus](#5-useformstatus)
6. [useOptimistic](#6-useoptimistic)
7. [use() — Reading Resources](#7-use--reading-resources)
8. [Server Actions (Next.js context)](#8-server-actions-nextjs-context)
9. [ref as a Prop](#9-ref-as-a-prop)
10. [When to Use These vs The Old Patterns](#10-when-to-use-these-vs-the-old-patterns)

---

## 1. Overview of What's New

React 19 centres on one theme: **async operations in components**. The new primitives give you first-class support for:

```
useTransition  → wrap async functions, get isPending automatically
useActionState → manage state for form actions (pending, errors, result)
useFormStatus  → read parent form's pending state from any child
useOptimistic  → show expected result immediately, roll back on error
use()          → read a Promise or Context anywhere in render
```

The underlying concept they share: **Actions**. An action is an async function that can be passed to transitions or forms. React automatically tracks their pending state.

---

## 2. Actions — Async Transitions

Before React 19, calling an async function required manual pending state management:

```jsx
// Before React 19
function SaveButton({ onSave }) {
  const [isPending, setIsPending] = useState(false);
  const [error, setError] = useState(null);

  async function handleClick() {
    setIsPending(true);
    setError(null);
    try {
      await onSave();
    } catch (err) {
      setError(err.message);
    } finally {
      setIsPending(false);
    }
  }

  return (
    <button onClick={handleClick} disabled={isPending}>
      {isPending ? "Saving..." : "Save"}
    </button>
  );
}
```

React 19 lets `useTransition` accept async functions:

```jsx
// React 19
function SaveButton({ onSave }) {
  const [isPending, startTransition] = useTransition();
  const [error, setError] = useState(null);

  function handleClick() {
    startTransition(async () => {
      try {
        await onSave(); // async function inside startTransition
      } catch (err) {
        setError(err.message);
      }
    });
    // isPending is automatically true while the async function runs
  }

  return (
    <button onClick={handleClick} disabled={isPending}>
      {isPending ? "Saving..." : "Save"}
    </button>
  );
}
```

---

## 3. `useTransition` with Async Functions

`useTransition` marks state updates as non-urgent transitions. In React 19, it also wraps async functions — tracking their pending state automatically.

```jsx
import { useTransition } from "react";

const [isPending, startTransition] = useTransition();
```

```jsx
function StudentForm({ studentId }) {
  const [name, setName] = useState("");
  const [isPending, startTransition] = useTransition();
  const [error, setError] = useState(null);

  function handleSubmit(e) {
    e.preventDefault();

    startTransition(async () => {
      try {
        await updateStudentName(studentId, name);
        // State updates here are batched and non-urgent
      } catch (err) {
        setError(err.message); // errors inside startTransition are caught
      }
    });
  }

  return (
    <form onSubmit={handleSubmit}>
      <input
        value={name}
        onChange={(e) => setName(e.target.value)}
        disabled={isPending}
      />
      {error && <p className="error">{error}</p>}
      <button type="submit" disabled={isPending}>
        {isPending ? "Updating..." : "Update"}
      </button>
    </form>
  );
}
```

---

## 4. `useActionState`

`useActionState` manages the state of a form action: the result, any error, and the pending status. It's designed to work with `<form action={fn}>`.

```jsx
import { useActionState } from "react";

const [state, formAction, isPending] = useActionState(action, initialState);
```

```jsx
// The action function receives previous state + FormData
async function submitContactForm(prevState, formData) {
  const name = formData.get("name");
  const email = formData.get("email");
  const message = formData.get("message");

  // Validate
  if (!name || !email || !message) {
    return { error: "All fields are required", success: false };
  }

  try {
    await sendContactEmail({ name, email, message });
    return { success: true, error: null };
  } catch (err) {
    return { success: false, error: err.message };
  }
}

function ContactForm() {
  const [state, formAction, isPending] = useActionState(submitContactForm, {
    success: false,
    error: null,
  });

  if (state.success) {
    return <p>Message sent! We'll get back to you shortly.</p>;
  }

  return (
    <form action={formAction}>
      <input name="name" placeholder="Your name" required />
      <input name="email" type="email" required />
      <textarea name="message" required />

      {state.error && <p className="error">{state.error}</p>}

      <button type="submit" disabled={isPending}>
        {isPending ? "Sending..." : "Send message"}
      </button>
    </form>
  );
}
```

---

## 5. `useFormStatus`

`useFormStatus` reads the status of the nearest parent `<form>`. It must be called inside a component that is a child of the form — not in the form component itself.

```jsx
import { useFormStatus } from "react-dom";

function SubmitButton() {
  const { pending } = useFormStatus();
  // pending is true while the form's action is executing

  return (
    <button type="submit" disabled={pending}>
      {pending ? "Submitting..." : "Submit"}
    </button>
  );
}
```

The power: `SubmitButton` can be a shared, reusable component that automatically knows when any parent form is pending — without being passed `isPending` as a prop.

```jsx
// Reusable across all forms in your app
function SubmitButton({ children = "Submit" }) {
  const { pending } = useFormStatus();

  return (
    <button
      type="submit"
      disabled={pending}
      className={pending ? "btn btn-loading" : "btn btn-primary"}
    >
      {pending ? (
        <>
          <Spinner size="sm" />
          <span>Please wait...</span>
        </>
      ) : children}
    </button>
  );
}

// Used in any form — automatically shows loading state
<form action={submitAdmissions}>
  <input name="name" />
  <input name="grade" />
  <SubmitButton>Apply for Admission</SubmitButton>
</form>

<form action={submitContact}>
  <textarea name="message" />
  <SubmitButton>Send Message</SubmitButton>
</form>
```

---

## 6. `useOptimistic`

`useOptimistic` lets you show the expected result of an action immediately, before the server confirms it. If the action fails, it automatically reverts to the previous state.

```jsx
import { useOptimistic } from "react";

const [optimisticValue, addOptimistic] = useOptimistic(
  currentValue, // the real value (from state/server)
  (currentValue, update) => newOptimisticValue, // how to compute optimistic value
);
```

```jsx
function LikeButton({ post }) {
  const [likes, setLikes] = useState(post.likes);
  const [isLiked, setIsLiked] = useState(post.isLikedByUser);

  const [optimisticLikes, addOptimisticLike] = useOptimistic(
    likes,
    (currentLikes, newLikes) => newLikes,
  );

  const [optimisticIsLiked, addOptimisticIsLiked] = useOptimistic(
    isLiked,
    (_, newState) => newState,
  );

  async function handleLike() {
    const newIsLiked = !isLiked;
    const newLikes = isLiked ? likes - 1 : likes + 1;

    // Show the result immediately — before the server responds
    addOptimisticLike(newLikes);
    addOptimisticIsLiked(newIsLiked);

    try {
      const result = await toggleLike(post.id);
      // Update real state with server's confirmed values
      setLikes(result.likes);
      setIsLiked(result.isLiked);
    } catch {
      // If it fails, optimistic values automatically revert to the real state
      // (likes and isLiked are unchanged, so the UI reverts)
    }
  }

  return (
    <button onClick={handleLike} className={optimisticIsLiked ? "liked" : ""}>
      ♥ {optimisticLikes}
    </button>
  );
}
```

### Optimistic list operations

```jsx
function MessageThread({ threadId }) {
  const [messages, setMessages] = useState([]);

  const [optimisticMessages, addOptimisticMessage] = useOptimistic(
    messages,
    (currentMessages, newMessage) => [...currentMessages, newMessage],
  );

  async function sendMessage(text) {
    const tempMessage = {
      id: "temp-" + Date.now(),
      text,
      sender: currentUser,
      pending: true, // flag to show "sending..." style
    };

    // Add immediately to UI
    addOptimisticMessage(tempMessage);

    try {
      const confirmedMessage = await sendMessageApi(threadId, text);
      setMessages((prev) => [...prev, confirmedMessage]);
    } catch (err) {
      // Optimistic message automatically removed on error
      showToast("Failed to send message", "error");
    }
  }

  return (
    <div>
      {optimisticMessages.map((msg) => (
        <Message
          key={msg.id}
          message={msg}
          className={msg.pending ? "opacity-50" : ""}
        />
      ))}
      <MessageInput onSend={sendMessage} />
    </div>
  );
}
```

---

## 7. `use()` — Reading Resources

`use()` is a new hook that reads the value of a Promise or Context during render. Unlike `useContext`, it can be called conditionally. Unlike `await`, it integrates with React's Suspense.

```jsx
import { use } from "react";

// Reading a Promise — component suspends until resolved
function StudentProfile({ studentPromise }) {
  const student = use(studentPromise); // throws the promise (Suspense catches it)
  return <div>{student.name}</div>;
}

// Must be wrapped in Suspense
<Suspense fallback={<Spinner />}>
  <StudentProfile studentPromise={fetchStudent(id)} />
</Suspense>;
```

```jsx
// Reading Context — use() vs useContext()
// useContext: must be at top level
// use(): can be inside conditions, loops, early returns

function AdminPanel({ isAdmin }) {
  if (!isAdmin) return null;

  const user = use(UserContext); // ✓ called after a conditional
  return <div>Admin: {user.name}</div>;
}

// useContext would fail here:
function AdminPanel({ isAdmin }) {
  if (!isAdmin) return null;
  const user = useContext(UserContext); // ✗ Hook called after conditional
}
```

---

## 8. Server Actions (Next.js context)

In Next.js 14+ with React 19, **Server Actions** are async functions that run on the server — directly callable from client components. This is where `useActionState` really shines.

```jsx
// app/actions/admissions.ts
"use server";

export async function submitAdmissionsForm(prevState: unknown, formData: FormData) {
  const data = {
    studentName: formData.get("studentName") as string,
    grade:       formData.get("grade") as string,
    parentEmail: formData.get("parentEmail") as string,
  };

  // Validation
  if (!data.studentName || !data.grade || !data.parentEmail) {
    return { success: false, error: "All fields are required" };
  }

  // Database call — runs on server, never exposed to client
  await prisma.admissionsApplication.create({ data });

  // Send confirmation email
  await sendConfirmationEmail(data.parentEmail, data.studentName);

  return { success: true, error: null };
}
```

```jsx
// app/admissions/page.tsx (client component)
"use client";
import { useActionState } from "react";
import { submitAdmissionsForm } from "../actions/admissions";

export default function AdmissionsPage() {
  const [state, formAction, isPending] = useActionState(submitAdmissionsForm, {
    success: false,
    error: null,
  });

  if (state.success) {
    return (
      <div className="text-center">
        <h2>Application Submitted!</h2>
        <p>We will contact you at your parent's email address.</p>
      </div>
    );
  }

  return (
    <form action={formAction} className="space-y-4">
      <input name="studentName" placeholder="Student's full name" required />
      <select name="grade">
        {[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
          <option key={g} value={g}>
            Grade {g}
          </option>
        ))}
      </select>
      <input
        name="parentEmail"
        type="email"
        placeholder="Parent's email"
        required
      />

      {state.error && <p className="text-red-500">{state.error}</p>}

      <button type="submit" disabled={isPending}>
        {isPending ? "Submitting..." : "Submit Application"}
      </button>
    </form>
  );
}
```

---

## 9. `ref` as a Prop

In React 19, `ref` is now a regular prop. You no longer need `forwardRef` to pass refs to custom components.

```jsx
// Before React 19
const Input = forwardRef(function Input(props, ref) {
  return <input ref={ref} {...props} />;
});

// React 19 — ref is just a prop
function Input({ ref, ...props }) {
  return <input ref={ref} {...props} />;
}

// Usage — identical in both cases
const inputRef = useRef(null);
<Input ref={inputRef} type="email" />;
```

`forwardRef` still works but is now deprecated. Migrate gradually.

---

## 10. When to Use These vs The Old Patterns

```
Feature           Use when                              Old pattern still fine?
─────────────────────────────────────────────────────────────────────────────
useTransition     Async button actions                  useState + manual pending: YES
(async)           Non-urgent state updates              (use new API for new code)

useActionState    Forms with server actions             Manual state: YES for simple forms
                  Form submission with result/error     React Hook Form: YES for complex forms

useFormStatus     Reusable submit buttons               Prop drilling isPending: YES (simpler)
                  Buttons that don't own the form

useOptimistic     Likes, votes, quick toggles           setState + rollback: YES (less clean)
                  Chat messages, list item actions

use()             Reading Promise in render             useEffect + useState: YES
                  Conditional Context reading           (use() is new; expect ecosystem to catch up)

Server Actions    Form submissions in Next.js           API routes + fetch: YES (more control)
                  Simple CRUD from forms                tRPC: YES for complex API needs

ref as prop       All custom components (new code)      forwardRef: YES (still works)
```

---

## Summary

```
Actions:
  Async functions passed to transitions or form actions
  React tracks their pending state automatically

useTransition (React 19 upgrade):
  Now accepts async functions
  isPending automatically tracks async function lifecycle

useActionState:
  [state, formAction, isPending] = useActionState(fn, initialState)
  fn receives (prevState, formData) → returns new state
  Works with <form action={formAction}>

useFormStatus:
  { pending } = useFormStatus()
  Must be inside a child of the form (not the form itself)
  Makes reusable submit buttons that know their form's pending state

useOptimistic:
  [optimisticValue, addOptimistic] = useOptimistic(realValue, updaterFn)
  Show expected result immediately
  Auto-reverts to real value when async action completes/fails

use():
  Reads a Promise (suspends until resolved) or Context
  Can be called conditionally (unlike useContext)
  Requires Suspense boundary for Promise reading

ref as prop:
  ref is now a regular prop in React 19
  forwardRef is deprecated but still works

Server Actions (Next.js):
  "use server" — async function that runs on the server
  Callable directly from client components
  Perfect partner for useActionState
```

---

_Next: [15 — React Router](./15%20-%20React%20Router.md)_
