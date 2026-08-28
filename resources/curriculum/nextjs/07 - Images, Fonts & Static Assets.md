> Images are the most common performance bottleneck on websites. Next.js has a built-in `<Image>` component that handles resizing, format conversion, lazy loading, and layout shift prevention automatically. Fonts have a similar story. This file covers both, plus how to handle other static files.

---

## Table of Contents

1. [The Image Component](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-image-component)
2. [Local Images vs Remote Images](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-local-images-vs-remote-images)
3. [Image Sizing Strategies](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-image-sizing-strategies)
4. [Image Optimisation Under the Hood](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-image-optimisation-under-the-hood)
5. [next/font — Zero Layout Shift Fonts](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-nextfont--zero-layout-shift-fonts)
6. [Static Assets in public/](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-static-assets-in-public)
7. [SVGs](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-svgs)

---

## 1. The Image Component

Never use `<img>` in Next.js. Use `<Image>` from `next/image`.

```tsx
import Image from "next/image";

// ❌ Raw <img> — no optimisation, no lazy loading, causes layout shift
<img src="/school-photo.jpg" alt="C.W.W. Kannangara Central College" />

// ✅ <Image> — optimised, lazy loaded, no layout shift
<Image
  src="/school-photo.jpg"
  alt="C.W.W. Kannangara Central College"
  width={1200}
  height={630}
/>
```

What `<Image>` does automatically:

- Converts to WebP/AVIF (30-50% smaller than JPEG/PNG)
- Generates multiple sizes and serves the right one for the device
- Lazy loads by default (only loads when near viewport)
- Prevents layout shift by reserving space before the image loads
- Caches optimised images at the CDN or server level

---

## 2. Local Images vs Remote Images

### Local images (in the project)

```tsx
// Import the image — Next.js knows the dimensions at build time
import schoolPhoto from "@/public/school-photo.jpg";
import Image from "next/image";

export default function HeroSection() {
  return (
    <Image
      src={schoolPhoto} // TypeScript knows: src, width, height
      alt="School building"
      priority // load immediately (not lazy) — use for above-the-fold
    />
  );
}
```

When you import a local image, TypeScript knows `width` and `height` automatically — no need to specify them. `priority` tells Next.js to preload the image (use this for the most important image on the page — hero, logo above the fold).

### Remote images (URLs from database, CMS, etc.)

```tsx
// Must specify width and height — Next.js can't know them ahead of time
<Image
  src="https://storage.example.com/photos/student-42.jpg"
  alt="Student profile photo"
  width={400}
  height={400}
/>
```

Remote image URLs also need to be whitelisted in `next.config.ts` — Next.js won't optimise images from domains it doesn't know about (security):

```tsx
// next.config.ts
import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  images: {
    remotePatterns: [
      {
        protocol: "https",
        hostname: "storage.example.com",
        pathname: "/photos/**",
      },
      {
        protocol: "https",
        hostname: "**.supabase.co", // wildcard subdomain
      },
    ],
  },
};

export default nextConfig;
```

---

## 3. Image Sizing Strategies

### Fixed size

```tsx
// Image renders at exactly this size
<Image
  src="/avatar.jpg"
  alt="Profile"
  width={80}
  height={80}
  className="rounded-full"
/>
```

### Responsive (fills available width)

```tsx
// Image scales to fill its container
// Container MUST have a defined size (width + height, or aspect-ratio)
<div className="relative w-full aspect-video">
  <Image
    src="/hero.jpg"
    alt="Hero"
    fill // fills the parent
    sizes="100vw" // hint: this image is 100% of viewport width
    className="object-cover"
    priority
  />
</div>
```

