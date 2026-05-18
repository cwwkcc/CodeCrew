GitHub is a hosting platform for Git repositories. It adds collaboration features — pull requests, code review, issues, Actions — on top of standard Git. The underlying data model is identical: everything on GitHub is still just blobs, trees, commits, and refs.

---

## Table of Contents

1. [Remotes]
2. [SSH vs HTTPS Authentication]
3. [Forks]
4. [Pull Requests]
5. [Code Review]
6. [GitHub Actions — CI/CD Basics]
7. [Issues and Project Management]
8. [GitHub CLI]

---

## 1. Remotes

A remote is a named URL pointing to another copy of the repository — usually on GitHub:

```bash
# View remotes
git remote -v
# origin  git@github.com:user/repo.git (fetch)
# origin  git@github.com:user/repo.git (push)

# Add a remote
git remote add origin git@github.com:user/repo.git

# Change remote URL (e.g. HTTPS → SSH)
git remote set-url origin git@github.com:user/repo.git

# Remove a remote
git remote remove origin

# Rename a remote
git remote rename origin upstream

# Fetch all branches from remote
git fetch origin

# Push a branch
git push origin feature/auth

# Push and set upstream tracking
git push -u origin feature/auth

# Delete a remote branch
git push origin --delete feature/auth

# Push all local tags
git push --tags
```

### Multiple Remotes

Common when contributing to open source — `origin` is your fork, `upstream` is the original:

```bash
git remote add upstream git@github.com:original-owner/repo.git

# Keep your fork updated with upstream
git fetch upstream
git checkout main
git rebase upstream/main
git push origin main
```

---

## 2. SSH vs HTTPS Authentication

### SSH (recommended)

```bash
# Generate an SSH key pair
ssh-keygen -t ed25519 -C "ashan@paideon.lk"
# Private key: ~/.ssh/id_ed25519     ← never share this
# Public key:  ~/.ssh/id_ed25519.pub ← add to GitHub

# Copy public key
cat ~/.ssh/id_ed25519.pub

# Add to GitHub:
# Profile → Settings → SSH and GPG keys → New SSH key → paste

# Test the connection
ssh -T git@github.com
# Hi username! You've successfully authenticated.
```

SSH remote URL format:

```
git@github.com:username/repo.git
```

### HTTPS

Uses a Personal Access Token (PAT) instead of a password:

```bash
# GitHub → Settings → Developer settings → Personal access tokens → Fine-grained token
# Permissions needed: Contents (read/write), Metadata (read)

# Store the token so you do not have to type it every time
git config --global credential.helper store     # stores in plaintext
git config --global credential.helper osxkeychain  # macOS keychain (safer)
```

HTTPS remote URL format:

```
https://github.com/username/repo.git
```

SSH is recommended — tokens expire, SSH keys do not, and SSH is faster.

---

## 3. Forks

A fork is a server-side copy of someone else's repository under your GitHub account. Used for contributing to projects you do not have write access to.

```
Original repo:   github.com/facebook/react
Your fork:       github.com/yourusername/react    ← independent copy
Your local:      ~/code/react                     ← cloned from your fork
```

### Fork Workflow

```bash
# 1. Fork on GitHub (click the Fork button)

# 2. Clone YOUR fork (not the original)
git clone git@github.com:yourusername/react.git
cd react

# 3. Add the original as upstream
git remote add upstream git@github.com:facebook/react.git

# 4. Verify remotes
git remote -v
# origin    git@github.com:yourusername/react.git
# upstream  git@github.com:facebook/react.git

# 5. Create a feature branch
git checkout -b fix/typo-in-docs

# 6. Make changes and commit
git commit -m "docs: fix typo in getting started guide"

# 7. Push to YOUR fork
git push -u origin fix/typo-in-docs

# 8. Open a Pull Request on GitHub from your fork's branch to the original's main

# 9. Keep your fork up to date with the original
git fetch upstream
git checkout main
git rebase upstream/main
git push origin main
```

---

## 4. Pull Requests

A Pull Request (PR) is a GitHub feature for requesting that your branch be merged into another branch. It is the central collaboration mechanism.

### Opening a PR

After pushing a branch:

