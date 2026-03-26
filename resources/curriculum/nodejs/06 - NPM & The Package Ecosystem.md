> npm is the default package manager for Node.js and the world's largest software registry. Every project you build will use it. This part covers how packages and `node_modules` actually work, how to read and manage `package.json`, the difference between dependency types, semantic versioning, and how to keep a project's dependencies clean and secure.

---

## Table of Contents

1. [What npm Is]
2. [package.json — The Project Manifest]
3. [Installing Packages]
4. [node_modules — How It Works]
5. [package-lock.json — Reproducible Installs]
6. [Dependency Types]
7. [Semantic Versioning]
8. [Version Ranges in package.json]
9. [npm Scripts]
10. [Publishing a Package]
11. [Security — Auditing Dependencies]
12. [pnpm — A Better npm]

---

## 1. What npm Is

npm has three distinct meanings:

```
1. The registry:   https://registry.npmjs.org
                   A database of over 2 million JavaScript packages.
                   When you install a package, npm downloads it from here.

2. The CLI tool:   The 'npm' command that ships with Node.js.
                   Used to install, update, publish, and manage packages.

3. The company:    npm Inc., now owned by GitHub (Microsoft).
                   Maintains the registry and CLI.
```

You interact with npm primarily through the CLI and `package.json`.

---

## 2. package.json — The Project Manifest

`package.json` is the configuration file for a Node.js project. It describes the project and its dependencies.

### Initialising a Project

```bash
npm init          # interactive — asks questions
npm init -y       # skip all questions, accept defaults
```

### A Full package.json

```json
{
  "name": "my-api",
  "version": "1.0.0",
  "description": "A REST API for managing users",
  "main": "dist/index.js",
  "type": "commonjs",
  "scripts": {
    "dev":       "tsx watch src/index.ts",
    "build":     "tsc",
    "start":     "node dist/index.js",
    "test":      "jest",
    "lint":      "eslint src"
  },
  "keywords": ["api", "rest", "node"],
  "author": "S.C. Roshana",
  "license": "MIT",
  "dependencies": {
    "express":   "^4.18.2",
    "zod":       "^3.22.0"
  },
  "devDependencies": {
    "typescript": "^5.3.0",
    "tsx":        "^4.6.0",
    "@types/express": "^4.17.21",
    "jest":       "^29.7.0"
  },
  "engines": {
    "node": ">=18.0.0"
  }
}
```

### Key Fields

```
"name"        Package name. Must be lowercase, no spaces. Unique on the registry if published.
"version"     Semantic version. Must increment for each published release.
"main"        Entry point for CommonJS: the file loaded when someone does require('your-package').
"type"        "commonjs" (default) or "module" (opt-in to ES Modules for all .js files).
"scripts"     Custom commands runnable with npm run <name>.
"engines"     Declare which Node.js versions this package supports.
"license"     SPDX license identifier. "MIT", "ISC", "Apache-2.0", etc.
```

---

## 3. Installing Packages

```bash
# Install a package and add to dependencies
npm install express
npm i express             # shorthand

# Install a specific version
npm install express@4.18.2

# Install multiple packages at once
npm install express zod prisma

# Install as a dev dependency
npm install --save-dev typescript
npm i -D typescript       # shorthand

# Install globally (available as CLI commands system-wide)
npm install -g typescript
npm install -g tsx

# Install all dependencies listed in package.json
npm install

# Install without devDependencies (for production)
npm install --omit=dev
npm ci                    # clean install — faster, uses package-lock.json exactly
```

### npm ci vs npm install

```
npm install:
  - Reads package.json version ranges
  - Resolves the latest matching version
  - Updates package-lock.json
  - Use during development

npm ci:
  - Reads package-lock.json exactly — no resolution
  - Deletes node_modules first, then installs clean
  - Fails if package-lock.json is missing or out of sync
  - Faster, deterministic
  - Use in CI/CD pipelines and production deployments
```

---

## 4. node_modules — How It Works

When you install a package, npm downloads it into `node_modules/`:

```
my-project/
├── node_modules/
│   ├── express/
│   │   ├── package.json
│   │   ├── index.js
│   │   └── lib/
│   ├── accepts/          ← express's dependency
│   ├── body-parser/      ← express's dependency
│   └── ...
├── package.json
└── src/
```

