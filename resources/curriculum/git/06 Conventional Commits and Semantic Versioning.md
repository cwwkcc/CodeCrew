Conventional Commits is a specification for writing structured commit messages. Semantic Versioning is a standard for version numbers. Together they enable automated changelogs, automatic version bumping, and clear communication about what changed and whether it is safe to update.

---

## Table of Contents

1. [Conventional Commits]
2. [Type Reference]
3. [Scopes]
4. [Breaking Changes]
5. [Semantic Versioning]
6. [How Commits Map to Version Bumps]
7. [Tooling]
8. [Commitlint and Husky]

---

## 1. Conventional Commits

### Format

```
<type>(<optional scope>): <short description>

<optional body>

<optional footer>
```

```
feat(auth): add JWT refresh token rotation

Replace single long-lived access token with short-lived access tokens
(15 minutes) and rotating refresh tokens (7 days). This limits the
blast radius of a token leak.

BREAKING CHANGE: clients must now handle 401 responses by requesting
a new access token using the refresh token endpoint.
Closes #142
```

### Rules

- Type and description are **required**
- First line (header) must be ≤ 72 characters
- Use the imperative mood: "add feature" not "added feature" or "adds feature"
- Body and footer are optional but valuable
- Blank line between header, body, and footer

---

## 2. Type Reference

|Type|When to use|Version bump|
|---|---|---|
|`feat`|New feature for the user|Minor (`1.0.0` → `1.1.0`)|
|`fix`|Bug fix for the user|Patch (`1.0.0` → `1.0.1`)|
|`docs`|Documentation only|None|
|`style`|Formatting, whitespace — no logic change|None|
|`refactor`|Code restructuring — no feature change, no bug fix|None|
|`perf`|Performance improvement|Patch|
|`test`|Adding or fixing tests|None|
|`build`|Build system, dependencies, CI config|None|
|`ci`|CI/CD configuration changes|None|
|`chore`|Maintenance tasks (updating lockfile, tools)|None|
|`revert`|Reverts a previous commit|Depends|

```bash
# Real examples
feat(dashboard): add weekly progress chart
fix(auth): prevent token reuse after logout
docs(api): add rate limiting section to README
refactor(db): extract query builder into separate module
perf(images): convert profile photos to WebP on upload
test(auth): add token expiry edge case tests
build(deps): bump express from 4.18.0 to 4.19.0
ci(github): add Node 20 to test matrix
chore: update .gitignore for Vite build artifacts
```

---

## 3. Scopes

Scopes are optional but recommended for larger codebases. They narrow down which part of the codebase was changed:

```bash
feat(auth): ...         # authentication module
feat(dashboard): ...    # dashboard feature
fix(api/users): ...     # users route in API
docs(readme): ...       # README file
build(docker): ...      # Docker configuration
```

A project should settle on a consistent set of scope names and use them across all commits. Common conventions:

```
# By module
feat(auth):
feat(courses):
feat(payments):

# By layer
fix(api):
fix(ui):
fix(db):

# By file
docs(readme):
style(eslint):
```

---

## 4. Breaking Changes

A breaking change is any change that requires users (or dependent services) to update their code. It must be explicitly marked.

Two ways to signal a breaking change:

### 1. Footer with `BREAKING CHANGE:`

```
feat(api): change authentication to bearer tokens

Replace cookie-based auth with Authorization header bearer tokens
for all API endpoints.

BREAKING CHANGE: all API clients must include
  Authorization: Bearer <token>
instead of sending a session cookie.
Closes #89
```

### 2. Exclamation mark after type

```
feat(api)!: change authentication to bearer tokens
```

Both trigger a **major** version bump (`1.4.2` → `2.0.0`).

```bash
# Examples of breaking change commits
feat(api)!: remove deprecated v1 endpoints
refactor(auth)!: replace session tokens with JWT
feat(db)!: change user ID format from integer to CUID
```

---

## 5. Semantic Versioning

Version numbers follow the format: `MAJOR.MINOR.PATCH`

