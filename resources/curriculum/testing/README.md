---
tags: [MOC, testing, curriculum]
---

# 🧪 Testing Curriculum

Automated testing from first principles through CI integration: the testing pyramid, unit tests, mocking, integration tests, and end-to-end tests with Playwright. 6 modules.

> **Location:** `resources/curriculum/testing/`
>
> **Relationship to `react/23 - Testing React Components`:** that module covers React Testing Library specifically — rendering components, simulating user interaction, testing hooks. This track covers everything React's testing module doesn't: the testing pyramid and philosophy, plain-function unit testing, mocking/test doubles in depth, integration testing against a real database, and full E2E browser testing. Read this track's Module 00 first for the conceptual foundation, then `react/23` for component-specific patterns.

---

## Module Index

| #   | Topic                                       | File                                                  |
| --- | ------------------------------------------- | ----------------------------------------------------- |
| 00  | Why Automated Testing & The Testing Pyramid | `00 - Why Automated Testing & The Testing Pyramid.md` |
| 01  | Unit Testing Fundamentals                   | `01 - Unit Testing Fundamentals.md`                   |
| 02  | Mocking & Test Doubles                      | `02 - Mocking & Test Doubles.md`                      |
| 03  | Integration Testing                         | `03 - Integration Testing.md`                         |
| 04  | End-to-End Testing with Playwright          | `04 - End-to-End Testing with Playwright.md`          |
| 05  | Testing in CI & Coverage                    | `05 - Testing in CI & Coverage.md`                    |

---

## 🔗 Cross‑Curriculum Links

| Module                     | Related Topics                                                                                                                                           |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `00 - Testing Pyramid`     | → [[../react/23 - Testing React Components]]                                                                                                             |
| `01 - Unit Testing`        | → [[../javascript/06 - Asynchronous JavaScript & APIs]] (async test patterns)                                                                            |
| `03 - Integration Testing` | → [[../database/03 - Joins & Relationships]], [[../database/06 - Prisma ORM Patterns for This Stack]]                                                    |
| `05 - Testing in CI`       | → [[../git/06 -  Conventional Commits and Semantic Versioning]] (GitHub Actions), [[../html/14 - Validation and Best Practices]] (CI validation pattern) |

## 📝 Assignments

- [[../../assignments/testing/test-your-own-projects/Assignment|test-your-own-projects]] – write real unit tests for the JavaScript curriculum's `student-grade-calculator` and `interactive-todo-list` assignments, including a hands-on refactor to separate testable logic from DOM code