### Nested node_modules — Version Conflicts

If two packages require different incompatible versions of the same dependency, npm nests the conflicting version:

```
node_modules/
  package-a/
    node_modules/
      lodash/         ← lodash@3 (only for package-a)
  lodash/             ← lodash@4 (for everyone else)
```

### Why node_modules Is Never Committed to Git

`node_modules` can contain thousands of files and hundreds of MB. It is always regeneratable from `package.json` and `package-lock.json`. Always add it to `.gitignore`:

```
# .gitignore
node_modules/
dist/
.env
```

---

## 5. package-lock.json — Reproducible Installs

`package-lock.json` records the exact version of every package (and their dependencies) that was installed. It is the snapshot of your entire dependency tree at a point in time.

```json
{
  "name": "my-api",
  "version": "1.0.0",
  "lockfileVersion": 3,
  "packages": {
    "node_modules/express": {
      "version": "4.18.2",
      "resolved": "https://registry.npmjs.org/express/-/express-4.18.2.tgz",
      "integrity": "sha512-...",
      "dependencies": {
        "accepts": "~1.3.8",
        ...
      }
    }
  }
}
```

**Always commit `package-lock.json` to git.** Without it:

- Two developers running `npm install` at different times may get different versions
- A CI machine may install different packages than your local machine
- A bug introduced by a transitive dependency update may appear on one machine but not another

---

## 6. Dependency Types

```
dependencies      Packages required at runtime — your production code needs them.
                  Example: express, prisma, zod

devDependencies   Packages only needed during development or build.
                  Not installed in production (npm install --omit=dev).
                  Example: typescript, eslint, jest, tsx

peerDependencies  Packages your library expects the consumer to provide.
                  Used when writing plugins or libraries.
                  Example: a React component library listing react as a peerDependency

optionalDependencies  Like dependencies, but npm won't fail if they can't be installed.
                  Used for platform-specific optional enhancements.
```

### The Rule

```
Ask: "Does my production server need this to run?"
  Yes → dependencies
  No  → devDependencies
```

TypeScript, build tools, test runners, and linters are always `devDependencies`. Their output is what goes to production, not the tools themselves.

---

## 7. Semantic Versioning

Every npm package version is a three-part number: `MAJOR.MINOR.PATCH`.

```
MAJOR  Breaking changes — existing code may break when upgrading.
       Example: express 4 → express 5 (different API in places)

MINOR  New features, backwards-compatible — existing code should still work.
       Example: express 4.17.0 → 4.18.0 (new features added)

PATCH  Bug fixes, backwards-compatible — safe to upgrade.
       Example: express 4.18.1 → 4.18.2 (bug fixed)
```

### Pre-release Tags

```
1.0.0-alpha.1    Early, unstable preview
1.0.0-beta.2     Feature-complete but may have bugs
1.0.0-rc.1       Release candidate — nearly final
```

---

## 8. Version Ranges in package.json

```
"express": "4.18.2"     Exact version. No automatic updates.

"express": "^4.18.2"    Caret. Accept MINOR and PATCH updates.
                        ≥ 4.18.2, < 5.0.0
                        This is the default when you npm install.

"express": "~4.18.2"    Tilde. Accept PATCH updates only.
                        ≥ 4.18.2, < 4.19.0

"express": "*"          Any version. Never use in production.
"express": ">=4.0.0"    Greater than or equal to.
"express": "4.x"        4.anything.

"express": "4.18.2 || 5.0.0"   Either version exactly.
```

### Recommendation

```
For applications (not libraries):
  Use ^ (caret) — accepts non-breaking updates.
  Rely on package-lock.json to pin the exact version per install.

For libraries you publish:
  Use exact versions or tight ranges in dependencies.
  Be conservative — your library runs in other people's projects alongside other packages.
```

---

## 9. npm Scripts

`scripts` in `package.json` are shortcuts for command-line commands. Run them with `npm run <name>`.

```json
{
  "scripts": {
    "dev":     "tsx watch src/index.ts",
    "build":   "tsc --project tsconfig.build.json",
    "start":   "node dist/index.js",
    "test":    "jest --coverage",
    "lint":    "eslint src --ext .ts",
    "format":  "prettier --write src"
  }
}
```

