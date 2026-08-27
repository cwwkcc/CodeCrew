The commands you use every single day. Each one maps directly to the internals covered in Module 00 — once you know what blobs, trees, commits, and refs are, these commands make complete sense.

---

## Table of Contents

1. [Setup and Configuration]
2. [Starting a Repository]
3. [Staging and Committing]
4. [Inspecting State]
5. [Viewing History]
6. [Comparing Changes]
7. [Stashing]
8. [Quick Reference]

---

## 1. Setup and Configuration

Git reads config from three levels, each overriding the previous:

```bash
# System — /etc/gitconfig (all users on this machine)
# Global — ~/.gitconfig   (your user account)
# Local  — .git/config    (this repository only)
```

```bash
# Identity — required before your first commit
git config --global user.name  "Ashan Kavinda"
git config --global user.email "ashan@paideon.lk"

# Default editor for commit messages
git config --global core.editor "code --wait"    # VS Code
git config --global core.editor "nvim"           # Neovim

# Default branch name for new repos
git config --global init.defaultBranch main

# Better diff output
git config --global core.pager "less -FX"

# Auto-correct typos in commands (after 1 second delay)
git config --global help.autocorrect 10

# View all config
git config --list --global

# View a single value
git config user.email
```

### `.gitconfig` directly

```ini
[user]
    name  = Ashan Kavinda
    email = ashan@paideon.lk

[core]
    editor     = code --wait
    autocrlf   = input        # convert CRLF → LF on commit (use 'true' on Windows)
    whitespace = trailing-space,space-before-tab

[init]
    defaultBranch = main

[pull]
    rebase = true             # pull --rebase by default

[push]
    autoSetupRemote = true    # auto-set upstream on first push

[alias]
    st   = status
    co   = checkout
    br   = branch
    lg   = log --oneline --graph --all --decorate
    undo = reset HEAD~1 --mixed
```

---

## 2. Starting a Repository

```bash
# New repo in current directory
git init

# New repo in a new directory
git init my-project

# Clone an existing remote repo
git clone https://github.com/user/repo.git

# Clone into a specific directory name
git clone https://github.com/user/repo.git my-folder

# Clone only the latest commit — much faster for large repos
git clone --depth 1 https://github.com/user/repo.git

# Clone a specific branch
git clone --branch develop https://github.com/user/repo.git
```

### `.gitignore`

Tell Git which files to never track. Committed at the root of the repo:

```gitignore
# Dependencies
node_modules/
.pnp
.pnp.js

# Build output
dist/
build/
out/
.next/

# Environment secrets — NEVER commit these
.env
.env.local
.env.*.local
.env.production

# OS files
.DS_Store
Thumbs.db

# IDE
.vscode/
.idea/
*.swp
*.swo

# Logs
*.log
npm-debug.log*

# Keys and certificates
*.pem
*.key
*.cert
```

Patterns:

```
file.txt         exact filename
*.log            any .log file
logs/            directory and all contents
!important.log   negate — do not ignore this despite *.log rule
**/temp          temp directory at any depth
doc/**/*.pdf     all PDFs inside doc/ at any depth
```

```bash
# Check why a file is ignored
git check-ignore -v filename.txt

# Show all ignored files
git status --ignored

# Untrack a file that is already committed (after adding to .gitignore)
git rm --cached filename.txt
```

---

## 3. Staging and Committing

### Staging

```bash
# Stage a single file
git add auth.js

# Stage multiple files
git add auth.js dashboard.js

# Stage all changes in current directory and below
git add .

# Stage all changes everywhere (including deletions)
git add -A

# Stage only modified and deleted files (not untracked)
git add -u

# Stage specific hunks interactively
git add -p auth.js
# y = stage hunk
# n = skip
# s = split hunk smaller
# e = manually edit the hunk
# q = quit

# Unstage a file (keep changes in working tree)
git restore --staged auth.js
# older syntax: git reset HEAD auth.js
```

### Committing

```bash
# Commit with a message
git commit -m "feat: add login form validation"

# Stage all tracked files and commit in one step
git commit -am "fix: correct token expiry check"
# (does NOT stage new untracked files — only modifications to tracked files)

# Open editor for a detailed commit message
git commit

# Amend the last commit — change message or add staged changes
git commit --amend -m "feat: add login form validation with error messages"
# Creates a new commit object — changes the hash
# Never amend commits that have been pushed to a shared remote

# Commit with a specific date
git commit --date="2025-11-01T10:30:00" -m "feat: backdate this commit"

# Empty commit — useful for triggering CI pipelines
git commit --allow-empty -m "chore: trigger deployment"
```

### Writing Good Commit Messages

```
<type>(<scope>): <short summary>
                                  ← blank line
<optional body — explain WHY, not what>
                                  ← blank line
<optional footer — BREAKING CHANGE, closes #123>
```

