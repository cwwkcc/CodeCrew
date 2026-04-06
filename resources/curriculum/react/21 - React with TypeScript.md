> TypeScript makes React code dramatically safer and more readable. Props are documented by their types. Hooks return typed values. Event handlers know what they're handling. This file covers the TypeScript patterns specific to React — not TypeScript fundamentals (see the TypeScript curriculum), but how to type the React-specific things you encounter every day in Project Nexus.

---

## Table of Contents

1. [Typing Component Props](#1-typing-component-props)
2. [Children Types](#2-children-types)
3. [useState with TypeScript](#3-usestate-with-typescript)
4. [useRef with TypeScript](#4-useref-with-typescript)
5. [Event Handlers](#5-event-handlers)
6. [Generic Components](#6-generic-components)
7. [Typing Custom Hooks](#7-typing-custom-hooks)
8. [forwardRef with TypeScript](#8-forwardref-with-typescript)
9. [Context with TypeScript](#9-context-with-typescript)
10. [Common Type Utilities for React](#10-common-type-utilities-for-react)

---

## 1. Typing Component Props

```tsx
// Interface — preferred for component props (extendable)
interface StudentCardProps {
  student: {
    id:       string;
    name:     string;
    grade:    number;
    score:    number;
    avatarUrl?: string;  // optional
  };
  onEdit:      (id: string) => void;
  onDelete:    (id: string) => Promise<void>;
  isSelected?: boolean;
  className?:  string;
}

function StudentCard({
  student,
  onEdit,
  onDelete,
  isSelected = false,
  className,
}: StudentCardProps) {
  return (
    <div className={`card ${isSelected ? "selected" : ""} ${className ?? ""}`}>
      <h3>{student.name}</h3>
      <button onClick={() => onEdit(student.id)}>Edit</button>
      <button onClick={() => onDelete(student.id)}>Delete</button>
    </div>
  );
}

// Type alias — useful for unions and computed types
type ButtonVariant = "primary" | "secondary" | "danger" | "ghost";
type ButtonSize    = "sm" | "md" | "lg";

interface ButtonProps {
  variant?:  ButtonVariant;
  size?:     ButtonSize;
  disabled?: boolean;
  isLoading?: boolean;
  onClick?:  () => void;
  type?:     "button" | "submit" | "reset";
  children:  React.ReactNode;
}
```

### Extending HTML element props

```tsx
// Extend native HTML props so your component accepts all standard attributes
interface InputProps extends React.InputHTMLAttributes<HTMLInputElement> {
  label:   string;
  error?:  string;
  helpText?: string;
}

function Input({ label, error, helpText, className, ...inputProps }: InputProps) {
  return (
    <div className="field">
      <label>{label}</label>
      <input
        className={`input ${error ? "input-error" : ""} ${className ?? ""}`}
        {...inputProps}  // passes type, value, onChange, placeholder, disabled, etc.
      />
      {error    && <p className="error">{error}</p>}
      {helpText && <p className="help">{helpText}</p>}
    </div>
  );
}

// Now Input accepts all standard input props + our custom ones
<Input
  label="Email"
  error={errors.email}
  type="email"
  value={email}
  onChange={e => setEmail(e.target.value)}
  autoComplete="email"
  disabled={isSubmitting}
/>
```

---

## 2. Children Types

```tsx
// React.ReactNode — most permissive (JSX, strings, numbers, null, arrays)
interface PanelProps {
  children: React.ReactNode;  // use this in most cases
}

// React.ReactElement — specifically a JSX element (not strings/numbers/null)
interface IconButtonProps {
  icon: React.ReactElement;
  label: string;
}
<IconButton icon={<StarIcon />} label="Favorite" />

// React.FC is mostly avoided now — just type the function directly
// ✗ Avoid:
const Button: React.FC<ButtonProps> = ({ children }) => <button>{children}</button>;

// ✓ Prefer:
function Button({ children }: ButtonProps) {
  return <button>{children}</button>;
}
```

---

## 3. `useState` with TypeScript

```tsx
// TypeScript infers the type from the initial value
const [count, setCount] = useState(0);          // number
const [name, setName]   = useState("");         // string
const [isOpen, setIsOpen] = useState(false);    // boolean

// When you need to be explicit (initial value is null or ambiguous)
const [user, setUser] = useState<User | null>(null);
const [students, setStudents] = useState<Student[]>([]);
const [selected, setSelected] = useState<string | null>(null);

// Union types — value can only be one of these
const [status, setStatus] = useState<"idle" | "loading" | "success" | "error">("idle");

// Object state
interface FormState {
  name:    string;
  email:   string;
  grade:   number;
}

const [form, setForm] = useState<FormState>({
  name:  "",
  email: "",
  grade: 11,
});

// Functional update — TypeScript infers prev type
setStudents(prev => [...prev, newStudent]);     // prev: Student[]
setForm(prev => ({ ...prev, grade: 12 }));     // prev: FormState
```

---

## 4. `useRef` with TypeScript

```tsx
// DOM ref — type matches the element type
const inputRef = useRef<HTMLInputElement>(null);
const divRef   = useRef<HTMLDivElement>(null);
const formRef  = useRef<HTMLFormElement>(null);
const videoRef = useRef<HTMLVideoElement>(null);

// After mount, inputRef.current is HTMLInputElement (not null)
// TypeScript doesn't know this — use optional chaining or non-null assertion
inputRef.current?.focus();       // ✓ optional chaining
inputRef.current!.focus();       // ✓ non-null assertion (only when you're sure it's mounted)

// Mutable ref — for non-DOM values
const timerRef     = useRef<ReturnType<typeof setTimeout> | null>(null);
const prevValueRef = useRef<string | undefined>(undefined);
const countRef     = useRef<number>(0);

// Non-null ref — when you know it will always be set before use
const containerRef = useRef<HTMLDivElement>(null!);  // non-null assertion on initial value
// Only use when you're certain — skips the null check
```

---

## 5. Event Handlers

```tsx
// The pattern: React.ChangeEvent<ElementType>
function handleChange(e: React.ChangeEvent<HTMLInputElement>) {
  const value = e.target.value;  // TypeScript knows this is string
}

function handleSelectChange(e: React.ChangeEvent<HTMLSelectElement>) {
  const value = e.target.value;
}

function handleTextareaChange(e: React.ChangeEvent<HTMLTextAreaElement>) {
  const value = e.target.value;
}

// Other event types
function handleClick(e: React.MouseEvent<HTMLButtonElement>) {
  e.currentTarget;  // typed as HTMLButtonElement
}

function handleSubmit(e: React.FormEvent<HTMLFormElement>) {
  e.preventDefault();
}

function handleKeyDown(e: React.KeyboardEvent<HTMLInputElement>) {
  e.key;      // "Enter" | "Escape" | etc.
  e.ctrlKey;  // boolean
}

function handleDrop(e: React.DragEvent<HTMLDivElement>) {
  e.dataTransfer.files;
}

// Inline — TypeScript infers the type automatically
<input onChange={e => setValue(e.target.value)} />
// e is inferred as React.ChangeEvent<HTMLInputElement>

// When typing a handler separately, be explicit
const handleChange: React.ChangeEventHandler<HTMLInputElement> = (e) => {
  setValue(e.target.value);
};
```

---

## 6. Generic Components

Components that work with different data types while staying fully typed.

```tsx
// Generic list — works with any item type that has an id
interface ListProps<T extends { id: string }> {
  items:       T[];
  renderItem:  (item: T) => React.ReactNode;
  keyExtractor?: (item: T) => string;
  emptyMessage?: string;
}

function List<T extends { id: string }>({
  items,
  renderItem,
  keyExtractor = (item) => item.id,
  emptyMessage = "No items",
}: ListProps<T>) {
  if (items.length === 0) return <p>{emptyMessage}</p>;

  return (
    <ul>
      {items.map(item => (
        <li key={keyExtractor(item)}>
          {renderItem(item)}
        </li>
      ))}
    </ul>
  );
}

// TypeScript infers T = Student from the items prop
<List
  items={students}
  renderItem={(student) => <StudentCard student={student} />}
  // student is typed as Student automatically
/>

// Generic select/combobox
interface SelectOption<T> {
  label: string;
  value: T;
}

interface SelectProps<T> {
  options:   SelectOption<T>[];
  value:     T | null;
  onChange:  (value: T) => void;
  placeholder?: string;
}

function Select<T>({ options, value, onChange, placeholder }: SelectProps<T>) {
  return (
    <select
      value={options.findIndex(o => o.value === value)}
      onChange={e => onChange(options[Number(e.target.value)].value)}
    >
      {placeholder && <option value={-1}>{placeholder}</option>}
      {options.map((opt, i) => (
        <option key={i} value={i}>{opt.label}</option>
      ))}
    </select>
  );
}

// T is inferred from the options + value type
<Select<number>
  options={[{ label: "Grade 11", value: 11 }, { label: "Grade 12", value: 12 }]}
  value={selectedGrade}
  onChange={setSelectedGrade}
/>
```

---

## 7. Typing Custom Hooks

```tsx
// Hook that returns multiple values — use a named object
function useStudentData(id: string) {
  const [student, setStudent] = useState<Student | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    fetchStudent(id)
      .then(setStudent)
      .catch(err => setError(err.message))
      .finally(() => setIsLoading(false));
  }, [id]);

  return { student, isLoading, error };
  // Return type is inferred: { student: Student | null; isLoading: boolean; error: string | null }
}

// Explicit return type (when the inference is complex or you want to document it)
interface UseStudentDataReturn {
  student:   Student | null;
  isLoading: boolean;
  error:     string | null;
  refetch:   () => void;
}

function useStudentData(id: string): UseStudentDataReturn { /* ... */ }

// Generic hook
function useLocalStorage<T>(key: string, initial: T): [T, (value: T) => void] {
  const [value, setValue] = useState<T>(() => {
    const stored = localStorage.getItem(key);
    return stored ? (JSON.parse(stored) as T) : initial;
  });

  const set = (newValue: T) => {
    setValue(newValue);
    localStorage.setItem(key, JSON.stringify(newValue));
  };

  return [value, set];
}

// Usage — T is inferred from initial value
const [theme, setTheme] = useLocalStorage("theme", "dark");  // T = string
const [count, setCount] = useLocalStorage("count", 0);       // T = number
```

---

## 8. `forwardRef` with TypeScript

```tsx
import { forwardRef } from "react";

// forwardRef<RefType, PropsType>
const Input = forwardRef<HTMLInputElement, InputProps>(
  function Input({ label, error, ...inputProps }, ref) {
    return (
      <div className="field">
        <label>{label}</label>
        <input ref={ref} {...inputProps} />
        {error && <p className="error">{error}</p>}
      </div>
    );
  }
);

// Usage
const inputRef = useRef<HTMLInputElement>(null);
<Input ref={inputRef} label="Email" type="email" />

// Custom imperative handle
interface VideoPlayerHandle {
  play:   () => void;
  pause:  () => void;
  seek:   (time: number) => void;
}

interface VideoPlayerProps {
  src: string;
}

const VideoPlayer = forwardRef<VideoPlayerHandle, VideoPlayerProps>(
  function VideoPlayer({ src }, ref) {
    const videoRef = useRef<HTMLVideoElement>(null);

    useImperativeHandle(ref, () => ({
      play:  () => videoRef.current?.play(),
      pause: () => videoRef.current?.pause(),
      seek:  (time) => { if (videoRef.current) videoRef.current.currentTime = time; },
    }));

    return <video ref={videoRef} src={src} />;
  }
);

const playerRef = useRef<VideoPlayerHandle>(null);
playerRef.current?.play();
```

---

## 9. Context with TypeScript

```tsx
import { createContext, useContext, useState } from "react";

// Define the context type
interface AuthContextType {
  user:    User | null;
  login:   (credentials: LoginCredentials) => Promise<void>;
  logout:  () => void;
  isLoading: boolean;
}

// Create with null — forces consumers to check they're inside Provider
const AuthContext = createContext<AuthContextType | null>(null);

// Provider component
export function AuthProvider({ children }: { children: React.ReactNode }) {
  const [user, setUser] = useState<User | null>(null);
  const [isLoading, setIsLoading] = useState(false);

  const login = async (credentials: LoginCredentials) => {
    setIsLoading(true);
    const loggedInUser = await loginApi(credentials);
    setUser(loggedInUser);
    setIsLoading(false);
  };

  const logout = () => setUser(null);

  return (
    <AuthContext.Provider value={{ user, login, logout, isLoading }}>
      {children}
    </AuthContext.Provider>
  );
}

// Custom hook — narrows the null away and throws if outside Provider
export function useAuth(): AuthContextType {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error("useAuth must be used inside AuthProvider");
  }
  return context;  // AuthContextType (not null) — TypeScript knows
}

// Usage
const { user, login, logout } = useAuth();
// All fully typed — user: User | null, login: (creds) => Promise<void>
```

---

## 10. Common Type Utilities for React

```tsx
// ComponentProps — extract props from a component
import { ComponentProps } from "react";

type ButtonProps = ComponentProps<"button">;   // all native button props
type DivProps    = ComponentProps<"div">;

// Extend native props with custom props
interface CardProps extends ComponentProps<"div"> {
  title: string;
  isHighlighted?: boolean;
}

// ComponentPropsWithoutRef / ComponentPropsWithRef
type InputPropsWithRef    = ComponentPropsWithRef<"input">;
type InputPropsWithoutRef = ComponentPropsWithoutRef<"input">;

// ElementRef — get the ref type for an element
import { ElementRef } from "react";
const ref = useRef<ElementRef<"canvas">>(null);  // HTMLCanvasElement
const ref2 = useRef<ElementRef<typeof Input>>(null);  // for custom components

// React.HTMLAttributes<T> — standard HTML attributes
interface SectionProps extends React.HTMLAttributes<HTMLElement> {
  as?: keyof JSX.IntrinsicElements;  // "div" | "section" | "article" | etc.
}

// Discriminated union props — mutually exclusive prop sets
type ButtonProps =
  | { variant: "link"; href: string; onClick?: never }
  | { variant: "button"; href?: never; onClick: () => void };

// PropsWithChildren — adds children to any props type
import { PropsWithChildren } from "react";
type CardProps = PropsWithChildren<{ title: string }>;
// { title: string; children?: ReactNode }
```

---

## Summary

```
Component props:
  interface Props { ... } — preferred (extendable)
  Extend HTML props: interface InputProps extends React.InputHTMLAttributes<HTMLInputElement>
  Spread with {...inputProps} to pass through all native attributes

Children:
  React.ReactNode — most permissive (use by default)
  React.ReactElement — specifically JSX elements

useState:
  TypeScript infers from initial value
  useState<User | null>(null) — explicit when initial is null or ambiguous

useRef:
  useRef<HTMLInputElement>(null) — DOM refs
  Use optional chaining: ref.current?.method() (TypeScript sees null)

Events:
  React.ChangeEvent<HTMLInputElement>
  React.MouseEvent<HTMLButtonElement>
  React.FormEvent<HTMLFormElement>
  React.KeyboardEvent<HTMLInputElement>
  Inline handlers: TypeScript infers the type automatically

Generic components:
  function List<T extends { id: string }>({ items }: ListProps<T>) {}
  T is inferred from the items prop at usage

Custom hooks:
  Return named object — { data, isLoading, error }
  Generic hooks: function useStorage<T>(key: string, initial: T)
  Explicit return type for complex hooks

forwardRef:
  forwardRef<RefType, PropsType>(function Comp(props, ref) {})
  useImperativeHandle with typed handle interface

Context:
  createContext<Type | null>(null)
  Custom hook throws if null — narrows to Type at usage point

Utilities:
  ComponentProps<"button"> — native element props
  ComponentProps<typeof MyComp> — custom component props
  PropsWithChildren<T> — adds children?: ReactNode
```

---

_Next: [22 — Design Patterns in React](./22%20-%20Design%20Patterns%20in%20React.md)_