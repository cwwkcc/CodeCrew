> Node.js ships with a standard library of built-in modules — no installation required. This part covers the ones you will use in almost every real project: `fs` for the file system, `path` for working with file paths, `os` for system information, and `url` for parsing and constructing URLs.

---

## Table of Contents

1. [How Built-in Modules Work]
2. [fs — The File System Module]
3. [Reading Files]
4. [Writing Files]
5. [Working with Directories]
6. [File Metadata — stat]
7. [Watching Files]
8. [fs/promises — The Modern API]
9. [path — Working with File Paths]
10. [os — Operating System Information]
11. [url — Parsing and Building URLs]
12. [The querystring / URLSearchParams API]

---

## 1. How Built-in Modules Work

Built-in modules are compiled into the Node.js binary. They do not exist in `node_modules` and do not need to be installed.

```javascript
// CommonJS
const fs = require("fs");
const path = require("path");

// ES Modules
import fs from "fs";
import path from "path";

// Explicit node: prefix (recommended in modern Node.js)
import fs from "node:fs";
import path from "node:path";
```

The `node:` prefix makes it unambiguous that you're importing a built-in, not an npm package with the same name. It's a good habit to use it.

---

## 2. fs — The File System Module

`fs` is the interface between Node.js and the operating system's file system. Every file operation — read, write, delete, rename, watch — goes through it.

`fs` provides three styles of API for the same operations:

```
Callback API     fs.readFile(path, callback)          — original style
Sync API         fs.readFileSync(path)                — blocking, simple
Promises API     fs.promises.readFile(path)           — modern, preferred
                 (also importable as 'fs/promises')
```

In production servers, never use the Sync API — it blocks the event loop for the entire duration of the I/O operation, preventing any other requests from being handled. Use promises or callbacks.

---

## 3. Reading Files

### Read Entire File

```javascript
const fs = require("node:fs/promises");

// As a Buffer (raw bytes)
const buffer = await fs.readFile("data.bin");

// As a string
const text = await fs.readFile("config.json", { encoding: "utf8" });
// or shorthand:
const text2 = await fs.readFile("config.json", "utf8");

// Parse JSON directly
const config = JSON.parse(await fs.readFile("config.json", "utf8"));
```

### Read as a Stream (large files)

```javascript
const fs = require("node:fs");

const stream = fs.createReadStream("large-file.csv", {
  encoding: "utf8",
  highWaterMark: 64 * 1024, // 64KB chunks
});

for await (const chunk of stream) {
  process(chunk);
}
```

Use `createReadStream` for any file over a few MB. `readFile` loads the entire file into memory.

### Check if File Exists Before Reading

```javascript
const fs = require("node:fs/promises");

// Preferred pattern: try to open and handle the error
try {
  const content = await fs.readFile("config.json", "utf8");
} catch (err) {
  if (err.code === "ENOENT") {
    console.log("File not found — using defaults");
  } else {
    throw err; // unexpected error, re-throw
  }
}

// Avoid: fs.access() followed by fs.readFile() — TOCTOU race condition
// Don't check then act. Just act and handle the error.
```

---

## 4. Writing Files

### Write Entire File

```javascript
const fs = require("node:fs/promises");

// Write a string (creates file if not exists, overwrites if it does)
await fs.writeFile("output.txt", "Hello World\n", "utf8");

// Write a Buffer
await fs.writeFile("output.bin", buffer);

// Write JSON
await fs.writeFile("config.json", JSON.stringify(config, null, 2), "utf8");
```

### Append to File

```javascript
// Append — does not overwrite
await fs.appendFile(
  "log.txt",
  `[${new Date().toISOString()}] Event occurred\n`,
  "utf8",
);
```

### Write with Fine-grained Control

```javascript
// Open → write at specific position → close
const handle = await fs.open("file.txt", "r+"); // open for reading and writing
await handle.write("Overwrite this", 0, "utf8"); // write at offset 0
await handle.close();
```

### Write as a Stream (large output)

