> Server Components handle data. Client Components handle interaction. Knowing where to draw the line — and how to keep Client Components small and push them to the leaves of the tree — is what separates well-architected Next.js apps from ones that accidentally ship everything to the browser.

---

## Table of Contents

1. [When You Need a Client Component](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-when-you-need-a-client-component)
2. [The "use client" Directive](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-the-use-client-directive)
3. [Pushing Client Components to the Leaves](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-pushing-client-components-to-the-leaves)
4. [Client Component Patterns](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-client-component-patterns)
5. [Navigation Hooks](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-navigation-hooks)
6. [Context in the App Router](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-context-in-the-app-router)
7. [Client-Side Data Fetching](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-client-side-data-fetching)
8. [Common Mistakes](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-common-mistakes)

---

## 1. When You Need a Client Component

The default is Server Component. You reach for `"use client"` only when you genuinely need something that only exists in the browser.

```
Use "use client" when you need:
  ✅ useState, useReducer (component state)
  ✅ useEffect (side effects, subscriptions, timers)
  ✅ useRef for DOM manipulation or instance values
  ✅ onClick, onChange, onSubmit event handlers
  ✅ Browser APIs: window, document, localStorage, navigator
  ✅ Third-party libraries that require the DOM (charts, maps, rich text editors)
  ✅ Framer Motion animations (uses DOM APIs)
  ✅ useRouter, usePathname, useSearchParams (navigation hooks)

Do NOT use "use client" for:
  ❌ Just to use async/await (Server Components can do that)
  ❌ Just to avoid TypeScript errors
  ❌ Components that only display data without interaction
  ❌ Fetching data (do it in the parent Server Component, pass as props)
```

---

## 2. The "use client" Directive

`"use client"` goes at the very top of the file, before any imports:

```tsx
"use client";

import { useState } from "react";
import { motion } from "framer-motion";

export default function SearchBar({ initialQuery }: { initialQuery: string }) {
  const [query, setQuery] = useState(initialQuery);

  return (
    <motion.div initial={{ opacity: 0 }} animate={{ opacity: 1 }}>
      <input
        value={query}
        onChange={(e) => setQuery(e.target.value)}
        placeholder="Search..."
      />
    </motion.div>
  );
}
```

### "use client" marks a boundary, not a single component

`"use client"` declares a boundary between server and client code. Every component imported into a `"use client"` file is also treated as a Client Component — even if it doesn't have the directive itself.

```tsx
// components/SearchBar.tsx
"use client";

import SearchIcon from "./SearchIcon";    // becomes a Client Component (in this context)
import ClearButton from "./ClearButton";  // becomes a Client Component (in this context)

export default function SearchBar() { ... }
```

`SearchIcon` and `ClearButton` don't have `"use client"`, but because they're imported into a file that does, they run on the client when rendered from here. If `SearchIcon` is also imported directly from a Server Component elsewhere, it still runs as a Server Component there. The boundary is per import chain, not per file.

---

## 3. Pushing Client Components to the Leaves

The most important architectural rule: keep `"use client"` as deep in the tree as possible. Don't put it on a large component that wraps everything — extract the interactive bit into its own small Client Component.

### Wrong — client boundary too high

```tsx
// ❌ Bad — entire page is a Client Component because of one interactive element
"use client";

import { useState } from "react";

export default function StudentsPage() {
  const [searchQuery, setSearchQuery] = useState("");

  // All of this data fetching now happens client-side
  // All of this JSX is in the browser bundle
  return (
    <main>
      <h1>Students</h1>
      <input
        value={searchQuery}
        onChange={(e) => setSearchQuery(e.target.value)}
      />
      <HeroSection />
      <StudentGrid />
      <Footer />
    </main>
  );
}
```

### Right — extract the interactive part

```tsx
// ✅ Good — only the search input is a Client Component
// app/[locale]/students/page.tsx — Server Component
import { prisma } from "@/lib/prisma";
import StudentSearchInput from "./StudentSearchInput"; // Client Component
import StudentGrid from "./StudentGrid"; // Server Component

export default async function StudentsPage({ searchParams }) {
  const query = searchParams.q ?? "";
  const students = await prisma.student.findMany({
    where: query
      ? { name: { contains: query, mode: "insensitive" } }
      : undefined,
    orderBy: { name: "asc" },
  });

  return (
    <main>
      <h1>Students</h1>
      <StudentSearchInput defaultValue={query} /> {/* tiny Client Component */}
      <StudentGrid students={students} /> {/* Server Component */}
    </main>
  );
}
```

