> **Series overview:** This is file 2 of 7. It covers every layout utility in Tailwind: the box model, display types, positioning, overflow, z-index, aspect ratio, and the container class. These are the structural bones of any UI.

---

## Table of Contents

1. [The Box Model in Tailwind]
2. [Display Utilities]
3. [Width & Height]
4. [Min/Max Width & Height]
5. [Padding]
6. [Margin]
7. [Positioning]
8. [Inset — Top, Right, Bottom, Left]
9. [Z-Index]
10. [Overflow]
11. [Visibility & Opacity]
12. [Aspect Ratio]
13. [Container]
14. [Space Between — Distributing Margin to Children]

---

## 1. The Box Model in Tailwind

Every HTML element is a box. The box has four layers: content, padding, border, margin.

```
  ┌─── margin ──────────────────────────────────────┐
  │  ┌─── border ────────────────────────────────┐  │
  │  │  ┌─── padding ──────────────────────────┐ │  │
  │  │  │  ┌─── content ───────────────────┐   │ │  │
  │  │  │  │  width × height               │   │ │  │
  │  │  │  └───────────────────────────────┘   │ │  │
  │  │  └──────────────────────────────────────┘ │  │
  │  └────────────────────────────────────────────┘  │
  └─────────────────────────────────────────────────┘
```

### box-sizing: border-box

Tailwind's Preflight sets `box-sizing: border-box` globally. This is crucial.

```
Default browser behaviour (content-box):
  width applies to the content area only.
  A div with width: 200px and padding: 20px is actually 240px wide.

border-box (Tailwind's default):
  width applies to the ENTIRE box (border + padding + content).
  A div with w-48 (192px) and p-5 (20px padding) stays 192px wide.
  The content area shrinks to absorb the padding.
```

```html
<!-- Without border-box: this would be wider than its parent -->
<!-- With border-box (Tailwind default): exactly 100% wide -->
<div class="w-full p-8 border-4">
  <!-- w-full = 100% of parent -->
  <!-- p-8 and border-4 do NOT add to the width -->
</div>
```

---

## 2. Display Utilities

```
block         → display: block
inline-block  → display: inline-block
inline        → display: inline
flex          → display: flex
inline-flex   → display: inline-flex
grid          → display: grid
inline-grid   → display: inline-grid
table         → display: table
hidden        → display: none
contents      → display: contents
```

```html
<!-- Block: full width, starts on new line -->
<span class="block">Now behaves like a div</span>

<!-- Inline: flows with text, ignores width/height -->
<div class="inline">Now flows like a span</div>

<!-- Inline-block: flows with text but respects width/height -->
<div class="inline-block w-24 h-24 bg-blue-500"></div>

<!-- Hidden: removed from layout (not just invisible) -->
<div class="hidden">This takes up no space</div>

<!-- Contents: element acts as if it doesn't exist in layout
     children behave as direct children of the parent -->
<div class="contents">
  <!-- These divs behave as siblings of the outer div's siblings -->
  <div class="flex-1">Column A</div>
  <div class="flex-1">Column B</div>
</div>
```

---

## 3. Width & Height

### Fixed Widths

```html
<!-- Spacing scale values (same as padding/margin) -->
<div class="w-4">
  <!-- width: 1rem (16px) -->
  <div class="w-8">
    <!-- width: 2rem (32px) -->
    <div class="w-16">
      <!-- width: 4rem (64px) -->
      <div class="w-32">
        <!-- width: 8rem (128px) -->
        <div class="w-64"><!-- width: 16rem (256px) --></div>
      </div>
    </div>
  </div>
</div>
```

### Fractional Widths

