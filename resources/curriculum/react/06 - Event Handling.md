> React wraps the browser's native event system in its own synthetic event layer. Understanding how React events work — and how they differ from raw DOM events — explains behaviours that otherwise seem unpredictable.

---

## Table of Contents

1. [React Synthetic Events](#1-react-synthetic-events)
2. [Attaching Event Handlers](#2-attaching-event-handlers)
3. [The Event Object](#3-the-event-object)
4. [Common Events Reference](#4-common-events-reference)
5. [Passing Arguments to Handlers](#5-passing-arguments-to-handlers)
6. [Event Propagation in React](#6-event-propagation-in-react)
7. [Keyboard Events](#7-keyboard-events)
8. [Pointer and Touch Events](#8-pointer-and-touch-events)
9. [Global Events and Cleanup](#9-global-events-and-cleanup)

---

## 1. React Synthetic Events

React does not attach event listeners directly to DOM elements. Instead, it uses **event delegation**: a single listener at the root of the React tree captures all events via bubbling. React then dispatches the event to the correct handler.

This is why you never call `addEventListener` in React components — you use the JSX event props (`onClick`, `onChange`, etc.) and React handles the plumbing.

**SyntheticEvent** is React's cross-browser wrapper around native events. It normalises differences between browsers and provides the same API everywhere.

```jsx
function Button({ onClick }) {
  // React attaches one listener at the root
  // When the button is clicked, React finds onClick and calls it
  return <button onClick={onClick}>Click me</button>;
}
```

---

## 2. Attaching Event Handlers

```jsx
// Inline arrow function — most common
<button onClick={() => setCount(c => c + 1)}>+</button>

// Named handler — cleaner for complex logic
function handleSubmit(e) {
  e.preventDefault();
  // ...
}
<form onSubmit={handleSubmit}>

// Pass the function reference — NOT calling it
<button onClick={handleClick}>   // ✓ — passes the function
<button onClick={handleClick()}> // ✗ — calls it during render, passes the return value

// With arrow function wrapper (when you need to call with args)
<button onClick={() => handleDelete(item.id)}>Delete</button>
```

---

## 3. The Event Object

React passes a SyntheticEvent object to every handler. It has the same properties as native DOM events.

```jsx
function InputField() {
  function handleChange(e) {
    e.type;           // "change"
    e.target;         // the DOM element that triggered the event
    e.target.value;   // current input value
    e.target.name;    // input name attribute
    e.target.checked; // for checkboxes
    e.currentTarget;  // element the handler is attached to

    e.preventDefault();      // stop default browser behaviour
    e.stopPropagation();     // stop bubbling to parent elements

    e.nativeEvent;    // access the original native browser event
  }

  return <input onChange={handleChange} />;
}

// Mouse events
function ClickTarget() {
  function handleClick(e) {
    e.clientX; e.clientY;   // position relative to viewport
    e.pageX;   e.pageY;     // position relative to full document
    e.button;               // 0=left, 1=middle, 2=right
    e.ctrlKey;              // was Ctrl held?
    e.shiftKey;             // was Shift held?
    e.altKey;               // was Alt held?
    e.metaKey;              // was Cmd/Win held?
  }

  return <div onClick={handleClick} />;
}
```

---

## 4. Common Events Reference

```jsx
// Mouse
onClick          // click (primary button)
onDoubleClick    // double click
onMouseDown      // mouse button pressed
onMouseUp        // mouse button released
onMouseMove      // mouse moved over element
onMouseEnter     // mouse entered (no bubbling)
onMouseLeave     // mouse left (no bubbling)
onMouseOver      // mouse over element or descendant (bubbles)
onMouseOut       // mouse out of element or descendant (bubbles)
onContextMenu    // right click

// Keyboard
onKeyDown        // key pressed (fires repeatedly when held)
onKeyUp          // key released
// (onKeyPress is deprecated)

// Form
onChange         // input value changed
onInput          // synonym in React (use onChange)
onSubmit         // form submitted
onReset          // form reset
onFocus          // element received focus
onBlur           // element lost focus
onFocusIn        // focus (bubbles) — use onFocus in React (it bubbles)
onFocusOut       // blur (bubbles) — use onBlur in React (it bubbles)
onSelect         // text selected in input/textarea

// Clipboard
onCopy
onCut
onPaste

// Drag
onDragStart
onDrag
onDragEnd
onDragEnter
onDragOver
onDragLeave
onDrop

// Touch
onTouchStart
onTouchMove
onTouchEnd
onTouchCancel

// Wheel
onWheel

// Media
onPlay
onPause
onEnded
onVolumeChange
onTimeUpdate

// Other
onScroll
onResize         // not on elements, use ResizeObserver instead
onLoad
onError
onAnimationEnd
onTransitionEnd
```

---

## 5. Passing Arguments to Handlers

You often need to pass data to a handler — like which item was clicked.

```jsx
// ✓ Arrow function wrapper — most readable
function StudentList({ students, onDelete }) {
  return (
    <ul>
      {students.map(student => (
        <li key={student.id}>
          {student.name}
          <button onClick={() => onDelete(student.id)}>
            Delete
          </button>
        </li>
      ))}
    </ul>
  );
}

// ✓ Data attributes — alternative approach (avoids creating functions in loops)
function StudentList({ students, onDelete }) {
  function handleDelete(e) {
    const id = e.currentTarget.dataset.studentId;
    onDelete(id);
  }

  return (
    <ul>
      {students.map(student => (
        <li key={student.id}>
          {student.name}
          <button
            data-student-id={student.id}
            onClick={handleDelete}
          >
            Delete
          </button>
        </li>
      ))}
    </ul>
  );
}

// ✓ Curried handler factory
const makeDeleteHandler = (id) => () => onDelete(id);

students.map(s => (
  <button key={s.id} onClick={makeDeleteHandler(s.id)}>Delete</button>
))
```

---

## 6. Event Propagation in React

React events bubble by default — from the target up to the root.

```jsx
function Parent() {
  function handleParentClick(e) {
    console.log("parent clicked");
  }

  return (
    <div onClick={handleParentClick} className="parent">
      <Child />
    </div>
  );
}

function Child() {
  function handleChildClick(e) {
    console.log("child clicked");
    e.stopPropagation();  // prevents "parent clicked" from logging
  }

  return (
    <button onClick={handleChildClick} className="child">
      Click me
    </button>
  );
}

// Without stopPropagation:
// Clicking the button logs: "child clicked", "parent clicked"

// With stopPropagation in child:
// Clicking the button logs: "child clicked" only
```

### `preventDefault` vs `stopPropagation`

```jsx
// preventDefault — stop the browser's default behaviour for this event
<a href="/about" onClick={e => {
  e.preventDefault();   // prevents navigation
  handleClick();        // do something else instead
}}>

<form onSubmit={e => {
  e.preventDefault();   // prevents page reload on submit
  handleFormSubmit();
}}>

// stopPropagation — stop the event from bubbling up to parent handlers
<div onClick={handleDivClick}>
  <button onClick={e => {
    e.stopPropagation(); // prevents handleDivClick from firing
    handleButtonClick();
  }}>
```

---

## 7. Keyboard Events

```jsx
function SearchInput({ onSearch }) {
  const [value, setValue] = useState("");

  function handleKeyDown(e) {
    if (e.key === "Enter") {
      onSearch(value);
    }
    if (e.key === "Escape") {
      setValue("");
    }
  }

  return (
    <input
      value={value}
      onChange={e => setValue(e.target.value)}
      onKeyDown={handleKeyDown}
      placeholder="Search... (Enter to search, Esc to clear)"
    />
  );
}

// Key values for common keys:
// "Enter", "Escape", "Tab", "Backspace", "Delete"
// "ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"
// "a"–"z", "A"–"Z", "0"–"9"
// " " (spacebar)
// "F1"–"F12"

// Modifier key combinations
function handleKeyDown(e) {
  if (e.key === "s" && (e.ctrlKey || e.metaKey)) {
    e.preventDefault();  // prevent browser's "save page" dialog
    handleSave();
  }

  if (e.key === "z" && (e.ctrlKey || e.metaKey)) {
    e.preventDefault();
    handleUndo();
  }
}

// Accessible keyboard navigation
function Dropdown({ items, onSelect }) {
  const [focusIndex, setFocusIndex] = useState(-1);

  function handleKeyDown(e) {
    if (e.key === "ArrowDown") {
      e.preventDefault();
      setFocusIndex(i => Math.min(i + 1, items.length - 1));
    }
    if (e.key === "ArrowUp") {
      e.preventDefault();
      setFocusIndex(i => Math.max(i - 1, 0));
    }
    if (e.key === "Enter" && focusIndex >= 0) {
      onSelect(items[focusIndex]);
    }
    if (e.key === "Escape") {
      setFocusIndex(-1);
    }
  }

  return (
    <ul role="listbox" onKeyDown={handleKeyDown}>
      {items.map((item, i) => (
        <li
          key={item.id}
          role="option"
          aria-selected={i === focusIndex}
          tabIndex={i === focusIndex ? 0 : -1}
          onClick={() => onSelect(item)}
        >
          {item.label}
        </li>
      ))}
    </ul>
  );
}
```

---

## 8. Pointer and Touch Events

```jsx
// onPointerDown/Up/Move — unified across mouse, touch, stylus
function DrawingCanvas() {
  const [isDrawing, setIsDrawing] = useState(false);
  const [path, setPath] = useState([]);

  function handlePointerDown(e) {
    setIsDrawing(true);
    e.currentTarget.setPointerCapture(e.pointerId);  // keep tracking even outside element
    setPath([{ x: e.clientX, y: e.clientY }]);
  }

  function handlePointerMove(e) {
    if (!isDrawing) return;
    setPath(prev => [...prev, { x: e.clientX, y: e.clientY }]);
  }

  function handlePointerUp() {
    setIsDrawing(false);
  }

  return (
    <canvas
      onPointerDown={handlePointerDown}
      onPointerMove={handlePointerMove}
      onPointerUp={handlePointerUp}
    />
  );
}

// Touch events — mobile specific
function SwipeableCard({ onSwipeLeft, onSwipeRight }) {
  const startX = useRef(null);

  function handleTouchStart(e) {
    startX.current = e.touches[0].clientX;
  }

  function handleTouchEnd(e) {
    const diff = e.changedTouches[0].clientX - startX.current;
    if (diff > 50)  onSwipeRight();
    if (diff < -50) onSwipeLeft();
  }

  return (
    <div onTouchStart={handleTouchStart} onTouchEnd={handleTouchEnd}>
      {/* content */}
    </div>
  );
}
```

---

## 9. Global Events and Cleanup

Some events don't belong on specific elements — scroll, keyboard shortcuts, window resize. Use `useEffect` to attach and clean up global listeners.

```jsx
import { useEffect } from "react";

function KeyboardShortcuts({ onSave, onUndo }) {
  useEffect(() => {
    function handleKeyDown(e) {
      if ((e.ctrlKey || e.metaKey) && e.key === "s") {
        e.preventDefault();
        onSave();
      }
      if ((e.ctrlKey || e.metaKey) && e.key === "z") {
        e.preventDefault();
        onUndo();
      }
    }

    document.addEventListener("keydown", handleKeyDown);

    // CRITICAL: clean up to avoid memory leaks and duplicate handlers
    return () => {
      document.removeEventListener("keydown", handleKeyDown);
    };
  }, [onSave, onUndo]);  // re-attach if handlers change

  return null;  // this component renders nothing — just registers shortcuts
}

// Scroll position
function useScrollPosition() {
  const [scrollY, setScrollY] = useState(0);

  useEffect(() => {
    function handleScroll() {
      setScrollY(window.scrollY);
    }

    window.addEventListener("scroll", handleScroll, { passive: true });
    return () => window.removeEventListener("scroll", handleScroll);
  }, []);

  return scrollY;
}

// Click outside to close
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
  const ref = useRef(null);
  useClickOutside(ref, onClose);

  return <div ref={ref}>{children}</div>;
}
```

---

## Summary

```
React synthetic events:
  React uses event delegation — one listener at the root
  SyntheticEvent wraps native events for cross-browser consistency
  Same API as native events, plus .nativeEvent for raw access

Attaching handlers:
  onClick={handler}        — pass function reference
  onClick={() => fn(arg)}  — arrow wrapper when you need arguments
  onClick={fn()}           — ✗ WRONG: calls during render, passes return value

Event object:
  e.target          — element that triggered the event
  e.currentTarget   — element the handler is on
  e.preventDefault() — stop default browser behaviour
  e.stopPropagation() — stop bubbling

Passing arguments:
  Arrow wrapper: onClick={() => handleDelete(id)}
  Data attributes: data-id={id} then e.currentTarget.dataset.id

Propagation:
  Events bubble up by default
  e.stopPropagation() — stop the bubble
  e.preventDefault()  — stop browser default (form submit, link nav)

Keyboard:
  e.key — "Enter", "Escape", "ArrowDown", "a", etc.
  e.ctrlKey, e.metaKey, e.shiftKey, e.altKey — modifier keys
  Use for keyboard navigation, shortcuts

Global events:
  Attach in useEffect with addEventListener
  ALWAYS return cleanup: () => removeEventListener
  Not adding cleanup = memory leak + duplicate handlers
```

---

_Next: [07 — Forms & Controlled Components](./07%20-%20Forms%20%26%20Controlled%20Components.md)_