```
1.4.2
│ │ └── PATCH — backwards-compatible bug fix
│ └──── MINOR — backwards-compatible new feature
└────── MAJOR — breaking change
```

### Rules

|When to increment|What it means|
|---|---|
|`PATCH` (1.0.0 → 1.0.1)|Bug fixed. Safe to update.|
|`MINOR` (1.0.0 → 1.1.0)|New feature added. Safe to update.|
|`MAJOR` (1.0.0 → 2.0.0)|Breaking change. Check migration guide before updating.|

### Pre-release Versions

```
1.0.0-alpha.1      early, unstable
1.0.0-beta.1       feature-complete, testing
1.0.0-rc.1         release candidate — stable unless critical bug found
1.0.0              stable release
```

### Version `0.x.y`

While `MAJOR` is 0, anything may change. `0.x.y` signals the API is not yet stable. Breaking changes do not require a major bump.

---

## 6. How Commits Map to Version Bumps

When automated release tools (like `semantic-release`) analyse commit history, they determine the next version by looking at the highest-impact commit since the last release:

```
Commits since last release:
  fix(auth): correct token expiry check        → PATCH
  feat(dashboard): add weekly chart             → MINOR
  docs: update API reference                    → none

Highest impact: feat → MINOR bump
Previous version: 1.3.1
New version: 1.4.0
```

```
Commits since last release:
  feat(api)!: remove v1 endpoints              → MAJOR (breaking)
  feat(auth): refresh token rotation           → MINOR
  fix(login): null check on response           → PATCH

Highest impact: BREAKING → MAJOR bump
Previous version: 1.4.0
New version: 2.0.0
```

---

## 7. Tooling

### `semantic-release`

Fully automated version management and package publishing:

```bash
npm install --save-dev semantic-release
```

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    branches: [main]

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: '20'
      - run: npm ci
      - run: npx semantic-release
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
          NPM_TOKEN: ${{ secrets.NPM_TOKEN }}
```

On every push to main, `semantic-release`:

1. Analyses commit messages since the last tag
2. Determines the next version number
3. Generates a changelog
4. Creates a GitHub release with release notes
5. Publishes to npm (if configured)
6. Tags the commit

### `conventional-changelog`

Generate a changelog manually:

```bash
npx conventional-changelog-cli -p conventional -i CHANGELOG.md -s
```

### `standard-version`

Bump version and generate changelog without automated publishing:

```bash
npm install --save-dev standard-version

# Bump version based on commits
npx standard-version

# First release
npx standard-version --first-release

# Force a specific bump
npx standard-version --release-as major
npx standard-version --release-as minor
npx standard-version --release-as patch
```

---

## 8. Commitlint and Husky

Enforce conventional commits locally using commitlint + Husky (Git hooks manager).

### Setup

```bash
# Install
npm install --save-dev @commitlint/cli @commitlint/config-conventional husky

# Initialise Husky
npx husky init
```

### Configure commitlint

```js
// commitlint.config.js
export default {
  extends: ['@commitlint/config-conventional'],
  rules: {
    'type-enum': [
      2,          // 2 = error
      'always',
      ['feat', 'fix', 'docs', 'style', 'refactor', 'perf', 'test', 'build', 'ci', 'chore', 'revert'],
    ],
    'subject-max-length': [2, 'always', 72],
    'subject-case': [2, 'always', 'lower-case'],
  },
};
```

### Add the commit-msg hook

```bash
echo "npx --no -- commitlint --edit \$1" > .husky/commit-msg
chmod +x .husky/commit-msg
```

Now every `git commit` runs commitlint. A non-conventional message is rejected:

```bash
git commit -m "fix stuff"
# ✖ subject may not be empty [subject-empty]
# ✖ type may not be empty [type-empty]
# ✖ found 2 problems, 0 warnings
# (hint: try "git commit --edit" to fix the message)
```

### Pre-commit Hook (lint + test before commit)

```bash
# .husky/pre-commit
npm run lint
npm run test -- --passWithNoTests
```

```json
// package.json
{
  "scripts": {
    "lint": "eslint . --ext .ts,.tsx",
    "test": "vitest run"
  }
}
```

---