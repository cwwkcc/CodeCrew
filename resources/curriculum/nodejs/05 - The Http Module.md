> Before Express, NestJS, or any framework, there is Node's built-in `http` module. It is what every framework is built on top of. Understanding how it works gives you a mental model of what abstractions like Express are actually doing — and what happens when something goes wrong at the framework level.

---

## Table of Contents

1. [How Node.js HTTP Works]
2. [Creating a Basic Server]
3. [The Request Object]
4. [Reading the Request Body]
5. [The Response Object]
6. [Routing by Hand]
7. [Serving Static Files]
8. [Making HTTP Requests — http.request]
9. [HTTPS — The https Module]
10. [What Express Actually Adds]

---

## 1. How Node.js HTTP Works

Node's `http` module wraps libuv's TCP socket handling and the built-in HTTP parser. When a request arrives:

```
1. OS accepts the TCP connection
2. Node receives data on the socket
3. The HTTP parser reads the incoming bytes and constructs:
     - IncomingMessage (req) — parsed headers, method, URL
     - The body remains as a Readable stream — not parsed
4. Node calls your request listener with (req, res)
5. Your code reads the body (if needed), writes the response
6. res.end() flushes the response and closes or reuses the connection
```

The key insight: Node.js does not buffer the request body. You must consume it as a stream. This is why frameworks like Express have `express.json()` middleware — it reads and parses the body for you.

---

## 2. Creating a Basic Server

```javascript
const http = require("node:http");

const server = http.createServer((req, res) => {
  // req: http.IncomingMessage — the incoming request
  // res: http.ServerResponse  — the outgoing response

  res.writeHead(200, { "Content-Type": "text/plain" });
  res.end("Hello World\n");
});

server.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});
```

### server.listen Options

```javascript
// Listen on a port (all interfaces)
server.listen(3000);

// Listen on a specific host
server.listen(3000, "127.0.0.1"); // localhost only

// With callback (fires when server is ready)
server.listen(3000, () => {
  console.log("Ready");
});

// Use port 0 to let the OS assign a free port (useful in tests)
server.listen(0, () => {
  console.log("Listening on port", server.address().port);
});
```

### Graceful Shutdown

```javascript
process.on("SIGTERM", () => {
  server.close(() => {
    console.log("Server closed. Process exiting.");
    process.exit(0);
  });
});
```

---

## 3. The Request Object

`req` is an instance of `http.IncomingMessage`, which extends Readable.

```javascript
const server = http.createServer((req, res) => {
  console.log(req.method); // 'GET', 'POST', 'PUT', 'DELETE', etc.
  console.log(req.url); // '/users?page=2' — path + query string, no host
  console.log(req.headers); // object of all headers, keys lowercased

  // Specific headers
  console.log(req.headers["content-type"]); // 'application/json'
  console.log(req.headers["authorization"]); // 'Bearer abc123'
  console.log(req.headers["host"]); // 'localhost:3000'

  // HTTP version
  console.log(req.httpVersion); // '1.1'

  // Remote address
  console.log(req.socket.remoteAddress); // '127.0.0.1'
});
```

### Parsing the URL

`req.url` is just the path and query string — no protocol or host. Use `URL` to parse it:

```javascript
const { URL } = require("node:url");

const server = http.createServer((req, res) => {
  // Need the base to parse relative URLs
  const url = new URL(req.url, `http://${req.headers.host}`);

  console.log(url.pathname); // '/users'
  console.log(url.searchParams.get("page")); // '2'
});
```

---

## 4. Reading the Request Body

The body is a Readable stream. You must collect the chunks manually (or use a framework that does it for you).

### Collecting the Body

```javascript
async function readBody(req) {
  const chunks = [];
  for await (const chunk of req) {
    chunks.push(chunk);
  }
  return Buffer.concat(chunks).toString("utf8");
}

const server = http.createServer(async (req, res) => {
  if (req.method === "POST") {
    const body = await readBody(req);
    console.log("Body:", body);
  }
  res.end("OK");
});
```

### Parsing JSON Bodies

```javascript
async function readJSON(req) {
  const raw = await readBody(req);
  try {
    return JSON.parse(raw);
  } catch {
    return null;
  }
}