`fill` makes the image fill its container. The container needs `position: relative` (added by Tailwind's `relative` class) and defined dimensions.

```tsx
// Card with responsive image — image fills the card's image slot
function StudentCard({ student }) {
  return (
    <div className="card">
      <div className="relative h-48 w-full">
        <Image
          src={student.photoUrl}
          alt={student.name}
          fill
          sizes="(max-width: 768px) 100vw, (max-width: 1200px) 50vw, 33vw"
          className="object-cover"
        />
      </div>
      <div className="p-4">
        <h3>{student.name}</h3>
      </div>
    </div>
  );
}
```

### The `sizes` prop

`sizes` tells the browser how wide the image will be at different viewport sizes — used to select the right pre-generated size. It uses CSS media query syntax.

```tsx
// "This image is:
//   full width on mobile (<768px)
//   half width on tablet (<1200px)
//   one third width on desktop"
sizes = "(max-width: 768px) 100vw, (max-width: 1200px) 50vw, 33vw";

// Hero image — always full width
sizes = "100vw";

// Fixed size — always 400px
sizes = "400px";

// Sidebar widget — always 300px
sizes = "300px";
```

Getting `sizes` right means the browser downloads the smallest image that still looks sharp — saves bandwidth.

---

## 4. Image Optimisation Under the Hood

When a browser requests an image through Next.js's `<Image>`:

1. Request hits `/_next/image?url=...&w=800&q=75`
2. Next.js fetches the original image (local file or remote URL)
3. Resizes to the requested width
4. Converts to WebP (or AVIF)
5. Compresses at quality 75 (configurable)
6. Caches the result
7. Returns the optimised image

Subsequent requests for the same image/size/format return the cached version instantly.

### Quality setting

```tsx
// Default quality is 75 — good balance of size and sharpness
<Image src="/photo.jpg" alt="..." width={800} height={600} />

// Increase for hero images where quality is critical
<Image src="/hero.jpg" alt="..." width={1200} height={630} quality={90} priority />

// Decrease for thumbnails where file size matters more
<Image src="/thumb.jpg" alt="..." width={100} height={100} quality={60} />
```

---

## 5. next/font — Zero Layout Shift Fonts

Loading fonts with `<link>` from Google Fonts causes a flash of unstyled text (FOUT) or layout shift — the page renders with a fallback font, then jumps when the real font loads.

`next/font` downloads fonts at build time and self-hosts them with zero layout shift.

### Google Fonts

```tsx
// app/layout.tsx
import { Cormorant_Garamond, IBM_Plex_Sans } from "next/font/google";

// Configure at module level — runs at build time
const cormorant = Cormorant_Garamond({
  subsets: ["latin"],
  weight: ["400", "600", "700"],
  style: ["normal", "italic"],
  variable: "--font-cormorant", // expose as CSS variable
  display: "swap", // use fallback until font is ready
});

const ibmPlex = IBM_Plex_Sans({
  subsets: ["latin"],
  weight: ["400", "500", "600"],
  variable: "--font-ibm-plex",
  display: "swap",
});

export default function RootLayout({ children }) {
  return (
    // Apply CSS variables to <html> so they're available everywhere
    <html lang="en" className={`${cormorant.variable} ${ibmPlex.variable}`}>
      <body>{children}</body>
    </html>
  );
}
```

```css
/* globals.css — use the CSS variables in Tailwind or plain CSS */
:root {
  --font-cormorant: /* set by next/font */;
  --font-ibm-plex: /* set by next/font */;
}
```

```tsx
// tailwind.config.ts — map CSS variables to Tailwind font families
theme: {
  extend: {
    fontFamily: {
      cormorant: ["var(--font-cormorant)", "serif"],
      sans:      ["var(--font-ibm-plex)", "sans-serif"],
    },
  },
},

// Usage in JSX
<h1 className="font-cormorant text-4xl">School Name</h1>
<p className="font-sans text-base">Body text</p>
```

### Local fonts

```tsx
import localFont from "next/font/local";

const nexusFont = localFont({
  src: [
    { path: "./fonts/NexusFont-Regular.woff2", weight: "400" },
    { path: "./fonts/NexusFont-SemiBold.woff2", weight: "600" },
  ],
  variable: "--font-nexus",
});
```

Font files go in `app/fonts/` or `public/fonts/`. Using `app/fonts/` is preferred — files are co-located with the layout and not publicly accessible.

### Why zero layout shift?

`next/font` generates a `size-adjust` CSS value for the fallback font that matches the metrics of the real font as closely as possible. When the font swaps, the text takes up the same space — no jump.

---

## 6. Static Assets in public/

Files in the `public/` folder are served at the root URL:

```
public/
  favicon.ico          →  /favicon.ico
  og/
    default.jpg        →  /og/default.jpg
  documents/
    prospectus.pdf     →  /documents/prospectus.pdf
  school-logo.svg      →  /school-logo.svg
```

```tsx
// Reference public files by their URL path (not a file path)
<Image src="/school-logo.svg" alt="School logo" width={120} height={60} />
<a href="/documents/prospectus.pdf" download>Download Prospectus</a>
```

### What goes in public/

```
✅ Good for public/:
  Favicon, app icons (favicon.ico, apple-touch-icon.png)
  Open Graph images (/og/)
  PDFs and downloadable documents
  Static data files (sitemap.xml, robots.txt)
  Videos (though a CDN is better for large files)

❌ Not for public/:
  Images used in <Image> component — can go anywhere, but public/ is common
  Fonts — use app/fonts/ with next/font instead
  Sensitive files — public/ is world-readable
```

### robots.txt and sitemap.xml

These can be static files in `public/`, or generated dynamically:

```tsx
// app/robots.ts — generates /robots.txt
import type { MetadataRoute } from "next";

export default function robots(): MetadataRoute.Robots {
  return {
    rules: { userAgent: "*", allow: "/" },
    sitemap: "https://cwwkcc.edu.lk/sitemap.xml",
  };
}

// app/sitemap.ts — generates /sitemap.xml
import type { MetadataRoute } from "next";
import { prisma } from "@/lib/prisma";

export default async function sitemap(): Promise<MetadataRoute.Sitemap> {
  const posts = await prisma.post.findMany({
    select: { slug: true, updatedAt: true },
    where: { published: true },
  });

  return [
    { url: "https://cwwkcc.edu.lk/en", lastModified: new Date() },
    { url: "https://cwwkcc.edu.lk/en/about", lastModified: new Date() },
    ...posts.map((post) => ({
      url: `https://cwwkcc.edu.lk/en/news/${post.slug}`,
      lastModified: post.updatedAt,
    })),
  ];
}
```

---

## 7. SVGs

For SVGs, you have three options depending on the use case.

### As an image file (simple icons, logos)

```tsx
import Image from "next/image";

