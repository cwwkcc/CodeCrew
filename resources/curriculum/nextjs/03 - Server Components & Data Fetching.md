> Server Components are the core of Next.js App Router. They let you fetch data directly inside your components — no `useEffect`, no API layer, no loading state boilerplate. This file covers how Server Components work, how to fetch data in them correctly, and the patterns you'll use in every real project.

---

## Table of Contents

1. [Server Components in Depth](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-server-components-in-depth)
2. [Async Data Fetching](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-async-data-fetching)
3. [Parallel vs Sequential Fetching](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-parallel-vs-sequential-fetching)
4. [fetch() in Next.js — Caching and Revalidation](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-fetch-in-nextjs--caching-and-revalidation)
5. [Direct Database Access with Prisma](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-direct-database-access-with-prisma)
6. [Streaming with Suspense](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-streaming-with-suspense)
7. [Passing Server Data to Client Components](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-passing-server-data-to-client-components)
8. [Common Patterns](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-common-patterns)

---

## 1. Server Components in Depth

A Server Component is a React component that runs exclusively on the server. No `"use client"` directive = Server Component.

```tsx
// This is a Server Component
// — runs on the server
// — can await anything
// — its source code never reaches the browser
// — the result (HTML) is sent to the browser

export default async function StudentsPage() {
  const students = await prisma.student.findMany();
  return <StudentGrid students={students} />;
}
```

### What Server Components can and cannot do

```
✅ Can do:
  async/await directly in the component body
  Access databases (Prisma, raw SQL)
  Read environment variables (including secrets — NEVER sent to client)
  Access the file system
  Use server-only libraries
  Render other Server Components
  Render Client Components (passing props to them)

❌ Cannot do:
  useState, useReducer, useContext (no state)
  useEffect, useLayoutEffect (no lifecycle)
  Event handlers: onClick, onChange, onSubmit
  Browser APIs: window, document, localStorage
  Use hooks (any hook — they're client-only)
```

The browser never sees the component function. It sees the rendered HTML output. This means:

```tsx
// This is SAFE — API key never reaches browser
export default async function WeatherWidget() {
  const res = await fetch("https://api.weather.com/current", {
    headers: { "X-API-Key": process.env.WEATHER_API_KEY }, // server-only env var
  });
  const data = await res.json();
  return <p>Temperature: {data.temp}°C</p>;
}
```

If this were a Client Component using `useEffect`, the API key would be in the browser bundle. As a Server Component, it's invisible to the client.

### server-only package

For code that should never accidentally run on the client, use the `server-only` package:

```bash
npm install server-only
```

```tsx
// lib/db.ts
import "server-only"; // throws a build error if imported in a Client Component

import { prisma } from "./prisma";

export async function getStudents() {
  return prisma.student.findMany({ orderBy: { name: "asc" } });
}
```

If you accidentally import `getStudents` in a Client Component, the build fails with a clear error. Better than silently shipping database credentials to the browser.

---

## 2. Async Data Fetching

The simplest pattern — `async` page component, `await` your data:

```tsx
// app/[locale]/students/page.tsx
import { prisma } from "@/lib/db";
import StudentGrid from "./StudentGrid";
import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "Students",
};

export default async function StudentsPage({
  searchParams,
}: {
  searchParams: { grade?: string; page?: string };
}) {
  const grade = searchParams.grade ? parseInt(searchParams.grade) : undefined;
  const page = parseInt(searchParams.page ?? "1");
  const limit = 12;

  const [students, total] = await Promise.all([
    prisma.student.findMany({
      where: grade ? { grade } : undefined,
      skip: (page - 1) * limit,
      take: limit,
      orderBy: { name: "asc" },
      select: { id: true, name: true, grade: true, photoUrl: true },
    }),
    prisma.student.count({
      where: grade ? { grade } : undefined,
    }),
  ]);

  return (
    <main className="px-6 py-8">
      <h1 className="text-3xl font-cormorant font-semibold mb-6">Students</h1>
      <StudentGrid
        students={students}
        total={total}
        page={page}
        limit={limit}
      />
    </main>
  );
}
```

No `useState`. No `useEffect`. No loading state. No error catch (the `error.tsx` handles it). Just fetch and render.

---

## 3. Parallel vs Sequential Fetching

### Sequential (waterfall) — usually wrong

