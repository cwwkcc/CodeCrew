> Next.js navigation is faster than a browser page reload — it only fetches and swaps the changed part of the page, not the whole document. This file covers the `<Link>` component, programmatic navigation, prefetching, scroll behaviour, and the patterns you'll use for locale-aware navigation in Nexus.

---

## Table of Contents

1. [The Link Component](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-link-component)
2. [Prefetching](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-prefetching)
3. [Active Links](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-active-links)
4. [Programmatic Navigation](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-programmatic-navigation)
5. [Locale-Aware Navigation (Nexus)](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-locale-aware-navigation-nexus)
6. [Scroll Behaviour](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-scroll-behaviour)
7. [Loading UI During Navigation](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-loading-ui-during-navigation)

---

## 1. The Link Component

`<Link>` from `next/link` is the primary way to navigate. It renders an `<a>` tag but intercepts clicks for client-side navigation — no full page reload.

```tsx
import Link from "next/link";

// Basic usage
<Link href="/students">Students</Link>

// With query string
<Link href="/students?grade=9">Grade 9 Students</Link>

// With an object (cleaner for dynamic hrefs)
<Link href={{ pathname: "/students", query: { grade: 9, page: 2 } }}>
  Grade 9 — Page 2
</Link>

// Dynamic route
<Link href={`/students/${student.id}`}>{student.name}</Link>

// Open in new tab
<Link href="/about" target="_blank" rel="noopener noreferrer">
  About (new tab)
</Link>
```

`<Link>` works in both Server and Client Components — it's just an `<a>` tag with extra behaviour.

### Replace instead of push

```tsx
// Default: pushes to browser history — user can go back
<Link href="/students">Students</Link>

// Replace: replaces history entry — user can't go back to the previous page
<Link href="/login" replace>
  Login
</Link>
```

---

## 2. Prefetching

Next.js prefetches linked routes automatically when they appear in the viewport. By the time the user clicks, the destination page is already fetched and ready.

```tsx
// Prefetching is on by default in production
<Link href="/students">Students</Link>
// → Next.js fetches /students as soon as this link is visible

// Disable prefetching (e.g. for links to rarely-visited pages)
<Link href="/admin/reports/2019" prefetch={false}>
  2019 Report
</Link>

// Force prefetch even before viewport (not commonly needed)
<Link href="/students" prefetch={true}>
  Students
</Link>
```

**What gets prefetched:**

- Static pages: the full page
- Dynamic pages: up to the first loading boundary (the `loading.tsx` skeleton)

So clicking a link shows the skeleton immediately from prefetched data, then the real content streams in. Feels instant.

---

## 3. Active Links

