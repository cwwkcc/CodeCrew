> `layout.tsx` is the mechanism that lets parts of your UI persist across navigation — like a header that doesn't remount when you move between pages. `loading.tsx` and `error.tsx` automate the Suspense and ErrorBoundary boilerplate you'd write manually. This file covers all the special files Next.js recognises and what they do.

---

## Table of Contents

1. [layout.tsx — Persistent UI Across Navigation](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-layouttsx--persistent-ui-across-navigation)
2. [Nested Layouts](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-nested-layouts)
3. [loading.tsx — Automatic Suspense](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-loadingtsx--automatic-suspense)
4. [error.tsx — Automatic Error Boundary](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-errortsx--automatic-error-boundary)
5. [not-found.tsx](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-not-foundtsx)
6. [template.tsx — Non-Persistent Layouts](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-templatetsx--non-persistent-layouts)
7. [route.ts — API Endpoints](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-routets--api-endpoints)
8. [Composing Special Files Together](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-composing-special-files-together)

---

## 1. layout.tsx — Persistent UI Across Navigation

A `layout.tsx` file wraps all pages in the same folder and its subfolders. It receives `children` — the page that's currently active.

```tsx
// app/layout.tsx — root layout, wraps every page in the entire app
import type { Metadata } from "next";
import { Cormorant_Garamond, IBM_Plex_Sans } from "next/font/google";
import "./globals.css";

const cormorant = Cormorant_Garamond({
  subsets: ["latin"],
  weight: ["400", "600"],
  variable: "--font-cormorant",
});

const ibmPlex = IBM_Plex_Sans({
  subsets: ["latin"],
  weight: ["400", "500", "600"],
  variable: "--font-ibm-plex",
});

export const metadata: Metadata = {
  title: { template: "%s — CWWKCC", default: "CWWKCC" },
  description: "C.W.W. Kannangara Central College official website.",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className={`${cormorant.variable} ${ibmPlex.variable}`}>
        {children}
      </body>
    </html>
  );
}
```

The root `layout.tsx` is special — it must include `<html>` and `<body>`. All other layouts just return a fragment or wrapper div.

**Why "persistent"?** When you navigate from `/students` to `/news`, the `RootLayout` component stays mounted — it doesn't unmount and remount. Only the `children` (the page) changes. This means:

- The `<html>` and `<body>` don't re-render
- Fonts load once
- Any state inside the layout is preserved across navigation

This is different from traditional multi-page apps where the entire page reloads, and it's what makes Next.js feel fast — only the changed content re-renders.

---

## 2. Nested Layouts

Layouts nest. A page receives the nearest layout's `children`, which in turn is passed up to parent layouts.

```
app/
  layout.tsx           ← RootLayout
  [locale]/
    layout.tsx         ← LocaleLayout
    page.tsx           ← homepage
    students/
      layout.tsx       ← StudentsLayout
      page.tsx         ← /students
      [id]/
        page.tsx       ← /students/42 (no layout here — uses StudentsLayout)
```

When `/students/42` is rendered, the tree is:

```
RootLayout
  └── LocaleLayout
        └── StudentsLayout
              └── StudentDetailPage  (the page.tsx)
```

```tsx
// app/[locale]/layout.tsx
import Header from "@/components/Header";
import Footer from "@/components/Footer";

export default function LocaleLayout({
  children,
  params,
}: {
  children: React.ReactNode;
  params: { locale: string };
}) {
  return (
    <div className="flex flex-col min-h-screen">
      <Header locale={params.locale} />
      <main className="flex-1">{children}</main>
      <Footer />
    </div>
  );
}
```

```tsx
// app/[locale]/students/layout.tsx
import StudentsNav from "./StudentsNav";

export default function StudentsLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <div className="students-section">
      <StudentsNav />
      <div className="students-content">{children}</div>
    </div>
  );
}
```

When navigating from `/students` to `/students/42`:

- `RootLayout` stays mounted — no change
- `LocaleLayout` stays mounted — Header and Footer stay
- `StudentsLayout` stays mounted — `StudentsNav` stays
- Only the page (`page.tsx`) swaps

### Layouts can fetch data

```tsx
// app/[locale]/layout.tsx — fetch nav data once, available for all pages
export default async function LocaleLayout({
  children,
  params,
}: {
  children: React.ReactNode;
  params: { locale: string };
}) {
  const navigation = await getNavigationItems(params.locale);
  const announcements = await getActiveAnnouncements();

  return (
    <div>
      <Header nav={navigation} announcements={announcements} />
      {children}
      <Footer nav={navigation} />
    </div>
  );
}
```

This runs once per navigation into the locale segment — not on every page render.

---

## 3. loading.tsx — Automatic Suspense

`loading.tsx` is automatically wrapped in a `<Suspense>` boundary around the `page.tsx` in the same folder. While the page is loading (its async Server Component is awaiting data), Next.js shows the loading UI.

```tsx
// app/[locale]/students/loading.tsx
export default function StudentsLoading() {
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 p-6">
      {Array.from({ length: 6 }).map((_, i) => (
        <div key={i} className="card animate-pulse">
          <div className="h-32 bg-muted rounded-t-lg" />
          <div className="p-4 space-y-2">
            <div className="h-4 bg-muted rounded w-3/4" />
            <div className="h-3 bg-muted rounded w-1/2" />
          </div>
        </div>
      ))}
    </div>
  );
}
```

This is the equivalent of writing:

```tsx
<Suspense fallback={<StudentsLoading />}>
  <StudentsPage />
</Suspense>
```

...but Next.js handles it for you. The `loading.tsx` UI appears immediately when navigation starts, before the page's data fetching completes.

### Instant loading states (important)

Because `loading.tsx` shows immediately on navigation, users get instant feedback that something is happening — even if the page takes 2 seconds to fetch data. This is a significant UX improvement over a blank screen.

```tsx
// app/[locale]/news/[slug]/loading.tsx — article skeleton
export default function ArticleLoading() {
  return (
    <article className="max-w-2xl mx-auto px-6 py-12 animate-pulse">
      <div className="h-6 bg-muted rounded w-1/4 mb-4" /> {/* date */}
      <div className="h-10 bg-muted rounded w-3/4 mb-2" /> {/* title */}
      <div className="h-10 bg-muted rounded w-1/2 mb-8" /> {/* title line 2 */}
      <div className="space-y-3">
        {Array.from({ length: 8 }).map((_, i) => (
          <div
            key={i}
            className={`h-4 bg-muted rounded ${i % 4 === 3 ? "w-2/3" : "w-full"}`}
          />
        ))}
      </div>
    </article>
  );
}
```

---

## 4. error.tsx — Automatic Error Boundary

`error.tsx` is automatically wrapped in an `<ErrorBoundary>` around the `page.tsx` in the same folder. When the page throws — a failed database query, an unhandled error, a component crash — Next.js renders the error UI instead of crashing the whole app.

```tsx
// app/[locale]/students/error.tsx
// MUST be a Client Component — Error Boundaries are class-based under the hood
"use client";

import { useEffect } from "react";
import Link from "next/link";

export default function StudentsError({
  error,
  reset,
}: {
  error: Error & { digest?: string }; // digest = server-side error ID for logging
  reset: () => void; // calling this retries rendering the page
}) {
  useEffect(() => {
    // Log to error monitoring (Sentry, etc.)
    console.error(error);
  }, [error]);

  return (
    <div className="flex flex-col items-center justify-center py-16 text-center">
      <h2 className="text-xl font-semibold">Failed to load students</h2>
      <p className="text-muted-foreground mt-2">
        {error.message || "Something went wrong. Please try again."}
      </p>
      <div className="flex gap-3 mt-6">
        <button onClick={reset} className="btn-primary">
          Try again
        </button>
        <Link href="/" className="btn-outline">
          Go home
        </Link>
      </div>
    </div>
  );
}
```