```tsx
// ❌ Bad — each await blocks the next
export default async function StudentDetailPage({ params }) {
  const student = await getStudent(params.id); // wait...
  const grades = await getGrades(params.id); // wait...
  const activities = await getActivities(params.id); // wait...
  // Total: sum of all three durations
}
```

### Parallel — almost always right

```tsx
// ✅ Good — all three run at the same time
export default async function StudentDetailPage({ params }) {
  const [student, grades, activities] = await Promise.all([
    getStudent(params.id),
    getGrades(params.id),
    getActivities(params.id),
  ]);
  // Total: duration of the slowest query, not the sum
}
```

### When sequential is correct

Sequential is only right when one piece of data depends on another:

```tsx
// This must be sequential — can't fetch grades without knowing the student exists
export default async function StudentDetailPage({ params }) {
  const student = await getStudent(params.id);
  if (!student) notFound();

  // Only now fetch related data using student.classId
  const [grades, classmates] = await Promise.all([
    getGrades(student.id),
    getClassmates(student.classId), // needs student.classId
  ]);
}
```

---

## 4. fetch() in Next.js — Caching and Revalidation

Next.js extends the native `fetch` API with caching controls. Every `fetch` call can specify how long its response should be cached.

### Cache options

```tsx
// Default in Next.js 15: no-store (not cached — always fresh)
const res = await fetch("https://api.example.com/data");

// Explicitly cached indefinitely (until manually revalidated)
const res = await fetch("https://api.example.com/data", {
  cache: "force-cache",
});

// Cached but re-fetched in the background after N seconds (ISR for fetch)
const res = await fetch("https://api.example.com/news", {
  next: { revalidate: 60 }, // stale after 60 seconds
});

// Never cached — always fresh
const res = await fetch("https://api.example.com/live-scores", {
  cache: "no-store",
});
```

### Route-level revalidation

Instead of per-fetch, you can set a revalidation time for the whole page:

```tsx
// app/[locale]/news/page.tsx

// Re-render this entire page at most once every 5 minutes
export const revalidate = 300;

export default async function NewsPage() {
  const news = await fetch("https://cms.example.com/news").then((r) =>
    r.json(),
  );
  return <NewsList items={news} />;
}
```

### on-demand revalidation

Force a page to re-render on demand (e.g., when content is published):

```tsx
// app/api/revalidate/route.ts
import { revalidatePath, revalidateTag } from "next/cache";
import { NextRequest, NextResponse } from "next/server";

export async function POST(request: NextRequest) {
  const secret = request.headers.get("x-revalidate-secret");
  if (secret !== process.env.REVALIDATE_SECRET) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  const { path, tag } = await request.json();

  if (path) revalidatePath(path); // revalidate a specific path
  if (tag) revalidateTag(tag); // revalidate all fetches tagged with this

  return NextResponse.json({ revalidated: true });
}
```

```tsx
// Tag a fetch so it can be revalidated by tag
const news = await fetch("https://cms.example.com/news", {
  next: { tags: ["news"] },
});

// Later: revalidateTag("news") revalidates all fetches with this tag
```

### fetch vs Prisma caching

`fetch` has built-in Next.js caching. Prisma queries don't — they always hit the database. For Prisma, use `unstable_cache`:

```tsx
import { unstable_cache } from "next/cache";

// Wrap expensive Prisma queries in unstable_cache
const getCachedStudents = unstable_cache(
  async () => {
    return prisma.student.findMany({ orderBy: { name: "asc" } });
  },
  ["students-list"], // cache key
  { revalidate: 300 }, // re-run after 5 minutes
);

export default async function StudentsPage() {
  const students = await getCachedStudents();
  return <StudentGrid students={students} />;
}
```

---

## 5. Direct Database Access with Prisma

In Next.js, Server Components can import and use Prisma directly. No API layer needed for internal data.

### The Prisma client singleton

In development, module hot-reloading can create multiple Prisma client instances. Prevent this with a singleton:

```tsx
// lib/prisma.ts
import "server-only";
import { PrismaClient } from "@prisma/client";

const globalForPrisma = globalThis as unknown as { prisma: PrismaClient };

export const prisma =
  globalForPrisma.prisma ??
  new PrismaClient({
    log:
      process.env.NODE_ENV === "development" ? ["query", "error"] : ["error"],
  });

if (process.env.NODE_ENV !== "production") globalForPrisma.prisma = prisma;
```

### Using Prisma in Server Components