```bash
npm run dev
npm run build
npm run test

# 'start' and 'test' are special — no 'run' needed
npm start
npm test
```

### Pre and Post Hooks

npm automatically runs `pre<name>` before and `post<name>` after a script:

```json
{
  "scripts": {
    "prebuild":  "npm run lint",
    "build":     "tsc",
    "postbuild": "echo Build complete"
  }
}
```

### Passing Arguments to Scripts

```bash
npm run test -- --watch          # passes --watch to jest
npm run lint -- --fix            # passes --fix to eslint
```

---

## 10. Publishing a Package

```bash
# 1. Login to npm
npm login

# 2. Make sure name in package.json is unique
# Check: https://www.npmjs.com/package/<your-name>

# 3. Publish
npm publish

# For scoped packages (name starts with @username/)
npm publish --access public

# 4. Update and republish
npm version patch   # bumps 1.0.0 → 1.0.1
npm version minor   # bumps 1.0.0 → 1.1.0
npm version major   # bumps 1.0.0 → 2.0.0
npm publish
```

### What Gets Published

By default, everything not in `.gitignore` is published. Control it with `.npmignore` or the `files` field:

```json
{
  "files": ["dist", "README.md", "LICENSE"]
}
```

---

## 11. Security — Auditing Dependencies

Every dependency you add brings its dependencies. A project can have hundreds of indirect packages — each a potential vulnerability.

```bash
# Audit all dependencies for known vulnerabilities
npm audit

# Output looks like:
# found 2 vulnerabilities (1 moderate, 1 high)

# Fix automatically where possible
npm audit fix

# Fix including breaking changes (major version bumps)
npm audit fix --force  # use carefully — may break your code
```

### Common Security Practices

```
1. Run npm audit regularly — before every deployment.
2. Use npm ci in CI/CD — ensures exact, locked versions.
3. Avoid installing packages you don't need.
4. Check a package before installing:
   - How many weekly downloads?
   - When was it last updated?
   - How many open issues?
   - Is the source code readable and maintained?
5. Use Dependabot (GitHub) or Renovate to automate dependency updates.
```

### Checking What a Package Contains Before Installing

```bash
# View package info without installing
npm info express
npm info express version         # latest version
npm info express versions        # all versions
npm info express dependencies    # what it depends on

# View files that would be published (from a local package)
npm pack --dry-run
```

---

## 12. pnpm — A Better npm

pnpm (Performant npm) is an alternative package manager that solves a major problem with npm's approach.

### The Problem with npm's node_modules

```
Project A depends on lodash@4
Project B depends on lodash@4

With npm:
  project-a/node_modules/lodash/  ← full copy
  project-b/node_modules/lodash/  ← another full copy

100 projects, all using lodash → 100 copies of the same files on disk.
```

### How pnpm Solves It

```
pnpm stores all packages in a global content-addressable store.
  ~/.pnpm-store/lodash@4.17.21/...  ← one copy, ever

Each project gets a node_modules/ with hardlinks (or symlinks)
pointing to the store.

project-a/node_modules/lodash → ~/.pnpm-store/lodash@4.17.21/  (hardlink)
project-b/node_modules/lodash → ~/.pnpm-store/lodash@4.17.21/  (hardlink)

No matter how many projects use lodash@4.17.21, it exists once on disk.
```

### pnpm Benefits

```
Disk space:   Dramatically less — shared global store
Speed:        Faster installs — already-downloaded packages are instant
Strictness:   Packages can only access their declared dependencies
              (npm's flat node_modules lets packages use undeclared deps)
Monorepos:    First-class workspace support — the reason Paideon uses it
```

### Basic pnpm Commands

```bash
# Install
pnpm install           # equivalent to npm install
pnpm add express       # equivalent to npm install express
pnpm add -D typescript # equivalent to npm install -D typescript
pnpm remove express    # equivalent to npm uninstall express

# Run scripts
pnpm dev
pnpm build
pnpm run lint

# Workspaces (monorepo)
pnpm --filter api add express   # add to specific package
pnpm -r run build               # run build in all packages
```