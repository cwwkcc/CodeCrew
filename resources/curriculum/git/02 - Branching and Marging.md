A branch in Git is nothing more than a file containing a 40-character SHA-1 hash — a pointer to a commit. Creating a branch costs almost nothing. This makes branching the primary tool for isolating work, and merging the tool for bringing it back together.

---

## Table of Contents

1. [What a Branch Actually Is]
2. [Branch Commands]
3. [Merging]
4. [Merge Strategies]
5. [Resolving Merge Conflicts]
6. [Remote Branches]
7. [Tracking Branches]

---

## 1. What a Branch Actually Is

```bash
cat .git/refs/heads/main
# a1b2c3d4e5f6789012345678901234567890abcd
```

That is it. A branch is a 41-byte file (40 hex chars + newline). When you commit, Git writes the new commit's hash into this file. The branch "moves forward" because the pointer is updated.

```
Before commit:    main → C
Make a commit:    main → D   (D's parent is C)

Before new branch:   main → C
git checkout -b feature:
  feature → C   (same hash as main)
  HEAD    → refs/heads/feature

Make a commit on feature:
  feature → D   (D's parent is C)
  main    → C   (unchanged)
```

Two branches diverging is just two pointer files pointing at different nodes in the DAG.

---

## 2. Branch Commands

```bash
# List local branches (* = current branch)
git branch
# * main
#   feature/auth
#   fix/login-error

# List all branches including remote-tracking
git branch -a

# List with last commit info
git branch -v

# List with upstream tracking info
git branch -vv

# Create a branch (does NOT switch to it)
git branch feature/auth

# Create and switch
git checkout -b feature/auth
git switch -c feature/auth     # modern syntax

# Switch to an existing branch
git checkout main
git switch main                # modern syntax

# Rename a branch
git branch -m old-name new-name
git branch -m new-name         # rename current branch

# Delete a branch (safe — refuses if unmerged)
git branch -d feature/auth

# Force delete (even if unmerged — you will lose those commits)
git branch -D feature/auth

# Delete a remote branch
git push origin --delete feature/auth

# Create a branch from a specific commit or tag
git checkout -b hotfix/v2 v2.0.0
git checkout -b experiment a1b2c3d
```

---

## 3. Merging

Merging integrates one branch into another. You always merge **into** the current branch:

```bash
# Switch to the branch you want to merge INTO
git checkout main

# Merge feature branch into main
git merge feature/auth
```

### Fast-Forward Merge

If the target branch has not diverged since the feature branched off, Git simply moves the pointer forward. No merge commit is created:

```
Before:
  main    → C
  feature → E  (C ← D ← E)

After: git checkout main && git merge feature
  main    → E   (pointer just moved forward)
  feature → E
```

```bash
git merge feature/auth
# Updating a1b2c3d..f4e5d6e
# Fast-forward
#  src/auth.js | 42 +++++++++++++
```

### Three-Way Merge

When both branches have diverged, Git finds the common ancestor and creates a new merge commit with two parents:

```
Before:
  main    → C ← D
  feature → C ← E ← F

Merge commit M has parents D and F:
  main → M
         ├── parent 1: D
         └── parent 2: F
```

```bash
git merge feature/auth
# Merge made by the 'ort' strategy.
#  src/auth.js | 42 +++++++++++++
```

### `--no-ff` — Force a Merge Commit

Even when a fast-forward is possible, create a merge commit anyway. Preserves a visible record that a feature branch existed:

```bash
git merge --no-ff feature/auth -m "feat: merge auth feature"
```

### `--squash` — Collapse All Feature Commits

Combine all commits from the feature branch into one staged changeset, then commit manually. Produces a clean linear history:

```bash
git merge --squash feature/auth
git commit -m "feat: add complete auth system"
# All feature commits are squashed into one
# The feature branch is NOT merged — no merge commit
```

---

## 4. Merge Strategies

```bash
# Default — Git chooses (usually 'ort' in modern Git)
git merge feature

# Recursive / ort (default for two-branch merges)
git merge -s ort feature

# Octopus — merge more than two branches at once
git merge branch-a branch-b branch-c

# Ours — keep current branch content, discard theirs entirely
git merge -s ours obsolete-branch

# Subtree — merge another repo as a subdirectory
git merge -s subtree other-project
```

### Strategy Options (`-X`)