```tsx
// app/[locale]/students/[id]/page.tsx
import { prisma } from "@/lib/prisma";
import { notFound } from "next/navigation";

export default async function StudentDetailPage({
  params,
}: {
  params: { id: string; locale: string };
}) {
  const student = await prisma.student.findUnique({
    where: { id: parseInt(params.id) },
    include: {
      grades: { orderBy: { year: "desc" } },
      activities: { where: { isActive: true } },
      class: { select: { name: true, teacher: { select: { name: true } } } },
    },
  });

  if (!student) notFound();

  return <StudentProfile student={student} locale={params.locale} />;
}
```

This runs entirely on the server. The Prisma client, database connection, and query results are server-side only. The browser receives rendered HTML.

---

## 6. Streaming with Suspense

By default, Next.js waits for all data in a Server Component before sending any HTML. Streaming lets you send the page shell immediately and stream in slow parts as they're ready.

### Without streaming (waterfall)

```tsx
// The entire page waits for ALL three queries before any HTML is sent
export default async function DashboardPage() {
  const [stats, activity, announcements] = await Promise.all([
    getStats(), // 50ms
    getActivity(), // 800ms ← everyone waits for this
    getAnnouncements(), // 200ms
  ]);

  return (
    <div>
      <Stats data={stats} />
      <ActivityFeed data={activity} />
      <Announcements data={announcements} />
    </div>
  );
}
```

### With streaming (parallel Suspense)

```tsx
// Shell renders and streams immediately
// Each section streams in as its own data resolves
import { Suspense } from "react";
import Stats from "./Stats";
import ActivityFeed from "./ActivityFeed";
import Announcements from "./Announcements";

export default function DashboardPage() {
  // Note: not async — no awaits here
  return (
    <div className="dashboard-grid">
      <Suspense fallback={<StatsSkeleton />}>
        <Stats /> {/* async inside — fetches its own data */}
      </Suspense>

      <Suspense fallback={<ActivitySkeleton />}>
        <ActivityFeed /> {/* slow — streams in when ready */}
      </Suspense>

      <Suspense fallback={<AnnouncementsSkeleton />}>
        <Announcements /> {/* independent — doesn't wait for ActivityFeed */}
      </Suspense>
    </div>
  );
}

// Each component fetches its own data
async function Stats() {
  const stats = await getStats(); // 50ms — streams first
  return <StatsGrid data={stats} />;
}

async function ActivityFeed() {
  const activity = await getActivity(); // 800ms — streams last
  return <FeedList data={activity} />;
}

async function Announcements() {
  const items = await getAnnouncements(); // 200ms — streams second
  return <AnnouncementsList items={items} />;
}
```

The user sees:

1. Page shell immediately
2. Stats appear at ~50ms
3. Announcements appear at ~200ms
4. ActivityFeed appears at ~800ms

Instead of: nothing for 800ms, then everything at once.

---

## 7. Passing Server Data to Client Components

Server Components can render Client Components and pass data to them via props. The data crosses the server/client boundary as serialised JSON — it must be serialisable.

```tsx
// Server Component — fetches data
// app/[locale]/students/page.tsx
import { prisma } from "@/lib/prisma";
import StudentFilterBar from "./StudentFilterBar"; // Client Component
import StudentGrid from "./StudentGrid"; // Server Component

export default async function StudentsPage({ searchParams }) {
  const students = await prisma.student.findMany({
    select: { id: true, name: true, grade: true, photoUrl: true },
  });

  const grades = await prisma.student.findMany({
    select: { grade: true },
    distinct: ["grade"],
    orderBy: { grade: "asc" },
  });

  return (
    <main>
      {/* Client Component — receives pre-fetched filter options */}
      <StudentFilterBar grades={grades.map((g) => g.grade)} />

      {/* Server Component — receives pre-fetched data */}
      <StudentGrid students={students} />
    </main>
  );
}
```

```tsx
// Client Component — handles interactivity
// components/StudentFilterBar.tsx
"use client";

import { useRouter, usePathname, useSearchParams } from "next/navigation";

export default function StudentFilterBar({ grades }: { grades: number[] }) {
  const router = useRouter();
  const pathname = usePathname();
  const searchParams = useSearchParams();

  function setGrade(grade: string) {
    const params = new URLSearchParams(searchParams.toString());
    if (grade) params.set("grade", grade);
    else params.delete("grade");
    router.push(`${pathname}?${params.toString()}`);
  }

  return (
    <div className="filter-bar">
      <button onClick={() => setGrade("")}>All</button>
      {grades.map((g) => (
        <button key={g} onClick={() => setGrade(String(g))}>
          Grade {g}
        </button>
      ))}
    </div>
  );
}
```

