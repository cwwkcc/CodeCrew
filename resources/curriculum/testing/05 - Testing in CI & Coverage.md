A test suite that only runs on your own machine, when you remember to run it, catches far fewer bugs than one that runs automatically on every push. This module covers wiring tests into CI, and how to think about coverage without chasing a meaningless number.

---

## Wiring Tests into GitHub Actions

This slots directly alongside the pnpm/CI workflows already running for Nexus:

```yaml
# .github/workflows/test.yml
name: Test
on: [push, pull_request]

jobs:
  unit-and-integration:
    runs-on: ubuntu-latest
    services:
      postgres:
        image: postgres:16
        env:
          POSTGRES_PASSWORD: test
          POSTGRES_DB: nexus_test
        ports: ["5432:5432"]
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5

    steps:
      - uses: actions/checkout@v4
      - uses: pnpm/action-setup@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 20
          cache: pnpm

      - run: pnpm install --frozen-lockfile
      - run: pnpm prisma migrate deploy
        env:
          DATABASE_URL: postgresql://postgres:test@localhost:5432/nexus_test
      - run: pnpm test:run
        env:
          DATABASE_URL: postgresql://postgres:test@localhost:5432/nexus_test

  e2e:
    runs-on: ubuntu-latest
    needs: unit-and-integration
    steps:
      - uses: actions/checkout@v4
      - uses: pnpm/action-setup@v4
      - uses: actions/setup-node@v4
        with: { node-version: 20, cache: pnpm }
      - run: pnpm install --frozen-lockfile
      - run: npx playwright install --with-deps
      - run: npx playwright test
```

The `services: postgres:` block spins up a real, throwaway PostgreSQL instance for the duration of the job — this is what the integration tests from Module 03 run against in CI, separate from any real database entirely. Splitting E2E into its own job (running only after unit/integration pass) means a slow E2E failure doesn't block fast feedback on the cheaper tests, and `needs:` prevents wasting CI minutes running expensive E2E tests when the fast tests already failed.

---

## Failing the Build vs. Warning

A CI test step should **fail the pipeline** on a failing test — not just print a warning that's easy to ignore. `pnpm test:run` (Vitest's non-watch mode) and `npx playwright test` both already exit with a non-zero status code on failure, which GitHub Actions treats as a failed step by default — no extra configuration needed. This is what actually blocks a broken pull request from being merged, the same way `html-validate` blocks a broken build in the HTML curriculum's CI example.

---

## Code Coverage — What It Measures and What It Doesn't

**Coverage** is the percentage of your code that was executed at least once while running the test suite:

```bash
npx vitest run --coverage
```

```
File                | % Statements | % Branches | % Functions | % Lines
--------------------|--------------|------------|-------------|--------
formatCurrency.ts    |        100   |       100  |        100  |    100
studentValidation.ts |         62   |        40  |         75  |     62
```

A low number on `studentValidation.ts` means real, untested code paths — worth investigating. But coverage measures **execution**, not **correctness** — a test that calls a function and asserts nothing meaningful (or asserts something trivially true) still counts as "covered":

```ts
// This "tests" formatCurrency and shows up as 100% covered —
// but asserts nothing useful, and would pass even if the function
// were completely broken
test("formatCurrency runs without crashing", () => {
  formatCurrency(100); // no expect() at all
});
```

**100% coverage with tests like this is worse than useless** — it creates false confidence. A team that chases the coverage number as a target, rather than as a signal pointing at untested code worth looking at, ends up with exactly this pattern.

---

## A Sane Way to Use Coverage

- Use it to find **obviously untested, risky code** — a payment calculation or an auth check at 40% coverage is worth immediate attention; a trivial getter at 60% usually isn't.
- Don't set a hard minimum threshold (like "builds fail below 80%") without also reviewing _what_ is and isn't covered — a threshold alone can be gamed with exactly the hollow test above.
- Treat coverage as one signal among several (code review, real bug reports, how critical the code path is), not the definition of "well tested."

---

## Common Mistakes

- **Not running E2E tests in a separate, later CI job.** Running everything in one job means a fast unit test failure still waits for slow E2E tests to finish before you find out — wasted time on every failed run.
- **Treating a CI test step that "warns" as good enough.** If a broken test doesn't actually fail the pipeline and block the merge, it's not really enforcing anything — confirm the exit code actually propagates.
- **Chasing a coverage percentage instead of using it as a signal.** Coverage tells you what code _ran_, not what code is _correct_ — a high number backed by shallow, assertion-free tests is a false signal, and it's worth spot-checking what a suspiciously perfect coverage report actually contains.
- **No database service in the CI job that runs integration tests**, leading to integration tests either being skipped in CI (defeating their purpose) or someone hard-coding a real database connection string into a workflow file (a security problem, not just a testing one).
