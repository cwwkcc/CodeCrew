> The web is built on layers. Before you can understand HTTP, you need to understand TCP. Before TCP, you need to understand IP. Before IP, you need to understand what a packet is. This goes all the way down to physics. This part covers every layer — from electrons on a wire to the connection your browser opens.

---

## Table of Contents

1. [The Physical Layer — How Bits Actually Travel](#1-the-physical-layer--how-bits-actually-travel)
2. [Packets — How Data Is Broken Up](#2-packets--how-data-is-broken-up)
3. [IP Addresses — The Postal System of the Internet](#3-ip-addresses--the-postal-system-of-the-internet)
4. [The OSI Model — A Mental Map](#4-the-osi-model--a-mental-map)
5. [Ports — How One Machine Runs Many Services](#5-ports--how-one-machine-runs-many-services)
6. [DNS — Translating Names to Addresses](#6-dns--translating-names-to-addresses)
7. [TCP — Reliable Delivery](#7-tcp--reliable-delivery)
8. [UDP — Fast, Unreliable Delivery](#8-udp--fast-unreliable-delivery)
9. [TCP vs UDP — When to Use Which](#9-tcp-vs-udp--when-to-use-which)
10. [Sockets — The Programming Interface to the Network](#10-sockets--the-programming-interface-to-the-network)
11. [NAT — Why Your Home Has One Public IP](#11-nat--why-your-home-has-one-public-ip)
12. [The Full Journey — Paideon Request from Sri Lanka](#12-the-full-journey--paideon-request-from-sri-lanka)

---

## 1. The Physical Layer — How Bits Actually Travel

Before any protocol, before any software, data is ultimately physical. A bit is a voltage level, a light pulse, or a radio wave.

```
Medium         How a 1 vs 0 is represented
─────────────────────────────────────────────────────────────
Copper cable   High voltage (~5V) = 1 / Low voltage (~0V) = 0
Fiber optic    Light pulse present = 1 / No light = 0
WiFi (radio)   Phase shift or amplitude variation in radio wave
```

Your data physically travels:

```
Your device
  │
  │ WiFi (radio waves at 2.4GHz or 5GHz)
  ▼
Your home router
  │
  │ Ethernet (copper) or fiber → to your ISP's local node
  ▼
ISP backbone (fiber optic cables, sometimes undersea)
  │
  │ Hops through 10–20 routers along the way
  ▼
Data center
  │
  │ Internal ethernet switching
  ▼
Physical server machine
```

Speed of light in fiber is ~200,000 km/s (2/3 of vacuum speed). The distance from Colombo to a Frankfurt server is ~8,500 km. Minimum latency (round trip): ~85ms just from physics. Real-world round trip: ~180–250ms after routing overhead.

---

## 2. Packets — How Data Is Broken Up

The internet does not send files as a single continuous stream. Every piece of data is broken into small chunks called **packets**.

```
Why packets?

Imagine sending a 100MB video file as one continuous stream:
  → If any part corrupts → entire transfer fails, restart from zero
  → One sender monopolises the entire cable for minutes
  → A broken cable mid-transfer = complete failure

Packets solve this:
  → Break 100MB into ~70,000 packets of ~1,500 bytes each
  → Each packet is independent
  → Packets travel different routes through the network
  → They get reassembled at the destination
  → Only corrupted packets need to be resent, not the whole file
  → Multiple senders share the same cable by interleaving packets
```

A packet has two parts:

```
┌─────────────────────────────────────┐
│           HEADER                    │
│  Source IP: 192.168.1.5             │  "From" address
│  Dest IP:   157.245.23.14           │  "To" address
│  Sequence:  4823                    │  Which chunk this is
│  Checksum:  0x4F2A                  │  Corruption detection
│  TTL:       64                      │  Max hops before discard
├─────────────────────────────────────┤
│           PAYLOAD                   │
│  [up to 1,500 bytes of actual data] │
└─────────────────────────────────────┘
```

**TTL (Time To Live):** Each router that forwards a packet decrements TTL by 1. When TTL reaches 0, the packet is discarded. This prevents lost packets from circling forever. Starting TTL of 64 means a packet can traverse at most 64 routers.

**Checksum:** A mathematical fingerprint of the packet contents. Recipient recomputes it and compares. If they differ, the packet was corrupted in transit.

---

## 3. IP Addresses — The Postal System of the Internet

Every device on a network needs an address. IP (Internet Protocol) provides that addressing system.

### IPv4

```
Format: four octets separated by dots
Example: 157.245.23.14

Each octet: 0–255 (8 bits)
Total: 32 bits = 2^32 = ~4.3 billion possible addresses

We ran out in 2011.
IANA allocated the last blocks to regional registries in February 2011.
```

**Special IPv4 ranges (not routable on the public internet):**

```
10.0.0.0/8         Private (home/office networks)
172.16.0.0/12      Private (Docker uses 172.17.0.0/16 by default)
192.168.0.0/16     Private (your home router: 192.168.1.1)
127.0.0.0/8        Loopback (127.0.0.1 = "this machine itself")
0.0.0.0            Means "all interfaces on this machine"
255.255.255.255    Broadcast (send to everyone on local network)
```

When NestJS listens on `0.0.0.0:3001`, it is saying: "Accept connections on any network interface on this machine." When it listens on `127.0.0.1:3001`, it is saying: "Accept connections only from this machine itself."

### IPv6

```
Format: eight groups of four hexadecimal digits, separated by colons
Example: 2001:0db8:85a3:0000:0000:8a2e:0370:7334

Compressed: consecutive groups of zeros can be replaced with ::
Same address: 2001:db8:85a3::8a2e:370:7334

Total: 128 bits = 2^128 = 340 undecillion addresses
We will never run out.

Loopback:  ::1  (equivalent of 127.0.0.1)
```

IPv6 is deployed in parallel with IPv4. Most servers and devices support both (called dual-stack).

### CIDR Notation

You will see addresses written like `192.168.1.0/24`. The number after the slash is the network prefix length.

```
192.168.1.0/24  →  24 bits are the network, 8 bits are the host
                   Network: 192.168.1.x
                   Hosts: 192.168.1.1 to 192.168.1.254
                   Total: 254 usable addresses

10.0.0.0/8      →  8 bits are the network, 24 bits are the host
                   Total: 16,777,214 usable addresses
```

This is how your cloud provider gives your VPS a range, and how your Nginx server can be configured to only accept requests from certain IP blocks.

---

## 4. The OSI Model — A Mental Map

The OSI (Open Systems Interconnection) model is a conceptual framework that describes networking in 7 layers. You will see it referenced everywhere. It is not a protocol — it is a way of thinking.

```
Layer 7 — Application    HTTP, HTTPS, DNS, SMTP, WebSocket
           "What does the data mean?"

Layer 6 — Presentation   Encryption (TLS), compression, encoding
           "How is the data formatted/encrypted?"

Layer 5 — Session        Maintaining a connection over time
           "Is the conversation ongoing?"

Layer 4 — Transport      TCP, UDP
           "How reliably does data get from A to B?"

Layer 3 — Network        IP, routing
           "Which path does the data take?"

Layer 2 — Data Link      Ethernet, WiFi (MAC addresses)
           "How does data move between directly connected devices?"

Layer 1 — Physical       Cables, radio waves, voltages
           "How are bits physically transmitted?"
```

In practice, most web developers work in layers 4–7. When someone says "layer 4 load balancer" they mean a load balancer that distributes traffic based on TCP connections (without looking inside the HTTP). A "layer 7 load balancer" can read HTTP headers and route based on URL path or hostname.

The real-world TCP/IP model collapses OSI into 4 layers:

```
Application   (OSI 5–7) → HTTP, DNS, TLS
Transport     (OSI 4)   → TCP, UDP
Internet      (OSI 3)   → IP
Network       (OSI 1–2) → Ethernet, WiFi
```

---

## 5. Ports — How One Machine Runs Many Services

An IP address identifies a machine. A **port** identifies a specific service on that machine.

```
IP address = the building
Port number = the apartment number

157.245.23.14:443   → the HTTPS server on that machine
157.245.23.14:22    → the SSH server on that machine
157.245.23.14:5432  → the PostgreSQL server on that machine
```

Ports are 16-bit numbers: 0–65535.

**Well-known ports (0–1023):** Reserved for system services. Require root/administrator privileges to listen on.

```
Port   Protocol   Service
──────────────────────────────────────
20     TCP        FTP data
21     TCP        FTP control
22     TCP        SSH
25     TCP        SMTP (email sending)
53     TCP/UDP    DNS
80     TCP        HTTP
443    TCP        HTTPS
465    TCP        SMTP over TLS
587    TCP        SMTP submission
3306   TCP        MySQL
5432   TCP        PostgreSQL
```

**Ephemeral ports (49152–65535):** When your browser makes a connection, the OS assigns it a random source port from this range. This is how the server knows which response goes back to which browser tab.

```
Your browser tab makes a request:
  Source: 192.168.1.5:54321  (your IP, random ephemeral port)
  Dest:   157.245.23.14:443  (server IP, HTTPS port)

Server responds:
  Source: 157.245.23.14:443
  Dest:   192.168.1.5:54321   ← the response goes back to THAT tab

You open a second tab:
  Source: 192.168.1.5:54389  (different ephemeral port)
  Dest:   157.245.23.14:443

Server can now distinguish two simultaneous connections from the same browser
because they have different source ports.
```

**For Paideon:**

```
Nginx:   listening on 0.0.0.0:80   and 0.0.0.0:443
NestJS:  listening on 127.0.0.1:3001  (internal only)
Next.js: listening on 127.0.0.1:3000  (internal only)
Postgres:listening on 127.0.0.1:5432  (internal only)

Only Nginx is exposed to the internet.
Everything else is only accessible from localhost.
```

---

## 6. DNS — Translating Names to Addresses

DNS (Domain Name System) translates human-readable domain names into IP addresses. It is a distributed, hierarchical database.

### The Hierarchy

```
                      . (root)
                      │
          ┌───────────┼───────────┐
         .com        .lk         .org   ← Top-Level Domains (TLDs)
          │           │
      google.com   paideon.lk          ← Second-Level Domains
          │
    www.google.com                     ← Subdomains
    mail.google.com
    api.google.com
```

### The Full Lookup Process

```
You type: paideon.lk

Step 1 — Browser cache
  Browser remembers previous lookups.
  If paideon.lk was looked up recently → use cached IP.
  Each record has a TTL (Time To Live). After TTL expires, re-lookup.

Step 2 — OS cache / hosts file
  OS checks /etc/hosts (Linux/Mac) or C:\Windows\System32\drivers\etc\hosts
  This file can override DNS for any domain.
  Common dev use: 127.0.0.1 myapp.local → makes myapp.local resolve locally.

Step 3 — Recursive resolver (your ISP or 8.8.8.8 or 1.1.1.1)
  If not cached locally, OS asks the configured DNS resolver.
  Cloudflare's 1.1.1.1 is fast and privacy-respecting.
  Google's 8.8.8.8 is universal.

Step 4 — Recursive resolution (if resolver doesn't have it cached)

  Resolver → Root nameserver (13 root servers worldwide, operated by IANA)
    "Who handles .lk?"
  Root → "Ask 192.112.36.4 (the .lk TLD nameserver)"

  Resolver → .lk TLD nameserver
    "Who handles paideon.lk?"
  .lk nameserver → "Ask ns1.cloudflare.com (paideon's nameserver)"

  Resolver → Cloudflare nameserver
    "What is the A record for paideon.lk?"
  Cloudflare → "104.21.45.67" (TTL: 300 seconds)

  Resolver caches this for 300 seconds.
  Returns answer to your OS.
  OS caches it.
  Browser caches it.
  Connection proceeds.
```

### DNS Record Types

```
A       Maps hostname to IPv4 address
        paideon.lk.   → 104.21.45.67

AAAA    Maps hostname to IPv6 address
        paideon.lk.   → 2606:4700::6815:2d43

CNAME   Maps hostname to another hostname (alias)
        www.paideon.lk → paideon.lk
        (browser then looks up paideon.lk)

MX      Mail exchanger — where to deliver email for this domain
        paideon.lk → mail priority 10 → aspmx.l.google.com

TXT     Arbitrary text — used for verification, SPF, DKIM
        paideon.lk → "v=spf1 include:_spf.google.com ~all"

NS      Nameserver — which server handles DNS for this domain
        paideon.lk → ns1.cloudflare.com, ns2.cloudflare.com

SOA     Start of Authority — administrative info about the zone

CAA     Certificate Authority Authorization
        paideon.lk → 0 issue "letsencrypt.org"
        (only Let's Encrypt may issue SSL certs for this domain)
```

### TTL and Propagation

When you change a DNS record, the change doesn't instantly reach everyone. Everyone who cached the old record continues using it until their TTL expires.

```
Old A record:  paideon.lk → 104.21.45.67   TTL: 3600 seconds (1 hour)
You change to: paideon.lk → 157.245.23.14

Anyone who looked up paideon.lk in the last hour still has the old IP cached.
After at most 1 hour, their cache expires and they get the new IP.

"DNS propagation" just means waiting for all cached TTLs to expire.
It is not magic — it is deterministic. Max propagation time = the TTL.

Best practice: lower TTL to 300s (5 min) before a migration,
make the change, wait for full propagation, then raise TTL back.
```

---

## 7. TCP — Reliable Delivery

TCP (Transmission Control Protocol) sits at Layer 4. It takes the best-effort, unreliable delivery of IP and adds reliability guarantees on top.

### What TCP Guarantees

```
1. Delivery     — If you send it, it will arrive (or the connection fails)
2. Order        — Bytes arrive in the exact order they were sent
3. No duplicates — Each byte arrives exactly once
4. Error detection — Corrupted data is detected and retransmitted
```

### The Three-Way Handshake

A TCP connection must be established before any data flows.

```
Client                    Server
  │                         │
  │── SYN (seq=100) ──────▶ │   "I want to connect, my sequence starts at 100"
  │                         │
  │◀── SYN-ACK (seq=200, ── │   "OK. My sequence starts at 200. Got your 100."
  │         ack=101)         │   ack=101 means "I expect byte 101 next"
  │                         │
  │── ACK (ack=201) ──────▶ │   "Got your 200. I expect byte 201 next."
  │                         │
  │   [ Connection open ]   │
  │                         │
  │── [data] ─────────────▶ │
```

This 3-way handshake adds one full round trip before any data can be sent. On a 200ms connection (Colombo → Frankfurt), this is 200ms before HTTP even begins. TLS adds another handshake on top. This is why reducing round trips matters for performance.

### Sequence Numbers and Acknowledgment

```
Client sends: bytes 1–1000 (sequence 1)
Client sends: bytes 1001–2000 (sequence 1001)
Client sends: bytes 2001–3000 (sequence 2001)

Server receives 1–1000 and 2001–3000. Packet 1001–2000 was lost.

Server sends: ACK 1001  ("I got up to 1000, I'm waiting for 1001")

Client retransmits bytes 1001–2000.
Server receives 1001–2000.

Server sends: ACK 3001  ("I now have everything up to 3000, send 3001 next")
```

### Connection Teardown

```
Client               Server
  │── FIN ─────────▶ │   "I'm done sending"
  │◀── ACK ─────────│   "Got it"
  │◀── FIN ─────────│   "I'm done sending too"
  │── ACK ─────────▶│   "Got it"
  │  [Connection closed]
```

### TCP Congestion Control

TCP automatically adjusts sending speed based on network conditions.

```
Slow Start:
  Begin by sending a small amount (congestion window = 1 packet)
  Double the window every round trip: 1 → 2 → 4 → 8 → 16...
  Until a packet is lost (network is congested)

Congestion Avoidance:
  After loss, halve the window size
  Grow linearly from there
  This is called AIMD: Additive Increase, Multiplicative Decrease

Result: TCP probes for available bandwidth without overwhelming the network.
Multiple flows on the same network converge on equal shares automatically.
```

---

## 8. UDP — Fast, Unreliable Delivery

UDP (User Datagram Protocol) is the alternative to TCP at Layer 4. It provides no reliability guarantees at all.

```
UDP = IP + ports + checksum. That's it.

No handshake.
No acknowledgment.
No retransmission.
No ordering.
No congestion control.

You send a packet and hope it arrives.
```

### Why Would Anyone Use This?

```
Scenario: Video call (Zoom, Google Meet)

You're speaking. Your voice is encoded as audio frames.
30 audio frames per second.

If frame #14 is lost:
  TCP approach → retransmit frame #14 → wait for it → get it late
  → now your audio is behind by 100ms → everything is delayed
  → it gets worse and worse → choppy audio

  UDP approach → frame #14 is gone → skip it → play frame #15
  → there's a tiny glitch in audio → nobody notices
  → the call stays in real time

Real-time audio/video: a late packet is worse than a lost packet.
UDP is correct here. TCP would be wrong.
```

UDP use cases:

```
DNS          → Single request-response, fast. If lost, retry at app layer.
Video calls  → Zoom, Google Meet, WebRTC
Online games → Position updates. Stale positions are useless anyway.
Streaming    → Live video (a dropped frame is fine)
HTTP/3       → QUIC protocol (UDP-based, with reliability built on top at app level)
```

---

## 9. TCP vs UDP — When to Use Which

```
                    TCP                     UDP
─────────────────────────────────────────────────────────────
Reliability         Guaranteed              Best effort
Ordering            Guaranteed              Not guaranteed
Speed               Slower (overhead)       Faster (minimal overhead)
Handshake           Required (3-way)        None
When correct        File transfer,          Video calls, games,
                    HTTP, email, SSH,       DNS, live streaming,
                    databases               HTTP/3 (QUIC)
```

HTTP/1.1 and HTTP/2 run over TCP. HTTP/3 runs over QUIC, which is built on UDP — but QUIC implements its own reliability at the application layer. This is not a contradiction: QUIC is reliable, but it implements that reliability itself rather than depending on TCP. The reason is control: QUIC can recover from packet loss per-stream without blocking other streams, which TCP cannot do.

---

## 10. Sockets — The Programming Interface to the Network

A **socket** is the programming abstraction that represents one endpoint of a network connection. It's how your code talks to the network — you write to a socket, the OS handles everything below.

```
When NestJS starts on port 3001, it:

1. Creates a socket:     socket(AF_INET, SOCK_STREAM, 0)
                         AF_INET   = IPv4
                         SOCK_STREAM = TCP (stream-based)

2. Binds to an address:  bind(socket, {IP: "127.0.0.1", port: 3001})

3. Listens for connections: listen(socket, backlog=511)
   backlog = how many pending connections to queue up

4. Accepts connections:  accept(socket) → returns new_socket
   This blocks until a client connects.
   Returns a new socket representing THIS specific connection.
   The original socket continues listening for new connections.

5. Reads from connection: recv(new_socket, buffer, 4096)
6. Writes to connection:  send(new_socket, data, length)
7. Closes connection:     close(new_socket)
```

Node.js abstracts all this. You call `app.listen(3001)` and the framework handles the socket lifecycle. But underneath, this is exactly what happens.

**A socket is identified by a 4-tuple:**

```
(source IP, source port, dest IP, dest port)

This 4-tuple uniquely identifies every connection.
Two connections from the same client have different source ports.
The OS uses this tuple to route incoming packets to the right socket.
```

---

## 11. NAT — Why Your Home Has One Public IP

Your home router has one public IP (assigned by your ISP). But you have many devices: laptop, phone, tablet, smart TV. They all have private IPs (192.168.x.x). How do they all share one public IP?

**NAT = Network Address Translation**

```
Your laptop: 192.168.1.5
Your phone:  192.168.1.6
Public IP:   203.45.67.89  (assigned to your router by ISP)

Laptop requests paideon.lk:
  Original packet:  192.168.1.5:54321  →  104.21.45.67:443

  Router translates:
    Rewrites source: 203.45.67.89:44921  →  104.21.45.67:443
    Records in NAT table: {44921 → 192.168.1.5:54321}

  Response comes back to: 203.45.67.89:44921

  Router looks up NAT table:
    44921 → 192.168.1.5:54321
  Forwards to your laptop: 192.168.1.5:54321

Phone requests google.com:
  Router uses a different ephemeral port: 44922
  Records: {44922 → 192.168.1.6:55123}

The server sees both requests coming from 203.45.67.89
with different source ports. It cannot tell they are different devices.
```

This is why servers cannot rely on IP addresses for user identification — many users can share one IP (office network, university, mobile carrier). Your rate limiter on NestJS sees a university's entire student body as one IP if they use the university proxy.

---

## 12. The Full Journey — Paideon Request from Sri Lanka

Putting it all together. A student in Colombo opens `https://paideon.lk/login`.

```
Step 1 — DNS Resolution
  Browser checks cache: not found
  OS checks /etc/hosts: not found
  OS asks DNS resolver (Cloudflare 1.1.1.1):
    1.1.1.1 → .lk TLD → Cloudflare NS → returns 104.21.45.67
  Browser now knows: paideon.lk = 104.21.45.67

Step 2 — TCP Handshake (to Cloudflare, ~15ms — nearest edge)
  SYN → SYN-ACK → ACK
  TCP connection established to 104.21.45.67:443

Step 3 — TLS Handshake (~15ms, 1 round trip in TLS 1.3)
  ClientHello → ServerHello + Certificate + Keys → ACK
  Encrypted session established

Step 4 — HTTP Request (encrypted)
  GET /login HTTP/1.1
  Host: paideon.lk
  [other headers]

Step 5 — Cloudflare → Your VPS in Frankfurt (~150ms network)
  Cloudflare decrypts the HTTPS from the student
  Re-encrypts (or plain HTTP on internal link) → forwards to VPS
  VPS receives the request on Nginx port 443

Step 6 — Nginx → Next.js (internal, <1ms)
  Nginx matches path: /login (not /api/) → proxy to localhost:3000

Step 7 — Next.js renders /login page
  Server-side renders the React component
  Returns HTML

Step 8 — Response travels back
  Next.js → Nginx → Cloudflare (encrypts) → student's browser

Step 9 — Browser renders HTML
  Parses HTML → requests CSS and JS bundles
  Render → React hydration → page is interactive

Total time: ~300–400ms for first load (DNS + TCP + TLS + network + render)
Subsequent requests: DNS cached, TCP reused (keep-alive) → ~150–200ms
```

---

_Next: [Part 02 — HTTP Deep Dive](01%20-%20HTTP%20Deep%20Dive.md)_
