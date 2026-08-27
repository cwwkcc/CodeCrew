Git's immutable object model means very little is truly lost. Almost every "undo" operation is really just moving a pointer or creating a new commit that reverses changes. The reflog is your safety net for anything that seems lost.

---

## Table of Contents

1. [Undoing in the Working Tree]
2. [Undoing in the Index]
3. [Undoing Commits with `reset`]
4. [`reset` Modes Explained]
5. [Undoing with `revert`]
6. [`reset` vs `revert`]
7. [Recovering Lost Work with Reflog]
8. [Other Undo Operations]

---

## 1. Undoing in the Working Tree

Discard changes to a file that have not been staged yet:

```bash
# Restore a file to the index version (discard unstaged changes)
git restore auth.js

# Restore to the HEAD version (discard all changes — staged or not)
git restore --source=HEAD auth.js

# Restore all files
git restore .

# Old syntax (still works)
git checkout -- auth.js
```

This is irreversible — Git has no record of your unstaged changes. Use with caution.

---

## 2. Undoing in the Index

Remove a file from the staging area (keep the changes in the working tree):

```bash
# Unstage a file — changes stay in working tree
git restore --staged auth.js

# Unstage everything
git restore --staged .

# Old syntax
git reset HEAD auth.js
```

---

## 3. Undoing Commits with `reset`

`git reset` moves the current branch pointer backward to a previous commit. What happens to the commits that were "undone" depends on the mode.

```bash
# Undo last commit — keep changes staged
git reset --soft HEAD~1

# Undo last commit — keep changes in working tree (unstaged)
git reset --mixed HEAD~1    # default — same as: git reset HEAD~1

# Undo last commit — discard changes entirely
git reset --hard HEAD~1

# Undo last 3 commits
git reset --hard HEAD~3

# Reset to a specific commit
git reset --hard a1b2c3d
```

---

## 4. `reset` Modes Explained

`reset` moves the branch pointer and then optionally updates the index and working tree:

```
           Branch    Index     Working Tree
──────────────────────────────────────────────
--soft     moved     unchanged  unchanged
--mixed    moved     reset      unchanged     ← default
--hard     moved     reset      reset
```

### `--soft`

```bash
git reset --soft HEAD~1
```

The branch pointer moves back. The index and working tree are untouched. The undone commit's changes are still staged — ready to be recommitted with a different message or combined with other changes.

Use case: fix a commit message, split a commit into smaller ones.

```bash
# Wrong commit message
git commit -m "fix stuff"
git reset --soft HEAD~1
git commit -m "fix: correct null check in token validation"
```

### `--mixed` (default)

```bash
git reset HEAD~1
```

The branch pointer moves back. The index is reset to match the target commit. Working tree is untouched. The changes from the undone commit appear as unstaged modifications.

Use case: undo a commit and re-stage only part of it.

```bash
git reset HEAD~1          # undo commit, changes become unstaged
git add -p auth.js        # stage only the relevant parts
git commit -m "fix: token validation"
```

### `--hard`

```bash
git reset --hard HEAD~1
```

The branch pointer, index, and working tree all reset to the target commit. The changes from the undone commit are **gone from all three areas**.

Use case: completely throw away commits and their changes.

```bash
# Undo last 2 commits — permanently discard those changes
git reset --hard HEAD~2

# Restore to a specific clean state
git reset --hard origin/main
```

`--hard` discards working tree changes. But the commits are not gone — they still exist in `.git/objects/`. You can recover them via the reflog for up to 90 days.

---

## 5. Undoing with `revert`

`revert` creates a **new commit** that undoes the effect of a previous commit. The original commit stays in history:

```bash
# Revert the most recent commit
git revert HEAD

# Revert a specific commit by hash
git revert a1b2c3d

# Revert a range of commits
git revert HEAD~3..HEAD

# Revert without auto-committing (stage only)
git revert -n a1b2c3d
git revert -n HEAD~3..HEAD
git commit -m "revert: undo auth changes"

# Revert a merge commit (must specify which parent to revert to)
git revert -m 1 M    # -m 1 = keep first parent (usually main), undo second (feature)
```

What the history looks like:

```
Before:
  A ← B ← C ← D

git revert C:

After:
  A ← B ← C ← D ← C'
                    ↑
                    C' undoes C's changes
                    C still exists
```

