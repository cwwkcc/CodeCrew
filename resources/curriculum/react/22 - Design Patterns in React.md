> Design patterns in React are reusable solutions to recurring component design problems. Knowing them means you can recognise the shape of a problem and reach for the proven solution rather than inventing something bespoke every time.

---

## Table of Contents

1. [Compound Components](#1-compound-components)
2. [Render Props](#2-render-props)
3. [Higher-Order Components](#3-higher-order-components)
4. [Provider Pattern](#4-provider-pattern)
5. [Container / Presentational Split](#5-container--presentational-split)
6. [Controlled vs Uncontrolled Components](#6-controlled-vs-uncontrolled-components)
7. [Composition over Configuration](#7-composition-over-configuration)

---

## 1. Compound Components

**Problem:** A component needs multiple related sub-parts that share implicit state. Passing all the state through props to every sub-component is verbose and fragile.

**Solution:** The parent manages shared state via Context; sub-components consume it. They're used together like HTML's `<select>` and `<option>`.

```tsx
// Building a Select/Dropdown compound component
import { createContext, useContext, useState } from "react";

interface SelectContextType {
  value: string | null;
  onChange: (value: string) => void;
  isOpen: boolean;
  toggle: () => void;
  close: () => void;
}

const SelectContext = createContext<SelectContextType | null>(null);

function useSelectContext() {
  const ctx = useContext(SelectContext);
  if (!ctx) throw new Error("Select subcomponents must be inside <Select>");
  return ctx;
}

// Parent component — manages state, provides context
function Select({
  value,
  onChange,
  children,
}: {
  value: string | null;
  onChange: (value: string) => void;
  children: React.ReactNode;
}) {
  const [isOpen, setIsOpen] = useState(false);

  return (
    <SelectContext.Provider
      value={{
        value,
        onChange: (v) => {
          onChange(v);
          setIsOpen(false);
        },
        isOpen,
        toggle: () => setIsOpen((o) => !o),
        close: () => setIsOpen(false),
      }}
    >
      <div className="select-root">{children}</div>
    </SelectContext.Provider>
  );
}

// Sub-components — consume context
Select.Trigger = function SelectTrigger({
  children,
}: {
  children: React.ReactNode;
}) {
  const { toggle, isOpen } = useSelectContext();
  return (
    <button onClick={toggle} aria-expanded={isOpen}>
      {children}
      <span>{isOpen ? "▲" : "▼"}</span>
    </button>
  );
};

Select.Content = function SelectContent({
  children,
}: {
  children: React.ReactNode;
}) {
  const { isOpen } = useSelectContext();
  if (!isOpen) return null;
  return <div className="select-content">{children}</div>;
};

Select.Item = function SelectItem({
  value,
  children,
}: {
  value: string;
  children: React.ReactNode;
}) {
  const { value: selected, onChange } = useSelectContext();
  return (
    <button
      className={selected === value ? "item selected" : "item"}
      onClick={() => onChange(value)}
    >
      {children}
    </button>
  );
};

// Clean, expressive usage
function GradeSelector() {
  const [grade, setGrade] = useState<string | null>(null);

  return (
    <Select value={grade} onChange={setGrade}>
      <Select.Trigger>
        {grade ? `Grade ${grade}` : "Select grade"}
      </Select.Trigger>
      <Select.Content>
        {[6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
          <Select.Item key={g} value={String(g)}>
            Grade {g}
          </Select.Item>
        ))}
      </Select.Content>
    </Select>
  );
}
```

This pattern is used by: Radix UI, shadcn/ui, Headless UI, Reach UI. When you use `<Select.Trigger>` from shadcn, this is what's happening underneath.

---

## 2. Render Props

**Problem:** You want to share stateful logic with a component but the caller needs control over what gets rendered with that state.

**Solution:** The component receives a function as a prop (`renderX` or `children`) and calls it with the shared state.

```tsx
// DataFetcher — logic reuse with render props
interface DataFetcherProps<T> {
  url: string;
  renderLoading: () => React.ReactNode;
  renderError: (error: string) => React.ReactNode;
  renderData: (data: T) => React.ReactNode;
}

function DataFetcher<T>({
  url,
  renderLoading,
  renderError,
  renderData,
}: DataFetcherProps<T>) {
  const { data, isLoading, error } = useFetch<T>(url);

  if (isLoading) return <>{renderLoading()}</>;
  if (error) return <>{renderError(error)}</>;
  if (!data) return null;
  return <>{renderData(data)}</>;
}

// Usage — caller decides how to render each state
<DataFetcher<Student[]>
  url="/api/students"
  renderLoading={() => <StudentListSkeleton />}
  renderError={(err) => <RetryableError message={err} onRetry={refetch} />}
  renderData={(students) => <StudentGrid students={students} />}
/>;

// Children as render prop — common alternative
interface TooltipProps {
  content: string;
  children: (props: {
    ref: React.Ref<any>;
    onMouseEnter: () => void;
    onMouseLeave: () => void;
  }) => React.ReactNode;
}

function Tooltip({ content, children }: TooltipProps) {
  const [isVisible, setIsVisible] = useState(false);
  const triggerRef = useRef<HTMLElement>(null);

  return (
    <>
      {children({
        ref: triggerRef,
        onMouseEnter: () => setIsVisible(true),
        onMouseLeave: () => setIsVisible(false),
      })}
      {isVisible && (
        <TooltipPopover anchor={triggerRef.current} content={content} />
      )}
    </>
  );
}

<Tooltip content="Delete this student">
  {({ ref, onMouseEnter, onMouseLeave }) => (
    <button ref={ref} onMouseEnter={onMouseEnter} onMouseLeave={onMouseLeave}>
      Delete
    </button>
  )}
</Tooltip>;
```

**Note:** Custom hooks have largely replaced render props for logic reuse. Render props are still useful when the consumer needs full control over the rendered output for a specific part of the UI.

---

## 3. Higher-Order Components

**Problem:** You want to add shared behaviour (auth checking, logging, error boundary) to multiple components without modifying each one.

**Solution:** A function that takes a component and returns an enhanced component.

```tsx
// withAuth — add authentication check to any component
function withAuth<P extends object>(WrappedComponent: React.ComponentType<P>) {
  return function AuthenticatedComponent(props: P) {
    const { user, isLoading } = useAuth();

    if (isLoading) return <FullPageSpinner />;
    if (!user) return <Navigate to="/login" />;

    return <WrappedComponent {...props} />;
  };
}

// withErrorBoundary — wrap any component in an error boundary
function withErrorBoundary<P extends object>(
  WrappedComponent: React.ComponentType<P>,
  fallback: React.ReactNode = <GenericErrorMessage />,
) {
  return class extends React.Component<P, { hasError: boolean }> {
    state = { hasError: false };

    static getDerivedStateFromError() {
      return { hasError: true };
    }

    render() {
      if (this.state.hasError) return fallback;
      return <WrappedComponent {...this.props} />;
    }
  };
}

// Usage
const ProtectedDashboard = withAuth(Dashboard);
const SafeChart = withErrorBoundary(Chart, <ChartErrorFallback />);
```

**Note:** HOCs were dominant in the class component era. With hooks, custom hooks often express the same logic more clearly. HOCs are still useful for class-component-compatible logic (error boundaries must be class components).

---

## 4. Provider Pattern

**Problem:** Multiple components at different depths need access to the same data or behaviour.

**Solution:** A Provider component manages the state and makes it available via Context.

```tsx
// Already covered in depth in File 10 (useContext)
// The pattern here is about how to structure the Provider file

// feature/auth/index.ts — clean exports
export { AuthProvider } from "./AuthProvider";
export { useAuth } from "./useAuth";
export type { User, LoginCredentials } from "./types";

// AuthProvider.tsx
export function AuthProvider({ children }: { children: React.ReactNode }) {
  // All auth state and logic lives here
  const [user, setUser] = useState<User | null>(null);

  const value = useMemo(
    () => ({
      user,
      login: async (creds: LoginCredentials) => {
        /* ... */
      },
      logout: () => {
        /* ... */
      },
    }),
    [user],
  );

  return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>;
}

// useAuth.ts
export function useAuth() {
  const ctx = useContext(AuthContext);
  if (!ctx) throw new Error("useAuth must be used inside AuthProvider");
  return ctx;
}

// main.tsx — compose providers
function App() {
  return (
    <ThemeProvider>
      <AuthProvider>
        <QueryClientProvider client={queryClient}>
          <Router />
        </QueryClientProvider>
      </AuthProvider>
    </ThemeProvider>
  );
}
```

---

## 5. Container / Presentational Split

**Problem:** Components mix data fetching/logic with rendering. They're hard to test, reuse, and read.

**Solution:** Split into a "container" (knows about data) and a "presentational" component (knows about rendering).

```tsx
// Presentational — pure, testable, reusable
interface StudentGridProps {
  students: Student[];
  isLoading: boolean;
  error: string | null;
  onDelete: (id: string) => void;
  onEdit: (student: Student) => void;
  emptyMessage?: string;
}

function StudentGrid({
  students,
  isLoading,
  error,
  onDelete,
  onEdit,
  emptyMessage = "No students found",
}: StudentGridProps) {
  if (isLoading) return <GridSkeleton />;
  if (error) return <ErrorMessage message={error} />;
  if (students.length === 0) return <EmptyState message={emptyMessage} />;

  return (
    <div className="grid">
      {students.map((student) => (
        <StudentCard
          key={student.id}
          student={student}
          onDelete={() => onDelete(student.id)}
          onEdit={() => onEdit(student)}
        />
      ))}
    </div>
  );
}
// StudentGrid knows nothing about fetching, routing, or auth

// Container — knows about data, business logic, navigation
function StudentsPageContainer() {
  const {
    data: students,
    isLoading,
    error,
  } = useQuery({
    queryKey: ["students"],
    queryFn: fetchStudents,
  });

  const deleteMutation = useMutation({
    mutationFn: deleteStudent,
    onSuccess: () => queryClient.invalidateQueries({ queryKey: ["students"] }),
  });

  const navigate = useNavigate();

  return (
    <StudentGrid
      students={students ?? []}
      isLoading={isLoading}
      error={error?.message ?? null}
      onDelete={(id) => deleteMutation.mutate(id)}
      onEdit={(student) => navigate(`/students/${student.id}/edit`)}
    />
  );
}
```

**Modern take:** With hooks, the "container" is often just the component itself, with the fetching logic extracted to a custom hook. The important principle is keeping the presentational component pure and separately testable.

---

## 6. Controlled vs Uncontrolled Components

Already covered for forms in File 07. The broader pattern applies to any component with internal state that might need external control.

```tsx
// Uncontrolled Accordion — manages its own open/close state
function Accordion({ items }: { items: AccordionItem[] }) {
  const [openIndex, setOpenIndex] = useState<number | null>(null);

  return (
    <div>
      {items.map((item, i) => (
        <AccordionItem
          key={item.id}
          item={item}
          isOpen={openIndex === i}
          onToggle={() => setOpenIndex(openIndex === i ? null : i)}
        />
      ))}
    </div>
  );
}

// Controlled Accordion — caller controls open/close state
interface ControlledAccordionProps {
  items:     AccordionItem[];
  openIndex: number | null;
  onToggle:  (index: number | null) => void;
}

function ControlledAccordion({ items, openIndex, onToggle }: ControlledAccordionProps) {
  return (
    <div>
      {items.map((item, i) => (
        <AccordionItem
          key={item.id}
          item={item}
          isOpen={openIndex === i}
          onToggle={() => onToggle(openIndex === i ? null : i)}
        />
      ))}
    </div>
  );
}

// Best of both: support both controlled and uncontrolled
function SmartAccordion({
  items,
  openIndex: controlledIndex,
  onToggle,
}: {
  items:      AccordionItem[];
  openIndex?: number | null;   // optional — if provided, component is controlled
  onToggle?:  (index: number | null) => void;
}) {
  const [internalIndex, setInternalIndex] = useState<number | null>(null);

  const isControlled  = controlledIndex !== undefined;
  const currentIndex  = isControlled ? controlledIndex : internalIndex;

  function handleToggle(index: number) {
    const next = currentIndex === index ? null : index;
    if (!isControlled) setInternalIndex(next);
    onToggle?.(next);
  }

  return (/* ... */);
}
```

---

## 7. Composition over Configuration

**Problem:** A component grows to accept dozens of props to support every possible variation. Props like `showHeader`, `headerVariant`, `footerContent`, `leftContent` are a sign of this.

**Solution:** Accept composition via `children` or slot props instead of configuring every detail.

```tsx
// Configuration approach — prop explosion
interface CardProps {
  title: string;
  subtitle?: string;
  showAvatar?: boolean;
  avatarSrc?: string;
  footerText?: string;
  showFooterBorder?: boolean;
  actions?: Array<{ label: string; onClick: () => void }>;
  variant?: "default" | "compact" | "featured";
  // ... 10 more props
}

// Composition approach — flexible and extensible
interface CardProps {
  header?: React.ReactNode; // caller composes the header
  footer?: React.ReactNode; // caller composes the footer
  children: React.ReactNode;
  className?: string;
}

function Card({ header, footer, children, className }: CardProps) {
  return (
    <div className={`card ${className ?? ""}`}>
      {header && <div className="card-header">{header}</div>}
      <div className="card-body">{children}</div>
      {footer && <div className="card-footer">{footer}</div>}
    </div>
  );
}

// Caller composes exactly what they need
<Card
  header={
    <div className="flex items-center gap-3">
      <Avatar src={student.avatarUrl} />
      <div>
        <h3>{student.name}</h3>
        <p className="text-sm text-muted">Grade {student.grade}</p>
      </div>
    </div>
  }
  footer={
    <div className="flex gap-2">
      <Button variant="ghost" onClick={onEdit}>
        Edit
      </Button>
      <Button variant="danger" onClick={onDelete}>
        Delete
      </Button>
    </div>
  }
>
  <ScoreChart scores={student.scores} />
</Card>;
```

---

## Summary

```
Compound Components:
  Parent manages state via Context
  Sub-components consume it: Select.Trigger, Select.Item
  Used by Radix, shadcn, Headless UI
  Best for: related sub-parts that share state

Render Props:
  Share state by passing it to a function prop
  Caller controls what gets rendered with that state
  Best for: flexible rendering decisions, render-time logic
  Custom hooks often replace this for pure logic sharing

Higher-Order Components:
  Function that wraps a component and adds behaviour
  Still needed for class components (error boundaries)
  Custom hooks are often cleaner for functional components

Provider Pattern:
  Context + Provider component + custom hook
  Clean file structure: Provider.tsx, useX.ts, types.ts
  Compose multiple providers in main.tsx

Container / Presentational:
  Presentational: pure, accepts data as props, easily tested
  Container (or hook): knows about fetching, routing, mutations
  Key insight: keep presentational components ignorant of data sources

Controlled vs Uncontrolled:
  Uncontrolled: component manages its own state
  Controlled: caller provides state + onChange
  Smart: support both by checking if controlled prop is provided

Composition over Configuration:
  Accept children / slot props instead of boolean flags
  Caller composes what they need
  Component stays simple and extensible
```

---

_Next: [23 — Testing React Components](./23%20-%20Testing%20React%20Components.md)_
