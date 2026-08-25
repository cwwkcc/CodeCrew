> WebSockets enable real-time bidirectional communication — essential for live notifications, chat, collaborative features. But they require a different security model from HTTP. Authentication, message validation, rate limiting, and connection management all work differently.

---

## Table of Contents

1. [How WebSockets Work](#1-how-websockets-work)
2. [WebSocket Authentication](#2-websocket-authentication)
3. [Connection Security — The Upgrade Request](#3-connection-security--the-upgrade-request)
4. [Message Validation](#4-message-validation)
5. [Rate Limiting WebSocket Connections](#5-rate-limiting-websocket-connections)
6. [WebSocket vs HTTP Security Model](#6-websocket-vs-http-security-model)
7. [Cross-Origin WebSocket Attacks (CSWSH)](#7-cross-origin-websocket-attacks-cswsh)
8. [WebSocket in NestJS](#8-websocket-in-nestjs)
9. [Disconnection and Cleanup](#9-disconnection-and-cleanup)

---

## 1. How WebSockets Work

HTTP is request-response: client asks, server answers, connection closes (or is reused). WebSockets upgrade an HTTP connection to a persistent, bidirectional channel.

```
HTTP upgrade handshake:
  Client → Server:
    GET /socket HTTP/1.1
    Host: api.yourschool.lk
    Upgrade: websocket
    Connection: Upgrade
    Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==   ← random base64 value
    Sec-WebSocket-Version: 13

  Server → Client:
    HTTP/1.1 101 Switching Protocols
    Upgrade: websocket
    Connection: Upgrade
    Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=   ← derived from client's key

After 101: the connection is a WebSocket — full duplex TCP channel.
Both sides can send messages at any time without waiting for a request.
```

The WebSocket connection persists until either side closes it or the connection drops. This is fundamentally different from HTTP's stateless request/response pattern.

---

## 2. WebSocket Authentication

The HTTP upgrade request is the only opportunity to authenticate before establishing the WebSocket connection. After the handshake, you can also authenticate via the first message.

### Method 1: Query Parameter JWT (Common but Imperfect)

```
wss://api.yourschool.lk/socket?token=eyJ...

The token is in the URL — visible in:
  → Nginx/server access logs
  → Browser history
  → Proxy logs
  → Referrer header if the page has external resources

Acceptable for many use cases; avoid for highly sensitive connections.
```

```ts
// NestJS WebSocket Gateway
@WebSocketGateway({
  cors: { origin: process.env.ALLOWED_ORIGIN, credentials: true },
})
export class NotificationGateway {
  @WebSocketServer()
  server: Server;

  async handleConnection(client: Socket) {
    const token = client.handshake.query.token as string;

    if (!token) {
      client.emit("error", { message: "Authentication required" });
      client.disconnect(true);
      return;
    }

    try {
      const payload = this.jwtService.verify(token);
      client.data.user = payload; // attach user to socket for later use
      client.data.schoolId = payload.schoolId;

      // Subscribe client to their school's room
      client.join(`school:${payload.schoolId}`);
      client.emit("connected", { userId: payload.sub });
    } catch {
      client.emit("error", { message: "Invalid token" });
      client.disconnect(true);
    }
  }
}
```

### Method 2: Cookie Authentication (More Secure)

If the frontend is same-origin, the browser automatically sends cookies with the WebSocket upgrade request. This is the most secure approach — cookies are httpOnly and don't appear in URLs.

```ts
async handleConnection(client: Socket) {
  // Cookies are in the handshake headers
  const cookies = client.handshake.headers.cookie;
  const accessToken = parseCookie(cookies)["accessToken"];  // if stored in cookie

  // Or use the refresh token cookie to validate the session
  // (depends on your auth architecture)
}
```

### Method 3: First Message Authentication

Connect unauthenticated → first message must be an `authenticate` message with the token → server validates → subsequent messages allowed.

```ts
async handleConnection(client: Socket) {
  // Unauthenticated initially
  client.data.authenticated = false;

  // Set timeout: must authenticate within 5 seconds
  const authTimeout = setTimeout(() => {
    if (!client.data.authenticated) {
      client.disconnect(true);
    }
  }, 5000);

  client.data.authTimeout = authTimeout;
}

@SubscribeMessage("authenticate")
async handleAuthenticate(
  @ConnectedSocket() client: Socket,
  @MessageBody() data: { token: string }
) {
  clearTimeout(client.data.authTimeout);

  try {
    const payload = this.jwtService.verify(data.token);
    client.data.user = payload;
    client.data.authenticated = true;
    client.join(`school:${payload.schoolId}`);
    client.emit("authenticated", { success: true });
  } catch {
    client.emit("error", { message: "Authentication failed" });
    client.disconnect(true);
  }
}

// Guard for subsequent messages
@SubscribeMessage("send_message")
async handleMessage(
  @ConnectedSocket() client: Socket,
  @MessageBody() data: unknown
) {
  if (!client.data.authenticated) {
    client.emit("error", { message: "Not authenticated" });
    return;
  }
  // Process message...
}
```

---

## 3. Connection Security — The Upgrade Request

### wss:// Not ws://

Always use `wss://` (WebSocket Secure) — encrypted via TLS. `ws://` sends everything in plaintext.

```
ws://  → unencrypted → anyone on the network can read all messages
wss:// → TLS encrypted → same security as HTTPS
```

If your server is already behind HTTPS/Nginx with TLS termination, the WebSocket connection inherits that TLS.

```nginx
# Nginx WebSocket proxy
location /socket.io/ {
    proxy_pass http://backend:3000;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;    ← required for WebSocket upgrade
    proxy_set_header Connection "upgrade";     ← required for WebSocket upgrade
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_read_timeout 86400s;                 ← keep connection alive
}
```

---

## 4. Message Validation

Unlike HTTP, where the structure of each request is clear from the path and method, WebSocket messages can contain arbitrary data. Every message must be validated.

```ts
// Without validation — dangerous
@SubscribeMessage("update_grade")
async handleUpdateGrade(
  @ConnectedSocket() client: Socket,
  @MessageBody() data: any  // could be anything
) {
  await this.gradeService.update(data.studentId, data.grade);
  // data.grade could be a string, object, negative number, SQL injection, anything
}

// With validation using class-validator
class UpdateGradeDto {
  @IsUUID()
  studentId: string;

  @IsNumber()
  @Min(0)
  @Max(100)
  grade: number;
}

@SubscribeMessage("update_grade")
@UseGuards(WsAuthGuard)
async handleUpdateGrade(
  @ConnectedSocket() client: Socket,
  @MessageBody(new ValidationPipe()) data: UpdateGradeDto
) {
  // data is now validated and typed
  await this.gradeService.update(data.studentId, data.grade, client.data.user);
}
```

### Message Size Limits

```ts
// Socket.io configuration
const io = new Server(httpServer, {
  maxHttpBufferSize: 1e6, // 1 MB max message size
  pingTimeout: 30000,
  pingInterval: 10000,
});
```

Without a size limit, a client can send a 1GB message to exhaust server memory.

---

## 5. Rate Limiting WebSocket Connections

Rate limiting WebSockets requires a different approach than HTTP middleware.

### Connection Rate Limiting

```ts
// In-memory rate limiter for connections per IP
const connectionAttempts = new Map<
  string,
  { count: number; resetAt: number }
>();

@WebSocketGateway()
export class AppGateway {
  async handleConnection(client: Socket) {
    const ip = client.handshake.address;
    const now = Date.now();
    const windowMs = 60_000; // 1 minute
    const maxConnections = 10; // max 10 connections per minute per IP

    const record = connectionAttempts.get(ip);

    if (record && record.resetAt > now) {
      if (record.count >= maxConnections) {
        client.emit("error", { message: "Too many connections" });
        client.disconnect(true);
        return;
      }
      record.count++;
    } else {
      connectionAttempts.set(ip, { count: 1, resetAt: now + windowMs });
    }

    // ... proceed with authentication
  }
}
```

### Message Rate Limiting

```ts
// Per-socket message rate limiting
const RATE_LIMIT_WINDOW = 10_000; // 10 seconds
const RATE_LIMIT_MAX = 20;        // 20 messages per 10 seconds

@SubscribeMessage("*")  // intercept all messages (middleware pattern)
async rateLimitMiddleware(client: Socket, data: any): Promise<boolean> {
  const now = Date.now();

  if (!client.data.rateLimit) {
    client.data.rateLimit = { count: 0, windowStart: now };
  }

  const rl = client.data.rateLimit;

  if (now - rl.windowStart > RATE_LIMIT_WINDOW) {
    rl.count = 0;
    rl.windowStart = now;
  }

  rl.count++;

  if (rl.count > RATE_LIMIT_MAX) {
    client.emit("error", { message: "Rate limit exceeded" });
    return false;
  }

  return true;
}
```

### Using a Guard for Per-Event Rate Limiting

```ts
@Injectable()
export class WsRateLimitGuard implements CanActivate {
  canActivate(context: ExecutionContext): boolean {
    const client: Socket = context.switchToWs().getClient();
    // Check rate limit, return false to block the message
    return this.checkRateLimit(client);
  }
}

@SubscribeMessage("send_notification")
@UseGuards(WsRateLimitGuard)
async handleNotification() { ... }
```

---

## 6. WebSocket vs HTTP Security Model

```
HTTP:
  Each request is independent
  Auth checked per-request (JWT verified every time)
  Stateless — no persistent connection
  Rate limiting: per-request at middleware level
  CSRF: cookies automatically attached → vulnerability

WebSocket:
  Authentication happens ONCE at connection time
  The connection persists — token can expire mid-session
  Stateful — server tracks connected clients
  Rate limiting: per-connection or per-message (custom implementation)
  CSRF: WebSocket connections check Origin header (different mechanism)
```

### Handling Token Expiry During a WebSocket Session

```ts
// Problem: user connects with a 15-min JWT, but stays connected for 2 hours
// Solution: check token validity periodically OR handle on the client

// Option 1: Server-side token refresh over WebSocket
@SubscribeMessage("refresh_token")
async handleTokenRefresh(
  @ConnectedSocket() client: Socket,
  @MessageBody() data: { refreshToken: string }
) {
  const newAccessToken = await this.authService.refreshViaCookie(data.refreshToken);
  // Update client's stored user payload
  const payload = this.jwtService.verify(newAccessToken);
  client.data.user = payload;
  client.emit("token_refreshed", { accessToken: newAccessToken });
}

// Option 2: Check token on each message (slight overhead but simpler)
@SubscribeMessage("any_message")
async handleMessage(@ConnectedSocket() client: Socket) {
  const expiry = client.data.user.exp * 1000;
  if (Date.now() > expiry) {
    client.emit("token_expired");
    client.disconnect(true);
    return;
  }
  // process message...
}
```

---

## 7. Cross-Origin WebSocket Attacks (CSWSH)

CSWSH is the WebSocket equivalent of CSRF. A malicious page can open a WebSocket connection to your server if the server doesn't check the Origin.

```
Attack:
  User is logged in to yourschool.lk
  User visits evil.com (in another tab)
  evil.com's JavaScript: new WebSocket("wss://api.yourschool.lk/socket")

  If server doesn't check Origin:
    Connection established
    evil.com's JS can send messages to your server AS the logged-in user
    (if using cookie auth — cookies are sent with WS handshake)
```

```ts
// VULNERABLE — accepts connections from any origin
@WebSocketGateway()

// SECURE — restrict to your domain
@WebSocketGateway({
  cors: {
    origin: process.env.ALLOWED_ORIGIN, // "https://yourschool.lk"
    credentials: true,
  },
})

// In handleConnection — additional check
async handleConnection(client: Socket) {
  const origin = client.handshake.headers.origin;
  const allowedOrigins = [process.env.ALLOWED_ORIGIN];

  if (!allowedOrigins.includes(origin)) {
    client.disconnect(true);
    return;
  }
}
```

**Why Bearer token auth avoids CSWSH:** If you authenticate via the first message (bearer token in message), evil.com's JS can't read your token from memory. The WebSocket connection opens but authentication will fail.

---

## 8. WebSocket in NestJS

Full secure implementation example:

```ts
// notifications.gateway.ts
@WebSocketGateway({
  namespace: "/notifications",
  cors: {
    origin: process.env.ALLOWED_ORIGIN,
    credentials: true,
  },
})
export class NotificationsGateway
  implements OnGatewayInit, OnGatewayConnection, OnGatewayDisconnect
{
  @WebSocketServer()
  server: Server;

  private connectedUsers = new Map<string, string>(); // userId → socketId

  afterInit(server: Server) {
    // Set global options
    server.use((socket, next) => {
      // Custom middleware — runs before handleConnection
      const origin = socket.handshake.headers.origin;
      if (origin !== process.env.ALLOWED_ORIGIN) {
        next(new Error("Invalid origin"));
      } else {
        next();
      }
    });
  }

  async handleConnection(client: Socket) {
    const token = client.handshake.auth.token as string; // client sends: { auth: { token } }

    if (!token) {
      client.disconnect(true);
      return;
    }

    try {
      const payload = this.jwtService.verify<JwtPayload>(token);
      client.data.user = payload;

      // Track user → socket mapping
      this.connectedUsers.set(payload.sub, client.id);

      // Join school room for broadcast messages
      client.join(`school:${payload.schoolId}`);
    } catch {
      client.disconnect(true);
    }
  }

  handleDisconnect(client: Socket) {
    if (client.data.user) {
      this.connectedUsers.delete(client.data.user.sub);
    }
  }

  // Server → specific user
  sendToUser(userId: string, event: string, data: unknown) {
    const socketId = this.connectedUsers.get(userId);
    if (socketId) {
      this.server.to(socketId).emit(event, data);
    }
  }

  // Server → all users in a school
  broadcastToSchool(schoolId: string, event: string, data: unknown) {
    this.server.to(`school:${schoolId}`).emit(event, data);
  }
}
```

---

## 9. Disconnection and Cleanup

WebSocket connections can drop silently (network issues, browser tab closed, device sleep). The server must detect and clean up stale connections.

```ts
// Heartbeat / ping-pong to detect dead connections
// Socket.io handles this automatically via pingInterval/pingTimeout config

const io = new Server(httpServer, {
  pingInterval: 10000,  // send ping every 10 seconds
  pingTimeout: 5000,    // wait 5 seconds for pong response
                        // if no pong → disconnect
});

// For your own heartbeat (if needed):
@SubscribeMessage("ping")
handlePing(@ConnectedSocket() client: Socket) {
  client.data.lastPing = Date.now();
  client.emit("pong");
}

// Cleanup in handleDisconnect
handleDisconnect(client: Socket) {
  // Clean up any resources associated with this connection
  // Examples:
  //   Remove from active presence tracking
  //   Release any locks the client held
  //   Decrement active connection counters
  //   Log disconnection for analytics

  if (client.data.user) {
    this.presenceService.setOffline(client.data.user.sub);
    this.connectedUsers.delete(client.data.user.sub);
  }
}
```

---

## Summary

```
WebSockets: persistent bidirectional connection, authenticated at upgrade time

Authentication options:
  Query param token: simple, token in URL (in logs)
  Cookie: secure, browser sends automatically, httpOnly
  First message: flexible, no token in URL, small delay before auth

Always:
  Use wss:// (WebSocket Secure) — not ws://
  Verify token in handleConnection, disconnect if invalid
  Join school room immediately after auth

Message security:
  Validate ALL incoming messages with class-validator DTOs
  Set max message size (maxHttpBufferSize: 1e6 = 1MB)

Rate limiting:
  Connection rate: max connections per IP per window
  Message rate: max messages per socket per window
  Disconnect offenders

CSWSH (WebSocket CSRF):
  Check Origin header on connection
  Restrict CORS to your domain only
  Bearer token auth in first message defeats CSWSH

Token expiry:
  JWTs can expire mid-session
  Refresh via WebSocket message or check on each message

Cleanup on disconnect:
  Remove from presence tracking, clean up resources
  Socket.io handles ping/pong detection of dead connections automatically
```
