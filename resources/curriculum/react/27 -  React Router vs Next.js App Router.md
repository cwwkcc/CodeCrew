> File 15 taught React Router. This file bridges the gap to Next.js App Router — the routing system used in Project Nexus. The mental models are related but the mechanics are different. Understanding both, and how they map to each other, means you can read any React codebase and build confidently in Nexus.

---

## Table of Contents

1. [The Core Difference](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-core-difference)
2. [File-System Routing](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-file-system-routing)
3. [Layouts](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-layouts)
4. [Navigation](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-navigation)
5. [Dynamic Routes and Params](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-dynamic-routes-and-params)
6. [Search Params](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-search-params)
7. [Data Loading](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-data-loading)
8. [Loading and Error States](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-loading-and-error-states)
9. [Protected Routes](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#9-protected-routes)
10. [Side-by-Side Reference](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#10-side-by-side-reference)
11. [When to Use Each](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#11-when-to-use-each)

---

## 1. The Core Difference

**React Router** is a library you install. You write route definitions in JavaScript — `<Route path="/students" element={<StudentsPage />} />` — and React Router matches URLs to components at runtime in the browser.

**Next.js App Router** is baked into the framework. Routes are defined by the **file system** — a file at `app/students/page.tsx` automatically becomes the `/students` route. There's no route config to write.

The deeper difference is where code runs:

```
React Router:
  Everything runs in the browser
  Data fetching happens after the JS bundle loads and renders
  Server only serves one HTML file + JS bundle

Next.js App Router:
  Components can run on the server (Server Components — the default)
  Server Components fetch data on the server before sending HTML
  Client Components (opt-in with "use client") run in the browser
  You get server rendering, streaming, and client interactivity — all in one
```

This is why Nexus uses Next.js: the school website needs SEO, fast initial loads, and server-side data access (Prisma talks to the database on the server, never exposing credentials to the browser).

---

## 2. File-System Routing

In React Router you declare routes explicitly:

```jsx
// React Router
<Routes>
  <Route path="/"              element={<HomePage />} />
  <Route path="/students"      element={<StudentsPage />} />
  <Route path="/students/:id"  element={<StudentDetailPage />} />
  <Route path="/about"         element={<AboutPage />} />
  <Route path="*"              element={<NotFoundPage />} />
</Routes>
```

In Next.js App Router you create files — the path is the route:

```
app/
  page.tsx                    →  /
  students/
    page.tsx                  →  /students
    [id]/
      page.tsx                →  /students/123
  about/
    page.tsx                  →  /about
  not-found.tsx               →  rendered for unmatched routes
```

### Special file names

Each folder in `app/` can have these reserved files:

```
page.tsx        — the UI for this route (makes the route publicly accessible)
layout.tsx      — wraps this route and all its children (persistent across navigation)
loading.tsx     — shown while the page is loading (Suspense fallback)
error.tsx       — shown when the page throws (Error Boundary)
not-found.tsx   — shown when notFound() is called or no route matches
template.tsx    — like layout but re-mounts on every navigation
route.ts        — API endpoint (no UI — returns JSON, handles POST/GET etc.)
```

---

## 3. Layouts

In React Router, a shared layout is a parent route that renders `<Outlet />`:

```jsx
// React Router
<Routes>
  <Route path="/" element={<RootLayout />}>        {/* shared layout */}
    <Route index      element={<HomePage />} />
    <Route path="students" element={<StudentsPage />} />
    <Route path="about"    element={<AboutPage />} />
  </Route>
</Routes>

function RootLayout() {
  return (
    <div>
      <Header />
      <Outlet />     {/* child page renders here */}
      <Footer />
    </div>
  );
}
```

In Next.js, `layout.tsx` wraps all routes in the same folder:

```tsx
// app/layout.tsx — wraps every page in the app
export default function RootLayout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="en">
      <body>
        <Header />
        {children}     {/* child page renders here — equivalent to <Outlet /> */}
        <Footer />
      </body>
    </html>
  );
}

// app/students/layout.tsx — wraps only /students/* routes
export default function StudentsLayout({ children }: { children: React.ReactNode }) {
  return (
    <div className="students-section">
      <StudentsNav />
      <main>{children}</main>
    </div>
  );
}
```

**Key difference:** Next.js layouts **persist** across navigations within their segment. The `RootLayout` component stays mounted when you navigate from `/students` to `/about` — the Header and Footer don't unmount and remount. React Router's layouts also persist, but this is more explicit in Next.js.

### Locale-based layout (Nexus-specific)

Nexus uses `[locale]` as the first route segment for i18n:

```
app/
  [locale]/
    layout.tsx         — wraps all locale-specific routes
    page.tsx           — homepage (/)
    students/
      page.tsx         — /en/students, /si/students etc.
    about/
      page.tsx
```

```tsx
// app/[locale]/layout.tsx
export default function LocaleLayout({
  children,
  params,
}: {
  children: React.ReactNode;
  params: { locale: string };
}) {
  return (
    <div lang={params.locale} dir={params.locale === "ar" ? "rtl" : "ltr"}>
      <Header locale={params.locale} />
      {children}
      <Footer />
    </div>
  );
}
```

---

## 4. Navigation

### Links

```jsx
// React Router
import { Link, NavLink } from "react-router-dom";

<Link to="/students">Students</Link>
<NavLink to="/students" className={({ isActive }) => isActive ? "active" : ""}>
  Students
</NavLink>
```

```tsx
// Next.js
import Link from "next/link";

<Link href="/students">Students</Link>

// Active styling — no built-in isActive; use usePathname()
import { usePathname } from "next/navigation";

function NavLink({ href, children }: { href: string; children: React.ReactNode }) {
  const pathname = usePathname();
  const isActive = pathname === href || pathname.startsWith(href + "/");

  return (
    <Link
      href={href}
      className={isActive ? "nav-link nav-link-active" : "nav-link"}
    >
      {children}
    </Link>
  );
}
```

### Programmatic navigation

```jsx
// React Router
import { useNavigate } from "react-router-dom";

function LoginForm() {
  const navigate = useNavigate();

  async function handleLogin() {
    await login(credentials);
    navigate("/dashboard");
    navigate(-1);                          // go back
    navigate("/login", { replace: true }); // replace history entry
  }
}
```

```tsx
// Next.js — client component
"use client";
import { useRouter } from "next/navigation";

function LoginForm() {
  const router = useRouter();

  async function handleLogin() {
    await login(credentials);
    router.push("/dashboard");
    router.back();                  // go back
    router.replace("/dashboard");   // replace history entry
    router.refresh();               // re-fetch server component data
  }
}

// Next.js — server component / server action
import { redirect } from "next/navigation";

async function LoginAction(formData: FormData) {
  "use server";
  await login(formData);
  redirect("/dashboard");    // redirect after server action
}
```

---

## 5. Dynamic Routes and Params

```jsx
// React Router — :id in path string
<Route path="/students/:id" element={<StudentPage />} />

function StudentPage() {
  const { id } = useParams();
  // ...
}
```

```tsx
// Next.js — [id] folder name
// File: app/students/[id]/page.tsx

// Server Component — params come as props
export default async function StudentPage({
  params,
}: {
  params: { id: string };
}) {
  const student = await fetchStudent(params.id);
  return <StudentProfile student={student} />;
}

// Client Component — use useParams() hook
"use client";
import { useParams } from "next/navigation";

export default function StudentPage() {
  const params = useParams<{ id: string }>();
  // ...
}
```

### Catch-all routes

```
// React Router
<Route path="/docs/*" element={<DocsPage />} />
useParams() → { "*": "guides/installation" }

// Next.js file structure
app/docs/[...slug]/page.tsx        →  /docs/a/b/c
app/docs/[[...slug]]/page.tsx      →  /docs (optional — also matches /docs itself)

// Next.js params
params.slug  →  ["guides", "installation"]
```

---

## 6. Search Params

```jsx
// React Router
import { useSearchParams } from "react-router-dom";

function StudentsPage() {
  const [searchParams, setSearchParams] = useSearchParams();
  const grade = searchParams.get("grade") ?? "";

  function setGrade(value: string) {
    setSearchParams(prev => { prev.set("grade", value); return prev; });
  }
}
```

```tsx
// Next.js — server component
export default function StudentsPage({
  searchParams,
}: {
  searchParams: { grade?: string; page?: string };
}) {
  const grade = searchParams.grade ?? "";
  const page  = Number(searchParams.page ?? "1");
  // searchParams are read-only in server components — no setter
}

// Next.js — client component
"use client";
import { useSearchParams, useRouter, usePathname } from "next/navigation";

function StudentsFilter() {
  const searchParams = useSearchParams();
  const router       = useRouter();
  const pathname     = usePathname();

  const grade = searchParams.get("grade") ?? "";

  function setGrade(value: string) {
    const params = new URLSearchParams(searchParams.toString());
    if (value) params.set("grade", value);
    else params.delete("grade");
    router.push(`${pathname}?${params.toString()}`);
  }
}
```

---

## 7. Data Loading

This is the biggest difference. React Router loaders run in the browser. Next.js Server Components fetch on the server.

```jsx
// React Router — loader runs in the browser
const router = createBrowserRouter([
  {
    path: "/students/:id",
    element: <StudentPage />,
    loader: async ({ params }) => {
      // browser fetches from your API
      const res = await fetch(`/api/students/${params.id}`);
      return res.json();
    },
  },
]);

function StudentPage() {
  const student = useLoaderData();
  return <StudentProfile student={student} />;
}
```

```tsx
// Next.js — Server Component fetches directly, no API needed
// File: app/students/[id]/page.tsx
import { prisma } from "@/lib/prisma";

export default async function StudentPage({
  params,
}: {
  params: { id: string };
}) {
  // Runs on the server — direct database access
  const student = await prisma.student.findUnique({
    where: { id: params.id },
    include: { grades: true, activities: true },
  });

  if (!student) notFound();

  return <StudentProfile student={student} />;
}
```

**The mental shift:** In Next.js, Server Component pages _are_ the loader. There's no separate function. The `async` page component fetches its own data directly, and that code never runs in the browser.

### Parallel data fetching

```tsx
// Next.js — fetch in parallel with Promise.all
export default async function DashboardPage() {
  // These run in parallel — not waterfall
  const [students, announcements, events] = await Promise.all([
    prisma.student.findMany({ take: 10 }),
    prisma.announcement.findMany({ take: 5 }),
    prisma.event.findMany({ take: 5 }),
  ]);

  return (
    <div>
      <StudentsList students={students} />
      <Announcements items={announcements} />
      <EventsCalendar events={events} />
    </div>
  );
}
```

---

## 8. Loading and Error States

```jsx
// React Router — errorElement in route config
const router = createBrowserRouter([
  {
    path: "/students",
    element: <StudentsPage />,
    errorElement: <StudentsError />,
    loader: studentsLoader,
  },
]);
```

```tsx
// Next.js — special files in the route folder

// app/students/loading.tsx — shown while page.tsx is loading
// (automatically wraps in <Suspense>)
export default function StudentsLoading() {
  return <StudentGridSkeleton />;
}

// app/students/error.tsx — shown when page.tsx throws
// (automatically wraps in <ErrorBoundary>)
// Must be a Client Component
"use client";

export default function StudentsError({
  error,
  reset,
}: {
  error: Error & { digest?: string };
  reset: () => void;
}) {
  return (
    <div className="error-state">
      <h2>Failed to load students</h2>
      <p>{error.message}</p>
      <button onClick={reset}>Try again</button>
    </div>
  );
}

// app/students/not-found.tsx — shown when notFound() is called
export default function StudentNotFound() {
  return (
    <div>
      <h2>Student not found</h2>
      <Link href="/students">Back to students</Link>
    </div>
  );
}
```

The file convention means you never have to manually wrap with `<Suspense>` or `<ErrorBoundary>` for page-level states — Next.js does it automatically.

---

## 9. Protected Routes

```jsx
// React Router — component-level guard
function RequireAuth({ children }) {
  const { user } = useAuth();
  const location = useLocation();

  if (!user) return <Navigate to="/login" state={{ from: location }} replace />;
  return children;
}

<Route path="/dashboard" element={<RequireAuth><Dashboard /></RequireAuth>} />
```

```tsx
// Next.js — middleware (runs on edge, before the page loads)
// middleware.ts (at project root, not in app/)
import { NextResponse } from "next/server";
import type { NextRequest } from "next/server";
import { getSessionFromCookie } from "@/lib/auth";

export async function middleware(request: NextRequest) {
  const session = await getSessionFromCookie(request);
  const isAuthRoute = request.nextUrl.pathname.startsWith("/dashboard");

  if (isAuthRoute && !session) {
    const loginUrl = new URL("/login", request.url);
    loginUrl.searchParams.set("from", request.nextUrl.pathname);
    return NextResponse.redirect(loginUrl);
  }

  return NextResponse.next();
}

export const config = {
  matcher: ["/dashboard/:path*", "/admin/:path*"],
};

// Alternative — server-side redirect inside the page
// app/dashboard/page.tsx
export default async function DashboardPage() {
  const session = await getSession();

  if (!session) {
    redirect("/login");   // server-side redirect — no flash
  }

  return <Dashboard user={session.user} />;
}
```

Middleware runs before the page code — no flash of unauthenticated content. The server component redirect is a fallback for when middleware isn't enough.

---

## 10. Side-by-Side Reference

```
Concept                  React Router (v6)              Next.js App Router
──────────────────────────────────────────────────────────────────────────
Route definition         <Route path="..." />           app/path/page.tsx
Dynamic segment          :id in path string             [id] folder name
Catch-all                *                              [...slug]
Shared layout            Parent route + <Outlet />      layout.tsx
Active link              <NavLink isActive>             usePathname() + custom
Navigate (declarative)   <Link to="...">                <Link href="...">
Navigate (programmatic)  useNavigate()                  useRouter().push()
Go back                  navigate(-1)                   router.back()
Replace history          navigate(x, {replace:true})    router.replace()
Server redirect          N/A (everything is client)     redirect() from next/navigation
Read route params        useParams()                    params prop (server) / useParams() (client)
Read search params       useSearchParams()              searchParams prop (server) / useSearchParams() (client)
Update search params     setSearchParams()              router.push with new URLSearchParams
Data loading             loader: async ({params}) =>    async page component (Server Component)
Access loaded data       useLoaderData()                direct variable (fetched in same component)
Loading state            pending state in loader        loading.tsx (auto Suspense)
Error state              errorElement                   error.tsx (auto ErrorBoundary)
Not found                path="*" catch-all             not-found.tsx + notFound()
Auth guard               <RequireAuth> wrapper          middleware.ts + redirect()
```

---

## 11. When to Use Each

**Use React Router when:**

- Building a pure client-side SPA (no server rendering needed)
- The app doesn't need SEO (dashboards, internal tools, admin panels)
- You need full control over the routing logic in JavaScript
- You want to embed a routed React app inside an existing non-Next.js server

**Use Next.js App Router when:**

- You need SEO (public-facing pages, school website, marketing pages)
- You need server-side data access (database, private APIs, auth tokens)
- You want automatic code-splitting and streaming
- You're building a full-stack app (API routes alongside pages)
- **This is Nexus — always use App Router here**

**Knowing both matters because:**

- Much of the React ecosystem still uses React Router (Remix, Vite SPAs, CRA projects)
- Interviews will ask about both
- Understanding React Router makes Next.js routing feel intuitive — they're solving the same problem, just at different layers

---

## Summary

```
Core difference:
  React Router:     library, JS config, runs in browser
  Next.js App Router: framework-native, file system, runs on server + client

File-system routing:
  app/students/page.tsx         →  /students
  app/students/[id]/page.tsx    →  /students/:id
  app/[locale]/page.tsx         →  /:locale (Nexus pattern)

Special files:
  page.tsx      — the page UI
  layout.tsx    — persistent wrapper (like parent route + <Outlet />)
  loading.tsx   — auto Suspense fallback
  error.tsx     — auto ErrorBoundary (must be "use client")
  not-found.tsx — shown when notFound() called

Navigation:
  <Link href="/path">           ← Next.js (import from "next/link")
  useRouter().push()            ← programmatic (client components)
  redirect()                    ← server components / server actions
  usePathname()                 ← for active link detection

Data loading:
  React Router: loader fn → useLoaderData()
  Next.js: async page component → direct await (no API needed)
  Next.js: Promise.all([...]) for parallel fetches

Auth:
  React Router: <RequireAuth> wrapper component
  Next.js: middleware.ts runs before page, redirect() in server components

Key mental shift for Nexus:
  Server Components are async functions — await database directly
  No need for useEffect + fetch — fetch happens before HTML is sent
  "use client" is the opt-in for interactivity, not the default
```

---

_Next: [25 — Suspense & Error Boundaries](https://claude.ai/chat/25%20-%20Suspense%20%26%20Error%20Boundaries.md)_