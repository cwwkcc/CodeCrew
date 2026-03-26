> Every real Node.js application needs configuration that changes between environments — database URLs in development vs production, API keys that must never be committed to source control, feature flags, port numbers. The mechanism for all of this is environment variables, accessed through the `process` object.

---

## Table of Contents

1. [What Environment Variables Are]
2. [The process Object]
3. [process.env — Reading Environment Variables]
4. [Setting Environment Variables]
5. [.env Files and dotenv]
6. [Validating Environment Variables]
7. [process.argv — Command-Line Arguments]
8. [process.exit — Exiting the Process]
9. [process Signals — Graceful Shutdown]
10. [process.cwd and process.chdir]
11. [process.memoryUsage and process.cpuUsage]
---

## 1. What Environment Variables Are

Environment variables are key-value pairs provided by the operating system to a running process. They exist outside your code and outside your repository.

```
Why they exist:

  Your code runs in multiple places:
    - Your laptop (development)
    - A teammate's laptop
    - A CI/CD pipeline (testing)
    - A staging server
    - A production server

  Each environment needs different configuration:
    DATABASE_URL    different database per environment
    API_KEY         different API key per service
    PORT            may differ per deployment
    NODE_ENV        tells code which environment it's in

  You cannot hardcode these into your source code:
    - API keys in source code → committed to git → leaked
    - Database URLs vary per environment → hardcoding breaks deploys

  Environment variables solve this: code reads config from the environment,
  and each environment provides its own values.
```

---

## 2. The process Object

`process` is a global object in Node.js — available without any import. It represents the currently running Node.js process.

```javascript
process.version;          // Node.js version: 'v20.10.0'
process.versions;         // versions of V8, libuv, OpenSSL, etc.
process.platform;         // 'linux', 'darwin', 'win32'
process.arch;             // 'x64', 'arm64'
process.pid;              // process ID (integer)
process.ppid;             // parent process ID
process.title;            // process name (appears in task manager)
process.uptime();         // seconds since process started
process.hrtime.bigint();  // high-resolution time in nanoseconds (for benchmarking)
```

---

## 3. process.env — Reading Environment Variables

`process.env` is an object containing all environment variables as strings.

```javascript
console.log(process.env.NODE_ENV);    // 'development', 'production', 'test'
console.log(process.env.DATABASE_URL); // 'postgresql://localhost:5432/mydb'
console.log(process.env.PORT);        // '3000' — always a string
console.log(process.env.MISSING);     // undefined — not set
```

### Important: All Values Are Strings

```javascript
// process.env.PORT is '3000', not 3000
const port = process.env.PORT;
port + 1;          // '30001' — string concatenation!
Number(port) + 1;  // 3001    — correct

// Convert types when reading
const port    = Number(process.env.PORT    ?? 3000);
const debug   = process.env.DEBUG === 'true';  // string 'true' → boolean
const timeout = parseInt(process.env.TIMEOUT ?? '5000', 10);
```

### Checking for Required Variables

```javascript
// Simple — throws if not set
function requireEnv(key) {
  const value = process.env[key];
  if (!value) throw new Error(`Missing required environment variable: ${key}`);
  return value;
}

const dbUrl   = requireEnv('DATABASE_URL');
const apiKey  = requireEnv('STRIPE_SECRET_KEY');
const jwtSecret = requireEnv('JWT_SECRET');
```

---

## 4. Setting Environment Variables

### On the Command Line

```bash
# Unix / macOS
NODE_ENV=production PORT=8080 node server.js

# Windows (cmd)
set NODE_ENV=production && node server.js

# Windows (PowerShell)
$env:NODE_ENV="production"; node server.js
```

### In npm Scripts (cross-platform with cross-env)

Different shells set env vars differently, which breaks npm scripts across platforms. `cross-env` normalises this:

```bash
npm install --save-dev cross-env
```

```json
{
  "scripts": {
    "dev":   "cross-env NODE_ENV=development tsx watch src/index.ts",
    "start": "cross-env NODE_ENV=production node dist/index.js",
    "test":  "cross-env NODE_ENV=test jest"
  }
}
```

### At the System Level

For persistent variables (survive terminal restarts):

```bash
# Linux/macOS — add to ~/.bashrc or ~/.zshrc
export DATABASE_URL="postgresql://localhost:5432/mydb"
export JWT_SECRET="supersecretvalue"

# Reload
source ~/.bashrc
```

In production servers, environment variables are typically set in the deployment platform's configuration (Render, Railway, Fly.io, Docker Compose, Kubernetes secrets, etc.) — never in files committed to the repo.

---

## 5. .env Files and dotenv

Managing environment variables for local development by hand is tedious. `.env` files solve this.

A `.env` file is a plain text file in your project root that lists key-value pairs:

