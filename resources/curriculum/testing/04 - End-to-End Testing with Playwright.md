Unit and integration tests never open an actual browser. **End-to-end (E2E) tests** do — they drive a real browser through a real user flow against your actual running app, catching the class of bug that only shows up when everything is genuinely connected: a broken redirect, a button that's technically rendered but visually unclickable, a form that submits correctly but never shows the success message.

---

## Why E2E Tests Are Different (and Used Sparingly)

Referring back to the pyramid in Module 00: E2E tests sit at the top because they're the slowest and most expensive to write and maintain — but they're also the only test that verifies the _whole real system_, browser rendering and all, actually works. The right amount is a handful of tests covering the flows where a failure would be genuinely bad: logging in, submitting the contact form, an admin publishing a page — not "every possible click path."

---

## Setting Up Playwright

```bash
npm init playwright@latest
```

This scaffolds a `tests/` (or `e2e/`) folder, a `playwright.config.ts`, and installs real browser binaries (Chromium, Firefox, WebKit) that Playwright drives directly.

```ts
// playwright.config.ts
import { defineConfig } from "@playwright/test";

export default defineConfig({
  testDir: "./e2e",
  use: {
    baseURL: "http://localhost:3000",
    trace: "on-first-retry", // records a full trace if a test fails, then retries
  },
  webServer: {
    command: "npm run dev", // Playwright starts your app automatically before tests
    url: "http://localhost:3000",
    reuseExistingServer: true,
  },
});
```

---

## A First Test

```ts
// e2e/homepage.spec.ts
import { test, expect } from "@playwright/test";

test("homepage loads and shows the school name", async ({ page }) => {
  await page.goto("/");
  await expect(
    page.getByRole("heading", { name: /C.W.W. Kannangara/i }),
  ).toBeVisible();
});
```

`page` is a real, automated browser tab. `getByRole` queries the page the way a screen reader or a real user would — by its accessible role and visible text — which is the same philosophy `react/23`'s React Testing Library follows for components, applied here to a whole rendered page.

---

## Testing a Real User Flow

```ts
// e2e/contact-form.spec.ts
import { test, expect } from "@playwright/test";

test("visitor can submit the contact form successfully", async ({ page }) => {
  await page.goto("/contact");

  await page.getByLabel("Full name").fill("Ashan Perera");
  await page.getByLabel("Email").fill("ashan@example.com");
  await page.getByLabel("Message").fill("Interested in the ICT program.");

  await page.getByRole("button", { name: "Send message" }).click();

  await expect(page.getByText("Thank you — we'll be in touch.")).toBeVisible();
});

test("shows a validation error for an invalid email", async ({ page }) => {
  await page.goto("/contact");

  await page.getByLabel("Email").fill("not-an-email");
  await page.getByRole("button", { name: "Send message" }).click();

  await expect(
    page.getByText("Please enter a valid email address"),
  ).toBeVisible();
});
```

This exercises the real form component, the real client-side validation, the real form submission, and the real success/error UI — end to end, exactly as a visitor would experience it.

---

## Waiting for Things — Playwright's Auto-Waiting

```ts
// WRONG instinct coming from other tools — arbitrary, flaky waits
await page.waitForTimeout(2000); // hopes 2 seconds is "enough" — sometimes isn't,
// and wastes time when it's more than enough

// CORRECT — Playwright's actions and assertions already auto-wait,
// retrying until the element appears/matches or a timeout is hit
await expect(page.getByText("Thank you")).toBeVisible();
```

Playwright's built-in assertions (`expect(locator).toBeVisible()`, `.toHaveText()`, etc.) already retry automatically for a few seconds before failing — there's rarely a good reason to add a manual, fixed-length wait. Reaching for `waitForTimeout` is one of the most common sources of **flaky tests** (tests that sometimes pass and sometimes fail with no code change) — either the wait is too short on a slow CI machine, or needlessly long and slows the whole suite down.

---

## Locating Elements — Preferring Accessible Queries

```ts
// FRAGILE — breaks the moment a class name changes for styling reasons
await page.locator(".btn.btn-primary.submit-btn").click();

// ROBUST — targets the same thing a real user (or screen reader) would find
await page.getByRole("button", { name: "Submit" }).click();
```

| Locator       | Prefer for                                                                                                 |
| ------------- | ---------------------------------------------------------------------------------------------------------- |
| `getByRole`   | Buttons, links, headings, form controls — the default first choice                                         |
| `getByLabel`  | Form inputs (matches the pattern from `html/07 - Forms`)                                                   |
| `getByText`   | Visible text content                                                                                       |
| `getByTestId` | Last resort, for elements with no meaningful role/label/text — requires adding `data-testid` in the markup |

This mirrors the "test behavior, not implementation" philosophy from `react/23` — a CSS class is an implementation detail that changes for styling reasons unrelated to whether the feature actually works; a button's accessible role and name almost never change unintentionally.

---

## Running in CI

```bash
npx playwright test              # run all E2E tests
npx playwright test --ui         # interactive mode — great for writing/debugging locally
npx playwright show-report       # view the HTML report of the last run, including traces
```

E2E tests are the slowest tests in the suite and usually run less frequently than unit/integration tests in CI (Module 05) — often on every pull request rather than on every single commit, or as a separate, longer-running job.

---

## Common Mistakes

- **Testing every possible path end-to-end** instead of reserving E2E for the handful of flows that genuinely need whole-system confidence. This is the inverted pyramid problem from Module 00, and it makes the suite slow and brittle.
- **Fixed-length waits (`waitForTimeout`) instead of Playwright's built-in auto-waiting assertions.** This is the single biggest source of flaky E2E tests.
- **Locating elements by CSS class or DOM structure instead of accessible role/label/text.** A locator like `.container > div:nth-child(3) > button` breaks on any unrelated layout change; `getByRole("button", { name: "Submit" })` doesn't.
- **Running E2E tests against a shared, real database** the same way development does. Like integration tests (Module 03), E2E tests should run against a dedicated test environment — otherwise a form-submission test is creating real, junk rows in real data.
