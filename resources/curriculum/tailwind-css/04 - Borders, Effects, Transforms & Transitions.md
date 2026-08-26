> **Series overview:** This is file 5 of 7. It covers borders, border-radius, box shadows, ring utilities, transforms (translate, scale, rotate, skew), transitions, and animations — the decorative and motion layer of any interface.

---

## Table of Contents

1. [Border Width](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#1-border-width)
2. [Border Colour](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#2-border-colour)
3. [Border Style](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#3-border-style)
4. [Border Radius](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#4-border-radius)
5. [Outline](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#5-outline)
6. [Ring Utilities — Focus Outlines](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#6-ring-utilities--focus-outlines)
7. [Divide — Borders Between Children](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#7-divide--borders-between-children)
8. [Box Shadow](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#8-box-shadow)
9. [Text Shadow & Drop Shadow Filter](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#9-text-shadow--drop-shadow-filter)
10. [Transforms — Translate, Scale, Rotate, Skew](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#10-transforms--translate-scale-rotate-skew)
11. [Transform Origin](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#11-transform-origin)
12. [Transition](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#12-transition)
13. [Animation](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#13-animation)
14. [Backdrop Filter — Frosted Glass](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#14-backdrop-filter--frosted-glass)
15. [Filter — Blur, Brightness, Contrast](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#15-filter--blur-brightness-contrast)

---

## 1. Border Width

```html
<!-- All sides -->
<div class="border">
  <!-- border-width: 1px (shorthand — all sides) -->
  <div class="border-0">
    <!-- border-width: 0 -->
    <div class="border-2">
      <!-- border-width: 2px -->
      <div class="border-4">
        <!-- border-width: 4px -->
        <div class="border-8">
          <!-- border-width: 8px -->

          <!-- Individual sides -->
          <div class="border-t">
            <!-- border-top-width: 1px -->
            <div class="border-r">
              <!-- border-right-width: 1px -->
              <div class="border-b">
                <!-- border-bottom-width: 1px -->
                <div class="border-l">
                  <!-- border-left-width: 1px -->
                  <div class="border-t-2">
                    <!-- border-top-width: 2px -->
                    <div class="border-b-4">
                      <!-- border-bottom-width: 4px -->
                      <div class="border-l-8">
                        <!-- border-left-width: 8px — accent bar -->

                        <!-- Horizontal and vertical -->
                        <div class="border-x">
                          <!-- border-left + border-right: 1px -->
                          <div class="border-y">
                            <!-- border-top + border-bottom: 1px -->
                            <div class="border-x-2">
                              <!-- 2px on left and right -->
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

```
Note: border-width alone is not visible.
You must also set a border-colour (or use border-current / border-inherit).
border shorthand defaults to the browser's default colour — usually black.
Tailwind does not apply a default border-colour automatically.

Typical pattern:
  <div class="border border-gray-200">     — 1px light gray border
  <div class="border-2 border-blue-500">  — 2px blue border
  <div class="border-b border-gray-100">  — bottom separator line
```

---

## 2. Border Colour

```html
<!-- Pattern: border-{colour}-{shade} -->
<div class="border border-gray-200">
  <!-- subtle divider -->
  <div class="border border-gray-300">
    <!-- slightly more visible -->
    <div class="border border-blue-500">
      <!-- highlighted/active border -->
      <div class="border border-red-500">
        <!-- error state -->
        <div class="border border-transparent">
          <!-- invisible (but takes space) -->
          <div class="border border-current">
            <!-- inherits text colour -->
            <div class="border border-inherit">
              <!-- inherits from parent -->

              <!-- With opacity -->
              <div class="border border-gray-900/10">
                <!-- very subtle border -->
                <div class="border border-black/20">
                  <!-- 20% opaque black border -->

                  <!-- Individual side colours -->
                  <div
                    class="border-t border-t-blue-500 border-l border-l-transparent"
                  ></div>
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

## 3. Border Style

```html
<div class="border-solid">
  <!-- border-style: solid (default) -->
  <div class="border-dashed">
    <!-- border-style: dashed -->
    <div class="border-dotted">
      <!-- border-style: dotted -->
      <div class="border-double">
        <!-- border-style: double (need border-4 to see) -->
        <div class="border-hidden">
          <!-- border-style: hidden -->
          <div class="border-none">
            <!-- border-style: none (removes border) -->
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

---

## 4. Border Radius

```html
<!-- All corners -->
<div class="rounded-none">
  <!-- border-radius: 0 -->
  <div class="rounded-sm">
    <!-- border-radius: 0.125rem (2px) -->
    <div class="rounded">
      <!-- border-radius: 0.25rem  (4px) -->
      <div class="rounded-md">
        <!-- border-radius: 0.375rem (6px) -->
        <div class="rounded-lg">
          <!-- border-radius: 0.5rem   (8px) -->
          <div class="rounded-xl">
            <!-- border-radius: 0.75rem  (12px) -->
            <div class="rounded-2xl">
              <!-- border-radius: 1rem     (16px) -->
              <div class="rounded-3xl">
                <!-- border-radius: 1.5rem   (24px) -->
                <div class="rounded-full">
                  <!-- border-radius: 9999px   (pill / circle) -->

                  <!-- Individual corners -->
                  <div class="rounded-t-lg">
                    <!-- top-left + top-right -->
                    <div class="rounded-b-lg">
                      <!-- bottom-left + bottom-right -->
                      <div class="rounded-l-lg">
                        <!-- top-left + bottom-left -->
                        <div class="rounded-r-lg">
                          <!-- top-right + bottom-right -->
                          <div class="rounded-tl-lg">
                            <!-- top-left only -->
                            <div class="rounded-tr-lg">
                              <!-- top-right only -->
                              <div class="rounded-bl-lg">
                                <!-- bottom-left only -->
                                <div class="rounded-br-lg">
                                  <!-- bottom-right only -->
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

```html
<!-- Common patterns -->

<!-- Card -->
<div class="bg-white rounded-xl shadow-md overflow-hidden">
  <!-- overflow-hidden is needed to clip children to the rounded corners -->
</div>

<!-- Pill badge -->
<span
  class="bg-blue-100 text-blue-800 text-xs font-medium px-2.5 py-0.5 rounded-full"
>
  New
</span>

<!-- Avatar circle -->
<img class="w-10 h-10 rounded-full object-cover" src="..." />

<!-- Modal dialog -->
<div class="bg-white rounded-2xl shadow-xl p-8 max-w-md w-full"></div>

<!-- Tab bar — top corners rounded, bottom flat -->
<button
  class="rounded-t-lg px-4 py-2 bg-white border-t border-l border-r border-gray-200"
>
  Tab Label
</button>
```

---

## 5. Outline

```html
<!-- Outline width -->
<div class="outline-0">
  <!-- outline-width: 0 -->
  <div class="outline-1">
    <!-- outline-width: 1px -->
    <div class="outline-2">
      <!-- outline-width: 2px -->
      <div class="outline-4">
        <!-- outline-width: 4px -->
        <div class="outline-8">
          <!-- outline-width: 8px -->

          <!-- Outline colour -->
          <div class="outline outline-blue-500">
            <!-- Outline style -->
            <div class="outline">
              <!-- outline-style: solid (shorthand) -->
              <div class="outline-dashed">
                <!-- outline-style: dashed -->
                <div class="outline-dotted">
                  <!-- outline-style: dotted -->
                  <div class="outline-double">
                    <!-- outline-style: double -->
                    <div class="outline-none">
                      <!-- outline: 2px solid transparent; outline-offset: 2px -->
                      <!-- use this on focusable elements when using ring instead -->

                      <!-- Outline offset -->
                      <div class="outline-offset-0">
                        <!-- outline-offset: 0 -->
                        <div class="outline-offset-1">
                          <!-- outline-offset: 1px -->
                          <div class="outline-offset-2">
                            <!-- outline-offset: 2px -->
                            <div class="outline-offset-4">
                              <!-- outline-offset: 4px -->
                              <div class="outline-offset-8">
                                <!-- outline-offset: 8px -->
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

```
Outline vs border:
  border takes up space — it's part of the box model.
  outline does NOT take up space — it renders outside the element.
  outline is commonly used for focus styles and debug overlays.

  DO NOT use outline: none (browser default) or outline-none without
  providing an alternative focus indicator.
  See ring utilities (next section) for accessible focus styles.
```

---

## 6. Ring Utilities — Focus Outlines

Rings simulate an `outline` using `box-shadow`. They avoid the box-model impact of borders and can be layered.

```html
<!-- Ring width -->
<button class="ring-0">
  <!-- box-shadow: none -->
  <button class="ring-1">
    <!-- 1px ring -->
    <button class="ring-2">
      <!-- 2px ring -->
      <button class="ring">
        <!-- 3px ring (default) -->
        <button class="ring-4">
          <!-- 4px ring -->
          <button class="ring-8">
            <!-- 8px ring -->

            <!-- Ring colour -->
            <button class="ring ring-blue-500">
              <button class="ring ring-blue-500/50">
                <!-- with opacity -->

                <!-- Ring offset (space between element and ring) -->
                <button class="ring ring-blue-500 ring-offset-2">
                  <button class="ring ring-blue-500 ring-offset-4">
                    <button
                      class="ring ring-blue-500 ring-offset-2 ring-offset-white"
                    >
                      <!-- ring-offset-{colour} matches offset bg to the page background -->
                    </button>
                  </button>
                </button>
              </button>
            </button>
          </button>
        </button>
      </button>
    </button>
  </button>
</button>
```

```html
<!-- Standard accessible focus style pattern -->
<button
  class="
  px-4 py-2 bg-blue-600 text-white rounded-md
  focus:outline-none
  focus:ring-2 focus:ring-blue-500 focus:ring-offset-2
"
>
  Button
</button>

<!-- Explanation:
  focus:outline-none  → removes browser default outline (we replace it)
  focus:ring-2        → 2px blue ring on focus
  focus:ring-offset-2 → 2px gap between button edge and ring (legibility)
  
  This pattern is accessible: keyboard users still see a clear focus indicator.
  Never use focus:outline-none alone without providing a replacement. -->
```

---

## 7. Divide — Borders Between Children

`divide-*` adds borders between sibling elements using a CSS selector trick.

```html
<!-- Vertical dividers (between rows in a column) -->
<div class="flex flex-col divide-y divide-gray-200">
  <div class="py-4">Item 1</div>
  <div class="py-4">Item 2</div>
  <!-- border-top: 1px gray-200 -->
  <div class="py-4">Item 3</div>
  <!-- border-top: 1px gray-200 -->
  <!-- No border above Item 1 -->
</div>

<!-- Horizontal dividers (between columns in a row) -->
<div class="flex divide-x divide-gray-300">
  <div class="px-4">Col A</div>
  <div class="px-4">Col B</div>
  <!-- border-left: 1px gray-300 -->
  <div class="px-4">Col C</div>
  <!-- border-left: 1px gray-300 -->
</div>

<!-- Divide width -->
<div class="flex flex-col divide-y divide-y-2">
  <!-- 2px dividers -->
  <div class="flex flex-col divide-y divide-y-4">
    <!-- 4px dividers -->

    <!-- Divide colour with opacity -->
    <div class="divide-y divide-gray-900/10">
      <!-- Divide style -->
      <div class="divide-y divide-dashed divide-gray-200">
        <div class="divide-y divide-dotted divide-gray-200"></div>
      </div>
    </div>
  </div>
</div>
```

---

## 8. Box Shadow

```html
<!-- Drop shadows -->
<div class="shadow-sm">
  <!-- very small shadow -->
  <div class="shadow">
    <!-- small shadow -->
    <div class="shadow-md">
      <!-- medium shadow -->
      <div class="shadow-lg">
        <!-- large shadow -->
        <div class="shadow-xl">
          <!-- extra large shadow -->
          <div class="shadow-2xl">
            <!-- double extra large -->
            <div class="shadow-inner">
              <!-- inset shadow (pressed effect) -->
              <div class="shadow-none">
                <!-- removes shadow -->

                <!-- Coloured shadows (v3.1+) -->
                <div class="shadow-lg shadow-blue-500/50">
                  <div class="shadow-xl shadow-indigo-500/25"></div>
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
Box shadow values (what Tailwind generates):
  shadow-sm:  0 1px 2px 0 rgb(0 0 0 / 0.05)
  shadow:     0 1px 3px 0 rgb(0 0 0 / 0.1), 0 1px 2px -1px rgb(0 0 0 / 0.1)
  shadow-md:  0 4px 6px -1px rgb(0 0 0 / 0.1), 0 2px 4px -2px rgb(0 0 0 / 0.1)
  shadow-lg:  0 10px 15px -3px rgb(0 0 0 / 0.1), 0 4px 6px -4px rgb(0 0 0 / 0.1)
  shadow-xl:  0 20px 25px -5px rgb(0 0 0 / 0.1), 0 8px 10px -6px rgb(0 0 0 / 0.1)
  shadow-2xl: 0 25px 50px -12px rgb(0 0 0 / 0.25)
  shadow-inner: inset 0 2px 4px 0 rgb(0 0 0 / 0.05)
```

---

## 9. Text Shadow & Drop Shadow Filter

Tailwind doesn't include text-shadow utilities by default. Use the filter drop-shadow for image/SVG shadows.

```html
<!-- drop-shadow applies to the shape, not the box -->
<img class="drop-shadow-md" src="..." />
<img class="drop-shadow-lg" src="..." />
<img class="drop-shadow-xl" src="..." />
<img class="drop-shadow-2xl" src="..." />
<img class="drop-shadow-none" src="..." />
```

---

## 10. Transforms — Translate, Scale, Rotate, Skew

Transforms move, resize, or deform elements without affecting document flow.

```html
<!-- Translate (move) -->
<div class="translate-x-4">
  <!-- translateX(1rem) -->
  <div class="translate-y-4">
    <!-- translateY(1rem) -->
    <div class="-translate-x-4">
      <!-- translateX(-1rem) — move left -->
      <div class="-translate-y-4">
        <!-- translateY(-1rem) — move up -->
        <div class="translate-x-1/2">
          <!-- translateX(50%) -->
          <div class="translate-x-full">
            <!-- translateX(100%) — move its own width right -->
            <div class="-translate-x-full">
              <!-- move its own width left (off-screen slide) -->
              <div class="translate-x-px">
                <!-- translateX(1px) -->

                <!-- Scale -->
                <div class="scale-0">
                  <!-- scale(0) — invisible -->
                  <div class="scale-50">
                    <!-- scale(0.5) — half size -->
                    <div class="scale-75">
                      <!-- scale(0.75) -->
                      <div class="scale-90">
                        <!-- scale(0.9) — subtle shrink -->
                        <div class="scale-95">
                          <!-- scale(0.95) — button press effect -->
                          <div class="scale-100">
                            <!-- scale(1.0) — original (explicit reset) -->
                            <div class="scale-105">
                              <!-- scale(1.05) — hover grow -->
                              <div class="scale-110">
                                <!-- scale(1.1) -->
                                <div class="scale-125">
                                  <!-- scale(1.25) -->
                                  <div class="scale-150">
                                    <!-- scale(1.5) -->
                                    <div class="-scale-x-100">
                                      <!-- mirror horizontally (flip) -->
                                      <div class="-scale-y-100">
                                        <!-- mirror vertically (flip) -->

                                        <!-- Rotate -->
                                        <div class="rotate-0">
                                          <!-- rotate(0deg) -->
                                          <div class="rotate-1">
                                            <!-- rotate(1deg) -->
                                            <div class="rotate-2">
                                              <!-- rotate(2deg) — subtle tilt -->
                                              <div class="rotate-3">
                                                <!-- rotate(3deg) -->
                                                <div class="rotate-6">
                                                  <!-- rotate(6deg) -->
                                                  <div class="rotate-12">
                                                    <!-- rotate(12deg) -->
                                                    <div class="rotate-45">
                                                      <!-- rotate(45deg) — diamond -->
                                                      <div class="rotate-90">
                                                        <!-- rotate(90deg) -->
                                                        <div class="rotate-180">
                                                          <!-- rotate(180deg) — flip upside down -->
                                                          <div
                                                            class="-rotate-6"
                                                          >
                                                            <!-- rotate(-6deg) — tilt the other way -->
                                                            <div
                                                              class="-rotate-45"
                                                            >
                                                              <!-- rotate(-45deg) -->

                                                              <!-- Skew -->
                                                              <div
                                                                class="skew-x-2"
                                                              >
                                                                <!-- skewX(2deg) -->
                                                                <div
                                                                  class="skew-x-6"
                                                                >
                                                                  <!-- skewX(6deg) -->
                                                                  <div
                                                                    class="skew-y-3"
                                                                  >
                                                                    <!-- skewY(3deg) -->
                                                                    <div
                                                                      class="-skew-x-6"
                                                                    >
                                                                      <!-- skewX(-6deg) -->
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
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```html
<!-- Practical examples -->

<!-- Hover grow on card -->
<div class="transition-transform hover:scale-105 cursor-pointer">Card</div>

<!-- Animated hamburger menu — rotate bars on toggle -->
<span
  class="block h-0.5 w-6 bg-current transition-transform"
  :class="{ 'rotate-45 translate-y-1.5': isOpen }"
>
</span>

<!-- Accordion chevron — rotate 180° when open -->
<svg class="transition-transform" :class="{ 'rotate-180': isOpen }"></svg>
```

---

## 11. Transform Origin

Controls the point around which a transform is applied.

```html
<div class="origin-center">
  <!-- 50% 50% — default -->
  <div class="origin-top">
    <!-- 50% 0% -->
    <div class="origin-top-right">
      <!-- 100% 0% -->
      <div class="origin-right">
        <!-- 100% 50% -->
        <div class="origin-bottom-right">
          <!-- 100% 100% -->
          <div class="origin-bottom">
            <!-- 50% 100% -->
            <div class="origin-bottom-left">
              <!-- 0% 100% -->
              <div class="origin-left">
                <!-- 0% 50% -->
                <div class="origin-top-left"><!-- 0% 0% --></div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```html
<!-- Dropdown menus typically scale from the top -->
<div
  class="origin-top scale-95 opacity-0 transition-all
            group-hover:scale-100 group-hover:opacity-100"
>
  Dropdown content
</div>
```

---

## 12. Transition

```html
<!-- transition — what to transition -->
<div class="transition">
  <!-- transition: color, background-color, border-color,
                                              text-decoration-color, fill, stroke,
                                              opacity, box-shadow, transform, filter,
                                              backdrop-filter 150ms cubic-bezier(0.4, 0, 0.2, 1) -->
  <div class="transition-all">
    <!-- transition: all ... -->
    <div class="transition-colors">
      <!-- transition: color, bg, border-color... -->
      <div class="transition-opacity">
        <!-- transition: opacity ... -->
        <div class="transition-shadow">
          <!-- transition: box-shadow ... -->
          <div class="transition-transform">
            <!-- transition: transform ... -->
            <div class="transition-none">
              <!-- transition: none (disables) -->

              <!-- Duration -->
              <div class="duration-75">
                <!-- transition-duration: 75ms -->
                <div class="duration-100">
                  <!-- 100ms -->
                  <div class="duration-150">
                    <!-- 150ms (default for transition) -->
                    <div class="duration-200">
                      <!-- 200ms -->
                      <div class="duration-300">
                        <!-- 300ms -->
                        <div class="duration-500">
                          <!-- 500ms -->
                          <div class="duration-700">
                            <!-- 700ms -->
                            <div class="duration-1000">
                              <!-- 1000ms -->

                              <!-- Easing -->
                              <div class="ease-linear">
                                <!-- transition-timing-function: linear -->
                                <div class="ease-in">
                                  <!-- cubic-bezier(0.4, 0, 1, 1) — starts slow -->
                                  <div class="ease-out">
                                    <!-- cubic-bezier(0, 0, 0.2, 1) — ends slow (default) -->
                                    <div class="ease-in-out">
                                      <!-- cubic-bezier(0.4, 0, 0.2, 1) — both -->

                                      <!-- Delay -->
                                      <div class="delay-75">
                                        <!-- transition-delay: 75ms -->
                                        <div class="delay-100">
                                          <!-- 100ms -->
                                          <div class="delay-150">
                                            <!-- 150ms -->
                                            <div class="delay-200">
                                              <!-- 200ms -->
                                              <div class="delay-300">
                                                <!-- 300ms -->
                                                <div class="delay-500">
                                                  <!-- 500ms -->
                                                  <div class="delay-700">
                                                    <!-- 700ms -->
                                                    <div class="delay-1000">
                                                      <!-- 1000ms -->
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

```html
<!-- Full button hover transition example -->
<button
  class="
  bg-blue-600 text-white px-6 py-2 rounded-lg font-medium
  transition-all duration-200 ease-in-out
  hover:bg-blue-700
  hover:shadow-lg
  hover:scale-105
  active:scale-95 active:shadow-sm
"
>
  Click me
</button>
```

---

## 13. Animation

Tailwind includes four built-in animations.

```html
<!-- spin — continuously rotates (for loaders) -->
<svg class="animate-spin h-5 w-5 text-blue-500" viewBox="0 0 24 24">
  <circle class="opacity-25" ... />
  <path class="opacity-75" ... />
</svg>

<!-- ping — expands and fades out (for notifications/live indicators) -->
<span class="relative flex h-3 w-3">
  <span
    class="animate-ping absolute inline-flex h-full w-full rounded-full bg-sky-400 opacity-75"
  ></span>
  <span class="relative inline-flex rounded-full h-3 w-3 bg-sky-500"></span>
</span>

<!-- pulse — fades in and out (for skeleton loaders) -->
<div class="animate-pulse bg-gray-200 rounded h-4 w-full mb-2"></div>
<div class="animate-pulse bg-gray-200 rounded h-4 w-3/4 mb-2"></div>
<div class="animate-pulse bg-gray-200 rounded h-4 w-1/2"></div>

<!-- bounce — bounces up and down (use sparingly) -->
<span class="animate-bounce text-2xl">↓</span>

<!-- none — remove animation -->
<div class="animate-none"></div>
```

```html
<!-- Skeleton loader pattern using animate-pulse -->
<div class="animate-pulse space-y-3">
  <div class="flex items-center gap-3">
    <div class="rounded-full bg-gray-200 h-10 w-10"></div>
    <!-- avatar -->
    <div class="flex-1 space-y-1">
      <div class="h-4 bg-gray-200 rounded w-3/4"></div>
      <!-- name line -->
      <div class="h-3 bg-gray-200 rounded w-1/2"></div>
      <!-- sub-line -->
    </div>
  </div>
  <div class="h-4 bg-gray-200 rounded w-full"></div>
  <div class="h-4 bg-gray-200 rounded w-5/6"></div>
</div>
```

---

## 14. Backdrop Filter — Frosted Glass

Applies CSS filters to the area **behind** an element. Requires the element to have some transparency.

```html
<!-- blur the background — frosted glass effect -->
<div class="backdrop-blur-sm">
  <!-- backdrop-filter: blur(4px) -->
  <div class="backdrop-blur">
    <!-- blur(8px) -->
    <div class="backdrop-blur-md">
      <!-- blur(12px) -->
      <div class="backdrop-blur-lg">
        <!-- blur(16px) -->
        <div class="backdrop-blur-xl">
          <!-- blur(24px) -->
          <div class="backdrop-blur-2xl">
            <!-- blur(40px) -->
            <div class="backdrop-blur-3xl">
              <!-- blur(64px) -->
              <div class="backdrop-blur-none">
                <!-- removes blur -->

                <!-- Other backdrop filters -->
                <div class="backdrop-brightness-75">
                  <!-- backdrop-filter: brightness(0.75) -->
                  <div class="backdrop-contrast-125">
                    <!-- contrast(1.25) -->
                    <div class="backdrop-grayscale">
                      <!-- grayscale(1) -->
                      <div class="backdrop-invert">
                        <!-- invert(1) -->
                        <div class="backdrop-opacity-50">
                          <!-- opacity(0.5) -->
                          <div class="backdrop-saturate-150">
                            <!-- saturate(1.5) -->
                            <div class="backdrop-sepia"><!-- sepia(1) --></div>
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

```html
<!-- Frosted glass navigation bar -->
<header
  class="
  fixed top-0 left-0 right-0
  bg-white/70
  backdrop-blur-md
  border-b border-white/20
  shadow-sm
  z-50
"
>
  <!-- Semi-transparent background + backdrop blur = frosted glass -->
  <!-- Content behind the header is blurred through it -->
</header>

<!-- Glass card -->
<div
  class="
  bg-white/20
  backdrop-blur-lg
  border border-white/30
  rounded-xl shadow-xl
  p-6
"
>
  Glass card
</div>
```

---

## 15. Filter — Blur, Brightness, Contrast

Applies CSS filters to the element itself (not its backdrop).

```html
<!-- Blur (the element, not what's behind it) -->
<img class="blur-sm" />
<!-- filter: blur(4px) -->
<img class="blur" />
<!-- filter: blur(8px) -->
<img class="blur-md" />
<!-- filter: blur(12px) -->
<img class="blur-lg" />
<!-- filter: blur(16px) -->
<img class="blur-xl" />
<!-- filter: blur(24px) -->
<img class="blur-2xl" />
<!-- filter: blur(40px) -->
<img class="blur-3xl" />
<!-- filter: blur(64px) -->
<img class="blur-none" />
<!-- filter: none (removes blur) -->

<!-- Brightness -->
<img class="brightness-50" />
<!-- filter: brightness(0.5) — darker -->
<img class="brightness-75" />
<!-- 75% brightness -->
<img class="brightness-90" />
<!-- slightly dimmed -->
<img class="brightness-100" />
<!-- normal (explicit reset) -->
<img class="brightness-110" />
<!-- slightly brighter -->
<img class="brightness-125" />
<!-- brighter -->
<img class="brightness-150" />
<!-- much brighter -->
<img class="brightness-200" />
<!-- very bright -->

<!-- Contrast -->
<img class="contrast-50" />
<!-- low contrast -->
<img class="contrast-75" />
<img class="contrast-100" />
<!-- normal -->
<img class="contrast-125" />
<!-- higher contrast -->
<img class="contrast-150" />
<img class="contrast-200" />

<!-- Grayscale -->
<img class="grayscale" />
<!-- filter: grayscale(1) — fully desaturated -->
<img class="grayscale-0" />
<!-- filter: grayscale(0) — full colour (reset) -->

<!-- Saturate -->
<img class="saturate-0" />
<!-- fully desaturated -->
<img class="saturate-50" />
<!-- 50% saturation -->
<img class="saturate-100" />
<!-- normal -->
<img class="saturate-150" />
<!-- oversaturated — vivid -->
<img class="saturate-200" />

<!-- Hue rotate -->
<img class="hue-rotate-15" />
<!-- rotate hue by 15 degrees -->
<img class="hue-rotate-30" />
<img class="hue-rotate-60" />
<img class="hue-rotate-90" />
<img class="hue-rotate-180" />
<!-- complementary colour -->

<!-- Invert -->
<img class="invert" />
<!-- filter: invert(1) — negative image -->
<img class="invert-0" />
<!-- normal (reset) -->

<!-- Sepia -->
<img class="sepia" />
<!-- filter: sepia(1) — old photo look -->
<img class="sepia-0" />
<!-- normal (reset) -->
```

```html
<!-- Disabled state with visual feedback — a common pattern -->
<button
  class="bg-blue-600 text-white px-4 py-2 rounded
         disabled:opacity-50 disabled:cursor-not-allowed disabled:brightness-90"
  :disabled="isLoading"
>
  Submit
</button>

<!-- Image hover reveal — greyscale to colour -->
<img
  class="grayscale transition-all duration-300 hover:grayscale-0 hover:scale-105"
  src="..."
/>
```
