> **Series overview:** This is file 3 of 7. It covers every Flexbox and CSS Grid utility Tailwind provides, from the container properties to individual item properties. Both systems are explained from first principles before the utilities are introduced.

---

## Table of Contents

1. [Flexbox — The Mental Model](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#1-flexbox--the-mental-model)
2. [Enabling Flex & Flex Direction](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#2-enabling-flex--flex-direction)
3. [Flex Wrapping](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#3-flex-wrapping)
4. [Justify Content — Main Axis Alignment](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#4-justify-content--main-axis-alignment)
5. [Align Items — Cross Axis Alignment](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#5-align-items--cross-axis-alignment)
6. [Align Content — Multi-Line Cross Axis](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#6-align-content--multi-line-cross-axis)
7. [Gap](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#7-gap)
8. [Flex Item Properties — Grow, Shrink, Basis](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#8-flex-item-properties--grow-shrink-basis)
9. [Order](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#9-order)
10. [Align Self — Per-Item Alignment Override](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#10-align-self--per-item-alignment-override)
11. [CSS Grid — The Mental Model](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#11-css-grid--the-mental-model)
12. [Defining Grid Columns & Rows](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#12-defining-grid-columns--rows)
13. [Spanning Columns & Rows](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#13-spanning-columns--rows)
14. [Grid Flow & Auto Sizing](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#14-grid-flow--auto-sizing)
15. [Justify & Align in Grid](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#15-justify--align-in-grid)
16. [Flexbox vs Grid — When to Use Which](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#16-flexbox-vs-grid--when-to-use-which)
17. [Common Layout Patterns](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#17-common-layout-patterns)

---

## 1. Flexbox — The Mental Model

Flexbox is a one-dimensional layout system. It arranges items along a single axis — either a row (horizontal) or a column (vertical).

```
The container has two axes:

  flex-row (default):
    main axis  →→→→→→→→→→→→→→→→→→→→→→→  (horizontal)
    cross axis ↓ (vertical)

  flex-col:
    main axis  ↓ (vertical)
    cross axis →→→→→→→→→  (horizontal)

justify-* controls distribution along the MAIN axis.
items-*   controls alignment along the CROSS axis.

This is the most important thing to understand about flexbox.
```

---

## 2. Enabling Flex & Flex Direction

```html
<!-- Make an element a flex container -->
<div class="flex">
  <!-- display: flex (block-level) -->
  <div class="inline-flex">
    <!-- display: inline-flex (inline-level) -->

    <!-- Flex direction — which way children flow -->
    <div class="flex flex-row">
      <!-- default: left to right -->
      <div class="flex flex-row-reverse">
        <!-- right to left -->
        <div class="flex flex-col">
          <!-- top to bottom -->
          <div class="flex flex-col-reverse"><!-- bottom to top --></div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```html
<!-- Children in a flex container are flex items automatically.
     No class needed on the children — they respond to the container. -->

<div class="flex flex-row gap-4">
  <div class="bg-red-200 p-4">Item 1</div>
  <div class="bg-blue-200 p-4">Item 2</div>
  <div class="bg-green-200 p-4">Item 3</div>
  <!-- These three divs arrange themselves in a row automatically -->
</div>
```

---

## 3. Flex Wrapping

By default, flex items never wrap — they all try to fit on one line, shrinking if necessary.

```html
<div class="flex flex-nowrap">
  <!-- default: never wrap -->
  <div class="flex flex-wrap">
    <!-- wrap to next line when needed -->
    <div class="flex flex-wrap-reverse">
      <!-- wrap upward instead of downward -->
    </div>
  </div>
</div>
```

```html
<!-- A gallery that wraps gracefully on small screens -->
<div class="flex flex-wrap gap-4">
  <div class="w-48 h-48 bg-gray-200">Card</div>
  <div class="w-48 h-48 bg-gray-200">Card</div>
  <div class="w-48 h-48 bg-gray-200">Card</div>
  <!-- Cards maintain w-48 (192px) each. When the container is too
       narrow for all three, they wrap to a second row. -->
</div>
```

---

## 4. Justify Content — Main Axis Alignment

Controls how items are distributed along the main axis.

```html
<div class="flex justify-start">
  <!-- pack to start (default) -->
  <div class="flex justify-end">
    <!-- pack to end -->
    <div class="flex justify-center">
      <!-- centered -->
      <div class="flex justify-between">
        <!-- first and last at edges, space between -->
        <div class="flex justify-around">
          <!-- equal space around each item -->
          <div class="flex justify-evenly">
            <!-- equal space between items and edges -->
            <div class="flex justify-stretch">
              <!-- items stretch to fill (for grid) -->
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

```
justify-between visualised with 3 items:

  [Item 1]        [Item 2]        [Item 3]
  ↑                                       ↑
  edge                                    edge

justify-around:

    [Item 1]      [Item 2]      [Item 3]
  ↑ ↑             ↑ ↑          ↑ ↑      ↑
  ½x  x           x   x         x  ½x
  (half space at edges, full space between)

justify-evenly:

   [Item 1]       [Item 2]       [Item 3]
  ↑↑               ↑↑             ↑↑     ↑↑
  x                x              x       x
  (equal space everywhere, including edges)
```

---

## 5. Align Items — Cross Axis Alignment

Controls how items are aligned on the cross axis (perpendicular to main axis).

```html
<div class="flex items-start">
  <!-- cross axis start (top for flex-row) -->
  <div class="flex items-end">
    <!-- cross axis end (bottom for flex-row) -->
    <div class="flex items-center">
      <!-- centered on cross axis -->
      <div class="flex items-baseline">
        <!-- aligned by text baseline -->
        <div class="flex items-stretch">
          <!-- stretch to fill cross axis (default) -->
        </div>
      </div>
    </div>
  </div>
</div>
```

```html
<!-- Centering an element horizontally AND vertically -->
<!-- This is the most common flexbox use case -->
<div class="flex items-center justify-center h-screen">
  <div class="bg-white p-8 rounded-xl shadow-lg">
    Perfectly centered content
  </div>
</div>
```

```
items-stretch (default) explained:

  Without items-stretch:  ┌──┐ ┌────┐ ┌──────┐
  (items-start)           └──┘ └────┘ └──────┘

  With items-stretch:     ┌──┐ ┌────┐ ┌──────┐
                          │  │ │    │ │      │
                          │  │ │    │ │      │
                          └──┘ └────┘ └──────┘

  All items stretch to match the tallest item's height.
  Good for equal-height card columns.
```

---

## 6. Align Content — Multi-Line Cross Axis

When flex items wrap onto multiple lines, `align-content` controls how those lines are distributed vertically.

```html
<!-- Only has effect when flex-wrap and multiple lines exist -->
<div class="flex flex-wrap content-start">
  <!-- lines packed to top -->
  <div class="flex flex-wrap content-end">
    <!-- lines packed to bottom -->
    <div class="flex flex-wrap content-center">
      <!-- lines centered vertically -->
      <div class="flex flex-wrap content-between">
        <!-- space between lines -->
        <div class="flex flex-wrap content-around">
          <!-- space around lines -->
          <div class="flex flex-wrap content-evenly">
            <!-- even space between lines -->
            <div class="flex flex-wrap content-stretch">
              <!-- lines stretch to fill (default) -->
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

---

## 7. Gap

Gap adds space **between** flex or grid items without adding space at the outer edges. Prefer it over `space-x-*`/`space-y-*` for wrapping layouts.

```html
<!-- Same gap in both directions -->
<div class="flex flex-wrap gap-4">
  <!-- gap: 1rem between all items -->

  <!-- Different horizontal and vertical gap -->
  <div class="flex flex-wrap gap-x-6 gap-y-4">
    <!-- column gap (horizontal): 1.5rem -->
    <!-- row gap (vertical):      1rem    -->

    <!-- Shorthand reference -->
    <div class="gap-0">
      <!-- gap: 0 -->
      <div class="gap-1">
        <!-- gap: 0.25rem -->
        <div class="gap-2">
          <!-- gap: 0.5rem -->
          <div class="gap-4">
            <!-- gap: 1rem -->
            <div class="gap-6">
              <!-- gap: 1.5rem -->
              <div class="gap-8">
                <!-- gap: 2rem -->
                <div class="gap-px"><!-- gap: 1px --></div>
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

## 8. Flex Item Properties — Grow, Shrink, Basis

These classes go on the flex **items** (children), not the container.

### flex-basis — Starting Size

```html
<!-- flex-basis sets the default size of an item before growing/shrinking -->
<div class="flex">
  <div class="basis-0">
    <!-- flex-basis: 0 -->
    <div class="basis-auto">
      <!-- flex-basis: auto (uses width/height) -->
      <div class="basis-full">
        <!-- flex-basis: 100% -->
        <div class="basis-1/2">
          <!-- flex-basis: 50% -->
          <div class="basis-1/3">
            <!-- flex-basis: 33.333% -->
            <div class="basis-64"><!-- flex-basis: 16rem --></div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### flex-grow — How Much to Grow

```html
<!-- flex-grow 0 = don't grow (default) -->
<!-- flex-grow 1 = grow to fill available space -->
<div class="flex">
  <div class="grow-0 w-24">Fixed sidebar</div>
  <div class="grow">Main content — takes all remaining space</div>
</div>
```

### flex-shrink — How Much to Shrink

```html
<!-- flex-shrink 0 = never shrink below basis/width -->
<!-- flex-shrink 1 = can shrink if needed (default) -->
<div class="flex">
  <div class="shrink-0 w-24">This will never get smaller than w-24</div>
  <div class="shrink">This can shrink if the container is small</div>
</div>
```

### Shorthand flex Classes

```html
<!-- flex shorthand — sets grow, shrink, basis together -->
<div class="flex-1">
  <!-- flex: 1 1 0% — grow and shrink equally, start at 0 -->
  <div class="flex-auto">
    <!-- flex: 1 1 auto — grow and shrink, start at natural size -->
    <div class="flex-initial">
      <!-- flex: 0 1 auto — don't grow, can shrink (default) -->
      <div class="flex-none">
        <!-- flex: none — don't grow, don't shrink -->
      </div>
    </div>
  </div>
</div>
```

```html
<!-- Equal-width columns that share available space -->
<div class="flex">
  <div class="flex-1 bg-red-100 p-4">Column A</div>
  <div class="flex-1 bg-blue-100 p-4">Column B</div>
  <div class="flex-1 bg-green-100 p-4">Column C</div>
  <!-- All three start at basis:0 then grow equally to fill container -->
</div>

<!-- Fixed sidebar + flexible main -->
<div class="flex h-screen">
  <aside class="flex-none w-64 bg-gray-100">Sidebar</aside>
  <main class="flex-1 overflow-y-auto">Main content</main>
</div>
```

---

## 9. Order

By default, flex items appear in DOM order. `order-*` changes visual order without changing HTML.

```html
<div class="flex">
  <div class="order-3">Third visually, first in DOM</div>
  <div class="order-1">First visually, second in DOM</div>
  <div class="order-2">Second visually, third in DOM</div>
</div>

<!-- Available values: order-1 through order-12 -->
<!-- Also: order-first (-9999), order-last (9999), order-none (0) -->
```

---

## 10. Align Self — Per-Item Alignment Override

Overrides `align-items` for a single child.

```html
<div class="flex items-start h-32">
  <div>Aligned to top (from items-start)</div>
  <div class="self-center">This one is centered</div>
  <div class="self-end">This one is at the bottom</div>
</div>

<!-- Values mirror align-items: -->
<!-- self-auto, self-start, self-end, self-center, self-stretch, self-baseline -->
```

---

## 11. CSS Grid — The Mental Model

Grid is a two-dimensional layout system. It works with both rows AND columns simultaneously.

```
Grid terminology:

  grid container: the element with display: grid
  grid items:     direct children of the grid container
  grid line:      the dividing lines of the grid (numbered from 1)
  grid track:     a column or row between two grid lines
  grid cell:      a single unit of the grid
  grid area:      a rectangular region spanning multiple cells

  ↓ column tracks →
  1   2   3   4
  |   |   |   |
──┼───┼───┼───┼──  ← row 1
  │   │   │   │
──┼───┼───┼───┼──  ← row 2
  │   │   │   │
──┼───┼───┼───┼──  ← row 3
```

---

## 12. Defining Grid Columns & Rows

```html
<!-- Define columns with grid-cols-* -->
<div class="grid grid-cols-3">
  <!-- Creates 3 equal columns -->
  <!-- Children auto-place into cells left-to-right, wrapping to new rows -->
</div>

<!-- Standard presets -->
<div class="grid grid-cols-1">
  <!-- 1 column (stacked) -->
  <div class="grid grid-cols-2">
    <!-- 2 equal columns -->
    <div class="grid grid-cols-3">
      <!-- 3 equal columns -->
      <div class="grid grid-cols-4">
        <!-- 4 equal columns -->
        <div class="grid grid-cols-6">
          <!-- 6 equal columns -->
          <div class="grid grid-cols-12">
            <!-- 12-column grid -->
            <div class="grid grid-cols-none">
              <!-- removes grid-template-columns -->

              <!-- grid-cols-N generates: grid-template-columns: repeat(N, minmax(0, 1fr)) -->
              <!-- 1fr = "one fraction of the available space" — always equal columns -->
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
```

### Custom Column Templates

```html
<!-- Arbitrary value for precise control -->
<div class="grid grid-cols-[200px_1fr_1fr]">
  <!-- First column: fixed 200px -->
  <!-- Second and third: each take equal share of remaining space -->
</div>

<div class="grid grid-cols-[repeat(3,_minmax(200px,_1fr))]">
  <!-- 3 columns, each at least 200px, growing equally -->
</div>
```

### Defining Rows

```html
<!-- Explicit row heights -->
<div class="grid grid-rows-3">
  <!-- 3 equal rows (1fr each) -->
  <div class="grid grid-rows-none">
    <!-- no explicit rows -->
    <div class="grid grid-rows-[auto_1fr_auto]">
      <!-- auto = shrinks to content, 1fr = takes remaining space -->
      <!-- A classic header/content/footer layout -->
    </div>
  </div>
</div>
```

---

## 13. Spanning Columns & Rows

Grid items can span multiple columns or rows.

```html
<!-- Spanning columns -->
<div class="grid grid-cols-3 gap-4">
  <div class="col-span-1">Spans 1 column</div>
  <div class="col-span-2">Spans 2 columns</div>

  <div class="col-span-3">Spans all 3 columns (full width)</div>

  <div class="col-span-full">Also spans all columns</div>
</div>

<!-- Spanning rows -->
<div class="grid grid-cols-3 grid-rows-3 gap-4">
  <div class="row-span-2 col-span-2">Spans 2 rows and 2 columns</div>
  <div>Small cell</div>
  <div>Small cell</div>
  <div class="col-span-3">Full-width bottom row</div>
</div>
```

### Placing Items Explicitly

```html
<!-- col-start and col-end for exact placement -->
<div class="grid grid-cols-6">
  <div class="col-start-2 col-end-5">
    <!-- Starts at grid line 2, ends at grid line 5 (spans 3 columns) -->
  </div>

  <div class="col-start-1 col-span-2">
    <!-- Starts at line 1, spans 2 columns -->
  </div>
</div>
```

---

## 14. Grid Flow & Auto Sizing

```html
<!-- grid-auto-flow controls how auto-placed items fill the grid -->
<div class="grid grid-flow-row">
  <!-- fill row by row (default) -->
  <div class="grid grid-flow-col">
    <!-- fill column by column -->
    <div class="grid grid-flow-dense">
      <!-- fill gaps left by large items -->
      <div class="grid grid-flow-row-dense">
        <div class="grid grid-flow-col-dense"></div>
      </div>
    </div>
  </div>
</div>
```

### Auto Column/Row Sizes

```html
<!-- Size of implicitly-created tracks (when items overflow defined tracks) -->
<div class="grid grid-cols-3 auto-cols-fr">
  <!-- Implicit columns are 1fr wide -->
</div>

<div class="grid auto-rows-[120px]">
  <!-- Implicit rows are 120px tall -->
</div>

<!-- Common: auto-rows-fr makes implicit rows fill available space -->
<div class="grid grid-rows-[auto_1fr] h-screen">
  <!-- First row: auto height (for header) -->
  <!-- Second row: 1fr (for main content) -->
</div>
```

---

## 15. Justify & Align in Grid

Grid has more alignment options than flex because of the two-dimensional nature.

### Aligning Tracks Within the Container

```html
<!-- justify-content: align the grid columns within the container -->
<div class="grid grid-cols-3 justify-center">
  <!-- columns centred -->
  <div class="grid grid-cols-3 justify-between">
    <!-- columns spread out -->

    <!-- align-content: align the grid rows within the container -->
    <div class="grid grid-rows-3 content-center h-screen">
      <!-- rows centred -->
    </div>
  </div>
</div>
```

### Aligning Items Within Their Cells

```html
<!-- justify-items: align items horizontally within their grid cell -->
<div class="grid justify-items-start">
  <!-- left of cell -->
  <div class="grid justify-items-end">
    <!-- right of cell -->
    <div class="grid justify-items-center">
      <!-- centred horizontally -->
      <div class="grid justify-items-stretch">
        <!-- fill cell width (default) -->

        <!-- align-items: align items vertically within their grid cell -->
        <div class="grid items-start">
          <!-- top of cell -->
          <div class="grid items-end">
            <!-- bottom of cell -->
            <div class="grid items-center">
              <!-- vertically centred -->
              <div class="grid items-stretch">
                <!-- fill cell height (default) -->

                <!-- place-items: shorthand for align-items + justify-items -->
                <div class="grid place-items-center">
                  <!-- centred in cell both ways -->
                  <div class="grid place-items-start">
                    <!-- top-left of cell -->
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

### Per-Item Overrides

```html
<div class="grid grid-cols-3">
  <div class="justify-self-center">Centred in its cell</div>
  <div class="self-end">At the bottom of its cell</div>
  <div class="place-self-center">Centred both ways in its cell</div>
</div>
```

---

## 16. Flexbox vs Grid — When to Use Which

```
One rule of thumb:
  Layout you're designing from the CONTENT OUT → Flexbox
  Layout you're designing from the GRID IN → CSS Grid

Flexbox is content-driven:
  - Navigation bars
  - Button groups
  - Centering a single element
  - Any list of items arranged in one direction
  - Items that need to shrink/grow based on their content

Grid is structure-driven:
  - Page-level layout (header/sidebar/main/footer)
  - Card grids
  - Image galleries
  - Any layout where rows and columns must align across items
  - Complex overlapping layouts
```

```html
<!-- USE FLEX: navigation items in a row -->
<nav class="flex items-center gap-6">
  <a href="/">Home</a>
  <a href="/about">About</a>
  <a href="/blog">Blog</a>
  <a href="/contact" class="ml-auto">Contact</a>
</nav>

<!-- USE GRID: card gallery that keeps columns aligned -->
<div class="grid grid-cols-3 gap-6">
  <div class="bg-white rounded-lg p-6 shadow">Card 1</div>
  <div class="bg-white rounded-lg p-6 shadow">Card 2</div>
  <div class="bg-white rounded-lg p-6 shadow">Card 3</div>
  <!-- Cards are same size regardless of content length.
       Their columns align vertically. -->
</div>

<!-- NEST THEM: grid for page layout, flex within cells -->
<div class="grid grid-cols-[1fr_3fr] h-screen">
  <aside class="flex flex-col gap-4 p-4 bg-gray-100">
    <a href="#">Dashboard</a>
    <a href="#">Settings</a>
    <a href="#" class="mt-auto">Log out</a>
    <!-- mt-auto pushes logout to bottom of flex column -->
  </aside>
  <main class="p-8 overflow-y-auto">Main content</main>
</div>
```

---

## 17. Common Layout Patterns

### Centered Hero Section

```html
<section
  class="flex flex-col items-center justify-center min-h-screen text-center px-4"
>
  <h1 class="text-5xl font-bold text-gray-900">Headline</h1>
  <p class="mt-4 text-xl text-gray-500 max-w-2xl">Subtext here</p>
  <div class="mt-8 flex gap-4">
    <button class="bg-blue-600 text-white px-6 py-3 rounded-lg">Primary</button>
    <button class="border border-gray-300 px-6 py-3 rounded-lg">
      Secondary
    </button>
  </div>
</section>
```

### Sidebar + Content Layout

```html
<div class="flex h-screen overflow-hidden">
  <aside class="w-64 flex-none bg-gray-900 text-white overflow-y-auto">
    Sidebar
  </aside>
  <main class="flex-1 overflow-y-auto bg-gray-50 p-8">Main content</main>
</div>
```

### Responsive Card Grid

```html
<!-- 1 column on mobile, 2 on sm, 3 on lg -->
<div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
  <div class="bg-white rounded-xl shadow p-6">Card</div>
  <div class="bg-white rounded-xl shadow p-6">Card</div>
  <div class="bg-white rounded-xl shadow p-6">Card</div>
</div>
```

### Sticky Header + Scrollable Content

```html
<div class="flex flex-col h-screen">
  <header class="flex-none h-16 bg-white border-b flex items-center px-6">
    Header
  </header>
  <main class="flex-1 overflow-y-auto p-6">
    <!-- flex-1 makes this fill remaining height, overflow-y-auto scrolls -->
    Long scrollable content...
  </main>
</div>
```

### Holy Grail Layout (Header / Sidebar / Main / Footer)

```html
<div class="grid grid-rows-[auto_1fr_auto] min-h-screen">
  <header class="bg-white border-b h-16 flex items-center px-6">Header</header>

  <div class="grid grid-cols-[240px_1fr] overflow-hidden">
    <nav class="bg-gray-100 overflow-y-auto p-4">Sidebar</nav>
    <main class="overflow-y-auto p-8">Main content</main>
  </div>

  <footer class="bg-gray-900 text-white text-center py-4">Footer</footer>
</div>
```