const server = http.createServer(async (req, res) => {
  if (
    req.method === "POST" &&
    req.headers["content-type"] === "application/json"
  ) {
    const data = await readJSON(req);
    if (!data) {
      res.writeHead(400, { "Content-Type": "application/json" });
      res.end(JSON.stringify({ error: "Invalid JSON" }));
      return;
    }
    console.log("Received:", data);
  }
  res.end("OK");
});
```

### Limiting Body Size

Always enforce a size limit. Without one, a client can send a request body of any size and exhaust your server's memory.

```javascript
async function readBody(req, maxBytes = 1024 * 1024) {
  // 1MB default
  const chunks = [];
  let totalBytes = 0;

  for await (const chunk of req) {
    totalBytes += chunk.length;
    if (totalBytes > maxBytes) {
      throw Object.assign(new Error("Payload too large"), {
        code: "PAYLOAD_TOO_LARGE",
      });
    }
    chunks.push(chunk);
  }

  return Buffer.concat(chunks).toString("utf8");
}
```

---

## 5. The Response Object

`res` is an instance of `http.ServerResponse`, which extends Writable.

### Setting Status and Headers

```javascript
// writeHead — set status code and multiple headers at once
res.writeHead(201, {
  "Content-Type": "application/json",
  Location: "/users/42",
});

// Alternatively, set them separately
res.statusCode = 201;
res.setHeader("Content-Type", "application/json");
res.setHeader("X-Request-Id", requestId);

// Remove a header
res.removeHeader("X-Powered-By");

// Check if headers have been sent
console.log(res.headersSent); // false until res.write() or res.end() is called
```

### Writing the Body

```javascript
// end() — write final chunk and finish the response
res.end("Hello"); // string
res.end(Buffer.from("Hello")); // Buffer
res.end(); // no body — for 204 No Content

// write() — write a chunk without finishing (for streaming responses)
res.write("chunk 1");
res.write("chunk 2");
res.end("final chunk");
```

### Sending JSON

```javascript
function sendJSON(res, statusCode, data) {
  const body = JSON.stringify(data);
  res.writeHead(statusCode, {
    "Content-Type": "application/json",
    "Content-Length": Buffer.byteLength(body), // byte count, not char count
  });
  res.end(body);
}

// Usage
sendJSON(res, 200, { users: [], total: 0 });
sendJSON(res, 404, { error: "Not found" });
```

---

## 6. Routing by Hand

Without a framework, routing is a manual switch on `req.method` and `req.url`.

```javascript
const http = require("node:http");
const { URL } = require("node:url");

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, `http://${req.headers.host}`);
  const method = req.method;
  const path = url.pathname;

  // GET /
  if (method === "GET" && path === "/") {
    return sendJSON(res, 200, { message: "Welcome" });
  }

  // GET /users
  if (method === "GET" && path === "/users") {
    const page = Number(url.searchParams.get("page") ?? 1);
    return sendJSON(res, 200, { users: [], page });
  }

  // POST /users
  if (method === "POST" && path === "/users") {
    const body = await readJSON(req);
    if (!body) return sendJSON(res, 400, { error: "Invalid JSON" });
    // create user...
    return sendJSON(res, 201, { id: 1, ...body });
  }

  // GET /users/:id — simple dynamic segment
  const userMatch = path.match(/^\/users\/(\d+)$/);
  if (method === "GET" && userMatch) {
    const id = Number(userMatch[1]);
    return sendJSON(res, 200, { id });
  }

  // 404 fallback
  sendJSON(res, 404, { error: "Not found" });
});
```

This is exactly the kind of thing frameworks abstract away. A few routes is manageable. With 50+ routes it becomes unmaintainable — which is why Express exists.

---

## 7. Serving Static Files

```javascript
const http = require("node:http");
const fs = require("node:fs/promises");
const path = require("node:path");

const MIME_TYPES = {
  ".html": "text/html",
  ".css": "text/css",
  ".js": "application/javascript",
  ".json": "application/json",
  ".png": "image/png",
  ".jpg": "image/jpeg",
  ".svg": "image/svg+xml",
  ".ico": "image/x-icon",
};

const PUBLIC_DIR = path.join(__dirname, "public");

