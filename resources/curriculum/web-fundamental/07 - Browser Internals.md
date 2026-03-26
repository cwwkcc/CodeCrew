> The browser is the most complex piece of software most people interact with daily. Understanding how it works — how it turns HTML text into pixels, how JavaScript executes, how network requests happen asynchronously — makes you a fundamentally better web developer. Everything clicks when you understand the machine.

---

## Table of Contents

1. [What the Browser Is — Architecture Overview](#1-what-the-browser-is--architecture-overview)
2. [Processes and Threads](#2-processes-and-threads)
3. [Navigation — What Happens When You Type a URL](#3-navigation--what-happens-when-you-type-a-url)
4. [HTML Parsing — Building the DOM](#4-html-parsing--building-the-dom)
5. [CSS Parsing — Building the CSSOM](#5-css-parsing--building-the-cssom)
6. [The Render Pipeline — From Trees to Pixels](#6-the-render-pipeline--from-trees-to-pixels)
7. [The JavaScript Engine — How V8 Works](#7-the-javascript-engine--how-v8-works)
8. [The Event Loop — How Async Works in the Browser](#8-the-event-loop--how-async-works-in-the-browser)
9. [How fetch() Works Under the Hood](#9-how-fetch-works-under-the-hood)
10. [React Rendering in the Browser](#10-react-rendering-in-the-browser)
11. [Service Workers — The Browser's Proxy](#11-service-workers--the-browsers-proxy)

---

## 1. What the Browser Is — Architecture Overview

A browser is a platform. It's an operating system for web applications. It includes:

```
┌─────────────────────────────────────────────────────────────┐
│                        BROWSER                              │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │   Browser   │  │  Rendering  │  │   JavaScript        │ │
│  │    UI       │  │   Engine    │  │     Engine          │ │
│  │ (address    │  │  (Blink in  │  │  (V8 in Chrome,     │ │
│  │  bar, tabs) │  │   Chrome)   │  │   SpiderMonkey in   │ │
│  └─────────────┘  └─────────────┘  │   Firefox)          │ │
│                                    └─────────────────────┘ │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │  Networking │  │   Storage   │  │   Browser APIs      │ │
│  │  (HTTP,     │  │  (cookies,  │  │  (DOM, fetch,       │ │
│  │  WebSocket) │  │  IndexedDB, │  │   Canvas, WebGL,    │ │
│  │             │  │  cache API) │  │   Notifications...) │ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

JavaScript does not include fetch(), setTimeout(), DOM APIs, or localStorage. Those are provided by the browser environment. The JavaScript engine (V8) handles only pure JS execution. Everything else comes from the browser's C++ internals, exposed to JavaScript via Web APIs.

---

## 2. Processes and Threads

Modern browsers use multi-process architecture for isolation and stability.

```
Chrome's process model:

Browser Process (one, always)
  → Controls UI (address bar, tabs, back button)
  → Manages other processes
  → Handles network requests

Renderer Process (one per site origin, or per tab)
  → Runs HTML parsing, CSS, layout, paint
  → Runs JavaScript
  → Sandboxed — no direct OS access
  → If it crashes: only that tab dies, browser survives
  → Isolated: paideon.lk renderer cannot touch google.com memory

GPU Process (one)
  → Handles GPU compositing
  → Receives paint instructions from renderer

Network Process (one)
  → Handles all HTTP requests
  → Separated so network access is controlled

Plugin/Extension processes (separate per plugin)

Why this matters:
  JavaScript runs in the renderer process.
  The renderer process has ONE main thread.
  Everything that touches the DOM (layout, paint, JS execution) runs on this thread.
  This is why long-running JS blocks rendering.
```

### The Renderer Process's Threads

```
Main thread:
  DOM parsing, CSSOM building, layout, paint, JavaScript execution.
  Handles user input events.
  Most of what you care about happens here.
  SINGLE-THREADED. This is the bottleneck.

Compositor thread:
  Handles compositing of already-painted layers.
  CSS animations that use transform/opacity ONLY run here.
  This is why transform animations are smooth — they don't need the main thread.
  scroll, pinch-zoom also run here.

Raster threads (pool):
  Convert paint instructions into actual pixel bitmaps.
  Multiple threads → parallel rasterization.

Web Workers run in separate threads.
  They cannot touch the DOM.
  They communicate with the main thread via postMessage.
```

---

## 3. Navigation — What Happens When You Type a URL

```
User types: https://paideon.lk/login and presses Enter

1. Browser process handles the input.
   Is this a URL or a search query?
   URL: has scheme, hostname structure.
   Search: everything else → goes to default search engine.

2. Network process: starts DNS lookup, TCP+TLS.
   (See Part 01 and Part 03)

3. HTTP request sent.
   Server responds.

4. Browser examines response headers:
   Content-Type: text/html → it's a web page → continue navigation
   Content-Type: application/pdf → trigger download or PDF viewer
   Content-Type: application/octet-stream → trigger download

5. Safety checks:
   Check URL against Google Safe Browsing list (locally cached).
   Check certificate validity (ongoing from TLS handshake).
   If any check fails → navigation stops, warning shown.

6. Renderer process prepared:
   If same site as current tab → can reuse renderer process.
   If different site (cross-origin) → new renderer process (process isolation).

7. HTML bytes streamed to renderer process.
   Parsing begins BEFORE the full HTML is received.
   Progressive rendering — browser doesn't wait for entire page.
```

---

## 4. HTML Parsing — Building the DOM

HTML parsing is complex. HTML is intentionally fault-tolerant. The parser handles malformed HTML gracefully.

```
Input: stream of bytes
Output: DOM (Document Object Model) tree

Process:
  Bytes → Characters → Tokens → Nodes → DOM tree

Example input:
  <html>
    <body>
      <h1>Welcome</h1>
      <p>Hello <strong>Ashan</strong></p>
    </body>
  </html>

DOM tree:
  Document
  └── html
      └── body
          ├── h1
          │   └── TextNode: "Welcome"
          └── p
              ├── TextNode: "Hello "
              └── strong
                  └── TextNode: "Ashan"
```

### Parsing Interruptions

```
Normally: parser reads HTML → builds DOM nodes → next line.

<script src="app.js"></script> encountered:
  Parser STOPS.
  Browser fetches app.js (if not already downloaded).
  V8 executes app.js completely.
  Parser resumes from where it left off.

Why? JavaScript can call document.write() which injects HTML.
That HTML needs to be parsed. The parser doesn't know if JS will do this.
So it must stop and run JS before continuing.

This is "parser blocking." Heavy synchronous JS in <head> = slow page load.

Exceptions:
  <script defer> → downloaded in parallel, executed AFTER full HTML parsed.
  <script async> → downloaded in parallel, executed immediately when ready.
  <script type="module"> → deferred by default.

<link rel="stylesheet" href="styles.css">:
  CSS doesn't block HTML parsing.
  But CSS DOES block JavaScript execution.
  If JS might read computed styles, it must wait for CSS.
  CSS → JS → parser blocked chain is the real killer.

The preload scanner:
  While the main parser is blocked by a script:
  A secondary "preload scanner" continues reading ahead in the HTML.
  Finds <link>, <script>, <img> → starts downloading them immediately.
  When the main parser unblocks: resources may already be cached.
  This is why putting scripts at the bottom of <body> isn't always necessary.
```

---

## 5. CSS Parsing — Building the CSSOM

```
Input: CSS text
Output: CSSOM (CSS Object Model) — a tree parallel to the DOM

.student-card {
  background: white;
  border-radius: 8px;
  padding: 16px;
}

.student-card .name {
  font-size: 1.125rem;
  font-weight: 600;
}

CSSOM:
  CSSStyleSheet
  └── CSSRule: .student-card
  │     background: white
  │     border-radius: 8px
  │     padding: 16px
  └── CSSRule: .student-card .name
        font-size: 1.125rem
        font-weight: 600

CSS is render-blocking:
  The browser must fully build the CSSOM before it can render anything.
  Reason: any rule could affect any element.
  Browser can't paint until it knows all styles.
  
  This is why reducing CSS size / complexity matters.
  Critical CSS (inline) → small styles for above-the-fold content.
  Load the rest asynchronously.

Specificity:
  When multiple rules apply to one element, specificity determines which wins.
  ID selectors (#id) > Class selectors (.class) > Element selectors (p)
  Inline styles > all stylesheet rules
  !important > inline styles (use sparingly)
  
  Browser computes the final "cascade" → each element gets its computed style.
```

---

## 6. The Render Pipeline — From Trees to Pixels

```
DOM + CSSOM → Style → Layout → Paint → Composite

STYLE (Recalculate Styles):
  Combine DOM tree + CSSOM.
  For each DOM node: determine its computed style.
  Result: each node knows its color, font, display type, etc.

LAYOUT (Reflow):
  Calculate the position and size of every element.
  Box model: margin, border, padding, content dimensions.
  Affected by: display, width, height, flexbox, grid, position.
  
  This is expensive. Changing anything that affects element size
  or position triggers a full or partial layout recalculation.
  
  Layout thrashing:
    Reading layout properties (offsetWidth, getBoundingClientRect)
    FORCES the browser to run layout synchronously (to return accurate values).
    
    WRONG (layout thrash — read-write-read-write):
      for (let el of elements) {
        const width = el.offsetWidth;          // forces layout
        el.style.width = (width * 2) + 'px';  // invalidates layout
      }
    
    RIGHT (batch reads then writes):
      const widths = elements.map(el => el.offsetWidth);  // one layout
      elements.forEach((el, i) => {
        el.style.width = (widths[i] * 2) + 'px';          // batch writes
      });

PAINT:
  Determine what to draw for each element, in which order.
  Fills in pixels: text, colors, borders, shadows, images.
  
  Paint does NOT mean writing pixels to screen yet.
  It creates paint records: "draw a rectangle at x,y with color blue".
  
  What triggers repaint:
    Changing color, background, visibility.
    Does NOT require re-layout (if size/position unchanged).

COMPOSITE:
  Browser may split the page into layers.
  Each layer is painted independently.
  Layers are combined (composited) by the GPU.
  
  Some properties create their own compositor layer:
    transform, opacity, will-change, position:fixed, video elements.
  
  Compositing is cheap and runs on the GPU thread.
  Animating transform or opacity = compositor thread only = smooth 60fps.
  Animating width or left = triggers layout → compositor thread involved → janky.
  
  This is why CSS animations use transform instead of width:
    WRONG:  { width: 100px } → { width: 200px }  → triggers layout every frame
    RIGHT:  { transform: scaleX(1) } → { transform: scaleX(2) }  → GPU only
```

---

## 7. The JavaScript Engine — How V8 Works

V8 (used in Chrome and Node.js) is a just-in-time (JIT) compiler. It doesn't interpret JavaScript line by line — it compiles it to native machine code.

```
Your JS code
    ↓
Parse → AST (Abstract Syntax Tree)
    ↓
Ignition (interpreter)
  → Generates bytecode
  → Runs immediately (fast startup)
  → Collects profiling data as it runs:
    "This function is called 10,000 times"
    ↓
TurboFan (optimizing compiler)
  → For "hot" functions (called many times):
  → Compiles bytecode to native machine code
  → Assumptions: "this argument is always an integer"
  → Highly optimized native code
  → Runs ~10-100× faster than bytecode
  
But:
  If an assumption breaks: "this argument is a string now"
  → Deoptimization: throw away compiled code, fall back to bytecode
  → Recompile with new assumptions

For performance:
  Consistent types in functions → V8 can make stable assumptions.
  TypeScript helps with this (but type erasure means runtime types still vary).
  Don't change object shapes: { x: 1 } then { x: 1, y: 2 } (adds property)
  → V8 creates new "hidden class" → deoptimization risk.
  Always initialize object properties in the same order.
```

---

## 8. The Event Loop — How Async Works in the Browser

JavaScript is single-threaded. Yet it can do multiple things "at once." This is the event loop.

```
COMPONENTS:

Call Stack:
  Where functions execute. Last In, First Out (stack).
  When stack is empty: browser can paint, handle events.
  When stack is full: browser is blocked.

Web APIs:
  Provided by browser, not JS engine.
  setTimeout, fetch, addEventListener, requestAnimationFrame...
  These run OUTSIDE the call stack (in browser's C++ code).
  When done: they place a callback in a queue.

Task Queue (Macrotask Queue):
  Callbacks from setTimeout, setInterval, fetch response handlers.
  One task is picked and run per event loop iteration.

Microtask Queue:
  Callbacks from Promise.then(), queueMicrotask(), async/await.
  ALL microtasks are drained after every task, before the next task.
  Microtasks can enqueue more microtasks (careful: infinite loop risk).

Animation Frame Queue:
  Callbacks from requestAnimationFrame().
  Run before every paint, after microtasks.
  Use for: anything that needs to sync with browser painting.

Event Loop algorithm:
  1. Pick one task from Task Queue (if any).
  2. Execute it (call stack runs to completion).
  3. Drain Microtask Queue (run ALL microtasks, including newly added ones).
  4. Run Animation Frame Queue callbacks (if paint is due).
  5. Browser may repaint.
  6. Go to step 1.
```

### Concrete Example

```javascript
console.log('1');                // stack: runs immediately

setTimeout(() => {
  console.log('2');              // task queue: runs later
}, 0);

Promise.resolve()
  .then(() => console.log('3')) // microtask queue: runs before setTimeout
  .then(() => console.log('4')); // microtask queue: also runs before setTimeout

console.log('5');                // stack: runs immediately

// Output: 1, 5, 3, 4, 2

// Explanation:
// '1' → call stack
// setTimeout callback → task queue
// Promise.then('3') → microtask queue
// '5' → call stack
// Stack empty → drain microtasks: '3', then '4'
// Stack empty, microtasks empty → pick next task: setTimeout → '2'
```

### Why This Matters for React

```javascript
// React batches state updates in event handlers:
function handleClick() {
  setCount(c => c + 1);   // doesn't re-render immediately
  setName('Ashan');       // doesn't re-render immediately
}
// Both updates batched → single re-render after handler completes.

// React 18 automatic batching even in async:
setTimeout(() => {
  setCount(c => c + 1);   // batched (React 18)
  setName('Ashan');       // batched (React 18)
  // single re-render
}, 1000);
```

### requestAnimationFrame

```javascript
// Wrong: animating with setInterval (not synced with paint)
setInterval(() => {
  el.style.left = (x += 1) + 'px';
}, 16);   // "16ms = 60fps" but timer is imprecise, may cause jank

// Right: requestAnimationFrame (runs right before browser paints)
function animate(timestamp) {
  el.style.left = (x += 1) + 'px';
  requestAnimationFrame(animate);  // schedule next frame
}
requestAnimationFrame(animate);
// Browser calls this right before painting → perfectly smooth
```

---

## 9. How fetch() Works Under the Hood

```javascript
fetch('https://api.paideon.lk/api/students', {
  headers: { Authorization: `Bearer ${token}` }
})
.then(response => response.json())
.then(data => console.log(data));
```

**What actually happens:**

```
1. fetch() is called on the main thread.
   Main thread does NOT make the HTTP request.
   
2. fetch() passes the request to the browser's Network Process.
   (Separate process — doesn't affect main thread.)
   
   Main thread continues executing other code immediately.
   This is why fetch is non-blocking.

3. Network process:
   DNS lookup (or cache hit).
   TCP connection (or reuse existing keep-alive connection).
   TLS (or reuse existing session).
   Send HTTP request.
   Receive response headers.
   
4. Response headers received:
   A task is placed in the Task Queue: "fetch resolved with response object"

5. Event loop picks the task:
   .then(response => ...) callback runs on main thread.
   response.json() is called → starts reading response BODY.
   response.json() is also async (body may not be fully received yet).

6. Body fully received:
   Another task in Task Queue: ".json() resolved with parsed data"

7. Event loop picks it:
   .then(data => ...) runs.
   console.log(data) executes.

The entire HTTP request happened in a separate process.
The main thread was free the entire time.
This is how async/await works — not parallelism but non-blocking waiting.
```

---

## 10. React Rendering in the Browser

```
React rendering ≠ browser rendering.
React has its own "virtual" render cycle, separate from browser paint.

React RENDER phase:
  React calls your component functions.
  Builds a new virtual DOM tree (plain JS objects).
  Diffs against previous virtual DOM tree (reconciliation).
  Identifies what changed.
  
  This runs entirely in JavaScript memory. No browser painting yet.

React COMMIT phase:
  React applies the diffs to the real DOM.
  Calls useLayoutEffect (synchronous, after DOM update, before paint).
  Browser paint happens.
  Calls useEffect (asynchronous, after paint).

React 18 Concurrent Mode:
  Render phase can be interrupted.
  High-priority updates (user input) can preempt low-priority renders.
  
  Example:
    Slow list re-renders 1000 items.
    User types in a search box.
    React pauses the list render, handles the keypress, resumes list render.
    Result: input stays responsive even during heavy computation.

React Fiber:
  React's reconciliation algorithm.
  Each component is a "fiber" — a unit of work.
  React can pause and resume fiber work.
  This is what enables concurrent features.

useEffect timing:
  Component renders → DOM updated → browser paints → useEffect runs.
  
  Effect runs AFTER the browser has painted.
  This is why data fetching in useEffect causes a loading flash:
  1. Render (with no data) → paint (shows spinner/empty state)
  2. useEffect runs → fetch → data arrives → re-render → paint (shows data)

useLayoutEffect:
  Component renders → DOM updated → useLayoutEffect runs → browser paints.
  
  Runs synchronously BEFORE browser paint.
  Use when you need to read DOM dimensions before paint (to prevent flicker).
  Avoid heavy computation here — delays painting.
```

---

## 11. Service Workers — The Browser's Proxy

A Service Worker is a JavaScript file that runs in its own thread, acts as a programmable proxy between your app and the network.

```
Without service worker:
  Page → Network request → Server response → Page

With service worker:
  Page → Service worker intercepts → [serve from cache OR pass to network] → Page

The service worker can:
  → Serve content from cache (offline capability)
  → Intercept and modify requests
  → Pre-cache resources (install them before they're needed)
  → Receive push notifications (even when page is closed)
  → Background sync (send data when connection restored)

Service worker lifecycle:
  1. Registration:
     navigator.serviceWorker.register('/sw.js');
  
  2. Install event (one-time):
     Pre-cache critical resources.
     
  3. Activate event:
     Clean up old caches from previous versions.
  
  4. Fetch event (every request):
     Intercept requests, decide: cache or network.

Service workers are scope-restricted:
  A sw.js at /sw.js controls all pages at /.
  A sw.js at /app/sw.js only controls pages under /app/.

Paideon potential use:
  Cache the app shell (HTML, CSS, JS) for offline support.
  School computers may have unreliable connections.
  A PWA with service worker → app loads instantly, works offline (read-only).
  Sync changes when connection restored.
```

---

_Next: [Part 09 — URLs, URIs & the Origin Model](./Part%2009%20-%20URLs%2C%20URIs%20%26%20the%20Origin%20Model.md)_