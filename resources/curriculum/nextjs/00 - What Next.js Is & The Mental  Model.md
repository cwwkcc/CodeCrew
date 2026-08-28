> You know React. React renders UI in the browser — you get one HTML file, a big JavaScript bundle, and the browser does everything. Next.js keeps React but moves a significant portion of work to the server. Understanding _why_ that matters, and _how_ Next.js thinks about the boundary between server and client, is the mental model you need before anything else.

---

## Table of Contents

1. [The Problem Next.js Solves](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-problem-nextjs-solves)
2. [What Next.js Actually Is](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-what-nextjs-actually-is)
3. [Server Components vs Client Components](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-server-components-vs-client-components)
4. [The Rendering Models](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-the-rendering-models)
5. [The App Router](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-the-app-router)
6. [Next.js vs Vite + React](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-nextjs-vs-vite--react)
7. [Project Nexus Context](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-project-nexus-context)
8. [Your First Next.js App](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-your-first-nextjs-app)

---

## 1. The Problem Next.js Solves

A React SPA (what you build with Vite) works like this:

```
Browser requests /students
  → Server sends one empty HTML file + JS bundle
  → Browser downloads and parses JS (takes time)
  → React renders the page in the browser
  → Browser fetches data from API
  → React re-renders with data
```

This has real consequences:

**SEO is broken.** Google's crawler sees an empty HTML page. By the time JavaScript runs and renders content, the crawler may have already moved on. School websites need to be found on Google.

**Slow initial load.** Users see nothing (or a spinner) until all the JS has downloaded, parsed, and run — even for content that never changes, like a school's About page.

**Data is public.** Your database credentials, API keys, and query logic have to live in the browser bundle or go through a separate API layer. There's no way to run server-only code.

```
Next.js request to /students
  → Server runs the React component — fetches data directly from database
  → Server sends complete, rendered HTML
  → Browser displays content immediately (fast, SEO-visible)
  → React "hydrates" — attaches event listeners to the existing HTML
  → Page is now interactive
```

The same React components, but part of the work moves to the server.

---

## 2. What Next.js Actually Is

Next.js is a **React framework** — it wraps React and adds:

```
File-system routing     — folder structure = URL structure, no <Route> config
Server Components       — React components that run on the server, never sent to browser
API Routes              — backend endpoints inside the same project
Server Actions          — server-side functions called directly from components
Streaming               — send HTML progressively as it's ready, don't wait for all data
Image optimisation      — automatic WebP conversion, lazy loading, size optimisation
Font optimisation       — self-host Google Fonts with zero layout shift
Middleware              — run code before a request hits a route (auth, redirects, i18n)
Built-in TypeScript     — zero config TypeScript support
```

It's a full-stack framework. One project can have your React UI, your database logic, and your API — no separate Express server needed.

---

## 3. Server Components vs Client Components

This is the most important concept in Next.js App Router. Everything else follows from it.

### Server Components (default)

```tsx
// app/students/page.tsx
// No "use client" directive = Server Component
import { prisma } from "@/lib/prisma";

export default async function StudentsPage() {
  // This runs on the server. prisma is never in the browser bundle.
  // Database credentials never reach the client.
  const students = await prisma.student.findMany();

  return (
    <ul>
      {students.map((s) => (
        <li key={s.id}>{s.name}</li>
      ))}
    </ul>
  );
}
```

Server Components:

- Run only on the server (never in the browser)
- Can be `async` — `await` anything directly
- Can access databases, file system, secrets
- Their code is never sent to the browser — zero bundle cost
- **Cannot** use state (`useState`), effects (`useEffect`), or browser APIs
- **Cannot** attach event handlers (`onClick`, `onChange`)

### Client Components

```tsx
// "use client" at the top — this is a Client Component
"use client";

import { useState } from "react";

export default function SearchBar({
  onSearch,
}: {
  onSearch: (q: string) => void;
}) {
  const [query, setQuery] = useState("");

  return (
    <input
      value={query}
      onChange={(e) => setQuery(e.target.value)}
      onKeyDown={(e) => e.key === "Enter" && onSearch(query)}
      placeholder="Search students..."
    />
  );
}
```

Client Components:

- Run in the browser (and also pre-rendered on the server for the initial HTML)
- Can use hooks: `useState`, `useEffect`, `useRef`, etc.
- Can attach event handlers
- Their code IS sent to the browser — they cost bundle size
- **Cannot** directly access databases or secrets

### The key insight

