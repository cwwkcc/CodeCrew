> Environment variables are how you separate code from configuration — database URLs, API keys, secrets, and feature flags change between environments (development, staging, production) but the code stays the same. Next.js has a specific system for them, with strict rules about which variables reach the browser.

---

## Table of Contents

1. [Environment Variable Files](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-environment-variable-files)
2. [Server vs Client Variables](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-server-vs-client-variables)
3. [Accessing Variables](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-accessing-variables)
4. [Validating Variables at Startup](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-validating-variables-at-startup)
5. [next.config.ts](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-nextconfigts)
6. [Environment-Specific Behaviour](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-environment-specific-behaviour)

---

## 1. Environment Variable Files

Next.js loads `.env` files automatically. They're never committed to git.

```
.env                  — defaults for all environments (commit this if no secrets)
.env.local            — local overrides, NEVER committed (add to .gitignore)
.env.development      — development-only (can commit if no secrets)
.env.production       — production-only (can commit if no secrets)
.env.development.local — local development overrides, never committed
.env.production.local  — local production overrides, never committed
```

Loading priority (later files override earlier ones):

```
.env → .env.{environment} → .env.local → .env.{environment}.local
```

In practice, Nexus uses:

```
.env.example       — template with all keys, empty values (committed — documents what's needed)
.env.local         — actual values for local development (in .gitignore)
```

---

## 2. Server vs Client Variables

**The critical rule:** variables are server-only by default. To expose a variable to the browser, prefix it with `NEXT_PUBLIC_`.

```bash
# .env.local

# Server-only — NEVER sent to browser
DATABASE_URL="postgresql://user:password@localhost:5432/nexus"
SESSION_SECRET="a-very-long-random-secret-key"
SMTP_PASSWORD="email-service-password"
ADMIN_EMAIL="admin@cwwkcc.edu.lk"

# Browser-accessible — prefix with NEXT_PUBLIC_
NEXT_PUBLIC_APP_URL="http://localhost:3000"
NEXT_PUBLIC_SCHOOL_NAME="C.W.W. Kannangara Central College"
NEXT_PUBLIC_GOOGLE_ANALYTICS_ID="G-XXXXXXXXXX"
```

**What "browser-accessible" means:** the variable's value is embedded in the JavaScript bundle sent to the browser. Anyone can open DevTools and see it. Never put secrets in `NEXT_PUBLIC_` variables.

```tsx
// ❌ WRONG — secret will be visible to anyone who views page source
NEXT_PUBLIC_DATABASE_URL = "postgresql://...";
NEXT_PUBLIC_SESSION_SECRET = "my-secret";

// ✅ Correct — only non-sensitive values in NEXT_PUBLIC_
NEXT_PUBLIC_APP_URL = "https://cwwkcc.edu.lk";
NEXT_PUBLIC_RECAPTCHA_SITE_KEY = "6LeXXXXXXXXXXXXX"; // site key is meant to be public
```

---

## 3. Accessing Variables

```tsx
// Server Components, Server Actions, API Routes, middleware
// Access directly via process.env
const dbUrl = process.env.DATABASE_URL;
const sessionSecret = process.env.SESSION_SECRET;

// Client Components
// Only NEXT_PUBLIC_ variables available — others are undefined
const appUrl = process.env.NEXT_PUBLIC_APP_URL;
const schoolName = process.env.NEXT_PUBLIC_SCHOOL_NAME;

// TypeScript — process.env values are always string | undefined
// Check before using
if (!process.env.DATABASE_URL) {
  throw new Error("DATABASE_URL is not set");
}
```

### Type safety for environment variables

`process.env` values are `string | undefined` by default. Add a type declaration to silence TypeScript errors:

```tsx
// env.d.ts (at project root)
declare namespace NodeJS {
  interface ProcessEnv {
    DATABASE_URL: string;
    SESSION_SECRET: string;
    SMTP_HOST: string;
    SMTP_PORT: string;
    SMTP_USER: string;
    SMTP_PASSWORD: string;
    NEXT_PUBLIC_APP_URL: string;
    NODE_ENV: "development" | "production" | "test";
  }
}
```

Now `process.env.DATABASE_URL` is `string` instead of `string | undefined`. Still validate at runtime (see next section) — TypeScript types don't guarantee the variable is actually set.

---

## 4. Validating Variables at Startup

Use Zod to validate all environment variables when the app starts. If a required variable is missing, the app fails immediately with a clear error — instead of failing mysteriously at runtime when the variable is first used.

```tsx
// lib/env.ts
import { z } from "zod";

const EnvSchema = z.object({
  // Server-only
  DATABASE_URL: z.string().url(),
  SESSION_SECRET: z
    .string()
    .min(32, "SESSION_SECRET must be at least 32 characters"),
  SMTP_HOST: z.string().min(1),
  SMTP_PORT: z.coerce.number().int().positive(),
  SMTP_USER: z.string().email(),
  SMTP_PASSWORD: z.string().min(1),
  NODE_ENV: z
    .enum(["development", "production", "test"])
    .default("development"),

  // Public (NEXT_PUBLIC_ prefix)
  NEXT_PUBLIC_APP_URL: z.string().url(),
  NEXT_PUBLIC_SCHOOL_NAME: z.string().min(1),
});

// Parse and validate — throws if anything is wrong
const parsed = EnvSchema.safeParse(process.env);

if (!parsed.success) {
  console.error("❌ Invalid environment variables:");
  console.error(parsed.error.flatten().fieldErrors);
  throw new Error("Invalid environment variables — see above");
}

export const env = parsed.data;
```