```javascript
const fs = require("node:fs");

const writable = fs.createWriteStream("output.csv");

for (const row of largeDataset) {
  writable.write(row.join(",") + "\n");
}

writable.end();
await new Promise((resolve, reject) => {
  writable.on("finish", resolve);
  writable.on("error", reject);
});
```

### File Open Flags

```
'r'   Open for reading. Error if file doesn't exist.
'r+'  Open for reading and writing. Error if file doesn't exist.
'w'   Open for writing. Creates file if not exist. Truncates if exists.
'w+'  Open for reading and writing. Creates if not exist. Truncates if exists.
'a'   Open for appending. Creates file if not exist.
'a+'  Open for reading and appending.
'x'   Like 'w', but fails if file already exists (exclusive create).
```

---

## 5. Working with Directories

```javascript
const fs = require("node:fs/promises");

// Create a directory
await fs.mkdir("logs");

// Create recursively (won't fail if parent dirs don't exist)
await fs.mkdir("data/2026/march", { recursive: true });

// Read directory contents
const entries = await fs.readdir("src");
console.log(entries); // ['index.js', 'utils.js', 'types.ts']

// Read with file type info
const entriesWithTypes = await fs.readdir("src", { withFileTypes: true });
for (const entry of entriesWithTypes) {
  if (entry.isDirectory()) console.log("Dir:", entry.name);
  if (entry.isFile()) console.log("File:", entry.name);
}

// Remove a file
await fs.unlink("temp.txt");

// Remove an empty directory
await fs.rmdir("old-logs");

// Remove a directory and all its contents (recursive)
await fs.rm("old-logs", { recursive: true, force: true });

// Rename / move
await fs.rename("old-name.txt", "new-name.txt");
await fs.rename("file.txt", "other-dir/file.txt"); // also works as a move

// Copy a file
await fs.copyFile("source.txt", "dest.txt");
// Fail if dest exists:
await fs.copyFile("source.txt", "dest.txt", fs.constants.COPYFILE_EXCL);
```

---

## 6. File Metadata — stat

`stat` returns metadata about a file or directory without reading its contents.

```javascript
const fs = require("node:fs/promises");

const stats = await fs.stat("file.txt");

console.log(stats.isFile()); // true
console.log(stats.isDirectory()); // false
console.log(stats.size); // file size in bytes
console.log(stats.mtime); // Date — last modified time
console.log(stats.ctime); // Date — last status change time
console.log(stats.birthtime); // Date — creation time

// stat follows symlinks — lstat does not
const lstats = await fs.lstat("symlink");
console.log(lstats.isSymbolicLink()); // true
```

### Recursive Directory Walk

```javascript
const fs = require("node:fs/promises");
const path = require("node:path");

async function walk(dir) {
  const entries = await fs.readdir(dir, { withFileTypes: true });
  const results = [];

  for (const entry of entries) {
    const fullPath = path.join(dir, entry.name);
    if (entry.isDirectory()) {
      results.push(...(await walk(fullPath))); // recurse
    } else {
      results.push(fullPath);
    }
  }

  return results;
}

const allFiles = await walk("src");
```

---

## 7. Watching Files

`fs.watch` notifies you when a file or directory changes.

```javascript
const fs = require("node:fs");

const watcher = fs.watch("config.json", (eventType, filename) => {
  console.log(`Event: ${eventType}, File: ${filename}`);
  // eventType is 'rename' or 'change'
});

// Stop watching
watcher.close();

// Watch a directory (notified when any file inside changes)
fs.watch("src", { recursive: true }, (eventType, filename) => {
  console.log(`${filename} was ${eventType}d`);
});
```

`fs.watch` has known inconsistencies across operating systems. For production use (build tools, dev servers), use the `chokidar` npm package instead — it normalises behaviour across platforms.

---

## 8. fs/promises — The Modern API

`fs/promises` is the cleanest way to use `fs` in modern Node.js. All methods return Promises.