```html
<!-- Fractions of the parent's width -->
<div class="w-1/2">
  <!-- width: 50% -->
  <div class="w-1/3">
    <!-- width: 33.333% -->
    <div class="w-2/3">
      <!-- width: 66.666% -->
      <div class="w-1/4">
        <!-- width: 25% -->
        <div class="w-3/4">
          <!-- width: 75% -->
          <div class="w-1/5">
            <!-- width: 20% -->
            <div class="w-2/5">
              <!-- width: 40% -->
              <div class="w-1/6"><!-- width: 16.666% --></div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### Keyword Widths

```html
<div class="w-full">
  <!-- width: 100% -->
  <div class="w-screen">
    <!-- width: 100vw -->
    <div class="w-svw">
      <!-- width: 100svw (small viewport) -->
      <div class="w-dvw">
        <!-- width: 100dvw (dynamic viewport) -->
        <div class="w-auto">
          <!-- width: auto -->
          <div class="w-fit">
            <!-- width: fit-content -->
            <div class="w-max">
              <!-- width: max-content -->
              <div class="w-min"><!-- width: min-content --></div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### Heights

```html
<!-- Same scale as widths -->
<div class="h-4">
  <!-- height: 1rem -->
  <div class="h-screen">
    <!-- height: 100vh -->
    <div class="h-svh">
      <!-- height: 100svh (small viewport height) -->
      <div class="h-dvh">
        <!-- height: 100dvh (dynamic viewport height) -->
        <div class="h-full">
          <!-- height: 100% (parent must have height) -->
          <div class="h-auto">
            <!-- height: auto -->
            <div class="h-fit"><!-- height: fit-content --></div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```
Why svh and dvh?

On mobile, the browser chrome (address bar) changes height as you scroll.
100vh includes the area behind the chrome on some browsers, causing overflow.
100svh = height of the smallest possible viewport (chrome fully visible).
100dvh = height of the current viewport (dynamically updates).
Use h-dvh for mobile full-screen layouts.
```

---

## 4. Min/Max Width & Height

```html
<!-- Minimum width -->
<div class="min-w-0">
  <!-- min-width: 0 (overrides flex shrink limit) -->
  <div class="min-w-full">
    <!-- min-width: 100% -->
    <div class="min-w-min">
      <!-- min-width: min-content -->
      <div class="min-w-max">
        <!-- min-width: max-content -->

        <!-- Maximum width — crucial for readable content columns -->
        <div class="max-w-xs">
          <!-- max-width: 20rem  (320px) -->
          <div class="max-w-sm">
            <!-- max-width: 24rem  (384px) -->
            <div class="max-w-md">
              <!-- max-width: 28rem  (448px) -->
              <div class="max-w-lg">
                <!-- max-width: 32rem  (512px) -->
                <div class="max-w-xl">
                  <!-- max-width: 36rem  (576px) -->
                  <div class="max-w-2xl">
                    <!-- max-width: 42rem  (672px) -->
                    <div class="max-w-3xl">
                      <!-- max-width: 48rem  (768px) -->
                      <div class="max-w-4xl">
                        <!-- max-width: 56rem  (896px) -->
                        <div class="max-w-5xl">
                          <!-- max-width: 64rem (1024px) -->
                          <div class="max-w-6xl">
                            <!-- max-width: 72rem (1152px) -->
                            <div class="max-w-7xl">
                              <!-- max-width: 80rem (1280px) -->
                              <div class="max-w-full">
                                <!-- max-width: 100% -->
                                <div class="max-w-none">
                                  <!-- max-width: none (removes any max-width) -->
                                  <div class="max-w-prose">
                                    <!-- max-width: 65ch (optimal reading width) -->
                                    <div class="max-w-screen-sm">
                                      <!-- max-width: 640px (sm breakpoint) -->
                                      <div class="max-w-screen-md">
                                        <!-- max-width: 768px -->
                                        <div class="max-w-screen-lg">
                                          <!-- max-width: 1024px -->
                                          <div class="max-w-screen-xl">
                                            <!-- max-width: 1280px -->
                                            <div class="max-w-screen-2xl">
                                              <!-- max-width: 1536px -->

                                              <!-- Minimum height -->
                                              <div class="min-h-0">
                                                <!-- min-height: 0 -->
                                                <div class="min-h-full">
                                                  <!-- min-height: 100% -->
                                                  <div class="min-h-screen">
                                                    <!-- min-height: 100vh -->
                                                    <div class="min-h-svh">
                                                      <!-- min-height: 100svh -->
                                                      <div class="min-h-dvh">
                                                        <!-- min-height: 100dvh -->
                                                      </div>
                                                    </div>
                                                  </div>
                                                </div>
                                              </div>
                                            </div>
                                          </div>
                                        </div>
                                      </div>
                                    </div>
                                  </div>
                                </div>
                              </div>
                            </div>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### Common Pattern — Centered Content Column

