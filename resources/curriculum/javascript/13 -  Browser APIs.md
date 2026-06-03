The browser exposes a large set of APIs beyond the DOM — storage, navigation, clipboard, network status, observers, workers, and more. These are the APIs you reach for when building real web applications.

---

## Table of Contents

1. [Web Storage — localStorage and sessionStorage](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#1-web-storage--localstorage-and-sessionstorage)
2. [Cookies](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#2-cookies)
3. [The History API](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#3-the-history-api)
4. [The URL API](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#4-the-url-api)
5. [The Clipboard API](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#5-the-clipboard-api)
6. [The Fetch API — Advanced](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#6-the-fetch-api--advanced)
7. [Web Workers](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#7-web-workers)
8. [Broadcast Channel](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#8-broadcast-channel)
9. [Performance API](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#9-performance-api)
10. [Network Information and Online Status](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#10-network-information-and-online-status)
11. [Geolocation](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#11-geolocation)
12. [Notifications API](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#12-notifications-api)
13. [ResizeObserver and IntersectionObserver](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#13-resizeobserver-and-intersectionobserver)

---

## 1. Web Storage — localStorage and sessionStorage

Both store key-value pairs as strings. The difference is lifetime:

||`localStorage`|`sessionStorage`|
|---|---|---|
|Lifetime|Persists until cleared|Cleared when tab/window closes|
|Scope|All tabs on the same origin|Only the current tab|
|Capacity|~5MB|~5MB|

```javascript
// localStorage — persists across sessions
localStorage.setItem('theme', 'dark');
localStorage.getItem('theme');          // 'dark'
localStorage.removeItem('theme');
localStorage.clear();                   // remove everything
localStorage.length;                    // number of entries
localStorage.key(0);                    // key at index 0

// sessionStorage — same API, different lifetime
sessionStorage.setItem('draft', 'in progress');
sessionStorage.getItem('draft');
```

### Storing Objects

Web Storage only stores strings. Use JSON for objects:

```javascript
// Store
const user = { id: 'abc', name: 'Ashan', role: 'admin' };
localStorage.setItem('user', JSON.stringify(user));

// Retrieve
const stored = localStorage.getItem('user');
const user = stored ? JSON.parse(stored) : null;
```

### Storage Event

When `localStorage` changes in one tab, other tabs on the same origin receive a `storage` event:

```javascript
window.addEventListener('storage', (event) => {
  console.log(event.key);        // which key changed
  console.log(event.oldValue);   // previous value
  console.log(event.newValue);   // new value
  console.log(event.url);        // URL of the tab that changed it
  console.log(event.storageArea); // localStorage or sessionStorage
});
```

The `storage` event does NOT fire in the tab that made the change — only in other tabs.

### When NOT to Use Web Storage

```javascript
// NEVER store tokens in localStorage — XSS can steal them
localStorage.setItem('accessToken', token);   // vulnerable to XSS

// Store session tokens in HTTP-only cookies instead
// HTTP-only cookies are not accessible from JavaScript at all
```

---

## 2. Cookies

Cookies are stored by the browser and sent automatically with every HTTP request to the matching domain.

```javascript
// Set a cookie
document.cookie = 'theme=dark; max-age=2592000; path=/; SameSite=Lax';

// Read cookies — returns all as one string
document.cookie   // 'theme=dark; username=ashan'

// Parse cookies
function getCookie(name) {
  return document.cookie
    .split('; ')
    .find(row => row.startsWith(name + '='))
    ?.split('=')[1];
}

// Delete a cookie — set max-age to 0
document.cookie = 'theme=; max-age=0; path=/';
```

### Cookie Attributes

|Attribute|Purpose|
|---|---|
|`max-age=N`|Expire after N seconds|
|`expires=date`|Expire at a specific date|
|`path=/`|Available on all paths (default is current path)|
|`domain=.example.com`|Available on subdomains|
|`Secure`|Only sent over HTTPS|
|`HttpOnly`|Not accessible from JavaScript (set server-side only)|
|`SameSite=Strict`|Never sent on cross-site requests|
|`SameSite=Lax`|Sent on same-site + top-level navigations|
|`SameSite=None; Secure`|Sent on all requests (requires Secure)|

`HttpOnly` and `Secure` cookies can only be set by the server. For auth tokens, always use server-set `HttpOnly; Secure; SameSite=Lax` cookies — never `document.cookie`.

---

## 3. The History API

Manipulate browser navigation history without page reloads — the foundation of client-side routing in SPAs:

```javascript
// Push a new URL onto the history stack (adds a new history entry)
history.pushState({ page: 'about' }, '', '/about');

// Replace the current history entry (no new entry added)
history.replaceState({ page: 'home' }, '', '/');

// Navigate history
history.back();        // same as clicking browser back
history.forward();     // same as clicking browser forward
history.go(-2);        // go back 2 steps
history.go(1);         // go forward 1 step

// Current history stack size
history.length;

// State object for current entry
history.state;         // { page: 'about' }
```

### `popstate` Event

Fires when the user navigates via browser back/forward, or when `history.go()` is called. Does NOT fire on `pushState` or `replaceState`:

```javascript
window.addEventListener('popstate', (event) => {
  console.log(event.state);    // the state object passed to pushState
  console.log(location.pathname); // current URL path
  // Re-render the appropriate content for this URL
  renderRoute(location.pathname);
});
```

### Simple Client-Side Router Pattern

```javascript
function navigate(path, state = {}) {
  history.pushState(state, '', path);
  renderRoute(path);
}

window.addEventListener('popstate', () => {
  renderRoute(location.pathname);
});

function renderRoute(path) {
  const routes = {
    '/':        HomePage,
    '/about':   AboutPage,
    '/courses': CoursesPage,
  };
  const Component = routes[path] ?? NotFoundPage;
  document.getElementById('app').innerHTML = Component();
}
```

---

## 4. The URL API

Parse, construct, and manipulate URLs without string concatenation:

```javascript
// Parse a URL
const url = new URL('https://api.paideon.lk:8443/v1/students?class=10A&sort=name#top');

url.protocol    // 'https:'
url.hostname    // 'api.paideon.lk'
url.port        // '8443'
url.host        // 'api.paideon.lk:8443'
url.pathname    // '/v1/students'
url.search      // '?class=10A&sort=name'
url.hash        // '#top'
url.origin      // 'https://api.paideon.lk:8443'
url.href        // full URL string

// Search params
url.searchParams.get('class')        // '10A'
url.searchParams.has('sort')         // true
url.searchParams.getAll('ids')       // array if key repeated
url.searchParams.set('page', '2');
url.searchParams.append('ids', '123');
url.searchParams.delete('sort');
url.searchParams.toString()          // 'class=10A&page=2&ids=123'

// Construct from parts
const api = new URL('/v1/students', 'https://api.paideon.lk');
api.searchParams.set('class', '10A');
api.href   // 'https://api.paideon.lk/v1/students?class=10A'

// Build query string
const params = new URLSearchParams({ class: '10A', sort: 'name', page: '1' });
params.toString()   // 'class=10A&sort=name&page=1'

// Iterate params
for (const [key, value] of url.searchParams) {
  console.log(key, value);
}
```

Never build URLs with string concatenation. `URL` handles encoding automatically.

---

## 5. The Clipboard API

Read and write the system clipboard:

```javascript
// Write text to clipboard
async function copyToClipboard(text) {
  try {
    await navigator.clipboard.writeText(text);
    console.log('Copied');
  } catch (err) {
    console.error('Clipboard write failed:', err);
  }
}

// Read text from clipboard
async function readFromClipboard() {
  try {
    const text = await navigator.clipboard.readText();
    return text;
  } catch (err) {
    console.error('Clipboard read failed:', err);
    return null;
  }
}

// Write rich content (HTML, images)
await navigator.clipboard.write([
  new ClipboardItem({
    'text/plain': new Blob(['plain text'], { type: 'text/plain' }),
    'text/html':  new Blob(['<b>bold</b>'], { type: 'text/html' }),
  })
]);
```

`readText()` requires user permission (browser prompts on first use). `writeText()` works in response to a user gesture without a separate permission prompt.

---

## 6. The Fetch API — Advanced

Beyond basic GET requests — headers, streaming, cancellation:

```javascript
// POST with JSON body
const response = await fetch('/api/users', {
  method:  'POST',
  headers: { 'Content-Type': 'application/json' },
  body:    JSON.stringify({ name: 'Ashan', email: 'a@b.com' }),
});

const data = await response.json();

// Common response methods
response.status        // 200, 404, 500...
response.ok            // true if status 200-299
response.headers.get('Content-Type')
await response.json()  // parse as JSON
await response.text()  // parse as plain text
await response.blob()  // parse as binary Blob
await response.arrayBuffer()

// Abort a request
const controller = new AbortController();
const timeout = setTimeout(() => controller.abort(), 5000);

try {
  const response = await fetch('/api/data', { signal: controller.signal });
  clearTimeout(timeout);
  return await response.json();
} catch (err) {
  if (err.name === 'AbortError') {
    console.log('Request timed out');
  }
}

// Streaming a large response (e.g. server-sent AI output)
const response = await fetch('/api/stream');
const reader = response.body.getReader();
const decoder = new TextDecoder();

while (true) {
  const { done, value } = await reader.read();
  if (done) break;
  process(decoder.decode(value, { stream: true }));
}
```

---

## 7. Web Workers

Move expensive JavaScript off the main thread. Workers run in a separate thread — no DOM access, but full JS environment and network access:

```javascript
// main.js — create and communicate with a worker
const worker = new Worker('/workers/heavy-task.js');

// Send data to the worker
worker.postMessage({ numbers: [1, 2, 3, 4, 5] });

// Receive results
worker.addEventListener('message', (event) => {
  console.log('Result:', event.data);
});

worker.addEventListener('error', (event) => {
  console.error('Worker error:', event.message);
});

// Terminate the worker when done
worker.terminate();
```

```javascript
// workers/heavy-task.js — the worker file
self.addEventListener('message', (event) => {
  const { numbers } = event.data;

  // Expensive computation — does NOT block the main thread
  const result = numbers.reduce((sum, n) => sum + n * n, 0);

  // Send result back to main thread
  self.postMessage(result);
});
```

### Transferable Objects

Large data (ArrayBuffers) can be transferred without copying — the original loses ownership:

```javascript
const buffer = new ArrayBuffer(1024 * 1024 * 10); // 10MB
worker.postMessage({ buffer }, [buffer]);  // transfer, not copy
// buffer is now unusable in main thread — zero-copy transfer
```

### Use Cases

- Image/video processing
- Cryptography
- Heavy data parsing (large CSVs, JSON)
- Physics simulations
- Anything that would freeze the UI for > 16ms

---

## 8. Broadcast Channel

Send messages between all tabs/windows/workers of the same origin:

```javascript
// In every tab
const channel = new BroadcastChannel('auth');

// Send a message to all OTHER tabs
channel.postMessage({ type: 'LOGOUT' });

// Receive messages from other tabs
channel.addEventListener('message', (event) => {
  if (event.data.type === 'LOGOUT') {
    // Log this tab out too
    clearSession();
    window.location.href = '/login';
  }
});

// Close when done
channel.close();
```

Use cases: sync logout across tabs, theme changes, cart updates, real-time collaboration state.

---

## 9. Performance API

Measure execution time with high-resolution timestamps:

```javascript
// High-resolution timestamp (microseconds)
const start = performance.now();
heavyOperation();
const end = performance.now();
console.log(`Took ${end - start}ms`);

// Named marks and measures
performance.mark('fetchStart');
const data = await fetchData();
performance.mark('fetchEnd');
performance.measure('fetchDuration', 'fetchStart', 'fetchEnd');

const [measure] = performance.getEntriesByName('fetchDuration');
console.log(measure.duration);   // ms

// View all performance entries
performance.getEntries();
performance.getEntriesByType('navigation');
performance.getEntriesByType('resource');  // all resource loads

// Navigation timing
const nav = performance.getEntriesByType('navigation')[0];
nav.domContentLoadedEventEnd - nav.startTime   // DOMContentLoaded time
nav.loadEventEnd - nav.startTime               // full page load time
```

---

## 10. Network Information and Online Status

```javascript
// Online / offline status
navigator.onLine   // true or false

window.addEventListener('online',  () => console.log('Back online'));
window.addEventListener('offline', () => console.log('Gone offline'));

// Network Information API (limited browser support)
const conn = navigator.connection;
conn?.effectiveType    // '4g', '3g', '2g', 'slow-2g'
conn?.downlink         // estimated bandwidth in Mbps
conn?.rtt              // round-trip time in ms
conn?.saveData         // user has enabled data saver

navigator.connection?.addEventListener('change', () => {
  if (navigator.connection.saveData) {
    // Load lower-quality assets
  }
});
```

---

## 11. Geolocation

```javascript
// One-time position
navigator.geolocation.getCurrentPosition(
  (position) => {
    const { latitude, longitude, accuracy } = position.coords;
    console.log(latitude, longitude);
  },
  (error) => {
    // error.code: 1=denied, 2=unavailable, 3=timeout
    console.error(error.message);
  },
  {
    enableHighAccuracy: true,   // GPS if available (slower, more battery)
    timeout: 5000,              // fail if no position within 5s
    maximumAge: 60000,          // accept cached position up to 1 min old
  }
);

// Watch position (continuous updates)
const watchId = navigator.geolocation.watchPosition(
  (position) => updateMap(position.coords),
  (error)    => handleError(error)
);

// Stop watching
navigator.geolocation.clearWatch(watchId);
```

Requires user permission. HTTPS required in production.

---

## 12. Notifications API

```javascript
// Request permission
const permission = await Notification.requestPermission();
// 'granted', 'denied', 'default'

// Show a notification
if (Notification.permission === 'granted') {
  const notification = new Notification('New message', {
    body:  'Ashan sent you a message',
    icon:  '/icons/message.png',
    badge: '/icons/badge.png',
    tag:   'new-message',         // replace previous notification with same tag
    data:  { url: '/messages/123' },
  });

  notification.addEventListener('click', () => {
    window.open(notification.data.url);
    notification.close();
  });

  // Auto-close after 5 seconds
  setTimeout(() => notification.close(), 5000);
}
```

---

## 13. ResizeObserver and IntersectionObserver

### ResizeObserver

Fires when an element's size changes:

```javascript
const ro = new ResizeObserver((entries) => {
  for (const entry of entries) {
    const { width, height } = entry.contentRect;
    console.log('Size changed:', width, height);
  }
});

ro.observe(document.querySelector('.card'));
ro.unobserve(element);
ro.disconnect();   // stop observing everything
```

Use for responsive components that react to their own container size rather than the viewport.

### IntersectionObserver

Fires when an element enters or leaves the viewport:

```javascript
const io = new IntersectionObserver(
  (entries) => {
    for (const entry of entries) {
      if (entry.isIntersecting) {
        entry.target.classList.add('visible');
        io.unobserve(entry.target);   // observe once
      }
    }
  },
  {
    root:       null,    // null = viewport
    rootMargin: '0px',   // expand/shrink the root box
    threshold:  0.1,     // fire when 10% visible
  }
);

document.querySelectorAll('.fade-in').forEach(el => io.observe(el));
```

Use cases: lazy-loading images, triggering entrance animations, infinite scroll, ad impression tracking.

```javascript
// Lazy-load images
const lazyImages = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const img = entry.target;
      img.src = img.dataset.src;      // move data-src to src
      lazyImages.unobserve(img);
    }
  });
});

document.querySelectorAll('img[data-src]').forEach(img => lazyImages.observe(img));
```

---

_Next: [Modules In Depth](https://claude.ai/chat/14%20-%20Modules%20In%20Depth.md)_