const server = http.createServer(async (req, res) => {
  if (req.method !== "GET") {
    res.writeHead(405);
    return res.end();
  }

  // Prevent directory traversal attacks
  const safePath = path.normalize(req.url).replace(/^(\.\.[\/\\])+/, "");
  const filePath = path.join(
    PUBLIC_DIR,
    safePath === "/" ? "index.html" : safePath,
  );

  try {
    const content = await fs.readFile(filePath);
    const ext = path.extname(filePath);
    const mime = MIME_TYPES[ext] ?? "application/octet-stream";

    res.writeHead(200, { "Content-Type": mime });
    res.end(content);
  } catch (err) {
    if (err.code === "ENOENT") {
      res.writeHead(404);
      res.end("Not Found");
    } else {
      res.writeHead(500);
      res.end("Internal Server Error");
    }
  }
});
```

---

## 8. Making HTTP Requests — http.request

Node can also act as an HTTP client using `http.request`.

```javascript
const http = require("node:http");
const https = require("node:https");

// Promisified helper
function httpGet(url) {
  return new Promise((resolve, reject) => {
    const client = url.startsWith("https") ? https : http;

    client
      .get(url, (res) => {
        const chunks = [];
        res.on("data", (chunk) => chunks.push(chunk));
        res.on("end", () => {
          const body = Buffer.concat(chunks).toString("utf8");
          resolve({ statusCode: res.statusCode, body });
        });
        res.on("error", reject);
      })
      .on("error", reject);
  });
}

const { statusCode, body } = await httpGet(
  "https://api.github.com/users/octocat",
);
const user = JSON.parse(body);
```

### POST with a Body

```javascript
function httpPost(url, data) {
  return new Promise((resolve, reject) => {
    const body = JSON.stringify(data);
    const parsed = new URL(url);

    const options = {
      hostname: parsed.hostname,
      port: parsed.port || 443,
      path: parsed.pathname,
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "Content-Length": Buffer.byteLength(body),
      },
    };

    const req = https.request(options, (res) => {
      const chunks = [];
      res.on("data", (chunk) => chunks.push(chunk));
      res.on("end", () =>
        resolve({
          statusCode: res.statusCode,
          body: Buffer.concat(chunks).toString("utf8"),
        }),
      );
    });

    req.on("error", reject);
    req.write(body);
    req.end();
  });
}
```

In practice, use `fetch` (built into Node 18+) or the `undici` package rather than raw `http.request` — far less boilerplate:

```javascript
// Node 18+ — fetch is a global
const res = await fetch("https://api.example.com/users");
const data = await res.json();

// POST
const res2 = await fetch("https://api.example.com/users", {
  method: "POST",
  headers: { "Content-Type": "application/json" },
  body: JSON.stringify({ name: "Alice" }),
});
const created = await res2.json();
```

---

## 9. HTTPS — The https Module

`https` has the same API as `http` but wraps connections in TLS.

```javascript
const https = require("node:https");
const fs = require("node:fs");

// You need a TLS certificate and key
const server = https.createServer(
  {
    key: fs.readFileSync("key.pem"),
    cert: fs.readFileSync("cert.pem"),
  },
  (req, res) => {
    res.writeHead(200);
    res.end("Secure Hello");
  },
);

server.listen(443);
```

In practice, you almost never use `https` directly in Node.js. You run your Node server on a plain HTTP port and put a **reverse proxy** (Nginx, Caddy, Cloudflare) in front of it that handles TLS termination. The proxy decrypts the traffic and forwards plain HTTP to Node.

```
Client ──(HTTPS)──► Nginx/Caddy ──(HTTP)──► Node.js :3000
                    (handles TLS)
```

This is the standard production architecture.

---

## 10. What Express Actually Adds

Express is a thin layer on top of `http`. Understanding what it actually does:

```
Built-in http module:

  const server = http.createServer((req, res) => {
    // req.url is a raw string: '/users?page=2'
    // No body parsing
    // No routing
    // No middleware
    // res has no .json(), .status(), .send()
    if (req.method === 'GET' && req.url.startsWith('/users')) {
      // parse query string manually
      // write response manually
    }
  });

Express:

  const app = express();

  app.use(express.json());  // reads and parses body automatically

  app.get('/users', (req, res) => {
    // req.query   → { page: '2' } (parsed automatically)
    // req.body    → parsed JSON (from express.json() middleware)
    // req.params  → { id: '42' } (from route pattern)
    res.json({ users: [] });  // sets Content-Type, serializes, calls end()
  });
```

Express gives you: routing with parameters, middleware chaining, `req.query`/`req.body`/`req.params`, `res.json()`/`res.status()`/`res.send()`, and error handling middleware.

All of it is implemented in terms of the `http.IncomingMessage` and `http.ServerResponse` objects you've been working with in this part.
