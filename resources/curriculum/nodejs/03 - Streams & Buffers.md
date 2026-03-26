> Most Node.js I/O — reading files, handling HTTP requests, piping data between processes — is built on two primitives: **Buffers** (raw binary data) and **Streams** (data that arrives or is sent in chunks over time). Understanding both is what separates developers who cargo-cult Node.js from developers who understand it.

---

## Table of Contents

1. [Why Buffers Exist]
2. [Buffer — Raw Binary Data]
3. [Creating and Working with Buffers]
4. [Encoding — Translating Between Binary and Text]
5. [Why Streams Exist]
6. [The Four Stream Types]
7. [Readable Streams]
8. [Writable Streams]
9. [pipe — Connecting Streams]
10. [Transform Streams]
11. [Duplex Streams]
12. [Backpressure]
13. [Stream Modes — Flowing vs Paused]
14. [async iteration over Streams]

---

## 1. Why Buffers Exist

JavaScript was designed for the browser. It had strings, numbers, arrays — no concept of raw binary data. When Node.js needed to read files, handle TCP sockets, and deal with binary protocols, it needed a type that JavaScript didn't have.

```
The problem:

  You read a 1 GB file.
  A JavaScript string would hold it — but strings in V8 are UTF-16 encoded.
  A 1 GB binary file stored as a UTF-16 string could use up to 2 GB of memory.
  And many bytes in binary files are not valid UTF-16 characters at all.

  Node.js needed a way to hold raw bytes without any encoding overhead.
  Buffer is that type.
```

A Buffer is a fixed-size allocation of raw memory outside V8's heap, representing a sequence of bytes directly.

---

## 2. Buffer — Raw Binary Data

A Buffer is essentially a `Uint8Array` — an array where each element is a single byte (0–255).

```
Buffer:  [ 72 | 101 | 108 | 108 | 111 ]
          H      e     l     l     o       (if interpreted as ASCII/UTF-8)
          0x48  0x65  0x6C  0x6C  0x6F    (in hex)

Each slot is one byte: an integer from 0 to 255.
The Buffer doesn't know or care what the bytes represent.
That's your job when you read or write it.
```

Buffers are fixed-size. Once allocated, their length cannot change (unlike arrays). This is a deliberate design — it maps directly to how memory works at the OS level.

---

## 3. Creating and Working with Buffers

### Allocating

```javascript
// Safe allocation — fills with zeros
const buf = Buffer.alloc(10);
console.log(buf); // <Buffer 00 00 00 00 00 00 00 00 00 00>

// Unsafe allocation — faster, but contains whatever was in memory
// Only use when you're about to write to every byte
const buf2 = Buffer.allocUnsafe(10);
```

### From a String

```javascript
const buf = Buffer.from('Hello');
console.log(buf);          // <Buffer 48 65 6c 6c 6f>
console.log(buf.length);   // 5

// With explicit encoding
const buf2 = Buffer.from('Hello', 'utf8');   // default
const buf3 = Buffer.from('48656c6c6f', 'hex');
const buf4 = Buffer.from('SGVsbG8=', 'base64');
```

### From an Array of Bytes

```javascript
const buf = Buffer.from([72, 101, 108, 108, 111]);
console.log(buf.toString()); // 'Hello'
```

### Reading and Writing

```javascript
const buf = Buffer.alloc(4);

buf.writeUInt8(255, 0);         // write 255 at offset 0
buf.writeUInt16LE(1000, 1);     // write 1000 as 16-bit little-endian at offset 1
buf.writeUInt8(42, 3);          // write 42 at offset 3

console.log(buf.readUInt8(0));       // 255
console.log(buf.readUInt16LE(1));    // 1000

// Direct index access
const buf2 = Buffer.from('ABC');
console.log(buf2[0]);   // 65 (ASCII code for 'A')
buf2[0] = 90;           // modify in place
console.log(buf2.toString()); // 'ZBC'
```

### Slicing

```javascript
const buf = Buffer.from('Hello World');

// slice shares memory with the original — NOT a copy
const slice = buf.slice(0, 5);
console.log(slice.toString()); // 'Hello'

slice[0] = 74; // modifies the original too
console.log(buf.toString()); // 'Jello World'

// subarray is the modern equivalent (same behaviour)
const sub = buf.subarray(6, 11);
console.log(sub.toString()); // 'World'

// For a true copy:
const copy = Buffer.from(buf.slice(0, 5));
```

### Concatenating