`<Link>` has no built-in `isActive` prop (unlike React Router's `<NavLink>`). Use `usePathname()`:

```tsx
// components/NavLink.tsx
"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";

interface NavLinkProps {
  href: string;
  children: React.ReactNode;
  exact?: boolean; // true = only active on exact match, false = active for prefix too
}

export default function NavLink({
  href,
  children,
  exact = false,
}: NavLinkProps) {
  const pathname = usePathname();

  const isActive = exact
    ? pathname === href
    : pathname === href || pathname.startsWith(href + "/");

  return (
    <Link
      href={href}
      className={`nav-link ${isActive ? "nav-link--active" : ""}`}
      aria-current={isActive ? "page" : undefined}
    >
      {children}
    </Link>
  );
}
```

```tsx
// Usage — in a Server Component or Client Component
<NavLink href="/students">Students</NavLink>
<NavLink href="/news">News</NavLink>
<NavLink href="/" exact>Home</NavLink>  {/* only active on exact /  */}
```

### Locale-aware active link

In Nexus, all paths are prefixed with a locale (`/en/`, `/si/`). Strip the locale prefix for comparison:

```tsx
// components/NavLink.tsx (Nexus version)
"use client";

import Link from "next/link";
import { usePathname, useParams } from "next/navigation";

export default function NavLink({
  href,
  children,
}: {
  href: string;
  children: React.ReactNode;
}) {
  const pathname = usePathname();
  const params = useParams<{ locale: string }>();
  const locale = params.locale ?? "en";

  // Full href with locale prefix
  const localizedHref = `/${locale}${href}`;

  // Strip locale from pathname for comparison
  const pathWithoutLocale = pathname.replace(`/${locale}`, "") || "/";
  const isActive =
    pathWithoutLocale === href || pathWithoutLocale.startsWith(href + "/");

  return (
    <Link
      href={localizedHref}
      className={`nav-link ${isActive ? "nav-link--active" : ""}`}
      aria-current={isActive ? "page" : undefined}
    >
      {children}
    </Link>
  );
}
```

---

## 4. Programmatic Navigation

For navigating in response to events (form submissions, button clicks, etc.):

```tsx
"use client";

import { useRouter } from "next/navigation";

export default function SearchForm() {
  const router = useRouter();

  function handleSearch(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();
    const query = new FormData(e.currentTarget).get("q") as string;
    if (query.trim()) {
      router.push(`/search?q=${encodeURIComponent(query)}`);
    }
  }

  return (
    <form onSubmit={handleSearch}>
      <input name="q" type="search" placeholder="Search..." />
      <button type="submit">Search</button>
    </form>
  );
}
```

```tsx
// All router methods
router.push("/path"); // navigate, add to history
router.replace("/path"); // navigate, replace current history entry
router.back(); // go back (like browser back button)
router.forward(); // go forward
router.refresh(); // re-fetch server component data for current route
router.prefetch("/path"); // manually prefetch a route
```

### After a Server Action

In a Server Action, use `redirect()` — not `router.push()`:

```tsx
// actions/students.ts
"use server";

import { redirect } from "next/navigation";

export async function createStudent(formData: FormData) {
  const student = await prisma.student.create({ data: { ... } });
  revalidatePath("/students");
  redirect(`/students/${student.id}`);  // runs server-side — no router needed
}
```

---

## 5. Locale-Aware Navigation (Nexus)

Nexus routes are under `[locale]` — every internal link needs the locale prefix. There are a few strategies.

### Strategy 1 — Helper function

```tsx
// lib/navigation.ts
export function localePath(locale: string, path: string): string {
  return `/${locale}${path === "/" ? "" : path}`;
}

// Usage
<Link href={localePath(locale, "/students")}>Students</Link>
<Link href={localePath(locale, `/students/${id}`)}>View</Link>
```

### Strategy 2 — Locale-aware Link wrapper

```tsx
// components/LocaleLink.tsx — wraps next/link with locale prefix
import Link from "next/link";
import { useParams } from "next/navigation";

export default function LocaleLink({
  href,
  children,
  ...props
}: React.ComponentProps<typeof Link> & { href: string }) {
  const { locale } = useParams<{ locale: string }>();
  const localizedHref = `/${locale}${href.startsWith("/") ? href : "/" + href}`;

  return (
    <Link href={localizedHref} {...props}>
      {children}
    </Link>
  );
}
```

### Strategy 3 — next-intl navigation (what Nexus uses)

`next-intl` generates locale-aware navigation utilities for you:

```tsx
// navigation.ts (generated by next-intl setup)
import { createSharedPathnamesNavigation } from "next-intl/navigation";

export const locales = ["en", "si"] as const;

export const { Link, redirect, usePathname, useRouter } =
  createSharedPathnamesNavigation({ locales });
```

```tsx
// Now use Link from navigation.ts — auto-prefixes locale
import { Link } from "@/navigation";

<Link href="/students">Students</Link>;
// → renders as /en/students or /si/students based on current locale

// useRouter from navigation.ts — locale-aware push
import { useRouter } from "@/navigation";

const router = useRouter();
router.push("/students"); // → navigates to /en/students or /si/students
```

This is the cleanest approach — you write locale-free paths everywhere and the library handles prefixing.

### Language switcher

```tsx
// components/LanguageSwitcher.tsx
"use client";

import { usePathname, useRouter } from "@/navigation"; // next-intl navigation
import { useParams } from "next/navigation";

const localeLabels: Record<string, string> = {
  en: "English",
  si: "සිංහල",
};

export default function LanguageSwitcher() {
  const pathname = usePathname(); // path WITHOUT locale prefix
  const params = useParams<{ locale: string }>();
  const router = useRouter();

  function switchLocale(newLocale: string) {
    router.replace(pathname, { locale: newLocale });
    // Navigates to the same page in the new locale
  }

  return (
    <div className="locale-switcher">
      {(["en", "si"] as const).map((loc) => (
        <button
          key={loc}
          onClick={() => switchLocale(loc)}
          className={
            params.locale === loc
              ? "locale-btn locale-btn--active"
              : "locale-btn"
          }
          lang={loc}
        >
          {localeLabels[loc]}
        </button>
      ))}
    </div>
  );
}
```

---

## 6. Scroll Behaviour

By default, navigating to a new page scrolls to the top. Navigating back/forward restores the previous scroll position.

```tsx
// Disable scroll-to-top on navigation (e.g. pagination — stay at same position)
<Link href="/students?page=2" scroll={false}>
  Next page →
</Link>;

// Programmatic navigation without scroll reset
router.push("/students?page=2", { scroll: false });
```

### Scroll to a hash

```tsx
// Scrolls to the element with id="results" on the target page
<Link href="/about#history">Our History</Link>

// On the target page — the element must have the matching id
<section id="history">
  <h2>Our History</h2>
  ...
</section>
```

---

## 7. Loading UI During Navigation

When the user clicks a `<Link>`, there's a moment before the destination page's data is ready. There are two ways to show feedback:

### 1. loading.tsx (recommended)

Covered in File 02. The loading skeleton shows immediately on navigation. No extra code needed.

### 2. useTransition for programmatic navigation

```tsx
"use client";

import { useTransition } from "react";
import { useRouter } from "next/navigation";

export default function FilterButton({ grade }: { grade: number }) {
  const router = useRouter();
  const [isPending, startTransition] = useTransition();

  function applyFilter() {
    startTransition(() => {
      router.push(`/students?grade=${grade}`);
    });
  }

  return (
    <button
      onClick={applyFilter}
      disabled={isPending}
      className={isPending ? "btn btn--loading" : "btn"}
    >
      Grade {grade}
    </button>
  );
}
```

### 3. Global navigation progress bar (NProgress pattern)

For a thin progress bar at the top of the page during navigation:

```tsx
// components/NavigationProgress.tsx
"use client";

import { useEffect, useTransition } from "react";
import { useRouter, usePathname } from "next/navigation";
import NProgress from "nprogress";
import "nprogress/nprogress.css";

export default function NavigationProgress() {
  const pathname = usePathname();

  useEffect(() => {
    NProgress.done(); // finish the bar when pathname changes (navigation complete)
  }, [pathname]);

  return null;
}
```

---

## Summary

```
<Link> component:
  Import from "next/link" — works in Server and Client Components
  Renders an <a> tag, intercepts clicks for client-side navigation
  href="/path" or href={{ pathname, query }} for object form
  replace prop: replace history entry instead of push
  scroll={false}: don't scroll to top on navigation

Prefetching:
  On by default in production — links in viewport are prefetched
  prefetch={false} to disable for rarely-visited links
  Dynamic pages: prefetches up to the loading skeleton

Active links:
  No built-in isActive — use usePathname() ("use client")
  pathname === href for exact match
  pathname.startsWith(href + "/") for prefix match
  aria-current="page" for accessibility

Programmatic navigation:
  useRouter() — push, replace, back, forward, refresh, prefetch
  After Server Actions: use redirect() from "next/navigation"

Locale-aware navigation (Nexus):
  Helper function: localePath(locale, "/path")
  Locale-aware Link wrapper component
  Best: next-intl createSharedPathnamesNavigation
    → write locale-free paths everywhere
    → library handles /en/ or /si/ prefix automatically

Scroll:
  Default: scroll to top on push, restore position on back/forward
  scroll={false}: stay at current position (pagination, filters)
  Hash links: /page#section-id scrolls to element with that id

Loading during navigation:
  loading.tsx: automatic, shows on any navigation to that segment
  useTransition: isPending for manual loading state on programmatic nav
  NProgress: thin progress bar for global navigation feedback
```

---

_Next: [07 — Images, Fonts & Static Assets](https://claude.ai/chat/07%20-%20Images%2C%20Fonts%20%26%20Static%20Assets.md)_
