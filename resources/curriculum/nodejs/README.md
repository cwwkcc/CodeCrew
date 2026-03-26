# Node.js Curriculum

A structured curriculum for understanding Node.js from the inside out — the runtime, modules, event loop, I/O primitives, core APIs, the HTTP layer, the package ecosystem, error handling, and configuration. All 9 modules are numbered `00–08` — read them in order.

> **Location:** `resources/curriculum/nodejs/`

---

## Module Index

### Part 1 — How Node.js Works

|#|Topic|File|
|---|---|---|
|00|What Node.js Is & How It Works — V8, libuv, and the Runtime|`00 - What Node.js Is & How It Works.md`|
|01|Modules — CommonJS, ES Modules, and the Module System|`01 - Modules.md`|
|02|The Event Loop in Depth — Phases, Timers, and the Microtask Queue|`02 - The Event Loop in Depth.md`|

### Part 2 — I/O Primitives

|#|Topic|File|
|---|---|---|
|03|Streams & Buffers — Raw Binary Data and Streaming I/O|`03 - Streams & Buffers.md`|
|04|Core Modules — fs, path, os, and url|`04 - Core Modules (fs, path, os, url).md`|

### Part 3 — The Network Layer

|#|Topic|File|
|---|---|---|
|05|The http Module — Building Servers and Making Requests|`05 - The http Module.md`|

### Part 4 — The Ecosystem

|#|Topic|File|
|---|---|---|
|06|npm & the Package Ecosystem — Installing, Versioning, and Security|`06 - npm & the Package Ecosystem.md`|

### Part 5 — Production Concerns

|#|Topic|File|
|---|---|---|
|07|Error Handling in Node.js — Operational Errors, Custom Classes, and Graceful Recovery|`07 - Error Handling in Node.js.md`|
|08|Environment Variables & process — Configuration, Signals, and the process Object|`08 - Environment Variables & process.md`|

---

## Notes

- Complete `javascript/06 - Asynchronous JavaScript & APIs.md` before starting here.
- Modules 00–02 build the conceptual foundation — don't skip them.
- Module 05 (http) is the bridge to frameworks like Express and NestJS — after completing this curriculum you understand exactly what those frameworks add.