**`reset()`** re-renders the segment — triggers the page to attempt rendering again. Useful for transient errors (network blip, temporary DB issue).

**`error.digest`** is a hash of the server-side error. Because server errors don't expose details to the client (security), the digest lets you correlate a client-visible error with the full server-side stack trace in your logs.

### Error boundary scope

```
app/
  [locale]/
    error.tsx         ← catches errors from ANY page under [locale]/
    students/
      error.tsx       ← overrides the parent — more specific handler
      [id]/
        page.tsx      ← if this throws, students/error.tsx handles it
    news/
      page.tsx        ← if this throws, [locale]/error.tsx handles it
                         (no news/error.tsx, so bubbles up)
```

The most specific `error.tsx` wins. If none exists in the segment, it bubbles to the nearest parent.

### Root error (global.error.tsx)

The root `app/error.tsx` doesn't catch errors in `app/layout.tsx` — layouts are outside the error boundary. For that, use `app/global-error.tsx`:

```tsx
// app/global-error.tsx
// Catches errors in the root layout — replaces the entire page
"use client";

export default function GlobalError({
  error,
  reset,
}: {
  error: Error & { digest?: string };
  reset: () => void;
}) {
  return (
    // Must include <html> and <body> — root layout is gone
    <html>
      <body>
        <div className="min-h-screen flex flex-col items-center justify-center">
          <h1>Something went seriously wrong</h1>
          <button onClick={reset}>Try again</button>
        </div>
      </body>
    </html>
  );
}
```

---

## 5. not-found.tsx

Rendered when `notFound()` is called from a page or when no route matches a URL. Covered in detail in File 01 (routing section). Key points:

```tsx
// app/[locale]/students/[id]/not-found.tsx
import Link from "next/link";

export default function StudentNotFound() {
  return (
    <div className="text-center py-16">
      <h1 className="text-2xl font-semibold text-foreground">
        Student not found
      </h1>
      <p className="text-muted-foreground mt-2">
        This student record doesn't exist or may have been removed.
      </p>
      <Link href="/students" className="btn-primary mt-6 inline-block">
        View all students
      </Link>
    </div>
  );
}
```

`not-found.tsx` is a **Server Component** by default (unlike `error.tsx`). It doesn't receive any props — just render a helpful UI.

---

## 6. template.tsx — Non-Persistent Layouts

`template.tsx` works like `layout.tsx` but re-mounts on every navigation. Use it when you need the wrapper to reset on navigation — like a page transition animation or resetting scroll position.

```tsx
// app/[locale]/template.tsx
"use client";

import { motion } from "framer-motion";

export default function LocaleTemplate({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <motion.div
      initial={{ opacity: 0, y: 8 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ duration: 0.2 }}
    >
      {children}
    </motion.div>
  );
}
```

Because `template.tsx` remounts, the animation triggers on every page transition. If you used `layout.tsx` for this, the motion component would only animate once (when first mounted).

**`layout.tsx` vs `template.tsx`:**

```
layout.tsx    — persists across navigation — state preserved, no re-mount
template.tsx  — re-mounts on navigation — state resets, good for animations
```

In practice, `layout.tsx` is almost always what you want. `template.tsx` is for edge cases.

---

## 7. route.ts — API Endpoints

A `route.ts` file in any folder creates an API endpoint — no page UI, just request/response handlers.

```
app/
  api/
    students/
      route.ts           →  GET/POST /api/students
    students/
      [id]/
        route.ts         →  GET/PUT/DELETE /api/students/:id
    revalidate/
      route.ts           →  POST /api/revalidate
```

```tsx
// app/api/students/route.ts
import { NextRequest, NextResponse } from "next/server";
import { prisma } from "@/lib/prisma";

// GET /api/students
export async function GET(request: NextRequest) {
  const { searchParams } = new URL(request.url);
  const grade = searchParams.get("grade");

  const students = await prisma.student.findMany({
    where: grade ? { grade: parseInt(grade) } : undefined,
    select: { id: true, name: true, grade: true },
    orderBy: { name: "asc" },
  });

  return NextResponse.json(students);
}

// POST /api/students
export async function POST(request: NextRequest) {
  const body = await request.json();

  const student = await prisma.student.create({
    data: {
      name: body.name,
      grade: body.grade,
    },
  });

  return NextResponse.json(student, { status: 201 });
}
```

