> Nexus serves content in both English and Sinhala. `next-intl` is the library that handles this — locale routing, translated strings, locale-aware formatting, and right-to-left support. This file covers the setup used in Nexus and how to work with it day to day.

---

## Table of Contents

1. [The i18n Mental Model](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-i18n-mental-model)
2. [Setup & File Structure](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-setup--file-structure)
3. [Using Translations](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-using-translations)
4. [Locale-Aware Formatting](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-locale-aware-formatting)
5. [Locale-Aware Navigation](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-locale-aware-navigation)
6. [Server vs Client Components](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-server-vs-client-components)
7. [Adding a New Language](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-adding-a-new-language)
8. [Nexus Translation Keys](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-nexus-translation-keys)

---

## 1. The i18n Mental Model

Internationalisation (i18n) has two parts:

**Routing:** which locale is active? In Nexus, the locale is in the URL — `/en/students` vs `/si/students`. The `[locale]` segment carries it through every page.

**Translations:** what text to display? Strings like "Students", "View Profile", "Published on" have different values per locale. They're stored in JSON files and looked up by key at render time.

```
URL: /si/students
  → locale = "si"
  → load messages/si.json
  → t("nav.students") → "සිසු"

URL: /en/students
  → locale = "en"
  → load messages/en.json
  → t("nav.students") → "Students"
```

---

## 2. Setup & File Structure

```bash
npm install next-intl
```

```
src/
  app/
    [locale]/
      layout.tsx       — locale layout
      page.tsx
    layout.tsx         — root layout
  i18n/
    routing.ts         — locale config + navigation helpers
    request.ts         — per-request locale resolution
  messages/
    en.json            — English strings
    si.json            — Sinhala strings
  middleware.ts        — locale detection + routing
```

### routing.ts — locale config

```tsx
// src/i18n/routing.ts
import { defineRouting } from "next-intl/routing";
import { createSharedPathnamesNavigation } from "next-intl/navigation";

export const routing = defineRouting({
  locales: ["en", "si"],
  defaultLocale: "en",
});

// Locale-aware navigation utilities (covers File 06)
export const { Link, redirect, usePathname, useRouter, permanentRedirect } =
  createSharedPathnamesNavigation(routing);
```

### request.ts — per-request locale

```tsx
// src/i18n/request.ts
import { getRequestConfig } from "next-intl/server";
import { routing } from "./routing";

export default getRequestConfig(async ({ requestLocale }) => {
  let locale = await requestLocale;

  // Validate locale — fall back to default if invalid
  if (!locale || !routing.locales.includes(locale as "en" | "si")) {
    locale = routing.defaultLocale;
  }

  return {
    locale,
    messages: (await import(`../../messages/${locale}.json`)).default,
  };
});
```

### middleware.ts — locale routing

```tsx
// middleware.ts
import createMiddleware from "next-intl/middleware";
import { routing } from "./src/i18n/routing";

export default createMiddleware(routing);

export const config = {
  // Match all paths except Next.js internals and static files
  matcher: ["/((?!_next|_vercel|.*\\..*).*)"],
};
```

### next.config.ts — wire up next-intl

```tsx
import type { NextConfig } from "next";
import createNextIntlPlugin from "next-intl/plugin";

const withNextIntl = createNextIntlPlugin("./src/i18n/request.ts");

const nextConfig: NextConfig = {
  // your config
};

export default withNextIntl(nextConfig);
```

---

## 3. Using Translations

### Message files

```json
// messages/en.json
{
  "nav": {
    "home": "Home",
    "students": "Students",
    "news": "News",
    "about": "About",
    "contact": "Contact"
  },
  "students": {
    "title": "Students",
    "viewProfile": "View Profile",
    "grade": "Grade {grade}",
    "noResults": "No students found.",
    "count": "{count, plural, one {# student} other {# students}}"
  },
  "common": {
    "loading": "Loading...",
    "error": "Something went wrong. Please try again.",
    "backTo": "Back to {page}"
  }
}
```

```json
// messages/si.json
{
  "nav": {
    "home": "මුල් පිටුව",
    "students": "සිසු",
    "news": "පුවත්",
    "about": "අප ගැන",
    "contact": "අප අමතන්න"
  },
  "students": {
    "title": "සිසු",
    "viewProfile": "පැතිකඩ බලන්න",
    "grade": "ශ්‍රේණිය {grade}",
    "noResults": "සිසුන් සොයා ගත නොහැක.",
    "count": "{count} සිසු{count, plural, one {} other {}}"
  },
  "common": {
    "loading": "පූරණය වෙමින්...",
    "error": "දෝෂයක් ඇත. නැවත උත්සාහ කරන්න.",
    "backTo": "{page} වෙත ආපසු"
  }
}
```

