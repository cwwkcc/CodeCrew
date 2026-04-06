> Data fetching with `useEffect` works but doesn't scale — you end up re-implementing caching, deduplication, background refetching, and error handling in every component. TanStack Query (formerly React Query) is the standard solution: a full server state management library that handles all of this correctly by default.

---

## Table of Contents

1. [The Problem with useEffect Fetching](#1-the-problem-with-useeffect-fetching)
2. [Server State vs Client State](#2-server-state-vs-client-state)
3. [Setup](#3-setup)
4. [useQuery — The Core Hook](#4-usequery--the-core-hook)
5. [Query Keys](#5-query-keys)
6. [useMutation — Modifying Data](#6-usemutation--modifying-data)
7. [Cache Invalidation](#7-cache-invalidation)
8. [Optimistic Updates](#8-optimistic-updates)
9. [Pagination and Infinite Scroll](#9-pagination-and-infinite-scroll)
10. [Prefetching](#10-prefetching)
11. [TanStack Query with tRPC (Project Nexus)](#11-tanstack-query-with-trpc-project-nexus)

---

## 1. The Problem with `useEffect` Fetching

Every time you fetch data with `useEffect`, you need to handle:

```jsx
// You write this. Every time. For every piece of server data.
const [data, setData] = useState(null);
const [isLoading, setIsLoading] = useState(true);
const [error, setError] = useState(null);

useEffect(() => {
  let cancelled = false;
  setIsLoading(true);

  fetchStudents()
    .then(data => { if (!cancelled) { setData(data); setIsLoading(false); } })
    .catch(err  => { if (!cancelled) { setError(err);  setIsLoading(false); } });

  return () => { cancelled = true; };
}, []);

// And then you have no:
// - Caching (same data fetched again on remount)
// - Deduplication (two components fetching same data = two requests)
// - Background refetching (stale data shown after focus returns)
// - Retry on failure
// - Pagination
// - Optimistic updates
```

TanStack Query replaces all of this.

---

## 2. Server State vs Client State

```
Client state:                    Server state:
  isModalOpen                      students list
  selectedTab                      student profile
  formDraft                        course catalogue
  theme                            exam results

Owned by:  your app               the server/database
Lives in:  useState, Zustand       the network
Changes:   on user action          at any time (other users, background jobs)
Needs:     simple state            caching, sync, refetch, retry
```

TanStack Query is specifically for **server state** — data that lives on a server, may be stale, and needs synchronisation. Don't use it for client state.

---

## 3. Setup

```bash
npm install @tanstack/react-query
npm install @tanstack/react-query-devtools  # optional but highly recommended
```

```jsx
// main.jsx
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { ReactQueryDevtools } from "@tanstack/react-query-devtools";

const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      staleTime: 5 * 60 * 1000,   // data is fresh for 5 minutes
      retry: 2,                    // retry failed requests twice
      refetchOnWindowFocus: true,  // refetch when tab regains focus
    },
  },
});

ReactDOM.createRoot(document.getElementById("root")).render(
  <QueryClientProvider client={queryClient}>
    <App />
    <ReactQueryDevtools initialIsOpen={false} />
  </QueryClientProvider>
);
```

---

## 4. `useQuery` — The Core Hook

```jsx
import { useQuery } from "@tanstack/react-query";

const {
  data,
  isLoading,       // true only on first load (no cached data)
  isFetching,      // true whenever a request is in flight (including background)
  isError,
  error,
  isSuccess,
  refetch,
  dataUpdatedAt,
} = useQuery({
  queryKey: ["students"],
  queryFn: () => fetchStudents(),
});
```

```jsx
function StudentsPage() {
  const { data: students, isLoading, isError, error } = useQuery({
    queryKey: ["students"],
    queryFn: fetchStudents,
  });

  if (isLoading) return <Spinner />;
  if (isError)   return <ErrorMessage message={error.message} />;

  return (
    <ul>
      {students.map(s => <StudentCard key={s.id} student={s} />)}
    </ul>
  );
}
```

### Query with parameters

```jsx
function StudentDetail({ studentId }) {
  const { data: student, isLoading } = useQuery({
    queryKey: ["student", studentId],   // unique key per student
    queryFn: () => fetchStudent(studentId),
    enabled: !!studentId,               // don't fetch if no id
  });

  if (isLoading) return <Spinner />;
  return <StudentProfile student={student} />;
}
```

### Useful options

```jsx
useQuery({
  queryKey: ["students", { grade, query }],
  queryFn: () => fetchStudents({ grade, query }),

  staleTime: 10 * 60 * 1000,  // consider fresh for 10 minutes (default: 0)
  gcTime:    30 * 60 * 1000,  // keep in cache for 30 minutes even if unused (default: 5 min)

  enabled: isLoggedIn,         // only fetch when condition is true
  refetchInterval: 30_000,     // poll every 30 seconds (live data)
  refetchOnWindowFocus: false, // don't refetch when tab regains focus

  select: (data) => data.students,  // transform data before returning
  // data is now data.students, not the full response

  initialData: [],             // use this while loading (bypasses isLoading = true)
  placeholderData: keepPreviousData,  // keep previous data while fetching new
  // ^ great for pagination — no blank flash between pages
});
```

---

## 5. Query Keys

Query keys are arrays that uniquely identify a piece of data. TanStack Query uses them for caching and invalidation.

```jsx
// Simple key — no parameters
["students"]

// Parameterised — unique per combination
["student", studentId]
["students", { grade: 11 }]
["students", { grade: 11, page: 2, query: "ashan" }]

// Hierarchical — for invalidating groups
["students"]               // all students queries
["students", studentId]    // specific student
["students", studentId, "results"]  // student's results

// Convention: [entity, id?, scope?]
["students"]
["students", "s123"]
["students", "s123", "scores"]
["courses"]
["courses", "c456"]
["courses", "c456", "students"]
```

```jsx
// queryClient.invalidateQueries({ queryKey: ["students"] })
// invalidates ALL queries whose key starts with ["students"]
// So ["students"], ["students", "s123"], ["students", { grade: 11 }] all get invalidated
```

---

## 6. `useMutation` — Modifying Data

```jsx
import { useMutation, useQueryClient } from "@tanstack/react-query";

function AddStudentForm() {
  const queryClient = useQueryClient();

  const { mutate, mutateAsync, isPending, isError, error } = useMutation({
    mutationFn: (newStudent) => createStudent(newStudent),

    onSuccess: (createdStudent) => {
      // Invalidate so the list refetches with the new student
      queryClient.invalidateQueries({ queryKey: ["students"] });
      showToast("Student added successfully", "success");
    },

    onError: (error) => {
      showToast(`Failed to add student: ${error.message}`, "error");
    },

    onSettled: () => {
      // Runs after success or error — like finally
      setIsFormOpen(false);
    },
  });

  function handleSubmit(formData) {
    mutate(formData);  // fire and forget
    // OR
    // await mutateAsync(formData);  // returns a promise you can await
  }

  return (
    <form onSubmit={handleSubmit}>
      {/* ... */}
      <button type="submit" disabled={isPending}>
        {isPending ? "Adding..." : "Add Student"}
      </button>
      {isError && <p>{error.message}</p>}
    </form>
  );
}
```

### Multiple mutations

```jsx
const updateScore  = useMutation({ mutationFn: updateStudentScore });
const deleteStudent = useMutation({ mutationFn: deleteStudentById });
const promoteGrade  = useMutation({ mutationFn: promoteToNextGrade });

// Each has its own isPending, isError, etc.
```

---

## 7. Cache Invalidation

After mutations, you need to update the cache so the UI reflects the change.

```jsx
const queryClient = useQueryClient();

// Strategy 1: Invalidate — trigger a refetch
queryClient.invalidateQueries({ queryKey: ["students"] });
// All queries starting with ["students"] will refetch

// Strategy 2: Update the cache directly (no refetch)
queryClient.setQueryData(["student", studentId], (old) => ({
  ...old,
  score: newScore,
}));

// Strategy 3: Remove from cache (forces fresh fetch on next access)
queryClient.removeQueries({ queryKey: ["student", deletedId] });

// Strategy 4: Cancel in-flight queries before mutation
// (prevents race conditions)
const updateStudent = useMutation({
  mutationFn: updateStudentApi,
  onMutate: async (updatedData) => {
    await queryClient.cancelQueries({ queryKey: ["student", updatedData.id] });
    const previous = queryClient.getQueryData(["student", updatedData.id]);
    queryClient.setQueryData(["student", updatedData.id], updatedData);
    return { previous };  // for rollback
  },
  onError: (err, variables, context) => {
    queryClient.setQueryData(["student", variables.id], context.previous);
  },
  onSettled: (_, __, variables) => {
    queryClient.invalidateQueries({ queryKey: ["student", variables.id] });
  },
});
```

---

## 8. Optimistic Updates

Show the result immediately, roll back on failure.

```jsx
function LikeButton({ post }) {
  const queryClient = useQueryClient();

  const likeMutation = useMutation({
    mutationFn: () => toggleLike(post.id),

    onMutate: async () => {
      // Cancel any outgoing refetches
      await queryClient.cancelQueries({ queryKey: ["post", post.id] });

      // Snapshot the previous value for rollback
      const previous = queryClient.getQueryData(["post", post.id]);

      // Optimistically update the cache
      queryClient.setQueryData(["post", post.id], (old) => ({
        ...old,
        isLiked: !old.isLiked,
        likes:   old.isLiked ? old.likes - 1 : old.likes + 1,
      }));

      return { previous };  // context for onError
    },

    onError: (err, variables, context) => {
      // Roll back to the previous value
      queryClient.setQueryData(["post", post.id], context.previous);
    },

    onSettled: () => {
      // Always refetch to sync with server truth
      queryClient.invalidateQueries({ queryKey: ["post", post.id] });
    },
  });

  const post = useQuery({ queryKey: ["post", post.id], queryFn: () => fetchPost(post.id) });

  return (
    <button onClick={() => likeMutation.mutate()}>
      {post.data?.isLiked ? "♥" : "♡"} {post.data?.likes}
    </button>
  );
}
```

---

## 9. Pagination and Infinite Scroll

### Standard pagination

```jsx
function PaginatedStudents({ grade }) {
  const [page, setPage] = useState(1);

  const { data, isLoading, isFetching, isPlaceholderData } = useQuery({
    queryKey: ["students", { grade, page }],
    queryFn: () => fetchStudents({ grade, page, limit: 20 }),
    placeholderData: keepPreviousData,  // show previous page while loading next
  });

  return (
    <div>
      {isLoading ? (
        <Spinner />
      ) : (
        <StudentGrid
          students={data.students}
          className={isPlaceholderData ? "opacity-50" : ""}
        />
      )}
      <div className="pagination">
        <button
          onClick={() => setPage(p => p - 1)}
          disabled={page === 1}
        >
          ← Previous
        </button>
        <span>Page {page} of {data?.totalPages}</span>
        <button
          onClick={() => setPage(p => p + 1)}
          disabled={isPlaceholderData || page >= (data?.totalPages ?? 1)}
        >
          Next →
        </button>
      </div>
    </div>
  );
}
```

### Infinite scroll

```jsx
import { useInfiniteQuery } from "@tanstack/react-query";

function InfiniteStudentList({ grade }) {
  const {
    data,
    fetchNextPage,
    hasNextPage,
    isFetchingNextPage,
    isLoading,
  } = useInfiniteQuery({
    queryKey: ["students", "infinite", { grade }],
    queryFn: ({ pageParam = 1 }) => fetchStudents({ grade, page: pageParam, limit: 20 }),
    initialPageParam: 1,
    getNextPageParam: (lastPage, allPages) => {
      // Return next page number, or undefined if no more pages
      return lastPage.hasMore ? allPages.length + 1 : undefined;
    },
  });

  // Flatten pages into single array
  const students = data?.pages.flatMap(page => page.students) ?? [];

  // Auto-fetch next page when sentinel is visible
  const sentinelRef = useRef(null);
  useEffect(() => {
    const observer = new IntersectionObserver(([entry]) => {
      if (entry.isIntersecting && hasNextPage && !isFetchingNextPage) {
        fetchNextPage();
      }
    });
    if (sentinelRef.current) observer.observe(sentinelRef.current);
    return () => observer.disconnect();
  }, [hasNextPage, isFetchingNextPage, fetchNextPage]);

  if (isLoading) return <Spinner />;

  return (
    <div>
      <StudentGrid students={students} />
      <div ref={sentinelRef}>
        {isFetchingNextPage && <Spinner />}
        {!hasNextPage && <p>All students loaded</p>}
      </div>
    </div>
  );
}
```

---

## 10. Prefetching

Fetch data before the user navigates to it — instant page loads.

```jsx
// Prefetch on hover (user is likely about to click)
function StudentListItem({ student }) {
  const queryClient = useQueryClient();

  function handleMouseEnter() {
    queryClient.prefetchQuery({
      queryKey: ["student", student.id],
      queryFn: () => fetchStudent(student.id),
      staleTime: 10 * 60 * 1000,  // don't re-prefetch if already fresh
    });
  }

  return (
    <li onMouseEnter={handleMouseEnter}>
      <Link to={`/students/${student.id}`}>{student.name}</Link>
    </li>
  );
}

// Prefetch in route loader (createBrowserRouter)
{
  path: "/students",
  loader: async () => {
    await queryClient.prefetchQuery({
      queryKey: ["students"],
      queryFn: fetchStudents,
    });
    return null;
  },
  element: <StudentsPage />,
}
```

---

## 11. TanStack Query with tRPC (Project Nexus)

In Project Nexus, tRPC wraps TanStack Query automatically. Every tRPC procedure becomes a typed query or mutation.

```typescript
// The tRPC client is set up with TanStack Query integration
// You get type-safe queries with zero boilerplate

// Equivalent to: useQuery({ queryKey: ["students.list", { grade }], queryFn: ... })
const { data: students } = trpc.students.list.useQuery({ grade: 11 });

// Equivalent to: useMutation({ mutationFn: createStudent })
const createStudent = trpc.students.create.useMutation({
  onSuccess: () => {
    utils.students.list.invalidate();  // type-safe invalidation
  },
});

// The query key is derived from the procedure path — automatic
// Type errors surface at compile time — no runtime surprises
// The full TanStack Query API is still available

const { data, isLoading, isError } = trpc.students.getById.useQuery(
  { id: studentId },
  {
    enabled: !!studentId,
    staleTime: 5 * 60 * 1000,
    placeholderData: keepPreviousData,
  }
);
```

---

## Summary

```
TanStack Query replaces:
  Manual useEffect fetching, loading/error state, caching, deduplication

Core concepts:
  QueryClient: the cache — wrap app in <QueryClientProvider client={queryClient}>
  useQuery:    fetch and cache data
  useMutation: modify data (create, update, delete)

useQuery:
  queryKey:  unique identifier for this data — used for caching + invalidation
  queryFn:   async function that returns the data
  enabled:   boolean — don't fetch if false
  staleTime: how long to consider data fresh (default: 0 = always stale)
  select:    transform data before returning to component

Status:
  isLoading:  true only on initial load (no cached data yet)
  isFetching: true whenever a request is in flight
  isError:    true if the last request failed

useMutation:
  mutationFn: async function that takes variables and performs the change
  onSuccess:  runs after success — invalidate cache here
  onError:    runs after failure
  onSettled:  runs after either — like finally

Cache invalidation:
  invalidateQueries({ queryKey }) — trigger refetch of matching queries
  setQueryData(key, updater) — update cache directly (no refetch)

Optimistic updates:
  onMutate: update cache immediately
  onError: roll back using context.previous
  onSettled: always refetch to sync with server

Query keys:
  ["entity"] → all queries for entity
  ["entity", id] → specific entity
  ["entity", { filters }] → filtered query
  Hierarchical: invalidating ["students"] also invalidates ["students", id]

tRPC (Project Nexus):
  trpc.procedure.useQuery() — same as useQuery, fully typed
  trpc.procedure.useMutation() — same as useMutation, fully typed
  utils.procedure.invalidate() — type-safe invalidation
```

---

_Next: [17 — Forms & Validation (React Hook Form + Zod)](./17%20-%20Forms%20%26%20Validation.md)_