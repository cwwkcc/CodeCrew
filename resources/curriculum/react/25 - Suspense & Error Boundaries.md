> React's two safety nets for async and runtime failures. `<Suspense>` lets you declaratively describe loading states while async work is in progress. `<ErrorBoundary>` catches runtime errors in the component tree and renders a fallback instead of crashing the whole app. Together they make your UI resilient.

---

## Table of Contents

1. [The Problem They Solve](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-problem-they-solve)
2. [Suspense — Declarative Loading States](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-suspense--declarative-loading-states)
3. [Lazy Loading with Suspense](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-lazy-loading-with-suspense)
4. [Suspense with Data Fetching](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-suspense-with-data-fetching)
5. [Nested Suspense Boundaries](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-nested-suspense-boundaries)
6. [Error Boundaries](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-error-boundaries)
7. [useErrorBoundary Hook](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-useerrorboundary-hook)
8. [Combining Suspense and Error Boundaries](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-combining-suspense-and-error-boundaries)
9. [Patterns and Best Practices](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#9-patterns-and-best-practices)

---

## 1. The Problem They Solve

Without these tools, you handle async and errors imperatively — per component, manually:

```jsx
function StudentPage({ id }) {
  const [student, setStudent] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    setIsLoading(true);
    fetchStudent(id)
      .then(setStudent)
      .catch(setError)
      .finally(() => setIsLoading(false));
  }, [id]);

  if (isLoading) return <Spinner />;
  if (error)     return <ErrorMessage error={error} />;
  return <StudentProfile student={student} />;
}
```

This works, but it means every component that fetches data writes the same loading/error boilerplate. If a child component throws an unexpected runtime error — a null access, a failed JSON parse — the entire app crashes with a white screen.

`<Suspense>` and `<ErrorBoundary>` lift loading and error states _out_ of individual components and into the component tree, where they can be handled once, consistently.

```
Suspense:       handles the "loading" state declaratively
ErrorBoundary:  handles the "error" state declaratively (including unexpected crashes)
```

---

## 2. Suspense — Declarative Loading States

`<Suspense>` wraps a part of the tree. While any component inside is "suspended" (waiting for async work), Suspense renders the `fallback` instead. When the async work completes, the real component appears.

```jsx
import { Suspense } from "react";

function App() {
  return (
    <Suspense fallback={<Spinner />}>
      <StudentPage />
    </Suspense>
  );
}
```

The `fallback` can be anything — a spinner, a skeleton, a placeholder layout:

```jsx
// Minimal spinner
<Suspense fallback={<div className="flex justify-center p-8"><Spinner /></div>}>
  <HeavyComponent />
</Suspense>

// Skeleton that matches the real layout
<Suspense fallback={<StudentCardSkeleton />}>
  <StudentCard id={id} />
</Suspense>

// Nothing — useful when the content is non-critical
<Suspense fallback={null}>
  <SidebarWidget />
</Suspense>
```

A component "suspends" by throwing a Promise. React catches it, shows the fallback, and re-renders when the Promise resolves. You don't throw Promises manually — libraries like TanStack Query and Next.js handle this for you.

---

## 3. Lazy Loading with Suspense

The most common use of Suspense today: splitting your bundle and loading heavy components only when needed.

```jsx
import { lazy, Suspense } from "react";

// Instead of: import AdminPanel from "./AdminPanel"
const AdminPanel    = lazy(() => import("./AdminPanel"));
const ResultsChart  = lazy(() => import("./ResultsChart"));
const PanoramicView = lazy(() => import("./PanoramicView"));
```

`lazy()` takes a function that returns a dynamic `import()`. The component's JavaScript is only downloaded when the component is first rendered.

```jsx
function App() {
  const { user } = useAuth();

  return (
    <Routes>
      <Route path="/" element={<HomePage />} />

      {/* ResultsChart JS only loads when user visits /results */}
      <Route
        path="/results"
        element={
          <Suspense fallback={<ChartSkeleton />}>
            <ResultsChart />
          </Suspense>
        }
      />

      {/* Admin bundle never loads for non-admin users */}
      {user?.isAdmin && (
        <Route
          path="/admin"
          element={
            <Suspense fallback={<PageSpinner />}>
              <AdminPanel />
            </Suspense>
          }
        />
      )}
    </Routes>
  );
}
```

### Lazy loading at the layout level (Next.js)

```tsx
// In Next.js, use dynamic() instead of lazy()
import dynamic from "next/dynamic";

const PanoramicViewer = dynamic(() => import("@/components/PanoramicViewer"), {
  loading: () => <PanoramicSkeleton />,
  ssr: false,           // don't render on server (uses browser APIs)
});

const AudioPlayer = dynamic(() => import("@/components/AudioPlayer"), {
  loading: () => null,
  ssr: false,
});

export default function SchoolTourPage() {
  return (
    <div>
      <PanoramicViewer src="/tour/main-hall.jpg" />
      <AudioPlayer src="/audio/school-anthem.mp3" />
    </div>
  );
}
```

---

## 4. Suspense with Data Fetching

TanStack Query supports Suspense mode — components suspend while data is loading rather than returning `isLoading: true`.

```tsx
import { useSuspenseQuery } from "@tanstack/react-query";

// Without Suspense (standard)
function StudentList() {
  const { data, isLoading, error } = useQuery({
    queryKey: ["students"],
    queryFn: fetchStudents,
  });

  if (isLoading) return <Spinner />;
  if (error)     return <ErrorMessage />;
  return <StudentGrid students={data} />;
}

// With Suspense — loading and error handled by parent boundaries
function StudentList() {
  const { data } = useSuspenseQuery({
    queryKey: ["students"],
    queryFn: fetchStudents,
  });
  // data is guaranteed non-undefined here — component suspends until it's ready
  return <StudentGrid students={data} />;
}

// Parent handles both states
function StudentsPage() {
  return (
    <ErrorBoundary fallback={<StudentsError />}>
      <Suspense fallback={<StudentGridSkeleton />}>
        <StudentList />
      </Suspense>
    </ErrorBoundary>
  );
}
```

**Why `useSuspenseQuery` is cleaner:**

- Component code only handles the success case — no `if (isLoading)` branching
- The component is a pure function of data: `data → JSX`
- Loading and error states are handled once, at the boundary, not in every component

---

## 5. Nested Suspense Boundaries

Multiple Suspense boundaries let different parts of the page load independently.

```jsx
function DashboardPage() {
  return (
    <div className="dashboard-grid">

      {/* Header loads fast — own boundary so it doesn't block */}
      <Suspense fallback={<HeaderSkeleton />}>
        <DashboardHeader />
      </Suspense>

      <div className="dashboard-main">
        {/* Stats and activity load separately */}
        <Suspense fallback={<StatCardsSkeleton />}>
          <StatsCards />
        </Suspense>

        <Suspense fallback={<ActivityFeedSkeleton />}>
          <ActivityFeed />
        </Suspense>
      </div>

      {/* Sidebar loads last, doesn't block the main content */}
      <Suspense fallback={<SidebarSkeleton />}>
        <Sidebar />
      </Suspense>

    </div>
  );
}
```

Without nested boundaries, a slow `<ActivityFeed />` would block the entire page from showing — even the fast `<StatsCards />`. With nested boundaries, each section is independent.

**The rule:** place Suspense boundaries where you want a seam in the loading experience. Coarser = simpler code. Finer = better perceived performance.

---

## 6. Error Boundaries

An Error Boundary is a class component that catches JavaScript errors anywhere in its child component tree and renders a fallback UI instead of crashing.

**Why class component?** Error boundaries require `componentDidCatch` and `getDerivedStateFromError` lifecycle methods — these don't have Hook equivalents yet (React 19 is working on this). In practice you write one once and never touch it again.

```tsx
import { Component, type ReactNode, type ErrorInfo } from "react";

interface Props {
  fallback: ReactNode | ((error: Error, reset: () => void) => ReactNode);
  children: ReactNode;
  onError?: (error: Error, info: ErrorInfo) => void;
}

interface State {
  hasError: boolean;
  error: Error | null;
}

class ErrorBoundary extends Component<Props, State> {
  state: State = { hasError: false, error: null };

  static getDerivedStateFromError(error: Error): State {
    // Called when a child throws — update state to trigger fallback render
    return { hasError: true, error };
  }

  componentDidCatch(error: Error, info: ErrorInfo) {
    // Log to your error monitoring service (Sentry, etc.)
    this.props.onError?.(error, info);
    console.error("ErrorBoundary caught:", error, info.componentStack);
  }

  reset = () => {
    this.setState({ hasError: false, error: null });
  };

  render() {
    if (this.state.hasError && this.state.error) {
      const { fallback } = this.props;
      return typeof fallback === "function"
        ? fallback(this.state.error, this.reset)
        : fallback;
    }
    return this.props.children;
  }
}

export default ErrorBoundary;
```

### Using it

```tsx
// Simple fallback
<ErrorBoundary fallback={<p>Something went wrong.</p>}>
  <StudentTable />
</ErrorBoundary>

// Fallback with reset button
<ErrorBoundary
  fallback={(error, reset) => (
    <div className="error-state">
      <p>Failed to load: {error.message}</p>
      <button onClick={reset}>Try again</button>
    </div>
  )}
>
  <ResultsChart />
</ErrorBoundary>

// With error logging
<ErrorBoundary
  fallback={<PageErrorFallback />}
  onError={(error, info) => {
    Sentry.captureException(error, { extra: { componentStack: info.componentStack } });
  }}
>
  <App />
</ErrorBoundary>
```

### What errors does it catch?

```
✅ Errors thrown during rendering
✅ Errors in lifecycle methods
✅ Errors in constructor of child components
✅ Errors in useEffect cleanup (in React 19+)

❌ Errors in event handlers — use try/catch in the handler itself
❌ Errors in async code (setTimeout, fetch callbacks) — use try/catch
❌ Errors in the boundary component itself
❌ Server-side rendering errors
```

```jsx
// Event handler errors — catch manually
function DeleteButton({ id }) {
  const [error, setError] = useState<Error | null>(null);

  async function handleDelete() {
    try {
      await deleteStudent(id);
    } catch (err) {
      setError(err as Error);  // manage in component state
    }
  }

  if (error) return <p className="text-destructive">{error.message}</p>;
  return <button onClick={handleDelete}>Delete</button>;
}
```

### react-error-boundary (recommended in practice)

The `react-error-boundary` package gives you a production-ready `ErrorBoundary` component so you don't have to maintain the class yourself.

```bash
npm install react-error-boundary
```

```tsx
import { ErrorBoundary, useErrorBoundary } from "react-error-boundary";

// Simple use
<ErrorBoundary fallbackRender={({ error, resetErrorBoundary }) => (
  <div>
    <p>Error: {error.message}</p>
    <button onClick={resetErrorBoundary}>Try again</button>
  </div>
)}>
  <StudentTable />
</ErrorBoundary>

// With key for automatic reset when a dependency changes
<ErrorBoundary
  key={studentId}           // boundary resets when studentId changes
  fallbackRender={ErrorFallback}
>
  <StudentDetail id={studentId} />
</ErrorBoundary>
```

---

## 7. useErrorBoundary Hook

`react-error-boundary` exports `useErrorBoundary` — lets you manually trigger an error boundary from inside a child component. Useful for catching async errors and routing them to the nearest boundary.

```tsx
import { useErrorBoundary } from "react-error-boundary";

function StudentDetail({ id }: { id: string }) {
  const { showBoundary } = useErrorBoundary();
  const [student, setStudent] = useState<Student | null>(null);

  useEffect(() => {
    fetchStudent(id)
      .then(setStudent)
      .catch(showBoundary);  // async error → nearest ErrorBoundary
  }, [id, showBoundary]);

  if (!student) return <Spinner />;
  return <StudentProfile student={student} />;
}
```

This bridges the gap between async errors (which boundaries can't catch natively) and the boundary mechanism.

---

## 8. Combining Suspense and Error Boundaries

In practice, they're almost always used together. Suspense handles "waiting", ErrorBoundary handles "failed".

```tsx
// A reusable wrapper that handles both
function AsyncBoundary({
  children,
  loadingFallback = <Spinner />,
  errorFallback,
}: {
  children: ReactNode;
  loadingFallback?: ReactNode;
  errorFallback?: ComponentType<FallbackProps>;
}) {
  return (
    <ErrorBoundary fallbackRender={errorFallback ?? DefaultErrorFallback}>
      <Suspense fallback={loadingFallback}>
        {children}
      </Suspense>
    </ErrorBoundary>
  );
}

// Usage — one component handles both concerns
<AsyncBoundary loadingFallback={<StudentsSkeleton />}>
  <StudentsPage />
</AsyncBoundary>

<AsyncBoundary
  loadingFallback={<ChartSkeleton />}
  errorFallback={({ error, resetErrorBoundary }) => (
    <ChartError message={error.message} onRetry={resetErrorBoundary} />
  )}
>
  <ResultsChart />
</AsyncBoundary>
```

### Real page structure (Nexus-style)

```tsx
// app/[locale]/students/page.tsx
export default function StudentsPage() {
  return (
    <main>
      <PageHeader title="Students" />

      {/* Independent sections — each loads and fails independently */}
      <AsyncBoundary loadingFallback={<FilterBarSkeleton />}>
        <StudentFilterBar />
      </AsyncBoundary>

      <AsyncBoundary
        loadingFallback={<StudentGridSkeleton count={12} />}
        errorFallback={StudentGridError}
      >
        <StudentGrid />
      </AsyncBoundary>

    </main>
  );
}
```

---

## 9. Patterns and Best Practices

### Place boundaries at meaningful seams

```jsx
// Too coarse — one spinner blocks the whole page
<Suspense fallback={<Spinner />}>
  <EntirePage />
</Suspense>

// Too fine — every component has its own spinner (jarring)
<Suspense fallback={<Spinner />}><Header /></Suspense>
<Suspense fallback={<Spinner />}><Nav /></Suspense>
<Suspense fallback={<Spinner />}><Footer /></Suspense>

// Right — seams at independently meaningful content areas
<Suspense fallback={<HeroSkeleton />}>
  <HeroSection />
</Suspense>
<Suspense fallback={<NewsFeedSkeleton />}>
  <NewsFeed />
</Suspense>
```

### Match skeleton to real layout

```jsx
// Bad — generic spinner in a card-grid context looks like a crash
<Suspense fallback={<div className="animate-spin" />}>
  <StudentGrid />
</Suspense>

// Good — skeleton matches what's about to appear
<Suspense fallback={
  <div className="grid grid-cols-3 gap-4">
    {Array.from({ length: 6 }).map((_, i) => (
      <StudentCardSkeleton key={i} />
    ))}
  </div>
}>
  <StudentGrid />
</Suspense>
```

### Error boundaries at every async seam

```jsx
// If there's a Suspense, there should be an ErrorBoundary wrapping it
// (or use AsyncBoundary which combines them)
<ErrorBoundary fallback={<StudentGridError />}>
  <Suspense fallback={<StudentGridSkeleton />}>
    <StudentGrid />
  </Suspense>
</ErrorBoundary>
```

### One root-level boundary as a last resort

```tsx
// In your root layout — catches anything that slips through
export default function RootLayout({ children }) {
  return (
    <html>
      <body>
        <ErrorBoundary fallbackRender={RootErrorFallback}>
          {children}
        </ErrorBoundary>
      </body>
    </html>
  );
}

function RootErrorFallback({ error, resetErrorBoundary }) {
  return (
    <div className="min-h-screen flex flex-col items-center justify-center gap-4">
      <h1 className="text-2xl font-semibold">Something went wrong</h1>
      <p className="text-muted-foreground">{error.message}</p>
      <button onClick={resetErrorBoundary} className="btn-primary">
        Reload
      </button>
    </div>
  );
}
```

---

## Summary

```
Suspense:
  Renders fallback while children are "suspended" (waiting for async work)
  Components suspend by throwing a Promise — libraries do this for you
  Use for: lazy(), useSuspenseQuery(), Next.js streaming

lazy() + Suspense:
  Split your bundle — component JS only loads when rendered
  React: lazy(() => import("./Component"))
  Next.js: dynamic(() => import("./Component"), { ssr: false })

Nested Suspense:
  Each boundary is independent — slow content doesn't block fast content
  Place at content-area seams, not around individual small components
  Match the fallback shape to the real layout (skeletons > spinners)

ErrorBoundary:
  Class component — catches render errors in child tree
  getDerivedStateFromError → triggers fallback render
  componentDidCatch → logging (Sentry etc.)
  Use react-error-boundary package in practice

What boundaries catch vs don't:
  ✅ Render errors, lifecycle errors, constructor errors
  ❌ Event handler errors, async errors (setTimeout/fetch)
  For async errors: useErrorBoundary().showBoundary(err)

Combining them:
  Always pair Suspense with ErrorBoundary — one for loading, one for failure
  AsyncBoundary wrapper combines both into one reusable component
  One root-level ErrorBoundary as a last resort catch-all
```

---

_Next: [26 — useImperativeHandle & forwardRef](https://claude.ai/chat/26%20-%20useImperativeHandle%20%26%20forwardRef.md)_