```
Default = Server Component
"use client" = opt in to browser capabilities

Not: "server components are special"
Rather: "client components are the opt-in"
```

Most of your code should be Server Components. Only reach for `"use client"` when you specifically need interactivity, browser APIs, or React hooks.

### Composing them

Server Components can render Client Components. Client Components cannot render Server Components (they're already in the browser — the server is gone).

```tsx
// app/students/page.tsx — Server Component
import { prisma } from "@/lib/prisma";
import SearchBar from "./SearchBar";       // Client Component
import StudentGrid from "./StudentGrid";   // Server Component

export default async function StudentsPage() {
  const students = await prisma.student.findMany();

  return (
    <div>
      <SearchBar onSearch={...} />     {/* Client — needs interactivity */}
      <StudentGrid students={students} /> {/* Server — just renders data */}
    </div>
  );
}
```

Pass server-fetched data to client components via props. The data crosses the server/client boundary as serialised JSON.

---

## 4. The Rendering Models

Next.js has several strategies for when and where components render. Understanding them helps you choose the right one.

### Static Rendering (default for pages without dynamic data)

```tsx
// This page has no dynamic data — Next.js renders it once at build time
// and serves the same HTML to every visitor
export default function AboutPage() {
  return (
    <div>
      <h1>About C.W.W. Kannangara Central College</h1>
      <p>Founded in 1953...</p>
    </div>
  );
}
```

Result: A static HTML file. Fastest possible delivery — served directly from a CDN with no server involved per request.

### Dynamic Rendering (per-request)

```tsx
import { cookies } from "next/headers";

// Reading cookies, headers, or searchParams makes a page dynamic —
// Next.js renders it fresh for every request
export default async function DashboardPage() {
  const cookieStore = cookies();
  const session = cookieStore.get("session");

  const user = await getUserFromSession(session?.value);
  return <Dashboard user={user} />;
}
```

### Incremental Static Regeneration (ISR)

```tsx
// Render statically, but re-render in the background every N seconds
export const revalidate = 60; // re-generate this page every 60 seconds

export default async function NewsPage() {
  const news = await fetchLatestNews();
  return <NewsList items={news} />;
}
```

Good for content that changes occasionally (news, events, announcements) — you get static performance with relatively fresh data.

### Streaming

```tsx
import { Suspense } from "react";
import StudentGrid from "./StudentGrid";
import AnnouncementSidebar from "./AnnouncementSidebar";

// The shell renders and streams immediately.
// StudentGrid and AnnouncementSidebar stream in as their data resolves.
export default function HomePage() {
  return (
    <main>
      <HeroSection /> {/* sends immediately */}
      <Suspense fallback={<GridSkeleton />}>
        <StudentGrid /> {/* streams in when DB query resolves */}
      </Suspense>
      <Suspense fallback={<SidebarSkeleton />}>
        <AnnouncementSidebar /> {/* streams in independently */}
      </Suspense>
    </main>
  );
}
```

The browser gets and renders the fast parts immediately. Slow parts stream in as they're ready — no waiting for the slowest query.

---

## 5. The App Router

Next.js has two routers: the legacy Pages Router (`pages/` directory) and the modern App Router (`app/` directory). **App Router is current — this entire module covers App Router only.**

The App Router is file-system based:

```
app/
  layout.tsx           →  root layout (wraps everything)
  page.tsx             →  / (homepage)
  globals.css

  students/
    layout.tsx         →  layout for all /students/* routes
    page.tsx           →  /students
    loading.tsx        →  shown while /students page loads
    error.tsx          →  shown if /students page throws
    [id]/
      page.tsx         →  /students/123

  about/
    page.tsx           →  /about

  api/
    students/
      route.ts         →  POST/GET /api/students (API endpoint)

  [locale]/            →  /en/*, /si/* (Nexus i18n pattern)
    page.tsx
    layout.tsx
```

Special file names in each folder:

- `page.tsx` — the page UI (makes the route publicly accessible)
- `layout.tsx` — wraps this segment and all children (persists across navigation)
- `loading.tsx` — automatic Suspense fallback while page loads
- `error.tsx` — automatic Error Boundary when page throws
- `not-found.tsx` — rendered when `notFound()` is called
- `route.ts` — API endpoint (no UI)

Full detail in Files 02 (routing) and 03 (layouts).

---

## 6. Next.js vs Vite + React

```
                    Vite + React SPA          Next.js App Router
─────────────────────────────────────────────────────────────────
Rendering           Browser only              Server + Browser
Initial load        Empty HTML + JS bundle    Full HTML from server
SEO                 Poor (JS must run first)  Excellent (HTML ready)
Data fetching       useEffect / TanStack      async Server Components
Database access     Via separate API          Direct (Server Components)
Bundle size         Everything in browser     Server code excluded
Routing             React Router              File system
TypeScript          Manual setup              Built-in
Image optimisation  Manual                    Built-in <Image />
Good for            Dashboards, SPAs          Public sites, full-stack apps
```

**When to use Vite + React instead of Next.js:**

- Internal admin dashboard (no SEO needed)
- App behind a login wall (no public content)
- Embedding into an existing server setup
- You need 100% control over the build process

**When to use Next.js:**

- Public-facing website (SEO matters) → Nexus
- Full-stack app with database access
- Mixed public + authenticated pages
- You want streaming, ISR, image optimisation out of the box

---

## 7. Project Nexus Context

Nexus is a Next.js App Router project. Every concept in this module applies directly.

```
apps/web/
  src/
    app/
      [locale]/           — all routes are under a locale segment (/en/*, /si/*)
        layout.tsx        — locale-aware root layout
        page.tsx          — homepage
        students/
          page.tsx        — student listings (Server Component, fetches from Prisma)
        news/
          page.tsx
          [slug]/
            page.tsx      — individual news article
        admin/
          layout.tsx      — admin-only layout with auth check
          dashboard/
            page.tsx
    components/
      ui/                 — design system atoms (mostly Server Components)
      features/           — feature components (mix of server and client)
    lib/
      prisma.ts           — Prisma client (server-only)
      auth.ts             — auth utilities (server-only)
```

The `[locale]` segment at the root is Nexus's i18n pattern — all pages live under `/en/` or `/si/` (Sinhala). The layout for `[locale]` handles setting the right `lang` attribute and loading the right translations.

---

## 8. Your First Next.js App

```bash
npx create-next-app@latest my-app
```

You'll be asked:

```
Would you like to use TypeScript?  → Yes
Would you like to use ESLint?      → Yes
Would you like to use Tailwind CSS? → Yes
Would you like to use the src/ directory? → Yes
Would you like to use App Router?  → Yes  ← critical
Would you like to customise the default import alias? → No (@ is fine)
```

The generated structure:

```
my-app/
  src/
    app/
      layout.tsx     — root layout with <html> and <body>
      page.tsx       — homepage
      globals.css    — global styles
  public/            — static files (images, fonts)
  next.config.ts     — Next.js configuration
  tailwind.config.ts — Tailwind configuration
  tsconfig.json      — TypeScript configuration
  package.json
```

```bash
npm run dev      # start development server on localhost:3000
npm run build    # production build
npm run start    # serve the production build
```

The root layout — every page goes through this:

```tsx
// src/app/layout.tsx
import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "My App",
  description: "Built with Next.js",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className={inter.className}>{children}</body>
    </html>
  );
}
```

The homepage:

```tsx
// src/app/page.tsx
export default function HomePage() {
  return (
    <main>
      <h1>Hello, Next.js</h1>
    </main>
  );
}
```

---

## Summary

```
The problem:
  React SPAs have poor SEO, slow initial loads, and no server-side code execution.
  Next.js moves work to the server while keeping React as the component model.

Server Components (default — no directive needed):
  Run on the server only. Can be async. Direct DB access.
  Code never sent to browser. Cannot use hooks or event handlers.

Client Components ("use client" at top of file):
  Run in browser. Can use useState, useEffect, event handlers.
  Code IS sent to browser. Opt-in, not the default.

Mental model:
  Default = Server. "use client" = opt-in to browser capabilities.
  Keep as much as possible on the server.

Rendering models:
  Static    — rendered once at build time, served from CDN (fastest)
  Dynamic   — rendered fresh per request (cookies, headers, searchParams)
  ISR       — static but revalidates on a schedule (revalidate = 60)
  Streaming — shell renders immediately, slow parts stream in via Suspense

App Router:
  File system = routes. No <Route> config needed.
  Special files: page.tsx, layout.tsx, loading.tsx, error.tsx, not-found.tsx
  API endpoints: route.ts

Next.js vs Vite + React:
  Vite: dashboards, SPAs, no SEO needed
  Next.js: public sites, full-stack, SEO, mixed content — this is Nexus
```

---

_Next: [01 — File System Routing](https://claude.ai/chat/01%20-%20File%20System%20Routing.md)_