### What can cross the boundary

```
✅ Serialisable — safe to pass as props:
  strings, numbers, booleans, null
  plain objects and arrays
  Dates (serialised as ISO strings)

❌ Not serialisable — cannot pass:
  Functions (callbacks from server to client)
  Class instances
  React components
  Symbols, Maps, Sets (use arrays/objects instead)
```

---

## 8. Common Patterns

### Pattern 1 — Server Component wraps Client Component

```tsx
// Server fetches, Client handles interaction
async function StudentDetailPage({ params }) {
  const student = await prisma.student.findUnique({ where: { id: params.id } });
  if (!student) notFound();

  return <StudentDetailClient student={student} />; // Client Component
}
```

```tsx
"use client";
function StudentDetailClient({ student }) {
  const [activeTab, setActiveTab] = useState("profile");
  // ... tabs, interactions
}
```

### Pattern 2 — Data access layer

```tsx
// lib/students.ts — server-only data functions
import "server-only";
import { prisma } from "./prisma";
import { cache } from "react";

// React's cache() deduplicates calls in the same render
export const getStudent = cache(async (id: number) => {
  return prisma.student.findUnique({
    where: { id },
    include: { grades: true, activities: true },
  });
});

export const getStudents = cache(async (grade?: number) => {
  return prisma.student.findMany({
    where: grade ? { grade } : undefined,
    orderBy: { name: "asc" },
  });
});
```

```tsx
// Multiple Server Components calling getStudent(42) in the same render
// only results in ONE database query — React's cache() deduplicates
async function StudentPage({ params }) {
  const student = await getStudent(parseInt(params.id)); // query 1
  return (
    <>
      <StudentHeader studentId={parseInt(params.id)} />{" "}
      {/* would re-query... */}
      <StudentProfile student={student} />
    </>
  );
}

async function StudentHeader({ studentId }) {
  const student = await getStudent(studentId); // ...but cache() returns same result
  return <h1>{student?.name}</h1>;
}
```

### Pattern 3 — Conditional rendering based on auth

```tsx
// app/[locale]/layout.tsx
import { getSession } from "@/lib/auth";

export default async function LocaleLayout({ children, params }) {
  const session = await getSession();

  return (
    <div>
      <Header user={session?.user ?? null} />
      {children}
    </div>
  );
}
```

```tsx
// Server Component — conditionally show admin controls
async function StudentActions({ studentId }) {
  const session = await getSession();

  if (!session?.user.isAdmin) return null; // non-admins see nothing

  return (
    <div className="admin-actions">
      <EditStudentButton id={studentId} />
      <DeleteStudentButton id={studentId} />
    </div>
  );
}
```

---

## Summary

```
Server Components:
  Default in App Router — no directive needed
  Can be async — await data directly in the component body
  Code never sent to browser — secrets safe, bundle smaller
  Cannot use hooks, state, or event handlers

Fetching patterns:
  Sequential: await A, then await B — use only when B depends on A
  Parallel:   Promise.all([A, B, C]) — almost always correct

fetch() caching:
  cache: "no-store"           — never cache (always fresh)
  cache: "force-cache"        — cache indefinitely
  next: { revalidate: N }     — cache for N seconds
  next: { tags: ["name"] }    — tag for on-demand revalidation

Prisma:
  Direct access in Server Components — no API layer needed
  Singleton pattern to avoid multiple instances in development
  unstable_cache() to cache Prisma results like fetch()

React cache():
  Deduplicates calls within the same render tree
  call getStudent(42) in 3 Server Components → 1 DB query

Streaming:
  Don't await everything in the page — pass async components to <Suspense>
  Each Suspense boundary streams independently
  User sees content progressively instead of all-at-once

Server → Client boundary:
  Pass serialisable props (strings, numbers, plain objects, arrays)
  Cannot pass functions or class instances across the boundary
  server-only package prevents accidental client imports
```

---

_Next: [04 — Client Components & Interactivity](https://claude.ai/chat/04%20-%20Client%20Components%20%26%20Interactivity.md)_