```tsx
// Usage — import env instead of process.env
import { env } from "@/lib/env";

const dbClient = new PrismaClient({
  datasources: { db: { url: env.DATABASE_URL } },
});

const mailer = createTransport({
  host: env.SMTP_HOST,
  port: env.SMTP_PORT,
  auth: { user: env.SMTP_USER, pass: env.SMTP_PASSWORD },
});
```

`env` is fully typed by Zod's inference — `env.SMTP_PORT` is `number`, not `string`. `env.DATABASE_URL` is `string`, not `string | undefined`.

### The .env.example template

```bash
# .env.example — committed to git, documents required variables

# Database
DATABASE_URL="postgresql://USER:PASSWORD@HOST:PORT/DATABASE"

# Authentication
SESSION_SECRET="replace-with-at-least-32-random-characters"

# Email (SMTP)
SMTP_HOST="smtp.example.com"
SMTP_PORT="587"
SMTP_USER="noreply@cwwkcc.edu.lk"
SMTP_PASSWORD="your-smtp-password"

# Public (safe to expose to browser)
NEXT_PUBLIC_APP_URL="https://cwwkcc.edu.lk"
NEXT_PUBLIC_SCHOOL_NAME="C.W.W. Kannangara Central College"
```

New developers copy `.env.example` to `.env.local` and fill in the values.

---

## 5. next.config.ts

`next.config.ts` is where you configure the Next.js framework itself:

```tsx
// next.config.ts
import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  // Remote image domains for next/image
  images: {
    remotePatterns: [
      { protocol: "https", hostname: "storage.cwwkcc.edu.lk" },
      { protocol: "https", hostname: "**.supabase.co" },
    ],
  },

  // Redirect /old-path → /new-path (permanent redirect)
  async redirects() {
    return [
      {
        source: "/school",
        destination: "/about",
        permanent: true, // 308 — search engines update their index
      },
      {
        source: "/students/search",
        destination: "/students",
        permanent: false, // 307 — temporary
      },
    ];
  },

  // Rewrite — serve /api/proxy/* from a different URL (useful for CORS)
  async rewrites() {
    return [
      {
        source: "/api/analytics/:path*",
        destination: "https://analytics.example.com/:path*",
      },
    ];
  },

  // Custom response headers for all routes
  async headers() {
    return [
      {
        source: "/(.*)",
        headers: [
          { key: "X-Frame-Options", value: "DENY" },
          { key: "X-Content-Type-Options", value: "nosniff" },
          { key: "Referrer-Policy", value: "strict-origin-when-cross-origin" },
        ],
      },
    ];
  },
};

export default nextConfig;
```

### Common next.config.ts options

```tsx
const nextConfig: NextConfig = {
  // Strict mode for React (highlights potential problems)
  reactStrictMode: true,

  // Enable experimental features
  experimental: {
    // typedRoutes: true,        // type-safe href props
    // serverActions: { ... },   // configure server action limits
  },

  // Output mode for deployment
  output: "standalone", // self-contained output for Docker
  // output: "export",   // static HTML export (no server required)

  // Turbopack (faster dev builds — replaces Webpack)
  // enabled by default in Next.js 15 dev mode

  // Compiler options
  compiler: {
    removeConsole: process.env.NODE_ENV === "production", // strip console.log in prod
  },

  // i18n (if not using next-intl)
  // i18n: { locales: ["en", "si"], defaultLocale: "en" },
};
```

---

## 6. Environment-Specific Behaviour

```tsx
// Check the current environment
const isDevelopment = process.env.NODE_ENV === "development";
const isProduction = process.env.NODE_ENV === "production";

// Only log in development
if (isDevelopment) {
  console.log("Debug info:", data);
}

// Stricter caching in production
const students = await prisma.student.findMany();
// In development, you might want to disable caching for faster iteration

// Different base URLs
const apiUrl = process.env.NEXT_PUBLIC_APP_URL;
// development: http://localhost:3000
// production:  https://cwwkcc.edu.lk

// Error detail — never expose stack traces in production
if (isDevelopment) {
  console.error(error);
} else {
  // Log to monitoring service, show generic message to user
  Sentry.captureException(error);
}
```

### Feature flags via environment variables

```bash
# .env.local
NEXT_PUBLIC_FEATURE_GALLERY="true"
NEXT_PUBLIC_FEATURE_RESULTS_PORTAL="false"
```

```tsx
const features = {
  gallery: process.env.NEXT_PUBLIC_FEATURE_GALLERY === "true",
  resultsPortal: process.env.NEXT_PUBLIC_FEATURE_RESULTS_PORTAL === "true",
};

// In a component
{
  features.gallery && <Link href="/gallery">Gallery</Link>;
}
```

---

## Summary

```
Files:
  .env.local         — local values (in .gitignore, never committed)
  .env.example       — template with keys, empty values (committed)
  .env.production    — production defaults (committed if no secrets)

Server vs client:
  Default: server-only — not sent to browser, can contain secrets
  NEXT_PUBLIC_ prefix: sent to browser — visible to anyone, no secrets

Validation:
  Use Zod to validate all env vars at startup
  z.object({...}).safeParse(process.env)
  Fail fast with clear error if anything is missing or wrong
  Import env from lib/env.ts — typed, validated, no process.env scatter

next.config.ts:
  images.remotePatterns  — whitelist remote image domains
  redirects()            — permanent/temporary URL redirects
  rewrites()             — proxy requests to other services
  headers()              — add security/cache headers to responses
  reactStrictMode: true  — catch potential issues in development
  output: "standalone"   — for Docker deployments

Environment-specific:
  process.env.NODE_ENV === "development" / "production" / "test"
  Used for: debug logging, cache settings, error detail level
  Feature flags: NEXT_PUBLIC_FEATURE_X="true" → check in components
```

---

_Next: [10 — Deployment & the Production Build](https://claude.ai/chat/10%20-%20Deployment%20%26%20the%20Production%20Build.md)_