```javascript
const a = Buffer.from('Hello ');
const b = Buffer.from('World');

// Buffers are fixed-size — you can't push to them
// Use Buffer.concat to create a new combined Buffer
const combined = Buffer.concat([a, b]);
console.log(combined.toString()); // 'Hello World'

// With explicit total length (slightly faster)
const combined2 = Buffer.concat([a, b], a.length + b.length);
```

---

## 4. Encoding — Translating Between Binary and Text

When you call `buf.toString()` or `Buffer.from(string)`, an encoding converts between bytes and text. The encoding matters because different encodings map the same bytes to different characters.

```javascript
const buf = Buffer.from('Hello');

console.log(buf.toString('utf8'));    // 'Hello'     (default)
console.log(buf.toString('ascii'));   // 'Hello'
console.log(buf.toString('hex'));     // '48656c6c6f'
console.log(buf.toString('base64')); // 'SGVsbG8='
```

### Common Encodings in Node.js

```
'utf8'      Default. Variable-width. Handles all Unicode. Use this for text.
'ascii'     7-bit only. Strips the 8th bit. Only for pure ASCII content.
'utf16le'   Little-endian UTF-16. Used in some Windows APIs.
'latin1'    One byte per character. Maps byte values directly to Unicode code points.
'hex'       Two hex digits per byte. '48' = byte value 72 = 'H'.
'base64'    3 bytes → 4 ASCII characters. Used to embed binary in text (images in JSON, JWTs).
'base64url' Like base64 but URL-safe (replaces + with -, / with _).
```

### When Encoding Matters

```javascript
// A multi-byte UTF-8 character
const buf = Buffer.from('é');       // 'é' is 2 bytes in UTF-8: 0xC3 0xA9
console.log(buf.length);            // 2
console.log('é'.length);           // 1 — JS string length counts code units, not bytes

// Buffer.byteLength gives the byte count, not character count
console.log(Buffer.byteLength('é', 'utf8'));   // 2
console.log(Buffer.byteLength('Hello', 'utf8')); // 5
```

This distinction matters when you're building HTTP headers (Content-Length must be byte count, not character count) or implementing network protocols.

---

## 5. Why Streams Exist

Without streams, you would load entire files or request bodies into memory before processing them.

```
Loading a 2 GB video file without streams:

  fs.readFile('video.mp4', (err, data) => {
    // data is a 2 GB Buffer sitting in RAM
    res.end(data);
  });

  Problem:
    - Your server needs 2 GB of free RAM just for this one request
    - Nothing happens until the entire file is read
    - If 100 users request simultaneously: 200 GB RAM required

With streams:

  const readable = fs.createReadStream('video.mp4');
  readable.pipe(res);

  What happens:
    - Node reads a 64KB chunk from disk
    - Sends that 64KB chunk to the client
    - Reads the next 64KB chunk
    - Sends it
    - Repeats until done
    - Peak memory usage: ~64KB regardless of file size
```

Streams are the mechanism for processing data **as it arrives** rather than waiting for all of it.

---

## 6. The Four Stream Types

```
┌─────────────┬──────────────────────────────────────────────────┐
│ Type        │ Description                                      │
├─────────────┼──────────────────────────────────────────────────┤
│ Readable    │ Source of data. You read FROM it.                │
│             │ Examples: fs.createReadStream, http.IncomingMessage│
├─────────────┼──────────────────────────────────────────────────┤
│ Writable    │ Destination for data. You write TO it.           │
│             │ Examples: fs.createWriteStream, http.ServerResponse│
├─────────────┼──────────────────────────────────────────────────┤
│ Duplex      │ Both readable and writable. Two independent      │
│             │ channels. Examples: TCP socket (net.Socket)      │
├─────────────┼──────────────────────────────────────────────────┤
│ Transform   │ Duplex where output is derived from input.       │
│             │ Examples: zlib.createGzip, crypto.createCipher   │
└─────────────┴──────────────────────────────────────────────────┘
```

---

## 7. Readable Streams

A Readable stream emits data events as chunks become available.

### Consuming with 'data' events

```javascript
const fs = require('fs');

const readable = fs.createReadStream('file.txt', { encoding: 'utf8' });

readable.on('data', (chunk) => {
  // chunk is a string (because encoding was set) or Buffer
  console.log('Received chunk:', chunk.length, 'bytes');
});

readable.on('end', () => {
  console.log('No more data.');
});

readable.on('error', (err) => {
  console.error('Stream error:', err);
});
```

