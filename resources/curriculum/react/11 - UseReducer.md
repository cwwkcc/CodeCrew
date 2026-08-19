> `useReducer` is `useState` for complex state. When state has multiple sub-values that update together, or when the next state depends on the previous in non-trivial ways, `useReducer` makes the logic explicit, testable, and predictable.

---

## Table of Contents

1. [When useState Isn't Enough](#1-when-usestate-isnt-enough)
2. [useReducer — The Hook](#2-usereducer--the-hook)
3. [Actions and Action Creators](#3-actions-and-action-creators)
4. [Complex State Patterns](#4-complex-state-patterns)
5. [useReducer with useContext — Global State](#5-usereducer-with-usecontext--global-state)
6. [Comparing useState and useReducer](#6-comparing-usestate-and-usereducer)

---

## 1. When `useState` Isn't Enough

Multiple related state variables that always update together are a sign that `useReducer` would be cleaner:

```jsx
// Fragmented state — 5 separate useState calls for related state
function ShoppingCart() {
  const [items, setItems] = useState([]);
  const [total, setTotal] = useState(0);
  const [discountCode, setDiscountCode] = useState(null);
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [error, setError] = useState(null);

  function addItem(product) {
    setItems((prev) => [...prev, product]);
    setTotal((prev) => prev + product.price); // must update both in sync
    setError(null);
  }

  function applyDiscount(code) {
    if (validCodes.includes(code)) {
      setDiscountCode(code);
      setTotal((prev) => prev * 0.9); // must also update total
    } else {
      setError("Invalid discount code");
    }
  }

  // Logic is scattered — hard to trace what changes what
}
```

With `useReducer`, all state transitions are in one place:

```jsx
function cartReducer(state, action) {
  switch (action.type) {
    case "ADD_ITEM":
      return {
        ...state,
        items: [...state.items, action.product],
        total: state.total + action.product.price,
        error: null,
      };
    case "APPLY_DISCOUNT":
      if (!validCodes.includes(action.code)) {
        return { ...state, error: "Invalid discount code" };
      }
      return {
        ...state,
        discountCode: action.code,
        total: state.total * 0.9,
        error: null,
      };
    // ...
  }
}
```

---

## 2. `useReducer` — The Hook

```jsx
const [state, dispatch] = useReducer(reducer, initialState);
```

- `reducer` — a pure function: `(state, action) => newState`
- `initialState` — the starting state value
- `state` — current state
- `dispatch` — function that sends actions to the reducer

```jsx
import { useReducer } from "react";

// The reducer — defines all possible state transitions
function counterReducer(state, action) {
  switch (action.type) {
    case "INCREMENT":
      return { count: state.count + 1 };
    case "DECREMENT":
      return { count: state.count - 1 };
    case "RESET":
      return { count: 0 };
    case "SET":
      return { count: action.value };
    default:
      throw new Error(`Unknown action: ${action.type}`);
    // Throw on unknown actions — surfaces bugs immediately
  }
}

const initialState = { count: 0 };

function Counter() {
  const [state, dispatch] = useReducer(counterReducer, initialState);

  return (
    <div>
      <p>Count: {state.count}</p>
      <button onClick={() => dispatch({ type: "INCREMENT" })}>+</button>
      <button onClick={() => dispatch({ type: "DECREMENT" })}>-</button>
      <button onClick={() => dispatch({ type: "RESET" })}>Reset</button>
      <button onClick={() => dispatch({ type: "SET", value: 10 })}>
        Set to 10
      </button>
    </div>
  );
}
```

---

## 3. Actions and Action Creators

An **action** is a plain object with a `type` (what happened) and optional payload (data about what happened).

```jsx
// Action objects
{ type: "ADD_STUDENT" }
{ type: "REMOVE_STUDENT", id: "s123" }
{ type: "UPDATE_SCORE",   id: "s123", score: 91 }
{ type: "SET_FILTER",     filter: { grade: 11, query: "ash" } }

// Action creators — functions that return action objects
// Makes dispatch calls cleaner and less typo-prone
const addStudent    = (student) => ({ type: "ADD_STUDENT",   student });
const removeStudent = (id)      => ({ type: "REMOVE_STUDENT", id });
const updateScore   = (id, score) => ({ type: "UPDATE_SCORE", id, score });

// Usage
dispatch(addStudent({ id: "s1", name: "Ashan", grade: 11 }));
dispatch(removeStudent("s1"));
dispatch(updateScore("s1", 91));

// vs. inline (more error-prone)
dispatch({ type: "ADD_STUDENT", student: { id: "s1", name: "Ashan", grade: 11 } });
```

---

## 4. Complex State Patterns

### Form with validation

```jsx
const initialFormState = {
  values: { name: "", email: "", grade: "", message: "" },
  errors: {},
  touched: {},
  isSubmitting: false,
  isSubmitted: false,
};

function formReducer(state, action) {
  switch (action.type) {
    case "CHANGE":
      return {
        ...state,
        values: { ...state.values, [action.field]: action.value },
        errors: { ...state.errors, [action.field]: "" }, // clear error on change
      };

    case "BLUR":
      return {
        ...state,
        touched: { ...state.touched, [action.field]: true },
        errors: {
          ...state.errors,
          [action.field]: validateField(
            action.field,
            state.values[action.field],
          ),
        },
      };

    case "SUBMIT_START":
      return {
        ...state,
        isSubmitting: true,
        errors: validateAll(state.values),
        touched: Object.keys(state.values).reduce(
          (acc, k) => ({ ...acc, [k]: true }),
          {},
        ),
      };

    case "SUBMIT_SUCCESS":
      return { ...initialFormState, isSubmitted: true };

    case "SUBMIT_ERROR":
      return {
        ...state,
        isSubmitting: false,
        errors: { ...state.errors, form: action.error },
      };

    case "RESET":
      return initialFormState;

    default:
      return state;
  }
}

function ContactForm() {
  const [form, dispatch] = useReducer(formReducer, initialFormState);

  function handleChange(e) {
    dispatch({ type: "CHANGE", field: e.target.name, value: e.target.value });
  }

  function handleBlur(e) {
    dispatch({ type: "BLUR", field: e.target.name });
  }

  async function handleSubmit(e) {
    e.preventDefault();
    const nextState = formReducer(form, { type: "SUBMIT_START" });

    dispatch({ type: "SUBMIT_START" });

    if (Object.keys(nextState.errors).length > 0) return; // has validation errors

    try {
      await submitContact(form.values);
      dispatch({ type: "SUBMIT_SUCCESS" });
    } catch (err) {
      dispatch({ type: "SUBMIT_ERROR", error: err.message });
    }
  }

  if (form.isSubmitted) return <SuccessMessage />;

  return (
    <form onSubmit={handleSubmit}>
      <input
        name="name"
        value={form.values.name}
        onChange={handleChange}
        onBlur={handleBlur}
      />
      {form.touched.name && form.errors.name && (
        <p className="error">{form.errors.name}</p>
      )}
      {/* ... other fields */}
      <button type="submit" disabled={form.isSubmitting}>
        {form.isSubmitting ? "Sending..." : "Send"}
      </button>
    </form>
  );
}
```

### Data fetching state machine

```jsx
// Represents async state as an explicit state machine
// States: idle → loading → success | error
const initialState = { status: "idle", data: null, error: null };

function dataReducer(state, action) {
  switch (action.type) {
    case "FETCH_START":
      return { status: "loading", data: null, error: null };

    case "FETCH_SUCCESS":
      return { status: "success", data: action.data, error: null };

    case "FETCH_ERROR":
      return { status: "error", data: null, error: action.error };

    case "RESET":
      return initialState;

    default:
      return state;
  }
}

function useDataFetcher(fetchFn, deps) {
  const [state, dispatch] = useReducer(dataReducer, initialState);

  useEffect(() => {
    dispatch({ type: "FETCH_START" });

    fetchFn()
      .then((data) => dispatch({ type: "FETCH_SUCCESS", data }))
      .catch((err) => dispatch({ type: "FETCH_ERROR", error: err.message }));
  }, deps);

  return state;
}

// Usage
function StudentList() {
  const {
    status,
    data: students,
    error,
  } = useDataFetcher(() => fetchStudents(), []);

  if (status === "loading") return <Spinner />;
  if (status === "error") return <ErrorMessage message={error} />;
  if (status === "idle") return null;

  return students.map((s) => <StudentCard key={s.id} student={s} />);
}
```

### Multi-step wizard

```jsx
const initialWizardState = {
  step: 0,
  steps: ["Personal Info", "Academic Details", "Documents", "Review"],
  data: {
    personal: {},
    academic: {},
    documents: {},
  },
  isCompleted: false,
};

function wizardReducer(state, action) {
  switch (action.type) {
    case "NEXT":
      return {
        ...state,
        data: {
          ...state.data,
          [getStepKey(state.step)]: action.data,
        },
        step: Math.min(state.step + 1, state.steps.length - 1),
      };

    case "PREV":
      return { ...state, step: Math.max(state.step - 1, 0) };

    case "GOTO":
      return { ...state, step: action.step };

    case "COMPLETE":
      return { ...state, isCompleted: true };

    case "RESET":
      return initialWizardState;

    default:
      return state;
  }
}

function AdmissionsWizard() {
  const [wizard, dispatch] = useReducer(wizardReducer, initialWizardState);

  const StepComponents = [
    PersonalInfoStep,
    AcademicDetailsStep,
    DocumentsStep,
    ReviewStep,
  ];
  const CurrentStep = StepComponents[wizard.step];

  return (
    <div>
      <ProgressBar steps={wizard.steps} current={wizard.step} />
      <CurrentStep
        data={wizard.data[getStepKey(wizard.step)]}
        onNext={(data) => dispatch({ type: "NEXT", data })}
        onBack={() => dispatch({ type: "PREV" })}
      />
    </div>
  );
}
```

---

## 5. `useReducer` with `useContext` — Global State

The standard pattern for app-level state without external libraries.

```jsx
// store.jsx
import { createContext, useContext, useReducer, useMemo } from "react";

const initialState = {
  user: null,
  theme: "light",
  notifications: [],
  sidebar: { isOpen: true },
};

function appReducer(state, action) {
  switch (action.type) {
    case "SET_USER":
      return { ...state, user: action.user };

    case "TOGGLE_THEME":
      return { ...state, theme: state.theme === "light" ? "dark" : "light" };

    case "ADD_NOTIFICATION":
      return {
        ...state,
        notifications: [
          ...state.notifications,
          { id: crypto.randomUUID(), ...action.notification },
        ],
      };

    case "DISMISS_NOTIFICATION":
      return {
        ...state,
        notifications: state.notifications.filter((n) => n.id !== action.id),
      };

    case "TOGGLE_SIDEBAR":
      return {
        ...state,
        sidebar: { ...state.sidebar, isOpen: !state.sidebar.isOpen },
      };

    default:
      return state;
  }
}

// Separate contexts for state and dispatch to prevent unnecessary re-renders
const StateContext = createContext(null);
const DispatchContext = createContext(null);

export function AppProvider({ children }) {
  const [state, dispatch] = useReducer(appReducer, initialState);

  // dispatch is stable — never changes — no memo needed
  return (
    <DispatchContext.Provider value={dispatch}>
      <StateContext.Provider value={state}>{children}</StateContext.Provider>
    </DispatchContext.Provider>
  );
}

export function useAppState() {
  return useContext(StateContext);
}
export function useAppDispatch() {
  return useContext(DispatchContext);
}

// Convenience hooks for specific slices
export function useUser() {
  return useAppState().user;
}
export function useTheme() {
  return useAppState().theme;
}
export function useNotifications() {
  return useAppState().notifications;
}

// Action creator hooks
export function useThemeActions() {
  const dispatch = useAppDispatch();
  return {
    toggleTheme: () => dispatch({ type: "TOGGLE_THEME" }),
  };
}
```

```jsx
// Usage
function ThemeToggle() {
  const { theme } = useAppState(); // subscribes to all state changes
  const dispatch = useAppDispatch(); // NEVER re-renders (dispatch is stable)

  return (
    <button onClick={() => dispatch({ type: "TOGGLE_THEME" })}>
      Switch to {theme === "light" ? "dark" : "light"} mode
    </button>
  );
}
```

---

## 6. Comparing `useState` and `useReducer`

```
                    useState                 useReducer
────────────────────────────────────────────────────────────
Complexity          Simple                   Moderate
Best for            Independent values       Related state group
State transitions   Scattered in handlers    Centralised in reducer
Testability         Test components          Test reducer in isolation
Readability         Less boilerplate         More explicit
Next state          Can depend on prev       Always has access to full state
Action history      No                       Can be logged/debugged easily
```

```jsx
// Use useState when:
const [isOpen, setIsOpen] = useState(false);
const [name, setName] = useState("");
const [count, setCount] = useState(0);

// Use useReducer when:
// - State has multiple sub-values that change together
// - Next state depends on previous in complex ways
// - State transitions are business logic worth testing
// - You want to log/debug state changes

const [formState, dispatch] = useReducer(formReducer, initialFormState);
const [cartState, dispatch] = useReducer(cartReducer, initialCartState);
```

### Testing reducers

The best thing about `useReducer` — reducers are pure functions. Easy to test:

```jsx
// reducer.test.js — no React, no components needed
import { cartReducer, initialState } from "./cartReducer";

test("adding an item increases total", () => {
  const product = { id: "p1", name: "Book", price: 500 };
  const action = { type: "ADD_ITEM", product };

  const newState = cartReducer(initialState, action);

  expect(newState.items).toHaveLength(1);
  expect(newState.total).toBe(500);
});

test("invalid discount code shows error", () => {
  const action = { type: "APPLY_DISCOUNT", code: "INVALID" };
  const newState = cartReducer(initialState, action);

  expect(newState.error).toBe("Invalid discount code");
  expect(newState.discountCode).toBeNull();
});
```

---

## Summary

```
useReducer:
  const [state, dispatch] = useReducer(reducer, initialState)
  reducer: (state, action) => newState  — pure function
  dispatch({ type: "ACTION_TYPE", ...payload })

When to use over useState:
  Multiple related values that update together
  Complex state transitions with business logic
  State that depends on previous state in non-trivial ways
  State you want to test in isolation

Reducer rules:
  Must be pure — no side effects, no mutations
  Return new state — never mutate the existing state
  Throw on unknown actions — surfaces bugs

Action objects:
  { type: "ACTION_NAME" }               — no payload
  { type: "ACTION_NAME", data: value }  — with payload
  Action creators — functions that return action objects

Common patterns:
  Form state — values, errors, touched, submitting
  Async state machine — idle/loading/success/error
  Multi-step wizard — step, data per step

With Context:
  useReducer + useContext = global state without external libraries
  Separate StateContext from DispatchContext
  Dispatch is stable — components that only dispatch won't re-render on state changes

Testing:
  Reducers are pure functions — test without mounting any component
  Test each action type independently
```

---

_Next: [12 — useMemo & useCallback](./12%20-%20useMemo%20%26%20useCallback.md)_
