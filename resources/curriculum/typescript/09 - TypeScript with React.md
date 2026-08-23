TypeScript and React are designed for each other. TypeScript catches prop mismatches, missing handlers, and incorrect hook usage at compile time — before the browser ever runs your code. This file covers every pattern you need to write fully typed React applications.

---

## Table of Contents

1. [Project Setup](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#1-project-setup)
2. [Typing Component Props](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#2-typing-component-props)
3. [Common Prop Types](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#3-common-prop-types)
4. [Children](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#4-children)
5. [Typing Hooks](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#5-typing-hooks)
6. [Event Handlers](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#6-event-handlers)
7. [Typing Context](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#7-typing-context)
8. [forwardRef](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#8-forwardref)
9. [Generic Components](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#9-generic-components)
10. [Discriminated Union Props](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#10-discriminated-union-props)
11. [Typing External Libraries](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#11-typing-external-libraries)
12. [Common Mistakes](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#12-common-mistakes)

---

## 1. Project Setup

```bash
# Vite — recommended
npm create vite@latest my-app -- --template react-ts
cd my-app && npm install

# Next.js
npx create-next-app@latest my-app --typescript
```

Key `tsconfig.json` settings for React:

```json
{
  "compilerOptions": {
    "target": "ES2020",
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "module": "ESNext",
    "moduleResolution": "Bundler",
    "jsx": "react-jsx", // no need to import React in every file
    "strict": true,
    "noUnusedLocals": true,
    "noUnusedParameters": true,
    "noImplicitReturns": true,
    "skipLibCheck": true
  }
}
```

---

## 2. Typing Component Props

### Function Components

```tsx
// Define props as a type or interface
type ButtonProps = {
  label: string;
  onClick: () => void;
  disabled?: boolean; // optional
  variant?: "primary" | "secondary" | "danger";
};

// Inline annotation
function Button({
  label,
  onClick,
  disabled = false,
  variant = "primary",
}: ButtonProps) {
  return (
    <button
      onClick={onClick}
      disabled={disabled}
      className={`btn btn-${variant}`}
    >
      {label}
    </button>
  );
}

// Arrow function
const Button = ({ label, onClick, disabled = false }: ButtonProps) => (
  <button onClick={onClick} disabled={disabled}>
    {label}
  </button>
);
```

### `type` vs `interface` for Props

```tsx
// type — preferred for props (more flexible, supports unions)
type CardProps = {
  title: string;
  body: string;
};

// interface — preferred when you need declaration merging or extending
interface ButtonProps {
  label: string;
}
interface IconButtonProps extends ButtonProps {
  icon: string;
}
```

Both work. Be consistent within a codebase.

---

## 3. Common Prop Types

```tsx
type ExampleProps = {
  // Primitives
  name: string;
  count: number;
  active: boolean;

  // Optional
  subtitle?: string;

  // Literal union
  size: "sm" | "md" | "lg";
  status: "idle" | "loading" | "success" | "error";

  // Arrays
  tags: string[];
  items: { id: string; label: string }[];

  // Objects
  user: { id: string; name: string; email: string };

  // Functions
  onClick: () => void;
  onChange: (value: string) => void;
  onSelect: (id: string, item: Item) => void;
  onSubmit: (data: FormData) => Promise<void>;

  // React elements
  icon: React.ReactNode; // anything React can render
  header: React.ReactElement; // specifically a JSX element
  as?: React.ElementType; // a component or HTML tag string

  // Style
  style?: React.CSSProperties;
  className?: string;

  // HTML element ref
  inputRef?: React.Ref<HTMLInputElement>;

  // Any (avoid where possible)
  data: unknown;
};
```

---

## 4. Children

```tsx
import { ReactNode, PropsWithChildren } from "react";

// Explicit children prop
type CardProps = {
  title: string;
  children: ReactNode;
};

function Card({ title, children }: CardProps) {
  return (
    <div className="card">
      <h2>{title}</h2>
      {children}
    </div>
  );
}

// PropsWithChildren utility — adds children?: ReactNode automatically
type CardProps = PropsWithChildren<{ title: string }>;

// ReactNode vs ReactElement
// ReactNode  — anything React can render: JSX, string, number, null, array
// ReactElement — specifically a JSX element (result of React.createElement)
// In practice, use ReactNode for children unless you need to call element methods
```

---

## 5. Typing Hooks

### `useState`

```tsx
// TypeScript infers the type from the initial value
const [count, setCount] = useState(0); // number
const [name, setName] = useState(""); // string
const [open, setOpen] = useState(false); // boolean

// Explicit type when initial value is null or ambiguous
const [user, setUser] = useState<User | null>(null);
const [items, setItems] = useState<string[]>([]);
const [data, setData] = useState<ApiResponse | undefined>(undefined);

// Object state
type FormState = { email: string; password: string };
const [form, setForm] = useState<FormState>({ email: "", password: "" });

// Partial updates with functional setter
setForm((prev) => ({ ...prev, email: "new@example.com" }));
```

### `useRef`

```tsx
// DOM element ref — initialise with null
const inputRef = useRef<HTMLInputElement>(null);
const divRef = useRef<HTMLDivElement>(null);
const videoRef = useRef<HTMLVideoElement>(null);

// Access the element
inputRef.current?.focus();
inputRef.current?.value;

// Mutable ref (not a DOM element — does not cause re-render)
const timerRef = useRef<ReturnType<typeof setTimeout> | null>(null);
const countRef = useRef<number>(0);

// Use in JSX
<input ref={inputRef} type="text" />;
```

### `useReducer`

```tsx
type State = {
  count: number;
  loading: boolean;
  error: string | null;
};

type Action =
  | { type: "INCREMENT" }
  | { type: "DECREMENT" }
  | { type: "SET_LOADING"; payload: boolean }
  | { type: "SET_ERROR"; payload: string };

const initialState: State = { count: 0, loading: false, error: null };

function reducer(state: State, action: Action): State {
  switch (action.type) {
    case "INCREMENT":
      return { ...state, count: state.count + 1 };
    case "DECREMENT":
      return { ...state, count: state.count - 1 };
    case "SET_LOADING":
      return { ...state, loading: action.payload };
    case "SET_ERROR":
      return { ...state, error: action.payload };
    default:
      return state;
  }
}

const [state, dispatch] = useReducer(reducer, initialState);
dispatch({ type: "INCREMENT" });
dispatch({ type: "SET_ERROR", payload: "Something went wrong" });
```

### `useCallback` and `useMemo`

```tsx
// TypeScript infers return type from the callback
const handleClick = useCallback((id: string) => {
  console.log(id);
}, []);

const sortedItems = useMemo(
  () => items.sort((a, b) => a.name.localeCompare(b.name)),
  [items],
);

// Explicit types when needed
const fetchUser = useCallback(async (id: string): Promise<User> => {
  return api.getUser(id);
}, []);
```

### Custom Hooks

```tsx
// Return type inferred — or annotate explicitly
function useToggle(initial = false): [boolean, () => void] {
  const [state, setState] = useState(initial);
  const toggle = useCallback(() => setState((s) => !s), []);
  return [state, toggle];
}

// Return an object for multiple values
function useLocalStorage<T>(key: string, initialValue: T) {
  const [value, setValue] = useState<T>(() => {
    const stored = localStorage.getItem(key);
    return stored ? (JSON.parse(stored) as T) : initialValue;
  });

  const set = useCallback(
    (newValue: T) => {
      setValue(newValue);
      localStorage.setItem(key, JSON.stringify(newValue));
    },
    [key],
  );

  return { value, set };
}

// Usage
const [open, toggleOpen] = useToggle(false);
const { value: theme, set: setTheme } = useLocalStorage("theme", "light");
```

---

## 6. Event Handlers

React provides typed event objects for every DOM event. The pattern is `React.EventName<HTMLElementType>`:

```tsx
// Input change
function handleChange(e: React.ChangeEvent<HTMLInputElement>) {
  console.log(e.target.value);
}

// Textarea change
function handleChange(e: React.ChangeEvent<HTMLTextAreaElement>) {}

// Select change
function handleChange(e: React.ChangeEvent<HTMLSelectElement>) {
  console.log(e.target.value);
}

// Click
function handleClick(e: React.MouseEvent<HTMLButtonElement>) {
  e.preventDefault();
}

// Form submit
function handleSubmit(e: React.FormEvent<HTMLFormElement>) {
  e.preventDefault();
  const formData = new FormData(e.currentTarget);
}

// Keyboard
function handleKeyDown(e: React.KeyboardEvent<HTMLInputElement>) {
  if (e.key === 'Enter') { ... }
}

// Drag
function handleDrop(e: React.DragEvent<HTMLDivElement>) {
  const files = e.dataTransfer.files;
}

// Focus
function handleFocus(e: React.FocusEvent<HTMLInputElement>) {}
```

### Typing Event Handlers as Props

```tsx
type InputProps = {
  onChange: React.ChangeEventHandler<HTMLInputElement>; // shorthand
  onKeyDown: React.KeyboardEventHandler<HTMLInputElement>;
  onClick: React.MouseEventHandler<HTMLButtonElement>;
};

// Equivalent to:
type InputProps = {
  onChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
  onKeyDown: (e: React.KeyboardEvent<HTMLInputElement>) => void;
  onClick: (e: React.MouseEvent<HTMLButtonElement>) => void;
};
```

---

## 7. Typing Context

```tsx
import { createContext, useContext, useState, ReactNode } from "react";

// 1. Define the context type
type ThemeContextType = {
  theme: "light" | "dark";
  toggle: () => void;
};

// 2. Create context with undefined as initial value
const ThemeContext = createContext<ThemeContextType | undefined>(undefined);

// 3. Custom hook with null check — never returns undefined
function useTheme(): ThemeContextType {
  const ctx = useContext(ThemeContext);
  if (!ctx) throw new Error("useTheme must be used inside ThemeProvider");
  return ctx;
}

// 4. Provider component
function ThemeProvider({ children }: { children: ReactNode }) {
  const [theme, setTheme] = useState<"light" | "dark">("light");
  const toggle = () => setTheme((t) => (t === "light" ? "dark" : "light"));

  return (
    <ThemeContext.Provider value={{ theme, toggle }}>
      {children}
    </ThemeContext.Provider>
  );
}

// 5. Usage
function Navbar() {
  const { theme, toggle } = useTheme();
  return <button onClick={toggle}>{theme}</button>;
}
```

---

## 8. `forwardRef`

Forward a ref from a parent to a DOM element inside a component:

```tsx
import { forwardRef, useImperativeHandle, useRef } from "react";

// Basic forwardRef — expose the DOM element directly
type InputProps = {
  label: string;
  placeholder?: string;
};

const Input = forwardRef<HTMLInputElement, InputProps>(
  ({ label, placeholder }, ref) => (
    <div>
      <label>{label}</label>
      <input ref={ref} placeholder={placeholder} />
    </div>
  ),
);

Input.displayName = "Input";

// Usage
function Form() {
  const inputRef = useRef<HTMLInputElement>(null);

  return (
    <>
      <Input ref={inputRef} label="Email" />
      <button onClick={() => inputRef.current?.focus()}>Focus</button>
    </>
  );
}
```

### `useImperativeHandle` — Expose a Custom API

Instead of exposing the raw DOM element, expose a controlled set of methods:

```tsx
type DialogHandle = {
  open: () => void;
  close: () => void;
};

type DialogProps = {
  title: string;
  children: ReactNode;
};

const Dialog = forwardRef<DialogHandle, DialogProps>(
  ({ title, children }, ref) => {
    const [isOpen, setIsOpen] = useState(false);

    useImperativeHandle(ref, () => ({
      open: () => setIsOpen(true),
      close: () => setIsOpen(false),
    }));

    if (!isOpen) return null;
    return (
      <div className="dialog">
        <h2>{title}</h2>
        {children}
      </div>
    );
  },
);

// Usage — parent controls dialog via ref methods, not state
function App() {
  const dialogRef = useRef<DialogHandle>(null);

  return (
    <>
      <button onClick={() => dialogRef.current?.open()}>Open</button>
      <Dialog ref={dialogRef} title="Confirm">
        Content
      </Dialog>
    </>
  );
}
```

---

## 9. Generic Components

Components that work with any data type:

```tsx
// Generic list component
type ListProps<T> = {
  items: T[];
  renderItem: (item: T, index: number) => ReactNode;
  keyExtractor: (item: T) => string;
};

function List<T>({ items, renderItem, keyExtractor }: ListProps<T>) {
  return (
    <ul>
      {items.map((item, i) => (
        <li key={keyExtractor(item)}>{renderItem(item, i)}</li>
      ))}
    </ul>
  );
}

// Usage — T is inferred from the items array
<List
  items={users}
  keyExtractor={(u) => u.id}
  renderItem={(u) => <span>{u.name}</span>}
/>;

// Generic select
type SelectProps<T> = {
  options: T[];
  value: T;
  onChange: (value: T) => void;
  getLabel: (option: T) => string;
  getValue: (option: T) => string;
};

function Select<T>({
  options,
  value,
  onChange,
  getLabel,
  getValue,
}: SelectProps<T>) {
  return (
    <select
      value={getValue(value)}
      onChange={(e) => {
        const selected = options.find((o) => getValue(o) === e.target.value);
        if (selected) onChange(selected);
      }}
    >
      {options.map((option) => (
        <option key={getValue(option)} value={getValue(option)}>
          {getLabel(option)}
        </option>
      ))}
    </select>
  );
}
```

---

## 10. Discriminated Union Props

Model components that behave differently based on a variant or mode:

```tsx
// Alert with different content per type
type AlertProps =
  | { type: "success"; message: string }
  | { type: "error"; message: string; retry?: () => void }
  | { type: "info"; message: string; link?: string };

function Alert(props: AlertProps) {
  if (props.type === "error") {
    // TypeScript knows retry is available here
    return (
      <div className="alert alert-error">
        {props.message}
        {props.retry && <button onClick={props.retry}>Retry</button>}
      </div>
    );
  }
  if (props.type === "info") {
    return (
      <div className="alert alert-info">
        {props.message}
        {props.link && <a href={props.link}>Learn more</a>}
      </div>
    );
  }
  return <div className="alert alert-success">{props.message}</div>;
}

// Button as link or button
type ButtonProps =
  | { as: "button"; onClick: () => void; href?: never }
  | { as: "link"; href: string; onClick?: never };

type CommonProps = { label: string; disabled?: boolean };

function Button({ label, disabled, ...props }: CommonProps & ButtonProps) {
  if (props.as === "link") {
    return <a href={props.href}>{label}</a>;
  }
  return (
    <button onClick={props.onClick} disabled={disabled}>
      {label}
    </button>
  );
}
```

---

## 11. Typing External Libraries

### Typing `react-router-dom`

```tsx
import { useParams, useNavigate, useLocation } from "react-router-dom";

// useParams — type the expected params
const { id, slug } = useParams<{ id: string; slug: string }>();

// useNavigate
const navigate = useNavigate();
navigate("/dashboard");
navigate(-1); // back

// useLocation — type the state
const location = useLocation();
const state = location.state as { from?: string } | null;
```

### Typing `react-hook-form`

```tsx
import { useForm, SubmitHandler } from "react-hook-form";

type LoginForm = {
  email: string;
  password: string;
  remember: boolean;
};

const {
  register,
  handleSubmit,
  formState: { errors },
} = useForm<LoginForm>();

const onSubmit: SubmitHandler<LoginForm> = (data) => {
  // data is fully typed as LoginForm
  console.log(data.email, data.password);
};
```

### Typing `zustand`

```tsx
import { create } from "zustand";

type AuthState = {
  user: User | null;
  token: string | null;
  login: (user: User, token: string) => void;
  logout: () => void;
};

const useAuthStore = create<AuthState>((set) => ({
  user: null,
  token: null,
  login: (user, token) => set({ user, token }),
  logout: () => set({ user: null, token: null }),
}));

// Usage — fully typed
const { user, login, logout } = useAuthStore();
```

---

## 12. Common Mistakes

```tsx
// WRONG — using FC<Props> (React.FC) adds implicit children and was problematic
const Button: React.FC<ButtonProps> = ({ label }) => <button>{label}</button>;

// RIGHT — just annotate the props directly
const Button = ({ label }: ButtonProps) => <button>{label}</button>;

// WRONG — typing children as JSX.Element (too narrow)
type Props = { children: JSX.Element }; // won't accept strings, arrays, null

// RIGHT — ReactNode accepts everything React can render
type Props = { children: React.ReactNode };

// WRONG — ignoring the event type
const handleChange = (e: any) => console.log(e.target.value);

// RIGHT
const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
  console.log(e.target.value);
};

// WRONG — useRef without the correct type
const ref = useRef(null); // type is MutableRefObject<null>
ref.current?.focus(); // error — null has no focus()

// RIGHT
const ref = useRef<HTMLInputElement>(null);
ref.current?.focus(); // fine

// WRONG — creating context with a default value that forces non-null assertion everywhere
const Ctx = createContext<UserContextType>({} as UserContextType);

// RIGHT — undefined default + null check in custom hook
const Ctx = createContext<UserContextType | undefined>(undefined);
function useUser() {
  const ctx = useContext(Ctx);
  if (!ctx) throw new Error("useUser must be inside UserProvider");
  return ctx;
}

// WRONG — spreading unknown props with no type safety
function Button({ ...props }: any) {
  return <button {...props} />;
}

// RIGHT — extend from the native element's props
type ButtonProps = React.ComponentPropsWithoutRef<"button"> & {
  variant?: "primary" | "secondary";
};

function Button({ variant = "primary", ...props }: ButtonProps) {
  return <button className={`btn-${variant}`} {...props} />;
}
```

---

_End of TypeScript module. Next: [React](https://claude.ai/react/00%20-%20React%20Mental%20Model.md)_