```html
<main class="max-w-4xl mx-auto px-4">
  <!-- max-w-4xl: won't stretch past 56rem on wide screens -->
  <!-- mx-auto: centered horizontally -->
  <!-- px-4: breathing room on mobile -->
</main>
```

---

## 5. Padding

```html
<!-- All sides -->
<div class="p-0">
  <!-- padding: 0 -->
  <div class="p-1">
    <!-- padding: 0.25rem -->
    <div class="p-2">
      <!-- padding: 0.5rem -->
      <div class="p-4">
        <!-- padding: 1rem -->
        <div class="p-8">
          <!-- padding: 2rem -->

          <!-- Horizontal (left + right) -->
          <div class="px-4">
            <!-- padding-left: 1rem; padding-right: 1rem -->

            <!-- Vertical (top + bottom) -->
            <div class="py-4">
              <!-- padding-top: 1rem; padding-bottom: 1rem -->

              <!-- Individual sides -->
              <div class="pt-4">
                <!-- padding-top: 1rem -->
                <div class="pr-4">
                  <!-- padding-right: 1rem -->
                  <div class="pb-4">
                    <!-- padding-bottom: 1rem -->
                    <div class="pl-4">
                      <!-- padding-left: 1rem -->

                      <!-- Logical properties (for RTL support) -->
                      <div class="ps-4">
                        <!-- padding-inline-start: 1rem (left in LTR, right in RTL) -->
                        <div class="pe-4">
                          <!-- padding-inline-end: 1rem -->
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### The Order of Specificity

```html
<!-- If you specify both a general and specific direction, order matters.
     Tailwind generates utilities in consistent order in the stylesheet,
     but to be safe, put the more specific one after the general one. -->

<!-- p-4 sets all sides to 1rem, then pt-8 overrides top to 2rem -->
<div class="p-4 pt-8"></div>
```

---

## 6. Margin

```html
<!-- Same pattern as padding, prefixed with m -->
<div class="m-4">
  <!-- margin: 1rem all sides -->
  <div class="mx-4">
    <!-- margin-left + margin-right: 1rem -->
    <div class="my-4">
      <!-- margin-top + margin-bottom: 1rem -->
      <div class="mt-4">
        <!-- margin-top: 1rem -->
        <div class="mr-4">
          <!-- margin-right: 1rem -->
          <div class="mb-4">
            <!-- margin-bottom: 1rem -->
            <div class="ml-4">
              <!-- margin-left: 1rem -->

              <!-- Auto margin — used for centering -->
              <div class="mx-auto">
                <!-- horizontally center a block element -->
                <div class="ml-auto">
                  <!-- push to right edge (in flex row) -->
                  <div class="mt-auto">
                    <!-- push to bottom (in flex column) -->

                    <!-- Negative margins -->
                    <div class="-mt-4">
                      <!-- margin-top: -1rem -->
                      <div class="-mx-4">
                        <!-- margin-left: -1rem; margin-right: -1rem -->
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```
When to use negative margins:

Pull elements outside their container:
  <div class="px-4">               <!-- container with padding -->
    <img class="-mx-4">           <!-- image bleeds to edges, ignoring px-4 -->
  </div>

Overlap elements:
  <div class="relative">
    <img class="w-full">
    <div class="-mt-16">          <!-- overlaps the image by 4rem -->
    </div>
  </div>
```