### In Server Components

```tsx
// app/[locale]/students/page.tsx
import { getTranslations } from "next-intl/server";

export default async function StudentsPage({
  params,
}: {
  params: { locale: string };
}) {
  const t = await getTranslations({
    locale: params.locale,
    namespace: "students",
  });
  const students = await prisma.student.findMany();

  return (
    <main>
      <h1>{t("title")}</h1> {/* "Students" / "සිසු" */}
      <p>{t("count", { count: students.length })}</p>
      {students.length === 0 && <p>{t("noResults")}</p>}
      <StudentGrid students={students} />
    </main>
  );
}
```

### In Client Components

```tsx
// components/StudentCard.tsx
"use client";

import { useTranslations } from "next-intl";
import { Link } from "@/i18n/routing"; // locale-aware Link

export default function StudentCard({ student }: { student: Student }) {
  const t = useTranslations("students");

  return (
    <div className="card">
      <h3>{student.name}</h3>
      <p>{t("grade", { grade: student.grade })}</p>
      <Link href={`/students/${student.id}`} className="btn-outline">
        {t("viewProfile")}
      </Link>
    </div>
  );
}
```

### Dynamic values in translations

```json
// Interpolation — {variableName}
"grade": "Grade {grade}"
// t("grade", { grade: 9 }) → "Grade 9"

// Plural — ICU message syntax
"count": "{count, plural, one {# student} other {# students}}"
// t("count", { count: 1 }) → "1 student"
// t("count", { count: 42 }) → "42 students"

// Select — for gender or other enumerations
"greeting": "{gender, select, male {Mr.} female {Ms.} other {}} {name}"
```

### Metadata with translations

```tsx
// app/[locale]/students/page.tsx
import { getTranslations } from "next-intl/server";
import type { Metadata } from "next";

export async function generateMetadata({
  params,
}: {
  params: { locale: string };
}): Promise<Metadata> {
  const t = await getTranslations({
    locale: params.locale,
    namespace: "students",
  });
  return { title: t("title") };
  // English: "Students — CWWKCC"
  // Sinhala: "සිසු — CWWKCC"
}
```

---

## 4. Locale-Aware Formatting

`next-intl` wraps the Intl API for locale-aware formatting of dates, numbers, and relative time.

### In Server Components

```tsx
import { getFormatter } from "next-intl/server";

export default async function NewsArticle({ article, params }) {
  const format = await getFormatter({ locale: params.locale });

  return (
    <article>
      {/* Date: "June 4, 2026" / "2026 ජූනි 4" */}
      <time dateTime={article.publishedAt.toISOString()}>
        {format.dateTime(article.publishedAt, {
          dateStyle: "long",
        })}
      </time>

      {/* Relative time: "3 days ago" / "දින 3 කට පෙර" */}
      <span>{format.relativeTime(article.publishedAt)}</span>

      {/* Number */}
      <span>
        {format.number(1234567)}
        {/* en: "1,234,567" — si: "12,34,567" (Sinhala grouping) */}
      </span>
    </article>
  );
}
```

### In Client Components

```tsx
"use client";
import { useFormatter } from "next-intl";

export default function PublishedDate({ date }: { date: Date }) {
  const format = useFormatter();

  return (
    <time dateTime={date.toISOString()}>
      {format.dateTime(date, { dateStyle: "medium" })}
    </time>
  );
}
```

---

## 5. Locale-Aware Navigation

Always use the `Link`, `useRouter`, `usePathname`, and `redirect` from `@/i18n/routing` — not from `next/link` or `next/navigation`. These automatically handle the locale prefix.

```tsx
// ✅ Correct — from @/i18n/routing
import { Link, useRouter, redirect } from "@/i18n/routing";

<Link href="/students">Students</Link>;
// → renders as /en/students or /si/students based on current locale

router.push("/students");
// → navigates to /en/students or /si/students

redirect("/login"); // in Server Actions
// → redirects to /en/login or /si/login

// ❌ Wrong — bare next/link without locale
import Link from "next/link";
<Link href="/students">Students</Link>;
// → renders as /students — breaks locale routing
```

### Language switcher