### Controlling Chunk Size

```javascript
// Default highWaterMark is 64KB (65536 bytes)
const readable = fs.createReadStream('file.txt', {
  highWaterMark: 1024 * 16,  // 16KB chunks
});

// For object streams (non-binary), default highWaterMark is 16 objects
```

### Common Readable Sources

```javascript
const fs   = require('fs');
const http = require('http');
const { Readable } = require('stream');

// File
const fileStream = fs.createReadStream('data.csv');

// HTTP request body
http.createServer((req, res) => {
  // req is a Readable stream
  req.on('data', chunk => { /* ... */ });
  req.on('end', () => { /* ... */ });
});

// Creating a Readable from scratch
const readable = new Readable({
  read() {
    this.push('Hello ');
    this.push('World');
    this.push(null);  // null signals end of stream
  }
});
```

---

## 8. Writable Streams

A Writable stream accepts data written to it and does something with it — writes to a file, sends over a network, etc.

```javascript
const fs = require('fs');

const writable = fs.createWriteStream('output.txt');

writable.write('First line\n');
writable.write('Second line\n');
writable.end('Final line\n');  // end() writes last chunk and signals done

writable.on('finish', () => {
  console.log('All data flushed to disk.');
});

writable.on('error', (err) => {
  console.error('Write error:', err);
});
```

### write() Return Value

`write()` returns a boolean — `true` if the internal buffer has room, `false` if it is full (backpressure, covered later).

```javascript
const canWriteMore = writable.write(chunk);
if (!canWriteMore) {
  // Stop writing until 'drain' event fires
  readable.pause();
  writable.once('drain', () => readable.resume());
}
```

### Common Writable Destinations

```javascript
// File
const file = fs.createWriteStream('log.txt');

// HTTP response
http.createServer((req, res) => {
  // res is a Writable stream
  res.write('Hello ');
  res.end('World');
});

// process.stdout / process.stderr
process.stdout.write('Hello\n');
```

---

## 9. pipe — Connecting Streams

`pipe()` connects a Readable to a Writable, automatically handling data flow and backpressure.

```javascript
const fs = require('fs');

// Copy a file
const source = fs.createReadStream('input.txt');
const dest   = fs.createWriteStream('output.txt');

source.pipe(dest);

dest.on('finish', () => console.log('Copy complete.'));
```

### Chaining pipe

`pipe()` returns the destination stream, so you can chain multiple transforms:

```javascript
const fs   = require('fs');
const zlib = require('zlib');

// Read file → compress with gzip → write compressed file
fs.createReadStream('file.txt')
  .pipe(zlib.createGzip())
  .pipe(fs.createWriteStream('file.txt.gz'));
```

### stream.pipeline — The Better Way

`pipe()` has a known issue: errors in intermediate streams don't propagate properly, leaving streams unclosed. `stream.pipeline()` fixes this.

```javascript
const { pipeline } = require('stream');
const fs   = require('fs');
const zlib = require('zlib');

pipeline(
  fs.createReadStream('file.txt'),
  zlib.createGzip(),
  fs.createWriteStream('file.txt.gz'),
  (err) => {
    if (err) console.error('Pipeline failed:', err);
    else     console.log('Pipeline complete.');
  }
);

// With promises (Node 15+)
const { pipeline } = require('stream/promises');

await pipeline(
  fs.createReadStream('file.txt'),
  zlib.createGzip(),
  fs.createWriteStream('file.txt.gz'),
);
```

Always use `pipeline()` instead of `pipe()` in production code.

---

## 10. Transform Streams

A Transform stream sits in the middle of a pipeline — it reads input, transforms it, and outputs the result. It is both readable and writable.

```javascript
const { Transform } = require('stream');

// Transform that uppercases all text
const uppercase = new Transform({
  transform(chunk, encoding, callback) {
    // chunk is a Buffer or string
    // Push the transformed result
    this.push(chunk.toString().toUpperCase());
    // Call callback when done with this chunk
    callback();
  }
});

process.stdin.pipe(uppercase).pipe(process.stdout);
```

### Transform with Error

```javascript
const { Transform } = require('stream');

const jsonParser = new Transform({
  objectMode: true,       // output objects, not Buffers
  transform(chunk, encoding, callback) {
    try {
      const parsed = JSON.parse(chunk.toString());
      this.push(parsed);
      callback();
    } catch (err) {
      callback(err);  // pass error to pipeline
    }
  }
});
```

