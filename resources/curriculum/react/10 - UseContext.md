> Props work well for passing data through one or two levels. But when data needs to reach deeply nested components, prop drilling turns every intermediate component into an unwilling messenger. Context solves this by making data available anywhere in a component tree without explicit prop passing.

---

## Table of Contents

1. [The Prop Drilling Problem](#1-the-prop-drilling-problem)
2. [Creating and Using Context](#2-creating-and-using-context)
3. [Context with State — Dynamic Values](#3-context-with-state--dynamic-values)
4. [Multiple Contexts](#4-multiple-contexts)
5. [Context Performance — When Re-renders Happen](#5-context-performance--when-re-renders-happen)
6. [Optimising Context Performance](#6-optimising-context-performance)
7. [Real-World Context Patterns](#7-real-world-context-patterns)
8. [Context vs Zustand — When to Use Each](#8-context-vs-zustand--when-to-use-each)

---

## 1. The Prop Drilling Problem

Prop drilling is passing props through layers of components that don't use them — they just forward them down.

```jsx
// App needs to pass `user` to UserAvatar, which is 4 levels deep
function App() {
  const [user, setUser] = useState(currentUser);
  return <Layout user={user} />;      // Layout doesn't use user
}

function Layout({ user }) {
  return <Sidebar user={user} />;     // Sidebar doesn't use user
}

function Sidebar({ user }) {
  return <NavMenu user={user} />;     // NavMenu doesn't use user
}

function NavMenu({ user }) {
  return <UserAvatar user={user} />;  // FINALLY used here
}
```

Layout, Sidebar, and NavMenu all have to accept and forward `user` just to get it to `UserAvatar`. This is prop drilling — it creates coupling between components that shouldn't care about each other.

---

## 2. Creating and Using Context

```jsx
import { createContext, useContext } from "react";

// 1. Create the context with a default value
const ThemeContext = createContext("light");  // "light" is the default

// 2. Provide the context value — wrap the part of the tree that needs it
function App() {
  return (
    <ThemeContext.Provider value="dark">
      <Layout />
    </ThemeContext.Provider>
  );
}

// 3. Consume the context — anywhere inside the Provider
function Button({ children }) {
  const theme = useContext(ThemeContext);  // "dark"
  return (
    <button className={`btn btn-${theme}`}>
      {children}
    </button>
  );
}

// No props needed — Button can be anywhere inside the Provider
function Layout() {
  return (
    <div>
      <Sidebar />
      <Main />
    </div>
  );
}
```

**How `useContext` works:** It finds the nearest `ThemeContext.Provider` above the calling component in the tree and returns its `value`. If there's no Provider, it returns the default value from `createContext`.

---

## 3. Context with State — Dynamic Values

Context becomes truly useful when combined with state. The Provider holds state; consumers read and update it.

```jsx
// auth-context.jsx
import { createContext, useContext, useState, useEffect } from "react";

// 1. Create the context
const AuthContext = createContext(null);

// 2. Build a Provider component that manages the state
export function AuthProvider({ children }) {
  const [user, setUser] = useState(null);
  const [isLoading, setIsLoading] = useState(true);

  // Load persisted auth on mount
  useEffect(() => {
    const token = localStorage.getItem("token");
    if (token) {
      verifyToken(token)
        .then(user => setUser(user))
        .catch(() => localStorage.removeItem("token"))
        .finally(() => setIsLoading(false));
    } else {
      setIsLoading(false);
    }
  }, []);

  async function login(credentials) {
    const { user, token } = await loginApi(credentials);
    localStorage.setItem("token", token);
    setUser(user);
  }

  function logout() {
    localStorage.removeItem("token");
    setUser(null);
  }

  const value = { user, isLoading, login, logout };

  return (
    <AuthContext.Provider value={value}>
      {isLoading ? <FullPageSpinner /> : children}
    </AuthContext.Provider>
  );
}

// 3. Build a custom hook for consuming the context
export function useAuth() {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error("useAuth must be used inside AuthProvider");
  }
  return context;
}
```

```jsx
// main.jsx — wrap the app with the Provider
function App() {
  return (
    <AuthProvider>
      <Router>
        <Routes>
          <Route path="/" element={<HomePage />} />
          <Route path="/dashboard" element={<ProtectedRoute><Dashboard /></ProtectedRoute>} />
        </Routes>
      </Router>
    </AuthProvider>
  );
}
```

```jsx
// Any component in the tree can now access auth
function NavBar() {
  const { user, logout } = useAuth();

  return (
    <nav>
      {user ? (
        <>
          <span>Hello, {user.name}</span>
          <button onClick={logout}>Log out</button>
        </>
      ) : (
        <Link to="/login">Log in</Link>
      )}
    </nav>
  );
}

function ProtectedRoute({ children }) {
  const { user } = useAuth();
  return user ? children : <Navigate to="/login" />;
}
```

---

## 4. Multiple Contexts

Use separate contexts for separate concerns. Don't put everything in one giant context.

```jsx
// Each context has a single, focused responsibility
<AuthProvider>          {/* user, login, logout */}
  <ThemeProvider>       {/* theme, toggleTheme */}
    <ToastProvider>     {/* toasts, showToast, dismissToast */}
      <App />
    </ToastProvider>
  </ThemeProvider>
</AuthProvider>

// Consumers only subscribe to what they need
function ProfileButton() {
  const { user } = useAuth();     // subscribes to auth
  const { theme } = useTheme();   // subscribes to theme
  // doesn't subscribe to toasts — won't re-render on toast changes
}
```

---

## 5. Context Performance — When Re-renders Happen

**Every component that calls `useContext(MyContext)` re-renders whenever the context value changes** — even if the part of the value the component uses didn't change.

```jsx
const UserContext = createContext(null);

function App() {
  const [user, setUser] = useState({ name: "Ashan", role: "student" });
  const [lastActive, setLastActive] = useState(new Date());

  // This value object is recreated on every render
  // When lastActive updates, EVERY consumer re-renders
  return (
    <UserContext.Provider value={{ user, lastActive, setUser }}>
      <div>
        <UserName />          {/* re-renders when lastActive changes — unnecessary */}
        <UserRole />          {/* re-renders when lastActive changes — unnecessary */}
        <ActivityTracker />   {/* actually needs lastActive */}
      </div>
    </UserContext.Provider>
  );
}
```

---

## 6. Optimising Context Performance

### Split context by update frequency

```jsx
// Stable values (user object — changes rarely)
const UserContext = createContext(null);

// Frequently changing values (active status — changes every minute)
const ActivityContext = createContext(null);

function App() {
  const [user, setUser] = useState({ name: "Ashan" });
  const [lastActive, setLastActive] = useState(new Date());

  return (
    <UserContext.Provider value={user}>
      {/* UserName and UserRole only re-render when user changes */}
      <ActivityContext.Provider value={lastActive}>
        {/* ActivityTracker re-renders when lastActive changes */}
        <UserName />
        <UserRole />
        <ActivityTracker />
      </ActivityContext.Provider>
    </UserContext.Provider>
  );
}
```

### Memoize the context value

```jsx
import { useMemo } from "react";

function AuthProvider({ children }) {
  const [user, setUser] = useState(null);

  // Memoize: only creates a new object when user actually changes
  // Without useMemo: new object on every render → every consumer re-renders
  const value = useMemo(() => ({
    user,
    login: async (credentials) => {
      const loggedInUser = await loginApi(credentials);
      setUser(loggedInUser);
    },
    logout: () => setUser(null),
  }), [user]);  // only recalculates when user changes

  return (
    <AuthContext.Provider value={value}>
      {children}
    </AuthContext.Provider>
  );
}
```

### Separate state and dispatch (reducer pattern)

```jsx
// State changes → re-render state consumers
// Dispatch never changes → dispatch consumers don't re-render on state change
const StateContext  = createContext(null);
const DispatchContext = createContext(null);

function AppProvider({ children }) {
  const [state, dispatch] = useReducer(reducer, initialState);

  return (
    <DispatchContext.Provider value={dispatch}>
      {/* dispatch is stable — components that only dispatch never re-render */}
      <StateContext.Provider value={state}>
        {/* components that read state re-render on state change */}
        {children}
      </StateContext.Provider>
    </DispatchContext.Provider>
  );
}

// A component that only dispatches — won't re-render on state changes
function AddButton() {
  const dispatch = useContext(DispatchContext);
  return <button onClick={() => dispatch({ type: "ADD" })}>Add</button>;
}
```

---

## 7. Real-World Context Patterns

### Toast notification system

```jsx
// toast-context.jsx
const ToastContext = createContext(null);

export function ToastProvider({ children }) {
  const [toasts, setToasts] = useState([]);

  const showToast = useCallback((message, type = "info", duration = 4000) => {
    const id = crypto.randomUUID();
    setToasts(prev => [...prev, { id, message, type }]);
    setTimeout(() => {
      setToasts(prev => prev.filter(t => t.id !== id));
    }, duration);
  }, []);

  const dismissToast = useCallback((id) => {
    setToasts(prev => prev.filter(t => t.id !== id));
  }, []);

  const value = useMemo(() => ({ showToast, dismissToast }), [showToast, dismissToast]);

  return (
    <ToastContext.Provider value={value}>
      {children}
      <ToastContainer toasts={toasts} onDismiss={dismissToast} />
    </ToastContext.Provider>
  );
}

export function useToast() {
  const ctx = useContext(ToastContext);
  if (!ctx) throw new Error("useToast must be inside ToastProvider");
  return ctx;
}

// Usage anywhere in the app
function SaveButton({ onSave }) {
  const { showToast } = useToast();

  async function handleSave() {
    try {
      await onSave();
      showToast("Saved successfully!", "success");
    } catch {
      showToast("Failed to save. Please try again.", "error");
    }
  }

  return <button onClick={handleSave}>Save</button>;
}
```

### Theme system

```jsx
// theme-context.jsx
const ThemeContext = createContext(null);

export function ThemeProvider({ children }) {
  const [theme, setTheme] = useState(() => {
    return localStorage.getItem("theme") ?? "light";
  });

  useEffect(() => {
    document.documentElement.setAttribute("data-theme", theme);
    localStorage.setItem("theme", theme);
  }, [theme]);

  const toggleTheme = useCallback(() => {
    setTheme(t => t === "light" ? "dark" : "light");
  }, []);

  const value = useMemo(() => ({ theme, toggleTheme }), [theme, toggleTheme]);

  return (
    <ThemeContext.Provider value={value}>
      {children}
    </ThemeContext.Provider>
  );
}

export const useTheme = () => {
  const ctx = useContext(ThemeContext);
  if (!ctx) throw new Error("useTheme must be inside ThemeProvider");
  return ctx;
};
```

---

## 8. Context vs Zustand — When to Use Each

Context is built into React but has limitations at scale. Zustand (File 18) is a small external library that solves them.

```
Use Context when:
  - State is naturally scoped to a subtree (a modal, a form, a wizard)
  - State doesn't update very frequently
  - You don't want an external dependency
  - Examples: auth, theme, i18n, toast notifications

Use Zustand when:
  - State is truly global (needed by unrelated parts of the app)
  - State updates frequently (it has fine-grained subscriptions)
  - Multiple components update the state from different places
  - You need persistence, devtools, middleware
  - Examples: shopping cart, sidebar open/close, global filters
```

```jsx
// Context: fine for theme (rarely changes, tree-scoped)
const { theme } = useTheme();

// Zustand: better for notification count (updates constantly, needed everywhere)
const notifCount = useNotifStore(state => state.unreadCount);
// Only re-renders when unreadCount changes — not when other store state changes
```

The key Zustand advantage: **selector-based subscriptions**. Components only re-render when the specific slice of state they select changes. Context re-renders on any change to the context value.

---

## Summary

```
Problem context solves:
  Prop drilling — passing props through layers that don't use them

Three steps:
  1. createContext(defaultValue) — create the context
  2. <Context.Provider value={...}> — provide the value
  3. useContext(MyContext) — consume anywhere inside the Provider

Dynamic context:
  Combine with useState/useReducer in the Provider
  Custom hook (useAuth, useTheme) — validates usage and provides clean API

Multiple contexts:
  Separate context per concern — don't put everything in one
  Nesting Providers is normal and expected

Performance:
  Every useContext consumer re-renders when context value changes
  Solutions:
    - Split by update frequency (stable vs frequent)
    - Memoize value with useMemo
    - Separate state from dispatch contexts

Real-world patterns:
  Auth: user, login, logout, isLoading — most apps need this
  Theme: theme, toggleTheme — persisted to localStorage
  Toast: showToast, dismissToast — global notification system

Context vs Zustand:
  Context: subtree-scoped, infrequent updates, no dependencies
  Zustand: truly global, frequent updates, selector subscriptions
```

---

_Next: [11 — useReducer](./11%20-%20useReducer.md)_