```
feat(auth): add JWT refresh token rotation

Previous implementation used a single long-lived token.
This replaces it with short-lived access tokens (15m) and
rotating refresh tokens (7d) to reduce blast radius on leak.

BREAKING CHANGE: clients must handle 401 and request a new token
Closes #142
```

Rules:

- Summary line ≤ 72 characters
- Use imperative mood: "add feature" not "added feature"
- Body explains **why**, not what (the diff shows what)

---

## 4. Inspecting State

```bash
# Full status — what is staged, what is not, what is untracked
git status

# Short status
git status -s
# Output:
# M  auth.js       ← staged modification
#  M index.js      ← unstaged modification
# ?? newfile.js    ← untracked
# A  styles.css    ← staged new file
# D  old.js        ← staged deletion

# Show current branch and remote tracking info
git branch -vv

# Show where HEAD is pointing
cat .git/HEAD

# Show the hash of a ref
git rev-parse HEAD
git rev-parse main
git rev-parse HEAD~3   # 3 commits back
```

### Relative Ref Syntax

```bash
HEAD~1   # one commit before HEAD (parent)
HEAD~3   # three commits before HEAD
HEAD^    # first parent of HEAD (same as HEAD~1)
HEAD^2   # second parent of HEAD (merge commits only)
main~5   # five commits before main branch tip
v1.0.0~1 # one commit before the v1.0.0 tag
```

---

## 5. Viewing History

```bash
# Default log
git log

# Compact — one line per commit
git log --oneline

# With branch graph
git log --oneline --graph --all --decorate

# Limit to last N commits
git log -5

# Show commits by a specific author
git log --author="Ashan"

# Show commits in a date range
git log --after="2025-01-01" --before="2025-06-01"

# Show commits that changed a specific file
git log -- src/auth.js

# Show commits whose message matches a pattern
git log --grep="fix:"

# Show diff for each commit (full patch)
git log -p

# Show stats (lines added/removed per file)
git log --stat

# Show a specific commit
git show a1b2c3d

# Show only the files changed in a commit
git show --name-only a1b2c3d
git show --name-status a1b2c3d
```

### Searching Content in History

```bash
# Find all commits that added or removed a specific string
git log -S "getUserById"

# Find commits where a string appears in the diff
git log -G "password.*hash"

# Search across all branches
git log --all -S "getUserById"
```

---

## 6. Comparing Changes

```bash
# Working tree vs index (unstaged changes)
git diff

# Index vs HEAD (staged changes — what will be in the next commit)
git diff --staged
git diff --cached   # same thing

# Working tree vs HEAD (all changes since last commit)
git diff HEAD

# Two specific commits
git diff a1b2c3d f4e5d6e

# Two branches
git diff main feature/auth

# A specific file only
git diff HEAD -- src/auth.js

# Show only file names, not the full diff
git diff --name-only HEAD
git diff --name-status HEAD

# Word-level diff (useful for prose/docs)
git diff --word-diff
```

---

## 7. Stashing

Stash saves your working tree and index changes to a stack so you can switch context without committing:

```bash
# Save current changes to stash
git stash

# Save with a description
git stash push -m "wip: half-finished auth refactor"

# Include untracked files
git stash push -u

# Include untracked AND ignored files
git stash push -a

# List all stashes
git stash list
# stash@{0}: On main: wip: half-finished auth refactor
# stash@{1}: WIP on feature: 9a8b7c quick fix attempt

# Apply the most recent stash (keeps it in the list)
git stash apply

# Apply and remove from list
git stash pop

# Apply a specific stash
git stash apply stash@{2}

# Show what is in a stash
git stash show -p stash@{0}

# Create a branch from a stash
git stash branch new-feature stash@{0}

# Delete a specific stash
git stash drop stash@{1}

# Delete all stashes
git stash clear
```

### When to Use Stash

```bash
# Scenario: you are mid-feature and need to urgently fix a bug

# 1. Stash your in-progress work
git stash push -m "wip: user profile editor"

# 2. Switch to main, create a hotfix branch
git checkout main
git checkout -b hotfix/login-error

# 3. Fix the bug, commit, push
git commit -m "fix: null check on login response"

# 4. Return to your feature
git checkout feature/profile-editor
git stash pop
```

---

## 8. Quick Reference

```bash
# ── SETUP ──────────────────────────────────────────────────────
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
git init
git clone <url>

# ── STAGE & COMMIT ─────────────────────────────────────────────
git add <file>             # stage file
git add .                  # stage all
git add -p                 # stage hunks interactively
git restore --staged <file># unstage
git commit -m "message"    # commit staged
git commit -am "message"   # stage tracked + commit
git commit --amend         # fix last commit

# ── STATUS & HISTORY ───────────────────────────────────────────
git status                 # current state
git log --oneline --graph  # visual history
git show <hash>            # show a commit
git diff                   # unstaged changes
git diff --staged          # staged changes

# ── STASH ──────────────────────────────────────────────────────
git stash                  # save changes
git stash pop              # restore last stash
git stash list             # all stashes
```

---

_Next: [Branching and Merging]
