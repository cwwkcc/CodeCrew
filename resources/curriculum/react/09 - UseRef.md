> `useRef` is the escape hatch from React's rendering model. It gives you a mutable container that persists across renders without causing re-renders. Its two main uses are accessing DOM nodes directly and storing values that need to survive renders but shouldn't trigger them.

---

## Table of Contents

1. [What useRef Is](#1-what-useref-is)
2. [DOM Access — The Primary Use Case](#2-dom-access--the-primary-use-case)
3. [Storing Mutable Values](#3-storing-mutable-values)
4. [The Difference Between ref and state](#4-the-difference-between-ref-and-state)
5. [Callback Refs](#5-callback-refs)
6. [Forwarding Refs](#6-forwarding-refs)
7. [useImperativeHandle](#7-useimperativehandle)
8. [Common Patterns](#8-common-patterns)

---

## 1. What `useRef` Is

`useRef` returns an object `{ current: initialValue }`. This object:

- Persists for the full lifetime of the component
- Can be mutated freely — `ref.current = newValue`
- **Does not cause a re-render when mutated**

```jsx
const ref = useRef(0);

ref.current; // 0 initially
ref.current = 42; // mutate freely — no re-render
ref.current; // 42

// The ref object itself never changes — same reference every render
// Only ref.current changes
```

Think of `useRef` as a component-scoped instance variable — like a property on `this` in a class component. It exists outside React's data flow.

---

## 2. DOM Access — The Primary Use Case

Pass a ref to a JSX element's `ref` prop. React will set `ref.current` to the DOM node after mounting, and `null` after unmounting.

```jsx
function SearchInput({ onSearch }) {
  const inputRef = useRef(null);

  // Focus the input when the component mounts
  useEffect(() => {
    inputRef.current.focus();
  }, []);

  function handleClear() {
    inputRef.current.value = ""; // direct DOM mutation (unusual but valid)
    inputRef.current.focus();
  }

  return (
    <div>
      <input ref={inputRef} placeholder="Search..." />
      <button onClick={handleClear}>Clear</button>
    </div>
  );
}
```

### Common DOM ref patterns

```jsx
// Focus management
function Modal({ isOpen, onClose }) {
  const closeButtonRef = useRef(null);

  useEffect(() => {
    if (isOpen) {
      closeButtonRef.current?.focus(); // focus close button when modal opens
    }
  }, [isOpen]);

  return isOpen ? (
    <div role="dialog" aria-modal="true">
      <button ref={closeButtonRef} onClick={onClose}>
        ×
      </button>
      {/* ... */}
    </div>
  ) : null;
}

// Scroll into view
function ChatMessages({ messages }) {
  const bottomRef = useRef(null);

  useEffect(() => {
    bottomRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]); // scroll down when new messages arrive

  return (
    <div className="messages">
      {messages.map((msg) => (
        <Message key={msg.id} message={msg} />
      ))}
      <div ref={bottomRef} /> {/* invisible sentinel at the bottom */}
    </div>
  );
}

// Measuring element dimensions
function useElementSize(ref) {
  const [size, setSize] = useState({ width: 0, height: 0 });

  useLayoutEffect(() => {
    if (!ref.current) return;

    const observer = new ResizeObserver(([entry]) => {
      setSize({
        width: entry.contentRect.width,
        height: entry.contentRect.height,
      });
    });

    observer.observe(ref.current);
    return () => observer.disconnect();
  }, [ref]);

  return size;
}

function AdaptiveChart() {
  const containerRef = useRef(null);
  const { width, height } = useElementSize(containerRef);

  return (
    <div ref={containerRef} style={{ width: "100%" }}>
      <canvas width={width} height={height} />
    </div>
  );
}

// Media playback control
function VideoPlayer({ src }) {
  const videoRef = useRef(null);

  function handlePlay() {
    videoRef.current.play();
  }
  function handlePause() {
    videoRef.current.pause();
  }
  function seek(seconds) {
    videoRef.current.currentTime = seconds;
  }

  return (
    <div>
      <video ref={videoRef} src={src} />
      <button onClick={handlePlay}>Play</button>
      <button onClick={handlePause}>Pause</button>
    </div>
  );
}
```

---

## 3. Storing Mutable Values

Refs are perfect for values that need to persist across renders but don't affect the UI — storing them in state would cause unnecessary re-renders.

### Storing the previous value of state

```jsx
function usePrevious(value) {
  const ref = useRef(undefined);

  useEffect(() => {
    ref.current = value;
    // This runs after render, so during render,
    // ref.current still holds the previous render's value
  });

  return ref.current;
}

function AnimatedCounter({ count }) {
  const prevCount = usePrevious(count);
  const direction = count > prevCount ? "up" : "down";

  return <span className={`counter animate-${direction}`}>{count}</span>;
}
```

### Timer IDs

```jsx
function Notification({ message, duration = 3000, onDismiss }) {
  const timerRef = useRef(null);

  useEffect(() => {
    timerRef.current = setTimeout(onDismiss, duration);
    return () => clearTimeout(timerRef.current);
  }, [duration, onDismiss]);

  function handleDismiss() {
    clearTimeout(timerRef.current); // cancel auto-dismiss
    onDismiss();
  }

  return (
    <div className="notification">
      <p>{message}</p>
      <button onClick={handleDismiss}>×</button>
    </div>
  );
}
```

### Tracking if component is mounted

```jsx
// Prevent setState after unmount (avoids "update on unmounted component" warning)
function useIsMounted() {
  const isMounted = useRef(false);

  useEffect(() => {
    isMounted.current = true;
    return () => {
      isMounted.current = false;
    };
  }, []);

  return isMounted;
}

function DataLoader({ id }) {
  const [data, setData] = useState(null);
  const isMounted = useIsMounted();

  useEffect(() => {
    fetchData(id).then((result) => {
      if (isMounted.current) {
        // only update state if still mounted
        setData(result);
      }
    });
  }, [id]);
}
```

### Stale closure workaround

```jsx
// Store latest callback in a ref to avoid stale closures in intervals
function useInterval(callback, delay) {
  const callbackRef = useRef(callback);

  // Keep ref up to date with the latest callback
  useEffect(() => {
    callbackRef.current = callback;
  }, [callback]);

  useEffect(() => {
    const id = setInterval(() => {
      callbackRef.current(); // always calls the latest version
    }, delay);
    return () => clearInterval(id);
  }, [delay]); // only restarts if delay changes, not on every callback change
}

function Timer() {
  const [count, setCount] = useState(0);

  useInterval(() => {
    setCount((c) => c + 1);
  }, 1000);

  return <p>{count}</p>;
}
```

---

## 4. The Difference Between `ref` and State

```
                    State (useState)         Ref (useRef)
────────────────────────────────────────────────────────────
Causes re-render    Yes                      No
Mutable during      No (use setter)          Yes (ref.current = ...)
  render
Value persists      Yes (React manages)      Yes (same object)
Use for             UI data                  Infrastructure / DOM
Read during         Always current           May be stale
  render            value
```

```jsx
// When to use state vs ref:

// STATE — does the UI need to reflect this value?
const [count, setCount] = useState(0);
return <p>Count: {count}</p>; // yes, UI shows count

// REF — does this need to persist, but NOT affect UI?
const timerIdRef = useRef(null);
// timerIdRef.current is used by cleanup, never rendered
```

---

## 5. Callback Refs

Instead of a ref object, you can pass a function to `ref`. React calls it with the DOM node when the element mounts, and with `null` when it unmounts.

```jsx
// Useful when you need to run logic the moment a node is attached
function MeasuredBox() {
  const [height, setHeight] = useState(0);

  const measuredRef = useCallback((node) => {
    if (node !== null) {
      setHeight(node.getBoundingClientRect().height);
    }
  }, []);

  return (
    <div ref={measuredRef}>
      <p>This box is {height}px tall</p>
    </div>
  );
}

// Callback refs are also how you set refs on conditionally rendered elements
function Component({ showInput }) {
  const [inputNode, setInputNode] = useState(null);

  useEffect(() => {
    if (inputNode) inputNode.focus();
  }, [inputNode]);

  return showInput ? <input ref={setInputNode} /> : null;
}
```

---

## 6. Forwarding Refs

By default, `ref` is not a prop — you can't pass a ref to a custom component the same way you pass it to `<div>` or `<input>`. Use `forwardRef` to forward a ref through a component to a DOM element inside it.

```jsx
import { forwardRef } from "react";

// Wrap the component with forwardRef
const Input = forwardRef(function Input({ label, ...props }, ref) {
  return (
    <div className="field">
      <label>{label}</label>
      <input ref={ref} {...props} />
      {/* The ref is forwarded to the actual <input> DOM node */}
    </div>
  );
});

// Now the parent can get a ref to the <input> inside Input
function LoginForm() {
  const emailRef = useRef(null);

  useEffect(() => {
    emailRef.current.focus();
  }, []);

  return (
    <form>
      <Input
        ref={emailRef} // ← this now reaches the <input> inside Input
        label="Email"
        type="email"
      />
    </form>
  );
}
```

---

## 7. `useImperativeHandle`

Used with `forwardRef` to customise what value the parent receives when it uses `ref` — instead of exposing the raw DOM node, you expose specific methods.

```jsx
import { forwardRef, useImperativeHandle } from "react";

const VideoPlayer = forwardRef(function VideoPlayer({ src }, ref) {
  const videoRef = useRef(null);

  // Expose only specific methods to the parent
  useImperativeHandle(
    ref,
    () => ({
      play() {
        videoRef.current.play();
      },
      pause() {
        videoRef.current.pause();
      },
      seek(time) {
        videoRef.current.currentTime = time;
      },
      // The parent CANNOT access videoRef.current directly
      // They can only use these three methods
    }),
    [],
  );

  return <video ref={videoRef} src={src} />;
});

function Page() {
  const playerRef = useRef(null);

  return (
    <div>
      <VideoPlayer ref={playerRef} src="/lecture.mp4" />
      <button onClick={() => playerRef.current.play()}>Play</button>
      <button onClick={() => playerRef.current.seek(120)}>Skip to 2min</button>
    </div>
  );
}
```

---

## 8. Common Patterns

### Click outside to close

```jsx
function useClickOutside(ref, handler) {
  useEffect(() => {
    function handleClick(e) {
      if (ref.current && !ref.current.contains(e.target)) {
        handler();
      }
    }
    document.addEventListener("mousedown", handleClick);
    return () => document.removeEventListener("mousedown", handleClick);
  }, [ref, handler]);
}

function Dropdown({ children, onClose }) {
  const dropdownRef = useRef(null);
  useClickOutside(dropdownRef, onClose);

  return (
    <div ref={dropdownRef} className="dropdown">
      {children}
    </div>
  );
}
```

### Intersection observer — element visibility

```jsx
function useInView(options) {
  const ref = useRef(null);
  const [isInView, setIsInView] = useState(false);

  useEffect(() => {
    const observer = new IntersectionObserver(([entry]) => {
      setIsInView(entry.isIntersecting);
    }, options);

    if (ref.current) observer.observe(ref.current);
    return () => observer.disconnect();
  }, [options]);

  return [ref, isInView];
}

function AnimatedSection({ children }) {
  const [ref, isInView] = useInView({ threshold: 0.1 });

  return (
    <section ref={ref} className={isInView ? "visible animate-in" : "hidden"}>
      {children}
    </section>
  );
}
```

### Autosave — debounced write with ref

```jsx
function useAutosave(value, saveFn, delay = 2000) {
  const timerRef = useRef(null);
  const savedValueRef = useRef(value);

  useEffect(() => {
    // Don't save if value hasn't changed
    if (value === savedValueRef.current) return;

    clearTimeout(timerRef.current);
    timerRef.current = setTimeout(async () => {
      await saveFn(value);
      savedValueRef.current = value;
    }, delay);

    return () => clearTimeout(timerRef.current);
  }, [value, saveFn, delay]);
}

function NoteEditor({ noteId, initialContent }) {
  const [content, setContent] = useState(initialContent);

  useAutosave(content, (text) => saveNote(noteId, text));

  return (
    <textarea
      value={content}
      onChange={(e) => setContent(e.target.value)}
      placeholder="Start writing..."
    />
  );
}
```

---

## Summary

```
useRef returns { current: initialValue }
  Persists across renders — same object every render
  Mutating ref.current does NOT trigger a re-render
  Two main uses: DOM access and mutable instance variables

DOM access:
  <div ref={myRef} /> — React sets ref.current to the DOM node on mount
  ref.current is null before mount and after unmount
  Use for: focus, scroll, measure, media control, canvas

Mutable values (no re-render):
  Timer IDs (clearTimeout/clearInterval)
  Previous values
  Stale closure workarounds (store latest callback)
  Mounted/unmounted tracking

State vs Ref:
  State: causes re-render, for UI data
  Ref: no re-render, for infrastructure/DOM

Callback refs:
  Pass a function to ref instead of a ref object
  React calls it with the node on mount, null on unmount
  Useful when you need to measure immediately on attach

forwardRef:
  Lets a parent pass a ref into a custom component
  The ref is forwarded to a DOM element inside

useImperativeHandle:
  Used with forwardRef to expose a custom API instead of raw DOM node
  Parent gets specific methods, not direct DOM access
```

---

_Next: [10 — useContext](./10%20-%20useContext.md)_
