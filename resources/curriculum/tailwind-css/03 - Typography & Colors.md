> **Series overview:** This is file 4 of 7. It covers all text styling utilities (font family, size, weight, line height, letter spacing, colour, alignment, decoration) and the complete Tailwind colour system — how the palette is structured, how colour classes work, and how to apply colour transparency.

---

## Table of Contents

1. [Font Family](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#1-font-family)
2. [Font Size — The Type Scale](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#2-font-size--the-type-scale)
3. [Font Weight](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#3-font-weight)
4. [Font Style & Variant](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#4-font-style--variant)
5. [Line Height](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#5-line-height)
6. [Letter Spacing (Tracking)](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#6-letter-spacing-tracking)
7. [Text Alignment](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#7-text-alignment)
8. [Text Colour](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#8-text-colour)
9. [Text Decoration](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#9-text-decoration)
10. [Text Transform](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#10-text-transform)
11. [Text Overflow & Wrapping](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#11-text-overflow--wrapping)
12. [The Tailwind Colour Palette](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#12-the-tailwind-colour-palette)
13. [Background Colour & Gradients](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#13-background-colour--gradients)
14. [Colour Opacity Modifier](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#14-colour-opacity-modifier)
15. [Placeholder & Caret Colour](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#15-placeholder--caret-colour)
16. [The @tailwindcss/typography Plugin](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#16-the-tailwindcsstypography-plugin)

---

## 1. Font Family

```html
<!-- Tailwind's three default stacks -->
<p class="font-sans">
  <!-- font-family: ui-sans-serif, system-ui, -apple-system, ... -->
  <!-- Uses the system's sans-serif font (Inter, SF Pro, Segoe UI, etc.) -->
</p>

<p class="font-serif">
  <!-- font-family: ui-serif, Georgia, Cambria, ... -->
</p>

<p class="font-mono">
  <!-- font-family: ui-monospace, SFMono-Regular, Menlo, ... -->
  <!-- Use for code, IDs, any monospaced content -->
</p>
```

```js
// Customise font families in tailwind.config.js
// First, load your font (in CSS or via <link> / next/font)
module.exports = {
  theme: {
    extend: {
      fontFamily: {
        // 'font-sans' is already defined; extend overrides it
        sans: ['Inter', 'ui-sans-serif', 'system-ui'],
        display: ['Playfair Display', 'serif'],  // new utility: font-display
        mono: ['JetBrains Mono', 'ui-monospace'],
      },
    },
  },
}
```

```tsx
// In Next.js with next/font (no layout shift, self-hosted)
import { Inter, JetBrains_Mono } from 'next/font/google';

const inter = Inter({ subsets: ['latin'], variable: '--font-inter' });
const mono  = JetBrains_Mono({ subsets: ['latin'], variable: '--font-mono' });

// Apply CSS variables to the root element
<html className={`${inter.variable} ${mono.variable}`}>

// Then in config:
fontFamily: {
  sans: ['var(--font-inter)', 'ui-sans-serif'],
  mono: ['var(--font-mono)', 'ui-monospace'],
}
```

---

## 2. Font Size — The Type Scale

Each `text-*` size sets both `font-size` and a sensible default `line-height`.

```html
<p class="text-xs">   <!-- font-size: 0.75rem;  line-height: 1rem    -->
<p class="text-sm">   <!-- font-size: 0.875rem; line-height: 1.25rem -->
<p class="text-base"> <!-- font-size: 1rem;     line-height: 1.5rem  -->
<p class="text-lg">   <!-- font-size: 1.125rem; line-height: 1.75rem -->
<p class="text-xl">   <!-- font-size: 1.25rem;  line-height: 1.75rem -->
<p class="text-2xl">  <!-- font-size: 1.5rem;   line-height: 2rem    -->
<p class="text-3xl">  <!-- font-size: 1.875rem; line-height: 2.25rem -->
<p class="text-4xl">  <!-- font-size: 2.25rem;  line-height: 2.5rem  -->
<p class="text-5xl">  <!-- font-size: 3rem;     line-height: 1        -->
<p class="text-6xl">  <!-- font-size: 3.75rem;  line-height: 1        -->
<p class="text-7xl">  <!-- font-size: 4.5rem;   line-height: 1        -->
<p class="text-8xl">  <!-- font-size: 6rem;     line-height: 1        -->
<p class="text-9xl">  <!-- font-size: 8rem;     line-height: 1        -->
```

```
Note: text-5xl and above default to line-height: 1 (no extra leading).
Large display type sits right on the baseline by default — this looks
tighter and more intentional for headings.
For large body text, override with leading-snug or leading-normal.
```

```html
<!-- Arbitrary font size -->
<p class="text-[17px]">Custom size outside the scale</p>
<p class="text-[clamp(1rem,_2.5vw,_2rem)]">Fluid typography</p>
```

---

## 3. Font Weight

```html
<p class="font-thin">       <!-- font-weight: 100 -->
<p class="font-extralight"> <!-- font-weight: 200 -->
<p class="font-light">      <!-- font-weight: 300 -->
<p class="font-normal">     <!-- font-weight: 400 -->
<p class="font-medium">     <!-- font-weight: 500 -->
<p class="font-semibold">   <!-- font-weight: 600 -->
<p class="font-bold">       <!-- font-weight: 700 -->
<p class="font-extrabold">  <!-- font-weight: 800 -->
<p class="font-black">      <!-- font-weight: 900 -->
```

```
Weights only work if the font you're using has that weight loaded.
Loading Inter with weights 400 and 700 → font-medium (500), font-semibold (600),
and font-extrabold (800) will all fall back to the nearest available weight.

With Google Fonts, specify which weights you need:
  Inter({ weight: ['400', '500', '600', '700'] })
```

---

## 4. Font Style & Variant

```html
<p class="italic">           <!-- font-style: italic -->
<p class="not-italic">       <!-- font-style: normal (resets italic) -->
<p class="normal-nums">      <!-- font-variant-numeric: normal -->
<p class="ordinal">          <!-- font-variant-numeric: ordinal (1ˢᵗ, 2ⁿᵈ) -->
<p class="slashed-zero">     <!-- font-variant-numeric: slashed-zero (0 with slash) -->
<p class="lining-nums">      <!-- font-variant-numeric: lining-nums -->
<p class="oldstyle-nums">    <!-- font-variant-numeric: oldstyle-nums -->
<p class="proportional-nums"><!-- font-variant-numeric: proportional-nums -->
<p class="tabular-nums">     <!-- font-variant-numeric: tabular-nums -->
```

---

## 5. Line Height

`leading-*` utilities control line spacing within a block of text.

```html
<!-- Relative (multiplier of font-size) -->
<p class="leading-none">     <!-- line-height: 1 -->
<p class="leading-tight">    <!-- line-height: 1.25 -->
<p class="leading-snug">     <!-- line-height: 1.375 -->
<p class="leading-normal">   <!-- line-height: 1.5 -->
<p class="leading-relaxed">  <!-- line-height: 1.625 -->
<p class="leading-loose">    <!-- line-height: 2 -->

<!-- Fixed (rem values) -->
<p class="leading-3">   <!-- line-height: 0.75rem -->
<p class="leading-4">   <!-- line-height: 1rem -->
<p class="leading-5">   <!-- line-height: 1.25rem -->
<p class="leading-6">   <!-- line-height: 1.5rem -->
<p class="leading-7">   <!-- line-height: 1.75rem -->
<p class="leading-8">   <!-- line-height: 2rem -->
<p class="leading-9">   <!-- line-height: 2.25rem -->
<p class="leading-10">  <!-- line-height: 2.5rem -->
```

```
Relative vs fixed line-height:

  leading-relaxed = line-height: 1.625
  At font-size 1rem: 1.625rem between baselines
  At font-size 2rem: 3.25rem between baselines

  leading-6 = line-height: 1.5rem (always)
  At font-size 1rem: 1.5rem between baselines
  At font-size 2rem: still 1.5rem — lines start overlapping!

Use relative (leading-normal, leading-relaxed) for body text.
Use fixed (leading-6) only when you need pixel-perfect control
independent of font size.
```

---

## 6. Letter Spacing (Tracking)

```html
<p class="tracking-tighter"> <!-- letter-spacing: -0.05em  -->
<p class="tracking-tight">   <!-- letter-spacing: -0.025em -->
<p class="tracking-normal">  <!-- letter-spacing:  0em     -->
<p class="tracking-wide">    <!-- letter-spacing:  0.025em -->
<p class="tracking-wider">   <!-- letter-spacing:  0.05em  -->
<p class="tracking-widest">  <!-- letter-spacing:  0.1em   -->
```

```
Practical usage:
  tracking-tight for large headings (tighter at display sizes looks better)
  tracking-wide or tracking-wider for small uppercase labels
  tracking-normal for body text

<p class="text-xs font-semibold uppercase tracking-wider text-gray-500">
  Category Label
</p>
```

---

## 7. Text Alignment

```html
<p class="text-left">    <!-- text-align: left -->
<p class="text-center">  <!-- text-align: center -->
<p class="text-right">   <!-- text-align: right -->
<p class="text-justify"> <!-- text-align: justify -->
<p class="text-start">   <!-- text-align: start (left in LTR, right in RTL) -->
<p class="text-end">     <!-- text-align: end   (right in LTR, left in RTL) -->
```

---

## 8. Text Colour

```html
<!-- Pattern: text-{colour}-{shade} -->
<p class="text-gray-900">Very dark gray — good for body text</p>
<p class="text-gray-600">Mid gray — good for secondary text</p>
<p class="text-gray-400">Light gray — good for placeholders/hints</p>
<p class="text-blue-600">Blue — for links and accents</p>
<p class="text-red-500">Red — for errors and warnings</p>
<p class="text-green-500">Green — for success states</p>

<!-- Special values -->
<p class="text-black">    <!-- color: rgb(0 0 0) -->
<p class="text-white">    <!-- color: rgb(255 255 255) -->
<p class="text-transparent"> <!-- color: transparent -->
<p class="text-inherit">  <!-- color: inherit (from parent) -->
<p class="text-current">  <!-- color: currentColor -->
```

---

## 9. Text Decoration

```html
<!-- Decoration type -->
<a class="underline">        <!-- text-decoration-line: underline -->
<a class="overline">         <!-- text-decoration-line: overline -->
<p class="line-through">     <!-- text-decoration-line: line-through -->
<p class="no-underline">     <!-- text-decoration-line: none -->

<!-- Decoration colour -->
<a class="underline decoration-blue-500">Blue underline</a>
<a class="underline decoration-current">Same colour as text</a>
<a class="underline decoration-transparent">Transparent (hidden) underline -->
                                            (reserve space without showing) -->

<!-- Decoration style -->
<a class="underline decoration-solid">     <!-- solid line (default) -->
<a class="underline decoration-double">    <!-- double line -->
<a class="underline decoration-dotted">    <!-- dotted line -->
<a class="underline decoration-dashed">    <!-- dashed line -->
<a class="underline decoration-wavy">      <!-- wavy line -->

<!-- Decoration thickness -->
<a class="underline decoration-1">        <!-- thickness: 1px -->
<a class="underline decoration-2">        <!-- thickness: 2px -->
<a class="underline decoration-4">        <!-- thickness: 4px -->
<a class="underline decoration-8">        <!-- thickness: 8px -->
<a class="underline decoration-auto">     <!-- thickness: auto -->
<a class="underline decoration-from-font"><!-- thickness from font metrics -->

<!-- Underline offset (distance from text baseline) -->
<a class="underline underline-offset-1">  <!-- 1px from baseline -->
<a class="underline underline-offset-2">  <!-- 2px -->
<a class="underline underline-offset-4">  <!-- 4px -->
<a class="underline underline-offset-8">  <!-- 8px -->
<a class="underline underline-offset-auto">
```

---

## 10. Text Transform

```html
<p class="uppercase">    <!-- text-transform: uppercase -->
<p class="lowercase">    <!-- text-transform: lowercase -->
<p class="capitalize">   <!-- text-transform: capitalize (first letter of each word) -->
<p class="normal-case">  <!-- text-transform: none (resets) -->
```

---

## 11. Text Overflow & Wrapping

(See Part 01 Section 10 for full overflow utilities. Summary here for typography context.)

```html
<!-- Single-line truncation with ellipsis -->
<p class="truncate w-48">Long text that exceeds width...</p>
<!-- overflow: hidden; text-overflow: ellipsis; white-space: nowrap -->

<!-- Multi-line clamp -->
<p class="line-clamp-2">Visible line one. Visible line two. Hidden line three.</p>
<p class="line-clamp-3">Three lines shown.</p>
<p class="line-clamp-none">Remove line clamping</p>

<!-- Whitespace control -->
<p class="whitespace-normal">   <!-- wrap normally (default) -->
<p class="whitespace-nowrap">   <!-- no wrapping (single line) -->
<p class="whitespace-pre">      <!-- preserve newlines and spaces -->
<p class="whitespace-pre-line"> <!-- preserve newlines, collapse spaces -->
<p class="whitespace-pre-wrap"> <!-- preserve both, allow wrapping -->
<p class="whitespace-break-spaces"> <!-- treat non-breaking spaces as wrappable -->

<!-- Word break -->
<p class="break-normal">  <!-- default word/line break rules -->
<p class="break-words">   <!-- break long words if they overflow -->
<p class="break-all">     <!-- break at any character -->
<p class="break-keep">    <!-- don't break CJK words (Korean/Chinese/Japanese) -->

<!-- Hyphens -->
<p class="hyphens-none">   <!-- hyphens: none -->
<p class="hyphens-manual"> <!-- hyphens: manual (only at &shy;) -->
<p class="hyphens-auto">   <!-- hyphens: auto (browser inserts hyphens) -->
```

---

## 12. The Tailwind Colour Palette

Tailwind ships a carefully designed colour palette. Each colour has 11 shades from 50 (lightest) to 950 (darkest).

```
Shades:
  50   → very light tint (backgrounds, hover states)
  100  → light tint
  200  → light
  300  → medium-light
  400  → medium
  500  → base colour (brand colour reference)
  600  → medium-dark (darker on hover: hover:bg-blue-600 on bg-blue-500)
  700  → dark
  800  → very dark
  900  → near-black tint
  950  → darkest (added in Tailwind v3.3)

Available colour families:
  Greys:    slate, gray, zinc, neutral, stone
  Warm:     red, orange, amber, yellow
  Green:    lime, green, emerald, teal
  Blue:     cyan, sky, blue, indigo, violet
  Purple:   purple, fuchsia, pink, rose

Special:   black, white, transparent, inherit, current
```

```html
<!-- Grey families — subtle differences in undertone -->
<div class="bg-slate-100">   <!-- slate: cool blue-grey -->
<div class="bg-gray-100">    <!-- gray: neutral grey -->
<div class="bg-zinc-100">    <!-- zinc: cooler neutral -->
<div class="bg-neutral-100"> <!-- neutral: pure grey -->
<div class="bg-stone-100">   <!-- stone: warm grey -->

<!-- Most UIs use one grey family throughout for consistency -->
```

```js
// Disable unused colours to reduce config complexity
// (doesn't affect CSS size — JIT only generates what's used)
module.exports = {
  theme: {
    colors: {
      // Only include what your design actually uses
      transparent: 'transparent',
      current: 'currentColor',
      white: '#ffffff',
      black: '#000000',
      gray: require('tailwindcss/colors').gray,
      blue: require('tailwindcss/colors').blue,
      red: require('tailwindcss/colors').red,
      green: require('tailwindcss/colors').green,
    },
  },
}
```

---

## 13. Background Colour & Gradients

```html
<!-- Solid backgrounds -->
<div class="bg-white">
<div class="bg-gray-50">    <!-- very light gray — common for page background -->
<div class="bg-gray-100">   <!-- light gray — sidebar, card hover -->
<div class="bg-blue-600">   <!-- primary action backgrounds -->
<div class="bg-transparent"><!-- transparent -->

<!-- Background size -->
<div class="bg-auto">    <!-- background-size: auto -->
<div class="bg-cover">   <!-- background-size: cover (fills, may crop) -->
<div class="bg-contain"> <!-- background-size: contain (fits, may have gaps) -->

<!-- Background position -->
<div class="bg-center">  <!-- background-position: center -->
<div class="bg-top">
<div class="bg-bottom">
<div class="bg-left">
<div class="bg-right">
<div class="bg-left-top">
<div class="bg-right-bottom">

<!-- Background repeat -->
<div class="bg-repeat">        <!-- default -->
<div class="bg-no-repeat">
<div class="bg-repeat-x">
<div class="bg-repeat-y">

<!-- Background origin & clip -->
<div class="bg-origin-border">   <!-- background starts at border edge -->
<div class="bg-origin-padding">  <!-- background starts at padding edge -->
<div class="bg-origin-content">  <!-- background starts at content edge -->
```

### Gradients

```html
<!-- Step 1: set direction with bg-gradient-to-* -->
<div class="bg-gradient-to-r">   <!-- left → right -->
<div class="bg-gradient-to-l">   <!-- right → left -->
<div class="bg-gradient-to-t">   <!-- bottom → top -->
<div class="bg-gradient-to-b">   <!-- top → bottom -->
<div class="bg-gradient-to-br">  <!-- top-left → bottom-right -->
<div class="bg-gradient-to-tr">  <!-- bottom-left → top-right -->

<!-- Step 2: set colours with from-*, via-*, to-* -->
<div class="bg-gradient-to-r from-blue-500 to-purple-600">
  <!-- Blue on left, purple on right -->
</div>

<div class="bg-gradient-to-r from-cyan-400 via-blue-500 to-indigo-600">
  <!-- Three-stop gradient -->
</div>

<!-- Transparent gradient (for fading into background) -->
<div class="bg-gradient-to-b from-transparent to-black/60">
  <!-- Fades from nothing to semi-transparent black — overlay effect -->
</div>

<!-- Gradient position control (v3.2+) -->
<div class="bg-gradient-to-r from-blue-600 from-10% via-sky-500 via-30% to-emerald-400 to-90%">
  <!-- Control exactly where each stop falls -->
</div>
```

---

## 14. Colour Opacity Modifier

The `/` modifier after any colour class controls the alpha channel. This is different from `opacity-50`, which affects the entire element including its children.

```html
<!-- Background with opacity -->
<div class="bg-blue-500/50">   <!-- background-color: rgb(59 130 246 / 0.5) -->
<div class="bg-black/20">      <!-- background-color: rgb(0 0 0 / 0.2) — overlay -->
<div class="bg-white/10">      <!-- frosted glass effect starting point -->

<!-- Text with opacity -->
<p class="text-gray-900/75">   <!-- text at 75% opacity -->

<!-- Border with opacity -->
<div class="border border-gray-900/10"> <!-- very subtle border -->

<!-- Shadow colour with opacity -->
<div class="shadow-lg shadow-blue-500/50"> <!-- coloured shadow -->

<!-- Ring with opacity -->
<input class="ring-2 ring-blue-500/50">
```

```
Difference between /opacity and opacity-* utility:

  opacity-50:
    Makes the ENTIRE element (including children and background) 50% opaque.
    Creates a stacking context.
    Children inherit the opacity — you can't fix this.

  bg-blue-500/50:
    ONLY the background is 50% opaque.
    Text, borders, and children are unaffected.
    No stacking context.

Use /modifier for background overlays, subtle borders, and shadows.
Use opacity-* for fade-in/out animations where the whole element should fade.
```

---

## 15. Placeholder & Caret Colour

```html
<!-- Style the placeholder text in inputs -->
<input
  type="text"
  placeholder="Search..."
  class="placeholder:text-gray-400 placeholder:text-sm"
>
<!-- placeholder: is a variant prefix, not a property -->
<!-- You can use any text utility: placeholder:italic, placeholder:font-medium, etc. -->

<!-- Style the text cursor (caret) -->
<input class="caret-blue-500">     <!-- caret-color: rgb(59 130 246) -->
<input class="caret-transparent">  <!-- invisible caret -->

<!-- Selection colour -->
<p class="selection:bg-yellow-300 selection:text-black">
  Selecting this text shows yellow highlight with black text
</p>

<!-- Apply globally to all text -->
<html class="selection:bg-blue-100">
```

---

## 16. The @tailwindcss/typography Plugin

For rendering rich text content (markdown, CMS output, blog posts) that you don't fully control.

```bash
npm install @tailwindcss/typography
```

```js
// tailwind.config.js
plugins: [require('@tailwindcss/typography')],
```

```html
<!-- Apply to any container with uncontrolled HTML content -->
<article class="prose prose-lg max-w-none">
  <!-- All heading, paragraph, list, link, code, blockquote, table styles
       are applied automatically based on semantic HTML tags. -->
  <h1>Heading gets size, weight, margin automatically</h1>
  <p>Body text gets proper line-height, colour, margin.</p>
  <ul><li>Lists get bullets and spacing.</li></ul>
  <code>Inline code gets styled.</code>
  <pre><code>Code blocks get background, padding, font.</code></pre>
</article>
```

```html
<!-- Prose sizes -->
<div class="prose-sm">   <!-- for smaller text -->
<div class="prose">      <!-- default (~16px body) -->
<div class="prose-lg">   <!-- larger text -->
<div class="prose-xl">   <!-- for editorial long-form -->
<div class="prose-2xl">  <!-- display-scale prose -->

<!-- Colour scheme variants -->
<div class="prose prose-gray">    <!-- gray (default) -->
<div class="prose prose-slate">   <!-- slate greys -->
<div class="prose prose-zinc">    <!-- zinc greys -->
<div class="prose prose-invert">  <!-- dark mode — white text -->

<!-- Customise individual elements with prose modifiers -->
<div class="prose prose-headings:font-bold prose-a:text-blue-600 prose-a:no-underline hover:prose-a:underline">
```