```javascript
// Import
const fs = require("node:fs/promises");
// or
import { readFile, writeFile, mkdir } from "node:fs/promises";

// Full async/await flow
async function processConfig(inputPath, outputPath) {
  const raw = await fs.readFile(inputPath, "utf8");
  const config = JSON.parse(raw);

  config.processedAt = new Date().toISOString();

  await fs.mkdir(path.dirname(outputPath), { recursive: true });
  await fs.writeFile(outputPath, JSON.stringify(config, null, 2), "utf8");
}
```

The callback-based `fs` API (`fs.readFile(path, callback)`) exists for historical reasons. Prefer `fs/promises` in all new code.

---

## 9. path — Working with File Paths

`path` provides utilities for constructing and parsing file paths in a cross-platform way. Never concatenate paths with string operations — use `path`.

```
Why path matters:

  Windows paths:  C:\Users\roshana\project\index.js  (backslash)
  Unix paths:     /home/roshana/project/index.js      (forward slash)

  'src' + '/' + 'index.js' → breaks on Windows
  path.join('src', 'index.js') → correct on all platforms
```

### Joining Paths

```javascript
const path = require("node:path");

path.join("src", "utils", "index.js");
// Unix:    'src/utils/index.js'
// Windows: 'src\\utils\\index.js'

// Handles extra slashes and dots
path.join("src/", "/utils/", "index.js"); // 'src/utils/index.js'
path.join("a", "b", "..", "c"); // 'a/c'  (.. resolved)
```

### Resolving to Absolute Path

```javascript
// resolve() produces an absolute path from the current working directory
path.resolve("src", "index.js");
// e.g. '/home/roshana/project/src/index.js'

// With an absolute segment, it starts fresh from that point
path.resolve("/app", "src", "index.js");
// '/app/src/index.js'

// Best practice: use __dirname to anchor relative paths
const configPath = path.resolve(__dirname, "../config/app.json");
// __dirname is the directory of the current file — always reliable
```

### Parsing Paths

```javascript
const file = "/home/roshana/project/src/utils.ts";

path.dirname(file); // '/home/roshana/project/src'
path.basename(file); // 'utils.ts'
path.extname(file); // '.ts'

path.basename(file, ".ts"); // 'utils' — strip extension

path.parse(file);
// {
//   root: '/',
//   dir: '/home/roshana/project/src',
//   base: 'utils.ts',
//   ext: '.ts',
//   name: 'utils'
// }
```

### Building Paths

```javascript
path.format({
  dir: "/home/roshana/project/src",
  name: "utils",
  ext: ".ts",
});
// '/home/roshana/project/src/utils.ts'
```

### Normalising

```javascript
// Clean up messy paths
path.normalize("/foo/bar//baz/../qux");
// '/foo/bar/qux'
```

### Relative Paths Between Two Paths

```javascript
path.relative("/app/src", "/app/src/utils/index.js");
// 'utils/index.js'

path.relative("/app/src", "/app/lib/db.js");
// '../../lib/db.js'
```

### __dirname and __filename

```javascript
console.log(__dirname); // absolute path of the directory of this file
console.log(__filename); // absolute path of this file

// Common pattern: path relative to the current file
const templatesDir = path.join(__dirname, "templates");
```

In ES Modules, `__dirname` is not available. Use `import.meta.url` instead:

```javascript
import { fileURLToPath } from "node:url";
import { dirname, join } from "node:path";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const templatesDir = join(__dirname, "templates");
```

---

## 10. os — Operating System Information

`os` exposes information about the operating system and the hardware Node.js is running on.

```javascript
const os = require("node:os");

os.platform(); // 'linux', 'darwin', 'win32'
os.arch(); // 'x64', 'arm64'
os.hostname(); // 'server-01'
os.release(); // OS version string

os.cpus(); // array of CPU info objects
os.cpus().length; // number of logical CPU cores

os.totalmem(); // total RAM in bytes
os.freemem(); // available RAM in bytes

// Human-readable
const totalGB = (os.totalmem() / 1024 ** 3).toFixed(1);
const freeGB = (os.freemem() / 1024 ** 3).toFixed(1);
console.log(`RAM: ${freeGB}GB free / ${totalGB}GB total`);

os.uptime(); // system uptime in seconds

os.homedir(); // '/home/roshana' or 'C:\Users\roshana'
os.tmpdir(); // system temp directory — safe place to write temp files

os.EOL; // line ending: '\n' on Unix, '\r\n' on Windows
```