### Built-in Transform Streams

```javascript
const zlib   = require('zlib');
const crypto = require('crypto');

// Compression
zlib.createGzip()    // compress
zlib.createGunzip()  // decompress

// Encryption
crypto.createCipheriv(algorithm, key, iv)
crypto.createDecipheriv(algorithm, key, iv)
```

---

## 11. Duplex Streams

A Duplex stream is readable and writable, but the read and write sides are independent — unlike Transform where output derives from input.

```javascript
const { Duplex } = require('stream');

const duplex = new Duplex({
  read(size) {
    this.push('data from readable side');
    this.push(null);
  },
  write(chunk, encoding, callback) {
    console.log('Received on writable side:', chunk.toString());
    callback();
  }
});
```

The most common Duplex stream you'll work with is a TCP socket:

```javascript
const net = require('net');

const server = net.createServer((socket) => {
  // socket is a Duplex stream
  socket.on('data', (data) => {
    socket.write('Echo: ' + data);  // write back what was received
  });
});
```

---

## 12. Backpressure

Backpressure is what happens when a Writable stream cannot keep up with a Readable stream.

```
Without backpressure handling:

  Readable reads at 100MB/s (fast disk)
  Writable writes at 10MB/s (slow network)

  After 1 second:
    - Readable has produced 100MB
    - Writable has consumed 10MB
    - 90MB is buffered in memory
    - After 10 seconds: 900MB buffered → out of memory
```

`pipe()` and `pipeline()` handle backpressure automatically. If you're manually consuming a stream, you must handle it yourself:

```javascript
const fs = require('fs');

const readable = fs.createReadStream('large-file.bin');
const writable = fs.createWriteStream('output.bin');

readable.on('data', (chunk) => {
  const canContinue = writable.write(chunk);

  if (!canContinue) {
    // Writable buffer is full — pause the readable
    readable.pause();

    // Resume when the writable has drained
    writable.once('drain', () => {
      readable.resume();
    });
  }
});

readable.on('end', () => writable.end());
```

`pipe()` does exactly this internally. Use it and you get backpressure for free.

---

## 13. Stream Modes — Flowing vs Paused

A Readable stream is always in one of two modes:

```
Paused mode (default):
  Data is not emitted until you explicitly ask for it.
  You call readable.read() manually, or attach a 'data' listener.

Flowing mode:
  Data is emitted as fast as it arrives.
  Triggered by: attaching a 'data' listener, calling resume(), or calling pipe().

Switching modes:
  Paused → Flowing:  add 'data' listener, or call readable.resume(), or pipe()
  Flowing → Paused:  call readable.pause(), or remove all 'data' listeners
```

```javascript
const readable = fs.createReadStream('file.txt');

// Paused mode — pull data manually
readable.on('readable', () => {
  let chunk;
  while ((chunk = readable.read(64)) !== null) {
    // read up to 64 bytes at a time
    process(chunk);
  }
});

// Flowing mode — data pushed to you
readable.on('data', (chunk) => {
  process(chunk);
});
```

In practice: use `pipe()` or async iteration. Manually managing modes is only necessary when building custom stream infrastructure.

---

## 14. async iteration over Streams

Node.js 10+ supports `for await...of` on streams, which is the cleanest way to consume a Readable.

```javascript
const fs = require('fs');

async function processFile(path) {
  const stream = fs.createReadStream(path, { encoding: 'utf8' });

  for await (const chunk of stream) {
    console.log('Chunk:', chunk.length, 'chars');
  }

  console.log('Done.');
}
```

### Collecting a Stream into a Buffer

```javascript
async function streamToBuffer(readable) {
  const chunks = [];
  for await (const chunk of readable) {
    chunks.push(Buffer.isBuffer(chunk) ? chunk : Buffer.from(chunk));
  }
  return Buffer.concat(chunks);
}

// Usage: collect an HTTP request body
http.createServer(async (req, res) => {
  const body = await streamToBuffer(req);
  const text = body.toString('utf8');
  console.log('Body:', text);
  res.end('OK');
});
```

### Error Handling with async iteration

```javascript
async function readFile(path) {
  const stream = fs.createReadStream(path);
  try {
    for await (const chunk of stream) {
      process(chunk);
    }
  } catch (err) {
    console.error('Stream error:', err);
  }
}
```

Async iteration is the recommended modern approach for consuming Readable streams when you don't need `pipe()`.