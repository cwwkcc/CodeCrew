> **Series overview:** This is file 1 of 7. It covers what Tailwind CSS actually is, why it exists, how it generates CSS, and the mental shift from semantic class names to utility classes. Every concept is explained from first principles. Updated for **Tailwind v4** (CSS-first `@theme` config, the current default as of 2026) — v3's `tailwind.config.js` approach is kept throughout as clearly-labeled legacy content, since it's still what you'll find in most existing codebases.

---

## Table of Contents

1. [What Tailwind Is — And What It Is Not]
2. [The Problem Tailwind Solves]
3. [Utility-First vs Other Approaches]
4. [How Tailwind Generates CSS — The JIT Engine]
5. [Installing Tailwind]
6. [The Configuration File]
7. [The Spacing Scale — The Foundation of Everything]
8. [Reading Tailwind Class Names]
9. [Why Tailwind Classes Don't Conflict]
10. [Tailwind vs Bootstrap vs Plain CSS]
11. [The Mental Shift — Thinking in Utilities]

---

## 1. What Tailwind Is — And What It Is Not

Tailwind CSS is a **utility-first CSS framework**. It does not give you components. It gives you single-purpose CSS classes — one class, one rule.

```
Tailwind is NOT:
  - A component library (it has no .btn, .card, .modal classes)
  - A design system with pre-built UI components
  - Bootstrap

Tailwind IS:
  - A large set of small, single-purpose CSS classes
  - A build tool that generates only the CSS you actually use
  - A constraint system that enforces design consistency

A Bootstrap button:
  <button class="btn btn-primary">Click</button>

  .btn-primary {
    background-color: #0d6efd;
    border-color: #0d6efd;
    padding: 0.375rem 0.75rem;
    font-size: 1rem;
    border-radius: 0.375rem;
    /* ...more rules */
  }

  One class does many things. You get what Bootstrap decided.

A Tailwind button:
  <button class="bg-blue-600 text-white px-4 py-2 text-sm rounded-md">Click</button>

  Each class does exactly one thing.
  You compose them to get the result you want.
  You are in full control.
```

---

## 2. The Problem Tailwind Solves

### The CSS Naming Problem

Writing CSS means naming things. Naming things is hard.

```css
/* What do you call this? */
.container {
}
.wrapper {
}
.inner-wrapper {
}
.content-area {
}
.main-content {
}

/* These names mean nothing. They just say "it wraps stuff". */
/* In a large codebase you get hundreds of these. */
/* No one knows what they do without reading the CSS. */
```

### The CSS Growth Problem

CSS files only ever grow. Deleting CSS is dangerous — something might still use it.

```css
/* Added in 2022 for the homepage hero */
.hero-title {
  font-size: 3rem;
  font-weight: 700;
  color: #1a1a1a;
}

/* Is this still used in 2024? No way to know without checking. */
/* So it stays. Forever. */
```

### The CSS Specificity Problem

```css
/* In one file */
.card-header h2 {
  color: blue;
}

/* In another file */
.title {
  color: red;
}

/* Which wins? Depends on file load order AND specificity. */
/* .card-header h2 wins — it's more specific (two selectors). */
/* This produces invisible coupling between unrelated files. */
```

### How Tailwind Sidesteps All of This

```
No naming: you write classes directly in HTML. No naming ceremony.

No dead CSS: the build tool scans your HTML and generates ONLY the
CSS for classes that actually appear. Nothing extra.

No specificity conflicts: every utility class targets a single
element with a single declaration. Specificity is always equal.
The last class wins, predictably.
```

---

## 3. Utility-First vs Other Approaches

### Inline Styles

```html
<!-- Inline styles — maximum control, zero reuse, hard to maintain -->
<div
  style="display: flex; align-items: center; padding: 16px; background-color: #f3f4f6;"
></div>

Problems: - Can't use :hover, :focus, media queries, or any pseudo-classes -
Verbose and repetitive - No design constraints — every number is a freeform
value
```

### Semantic CSS (BEM, OOCSS)

```html
<!-- Semantic CSS — meaningful names, but brittle -->
<div class="card card--featured">
  <h2 class="card__title">Hello</h2>
</div>
```

```css
/* You write this CSS separately */
.card {
  padding: 16px;
  background: white;
  border-radius: 8px;
}

.card--featured {
  border: 2px solid blue;
}

.card__title {
  font-size: 1.25rem;
  font-weight: 600;
}
```

```
Problems:
  - Context switching between HTML and CSS constantly
  - CSS file grows indefinitely
  - The "semantic" name becomes a lie as the design evolves
  - .card now does 15 different things and none of them are named
```

### CSS Modules

```jsx
// CardTitle.module.css
.title {
  font-size: 1.25rem;
  font-weight: 600;
  color: #111;
}

// CardTitle.jsx
import styles from './CardTitle.module.css';
<h2 className={styles.title}>Hello</h2>
```

```
Better: scoped to the component, no global conflicts.
Still: separate file for every component, still naming things,
       still a growing collection of small CSS files.
```

### CSS-in-JS (styled-components, Emotion)

```jsx
const Title = styled.h2`
  font-size: 1.25rem;
  font-weight: 600;
  color: #111;
`;

<Title>Hello</Title>;
```

```
Better: collocated with component, fully dynamic with props.
Cost: runtime overhead (styles injected into <style> tags at runtime),
      harder to cache, larger bundles.
```

### Tailwind — Utilities in the HTML

```html
<h2 class="text-xl font-semibold text-gray-900">Hello</h2>
```

```
The HTML is the stylesheet.
No context switching.
No naming.
No dead CSS.
The component fully describes its own appearance.

Trade-off: HTML gets verbose. Long class strings are a real cost.
Tailwind's answer: you can always extract repeated patterns into
components (in React/Vue) or use @apply in CSS.
```

---

## 4. How Tailwind Generates CSS — The JIT Engine

Before Tailwind v3, the full Tailwind stylesheet was ~3MB. You'd include it in development and then "purge" unused classes before production. This was error-prone.

Since v3, Tailwind uses a **Just-In-Time (JIT) compiler** that works differently.

```
Old way (pre-v3):
  1. Tailwind generates ALL possible utility classes (~3MB CSS)
  2. PurgeCSS scans your files and removes unused classes
  3. Result: small production CSS

JIT way (v3+):
  1. Tailwind starts with an empty stylesheet
  2. You write code — your editor saves a file
  3. Tailwind scans that file for class names
  4. Tailwind generates CSS ONLY for the classes it found
  5. Your dev CSS is already small — no purge step needed

The JIT engine watches your files in real time.
The CSS file only ever contains what you're actually using.
```

> **v4 note:** the JIT model above is still exactly how Tailwind thinks — only-generate-what's-used hasn't changed. What changed is the engine underneath it: v4 replaced the v3 JavaScript/PostCSS pipeline with **Oxide**, a Rust engine that does the same scan-and-generate job several times faster, with near-instant incremental rebuilds. The mental model in this section is current; only the implementation got faster.

### Why JIT Enables Arbitrary Values

Before JIT, only pre-generated classes existed. `text-[17px]` was impossible — that CSS was never generated.

With JIT, any class can be generated on demand:

```html
<!-- Arbitrary values — any CSS value in square brackets -->
<div class="w-[320px] top-[117px] bg-[#bada55] text-[22px]"></div>

<!-- Tailwind sees these during build, generates exactly: -->
<!-- .w-\[320px\] { width: 320px; } -->
<!-- .top-\[117px\] { top: 117px; } -->
<!-- .bg-\[\#bada55\] { background-color: #bada55; } -->
<!-- .text-\[22px\] { font-size: 22px; } -->
```

### How Content Scanning Works

```js
// tailwind.config.js
module.exports = {
  content: [
    "./src/**/*.{html,js,jsx,ts,tsx}", // scan all these files
    "./pages/**/*.{js,ts,jsx,tsx}",
    "./components/**/*.{js,ts,jsx,tsx}",
  ],
  // ...
};
```

```
Tailwind uses a fast text scanner — it finds strings that look like
class names. It does NOT parse your JS or understand logic.

This means:
  SAFE — the full string exists in source:
    className="bg-red-500"
    className={isError ? "bg-red-500" : "bg-green-500"}

  UNSAFE — string is assembled at runtime:
    const color = "red";
    className={`bg-${color}-500`}   ← Tailwind never sees "bg-red-500"
                                      It only sees the template literal

If you need dynamic classes, safelist them in config or keep full
class strings in a lookup object.
```

---

## 5. Installing Tailwind

> **As of Tailwind v4 (current since 2025, and the default for any new project in 2026), installation is CSS-first — there's no `tailwind.config.js` and no `@tailwind` directives to remember.** The v3 workflow below is still shown because you'll run into it constantly in older codebases, tutorials, and Stack Overflow answers — but don't set up a new project this way.

### v4 — Current Default

```bash
# Vite-based projects (recommended integration)
npm install tailwindcss @tailwindcss/vite
```

```ts
// vite.config.ts
import { defineConfig } from "vite";
import tailwindcss from "@tailwindcss/vite";

export default defineConfig({
  plugins: [tailwindcss()],
});
```

```css
/* app.css — this one line replaces all three @tailwind directives */
@import "tailwindcss";
```

```bash
# Framework using PostCSS instead of Vite (e.g. some Next.js setups)
npm install tailwindcss @tailwindcss/postcss
```

```js
// postcss.config.js
export default {
  plugins: {
    "@tailwindcss/postcss": {},
  },
};
```

```bash
# No build tool at all — standalone CLI
npx @tailwindcss/cli -i ./src/input.css -o ./dist/output.css --watch
```

There is no `content: [...]` array to maintain — v4 automatically detects which files use Tailwind classes by scanning your project (respecting `.gitignore`). For the rare case where a file lives outside the normal scan path, `@source "../some-other-folder";` in your CSS adds it explicitly.

### v3 — What You'll See in Older Projects (Legacy)

```bash
npm install -D tailwindcss postcss autoprefixer
npx tailwindcss init -p
```

```css
/* src/input.css — the v3 way: three directives instead of one @import */
@tailwind base; /* Preflight — Tailwind's CSS reset */
@tailwind components; /* Where @apply component classes go */
@tailwind utilities; /* All utility classes */
```

```js
// tailwind.config.js — a whole JS file just to declare which files to scan
/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./src/**/*.{html,js,ts,jsx,tsx}"],
  theme: { extend: {} },
  plugins: [],
};
```

If you open a project and see `@tailwind base;` or a `tailwind.config.js`, it's running v3 (or v4 in compatibility mode via `@config`). That's fine — v3 is still maintained and plenty of production apps run it — but write new projects the v4 way.

### What Preflight Does

Both versions ship the same reset, called **Preflight**, built on top of `modern-normalize`. It removes browser default styles so you're styling from a blank canvas rather than fighting `<h1>`'s default size or a `<button>`'s default border:

```css
/* What Preflight does (simplified): */
*,
*::before,
*::after {
  box-sizing: border-box;
}
h1,
h2,
h3,
h4,
h5,
h6 {
  font-size: inherit;
  font-weight: inherit;
}
a {
  color: inherit;
  text-decoration: inherit;
}
button {
  background-color: transparent;
  padding: 0;
}
```

---

## 6. The Configuration File — Then and Now

> **v4 moves configuration out of JavaScript and into CSS.** `tailwind.config.js` still works during migration (see Module 06's migration guide), but a new project doesn't need one at all.

### v4 — `@theme` in CSS

```css
/* app.css */
@import "tailwindcss";

@theme {
  --color-brand-50: #eff6ff;
  --color-brand-500: #3b82f6;
  --color-brand-900: #1e3a8a;

  --font-sans: "Inter", sans-serif;

  --spacing-128: 32rem; /* generates p-128, m-128, w-128, etc. — same as v3 */
}
```

Every value defined in `@theme` automatically becomes both a Tailwind utility _and_ a real CSS custom property (`var(--color-brand-500)`), usable anywhere in your CSS — not just inside class names. That dual nature is the headline change: in v3, config values only existed at build time; in v4, they exist at runtime as inspectable CSS variables.

### v3 — `tailwind.config.js` (Legacy)

```js
/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./src/**/*.{html,js,ts,jsx,tsx}"],
  darkMode: "class",
  theme: {
    extend: {
      colors: {
        brand: { 50: "#eff6ff", 500: "#3b82f6", 900: "#1e3a8a" },
      },
      fontFamily: { sans: ["Inter", "sans-serif"] },
      spacing: { 128: "32rem" },
    },
  },
  plugins: [require("@tailwindcss/forms"), require("@tailwindcss/typography")],
};
```

Same result, different mechanism: v3 values live only inside the JS build step; v4 values are real CSS the browser can see. Module 06 covers the full `@theme` API and the v3 → v4 migration in depth, including the handful of breaking changes (renamed utilities, a changed default border colour) that trip people up on the way over.

---

## 7. Your First Tailwind HTML

```html
<!-- A card component built entirely with utilities -->
<div class="max-w-sm mx-auto bg-white rounded-xl shadow-md overflow-hidden">
  <div class="p-6">
    <p class="text-sm font-medium text-blue-600 uppercase tracking-wide">
      Case Study
    </p>
    <h2 class="mt-1 text-xl font-semibold text-gray-900 leading-tight">
      Finding customers for your new business
    </h2>
    <p class="mt-2 text-gray-500">
      Getting a new business off the ground is a lot of work.
    </p>
  </div>
</div>
```

```
Reading each class:
  max-w-sm         → max-width: 24rem (384px)
  mx-auto          → margin-left: auto; margin-right: auto  (centers block)
  bg-white         → background-color: rgb(255 255 255)
  rounded-xl       → border-radius: 0.75rem
  shadow-md        → box-shadow: (a medium drop shadow)
  overflow-hidden  → overflow: hidden
  p-6              → padding: 1.5rem (24px) on all sides
  text-sm          → font-size: 0.875rem; line-height: 1.25rem
  font-medium      → font-weight: 500
  text-blue-600    → color: rgb(37 99 235)
  uppercase        → text-transform: uppercase
  tracking-wide    → letter-spacing: 0.025em
  mt-1             → margin-top: 0.25rem
  text-xl          → font-size: 1.25rem; line-height: 1.75rem
  font-semibold    → font-weight: 600
  text-gray-900    → color: rgb(17 24 39)
  leading-tight    → line-height: 1.25
  mt-2             → margin-top: 0.5rem
  text-gray-500    → color: rgb(107 114 128)
```

---

## 8. The Spacing Scale — The Foundation of Everything

Tailwind uses a consistent spacing scale based on a 4px base unit. This scale is used by padding, margin, width, height, gap, and more.

```
Number → rem value → px (at 16px root font size)

0   →  0rem     →   0px
0.5 →  0.125rem →   2px
1   →  0.25rem  →   4px
1.5 →  0.375rem →   6px
2   →  0.5rem   →   8px
2.5 →  0.625rem →  10px
3   →  0.75rem  →  12px
3.5 →  0.875rem →  14px
4   →  1rem     →  16px
5   →  1.25rem  →  20px
6   →  1.5rem   →  24px
7   →  1.75rem  →  28px
8   →  2rem     →  32px
9   →  2.25rem  →  36px
10  →  2.5rem   →  40px
11  →  2.75rem  →  44px
12  →  3rem     →  48px
14  →  3.5rem   →  56px
16  →  4rem     →  64px
20  →  5rem     →  80px
24  →  6rem     →  96px
28  →  7rem     → 112px
32  →  8rem     → 128px
36  →  9rem     → 144px
40  → 10rem     → 160px
44  → 11rem     → 176px
48  → 12rem     → 192px
52  → 13rem     → 208px
56  → 14rem     → 224px
60  → 15rem     → 240px
64  → 16rem     → 256px
72  → 18rem     → 288px
80  → 20rem     → 320px
96  → 24rem     → 384px
```

```
Why a scale?

Design consistency. If every developer picks arbitrary pixel values,
you end up with margins of 13px, 15px, 17px, 22px all over the UI.
Nothing aligns. Nothing feels cohesive.

The scale forces you to choose from a finite set of values.
p-4 and p-6 are both valid. p-5.3 doesn't exist.
This constraint is a feature — it produces visual harmony.
```

---

## 9. Reading Tailwind Class Names

Most Tailwind classes follow predictable patterns. Once you learn the patterns, you can guess any class.

```
Pattern: [property]-[value]

bg-red-500      → background-color: red at shade 500
text-gray-700   → color: gray at shade 700
border-blue-300 → border-color: blue at shade 300
p-4             → padding: 1rem (all sides)
px-4            → padding-left + padding-right: 1rem
py-4            → padding-top + padding-bottom: 1rem
pt-4            → padding-top: 1rem
pl-4            → padding-left: 1rem
m-4             → margin: 1rem
mx-auto         → margin-left: auto; margin-right: auto
-mt-4           → margin-top: -1rem (negative)
w-full          → width: 100%
w-1/2           → width: 50%
h-screen        → height: 100vh
min-h-0         → min-height: 0
max-w-lg        → max-width: 32rem
text-lg         → font-size: 1.125rem; line-height: 1.75rem
font-bold       → font-weight: 700
leading-relaxed → line-height: 1.625
tracking-tight  → letter-spacing: -0.025em
rounded-full    → border-radius: 9999px (pill shape)
shadow-lg       → large drop shadow
opacity-50      → opacity: 0.5
cursor-pointer  → cursor: pointer
select-none     → user-select: none
```

### Directional Shorthands

```
t  = top
r  = right
b  = bottom
l  = left
x  = left + right (horizontal)
y  = top + bottom (vertical)

pt-4   → padding-top: 1rem
pr-4   → padding-right: 1rem
pb-4   → padding-bottom: 1rem
pl-4   → padding-left: 1rem
px-4   → padding-left: 1rem; padding-right: 1rem
py-4   → padding-top: 1rem; padding-bottom: 1rem

Same pattern for margin (mt, mr, mb, ml, mx, my)
and for border-width (border-t, border-r, border-b, border-l)
```

---

## 10. Why Tailwind Classes Don't Conflict

Each Tailwind utility class has the same CSS specificity: `0,1,0` — a single class selector.

```css
/* Both have identical specificity */
.bg-red-500 {
  background-color: rgb(239 68 68);
}
.bg-blue-500 {
  background-color: rgb(59 130 246);
}
```

```html
<!-- When both appear, the LAST one in the stylesheet wins -->
<!-- Tailwind's stylesheet lists bg-blue before bg-red alphabetically -->
<!-- So bg-red-500 wins here — last in the generated CSS -->
<div class="bg-blue-500 bg-red-500">
  <!-- But this is undefined behaviour. Don't apply two values for
     the same property on the same element. Use conditional logic. -->
</div>
```

### Conditional Classes in React

```tsx
// Use ternary or clsx/cn for conditional classes
<button
  className={`px-4 py-2 rounded font-medium ${
    isActive ? 'bg-blue-600 text-white' : 'bg-gray-100 text-gray-700'
  }`}
>

// clsx (npm package) — cleaner for multiple conditions
import clsx from 'clsx';

<button
  className={clsx(
    'px-4 py-2 rounded font-medium',
    isActive && 'bg-blue-600 text-white',
    !isActive && 'bg-gray-100 text-gray-700',
    isDisabled && 'opacity-50 cursor-not-allowed',
  )}
>

// tailwind-merge (npm package) — resolves conflicting utilities
// clsx just concatenates strings; tailwind-merge deduplicates conflicts
import { twMerge } from 'tailwind-merge';

// Without tailwind-merge: "p-4 p-6" — both in output (wasteful)
// With tailwind-merge:    "p-4 p-6" → "p-6" (last wins, deduplicated)

// Common pattern — combine both:
import { clsx } from 'clsx';
import { twMerge } from 'tailwind-merge';

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}
```

---

## 11. Tailwind vs Bootstrap vs Plain CSS

| Dimension            | Plain CSS             | Bootstrap                     | Tailwind                          |
| -------------------- | --------------------- | ----------------------------- | --------------------------------- |
| Learning curve       | Low (if you know CSS) | Low (learn component classes) | Medium (learn utility vocabulary) |
| CSS file size (prod) | Depends on discipline | ~30KB (with purge)            | ~5–15KB                           |
| Design constraint    | None                  | Bootstrap's design system     | Your configured scale             |
| Custom design        | Full control          | Fight the framework           | Full control                      |
| Component look       | Fully custom          | Bootstrap look                | Fully custom                      |
| Naming required      | Yes                   | Minimal                       | None                              |
| Dead CSS             | Common                | Handled by purge              | Never generated                   |
| Responsive           | Manual media queries  | Grid classes (col-md-6)       | Breakpoint prefixes (md:)         |
| Dark mode            | Manual                | Requires overrides            | Built-in variant                  |

---

## 12. The Mental Shift — Thinking in Utilities

The hardest part of Tailwind is unlearning the habit of reaching for a CSS file.

```
Old mental model:
  1. Write HTML structure
  2. Add a class name that describes the element
  3. Open (or create) a CSS file
  4. Write styles for that class
  5. Switch back to HTML

New mental model:
  1. Write HTML structure
  2. Apply utility classes directly
  3. Done
```

### What "Components" Mean in Tailwind

```tsx
// In React/Next.js, your component IS the reuse mechanism.
// You don't need a .card CSS class — you have a <Card> component.

function Card({ title, body }: { title: string; body: string }) {
  return (
    <div className="bg-white rounded-xl shadow-md p-6">
      <h2 className="text-lg font-semibold text-gray-900">{title}</h2>
      <p className="mt-2 text-sm text-gray-500">{body}</p>
    </div>
  );
}

// The utility classes ARE the component's styles.
// You never write .card { } anywhere.
// Reuse is achieved by reusing the component, not the CSS class.
```

### When to Use @apply

```css
/* @apply lets you extract utilities into a CSS class.
   Use it sparingly — only when you can't use a component. */

/* GOOD: for elements you can't componentize (markdown prose, 
   third-party HTML you don't control) */
.prose h2 {
  @apply text-2xl font-bold text-gray-900 mt-8 mb-4;
}

/* BAD: for things you can just make a React component */
.card {
  @apply bg-white rounded-xl shadow-md p-6;
}
/* This brings back all the problems Tailwind was designed to avoid:
   a CSS file you have to maintain, a name you had to invent,
   and dead CSS risk. Just use <Card /> instead. */
```
