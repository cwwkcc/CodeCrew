> Containers are not inherently secure. A misconfigured Docker setup can expose your infrastructure to container escapes, secrets in image layers, privilege escalation, and more. This covers the full picture of hardening Docker for production.

---

## Table of Contents

1. [Container Isolation — What Docker Provides and Doesn't](#1-container-isolation--what-docker-provides-and-doesnt)
2. [Running as Non-Root](#2-running-as-non-root)
3. [Minimal Base Images](#3-minimal-base-images)
4. [Secrets in Docker — The Wrong Ways and the Right Way](#4-secrets-in-docker--the-wrong-ways-and-the-right-way)
5. [Multi-Stage Builds](#5-multi-stage-builds)
6. [Image Scanning](#6-image-scanning)
7. [Read-Only Filesystems](#7-read-only-filesystems)
8. [Resource Limits](#8-resource-limits)
9. [Network Security](#9-network-security)
10. [Production Dockerfile for NestJS](#10-production-dockerfile-for-nestjs)

---

## 1. Container Isolation — What Docker Provides and Doesn't

```
Docker DOES provide:
  → Process isolation (container processes are isolated from host)
  → Filesystem isolation (container has its own filesystem)
  → Network isolation (separate network namespace)
  → Resource isolation (CPU, memory limits)

Docker does NOT provide:
  → Full kernel isolation (containers share the host kernel)
  → Complete privilege separation (root in container ≈ root on host by default)
  → Automatic secrets security
  → Image layer security (sensitive data baked into layers)
```

Container escape attacks exist — they're harder to execute than in VMs, but not impossible. Key principle: assume a compromised container can reach the host if not hardened.

---

## 2. Running as Non-Root

By default, Docker containers run as root (UID 0). Root in a container has the same capabilities as root on the host if not restricted.

```dockerfile
# BAD — running as root (default)
FROM node:20-alpine
WORKDIR /app
COPY . .
RUN npm ci
CMD ["node", "dist/main.js"]
# Process runs as root — if compromised, attacker has host-level access

# GOOD — create and use a non-root user
FROM node:20-alpine
WORKDIR /app

# Create app user and group
RUN addgroup -g 1001 -S appgroup && \
    adduser -u 1001 -S appuser -G appgroup

COPY --chown=appuser:appgroup . .
RUN npm ci --only=production

# Switch to non-root user
USER appuser

CMD ["node", "dist/main.js"]
```

```yaml
# docker-compose.yml — override user even if Dockerfile didn't set it
services:
  api:
    image: paideon-api
    user: "1001:1001" # uid:gid
```

A non-root process that escapes the container has far fewer host privileges than a root process.

---

## 3. Minimal Base Images

Large images have more surface area — more packages = more potential vulnerabilities.

```dockerfile
# Bloated — includes gcc, make, build tools, shell utilities
FROM node:20

# Better — Alpine Linux (~5MB base)
FROM node:20-alpine

# Best for production — Distroless (no shell, no package manager, minimal attack surface)
FROM gcr.io/distroless/nodejs20-debian12

# Size comparison:
# node:20         →  1GB+
# node:20-alpine  →  ~180MB
# distroless      →  ~120MB

# Distroless advantage: no shell means even if attacker gets RCE, they can't run commands
# Distroless disadvantage: harder to debug (no shell to exec into)
```

---

## 4. Secrets in Docker — The Wrong Ways and the Right Way

### WRONG: ENV in Dockerfile

```dockerfile
# NEVER DO THIS
ENV DATABASE_URL=postgresql://user:password@host/db
ENV JWT_SECRET=mysecret123

# Problem: these values are baked into every image layer
# Anyone with access to the image can read them:
docker inspect <image_id>
# → All ENV values visible in JSON output
# → git history of the Dockerfile also contains them
```

### WRONG: COPY secrets into image

```dockerfile
# NEVER DO THIS
COPY .env /app/.env
# .env is now inside the image — anyone who pulls the image gets your secrets
```

### CORRECT: Secrets at runtime

```yaml
# docker-compose.yml — inject from host environment
services:
  api:
    image: paideon-api
    env_file:
      - .env # loaded at runtime, not baked into image
    # OR
    environment:
      - DATABASE_URL=${DATABASE_URL} # from host environment
      - JWT_SECRET=${JWT_SECRET}
```

```bash
# Production: inject from secrets manager
docker run \
  -e DATABASE_URL="$(aws secretsmanager get-secret-value --secret-id paideon/db --query SecretString --output text | jq -r .DATABASE_URL)" \
  -e JWT_SECRET="$(aws secretsmanager get-secret-value --secret-id paideon/jwt --query SecretString --output text | jq -r .JWT_SECRET)" \
  paideon-api
```

### Docker Secrets (Swarm/Kubernetes)

```yaml
# Docker Swarm secrets
docker secret create db_password - < /dev/stdin  # type password, Ctrl+D

services:
  api:
    secrets:
      - db_password
    environment:
      - DB_PASSWORD_FILE=/run/secrets/db_password  # app reads from file
```

---

## 5. Multi-Stage Builds

Multi-stage builds separate the build environment from the runtime environment — only the compiled artifact reaches production.

```dockerfile
# Stage 1: Build
FROM node:20-alpine AS builder
WORKDIR /build

# Copy package files first (layer caching — only re-run npm ci if package.json changes)
COPY package.json package-lock.json ./
RUN npm ci                          # includes devDependencies (TypeScript, etc.)

COPY . .
RUN npm run build                   # compile TypeScript → dist/
RUN npm prune --production          # remove devDependencies

# Stage 2: Runtime
FROM node:20-alpine AS runtime
WORKDIR /app

# Only copy what's needed to run
COPY --from=builder /build/dist ./dist
COPY --from=builder /build/node_modules ./node_modules
COPY --from=builder /build/package.json ./

# Non-root user
RUN addgroup -g 1001 -S app && adduser -u 1001 -S app -G app
USER app

EXPOSE 3000
CMD ["node", "dist/main.js"]

# Result:
# Builder stage: ~500MB (includes TypeScript compiler, source files)
# Runtime image: ~180MB (only compiled JS + production deps)
# No source code in production image
# No build tools in production image (smaller attack surface)
```

---

## 6. Image Scanning

Scan images for known CVEs in the OS packages and Node.js dependencies.

```bash
# Docker Scout (built-in to Docker Desktop)
docker scout cves paideon-api:latest

# Trivy (open source, very comprehensive)
trivy image paideon-api:latest
# Scans: OS packages, npm dependencies, config files

# Snyk
snyk container test paideon-api:latest

# In CI/CD pipeline (GitHub Actions)
- name: Scan image for vulnerabilities
  uses: aquasecurity/trivy-action@master
  with:
    image-ref: paideon-api:latest
    format: sarif
    exit-code: "1"          # fail the build if critical vulns found
    severity: "CRITICAL,HIGH"
```

---

## 7. Read-Only Filesystems

If the application doesn't need to write to its own filesystem, make it read-only:

```yaml
services:
  api:
    image: paideon-api
    read_only: true # container filesystem is read-only
    tmpfs:
      - /tmp # app may need /tmp for temp files — mount as tmpfs (in-memory)
    volumes:
      - uploads:/app/uploads # only this directory is writable
```

If an attacker gains code execution in the container, they can't modify the application binaries or write new scripts to the filesystem.

---

## 8. Resource Limits

Without limits, a compromised container can consume all host resources (denial of service, resource exhaustion).

```yaml
services:
  api:
    image: paideon-api
    deploy:
      resources:
        limits:
          cpus: "1.0"        # max 1 CPU core
          memory: 512M       # max 512MB RAM
        reservations:
          cpus: "0.25"       # guaranteed 0.25 CPU
          memory: 128M       # guaranteed 128MB RAM

# OR in docker run:
docker run \
  --memory="512m" \
  --cpus="1.0" \
  --pids-limit=100 \  # max 100 processes (prevents fork bombs)
  paideon-api
```

---

## 9. Network Security

```yaml
services:
  api:
    networks:
      - frontend # faces the internet (via Nginx reverse proxy)
      - backend # connects to database

  db:
    networks:
      - backend # only on backend network, not reachable from internet
    # db is NOT in frontend network → can't be accessed except by services in backend network

  nginx:
    networks:
      - frontend
    ports:
      - "80:80"
      - "443:443"

networks:
  frontend:
    driver: bridge
  backend:
    driver: bridge
    internal: true # no outbound internet access for backend network
```

---

## 10. Production Dockerfile for NestJS

```dockerfile
# Production-ready NestJS Dockerfile
FROM node:20-alpine AS deps
WORKDIR /app
COPY package.json package-lock.json ./
RUN npm ci

FROM node:20-alpine AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY . .
RUN npm run build
RUN npm prune --production

FROM node:20-alpine AS runner
WORKDIR /app

# Security: create non-root user
RUN addgroup -g 1001 -S nodejs && \
    adduser -u 1001 -S nestjs -G nodejs

# Copy only necessary files
COPY --from=builder --chown=nestjs:nodejs /app/dist ./dist
COPY --from=builder --chown=nestjs:nodejs /app/node_modules ./node_modules
COPY --from=builder --chown=nestjs:nodejs /app/package.json ./

# Switch to non-root
USER nestjs

# Health check
HEALTHCHECK --interval=30s --timeout=5s --start-period=10s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3000/health', (r) => process.exit(r.statusCode === 200 ? 0 : 1))"

EXPOSE 3000
CMD ["node", "dist/main.js"]
```

---

## Summary

```
Container isolation: process/network/filesystem, but shared kernel
  → Root in container ≈ root on host → always run as non-root

Non-root user: adduser in Dockerfile, USER instruction, user: in compose
Minimal images: alpine (~180MB) or distroless (~120MB)
  → Smaller attack surface, fewer vulnerable packages

Secrets — never in image:
  → No ENV with secrets in Dockerfile
  → No COPY of .env into image
  → Inject at runtime via env_file, environment vars, or secrets manager

Multi-stage builds:
  → Source code and build tools don't reach production image
  → Only compiled JS + production dependencies

Image scanning: Trivy or Snyk — fail CI on critical vulnerabilities
Read-only filesystem: prevents attacker from writing to container FS
Resource limits: --memory, --cpus, --pids-limit
Network segmentation: separate networks for frontend and backend services
```
