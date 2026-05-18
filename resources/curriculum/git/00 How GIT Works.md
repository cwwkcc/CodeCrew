Git is not a backup tool or a "track changes" system. It is a **content-addressable filesystem** with a version control interface built on top. Understanding what Git actually stores and how it thinks about history makes every command — branching, rebasing, reset, cherry-pick — obvious rather than mysterious.

---

## Table of Contents

1. [The Object Database]
2. [The Four Object Types]
3. [How a Commit Is Built]
4. [The DAG — Directed Acyclic Graph]
5. [References — How Git Names Commits]
6. [HEAD — Where You Are]
7. [The Three Areas]
8. [The Index in Detail]
9. [What `.git/` Contains]
10. [Content-Addressable Storage — Why It Matters]
---

## 1. The Object Database

Everything Git knows about your project lives in `.git/objects/`. Every file version, every directory snapshot, every commit — stored as an **object**.

An object is:

1. A header (`type size\0`) prepended to the content
2. The whole thing compressed with zlib
3. Stored at a path derived from its **SHA-1 hash**

```
SHA-1 hash of content:  a1b2c3d4e5f6...  (40 hex characters)

Stored at:  .git/objects/a1/b2c3d4e5f6...
             ─────────────  ────────────────
             first 2 chars  remaining 38 chars
             = directory    = filename
```

The hash is computed from the content itself. The same content always produces the same hash. Different content always produces a different hash. This is the core property everything else depends on.

---

## 2. The Four Object Types

### Blob — File Content

A blob stores the raw content of a single file. Nothing else — no filename, no permissions, just bytes:

```
blob 14\0Hello, World!\n
      ──  ───────────────
      size  content
```

```bash
# Create a blob manually
echo "Hello, World!" | git hash-object --stdin -w
# Output: 8ab686eafeb1f44702738c8b0f24f2567c36da6d

# Read it back
git cat-file -p 8ab686eafeb1f44702738c8b0f24f2567c36da6d
# Hello, World!
```

Because blobs are content-addressed, if two files have identical content they share **one blob**. Git never stores the same bytes twice.

### Tree — Directory Snapshot

A tree is a snapshot of a directory. It maps filenames and permissions to blobs (files) or other trees (subdirectories):

```
tree 98\0
100644 blob a1b2c3...  README.md
100644 blob f4e5d6...  package.json
040000 tree 9a8b7c...  src
```

```bash
git cat-file -p HEAD^{tree}
# 100644 blob a1b2c3d...   README.md
# 100644 blob f4e5d6e...   package.json
# 040000 tree 9a8b7c8...   src
```

File mode values:

|Mode|Meaning|
|---|---|
|`100644`|Regular file|
|`100755`|Executable file|
|`120000`|Symbolic link|
|`040000`|Directory (tree)|
|`160000`|Gitlink (submodule)|

### Commit — Snapshot + Metadata

A commit points to a tree (the full project snapshot) and contains metadata:

```
commit 250\0
tree   9a8b7c8d...        ← root tree of this snapshot
parent f1e2d3c4...        ← previous commit (none for the first commit)
author  Ashan <a@b.com> 1730000000 +0530
committer Ashan <a@b.com> 1730000000 +0530

Add login form validation
```

A commit with two parents is a merge commit. The first commit has no parent at all.

```bash
git cat-file -p HEAD
# tree   9a8b7c8d...
# parent f1e2d3c4...
# author  Ashan Kavinda <ashan@paideon.lk> 1730000000 +0530
# committer Ashan Kavinda <ashan@paideon.lk> 1730000000 +0530
#
# Add login form validation
```

### Tag — Named Commit

An annotated tag is an object that points to a commit and stores a name, message, and tagger:

```
tag 150\0
object f1e2d3c4...     ← the commit being tagged
type   commit
tag    v1.0.0
tagger Ashan <a@b.com> 1730000000 +0530

First stable release
```

Lightweight tags are just references (not objects). Annotated tags are full objects with their own hash.

---

## 3. How a Commit Is Built

When you run `git commit`, this is what actually happens:

```
Step 1: Every tracked file becomes a blob
──────────────────────────────────────────
src/index.js   → blob a1b2c3...
src/auth.js    → blob d4e5f6...
README.md      → blob 7g8h9i...

Step 2: Directories become trees
──────────────────────────────────────────
src/ directory → tree {
  100644 blob a1b2c3...  index.js
  100644 blob d4e5f6...  auth.js
}

Step 3: Root tree is built
──────────────────────────────────────────
root tree → tree {
  040000 tree 9j0k1l...  src
  100644 blob 7g8h9i...  README.md
}

Step 4: Commit object is created
──────────────────────────────────────────
commit → {
  tree:      root tree hash
  parent:    previous commit hash
  author:    ...
  committer: ...
  message:   ...
}
```

A commit is not a diff. It is a **complete snapshot** of every file at that moment. Git computes diffs on the fly when you ask for them by comparing two snapshots.

---

## 4. The DAG — Directed Acyclic Graph

Commits form a Directed Acyclic Graph. Each commit points **back** to its parent(s):

```
A ← B ← C ← D      (main)
            ↑
            └── E ← F   (feature)
```

- **Directed** — edges point in one direction (child → parent)
- **Acyclic** — no cycles possible (a commit cannot be its own ancestor)

This structure means:

- History is immutable — you cannot change a past commit without changing its hash and all descendant hashes
- Branching is free — a branch is just a pointer to a commit node
- Merging creates a commit with two parents

### Visualising the DAG

```bash
git log --oneline --graph --all
# * f1e2d3c (HEAD -> main) Add dashboard
# * a1b2c3d Add auth
# | * 9x8y7z (feature/login) Add login form
# |/
# * 7g6h5i  Initial commit
```

---

## 5. References — How Git Names Commits

Hashes are 40 characters long and unreadable. References (refs) are human-readable names that point to a commit hash:

```
.git/refs/
  heads/
    main          → a1b2c3d4e5f6...   (local branch)
    feature/auth  → 9x8y7z0a1b2c...   (local branch)
  remotes/
    origin/
      main        → f1e2d3c4d5e6...   (remote-tracking branch)
  tags/
    v1.0.0        → 7g8h9i0j1k2l...   (tag)
```

Each file contains exactly one thing: the SHA-1 hash of the commit it points to.

```bash
cat .git/refs/heads/main
# a1b2c3d4e5f6789012345678901234567890abcd

# Git resolves this automatically when you write branch names
git log main         # same as: git log a1b2c3d4...
git checkout main    # moves HEAD to the main ref
```

### Packed Refs

After enough refs accumulate, Git packs them into a single file:

```bash
cat .git/packed-refs
# # pack-refs with: peeled fully-peeled sorted
# a1b2c3d4... refs/heads/main
# 9x8y7z0a... refs/heads/feature/auth
# 7g8h9i0j... refs/tags/v1.0.0
# ^f1e2d3c4... (peeled tag — actual commit the tag points to)
```

---

## 6. HEAD — Where You Are

`HEAD` is a special reference that points to your current position in the repository. It lives at `.git/HEAD`.

### Attached HEAD (normal state)

HEAD points to a branch, which points to a commit:

```
HEAD → refs/heads/main → a1b2c3d4...
```

```bash
cat .git/HEAD
# ref: refs/heads/main
```

When you make a new commit, the branch pointer advances to the new commit. HEAD follows because it points to the branch:

```
Before commit:  HEAD → main → C
After commit:   HEAD → main → D    (D's parent is C)
```

### Detached HEAD

HEAD points directly to a commit hash instead of a branch:

```bash
git checkout a1b2c3d   # detached HEAD

cat .git/HEAD
# a1b2c3d4e5f6789012345678901234567890abcd
```

In this state, new commits are made but no branch advances to track them. Switch away without creating a branch and those commits become unreachable — eventually garbage-collected.

```bash
# Rescue a detached HEAD by creating a branch
git checkout -b rescue-branch
```

---

## 7. The Three Areas

Every file in a Git repository can exist in three places simultaneously. This is the mental model that explains the entire `add` / `commit` workflow:

```
┌─────────────────┐   git add    ┌─────────────────┐  git commit  ┌─────────────────┐
│  Working Tree   │ ───────────► │     Index        │ ───────────► │   Repository    │
│                 │              │  (Staging Area)  │              │  (.git/objects) │
│  Files on disk  │ ◄─────────── │                  │ ◄─────────── │                 │
│  that you edit  │ git checkout │                  │  git reset   │  Commit history │
└─────────────────┘              └─────────────────┘              └─────────────────┘
```

### Working Tree

The actual files you see and edit in your project folder. Git calls this the working tree or working directory.

Changes here are invisible to Git until you stage them with `git add`.