// Works fine for static SVGs that don't need colour control
<Image src="/icons/book.svg" alt="Library" width={24} height={24} />;
```

### Inline as JSX (for colour control via Tailwind)

```tsx
// components/icons/BookIcon.tsx
export default function BookIcon({ className }: { className?: string }) {
  return (
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"  // inherits text colour — control with Tailwind
      strokeWidth={2}
      className={className}
    >
      <path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20" />
      <path d="M6.5 2H20v20H6.5A2.5 2.5 0 0 1 4 19.5v-15A2.5 2.5 0 0 1 6.5 2z" />
    </svg>
  );
}

// Usage — inherits text colour, sizable with Tailwind
<BookIcon className="w-5 h-5 text-primary" />
<BookIcon className="w-8 h-8 text-muted-foreground" />
```

### As a React component with SVGR

```bash
npm install --save-dev @svgr/webpack
```

```tsx
// next.config.ts
const nextConfig = {
  webpack(config) {
    config.module.rules.push({
      test: /\.svg$/,
      use: ["@svgr/webpack"],
    });
    return config;
  },
};

// Usage — SVG files imported as React components
import SchoolLogo from "@/public/school-logo.svg";

<SchoolLogo className="w-32 h-auto" />;
```

For Nexus, inline JSX icons (using Lucide React or hand-written) are the cleanest approach — full TypeScript support, colour via `currentColor`, size via Tailwind.

---

## Summary

```
<Image> component:
  Always use instead of <img> in Next.js
  Automatic WebP conversion, lazy loading, layout shift prevention
  Local images: import and use directly — width/height inferred
  Remote images: specify width/height + whitelist domain in next.config.ts
  fill + relative container: responsive images that fill their parent
  sizes prop: hints at display size for correct source selection
  priority: preload above-the-fold images (hero, logo)
  quality: 75 default, raise for hero images, lower for thumbnails

next/font:
  Import from "next/font/google" or "next/font/local"
  Configure at module level — runs at build time
  Use variable option to expose as CSS custom property
  Apply the variable className to <html> in root layout
  Map to Tailwind fontFamily via var(--font-name)
  Zero layout shift — generates matching fallback metrics

public/ folder:
  Files served at root URL: public/logo.svg → /logo.svg
  Use for: favicons, OG images, PDFs, robots.txt, sitemap.xml
  Better to generate sitemap.xml dynamically from app/sitemap.ts

SVGs:
  Static file via <Image>: simple, no colour control
  Inline JSX: full colour control via currentColor + Tailwind
  SVGR: import .svg files as React components — needs webpack config
  Nexus uses Lucide React + inline JSX for icons
```

---

_Next: [08 — Middleware & Authentication](https://claude.ai/chat/08%20-%20Middleware%20%26%20Authentication.md)_