```tsx
// components/StudentSearchInput.tsx
"use client";

import { useRouter, usePathname } from "next/navigation";
import { useDebouncedCallback } from "use-debounce";

export default function StudentSearchInput({
  defaultValue,
}: {
  defaultValue: string;
}) {
  const router = useRouter();
  const pathname = usePathname();

  const search = useDebouncedCallback((value: string) => {
    const params = new URLSearchParams();
    if (value) params.set("q", value);
    router.push(`${pathname}?${params.toString()}`);
  }, 300);

  return (
    <input
      defaultValue={defaultValue}
      onChange={(e) => search(e.target.value)}
      placeholder="Search students..."
      className="search-input"
    />
  );
}
```

The search input is one small file with `"use client"`. The page, grid, and everything else stays on the server.

---

## 4. Client Component Patterns

### Toggle / accordion

```tsx
"use client";

import { useState } from "react";
import { ChevronDown } from "lucide-react";

export default function Accordion({
  title,
  children,
}: {
  title: string;
  children: React.ReactNode;
}) {
  const [isOpen, setIsOpen] = useState(false);

  return (
    <div className="border rounded-lg">
      <button
        onClick={() => setIsOpen(!isOpen)}
        className="flex items-center justify-between w-full px-4 py-3"
        aria-expanded={isOpen}
      >
        <span className="font-medium">{title}</span>
        <ChevronDown
          className={`transition-transform ${isOpen ? "rotate-180" : ""}`}
          size={16}
        />
      </button>
      {isOpen && <div className="px-4 pb-4">{children}</div>}
    </div>
  );
}
```

Note: `children` can be a Server Component subtree passed down from a server parent. The children render on the server; the toggle logic runs on the client.

### Form with optimistic state

```tsx
"use client";

import { useState, useTransition } from "react";
import { submitContactForm } from "@/app/actions/contact"; // Server Action

export default function ContactForm() {
  const [isPending, startTransition] = useTransition();
  const [submitted, setSubmitted] = useState(false);

  function handleSubmit(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();
    const data = new FormData(e.currentTarget);

    startTransition(async () => {
      await submitContactForm(data);
      setSubmitted(true);
    });
  }

  if (submitted) {
    return (
      <div className="success-state">
        <p>Thank you! We'll be in touch.</p>
      </div>
    );
  }

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      <input name="name" type="text" placeholder="Your name" required />
      <input name="email" type="email" placeholder="Email address" required />
      <textarea name="message" placeholder="Your message" required />
      <button type="submit" disabled={isPending}>
        {isPending ? "Sending..." : "Send Message"}
      </button>
    </form>
  );
}
```

### Sticky header with scroll detection

```tsx
"use client";

import { useState, useEffect } from "react";

export default function StickyHeader({
  children,
}: {
  children: React.ReactNode;
}) {
  const [scrolled, setScrolled] = useState(false);

  useEffect(() => {
    function handleScroll() {
      setScrolled(window.scrollY > 60);
    }

    window.addEventListener("scroll", handleScroll, { passive: true });
    return () => window.removeEventListener("scroll", handleScroll);
  }, []);

  return (
    <header
      className={`
        fixed top-0 left-0 right-0 z-50 transition-all duration-200
        ${scrolled ? "bg-background shadow-sm" : "bg-transparent"}
      `}
    >
      {children}
    </header>
  );
}
```

The `children` (nav links, logo) are rendered from a Server Component parent. Only the scroll detection logic is client-side.

---

## 5. Navigation Hooks

These hooks require `"use client"` — they access browser navigation state.

### useRouter

```tsx
"use client";
import { useRouter } from "next/navigation";

export default function BackButton() {
  const router = useRouter();

  return <button onClick={() => router.back()}>← Back</button>;
}
```

```tsx
// Common router methods
router.push("/students"); // navigate to path
router.push("/students?grade=9"); // with query string
router.replace("/login"); // navigate without adding to history
router.back(); // go back
router.forward(); // go forward
router.refresh(); // re-fetch server component data for current page
router.prefetch("/students"); // prefetch a route (happens automatically on <Link> hover)
```

### usePathname