### Index (Staging Area)

A binary file at `.git/index` that records what the **next commit will look like**. It is a complete snapshot — not a list of changes, but a full picture of every tracked file.

When you `git add` a file:

- The content is hashed into a blob object in `.git/objects/`
- The index entry for that file path is updated to point to the new blob

When you `git commit`:

- Git reads the current index
- Builds tree objects from it
- Creates a commit pointing to the root tree

### Repository

The `.git/objects/` database — all blobs, trees, commits, and tags that have ever been committed.

### Seeing All Three Areas at Once

```bash
# Working tree vs index vs HEAD
git status

# Working tree vs index (what is not yet staged)
git diff

# Index vs HEAD (what is staged and ready to commit)
git diff --staged

# Working tree vs HEAD (everything changed since last commit)
git diff HEAD
```

---

## 8. The Index in Detail

The index is what makes partial staging possible. You can:

- Stage one file while leaving another unstaged
- Stage only some of the changes within a single file
- Have a file modified in both the index and working tree at the same time

```bash
# Stage specific changes within a file interactively
git add -p auth.js
# Git shows each changed hunk and asks what to do:
# y = stage this hunk
# n = skip this hunk
# s = split into smaller hunks
# q = quit

# Practical example:
git add auth.js          # version A goes into the index
# (edit auth.js again)   # working tree now has version B

git diff            # A vs B — unstaged changes
git diff --staged   # HEAD vs A — what will be committed

git commit -m "fix: validate token expiry"
# commits version A — the staged version
# version B is still in your working tree, unstaged
```

---

## 9. What `.git/` Contains

```
.git/
├── HEAD                  ← current branch ref or detached commit hash
├── config                ← repo-level git configuration
├── COMMIT_EDITMSG        ← the last commit message (used by hooks)
├── index                 ← the staging area — binary file
│
├── objects/              ← the entire object database
│   ├── pack/
│   │   ├── *.pack        ← packed objects (Git compresses old loose objects)
│   │   └── *.idx         ← index for fast lookup inside pack
│   ├── ab/               ← loose objects — directory named by first 2 hash chars
│   │   └── cdef12...     ← object file — remaining 38 chars of hash
│
├── refs/
│   ├── heads/            ← local branches
│   │   ├── main
│   │   └── feature/auth
│   ├── remotes/          ← remote-tracking branches
│   │   └── origin/
│   │       └── main
│   └── tags/
│       └── v1.0.0
│
├── logs/                 ← reflog — history of every ref movement
│   ├── HEAD
│   └── refs/heads/main
│
└── hooks/                ← scripts triggered by git events
    ├── pre-commit.sample
    ├── commit-msg.sample
    └── pre-push.sample
```

---

## 10. Content-Addressable Storage — Why It Matters

### Deduplication

Identical content is stored once. 1,000 commits all containing the same `package-lock.json` = one blob. Git is extremely space-efficient as a result.

### Integrity

You cannot modify a committed file, tree, or commit without changing its hash — and therefore every commit that descends from it. Corruption is automatically detectable.

```bash
git fsck          # verify the entire object database
# Dangling objects = unreferenced (e.g. from a deleted branch)
# Missing objects  = corruption
```

### Immutability of History

When you amend a commit or rebase a branch, Git does not modify the existing objects. It creates **new** objects with new hashes and moves the branch pointer to the new chain. The old commits remain in `.git/objects/` until garbage collection removes them:

```
Before amend:
  A ← B ← C         (HEAD → main)

After git commit --amend:
  A ← B ← C         (C still exists in objects, now unreferenced)
       ↑
       └── C'        (HEAD → main — new commit, new hash)
```

### The Reflog — Your Safety Net

Every time HEAD or a branch pointer moves, Git writes it to the reflog. This means almost nothing is permanently lost within 90 days:

```bash
git reflog
# a1b2c3d HEAD@{0}: commit: Add dashboard
# f1e2d3c HEAD@{1}: rebase finished: returning to refs/heads/main
# 9x8y7z0 HEAD@{2}: rebase: Add auth
# 7g6h5i4 HEAD@{3}: checkout: moving from feature to main

# Recover commits from a branch you accidentally deleted
git checkout -b recovered-branch HEAD@{3}

# Or just check out a specific lost commit
git checkout 9x8y7z0
```

Reflog is local only — it does not push to GitHub. It is your personal undo history.

---
