A Git workflow is a convention for how a team uses branches, merges, and releases. The right workflow depends on team size, deployment model, and release cadence. The two most common modern approaches are Feature Branch Workflow and Trunk-Based Development.

---

## Table of Contents

1. [Feature Branch Workflow]
2. [Trunk-Based Development]
3. [Git Flow]
4. [Comparing Workflows]
5. [Branch Naming Conventions]
6. [Release Strategies]
7. [Protecting main]

---

## 1. Feature Branch Workflow

Every feature, fix, or change lives on a dedicated branch. Changes reach `main` only through Pull Requests.

### The Cycle

```
main ─────────────────────────────────────────────────► (always deployable)
         │                         ↑
         │ git checkout -b         │ PR merged
         ▼                         │
      feature/user-search ─────────┘
         make commits
         push
         open PR
         code review
         CI passes
         merge
         delete branch
```

### Step by Step

```bash
# 1. Always start from an updated main
git checkout main
git pull

# 2. Create a feature branch
git checkout -b feat/user-search

# 3. Work in small, focused commits
git add -p
git commit -m "feat(search): add search endpoint"
git commit -m "feat(search): add debounce to search input"
git commit -m "test(search): add search result rendering tests"

# 4. Keep your branch up to date with main (rebase = clean history)
git fetch origin
git rebase origin/main

# 5. Push and open PR
git push -u origin feat/user-search
gh pr create --title "feat: user search" --body "..."

# 6. Address review comments
git add -p
git commit -m "fix(search): handle empty query string"
git push

# 7. After approval and CI pass — merge (squash on GitHub, or locally)
# Squash merge via GitHub UI: "Squash and merge"

# 8. Clean up
git checkout main
git pull
git branch -d feat/user-search
```

### Rules

- `main` is always releasable — never commit broken code to it
- Branches are short-lived — a few hours to a few days, not weeks
- PRs require at least one review approval before merging
- CI must pass before merging

---

## 2. Trunk-Based Development

Everyone commits directly to `main` (the "trunk") — or via very short-lived branches (1 day maximum). The key enabler is **feature flags** — deploy incomplete features behind a flag that is off in production.

```
main (trunk) ──────────────────────────────────►
  ↑   ↑   ↑   ↑   ↑   ↑   ↑   ↑   ↑   ↑   ↑
  │   │   │   │   │   │   │   │   │   │   │
  small commits, multiple times per day
  every commit triggers CI/CD
  deploy continuously
```

### Feature Flags

```typescript
// Feature not visible to users until flag is enabled
if (featureFlags.isEnabled('user-search', userId)) {
  return <UserSearch />;
}

return null;
```

```bash
# Even large features are committed incrementally
git commit -m "feat(search): add search index (flag: user-search, OFF)"
git commit -m "feat(search): add search UI behind flag"
git commit -m "feat(search): enable search for beta users"
git commit -m "feat(search): enable search globally, remove flag"
```

### Why Trunk-Based Development Works

- Eliminates long-lived branches and merge conflicts
- Forces small, incremental changes
- CI/CD runs on every commit — problems found immediately
- No "integration hell" — code is always integrated

### When It Does Not Work

- Without good test coverage and CI
- Without feature flags for large features
- On teams not disciplined about small commits
- For open source projects with external contributors (use PRs instead)

---

## 3. Git Flow

Git Flow is an older model with multiple long-lived branches. Designed for software with explicit release cycles (versioned software, mobile apps):

```
main        ─────────────────────────────────────────►  (production releases only)
                  ↑             ↑
hotfix/ ──────────┤             │
                  │             │
develop   ─────────────────────────────────────────────►
          ↑           ↑
          └── feature/        release/ ──────────────►
              branches        branches
```

### Branches

|Branch|Purpose|
|---|---|
|`main`|Tagged production releases only|
|`develop`|Integration branch — all features merge here|
|`feature/*`|New features — branch from develop, merge to develop|
|`release/*`|Release preparation — branch from develop, merge to main and develop|
|`hotfix/*`|Emergency fixes — branch from main, merge to main and develop|

### When to Use Git Flow