```bash
# When conflict: prefer our version
git merge -X ours feature

# When conflict: prefer their version
git merge -X theirs feature

# Ignore whitespace differences
git merge -Xignore-all-space feature
```

---

## 5. Resolving Merge Conflicts

A conflict happens when both branches changed the same part of the same file. Git cannot decide which version to keep — you must resolve it manually.

### What a Conflict Looks Like

```javascript
// auth.js after a conflict
function validateToken(token) {
<<<<<<< HEAD
  if (!token || token.length < 32) {
    throw new Error('Token too short');
  }
=======
  if (!token) {
    throw new Error('Token required');
  }
  if (token.length < 64) {
    throw new Error('Token must be 64 characters');
  }
>>>>>>> feature/auth
  return jwt.verify(token, process.env.JWT_SECRET);
}
```

```
<<<<<<< HEAD          ← your current branch's version
  ...content...
=======               ← divider
  ...content...
>>>>>>> feature/auth  ← incoming branch's version
```

### Resolution Process

```bash
# Step 1: See all conflicted files
git status
# both modified:   src/auth.js

# Step 2: Open each file and resolve
# Edit the file — remove conflict markers, keep what is correct

# Step 3: Stage the resolved file
git add src/auth.js

# Step 4: Complete the merge
git commit
# (Git prepopulates the commit message with merge info)

# Abort the merge entirely if you want to start over
git merge --abort
```

### Using a Merge Tool

```bash
# Open the configured merge tool for all conflicted files
git mergetool

# Configure VS Code as your merge tool
git config --global merge.tool vscode
git config --global mergetool.vscode.cmd 'code --wait $MERGED'
```

### Conflict Markers Explained

```
<<<<<<< HEAD                    ← start of current branch content
  current branch version
||||||| parent (with --diff3)   ← original content before both branches changed it
  common ancestor version
=======                         ← divider
  incoming branch version
>>>>>>> feature/auth            ← end of incoming branch content
```

Enable the three-way diff3 style to see the original:

```bash
git config --global merge.conflictstyle diff3
```

---

## 6. Remote Branches

A remote-tracking branch is a local read-only copy of a branch on the remote. It is updated when you fetch:

```
refs/remotes/origin/main   ← your local snapshot of origin's main
refs/remotes/origin/feature/auth
```

```bash
# Download all remote changes (does NOT merge — just updates remote-tracking branches)
git fetch origin

# Fetch all remotes
git fetch --all

# Fetch and prune remote-tracking branches that no longer exist on the remote
git fetch --prune

# See remote-tracking branches
git branch -r
# origin/main
# origin/feature/auth

# See what is different between your main and origin/main
git log main..origin/main           # commits on origin not on local main
git log origin/main..main           # commits on local main not on origin

# Pull = fetch + merge
git pull origin main

# Pull with rebase instead of merge (cleaner history)
git pull --rebase origin main

# Push local branch to remote
git push origin feature/auth

# Push and set upstream tracking in one step
git push -u origin feature/auth
# After this: git push / git pull work without specifying remote and branch
```

---

## 7. Tracking Branches

A tracking branch is a local branch that has an upstream relationship with a remote branch:

```bash
# Set upstream when pushing for the first time
git push -u origin feature/auth

# Set upstream on an existing branch
git branch --set-upstream-to=origin/main main

# Check tracking relationships
git branch -vv
# * main           a1b2c3d [origin/main] feat: add dashboard
#   feature/auth   f4e5d6e [origin/feature/auth: ahead 2] feat: auth WIP

# "ahead 2" — 2 local commits not yet pushed
# "behind 3" — 3 remote commits not yet pulled
# "ahead 1, behind 2" — diverged from remote
```

Once upstream is set, these short forms work:

```bash
git push          # pushes current branch to its upstream
git pull          # pulls from upstream and merges/rebases
git fetch         # updates all tracking branches
```

---

## Common Branching Patterns

```bash
# Feature branch workflow
git checkout main
git pull
git checkout -b feature/user-search
# ... make commits ...
git push -u origin feature/user-search
# ... open PR on GitHub ...
git checkout main
git pull
git branch -d feature/user-search

# Hotfix workflow
git checkout main
git pull
git checkout -b hotfix/xss-in-username
# ... fix the bug ...
git commit -m "fix: sanitize username in profile render"
git checkout main
git merge --no-ff hotfix/xss-in-username
git tag -a v1.2.1 -m "patch: xss fix"
git push && git push --tags
git branch -d hotfix/xss-in-username
```

---