---

## 6. `reset` vs `revert`

|                          | `reset`                                      | `revert`                          |
| ------------------------ | -------------------------------------------- | --------------------------------- |
| How it works             | Moves branch pointer backward                | Creates a new undo commit         |
| History                  | Rewrites — old commits become unreferenced   | Preserved — old commits untouched |
| Safe for shared branches | **No** — never reset commits that are pushed | **Yes** — safe to push            |
| Use for                  | Local cleanup before pushing                 | Undoing pushed commits            |

```bash
# WRONG — never reset commits that exist on origin
git reset --hard HEAD~2
git push --force    # destroys shared history, breaks teammates

# RIGHT — revert is safe
git revert HEAD~2..HEAD
git push
```

If you have already pushed and must use reset (e.g. accidentally pushed secrets), use `--force-with-lease` not `--force`:

```bash
git push --force-with-lease
# Safer than --force: fails if the remote has commits you have not seen
# Prevents overwriting someone else's push
```

---

## 7. Recovering Lost Work with Reflog

The reflog records every position HEAD has been at. Even after a `--hard` reset or a deleted branch, commits remain in `.git/objects/` for 90 days and can be found via reflog:

```bash
# View the reflog
git reflog
# a1b2c3d HEAD@{0}: reset: moving to HEAD~2
# f4e5d6e HEAD@{1}: commit: feat: add dashboard
# 9x8y7z0 HEAD@{2}: commit: feat: add user search
# 7g6h5i4 HEAD@{3}: commit: Initial commit

# Recover commits lost to reset --hard
git checkout -b recovery-branch HEAD@{1}
# or
git reset --hard HEAD@{1}
```

### Recovering a Deleted Branch

```bash
# You deleted a branch without merging
git branch -D feature/dashboard

# Find the last commit on it
git reflog | grep feature/dashboard
# or just browse recent reflog entries
git reflog
# 9x8y7z0 HEAD@{3}: checkout: moving from feature/dashboard to main

# Recreate the branch at that commit
git checkout -b feature/dashboard 9x8y7z0
```

### Recovering a Dropped Stash

```bash
git stash drop stash@{0}   # accidentally dropped

# Stash entries also appear in reflog — find the blob hash
git fsck --unreachable | grep commit | cut -d ' ' -f3 | xargs git log --merges --no-walk

# Or look at all unreachable commits
git fsck --lost-found
# Unreachable commits are in .git/lost-found/
```

---

## 8. Other Undo Operations

### Amend the Last Commit

```bash
# Change the last commit message
git commit --amend -m "feat: correct message"

# Add a forgotten file to the last commit
git add forgotten.js
git commit --amend --no-edit    # keep the same message

# Both message and content
git add forgotten.js
git commit --amend -m "feat: add auth with session handler"
```

Amend rewrites the commit — creates a new commit object with a new hash. Never amend a commit that has been pushed to a shared remote.

### Clean Untracked Files

```bash
# Preview what would be deleted
git clean -n

# Delete untracked files
git clean -f

# Delete untracked files and directories
git clean -fd

# Delete untracked and ignored files (use carefully — removes node_modules, .env, etc.)
git clean -fdx
```

### Restore a Deleted File

```bash
# Find which commit deleted the file
git log --all --full-history -- path/to/deleted-file.js

# Restore it from that commit
git checkout <commit-before-deletion>^ -- path/to/deleted-file.js
```

### Undo a Merge

```bash
# If the merge has NOT been pushed — reset to before the merge
git reset --hard HEAD~1     # or HEAD~n depending on commits since merge

# If the merge HAS been pushed — revert the merge commit
git revert -m 1 <merge-commit-hash>
```

---

## Quick Reference

```bash
# Discard unstaged changes in a file
git restore <file>

# Unstage a file
git restore --staged <file>

# Undo last commit — keep changes staged
git reset --soft HEAD~1

# Undo last commit — keep changes unstaged
git reset HEAD~1

# Undo last commit — discard changes
git reset --hard HEAD~1

# Safely undo a pushed commit
git revert HEAD

# Find lost commits
git reflog

# Fix the last commit message
git commit --amend -m "correct message"
```

---
