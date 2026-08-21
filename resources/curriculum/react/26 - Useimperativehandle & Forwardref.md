> React's data flow is top-down: parents pass props to children. But sometimes a parent needs to call a method on a child — focus an input, trigger an animation, scroll to a position. `forwardRef` lets a component expose its DOM node to a parent. `useImperativeHandle` lets a component control exactly what the parent can do with it.

---

## Table of Contents

1. [The Problem: Reaching Into Children](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-the-problem-reaching-into-children)
2. [forwardRef — Passing Refs Through Components](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-forwardref--passing-refs-through-components)
3. [useImperativeHandle — Controlling the Exposed API](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-useimperativehandle--controlling-the-exposed-api)
4. [TypeScript Typing](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-typescript-typing)
5. [Real-World Patterns](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-real-world-patterns)
6. [React 19: ref as a Prop](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-react-19-ref-as-a-prop)
7. [When Not to Use These](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-when-not-to-use-these)

---

## 1. The Problem: Reaching Into Children

```jsx
function SearchPage() {
  function handleSearchClick() {
    // How do we focus the SearchInput here?
    // We can't — we don't have a ref to the <input> inside it
  }

  return (
    <div>
      <button onClick={handleSearchClick}>Search</button>
      <SearchInput /> {/* <input> is buried inside this component */}
    </div>
  );
}

function SearchInput() {
  return (
    <div className="search-wrapper">
      <SearchIcon />
      <input type="search" placeholder="Search students..." />
    </div>
  );
}
```

You could move the `ref` to the outer `<div>` of `SearchInput` using `useRef`, but that gives you the wrapper — not the `<input>` you actually need.

**`forwardRef`** solves this by letting a component pass a ref it receives as a prop down to a specific DOM node inside it.

---

## 2. forwardRef — Passing Refs Through Components

`forwardRef` wraps a component and gives it a second parameter: the `ref` the parent passed.

```jsx
import { forwardRef } from "react";

// Without forwardRef — ref attaches to the wrapper div, not the input
function SearchInput({ placeholder }) {
  return (
    <div className="search-wrapper">
      <input type="search" placeholder={placeholder} />
    </div>
  );
}

// With forwardRef — ref is forwarded to the input
const SearchInput = forwardRef(function SearchInput({ placeholder }, ref) {
  return (
    <div className="search-wrapper">
      <input
        ref={ref} // attach the forwarded ref to the actual input
        type="search"
        placeholder={placeholder}
      />
    </div>
  );
});
```

Now the parent can pass a ref and get the `<input>` DOM node directly:

```jsx
import { useRef } from "react";

function SearchPage() {
  const searchRef = useRef(null);

  function handleSearchClick() {
    searchRef.current?.focus(); // directly focuses the <input>
  }

  return (
    <div>
      <button onClick={handleSearchClick}>Open Search</button>
      <SearchInput ref={searchRef} placeholder="Search students..." />
    </div>
  );
}
```

### The forwarded ref can be placed on any DOM node inside the component

```jsx
const Card = forwardRef(function Card({ children, className }, ref) {
  return (
    // ref goes on the element you want the parent to have access to
    <div ref={ref} className={`card ${className}`}>
      {children}
    </div>
  );
});

// Common use: scrolling a card into view from a parent
const cardRef = useRef(null);
cardRef.current?.scrollIntoView({ behavior: "smooth" });
```

---

## 3. useImperativeHandle — Controlling the Exposed API

By default, `forwardRef` exposes the raw DOM node. `useImperativeHandle` lets you expose a custom object instead — a controlled API with only the methods the parent should call.

```jsx
import { forwardRef, useImperativeHandle, useRef } from "react";

// Instead of exposing the raw <input> DOM node...
// Expose only what parents should be allowed to do
const SearchInput = forwardRef(function SearchInput(
  { placeholder, onSearch },
  ref,
) {
  const inputRef = useRef(null);

  useImperativeHandle(ref, () => ({
    focus() {
      inputRef.current?.focus();
    },
    clear() {
      if (inputRef.current) inputRef.current.value = "";
    },
    getValue() {
      return inputRef.current?.value ?? "";
    },
  }));

  return (
    <div className="search-wrapper">
      <input
        ref={inputRef} // internal ref — not exposed
        type="search"
        placeholder={placeholder}
        onKeyDown={(e) =>
          e.key === "Enter" && onSearch?.(e.currentTarget.value)
        }
      />
    </div>
  );
});
```

```jsx
function SearchPage() {
  const searchRef = useRef(null);

  function handleKeyShortcut(e) {
    if (e.key === "/" && e.metaKey) {
      searchRef.current?.focus(); // ✅ allowed
    }
  }

  function handleSubmit() {
    const query = searchRef.current?.getValue(); // ✅ allowed
    performSearch(query);
  }

  // searchRef.current.select()    ← ❌ not exposed — parent can't do this
  // searchRef.current.click()     ← ❌ not exposed
  // searchRef.current.style = ... ← ❌ not exposed
}
```