```tsx
"use client";
import Link from "next/link";
import { usePathname } from "next/navigation";

const navItems = [
  { label: "Home", href: "/" },
  { label: "Students", href: "/students" },
  { label: "News", href: "/news" },
  { label: "About", href: "/about" },
];

export default function NavLinks({ locale }: { locale: string }) {
  const pathname = usePathname();

  return (
    <nav className="flex gap-6">
      {navItems.map((item) => {
        const href = `/${locale}${item.href}`;
        const isActive = pathname === href || pathname.startsWith(href + "/");

        return (
          <Link
            key={item.href}
            href={href}
            className={isActive ? "nav-link nav-link--active" : "nav-link"}
            aria-current={isActive ? "page" : undefined}
          >
            {item.label}
          </Link>
        );
      })}
    </nav>
  );
}
```

### useSearchParams

```tsx
"use client";
import { useSearchParams } from "next/navigation";

export default function GradeFilter({ grades }: { grades: number[] }) {
  const searchParams = useSearchParams();
  const activeGrade = searchParams.get("grade");

  return (
    <div className="flex gap-2">
      {grades.map((grade) => (
        <Link
          key={grade}
          href={`?grade=${grade}`}
          className={
            activeGrade === String(grade) ? "chip chip--active" : "chip"
          }
        >
          Grade {grade}
        </Link>
      ))}
    </div>
  );
}
```

### useParams

```tsx
"use client";
import { useParams } from "next/navigation";

export default function LocaleSwitcher() {
  const params = useParams<{ locale: string }>();

  return (
    <div className="locale-switcher">
      <Link
        href={`/en`}
        aria-current={params.locale === "en" ? "true" : undefined}
      >
        English
      </Link>
      <Link
        href={`/si`}
        aria-current={params.locale === "si" ? "true" : undefined}
      >
        සිංහල
      </Link>
    </div>
  );
}
```

---

## 6. Context in the App Router

React Context requires a Provider, which needs to be a Client Component. But providers typically wrap large parts of the tree — you don't want to make your entire `layout.tsx` a Client Component.

The solution: extract the provider into its own Client Component, then use it in the Server Component layout.

```tsx
// components/providers/ThemeProvider.tsx
"use client";

import { createContext, useContext, useState } from "react";

type Theme = "light" | "dark";

const ThemeContext = createContext<{
  theme: Theme;
  setTheme: (theme: Theme) => void;
} | null>(null);

export function ThemeProvider({ children }: { children: React.ReactNode }) {
  const [theme, setTheme] = useState<Theme>("light");

  return (
    <ThemeContext.Provider value={{ theme, setTheme }}>
      <div data-theme={theme}>{children}</div>
    </ThemeContext.Provider>
  );
}

export function useTheme() {
  const ctx = useContext(ThemeContext);
  if (!ctx) throw new Error("useTheme must be used inside ThemeProvider");
  return ctx;
}
```

```tsx
// app/layout.tsx — Server Component — imports and uses the Client provider
import { ThemeProvider } from "@/components/providers/ThemeProvider";

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body>
        <ThemeProvider>
          {children}{" "}
          {/* Server Components render fine inside Client providers */}
        </ThemeProvider>
      </body>
    </html>
  );
}
```

The layout stays a Server Component. The Provider is a Client Component. The `children` — pages, other Server Components — are passed through and render on the server. The provider only adds the context wrapper.

### Multiple providers

```tsx
// components/providers/index.tsx — combine all providers here
"use client";

import { ThemeProvider } from "./ThemeProvider";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { useState } from "react";

export function Providers({ children }: { children: React.ReactNode }) {
  const [queryClient] = useState(() => new QueryClient());

  return (
    <QueryClientProvider client={queryClient}>
      <ThemeProvider>{children}</ThemeProvider>
    </QueryClientProvider>
  );
}
```

```tsx
// app/layout.tsx
import { Providers } from "@/components/providers";

export default function RootLayout({ children }) {
  return (
    <html>
      <body>
        <Providers>{children}</Providers>
      </body>
    </html>
  );
}
```

---

## 7. Client-Side Data Fetching

Most data fetching should happen in Server Components. But sometimes you need client-side fetching — for data that changes based on user interaction without a full page navigation, or for real-time updates.

Use TanStack Query (covered in React File 16) for client-side fetching in Next.js:

```tsx
"use client";

import { useQuery } from "@tanstack/react-query";

// For client-side fetching, call your API routes
// Don't call Prisma directly — that's server-only
export default function LiveAttendance({ classId }: { classId: string }) {
  const { data, isLoading } = useQuery({
    queryKey: ["attendance", classId],
    queryFn: () =>
      fetch(`/api/classes/${classId}/attendance`).then((r) => r.json()),
    refetchInterval: 30_000, // poll every 30 seconds
  });

  if (isLoading) return <AttendanceSkeleton />;

  return <AttendanceDisplay data={data} />;
}
```

**Server fetching vs client fetching — choose by the question:**

```
"Does this data exist when the page first loads?"
  → Yes (student list, article content, etc.) → Server Component fetch

"Does this data change based on user interaction without navigation?"
  → Yes (search results as user types, filters) → URL search params + Server re-render
     or Client Component + TanStack Query if navigation isn't appropriate

"Is this real-time or polling?" (live attendance, notifications)
  → Client Component + TanStack Query with refetchInterval

"Does it depend on browser state?" (geolocation, camera, microphone)
  → Client Component
```

---

## 8. Common Mistakes

### Adding "use client" to everything

```tsx
// ❌ Wrong — not everything needs to be a Client Component
"use client";
export default function StudentCard({ student }) {
  return <div>{student.name}</div>; // no hooks, no events — doesn't need client
}

// ✅ Right
export default function StudentCard({ student }) {
  return <div>{student.name}</div>; // Server Component — smaller bundle
}
```

### Importing server-only code into a Client Component

```tsx
// ❌ Wrong — will fail or leak secrets
"use client";
import { prisma } from "@/lib/prisma"; // server-only — build error or runtime crash

export default function StudentList() {
  // prisma can't run in the browser
}

// ✅ Right — fetch from an API route or receive data as props from a Server Component
("use client");
export default function StudentList({ students }: { students: Student[] }) {
  // data came from the server, delivered as props
}
```

### Creating a new QueryClient inside a Client Component (not in useState)

```tsx
// ❌ Wrong — creates a new client on every render
"use client";
const queryClient = new QueryClient(); // module-level is fine for some setups...

export function Providers({ children }) {
  const queryClient = new QueryClient(); // ❌ new instance every render
  return (
    <QueryClientProvider client={queryClient}>{children}</QueryClientProvider>
  );
}

// ✅ Right — stable instance using useState
("use client");
export function Providers({ children }) {
  const [queryClient] = useState(() => new QueryClient()); // created once
  return (
    <QueryClientProvider client={queryClient}>{children}</QueryClientProvider>
  );
}
```

### Using useSearchParams without Suspense

`useSearchParams()` requires a `<Suspense>` boundary around the component in Next.js — it reads from a context that may not be available during static rendering:

```tsx
// ❌ Causes build warning / unexpected behavior
"use client";
import { useSearchParams } from "next/navigation";

export default function FilterBar() {
  const searchParams = useSearchParams(); // needs Suspense
  // ...
}

// ✅ Wrap the component in Suspense at its usage site
<Suspense fallback={<FilterBarSkeleton />}>
  <FilterBar />
</Suspense>;
```

---

## Summary

```
"use client" is an opt-in for browser capabilities:
  Use it when you need: state, effects, event handlers, browser APIs
  Do NOT add it by default — most components don't need it

"use client" marks a boundary:
  All components imported into a "use client" file also run on the client
  Keep boundaries deep in the tree — don't put them on large wrapper components

Push to the leaves:
  Extract only the interactive part into a Client Component
  Leave the data-fetching and static rendering in Server Components
  Typical pattern: async Server Component fetches → passes data to small Client Component

Navigation hooks (all require "use client"):
  useRouter()        — push, replace, back, refresh
  usePathname()      — current URL path (for active link styling)
  useSearchParams()  — read query string (wrap in Suspense)
  useParams()        — read dynamic route segments

Context in App Router:
  Provider must be a Client Component
  Extract provider into its own file with "use client"
  Import and use it in your Server Component layout
  children pass through the provider and remain Server Components

Client-side data fetching:
  Use TanStack Query
  Call API routes — never import Prisma in a Client Component
  Default to Server Components; only use client fetching for live/interactive data
```

---

_Next: [05 — Server Actions & Mutations](https://claude.ai/chat/05%20-%20Server%20Actions%20%26%20Mutations.md)_
