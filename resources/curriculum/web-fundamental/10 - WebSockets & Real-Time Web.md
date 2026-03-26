> HTTP is request-response — the client always initiates. But some features need the server to push data: live notifications, real-time grade updates, attendance tracking across multiple screens. WebSockets solve this. This part covers the WebSocket protocol, when to use it versus alternatives, and how it fits into the Paideon stack.

---

## Table of Contents

1. [The Problem with HTTP for Real-Time](#1-the-problem-with-http-for-real-time)
2. [The WebSocket Protocol — How the Upgrade Works](#2-the-websocket-protocol--how-the-upgrade-works)
3. [WebSocket Frames — The Wire Format](#3-websocket-frames--the-wire-format)
4. [The WebSocket API in the Browser](#4-the-websocket-api-in-the-browser)
5. [Server-Sent Events (SSE) — One-Way Push](#5-server-sent-events-sse--one-way-push)
6. [Long Polling — The Old Way](#6-long-polling--the-old-way)
7. [WebSocket vs SSE vs Polling — When to Use Which](#7-websocket-vs-sse-vs-polling--when-to-use-which)
8. [WebSocket Authentication](#8-websocket-authentication)
9. [Heartbeats and Connection Management](#9-heartbeats-and-connection-management)
10. [Scaling WebSockets — The Sticky Session Problem](#10-scaling-websockets--the-sticky-session-problem)
11. [WebSockets in NestJS with Socket.io](#11-websockets-in-nestjs-with-socketio)
12. [Real-Time Features in Paideon](#12-real-time-features-in-paideon)

---

## 1. The Problem with HTTP for Real-Time

HTTP is fundamentally pull-based. The client asks, the server responds. The server cannot initiate contact.

```
Scenario: Teacher marks attendance. 
Principal's dashboard should update immediately.

HTTP approach (polling):
  Principal's browser: GET /api/attendance/today?class=10A  (every 5 seconds)
  Server: returns data
  Principal's browser: GET /api/attendance/today?class=10A  (5 seconds later)
  Server: returns same data (nothing changed)
  Principal's browser: GET /api/attendance/today?class=10A  (5 seconds later)
  Server: NOW returns updated data
  
  Delay: up to 5 seconds.
  Wasted requests: many. Server processes hundreds of identical requests per minute.
  Scale: 100 teachers marking attendance + 10 admins polling = 1,200 requests/min
         for data that changes infrequently.

WebSocket approach:
  Connection established once.
  Teacher marks attendance → server immediately pushes update to principal's browser.
  Delay: milliseconds.
  Server load: one persistent connection per client, events on change only.
```

---

## 2. The WebSocket Protocol — How the Upgrade Works

WebSocket connections start as HTTP, then upgrade. This is how they bypass firewalls that only allow HTTP/HTTPS traffic.

```
STEP 1: Client sends HTTP Upgrade request
─────────────────────────────────────────────────────────────────
GET /ws HTTP/1.1
Host: api.paideon.lk
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
Origin: https://paideon.lk

Key fields:
  Upgrade: websocket  → "I want to switch protocols"
  Connection: Upgrade → "This connection will change protocols"
  Sec-WebSocket-Key   → 16 random bytes, base64-encoded
                        Used to verify the server is a real WebSocket server
  Sec-WebSocket-Version: 13 → The WebSocket version (always 13 now)

STEP 2: Server accepts the upgrade
─────────────────────────────────────────────────────────────────
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=

101 = Switching Protocols (the only time you'll see this status code)

Sec-WebSocket-Accept is computed by:
  SHA-1(Sec-WebSocket-Key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")
  base64(result)

The magic string is a fixed GUID defined in the WebSocket spec (RFC 6455).
This prevents non-WebSocket HTTP servers from accidentally accepting upgrades.

STEP 3: The TCP connection is now a WebSocket connection
─────────────────────────────────────────────────────────────────
The same TCP connection that carried the HTTP handshake
now carries WebSocket frames.
HTTP is gone. Both sides can now send messages at any time.
Full duplex: both sides can send simultaneously.
```

### WebSocket URL Schemes

```
ws://   → WebSocket over plain TCP (insecure, avoid)
wss://  → WebSocket over TLS (use this, always)

wss://api.paideon.lk/ws

Just as http:// → https://, ws:// → wss://
The TLS layer is established first (same as HTTPS), then the WebSocket upgrade
happens inside the encrypted tunnel.
```

---

## 3. WebSocket Frames — The Wire Format

After the handshake, data travels in **frames**. This is more efficient than HTTP (no headers repeated on every message).

```
WebSocket Frame structure:
  FIN  RSV1 RSV2 RSV3  OPCODE  MASK  PAYLOAD LEN  [EXTENDED LEN]  [MASKING KEY]  [PAYLOAD]
  1    0    0    0     4 bits  1 bit  7 bits   ...                  32 bits         N bytes

FIN: 1 = final fragment, 0 = more fragments follow (for large messages)

OPCODE:
  0x0  Continuation frame (part of a fragmented message)
  0x1  Text frame (UTF-8 encoded text)
  0x2  Binary frame (arbitrary binary data)
  0x8  Close frame (graceful connection close)
  0x9  Ping frame (heartbeat)
  0xA  Pong frame (heartbeat response)

MASK:
  Client → Server: MUST be masked (MASK=1, masking key present)
  Server → Client: MUST NOT be masked (MASK=0)
  
  Masking: each byte XOR'd with a byte from the 4-byte masking key.
  Purpose: prevents cache poisoning via malicious intermediaries.

PAYLOAD LEN:
  7 bits can represent 0-125 bytes directly.
  126 → next 2 bytes are the actual length (up to 65535 bytes)
  127 → next 8 bytes are the actual length (up to ~18 exabytes)
```

In practice, you never deal with frames directly. Socket.io or ws library handles framing. But knowing the frame format explains why WebSocket has extremely low overhead — a short message has just 2-6 bytes of overhead vs HTTP's 200-800 bytes of headers.

---

## 4. The WebSocket API in the Browser

```javascript
// Establish connection
const ws = new WebSocket('wss://api.paideon.lk/ws');

// Connection opened
ws.addEventListener('open', (event) => {
  console.log('WebSocket connected');
  
  // Send a message (string)
  ws.send(JSON.stringify({
    type: 'SUBSCRIBE',
    channel: 'attendance:class-10A',
  }));
});

// Message received from server
ws.addEventListener('message', (event) => {
  const data = JSON.parse(event.data);
  
  switch (data.type) {
    case 'ATTENDANCE_UPDATE':
      updateAttendanceDisplay(data.payload);
      break;
    case 'NOTIFICATION':
      showNotification(data.payload);
      break;
  }
});

// Connection closed
ws.addEventListener('close', (event) => {
  console.log('WebSocket closed:', event.code, event.reason);
  // Reconnect logic here
});

// Error occurred
ws.addEventListener('error', (event) => {
  console.error('WebSocket error:', event);
});

// Send binary data
const buffer = new ArrayBuffer(8);
ws.send(buffer);

// Close the connection gracefully
ws.close(1000, 'User logged out');

// WebSocket ready states
ws.readyState === WebSocket.CONNECTING  // 0 — not yet open
ws.readyState === WebSocket.OPEN        // 1 — open, can send/receive
ws.readyState === WebSocket.CLOSING     // 2 — in process of closing
ws.readyState === WebSocket.CLOSED      // 3 — closed
```

### Message Format Convention

WebSocket has no built-in message structure. You define your own. A common pattern:

```javascript
// Message envelope
{
  "type": "ATTENDANCE_MARKED",       // what kind of event
  "payload": {                        // the data
    "studentId": "cuid-123",
    "status": "present",
    "class": "10A",
    "timestamp": "2026-03-13T08:30:00Z"
  },
  "id": "msg-uuid"                   // optional: for ack/deduplication
}
```

---

## 5. Server-Sent Events (SSE) — One-Way Push

SSE is a simpler alternative when you only need server-to-client push (not bidirectional).

```
SSE = HTTP response that never ends.

Client makes a normal GET request.
Server sends a long-running response, writing events over time.
Browser fires events when data arrives.

Protocol is dead simple — plain text over HTTP:
  data: {"type":"ATTENDANCE_UPDATE","studentId":"cuid-123"}\n\n
  data: {"type":"NOTIFICATION","message":"New assignment posted"}\n\n

Each event:
  data: [content]\n\n      (double newline = event separator)
  event: [type]\n           (optional custom event type)
  id: [id]\n                (optional event ID for reconnect)
  retry: [ms]\n             (optional reconnect delay)
```

### SSE in the Browser

```javascript
const source = new EventSource('https://api.paideon.lk/api/events', {
  withCredentials: true,  // send cookies (for auth)
});

// Default message event
source.addEventListener('message', (event) => {
  const data = JSON.parse(event.data);
  console.log(data);
});

// Custom event type
source.addEventListener('attendance_update', (event) => {
  updateAttendanceDisplay(JSON.parse(event.data));
});

// Connection error / server closed it
source.addEventListener('error', (event) => {
  if (source.readyState === EventSource.CLOSED) {
    console.log('SSE connection closed');
  }
});

// Close manually
source.close();
```

### SSE Automatic Reconnection

```
SSE reconnects automatically if the connection drops.
The browser retries after ~3 seconds by default.

Server sends retry interval:
  retry: 5000\n\n   (retry after 5 seconds)

Last-Event-Id:
  Each event can have an id field.
  On reconnect, browser sends: Last-Event-Id: [last-received-id]
  Server can resume from that point.
  Prevents missing events during reconnect.
```

### SSE in NestJS

```typescript
@Controller('events')
export class EventsController {
  constructor(private readonly eventsService: EventsService) {}

  @Sse('stream')
  @UseGuards(JwtAuthGuard)
  stream(@CurrentUser() user: User): Observable<MessageEvent> {
    return this.eventsService
      .getEventsForUser(user.id, user.schoolId)
      .pipe(map(event => ({ data: JSON.stringify(event) })));
  }
}

// EventsService uses an RxJS Subject
@Injectable()
export class EventsService {
  private subject$ = new Subject<ServerEvent>();

  emit(event: ServerEvent) {
    this.subject$.next(event);
  }

  getEventsForUser(userId: string, schoolId: string): Observable<ServerEvent> {
    return this.subject$.pipe(
      filter(e => e.schoolId === schoolId),
      filter(e => this.isEventVisibleToUser(e, userId)),
    );
  }
}
```

---

## 6. Long Polling — The Old Way

Long polling predates WebSocket and SSE. Understanding it helps you appreciate why the newer approaches exist.

```
REGULAR POLLING:
  Client: GET /api/events (every 5 seconds)
  Server: respond immediately (empty if nothing new)
  Client: GET /api/events (5 seconds later)
  ...

  Problem: high request volume, delays up to poll interval.

LONG POLLING:
  Client: GET /api/events
  Server: HOLDS the request open. Doesn't respond until there's something to say.
  
  [30 seconds pass, no events]
  
  Server: [event happens] → responds with the event
  Client: receives event, immediately makes another request
  Client: GET /api/events
  Server: holds open again...
  
  Effectively: server pushes events with minimal delay.
  One request in flight at all times.
  No timeout: server closes after ~30-60s and client reconnects (keep-alive pattern).

Downsides:
  Each "push" requires a full HTTP round trip.
  Servers hold many open connections with blocked threads.
  (Unless using async I/O — Node.js handles this fine, Java needs async servlet)
  Not truly bidirectional.

Long polling was the standard before WebSockets.
You'll still see it in older integrations and as a WebSocket fallback.
Socket.io falls back to long polling if WebSocket is unavailable.
```

---

## 7. WebSocket vs SSE vs Polling — When to Use Which

```
                    WebSocket       SSE             Polling
──────────────────────────────────────────────────────────────────────
Direction           Bidirectional   Server → Client  Client pull
Protocol            Custom (ws://)  HTTP             HTTP
Reconnect           Manual          Automatic        N/A (each request)
Binary support      Yes             No (text only)   Depends
Browser support     Excellent       Excellent*        Universal
Through proxies     Sometimes tricky Excellent        Excellent
Complexity          Higher          Low               Lowest
Load balancing      Tricky (sticky) Easy              Easy

*SSE not supported in IE/Edge Legacy. All modern browsers: yes.

CHOOSE WebSocket when:
  Bidirectional communication (client also sends events to server)
  Low-latency required (real-time games, collaborative editing, chat)
  High message frequency (100+ messages/second)
  
  Examples:
    Live collaborative document editing
    Real-time multiplayer features
    Chat between students/teachers
    Live quiz/polling (teacher pushes question, students answer)

CHOOSE SSE when:
  Server pushes only (client doesn't need to push back)
  Simplicity is important
  You want automatic reconnect built in
  You're already in an HTTP/2 environment (SSE multiplexes well)
  
  Examples:
    Live dashboard updates (attendance, grade submissions)
    Notification streams
    Progress updates for long operations (report generation)
    Live feed of events

CHOOSE Polling when:
  Updates are infrequent (hourly, daily)
  Infrastructure doesn't support persistent connections (serverless)
  Simplicity is paramount and real-time isn't critical
  
  Examples:
    Checking for new announcements (poll every 5 minutes)
    Syncing data in a background tab
    Anything where a few seconds of delay is acceptable

For Paideon:
  Notifications, attendance live view: SSE (server-only push, simple)
  Future chat feature: WebSocket (bidirectional)
  Report generation status: SSE
  Nothing truly needs polling if SSE is available
```

---

## 8. WebSocket Authentication

WebSocket connections start with an HTTP request — authentication happens there.

```
APPROACH A: Token in query string (not recommended)

wss://api.paideon.lk/ws?token=eyJhbGc...

Problems:
  Token appears in server logs, proxy logs, browser history.
  Tokens in URLs are a security risk.
  
Only acceptable if: token is short-lived (< 30 seconds), one-time use.

─────────────────────────────────────────────────────────────────

APPROACH B: Token in upgrade request header (preferred for custom clients)

GET /ws HTTP/1.1
Authorization: Bearer eyJhbGc...
Upgrade: websocket

Problem: Browser WebSocket API doesn't allow custom headers.
  new WebSocket(url)  → browser controls all headers, you can't add Authorization.
  
Works for: mobile apps (React Native), server-to-server WebSockets, custom clients.
Doesn't work for: browser JavaScript.

─────────────────────────────────────────────────────────────────

APPROACH C: Cookie authentication (recommended for browsers)

If the user has an HttpOnly cookie:
  Browser automatically sends it with the WebSocket upgrade request.
  (Same domain/path rules apply as HTTP cookies.)
  
  Server reads the cookie to authenticate the WebSocket connection.
  
  wss://api.paideon.lk/ws  (no token needed — cookie is sent automatically)
  
  Server:
    Reads Cookie: access_token=... from the upgrade request headers.
    Verifies the token.
    If valid: accepts upgrade.
    If invalid: returns 401, connection rejected.

─────────────────────────────────────────────────────────────────

APPROACH D: Authenticate after connection (first message)

1. Accept any WebSocket connection initially.
2. Client sends first message: { type: "AUTH", token: "eyJhbGc..." }
3. Server verifies token.
4. If valid: mark connection as authenticated, proceed.
5. If invalid: send error, close connection.

Advantage: works with browser WebSocket API (no custom headers needed).
Disadvantage: unauthenticated connections exist briefly, must handle state.

This is what Socket.io's auth option implements:
  const socket = io('wss://api.paideon.lk', {
    auth: { token: accessToken }
  });
  // Socket.io sends this as the first handshake data
```

---

## 9. Heartbeats and Connection Management

Persistent connections face a problem: network equipment (firewalls, NAT, load balancers) silently drops idle connections.

```
The problem:
  WebSocket connection open for 5 minutes with no messages.
  Intermediate firewall sees "no traffic" → closes the TCP connection.
  Server and client still think they're connected.
  Next message fails. Both sides are confused.

Solution: ping/pong heartbeats.

WebSocket has built-in ping/pong frames (opcode 0x9 and 0xA).

Server-initiated heartbeat (recommended):
  Every 30 seconds, server sends a ping frame.
  Client automatically responds with a pong frame.
  If no pong received within 10 seconds → client is dead → close connection.

Client-initiated heartbeat (as backup):
  Client sends a custom "ping" message every 25 seconds.
  Server responds with a "pong" message.
  If no pong within 10 seconds → server is dead → reconnect.
```

### Reconnection with Exponential Backoff

```javascript
class ReconnectingWebSocket {
  private ws: WebSocket | null = null;
  private reconnectDelay = 1000;
  private maxDelay = 30000;
  private shouldReconnect = true;

  constructor(private url: string, private getToken: () => string) {
    this.connect();
  }

  private connect() {
    this.ws = new WebSocket(this.url);

    this.ws.addEventListener('open', () => {
      this.reconnectDelay = 1000;  // reset on successful connect
      // Send auth token as first message
      this.ws!.send(JSON.stringify({ type: 'AUTH', token: this.getToken() }));
    });

    this.ws.addEventListener('close', (event) => {
      if (!this.shouldReconnect) return;
      if (event.code === 1008) return;  // Policy violation — don't reconnect
      
      setTimeout(() => this.connect(), this.reconnectDelay);
      this.reconnectDelay = Math.min(this.reconnectDelay * 2, this.maxDelay);
      // Add jitter to prevent reconnection storms
      this.reconnectDelay += Math.random() * 1000;
    });

    this.ws.addEventListener('message', (event) => {
      // handle message
    });
  }

  disconnect() {
    this.shouldReconnect = false;
    this.ws?.close(1000, 'Client disconnect');
  }
}
```

---

## 10. Scaling WebSockets — The Sticky Session Problem

WebSocket connections are long-lived and stateful (tied to one server process). This breaks horizontal scaling.

```
Without sticky sessions:

Server A holds connection for User 1.
Server B holds connection for User 2.

Teacher marks attendance → event should reach User 1 (on A) AND User 2 (on B).

Server A: knows about User 1's connection. Doesn't know about User 2.
Server B: knows about User 2's connection. Doesn't know about User 1.

How does Server A push to User 2?

SOLUTION 1: Sticky Sessions (simple, limited)
  Load balancer routes each user to the SAME server for the entire session.
  Based on: IP address, cookie, session ID.
  
  Problem: if Server A dies, all its connections die.
  Problem: uneven load distribution.
  Problem: doesn't help with pub/sub across servers.

SOLUTION 2: Redis Pub/Sub (correct approach for distributed systems)
  
  When event occurs → publish to Redis channel.
  All server instances subscribe to relevant Redis channels.
  Each server receives the event → finds connections for relevant users → pushes.
  
  Teacher marks attendance on Server B:
  → Server B publishes to Redis: "attendance:school-abc:class-10A" event
  → Server A is subscribed to "attendance:school-abc:*"
  → Server A receives it → pushes to all its connected users watching class 10A
  
  Socket.io has a Redis adapter that does this automatically:
    import { createAdapter } from '@socket.io/redis-adapter';
    const pubClient = createClient({ url: 'redis://localhost:6379' });
    const subClient = pubClient.duplicate();
    io.adapter(createAdapter(pubClient, subClient));

For Paideon (single server VPS):
  Not needed initially.
  One NestJS process handles all WebSocket connections.
  Plan for Redis adapter when scaling becomes necessary.
```

---

## 11. WebSockets in NestJS with Socket.io

NestJS has first-class support for WebSockets via Gateways.

```typescript
// notifications.gateway.ts
import {
  WebSocketGateway,
  WebSocketServer,
  SubscribeMessage,
  OnGatewayConnection,
  OnGatewayDisconnect,
  MessageBody,
  ConnectedSocket,
} from '@nestjs/websockets';
import { Server, Socket } from 'socket.io';
import { UseGuards } from '@nestjs/common';

@WebSocketGateway({
  cors: {
    origin: ['https://paideon.lk', 'http://localhost:3000'],
    credentials: true,
  },
  namespace: '/ws',
})
export class NotificationsGateway
  implements OnGatewayConnection, OnGatewayDisconnect
{
  @WebSocketServer()
  server: Server;

  async handleConnection(client: Socket) {
    // Authenticate on connection
    const token = client.handshake.auth.token;
    try {
      const payload = this.jwtService.verify(token);
      client.data.user = payload;
      // Join rooms based on user's school/role
      client.join(`school:${payload.schoolId}`);
      client.join(`user:${payload.sub}`);
    } catch {
      client.disconnect(true);  // invalid token → kick
    }
  }

  handleDisconnect(client: Socket) {
    // cleanup if needed
  }

  // Client subscribes to a specific class's events
  @SubscribeMessage('subscribe:class')
  handleClassSubscription(
    @MessageBody() data: { classId: string },
    @ConnectedSocket() client: Socket,
  ) {
    const user = client.data.user;
    // Verify user has access to this class
    client.join(`class:${data.classId}`);
  }

  // Push attendance update to everyone watching a class
  pushAttendanceUpdate(schoolId: string, classId: string, payload: object) {
    this.server
      .to(`school:${schoolId}`)
      .to(`class:${classId}`)
      .emit('attendance:updated', payload);
  }

  // Push notification to a specific user
  pushUserNotification(userId: string, notification: object) {
    this.server.to(`user:${userId}`).emit('notification', notification);
  }
}
```

### Frontend (Next.js + Socket.io client)

```typescript
// hooks/useWebSocket.ts
import { useEffect, useRef } from 'react';
import { io, Socket } from 'socket.io-client';
import { useAuthStore } from '@/stores/auth';

export function useWebSocket() {
  const socket = useRef<Socket | null>(null);
  const { accessToken } = useAuthStore();

  useEffect(() => {
    if (!accessToken) return;

    socket.current = io('https://api.paideon.lk/ws', {
      auth: { token: accessToken },
      reconnectionDelayMax: 10000,
      reconnectionAttempts: Infinity,
    });

    socket.current.on('attendance:updated', (data) => {
      // update attendance state
    });

    socket.current.on('notification', (data) => {
      // show notification
    });

    return () => {
      socket.current?.disconnect();
    };
  }, [accessToken]);

  return socket.current;
}
```

---

## 12. Real-Time Features in Paideon

```
FEATURE                   TRANSPORT   DIRECTION      CHANNEL
──────────────────────────────────────────────────────────────────────
Notifications             SSE or WS   Server→Client  user:${userId}
Attendance live view      WS          Server→Client  class:${classId}
Grade submission alerts   WS          Server→Client  school:${schoolId}
Announcement broadcast    WS          Server→Client  school:${schoolId}
Report generation status  SSE         Server→Client  user:${userId}
Live quiz (future)        WS          Bidirectional  quiz:${quizId}
Teacher-student chat      WS          Bidirectional  conversation:${id}

IMPLEMENTATION PRIORITY:
1. Notifications (most useful, moderate complexity)
   → Socket.io gateway in NestJS
   → Push on: new assignment, grade posted, announcement, schedule change
   
2. Attendance live view (admin/principal dashboard)
   → Subscribe to class channel when admin opens attendance view
   → Unsubscribe when they navigate away
   
3. Report generation progress
   → SSE is sufficient (one-way, progress 0-100%)
   → Client subscribes, server emits progress events

NGINX CONFIGURATION for WebSocket:
  location /ws {
      proxy_pass http://localhost:3001;
      proxy_http_version 1.1;
      proxy_set_header Upgrade $http_upgrade;
      proxy_set_header Connection "upgrade";
      proxy_set_header Host $host;
      proxy_set_header X-Real-IP $remote_addr;
      proxy_read_timeout 3600s;    # Keep alive for 1 hour
      proxy_send_timeout 3600s;
  }
  
  proxy_read_timeout is critical — default is 60s.
  Without it, Nginx closes idle WebSocket connections after 1 minute.
```

---

_Next: [Part 12 — JSON & Data Formats](./Part%2012%20-%20JSON%20%26%20Data%20Formats.md)_