**Why restrict the API?** Because exposing the raw DOM node lets the parent do anything — modify styles, read internals, trigger native browser behaviour you didn't account for. `useImperativeHandle` makes the contract explicit and controlled.

### useImperativeHandle signature

```jsx
useImperativeHandle(
  ref, // the ref from forwardRef's second parameter
  () => ({
    // factory that returns the exposed object
    method() {},
    anotherMethod() {},
  }),
  [dependencies], // optional — re-run factory when these change
  // like useEffect deps — usually [] or stable values
);
```

---

## 4. TypeScript Typing

Type both the forwarded ref and the exposed handle precisely.

```tsx
import { forwardRef, useImperativeHandle, useRef } from "react";

// 1. Define the handle type (what the parent gets)
interface SearchInputHandle {
  focus: () => void;
  clear: () => void;
  getValue: () => string;
}

// 2. Define the props type
interface SearchInputProps {
  placeholder?: string;
  onSearch?: (query: string) => void;
  className?: string;
}

// 3. forwardRef<HandleType, PropsType>
const SearchInput = forwardRef<SearchInputHandle, SearchInputProps>(
  function SearchInput({ placeholder, onSearch, className }, ref) {
    const inputRef = useRef<HTMLInputElement>(null);

    useImperativeHandle(ref, () => ({
      focus() {
        inputRef.current?.focus();
      },
      clear() {
        if (inputRef.current) inputRef.current.value = "";
      },
      getValue() {
        return inputRef.current?.value ?? "";
      },
    }));

    return (
      <div className={`search-wrapper ${className ?? ""}`}>
        <input
          ref={inputRef}
          type="search"
          placeholder={placeholder}
          onKeyDown={(e) => {
            if (e.key === "Enter") onSearch?.(e.currentTarget.value);
          }}
        />
      </div>
    );
  },
);

// In the parent — fully typed
function SearchPage() {
  const searchRef = useRef<SearchInputHandle>(null);
  //                       ↑ type the ref with the handle type

  function handleShortcut() {
    searchRef.current?.focus(); // ✅ TypeScript knows this exists
    searchRef.current?.getValue(); // ✅
    // searchRef.current?.select()     ← TypeScript error — not in handle type
  }

  return <SearchInput ref={searchRef} placeholder="Search..." />;
}
```

### Typing forwardRef with just a DOM node (no useImperativeHandle)

```tsx
// When you just want to forward to a DOM element
const Button = forwardRef<HTMLButtonElement, ButtonProps>(function Button(
  { children, ...props },
  ref,
) {
  return (
    <button ref={ref} {...props}>
      {children}
    </button>
  );
});

// Parent gets a proper HTMLButtonElement ref
const btnRef = useRef<HTMLButtonElement>(null);
btnRef.current?.getBoundingClientRect(); // all native DOM methods available
```

---

## 5. Real-World Patterns

### Focus management (modals, dialogs, drawers)

```tsx
interface DialogHandle {
  open: () => void;
  close: () => void;
}

const Dialog = forwardRef<DialogHandle, DialogProps>(function Dialog(
  { title, children },
  ref,
) {
  const [isOpen, setIsOpen] = useState(false);
  const firstFocusableRef = useRef<HTMLButtonElement>(null);

  useImperativeHandle(ref, () => ({
    open() {
      setIsOpen(true);
      // Focus the first interactive element after open
      setTimeout(() => firstFocusableRef.current?.focus(), 0);
    },
    close() {
      setIsOpen(false);
    },
  }));

  if (!isOpen) return null;

  return (
    <div className="dialog-overlay" role="dialog" aria-modal>
      <div className="dialog-content">
        <h2>{title}</h2>
        {children}
        <button ref={firstFocusableRef} onClick={() => setIsOpen(false)}>
          Close
        </button>
      </div>
    </div>
  );
});

function ParentPage() {
  const dialogRef = useRef<DialogHandle>(null);

  return (
    <>
      <button onClick={() => dialogRef.current?.open()}>Open Dialog</button>
      <Dialog ref={dialogRef} title="Confirm Action">
        <p>Are you sure?</p>
      </Dialog>
    </>
  );
}
```

### Animation control

```tsx
interface AnimatedBannerHandle {
  show: () => void;
  dismiss: () => void;
}

const AnimatedBanner = forwardRef<AnimatedBannerHandle, BannerProps>(
  function AnimatedBanner({ message }, ref) {
    const bannerRef = useRef<HTMLDivElement>(null);

    useImperativeHandle(ref, () => ({
      show() {
        bannerRef.current?.animate(
          [
            { opacity: 0, transform: "translateY(-10px)" },
            { opacity: 1, transform: "translateY(0)" },
          ],
          { duration: 200, fill: "forwards" },
        );
      },
      dismiss() {
        bannerRef.current?.animate([{ opacity: 1 }, { opacity: 0 }], {
          duration: 150,
          fill: "forwards",
        }).onfinish = () => {
          bannerRef.current?.remove();
        };
      },
    }));

    return (
      <div ref={bannerRef} className="banner">
        {message}
      </div>
    );
  },
);
```