1. Go to the repo on GitHub
2. GitHub shows a yellow banner: "Compare & pull request" — click it
3. Or: Pull requests tab → New pull request
4. Select base branch (e.g. `main`) and compare branch (your feature)
5. Write a title and description
6. Assign reviewers, labels, and a milestone if applicable
7. Submit

### PR Description Template

A good PR description:

```markdown
## What

Short explanation of what this PR does.

## Why

The problem this solves or the feature this adds.

## How

Technical summary of the approach taken.

## Testing

How to verify this works. Checklist of what was tested.

## Screenshots (if UI changes)

Before / After screenshots.

## Related Issues

Closes #142
```

### PR Merge Options

GitHub offers three merge strategies:

|Option|Result|
|---|---|
|**Create a merge commit**|Three-way merge — preserves branch history, creates merge commit|
|**Squash and merge**|All commits squashed into one — clean linear history|
|**Rebase and merge**|Commits replayed on top of base — linear, preserves individual commits|

Squash and merge is the most popular for feature branches — you get a single clean commit per feature in main's history.

### Draft PRs

Open as a draft to signal "work in progress — not ready for review":

- GitHub → New pull request → "Create draft pull request"
- Useful to get early feedback or trigger CI before the work is complete
- Convert to ready: "Ready for review" button in the PR

---

## 5. Code Review

### As a Reviewer

On the PR's "Files changed" tab:

- Click the `+` next to a line to add a comment
- "Start a review" to batch comments before submitting
- Submit as:
    - **Comment** — general feedback, no approval/rejection
    - **Approve** — looks good to merge
    - **Request changes** — must be addressed before merging

Good review comments:

```
# Instead of:
"This is wrong."

# Write:
"This function will throw if `user` is null. Can we add a null check here?
Something like: if (!user) return null;"
```

Use suggestions for small fixes — the author can apply them with one click:

````
```suggestion
const token = user?.token ?? generateToken();
```
````

### As an Author

```bash
# Fetch review changes suggested by a reviewer
git fetch origin
git checkout feature/auth
git rebase origin/main    # stay up to date

# After addressing review comments, push
git push
# PR automatically updates — no need to open a new one
```

---

## 6. GitHub Actions — CI/CD Basics

GitHub Actions runs automated workflows triggered by Git events. Workflows live in `.github/workflows/*.yml`:

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: '20'
          cache: 'npm'

      - name: Install dependencies
        run: npm ci

      - name: Run tests
        run: npm test

      - name: Build
        run: npm run build
```

### Common Triggers

```yaml
on:
  push:                        # on every push
  pull_request:                # on PR open/update
  workflow_dispatch:           # manual trigger from GitHub UI
  schedule:
    - cron: '0 0 * * *'       # daily at midnight UTC
  release:
    types: [published]         # when a release is published
```

### Status Checks

Require CI to pass before a PR can be merged:

- Repo settings → Branches → Add branch protection rule
- Enable "Require status checks to pass before merging"
- Select the check names from your workflow jobs

---

## 7. Issues and Project Management

### Issues

Issues track bugs, feature requests, and tasks:

- **Labels**: categorise (bug, enhancement, documentation, good first issue)
- **Milestones**: group issues into releases or sprints
- **Assignees**: who is responsible
- **Close via commit**: `git commit -m "fix: null check — closes #42"`

Closing keywords in commit messages or PR descriptions:

```
closes #42
fixes #42
resolves #42
```

### Linking PRs and Issues

Reference issues in PR descriptions:

```markdown
## Related
Closes #42
Fixes #38, #39
Related to #50
```

When the PR is merged, the referenced issues close automatically.

---

## 8. GitHub CLI

The `gh` CLI lets you do GitHub operations from the terminal without opening a browser:

```bash
# Install
brew install gh          # macOS
winget install gh        # Windows

# Authenticate
gh auth login

# Create a PR
gh pr create --title "feat: add auth" --body "Adds JWT authentication"

# Create a draft PR
gh pr create --draft

# View open PRs
gh pr list

# Check out a PR locally
gh pr checkout 42

# Review a PR
gh pr review 42 --approve
gh pr review 42 --request-changes --body "Please add tests"

# Merge a PR
gh pr merge 42 --squash

# Create a repo
gh repo create my-project --public

# Clone a repo
gh repo clone username/repo

# View issues
gh issue list
gh issue create --title "Bug: login fails on Safari"

# View CI status
gh run list
gh run view 12345678
```

---