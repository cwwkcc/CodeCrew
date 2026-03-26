> **Series overview:** This is file 6 of 7. It covers Tailwind's variant system — how responsive breakpoints, interactive states (hover, focus, active), structural pseudo-classes (first, last, odd, even), group/peer variants, dark mode, and arbitrary variants work. Understanding variants unlocks the full power of Tailwind.

---

## Table of Contents

1. [Variants — The Modifier Prefix System](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#1-variants--the-modifier-prefix-system)
2. [Responsive Breakpoints — Mobile-First](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#2-responsive-breakpoints--mobile-first)
3. [Responsive Patterns In Practice](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#3-responsive-patterns-in-practice)
4. [Interactive State Variants](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#4-interactive-state-variants)
5. [Form State Variants](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#5-form-state-variants)
6. [Structural Pseudo-Class Variants](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#6-structural-pseudo-class-variants)
7. [Before & After Pseudo-Elements](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#7-before--after-pseudo-elements)
8. [Group Variant — Styling Children Based on Parent State](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#8-group-variant--styling-children-based-on-parent-state)
9. [Peer Variant — Styling Siblings Based on Sibling State](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#9-peer-variant--styling-siblings-based-on-sibling-state)
10. [Dark Mode](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#10-dark-mode)
11. [Print Variant](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#11-print-variant)
12. [Motion Variants](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#12-motion-variants)
13. [Arbitrary Variants](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#13-arbitrary-variants)
14. [Stacking Variants](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#14-stacking-variants)
15. [Container Queries](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#15-container-queries)

---

## 1. Variants — The Modifier Prefix System

Every Tailwind utility can be prefixed with a **variant** — a modifier that scopes the utility to a condition.

```
Syntax: {variant}:{utility}

Examples:
  hover:bg-blue-700       → applies bg-blue-700 when hovered
  focus:ring-2            → applies ring-2 when focused
  md:flex                 → applies flex at md breakpoint and up
  dark:bg-gray-900        → applies bg-gray-900 in dark mode
  disabled:opacity-50     → applies opacity-50 when disabled
  first:pt-0              → applies pt-0 to the first child
```

```css
/* What Tailwind generates for hover:bg-blue-700 */
.hover\:bg-blue-700:hover {
  background-color: rgb(29 78 216);
}

/* The backslash escapes the colon in the class name.
   This is a legal CSS class name — browsers handle it fine. */
```

---

## 2. Responsive Breakpoints — Mobile-First

Tailwind uses a mobile-first breakpoint system. An unprefixed utility applies at ALL screen sizes. A prefixed utility (`md:`) applies at that size **and above**.

```
Breakpoints:
  (none) → all screen sizes (≥ 0px)
  sm     → ≥ 640px
  md     → ≥ 768px
  lg     → ≥ 1024px
  xl     → ≥ 1280px
  2xl    → ≥ 1536px
```

```css
/* What md: generates */
@media (min-width: 768px) {
  .md\:flex { display: flex; }
}
```

```
Mental model — mobile-first means:

  "Apply text-sm by default (mobile),
   then override with text-base at md and above."

  <p class="text-sm md:text-base lg:text-lg">

  NOT:
  "Apply text-lg normally, then downscale on small screens."
  Tailwind has no max-width variants by default.

Why mobile-first?

  1. You design for the constrained case first.
  2. min-width media queries stack properly — each breakpoint adds
     rules on top of the previous layer.
  3. Content-heavy layouts degrade gracefully to small screens.
```

```html
<!-- Reading the breakpoints:
  hidden    → hidden on ALL screen sizes
  sm:block  → becomes block at 640px+
  
  Result: hidden on mobile, visible from small screens up -->
<div class="hidden sm:block">Only visible sm and up</div>

<!-- Visible only on mobile, hidden on larger screens -->
<div class="block sm:hidden">Mobile only</div>

<!-- Show on mobile and desktop, hide on tablet -->
<div class="block md:hidden lg:block">Visible except md</div>
```

### Custom Breakpoints

```js
// tailwind.config.js
module.exports = {
  theme: {
    screens: {
      // These REPLACE the defaults
      'tablet': '640px',
      'laptop': '1024px',
      'desktop': '1280px',
    },
    // OR extend (adds to defaults)
    extend: {
      screens: {
        'xs': '475px',   // adds xs: variant
        '3xl': '1920px', // adds 3xl: variant
      },
    },
  },
}
```

---

## 3. Responsive Patterns In Practice

### Responsive Typography

```html
<h1 class="text-3xl font-bold sm:text-4xl lg:text-5xl xl:text-6xl">
  Headline
  <!-- 1.875rem on mobile → 2.25rem at sm → 3rem at lg → 3.75rem at xl -->
</h1>

<p class="text-base leading-relaxed md:text-lg md:leading-loose">
  Body copy
</p>
```

### Responsive Layout Changes

```html
<!-- Stack on mobile, row on desktop -->
<div class="flex flex-col md:flex-row gap-6">
  <aside class="w-full md:w-64 flex-none">Sidebar</aside>
  <main class="flex-1">Content</main>
</div>

<!-- 1 → 2 → 3 column grid -->
<div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
  <div class="bg-white rounded-xl p-6">Card</div>
  <div class="bg-white rounded-xl p-6">Card</div>
  <div class="bg-white rounded-xl p-6">Card</div>
</div>
```

### Responsive Spacing

```html
<section class="px-4 py-8 sm:px-6 sm:py-12 lg:px-8 lg:py-20">
  <!-- Tighter padding on mobile, more breathing room on larger screens -->
</section>
```

### Responsive Visibility

```html
<!-- Show/hide content at breakpoints -->
<nav class="hidden lg:flex gap-6">Desktop nav links</nav>
<button class="lg:hidden">☰ Mobile menu</button>
```

---

## 4. Interactive State Variants

```html
<!-- Hover -->
<button class="bg-blue-500 hover:bg-blue-600">Hover me</button>
<a class="text-gray-700 hover:text-blue-600 hover:underline">Link</a>
<div class="opacity-80 hover:opacity-100 transition-opacity">Image</div>

<!-- Focus — for keyboard and click focus -->
<input class="border border-gray-300 focus:border-blue-500 focus:ring-2 focus:ring-blue-500/20 focus:outline-none rounded px-3 py-2">

<!-- Focus-visible — ONLY when focused via keyboard (not click)
     Preferred for focus styles — avoids showing ring on mouse click -->
<button class="focus-visible:ring-2 focus-visible:ring-blue-500 focus-visible:ring-offset-2">

<!-- Active — while being clicked/pressed -->
<button class="bg-blue-600 active:bg-blue-800 active:scale-95">Press me</button>

<!-- Visited — for links already visited -->
<a class="text-blue-600 visited:text-purple-600">Link</a>

<!-- Disabled — when [disabled] attribute is present -->
<button
  class="bg-blue-600 text-white disabled:bg-gray-300 disabled:text-gray-500 disabled:cursor-not-allowed"
  disabled
>
  Disabled
</button>

<!-- Checked — for checkboxes and radio buttons -->
<input type="checkbox"
  class="appearance-none w-4 h-4 border-2 border-gray-300 rounded
         checked:bg-blue-600 checked:border-blue-600">

<!-- Indeterminate — for partially checked states -->
<input type="checkbox"
  class="indeterminate:bg-gray-400">
```

---

## 5. Form State Variants

```html
<!-- required — when [required] attribute is present -->
<input class="required:border-red-300" required>

<!-- optional — when not required -->
<input class="optional:border-gray-200">

<!-- valid / invalid — based on HTML5 constraint validation -->
<input
  type="email"
  class="border border-gray-300 valid:border-green-500 invalid:border-red-500"
>
<!-- Note: invalid triggers immediately on page load before user types.
     Use :user-invalid (CSS) via arbitrary variant for better UX. -->

<!-- placeholder-shown — when showing placeholder -->
<input
  placeholder="Email"
  class="placeholder-shown:border-gray-200 not-placeholder-shown:border-blue-400"
>

<!-- autofill — browser-autofilled inputs -->
<input class="autofill:bg-yellow-100">

<!-- read-only -->
<input readonly class="read-only:bg-gray-50 read-only:text-gray-500 read-only:cursor-default">

<!-- in-range / out-of-range — for number/date inputs with min/max -->
<input type="number" min="1" max="10"
  class="in-range:border-green-500 out-of-range:border-red-500">
```

---

## 6. Structural Pseudo-Class Variants

```html
<!-- first: / last: — first and last sibling -->
<ul>
  <li class="border-b border-gray-200 first:pt-0 last:border-none py-3">
    Item — no top padding on first, no bottom border on last
  </li>
</ul>

<!-- odd: / even: — alternating rows -->
<tr class="odd:bg-white even:bg-gray-50">
  Table row zebra striping
</tr>

<!-- only: — when the element is the only child -->
<p class="only:text-center">Centered only when it's the only child</p>

<!-- first-of-type: / last-of-type: -->
<p class="first-of-type:mt-0">No top margin on the first paragraph</p>

<!-- nth-child (via arbitrary variant) -->
<li class="[&:nth-child(3)]:bg-blue-50">Third item highlighted</li>

<!-- empty: — element with no children or text -->
<div class="empty:hidden">Hides if there's no content inside</div>
```

---

## 7. Before & After Pseudo-Elements

```html
<!-- before: and after: create ::before and ::after pseudo-elements -->
<!-- content-[''] is required for the pseudo-element to appear -->

<!-- Decorative label before an element -->
<span class="before:content-['Required:_'] before:text-red-500 before:font-medium">
  Email address
</span>

<!-- Arrow after a link -->
<a class="after:content-['_→'] after:ml-1 text-blue-600">
  Read more
</a>

<!-- Absolute positioning pseudo-elements -->
<div class="relative before:absolute before:inset-0 before:bg-black/20 before:content-['']">
  Overlay effect on parent
</div>
```

---

## 8. Group Variant — Styling Children Based on Parent State

When you hover a parent, you may want to change a child's style. `group` and `group-hover:` make this possible without JavaScript.

```html
<!-- Step 1: mark the parent with class="group" -->
<!-- Step 2: use group-hover:, group-focus:, etc. on children -->

<a href="#" class="group flex items-center gap-3 p-4 rounded-lg hover:bg-gray-50">
  <div class="text-gray-400 group-hover:text-blue-500 transition-colors">
    <!-- Icon changes colour when the whole <a> is hovered -->
    <svg>...</svg>
  </div>
  <div>
    <p class="font-medium text-gray-900 group-hover:text-blue-600">
      Title
    </p>
    <p class="text-sm text-gray-500 group-hover:text-gray-700">
      Subtitle
    </p>
  </div>
  <svg class="ml-auto opacity-0 group-hover:opacity-100 transition-opacity text-blue-500">
    <!-- Arrow appears on hover of the whole card -->
  </svg>
</a>
```

### Named Groups

When you have nested groups, you need to name them to target the right one.

```html
<div class="group/outer">
  <div class="group/inner">
    <button class="group-hover/outer:visible group-hover/inner:bg-blue-50">
      <!-- visible when outer group is hovered -->
      <!-- blue bg when inner group is hovered -->
    </button>
  </div>
</div>
```

---

## 9. Peer Variant — Styling Siblings Based on Sibling State

`peer` targets the next sibling based on the state of the marked element.

```html
<!-- The peer element must come BEFORE the element being styled.
     peer styles target elements that follow in the DOM. -->

<!-- Floating label pattern -->
<div class="relative">
  <input
    id="email"
    type="email"
    placeholder=" "
    class="peer block w-full border rounded px-3 pt-6 pb-2 text-sm
           focus:outline-none focus:ring-2 focus:ring-blue-500"
  >
  <label
    for="email"
    class="absolute left-3 top-4 text-sm text-gray-400 transition-all duration-200
           peer-placeholder-shown:top-4 peer-placeholder-shown:text-sm
           peer-focus:top-1 peer-focus:text-xs peer-focus:text-blue-500
           peer-not-placeholder-shown:top-1 peer-not-placeholder-shown:text-xs"
  >
    Email
  </label>
</div>

<!-- Error message that appears when input is invalid -->
<input
  type="email"
  class="peer border rounded px-3 py-2 focus:outline-none
         invalid:border-red-500"
>
<p class="hidden peer-invalid:block text-sm text-red-500 mt-1">
  Please enter a valid email address.
</p>
```

---

## 10. Dark Mode

### Strategy 1 — Media Query (Automatic)

```js
// tailwind.config.js
darkMode: 'media',  // Uses prefers-color-scheme media query
```

```html
<!-- dark: prefix applies when OS is in dark mode -->
<div class="bg-white dark:bg-gray-900 text-gray-900 dark:text-white">
  This text and background auto-switch with OS dark mode preference.
</div>
```

### Strategy 2 — Class (Manual Toggle)

```js
// tailwind.config.js
darkMode: 'class',  // Requires .dark on <html>
```

```html
<!-- When <html class="dark"> is set: -->
<html class="dark">
  <body>
    <div class="bg-white dark:bg-gray-900">
      <!-- bg-gray-900 activates -->
    </div>
  </body>
</html>
```

```tsx
// Toggle dark mode with JavaScript
function toggleDark() {
  document.documentElement.classList.toggle('dark');
}

// Persist preference
function applyTheme() {
  const saved = localStorage.getItem('theme');
  const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
  
  if (saved === 'dark' || (!saved && prefersDark)) {
    document.documentElement.classList.add('dark');
  } else {
    document.documentElement.classList.remove('dark');
  }
}

// In Next.js, call applyTheme() before the body renders (in _document.tsx or
// a <Script strategy="beforeInteractive">) to avoid flash of wrong theme.
```

### Dark Mode in Practice

```html
<!-- Full dark mode design for a card -->
<div class="
  bg-white dark:bg-gray-800
  border border-gray-200 dark:border-gray-700
  rounded-xl shadow-sm dark:shadow-none
  p-6
">
  <h2 class="text-xl font-semibold text-gray-900 dark:text-white">
    Card title
  </h2>
  <p class="mt-2 text-gray-500 dark:text-gray-400">
    Descriptive text
  </p>
  <button class="
    mt-4 px-4 py-2 rounded-md text-sm font-medium
    bg-blue-600 dark:bg-blue-500
    text-white
    hover:bg-blue-700 dark:hover:bg-blue-400
  ">
    Action
  </button>
</div>
```

```
Good dark mode colour pairs:
  Light text:    gray-900  →  dark: white or gray-100
  Secondary text:gray-500  →  dark: gray-400
  Hint text:     gray-400  →  dark: gray-500
  Background:    white     →  dark: gray-900
  Card bg:       gray-50   →  dark: gray-800
  Border:        gray-200  →  dark: gray-700
  Subtle border: gray-100  →  dark: gray-800
```

---

## 11. Print Variant

```html
<!-- Hide elements when printing -->
<nav class="print:hidden">Navigation</nav>
<button class="print:hidden">Interactive button</button>

<!-- Show only when printing -->
<div class="hidden print:block">Print-only content (URL, notes, etc.)</div>

<!-- Adjust layout for print -->
<article class="max-w-3xl mx-auto print:max-w-none print:mx-0">
  <p class="text-base print:text-sm">Body text</p>
</article>
```

---

## 12. Motion Variants

```html
<!-- Respect prefers-reduced-motion user preference -->
<div class="
  transition-transform duration-300
  hover:scale-105
  motion-reduce:transition-none
  motion-reduce:hover:scale-100
">
  <!-- On devices with reduced motion preference:
       No transition, no scaling — just instant change -->
</div>

<!-- motion-safe: applies ONLY when reduced-motion is NOT active -->
<div class="motion-safe:animate-bounce">
  <!-- Bounces only when the user hasn't requested reduced motion -->
</div>
```

---

## 13. Arbitrary Variants

For CSS selectors Tailwind doesn't support natively.

```html
<!-- Apply a style when a specific selector matches -->
<div class="[&:nth-child(3)]:bg-blue-100">Third item gets blue background</div>

<!-- Descendant selectors -->
<div class="[&_p]:text-gray-700 [&_h2]:font-bold">
  <!-- All p inside this div get text-gray-700 -->
  <!-- All h2 inside this div get font-bold -->
  <p>This paragraph</p>
  <h2>This heading</h2>
</div>

<!-- State-based descendant -->
<div class="hover:[&_svg]:stroke-blue-500">
  <!-- All SVGs inside change stroke on hover of this div -->
  <svg>...</svg>
</div>

<!-- Combining with responsive -->
<div class="md:[&_td]:px-4">
  <!-- td elements inside get px-4 at md breakpoint -->
</div>
```

---

## 14. Stacking Variants

Variants can be stacked — each one adds a condition.

```html
<!-- hover + dark mode -->
<button class="bg-blue-600 hover:bg-blue-700 dark:bg-blue-500 dark:hover:bg-blue-400">

<!-- responsive + hover -->
<div class="opacity-70 md:hover:opacity-100">
  <!-- opacity-70 at all sizes, but at md+ hovering makes it fully visible -->
</div>

<!-- group-hover + dark mode -->
<svg class="text-gray-400 group-hover:text-blue-500 dark:group-hover:text-blue-400">

<!-- responsive + dark mode + hover -->
<p class="text-gray-700 dark:text-gray-300 lg:text-gray-900 lg:dark:text-white lg:dark:hover:text-blue-100">
  <!-- Progressive enhancement: each variant adds specificity -->
</p>
```

---

## 15. Container Queries

Container queries apply styles based on the size of a **parent element**, not the viewport. Requires `@tailwindcss/container-queries` plugin (bundled in Tailwind v4).

```bash
npm install @tailwindcss/container-queries
```

```js
// tailwind.config.js
plugins: [require('@tailwindcss/container-queries')],
```

```html
<!-- Step 1: Mark the container with @container -->
<div class="@container">
  <!-- Step 2: Use @sm:, @md:, @lg: etc. on children -->
  <div class="flex flex-col @md:flex-row gap-4">
    <!-- Stack vertically when the container is small,
         row when the container is >= 28rem (448px) wide -->
    <img class="w-full @md:w-48 flex-none">
    <div>
      <h2 class="text-base @lg:text-xl">Title</h2>
      <p class="text-sm @lg:text-base">Description</p>
    </div>
  </div>
</div>
```

```
Container query breakpoints (defaults):
  @xs    → 20rem  (320px)
  @sm    → 24rem  (384px)
  @md    → 28rem  (448px)
  @lg    → 32rem  (512px)
  @xl    → 36rem  (576px)
  @2xl   → 42rem  (672px)
  @3xl   → 48rem  (768px)
  @4xl   → 56rem  (896px)
  @5xl   → 64rem  (1024px)
  @6xl   → 72rem  (1152px)
  @7xl   → 80rem  (1280px)

Why container queries beat viewport breakpoints for components:
  A card component placed in a 3-column grid has a narrow container.
  The same card in a single-column layout has a wide container.
  With viewport breakpoints, you can't style both correctly — both
  have the same viewport width.
  With container queries, each card responds to its own container width.
```