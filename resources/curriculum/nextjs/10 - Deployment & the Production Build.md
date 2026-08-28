> A development server and a production build are different things. `npm run dev` is optimised for iteration speed — no minification, no caching, hot reload. `npm run build` is optimised for user experience — bundled, minified, statically pre-rendered where possible, cached aggressively. This file covers what happens at build time, how to deploy, and what to know for Nexus.

---

## Table of Contents

1. [What the Build Does](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-what-the-build-does)
2. [Build Output Explained](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-build-output-explained)
3. [Deployment Targets](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-deployment-targets)
4. [Deploying to Vercel](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-deploying-to-vercel)
5. [Deploying with Docker](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-deploying-with-docker)
6. [Static Export](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-static-export)
7. [Pre-Production Checklist](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-pre-production-checklist)

---

## 1. What the Build Does

```bash
npm run build
```

Next.js analyses every page and decides how to render it:

```
Static (○)   — rendered at build time, output is an HTML file
ISR (◐)      — static at build time, periodically re-rendered
Dynamic (λ)  — rendered per request on the server
```

```
Route (app)                    Size    First Load JS
┌ ○ /en                        4.2 kB       101 kB
├ ○ /en/about                  1.8 kB        99 kB
├ ◐ /en/news                   2.1 kB       100 kB
├ λ /en/news/[slug]            3.5 kB       100 kB
├ ◐ /en/students               4.8 kB       102 kB
├ λ /en/students/[id]          6.2 kB       104 kB
└ λ /en/admin/dashboard        8.1 kB       110 kB
```

- `○ Static` — fastest. HTML generated once, served from CDN. No server involved per request.
- `◐ ISR` — static but re-renders on a schedule. Best of both worlds for content that changes occasionally.
- `λ Dynamic` — server renders fresh HTML per request. Needed for user-specific content, auth-gated pages.

### What triggers dynamic rendering

A page becomes dynamic if it uses:

- `cookies()` — reading request cookies
- `headers()` — reading request headers
- `searchParams` prop — reading query parameters
- `unstable_noStore()` — opting out of caching
- A `fetch` with `cache: "no-store"`

If none of these are present and the data can be resolved at build time, Next.js renders the page statically.

---

## 2. Build Output Explained

```bash
.next/
  static/          — compiled CSS, JS chunks — served by CDN
  server/          — server-side code (API routes, dynamic pages)
  cache/           — incremental build cache (speeds up subsequent builds)
  BUILD_ID         — unique ID for this build (for cache invalidation)
```

### Analysing bundle size

```bash
# See what's in your JavaScript bundle
ANALYZE=true npm run build
```

Requires `@next/bundle-analyzer`:

```bash
npm install --save-dev @next/bundle-analyzer
```

```tsx
// next.config.ts
import withBundleAnalyzer from "@next/bundle-analyzer";

const withAnalyzer = withBundleAnalyzer({
  enabled: process.env.ANALYZE === "true",
});

export default withAnalyzer({
  // your next config
});
```

Opens a visual treemap showing every module and its size. Use it to find unexpected large dependencies.

---

## 3. Deployment Targets

| Platform          | Best for                               | Cost                         |
| ----------------- | -------------------------------------- | ---------------------------- |
| **Vercel**        | Next.js apps — zero config, fastest DX | Free tier + usage            |
| **Netlify**       | Similar to Vercel                      | Free tier + usage            |
| **Railway**       | Full-stack with DB, simple pricing     | $5/month                     |
| **Render**        | Similar to Railway                     | Free tier (slow cold starts) |
| **Docker / VPS**  | Full control, Nexus on school server   | Server cost only             |
| **Static export** | If no server-side features needed      | Free (any CDN)               |

---

## 4. Deploying to Vercel

Vercel is built by the Next.js team — zero config deployment.

```bash
# Install Vercel CLI
npm install -g vercel

# Deploy from project root
vercel

# Production deploy
vercel --prod
```

Or connect a GitHub repo at vercel.com — every push to `main` deploys automatically.

**Environment variables on Vercel:**

- Go to Project → Settings → Environment Variables
- Add each variable from your `.env.local`
- Set which environments they apply to (Development / Preview / Production)
- Vercel injects them at build time and runtime

**What Vercel handles automatically:**

- SSL certificate
- CDN for static assets globally
- Edge Network for middleware
- Image optimisation
- Automatic preview deployments for pull requests

---

## 5. Deploying with Docker

For hosting on a VPS or school server — full control, no platform dependency.

### Dockerfile (Next.js standalone output)

```dockerfile
# Dockerfile
FROM node:20-alpine AS base

# Install dependencies only when needed
FROM base AS deps
RUN apk add --no-cache libc6-compat
WORKDIR /app

COPY package.json package-lock.json ./
RUN npm ci

# Build the application
FROM base AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY . .

# Generate Prisma client
RUN npx prisma generate

ENV NEXT_TELEMETRY_DISABLED=1
RUN npm run build

# Production image — minimal size
FROM base AS runner
WORKDIR /app

ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1

RUN addgroup --system --gid 1001 nodejs
RUN adduser  --system --uid 1001 nextjs

# Copy only what's needed to run
COPY --from=builder /app/public       ./public
COPY --from=builder /app/.next/standalone ./
COPY --from=builder /app/.next/static ./.next/static

USER nextjs

EXPOSE 3000
ENV PORT=3000
ENV HOSTNAME="0.0.0.0"

CMD ["node", "server.js"]
```