```tsx
// app/api/students/[id]/route.ts
import { NextRequest, NextResponse } from "next/server";

export async function GET(
  request: NextRequest,
  { params }: { params: { id: string } },
) {
  const student = await prisma.student.findUnique({
    where: { id: parseInt(params.id) },
  });

  if (!student) {
    return NextResponse.json({ error: "Not found" }, { status: 404 });
  }

  return NextResponse.json(student);
}

export async function DELETE(
  request: NextRequest,
  { params }: { params: { id: string } },
) {
  await prisma.student.delete({ where: { id: parseInt(params.id) } });
  return new NextResponse(null, { status: 204 });
}
```

### When do you need API routes?

If you're using Server Components and Server Actions, you may not need API routes at all — you can call database functions directly. API routes are for:

- External services that need to call your backend (webhooks, mobile apps)
- Client Components that need to fetch/mutate data (when Server Actions aren't appropriate)
- ISR revalidation triggers
- File uploads or other non-standard requests

---

## 8. Composing Special Files Together

Here's what the actual file structure looks like around a segment that uses all the special files:

```
app/[locale]/students/
  layout.tsx      — StudentsLayout (sidebar nav, section header)
  page.tsx        — async Server Component, fetches student list
  loading.tsx     — grid of skeleton cards
  error.tsx       — error state with retry button ("use client")
  [id]/
    page.tsx      — async Server Component, fetches one student
    loading.tsx   — single student skeleton
    not-found.tsx — student not found message
```

And how Next.js renders them when a user visits `/en/students/42`:

```
RootLayout                 (app/layout.tsx)
  └── LocaleLayout         (app/[locale]/layout.tsx)
        └── StudentsLayout (app/[locale]/students/layout.tsx)
              └── <Suspense fallback={<StudentDetailLoading />}>
                    └── <ErrorBoundary fallback={<StudentsError />}>
                          └── StudentDetailPage (app/[locale]/students/[id]/page.tsx)
```

You wrote four focused files. Next.js assembled the Suspense and ErrorBoundary wrapping automatically, in the right nesting order.

---

## Summary

```
layout.tsx:
  Wraps all pages in the same folder and its children
  Persists across navigation — stays mounted, doesn't remount
  Can be async — fetch data once for the whole section
  Root layout must include <html> and <body>

loading.tsx:
  Automatic <Suspense fallback={...}> around the page
  Shows immediately on navigation — instant loading feedback
  Make it match the shape of the real content (skeletons > spinners)

error.tsx:
  Automatic <ErrorBoundary> around the page
  MUST be "use client"
  Receives: error (the thrown error), reset (fn to retry the page)
  Use error.digest to correlate client errors with server logs
  Closest error.tsx wins; bubbles up if not found
  global-error.tsx for root layout errors

not-found.tsx:
  Rendered when notFound() is called
  Server Component by default (no "use client" needed)
  Closest not-found.tsx to the segment wins

template.tsx:
  Like layout.tsx but re-mounts on every navigation
  Use for page transition animations, scroll reset
  Rarely needed — layout.tsx is almost always right

route.ts:
  API endpoint — no UI, just HTTP handlers
  export async function GET / POST / PUT / DELETE
  Access params via second argument: ({ params })
  Use when external services need to call your app
  Often not needed if using Server Actions for mutations

Nesting order:
  RootLayout → ParentLayouts → SegmentLayout
  Wrapped automatically with Suspense (loading.tsx) and ErrorBoundary (error.tsx)
  Page renders inside the innermost boundary
```

---

_Next: [03 — Server Components & Data Fetching](https://claude.ai/chat/03%20-%20Server%20Components%20%26%20Data%20Fetching.md)_