```bash
# .env
NODE_ENV=development
PORT=3000
DATABASE_URL=postgresql://localhost:5432/mydb_dev
JWT_SECRET=local-dev-secret-not-for-production
REDIS_URL=redis://localhost:6379
DEBUG=true
```

`dotenv` reads this file and loads its contents into `process.env` at startup.

```bash
npm install dotenv
```

```javascript
// At the very top of your entry file (src/index.ts or src/main.ts)
import 'dotenv/config';
// or
require('dotenv').config();

// After this line, process.env has all the values from .env
console.log(process.env.PORT); // '3000'
```

### .env Must Never Be Committed

```bash
# .gitignore
.env
.env.local
.env.*.local
```

Instead, commit a `.env.example` file with all the variable names but no actual values:

```bash
# .env.example — commit this
NODE_ENV=development
PORT=3000
DATABASE_URL=            # your local postgres URL
JWT_SECRET=              # generate with: openssl rand -base64 32
REDIS_URL=redis://localhost:6379
```

When a new developer clones the project:

1. Copy `.env.example` to `.env`
2. Fill in their own values
3. Never commit `.env`

### Multiple .env Files

```
.env                 Loaded in all environments
.env.local           Local overrides — never committed
.env.development     Loaded only when NODE_ENV=development
.env.production      Loaded only when NODE_ENV=production
.env.test            Loaded only when NODE_ENV=test
```

Libraries like `dotenv-flow` handle loading the right file automatically. NestJS's `@nestjs/config` does this as well.

### Node.js 20.6+ — Built-in .env Support

Node.js 20.6 added native `.env` file loading without any package:

```bash
node --env-file=.env src/index.js
```

---

## 6. Validating Environment Variables

Reading `process.env` directly throughout your code is fragile. A better pattern: validate and parse all env vars at startup into a typed config object.

### Manual Validation

```javascript
function loadConfig() {
  const errors = [];

  const config = {
    nodeEnv:     process.env.NODE_ENV     ?? 'development',
    port:        Number(process.env.PORT  ?? 3000),
    databaseUrl: process.env.DATABASE_URL,
    jwtSecret:   process.env.JWT_SECRET,
    redisUrl:    process.env.REDIS_URL,
    debug:       process.env.DEBUG === 'true',
  };

  if (!config.databaseUrl) errors.push('DATABASE_URL is required');
  if (!config.jwtSecret)   errors.push('JWT_SECRET is required');
  if (isNaN(config.port))  errors.push('PORT must be a number');

  if (errors.length > 0) {
    console.error('Invalid environment configuration:');
    errors.forEach(e => console.error(' -', e));
    process.exit(1);  // fail fast — don't start with bad config
  }

  return config;
}

export const config = loadConfig();
```

```javascript
// Everywhere in your code — import config, not process.env directly
import { config } from './config.js';

app.listen(config.port);
```

### Validation with Zod (recommended)

Zod gives you typed validation with detailed error messages:

```javascript
import { z } from 'zod';

const envSchema = z.object({
  NODE_ENV:     z.enum(['development', 'production', 'test']).default('development'),
  PORT:         z.coerce.number().int().positive().default(3000),
  DATABASE_URL: z.string().url(),
  JWT_SECRET:   z.string().min(32),
  REDIS_URL:    z.string().url().optional(),
  DEBUG:        z.coerce.boolean().default(false),
});

const result = envSchema.safeParse(process.env);

if (!result.success) {
  console.error('❌ Invalid environment variables:');
  console.error(result.error.format());
  process.exit(1);
}

export const env = result.data;
// env.PORT is number (not string) — Zod coerced it
// env.DEBUG is boolean
// env.JWT_SECRET is guaranteed to be at least 32 chars
```

---

## 7. process.argv — Command-Line Arguments

`process.argv` is an array of all command-line arguments passed to the Node.js process.

```bash
node server.js --port 4000 --debug
```

```javascript
console.log(process.argv);
// [
//   '/usr/local/bin/node',    // argv[0] — path to node binary
//   '/app/server.js',         // argv[1] — path to script
//   '--port',                 // argv[2] — first argument
//   '4000',                   // argv[3]
//   '--debug'                 // argv[4]
// ]

// User arguments start at index 2
const userArgs = process.argv.slice(2);
// ['--port', '4000', '--debug']
```

### Simple Argument Parsing

```javascript
const args = process.argv.slice(2);

const port = (() => {
  const i = args.indexOf('--port');
  return i !== -1 ? Number(args[i + 1]) : 3000;
})();

const debug = args.includes('--debug');
```

For complex CLIs with flags, subcommands, and help text, use a library like `commander` or `yargs` instead of parsing `process.argv` manually.

---

## 8. process.exit — Exiting the Process