### Forwarding refs in component library components

Components built on Radix UI primitives (like in Nexus) use `forwardRef` throughout so consumers can access the underlying element:

```tsx
// Wrapping a Radix primitive and forwarding the ref
import * as RadixDialog from "@radix-ui/react-dialog";

const DialogContent = forwardRef<
  React.ElementRef<typeof RadixDialog.Content>,
  React.ComponentPropsWithoutRef<typeof RadixDialog.Content>
>(function DialogContent({ className, children, ...props }, ref) {
  return (
    <RadixDialog.Portal>
      <RadixDialog.Overlay className="dialog-overlay" />
      <RadixDialog.Content
        ref={ref} // forward ref to the Radix Content element
        className={cn("dialog-content", className)}
        {...props}
      >
        {children}
      </RadixDialog.Content>
    </RadixDialog.Portal>
  );
});

// This is exactly how shadcn/ui components are built
```

---

## 6. React 19: ref as a Prop

React 19 removes the need for `forwardRef`. Refs are now just props.

```jsx
// React 18 — requires forwardRef
const SearchInput = forwardRef(function SearchInput({ placeholder }, ref) {
  return <input ref={ref} placeholder={placeholder} />;
});

// React 19 — ref is just a regular prop
function SearchInput({ placeholder, ref }) {
  return <input ref={ref} placeholder={placeholder} />;
}

// Usage is identical in both versions
<SearchInput ref={searchRef} placeholder="Search..." />;
```

`useImperativeHandle` still works the same way in React 19 — you just don't need `forwardRef` wrapping it:

```jsx
// React 19
function SearchInput({ placeholder, ref }) {
  const inputRef = useRef(null);

  useImperativeHandle(ref, () => ({
    focus: () => inputRef.current?.focus(),
    clear: () => {
      if (inputRef.current) inputRef.current.value = "";
    },
  }));

  return <input ref={inputRef} placeholder={placeholder} />;
}
```

React 18 `forwardRef` syntax still works in React 19 — existing code doesn't break. The new syntax is just cleaner.

---

## 7. When Not to Use These

`forwardRef` and `useImperativeHandle` are **escape hatches** — reach for them only when declarative props genuinely can't solve the problem.

```
Use forwardRef when:
  ✅ Parent needs to call a DOM method (focus, scroll, play, pause)
  ✅ You're building a component library where callers need ref access
  ✅ Integrating with a third-party library that needs a DOM node
  ✅ Animation imperatives that don't fit in state

Do NOT use forwardRef when:
  ❌ You want to pass data from child to parent — use callback props instead
  ❌ You want to trigger a re-render in a child — use state lifted to parent
  ❌ You want to share state between siblings — lift state up or use context
```

```jsx
// Wrong — using ref to read child state
function Parent() {
  const childRef = useRef();

  function handleClick() {
    const value = childRef.current.getValue(); // reads child state via ref
    doSomething(value);
  }
}

// Right — lift state to parent
function Parent() {
  const [value, setValue] = useState("");

  function handleClick() {
    doSomething(value); // parent owns the state
  }

  return <Input value={value} onChange={setValue} />;
}
```

The decision tree:

```
Can I solve this with props and state? → Yes → use props and state
Does the parent need to call a method on a DOM node? → Yes → forwardRef
Does the parent need a controlled API (not raw DOM)? → Yes → add useImperativeHandle
```

---

## Summary

```
forwardRef:
  Wraps a component so it can receive and forward a ref from its parent
  forwardRef((props, ref) => JSX) — ref is the second parameter
  Attach ref to the DOM node you want the parent to access

useImperativeHandle:
  Replaces the default ref value with a custom object
  useImperativeHandle(ref, () => ({ method() {}, ... }), deps?)
  Gives parents a clean API instead of raw DOM access

TypeScript:
  forwardRef<HandleType, PropsType>
  useRef<HandleType>(null) in the parent
  Define a separate interface for the handle

Common use cases:
  focus() / blur() — inputs in search bars, modals, drawers
  scroll() — scroll list to specific item
  play() / pause() — media players (AudioPlayer in Nexus)
  open() / close() — controlled dialogs from parent
  show() / dismiss() — animations

React 19:
  ref is just a prop — forwardRef wrapper no longer needed
  useImperativeHandle still works the same way
  Existing forwardRef code continues to work

When to use vs avoid:
  ✅ DOM imperative methods (focus, scroll, play)
  ✅ Component library public APIs
  ❌ Data flow parent ↔ child — use props/callbacks
  ❌ Shared state — lift up or use context
```

---

_Next: [27 — React Router vs Next.js App Router](https://claude.ai/chat/27%20-%20React%20Router%20vs%20Next.js%20App%20Router.md)_
