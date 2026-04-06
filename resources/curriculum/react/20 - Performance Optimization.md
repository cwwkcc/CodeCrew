> Performance in React is mostly about not doing unnecessary work. The most important skill is knowing how to measure before you optimise — most perceived performance problems are not what you think they are, and optimising the wrong thing wastes time while the real bottleneck remains.

---

## Table of Contents

1. [Measure First — Always](#1-measure-first--always)
2. [What Causes Slow React Apps](#2-what-causes-slow-react-apps)
3. [React.memo — Skipping Child Re-renders](#3-reactmemo--skipping-child-re-renders)
4. [useMemo and useCallback — Reference Stability](#4-usememo-and-usecallback--reference-stability)
5. [Code Splitting and Lazy Loading](#5-code-splitting-and-lazy-loading)
6. [Virtualisation — Long Lists](#6-virtualisation--long-lists)
7. [Bundle Size](#7-bundle-size)
8. [Image Optimisation](#8-image-optimisation)
9. [State Location — The Biggest Win](#9-state-location--the-biggest-win)
10. [React Profiler Workflow](#10-react-profiler-workflow)

---

## 1. Measure First — Always

The first rule of performance optimisation: **don't guess. Measure.**

Most React apps are fast enough without explicit optimisation. React's rendering is lightweight. The virtual DOM diffing is fast. Most "performance problems" are:

1. Network latency (not React's fault)
2. State in the wrong place (an architectural issue)
3. A single genuinely expensive computation (fix with useMemo)
4. A massive list rendered all at once (fix with virtualisation)

**Tools:**

- React DevTools Profiler — finds slow renders
- Chrome DevTools Performance tab — finds JavaScript bottlenecks
- Lighthouse — overall page performance score
- `why-did-you-render` library — logs unnecessary re-renders

```jsx
// Add this to your dev entry point to detect unnecessary re-renders
if (process.env.NODE_ENV === "development") {
  const { default: whyDidYouRender } = await import("@welldone-software/why-did-you-render");
  whyDidYouRender(React, { trackAllPureComponents: true });
}
```

---

## 2. What Causes Slow React Apps

```
Problem                              Solution
───────────────────────────────────────────────────────────────
State too high in the tree           Move state closer to where it's used
  (every state change re-renders     (state colocation)
  large subtrees)

Expensive computation on every       useMemo
  render (large filter/sort ops)

Stable function/object reference     useCallback / useMemo
  needed by memoised child

Rendering 1000+ items at once        Virtualisation

Large initial bundle                 Code splitting / lazy loading

Synchronous layout reads             useLayoutEffect or defer reads

Expensive context causing mass       Split context / use Zustand
  re-renders
```

---

## 3. `React.memo` — Skipping Child Re-renders

`React.memo` wraps a component. Before re-rendering, React checks if props changed (shallow comparison). If not, it skips the render entirely.

```jsx
import { memo } from "react";

// Without memo: re-renders every time parent re-renders
function StudentCard({ student, onDelete }) {
  console.log("StudentCard rendered:", student.name);
  return (
    <div>
      <h3>{student.name}</h3>
      <button onClick={() => onDelete(student.id)}>Delete</button>
    </div>
  );
}

// With memo: only re-renders when student or onDelete changes
const StudentCard = memo(function StudentCard({ student, onDelete }) {
  return (
    <div>
      <h3>{student.name}</h3>
      <button onClick={() => onDelete(student.id)}>Delete</button>
    </div>
  );
});

// IMPORTANT: memo only helps if props are actually stable
// If parent creates new function/object on every render, memo is wasted
function Parent() {
  const [count, setCount] = useState(0);

  // ✗ New function every render — memo comparison fails
  return <StudentCard onDelete={(id) => deleteStudent(id)} />;

  // ✓ Stable reference — memo works
  const handleDelete = useCallback((id) => deleteStudent(id), []);
  return <StudentCard onDelete={handleDelete} />;
}
```

### Custom comparison function

```jsx
const StudentCard = memo(
  function StudentCard({ student, onDelete }) { /* ... */ },
  (prevProps, nextProps) => {
    // Return true to skip re-render (props are equal)
    // Return false to re-render (props changed)
    return (
      prevProps.student.id    === nextProps.student.id &&
      prevProps.student.score === nextProps.student.score &&
      prevProps.onDelete      === nextProps.onDelete
    );
    // Only re-render if id, score, or onDelete changed — ignore other student properties
  }
);
```

---

## 4. `useMemo` and `useCallback` — Reference Stability

Already covered in depth in File 12. The performance summary:

```jsx
// useMemo: cache expensive computed values
const filteredAndSorted = useMemo(() =>
  students
    .filter(s => s.grade === grade)
    .sort((a, b) => b.score - a.score),
  [students, grade]
);

// useCallback: stable function reference for memoised children
const handleDelete = useCallback(
  (id) => setStudents(prev => prev.filter(s => s.id !== id)),
  []
);

// The rule: only add these when you've MEASURED a problem
// Adding them everywhere actually hurts performance (comparison overhead)
```

---

## 5. Code Splitting and Lazy Loading

Split your bundle so users only download the code they need.

```jsx
import { lazy, Suspense } from "react";

// Lazily import heavy components — downloaded only when needed
const AdminPanel     = lazy(() => import("./AdminPanel"));
const ReportViewer   = lazy(() => import("./ReportViewer"));
const ChartDashboard = lazy(() => import("./ChartDashboard"));

function App() {
  return (
    <Routes>
      <Route path="/" element={<HomePage />} />  {/* always downloaded */}
      <Route
        path="/admin"
        element={
          <Suspense fallback={<PageSpinner />}>
            <AdminPanel />   {/* downloaded only when /admin is visited */}
          </Suspense>
        }
      />
      <Route
        path="/reports"
        element={
          <Suspense fallback={<PageSpinner />}>
            <ReportViewer />
          </Suspense>
        }
      />
    </Routes>
  );
}
```

### Lazy load heavy components conditionally

```jsx
// Don't load the chart library until the user opens the chart tab
const HeavyChart = lazy(() => import("./HeavyChart"));

function Dashboard() {
  const [showChart, setShowChart] = useState(false);

  return (
    <div>
      <button onClick={() => setShowChart(true)}>Show Chart</button>
      {showChart && (
        <Suspense fallback={<Spinner />}>
          <HeavyChart />
        </Suspense>
      )}
    </div>
  );
}
```

### Next.js dynamic import

```jsx
// Next.js — same concept, different API
import dynamic from "next/dynamic";

const HeavyChart = dynamic(() => import("./HeavyChart"), {
  loading: () => <Spinner />,
  ssr: false,  // don't render on server (for browser-only libraries)
});
```

---

## 6. Virtualisation — Long Lists

Rendering 1,000 DOM nodes is slow. Virtualisation renders only the visible ones (~10-20), regardless of list size.

```bash
npm install @tanstack/react-virtual
```

```jsx
import { useVirtualizer } from "@tanstack/react-virtual";

function VirtualStudentList({ students }) {
  const parentRef = useRef(null);

  const virtualizer = useVirtualizer({
    count:              students.length,
    getScrollElement:   () => parentRef.current,
    estimateSize:       () => 80,   // estimated row height in px
    overscan:           5,          // render 5 extra rows above/below visible area
  });

  return (
    <div
      ref={parentRef}
      style={{ height: 600, overflow: "auto" }}
    >
      {/* Total height — keeps scroll bar proportional */}
      <div style={{ height: virtualizer.getTotalSize(), position: "relative" }}>
        {virtualizer.getVirtualItems().map(virtualRow => (
          <div
            key={virtualRow.key}
            style={{
              position: "absolute",
              top:    0,
              left:   0,
              width:  "100%",
              height: `${virtualRow.size}px`,
              transform: `translateY(${virtualRow.start}px)`,
            }}
          >
            <StudentRow student={students[virtualRow.index]} />
          </div>
        ))}
      </div>
    </div>
  );
}
// Renders ~10 rows regardless of whether students has 100 or 100,000 items
```

**When to virtualise:** lists of 100+ items where each item is more than a single text line. For 50 items of simple text, virtualisation adds complexity for no gain.

---

## 7. Bundle Size

Large dependencies are the most common cause of slow initial load.

```bash
# Analyse your bundle
npm install --save-dev webpack-bundle-analyzer
# or with Vite:
npm install --save-dev rollup-plugin-visualizer
```

```jsx
// vite.config.ts
import { visualizer } from "rollup-plugin-visualizer";

export default defineConfig({
  plugins: [
    react(),
    visualizer({ open: true }),  // opens bundle visualisation in browser
  ],
});
```

### Common bundle size fixes

```jsx
// ✗ Import everything from a library
import { format, addDays, subDays, startOfMonth, endOfMonth } from "date-fns";
// ✓ Same — date-fns supports tree shaking, this is fine with modern bundlers

// ✗ Import entire lodash
import _ from "lodash";
_.map(arr, fn);

// ✓ Import only what you use
import map from "lodash/map";

// ✗ Large moment.js (includes all locales — 67KB gzipped)
import moment from "moment";

// ✓ Use date-fns instead (tree-shakeable, no locale bloat)
import { format } from "date-fns";

// Check size before adding a dependency
// bundlephobia.com — shows size + download time before you install
```

---

## 8. Image Optimisation

Images are usually the largest assets on a page.

```jsx
// In Next.js — always use next/image
import Image from "next/image";

// ✗ Regular img tag
<img src="/hero.jpg" />

// ✓ next/image — automatic:
//   - WebP/AVIF conversion
//   - Responsive sizes
//   - Lazy loading
//   - No layout shift (width/height required)
<Image
  src="/hero.jpg"
  alt="School hero image"
  width={1200}
  height={600}
  priority  // above-the-fold image — load eagerly
/>

// For images below the fold — lazy loading is automatic
<Image
  src="/staff/teacher-silva.jpg"
  alt="Mr. Silva"
  width={200}
  height={200}
/>
```

---

## 9. State Location — The Biggest Win

Moving state closer to where it's used is often the highest-impact optimisation — and it requires zero libraries.

```jsx
// PROBLEM: filter state lives in App, causing the entire tree to re-render on every keystroke
function App() {
  const [query, setQuery] = useState("");
  return (
    <div>
      <Header />          {/* re-renders on every keystroke — unnecessary */}
      <Sidebar />         {/* re-renders on every keystroke — unnecessary */}
      <main>
        <FilterBar query={query} onQueryChange={setQuery} />
        <StudentList query={query} />
      </main>
      <Footer />          {/* re-renders on every keystroke — unnecessary */}
    </div>
  );
}

// SOLUTION: move state to the subtree that actually uses it
function App() {
  return (
    <div>
      <Header />          {/* never re-renders due to filter changes */}
      <Sidebar />
      <main>
        <StudentSearch />  {/* self-contained — owns its own filter state */}
      </main>
      <Footer />
    </div>
  );
}

function StudentSearch() {
  const [query, setQuery] = useState("");  // lives here now
  return (
    <>
      <FilterBar query={query} onQueryChange={setQuery} />
      <StudentList query={query} />
    </>
  );
  // Only StudentSearch and its children re-render on keystroke
}
```

---

## 10. React Profiler Workflow

```
1. Open React DevTools → Profiler tab
2. Click record, perform the slow action, stop recording
3. Look for "tall" bars — those are slow renders
4. Click a bar to see:
   - Which component rendered
   - Why it rendered (which prop/state changed)
   - How long it took
5. Fix the worst offenders:
   - Unexpected renders → add React.memo or move state
   - Slow renders → add useMemo for expensive computation
   - Re-renders on every parent render → useCallback for callbacks
6. Re-measure to confirm the fix worked
```

```jsx
// React's built-in Profiler component for custom measurement
import { Profiler } from "react";

function onRenderCallback(
  id,           // component tree identifier
  phase,        // "mount" or "update"
  actualDuration,  // time for this render
  baseDuration,    // estimated time without memoisation
  startTime,
  commitTime,
) {
  if (actualDuration > 16) {  // slower than 60fps threshold
    console.warn(`Slow render in ${id}: ${actualDuration}ms`);
  }
}

<Profiler id="StudentGrid" onRender={onRenderCallback}>
  <StudentGrid students={students} />
</Profiler>
```

---

## Summary

```
The golden rule: MEASURE BEFORE OPTIMISING
  React DevTools Profiler → find actual slow renders
  Don't guess — optimise what the data shows

Common causes and fixes:
  State too high → move state down (colocation)
  Expensive computation → useMemo
  Unstable references → useCallback + React.memo (must work together)
  Long lists → virtualisation (@tanstack/react-virtual)
  Large bundle → code splitting (lazy + Suspense)
  Large images → next/image or proper formats

React.memo:
  Skips render if props are shallowly equal
  Only helps if props are actually stable (useCallback for functions)
  Custom comparison function for fine-grained control

Code splitting:
  lazy(() => import("./Component")) + <Suspense fallback={...}>
  Route-level splitting is the highest impact
  Conditional rendering of heavy components

Virtualisation:
  Render only visible rows — O(visible) instead of O(total)
  Use for 100+ item lists with non-trivial item components

State colocation:
  State in the wrong place is the #1 cause of unnecessary re-renders
  Move state as close to where it's used as possible
  No library required — just refactoring

Bundle size:
  bundlephobia.com before adding any dependency
  Tree-shakeable libraries (date-fns) over monolithic ones (moment)
  Lazy load routes and heavy conditional components
```

---

_Next: [21 — React with TypeScript](./21%20-%20React%20with%20TypeScript.md)_