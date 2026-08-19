> `useEffect` is the most misunderstood hook in React. It's not a lifecycle method. It's not "componentDidMount". It's a mechanism for synchronising your component with something outside React — a network request, a DOM subscription, a timer. Getting this mental model right fixes most of the bugs people blame on useEffect.

---

## Table of Contents

1. [What a Side Effect Is](#1-what-a-side-effect-is)
2. [useEffect — The Hook](#2-useeffect--the-hook)
3. [The Dependency Array](#3-the-dependency-array)
4. [Cleanup Functions](#4-cleanup-functions)
5. [Data Fetching with useEffect](#5-data-fetching-with-useeffect)
6. [Common Patterns](#6-common-patterns)
7. [useLayoutEffect](#7-uselayouteffect)
8. [The Rules of Effects](#8-the-rules-of-effects)
9. [Every Effect is a Synchronisation](#9-every-effect-is-a-synchronisation)

---

## 1. What a Side Effect Is

A **side effect** is anything a component does that reaches outside its own render — anything that interacts with the world beyond returning JSX.

```
Side effects in React:
  - Fetching data from an API
  - Setting up a subscription (WebSocket, event listener)
  - Manually manipulating the DOM
  - Setting a timer (setTimeout, setInterval)
  - Logging to an analytics service
  - Reading/writing to localStorage
  - Starting an animation

Not side effects (fine to do during render):
  - Computing derived values
  - Calling pure functions
  - Transforming props/state into JSX
```

Side effects don't belong in the render phase because React may call your component function multiple times, in any order, and even discard renders (in concurrent mode). Effects need to run at a predictable time — **after** the component is committed to the DOM.

---

## 2. `useEffect` — The Hook

```jsx
import { useEffect } from "react";

useEffect(
  () => {
    // Your side effect here
    // Runs after every render by default
  },
  [/* dependency array */],
);
```

```jsx
function DocumentTitle({ title }) {
  useEffect(() => {
    document.title = title;
    // Runs after every render where `title` changed
  }, [title]);

  return <h1>{title}</h1>;
}
```

**When does the effect run?**

1. React renders the component (calls the function, gets JSX)
2. React updates the real DOM
3. The browser paints the screen
4. **Then** React runs your `useEffect`

Effects always run **after** paint. They never block the browser from drawing.

---

## 3. The Dependency Array

The dependency array controls **when** the effect re-runs.

```jsx
// No dependency array — runs after EVERY render
useEffect(() => {
  console.log("rendered");
});

// Empty array [] — runs ONCE after mount, never again
useEffect(() => {
  fetchInitialData();
}, []);

// With dependencies — runs on mount AND whenever any dep changes
useEffect(() => {
  fetchUserData(userId);
}, [userId]);

useEffect(() => {
  document.title = `${title} | CWWKCC`;
}, [title]);

// Multiple dependencies — runs when ANY of them changes
useEffect(() => {
  fetchFilteredStudents(query, grade, sortBy);
}, [query, grade, sortBy]);
```

### The exhaustive-deps rule

The ESLint rule `react-hooks/exhaustive-deps` requires every value your effect uses from the component scope to be listed in the dependencies array. **Follow this rule.** Ignoring it leads to stale closure bugs.

```jsx
// ✗ Missing dependency — stale closure
function Component({ userId }) {
  const [data, setData] = useState(null);

  useEffect(() => {
    fetchUser(userId).then(setData);
    // userId is used but not in deps — if userId changes, effect won't re-run
  }, []); // ← missing userId

  return <div>{data?.name}</div>;
}

// ✓ Correct — all used values listed
useEffect(() => {
  fetchUser(userId).then(setData);
}, [userId]); // re-runs when userId changes

// ✓ If a function is used, it must be in deps (or defined inside the effect)
function Component({ onDataLoad }) {
  useEffect(() => {
    fetchData().then((data) => onDataLoad(data));
  }, [onDataLoad]); // onDataLoad must be in deps
}
```

---

## 4. Cleanup Functions

An effect can return a **cleanup function**. React calls it:

- Before re-running the effect (when deps change)
- When the component unmounts

Cleanup is how you prevent memory leaks, stale responses, and duplicate subscriptions.

```jsx
// Pattern: setup on run, teardown in cleanup
useEffect(() => {
  // Setup
  const subscription = subscribe(channel, handleMessage);

  // Cleanup — runs before next effect and on unmount
  return () => {
    subscription.unsubscribe();
  };
}, [channel]);
```

### Timers

```jsx
function Countdown({ from }) {
  const [count, setCount] = useState(from);

  useEffect(() => {
    if (count <= 0) return;

    const timerId = setTimeout(() => {
      setCount((c) => c - 1);
    }, 1000);

    return () => clearTimeout(timerId); // cancel if count changes or unmounts
  }, [count]);

  return <p>{count > 0 ? count : "Done!"}</p>;
}
```

### Event listeners

```jsx
function useWindowSize() {
  const [size, setSize] = useState({
    width: window.innerWidth,
    height: window.innerHeight,
  });

  useEffect(() => {
    function handleResize() {
      setSize({ width: window.innerWidth, height: window.innerHeight });
    }

    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
    // Without cleanup: every render would add another listener — memory leak
  }, []); // empty deps — run once, cleanup on unmount

  return size;
}
```

### WebSocket

```jsx
function useLiveScore(matchId) {
  const [score, setScore] = useState(null);

  useEffect(() => {
    const ws = new WebSocket(`wss://scores.api/match/${matchId}`);

    ws.onmessage = (event) => {
      setScore(JSON.parse(event.data));
    };

    ws.onerror = (err) => console.error("WS error:", err);

    return () => {
      ws.close(); // disconnect when matchId changes or component unmounts
    };
  }, [matchId]);

  return score;
}
```

---

## 5. Data Fetching with useEffect

Data fetching is the most common use of `useEffect`. There are several important patterns to get right.

### Basic fetch

```jsx
function StudentProfile({ studentId }) {
  const [student, setStudent] = useState(null);
  const [status, setStatus] = useState("loading");

  useEffect(() => {
    setStatus("loading");
    setStudent(null);

    fetchStudent(studentId)
      .then((data) => {
        setStudent(data);
        setStatus("success");
      })
      .catch((err) => {
        console.error(err);
        setStatus("error");
      });
  }, [studentId]);

  if (status === "loading") return <Spinner />;
  if (status === "error") return <ErrorMessage />;
  return <div>{student.name}</div>;
}
```

### The race condition problem

If `studentId` changes quickly (e.g. user clicks through profiles fast), multiple fetches can be in-flight simultaneously. A slow response from an earlier fetch can overwrite a newer one.

```jsx
// ✗ Race condition — response from old fetch can overwrite new one
useEffect(() => {
  fetchStudent(studentId).then(setStudent);
}, [studentId]);

// ✓ Fix 1: ignore stale responses with a flag
useEffect(() => {
  let cancelled = false;

  fetchStudent(studentId).then((data) => {
    if (!cancelled) {
      setStudent(data);
    }
    // If cancelled=true, this fetch response is stale — ignore it
  });

  return () => {
    cancelled = true; // cleanup: mark this effect's fetch as stale
  };
}, [studentId]);

// ✓ Fix 2: AbortController — actually cancel the in-flight request
useEffect(() => {
  const controller = new AbortController();

  fetchStudent(studentId, { signal: controller.signal })
    .then(setStudent)
    .catch((err) => {
      if (err.name === "AbortError") return; // expected — don't set error state
      setStatus("error");
    });

  return () => controller.abort(); // cancel on cleanup
}, [studentId]);
```

### Why TanStack Query is better for data fetching

`useEffect` for data fetching has many edge cases: race conditions, loading states, error states, caching, refetching. TanStack Query (File 16) handles all of these. For anything beyond a simple one-off fetch, use it.

```jsx
// With TanStack Query (File 16) — cleaner and handles all edge cases
const {
  data: student,
  isLoading,
  isError,
} = useQuery({
  queryKey: ["student", studentId],
  queryFn: () => fetchStudent(studentId),
});
```

---

## 6. Common Patterns

### Sync to external store

```jsx
function useLocalStorage(key, initialValue) {
  const [value, setValue] = useState(() => {
    const stored = localStorage.getItem(key);
    return stored ? JSON.parse(stored) : initialValue;
  });

  useEffect(() => {
    localStorage.setItem(key, JSON.stringify(value));
  }, [key, value]); // sync to localStorage whenever value changes

  return [value, setValue];
}

const [theme, setTheme] = useLocalStorage("theme", "dark");
```

### Document title

```jsx
function useDocumentTitle(title) {
  const defaultTitle = "CWWKCC";

  useEffect(() => {
    document.title = title ? `${title} | ${defaultTitle}` : defaultTitle;

    return () => {
      document.title = defaultTitle; // restore on unmount
    };
  }, [title]);
}

function StudentPage({ student }) {
  useDocumentTitle(student.name);
  return <div>...</div>;
}
```

### Scroll to top on route change

```jsx
function ScrollToTop() {
  const { pathname } = useLocation();

  useEffect(() => {
    window.scrollTo(0, 0);
  }, [pathname]);

  return null;
}
```

### Previous value

```jsx
function usePrevious(value) {
  const ref = useRef(undefined);

  useEffect(() => {
    ref.current = value;
    // Runs AFTER render — ref.current holds the previous render's value
    // during the current render
  });

  return ref.current; // returns previous render's value
}

function Component({ count }) {
  const prevCount = usePrevious(count);
  return (
    <p>
      {prevCount} → {count}
    </p>
  );
}
```

### Online/offline status

```jsx
function useOnlineStatus() {
  const [isOnline, setIsOnline] = useState(navigator.onLine);

  useEffect(() => {
    function handleOnline() {
      setIsOnline(true);
    }
    function handleOffline() {
      setIsOnline(false);
    }

    window.addEventListener("online", handleOnline);
    window.addEventListener("offline", handleOffline);

    return () => {
      window.removeEventListener("online", handleOnline);
      window.removeEventListener("offline", handleOffline);
    };
  }, []);

  return isOnline;
}
```

---

## 7. `useLayoutEffect`

`useLayoutEffect` has the same signature as `useEffect` but fires **synchronously after DOM mutations, before the browser paints**.

```
useEffect timeline:
  render → DOM update → browser PAINT → effect runs

useLayoutEffect timeline:
  render → DOM update → effect runs → browser PAINT
```

```jsx
// Use useLayoutEffect when you need to:
// 1. Read DOM measurements and immediately update state (to avoid flicker)
// 2. Imperatively modify the DOM before the user sees it

function Tooltip({ targetRef, children }) {
  const [position, setPosition] = useState({ top: 0, left: 0 });
  const tooltipRef = useRef(null);

  useLayoutEffect(() => {
    // Must use useLayoutEffect — if we used useEffect, the user would briefly
    // see the tooltip at position {0, 0} before it jumps to the correct position
    const targetRect = targetRef.current.getBoundingClientRect();
    const tooltipRect = tooltipRef.current.getBoundingClientRect();

    setPosition({
      top: targetRect.bottom + 8,
      left: targetRect.left + targetRect.width / 2 - tooltipRect.width / 2,
    });
  }, [targetRef]);

  return (
    <div
      ref={tooltipRef}
      style={{ position: "fixed", top: position.top, left: position.left }}
    >
      {children}
    </div>
  );
}
```

**Default to `useEffect`**. Only switch to `useLayoutEffect` when you have a visible flicker that `useEffect` causes. `useLayoutEffect`runs synchronously and blocks painting — misuse hurts performance.

---

## 8. The Rules of Effects

### Effects should be about one thing

```jsx
// ✗ One effect doing multiple unrelated things
useEffect(() => {
  document.title = title; // thing 1
  analytics.track("page_view"); // thing 2
  fetchRelatedContent(postId); // thing 3
}, [title, postId]);

// ✓ Separate effects for separate concerns
useEffect(() => {
  document.title = title;
}, [title]);

useEffect(() => {
  analytics.track("page_view");
}, []);

useEffect(() => {
  fetchRelatedContent(postId);
}, [postId]);
```

### Don't use effects for derived state

```jsx
// ✗ Using an effect to compute derived state
const [firstName, setFirstName] = useState("");
const [lastName, setLastName] = useState("");
const [fullName, setFullName] = useState("");

useEffect(() => {
  setFullName(firstName + " " + lastName);
}, [firstName, lastName]);
// Extra render, extra state, extra complexity

// ✓ Compute during render
const fullName = firstName + " " + lastName;
```

### Don't use effects for event handlers

```jsx
// ✗ Using an effect to respond to user actions
useEffect(() => {
  if (submitted) {
    saveToDatabase(formData);
    setSubmitted(false);
  }
}, [submitted]);

// ✓ Handle in the event handler directly
function handleSubmit() {
  saveToDatabase(formData);
}
```

---

## 9. Every Effect is a Synchronisation

The right mental model for `useEffect` is not "run code at lifecycle events." It's "**keep this external thing in sync with this React state/prop**."

```
useEffect(() => {
  // START synchronising: connect/subscribe/fetch/set with [deps]
  startSomething(deps);

  return () => {
    // STOP synchronising: disconnect/unsubscribe/cancel/unset
    stopSomething();
  };
}, [deps]);
```

```
Document title ↔ title prop
  start: document.title = title
  stop:  document.title = "default"

WebSocket connection ↔ matchId
  start: new WebSocket(matchId)
  stop:  ws.close()

Event listener ↔ (nothing — just needs cleanup)
  start: window.addEventListener("resize", fn)
  stop:  window.removeEventListener("resize", fn)

Fetch request ↔ userId
  start: controller = new AbortController(); fetch(userId, {signal})
  stop:  controller.abort()
```

If you can't identify what you're syncing and what the "stop" is — reconsider whether you need an effect at all.

---

## Summary

```
What effects are for:
  Synchronising with things outside React:
  network, DOM, subscriptions, timers, external stores

useEffect signature:
  useEffect(() => { /* setup */ return () => { /* cleanup */ }; }, [deps]);

Dependency array:
  Omitted:  runs after every render
  []:       runs once on mount
  [a, b]:   runs on mount + whenever a or b changes
  All values from component scope used inside must be listed

Cleanup function:
  Returned from the effect
  Runs before re-running the effect AND on unmount
  Required for: timers, event listeners, subscriptions, fetch abort

Data fetching:
  Use AbortController to cancel stale requests
  Set cancelled flag to ignore stale responses
  Better: use TanStack Query (File 16) for anything real

useLayoutEffect:
  Same as useEffect but runs before browser paint
  Only use to prevent visible flicker from DOM measurements
  Default to useEffect — useLayoutEffect blocks painting

Rules:
  One effect, one concern — split unrelated effects
  Don't use effects for derived state — compute during render
  Don't use effects for event responses — handle in event handlers
  Missing deps = stale closure bugs; extra deps = unnecessary re-runs

Mental model:
  Not "run at lifecycle" — "synchronise with external system"
  Every effect has a start (setup) and a stop (cleanup)
```

---

_Next: [09 — useRef](09%20-%20UseRef.md)_
