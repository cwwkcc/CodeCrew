> Middleware runs on every request before it hits a page or API route. It's the right place for authentication checks, locale detection, redirects, and header manipulation — because it runs at the edge (close to the user) before any component code executes. This file covers middleware and how to implement authentication in Next.js.

---

## Table of Contents

1. [What Middleware Is](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-what-middleware-is)
2. [Writing Middleware](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-writing-middleware)
3. [Matching Routes](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-matching-routes)
4. [Authentication Patterns](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-authentication-patterns)
5. [Session Management with jose](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-session-management-with-jose)
6. [Auth in Server Components](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-auth-in-server-components)
7. [Auth in Server Actions](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-auth-in-server-actions)
8. [Locale Detection (Nexus)](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-locale-detection-nexus)

---

## 1. What Middleware Is

Middleware is a function that runs before every request is completed. It sits between the incoming request and the route handler (page or API route).

```
Request → Middleware → Route (page.tsx / route.ts)
```

Because middleware runs before the page, it can:

- Redirect the user to a different URL
- Rewrite the URL (serve different content at the same URL)
- Add response headers
- Read request cookies and headers
- Reject the request entirely

Middleware runs on the **Edge Runtime** — a lightweight, fast JavaScript environment. It can't use Node.js APIs (`fs`, `crypto`, etc.) or most npm packages. It can use the Web APIs (`Request`, `Response`, `Headers`, `URL`, `fetch`, etc.).

---

## 2. Writing Middleware

Create a `middleware.ts` file at the project root (same level as `app/`):

```tsx
// middleware.ts
import { NextResponse } from "next/server";
import type { NextRequest } from "next/server";

export function middleware(request: NextRequest) {
  // request.nextUrl — the current URL
  // request.cookies — request cookies
  // request.headers — request headers

  // Continue without modification
  return NextResponse.next();

  // Redirect
  // return NextResponse.redirect(new URL("/login", request.url));

  // Rewrite (serve different content, URL stays same)
  // return NextResponse.rewrite(new URL("/en" + request.nextUrl.pathname, request.url));

  // Add a response header
  // const response = NextResponse.next();
  // response.headers.set("X-Custom-Header", "value");
  // return response;
}
```

`NextResponse.next()` continues the request normally. `NextResponse.redirect()` sends the user to a different URL. `NextResponse.rewrite()` serves different content without changing the visible URL.

---

## 3. Matching Routes

By default, middleware runs on every request including static files, images, and API routes. Use the `config` export to restrict which paths it runs on:

```tsx
// middleware.ts
export const config = {
  matcher: [
    /*
     * Match all request paths except:
     * - _next/static (static files)
     * - _next/image (image optimisation)
     * - favicon.ico
     * - public folder files
     */
    "/((?!_next/static|_next/image|favicon.ico|.*\\.(?:svg|png|jpg|jpeg|gif|webp)$).*)",
  ],
};
```

Or match specific paths:

```tsx
export const config = {
  matcher: [
    "/dashboard/:path*", // /dashboard and all sub-paths
    "/admin/:path*", // /admin and all sub-paths
    "/api/:path*", // all API routes
    "/(en|si)/:path*", // locale-prefixed paths (Nexus)
  ],
};
```

---

## 4. Authentication Patterns

The most common use of middleware: redirect unauthenticated users away from protected routes.

```tsx
// middleware.ts
import { NextResponse } from "next/server";
import type { NextRequest } from "next/server";
import { verifySession } from "@/lib/session"; // edge-compatible

const protectedPaths = ["/dashboard", "/admin", "/profile"];
const authPaths = ["/login", "/register"];

export async function middleware(request: NextRequest) {
  const { pathname } = request.nextUrl;

  // Check if this path requires authentication
  const isProtected = protectedPaths.some((p) => pathname.startsWith(p));
  const isAuthPath = authPaths.some((p) => pathname.startsWith(p));

  // Verify the session cookie
  const session = await verifySession(request.cookies.get("session")?.value);

  if (isProtected && !session) {
    // Not logged in, trying to access protected route → redirect to login
    const loginUrl = new URL("/login", request.url);
    loginUrl.searchParams.set("from", pathname); // remember where they were going
    return NextResponse.redirect(loginUrl);
  }

  if (isAuthPath && session) {
    // Already logged in, trying to access login/register → redirect to dashboard
    return NextResponse.redirect(new URL("/dashboard", request.url));
  }

  return NextResponse.next();
}

export const config = {
  matcher: ["/((?!_next/static|_next/image|favicon.ico).*)"],
};
```

---

## 5. Session Management with jose

Middleware can't use heavy libraries like `jsonwebtoken` (Node.js). Use `jose` — a Web API-compatible JWT library:

```bash
npm install jose
```

```tsx
// lib/session.ts
import { SignJWT, jwtVerify } from "jose";
import { cookies } from "next/headers";

const SESSION_SECRET = new TextEncoder().encode(process.env.SESSION_SECRET!);
const SESSION_COOKIE = "session";
const EXPIRY = "7d";

export type SessionPayload = {
  userId: number;
  isAdmin: boolean;
  email: string;
};

// Create a signed JWT and store it in a cookie
export async function createSession(payload: SessionPayload) {
  const token = await new SignJWT(payload)
    .setProtectedHeader({ alg: "HS256" })
    .setIssuedAt()
    .setExpirationTime(EXPIRY)
    .sign(SESSION_SECRET);

  const cookieStore = cookies();
  cookieStore.set(SESSION_COOKIE, token, {
    httpOnly: true, // not accessible from JS
    secure: process.env.NODE_ENV === "production",
    sameSite: "lax",
    maxAge: 60 * 60 * 24 * 7, // 7 days in seconds
    path: "/",
  });
}

// Verify a token and return the payload (works in middleware and Server Components)
export async function verifySession(
  token?: string,
): Promise<SessionPayload | null> {
  if (!token) return null;

  try {
    const { payload } = await jwtVerify(token, SESSION_SECRET);
    return payload as unknown as SessionPayload;
  } catch {
    return null;
  }
}

// Get the current session in a Server Component or Server Action
export async function getSession(): Promise<SessionPayload | null> {
  const cookieStore = cookies();
  const token = cookieStore.get(SESSION_COOKIE)?.value;
  return verifySession(token);
}

// Delete the session cookie (logout)
export async function deleteSession() {
  const cookieStore = cookies();
  cookieStore.delete(SESSION_COOKIE);
}
```

```tsx
// actions/auth.ts
"use server";

import { prisma } from "@/lib/prisma";
import { createSession, deleteSession } from "@/lib/session";
import { redirect } from "next/navigation";
import bcrypt from "bcryptjs";
import { z } from "zod";

const LoginSchema = z.object({
  email: z.string().email(),
  password: z.string().min(6),
});

export async function login(prevState: unknown, formData: FormData) {
  const result = LoginSchema.safeParse({
    email: formData.get("email"),
    password: formData.get("password"),
  });

  if (!result.success) {
    return { error: "Invalid email or password format." };
  }

  const { email, password } = result.data;

  const user = await prisma.user.findUnique({ where: { email } });
  if (!user || !(await bcrypt.compare(password, user.passwordHash))) {
    return { error: "Incorrect email or password." };
  }

  await createSession({
    userId: user.id,
    isAdmin: user.isAdmin,
    email: user.email,
  });

  redirect("/dashboard");
}

export async function logout() {
  await deleteSession();
  redirect("/login");
}
```

```tsx
// app/login/page.tsx — the login form
"use client";

import { useActionState } from "react";
import { login } from "@/actions/auth";

export default function LoginPage() {
  const [state, action, isPending] = useActionState(login, null);

  return (
    <form action={action} className="login-form">
      <h1>Sign In</h1>

      <input name="email" type="email" placeholder="Email" required />
      <input name="password" type="password" placeholder="Password" required />

      {state?.error && (
        <p className="text-destructive text-sm">{state.error}</p>
      )}

      <button type="submit" disabled={isPending}>
        {isPending ? "Signing in..." : "Sign In"}
      </button>
    </form>
  );
}
```

---

## 6. Auth in Server Components

Once middleware protects routes, Server Components can trust that a session exists for protected paths. Still, always verify — middleware protects the route, but Server Components should check for the data they need:

```tsx
// app/dashboard/page.tsx
import { getSession } from "@/lib/session";
import { redirect } from "next/navigation";

export default async function DashboardPage() {
  const session = await getSession();

  // Middleware should have caught this, but be defensive
  if (!session) redirect("/login");

  return (
    <main>
      <h1>Welcome, {session.email}</h1>
      {session.isAdmin && <AdminPanel />}
    </main>
  );
}
```

### Role-based access

```tsx
// app/admin/page.tsx
export default async function AdminPage() {
  const session = await getSession();

  if (!session) redirect("/login");
  if (!session.isAdmin) redirect("/dashboard"); // logged in but not admin

  return <AdminDashboard />;
}
```

### Conditionally showing UI

```tsx
// Server Component — no client code needed for server-side conditional rendering
export default async function Header() {
  const session = await getSession();

  return (
    <header>
      <Logo />
      <Nav />
      <div className="auth-section">
        {session ? (
          <UserMenu user={session} />
        ) : (
          <Link href="/login">Sign In</Link>
        )}
      </div>
    </header>
  );
}
```