- Applications with multiple versions in production simultaneously
- Mobile apps with App Store review cycles
- Enterprise software with scheduled quarterly releases
- Open source libraries with multiple maintained versions (v1.x, v2.x)

### When NOT to Use Git Flow

- Web applications deployed continuously
- Small teams
- Projects that release multiple times per day
- SaaS where there is only one production version

Git Flow has largely been replaced by Feature Branch Workflow + trunk-based approaches for web development.

---

## 4. Comparing Workflows

||Feature Branch|Trunk-Based|Git Flow|
|---|---|---|---|
|Branch lifespan|Days|Hours (or none)|Weeks|
|Release cadence|Continuous|Continuous|Scheduled|
|Merge conflicts|Low|Very low|High|
|Complexity|Low|Low|High|
|Good for|Most web teams|High-output teams|Versioned software|
|Requires|Code review culture|Feature flags + CI|Discipline|

---

## 5. Branch Naming Conventions

Consistent naming makes branches scannable and enables automation:

```bash
# Type prefix — mirrors conventional commits
feat/user-search
feat/refresh-token-rotation
fix/null-check-on-login
fix/cors-header-missing
docs/update-api-reference
chore/update-dependencies
test/add-auth-edge-cases
refactor/extract-query-builder

# With issue number
feat/142-user-search
fix/89-token-expiry
```

### Naming Rules

```bash
# Allowed
feat/user-search
feat/user_search         # underscores ok
fix/PROJ-142-login-error # ticket numbers ok

# Avoid
feat/UserSearch          # no capitals
feat/user search         # no spaces — spaces break command line
fix/f                    # not descriptive
wip                      # not descriptive
```

---

## 6. Release Strategies

### Tagging a Release

```bash
# Annotated tag — has its own object, message, and tagger
git tag -a v1.2.0 -m "Release v1.2.0 — adds user search and refresh tokens"

# Lightweight tag — just a pointer, no message
git tag v1.2.0

# Tag a specific past commit
git tag -a v1.1.3 a1b2c3d -m "Retroactive patch tag"

# Push tags to remote
git push origin v1.2.0
git push --tags              # push all tags

# List tags
git tag
git tag -l "v1.*"           # filter by pattern

# Show tag details
git show v1.2.0

# Delete a tag
git tag -d v1.2.0
git push origin --delete v1.2.0
```

### GitHub Releases

A GitHub Release wraps a tag with:

- Release notes (auto-generated from commit messages or manual)
- Binary attachments (compiled builds, APKs, etc.)
- Pre-release flag for beta versions

```bash
# Create via GitHub CLI
gh release create v1.2.0 \
  --title "v1.2.0 — User Search" \
  --notes "Adds user search (#142) and refresh token rotation (#89)" \
  --target main
```

### Changelog

Maintain a `CHANGELOG.md` at the root of the repo:

```markdown
# Changelog

## [1.2.0] - 2025-11-15

### Added
- User search with debounced input (#142)
- JWT refresh token rotation (#89)

### Fixed
- Null check on login response body (#156)

## [1.1.3] - 2025-10-28

### Fixed
- XSS in username display (#148)
- Incorrect redirect after password reset (#150)
```

---

## 7. Protecting main

Configure branch protection on GitHub to enforce the workflow:

**GitHub → Repo → Settings → Branches → Add branch protection rule**

Recommended settings for `main`:

```
✅ Require a pull request before merging
   ✅ Require approvals: 1
   ✅ Dismiss stale pull request approvals when new commits are pushed
   ✅ Require review from code owners

✅ Require status checks to pass before merging
   ✅ Require branches to be up to date
   → Add your CI checks: "test", "build", "lint"

✅ Require conversation resolution before merging

✅ Do not allow bypassing the above settings
```

### CODEOWNERS

Automatically assign reviewers based on which files were changed:

```
# .github/CODEOWNERS
# Format: path  @owner(s)

*                      @team/core           # default — everything
src/auth/              @team/security       # security team reviews auth
src/payments/          @team/payments       # payments team reviews payments
*.md                   @team/docs           # docs team reviews documentation
.github/               @team/devops         # devops reviews CI/CD changes
```

When a PR touches files matching a pattern, the listed owners are automatically added as required reviewers.

---