---

## 7. Positioning

```html
<!-- Position types -->
<div class="static">
  <!-- position: static (default) -->
  <div class="relative">
    <!-- position: relative (establishes stacking context) -->
    <div class="absolute">
      <!-- position: absolute (relative to nearest positioned ancestor) -->
      <div class="fixed">
        <!-- position: fixed (relative to viewport) -->
        <div class="sticky">
          <!-- position: sticky (relative until scroll threshold) -->
        </div>
      </div>
    </div>
  </div>
</div>
```

### How Absolute Positioning Works

```html
<!-- Parent must be positioned (relative, absolute, fixed, or sticky) -->
<!-- for absolute child to be relative to it -->

<div class="relative w-48 h-48 bg-gray-100">
  <!-- Without class="relative" on parent, the child would be
       positioned relative to the nearest positioned ancestor, 
       which might be the <html> element. -->

  <div class="absolute top-0 right-0 bg-red-500 text-white text-xs px-1">
    Badge
    <!-- Positioned at top-right corner of the parent div -->
  </div>
</div>
```

### Fixed Positioning

```html
<!-- Stays in place as user scrolls -->
<!-- Removed from document flow -->

<!-- Sticky header -->
<header
  class="fixed top-0 left-0 right-0 z-50 bg-white shadow-sm h-16"
></header>

<!-- Must add padding-top to content to account for fixed header -->
<main class="pt-16"></main>

<!-- Full-screen overlay -->
<div class="fixed inset-0 bg-black/50 z-40">
  <!-- inset-0 = top:0; right:0; bottom:0; left:0 -->
  <!-- bg-black/50 = black at 50% opacity -->
</div>
```

### Sticky Positioning

```html
<!-- Stays in normal flow until it reaches the threshold, then sticks -->
<thead>
  <tr>
    <th class="sticky top-0 bg-white z-10">Column Header</th>
    <!-- Scrolls normally until it reaches the top of the viewport,
         then stays there. Must be inside a scrollable container. -->
  </tr>
</thead>
```

---

## 8. Inset — Top, Right, Bottom, Left