```tsx
// next.config.ts — enable standalone output for Docker
const nextConfig: NextConfig = {
  output: "standalone",
};
```

```yaml
# docker-compose.yml
services:
  web:
    build: .
    ports:
      - "3000:3000"
    environment:
      DATABASE_URL: ${DATABASE_URL}
      SESSION_SECRET: ${SESSION_SECRET}
    depends_on:
      - db

  db:
    image: postgres:16-alpine
    environment:
      POSTGRES_DB: nexus
      POSTGRES_USER: nexus_user
      POSTGRES_PASSWORD: ${DB_PASSWORD}
    volumes:
      - postgres_data:/var/lib/postgresql/data

volumes:
  postgres_data:
```

```bash
# Build and run
docker compose up --build

# Run Prisma migrations
docker compose exec web npx prisma migrate deploy
```

---

## 6. Static Export

If your app has no server-side features (no Server Actions, no dynamic routes, no server-only code), you can export it as static HTML files that can be hosted anywhere — GitHub Pages, any CDN, any web server.

```tsx
// next.config.ts
const nextConfig: NextConfig = {
  output: "export",
  // trailingSlash: true,  // generates /students/index.html instead of /students.html
};
```

```bash
npm run build
# Output: /out directory — static HTML, CSS, JS files
```

**Limitations of static export:**

- No Server Components that fetch data (everything must be static at build time)
- No Server Actions
- No API routes
- No middleware
- No ISR

For Nexus — not suitable. The school website needs server-side data (student records, news from DB, authentication). Static export is for brochure sites with no dynamic data.

---

## 7. Pre-Production Checklist

Before deploying Nexus to production:

### Environment

```bash
# All required variables set in production environment
DATABASE_URL=...
SESSION_SECRET=...  # min 32 chars, random
SMTP_HOST=...
SMTP_PASSWORD=...
NEXT_PUBLIC_APP_URL=https://cwwkcc.edu.lk
NODE_ENV=production
```

### Database

```bash
# Run migrations (not db push — that's dev only)
npx prisma migrate deploy

# Seed initial data if needed
npx prisma db seed
```

### Build

```bash
# Test production build locally first
npm run build
npm run start    # runs the production build

# Check for TypeScript errors
npx tsc --noEmit

# Check for lint errors
npm run lint
```

### Security headers

```tsx
// next.config.ts — add security headers
async headers() {
  return [
    {
      source: "/(.*)",
      headers: [
        { key: "X-Frame-Options",           value: "DENY" },
        { key: "X-Content-Type-Options",    value: "nosniff" },
        { key: "X-XSS-Protection",          value: "1; mode=block" },
        { key: "Referrer-Policy",           value: "strict-origin-when-cross-origin" },
        { key: "Permissions-Policy",        value: "camera=(), microphone=(), geolocation=()" },
      ],
    },
  ];
},
```

### Performance checks

```
✅ Images use next/image with correct sizes and priority on hero images
✅ Fonts loaded via next/font (no Google Fonts <link> tags)
✅ No console.log in production (compiler.removeConsole: true)
✅ No large unused dependencies in the bundle
✅ Static pages are static (○), not accidentally dynamic (λ)
✅ loading.tsx files for all data-heavy segments
✅ Error boundaries (error.tsx) for all data-fetching segments
```

### Post-deployment

```bash
# Test the deployed app
# Check all pages load
# Check forms and mutations work
# Check authentication flow (login, protected routes, logout)
# Check mobile layout
# Test in Sinhala locale (Nexus)
# Check images load and are optimised (WebP in Network tab)
```

---

## Summary

```
npm run build:
  Analyses every page — assigns Static (○), ISR (◐), or Dynamic (λ)
  Static: no server needed — fastest, served from CDN
  Dynamic: renders per request — needed for auth, user data, cookies

What makes a page dynamic:
  cookies(), headers(), searchParams — triggers dynamic rendering
  fetch with cache: "no-store" — forces dynamic
  Everything else → Next.js tries to make it static

Deployment options:
  Vercel — zero config, best DX, managed by Next.js team
  Docker — full control, VPS/school server, self-hosted
  Static export — only for apps with no server features (not Nexus)

Docker deployment:
  output: "standalone" in next.config.ts
  Multi-stage Dockerfile: deps → builder → runner
  docker-compose.yml for app + database
  npx prisma migrate deploy after container starts

Pre-production:
  All env vars set in production environment
  npx prisma migrate deploy (not db push)
  npm run build passes without errors
  Security headers in next.config.ts
  Images using next/image, fonts using next/font
  loading.tsx and error.tsx on every data-fetching segment
```

---

_Next: [11 — Internationalisation with next-intl](https://claude.ai/chat/11%20-%20Internationalisation%20with%20next-intl.md)_
