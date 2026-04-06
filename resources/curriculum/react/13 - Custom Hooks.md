> Custom hooks are the most powerful composition tool in React. They let you extract stateful logic — including effects, subscriptions, and derived state — into reusable functions. A well-designed custom hook makes the calling component clean and declarative, hiding all complexity behind a clean interface.

---

## Table of Contents

1. [What Custom Hooks Are](#1-what-custom-hooks-are)
2. [The Rules and Conventions](#2-the-rules-and-conventions)
3. [Extracting Logic into Hooks](#3-extracting-logic-into-hooks)
4. [Data Fetching Hooks](#4-data-fetching-hooks)
5. [UI State Hooks](#5-ui-state-hooks)
6. [Browser API Hooks](#6-browser-api-hooks)
7. [Form Hooks](#7-form-hooks)
8. [Composition — Hooks That Use Hooks](#8-composition--hooks-that-use-hooks)
9. [Hook Design Guidelines](#9-hook-design-guidelines)

---

## 1. What Custom Hooks Are

A custom hook is a JavaScript function whose name starts with `use` and which calls other hooks inside it.

That's it. There's no special API, no registration — just a naming convention that tells React (and the linter) to apply the rules of hooks to this function.

```jsx
// This IS a custom hook — starts with "use", calls hooks inside
function useWindowWidth() {
  const [width, setWidth] = useState(window.innerWidth);

  useEffect(() => {
    const handleResize = () => setWidth(window.innerWidth);
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  return width;
}

// This is NOT a hook — doesn't call any hooks
function getWindowWidth() {
  return window.innerWidth;
}

// Usage
function ResponsiveComponent() {
  const width = useWindowWidth();
  return <p>Window is {width}px wide</p>;
}
```

**Each call to a custom hook creates independent state.** Two components both calling `useWindowWidth()` each get their own `width` state — they don't share it.

---

## 2. The Rules and Conventions

Custom hooks follow the same rules as built-in hooks:

- Must be called at the top level (not inside conditions, loops, or nested functions)
- Must start with `use` (this is what lets the linter enforce hook rules)
- Can only be called from React components or other custom hooks

```jsx
// ✓ Called at top level of component
function Component() {
  const { data } = useStudentData(id);
}

// ✗ Called conditionally — breaks hook rules
function Component({ isAdmin }) {
  if (isAdmin) {
    const { data } = useStudentData(id);  // error
  }
}
```

---

## 3. Extracting Logic into Hooks

The main motivation: extract complex logic from a component so it stays clean.

```jsx
// BEFORE — component is cluttered with implementation details
function StudentPage({ studentId }) {
  const [student, setStudent] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    let cancelled = false;
    setIsLoading(true);

    fetchStudent(studentId)
      .then(data => {
        if (!cancelled) {
          setStudent(data);
          setIsLoading(false);
        }
      })
      .catch(err => {
        if (!cancelled) {
          setError(err.message);
          setIsLoading(false);
        }
      });

    return () => { cancelled = true; };
  }, [studentId]);

  if (isLoading) return <Spinner />;
  if (error) return <ErrorMessage message={error} />;
  return <StudentProfile student={student} />;
}

// AFTER — component is clean; all logic is in the hook
function StudentPage({ studentId }) {
  const { data: student, isLoading, error } = useStudentData(studentId);

  if (isLoading) return <Spinner />;
  if (error) return <ErrorMessage message={error} />;
  return <StudentProfile student={student} />;
}

// The hook holds all the complexity
function useStudentData(studentId) {
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    let cancelled = false;
    setIsLoading(true);
    setData(null);
    setError(null);

    fetchStudent(studentId)
      .then(data => {
        if (!cancelled) { setData(data); setIsLoading(false); }
      })
      .catch(err => {
        if (!cancelled) { setError(err.message); setIsLoading(false); }
      });

    return () => { cancelled = true; };
  }, [studentId]);

  return { data, isLoading, error };
}
```

---

## 4. Data Fetching Hooks

### Generic fetch hook

```jsx
function useFetch(url) {
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    if (!url) return;

    const controller = new AbortController();
    setIsLoading(true);
    setError(null);

    fetch(url, { signal: controller.signal })
      .then(res => {
        if (!res.ok) throw new Error(`HTTP ${res.status}`);
        return res.json();
      })
      .then(data => {
        setData(data);
        setIsLoading(false);
      })
      .catch(err => {
        if (err.name === "AbortError") return;
        setError(err.message);
        setIsLoading(false);
      });

    return () => controller.abort();
  }, [url]);

  return { data, isLoading, error };
}

// Usage
function StudentList() {
  const { data: students, isLoading, error } = useFetch("/api/students");
  // ...
}
```

### Paginated data hook

```jsx
function usePaginatedStudents({ grade, query, pageSize = 20 }) {
  const [page, setPage] = useState(1);
  const [students, setStudents] = useState([]);
  const [total, setTotal] = useState(0);
  const [isLoading, setIsLoading] = useState(true);

  // Reset to page 1 when filters change
  useEffect(() => {
    setPage(1);
    setStudents([]);
  }, [grade, query]);

  useEffect(() => {
    setIsLoading(true);

    fetchStudents({ grade, query, page, limit: pageSize })
      .then(({ data, total }) => {
        setStudents(data);
        setTotal(total);
        setIsLoading(false);
      });
  }, [grade, query, page, pageSize]);

  const totalPages = Math.ceil(total / pageSize);
  const hasNext = page < totalPages;
  const hasPrev = page > 1;

  return {
    students,
    isLoading,
    page,
    totalPages,
    total,
    hasNext,
    hasPrev,
    goToPage: setPage,
    nextPage: () => setPage(p => p + 1),
    prevPage: () => setPage(p => p - 1),
  };
}
```

---

## 5. UI State Hooks

### Toggle

```jsx
function useToggle(initial = false) {
  const [value, setValue] = useState(initial);

  const toggle  = useCallback(() => setValue(v => !v), []);
  const setTrue  = useCallback(() => setValue(true), []);
  const setFalse = useCallback(() => setValue(false), []);

  return [value, { toggle, setTrue, setFalse }];
}

// Usage
function Modal() {
  const [isOpen, { toggle, setFalse }] = useToggle(false);

  return (
    <>
      <button onClick={toggle}>Open Modal</button>
      {isOpen && <ModalContent onClose={setFalse} />}
    </>
  );
}
```

### Disclosure (open/close with trigger)

```jsx
function useDisclosure(initial = false) {
  const [isOpen, setIsOpen] = useState(initial);

  return {
    isOpen,
    open:   useCallback(() => setIsOpen(true), []),
    close:  useCallback(() => setIsOpen(false), []),
    toggle: useCallback(() => setIsOpen(v => !v), []),
  };
}

function DropdownMenu({ items }) {
  const { isOpen, open, close, toggle } = useDisclosure();

  return (
    <div>
      <button onClick={toggle}>Menu</button>
      {isOpen && (
        <ul>
          {items.map(item => (
            <li key={item.id} onClick={() => { item.action(); close(); }}>
              {item.label}
            </li>
          ))}
        </ul>
      )}
    </div>
  );
}
```

### Counter

```jsx
function useCounter(initial = 0, { min, max, step = 1 } = {}) {
  const [count, setCount] = useState(initial);

  const increment = useCallback(() => {
    setCount(c => max !== undefined ? Math.min(c + step, max) : c + step);
  }, [max, step]);

  const decrement = useCallback(() => {
    setCount(c => min !== undefined ? Math.max(c - step, min) : c - step);
  }, [min, step]);

  const reset = useCallback(() => setCount(initial), [initial]);
  const set   = useCallback((val) => {
    const clamped = Math.min(Math.max(val, min ?? -Infinity), max ?? Infinity);
    setCount(clamped);
  }, [min, max]);

  return { count, increment, decrement, reset, set };
}

function QuantityPicker({ max }) {
  const { count, increment, decrement } = useCounter(1, { min: 1, max });

  return (
    <div>
      <button onClick={decrement}>-</button>
      <span>{count}</span>
      <button onClick={increment}>+</button>
    </div>
  );
}
```

---

## 6. Browser API Hooks

### Local storage

```jsx
function useLocalStorage(key, initialValue) {
  const [value, setValue] = useState(() => {
    try {
      const item = localStorage.getItem(key);
      return item ? JSON.parse(item) : initialValue;
    } catch {
      return initialValue;
    }
  });

  const set = useCallback((newValue) => {
    try {
      const toStore = newValue instanceof Function ? newValue(value) : newValue;
      setValue(toStore);
      localStorage.setItem(key, JSON.stringify(toStore));
    } catch (err) {
      console.error("useLocalStorage: failed to save", err);
    }
  }, [key, value]);

  const remove = useCallback(() => {
    localStorage.removeItem(key);
    setValue(initialValue);
  }, [key, initialValue]);

  return [value, set, remove];
}

// Usage — same API as useState + persistence
const [theme, setTheme] = useLocalStorage("theme", "dark");
```

### Media query

```jsx
function useMediaQuery(query) {
  const [matches, setMatches] = useState(
    () => window.matchMedia(query).matches
  );

  useEffect(() => {
    const mql = window.matchMedia(query);
    const handler = (e) => setMatches(e.matches);

    mql.addEventListener("change", handler);
    return () => mql.removeEventListener("change", handler);
  }, [query]);

  return matches;
}

function NavBar() {
  const isMobile = useMediaQuery("(max-width: 768px)");

  return isMobile ? <MobileNav /> : <DesktopNav />;
}
```

### Clipboard

```jsx
function useClipboard(timeout = 2000) {
  const [copied, setCopied] = useState(false);
  const timerRef = useRef(null);

  const copy = useCallback(async (text) => {
    try {
      await navigator.clipboard.writeText(text);
      setCopied(true);
      clearTimeout(timerRef.current);
      timerRef.current = setTimeout(() => setCopied(false), timeout);
    } catch (err) {
      console.error("Copy failed:", err);
    }
  }, [timeout]);

  useEffect(() => () => clearTimeout(timerRef.current), []);

  return { copied, copy };
}

function ShareButton({ url }) {
  const { copied, copy } = useClipboard();

  return (
    <button onClick={() => copy(url)}>
      {copied ? "Copied!" : "Copy link"}
    </button>
  );
}
```

### Online status

```jsx
function useOnlineStatus() {
  const [isOnline, setIsOnline] = useState(navigator.onLine);

  useEffect(() => {
    const on  = () => setIsOnline(true);
    const off = () => setIsOnline(false);
    window.addEventListener("online",  on);
    window.addEventListener("offline", off);
    return () => {
      window.removeEventListener("online",  on);
      window.removeEventListener("offline", off);
    };
  }, []);

  return isOnline;
}
```

---

## 7. Form Hooks

### Single field with validation

```jsx
function useField(initialValue = "", validate) {
  const [value, setValue] = useState(initialValue);
  const [error, setError] = useState("");
  const [touched, setTouched] = useState(false);

  const handleChange = useCallback((e) => {
    const newValue = e.target.value;
    setValue(newValue);
    if (touched && validate) {
      setError(validate(newValue) ?? "");
    }
  }, [touched, validate]);

  const handleBlur = useCallback(() => {
    setTouched(true);
    if (validate) setError(validate(value) ?? "");
  }, [value, validate]);

  const reset = useCallback(() => {
    setValue(initialValue);
    setError("");
    setTouched(false);
  }, [initialValue]);

  return {
    value,
    error: touched ? error : "",
    isValid: !error,
    inputProps: {
      value,
      onChange: handleChange,
      onBlur: handleBlur,
    },
    reset,
  };
}

// Usage
function LoginForm() {
  const email = useField("", v => {
    if (!v) return "Email is required";
    if (!v.includes("@")) return "Invalid email";
    return null;
  });

  const password = useField("", v => {
    if (!v) return "Password is required";
    if (v.length < 8) return "At least 8 characters";
    return null;
  });

  const canSubmit = email.isValid && password.isValid;

  return (
    <form>
      <input {...email.inputProps} type="email" />
      {email.error && <p>{email.error}</p>}

      <input {...password.inputProps} type="password" />
      {password.error && <p>{password.error}</p>}

      <button type="submit" disabled={!canSubmit}>Login</button>
    </form>
  );
}
```

---

## 8. Composition — Hooks That Use Hooks

```jsx
// A higher-level hook that composes simpler ones
function useStudentSearch() {
  const [query, setQuery] = useState("");
  const [grade, setGrade] = useState(null);
  const debouncedQuery = useDebounce(query, 300);  // custom hook
  const isOnline = useOnlineStatus();               // custom hook

  const { data: students, isLoading, error } = useFetch(
    isOnline
      ? `/api/students?query=${debouncedQuery}&grade=${grade ?? ""}`
      : null  // don't fetch when offline
  );

  return {
    query,
    setQuery,
    grade,
    setGrade,
    students: students ?? [],
    isLoading,
    error,
    isOnline,
  };
}

function useDebounce(value, delay) {
  const [debouncedValue, setDebouncedValue] = useState(value);

  useEffect(() => {
    const timer = setTimeout(() => setDebouncedValue(value), delay);
    return () => clearTimeout(timer);
  }, [value, delay]);

  return debouncedValue;
}

// The component is remarkably clean
function StudentSearchPage() {
  const {
    query, setQuery,
    grade, setGrade,
    students, isLoading, error, isOnline,
  } = useStudentSearch();

  return (
    <div>
      {!isOnline && <OfflineBanner />}
      <SearchBar query={query} onQueryChange={setQuery} />
      <GradeFilter grade={grade} onChange={setGrade} />
      {isLoading ? <Spinner /> : <StudentGrid students={students} />}
    </div>
  );
}
```

---

## 9. Hook Design Guidelines

### Return an object for multiple values

```jsx
// ✗ Array return — order-dependent, hard to destructure selectively
return [data, isLoading, error, refetch];

// ✓ Object return — named, selective destructuring
return { data, isLoading, error, refetch };

// Exception: two-value pairs (like useState) are fine as arrays
return [value, setValue];  // ✓ mirrors useState convention
```

### Reset and refetch capabilities

```jsx
function useStudentData(id) {
  const [data, setData] = useState(null);
  const [version, setVersion] = useState(0);  // bump to trigger refetch

  useEffect(() => {
    fetchStudent(id).then(setData);
  }, [id, version]);

  return {
    data,
    refetch: () => setVersion(v => v + 1),  // expose refetch
  };
}
```

### Error boundary for async operations

```jsx
// Hooks that fetch data should always expose error state
function useData(fetchFn, deps) {
  const [state, setState] = useState({
    data: null,
    isLoading: true,
    error: null,
  });

  // ...

  return state;  // always expose all three
}
```

### Don't abstract too early

If a hook is used in only one component and has no chance of being reused, keep the logic in the component. Only extract when there's a clear reuse case or the component becomes genuinely hard to understand.

---

## Summary

```
What custom hooks are:
  Functions starting with "use" that call other hooks
  Extract stateful logic from components — not just JSX
  Each usage creates independent state

Why extract:
  Make components clean and declarative
  Reuse stateful logic across multiple components
  Test logic independently from UI

Common categories:
  Data fetching:   useFetch, usePaginatedData, useStudentData
  UI state:        useToggle, useDisclosure, useCounter
  Browser APIs:    useLocalStorage, useMediaQuery, useOnlineStatus
  Forms:           useField, useForm (or use React Hook Form)

Composition:
  Hooks can use other hooks
  Build complex behaviour from simple, focused hooks
  Component calls one high-level hook; hook calls several simpler ones

Design guidelines:
  Return object for multiple values (not array)
  Two-value pairs (like useState) can stay as arrays
  Always expose: data, isLoading, error for async hooks
  Expose refetch/reset when data can change
  Don't abstract too early — only when reuse is real

Rules:
  Must start with "use"
  Must be called at top level (same rules as built-in hooks)
  Can be called from components or other custom hooks only
```

---

_Next: [14 — React 19 New Features](./14%20-%20React%2019%20New%20Features.md)_