```javascript
// Exit with success (code 0)
process.exit(0);
process.exit();  // defaults to 0

// Exit with failure (any non-zero code)
process.exit(1);  // convention: 1 = general error

// Exit codes
// 0   Success
// 1   General error
// 2   Misuse of shell built-in (rarely used in Node.js)
// 127 Command not found (used by shell, not Node.js directly)
```

### When to Call process.exit

```
DO call process.exit(1) when:
  - Environment validation fails at startup
  - An uncaught/unhandled error leaves the process in an unknown state
  - A fatal error occurs that cannot be recovered from

DO NOT call process.exit() to end normal execution:
  - Let the event loop drain naturally
  - The process exits on its own when there's nothing left to do

DO NOT call process.exit() inside business logic:
  - Throw an error instead — let error handling decide what to do
```

### process.exitCode

Instead of calling `process.exit(1)`, you can set `process.exitCode` and let the process exit naturally:

```javascript
process.exitCode = 1;
// Process will exit with code 1 when the event loop drains
```

---

## 9. process Signals — Graceful Shutdown

When you press Ctrl+C or a deployment system restarts your app, Node receives a signal. You can intercept it to perform cleanup before exiting.

```
SIGTERM   Polite termination request. Sent by process managers (PM2, Docker, Kubernetes).
          The process should clean up and exit.

SIGINT    Interrupt signal. Sent when the user presses Ctrl+C.
          Default behaviour: exit immediately.

SIGKILL   Forced kill. Cannot be caught. The OS terminates the process immediately.
          Sent after SIGTERM if the process doesn't exit in time.
```

```javascript
const http = require('node:http');

const server = http.createServer(handler);
server.listen(3000);

// Graceful shutdown handler
async function gracefulShutdown(signal) {
  console.log(`Received ${signal}. Shutting down gracefully...`);

  // Stop accepting new connections
  server.close(async () => {
    console.log('HTTP server closed.');

    // Close database connections, flush caches, etc.
    await db.disconnect();
    await redis.quit();

    console.log('All connections closed. Exiting.');
    process.exit(0);
  });

  // Force exit if cleanup takes too long
  setTimeout(() => {
    console.error('Forced shutdown after timeout.');
    process.exit(1);
  }, 10_000);  // 10 seconds
}

process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
process.on('SIGINT',  () => gracefulShutdown('SIGINT'));
```

Graceful shutdown ensures in-flight requests finish and database connections close cleanly before the process dies. Without it, abrupt exits can leave transactions uncommitted or data corrupted.

---

## 10. process.cwd and process.chdir

```javascript
// Current working directory — where the process was started from
console.log(process.cwd());
// '/home/roshana/project'  (wherever you ran `node server.js`)

// Change working directory
process.chdir('/tmp');
console.log(process.cwd()); // '/tmp'
```

### cwd vs __dirname

```
process.cwd()   The directory you ran the node command from.
                Changes if you call process.chdir().
                Unreliable for file paths relative to your source file.

__dirname       The directory of the source file being executed.
                Always the same, regardless of where you ran node from.
                Use this when building paths relative to your code.

// Correct — relative to the source file
const configPath = path.join(__dirname, '../config/app.json');

// Wrong — breaks if you run node from a different directory
const configPath = path.join(process.cwd(), 'config/app.json');
```

---

## 11. process.memoryUsage and process.cpuUsage

```javascript
// Memory usage of the Node.js process
const mem = process.memoryUsage();

console.log(mem);
// {
//   rss:          45_678_592,  // Resident Set Size — total memory allocated
//   heapTotal:    18_874_368,  // Total size of V8 heap
//   heapUsed:     12_345_678,  // Heap currently in use
//   external:      1_234_567,  // Memory used by C++ objects (Buffers)
//   arrayBuffers:    123_456   // Memory for ArrayBuffers/SharedArrayBuffers
// }

// Human-readable
const mb = (bytes) => (bytes / 1024 / 1024).toFixed(1) + ' MB';
console.log(`Heap: ${mb(mem.heapUsed)} / ${mb(mem.heapTotal)}`);
console.log(`RSS:  ${mb(mem.rss)}`);

// CPU usage
const startCpu = process.cpuUsage();
// ... do work ...
const deltaCpu = process.cpuUsage(startCpu);

console.log(deltaCpu);
// { user: 123456, system: 45678 }  (microseconds)
```

### Using These for Health Checks

```javascript
app.get('/health', (req, res) => {
  const mem = process.memoryUsage();
  const heapUsedMB = mem.heapUsed / 1024 / 1024;

  res.json({
    status:      'ok',
    uptime:      process.uptime(),
    heapUsedMB:  Math.round(heapUsedMB),
    nodeVersion: process.version,
    pid:         process.pid,
  });
});
```