```html
<!-- Inset — sets top, right, bottom, and left simultaneously -->
<div class="absolute inset-0">
  <!-- all sides: 0 (fill parent completely) -->
  <div class="absolute inset-4">
    <!-- all sides: 1rem -->
    <div class="absolute inset-x-0">
      <!-- left: 0; right: 0 (full width) -->
      <div class="absolute inset-y-0">
        <!-- top: 0; bottom: 0 (full height) -->

        <!-- Individual sides -->
        <div class="absolute top-0">
          <!-- top: 0 -->
          <div class="absolute right-0">
            <!-- right: 0 -->
            <div class="absolute bottom-0">
              <!-- bottom: 0 -->
              <div class="absolute left-0">
                <!-- left: 0 -->
                <div class="absolute top-4">
                  <!-- top: 1rem -->
                  <div class="absolute -top-4">
                    <!-- top: -1rem (overlaps parent edge) -->

                    <!-- Percentage values -->
                    <div class="absolute top-1/2 left-1/2">
                      <!-- top: 50%; left: 50% -->

                      <!-- Centering an absolute element — a classic pattern -->
                      <div
                        class="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2"
                      >
                        <!-- Position the top-left corner at the center of the parent,
       then translate back by 50% of its own size to truly center it. -->
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

---

## 9. Z-Index

```html
<!-- Controls stacking order — higher value = on top -->
<div class="z-0">
  <!-- z-index: 0 -->
  <div class="z-10">
    <!-- z-index: 10 -->
    <div class="z-20">
      <!-- z-index: 20 -->
      <div class="z-30">
        <!-- z-index: 30 -->
        <div class="z-40">
          <!-- z-index: 40 -->
          <div class="z-50">
            <!-- z-index: 50 -->
            <div class="z-auto">
              <!-- z-index: auto -->

              <!-- Negative z-index -->
              <div class="-z-10">
                <!-- z-index: -10 (goes behind the parent's background) -->
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```
Z-index only works on positioned elements (relative, absolute, fixed, sticky).
Adding z-50 to a static element does nothing.

Recommended stacking convention:
  z-0  to z-10 : page content layers
  z-20 to z-30 : sticky headers, sidebars
  z-40         : mobile menu overlays
  z-50         : modals, dialogs

  If you need values beyond z-50, add them in tailwind.config.js:
    theme: { extend: { zIndex: { 60: '60', 100: '100' } } }
```

---

## 10. Overflow

```html
<!-- overflow -->
<div class="overflow-auto">
  <!-- scroll only when needed (both axes) -->
  <div class="overflow-hidden">
    <!-- clip — content outside is invisible -->
    <div class="overflow-visible">
      <!-- default — content can overflow -->
      <div class="overflow-scroll">
        <!-- always show scrollbars -->
        <div class="overflow-clip">
          <!-- clip, no programmatic scrolling -->

          <!-- Individual axes -->
          <div class="overflow-x-auto">
            <!-- horizontal scroll when needed -->
            <div class="overflow-y-auto">
              <!-- vertical scroll when needed -->
              <div class="overflow-x-hidden">
                <!-- hide horizontal overflow -->
                <div class="overflow-y-hidden">
                  <!-- hide vertical overflow -->
                  <div class="overflow-x-scroll">
                    <!-- always show horizontal scrollbar -->
                    <div class="overflow-y-scroll">
                      <!-- always show vertical scrollbar -->
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```
overflow-hidden creates a new block formatting context.
This collapses inside float elements and prevents margin collapse.
It also clips rounded corners — rounded-xl only clips children
if you also add overflow-hidden.

<div class="rounded-xl overflow-hidden bg-white shadow-md">
  <img class="w-full">   <!-- image corners are clipped to rounded-xl -->
  <div class="p-4">...</div>
</div>
```

### Text Overflow

```html
<!-- Truncate long text with ellipsis -->
<p class="truncate">
  <!-- overflow: hidden; text-overflow: ellipsis; white-space: nowrap -->
  This is a very long line that will be truncated...
</p>

<!-- Line clamping — show N lines then ellipsis -->
<p class="line-clamp-2">
  <!-- -webkit-line-clamp: 2 -->
  First line of text. Second line. This third line is hidden.
</p>

<p class="line-clamp-3">
  <!-- show up to 3 lines -->

  <!-- Wrapping control -->
</p>

<p class="whitespace-nowrap"><!-- white-space: nowrap (no line breaks) --></p>
<p class="whitespace-pre"><!-- white-space: pre (preserve whitespace) --></p>
<p class="whitespace-pre-wrap">
  <!-- preserve whitespace, wrap at line end -->
</p>
<p class="break-words"><!-- break long words to fit container --></p>
<p class="break-all"><!-- break at any character --></p>
<p class="break-keep"><!-- don't break CJK text --></p>
```

---

## 11. Visibility & Opacity

```html
<!-- Visibility — hidden takes up space, display:none doesn't -->
<div class="visible">
  <!-- visibility: visible -->
  <div class="invisible">
    <!-- visibility: hidden (still occupies space) -->

    <!-- Compare with hidden which is display:none -->
    <div class="hidden">
      <!-- display: none — no space taken -->

      <!-- When to use invisible vs hidden:
     Use invisible when you want to reserve the space (e.g., loading state).
     Use hidden when you want the element gone from layout entirely. -->

      <!-- Opacity -->
      <div class="opacity-0">
        <!-- opacity: 0 (invisible but still occupies space) -->
        <div class="opacity-5">
          <!-- opacity: 0.05 -->
          <div class="opacity-10">
            <!-- opacity: 0.1 -->
            <div class="opacity-25">
              <!-- opacity: 0.25 -->
              <div class="opacity-50">
                <!-- opacity: 0.5 -->
                <div class="opacity-75">
                  <!-- opacity: 0.75 -->
                  <div class="opacity-90">
                    <!-- opacity: 0.9 -->
                    <div class="opacity-100">
                      <!-- opacity: 1 -->

                      <!-- opacity applies to the element AND all its children -->
                      <!-- For colour transparency only, use the / modifier on the colour itself -->
                      <div class="bg-black/50">
                        <!-- black at 50% opacity (background only) -->
                        <div class="text-blue-500/75">
                          <!-- text at 75% opacity -->
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

---

## 12. Aspect Ratio

```html
<!-- Maintain a specific width:height ratio -->
<div class="aspect-square">
  <!-- aspect-ratio: 1 / 1 (square) -->
  <div class="aspect-video">
    <!-- aspect-ratio: 16 / 9 -->
    <div class="aspect-auto">
      <!-- aspect-ratio: auto -->
      <div class="aspect-[4/3]"><!-- aspect-ratio: 4 / 3 (arbitrary) --></div>
    </div>
  </div>
</div>
```

```html
<!-- Responsive image/video container that maintains ratio -->
<div class="aspect-video w-full">
  <iframe class="w-full h-full" src="https://youtube.com/embed/..."></iframe>
  <!-- Without aspect-video: iframe has no height, collapses to 0.
       With aspect-video: container is always 16:9 regardless of width. -->
</div>

<!-- Square avatar -->
<div class="aspect-square w-12 rounded-full overflow-hidden">
  <img class="w-full h-full object-cover" src="..." />
</div>
```

---

## 13. Container

```html
<!-- Centers content and sets max-width at each breakpoint -->
<div class="container mx-auto px-4">
  <!-- container alone sets max-width but does NOT center itself.
       mx-auto is required for centering.
       px-4 adds horizontal padding for mobile. -->
</div>
```

```
Default container max-widths match the breakpoints:
  sm  (640px):  max-width: 640px
  md  (768px):  max-width: 768px
  lg  (1024px): max-width: 1024px
  xl  (1280px): max-width: 1280px
  2xl (1536px): max-width: 1536px

You can customise this in config:
  theme: {
    container: {
      center: true,       // always mx-auto
      padding: '1rem',    // always add padding
      screens: {
        xl: '1200px',     // custom max at xl
      },
    },
  }
```

---

## 14. Space Between — Distributing Margin to Children

Instead of adding margin to every child individually, `space-x-*` and `space-y-*` add margin between siblings using a CSS sibling selector.

```html
<!-- space-x-* adds margin-left to every child except the first -->
<div class="flex space-x-4">
  <div>A</div>
  <!-- no margin-left -->
  <div>B</div>
  <!-- margin-left: 1rem -->
  <div>C</div>
  <!-- margin-left: 1rem -->
</div>

<!-- space-y-* adds margin-top to every child except the first -->
<div class="flex flex-col space-y-4">
  <div>First</div>
  <!-- no margin-top -->
  <div>Second</div>
  <!-- margin-top: 1rem -->
  <div>Third</div>
  <!-- margin-top: 1rem -->
</div>

<!-- Reverse direction (for flex-row-reverse or flex-col-reverse) -->
<div class="flex flex-row-reverse space-x-4 space-x-reverse">
  <!-- space-x-reverse flips the margin from left to right -->
</div>
```

```
space-x-* generates:
  .space-x-4 > * + * { margin-left: 1rem; }

The > * + * selector means: every child that is preceded by a sibling.
This avoids adding margin to the first child.

Limitation: doesn't work when children wrap across lines.
For wrapping grids, use gap-* on the flex/grid container instead.
```
