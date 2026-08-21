> Testing gives you confidence that your code works and keeps working as it changes. React Testing Library (RTL) is the standard — it tests components the way users interact with them, not implementation details. This makes tests resilient to refactoring and meaningful as documentation.

---

## Table of Contents

1. [Testing Philosophy](#1-testing-philosophy)
2. [Setup](#2-setup)
3. [Rendering and Querying](#3-rendering-and-querying)
4. [User Interactions](#4-user-interactions)
5. [Async Testing](#5-async-testing)
6. [Mocking](#6-mocking)
7. [Testing Custom Hooks](#7-testing-custom-hooks)
8. [Common Patterns](#8-common-patterns)
9. [What NOT to Test](#9-what-not-to-test)

---

## 1. Testing Philosophy

React Testing Library's guiding principle:

> **"The more your tests resemble the way your software is used, the more confidence they can give you."**

This means:

- Query elements the way users find them: by label text, role, text content — not by CSS class or component name
- Interact the way users do: click, type, tab — not call internal methods
- Assert what users see: what's visible on screen — not internal state

```tsx
// ✗ Implementation detail test
const wrapper = mount(<LoginForm />);
expect(wrapper.state("email")).toBe("test@example.com");
// This test would break if you renamed the state variable — meaningless

// ✓ Behaviour test
render(<LoginForm />);
await userEvent.type(screen.getByLabelText("Email"), "test@example.com");
// This tests what actually matters — the user can type in the email field
```

---

## 2. Setup

```bash
npm install --save-dev vitest @testing-library/react @testing-library/user-event @testing-library/jest-dom jsdom
```

```typescript
// vitest.config.ts
import { defineConfig } from "vitest/config";
import react from "@vitejs/plugin-react";

export default defineConfig({
  plugins: [react()],
  test: {
    environment: "jsdom",
    globals: true,
    setupFiles: ["./src/test/setup.ts"],
  },
});
```

```typescript
// src/test/setup.ts
import "@testing-library/jest-dom";
// adds custom matchers: toBeInTheDocument, toBeVisible, toHaveValue, etc.
```

---

## 3. Rendering and Querying

```tsx
import { render, screen } from "@testing-library/react";

// render() renders the component into a jsdom environment
render(<StudentCard student={mockStudent} onDelete={vi.fn()} />);

// screen.getBy* — throws if not found (good for elements that should always be there)
screen.getByText("Ashan Perera"); // exact text match
screen.getByText(/ashan/i); // regex, case-insensitive
screen.getByRole("button", { name: "Delete" }); // by ARIA role + accessible name
screen.getByRole("heading", { level: 2 }); // h2 element
screen.getByLabelText("Email"); // input associated with label
screen.getByPlaceholderText("Search students...");
screen.getByTestId("score-display"); // data-testid attribute (last resort)

// screen.queryBy* — returns null if not found (for asserting absence)
expect(screen.queryByText("Error")).toBeNull();
expect(screen.queryByRole("dialog")).not.toBeInTheDocument();

// screen.findBy* — async, waits for element to appear
const heading = await screen.findByRole("heading", { name: "Students" });
// Useful for async rendering

// screen.getAllBy* / queryAllBy* / findAllBy* — returns array
const buttons = screen.getAllByRole("button");
expect(buttons).toHaveLength(3);
```

### Priority order for queries

```
1. getByRole              — uses ARIA roles, most aligned with accessibility
2. getByLabelText         — for form fields
3. getByPlaceholderText   — second choice for inputs
4. getByText              — for non-interactive content
5. getByDisplayValue      — for select, input, textarea current value
6. getByAltText           — for images
7. getByTestId            — last resort — add data-testid to elements if needed
```

---

## 4. User Interactions

```tsx
import userEvent from "@testing-library/user-event";

// Always set up userEvent at the start
const user = userEvent.setup();

// Typing
await user.type(screen.getByLabelText("Email"), "ashan@school.lk");
await user.clear(screen.getByLabelText("Email"));
await user.type(screen.getByLabelText("Email"), "new@email.com");

// Clicking
await user.click(screen.getByRole("button", { name: "Submit" }));
await user.dblClick(screen.getByText("Double click me"));

// Selecting from a dropdown
await user.selectOptions(screen.getByRole("combobox", { name: "Grade" }), "11");

// Keyboard
await user.keyboard("{Enter}");
await user.keyboard("{Escape}");
await user.keyboard("{Tab}");
await user.keyboard("Hello World"); // type text
await user.keyboard("{Control>}a{/Control}"); // Ctrl+A (select all)

// Hover
await user.hover(screen.getByRole("button", { name: "Info" }));
await user.unhover(screen.getByRole("button", { name: "Info" }));

// Upload file
const file = new File(["content"], "document.pdf", { type: "application/pdf" });
await user.upload(screen.getByLabelText("Upload document"), file);
```

---

## 5. Async Testing

```tsx
import { render, screen, waitFor } from "@testing-library/react";

// waitFor — wait for assertion to pass (retries until it does or times out)
await waitFor(() => {
  expect(screen.getByText("Student saved!")).toBeInTheDocument();
});

// findBy* — equivalent to getBy* + waitFor (preferred for single elements)
const successMessage = await screen.findByText("Student saved!");
expect(successMessage).toBeInTheDocument();

// waitForElementToBeRemoved — wait for something to disappear
render(<StudentList />);
const spinner = screen.getByRole("progressbar");
await waitForElementToBeRemoved(spinner);
// Now the list is loaded

// Testing loading state then success
test("shows loading then student data", async () => {
  render(<StudentProfile id="s123" />);

  // Initially loading
  expect(screen.getByRole("progressbar")).toBeInTheDocument();

  // After load
  await screen.findByRole("heading", { name: "Ashan Perera" });
  expect(screen.queryByRole("progressbar")).not.toBeInTheDocument();
});
```

---

## 6. Mocking

### Mocking API calls

```tsx
// vi.fn() — basic mock function
const mockOnDelete = vi.fn();
render(<StudentCard student={mockStudent} onDelete={mockOnDelete} />);
await user.click(screen.getByRole("button", { name: "Delete" }));
expect(mockOnDelete).toHaveBeenCalledWith("s123");
expect(mockOnDelete).toHaveBeenCalledTimes(1);

// Mocking fetch
vi.stubGlobal(
  "fetch",
  vi.fn().mockResolvedValue({
    ok: true,
    json: () => Promise.resolve({ students: [mockStudent] }),
  }),
);

// Mocking modules
vi.mock("./api/students", () => ({
  fetchStudents: vi.fn().mockResolvedValue([mockStudent1, mockStudent2]),
  deleteStudent: vi.fn().mockResolvedValue({ success: true }),
}));

// Mock with different responses
import { fetchStudents } from "./api/students";

vi.mocked(fetchStudents)
  .mockResolvedValueOnce([]) // first call returns empty
  .mockResolvedValueOnce([mockStudent1]); // second call returns one student
```

### Wrapping with providers

Most components need context providers (React Query, Router, Auth). Create a test wrapper.

```tsx
// src/test/test-utils.tsx
import { render, RenderOptions } from "@testing-library/react";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { MemoryRouter } from "react-router-dom";

function createTestQueryClient() {
  return new QueryClient({
    defaultOptions: {
      queries: {
        retry: false, // don't retry in tests — fail fast
        gcTime: 0, // don't cache between tests
      },
    },
  });
}

interface WrapperProps {
  children: React.ReactNode;
}

function AllProviders({ children }: WrapperProps) {
  const queryClient = createTestQueryClient();

  return (
    <QueryClientProvider client={queryClient}>
      <MemoryRouter>{children}</MemoryRouter>
    </QueryClientProvider>
  );
}

// Custom render — use this instead of RTL's render
function customRender(ui: React.ReactElement, options?: RenderOptions) {
  return render(ui, { wrapper: AllProviders, ...options });
}

export * from "@testing-library/react";
export { customRender as render };

// Tests import from test-utils instead of @testing-library/react
import { render, screen } from "../test/test-utils";
```

---

## 7. Testing Custom Hooks

Use `renderHook` from RTL for testing hooks directly.

```tsx
import { renderHook, act } from "@testing-library/react";
import { useCounter } from "./useCounter";

test("useCounter increments and decrements", () => {
  const { result } = renderHook(() => useCounter(10));

  expect(result.current.count).toBe(10);

  act(() => result.current.increment());
  expect(result.current.count).toBe(11);

  act(() => result.current.decrement());
  expect(result.current.count).toBe(10);

  act(() => result.current.reset());
  expect(result.current.count).toBe(10);
});

test("useCounter respects min and max", () => {
  const { result } = renderHook(() => useCounter(0, { min: 0, max: 5 }));

  act(() => result.current.decrement());
  expect(result.current.count).toBe(0); // can't go below min

  for (let i = 0; i < 10; i++) {
    act(() => result.current.increment());
  }
  expect(result.current.count).toBe(5); // can't exceed max
});
```

---

## 8. Common Patterns

### Full form test

```tsx
test("submits form with valid data", async () => {
  const mockSubmit = vi.fn();
  const user = userEvent.setup();

  render(<ContactForm onSubmit={mockSubmit} />);

  await user.type(screen.getByLabelText("Name"), "Ashan Perera");
  await user.type(screen.getByLabelText("Email"), "ashan@school.lk");
  await user.type(screen.getByLabelText("Message"), "Hello from CWWKCC");
  await user.click(screen.getByRole("button", { name: "Send" }));

  await waitFor(() => {
    expect(mockSubmit).toHaveBeenCalledWith({
      name: "Ashan Perera",
      email: "ashan@school.lk",
      message: "Hello from CWWKCC",
    });
  });
});

test("shows validation errors for empty form", async () => {
  const user = userEvent.setup();
  render(<ContactForm onSubmit={vi.fn()} />);

  await user.click(screen.getByRole("button", { name: "Send" }));

  expect(await screen.findByText("Name is required")).toBeInTheDocument();
  expect(screen.getByText("Email is required")).toBeInTheDocument();
  expect(screen.getByText("Message is required")).toBeInTheDocument();
});
```

### Accessibility assertions

```tsx
test("modal is accessible", async () => {
  const user = userEvent.setup();
  render(<Modal trigger={<button>Open</button>} content="Hello" />);

  await user.click(screen.getByRole("button", { name: "Open" }));

  const dialog = screen.getByRole("dialog");
  expect(dialog).toBeInTheDocument();
  expect(dialog).toHaveAttribute("aria-modal", "true");

  // Focus should move to modal
  expect(document.activeElement).toBeInTheDocument();
  // Focus should be trapped inside modal
  await user.keyboard("{Tab}");
  expect(dialog).toContainElement(document.activeElement as HTMLElement);

  // Close with Escape
  await user.keyboard("{Escape}");
  expect(screen.queryByRole("dialog")).not.toBeInTheDocument();
});
```

### Snapshot tests (use sparingly)

```tsx
test("StudentCard renders correctly", () => {
  const { container } = render(
    <StudentCard student={mockStudent} onDelete={vi.fn()} />,
  );
  expect(container.firstChild).toMatchSnapshot();
});
// Snapshot tests catch unexpected UI changes
// But they're brittle — any styling change breaks them
// Use for stable, intentionally static components only
```

---

## 9. What NOT to Test

```tsx
// ✗ Don't test implementation details
expect(component.state("isOpen")).toBe(true); // internal state
expect(component.find("div.container")).toExist(); // CSS class names

// ✗ Don't test library behaviour
expect(screen.getByRole("button")).toBeEnabled(); // fine, but...
// Don't test that clicking a disabled button doesn't fire — the browser handles that

// ✗ Don't test third-party libraries
// Test that YOUR code uses date-fns correctly, not that date-fns works

// ✗ Don't duplicate unit tests in integration tests
// If useStudentData is already unit tested, don't test its internals again
// in a component test — just test that the component renders correctly with data

// ✓ DO test:
// - User interactions produce expected visible results
// - Error states are shown correctly
// - Form validation works from the user's perspective
// - Async flows complete correctly
// - Accessibility attributes are present
// - Business logic (reducers, utilities) — unit test these separately
```

---

## Summary

```
Philosophy:
  Test behaviour, not implementation
  Query as users find: role > label > text > testId
  Interact as users do: userEvent.click/type/keyboard

Core API:
  render(<Component />)          — mount component into jsdom
  screen.getByRole("button")     — find visible elements
  screen.queryBy*                — returns null (for absence checks)
  screen.findBy*                 — async, waits for element

userEvent (always use over fireEvent):
  user = userEvent.setup()
  await user.click(element)
  await user.type(element, "text")
  await user.selectOptions(element, "value")
  await user.keyboard("{Enter}")

Async:
  await screen.findByText("...")        — wait for element to appear
  await waitFor(() => expect(...))       — retry until assertion passes
  await waitForElementToBeRemoved(el)   — wait for removal

Mocking:
  vi.fn() — mock function, check calls with .toHaveBeenCalledWith()
  vi.mock("./module") — mock an entire module
  vi.mocked(fn) — typed mock wrapper

Test wrapper:
  createTestQueryClient() with retry: false
  MemoryRouter for routing
  Custom render() that wraps all providers

Custom hooks:
  renderHook(() => useMyHook())
  act(() => result.current.action())

What to test:
  User interactions → visible results
  Loading / error / empty states
  Form validation from user perspective
  Accessibility (ARIA roles, focus management)

What NOT to test:
  Internal state, CSS class names
  Third-party library behaviour
  Implementation details that could change without breaking behaviour
```

---

_Next: [24 — Component Library (shadcn & Radix)](<./24%20-%20Component%20Library%20(shadcn%20%26%20Radix).md>)_