---

## 7. Auth in Server Actions

Always check auth inside Server Actions — they can be called by anyone, not just authenticated users navigating through your UI:

```tsx
// actions/admin.ts
"use server";

import { getSession } from "@/lib/session";

async function requireAdmin() {
  const session = await getSession();
  if (!session) throw new Error("Not authenticated");
  if (!session.isAdmin) throw new Error("Not authorized");
  return session;
}

export async function deleteStudent(id: number) {
  const session = await requireAdmin(); // throws if not admin

  await prisma.student.delete({ where: { id } });

  // Audit log
  await prisma.auditLog.create({
    data: {
      action: "DELETE_STUDENT",
      targetId: id,
      userId: session.userId,
      timestamp: new Date(),
    },
  });

  revalidatePath("/students");
}
```

Never trust that the client is the same person who was authenticated. Revalidate auth on every mutation.

---

## 8. Locale Detection (Nexus)

Middleware is also the right place to detect the user's preferred locale and redirect them:

```tsx
// middleware.ts (Nexus)
import { NextResponse } from "next/server";
import type { NextRequest } from "next/server";
import { verifySession } from "@/lib/session";

const locales = ["en", "si"] as const;
const defaultLocale = "en";

function getPreferredLocale(request: NextRequest): string {
  // Check cookie (user's saved preference)
  const savedLocale = request.cookies.get("NEXT_LOCALE")?.value;
  if (
    savedLocale &&
    locales.includes(savedLocale as (typeof locales)[number])
  ) {
    return savedLocale;
  }

  // Fall back to Accept-Language header
  const acceptLanguage = request.headers.get("accept-language") ?? "";
  const preferred = acceptLanguage
    .split(",")
    .map((lang) => lang.split(";")[0].trim().split("-")[0])
    .find((lang) => locales.includes(lang as (typeof locales)[number]));

  return preferred ?? defaultLocale;
}

export async function middleware(request: NextRequest) {
  const { pathname } = request.nextUrl;

  // Skip if already has a locale prefix
  const hasLocale = locales.some(
    (l) => pathname.startsWith(`/${l}/`) || pathname === `/${l}`,
  );

  if (
    !hasLocale &&
    !pathname.startsWith("/_next") &&
    !pathname.startsWith("/api")
  ) {
    // Redirect to locale-prefixed path
    const locale = getPreferredLocale(request);
    return NextResponse.redirect(
      new URL(`/${locale}${pathname === "/" ? "" : pathname}`, request.url),
    );
  }

  // Auth check for protected routes
  const locale = pathname.split("/")[1]; // e.g. "en"
  const pathAfter = pathname.slice(locale.length + 1); // remove /en

  if (pathAfter.startsWith("/admin")) {
    const session = await verifySession(request.cookies.get("session")?.value);
    if (!session?.isAdmin) {
      return NextResponse.redirect(new URL(`/${locale}/login`, request.url));
    }
  }

  return NextResponse.next();
}

export const config = {
  matcher: ["/((?!_next/static|_next/image|favicon.ico|.*\\.\\w+$).*)"],
};
```

In practice, Nexus uses `next-intl` which provides its own middleware for locale handling — the principle is the same but the implementation is handled by the library.

---

## Summary

```
Middleware:
  middleware.ts at project root (same level as app/)
  Runs before every matched request — on the Edge (fast, no Node.js APIs)
  export function middleware(request: NextRequest): NextResponse
  export const config = { matcher: [...] } to restrict which paths

NextResponse methods:
  NextResponse.next()              — continue normally
  NextResponse.redirect(new URL)   — redirect to another URL
  NextResponse.rewrite(new URL)    — serve different content, same URL
  response.headers.set(key, value) — add headers to the response

Auth flow:
  Login: verify credentials → createSession() → sets httpOnly cookie → redirect
  Middleware: verifySession(cookie) → redirect to /login if invalid
  Server Components: getSession() → check session → render conditionally
  Server Actions: requireAdmin() at the top of every protected action

Session with jose (edge-compatible JWT):
  SignJWT → sign with secret → store in httpOnly cookie
  jwtVerify → verify token → return payload
  httpOnly + secure + sameSite: "lax" — standard cookie security

Always check auth in Server Actions:
  Middleware protects routes, not individual functions
  Server Actions can be called directly — they're just POST endpoints
  requireAdmin() / requireAuth() helper throws if check fails

Locale in middleware (Nexus):
  Check cookie → Accept-Language header → default
  Redirect bare paths to /${locale}/${path}
  In Nexus: handled by next-intl middleware
```

---

_Next: [09 — Environment Variables & Configuration](https://claude.ai/chat/09%20-%20Environment%20Variables%20%26%20Configuration.md)_