```tsx
// components/LanguageSwitcher.tsx
"use client";

import { usePathname, useRouter } from "@/i18n/routing";
import { useParams } from "next/navigation";

export default function LanguageSwitcher() {
  const router = useRouter();
  const pathname = usePathname(); // path WITHOUT locale (e.g., "/students")
  const { locale } = useParams<{ locale: string }>();

  return (
    <div className="flex gap-2">
      {(["en", "si"] as const).map((loc) => (
        <button
          key={loc}
          onClick={() => router.replace(pathname, { locale: loc })}
          className={`locale-btn ${locale === loc ? "locale-btn--active" : ""}`}
          lang={loc}
          aria-current={locale === loc ? "true" : undefined}
        >
          {loc === "en" ? "English" : "සිංහල"}
        </button>
      ))}
    </div>
  );
}
```

---

## 6. Server vs Client Components

next-intl has separate APIs for Server and Client Components. Use the right one:

```
Server Components:
  getTranslations({ locale, namespace })  → returns t function
  getFormatter({ locale })                → returns format object
  getLocale()                             → returns current locale string
  getNow()                                → returns current Date (consistent with server)

Client Components:
  useTranslations(namespace)              → returns t function (hook)
  useFormatter()                          → returns format object (hook)
  useLocale()                             → returns current locale string (hook)
  useNow()                                → returns current Date (hook)
```

```tsx
// Server Component
import { getTranslations, getLocale } from "next-intl/server";

export default async function Header({
  params,
}: {
  params: { locale: string };
}) {
  const t = await getTranslations({ locale: params.locale, namespace: "nav" });
  const locale = await getLocale();

  return (
    <nav>
      <Link href="/">{t("home")}</Link>
      <Link href="/students">{t("students")}</Link>
    </nav>
  );
}

// Client Component
("use client");
import { useTranslations, useLocale } from "next-intl";

export default function NavMenu() {
  const t = useTranslations("nav");
  const locale = useLocale();

  return (
    <div>
      <span className="text-xs text-muted-foreground">
        {locale.toUpperCase()}
      </span>
      <Link href="/students">{t("students")}</Link>
    </div>
  );
}
```

---

## 7. Adding a New Language

To add Tamil (`ta`) support to Nexus:

1. Add to routing config:

```tsx
// src/i18n/routing.ts
export const routing = defineRouting({
  locales: ["en", "si", "ta"], // add "ta"
  defaultLocale: "en",
});
```

2. Create the message file:

```json
// messages/ta.json
{
  "nav": {
    "home":     "முகப்பு",
    "students": "மாணவர்கள்",
    ...
  }
}
```

3. Update request.ts to handle the new locale (it imports dynamically by locale name — no change needed if the file exists).

4. Update the language switcher UI to include the new option.

That's it. The middleware handles routing, the message file provides translations, and every page that uses `getTranslations` or `useTranslations` will work.

---

## 8. Nexus Translation Keys

The full key structure used in Nexus:

```
nav.*          — Navigation menu items
footer.*       — Footer links and text
common.*       — Shared UI strings (loading, error, back, etc.)
home.*         — Homepage-specific strings
students.*     — Student directory and profiles
news.*         — News articles and listing
results.*      — Exam results section
gallery.*      — Photo gallery
about.*        — About page
contact.*      — Contact form and page
admin.*        — Admin panel (only shown in admin UI)
meta.*         — Page titles and descriptions for SEO
errors.*       — Error messages (not-found, server error, etc.)
```

Keeping namespaces at the feature level (not the component level) means:

- `useTranslations("students")` gives you all student-related strings
- Components in the student section all pull from the same namespace
- Easy to find and update all strings for a given page/section

---

## Summary

```
Mental model:
  Locale in URL (/en/*, /si/*) — [locale] segment
  Translations in JSON files — messages/en.json, messages/si.json
  t("key") returns the string for the current locale

Setup (Nexus):
  routing.ts  — defines locales, defaultLocale
  request.ts  — loads message file per request
  middleware  — next-intl/middleware handles locale routing
  next.config — withNextIntl() wraps the config

Translations:
  Server Components: getTranslations({ locale, namespace }) — async
  Client Components: useTranslations(namespace) — hook
  Dynamic values: t("grade", { grade: 9 }) → "Grade 9"
  Plurals: ICU message syntax {count, plural, one {...} other {...}}

Formatting:
  Server: getFormatter({ locale }) → format.dateTime(), format.number()
  Client: useFormatter() → same methods

Navigation:
  Always import from @/i18n/routing — not next/link or next/navigation
  Link, useRouter, usePathname, redirect — all locale-aware
  LanguageSwitcher: router.replace(pathname, { locale: newLocale })

Adding a language:
  Add to routing.locales array
  Create messages/{locale}.json
  Update language switcher UI
  Middleware and translations work automatically
```

---

_End of Next.js module. See also: [React Router vs Next.js App Router](<>) for a side-by-side comparison._
