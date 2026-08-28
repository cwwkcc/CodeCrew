> In React Router you write route definitions in JavaScript. In Next.js App Router, the file system _is_ the route definition. A file at `app/students/page.tsx` automatically becomes the `/students` route — no config needed. This file covers how that system works in full.

---

## Table of Contents

1. [How File-System Routing Works](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-how-file-system-routing-works)
2. [The page.tsx File](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-the-pagetsx-file)
3. [Dynamic Segments](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-dynamic-segments)
4. [Catch-All Segments](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-catch-all-segments)
5. [Route Groups](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-route-groups)
6. [Parallel Routes](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-parallel-routes)
7. [notFound() and the not-found.tsx File](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-notfound-and-the-not-foundtsx-file)
8. [Metadata and the Head](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-metadata-and-the-head)
9. [Nexus Routing Structure](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#9-nexus-routing-structure)

---

## 1. How File-System Routing Works

Every folder inside `app/` that contains a `page.tsx` file becomes a route. The URL is the folder path.

```
app/
  page.tsx                    →  /
  about/
    page.tsx                  →  /about
  students/
    page.tsx                  →  /students
  news/
    page.tsx                  →  /news
  contact/
    page.tsx                  →  /contact
```

If a folder doesn't have a `page.tsx`, it doesn't create a route — it's just a folder (useful for organising layouts and components).

```
app/
  components/           ← no page.tsx — not a route, just a folder
    Header.tsx
    Footer.tsx
  students/
    page.tsx            ← /students (this exists)
    StudentCard.tsx     ← not a route, just a component file in the folder
```

---

## 2. The page.tsx File

`page.tsx` exports a default React component. That component is the page.

```tsx
// app/students/page.tsx
export default function StudentsPage() {
  return (
    <main>
      <h1>Students</h1>
    </main>
  );
}

// Server Component by default — can be async
export default async function StudentsPage() {
  const students = await fetchStudents(); // runs on server
  return (
    <main>
      <h1>Students</h1>
      <StudentGrid students={students} />
    </main>
  );
}
```

Pages receive two props automatically from Next.js:

```tsx
// app/students/[id]/page.tsx
export default async function StudentPage({
  params,
  searchParams,
}: {
  params: { id: string };
  searchParams: { tab?: string };
}) {
  const student = await fetchStudent(params.id);
  const activeTab = searchParams.tab ?? "profile";

  return <StudentDetail student={student} activeTab={activeTab} />;
}
```

- `params` — dynamic route segments (covered in section 3)
- `searchParams` — URL query string (`/students/1?tab=grades` → `{ tab: "grades" }`)

---

## 3. Dynamic Segments

Square brackets in a folder name create a dynamic segment — a part of the URL that can be anything.

```
app/
  students/
    page.tsx              →  /students
    [id]/
      page.tsx            →  /students/1, /students/42, /students/ashan
  news/
    [slug]/
      page.tsx            →  /news/annual-sports-day, /news/results-2024
  teachers/
    [id]/
      page.tsx            →  /teachers/mr-perera
      schedule/
        page.tsx          →  /teachers/mr-perera/schedule
```

The segment name in brackets becomes the key in `params`:

```tsx
// app/students/[id]/page.tsx
// URL: /students/42
export default async function StudentPage({
  params,
}: {
  params: { id: string };
}) {
  // params.id = "42"  — always a string, parse if you need a number
  const id = parseInt(params.id, 10);

  const student = await prisma.student.findUnique({ where: { id } });
  if (!student) notFound();

  return <StudentProfile student={student} />;
}
```

### Multiple dynamic segments

```
app/
  [locale]/
    students/
      [id]/
        page.tsx    →  /en/students/42
```

```tsx
// params = { locale: "en", id: "42" }
export default async function StudentPage({
  params,
}: {
  params: { locale: string; id: string };
}) {
  // access params.locale and params.id
}
```

### Static params (for static generation)

When you have a dynamic segment, Next.js needs to know which values exist to render them statically at build time:

```tsx
// app/students/[id]/page.tsx
import { prisma } from "@/lib/prisma";

// Tell Next.js all valid IDs so it can render them statically
export async function generateStaticParams() {
  const students = await prisma.student.findMany({ select: { id: true } });
  return students.map((s) => ({ id: String(s.id) }));
}

export default async function StudentPage({
  params,
}: {
  params: { id: string };
}) {
  const student = await prisma.student.findUnique({
    where: { id: parseInt(params.id) },
  });
  if (!student) notFound();
  return <StudentProfile student={student} />;
}
```

At build time, Next.js calls `generateStaticParams`, gets all IDs, and generates a static HTML file for each one. Instant load, no server needed per request.

---

## 4. Catch-All Segments

For routes with a variable number of segments.

```
[...slug]    — catch-all — matches 1 or more segments
[[...slug]]  — optional catch-all — matches 0 or more segments
```

```
app/
  docs/
    [...slug]/
      page.tsx
```

```
/docs/intro                    → params.slug = ["intro"]
/docs/guides/installation      → params.slug = ["guides", "installation"]
/docs/api/routes/dynamic       → params.slug = ["api", "routes", "dynamic"]
/docs                          → ❌ doesn't match [...slug] (needs at least one)
```

```tsx
export default function DocsPage({ params }: { params: { slug: string[] } }) {
  const path = params.slug.join("/"); // "guides/installation"
  // ...
}
```

Optional catch-all with `[[...slug]]`:

```
/docs            → params.slug = undefined  (matches the folder itself)
/docs/intro      → params.slug = ["intro"]
/docs/a/b/c      → params.slug = ["a", "b", "c"]
```

Useful when `/docs` should show an index and `/docs/anything/nested` should show content.

---

## 5. Route Groups

Parentheses in a folder name create a **route group** — the folder is excluded from the URL but can have its own layout.

```
app/
  (marketing)/
    page.tsx          →  /           (not /marketing)
    about/
      page.tsx        →  /about      (not /marketing/about)
    layout.tsx        →  applies to / and /about only

  (dashboard)/
    dashboard/
      page.tsx        →  /dashboard  (not /dashboard/dashboard)
    settings/
      page.tsx        →  /settings
    layout.tsx        →  applies to /dashboard and /settings only
```

This solves a real problem: the homepage and the dashboard both live under `app/` but need different layouts. Without route groups, they'd share the same `layout.tsx`. With route groups, each has its own.

### Nexus use case — multiple layouts

```
app/
  (public)/
    layout.tsx           — public layout: Header + Footer + language switcher
    [locale]/
      page.tsx           — homepage
      about/
        page.tsx
      students/
        page.tsx

  (admin)/
    layout.tsx           — admin layout: sidebar, auth check, no public header
    admin/
      dashboard/
        page.tsx
      students/
        page.tsx
```

---

## 6. Parallel Routes

Multiple pages rendered simultaneously in the same layout — each in its own "slot". Advanced, but worth knowing.

```
app/
  dashboard/
    layout.tsx
    page.tsx
    @analytics/
      page.tsx
    @notifications/
      page.tsx
```

```tsx
// app/dashboard/layout.tsx
export default function DashboardLayout({
  children,
  analytics,
  notifications,
}: {
  children: React.ReactNode;
  analytics: React.ReactNode;
  notifications: React.ReactNode;
}) {
  return (
    <div className="dashboard-grid">
      <main>{children}</main>
      <aside>{analytics}</aside>
      <aside>{notifications}</aside>
    </div>
  );
}
```

Each slot (`@analytics`, `@notifications`) renders its own page independently — they can load and fail separately. Not commonly needed, but it's how Next.js handles tabs and modals that need their own URL without a full page transition.

---

## 7. notFound() and the not-found.tsx File

When a dynamic route is requested but the resource doesn't exist, call `notFound()`:

```tsx
// app/students/[id]/page.tsx
import { notFound } from "next/navigation";

export default async function StudentPage({
  params,
}: {
  params: { id: string };
}) {
  const student = await prisma.student.findUnique({
    where: { id: parseInt(params.id) },
  });

  if (!student) notFound(); // stops rendering, shows not-found.tsx

  return <StudentProfile student={student} />;
}
```

`not-found.tsx` in the same folder (or a parent folder) handles the fallback:

```tsx
// app/students/[id]/not-found.tsx
import Link from "next/link";

export default function StudentNotFound() {
  return (
    <div className="text-center py-16">
      <h1 className="text-2xl font-semibold">Student not found</h1>
      <p className="text-muted-foreground mt-2">
        This student record doesn't exist or has been removed.
      </p>
      <Link href="/students" className="btn-primary mt-6 inline-block">
        Back to Students
      </Link>
    </div>
  );
}
```

The `not-found.tsx` closest to the route segment is used. If `app/students/[id]/not-found.tsx` doesn't exist, Next.js checks `app/students/not-found.tsx`, then `app/not-found.tsx`.

---

## 8. Metadata and the Head

Next.js handles the `<head>` through a metadata API — no manual `<title>` or `<meta>` tags.

### Static metadata

```tsx
// app/about/page.tsx
import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "About Us — CWWKCC",
  description:
    "Learn about C.W.W. Kannangara Central College, founded in 1953.",
  openGraph: {
    title: "About Us — CWWKCC",
    description: "Learn about C.W.W. Kannangara Central College.",
    images: ["/og/about.jpg"],
  },
};

export default function AboutPage() {
  return <main>...</main>;
}
```

### Dynamic metadata (for pages with route params)

```tsx
// app/students/[id]/page.tsx
import type { Metadata } from "next";

export async function generateMetadata({
  params,
}: {
  params: { id: string };
}): Promise<Metadata> {
  const student = await prisma.student.findUnique({
    where: { id: parseInt(params.id) },
    select: { name: true, grade: true },
  });

  if (!student) return { title: "Student Not Found" };

  return {
    title: `${student.name} — Grade ${student.grade} — CWWKCC`,
    description: `Student profile for ${student.name}, Grade ${student.grade}.`,
  };
}

export default async function StudentPage({
  params,
}: {
  params: { id: string };
}) {
  // ...
}
```

### Title template (avoid repeating the site name)

```tsx
// app/layout.tsx
export const metadata: Metadata = {
  title: {
    template: "%s — CWWKCC", // %s = page title
    default: "CWWKCC", // fallback when no page title set
  },
  description: "C.W.W. Kannangara Central College official website.",
};

// Any child page that sets title: "About Us"
// renders as: "About Us — CWWKCC"
```

---

## 9. Nexus Routing Structure

The full routing tree of Nexus, annotated:

```
apps/web/src/app/
  layout.tsx                        — bare root layout (fonts, global CSS)
  not-found.tsx                     — top-level 404

  [locale]/                         — /en/*, /si/* — all pages under locale
    layout.tsx                      — locale layout: sets lang, loads translations, Header + Footer
    page.tsx                        — /en (homepage)
    loading.tsx                     — full-page loading skeleton
    error.tsx                       — full-page error boundary

    about/
      page.tsx                      — /en/about

    students/
      page.tsx                      — /en/students (student listings)
      loading.tsx                   — student grid skeleton
      [id]/
        page.tsx                    — /en/students/42
        not-found.tsx               — student not found message

    news/
      page.tsx                      — /en/news
      [slug]/
        page.tsx                    — /en/news/annual-sports-day
        generateStaticParams()      — pre-renders all published articles at build time

    results/
      page.tsx                      — /en/results (exam results)

    gallery/
      page.tsx                      — /en/gallery

    contact/
      page.tsx                      — /en/contact

  (admin)/                          — route group — admin-only section
    layout.tsx                      — admin layout with auth guard
    admin/
      page.tsx                      — /admin (dashboard)
      students/
        page.tsx

  api/
    students/
      route.ts                      — GET /api/students
    news/
      route.ts                      — GET/POST /api/news
    revalidate/
      route.ts                      — POST /api/revalidate (ISR trigger)
```

Things to notice:

- All public pages live under `[locale]` — the locale segment is always present
- `loading.tsx` and `error.tsx` at the `[locale]` level handle app-wide states
- More specific `loading.tsx`/`error.tsx` override for specific sections
- Admin routes are in a route group `(admin)` with their own layout
- API routes live in `api/` — never under `[locale]` since they're language-agnostic

---

## Summary

```
File = Route:
  app/students/page.tsx → /students
  Folders without page.tsx are not routes

page.tsx:
  Default export = the page component
  Receives props: { params, searchParams }
  Can be async — runs on server by default

Dynamic segments:
  [id] folder → /students/:id → params.id
  Multiple segments in the same path → multiple keys in params
  generateStaticParams() → tell Next.js all valid values for static generation

Catch-all segments:
  [...slug] → matches 1+ segments → params.slug = string[]
  [[...slug]] → matches 0+ segments (optional)

Route groups:
  (name) folder → excluded from URL, but can have its own layout.tsx
  Use to apply different layouts to different sections without affecting the URL

notFound():
  Call when resource doesn't exist → Next.js renders not-found.tsx
  Closest not-found.tsx to the route segment is used

Metadata:
  export const metadata: Metadata = { title, description, openGraph }
  export async function generateMetadata({ params }) for dynamic titles
  Use title.template in root layout to avoid repeating the site name

Nexus pattern:
  All public routes under [locale] for i18n
  Route groups for admin section with separate layout
  api/ for API endpoints
```

---

_Next: [02 — Layouts & Special Files](https://claude.ai/chat/02%20-%20Layouts%20%26%20Special%20Files.md)_