### Practical Uses

```javascript
const os = require("node:os");

// Determine optimal worker thread count
const workerCount = os.cpus().length;

// Write temp files safely
const path = require("node:path");
const tempFile = path.join(os.tmpdir(), `process-${process.pid}.tmp`);

// Detect platform for conditional logic
if (os.platform() === "win32") {
  // Windows-specific code
}
```

---

## 11. url — Parsing and Building URLs

The `url` module provides the WHATWG URL API — the same `URL` class available in browsers.

```javascript
const { URL } = require("node:url");
// URL is also a global in Node.js 10+ — no import needed

const url = new URL(
  "https://api.example.com:8080/users?page=2&limit=10#results",
);

url.protocol; // 'https:'
url.hostname; // 'api.example.com'
url.port; // '8080'
url.host; // 'api.example.com:8080'
url.pathname; // '/users'
url.search; // '?page=2&limit=10'
url.hash; // '#results'
url.href; // full URL string
url.origin; // 'https://api.example.com:8080'
```

### Modifying URLs

```javascript
const url = new URL("https://api.example.com/users");

url.pathname = "/products";
url.searchParams.set("category", "books");
url.searchParams.set("page", "1");

console.log(url.href);
// 'https://api.example.com/products?category=books&page=1'
```

### Resolving Relative URLs

```javascript
const base = new URL("https://example.com/blog/posts/");
const relative = new URL("../authors", base);

console.log(relative.href);
// 'https://example.com/blog/authors'
```

### Building URLs Safely

Never build URLs with string concatenation — user input in query parameters can break the URL or inject unexpected values.

```javascript
// WRONG — breaks if category contains &, =, or spaces
const url = `https://api.example.com/search?q=${userQuery}`;

// CORRECT — searchParams handles encoding automatically
const url = new URL("https://api.example.com/search");
url.searchParams.set("q", userQuery);
url.searchParams.set("page", "1");
console.log(url.href);
// All special characters are percent-encoded correctly
```

---

## 12. The querystring / URLSearchParams API

`URLSearchParams` is the modern way to work with query strings. It is part of the WHATWG URL spec and available without any import.

```javascript
// Parse a query string
const params = new URLSearchParams("page=2&limit=10&sort=asc");

params.get("page"); // '2'
params.get("limit"); // '10'
params.has("sort"); // true
params.has("filter"); // false

// All values for a key (useful for checkbox arrays: ?tag=js&tag=ts)
const tags = new URLSearchParams("tag=js&tag=ts&tag=node");
tags.getAll("tag"); // ['js', 'ts', 'node']

// Iterate
for (const [key, value] of params) {
  console.log(key, value);
}

// Modify
params.set("page", "3"); // replace
params.append("tag", "js"); // add (keeps existing values)
params.delete("sort"); // remove

// Serialize back to string
params.toString(); // 'page=3&limit=10&tag=js'
```

### From a URL Object

```javascript
const url = new URL("https://api.example.com/search?q=node&page=1");

url.searchParams.get("q"); // 'node'
url.searchParams.set("page", "2");
console.log(url.href);
// 'https://api.example.com/search?q=node&page=2'
```

### Converting File Paths to URLs and Back

```javascript
const { pathToFileURL, fileURLToPath } = require("node:url");

// Convert a file path to a file:// URL
const fileUrl = pathToFileURL("/home/roshana/project/index.js");
console.log(fileUrl.href); // 'file:///home/roshana/project/index.js'

// Convert back
const filePath = fileURLToPath("file:///home/roshana/project/index.js");
console.log(filePath); // '/home/roshana/project/index.js'

// This is how you get __dirname in ES Modules
import { fileURLToPath } from "node:url";
const __dirname = fileURLToPath(new URL(".", import.meta.url));
```
